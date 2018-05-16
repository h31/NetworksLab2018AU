#include <stdio.h>
#include <WinSock2.h>
#include <stdint.h>

#pragma comment (lib, "ws2_32.lib")

void get_options(int argc, char** argv, uint16_t* server_port);
DWORD WINAPI server(LPVOID arg);

int main(int argc, char *argv[]) {
    uint16_t portno;
    get_options(argc, argv, &portno);
    DWORD server_thread_id;
    HANDLE server_thread_handle = CreateThread(NULL, 0, server, &portno, 0, &server_thread_id);
    WaitForSingleObject(server_thread_handle, INFINITE);
    CloseHandle(server_thread_handle);
	return 0;
}
