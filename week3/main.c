#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h> // hostent, gethostbyname(),...
#include <arpa/inet.h> // inet_ntoa(), inet_aton()

#define true 1
#define false 0
#define MAX_ONLINE_NUMBER 10
#define MAX_LENGTH 30

struct user {
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    int status; // 0 (blocked), 1 (active), 2 (idle)
    char homepage[MAX_LENGTH];
    struct user *next;
};

struct user *head = NULL;

char online_user[MAX_LENGTH]; // Who are online?

/* ---------------- Function Declaration ---------------- */

struct user *newAccount();
void init();
void printOnlineUser();
void printMenu();
void addAccount(char username[MAX_LENGTH], char password[MAX_LENGTH], int status, char homepage[MAX_LENGTH]);
void readFile();
void printFile();
void addToFile(char username[MAX_LENGTH], char password[MAX_LENGTH], char homepage[MAX_LENGTH]);
int checkUsername(char name[MAX_LENGTH]);
int checkSignIn(char name[MAX_LENGTH], char pass[MAX_LENGTH]);
void rewriteFile(char name[MAX_LENGTH], char new_pass[MAX_LENGTH], int new_status);
int check_ip(char *c);

void signUp();
void activate();
void signIn();
void search();
void changePass();
void signOut();
void printName();
void printIP();

/* -------------------------- Main Function -------------------------- */

int main() {
    char *option;
    // init();
    readFile();
    do {
        printFile();
        printOnlineUser();
        printMenu();
        scanf(" %[^\n]", option);
        if (strlen(option) != 1) break;
        if (!isdigit(option[0])) break;
        switch (atoi(&option[0])) {
            case 1: signUp(); 
                break;
            case 2: activate();
                break;
            case 3: signIn(); 
                break;
            case 4: search(); 
                break;
            case 5: changePass(); 
                break;
            case 6: signOut(); 
                break;
            case 7: printName();
                break;
            case 8: printIP();
                break;
            default: printf("Exit\n"); break;
        }

    } while(atoi(&option[0]) > 0 && atoi(&option[0]) < 9);

    // Done. Free linked-list
    while (head) {  
        struct user *tmp = head;
        head = head->next;
        free(tmp);
    }
}

/* ------------------ Malloc Encapsulation ------------------ */

struct user *newAccount() {
    // Try to allocate user structure.
    struct user *retVal = malloc (sizeof (struct user));
    retVal->next = NULL;
    return retVal;
}

/* -------------------------- Utilities --------------------------- */

// Create account's text file
void init() {
    FILE *f = fopen("nguoidung.txt", "w");
    fprintf(f, "hedspi1 hedpsi2016 0 google.com\n");
    fprintf(f, "hust hust123 1 123.456\n");
    fprintf(f, "soict soictfit 1 youtube.com\n");
    fclose(f);
}

// Show online users and number of online users
void printOnlineUser() {
    printf("Online User: ");
    if (strcmp(online_user, "") != 0)
        printf("%s\n", online_user);
}

// Print main menu option
void printMenu() {
    printf("\n\nUSER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
    printf("1. Register\n");
    printf("2. Activate\n");
    printf("3. Sign in\n");
    printf("4. Search\n");
    printf("5. Change passowrd\n");
    printf("6. Sign out\n");
    printf("7. Homepage with domain name\n");
    printf("8. Homepage with IP address\n");
    printf("Your choice (1-8, other to quit): ");
}

// Add account to linked list
void addAccount(char username[MAX_LENGTH], char password[MAX_LENGTH], int status, char homepage[MAX_LENGTH]) {
    struct user *new_user = newAccount();
    strcpy(new_user->username, username);
    strcpy(new_user->password, password);
    new_user->status = status;
    strcpy(new_user->homepage, homepage);
    if (head == NULL) head = new_user; // if linked list is empty
    else {
        struct user *tmp = head; // assign head to p 
        while (tmp->next != NULL) tmp = tmp->next; // traverse the list until the last node
        tmp->next = new_user; //Point the previous last node to the new node created.
    }
}

// Print account info from linked list to screen
void printFile() {
    struct user *tmp = head;
    printf("\n**** Account Information List ****\n");
    printf("-----------------------------------\n");
    while (tmp != NULL) {
        printf("%s %s %d %s\n", tmp->username, tmp->password, tmp->status, tmp->homepage);
        if (tmp->next != NULL) tmp = tmp->next;
        else break;
    }
    printf("-----------------------------------\n");
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
        char *homepage = strtok(NULL, " \n\t");
        addAccount(name, pass, status, homepage);
    }
    fclose(f);
}

