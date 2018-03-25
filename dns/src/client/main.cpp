#include <cstring>
#include <string>
#include <iostream>

#include <network.h>
#include <dns.h>
#include <utils/io.h>
#include <utils/errors.h>

dns::record_type output_type(std::string indent, unsigned short type_repr) {
    std::cout << indent << "type=";
    dns::record_type type = static_cast<dns::record_type>(type_repr);
    switch (type) {
        case dns::A:
            std::cout << "A";
            break;
        case dns::SOA:
            std::cout << "SOA";
            break;
        default:
            std::cout << "other(" << std::hex << type << ")";
    }
    std::cout << "\n";
    return type;
}

dns::record_class output_class(std::string indent, unsigned short class_repr) {
    std::cout << indent << "class=";
    dns::record_class clss = static_cast<dns::record_class>(class_repr);
    switch (clss) {
        case dns::IN:
            std::cout << "IN";
            break;
        default:
            std::cout << "other(" << std::hex << clss << ")";
    }
    std::cout << "\n";
    return clss;
}

void output_question(std::string indent, dns::question_record qr) {
    std::string inner_indent = indent + "\t";
    std::cout 
        << indent << "{" << "\n"
        << inner_indent << "name=\"" << qr.qname << "\"" << "\n";

    output_type(inner_indent, ntohs(qr.part.qtype));
    output_class(inner_indent, ntohs(qr.part.qclass));

    std::cout
        << indent << "}" << "\n";
}

void output_rdata(std::string indent, std::string rdata, dns::record_type type) {
    char *data = rdata.data();

    std::string inner_indent = indent + "\t";
    std::cout << indent << "rdata=" << "{" << "\n";
    switch (type) {
        case dns::A: {
            int bufpos = 0;
            unsigned int ip_int = (io::read_data<unsigned int>(data, bufpos));
            unsigned char *ip = (unsigned char *) &ip_int;
            std::cout 
                << inner_indent << "IP=" << std::dec
                << (int) ip[0] << "." 
                << (int) ip[1] << "." 
                << (int) ip[2] << "." 
                << (int) ip[3] 
                << "\n";
            break;
        }
        case dns::SOA: {
            int bufpos = 0;
            std::cout
                << inner_indent << "primary_master=" << io::read_name(data, bufpos, data) << "\n"
                << inner_indent << "admin_mailbox=" << io::read_name(data, bufpos, data) << "\n"
                << inner_indent << "serial_number=" << ntohl(io::read_data<unsigned int>(data, bufpos)) << "\n"
                << inner_indent << "refresh_interval=" << ntohl(io::read_data<unsigned int>(data, bufpos)) << "\n"
                << inner_indent << "retry_interval=" << ntohl(io::read_data<unsigned int>(data, bufpos)) << "\n"
                << inner_indent << "expires_in=" << ntohl(io::read_data<unsigned int>(data, bufpos)) << "\n"
                << inner_indent << "min_ttl=" << ntohl(io::read_data<unsigned int>(data, bufpos)) << "\n";
            break;
        }
        default:
            std::cout << inner_indent << rdata << "\n";
    }
    std::cout << indent << "}" << "\n";
}

void output_record(std::string indent, dns::resource_record rr) {
    std::string inner_indent = indent + "\t";
    std::cout 
        << indent << "{" << "\n"
        << inner_indent << "name=\"" << rr.name << "\"" << "\n";
    
    dns::record_type type = output_type(inner_indent, ntohs(rr.part.type));
    output_class(inner_indent, ntohs(rr.part.clss));
    
    std::cout 
        << inner_indent << "TTL=" << std::dec << ntohl(rr.part.ttl) << "\n"
        << inner_indent << "rdlength=" << std::dec << ntohs(rr.part.rdlength) << "\n";

    output_rdata(inner_indent, rr.rdata, type);

    std::cout << indent << "}" << "\n";
}

int main(int argc, char *argv[]) {
    SOCKET sockfd;
    ADDRINFOA *server_info = NULL;
    
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " serveradrr lookupaddr\n";
        exit(0);
    }

    // retrieve information about server
    ADDRINFOA hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int n = getaddrinfo(argv[1], std::to_string(DNS_PORT).c_str(), &hints, &server_info);
    if (n != 0) {
        std::cerr << gai_strerror(n) << ": " << ADDRINFO_ERROR << "\n";
        exit(1);
    }

    /* Create a socket point */
    sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    check_error(sockfd, SOCKET_OPEN_ERROR);

    // send query to server
    dns::message query;
    query.h = 
        {
            (unsigned short) htons(getpid()), // our id
            htons(1),                         // recursion desired
            0,                                // not truncated
            0,                                // not authoritative
            htons(dns::QUERY),                // standard query
            0,                                // query
            htons(dns::OK),                   // no error (obviously)
            0,
            0,
            htons(1),                         // 1 question
            0,
            0,
            0
        };
    query.q.qs.push_back(
        {
            std::string(argv[2]),
            {
                htons(dns::A), // only A-type records are supported
                htons(dns::IN) // Internet
            }
        }
    );

    try {
        // write query to server
        io::write_message(sockfd, query, server_info->ai_addr, server_info->ai_addrlen);

        // get answer
        dns::message answer = io::read_message(sockfd, server_info->ai_addr, &server_info->ai_addrlen);

        dns::header_rcode rcode = static_cast<dns::header_rcode>(answer.h.rcode);
        if (rcode != dns::OK) {
            std::cout << "WARNING: Responce rcode is not OK, but ";
            switch (rcode) {
                case dns::FORMAT:
                    std::cout << "FORMAT";
                    break;
                case dns::SERVER:
                    std::cout << "SERVER";
                    break;
                case dns::NAME:
                    std::cout << "NAME";
                    break;
                case dns::UNIMPL:
                    std::cout << "UNIMPL";
                    break;
                case dns::REFUSE:
                    std::cout << "REFUSE";
                    break;
                default:
                    std::cout << "unknown rcode";
            }
            std::cout << "\n";
        }
        if (ntohs(answer.h.id) != getpid()) {
            std::cout << "WARNING: Responce id does not correspond to ours\n";
        }

        std::cout << "Responce contents:\n";
        
        std::cout << "\t" << std::dec << ntohs(answer.h.qdcount) << " questions:\n";
        for (dns::question_record qr: answer.q.qs) {
            output_question("\t\t", qr);
        }

        std::cout << "\t" << std::dec << ntohs(answer.h.ancount) << " answers:\n";
        for (dns::resource_record rr: answer.ans.rrs) {
            output_record("\t\t", rr);
        }

        std::cout << "\t" << std::dec << ntohs(answer.h.nscount) << " authorities:\n";
        for (dns::resource_record rr: answer.auth.rrs) {
            output_record("\t\t", rr);
        }

        std::cout << "\t" << std::dec << ntohs(answer.h.arcount) << " additional records:\n";
        for (dns::resource_record rr: answer.add.rrs) {
            output_record("\t\t", rr);
        }

    } catch (io::io_exception e) {
        e.show_error();
    }
}