#include <stdio.h>
#include <string.h>

#define MAXBUF (8192)

int
main(void)
{
	char content[MAXBUF];
	(void)sprintf(content,
	    "<p>I am a program that is being executed on the server side and returns the result to the client </p>\r\n");

	printf("Content-length: %zu\r\n", strlen(content));
	printf("Content-type: text/html\r\n\r\n");
	printf("%s", content);
	(void)fflush(stdout);
}
