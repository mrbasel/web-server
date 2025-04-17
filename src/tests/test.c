#include <stdio.h>
#include <string.h>
#include "http/request.h"
#include "http/headers.h"
#include "arena/arena.h"

#define ARENA_SIZE 20000

#define AssertTrue(desc, expression) \
++total_tests; \
if(!(expression)) \
{ \
    ++failed_tests; \
    printf("%s(%d): %s failed.\n", __FILE__, __LINE__, desc); \
}


static int total_tests;
static int failed_tests;

void main() {
    {
        Arena* arena = arena_init(ARENA_SIZE);
        char buffer[] = "GET /logo.png HTTP/1.1\r\n";
        HttpRequest* request = parse_request(buffer, arena);
        AssertTrue("method parsed correctly", strcmp(request->method, "GET") == 0);
        AssertTrue("URI parsed correctly", strcmp(request->uri, "/logo.png") == 0);
        arena_free(arena);
    }

    {
        Arena* arena = arena_init(ARENA_SIZE);
        char buffer[] = "GET /logo.png HTTP/1.1\r\nHost: example.com\r\n";
        HttpRequest* request = parse_request(buffer, arena);
        AssertTrue("header name parsed correctly", strcmp(request->headers[0].name, "Host") == 0);
        AssertTrue("header value parsed correctly", strcmp(request->headers[0].value, "example.com") == 0);
        arena_free(arena);
    }

    {
        Arena* arena = arena_init(ARENA_SIZE);
        char buffer[] = "/logo.png HTTP/1.1\r\n";
        HttpRequest* request = parse_request(buffer, arena);
        AssertTrue("invalid request line (missing method) should fail", !request->_is_valid);
        arena_free(arena);
    }

    {
        Arena* arena = arena_init(ARENA_SIZE);
        char buffer[] = "\r\n";
        HttpRequest* request = parse_request(buffer, arena);
        AssertTrue("empty request line should fail", !request->_is_valid);
        arena_free(arena);
    }

    {
        Arena* arena = arena_init(ARENA_SIZE);
        char buffer[] = "GET /logo with spaces.png HTTP/1.1\r\n";
        HttpRequest* request = parse_request(buffer, arena);
        AssertTrue("invalid URI with spaces should fail", !request->_is_valid);
        arena_free(arena);
    }

    {
        Arena* arena = arena_init(ARENA_SIZE);
        char buffer[] = "GET  HTTP/1.1\r\n";
        HttpRequest* request = parse_request(buffer, arena);
        AssertTrue("empty URI should fail", !request->_is_valid);
        arena_free(arena);
    }

    {
        Arena* arena = arena_init(ARENA_SIZE);
        char buffer[] = "GET /logo.png HTTP/9.9\r\n";
        HttpRequest* request = parse_request(buffer, arena);
        AssertTrue("malformed protocol should fail", !request->_is_valid);
        arena_free(arena);
    }

    {
        Arena* arena = arena_init(ARENA_SIZE);
        char buffer[] = "GET /logo.png \r\n";
        HttpRequest* request = parse_request(buffer, arena);
        AssertTrue("missing protocol should fail", !request->_is_valid);
        arena_free(arena);
    }

    printf("%d/%d tests passed.\n", (total_tests - failed_tests), total_tests);
}
