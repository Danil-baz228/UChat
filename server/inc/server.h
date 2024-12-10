#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "../../resources/libmx/inc/libmx.h"

#define PORT 9090
#define BUFFER_SIZE 256
#define MAX_CLIENTS 100

extern int client_sockets[100];
extern int client_count;

//authenticate.c
int register_user(sqlite3 *db, const char *username, const char *password);
int authenticate_user(sqlite3 *db, const char *username, const char *password);

//db_handler.c
void check_sqlite_error(int rc, const char *message, sqlite3 *db);
int get_all_users(sqlite3 *db, char *result, size_t result_size);

//message_server.c
int send_message(sqlite3 *db, const char *sender, const char *receiver, const char *message, int client_sock);
int get_messages(sqlite3 *db, const char *user1, const char *user2, char *result, size_t result_size);
int delete_message(sqlite3 *db, int message_id);
int edit_message(sqlite3 *db, int message_id, const char *new_message);

//server.c
void broadcast_notification(const char *message, int exclude_sock);
void hash_password(const char *password, char *hashed_password, size_t hashed_password_size);

#endif // SERVER_H

