#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdbool.h>

typedef struct node
{
    char username[20];
    char password[20];
    int status;
    char homepage[25];
    struct node *next;
} node;

node *head = NULL;

void insertAccount(char username[20], char password[20], int status, char homepage[25])
{
    node *temp;
    temp = (node *)malloc(sizeof(node));
    strcpy(temp->username, username);
    strcpy(temp->password, password);
    temp->status = status;
    strcpy(temp->homepage, homepage);
    temp->next = head;
    head = temp;
}
node *findAccount(char name[20])
{
    node *current = head;
    if (head == NULL)
    {
        return NULL;
    }
    while (strcmp(current->username, name) != 0)
    {
        if (current->next == NULL)
        {
            return NULL;
        }
        else
        {
            current = current->next;
        }
    }
    return current;
}

void printlistAccount()
{
    node *temp;
    temp = head;
    char notification[8];

    printf("----------------->LIST ACCOUNTS<------------------\n");
    while (temp)
    {
        if (temp->status == 0)
        {
            strcpy(notification, "blocked");
        }
        else if (temp->status == 1)
        {
            strcpy(notification, "active");
        }
        else if (temp->status == 2)
        {
            strcpy(notification, "idle");
        }
        printf("Username: %s\n", temp->username);
        printf("Password: %s\n", temp->password);
        printf("Status : (%d) - %s\n", temp->status, notification);
        printf("---------------------\n");
        temp = temp->next;
    }
    printf("\n");
}

void openFile()
{
    node *account;
    char *username;
    char *password;
    char *homepage;
    int status;
    char c;
    int u = 0, p = 0, blank = 0;
    username = (char *)malloc(20);
    password = (char *)malloc(20);
    homepage = (char *)malloc(20);
    FILE *fptr;
    if ((fptr = fopen("nguoidung.txt", "r+")) == NULL)
    {
        printf("File does not exist !!\n");
        return;
    }
    while (1)
    {
        fscanf(fptr, "%s", username);
        fscanf(fptr, "%s", password);
        fscanf(fptr, "%d", &status);
        fscanf(fptr, "%s", homepage);
        if (feof(fptr))
            break;
        insertAccount(username, password, status, homepage);
    }
    free(username);
    free(password);
    free(homepage);
    fclose(fptr);
}

void writeFile()
{
    FILE *fptr;
    node *temp;
    temp = head;
    fptr = fopen("nguoidung.txt", "w+");
    while (temp)
    {
        fprintf(fptr, "%s %s %d", temp->username, temp->password, temp->status);
        fprintf(fptr, "\n");
        temp = temp->next;
    }
    fclose(fptr);
}

//Option 1. Register Account
void registerAccount()
{
    char username[20];
    char password[20];
    char notification[8];
    char homepage[25];
    printf("Username: ");
    scanf("%s", username);
    node *account = findAccount(username);
    if (account != NULL)
    {
        printf("Account existed !!\n");
    }
    else
    {
        printf("Password: ");
        scanf("%s", password);
        insertAccount(username, password, 2, homepage);
        writeFile();
        printf("--------------------->OUTPUT<---------------------\n");
        printf("Successful registration. Activation required.\n\n");
        printlistAccount();
    }
    printf("\n");
}