// Add new account to linked list and save to file
void addToFile(char username[MAX_LENGTH], char password[MAX_LENGTH], char homepage[MAX_LENGTH]) {
    addAccount(username, password, 2, homepage); // add to linked-list
    // write to file
    FILE *f = fopen("nguoidung.txt", "a");
    if (f == NULL) {
        printf("Error opening file!\n");   
        exit(1);             
    }
    fprintf(f, "%s %s 2 %s\n", username, password, homepage); // 2 : idle
    fclose(f);
}

// Check username existance?
int checkUsername(char name[MAX_LENGTH]) {
    // NoneExist: 0
    // Exist: 1
    // Account is blocked: 2
    // User is online: 3
    // Account is not activated: 4
    if (strcmp(online_user, name) == 0) return 3;

    struct user *tmp = head;
    while (tmp != NULL) {  
        if (strcmp(tmp->username, name) == 0) {
            if (tmp->status == 0) return 2;
            else if (tmp->status == 2) return 4;
            else return 1;
        }
        tmp = tmp->next;  
    }
    return 0;
}

// Check user's input when logging in
int checkSignIn(char name[MAX_LENGTH], char pass[MAX_LENGTH]) {
    // Cannot find account: 0
    // Login successful: 1
    // Password is incorrect: 2
    struct user *tmp = head;
    while (tmp != NULL) {  
        if (strcmp(tmp->username, name) == 0) {
            if (strcmp(tmp->password, pass) == 0) return 1;
            else return 2;
        } else tmp = tmp->next;
    }
    return 0;
}

// Change password or change status and save to file
void rewriteFile(char name[MAX_LENGTH], char new_pass[MAX_LENGTH], int new_status) {
    FILE *f = fopen("nguoidung.txt", "w");
    if (f == NULL) {
        printf("Error opening file!\n");   
        exit(1);             
    }
    struct user *tmp = head;
    while (tmp != NULL) {  
        if (strcmp(tmp->username, name) == 0) {
            strcpy(tmp->password, new_pass);
            if (new_status != 3) tmp->status = new_status;
        }
        fprintf(f, "%s %s %d %s\n", tmp->username, tmp->password, tmp->status, tmp->homepage);
        tmp = tmp->next;
    }
    fclose(f);
}

// check if a string is ip address or not
int check_ip(char *c)
{
    while(*c != '\0')
    {
        if(isxdigit(*c)==0) if(ispunct(*c)==0) return 0; // is hostname
        c++;
    }
    return 1; // is ip_address
}

//check if string contains white spaces or not
int check_space(char *c)
{
    while(*c != '\0')
    {
        if(isspace(*c)!=0) return 1; // string contains white-space
        c++;
    }
    return 0; // string not contains white-space
}

/* -------------------------- Core Functions --------------------------- */

// 1. Register
void signUp() {
    char name[MAX_LENGTH];
    char pass[MAX_LENGTH];
    char homepage[MAX_LENGTH];
    int exist=0;
    printf("Enter username: ");
    scanf(" %[^\n]", name);
    if (check_space(name) == 1) {
        printf("White spaces are not allowed in username\n");
        return;
    }
    if (checkUsername(name) == 1 || checkUsername(name) == 2 || checkUsername(name) == 4) {
        printf("Account existed\n");
        return;
    }
    printf("Enter password: ");
    scanf(" %[^\n]", pass);
    if (check_space(pass) == 1) {
        printf("White spaces are not allowed in input\n");
        return;
    }
    printf("Enter homepage (Domain name / IP address): ");
    scanf(" %[^\n]", homepage);
    if (check_space(homepage) == 1) {
        printf("White spaces are not allowed in input\n");
        return;
    }
    addToFile(name, pass, homepage);
    printf("Successful registration. Activation required.\n");
}

