#include <check.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/epoll/server/server.h"

int
main(void)
{
	serve_epoll();
	return 0;
}
