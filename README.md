# Socket Routing Project

## Overview

The Socket Routing Project is a simple server application that demonstrates routing HTTP requests to corresponding files on the server. It listens for incoming connections on a specified port, processes HTTP requests, and serves the requested files back to the client. It is built entirely using C.

## Features

- Basic HTTP server functionality
- Routing of HTTP requests to corresponding files
- Handles CSS files
- Error handling for file not found

## Prerequisites

- gcc

## Running the project

1. Clone the project
2. Build the project
```gcc -o server server.c```
3. Run the server
```./server```
4. Verify it's running correctly by:
``` 
curl http://localhost:8080/
```
or via the web browser