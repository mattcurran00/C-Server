#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8000
#define BUFF_SIZE 4096

int main() {

    int serverfd;
    int clientfd;

    struct sockaddr_in address;
    char buffer[BUFF_SIZE];
    int addrlen = sizeof(address);

    // Create socket
    serverfd = socket(AF_INET, SOCK_STREAM, 0);

    if (serverfd == -1) {
        perror("socket");
        return 1;
    }

    // Configure address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(serverfd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("bind");
        close(serverfd);
        return 1;
    }

    // Listen for connections
    if (listen(serverfd, 3) == -1) {
        perror("listen");
        close(serverfd);
        return 1;
    }

    fflush(stdout);
    printf("Pi Server listening on http://192.168.0.121:%d\n", PORT);

    while (1) {

        // Accept connection
        clientfd = accept(
            serverfd,
            (struct sockaddr*)&address,
            (socklen_t*)&addrlen
        );

        if (clientfd == -1) {
            perror("accept");
            continue;
        }

        // Read HTTP request
        ssize_t bytesReceived = read(
            clientfd,
            buffer,
            BUFF_SIZE - 1
        );

        if (bytesReceived <= 0) {
            close(clientfd);
            continue;
        }

        // Null terminate request
        buffer[bytesReceived] = '\0';

        // Extract HTTP method and path
        char method[16];
        char path[256];

        sscanf(
            buffer,
            "%15s %255s",
            method,
            path
        );

        printf("Method: %s\n", method);
        printf("Path: %s\n", path);
        fflush(stdout);

        // Handle /
        if (strcmp(path, "/") == 0) {

            FILE *file = fopen("public/index.html", "r");

            if (file == NULL) {
                perror("fopen");
                close(clientfd);
                continue;
            }

            char html[8192];

            size_t bytesRead = fread(
                html,
                1,
                sizeof(html) - 1,
                file
            );

            html[bytesRead] = '\0';

            fclose(file);

            // Build HTTP response header
            char responseHeader[256];

            snprintf(
                responseHeader,
                sizeof(responseHeader),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %zu\r\n"
                "\r\n",
                bytesRead
            );

            // Send HTTP header
            write(
                clientfd,
                responseHeader,
                strlen(responseHeader)
            );

            // Send HTML
            write(
                clientfd,
                html,
                bytesRead
            );

            printf("Served public/index.html\n");
        }

        // Handle unknown paths
        else {

            char *response =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/plain\r\n"
                "\r\n"
                "404 Not Found";

            write(
                clientfd,
                response,
                strlen(response)
            );

            printf("404: %s\n", path);
        }

        // Close connection
        close(clientfd);
    }

    close(serverfd);

    return 0;
}