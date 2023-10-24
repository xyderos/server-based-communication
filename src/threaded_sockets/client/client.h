#pragma once

// open file descriptor per thread
int client_open_file_descriptor(char *, int);

// request a file based on its name from the given file descriptor
void client_write_to_file_descriptor(int, char *);

// read the content of the response to stdout, can easily be modified
void client_read_from_file_descriptor(int);