// 2. Activate
void activate() {
    char name[MAX_LENGTH];
    char pass[MAX_LENGTH];
    char code[MAX_LENGTH];
    int count = 0;
    printf("Enter username: ");
    scanf(" %[^\n]", name);
    int checkName = checkUsername(name);
    if (checkName == 0) {
        printf("Cannot find account\n");
        return;
    }
    else if (checkName == 2) {
        printf("Account is blocked\n");
        return;
    }
    printf("Enter password: ");
    scanf(" %[^\n]", pass);
    if (checkSignIn(name, pass) == 2) {
        printf("Password is incorrect\n");
        return;
    }
    do {
        printf("Enter activation code: ");
        scanf(" %[^\n]", code);
        if (strcmp(code, "LTM121216") == 0) {
            rewriteFile(name, pass, 1); // 1 : active
            printf("Account is activated\n");
        }
        else {
            printf("Activation code is incorrect.\n");
            count++;
        }
        if (count == 6) { // Nhập sai quá 5 lần
            rewriteFile(name, pass, 0); // 0 : blocked
            printf("Account is blocked\n");
        }
    } while (strcmp(code, "LTM121216") != 0 && count <= 5);
}

// 3. Sign in
void signIn() {
    char name[MAX_LENGTH];
    char pass[MAX_LENGTH];
    int count = 0;

    printf("Enter username: ");
    scanf(" %[^\n]", name);
    int checkName = checkUsername(name);
    if (checkName == 0) {
        printf("Cannot find account\n");
        return;
    }
    else if (checkName == 2) {
        printf("Account is blocked\n");
        return;
    }
    else if (checkName == 3) {
        printf("User is online. No sign in needed.\n");
        return;
    }
    else if (checkName == 4) {
        printf("Account is not activated. Cannot sign in.\n");
        return;
    }

    do {
        printf("Enter password: ");
        scanf(" %[^\n]", pass);
        int checkPass = checkSignIn(name, pass);
        if (checkPass == 1) {
            printf("Hello %s\n", name);
            strcpy(online_user, name);
            return;
        }
        else if (checkPass == 2) {
            printf("Password is incorrect\n");
            count++;
        }
    } while (count <= 3);
    if (count == 4) { // Nhập sai quá 3 lần
        printf("Account is blocked\n"); // Account is now blocked
        rewriteFile(name, pass, 0); // 0 : blocked
    }
}

// 4. Search
void search() {
    char name[MAX_LENGTH];
    struct user *tmp = head;
    printf("Enter username: ");
    scanf(" %[^\n]", name);
    while (tmp != NULL) {  
        if (strcmp(tmp->username, name) == 0) {
            if (tmp->status == 1) printf("Account is active\n");
            else if (tmp->status == 0) printf("Account is blocked\n");
            return;
        }
        else tmp = tmp->next;  
    }
    printf("Cannot find account\n");
}

// 5. Change password
void changePass() {
    char name[MAX_LENGTH];
    char pass[MAX_LENGTH];
    char new_pass[MAX_LENGTH];
    int i;

    printf("Enter username: ");
    scanf(" %[^\n]", name);
    if (checkUsername(name) == 0) {
        printf("Cannot find account\n");
        return;
    }

    if (strcmp(name, online_user) != 0) {
        printf("Account is not sign in\n");
        return;
    }

    printf("Enter current password: ");
    scanf(" %[^\n]", pass);
    if (checkSignIn(name, pass) == 2) {
        printf("Current password is incorrect. Please try again\n");
        return;
    }

    printf("Enter new password: ");
    scanf(" %[^\n]", new_pass);
    if (check_space(new_pass) == 1) {
        printf("White spaces are not allowed in input\n");
        return;
    }
    rewriteFile(name, new_pass, 3); // 3 ~ keep the current status 
}

