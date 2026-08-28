# TCP Raspberry Pi Server

This repository is home to my implementation of a basic HTTP server written in C and hosted on a Raspberry Pi.

The main reasoning for this project, in all honesty, came from my frugality. At the end of my wit with my 128GB iPhone being perpetually out of storage, I bought some cloud storage. But through a long overdue tidy up of my desk, I found both a Raspberry Pi and a USB HDD.

Bingo.

This project is written in C, mainly because I like it. I also took university modules covering data communications and protocols such as TCP and UDP. I always found that the best way for me to understand something was to build it myself, and so, here we are.

## Features

* Serve files over HTTP
* Custom HTTP request handler and router
* Basic file type handling
* Manual memory management
* Lightweight design capable of running on an older Raspberry Pi
* No external web server or HTTP framework

## Technologies

* **C**
* **TCP/IP**
* **POSIX sockets**
* **GCC**
* **Make**

## Project Structure

The project is currently undergoing refactoring, with the goal of separating networking, request handling, routing, and file serving into their own modules.

## Architecture

The server follows a simple request-response architecture:

**Client → Socket → Request Parser → Router → File Server → HTTP Response**

The server currently handles connections sequentially, with each client request being processed before the server accepts the next connection.

## Building

Build instructions will be added once the project structure and build system have been finalised.

## How It Works

The server is implemented from scratch using the POSIX socket API. It listens for TCP connections on port `8000`, reads incoming HTTP requests, determines what resource was requested, and serves the corresponding file.

### 1. Create the socket

The server creates a TCP socket using `socket()` with IPv4 and TCP:

`socket(AF_INET, SOCK_STREAM, 0)`

The socket is then bound to port `8000` using `bind()` and placed into a listening state using `listen()`.

### 2. Accept connections

The server waits for incoming clients using `accept()`.

When a client connects, a new socket file descriptor is returned. This is used to communicate with that client.

The server currently handles connections sequentially rather than creating a new thread or process for each connection.

### 3. Read the HTTP request

The incoming request is read from the client socket using `read()`.

A typical HTTP request begins with something similar to:

`GET / HTTP/1.1`

The server extracts the HTTP method and requested path using `sscanf()`.

### 4. Route the request

The extracted method and path are passed to the request router.

Currently, the server has a very simple routing system:

* `GET /` → serves `index.html`
* Other paths → serves `404.html`

Additional routes and HTTP methods will be added as the project develops.

### 5. Locate the requested file

The server determines the appropriate directory based on the requested file extension.

For example:

* `.html` → `public/html/`
* `.js` → `public/js/`

The resulting filesystem path is then opened using `fopen()`.

### 6. Read the file

The server determines the size of the requested file using `fseek()` and `ftell()`.

Memory is allocated for the file using `malloc()`, and the file contents are read into memory using `fread()`.

### 7. Build and send the response

The server constructs an HTTP response containing the status code, content type, and content length.

For example:

`HTTP/1.1 200 OK`

The response headers are sent using `write()`, followed by the contents of the requested file.

### 8. Close the connection

Once the response has been sent, the allocated memory is freed and the client socket is closed.

The server then returns to `accept()` and waits for the next connection.

## Request Flow

A request follows this basic flow:

**Browser → TCP Connection → Accept → Read Request → Parse → Route → Read File → Build Response → Send Response → Close**

## Why I Built This

The main goal of this project is to understand what is actually happening underneath higher-level web frameworks and server software.

Rather than using an existing HTTP library or web server, I am implementing the core functionality myself to learn more about:

* TCP sockets
* HTTP
* POSIX APIs
* File I/O
* Memory management
* C
* Client-server architecture
* Linux systems programming

The project is still under development, with further refactoring and functionality planned.
