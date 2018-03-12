#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <memory.h>
#include "lists.h"
#include "client.h"
#include "message.h"

CRITICAL_SECTION msg_section;
CONDITION_VARIABLE msg_can_consume;

struct msg_list* msg_tail;
struct msg_list* msg_head;

void free_all(void *arg);

DWORD WINAPI handle_client_read(LPVOID arg) {
    client_t* client = (client_t*) arg;
	EnterCriticalSection(&client->msg_section);
    int n = read_message(client->sock, &client->name);
    if (n < 0) {
        perror("cannot read nick name of client from socket");
		closesocket(client->sock);
		client->is_closed = true;
		LeaveCriticalSection(&client->msg_section);
		WakeConditionVariable(&client->msg_can_consume);
		return 1;
    }
	LeaveCriticalSection(&client->msg_section);
	WakeConditionVariable(&client->msg_can_consume);

    while (1) {
		vector_t read_msg = { .data = NULL, .size = 0, .capacity = 0 };
        n = read_message(client->sock, &read_msg);
        if (n < 0) {
			free_vector(&read_msg);
			EnterCriticalSection(&client->msg_section);
			closesocket(client->sock);
			client->is_closed = true;
			LeaveCriticalSection(&client->msg_section);
			WakeConditionVariable(&client->msg_can_consume);
			return 0;
        }
        struct msg_list* node = malloc(sizeof(struct msg_list));
        node->next = NULL;
        node->client_name = client->name;
        vector_t msg = { .data = NULL, .size = 0, .capacity = 0 };
        msg.size = read_msg.size + client->name.size + 5;
        msg.capacity = msg.size + 1;
        msg.data = malloc(sizeof(char) * msg.capacity);
        snprintf(msg.data, msg.size, "[%s]: %s", client->name.data, read_msg.data);
        msg.data[msg.size] = '\0';
        free_vector(&read_msg);
        node->msg = msg;

        // add message to list and signal all writers
		EnterCriticalSection(&msg_section);
        msg_tail->next = node;
        msg_tail = node;
		LeaveCriticalSection(&msg_section);
		WakeAllConditionVariable(&msg_can_consume);
    }
}

DWORD WINAPI handle_client_write(LPVOID arg) {
    client_t* client = (client_t*) arg;
	EnterCriticalSection(&client->msg_section);
	while (client->name.data == NULL && client->is_closed == false) {
		SleepConditionVariableCS(&client->msg_can_consume, &client->msg_section, INFINITE);
    }
	if (client->is_closed == true) {
		LeaveCriticalSection(&client->msg_section);
		return 1;
	}
	LeaveCriticalSection(&client->msg_section);
    struct msg_list* cur = msg_head;
    while (1) {
		EnterCriticalSection(&msg_section);
		while (cur->next == NULL) {
			EnterCriticalSection(&client->msg_section);
			if (client->is_closed == true) {
				LeaveCriticalSection(&client->msg_section);
				LeaveCriticalSection(&msg_section);
				return 1;
			}
			LeaveCriticalSection(&client->msg_section);
			SleepConditionVariableCS(&msg_can_consume, &msg_section, INFINITE);
        }
        cur = cur->next;
        if (strcmp(client->name.data, cur->client_name.data) != 0) {
            int n = write_message(client->sock, &cur->msg);
            if (n < 0) {
                perror("ERROR: cannot write to client");
				closesocket(client->sock);
				client->is_closed = true;
				LeaveCriticalSection(&msg_section);
				return 1;
			}
        }
		LeaveCriticalSection(&msg_section);
    }
    return 0;
}

DWORD WINAPI server(LPVOID arg) {
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        perror("ERROR: initialize WSAStartup");
        return 1;
    }

	struct addrinfo hints, *res;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     // Fill IP auto

	uint16_t portno = *((uint16_t*)arg);
	char server_port[6];
	snprintf(server_port, 6, "%u", portno);
	if (getaddrinfo(NULL, server_port, &hints, &res) != 0) {
		perror("getaddrinfo failed");
		return 1;
	}

	SOCKET sock;
	if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == INVALID_SOCKET) {
		perror("ERROR: Could not create socket");
		return 1;
	}

	if (bind(sock, res->ai_addr, res->ai_addrlen) == SOCKET_ERROR) {
		perror("ERROR on binding");
		return 1;
	}
    struct thread_list* thread_head = malloc(sizeof(struct thread_list));
    thread_head->next = NULL;
    msg_head = malloc(sizeof(struct msg_list));
    msg_head->next = NULL;
    msg_tail = msg_head;
    struct thread_list* cur = thread_head;
    /* Now start listening for the clients, here process will
       * go in sleep mode and will wait for the incoming connection
    */

    listen(sock, 5);
    struct sockaddr_in cli_addr;
    int clilen = sizeof(cli_addr);

	// initialize global critical section and condition variable
	InitializeCriticalSection(&msg_section);
	InitializeConditionVariable(&msg_can_consume);

    while (1) {
        SOCKET newsock = accept(sock, (struct sockaddr *) &cli_addr, &clilen);
        if (newsock == INVALID_SOCKET) {
            perror("ERROR on accept");
            continue;
        }
        struct thread_list* thread = malloc(sizeof(struct thread_list));
        thread->next = NULL;
        cur->next = thread;
        client_t client;
        client.sock = newsock;
		client.is_closed = false;
        InitializeConditionVariable(&client.msg_can_consume);
		InitializeCriticalSection(&client.msg_section);

        thread->client = client;
		DWORD thread_id;
		thread->reader = CreateThread(NULL, 0, handle_client_read, &thread->client, 0, &thread_id);  
		thread->writer = CreateThread(NULL, 0, handle_client_write, &thread->client, 0, &thread_id);
		cur = cur->next;
    }
    pair_t p = {.msg_head = msg_head, .thread_head = thread_head};
	free_all(&p);
	WSACleanup();
}