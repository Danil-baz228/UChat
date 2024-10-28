#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
    int socket;
    char username[50];
} Client;

Client *clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    Client *client = (Client *)arg;
    char buffer[BUFFER_SIZE];
    char msg[BUFFER_SIZE + 50]; // Увеличиваем размер на 50 для никнейма и дополнительных символов

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(client->socket, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            break; // Клиент отключился
        }

        // Проверяем на переполнение
        int username_length = strlen(client->username);
        int message_length = bytes_read; // длина сообщения от клиента
        if (username_length + message_length + 2 > sizeof(msg)) { // +2 для ": " и '\0'
            fprintf(stderr, "Message too long from user: %s\n", client->username);
            continue; // Игнорируем это сообщение
        }

        // Формируем сообщение
        snprintf(msg, sizeof(msg), "%s: %.*s", client->username, message_length, buffer);

        // Пересылаем сообщение всем клиентам, кроме отправителя
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < client_count; i++) {
            if (clients[i] != client) {
                send(clients[i]->socket, msg, strlen(msg), 0);
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    // Удаляем клиента из списка
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == client) {
            clients[i] = clients[client_count - 1];
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    close(client->socket);
    free(client); // Освобождаем память для клиента
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Привязываем сокет к порту
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Set socket option failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязка сокета
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Начинаем прослушивание подключений
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if (client_count < MAX_CLIENTS) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            // Создаем нового клиента
            Client *client = malloc(sizeof(Client));
            client->socket = new_socket;

            // Получаем имя пользователя
            read(new_socket, client->username, sizeof(client->username));
            printf("User '%s' has connected.\n", client->username);

            // Добавляем нового клиента в массив клиентов
            pthread_mutex_lock(&clients_mutex);
            clients[client_count++] = client;
            pthread_mutex_unlock(&clients_mutex);

            // Создаем новый поток для клиента
            pthread_t tid;
            pthread_create(&tid, NULL, handle_client, (void *)client);
            pthread_detach(tid); // Отключаем поток, чтобы ресурсы освобождались при его завершении
        }
    }

    close(server_fd);
    return 0;
}
