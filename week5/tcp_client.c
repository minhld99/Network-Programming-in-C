#include <stdio.h>
#include <stdlib.h>
#include <string.h> // bzero()
#include <ctype.h> // isspace()
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_ntoa(), inet_aton()
#include <unistd.h> // close(sockfd), sleep()

#define BUFF_SIZE 100

/* ---------------- Function Declaration ---------------- */

int is_empty(const char *s);
int is_number(const char *s);

/* -------------------------- Main Function -------------------------- */
int main(int argc, char **argv) {
    if (argc < 3) {
        printf("error: too few arguments, expected 3, have %d\n", argc);
        printf("Usage:\t%s IPAddress PortNumber\n (For example: %s 127.0.0.1 5500)\n", argv[0], argv[0]);
        exit(-1);
    }
    else if (argc > 3) {
        printf("error: too many arguments, expected 3, have %d\n", argc);
        printf("Usage:\t%s IPAddress PortNumber\n (For example: %s 127.0.0.1 5500)\n", argv[0], argv[0]);
        exit(-1);
    }

    struct in_addr ipv4;
    int valid = inet_pton(AF_INET, argv[1], &ipv4);
    if (valid <= 0) { // If not ipv4
        if (valid == 0) printf("Not a valid ip address\n");
        else perror("error inet_pton");
        exit(EXIT_FAILURE);
    }

    if ((!is_number(argv[2])) || atoi(argv[2]) > 65535 || atoi(argv[2]) < 0) {
        printf("Invalid port number\n");
        exit(-1);
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[BUFF_SIZE], recvline[BUFF_SIZE];
    char SERV_ADDR[50];
    strcpy(SERV_ADDR, argv[1]);
    const unsigned short SERV_PORT = atoi(argv[2]);

    // Create a socket for the client
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Problem in creating the socket");
        exit(2);
    }

    // Creation of the remote server socket information structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR);
    servaddr.sin_port = htons(SERV_PORT); // convert to big-endian order

    // Connect the client to the server socket
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Problem in connecting to the server");
        exit(3);
    }
    printf("%s", "Send to server: ");
    while(fgets(sendline, BUFF_SIZE, stdin) != NULL) {
        char *offset = strstr(sendline, "\n");
        if (offset != NULL) *offset = '\0';
        send(sockfd, sendline, strlen(sendline), 0);

        if(recv(sockfd, recvline, BUFF_SIZE, 0) == 0) {
            // error: server terminated prematurely
            perror("The server terminated prematurely");
            exit(4);
        }
        printf("%s", "Send to server: ");
    }
    
    return 0;
}

/* -------------------------- Utilities --------------------------- */

int is_empty(const char *s) {
    while (*s != '\0') {
        if (!isspace((unsigned char)*s))
            return 0; // string is not empty
        s++;
    }
    return 1; // string is empty
}

int is_number(const char *s) {
    while (*s != '\0') {
        if (!isdigit((unsigned char)*s))
            return 0; // string is not number
        s++;
    }
    return 1; // string is number
}