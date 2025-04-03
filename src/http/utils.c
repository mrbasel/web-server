#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>
#include "http/utils.h"
#include "http/request.h"
#include "http/response.h"

void log_http_transaction(int socket_fd, HttpRequest* request, HttpResponse* response) {
    // TODO: refactor body
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char client_ip[INET_ADDRSTRLEN];

    int peername_err = getpeername(socket_fd, (struct sockaddr*)&client_addr, &addr_len);

    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    int port = ntohs(client_addr.sin_port);

    if (!request->_is_valid) printf("%d %s\n", response->statusCode, response->reason);
    else if (peername_err)
        printf("\"%s %s %s\" %d\n", request->method, request->uri, request->version, response->statusCode);
    else
        printf("%s:%d - \"%s %s %s\" %d\n", client_ip, port, request->method, request->uri, request->version, response->statusCode);
}

static const char *weekdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char* get_date_string(char* buffer) {
    time_t now = time(NULL);
    struct tm gm_time;
    gmtime_r(&now, &gm_time);
    
    sprintf(buffer, "%s, %02d %s %04d %02d:%02d:%02d GMT",
            weekdays[gm_time.tm_wday],
            gm_time.tm_mday,
            months[gm_time.tm_mon],
            gm_time.tm_year + 1900,
            gm_time.tm_hour,
            gm_time.tm_min,
            gm_time.tm_sec);
    
    return buffer;
}
