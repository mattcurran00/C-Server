# TCP Raspberry Pi Server

This repository is home to my implementation of a basic server written in C, and hosted 
on my own personal raspberry Pi that I had laying around. 

The main reasoning for this project, in all honesty, came from my frugality. At the end of my wit with my 
128GB iPhone being perpetually out of storage, I bought some cloud storage. But through a long overdue tidy up of my desk, I found both a raspberry Pi, and a USB HDD. Bingo. 

This project is written in C, for the main reason that I like it. Aside from that, I took modules in university that covered data communications and protocols like TCP / UDP. I always found that the best way for me to understand something was to built it myself, and so, here we are. 

## Features

* Options to serve all different types of files
* Custom request handler and router
* Clean seperation of concerns (Although this will be worked on)
* Ultra lightweight design (it runs on a 2014 Pi 1)

## Technologies 
* **C**
* **TCP**
* **GCC**
* **Make**

## Project Structure
TBD

## Architecture
TBD ( I have so much refactoring to do)

## Building
TBD also, its just not polished enough yet

## How it works
This project implements a basic HTTP server from scratch using the standard POSIX socket APIs. It listens for TCP connections on port 8000, parses incoming HTTP requests, determines which resource was requested, and serves the corresponding file from the local filesystem.

1. Create and configure the socket

The server creates a TCP socket using:

socket(AF_INET, SOCK_STREAM, 0);

It then configures an IPv4 address and binds the socket to port 8000 using bind().

The server starts listening for incoming connections with:

listen(serverfd, 3);
2. Accept incoming connections

The server continuously waits for clients using accept().

Each connection produces a client socket file descriptor which is used to communicate with the requesting browser.

The server currently handles connections sequentially rather than creating a new thread or process for each client.

3. Read and parse the HTTP request

Once a connection is accepted, the server reads the incoming request into a buffer using read().

A basic HTTP request looks like:

GET / HTTP/1.1
Host: 192.168.0.121:8000
...

The server extracts the HTTP method and requested path using:

sscanf(buffer, "%15s %255s", method, path);

For example:

Method: GET
Path: /
4. Route the request

The extracted method and path are passed to routeRequest().

Currently, the server supports a very small routing table:

GET /       → public/html/index.html
GET anything else → public/html/404.html

Unsupported HTTP methods are currently ignored.

5. Locate the requested file

The serve() function determines which directory should be used based on the requested file extension.

For example:

.html → public/html/
.js   → public/js/

The resulting filesystem path is then constructed and opened using fopen().

6. Read the file into memory

The server determines the size of the requested file using fseek() and ftell().

It then allocates enough memory to hold the file:

char *servedFile = malloc(fileSize);

The file is read into this buffer using fread().

7. Build and send the HTTP response

The server constructs a basic HTTP response containing:

HTTP status code
Content type
Content length

For example:

HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1234

The response headers are sent using write(), followed by the contents of the requested file.

8. Close the connection

After the file has been sent, the allocated memory is freed and the client socket is closed.

The server then returns to accept() and waits for the next connection.

Request Flow

The overall request lifecycle is:

Browser
   │
   │ TCP connection
   ▼
socket → bind → listen → accept
                     │
                     ▼
                read request
                     │
                     ▼
                parse method/path
                     │
                     ▼
                 route request
                     │
                     ▼
                locate file
                     │
                     ▼
                read file
                     │
                     ▼
              build HTTP response
                     │
                     ▼
                write response
                     │
                     ▼
              close connection

The project is intentionally implemented without an HTTP framework or web server library. The goal is to understand the underlying process of creating a TCP server, handling sockets, parsing HTTP requests, interacting with the filesystem, managing memory, and constructing HTTP responses directly in C.