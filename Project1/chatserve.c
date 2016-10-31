
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    signal(SIGCHLD,SIG_IGN);

    char buffer[500];
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int quit = 0;
    int n;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

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
                bzero(buffer, 500);
                n = read(newsockfd, buffer, 500);
                if (n > 0) {
                    if(strstr(buffer, "\\quit") != NULL) {
                        printf("Connection closed by client... exiting.\n");
                        quit = 1;
                    }
                    else
                        printf("%s", buffer);
                }
                else {
                    error("Error receiving message");
                }
                if (quit == 0) {
                    printf("Chat_Server> ");
                    bzero(buffer, 500);
                    fgets(buffer, 500, stdin);
                    if(strstr(buffer, "\\quit") != NULL) {
                        quit = 1;
                    }

                    n = write(newsockfd, buffer, strlen(buffer));

                    if (n < 0)
                        error("Error sending message");
                }
            }
            exit(0);
        }

    }
    close(sockfd);
    return 0;
}