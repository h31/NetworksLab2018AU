#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "vector.h"
#include "client.h"
#include "message.h"

typedef enum {READ, WRITE} client_mode;
client_mode mode = WRITE;

void read_line(vector_t* msg) {
    char ch;
    msg->size = 0;
    msg->capacity = 256;
    msg->data = malloc(sizeof(char) * msg->capacity);
	memset(msg->data, '\0', msg->capacity);
    scanf("%c", &ch);
    while (ch != '\n') {
        append(msg, ch);
        scanf("%c", &ch);
    }
}

DWORD WINAPI reader(LPVOID arg) {
    client_t* client = (client_t*) arg;
    SOCKET sock = client->sock;
    while (1) {
        vector_t msg = { .data = NULL,.size = 0,.capacity = 0 };
        int n = read_message(sock, &msg);
		if (n < 0) {
			free_vector(&msg);
			EnterCriticalSection(&client->msg_section);
			closesocket(client->sock);
			client->is_closed = true;
			LeaveCriticalSection(&client->msg_section);
			return 1;
		}

		EnterCriticalSection(&client->msg_section);
        while (mode != READ && client->is_closed == false) {
			SleepConditionVariableCS(&client->msg_can_consume, &client->msg_section, INFINITE);
        }
		if (client->is_closed == true) {
			LeaveCriticalSection(&client->msg_section);
			return 0;
		}
        printf("%s\n", msg.data);
		LeaveCriticalSection(&client->msg_section);
	}
}


DWORD WINAPI writer(LPVOID arg) {
    client_t* client = (client_t*) arg;
    vector_t name = client->name;

    SOCKET sock = client->sock;
	EnterCriticalSection(&client->msg_section);
    int n = write_message(sock, &name);
    if (n < 0) {
        perror("ERROR: cannot write to socket");
		closesocket(client->sock);
		client->is_closed = true;
		LeaveCriticalSection(&client->msg_section);
		WakeConditionVariable(&client->msg_can_consume);
		return 1;
    }
    mode = READ;
	LeaveCriticalSection(&client->msg_section);
	WakeConditionVariable(&client->msg_can_consume);

    while (1) {
        char m = '\0';
        while (m != 'm') {
            scanf("%c", &m);
        }
        getchar();
        
		EnterCriticalSection(&client->msg_section);
		mode = WRITE;
        printf("[%s]: ", name.data);
        vector_t msg;
        read_line(&msg);
        mode = READ;
        int n = write_message(sock, &msg);
        if (n < 0) {
			free_vector(&msg);
			closesocket(client->sock);
			client->is_closed = true;
			LeaveCriticalSection(&client->msg_section);
			WakeConditionVariable(&client->msg_can_consume);
			return 1;
        }
		LeaveCriticalSection(&client->msg_section);
		WakeConditionVariable(&client->msg_can_consume);
    }
}
