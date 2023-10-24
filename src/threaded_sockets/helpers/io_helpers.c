#include <unistd.h>

#include "io_helpers.h"

ssize_t
read_line(int file_descriptor, void *buffer, size_t length)
{
	char *buffer_pointer = buffer, c = 0;
	long rc = 0;
	size_t n = 0;

	for (; n < length - 1; n++) {
		rc = read(file_descriptor, &c, 1);
		if (rc == 1) {
			*buffer_pointer++ = c;
			if (c == '\n') {
				break;
			}
		} else if (rc == 0) {
			if (n == 1) {
				return 0;
			} else {
				break;
			}
		} else {
			return -1;
		}
	}
	*buffer_pointer = '\0';
	return (ssize_t)n;
}
