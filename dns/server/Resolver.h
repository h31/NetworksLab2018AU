#ifndef DNS_RESOLVER_H
#define DNS_RESOLVER_H

#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "../Message.h"

class Resolver {
public:
    explicit Resolver(const std::string & hostsfile) {
        std::ifstream ifstream(hostsfile);
        std::string line;

        while(!ifstream.eof()) {
            getline(ifstream, line);
            size_t end_of_ip = line.find_first_of(' ');
            size_t domain_first = line.find_last_of(' ') + 1;
            std::string ip(line, 0, end_of_ip);
            std::string domain_name(line, domain_first, line.length());
            records[domain_name] = ip;
        }
    }

    const std::string & get_ip(const std::string &domain_name) {
        return records[domain_name];
    }


private:
    std::map<std::string, std::string> records;
};


#endif //DNS_RESOLVER_H
