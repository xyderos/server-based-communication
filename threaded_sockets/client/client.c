#include <pthread.h>
#include <unistd.h>

#include "../../src/threaded_sockets/client/client.h"

#define PORT 10000
#define HOST "localhost"

static void *
send_request(void *arg)
{
	char *filename = (char *)arg;
	int client_fd = client_open_file_descriptor(HOST, PORT);

	client_write_to_file_descriptor(client_fd, filename);
	client_read_from_file_descriptor(client_fd);
	close(client_fd);

	return NULL;
}

int
main(void)
{
	pthread_t cgi, file;

	pthread_create(&cgi, NULL, &send_request, "/script");
	pthread_create(&file, NULL, &send_request, "readme.md");

	pthread_join(cgi, NULL);
	pthread_join(file, NULL);
}
