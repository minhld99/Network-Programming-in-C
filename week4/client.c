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

    int sockfd, rcvBytes, sendBytes;
    socklen_t len;
    char buff[BUFF_SIZE];
    char SERV_ADDR[50];
    struct sockaddr_in servaddr;

    strcpy(SERV_ADDR, argv[1]);
    const unsigned short SERV_PORT = atoi(argv[2]);
    //Step 1: Construct socket
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        perror("Error: ");
        return 0;
    }
    //Step 2: Define the address of the server
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_aton(SERV_ADDR, &servaddr.sin_addr);
    // servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(SERV_PORT);

    //Step 3: Communicate with server
    for (;;) {
        printf("Send to server: ");
        fgets(buff, sizeof(buff), stdin);
        char *offset = strstr(buff, "\n");
        if (offset != NULL) *offset = '\0';
        if (is_empty(buff) == 1) break;
        len = sizeof(servaddr);
        sendBytes = sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *) &servaddr, len);
        if(sendBytes < 0) {
            perror("Error: ");
            return 0;
        }
    }
    close(sockfd);
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