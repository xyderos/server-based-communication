#pragma once

#include "../utils/request.h"

int add_accept_request(int, struct sockaddr_in *, socklen_t *client_addr_len);

int add_read_request(int);

int add_write_request(request_t *req);
