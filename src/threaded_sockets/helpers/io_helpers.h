#pragma once

#include <sys/types.h>

#define MAXBUF (8192)

typedef struct sockaddr sockaddr_t;

ssize_t read_line(int, void *, size_t);
