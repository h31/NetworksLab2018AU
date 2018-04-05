#ifndef PROJECT_MESSAGE_FORMAT_H
#define PROJECT_MESSAGE_FORMAT_H

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "socket_utils.h"

// The structure of an elegram message:
//
// -----
// |
// | Header
// | (sizeof(header) bytes)
// |
// -----
// |
// | Some other data
// | (Unused in the first version. Needed for future protocol changes)
// |
// -----
// |
// | ascii-encoded null-terminated text
// | (text_length bytes)
// |
// -----
//
// The protocol is designed to be both forward and backward compatible.
typedef struct {
  uint32_t format_version;  // version of the protocol. Use `ELEGRAM_FORMAT_VERSION` constant
  char nickname[32];  // null-terminated nickname
  uint32_t text_offset;  // offset of the start of the text EXCLUDING the size of the header
  uint32_t text_length;  // the length of the text INCLUDING the null character
  struct tm timestamp;  // filled in by the server
  uint32_t header_checksum; // must be the last field. Use `elegram_header_checksum` function
} elegram_msg_header_t;

typedef struct {
  elegram_msg_header_t header;
  void* data;
} elegram_msg_t;

extern const uint32_t ELEGRAM_FORMAT_VERSION;

extern const size_t MAX_MESSAGE_LENGTH;

uint32_t elegram_header_checksum(elegram_msg_header_t header);

int read_message(elegram_msg_t* out, socket_t socket);

int write_message(const elegram_msg_t* message, socket_t socket);

static inline size_t message_data_length(const elegram_msg_header_t* header) {
  return header->text_offset + header->text_length;
}

#endif  // PROJECT_MESSAGE_FORMAT_H
