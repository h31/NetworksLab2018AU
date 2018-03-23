#include <stdio.h>
#include <stdlib.h>

#include "../common/connection.h"
#include "../common/handling.h"

uint8_t data_buffer[MAX_UDP_DATA_SIZE];
char message_buffer[MAX_UDP_DATA_SIZE];

int main(void) {
    int sock = get_udp_sock();
    struct sockaddr_in addr = bind_to_port(sock, 8888);

    while(1) {
    	struct sockaddr_in client_addr;

    	socklen_t addr_len = sizeof(client_addr);
    	if (recvfrom(sock, data_buffer, MAX_UDP_DATA_SIZE, 0, (struct sockaddr *) &client_addr, &addr_len) == -1) {
            perror("recvfrom() failed");
            exit(1);
    	}

    	get_query_name(data_buffer, message_buffer);
    	printf("%s\n", message_buffer);

        /*//print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);

        //now reply the client with the same data
        if (sendto(sock, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1)
        {
            die("sendto()");
        }*/
    }

    close(sock);

    return 0;
}
