#include <list>
#include <thread>
#include <mutex>
#include <iostream>
#include <memory>
#include <queue>
#include <map>
#include <sstream>

#include "socket.hpp"
#include "parser.hpp"
#include "protocol.hpp"

using namespace std;

#define DEBUG_TOKEN "[SERVER] "
#define RANGE 10

struct Server;
typedef lock_guard<recursive_mutex> Lguard;
typedef unsigned long long ull;

struct Task {
    ull l, r;

    bool operator<(Task& rhs) {
        return l < rhs.l;
    }
};

struct Server {
    queue<Task> tasks;
    priority_queue<pair<ull, vector<ull>>> segments;

    ull l, r;
    vector<ull> primes;

    net::socket listen;
    recursive_mutex mtx;

    Server(): l(0ull), r(0ull) {}
};

static void create_tasks(Server &server)
{
    Lguard guard(server.mtx);
    Task task;

    task.l = server.l;
    task.r = server.l + RANGE;
    server.r += RANGE;

    server.tasks.push(task);
}

static Task get_new_task(Server& server)
{
    Lguard guard(server.mtx);
    if (server.tasks.empty())
        create_tasks(server);

    Task task = server.tasks.back();
    server.tasks.pop();

    return task;
}


//  ===================================================
//      primes helpers
//

static std::vector<ull> collect_last_n(Server &server, size_t n)
{
    Lguard guard(server.mtx);
    n = std::min(n, server.primes.size());
    std::vector<ull> resp(n);

    for (size_t i = 0; i < n; ++i)
        resp[i] = server.primes[server.primes.size() - i - 1];

    return resp;
}

static void flush_primes(Server &server)
{
    Lguard guard(server.mtx);

    while (!server.segments.empty()) {
        auto& seg = server.segments.top();

        if (seg.first != server.l)
            break;

        for (ull num: seg.second)
            server.primes.push_back(num);

        server.l = seg.first + RANGE;
        server.segments.pop();
    }
}


//  ===================================================
//      process
//

static void process_get_primes(Server &server, net::socket_stream& sock)
{
    size_t n;

    sock.stream >> n;
    std::vector<ull> resp = collect_last_n(server, n);

    stringstream ss;
    ss << net::get_header() << CMD_GET_LAST_N_RESP << '\n' << resp.size() << '\n';
    for (ull num: resp)
        ss << num << ' ';
    ss << std::endl;

    string tmp = ss.str();
    sock.sock.send_all(tmp.c_str(), tmp.size());
}

static void send_task(Server &server, net::socket_stream& sock)
{
    Task task = get_new_task(server);

    stringstream ss;
    ss << net::get_header() << CMD_NODE_FREE_RESP << '\n';
    ss << task.l << ' ' << task.r << std::endl;

    string tmp = ss.str();
    sock.sock.send_all(tmp.c_str(), tmp.size());
}

static void gather_results(Server &server, net::socket_stream& sock)
{
    ull l, r, n;

    sock.stream >> l >> r >> n;
    sock.check_conn();

    vector<ull> primes(n);
    for (int i = 0; i < n; ++i)
        sock.stream >> primes[i];
    sock.check_conn();

    string tmp = net::get_header();
    sock.sock.send_all(tmp.c_str(), tmp.size());

    Lguard guard(server.mtx);
    server.segments.push(make_pair(l, move(primes)));
    flush_primes(server);
}

static void process_client(Server &server, net::socket_stream sock)
{
    int cmd;
    string str;

    try {
        getline(sock.stream, str);
        sock.stream >> cmd;

        switch (cmd) {
            case CMD_GET_LAST_N_QUERY:
                process_get_primes(server, sock);
                break;
            case CMD_NODE_FREE_NOTIFICATION:
                send_task(server, sock);
                break;
            case CMD_COMPUTE_END:
                gather_results(server, sock);
                break;
            default:
                break;
        }
    } catch(...) {
        std::cerr << "error ..." << std::endl;
    }
    sock.sock.close();
}

//  ===================================================
//      server thread
//

void server_run(uint16_t portno)
{
    Server server;

    std::cout << DEBUG_TOKEN "start listening" << std::endl;

    try {
        server.listen.listen(portno);
        std::cout << DEBUG_TOKEN "start accepting" << std::endl;

        while (true) {
            net::socket_stream sock(std::move(server.listen.accept()));
            Lguard(server.mtx);
            auto th = std::thread(process_client, std::ref(server), move(sock));
            th.detach();
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    net::socket_ops::init();

    if (argc < 2) {
        std::cerr << "usage " << argv[0] << " port\n";
        return 1;
    }

    uint16_t portno = (uint16_t) atoi(argv[1]);
    server_run(portno);

    return 0;
}