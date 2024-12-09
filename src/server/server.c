#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 8080

// Function to compute a simple hash from the password by summing ASCII values
unsigned long hash_password(const char *password) {
    unsigned long hash = 0;
    while (*password) {
        hash += (unsigned char)(*password);
        password++;
    }
    return hash;
}

bool check_user_login(const char *username, const char *password) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }

    char stored_username[50];
    unsigned long stored_hash;
    unsigned long password_hash = hash_password(password);

    while (fscanf(file, "%49s %lu", stored_username, &stored_hash) != EOF) {
        if (strcmp(username, stored_username) == 0) {
            fclose(file);
            return stored_hash == password_hash;
        }
    }

    fclose(file);
    return false;
}

bool register_user(const char *username, const char *password) {
    FILE *file = fopen("users.txt", "a");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }
    unsigned long password_hash = hash_password(password);
    fprintf(file, "%s %lu\n", username, password_hash);
    fclose(file);
    return true;
}

void add_bus(const char *bus_info) {
    FILE *file = fopen("buses.txt", "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    fprintf(file, "%s\n", bus_info);
    fclose(file);
}

void display_bus_seats(const char *source, const char *destination, int client_socket) {
    FILE *file = fopen("buses.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[256];
    bool bus_found = false;
    char response[1024] = "Available buses:\n";

    while (fgets(line, sizeof(line), file) != NULL) {
        char bus_source[50], bus_destination[50], seats[100];
        sscanf(line, "%49s %49s %[^\n]", bus_source, bus_destination, seats);
        if (strcmp(source, bus_source) == 0 && strcmp(destination, bus_destination) == 0) {
            strcat(response, seats);
            strcat(response, "\n");
            bus_found = true;
        }
    }

    if (!bus_found) {
        strcpy(response, "No buses available for the given route.\n");
    }

    send(client_socket, response, strlen(response), 0);
    fclose(file);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
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

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Client connected.\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(new_socket, buffer, sizeof(buffer));
        if (valread <= 0) {
            printf("Client disconnected or error.\n");
            break;
        }
        printf("Received data: %s\n", buffer);

        char response[1024] = {0};
        char command[50], username[50], password[50], source[50], destination[50], seats[100];

        if (sscanf(buffer, "%[^:]:%[^:]:%s", command, username, password) != 3) {
            snprintf(response, sizeof(response), "Invalid input format");
            send(new_socket, response, strlen(response), 0);
            continue;
        }

        if (strcmp(command, "user_login") == 0) {
            if (check_user_login(username, password)) {
                snprintf(response, sizeof(response), "Login successful for user: %s", username);
            } else {
                snprintf(response, sizeof(response), "Login failed for user: %s", username);
            }
        } else if (strcmp(command, "user_signup") == 0) {
            if (register_user(username, password)) {
                snprintf(response, sizeof(response), "Sign-up successful for user: %s", username);
            } else {
                snprintf(response, sizeof(response), "Sign-up failed for user: %s", username);
            }
        } else if (strcmp(command, "admin_add_bus") == 0) {
            sscanf(buffer, "%[^:]:%[^:]:%*[^:]:%49s %49s %[^\n]", source, destination, seats);
            char bus_info[256];
            snprintf(bus_info, sizeof(bus_info), "%s %s %s", source, destination, seats);
            add_bus(bus_info);
            snprintf(response, sizeof(response), "Bus added successfully for route %s to %s", source, destination);
        } else if (strcmp(command, "view_buses") == 0) {
            sscanf(buffer, "%[^:]:%[^:]:%*[^:]:%49s %49s", source, destination);
            display_bus_seats(source, destination, new_socket);
            continue;
        } else {
            snprintf(response, sizeof(response), "Unknown command");
        }

        send(new_socket, response, strlen(response), 0);
    }

    close(new_socket);
    close(server_fd);
    printf("Server closed.\n");

    return 0;
}
