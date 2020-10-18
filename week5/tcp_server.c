#include <stdio.h>
#include <stdlib.h>
#include <string.h> // bzero()
#include <ctype.h> // isdigit(), isalpha()
#include <sys/socket.h> // listen(), accept()
#include <netinet/in.h>
#include <arpa/inet.h> // inet_ntoa(), inet_aton()
#include <unistd.h> // close(sockfd)

#define BUFF_SIZE 100

struct user {
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];
    int status; // 0 (blocked), 1 (active), 2 (idle)
    int count; // // number of times of wrong-password's typing
    struct user *next;
};
struct user *head = NULL;

char online_user[BUFF_SIZE]; // Who are online?

/* ---------------- Function Declaration ---------------- */

struct user *newAccount();
void init();
void readFile();
void addAccount(char username[BUFF_SIZE], char password[BUFF_SIZE], int status);
void rewriteFile(char name[BUFF_SIZE], char new_pass[BUFF_SIZE], int new_status);
int is_empty(const char *s);
int is_number(const char *s);

/* -------------------------- Main Function -------------------------- */
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("error: too few arguments, expected 2, have %d\n", argc);
        printf("Usage:\t%s PortNumber\n (For example: %s 5500)\n", argv[0], argv[0]);
        exit(-1);
    }
    else if (argc > 2) {
        printf("error: too many arguments, expected 2, have %d\n", argc);
        printf("Usage:\t%s PortNumber\n (For example: %s 5500)\n", argv[0], argv[0]);
        exit(-1);
    }

    if ((!is_number(argv[1])) || atoi(argv[1]) > 65535 || atoi(argv[1]) < 0) {
        printf("Invalid port number\n");
        exit(-1);
    }

    init();
    readFile();
    const unsigned short SERV_PORT = atoi(argv[1]);

    int listenfd, connfd, n;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 1); // number of pending connections in queue = 1
    printf("%s\n", "Server running...waiting for connections.");

    for(;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("%s\n", "Received request...");
        char buff[BUFF_SIZE] = {0};
        while((n = recv(connfd, buff, BUFF_SIZE, 0)) > 0) {
            printf("%s\n", "String received from and resent to the client: ");
            buff[n] = '\0';
            puts(buff);
            send(connfd, buff, n, 0);
        } 
        if (n < 0) {
            perror("Read error");
            exit(1);
        }
        close(connfd); // close the file descriptor.
    }
    close(listenfd); // close listening socket
    return 0; 
}

/* -------------------------- Utilities --------------------------- */

// Malloc Encapsulation
struct user *newAccount() {
    // Try to allocate user structure.
    struct user *retVal = malloc (sizeof (struct user));
    retVal->next = NULL;
    retVal->count = 1;
    return retVal;
}

// Create account's text file
void init() {
    FILE *f = fopen("nguoidung.txt", "w");
    fprintf(f, "hedspi1 hedpsi2016 0\n");
    fprintf(f, "hust hust123 1\n");
    fprintf(f, "soict soictfit 1\n");
    fclose(f);
}

// Read from file into linked list
void readFile() {
    char str[100];
    FILE *f = fopen("nguoidung.txt", "r");
    if (f == NULL) {
        printf("Error opening file!\n");   
        exit(1);             
    }
    while (fgets(str, sizeof(str), f) != NULL) {
        char *name = strtok(str, " ");
        char *pass = strtok(NULL, " ");
        int status = atoi(strtok(NULL, " "));
        addAccount(name, pass, status);
    }
    fclose(f);
}

// Add account to linked list
void addAccount(char username[BUFF_SIZE], char password[BUFF_SIZE], int status) {
    struct user *new_user = newAccount();
    strcpy(new_user->username, username);
    strcpy(new_user->password, password);
    new_user->status = status;
    if (head == NULL) head = new_user; // if linked list is empty
    else {
        struct user *tmp = head; // assign head to p 
        while (tmp->next != NULL) tmp = tmp->next; // traverse the list until the last node
        tmp->next = new_user; //Point the previous last node to the new node created.
    }
}

// Change password or change status and save to file
void rewriteFile(char name[BUFF_SIZE], char new_pass[BUFF_SIZE], int new_status) {
    FILE *f = fopen("nguoidung.txt", "w");
    if (f == NULL) {
        printf("Error opening file!\n");   
        exit(1);             
    }
    struct user *tmp = head;
    while (tmp != NULL) {  
        if (strcmp(tmp->username, name) == 0) {
            memcpy(tmp->password, new_pass, BUFF_SIZE);
            // strcpy(tmp->password, new_pass);
            if (new_status != 3) tmp->status = new_status;
        }
        fprintf(f, "%s %s %d\n", tmp->username, tmp->password, tmp->status);
        tmp = tmp->next;
    }
    fclose(f);
}

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