#include <iostream>
#include <string>
#include <utility>
#include <memory>
#include <vector>
#include <list>
#include <queue>
#include <cstring>
#include <cstddef>
#include <limits>
#include <algorithm>
#include <mutex>
#include <thread>
#include <cassert>
#include <fstream>
#include <sstream>
#include "DnsAll.h"

static std::mutex print_mutex;

volatile bool is_finished = false;

template<typename T>
static void print_stdout(const T &arg, bool flush = true) {
    std::unique_lock<std::mutex> lock(print_mutex);
    std::cout << arg;
    if (flush) {
        std::cout << std::endl;
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " port" << std::endl;
        return 1;
    }
    try {
        auto const port = stoi(static_cast<std::string>(argv[1]));
        DnsServerSocket serverSocket{port};
        while (!is_finished) {
            serverSocket.step();
        }
    }
    catch (std::exception &e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        is_finished = true;
    }
    return 0;
}
