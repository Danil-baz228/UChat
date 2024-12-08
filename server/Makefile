# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
LIBS = -lsqlite3 -lpthread -lssl -lcrypto

# Исходники, объектные файлы и исполняемый файл
SRC = src/authenticate.c src/server.c src/message_server.c src/db_handler.c
OBJ = $(SRC:src/%.c=obj/%.o)
OUT = uchat_server

# Сборка объектных файлов
obj/%.o: src/%.c
	@mkdir -p obj
	@echo "Compiling $< -> $@"
	$(CC) $(CFLAGS) -c $< -o $@


# Сборка исполняемого файла
$(OUT): $(OBJ)
	@echo "Linking $(OBJ) -> $@"
	$(CC) -o $(OUT) $(OBJ) $(LIBS)

# Цель по умолчанию
all: $(OUT)

# Полная пересборка
re: clean all

# Очистка объектных файлов и исполняемого файла
clean:
	@echo "Cleaning up..."
	rm -rf obj $(OUT)

.PHONY: clean all re

