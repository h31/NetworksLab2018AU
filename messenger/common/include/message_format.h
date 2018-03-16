#ifndef PROJECT_MESSAGE_FORMAT_H
#define PROJECT_MESSAGE_FORMAT_H

#include <stdlib.h>
#include <stdint.h>

typedef struct {
  uint32_t format_version;
  char nickname[32];
  uint32_t message_offset;
  uint32_t message_len;
  uint32_t header_checksum; // must be the last field
} elegram_msg_header;

extern const uint32_t ELEGRAM_FORMAT_VERSION;

extern const size_t MAX_MESSAGE_LENGTH;

uint32_t elegram_header_checksum(elegram_msg_header header);

#endif  // PROJECT_MESSAGE_FORMAT_H
