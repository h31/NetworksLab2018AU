#include <climits>
#include <iostream>

#include <utils/io.h>
#include <utils/errors.h>

namespace io {

io_exception::io_exception(int _errcode, std::string _additional_message) : errcode(_errcode), additional_message(_additional_message) {}

void io_exception::show_error() {
    if (errcode < 0) {
        check_error(errcode, additional_message);
    } else {
        std::cerr << "error " << errcode << " -- " << additional_message << "\n";
    }
}

void add_data(std::vector<char> &data, char *pointer, unsigned int size) {
    char *end = pointer + size;
    for (; pointer != end; ++pointer) {
        data.push_back(*pointer);
    }
}

void add_name(std::vector<char> &data, std::string &name) {
    std::vector<char> name_part;
    for (char c: name) {
        if (c == '.') {
            data.push_back((char) name_part.size());
            add_data(data, name_part.data(), name_part.size());
            name_part.clear();
        } else {
            name_part.push_back(c);
        }
    }
    if (name_part.size() > 0) {
        data.push_back((char) name_part.size());
        add_data(data, name_part.data(), name_part.size());
        name_part.clear();
    }
    data.push_back(0);
}

void add_resource_record(std::vector<char> &data, dns::resource_record &record) {
    add_name(data, record.name);
    add_data(data, (char *) &record.part, sizeof(record.part));
    add_data(data, record.rdata.data(), record.rdata.size());
}

void write_message(SOCKET sockfd, dns::message message, sockaddr *to, socklen_t addrlen) {
    std::vector<char> data;
    
    add_data(data, (char *) &message.h, sizeof(message.h));
    
    if (ntohs(message.h.qdcount) != message.q.qs.size()) {
        throw io_exception(1, "qdcount does not match actual count of questions");
    }
    for (dns::question_record qr: message.q.qs) {
        add_name(data, qr.qname);
        add_data(data, (char *) &qr.part, sizeof(qr.part));
    }

    if (ntohs(message.h.ancount) != message.ans.rrs.size()) {
        throw io_exception(2, "ancount does not match actual count of answers");
    }
    for (dns::resource_record rr: message.ans.rrs) {
        add_resource_record(data, rr);
    }

    if (ntohs(message.h.nscount) != message.auth.rrs.size()) {
        throw io_exception(3, "nscount does not match actual count of authority records");
    }
    for (dns::resource_record rr: message.auth.rrs) {
        add_resource_record(data, rr);
    }

    if (ntohs(message.h.arcount) != message.add.rrs.size()) {
        throw io_exception(4, "arcount does not match actual count of additional records");
    }
    for (dns::resource_record rr: message.add.rrs) {
        add_resource_record(data, rr);
    }

    int n = sendto(sockfd, data.data(), data.size(), 0, to, addrlen);
    if (n < 0) {
        throw io_exception(n, SOCKET_WRITE_ERROR);
    }
}

std::string read_name(char *buf, int &bufpos, char *raw_data) {
    std::string result;
    unsigned char n = 0;
    do {
        if (n > 0 && buf[bufpos] > 0) {
            result.push_back('.');
        }
        n = buf[bufpos];
        if ((n >> 6) == 3) {
            int ptr = ntohs(read_data<unsigned short>(buf, bufpos)) & 0x0aff;
            result.append(read_name(raw_data, ptr, raw_data));
            n = 0;
        } else {
            ++bufpos;
            result.append(buf + bufpos, n);
            bufpos += n;
        }
    } while (n > 0);
    return result;
}

dns::resource_record read_resource_record(char *buf, int &bufpos) {
    dns::resource_record rr;
    
    rr.name = read_name(buf, bufpos, buf);
    rr.part = read_data<dns::resource_record_part>(buf, bufpos);
    rr.rdata = std::string(buf + bufpos, ntohs(rr.part.rdlength));
    bufpos += ntohs(rr.part.rdlength);

    return rr;
}

// let's hope that will be enough for the whole message
const int BUFSIZE = 65536;

dns::message read_message(SOCKET sockfd, sockaddr *from, socklen_t *addrlen) {
    char buf[BUFSIZE];
    int bufpos = 0;

    int n = recvfrom(sockfd, buf, BUFSIZE, 0, from, addrlen);
    if (n < 0) {
        throw io_exception(n, SOCKET_READ_ERROR);
    }

    dns::message message;
    
    message.h = read_data<dns::header>(buf, bufpos);

    message.q.qs.resize(ntohs(message.h.qdcount));
    for (int i = 0; i < ntohs(message.h.qdcount); ++i) {
        message.q.qs[i].qname = read_name(buf, bufpos, buf);
        message.q.qs[i].part = read_data<dns::question_part>(buf, bufpos);
    }

    message.ans.rrs.resize(ntohs(message.h.ancount));
    for (int i = 0; i < ntohs(message.h.ancount); ++i) {
        message.ans.rrs[i] = read_resource_record(buf, bufpos);
    }

    message.auth.rrs.resize(ntohs(message.h.nscount));
    for (int i = 0; i < ntohs(message.h.nscount); ++i) {
        message.auth.rrs[i] = read_resource_record(buf, bufpos);
    }

    message.add.rrs.resize(ntohs(message.h.arcount));
    for (int i = 0; i < ntohs(message.h.arcount); ++i) {
        message.add.rrs[i] = read_resource_record(buf, bufpos);
    }

    message.raw_data = std::vector<char>(buf, buf + n);

    return message;
}

} // namespace io