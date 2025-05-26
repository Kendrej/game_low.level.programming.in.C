CC = gcc
CFLAGS = -Wall -std=c11 -Iinclude
LIBS = -lallegro -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_image -lallegro_audio -lallegro_acodec

SRC = src/main.c src/graphics.c src/board.c src/input.c src/save.c

all: warcaby

warcaby: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o warcaby $(LIBS)

clean:
	rm -f warcaby
