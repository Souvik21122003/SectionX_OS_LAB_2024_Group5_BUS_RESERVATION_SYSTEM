#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#define PORT 8080
#define BUFFER_SIZE 2048  
#define ROWS 4
#define COLS 4
#define BASE_PRICE 100.0
void add_bus() {
    char source[50], destination[50], time[10];
    int seats_available;

    // Take bus details as input
    printf("Enter source: ");
    scanf("%s", source);

    printf("Enter destination: ");
    scanf("%s", destination);

    printf("Enter time (HH:MM): ");
    scanf("%s", time);

    printf("Enter available seats: ");
    scanf("%d", &seats_available);

    // Append the new bus details to bus.txt
    FILE *file = fopen("bus.txt", "a");
    if (file == NULL) {
        printf("Error: Unable to open bus.txt file.\n");
        return;
    }

    fprintf(file, "%s %s %s %d\n", source, destination, time, seats_available);
    fclose(file);

    printf("Bus added successfully!\n");
}
float calculate_dynamic_price(int age, int column,int cnt) {
    float price = 100.0;
    if (column == 1 || column == COLS) {
        price *= 1.2;
    }
    if (age < 12) {
        price *= 0.9;
    } else if (age >= 65) {
        price *= 0.85;
    }
    if(cnt<(ROWS*COLS)/2)
     price*=0.95;
    return price;
}


bool check_user_login(const char *username, const char *password) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }

    char stored_username[50], stored_password[50];
    while (fscanf(file, "%49s %49s", stored_username, stored_password) != EOF) {
        if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password) == 0) {
            fclose(file);
            return true;
        }
    }
    fclose(file);
    return false;
}
void* thread_function(void* arg) {
    int thread_id = (int)arg;

    printf("Thread %d: Waiting to access shared resource...\n", thread_id);

    // Wait (P operation)
    sem_wait(&semaphore);

    // Critical Section
    printf("Thread %d: Accessing shared resource...\n", thread_id);
    int temp = shared_resource; 
    temp += 1;                  
    shared_resource = temp;     
    printf("Thread %d: Updated shared resource to %d\n", thread_id, shared_resource);

    // Signal (V operation)
    sem_post(&semaphore);

    printf("Thread %d: Released access to shared resource.\n", thread_id);
    return NULL;
}
bool register_user(const char *username, const char *password) {
    FILE *file = fopen("users.txt", "a");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }

    fprintf(file, "%s %s\n", username, password);
    fclose(file);
    return true;
}

