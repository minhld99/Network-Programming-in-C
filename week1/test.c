#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define max 20

typedef struct users{
  char username[max];
  char password[max];
  int status;
  struct users *next;
}users;

users *head;
users *current;
char onlineUser[max];
int isOnline = 0;

users *CreateNode(){
    users *tmp = (users*)malloc(sizeof(users)); // Cấp phát vùng nhớ dùng malloc()
    tmp->next = NULL;// Cho next trỏ tới NULL
    return tmp;//Trả về node mới đã có giá trị
}

void AddNewUser(char un[max], char pw[max], int st){
    users *newuser = CreateNode(); // Khởi tạo node temp với data = value
    strcpy(newuser->username,un);
    strcpy(newuser->password,pw);
    newuser->status=st;

    if(head == NULL){
        head = newuser; //Nếu linked list đang trống thì Node temp là head luôn
    }else{
      users *tmp = head;
      while (tmp->next !=NULL) tmp = tmp->next;
      tmp->next = newuser; // Đổi head hiện tại = temp
    }
}

//==========================================
void readFile(){
    FILE *f;
    char un[max];
    char pw[max];
    int status1;
    int num;
    f = fopen("nguoidung.txt","r");
    if(f == NULL){
        printf("file not exit!");
        exit(1);
    }
    while(!feof(f)){
        fscanf(f, "%s",un);
        fscanf(f, "%s",pw);
        fscanf(f, "%d",&status1);
        AddNewUser(un,pw,status1);
    }
    fclose(f);
  }
//---------------------
void writeFile(char un[max], char pw[max]){
  FILE *f = fopen("nguoidung.txt", "r+");
  if(f == NULL){
        printf("file not exit!");
        exit(1);
  }
  fseek(f,0,SEEK_END);
  fprintf(f, "%s %s %d\n", un, pw, 2);
  fclose(f);
}
//-----------------
void rewriteFile(){
  users *p;
  FILE *f;
  f = fopen("nguoidung.txt","w");
  for(p = head; p->next != NULL; p = p->next){
      fprintf(f,"%s %s %d\n", p->username, p->password, p->status);
  }
  fclose(f);
}

//-----------------
void printFile(){
  users *p;
  printf("\n\n");
  for(p = head; p->next != NULL; p = p->next){
      printf("%s %s %d\n", p->username, p->password, p->status);
  }
    printf("----------------\n");
}

//==========================================
int CheckUserName(char un[max]){
    users *tmp;
    for(tmp = head; tmp->next != NULL; tmp = tmp->next){
        if(strcmp(tmp->username, un) == 0){
          return 1;
        }
    }
    return 0;
}
int CheckPassword(char un[max],char pw[max]){
    users *tmp;
    if(CheckUserName(un) == 1){
        for(tmp = head; tmp->next != NULL; tmp = tmp->next){
            if (strcmp(tmp->username, un) == 0) {
                if(strcmp(tmp->password, pw) == 0)
                    return 1;
                else return 0;
            }
    }
    }
    return -1;
}
void checkStatus(char un[max],char pw[max], int st){
  users *tmp;
  FILE *f;
  for(tmp = head; tmp->next != NULL; tmp =  tmp->next){
      if(strcmp(tmp->username,un) == 0){
        if(CheckPassword(un,pw) == 1){
            tmp->status = st;
        }
      }
    }
}
//==========================================
void SignUp(){
  char un[max];
  char pw[max];
  printf("Enter username: ");
  scanf("%s", un);
  if(CheckUserName(un) == 1){
    printf("Account existed\n");
    return;
  }
  printf("Enter password: ");
  scanf("%s",pw);
  writeFile(un,pw);
  printf("done!");
}
//==========================================
void Activate(){
    char un[max], pw[max], code[max];
    int check = 0;
    users *p;
    printf("Enter username: ");
    scanf("%s", un);
    if(CheckUserName(un) == 0){
        printf("Account not existed.\n");
        return;
    }
    printf("Enter password: ");
    scanf("%s", pw);
    if(CheckPassword(un,pw) == 0){
        printf("Password wrong\n");
        return;
    }
    while(check <= 3){
        printf("Enter activation code: ");
        scanf("%s",code);
        if(strcmp(code,"LTM121216") == 0){
            checkStatus(un,pw,1);
            rewriteFile();
            printf("Account is activated\n");
            return;
        }else{
            printf("Account is not activated\n");
            check++;
        }
    }
    checkStatus(un,pw,0);
    rewriteFile();
    printf("\n");
    printf("Account is blocked\n");
}