// 6. Sign out
void signOut() {
    char name[MAX_LENGTH];
    printf("Enter username: ");
    scanf(" %[^\n]", name);
    if (checkUsername(name) == 0) {
        printf("Cannot find account\n");
        return;
    }
    if (strcmp(name, online_user) == 0) {
        printf("Goodbye %s\n", online_user);
        strcpy(online_user, "");
        return;
    }
    printf("Account is not sign in\n");
}

// 7. Homepage with domain name
void printName() {
    if (strcmp(online_user, "") == 0) {
        printf("Account is not sign in\n");
        return;
    }
    struct user *tmp = head;
    while (tmp != NULL && strcmp(tmp->username, online_user) != 0) tmp = tmp->next;
    // if homepage is a domain name
    if (check_ip(tmp->homepage) == 0) printf("Domain Name: %s\n", tmp->homepage);
    // else if homepage is an ip address
    else {
        struct in_addr ipv4;
        int valid = inet_pton(AF_INET, tmp->homepage, &ipv4);
        if (valid <= 0) { // If not ipv4
            if (valid == 0) {
                struct in6_addr ipv6;
                valid = inet_pton(AF_INET6, tmp->homepage, &ipv6);
                if (valid <= 0) { // If not ipv6
                    if (valid == 0) printf("Domain Name: %s\n", tmp->homepage);
                    else perror("error inet_pton");
                    return;
                }
                else { // is ipv6
                    struct hostent *host = gethostbyaddr(&ipv6, sizeof(ipv6), AF_INET6);
                    if (host == NULL) printf("Not found information\n");
                    else {
                        printf("Official name: %s\n", host->h_name);
                        printf("Alias name:\n");
                        for(int i = 0; host->h_aliases[i] != NULL; i++) {
                            printf("%s\n", host->h_aliases[i]);
                        }
                    }
                }
            }
        }
        else { // ip ipv4
            struct hostent *host = gethostbyaddr(&ipv4, sizeof(ipv4), AF_INET);
            if (host == NULL) printf("Not found information\n");
            else {
                printf("Official name: %s\n", host->h_name);
                printf("Alias name:\n");
                for(int i = 0; host->h_aliases[i] != NULL; i++) {
                    printf("%s\n", host->h_aliases[i]);
                }
            }
        }
    }
}


// 8. Homepage with IP address
void printIP() {
    if (strcmp(online_user, "") == 0) {
        printf("Account is not sign in\n");
        return;
    }
    struct user *tmp = head;
    while (tmp != NULL && strcmp(tmp->username, online_user) != 0) tmp = tmp->next;
    // if homepage is ip address
    if (check_ip(tmp->homepage) == 1) {
        struct in_addr ipv4;
        int valid = inet_pton(AF_INET, tmp->homepage, &ipv4);
        if (valid > 0) {
            printf("IP Address: %s\n", tmp->homepage);
            return;
        }
        else { // If not ipv4
            if (valid == 0) {
                struct in6_addr ipv6;
                valid = inet_pton(AF_INET6, tmp->homepage, &ipv6);
                if (valid > 0) {
                    printf("IP Address: %s\n", tmp->homepage); 
                    return;
                }
            }
        }
    }
    // else if homepage is not ip address
    struct hostent *host = gethostbyname(tmp->homepage);
    if (host == NULL) printf("Not found information\n");
    else {
        printf("Official IP: %s\n", inet_ntoa(*(struct in_addr*)host->h_addr));
        printf("Alias IP:\n");
        unsigned int i=1;
        while (host->h_addr_list[i] != NULL) {
            printf("%s\n", inet_ntoa(*(struct in_addr*)(host->h_addr_list[i])));
            i++;
        }
    }

}