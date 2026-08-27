#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8000
#define BUFF_SIZE 4096

void readRequest(int clientfd, char *buffer);
void routeRequest(int cfd, char *method, char *path);
void buildResponse(char *response, size_t bRead, size_t responseSize);
char *getDir(char *path);
void serve(int cfd, char *path);

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

        readRequest(clientfd, buffer);
        close(clientfd);
    }

    close(serverfd);

    return 0;
}

void routeRequest(int cfd, char *method, char *path){

    if(strcmp(method, "GET") == 0){
        if(strcmp(path, "/") == 0){
            serve(cfd, "/index.html");
            return;
        }
    }
    return;
}

void readRequest(int clientfd, char *buffer){

    ssize_t bRecv = read(clientfd, buffer, BUFF_SIZE - 1);

    if(bRecv <= 0){
        close(clientfd);
        return;
    }
    
    buffer[bRecv] = '\0'; //null terminate request

    char method[16];
    char path[256];

    sscanf(buffer, "%15s %255s", method, path);

    routeRequest(clientfd, method, path);
}

void serve(int cfd, char *path){

    char filePath[256];
    char response[256];

    char *directory = getDir(path);

    if(directory == NULL){
        printf("ERROR: Unsupported File Type!\n");
        return;
    }
    snprintf(filePath, sizeof(filePath), "public/%s%s", directory, path); //and now we have the path

    FILE *file = fopen(filePath, "r");
    if(file == NULL){
        perror("fopen");
        return;
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if(fileSize < 0){
        fprintf(stderr, "ERROR: Could not determine file size!\n");
        fclose(file);
        return;
    }
    char *servedFile = malloc(fileSize);

    if(servedFile == NULL){
        fprintf(stderr, "ERROR: Failed to allocate memory!\n");
        fclose(file);
        return;
    }

    size_t bRead = fread(servedFile, 1, fileSize, file);
    fclose(file);

    buildResponse(response, bRead, sizeof(response));

    write(cfd, response, strlen(response));
    write(cfd, servedFile, bRead);

    printf("DEBUG: Serving...\n");

    free(servedFile);
}

/**
 * helper function to determine what path to use when using the local filesystem
 */
char *getDir(char *path){

    char *extension = strrchr(path, '.');

    if (extension == NULL){
        return NULL;
    }
    if (strcmp(extension, ".html") == 0){
        return "html";
    }
    if (strcmp(extension, ".js") == 0) {
        return "js";
    }

    return NULL;
}

void buildResponse(char *response, size_t bRead, size_t responseSize){

    snprintf(response, responseSize,
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: %zu\r\n"
    "\r\n",
    bRead);

    return;
}