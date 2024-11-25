#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 9090
#define BUFFER_SIZE 256

// Функция для обработки ошибок SQLite
void check_sqlite_error(int rc, const char *message, sqlite3 *db) {
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error: %s: %s\n", message, sqlite3_errmsg(db));
        if (db) sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
}

// Функция для добавления нового пользователя
int register_user(sqlite3 *db, const char *username, const char *password) {
    const char *sql = "INSERT INTO users (username, password) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    return 0;
}

// Функция для проверки логина и пароля пользователя
int authenticate_user(sqlite3 *db, const char *username, const char *password) {
    const char *sql = "SELECT COUNT(*) FROM users WHERE username = ? AND password = ?;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    int authenticated = 0;
    if (rc == SQLITE_ROW) {
        authenticated = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return authenticated;
}

// Функция для обработки клиента

// Добавляем поиск пользователей по имени
int search_users(sqlite3 *db, const char *query, char *result, size_t result_size) {
    const char *sql = "SELECT username FROM users WHERE username LIKE ?;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Добавляем wildcard к запросу
    char like_query[BUFFER_SIZE];
    snprintf(like_query, sizeof(like_query), "%%%s%%", query);
    sqlite3_bind_text(stmt, 1, like_query, -1, SQLITE_STATIC);

    // Составляем результат в строку
    result[0] = '\0';
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *username = (const char *)sqlite3_column_text(stmt, 0);
        strncat(result, username, result_size - strlen(result) - 1);
        strncat(result, "\n", result_size - strlen(result) - 1);
    }

    sqlite3_finalize(stmt);
    return 0;
}
int send_message(sqlite3 *db, const char *sender, const char *receiver, const char *message) {
    const char *sql_get_id = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt *stmt;
    int sender_id = -1, receiver_id = -1;

    // Получаем ID отправителя
    if (sqlite3_prepare_v2(db, sql_get_id, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, sender, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sender_id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    } else {
        fprintf(stderr, "Error preparing statement (sender_id): %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Получаем ID получателя
    if (sqlite3_prepare_v2(db, sql_get_id, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, receiver, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            receiver_id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    } else {
        fprintf(stderr, "Error preparing statement (receiver_id): %s\n", sqlite3_errmsg(db));
        return -1;
    }

    // Проверка существования пользователей
    if (sender_id == -1 || receiver_id == -1) {
        fprintf(stderr, "Error: User not found (sender: %s, receiver: %s)\n", sender, receiver);
        return -1;
    }

    // Вставляем сообщение в таблицу
    const char *sql_insert = "INSERT INTO messages (sender_id, receiver_id, message) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, sender_id);
        sqlite3_bind_int(stmt, 2, receiver_id);
        sqlite3_bind_text(stmt, 3, message, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            printf("Message from '%s' to '%s' successfully inserted into the database.\n", sender, receiver);
        } else {
            fprintf(stderr, "Error inserting message: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            return -1;
        }
        sqlite3_finalize(stmt);
    } else {
        fprintf(stderr, "Error preparing statement (insert message): %s\n", sqlite3_errmsg(db));
        return -1;
    }

    return 0;
}
int get_messages(sqlite3 *db, const char *user1, const char *user2, char *result, size_t result_size) {
    const char *sql = "SELECT u1.username AS sender_name, m.timestamp, m.message "
                      "FROM messages m "
                      "JOIN users u1 ON m.sender_id = u1.id "
                      "JOIN users u2 ON m.receiver_id = u2.id "
                      "WHERE (u1.username = ? AND u2.username = ?) "
                      "OR (u1.username = ? AND u2.username = ?) "
                      "ORDER BY m.timestamp ASC;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement for getting messages: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, user1, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user2, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user2, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, user1, -1, SQLITE_STATIC);

    result[0] = '\0'; // Инициализация результата

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *sender_name = (const char *)sqlite3_column_text(stmt, 0);
        const char *timestamp = (const char *)sqlite3_column_text(stmt, 1);
        const char *message = (const char *)sqlite3_column_text(stmt, 2);

        char formatted_message[512];
        snprintf(formatted_message, sizeof(formatted_message), "%s %s %s\n", sender_name, timestamp, message);

        // Конкатенация в результат
        strncat(result, formatted_message, result_size - strlen(result) - 1);
    }

    sqlite3_finalize(stmt);
    return 0;
}




int get_all_users(sqlite3 *db, char *result, size_t result_size) {
    const char *sql = "SELECT username FROM users;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    result[0] = '\0'; // Инициализируем пустой результат
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *username = (const char *)sqlite3_column_text(stmt, 0);
        strncat(result, username, result_size - strlen(result) - 1);
        strncat(result, "\n", result_size - strlen(result) - 1);
    }

    sqlite3_finalize(stmt);
    return 0;
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
    } else if (strcmp(command, "SEARCH") == 0) {
        char result[BUFFER_SIZE];
        if (search_users(db, arg1, result, sizeof(result)) == 0) {
            write(client_sock, result, strlen(result));
        } else {
            const char *response = "ERROR";
            write(client_sock, response, strlen(response));
        }
    } else if (strcmp(command, "SEND_MESSAGE") == 0) {
        if (send_message(db, arg1, arg2, arg3) == 0) {
            const char *response = "OK";
            write(client_sock, response, strlen(response));
        } else {
            const char *response = "ERROR";
            write(client_sock, response, strlen(response));
        }
    } else if (strcmp(command, "GET_MESSAGES") == 0) {
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

        printf("Client connected\n");
        handle_client(client_sock, db);
    }

    sqlite3_close(db);
    close(server_sock);
    return EXIT_SUCCESS;
}
