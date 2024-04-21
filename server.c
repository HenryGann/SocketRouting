#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define PORT 8080

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

                printf("Here");

                char* response =  "Hello world\n";
                char* ok_header = "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n%s";
                char buffer[1024];
                sprintf(buffer, ok_header, strlen(response), response);
                send(new_socket, buffer, strlen(buffer), 0);

                close(new_socket);
        }
        close(sock);
        return 0;
}
