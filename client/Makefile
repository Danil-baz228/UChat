CC = gcc
CFLAGS = -I./inc
LIBS = `pkg-config --cflags --libs gtk+-3.0`
SRC = src/client.c src/handlers.c src/chat.c src/stickers.c
OBJ = $(SRC:src/%.c=obj/%.o)
OUT = client

# Директория для объектных файлов
obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) $(LIBS) -c $< -o $@

# Сборка исполняемого файла
$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBS)

# Очистка объектных файлов и исполняемого файла
clean:
	rm -f obj/*.o $(OUT)

.PHONY: clean
