#include <arpa/inet.h>

#include <cstring>
#include <iostream>
#include <algorithm>
#include <utility>
#include <map>

#include "Message.h"

std::string get_formatted_domain_name(const std::string & domain_name) {
    std::string dom_name_rev(domain_name.rbegin(), domain_name.rend());
    std::string dom_name_formatted;
    uint8_t count = 0;
    for (char c : dom_name_rev) {
        if (c != '.') {
            dom_name_formatted.append(1, c);
            count++;
        } else {
            dom_name_formatted.append(1, char(count));
            count = 0;
        }
    }
    dom_name_formatted.append(1, char(count));
    dom_name_formatted = std::string(dom_name_formatted.rbegin(), dom_name_formatted.rend());
    dom_name_formatted.append(1, char(0));
    return dom_name_formatted;
}

dns_packet::dns_packet(uint16_t id, uint16_t qr, const std::string &domain_name) {
    header.id = id;
    header.flags = qr << 15;
    header.flags |= 1 << 8;
    header.qdcount = 1;
    header.ancount = 0;
    header.nscount = 0;
    header.arcount = 0;
    questions = new std::vector<dns_question>();
    questions->push_back(dns_question({domain_name, 1, 1}));
    questions_offsets = new std::map<std::string, uint16_t>();
    RRs = new std::vector<dns_response_packet>();
}

int dns_packet::dns_request_parse(char *data, u_int16_t size) {
    std::ostream::sync_with_stdio(false);
    for (int counter = 0; counter < size; counter++) {
//        std::cout << data[counter];
        data++;
    }
    data -= size;
    dns_header_parse(data);
    uint16_t idx = 12;
    int i = 0;
    questions = new std::vector<dns_question>();
    questions_offsets = new std::map<std::string, uint16_t>();
    while (i < header.qdcount)
    {
        idx += dns_question_parse(data, idx, i);
        i++;
    }

    int j = 0;
    RRs = new std::vector<dns_response_packet>();
    while (j < this->header.ancount) {
        idx += dns_RR_parse(data, idx, j);
        j++;
    }

    return 1;
}

int dns_packet::dns_header_parse(char *data)
{
    memcpy(&header, data, 12);
    header.id = ntohs(header.id);
    header.flags = ntohs(header.flags);
    header.qdcount = ntohs(header.qdcount);
    header.ancount = ntohs(header.ancount);
    header.arcount = ntohs(header.arcount);
    header.nscount = ntohs(header.nscount);
    return 1;
}

int get_domain_name(const char *data, std::string & dst, uint16_t idx) {
    dst.clear();
    int length = data[idx];
    data += idx;
    data -= idx;
    uint16_t init_idx = idx;
    while(length > 0) {
        idx++;
        while (length != 0) {
            dst.append(1, data[idx]);
            length--;
            idx++;
        }
        dst.append(1, '.');
        length = data[idx];
    }
    dst.erase(dst.size() - 1);
    return idx - init_idx + 1;
}

uint16_t get_2bytes(char *data, int idx) {
    uint16_t result;
    memcpy(&result, data + idx, sizeof(uint16_t));
    result = ntohs(result);
    return result;
}

uint32_t get_4bytes(char *data, int idx) {
    uint32_t result;
    memcpy(&result, data + idx, sizeof(uint32_t));
    result = ntohl(result);
    return result;
}

int dns_packet::dns_question_parse(char * data, uint16_t idx, uint16_t elem) {
    std::string domain_name;
    int n_bytes = get_domain_name(data, domain_name, idx);
    (*questions_offsets)[domain_name] = idx;
    uint16_t type = get_2bytes(data, idx + n_bytes);
    uint16_t q_class = get_2bytes(data, idx + n_bytes + 2);
    questions->push_back(dns_question({domain_name, type, q_class}));
    return n_bytes + 4;
}


int dns_packet::get_domain_name_from_offset(char* data, std::string& domain_name, uint16_t idx) {
    uint16_t value = get_2bytes(data, idx);
    value = static_cast<uint16_t>(value ^ 0xc000);
    get_domain_name(data, domain_name, value);
    return 2;
}

