#include "input.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "graphics.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>

#define TILE_SIZE 100

static int cursor_x = 0, cursor_y = 0;
static int selected = 0, selected_x = -1, selected_y = -1;
static PieceColor current_player = WHITE;

PieceColor get_current_player(void) { return current_player; }

void init_cursor(void) {
    cursor_x = cursor_y = 0;
    selected = 0; selected_x = selected_y = -1;
    current_player = WHITE;
}

void draw_cursor(void) {
    float x1 = cursor_x * TILE_SIZE, y1 = cursor_y * TILE_SIZE;
    float x2 = x1 + TILE_SIZE,      y2 = y1 + TILE_SIZE;
    ALLEGRO_COLOR col = selected ? al_map_rgb(0,255,0) : al_map_rgb(255,0,0);
    al_draw_rectangle(x1 + 2, y1 + 2, x2 - 2, y2 - 2, col, 3.0);
}

#define MV(dx,dy) do{ if(cursor_x+(dx)>=0&&cursor_x+(dx)<=7) cursor_x+=dx; \
                      if(cursor_y+(dy)>=0&&cursor_y+(dy)<=7) cursor_y+=dy; }while(0)
void move_cursor_up(void)    { MV(0,-1); }
void move_cursor_down(void)  { MV(0, 1); }
void move_cursor_left(void)  { MV(-1,0); }
void move_cursor_right(void) { MV( 1,0); }

bool handle_input(void) {
    ALLEGRO_EVENT ev; ALLEGRO_TIMEOUT t; al_init_timeout(&t,0.01);
    if (al_wait_for_event_until(get_event_queue(), &ev, &t)) {
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP:    move_cursor_up();    break;
                case ALLEGRO_KEY_DOWN:  move_cursor_down();  break;
                case ALLEGRO_KEY_LEFT:  move_cursor_left();  break;
                case ALLEGRO_KEY_RIGHT: move_cursor_right(); break;
                case ALLEGRO_KEY_SPACE: select_piece();      break;
                case ALLEGRO_KEY_ESCAPE:return false;
            }
        }
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) return false;
    }
    return true;
}

void select_piece(void) {
    Piece p = get_piece(cursor_x, cursor_y);
    if (!selected) {
        if (p.color == current_player) {
            selected = 1; selected_x = cursor_x; selected_y = cursor_y;
        }
    } else {
        if (!move_selected_piece()) {
            selected = 0; selected_x = selected_y = -1;
        }
    }
}

int has_additional_jump(int x, int y, PieceColor col) {
    Piece p = get_piece(x, y);
    int dir[2] = { -1, 1 };

    for (int dx_i = 0; dx_i < 2; dx_i++)
    for (int dy_i = 0; dy_i < 2; dy_i++) {
        int dx = dir[dx_i], dy = dir[dy_i];

        if (!p.isQueen) {
            if ((col == WHITE && dy != -1) || (col == BLACK && dy != 1))
                continue;
        }

        int mx = x + dx, my = y + dy;
        int nx = x + 2*dx, ny = y + 2*dy;

        if (nx < 0 || nx > 7 || ny < 0 || ny > 7) continue;

        if (p.isQueen) {
            int enemy = 0, cx = x + dx, cy = y + dy;
            while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8) {
                Piece q = get_piece(cx, cy);
                if (q.color == NONE) {
                    if (enemy) return 1;
                    cx += dx; cy += dy; continue;
                }
                if (q.color == col) break;
                if (!enemy) { enemy = 1; cx += dx; cy += dy; }
                else break;
            }
        } else {
            Piece mid = get_piece(mx, my), tgt = get_piece(nx, ny);
            if (mid.color != NONE && mid.color != col && tgt.color == NONE)
                return 1;
        }
    }
    return 0;
}

int has_additional_jump_for_player(PieceColor color) {
    for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++)
        if (get_piece(x, y).color == color &&
            has_additional_jump(x, y, color))
            return 1;
    return 0;
}

