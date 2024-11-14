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
void handle_client(int client_sock, sqlite3 *db) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    int bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        fprintf(stderr, "Error reading from client\n");
        close(client_sock);
        return;
    }

    char command[64], username[64], password[64];
    sscanf(buffer, "%s %s %s", command, username, password);

    if (strcmp(command, "REGISTER") == 0) {
        if (register_user(db, username, password) == 0) {
            const char *response = "OK";
            write(client_sock, response, strlen(response));
        } else {
            const char *response = "ERROR";
            write(client_sock, response, strlen(response));
        }
    } else if (strcmp(command, "LOGIN") == 0) {
        if (authenticate_user(db, username, password)) {
            const char *response = "OK";
            write(client_sock, response, strlen(response));
        } else {
            const char *response = "INVALID";
            write(client_sock, response, strlen(response));
        }
    } else {
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

