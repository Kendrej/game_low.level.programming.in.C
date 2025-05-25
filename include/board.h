#pragma once

typedef enum {
    NONE,
    BLACK,
    WHITE
} PieceColor;

typedef struct {
    PieceColor color;
    int isQueen;
} Piece;

void init_board(void);
void draw_pieces(void);
Piece get_piece(int x, int y);
void set_piece(int x, int y, Piece p);
