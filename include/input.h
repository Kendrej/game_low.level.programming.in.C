#pragma once
#include <stdbool.h>
#include "board.h"

PieceColor get_current_player(void);
void init_cursor(void);
void move_cursor_up(void);
void move_cursor_down(void);
void move_cursor_left(void);
void move_cursor_right(void);
void draw_cursor(void);
bool handle_input(void);
void select_piece(void);
int move_selected_piece(void);
int has_additional_jump(int x, int y, PieceColor color);
int has_additional_jump_for_player(PieceColor color);
int any_move_available(PieceColor color);
int any_piece_left(PieceColor color);
