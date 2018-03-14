#include <packets.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <errors.h>

void write_packet(int sockfd, packet p) {
    int n;

    std::size_t p_size = sizeof(p);

    n = write(sockfd, &p_size, sizeof(p_size));
    check_error(n, SOCKET_WRITE_ERROR);

    n = write(sockfd, &p, p_size);
    check_error(n, SOCKET_WRITE_ERROR);
}

packet read_packet(int sockfd) {
    int n;

    std::size_t p_size;

    n = read(sockfd, &p_size, sizeof(p_size));
    check_error(n, SOCKET_READ_ERROR);

    packet p;

    n = read(sockfd, &p, p_size);
    check_error(n, SOCKET_READ_ERROR);
    
    return p;
}