#ifndef UTILS_H
#define UTILS_H

struct HttpRequest;
typedef struct HttpRequest HttpRequest;

struct HttpResponse;
typedef struct HttpResponse HttpResponse;

void log_http_transaction(int socket_fd, HttpRequest* request, HttpResponse* response);

char* get_date_string(char* buffer);

#endif
