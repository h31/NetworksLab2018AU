#include "header.h"

#include <cstring>
#include <arpa/inet.h>

char *header::write_to_buffer() {
    char* buffer = new char[12];
    header to_send = header();
    to_send._id = htons(_id);
    to_send._flags = htons(_flags);
    to_send.t_questions = htons(t_questions);
    to_send.t_authority_rrs = htons(t_authority_rrs);
    to_send.t_additional_rrs = htons(t_additional_rrs);
    memcpy(buffer, &to_send, 12);
    return buffer;
}

header header::read_from_buffer(char *&buf) {
    header header_;
    memcpy(&header_._id, buf, sizeof(uint16_t));
    header_._id = ntohs(header_._id);
    buf += sizeof(uint16_t);
    memcpy(&header_._flags, buf, sizeof(uint16_t));
    header_._flags = ntohs(header_._flags);
    buf += sizeof(uint16_t);
    memcpy(&header_.t_questions, buf, sizeof(uint16_t));
    header_.t_questions = ntohs(header_.t_questions);
    buf += sizeof(uint16_t);
    memcpy(&header_.t_answer_rrs, buf, sizeof(uint16_t));
    header_.t_answer_rrs = ntohs(header_.t_answer_rrs);
    buf += sizeof(uint16_t);
    memcpy(&header_.t_authority_rrs, buf, sizeof(uint16_t));
    header_.t_authority_rrs = ntohs(header_.t_authority_rrs);
    buf += sizeof(uint16_t);
    memcpy(&header_.t_additional_rrs, buf, sizeof(uint16_t));
    header_.t_additional_rrs = ntohs(header_.t_additional_rrs);
    buf += sizeof(uint16_t);
    return header_;
}