int dns_packet::dns_RR_parse(char *data, uint16_t idx, uint16_t elem) {
    std::string domain_name;
    int n_bytes = get_domain_name_from_offset(data, domain_name, idx);
    uint16_t type = get_2bytes(data, idx + n_bytes);
    uint16_t resp_class = get_2bytes(data, idx + n_bytes + 2);
    uint32_t ttl = get_4bytes(data, idx + n_bytes + 4);
    uint16_t rdlength = get_2bytes(data, idx + n_bytes + 8);
    char RRdata[rdlength];
    memcpy(RRdata, data + idx + n_bytes + 10, rdlength);
    RRs->push_back(dns_response_packet({domain_name, type, resp_class, ttl, rdlength, RRdata}));
    return n_bytes + 10 + rdlength;
}


const char *dns_packet::get_data() {
    return (*questions)[0].qname.data();
}

uint16_t dns_packet::get_id() {
    return header.id;
}

void dns_packet::set_id(uint16_t id) {
    header.id = id;
}

int dns_packet::to_bytes(char *buffer) {
    uint16_t buf_len = 0;
    Header correct_order_header = to_network_byte_order();
    memcpy(buffer, &correct_order_header, 12);
    buf_len = 12;
    for (int count = 0; count < header.qdcount; count++) {
        buf_len += (*questions)[count].to_bytes(buffer, buf_len);
    }
    for (int count = 0; count < header.ancount; count++) {
        buf_len += (*RRs)[count].to_bytes(buffer, buf_len, questions_offsets);
    }
    return buf_len;
}

void dns_packet::addRR(std::string & domain_name, const std::string &ip) {
    if (!RRs) {
        RRs = new std::vector<dns_response_packet>();
    }
    int idx = 0;
    std::string s;
    for (int i = 0; i < 4 ; i++) {
        std::string byte;
        char c = ip[idx];
        while (c != '.' && idx != ip.size()) {
            byte.append(1, c);
            idx++;
            c = ip[idx];
        }
        idx++;
        s.append(1, (char)std::stoi(byte));
    }
    header.ancount += 1;
    dns_response_packet packet({domain_name, 1, 1, 100, 4, s});
    RRs->push_back(packet);
}

Header dns_packet::to_network_byte_order() {
    Header new_header = {htons(header.id),
                         htons(header.flags),
                         htons(header.qdcount),
                         htons(header.ancount),
                         htons(header.nscount),
                         htons(header.arcount)};
    return new_header;
}

void put_2bytes(uint16_t value, char *buffer, uint16_t idx) {
    value = htons(value);
    memcpy(buffer + idx, &value, sizeof(uint16_t));
}

void put_4bytes(uint32_t value, char *buffer, uint16_t idx) {
    value = htonl(value);
    memcpy(buffer + idx, &value, sizeof(uint32_t));
}

int dns_question::to_bytes(char *buffer, uint16_t idx) {
    int len = 0;
    std::string formatted = get_formatted_domain_name(qname);
    memcpy(buffer + idx, formatted.data(), formatted.size());
    len += formatted.size();
    put_2bytes(qtype, buffer, idx + len);
    len += sizeof(qtype);
    put_2bytes(qclass, buffer, idx + len);
    len += sizeof(qclass);
    return len;
}

int dns_response_packet::to_bytes(char *buffer, uint16_t idx, std::map<std::string, uint16_t> * offsets) {
    int len = 0;
    uint16_t offset = (*offsets)[domain_name];
    offset |= 11 << 14;
    put_2bytes(offset, buffer, idx);
    len += 2;
    put_2bytes(type, buffer, idx + len);
    len += sizeof(type);
    put_2bytes(response_class, buffer, idx + len);
    len += sizeof(response_class);
    put_4bytes(ttl, buffer, idx + len);
    len += sizeof(ttl);
    put_2bytes(rdlength, buffer, idx + len);
    len += sizeof(rdlength);
    memcpy(buffer + idx + len, rdata.data(), rdlength);
    len += rdlength;
    return len;
}

