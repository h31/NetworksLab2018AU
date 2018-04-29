#ifndef __LINUX_H__
#define __LINUX_H__

int read_from_socket(int sockfd, char *buffer, int len, int sz);
int write_to_socket(int sockfd, char *buffer, int len);

#endif // __LINUX_H__