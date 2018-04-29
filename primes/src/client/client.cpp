#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <assert.h>
#include <atomic>

#include "socket.hpp"
#include "parser.hpp"
#include "protocol.hpp"

using namespace std;

typedef unsigned long long u64;

static uint16_t portno;
static const char *host;
std::atomic_bool end_flag;


static bool prime(u64 num)
{
    if (num == 2 || num == 1)
        return true;

    if (num % 2 == 0)
        return false;

    for (u64 cnt = 3; cnt * cnt <= num; ++cnt)
        if (num % cnt == 0)
            return false;

    return true;
}

static vector<u64> primes(u64 a, u64 b)
{
    vector<u64> vs;

    for (u64 i = a; i < b; ++i)
        if (prime(i))
            vs.push_back(i);

    return vs;
}

net::socket_stream get_new_stream()
{
    net::socket sock;
    sock.connect(host, portno);
    net::socket_stream stream(std::move(sock));

    return move(stream);
}

static pair<u64, u64> read_task()
{
    auto s = get_new_stream();

    stringstream ss;
    ss << net::get_request_header();
    ss << CMD_NODE_FREE_NOTIFICATION << endl;
    string data = ss.str();

    s.sock.send_all(data.c_str(), data.size());

    u64 a, b;
    int cmd;
    getline(s.stream, data);
    s.stream >> cmd;

    assert(cmd == CMD_NODE_FREE_RESP);
    s.stream >> a >> b;
    s.check_conn();

    return make_pair(a, b);
}

void send_primes(vector<u64> primes, pair<u64, u64> range)
{
    auto s = get_new_stream();

    stringstream ss;
    ss << net::get_request_header();
    ss  << CMD_COMPUTE_END << endl
        << range.first << ' ' << range.second << '\n'
        << primes.size() << '\n';

    for (auto num: primes)
        ss << num << ' ';
    ss << endl;
    string data = ss.str();
    s.sock.send_all(data.c_str(), data.size());

    getline(s.stream, data);
    s.check_conn();
}

static void worker()
{
    while (!end_flag) {
        auto range = read_task();
        auto res = primes(range.first, range.second);
        send_primes(move(res), range);

        sleep(1);
    }
}

static void print_primes(int n)
{
    auto s = get_new_stream();

    stringstream ss;
    ss << net::get_request_header()
        << CMD_GET_LAST_N_QUERY << endl
        << n << endl;
    string data = ss.str();
    s.sock.send_all(data.c_str(), data.size());

    int cmd;
    getline(s.stream, data);
    s.stream >> cmd;

    assert(cmd == CMD_GET_LAST_N_RESP);
    s.stream >> n;
    for (int i = 0; i < n; ++i) {
        u64 num;
        s.stream >> num;
        cout << num << ' ';
    }
    cout << endl;
    s.check_conn();
}

static void user_worker()
{
    string op;

    while (1) {
        cout << "enter command (prime|primes|exit)" << endl;
        cin >> op;

        if (op == "exit") {
            end_flag = true;
            break;
        }

        bool prime = false;
        int n = 0;

        if (op == "prime") {
            prime = true;
            n = 1;
        } else if (op == "primes") {
            prime = true;
            cout << "enter prime count" << endl;
            cin >> n;
        }
        if (!prime) {
            cerr << "invalid command" << endl;
            continue;
        }

        print_primes(n);
    }
}

int main(int argc, char *argv[])
{
    net::socket_ops::init();

    if (argc < 3) {
        std::cerr << "usage " << argv[0] << " hostname port\n";
        return 1;
    }

    portno = (uint16_t) atoi(argv[2]);
    host = argv[1];
    end_flag = false;

    auto th = std::thread(worker);
    user_worker();
    th.join();

    return 0;
}