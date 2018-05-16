#include<stdio.h>

#include<sys/socket.h>
#include<arpa/inet.h>

#include "dns.h"

int main()
{
	unsigned char buf[QUERY_SIZE];
	int ret = EXIT_FAILURE;
	int sockin, sockout = -1;
	struct sockaddr_in addr = {};
	struct sockaddr_in addr_dns = {};
	socklen_t sl;
	ssize_t len;

	FILE *fd = popen("cat /etc/resolv.conf | grep -P \"^[ ]*nameserver\" | grep -Po \"(^nameserver )\\K.*\"", "r");
	if (!fd)
		error("popen failed");
	char ip[255];
	if (fscanf(fd, "%s", ip) != 1)
		error("");
	pclose(fd);

	sockin = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockin < 0) {
		ret = sockin;
		goto out;
	}

	sockout = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sockout < 0) {
		ret = sockout;
		goto out;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(DNS_PORT);

	ret = bind(sockin, (struct sockaddr *)&addr, sizeof addr);
	if (ret)
		goto out;

	while (1) {
		sl = sizeof addr;
		len = recvfrom(sockin, (char*)buf, QUERY_SIZE, 0, (struct sockaddr*)&addr, &sl);
		if (len < 0) {
			ret = (int)len;
			goto out;
		}

		addr_dns.sin_family = AF_INET;
		addr_dns.sin_addr.s_addr = inet_addr(ip);
		addr_dns.sin_port = htons(DNS_PORT);

		len = sendto(sockout, buf, (size_t)len, 0, (struct sockaddr*)&addr_dns, sizeof addr_dns);
		if (len < 0) {
			ret = (int)len;
			goto out;
		}

		sl = sizeof addr;
		len = recvfrom(sockout, buf, QUERY_SIZE, 0, (struct sockaddr*)&addr_dns, &sl);
		if (len < 0) {
			ret = (int)len;
			goto out;
		}

		len = sendto(sockin, (char*)buf, (size_t)len, 0, (struct sockaddr*)&addr, sizeof addr);
		if (len < 0) {
			ret = (int)len;
			goto out;
		}
	}

out:
	if (ret != 0)
		perror("");

	if (sockin >= 0) {
		shutdown(sockout, SHUT_RDWR);
		close(sockin);
	}
	if (sockout >= 0) {
		shutdown(sockout, SHUT_RDWR);
		close(sockout);
	}
	return ret;
}