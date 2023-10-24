#include "constants.h"

const char *const unimplemented_content =
    "HTTP/1.0 400 Bad Request\r\n"
    "Content-type: text/html\r\n"
    "\r\n"
    "<html>"
    "<head>"
    "<title>Guinga: Unimplemented</title>"
    "</head>"
    "<body>"
    "<h1>Bad Request (Unimplemented)</h1>"
    "<p>Your client sent a request Guinga did not understand and it is probably not your fault.</p>"
    "</body>"
    "</html>";

const char *const http_404_content =
    "HTTP/1.0 404 Not Found\r\n"
    "Content-type: text/html\r\n"
    "\r\n"
    "<html>"
    "<head>"
    "<title>Guinga: Not Found</title>"
    "</head>"
    "<body>"
    "<h1>Not Found (404)</h1>"
    "<p>Your client is asking for an object that was not found on this server.</p>"
    "</body>"
    "</html>";

const char *const server_string = "Server: Guinga/0.1\r\n";

const unsigned int server_port = 8000;

const unsigned int queue_depth = 256;

const unsigned int read_size = 8192;

const unsigned int max_connections = 10;

const unsigned int buffer_size = 1024;