//Option 2. Activate Account
node *activateAccount()
{
    char username[20];
    char password[20];
    char notification[8];
    char activationCode[20];
    int counter2 = 0;
    int counter1 = 0;
    char systemCode[20] = "LTM121216";
    printf("Username: ");
    scanf("%s", username);
    node *account = findAccount(username);
    if (account != NULL)
    {
        do
        {
            printf("Password: ");
            scanf("%s", password);
            if (strcmp(account->password, password) == 0)
            {
                do
                {
                    printf("Code: ");
                    scanf("%s", activationCode);
                    if (strcmp(activationCode, systemCode) == 0)
                    {
                        printf("Account is activated\n");
                        account->status = 1;
                        // ----------------> Print account
                        printf("----------------->NEW ACTIVATED ACCOUNTS<------------------\n");
                        if (account->status == 0)
                        {
                            strcpy(notification, "blocked");
                        }
                        else if (account->status == 1)
                        {
                            strcpy(notification, "active");
                        }
                        else if (account->status == 2)
                        {
                            strcpy(notification, "idle");
                        }
                        printf("Username: %s\n", account->username);
                        printf("Password: %s\n", account->password);
                        printf("Status : (%d) - %s\n", account->status, notification);
                        // ----------------> Print account
                        return account;
                    }
                    else
                    {
                        printf("Account is not activated\n");
                        counter2++;
                    }
                } while (counter2 < 5);
                if (counter2 == 5)
                {
                    printf("\nActivation code is incorrect. Account is blocked\n");
                    account->status = 0;
                    writeFile();
                    return NULL;
                }
            }
            else
            {
                printf("Password is incorrect !!.\n");
                counter1++;
            }
        } while (counter1 < 3);
    }
    else
    {
        printf("Cannot find account !!\n");
        return NULL;
    }
}

//Option3. Sign in Account
node *signinAccount()
{
    char username[20];
    char password[20];
    char notification[8];
    int counter = 0;
    printf("Username: ");
    scanf("%s", username);
    node *account = findAccount(username);
    if (account != NULL)
    {
        if (account->status == 0)
        {
            printf("Account is blocked !!\n");
        }
        else
        {
            do
            {
                printf("Password: ");
                scanf("%s", password);
                if (strcmp(account->password, password) == 0)
                {
                    printf("Hello %s !!\n", account->username);
                    return account;
                }
                else
                {
                    printf("Password is incorrect !!.\n");
                    counter++;
                }
            } while (counter < 3);
        }

        if (counter == 3)
        {
            printf("Account is blocked !!\n");
            account->status = 0;
            writeFile();
            // ----------------> Print account
            printf("----------------->NEW BLOCKED ACCOUNTS<------------------\n");
            if (account->status == 0)
            {
                strcpy(notification, "blocked");
            }
            else if (account->status == 1)
            {
                strcpy(notification, "active");
            }
            else if (account->status == 2)
            {
                strcpy(notification, "idle");
            }
            printf("Username: %s\n", account->username);
            printf("Status : (%d) - %s\n", account->status, notification);
            // ----------------> Print account
        }
    }
    else
    {
        printf("Cannot find account !!\n");
        return NULL;
    }
}

//Option4. Search account
void searchAccount()
{
    char username[20];
    char notification[8];
    printf("Username: ");
    scanf("%s", username);
    printf("\n");
    node *account = findAccount(username);
    if (account != NULL)
    {
        // ----------------> Print account
        if (account->status == 0)
        {
            strcpy(notification, "blocked");
        }
        else if (account->status == 1)
        {
            strcpy(notification, "active");
        }
        else if (account->status == 2)
        {
            strcpy(notification, "idle");
        }
        printf("-------------->INFORMATION ACCOUNTS<---------------\n");
        printf("Username: %s\n", account->username);
        printf("Status : (%d) - %s\n", account->status, notification);
        // ----------------> Print account
    }
    else
    {
        printf("Cannot find account !!\n");
    }
}
//Option5. Change password account
node *changePassword()
{
    char username[20];
    char password[20];
    char newPassword[20];
    char notification[8];
    int counter = 0;
    printf("Username: ");
    scanf("%s", username);
    node *account = findAccount(username);
    if (account != NULL)
    {
        do
        {
            printf("Password: ");
            scanf("%s", password);
            if (strcmp(account->password, password) == 0)
            {
                // If password is correct -> change password
                printf("New Password: ");
                scanf("%s", newPassword);
                strcpy(account->password, newPassword);
                printf("Password is changed !!\n");
                // ----------------> Print account
                printf("----------------->UPDATED ACCOUNTS<------------------\n");
                if (account->status == 0)
                {
                    strcpy(notification, "blocked");
                }
                else if (account->status == 1)
                {
                    strcpy(notification, "active");
                }
                else if (account->status == 2)
                {
                    strcpy(notification, "idle");
                }
                printf("Username: %s\n", account->username);
                printf("Password (upadted): %s\n", account->password);
                printf("Status : (%d) - %s\n", account->status, notification);
                // ----------------> Print account
                return account;
            }
            else
            {
                printf("Current password is incorrect. Please try again !!.\n");
                counter++;
            }
        } while (counter < 3);
    }
    else
    {
        printf("Cannot find account\n");
        return NULL;
    }
}
//Option6. Sign out account
node *signoutAccount()
{
    char username[20];
    printf("Username: ");
    scanf("%s", username);
    node *account = findAccount(username);
    if (account != NULL)
    {
        return account;
    }
    else
    {
        printf("Cannot find account !!\n");
        return NULL;
    }
}

