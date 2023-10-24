#pragma once
#define MAX_EVENTS 30000
extern const char *const unimplemented_content;
extern const char *const http_404_content;
extern const char *const server_string;
extern const unsigned int server_port;
extern const unsigned int queue_depth;
extern const unsigned int read_size;
extern const unsigned int max_connections;
extern const unsigned int buffer_size;

enum { EVENT_TYPE_ACCEPT = 0, EVENT_TYPE_READ = 1, EVENT_TYPE_WRITE = 2 };
