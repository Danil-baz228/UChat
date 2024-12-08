#include "../inc/server.h"

int client_sockets[100];
int client_count = 0;

void broadcast_notification(const char *message, int exclude_sock) {
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] != exclude_sock) { // Не отправляем уведомление отправителю
            if (write(client_sockets[i], message, strlen(message)) < 0) {
                fprintf(stderr, "Failed to send notification to client %d\n", client_sockets[i]);
            }
        }
    }
}


// Функция для хеширования пароля
void hash_password(const char *password, char *hashed_password, size_t hashed_password_size) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)password, strlen(password), hash);

    // Конвертируем байты в строку
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        snprintf(hashed_password + (i * 2), hashed_password_size - (i * 2), "%02x", hash[i]);
    }
}

// Обновляем обработчик клиента
void handle_client(int client_sock, sqlite3 *db) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    int bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        fprintf(stderr, "Error reading from client\n");
        close(client_sock);
        return;
    }

    char command[64], arg1[64], arg2[64], arg3[BUFFER_SIZE];
    sscanf(buffer, "%s %s %s %[^\n]", command, arg1, arg2, arg3);

    if (strcmp(command, "REGISTER") == 0) {
        if (register_user(db, arg1, arg2) == 0) {
            const char *response = "OK";
            write(client_sock, response, strlen(response));
        } else {
            const char *response = "ERROR";
            write(client_sock, response, strlen(response));
        }
    } else if (strcmp(command, "LOGIN") == 0) {
        if (authenticate_user(db, arg1, arg2)) {
            const char *response = "OK";
            write(client_sock, response, strlen(response));
        } else {
            const char *response = "INVALID";
            write(client_sock, response, strlen(response));
        }
    } else if (strcmp(command, "SEND_MESSAGE") == 0) {
        if (send_message(db, arg1, arg2, arg3, client_sock) == 0) { // Передаем client_sock
            const char *response = "OK";
            write(client_sock, response, strlen(response));
        } else {
            const char *response = "ERROR";
            write(client_sock, response, strlen(response));
        }
    } else if (strcmp(command, "EDIT_MESSAGE") == 0) {
        int message_id;
        char new_message[BUFFER_SIZE];

        // Найдите разделитель "|" и извлеките ID и сообщение
        char *delimiter = strchr(arg1, '|');
        if (delimiter) {
            *delimiter = '\0'; // Разделяем строку
            message_id = atoi(arg1); // Получаем ID сообщения
            strcpy(new_message, delimiter + 1); // Получаем текст сообщения

            // Убираем кавычки, если они есть
            size_t len = strlen(new_message);
            if (new_message[0] == '"' && new_message[len - 1] == '"') {
                new_message[len - 1] = '\0';
                memmove(new_message, new_message + 1, len - 1);
            }

            if (edit_message(db, message_id, new_message) == 0) {
                const char *response = "OK";
                write(client_sock, response, strlen(response));
            } else {
                const char *response = "ERROR";
                write(client_sock, response, strlen(response));
            }
        } else {
            fprintf(stderr, "Invalid EDIT_MESSAGE format: %s\n", arg1);
            const char *response = "INVALID_FORMAT";
            write(client_sock, response, strlen(response));
        }
    }
    else if (strcmp(command, "GET_MESSAGES") == 0) {
        char result[BUFFER_SIZE * 10];
        if (get_messages(db, arg1, arg2, result, sizeof(result)) == 0) {
            write(client_sock, result, strlen(result));
        } else {
            const char *response = "ERROR";
            write(client_sock, response, strlen(response));
        }
    } else if (strcmp(command, "GET_USERS") == 0) {
        char result[BUFFER_SIZE * 10]; // Буфер для списка пользователей
        if (get_all_users(db, result, sizeof(result)) == 0) {
            write(client_sock, result, strlen(result));
        } else {
            const char *response = "ERROR";
            write(client_sock, response, strlen(response));
        }
    }
    else if (strcmp(command, "DELETE_MESSAGE") == 0) {
    int message_id = atoi(arg1); // ID сообщения, передается первым аргументом

    if (delete_message(db, message_id) == 0) {
        const char *response = "OK";
        write(client_sock, response, strlen(response));
    } else {
        const char *response = "ERROR";
        write(client_sock, response, strlen(response));
    }
}
    else {
        const char *response = "UNKNOWN_COMMAND";
        write(client_sock, response, strlen(response));
    }

    close(client_sock);
}



int main() {
    sqlite3 *db;
    const char *db_path = "../resources/identifier.sqlite";

    if (access(db_path, F_OK) != 0) {
        fprintf(stderr, "Error: Database file not found at %s\n", db_path);
        return EXIT_FAILURE;
    }

    int rc = sqlite3_open(db_path, &db);
    check_sqlite_error(rc, "Cannot open database", db);

    printf("Database connection established: %s\n", db_path);

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        close(server_sock);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        handle_client(client_sock, db);
    }

    sqlite3_close(db);
    close(server_sock);
    return EXIT_SUCCESS;
}
