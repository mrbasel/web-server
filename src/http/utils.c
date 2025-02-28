#include <stdio.h>
#include <time.h>
#include "http/utils.h"
#include "http/request.h"
#include "http/response.h"

void log_http_transaction(HttpRequest* request, HttpResponse* response) {
    if (!request->method || !request->uri || !request->version) printf("GET / HTTP/1.1 %d\n", response->statusCode);
    else
        printf("%s %s %s %d\n", request->method, request->uri, request->version, response->statusCode);
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
