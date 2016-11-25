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
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
        n = read(sock, buffer + total, size - total);
        total += n;

        if (n < 0){
            error("Error receiving message");
            exit(1);
        }
    }

    strncpy(buffer, tempBuffer, size);
}

void getDirectory(char* path[]) {
    DIR *d;
    struct dirent *dir;

    d = opendir(".");
    if (d) {
        int i = 0;
        while ((dir = readdir(d)) != NULL) {

            if (dir->d_type == DT_REG) {
                path[i] = dir->d_name;
                i++;
            }
        }
    }
    closedir(d);
}

char* readFile(char* fileName) {
    char *source = NULL;

    FILE* fp = fopen(fileName, "r");

    if (fp == NULL) {
        printf("Unable to open file %s", fileName);
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

int handleRequest(int sock) {

    return 0;
}

char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
    return result;
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
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int quit = 0;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    portno = atoi(argv[1]);
    
    printf("Server open on %d", portno);
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
            exit(0);
        }

    }
    return 0;
}