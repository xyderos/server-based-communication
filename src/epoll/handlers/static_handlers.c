#include <liburing.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/constants.h"
#include "../network/network.h"
#include "static_handlers.h"

void
handle_unimplemented_method_epoll(int client_socket)
{
	send(client_socket, unimplemented_content,
	    strlen(unimplemented_content), 0);
}

void
handle_http_404_epoll(int client_socket)
{
	send(client_socket, http_404_content, strlen(http_404_content), 0);
}
