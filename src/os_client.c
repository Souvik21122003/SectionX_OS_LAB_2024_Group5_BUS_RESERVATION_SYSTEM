#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 2048  
int main() {
    int sock,valread;
    struct sockaddr_in serv_addr;
    char username[50], password[50];
    char buffer[1024] = {0};
    int choice, user_type;

    // Socket creation
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        return -1;
    }

    // Setting up server details
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }
    printf("Connected to the server.\n");

    // User or admin selection
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
                // User login
                printf("Enter username: ");
                scanf("%49s", username);
                printf("Enter password: ");
                scanf("%49s", password);
                snprintf(buffer, sizeof(buffer), "user_login:%s:%s", username, password);
                send(sock, buffer, strlen(buffer), 0);
                printf("User login request sent.\n");

                // Read server response
                memset(buffer, 0, sizeof(buffer));
                int valread = read(sock, buffer, sizeof(buffer));
                if (valread > 0) {
                    printf("Server response: %s\n", buffer);
                    if (strstr(buffer, "Login successful")) {
                        // Present post-login options
                        int loggedIn = 1;
                        while (loggedIn) {
                            printf("\n1. Search Bus\n");
                            printf("2. Log-out\n");
                            printf("Choose an option: ");
                            scanf("%d", &choice);
                            
                            if (choice == 1) {
                                // Send search bus request
                                char source[50], destination[50];
                                printf("Enter source: ");
                                scanf("%49s", source);
                                printf("Enter destination: ");
                                scanf("%49s", destination);
                                snprintf(buffer, sizeof(buffer), "search_bus:%s:%s", source, destination);
                                send(sock, buffer, strlen(buffer), 0);
                                  printf("data sent\n");
                                // Receive and print bus details
                                memset(buffer, 0, sizeof(buffer));
                                valread = read(sock, buffer, sizeof(buffer));
                                if (valread > 0) {
                                char input_string[BUFFER_SIZE];
                                    char input[BUFFER_SIZE];
                                int x=0;
                                    printf("Server response: %s\n", buffer);
                                    printf("Enter the seat number you want to book (e.g., '11', '12'): ");
    scanf("%s", input_string);
    send(sock, input_string, strlen(input_string), 0);
    
    memset(buffer, 0, sizeof(buffer)); // Clear buffer
    read(sock, buffer, sizeof(buffer) - 1);
    printf("Server: %s\n", buffer);
    
    scanf("%s", input);
    send(sock, input, strlen(input), 0);
    
    // 2. Read the server's response (payment prompt)
    memset(buffer, 0, sizeof(buffer)); // Clear buffer
    read(sock, buffer, sizeof(buffer) - 1);
    printf("Server: %s\n", buffer);

    // 3. Send payment confirmation (e.g., "1" to confirm)
    printf("Enter 'YES' to confirm payment: ");
    scanf("%s", input);
    send(sock, input, strlen(input), 0);

    // 4. Read the final response from the server
    memset(buffer, 0, sizeof(buffer)); // Clear buffer
    read(sock, buffer, sizeof(buffer) - 1);
    printf("Server: %s\n", buffer);
    //for-->ticket
     memset(buffer, 0, sizeof(buffer)); // Clear buffer
    read(sock, buffer, sizeof(buffer) - 1);
    printf("Server: %s\n", buffer);
                                    
                                } else {
                                    printf("No response or connection error.\n");
                                }
                            } else if (choice == 2) {
                                // Log-out
                                printf("Logging out...\n");
                                loggedIn = 0;
                            } else {
                                printf("Invalid option. Please try again.\n");
                            }
                        }
                    }
                } else {
                    printf("No response or connection error.\n");
                }
            } else if (choice == 2) {
                // User sign-up logic
                printf("Enter username: ");
                scanf("%49s", username);
                printf("Enter password: ");
                scanf("%49s", password);
                snprintf(buffer, sizeof(buffer), "user_signup:%s:%s", username, password);
                send(sock, buffer, strlen(buffer), 0);
                printf("User sign-up request sent.\n");

                // Read server response
                memset(buffer, 0, sizeof(buffer));
                valread = read(sock, buffer, sizeof(buffer));
                if (valread > 0) {
                    printf("Server response: %s\n", buffer);
                } else {
                    printf("No response or connection error.\n");
                }
            } else if (choice == 3) {
                // Exit
                printf("Exiting...\n");
                break;
            } else {
                printf("Invalid option. Please try again.\n");
            }
        } else if (user_type == 2) {
            // Admin logic can go here
            printf("\nAdmin Menu:\n");
            printf("1. Admin Login\n");
            printf("2. Exit\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            if (choice == 1) { // Admin login
    printf("Enter admin username: ");
    scanf("%49s", username);
    printf("Enter admin password: ");
    scanf("%49s", password);

    snprintf(buffer, sizeof(buffer), "admin_login:%s:%s", username, password);
    send(sock, buffer, strlen(buffer), 0);
    printf("Admin login request sent.\n");

    // Read server response
    memset(buffer, 0, sizeof(buffer));
    valread = read(sock, buffer, sizeof(buffer));
    if (valread > 0) {
        printf("Server response: %s\n", buffer);

        if (strcmp(buffer, "Admin login successful") == 0) {
            while (1) {
                printf("\nAdmin Menu:\n");
                printf("1. Add Bus\n");
                printf("2. Logout\n");
                printf("Enter choice: ");
                int admin_choice;
                scanf("%d", &admin_choice);

                if (admin_choice == 1) {
                    char source[50], destination[50], time[10];
                    int seats;

                    printf("Enter source: ");
                    scanf("%49s", source);
                    printf("Enter destination: ");
                    scanf("%49s", destination);
                    printf("Enter time (HH:MM): ");
                    scanf("%9s", time);
                    printf("Enter available seats: ");
                    scanf("%d", &seats);

                    snprintf(buffer, sizeof(buffer), "add_bus:%s:%s:%s:%d", source, destination, time, seats);
                    send(sock, buffer, strlen(buffer), 0);

                    // Read server response
                    memset(buffer, 0, sizeof(buffer));
                    valread = read(sock, buffer, sizeof(buffer));
                    printf("Server response: %s\n", buffer);
                } else if (admin_choice == 2) {
                    snprintf(buffer, sizeof(buffer), "logout");
                    send(sock, buffer, strlen(buffer), 0);

                    memset(buffer, 0, sizeof(buffer));
                    valread = read(sock, buffer, sizeof(buffer));
                    printf("Server response: %s\n", buffer);
                    break;
                } else {
                    printf("Invalid option. Try again.\n");
                }
            }
        }
    } else {
        printf("No response or connection error.\n");
    }
}
 else if (choice == 2) {
                printf("Exiting...\n");
                break;
            } else {
                printf("Invalid option. Please try again.\n");
            }
        } else {
            printf("Invalid selection. Please choose 1 for User or 2 for Admin.\n");
            break;
        }
    }

    close(sock);
    printf("Connection closed.\n");
    return 0;
}

