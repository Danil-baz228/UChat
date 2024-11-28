CC = gcc
CFLAGS = -I./inc `pkg-config --cflags gtk+-3.0 gstreamer-1.0`
LIBS = `pkg-config --libs gtk+-3.0 gstreamer-1.0`
SRC = src/client.c src/handlers.c src/chat.c src/window_manager.c
OBJ = $(SRC:src/%.c=obj/%.o)
OUT = client

# Директория для объектных файлов
obj/%.o: src/%.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

# Сборка исполняемого файла
$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ) $(LIBS)

# Очистка объектных файлов и исполняемого файла
clean:
	rm -f obj/*.o $(OUT)

.PHONY: clean
