CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -I./inc -I../libmx/inc
LIBS = -L../resources/libmx -lmx -lsqlite3 -lpthread -lssl -lcrypto

SRC = src/message_server.c src/authenticate.c src/server.c src/db_handler.c
OBJ = $(SRC:src/%.c=obj/%.o)
OUT = uchat_server

obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBS)

all: $(OUT)

clean:
	rm -rf obj $(OUT)

uninstall:
	rm -rf obj $(OUT)

reinstall: uninstall all

.PHONY: clean all uninstall reinstall
