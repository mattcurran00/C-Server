/**
 * Reminder to the idiot who writes this code that this code must
 * or really should be developed on either the mac or the vivobook
 * or whenever i actually am bothered to convert this desktop to linux
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>

#define PORT 8080 //define port number, global variable?
#define BUFFER_SIZE 4096 // buffer size in kb, 4 is usually enough to store a HTTP request



int main() {

    int serverFd;   //file descriptor for the server port. 
    int clientFd;   //file descriptor for incoming connections

    struct sockaddr_in address; //struct to hold the socket address information
    char buffer[BUFFER_SIZE];   //byte array to hold the incoming http request
    int addrlen = sizeof(address);  //size

    //socket creation
    //AF_INET: Arg basically saying to use IPv4
    //SOCK_STREAM: Use TCP as opposed to UDP 
    //0: let the OS choose the protocol
    serverFd = socket(AF_INET, SOCK_STREAM, 0); 

    //bind it to the port
    address.sin_family = AF_INET; //use ipv4
    address.sin_addr.s_addr = INADDR_ANY;   //listen on all network interfaces 
    address.sin_port = htons(PORT);     //converts CPU byte order to network byte order. Diff CPUs can store numbers differently. this ensures the port is always sent correctly over the network

    bind(serverFd, (struct sockaddr*)&address, sizeof(address)); //attaches this socket to the specified port configured. 

    //listen on said port

    listen(serverFd, 3); //tells the server to listen, and to accept 3 connections in the queue before refusing connections
    printf("Server running on http://localhost:%d\n", PORT); 

    while (1) { //runs forever. Real servers literally never stop accepting connections
        // Accept a connection
        /*Blocks execution until a client connects. When one does, a new file descriptor is returned specifically for that
        client. serverFd stays open, clientFd is just for this one "conversation"
        */
        clientFd = accept(serverFd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        // Read the request
        read(clientFd, buffer, BUFFER_SIZE);
        printf("Request received\n");

        // Send HTTP response
        char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"result\": 42}";

        write(clientFd, response, strlen(response));  //writes the response back to the client and closes the connection
        close(clientFd);
    }

    return 0;
}

