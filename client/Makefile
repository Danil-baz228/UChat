CC = gcc
CFLAGS = -I./inc -I../libmx/inc `pkg-config --cflags gtk+-3.0 gstreamer-1.0`
LIBS = `pkg-config --libs gtk+-3.0 gstreamer-1.0` -L../resources/libmx -lmx

SRC = src/client.c src/handlers.c src/chat.c src/window_manager.c src/search.c src/settings.c src/message_client.c
OBJ = $(SRC:src/%.c=obj/%.o)
OUT = uchat

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
