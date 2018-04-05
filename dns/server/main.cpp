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

static std::vector<std::string> get_dns_servers() {
    std::ifstream fin;
    fin.open("/etc/resolv.conf");
    if (!fin) {
        throw DnsError("Failed to open /etc/resolv.conf");
    }
    
    std::string line;
    while (!fin.eof() && std::getline(fin, line)) {
        if (line[0] == '#') {
            continue;
        }
        const std::string nameserver_prefix = "nameserver";
        if (line.substr(0, nameserver_prefix.size()) == nameserver_prefix) {
            std::istringstream is(line);
            std::string nameserver_ip;
            is >> nameserver_ip >> nameserver_ip;
        }
    }
    
    fin.close();
    std::vector<std::string> dns_servers;
    dns_servers.emplace_back("208.67.222.222");
    dns_servers.emplace_back("208.67.220.220");
    return dns_servers;
}

static std::list<SocketWrapper> sockets;
std::mutex sockets_mutex;
static std::mutex print_mutex;

static std::mutex dns_mutex;
DnsSocketWrapper dns_socket;

volatile bool is_finished = false;

template<typename T>
static void print_stdout(const T &arg, bool flush = true) {
    std::unique_lock<std::mutex> lock(print_mutex);
    std::cout << arg;
    if (flush) {
        std::cout << std::endl;
    }
}

static std::string get_ip(const std::string &hostname) {
    std::lock_guard<std::mutex> dns_lock(dns_mutex);
    if (!dns_socket) {
        throw DnsError("Dns socket not initialized");
    }
    return dns_socket->resolve(hostname);
//    return "192.168.0.1";
}

////write_dns_request
//static void client_session(SocketWrapper acceptSocket) {
//    while (!is_finished) {
//        uint32_t message_type;
//        try {
//            message_type = acceptSocket->read_uint();
//        }
//        catch (std::exception &e) {
//            std::cerr << "Exiting client session [" << acceptSocket->get_other_username() << "] with error: "
//                      << e.what() << std::endl;
//            break;
//        }
//        if (message_type == static_cast<uint32_t>(MessageType::FINISH)) {
//            break;
//        }
//        if (message_type != static_cast<uint32_t>(MessageType::REQUEST)) {
//            throw ProtocolError();
//        }
//        auto const hostname = acceptSocket->read_string();
//        print_stdout("Received hostname: " + hostname);
//        acceptSocket->write_uint(static_cast<uint32_t>(MessageType::RESPONSE));
//        std::string ip_string;
//        try {
//            ip_string = get_ip(hostname);
//        } catch (std::exception &e) {
//            ip_string = "ERROR: Invalid hostname";
//        }
//        acceptSocket->write_string(ip_string);
//    }
//    std::cout << "Finished session with client: " << acceptSocket->get_other_username() << std::endl;
//    {
//        std::lock_guard<std::mutex> sockets_lock{sockets_mutex};
//        acceptSocket.reset();
//    }
//}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " port" << std::endl;
        exit(1);
    }
    try {
        Socket::init();
//        auto const port = stoi(static_cast<std::string>(argv[1]));
//        auto serverSocket = std::make_shared<ServerSocket>(port);
//        serverSocket->listen();
//        std::cout << "Listening for incoming connections..." << std::endl;
//        while (!is_finished) {
//            auto acceptSocket = serverSocket->accept();
//            std::unique_lock<std::mutex> sockets_lock{sockets_mutex};
//            sockets.push_back(acceptSocket);
//            std::thread client_thread{client_session, acceptSocket};
//            client_thread.detach();
//        }
    
        //List of DNS Servers registered on the system
        std::vector<std::string> dns_servers = get_dns_servers();
        dns_socket.reset(new DnsSocket());
        dns_socket->set_dns_server(dns_servers[0]);
        std::string line = "google.com";
        do {
            std::cout << "Address: " << line << std::endl;
            auto answer = get_ip(line);
            std::cout << "Resolved: " << answer << std::endl;
        } while (std::getline(std::cin, line));
    }
    catch (std::exception &e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        is_finished = true;
    }
    return 0;
}
