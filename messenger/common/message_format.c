
#include "message_format.h"

const uint32_t ELEGRAM_FORMAT_VERSION = 1;

const size_t MAX_MESSAGE_LENGTH = 1024 * 1024;

uint32_t elegram_header_checksum(elegram_msg_header header) {
  uint32_t res = 1987;
  for (size_t i = 0; i < sizeof(header) - sizeof(header.header_checksum); ++i) {
    res += 127 * res + (uint32_t)(((char*)&header)[i]) + 1;
  }
  return res;
}
