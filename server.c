#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#include "http.h"

#define PORT 8080
#define MAX_REQUEST_SIZE 1024

void *send_data(int socket, char *response, char *header){
        size_t buffer_size = strlen(response) + strlen(header) + 1;
        // char* response =  "Hello world\n";
        // char* ok_header = "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n%s";
        char buffer[buffer_size];
        sprintf(buffer, header, strlen(response), response);
        send(socket, buffer, strlen(buffer), 0);
}

char *read_data(FILE *file){
        // Get the file size by using setting file position indicator
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        char *file_buffer = (char *)malloc(file_size + 1);

        fread(file_buffer, 1, file_size, file);
        file_buffer[file_size] = '\0';
        return file_buffer;
}

void *not_found(int socket){
        char* header = "HTTP/1.1 404 NOT FOUND\r\nContent-Length: %ld\r\n\r\n%s";
        FILE *file = fopen("./src/not_found.html", "r");
        char *data = read_data(file);
        send_data(socket, data, header);
}

void *ok(int socket, char* data){
        char* header = "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n%s";
        send_data(socket, data, header);
}

struct HTTPRequest process_request(int socket){
        char request[MAX_REQUEST_SIZE] = {0};
        if((recv(socket, request, MAX_REQUEST_SIZE - 1, 0)) < 0){
                perror("No data supplied.");
                exit(EXIT_FAILURE);
        }
        printf("Request: %s\n", request);

        struct HTTPRequest request_info;
        request_info.method = (char *)malloc(strlen(request) + 1);
        request_info.path = (char *)malloc(strlen(request) + 1);
        request_info.version = (char *)malloc(strlen(request) + 1);

        sscanf(request, "%s %s %s", request_info.method, request_info.path, request_info.version);

        printf("Method: %s\n", request_info.method);
        printf("Path: %s\n", request_info.path);
        printf("Version: %s\n", request_info.version);

        if(strcmp(request_info.path, "/") == 0){
                strcpy(request_info.path, "/index");
        }

        return request_info;
}

int main() {
        struct sockaddr_in address;
        int addr_len = sizeof(address);

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock < 0) {
                perror("Socket could not be created");
                exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_port = htons(PORT);
        address.sin_addr.s_addr = INADDR_ANY;

        int binding = bind(sock, (struct sockaddr *) &address, addr_len);
        if (binding < 0) {
                perror("Socket could not be bound");
                exit(EXIT_FAILURE);
        }

        if(listen(sock, 1) < 0) {
                perror("Error listening to port");
                exit(EXIT_FAILURE);
        }
        printf("Server is now live on port %d\n", PORT);
        
        while(1){
                int new_socket = accept(sock, (struct sockaddr *) &address, (socklen_t *)&addr_len);
                if (new_socket < 0) {
                        perror("Connection could not be made");
                        exit(EXIT_FAILURE);
                }

                struct HTTPRequest request_info = process_request(new_socket);

                char *file_path = (char *) malloc(strlen(request_info.path) + 11);
                if(file_path == NULL){
                        perror("Malloc failed");
                        exit(EXIT_FAILURE);
                }

                sprintf(file_path, "./src%s.html", request_info.path);
                printf("File to fetch: %s", file_path);

                FILE *file = fopen(file_path, "r");
                if (file == NULL) {
                        not_found(new_socket);
                }
                else{
                        char *file_data = read_data(file);
                        ok(new_socket, file_data);
                }
                
                close(new_socket);
        }
        close(sock);
        return 0;
}
