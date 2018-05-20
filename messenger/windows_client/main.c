#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>



#include <string.h>
#include <stdint.h>

#include <time.h>

HANDLE input_mode_mtx;

char is_input_mode;

char force_read(SOCKET sockfd, char *buffer, int len) {
    char size = len;
    while (size != 0) {
        int n = recv(sockfd, buffer + len - size, size, 0); // recv on Windows

        if (n < 0) {
            return 0;
        }

        size -= n;
    }
    return 1;
}

char read_message(SOCKET sockfd, char *buffer) {
    char len = 0;
    if (!force_read(sockfd, &len, 1)) {
        return 0;
    }

    if (!force_read(sockfd, buffer, len)) {
        return 0;
    }

    return 1;
}


DWORD WINAPI server_handler(CONST LPVOID arg) {
    int sockfd = *(int*)arg;
    char buffer[256];
    while (1) {

        /* Now read server response */
		memset(buffer, 0, 256);
        if (!read_message(sockfd, buffer)) {
            perror("ERROR reading from socket");
            exit(1);
        }

		WaitForSingleObject(input_mode_mtx, INFINITE);

        while (is_input_mode) {
            Sleep(1);
        }
        
        time_t t = time(NULL);
        struct tm* lt = localtime(&t);
        printf("<%02d:%02d> %s", lt->tm_hour, lt->tm_min, buffer);

		ReleaseMutex(input_mode_mtx);
    }
    return NULL;
}


char force_send(SOCKET sockfd, char *buffer, int len) {
    int n = len;
    while (n != 0) {
        int r = send(sockfd, buffer + len - n, n, 0);
        if (r < 0) {
            return 0;
        }
        n -= r;
    }
    return 1;
}

char send_message(SOCKET sockfd, char *nickname, char *text) {
    char len = strlen(nickname) + 3 + strlen(text) + 1;

    if (!force_send(sockfd, &len, 1)) {
        return 0;
    }

    if (!force_send(sockfd, "[", 1)) {
        return 0;
    }

    if (!force_send(sockfd, nickname, strlen(nickname))) {
        return 0;
    }

    if (!force_send(sockfd, "] ", 2)) {
        return 0;
    }

    if (!force_send(sockfd, text, strlen(text) + 1)) {
        return 0;
    }

    return 1;
}

int __cdecl main(int argc, wchar_t **argv) {
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	input_mode_mtx = CreateMutex(NULL, FALSE, NULL);
    char *nickname = NULL;
    uint16_t portno = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server = NULL;
	int iResult;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

    char buffer[256];

    if (argc != 4) {
        fprintf(stderr, "usage: client_windows hostname port nickname\n");
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
	iResult = getaddrinfo(argv[1], argv[2], &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}


    nickname = argv[3];

    if (strlen(nickname) > 40) {
        fprintf(stderr, "ERROR, nickname very long\n");
        exit(1);
    }

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

    /* Now ask for a message from the user, this message
       * will be read by server
    */

	CreateThread(NULL, 0, &server_handler, &ConnectSocket, 0, NULL);

    while (1) {
		memset(buffer, 0, 256);
        fgets(buffer, 200, stdin);
        while (strcmp(buffer, "m\n") != 0) {
            if (strcmp(buffer, "exit\n") == 0) {
				closesocket(ConnectSocket);
                return 0;
            }

            printf("Invalid input\n");
			memset(buffer, 0, 256);
            fgets(buffer, 200, stdin);
        }

		WaitForSingleObject(input_mode_mtx, INFINITE);
        is_input_mode = 1;
		ReleaseMutex(input_mode_mtx);

        printf("Please enter the message: ");
		memset(buffer, 0, 256);
        fgets(buffer, 200, stdin);
        is_input_mode = 0;

        /* Send message to the server */

        if (!send_message(ConnectSocket, nickname, buffer)) {
            perror("ERROR writing to socket");
            exit(1);
        }
    }

    return 0;
}
