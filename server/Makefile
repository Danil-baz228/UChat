# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
LIBS = -lsqlite3 -lpthread -lssl -lcrypto

# Исходники, объектные файлы и исполняемый файл
SRC = server.c
OBJ = obj/server.o
OUT = server_app

# Сборка объектных файлов
obj/%.o: %.c
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