//Option 7. From input ip get hostname
void getHostName(char *input_ip)
{
    struct hostent *hp;
    struct in_addr ip_addr;
    int i;

    if (!inet_aton(input_ip, &ip_addr))
    {
        printf("Not found information\n");
        exit(1);
    }
    hp = gethostbyaddr(&ip_addr, sizeof(ip_addr), AF_INET);
    if (!hp)
    {
        printf("Not found information\n");
        exit(1);
    }
    printf("Host name: %s\n", hp->h_name);
    printf("Alias hostname: ");
    printf("\n");
    if (hp->h_aliases[0] != NULL)
    {
        for (i = 0; hp->h_aliases[i] != NULL; i++)
        {
            printf("%s\n", hp->h_aliases[i]);
        }
    }
    else
    {
        printf("Sorry, Alias hostname is NULL !!\n");
        exit(1);
    }
    exit(0);
}
//Option 8. From input hostname get ip
void getIpAdress(char *input_hostname)
{
    struct hostent *hp;
    struct in_addr **ip_list;
    struct in_addr ip_addr;
    int i;
    hp = gethostbyname(input_hostname);
    if (!hp)
    {
        printf("Not found information\n");
        exit(1);
    }
    ip_addr = *(struct in_addr *)(hp->h_addr);
    printf("Official IP: %s\n", inet_ntoa(ip_addr));
    printf("Alias IP:\n");
    ip_list = (struct in_addr **)(hp->h_addr_list);
    if (ip_list == NULL)
    {
        printf("\nSorry, Alias IP address is NULL !!\n");
        exit(1);
    }
    for (i = 1; ip_list[i] != NULL; i++)
    {
        printf("%s\n", inet_ntoa(*ip_list[i]));
    }
    exit(0);
}
int validate_number(char *str)
{
    int num = atoi(str);
    if (num < 0 || num > 256)
        return 0;
    return 1;
}
int isNumber(char *str)
{
    int i, num = 0, length = strlen(str);
    for (i = 0; i < strlen(str); i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
            num++;
    }
    if (num != length)
        return 0;
    else
        return 1;
    // printf("%d\n",length);
    // printf("%d\n",num);
}
int validate_ip(char *ip_str)
{
    //check whether the IP is valid or not
    int word = 0;
    char sub_str[40];
    strcpy(sub_str, ip_str);
    char *ptr = strtok(sub_str, ".");
    while (ptr != NULL)
    {
        if (!validate_number(ptr))
            return 0;
        ptr = strtok(NULL, ".");
        word++;
    }
    if (word != 4)
        return 0;
    return 1;
}
// dấu chấm > 1, số từ  > 2 .
// từ = số dấu chấm + 1, không được bắt đầu bằng dấu chấm
int validate_hostname(char *host_str)
{
    int word = 0, dots = 0;
    char sub_str[40];
    printf("...\n");
    strcpy(sub_str, host_str);
    if (sub_str[0] == '.')
    {
        // printf("a");
        return 0;
    }
    char *ptr = strtok(sub_str, ".");
    while (ptr != NULL)
    {
        // if (isNumber(ptr))
        // {
        //     // printf("b");
        //     return 0;
        // }
        ptr = strtok(NULL, ".");
        dots++;
        word++;
    }
    dots -= 1;
    printf("%d", dots);
    printf("%d", word);
    if ((word == 4) || (word != (dots + 1)) || (word == 3))
    {
        return 0;
    }
    return 1;
}

