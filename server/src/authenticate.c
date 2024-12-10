#include "../inc/server.h"

int register_user(sqlite3 *db, const char *username, const char *password) {
    const char *sql = "INSERT INTO users (username, password) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    char hashed_password[65];
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

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char *stored_hashed_password = (const char *)sqlite3_column_text(stmt, 0);

        char hashed_password[65];
        hash_password(password, hashed_password, sizeof(hashed_password));

        int result = mx_strcmp(stored_hashed_password, hashed_password);
        sqlite3_finalize(stmt);
        return result == 0;
    }

    sqlite3_finalize(stmt);
    return 0;
}

