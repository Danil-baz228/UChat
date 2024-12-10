# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
AR = ar rcs
RM = rm -f

# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj

# Files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = libmx.a

# Rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(AR) $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET) # Удаление libmx.a
	$(RM) -r $(OBJ_DIR)

reinstall: clean all
