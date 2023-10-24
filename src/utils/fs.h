#pragma once

#include <sys/stat.h>

#include <liburing.h>

void transfer_file_contents(char *, int, off_t);

const char *get_filename_ext(const char *);

void copy_file_contents(char *, off_t, struct iovec *);
