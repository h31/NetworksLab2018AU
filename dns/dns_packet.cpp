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
        offset += current_size;
    }
    for (int i = 0; i < _header.t_answer_rrs; ++i) {
        char* answer = answers[i].write_to_buffer();
        memcpy(buffer + offset, answer, answers[i].get_size());
        delete []answer;
        offset += answers[i].get_size();
    }
    return buffer;
}

dns_packet dns_packet::read_from_buffer(char* buffer) {
    dns_packet dns;
    dns._header = header::read_from_buffer(buffer);
    for (int i = 0; i < dns._header.t_questions; i++) {
        query q = query::read_from_buffer(buffer);
        dns.questions.push_back(q);
    }
    for (int i = 0; i < dns._header.t_answer_rrs; i++) {
        dns.answers.push_back(resource_record::read_from_buffer(buffer));
    }
    return dns;
}