void printHostName()
{
    node *temp = head;
    while (temp != NULL)
        temp = temp->next;
    if ((validate_hostname(temp->homepage)) == 1)
    {
        printf("Wrong parameter\n");
        exit(1);
    }
    else if ((validate_ip(temp->homepage)) == 1)
    {
        getHostName(temp->homepage);
    }
    else
    {
        printf("Not found information\n");
        exit(1);
    }
}
void printIP()
{
    node *temp = head;
    while (temp != NULL)
        temp = temp->next;
    if (validate_ip(temp->homepage))
    {
        printf("Wrong parameter\n");
        exit(1);
    }
    else if (validate_hostname(temp->homepage))
    {
        getIpAdress(temp->homepage);
    }
    else
    {
        printf("Not found information\n");
        exit(1);
    }
}
int menu()
{
    int choice;
    printf("\n-----------------------------------\n");
    printf("USER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
    printf("1. Register\n");
    printf("2. Activate\n");
    printf("3. Sign in\n");
    printf("4. Search\n");
    printf("5. Change password\n");
    printf("6. Sign out\n");
    printf("7. Homepage with domain name\n");
    printf("8. Homepage with IP address\n");
    printf("Your choice (1-8, other to quit):");
    scanf("%d", &choice);
    printf("\n");
    return choice;
}

int main()
{
    int z;
    openFile();
    printlistAccount();
    node *loginAccount = NULL;
    node *signoutAccount1 = NULL;
    do
    {
        z = menu();

        switch (z)
        {
        case 1:
        {
            {
                printf("--------> Option 1: Register !!\n");
                registerAccount();
            }
            break;
        }
        case 2:
        {
            {
                printf("--------> Option 2: Activate !!\n");
                activateAccount();
            }
            break;
        }
        case 3:
        {
            {
                printf("--------> Option 3: Sign in !!\n");
                loginAccount = signinAccount();
            }

            break;
        }
        case 4:
        {
            {
                printf("--------> Option 4: Search !!\n");
                if (loginAccount != NULL)
                {
                    searchAccount();
                }
                else
                {
                    printf("Account is not sign in\n");
                }
            }

            break;
        }
        case 5:
        {
            printf("--------> Option 5: Change passowrd !!\n");
            if (loginAccount != NULL)
            {
                changePassword();
            }
            else
            {
                printf("Account is not sign in !!\n");
            }
            break;
        }
        case 6:
        {
            {
                printf("--------> Option 6: Sign out !!\n");
                if (loginAccount != NULL)
                {
                    signoutAccount1 = signoutAccount();
                    if (signoutAccount1)
                    {
                        if (strcmp(signoutAccount1->username, loginAccount->username) == 0)
                        {
                            printf("Goodbye %s !!\n", signoutAccount1->username);
                        }
                        else
                        {
                            printf("Account is not sign in !!\n");
                        }
                    }
                }
                else
                {
                    printf("Account is not sign in !!\n");
                }
            }

            break;
        }
        case 7:
        {
            printf("--------> Option 7. Homepage with domain name !!\n");
            if (loginAccount != NULL)
            {
                printHostName();
            }   
            else
            {
                printf("Account is not sign in !!\n");
            }
            break;
        }
        case 8:
        {
            printf("--------> Option 8. Homepage with IP address !!\n");
            if (loginAccount != NULL)
            {
                printIP();
            }
            else
            {
                printf("Account is not sign in !!\n");
            }
            break;
        }
        default:
        {
            break;
        }
        }
    } while (z >= 1 && z <= 8);
    return 0;
}