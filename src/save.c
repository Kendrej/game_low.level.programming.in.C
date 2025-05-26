#include "save.h"
#include "board.h"
#include "input.h"
#include <stdio.h>

bool save_game(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return false;

    PieceColor player = get_current_player();
    fprintf(f, "%d\n", player);  // 0 = NONE, 1 = WHITE, 2 = BLACK

    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++) {
            Piece p = get_piece(x, y);
            fprintf(f, "%d %d %d %d\n", x, y, p.color, p.isQueen);
        }

    fclose(f);
    return true;
}

bool load_game(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return false;

    int player;
    if (fscanf(f, "%d", &player) != 1) return false;

    PieceColor loaded_player = (PieceColor)player;
    set_current_player(loaded_player);  // musisz dodać tę funkcję setter w input.c/h

    Piece empty = { NONE, 0 };
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            set_piece(x, y, empty);

    int x, y, color, isQueen;
    while (fscanf(f, "%d %d %d %d", &x, &y, &color, &isQueen) == 4) {
        Piece p = { (PieceColor)color, isQueen };
        set_piece(x, y, p);
    }

    fclose(f);
    return true;
}
