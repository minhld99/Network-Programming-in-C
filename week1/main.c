#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// const int true = 1;
// const int false = 0;
// const int MAX_ONLINE_NUMBER = 10;
// const int MAX_LENGTH = 30;

#define true 1
#define false 0
#define MAX_ONLINE_NUMBER 10
#define MAX_LENGTH 30

struct user {
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    int status; // 0 (blocked), 1 (active), 2 (idle)
    struct user *next;
};

struct user *head = NULL;

int number = 0; // number of online users
char online_users[MAX_ONLINE_NUMBER][MAX_LENGTH]; // Who are online?

/* ---------------- Function Declaration ---------------- */

struct user *newAccount();
void init();
void printOnlineUsers();
void printMenu();
void addAccount(char username[MAX_LENGTH], char password[MAX_LENGTH], int status);
void readFile();
void printFile();
void addToFile();
int checkUsername(char name[MAX_LENGTH]);
int checkSignIn(char name[MAX_LENGTH], char pass[MAX_LENGTH]);
void rewriteFile(char name[MAX_LENGTH], char new_pass[MAX_LENGTH], int new_status);
void signUp();
void activate();
void signIn();
void search();
void changePass();
void signOut();

/* -------------------------- Main Function -------------------------- */

int main() {
    char *option;
    init();
    readFile();
    do {
        printFile();
        printOnlineUsers();
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
            default: printf("Exit\n"); break;
        }

    } while(atoi(&option[0]) > 0 && atoi(&option[0]) < 7);

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
    fprintf(f, "hedspi1 hedpsi2016 0\n");
    fprintf(f, "hust hust123 1\n");
    fprintf(f, "soict soictfit 1\n");
    fclose(f);
}

// Show online users and number of online users
void printOnlineUsers() {
    printf("Online User (%d): ", number);
    int tmp = 0;
    for (int i = 0; i < MAX_ONLINE_NUMBER; i++) {
        if (strcmp(online_users[i], "") != 0) {
            tmp++;
            if (tmp == number) printf("%s\n", online_users[i]);
            else printf("%s, ", online_users[i]);
        }
    }
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
    printf("Your choice (1-6, other to quit): ");
}

// Add account to linked list
void addAccount(char username[MAX_LENGTH], char password[MAX_LENGTH], int status) {
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

// Print account info from linked list to screen
void printFile() {
    struct user *tmp = head;
    printf("\n**** Account Information List ****\n");
    printf("-----------------------------------\n");
    while (tmp != NULL) {
        printf("%s %s %d\n", tmp->username, tmp->password, tmp->status);
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
        addAccount(name, pass, status);
    }
    fclose(f);
}

// Add new account to linked list and save to file
void addToFile(char username[MAX_LENGTH], char password[MAX_LENGTH]) {
    addAccount(username, password, 2); // add to linked-list
    // write to file
    FILE *f = fopen("nguoidung.txt", "a");
    if (f == NULL) {
        printf("Error opening file!\n");   
        exit(1);             
    }
    fprintf(f, "%s %s 2\n", username, password); // 2 : idle
    fclose(f);
}

// Check username existance?
int checkUsername(char name[MAX_LENGTH]) {
    // NoneExist: 0
    // Exist: 1
    // Account is blocked: 2
    // User is online: 3
    // Account is not activated: 4
    for (int i = 0; i < MAX_ONLINE_NUMBER; i++)
        if (strcmp(online_users[i], name) == 0) return 3;

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
        fprintf(f, "%s %s %d\n", tmp->username, tmp->password, tmp->status);
        tmp = tmp->next;
    }
    fclose(f);
}

/* -------------------------- Core Functions --------------------------- */

// 1. Register
void signUp() {
    char name[MAX_LENGTH];
    char pass[MAX_LENGTH];
    int exist=0;
    printf("Enter username: ");
    scanf("%s", name);
    if (checkUsername(name) == 1 || checkUsername(name) == 2 || checkUsername(name) == 4) {
        printf("Account existed\n");
        return;
    }
    printf("Enter password: ");
    scanf("%s", pass);
    addToFile(name, pass);
    printf("Successful registration. Activation required.\n");
}

// 2. Activate
void activate() {
    char name[MAX_LENGTH];
    char pass[MAX_LENGTH];
    char code[MAX_LENGTH];
    int count = 0;
    printf("Enter username: ");
    scanf("%s", name);
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
    scanf("%s", pass);
    if (checkSignIn(name, pass) == 2) {
        printf("Password is incorrect\n");
        return;
    }
    do {
        printf("Enter activation code: ");
        scanf("%s", code);
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
    scanf("%s", name);
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
        scanf("%s", pass);
        int checkPass = checkSignIn(name, pass);
        if (checkPass == 1) {
            printf("Hello %s\n", name);
            for (int i = 0; i < MAX_ONLINE_NUMBER; i++)
                if (strcmp(online_users[i], "") == 0) {
                    strcpy(online_users[i], name);
                    number++;
                    return;
                }
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
    scanf("%s", name);
    while (tmp != NULL) {  
        if (strcmp(tmp->username, name) == 0) {
            if (tmp->status == 1) printf("Account is active\n");
            else if (tmp->status == 0) printf("Account is blocked\n");
            return;
        }
        else  tmp = tmp->next;  
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
    scanf("%s", name);
    if (checkUsername(name) == 0) {
        printf("Cannot find account\n");
        return;
    }

    for (i = 0; i < MAX_ONLINE_NUMBER; i++)
        if (strcmp(name, online_users[i]) == 0) break;
    if (i == MAX_ONLINE_NUMBER) {
        printf("Account is not sign in\n");
        return;
    }

    printf("Enter current password: ");
    scanf("%s", pass);
    if (checkSignIn(name, pass) == 2) {
        printf("Current password is incorrect. Please try again\n");
        return;
    }

    printf("Enter new password: ");
    scanf("%s", new_pass);
    rewriteFile(name, new_pass, 3); // 3 ~ keep the current status 
}

// 6. Sign out
void signOut() {
    char name[MAX_LENGTH];
    printf("Enter username: ");
    scanf("%s", name);
    if (checkUsername(name) == 0) {
        printf("Cannot find account\n");
        return;
    }
    for (int i = 0; i < MAX_ONLINE_NUMBER; i++) {
        if (strcmp(name, online_users[i]) == 0) {
            printf("Goodbye %s\n", online_users[i]);
            strcpy(online_users[i], "");
            number--;
            return;
        }
    }
    printf("Account is not sign in\n");
}