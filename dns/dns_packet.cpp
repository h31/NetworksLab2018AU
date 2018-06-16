#include "dns_packet.h"

char *dns_packet::write_to_buf(size_t &offset) {
    char* buffer = new char[BUF_SIZE];
    char* head = _header.write_to_buffer();
    memcpy(buffer, head, 12);
    offset = 12;
    for (int i = 0; i < _header.t_questions; ++i) {
        char* question = questions[i].write_to_buffer();
        size_t current_size = static_cast<size_t>(questions[i].get_size());
        memcpy(buffer + offset, question, current_size);
        delete[] question;
        offset += current_size;
    }
    for (int i = 0; i < _header.t_answer_rrs; ++i) {
        char* answer = answers[i].write_to_buffer();
        memcpy(buffer + offset, answer, answers[i].get_size());
        delete[] answer;
        offset += answers[i].get_size();
    }
    return buffer;
}

dns_packet dns_packet::read_from_buffer(char* buffer) {
    dns_packet dns;
    auto pointer = buffer;
    dns._header = header::read_from_buffer(buffer);
    for (int i = 0; i < dns._header.t_questions; i++) {
        query q = query::read_from_buffer(buffer);
        dns.questions.push_back(q);
    }
    for (int i = 0; i < dns._header.t_answer_rrs; i++) {
        resource_record a = resource_record::read_from_buffer(pointer, buffer);
        dns.answers.push_back(a);
    }
    return dns;
}

void dns_packet::print_result() {
    int count = _header.t_answer_rrs;
    if (count != answers.size()) {
        std::cout << "Size of answers don't match." << std::endl;
        return;
    }
    if (_header._flags & 11) {
        std::cout << "The DNS server does not have this domain address" << std::endl;
        return;
    }

    for (int i = 0; i < count; i++) {
        resource_record answer = answers[i];
        std::cout << "Name: " << answer._name << std::endl;
        if (answer._type == T_A) {
            long *long_rdata = reinterpret_cast<long *>(answer._rdata);
            sockaddr_in address{};
            address.sin_addr.s_addr = static_cast<in_addr_t>(*long_rdata);
            std::cout << inet_ntoa(address.sin_addr) << std::endl;
        } else {
            std::cout << "Not IPv4" << std::endl;
        }
    }
}

