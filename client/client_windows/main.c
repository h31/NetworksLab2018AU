#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <memory.h>
#include "vector.h"
#include "client.h"

#pragma comment (lib, "ws2_32.lib")

void get_options(int argc, char** argv, char** server_addr,
                 uint16_t* server_port, char** nick_name);
DWORD WINAPI reader(LPVOID arg);
DWORD WINAPI writer(LPVOID arg);

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
		getchar();
        exit(1);
    }
    uint16_t portno;
    char* server_addr;
    char* nick_name;
    get_options(argc, argv, &server_addr, &portno, &nick_name);

	// initialize winsock
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		fprintf(stderr, "Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	char server_port[6];
	snprintf(server_port, 6, "%u", portno);

	if (getaddrinfo(server_addr, server_port, &hints, &res) != 0) {
		perror("ERROR: getaddrinfo failed");
		return 1;
	}

    /* Create a socket point */
    SOCKET sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == INVALID_SOCKET) {
        perror("ERROR opening socket");
        exit(1);
    }	

	/* Now connect to the server */
    if (connect(sock, res->ai_addr, res->ai_addrlen) == SOCKET_ERROR) {
        perror("ERROR connecting");
		freeaddrinfo(res);
        exit(1);
    }

    /* Now ask for a name from the user, this name
       * will be read by server
    */

    vector_t name;
    name.data = nick_name;
    name.size = strlen(nick_name);
    name.capacity = name.size;
    client_t client;
    client.sock = sock;
    client.name = name;
	client.is_closed = false;
	InitializeCriticalSection(&client.msg_section);
	InitializeConditionVariable(&client.msg_can_consume);
	DWORD thread_id;
	HANDLE reader_t = CreateThread(NULL, 0, reader, &client, 0, &thread_id);
	HANDLE writer_t = CreateThread(NULL, 0, writer, &client, 0, &thread_id);
	WaitForSingleObject(reader_t, INFINITE);
	WaitForSingleObject(writer_t, INFINITE);
	CloseHandle(reader_t);
	CloseHandle(writer_t);
    free(server_addr);
    free_client(&client);
	freeaddrinfo(res);
}