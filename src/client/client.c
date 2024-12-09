#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char username[50], password[50], command[50];
    char buffer[1024] = {0};
    int choice, user_type;

   
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        return -1;
    }

   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

   
    if (inet_pton(AF_INET, "172.18.1.20", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        return -1;
    }

   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }
    printf("Connected to the server.\n");

   
    printf("1. User\n");
    printf("2. Admin\n");
    printf("Choose an option: ");
    scanf("%d", &user_type);

   
    while (1) {
        memset(buffer, 0, sizeof(buffer));

        if (user_type == 1) {  
            printf("\nUser Menu:\n");
            printf("1. User Login\n");
            printf("2. User Sign Up\n");
            printf("3. Exit\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            if (choice == 1) {
                
                printf("Enter username: ");
                scanf("%49s", username);
                printf("Enter password: ");
                scanf("%49s", password);
                snprintf(buffer, sizeof(buffer), "user_login:%s:%s", username, password);
                send(sock, buffer, strlen(buffer), 0);
                printf("User login request sent.\n");
            } else if (choice == 2) {
                
                printf("Enter username: ");
                scanf("%49s", username);
                printf("Enter password: ");
                scanf("%49s", password);
                snprintf(buffer, sizeof(buffer), "user_signup:%s:%s", username, password);
                send(sock, buffer, strlen(buffer), 0);
                printf("User sign-up request sent.\n");
            } else if (choice == 3) {
                printf("Exiting...\n");
                break;
            } else {
                printf("Invalid option. Please try again.\n");
                continue;
            }
        } else if (user_type == 2) {  // Admin Menu
            printf("\nAdmin Menu:\n");
            printf("1. Admin Login\n");
            printf("2. Exit\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            if (choice == 1) {

                printf("Enter admin username: ");
                scanf("%49s", username);
                printf("Enter admin password: ");
                scanf("%49s", password);
                snprintf(buffer, sizeof(buffer), "admin_login:%s:%s", username, password);
                send(sock, buffer, strlen(buffer), 0);
                printf("Admin login request sent.\n");
            } else if (choice == 2) {
                printf("Exiting...\n");
                break;
            } else {
                printf("Invalid option. Please try again.\n");
                continue;
            }
        } else {
            printf("Invalid selection. Please choose 1 for User or 2 for Admin.\n");
            break;
        }


        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, sizeof(buffer));
        if (valread > 0) {
            printf("Server response: %s\n", buffer);
        } else {
            printf("No response or connection error.\n");
        }
    }


    close(sock);
    printf("Connection closed.\n");

    return 0;
}
