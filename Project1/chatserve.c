/* chatserve.c
 * By: Rob Navarro
 * CS372
 *
 * Usage: ./chatserve [port]
 * This program starts a chat server that can be connected to by different clients.
 * Several different sources were used while making this program. My main references consisted of:
 * - http://www.linuxhowtos.org/C_C++/socket.htm
 * - http://beej.us/guide/bgnet/output/html/multipage/index.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
 * An error function that prints out the error message and exits the program.
 * The code is referenced from: http://www.linuxhowtos.org/C_C++/socket.htm
 */
void error(char *msg) {
    perror(msg);
    exit(1);
}

/*
 * This function is used to set up the server.
 * Much of the code in this section was referenced from:
 * http://www.linuxhowtos.org/C_C++/socket.htm
 *
 * The socket is configured to listen for up to 5 connections.
 */
void startUp(int* sockfd, socklen_t* clilen, struct sockaddr_in* cli_add, int portno) {
    struct sockaddr_in serv_addr;

    *sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(*sockfd,5);
    *clilen = sizeof(cli_add);
}

/*
 * This function is used to send a message from the server to the client.
 * The parameters needed are the socket, buffer, and a reference to quit, which is used to exit the chat client.
 * The function checks if the server is sending a quit command. If so quit is set to 1.
 *
 */
void sendMessage(int sock, char buffer[], int* quit) {
    int n;

    printf("Chat_Server> ");
    bzero(buffer, 512);
    fgets(buffer, 500, stdin);
    if(strstr(buffer, "\\quit") != NULL) {
        *quit = 1;
    }

    n = write(sock, buffer, strlen(buffer));

    if (n < 0)
        error("Error sending message");

}

/*
 * This function is used to send a message from the server to the client.
 * The parameters needed are the socket, buffer, and a reference to quit, which is used to exit the chat client.
 * The buffer is checked for \quit after the message is received. If the client sent quit the program exits.
 */
void receiveMessage(int sock, char buffer[], int *quit) {
    int n;
    bzero(buffer, 512);
    n = read(sock, buffer, 512);
    if (n > 0) {
        if(strstr(buffer, "\\quit") != NULL) {
            printf("Connection closed by client... exiting.\n");
            *quit = 1;
        }
        else
            printf("%s", buffer);
    }
    else {
        error("Error receiving message");
    }
}

/*
 * The main function is used to process the work done in the chat server.
 * A while loop is used to fork of new chat processes as different clients connect.
 * Another while loop is used to continue the 2 way chat until the client or server sends '\quit'.
 */
int main(int argc, char *argv[]) {

    /*
     * The buffer is made to be a bit larger since we receive the handle from the client as well. The extra 12 chars
     * account for 10charname>. The > and following space add two more chars.
     */
    char buffer[512];
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int quit = 0;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    portno = atoi(argv[1]);

    startUp(&sockfd, &clilen, &cli_addr, portno);

    while(1) {
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
        if(newsockfd < 0) {
            error("Error on accept");
        }
        pid = fork();
        if (pid < 0) {
            error("Error on fork");
        }

        if (pid == 0) {
            close(sockfd);
            while(quit == 0) {
                receiveMessage(newsockfd, buffer, &quit);

                if (quit == 0) {
                    sendMessage(newsockfd, buffer, &quit);
                }
            }
            exit(0);
        }

    }
    return 0;
}