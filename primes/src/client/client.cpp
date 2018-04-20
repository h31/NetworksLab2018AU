#include <list>
#include <thread>
#include <mutex>
#include <iostream>
#include <memory>
#include <queue>
#include <map>
#include <mutex>
#include <condition_variable>
#include <iterator>

#include "socket.hpp"
#include "parser.hpp"
#include "protocol.hpp"

#define DEBUG_TOKEN "[CLIENT] "

using ull = unsigned long long;

std::mutex socket_mutex;
std::thread th;
std::condition_variable condition_variable;
bool fl; // TODO change name.

//static void send_task(Server &server, Client *client)
//{
//    Task *task = get_new_task(server);
//    unique_ptr<Task> guard(task);
//
//    client->task = task;
//    task->client = client;
//    server.tasks_enqueued[task->l] = task;
//    guard.release();
//
//    client->sock.stream << CMD_COMP_RANGE_QUERY << "\n"
//                        << task->l << ' ' << task->r << std::endl;
//    client->sock.check_conn();
//}

void client_session(net::socket_stream &ss, ull l, ull r) {

    std::vector<ull> primes;
    for (ull i = l; i < r; ++i) {
        bool is_prime = true;
        for (int j = 2; j < i; ++j) {
            if (i % j == 0) {
                is_prime = false;
            }
        }
        if (is_prime) {
            primes.push_back(i);
        }
    }
    std::unique_lock<std::mutex> lock(socket_mutex);
    ss.stream << l << ' ' << r << ' ' << primes.size() << ' ';
    for (auto const &p : primes) {
        ss.stream << p << ' ';
    }
    lock.unlock();
}

//while (true) {
//int cmd;
//parser.sock.stream >> cmd;
//parser.check_conn();
//
//switch (cmd) {
//case CMD_GET_LAST_N_QUERY:
//response_last_queries(server, client, parser);
//break;
//case CMD_OMP_RANGE_RESP:
//response_range(server, client, parser);
//send_task(server, client);
//default:
//return;
//}
//}

void client_run(const std::string &hostname, uint16_t portno)
{
    std::cout << DEBUG_TOKEN "starting client" << std::endl;
    
    try {
//        std::lock_guard<std::mutex> lock(socket_mutex);
        net::socket socket;
        socket.connect(hostname.c_str(), portno);
        std::cout << DEBUG_TOKEN "start accepting" << std::endl;
    
        net::socket_stream ss(std::move(socket));

        int command;
        ss.stream >> command;
        if (command != CMD_COMP_RANGE_QUERY)
            throw net::network_exception();
        ull l, r;
        ss.stream >> l >> r;
        
        th = std::thread(client_session, std::ref(ss), l, r);
        
        std::string line;
        while (true) {
//            std::cout << "Input "
            int n;
            std::cin >> n;
            
            std::lock_guard<std::mutex> lock(socket_mutex);
            ss.stream << CMD_GET_LAST_N_QUERY << ' ' << n;
            int cmd;
            ss.stream >> cmd;
            if (cmd != CMD_GET_LAST_N_RESP) {
                throw net::network_exception();
            }
            size_t nprimes;
            ss.stream >> nprimes;
            std::cout << "Received primes: " << nprimes << std::endl;
            std::vector<ull> primes(nprimes);
            for (size_t i = 0; i < nprimes; ++i) {
                ss.stream >> primes[i];
            }
            
            std::copy(primes.begin(), primes.end(), std::ostream_iterator<ull>(std::cout, " "));
            std::cout << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    th.join();
}

int main(int argc, char **argv) {
//    if (argc != 3) {
//        std::cerr << "Usage: " << argv[0] << " host port\n";
//        return 1;
//    }
//
//    const std::string hostname = argv[1];
//    const auto portno = (uint16_t) atoi(argv[2]);
    const std::string hostname = "localhost";
    const uint16_t portno = PRIME_PORT;
    
    client_run(hostname, portno);
    
    return 0;
}
