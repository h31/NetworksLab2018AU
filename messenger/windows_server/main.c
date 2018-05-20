#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <string.h>
#include <stdint.h>


#define MAX_COUNT_CLIENTS 100

SOCKET clients[MAX_COUNT_CLIENTS];
char is_active[MAX_COUNT_CLIENTS];
int count_active_clients;

HANDLE mtx;

static int reserve_socket_cell() {
    for (size_t i = 0; i < sizeof(is_active) / sizeof(is_active[0]); i++) {
        if (is_active[i] == 0) {
			WaitForSingleObject(mtx, INFINITE);
            ++count_active_clients;
            is_active[i] = 1;
			ReleaseMutex(mtx);
            return i;
        }
    }
    return -1;
}

static inline void free_socket_cell(int cell) {
	WaitForSingleObject(mtx, INFINITE);
    is_active[cell] = 0;
    --count_active_clients;
	ReleaseMutex(mtx);
}

static inline void notify_all(char *buffer, char message_len, int skip) {
    for (int i = 0; i < MAX_COUNT_CLIENTS; i++) {
        if (i == skip || is_active[i] == 0)
            continue;

        int n = 1;
        while (n != 0) {
            int r = send(clients[i], &message_len, n, 0);

            if (r < 0) {
                continue;
            }

            n -= r;
        } 

        n = message_len;

        while (n != 0) {
            int r = send(clients[i], buffer + message_len - n, n, 0);

            if (r < 0) {
                continue;
            }

            n -= r;
        }
    }
}

DWORD WINAPI client_handler(CONST LPVOID arg) {
	mtx = CreateMutex(NULL, FALSE, NULL);
    int cell = (char*)arg - is_active;
    char buffer[256];

    while (1) {
        char message_len = 0;
        int n = recv(clients[cell], &message_len, 1, 0); // recv on Windows

        if (n == 0) {
            continue;
        }

        if (n < 0) {
            perror("ERROR reading from socket");
            break;
        }

		memset(buffer, 0, 256);
        
        char size = message_len;
        while (size != 0) {
            n = recv(clients[cell], buffer + message_len - size, size, 0); // recv on Windows

            if (n < 0) {
                perror("ERROR reading from socket");
                break;
            }

            size -= n;
        }

        printf("Message get: %s\n", buffer);

        notify_all(buffer, message_len, cell);
    }

    free_socket_cell(cell);

	ExitThread(0);
}

int __cdecl main() {
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

    if (argc != 2) {
        fprintf(stderr, "usage: server_windows port\n");
        exit(0);
    }


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo("localhost", argv[1], &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
   

    /* Accept actual connection from the client */
    while (1) {

		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			continue;
		}

        if (count_active_clients + 1 > MAX_COUNT_CLIENTS) {
            perror("Customer limit exceeded");
			closesocket(ClientSocket);
            continue;
        }

        int cell = reserve_socket_cell();

        if (cell == -1) {
            perror("Customer limit exceeded");
			closesocket(ClientSocket);
            continue;
        }

        clients[cell] = ClientSocket;

		CreateThread(NULL, 0, &client_handler, is_active + cell, 0, NULL);

    }
}
