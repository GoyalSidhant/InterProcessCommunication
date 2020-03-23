#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int main(int argc, char *argv[] ){

    struct sockaddr_un name ; 
    
    int ret;
    int connection_socket;
    int data_socket;
    int result;
    int data;
    char buffer[BUFFER_SIZE];

    /* stops the server creation if the server with same exists */
    unlink(SOCKET_NAME);

    // Creation of master socket 
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if(connection_socket == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("Master socket created\n");

    memset(&name, 0, sizeof(struct sockaddr_un));


    /*Specify the socket Cridentials*/
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    /* Bind socket to socket name.*/

    ret = bind(connection_socket, (const struct sockaddr *) &name,sizeof(struct sockaddr_un));

    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("bind() call succeed\n");

    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for(;;){
        printf("Waiting for accept() system call \n"); 
        data_socket = accept(connection_socket, NULL, NULL);

        if (data_socket == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        
        printf("Connection accepted from client\n");

        for(;;){
            memset(buffer, 0, BUFFER_SIZE);
            printf("Waiting for data from the client\n");
            ret = read(data_socket, buffer, BUFFER_SIZE);

            if (ret == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            memcpy(&data, buffer, sizeof(int));
            if(data == 0) break;
            result += data;

            printf("sending final result back to client\n");
            ret = write(data_socket, buffer, BUFFER_SIZE);
            if (ret == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        /* Close socket. */
        close(data_socket);
        }

    }

    close(connection_socket);
    printf("connection closed..\n");


    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}