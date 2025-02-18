struct HttpRequest;
typedef struct HttpRequest HttpRequest;

struct HttpResponse;
typedef struct HttpResponse HttpResponse;

void log_http_transaction(HttpRequest* request, HttpResponse* response);

char* get_date_string(char* buffer);
