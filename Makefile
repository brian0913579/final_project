CC = gcc
CFLAGS = -Wall -g $(shell pkg-config --cflags allegro-5)
LIBS = $(shell pkg-config --libs allegro-5 allegro_main-5 allegro_primitives-5 allegro_image-5 allegro_font-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5)

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

TARGET = cancer_cell_game

$(shell mkdir -p $(OBJ_DIR))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -MMD -MP -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

-include $(DEPS)
