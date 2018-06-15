#include <iostream>
#include "server.h"

int main()
{
    server dns = server("hosts.txt");
    dns.run();
}
