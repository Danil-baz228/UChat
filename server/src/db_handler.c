#include "../inc/server.h"

void check_sqlite_error(int rc, const char *message, sqlite3 *db) {
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error: %s: %s\n", message, sqlite3_errmsg(db));
        if (db) sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
}

int get_all_users(sqlite3 *db, char *result, size_t result_size) {
    const char *sql = "SELECT username FROM users;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    result[0] = '\0';
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *username = (const char *)sqlite3_column_text(stmt, 0);
        strncat(result, username, result_size - strlen(result) - 1);
        strncat(result, "\n", result_size - strlen(result) - 1);
    }

    sqlite3_finalize(stmt);
    return 0;
}