//==========================================
void SignIn( ){
  users *user;
  char un[max], pw[max];
  int check = 0;
  printf("Enter username: ");
  scanf("%s", un);
  if(CheckUserName(un) == 0){
    printf("Cannot file account\n");
    return;
  }
  for(user = head; user->next !=NULL; user = user->next){
    if(strcmp(user->username,un) == 0){
      if(user->status == 0) {
        printf("Account is blocked\n");
        return;
      }
    }
  }
  do{
    printf("Enter password: ");
    scanf("%s", pw);
    if(CheckPassword(un,pw) == 1){
        printf("Hello %s\n",un);
        strcpy(onlineUser,un);
        isOnline = 1;
        return;
      }else{
        printf("Password is incorrect\n");
        check++;
      }
  }while(check <= 3);
  
  rewriteFile();
  printf("Account is blocked\n");
}
//==========================================
void SearchInfo(){
  char un[max], pw[max];
  users *user;
  if(isOnline == 1){
    printf("Enter username: ");
    scanf("%s", un);
    if(CheckUserName(un) == 0){
        printf("Cannot find account\n");
        return;
    }
    for(user = head; user->next != NULL; user = user->next){
        if(strcmp(un, user->username)==0){
            if(user->status == 0) printf("Account is blocked\n");
            else if(user->status == 1) printf("Account is active\n");
            else printf("Account is not activated\n");
        }
    }
  }
}
//==========================================
void ChangePassword(){
  printf("Online user: %s\n", onlineUser);
  char pw[max];
  char new_pw[max];
  users* tmp;
  if(isOnline == 0){
    printf("Account is not sign in\n");
    return ;
  }
  printf("Enter old password: ");
  scanf("%s", pw);
  if(CheckPassword(onlineUser,pw) == 0){
    printf("Current password is incorrect. Please try again\n");
  }else {
    printf("Enter new password: ");
    scanf("%s", new_pw);
    for(tmp  = head; tmp->next != NULL; tmp = tmp->next){
      if(strcmp(tmp->password, pw) == 0){
        strcpy(tmp->password,new_pw);
      }
    }rewriteFile();
  }
}
//==========================================
void SignOut(){
  users *tmp;
    char un[max];
    if(isOnline == 0){
       printf("Account is not sign in\n");
       return;
    }
    printf("Enter Username:");
    scanf("%s",un);
    if(CheckUserName(un) == 0){
      printf("Cannot find account \n");
    }
    for(tmp  = head; tmp->next != NULL; tmp = tmp->next){
      if(strcmp(tmp->username, un) == 0){
        printf("Goodbye %s\n", tmp->username);
        isOnline = 0;
        return;
      }
    }
}
//==========================================


int main(){
  int ch;
  readFile();
  do{
    printf("\n=========Menu=========\n");
    printf("1.Register.\n");
    printf("2.Activate.\n");
    printf("3.Sign in.\n");
    printf("4.Search.\n");
    printf("5.Change password.\n");
    printf("6.Sign out.\n");

    printf("Choose, pls: ");
    scanf("%d",&ch);
    // printf("choose again: ");
    // scanf("%d",&ch);
    switch (ch){
    case 1:
      SignUp();
      break;
    case 2:
      Activate();
      break;
    case 3:
      SignIn();
      break;
    case 4:
      SearchInfo();
      break;
    case 5:
      ChangePassword();
      break;
    case 6:
      SignOut();
      break;
    default: printf("Exit]n");
    }
  }while(ch < 7 && ch > 0);
  return 0;
}
