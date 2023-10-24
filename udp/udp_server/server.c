#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../../src/udp/udp.h"
#include "server.h"

int
main(void)
{
	struct sockaddr_storage peer_addr;
	socklen_t peer_addr_len = 0;
	ssize_t nread = 0;
	char buf[BUFFER_SIZE];
	int sfd = udp_open(NULL, "10000", 1), s = 0;
	assert(sfd > -1);

	for (;;) {
		peer_addr_len = sizeof(struct sockaddr_storage);

		nread = udp_read(sfd, buf, (struct sockaddr *)&peer_addr,
		    &peer_addr_len);
		if (nread == -1) {
			continue;
		}

		char host[NI_MAXHOST], service[NI_MAXSERV];

		s = getnameinfo((struct sockaddr *)&peer_addr, peer_addr_len,
		    host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);

		if (!s) {
			printf("Received %zd bytes from %s:%s: %s\n", nread,
			    host, service, buf);
		} else {
			(void)fprintf(stderr, "getnameinfo: %s\n",
			    gai_strerror(s));
		}
	}
}
