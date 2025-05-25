#include "board.h"
#include <allegro5/allegro_primitives.h>
#define TILE_SIZE 100

static Piece board[8][8];

void init_board() {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            board[y][x].color = NONE;
            board[y][x].isQueen = 0;
        }
    }

    // Czarne pionki (góra)
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 8; x++) {
            if ((x + y) % 2 == 1) {
                board[y][x].color = BLACK;
            }
        }
    }

    // Białe pionki (dół)
    for (int y = 5; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if ((x + y) % 2 == 1) {
                board[y][x].color = WHITE;
            }
        }
    }

}

void draw_pieces() {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece p = board[y][x];
            if (p.color == NONE) continue;

            ALLEGRO_COLOR color = p.color == WHITE ? al_map_rgb(255, 255, 255) : al_map_rgb(0, 0, 0);

            float cx = x * TILE_SIZE + TILE_SIZE / 2.0;
            float cy = y * TILE_SIZE + TILE_SIZE / 2.0;
            float r = TILE_SIZE * 0.4;

            al_draw_filled_circle(cx, cy, r, color);

            if (p.isQueen) {
                al_draw_circle(cx, cy, r - 5, al_map_rgb(255, 215, 0), 3.0);
            }
        }
    }
    //al_flip_display();
}

Piece get_piece(int x, int y) {
    if (x >= 0 && x < 8 && y >= 0 && y < 8) {
        return board[y][x];
    }
    return (Piece){NONE, 0};
}

void set_piece(int x, int y, Piece p) {
    if (x >= 0 && x < 8 && y >= 0 && y < 8) {
        board[y][x] = p;
    }
}
