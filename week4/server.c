#include <stdio.h>
#include <stdlib.h>
#include <string.h> // bzero()
#include <ctype.h> // isdigit(), isalpha()
#include <sys/socket.h>
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

    int sockfd, rcvBytes, sendBytes; 
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;
    int login_status = 0; // [0: not login yet] & [1: logged in]
    // int count = 1; // number of wrong-password's typing

    init();
    readFile();
    const unsigned short SERV_PORT = atoi(argv[1]);
    //Step 1: Construct socket
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        perror("Error: ");
        return 0;
    }
    //Step 2: Bind address to socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))) {
        perror("Error: ");
        return 0;
    }
    printf("Server started.\n");

      //Step 3: Communicate with client
    for (;;) {
        char name[BUFF_SIZE] = {0};
        char pass[BUFF_SIZE] = {0};
        char mesg[BUFF_SIZE] = {0};
        len = sizeof(cliaddr);
        if (login_status == 0) {
            rcvBytes = recvfrom(sockfd, name, BUFF_SIZE, 0, (struct sockaddr *) &cliaddr, &len);
            if(rcvBytes < 0){
                perror("Error: ");
                return 0;
            }
            name[rcvBytes] = '\0';
            struct user *tmp = head;
            while (tmp != NULL) {
                if (strcmp(tmp->username, name) == 0) {
                    if (tmp->status == 0 || tmp->status == 2) {
                        strcpy(mesg, "account not ready");
                        printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), mesg);
                    }
                    else {
                        strcpy(mesg, "Insert password");
                        printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), mesg);
                        rcvBytes = recvfrom(sockfd, pass, BUFF_SIZE, 0, (struct sockaddr *) &cliaddr, &len);
                        if(rcvBytes < 0){
                            perror("Error: ");
                            return 0;
                        }
                        pass[rcvBytes] = '\0';
                        if (strcmp(tmp->password, pass) == 0) {
                            tmp->count = 1;
                            strcpy(mesg, "OK");
                            printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), mesg);
                            strcpy(online_user, name);
                            login_status = 1;
                        }
                        else {
                            if (tmp->count == 3) {
                                rewriteFile(name, tmp->password, 0); // 0 : blocked
                                strcpy(mesg, "Account is blocked");
                                printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), mesg);
                            }
                            else {
                                strcpy(mesg, "Not OK");
                                printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), mesg);
                                tmp->count++;
                            }
                        }
                    }
                    break;
                }
                tmp = tmp->next;  
            }
            if (tmp == NULL) {
                strcpy(mesg, "Cannot find account");
                printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), mesg);
                continue;
            }
        }
        if (login_status == 1) {
            // receive new password
            rcvBytes = recvfrom(sockfd, pass, BUFF_SIZE, 0, (struct sockaddr *) &cliaddr, &len);
            if(rcvBytes < 0){
                perror("Error: ");
                return 0;
            }
            pass[rcvBytes] = '\0';
            if (strcmp(pass, "bye") == 0) {
                strcpy(mesg, "Goodbye ");
                strcat(mesg, online_user);
                printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), mesg);
                login_status = 0;
                strcpy(online_user, "");
                continue;
            }
            int alphabet = 0, number = 0, i; 
            char alpha[BUFF_SIZE] = {0}, digit[BUFF_SIZE] = {0}; 
            for (i=0; pass[i]!= '\0'; i++) { 
                // check for alphabets 
                if (isalpha(pass[i]) != 0) {
                    alpha[alphabet] = pass[i];
                    alphabet++; 
                }
                // check for decimal digits 
                else if (isdigit(pass[i]) != 0) {
                    digit[number] = pass[i];
                    number++; 
                }
                else {
                    alphabet = 0; number = 0;
                    strcpy(mesg, "Error");
                    printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), mesg);
                    break;
                }
            }
            if (alphabet != 0 || number != 0) {
                strcpy(mesg, digit);
                strcat(mesg, alpha);
                printf("[%s:%d]: %s\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), mesg);
                rewriteFile(online_user, pass, 3); // 3 ~ keep the current status 
            }             
        }
    }
    close(sockfd);
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