int any_move_available(PieceColor color) {
    for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++) {
        Piece p = get_piece(x, y);
        if (p.color != color) continue;

        if (has_additional_jump(x, y, color)) return 1;

        int dxs[2] = { -1, 1 };
        for (int dx_i = 0; dx_i < 2; dx_i++) {
            int dx = dxs[dx_i];
            int dy = p.isQueen ? dxs[dx_i] : (color == WHITE ? -1 : 1);

            if (!p.isQueen) {
                if ((color == WHITE && dy != -1) || (color == BLACK && dy != 1))
                    continue;
            }

            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8 &&
                get_piece(nx, ny).color == NONE)
                return 1;
        }
    }
    return 0;
}

int any_piece_left(PieceColor color) {
    for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++)
        if (get_piece(x, y).color == color)
            return 1;
    return 0;
}

int move_selected_piece(void) {
    if (get_piece(cursor_x, cursor_y).color != NONE) return 0;

    int dx = cursor_x - selected_x;
    int dy = cursor_y - selected_y;
    Piece piece = get_piece(selected_x, selected_y);
    int expected_dy = piece.isQueen ? 0 : (piece.color == WHITE ? -1 : 1);

    if (abs(dx) == abs(dy)) {
        if (!piece.isQueen) {
            if ((piece.color == WHITE && dy > 0) || (piece.color == BLACK && dy < 0))
                return 0;
        }

        int sx = (dx > 0) ? 1 : -1, sy = (dy > 0) ? 1 : -1;
        int x = selected_x + sx, y = selected_y + sy;
        int enemy = 0, mx = -1, my = -1;

        while ((x != cursor_x || y != cursor_y) && x >= 0 && x < 8 && y >= 0 && y < 8) {
            Piece q = get_piece(x, y);
            if (q.color != NONE) {
                if (q.color == current_player) return 0;
                if (enemy) return 0;
                enemy = 1; mx = x; my = y;
            }
            x += sx; y += sy;
        }

        if (x == cursor_x && y == cursor_y && enemy) {
            set_piece(cursor_x, cursor_y, piece);
            set_piece(selected_x, selected_y, (Piece){NONE, 0});
            set_piece(mx, my, (Piece){NONE, 0});

            if ((piece.color == WHITE && cursor_y == 0) ||
                (piece.color == BLACK && cursor_y == 7)) {
                piece.isQueen = 1;
                set_piece(cursor_x, cursor_y, piece);
            }

            if (has_additional_jump(cursor_x, cursor_y, current_player)) {
                selected = 1; selected_x = cursor_x; selected_y = cursor_y;
            } else {
                selected = 0; selected_x = selected_y = -1;
                current_player = (current_player == WHITE) ? BLACK : WHITE;
            }
            return 1;
        }
    }

    if (piece.isQueen && abs(dx) == abs(dy)) {
        int sx = (dx > 0) ? 1 : -1, sy = (dy > 0) ? 1 : -1;
        int x = selected_x + sx, y = selected_y + sy;
        while (x != cursor_x && y != cursor_y) {
            if (get_piece(x, y).color != NONE) return 0;
            x += sx; y += sy;
        }
        if (!has_additional_jump(selected_x, selected_y, current_player) &&
            !has_additional_jump_for_player(current_player)) {
            set_piece(cursor_x, cursor_y, piece);
            set_piece(selected_x, selected_y, (Piece){NONE, 0});
            selected = 0; selected_x = selected_y = -1;
            current_player = (current_player == WHITE) ? BLACK : WHITE;
            return 1;
        }
        return 0;
    }

    if (!has_additional_jump(selected_x, selected_y, current_player) &&
        !has_additional_jump_for_player(current_player)) {
        if (abs(dx) == 1 && dy == expected_dy) {
            set_piece(cursor_x, cursor_y, piece);
            set_piece(selected_x, selected_y, (Piece){NONE, 0});

            if ((piece.color == WHITE && cursor_y == 0) ||
                (piece.color == BLACK && cursor_y == 7)) {
                piece.isQueen = 1;
                set_piece(cursor_x, cursor_y, piece);
            }

            selected = 0; selected_x = selected_y = -1;
            current_player = (current_player == WHITE) ? BLACK : WHITE;
            return 1;
        }
    }

    return 0;
}
