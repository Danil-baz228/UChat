#include "../inc/server.h"

int send_message(sqlite3 *db, const char *sender, const char *receiver, const char *message, int client_sock) {
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

            // Отправляем уведомление клиентам
            broadcast_notification("NEW_MESSAGE", client_sock);
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
    const char *sql = "SELECT m.id, u1.username AS sender_name, m.timestamp, m.message "
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
        int message_id = sqlite3_column_int(stmt, 0); // ID сообщения
        const char *sender_name = (const char *)sqlite3_column_text(stmt, 1);
        const char *timestamp = (const char *)sqlite3_column_text(stmt, 2);
        const char *message = (const char *)sqlite3_column_text(stmt, 3);

        char formatted_message[512];
        snprintf(formatted_message, sizeof(formatted_message), "%d|%s|%s|%s\n", message_id, sender_name, timestamp, message);

        // Конкатенация в результат
        strncat(result, formatted_message, result_size - strlen(result) - 1);
    }

    sqlite3_finalize(stmt);
    return 0;
}


int delete_message(sqlite3 *db, int message_id) {
    const char *sql = "DELETE FROM messages WHERE id = ?;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing delete statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, message_id);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return 0;
    } else {
        fprintf(stderr, "Error deleting message: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
}

int edit_message(sqlite3 *db, int message_id, const char *new_message) {
    const char *sql = "UPDATE messages SET message = ? WHERE id = ?;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing SQL statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, new_message, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, message_id);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return 0;
    } else {
        fprintf(stderr, "Error executing SQL statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
}

