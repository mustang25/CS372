/* chatserve.c
 * By: Rob Navarro
 * CS372
 *
 * Usage: ./chatserve [port]
 * This program starts a chat server that can be connected to by different clients.
 * Several different sources were used while making this program. My main references consisted of:
 * - http://www.linuxhowtos.org/C_C++/socket.htm
 * - http://beej.us/guide/bgnet/output/html/multipage/index.html
 * - http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
 * - http://beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>

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
 * The socket is configured to listen for up to 10 connections.
 */
int startUp(int portno) {
    int sockfd;

    // failure
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return -1;
    }

    // some necessary structs and info
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(portno);
    server.sin_addr.s_addr = INADDR_ANY;

// http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#setsockoptman
/*******************************************************************************/
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
/*******************************************************************************/

    // failure
    if(bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0){
        return -1;
    }

    // failure
    if(listen(sockfd, 10) < 0){
        return -1;
    }

    // success
    return sockfd;
}


/*
 * This function is used to send a message from the server to the client.
 * The parameters needed are the socket, buffer, and a reference to quit, which is used to exit the chat client.
 * The function checks if the server is sending a quit command. If so quit is set to 1.
 *
 */
void sendMessage(int sock, char* buffer) {
    ssize_t n;
    size_t size = strlen(buffer) + 1;
    size_t total = 0;

    while (total < size) {
        n = write(sock, buffer, size - total);

        total += n;

        if (n < 0) {
            error("Error sending message");
            exit(1);
        }

        else if (n == 0) {
            total = size - total;
        }
    }
}

/*
 * This function is used to send a message from the server to the client.
 * The parameters needed are the socket, buffer, and a reference to quit, which is used to exit the chat client.
 * The buffer is checked for \quit after the message is received. If the client sent quit the program exits.
 */
void receiveMessage(int sock, char* buffer, size_t size) {
    char tempBuffer[size + 1];
    ssize_t n;
    size_t total = 0;

    while (total < size) {
        n = read(sock, tempBuffer + total, size - total);
        total += n;
        if (n < 0){
            error("Error receiving message");
            exit(1);
        }
    }
    strncpy(buffer, tempBuffer, size);
}


int getDirectory(char* path[]) {
    DIR *d;
    struct dirent *dir;
    int totalSize = 0;
    int totalFiles = 0;

    d = opendir(".");
    if (d) {
        int i = 0;
        while ((dir = readdir(d)) != NULL) {

            if (dir->d_type == DT_REG) {
                path[i] = dir->d_name;
                totalSize += strlen(path[i]);
                i++;
            }
        }
        totalFiles = i - 1;
    }
    closedir(d);
    return totalSize + totalFiles;
}

char* readFile(char* fileName) {
    char *source = NULL;

    FILE* fp = fopen(fileName, "r");

    if (fp == NULL) {
        error("Unable to open file");
    }

    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long bufSize = ftell(fp);
            if (bufSize == -1) {
                error("Invalid file");
                exit(1);
            }
            /* Allocate our buffer to that size. */
            source = malloc(sizeof(char) * (bufSize + 1));

            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

            /* Read the entire file into memory. */
            size_t newLen = fread(source, sizeof(char), bufSize, fp);
            if ( ferror( fp ) != 0 ) {
                fputs("Error reading file", stderr);
            } else {
                source[newLen++] = '\0'; /* Just to be safe. */
            }
        }
    }
    fclose(fp);
    return source;
}

int receiveNumber(int sock) {
    int num;
    ssize_t n = 0;
    n = read(sock, &num, sizeof(int));

    if (n < 0) {
        error("Unable to receive number through socket.");
    }
    return (num);
}

void sendNumber(int sock, int num) {
    ssize_t n = 0;

    n = write(sock, &num, sizeof(int));
    if (n < 0) {
        error("Unable to send number through socket.");
    }
}

int handleRequest(int sock, int* dataPort) {
    char command[3] = "\0";

    receiveMessage(sock, command, 3);
    *dataPort = receiveNumber(sock);


    if (strcmp(command, "-l") == 0) {
        return 1;
    }

    if (strcmp(command, "-g") == 0) {
        return 2;
    }

    return 0;
}

void sendFile(int sock, char* fileName) {
    char* toSend;
    toSend = readFile(fileName);

    sendNumber(sock, strlen(toSend));
    sendMessage(sock, toSend);
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
    int sockfd, newsockfd, datasockfd, portno, pid;
    socklen_t clilen, data_clilen;
    struct sockaddr_in serv_addr, cli_addr, data_serv_addr, data_cli_addr;
    int quit = 0;

    if (argc < 2) {
        error("Usage: ftserver [portNumber]\n");
        exit(1);
    }

    portno = atoi(argv[1]);
    if (portno < 1024 || portno > 65535) {
        error("Invalid port number! Must be within 1024 - 65535\n");
    }

    sockfd = startUp(portno);
    printf("Server open on %d\n", portno);

    while(1) {
        newsockfd = accept(sockfd,NULL, NULL);
        if(newsockfd < 0) {
            error("Error on accept\n");
        }
        pid = fork();
        if (pid < 0) {
            error("Error on fork\n");
        }

        if (pid == 0) {
            close(sockfd);
            int command = 0;
            int dataPort;
            int newsock;
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(cli_addr.sin_addr), clientIP, INET_ADDRSTRLEN);

            printf("Connection from %s.\n", clientIP);
            command = handleRequest(newsockfd, &dataPort);

            if (command == 0) {
                printf("Did not receive -l\n");
            }

            if (command == 1) {
                char* path[100];
                int i = 0;
                int length = 0;
                printf("List directory requested on port %d.\n", dataPort);
                length = getDirectory(path);

                newsock = startUp(dataPort);
                datasockfd = accept(newsock, NULL, NULL);
                if (datasockfd < 0) {
                    error("Unable to open data socket");
                }
                sendNumber(datasockfd, length);
                while (path[i] != NULL) {
                    sendMessage(datasockfd, path[i]);
                    i++;
                }
                close(newsock);
                close(datasockfd);
                exit(0);
            }

            if (command == 2) {
                int i = receiveNumber(newsockfd);
                char fileName[255] = "\0";
                receiveMessage(newsockfd, fileName, i);
                printf("File \"%s\" requested on port %d\n", fileName, dataPort);

                if (access(fileName, F_OK) == -1) {
                    printf("File not found. Sending error message to %s: %d\n", clientIP, portno);
                    char errorMessage[] = "FILE NOT FOUND!";
                    sendNumber(newsockfd, strlen(errorMessage));
                    sendMessage(newsockfd, errorMessage);
                    close(newsock);
                    close(datasockfd);
                    exit(1);
                }
                else {
                    char message[] = "FOUND!";
                    sendNumber(newsockfd, strlen(message));
                    sendMessage(newsockfd, message);
                }
                printf("Sending \"%s\" to %s: %d\n", fileName, clientIP, dataPort);
                
                newsock = startUp(dataPort);
                datasockfd = accept(newsock, NULL, NULL);
                if (datasockfd < 0) {
                    error("Unable to open data socket");
                }
                sendFile(datasockfd, fileName);
                close(newsock);
                close(datasockfd);
                exit(0);
            }
            exit(0);
        }

    }
}