bool search_bus(const char *s, const char *d) {
    FILE *file = fopen("bus.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char stored_source[50], stored_destination[50];
        if (sscanf(line, "%49s %49s", stored_source, stored_destination) == 2) {
            if (strcmp(s, stored_source) == 0 && strcmp(d, stored_destination) == 0) {
                fclose(file);
                return true;
            }
        }
    }
    fclose(file);
    return false;
}
void payment(int client, char *seat,float price) {
    char buffer[BUFFER_SIZE] = {0};
    char response_message[BUFFER_SIZE];

    // Send bus availability message
    
    printf("%f",price);
    snprintf(response_message, sizeof(response_message), "pay Rs.%f",price);
    send(client, response_message, strlen(response_message), 0);
    printf("Response sent to client: %s\n", response_message);

    // Read client response
    int valread = read(client, buffer, sizeof(buffer) - 1);
    if (valread > 0) {
        buffer[valread] = '\0'; // Null-terminate the string

        // Remove any trailing newline or whitespace
        buffer[strcspn(buffer, "\r\n")] = '\0';

        printf("Client sent: %s\n", buffer);

        // Compare client input
        if (strcmp(buffer, "YES") == 0) {
            snprintf(response_message, sizeof(response_message), "Payment received. Seat %s is now booked.", seat);
        } else {
            snprintf(response_message, sizeof(response_message), "Unknown command: %s", buffer);
        }

        // Send response to client
        send(client, response_message, strlen(response_message), 0);
        printf("Response sent to client: %s\n", response_message);
    }
}
void generateTicket(int client_sock, char *seat,float price) {
    char ticket_message[BUFFER_SIZE] = {0};

    // Construct the ticket message
    snprintf(ticket_message, sizeof(ticket_message),
         "\n-------------------------------\n"
         "|         BUS TICKET          |\n"
         "-------------------------------\n"
         "| Seat No.       : %-10s |\n"
         "| Amount Paid    : Rs.%-9.2f |\n"
         "| Status         : Booked     |\n"
         "| Thank You for Booking!      |\n"
         "-------------------------------\n",
         seat, price);


    // Print ticket on server console (optional)
    printf("%s", ticket_message);

    // Send the ticket to the client
    send(client_sock, ticket_message, strlen(ticket_message), 0);
    printf("Bus ticket sent to client.\n");
}
void send_bus_details(int client_sock) {
    FILE *file = fopen("bus.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    char buffer[BUFFER_SIZE] = {0};

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "##") != NULL) {
            send(client_sock, buffer, strlen(buffer), 0);
            memset(buffer, 0, sizeof(buffer)); 
            break;
        } else {
            strcat(buffer, line);
        }
    }

    if (strlen(buffer) > 0) {
        send(client_sock, buffer, strlen(buffer), 0);
    }

    strcpy(buffer, "END");
    send(client_sock, buffer, strlen(buffer), 0);

    fclose(file);
    char x[] = "Driver\n"
               "11.[x] 12.[A]  13.[A] 14.[A]\n"
               "21.[x] 22.[A]  23.[A] 24.[A]\n"
               "31.[x] 32.[A]  33.[A] 34.[A]\n"
               "41.[X] 42.[A]  43.[A] 44.[A]\n";
    int count = 0;

    // Count total available seats (A)
    for (int i = 0; i < strlen(x); i++) {
        if (x[i] == 'A') {
            count++;
        }
    }

    char seat[BUFFER_SIZE] = {0};
    char age_no[BUFFER_SIZE] = {0};
    while (1) {
         printf("Total available seats = %d\n", count);
        printf("%s\n", x);
        printf("Choose a seat: ");
         int valread = read(client_sock, seat, sizeof(seat) - 1);
         if (valread > 0) {
        buffer[valread] = '\0'; // Null-terminate the string
        printf("Client sent: %s\n", seat);
    }
        char *pos = strstr(x, seat); // Find seat in the string
        if (pos != NULL) {
            if (*(pos + 4) == 'A') { // Check if seat is available
                printf("Proceeding...\n");
                *(pos + 4) = 'x'; // Replace 'A' with 'x' to mark the seat as booked
                count--;

                printf("%s\n", x);
                printf("Total available seats = %d\n", count);
                 char r[BUFFER_SIZE];
                 snprintf(r, sizeof(r), "enter your age: %s", buffer);
                 send(client_sock, r, strlen(r), 0);
           int val = read(client_sock, age_no, sizeof(age_no) - 1);
         if (val > 0) {
        buffer[val] = '\0'; // Null-terminate the string
        printf("Client sent: %s\n", age_no);
    }                
                 int age=10*(age_no[0]-'0')+(age_no[1]-'0');
              char ch=seat[1];
              int col=ch-'0';
               float price=calculate_dynamic_price(age,col,count);
                payment(client_sock,seat,price);
                generateTicket(client_sock, seat,price);
                break;
            } else {
                 char response_message[BUFFER_SIZE];
    snprintf(response_message, sizeof(response_message), "already booked : %s", buffer);
    send(client_sock, response_message, strlen(response_message), 0);
            }
        } else {
            printf("Invalid seat. Please choose a valid seat.\n");
        }
    }
}
void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE] = {0};
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(client_sock, buffer, sizeof(buffer));
        if (valread <= 0) {
            printf("Client disconnected or error.\n");
            break;
        }
        printf("Received: %s\n", buffer);

        char response[BUFFER_SIZE] = {0};
        char command[50], username[50], password[50];
        sscanf(buffer, "%[^:]:%[^:]:%s", command, username, password);

        if (strcmp(command, "user_login") == 0) {
            if (check_user_login(username, password)) {
                snprintf(response, sizeof(response), "Login successful for user: %s. Choose 1 for Search Bus or 2 to Logout.", username);
            } else {
                snprintf(response, sizeof(response), "Invalid credentials for user: %s", username);
            }
        } else if (strcmp(command, "user_signup") == 0) {
            if (register_user(username, password)) {
                snprintf(response, sizeof(response), "Sign-up successful for user: %s", username);
            } else {
                snprintf(response, sizeof(response), "Sign-up failed for user: %s", username);
            }
        } else if (strcmp(command, "admin_login") == 0) {
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        snprintf(response, sizeof(response), "Admin login successful");
        send(client_sock, response, strlen(response), 0);

        // Admin logged in, wait for bus addition requests
        while (1) {
            memset(buffer, 0, sizeof(buffer));
            valread = read(client_sock, buffer, sizeof(buffer));
            if (valread > 0) {
               if (strncmp(buffer, "add_bus", 7) == 0) {
    char source[50], destination[50], time[20];
    int total_seats;

    // Parse bus details from buffer
    sscanf(buffer, "add_bus:%49[^:]:%49[^:]:%19[^:]:%d", source, destination, time, &total_seats);

    // Open the bus file to append
    FILE *file = fopen("bus.txt", "a");
    if (file == NULL) {
        perror("Error opening file");
        snprintf(response, sizeof(response), "Error: Could not open bus file");
        send(client_sock, response, strlen(response), 0);
        continue;
    }

    // Write bus details into the file
    fprintf(file, "Source: %s, Destination: %s, Time: %s, Seats: %d\n", source, destination, time, total_seats);
    fprintf(file, "Seat Matrix:\n");

    // Generate and write the seat matrix (4x4)
    for (int i = 1; i <= 4; i++) {
        for (int j = 1; j <= 4; j++) {
            fprintf(file, "%d%d.[A] ", i, j); // Format: row_column.[A]
        }
        fprintf(file, "|\n"); // End of row
    }
    fprintf(file, "\n"); // Separate buses with a newline
    fclose(file);

    // Send response back to the client
    snprintf(response, sizeof(response), "Bus added successfully with seat matrix.");
    send(client_sock, response, strlen(response), 0);
}
 else if (strncmp(buffer, "logout", 6) == 0) {
                    snprintf(response, sizeof(response), "Admin logged out.");
                    send(client_sock, response, strlen(response), 0);
                    break;
                }
            }
        }
    } else {
        snprintf(response, sizeof(response), "Invalid admin credentials.");
        send(client_sock, response, strlen(response), 0);
    }
        } else if (strcmp(command, "search_bus") == 0) {
            char source[50], destination[50];
            sscanf(buffer, "%*[^:]:%49[^:]:%49s", source, destination);
            printf("Received source=%s, destination=%s\n", source, destination);

            if (search_bus(source, destination)) {
                send_bus_details(client_sock);
            } else {
                snprintf(response, sizeof(response), "No bus available from %s to %s", source, destination);
                send(client_sock, response, strlen(response), 0);
            }
        } else if (strcmp(command, "logout") == 0) {
            snprintf(response, sizeof(response), "Logout successful for user: %s", username);
            send(client_sock, response, strlen(response), 0);
            break;
        } else {
            snprintf(response, sizeof(response), "Unknown command");
        }

        send(client_sock, response, strlen(response), 0);
        printf("Response sent: %s\n", response);
    }

    close(client_sock);
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    while (1) {
        int *new_socket = malloc(sizeof(int));
        if ((*new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            free(new_socket);
            continue;
        }
        printf("Client connected.\n");

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, new_socket) != 0) {
            perror("Failed to create thread");
            free(new_socket);
            continue;
        }
        pthread_detach(thread_id); // Detach the thread to free resources automatically
    }
    close(server_fd);
}
