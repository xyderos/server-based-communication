#include <sys/socket.h>

#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "../../src/threaded_sockets/helpers/io_helpers.h"
#include "../../src/threaded_sockets/helpers/thread_helpers.h"
#include "../../src/threaded_sockets/server/server.h"

#define BUFFERS (unsigned)10
#define THREADS 1
#define PORT 10000

static zem_t full, empty, mutex;
static size_t request_index = 0;

static void *
worker(void *arg)
{
	buffer_t *request_buffer = NULL, *req = NULL;
	// convert to thread local data
	size_t current_request = 0;

	while (1) {
		zem_wait(&full);
		zem_wait(&mutex);

		request_buffer = (buffer_t *)arg;

		current_request = request_index;

		request_index = (request_index + 1) % BUFFERS;

		zem_post(&mutex);

		req = &request_buffer[current_request];

		server_handle_request(req);

		close(req->fd);
		zem_post(&empty);
	}
}

int
main(void)
{
	size_t fill = 0, listen_fd = 0;
	int c_fd = 0;
	sockaddr_t c_addr;
	socklen_t c_len = sizeof(c_addr);

	buffer_t buffers[BUFFERS];
	pthread_t workers[THREADS];

	zem_init(&full, 0);
	zem_init(&empty, BUFFERS);
	zem_init(&mutex, 1);

	for (int i = 0; i < THREADS; i++) {
		pthread_create(&workers[i], NULL, &worker, buffers);
	}

	listen_fd = (size_t)server_open_file_descriptor(PORT);

	while (1) {

		zem_wait(&empty);

		c_fd = accept((int)listen_fd, &c_addr, &c_len);

		if (c_fd == -1) {
			zem_post(&empty);
			continue;
		}

		zem_wait(&mutex);

		// update the buffers with the parsed request
		if (server_build_request(c_fd, &buffers[fill]) != OK) {
			zem_post(&mutex);
			zem_post(&empty);
			continue;
		}

		fill = (fill + 1) % BUFFERS;

		zem_post(&mutex);
		zem_post(&full);
	}
}
