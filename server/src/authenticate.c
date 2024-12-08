#include "../inc/server.h"

// Функция для добавления нового пользователя
int register_user(sqlite3 *db, const char *username, const char *password) {
    const char *sql = "INSERT INTO users (username, password) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    // Хешируем пароль
    char hashed_password[65]; // 64 символа + \0
    hash_password(password, hashed_password, sizeof(hashed_password));

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed_password, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error executing statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    return 0;
}

int authenticate_user(sqlite3 *db, const char *username, const char *password) {
    const char *sql = "SELECT password FROM users WHERE username = ?;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // Привязываем имя пользователя
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    // Выполняем запрос
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char *stored_hashed_password = (const char *)sqlite3_column_text(stmt, 0);

        // Хешируем введённый пароль
        char hashed_password[65];
        hash_password(password, hashed_password, sizeof(hashed_password));

        // Проверяем совпадение хешей
        int result = strcmp(stored_hashed_password, hashed_password);
        sqlite3_finalize(stmt);
        return result == 0; // Возвращаем 1, если пароли совпадают
    }

    // Если пользователь не найден
    sqlite3_finalize(stmt);
    return 0;
}

