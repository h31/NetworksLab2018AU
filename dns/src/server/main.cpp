#include <csignal>
#include <cstring>
#include <iostream>

#include <network.h>
#include <dns.h>

#include <utils/errors.h>
#include <utils/io.h>

SOCKET sockfd;

void finish() {
    int result = close(sockfd);
    check_error(result, SOCKET_CLOSE_ERROR);
}

void termination_handler(int) {
    std::cerr << "Received signal to terminate. Terminating...\n";
    finish();
    exit(0);
}

int main(int, char **) {
    std::signal(SIGABRT, termination_handler);
    std::signal(SIGFPE, termination_handler);
    std::signal(SIGILL, termination_handler);
    std::signal(SIGINT, termination_handler);
    std::signal(SIGSEGV, termination_handler);
    std::signal(SIGTERM, termination_handler);

    socklen_t clilen;
    sockaddr_in serv_addr {};
    sockaddr cli_addr {};
    
    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    check_error(sockfd, SOCKET_OPEN_ERROR);

    /* Initialize socket structure */
    memset(&serv_addr, 0, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(DNS_PORT);

    /* Now bind the host address using bind() call.*/
    int result = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    check_error(result, SOCKET_BIND_ERROR);

    /* Now start listening for the clients, here process will
     * go in sleep mode and will wait for the incoming connection
    */
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // main loop
    while (true) {
        try {
            dns::message message = io::read_message(sockfd, &cli_addr, &clilen);

            // no need in these
            message.ans.rrs.clear();
            message.auth.rrs.clear();
            message.add.rrs.clear();

            if (static_cast<dns::header_opcode>(ntohs(message.h.opcode)) == dns::QUERY) {
                for (dns::question_record qr: message.q.qs) {
                    // type check
                    if (static_cast<dns::record_type>(ntohs(qr.part.qtype)) != dns::A) { // unsupported
                        message.h.rcode = dns::UNIMPL;
                        break;
                    }

                    // retrieve information about queried hostname
                    ADDRINFOA *host_info = NULL;
                    ADDRINFOA hints;
                    memset(&hints, 0, sizeof(hints));
                    hints.ai_family = AF_INET;

                    int n = getaddrinfo(
                        qr.qname.c_str(), 
                        std::to_string(HTTP_PORT).c_str(), 
                        &hints, 
                        &host_info
                    );
                    if (n != 0) { // server could not get address, so return code is "name server problem"
                        message.h.rcode = dns::SERVER;
                        break;
                    }

                    unsigned int ip = *((unsigned int *) &((sockaddr_in *) host_info->ai_addr)->sin_addr);

                    // construct answer
                    dns::resource_record rr = {
                        qr.qname.c_str(), 
                        {
                            qr.part.qtype,       // same type
                            qr.part.qclass,      // same class
                            htonl(24 * 60 * 60), // let's set ttl to one day
                            htons(sizeof(ip))    // length of address stored in rdata
                        },
                        std::string(
                            (char *) &ip, 
                            sizeof(ip)
                        )
                    };

                    // push to answers
                    message.ans.rrs.push_back(rr);

                    // free resources
                    freeaddrinfo(host_info);
                }
            } else { // unsupported
                message.h.rcode = dns::UNIMPL;
            }

            // update sizes
            message.h.ancount = htons(message.ans.rrs.size());
            message.h.nscount = htons(message.auth.rrs.size());
            message.h.arcount = htons(message.add.rrs.size());

            // write responce
            io::write_message(sockfd, message, (sockaddr *) &cli_addr, clilen);
        } catch (io::io_exception e) {
            e.show_error();
        }
    }

    finish();
}