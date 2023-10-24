#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../src/udp/udp.h"
#include "client.h"

int
main(void)
{
	int sfd = udp_open("localhost", "10000", 0);
	ssize_t nread = 0;
	char buf[BUFFER_SIZE] = "first hello";

	for (size_t i = 0; i < 2; i++) {
		printf("Send: %s\n", buf);
		if ((unsigned long)udp_write(sfd, buf, (int)strlen(buf), NULL,
			0) != strlen(buf)) {
			(void)fprintf(stderr, "partial/failed write\n");
			exit(EXIT_FAILURE);
		}

		nread = udp_read(sfd, buf, NULL, 0);

		if (nread == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		printf("Received %zd bytes: %s\n", nread, buf);
		strncpy(buf, "hello", 6);
	}

	close(sfd);
	close_semaphore();
}
