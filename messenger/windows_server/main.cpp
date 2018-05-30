#pragma comment(lib, "WS2_32.lib")
#include <iostream>
#include "server.h"

int main(int argc, char** argv) {
    elegram::server s(static_cast<uint16_t>(atoi(argv[1])));
    s.start();
    return 0;
}