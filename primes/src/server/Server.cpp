#include <list>
#include <thread>
#include <mutex>
#include <iostream>
#include <memory>
#include <queue>
#include <map>

#include "list.h"
#include "socket.hpp"
#include "parser.hpp"
#include "protocol.hpp"

using namespace std;

#define DEBUG_TOKEN "[SERVER] "
#define RANGE 1000

struct Client;
struct Server;
typedef lock_guard<recursive_mutex> Lguard;
typedef unsigned long long ull;

struct Task {
    ull l, r;

    Client *client;
    list_head_t head;

    bool operator<(Task& rhs) {
        return l < rhs.l;
    }
};

struct Server {
    list_head_t clients;
    list_head_t tasks_free;
    map<ull, Task*> tasks_enqueued;
    priority_queue<pair<ull, vector<ull>>> segments;

    ull l, r;
    vector<ull> primes;

    net::socket listen;
    recursive_mutex mtx;

    Server(): l(0ull), r(0ull)
    {
        INIT_LIST_HEAD(&clients);
        INIT_LIST_HEAD(&tasks_free);
    }
};

struct Client {
    list_head_t head;

    net::socket_stream sock;
    std::thread th;
    Task *task;

    explicit Client(net::socket_stream&& sock): sock(std::move(sock))
    {
        INIT_LIST_HEAD(&head);
        task = NULL;
    }
};

static void create_tasks(Server &server)
{
    Lguard guard(server.mtx);
    Task *task = new Task;

    task->l = server.l;
    task->r = server.l + RANGE;
    server.l += RANGE;

    task->client = nullptr;
    list_add(&task->head, &server.tasks_free);
}

static Task *get_new_task(Server& server)
{
    Lguard guard(server.mtx);
    if (list_empty(&server.tasks_free))
        create_tasks(server);

    Task *task = container_of(server.tasks_free.next, Task, head);
    list_del_init(server.tasks_free.next);

    return task;
}

//  ===================================================
//      send last n primes
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

static void response_last_queries(Server &server, Client *client, parser& parser)
{
    size_t n;

    parser.sock.stream >> n;
    parser.check_conn();

    std::vector<ull> resp = collect_last_n(server, n);

    parser.sock.stream << CMD_GET_LAST_N_RESP << '\n'
        << resp.size() << '\n';
    for (ull num: resp)
        parser.sock.stream << num << ' ';
    parser.sock.stream << std::endl;
    parser.sock.stream.flush();
    parser.check_conn();
}

//  ===================================================
//      get primes in range
//

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

static void response_range(Server &server, Client *client, parser& parser)
{
    ull l, r, n;

    parser.sock.stream >> l >> r >> n;
    parser.check_conn();

    vector<ull> primes(n);
    for (int i = 0; i < n; ++i)
        parser.sock.stream >> primes[i];
    parser.check_conn();

    Lguard guard(server.mtx);
    auto it = server.tasks_enqueued.find(l);
    if (it == server.tasks_enqueued.end())
        return;

    server.segments.push(make_pair(l, move(primes)));
    unique_ptr<Task> task(it->second);
    server.tasks_enqueued.erase(it);
    flush_primes(server);
}

static void send_task(Server &server, Client *client)
{
    Task *task = get_new_task(server);
    unique_ptr<Task> guard(task);

    client->task = task;
    task->client = client;
    server.tasks_enqueued[task->l] = task;
    guard.release();

    client->sock.stream << CMD_COMP_RANGE_QUERY << "\n"
        << task->l << ' ' << task->r << std::endl;
    client->sock.check_conn();
}


//  ===================================================
//      client event dispatching
//

static void process_loop(Server &server, Client *client, parser& parser)
{
    send_task(server, client);
    while (true) {
        int cmd;
        parser.sock.stream >> cmd;
        parser.check_conn();

        switch (cmd) {
            case CMD_GET_LAST_N_QUERY:
                response_last_queries(server, client, parser);
                break;
            case CMD_OMP_RANGE_RESP:
                response_range(server, client, parser);
                send_task(server, client);
            default:
                return;
        }
    }
}

static void process_client(Server &server, Client *client)
{
    parser parser(client->sock);

    try {
        process_loop(server, client, parser);
    } catch (...) {
        Task *task = client->task;
        client->task = nullptr;
        task->client = nullptr;

        server.tasks_enqueued.erase(task->l);
        list_add(&task->head, &server.tasks_free);

        list_del(&client->head);
        delete client;
    }
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
            Client *cl = new Client(move(sock));
            Lguard(server.mtx);
            list_add(&cl->head, &server.clients);
            cl->th = std::thread(process_client, std::ref(server), cl);
            cl->th.detach();
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    net::socket_ops::init();

//    if (argc < 2) {
//        std::cerr << "usage " << argv[0] << " port\n";
//        return 1;
//    }

    uint16_t portno = PRIME_PORT; //(uint16_t) atoi(argv[1]);
    server_run(portno);

    return 0;
}