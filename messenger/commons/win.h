#ifndef __WIN_H__
#define __WIN_H__

#include <winsock2.h>
#include <Ws2tcpip.h>

int read_from_socket(SOCKET sockfd, char *buffer, int len, int sz);
int write_to_socket(SOCKET sockfd, char *buffer, int len);

#endif // __LINUX_H__