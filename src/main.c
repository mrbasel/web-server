#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "server/server.h"

#define DEFAULT_PORT 8000

HttpResponse* handler(HttpRequest* req, HttpResponse* res) {
    if (strcmp(req->method, "GET") == 0) {
        strcpy(res->body, "Hello, World!\n");
        res->body_len = strlen(res->body);
        res->statusCode = 200;
    }
    return res;
}

void print_error_message() {
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⢶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣦⣄⡀⠀⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⠀⢀⣴⣾⣿⣮⣶⣶⣮⣞⣭⡝⡽⢻⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣬⣛⠿⣿⣿⣿⣷⣄⠀⠀\n");
    printf("⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⣿⣿⢟⣿⡿⣫⣿⣿⠟⣿⣿⣿⣿⣶⣧⣬⣛⠻⡟⣆⠀\n");
    printf("⠀⠀⠀⠀⣰⣿⣿⣿⣿⣿⡿⢅⣿⠏⣠⡿⡿⠋⠀⣿⣿⡿⣹⣿⣿⣿⣿⣿⣧⣿⡀\n");
    printf("⠀⠀⠀⢰⣿⣿⣿⣿⣳⣋⣤⣾⣥⡠⡟⡹⠁⠀⠀⣿⢿⡇⢸⣿⣿⣿⣿⣿⣿⣿⡇\n");
    printf("⠀⠀⢠⣿⣿⣿⣿⢣⠎⣠⣿⣿⣿⡿⠀⠀⠀⠀⠀⢋⢸⢻⢸⣿⠱⣿⣿⣿⣿⣿⡇\n");
    printf("⠀⢠⣿⣿⣿⣿⠁⠸⠀⠿⢿⣿⣿⠀⠀⠀⠀⠀⠀⢨⣶⣶⣬⠏⢹⣟⣿⣿⣿⣿⠃\n");
    printf("⠀⣾⣿⣿⣿⡿⠀⠀⠠⢲⣾⡯⠃⠀⠀⠀⠀⠀⢰⣿⣿⣿⣿⢶⢸⢹⣿⣿⣿⣿⠀\n");
    printf("⣼⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠋⢙⣿⣿⡇⢸⡌⣾⣿⣿⣿⡗⠀\n");
    printf("⣿⣿⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠙⠿⣛⠋⠀⠁⣰⣿⣿⣿⣿⠃⠀\n");
    printf("⣿⣿⣿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿⡏⠀⠀\n");
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⣆⠀⠀⠀⠁⠂⠀⠀⠀⠀⠀⠀⠀⢀⣼⣿⣿⣿⣿⡿⠀⠀⠀\n");
    printf("⣿⣿⣿⣿⣿⣿⣿⣿⠃⠳⢄⢀⡀⢀⡀⡀⢀⣀⣠⣤⣶⣿⣿⣿⣿⣿⣿⠇⠀⠀⠀\n");
    printf("⡟⢸⡿⠃⠉⣠⡿⠃⠀⠀⠈⠹⠿⠯⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀\n");
    printf("⠉⠀⠀⢀⡞⠁⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡿⠀⠹⡿⣽⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀⠀⠀\n");
    printf("⠀⠀⠀⠀⡄⠀⠀⠄⠀⠀⠀⢀⡀⠀⠁⠀⠀⠱⣿⣿⣿⣿⢿⣿⡟⠀⠀⠀⠀⠀⠀\n");
    printf("osaka say that there's somewthing wen't wrong \n")
}

int main(int argc, char* argv[]) {
    size_t workers = 4;
    int port = DEFAULT_PORT;
      // my void 1
      if (argc > 1 && (strtol(argv[1], NULL, 10)) == 0) {
        print_error_message();
        return 1; // exit if invalid port
    } else if (argc > 1) {
        port = strtol(argv[1], NULL, 10);
    }

    // my void 2 
    if (argc > 2 && (strtol(argv[2], NULL, 10)) == 0) {
        print_error_message();
        return 1; // exit if invalid workers
    } else if (argc > 2) {
        workers = strtol(argv[2], NULL, 10);
    }
    Server* server = create_server(port);
    printf("Server listening at %d with %ld workers\n", server->port, workers);
    server_listen(server, handler, 4);
}
