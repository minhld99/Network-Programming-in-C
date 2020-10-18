#include <stdio.h>
#include <stdlib.h> // atoi()
#include <ctype.h> // isxdigit(), ispunct()
#include <netdb.h> // hostent, gethostbyname(),...
#include <arpa/inet.h> // inet_ntoa(), inet_aton()

/* ---------------- Function Declaration ---------------- */

int check_ip(char *c);

/*----------------------------- Main Function -----------------------------*/

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("error: too few arguments, expected 3, have %d\n", argc);
        printf("Usage:\n\t%s 1 hostname\n\t%s 2 ip_address\n", argv[0], argv[0]);
        exit(-1);
    }
    else if (argc > 3) {
        printf("error: too many arguments, expected 3, have %d\n", argc);
        printf("Usage:\n\t%s 1 hostname\n\t%s 2 ip_address\n", argv[0], argv[0]);
        exit(-1);
    }

    // Get ipv4 address from hostname
    if (atoi(argv[1]) == 2) {
        if (check_ip(argv[2]) == 1) {
            printf("Wrong parameter\n");
            exit(1);
        }
        struct hostent *hp = gethostbyname(argv[2]);
        if (hp == NULL) printf("Not found information\n");
        else {
            printf("Official IP: %s\n", inet_ntoa(*(struct in_addr*)hp->h_addr));
            printf("Alias IP:\n");
            unsigned int i=1;
            while (hp->h_addr_list[i] != NULL) {
                printf("%s\n", inet_ntoa(*(struct in_addr*)(hp->h_addr_list[i])));
                i++;
            }
        }
    }

    // Get hostname from ip address
    else if (atoi(argv[1]) == 1) {
        if (check_ip(argv[2]) == 0) {
            printf("Wrong parameter\n");
            exit(1);
        }
        struct in_addr ipv4;
        int valid = inet_pton(AF_INET, argv[2], &ipv4);
        if (valid <= 0) { // If not ipv4
            if (valid == 0) {
                struct in6_addr ipv6;
                valid = inet_pton(AF_INET6, argv[2], &ipv6);
                if (valid <= 0) { // If not ipv6
                    if (valid == 0) printf("Not a valid ip address\n");
                    else perror("error inet_pton");
                    exit(EXIT_FAILURE);
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

    // Neither option 1 or 2
    else {
        printf("error: invalid parameter %s\n", argv[1]);
        printf("Usage:\n\t%s 1 hostname\n\t%s 2 ip_address\n", argv[0], argv[0]);
        exit(1);
    }
}

/*----------------------------- Ultilty -----------------------------*/

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