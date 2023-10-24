#include <netinet/in.h>

#include <liburing.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/constants.h"
#include "globals.h"
#include "listening_socket.h"

int
setup_listening_socket(int port)
{
	int sock = 0, enable = 1;
	struct sockaddr_in srv_addr;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		exit(1);
	}

	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) <
	    0) {
		exit(1);
	}

	memset(&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons((unsigned short)port);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (const struct sockaddr *)&srv_addr, sizeof(srv_addr)) <
	    0) {
		exit(1);
	}

	if (listen(sock, max_connections) < 0) {
		exit(1);
	}

	return sock;
}
