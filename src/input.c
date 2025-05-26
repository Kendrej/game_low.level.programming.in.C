#include "input.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "graphics.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>

#define TILE_SIZE 100

static int cursor_x = 0;
static int cursor_y = 0;

static int selected = 0;
static int selected_x = -1;
static int selected_y = -1;

static PieceColor current_player = WHITE;

void init_cursor() {
    cursor_x = 0;
    cursor_y = 0;
    selected = 0;
    selected_x = -1;
    selected_y = -1;
    current_player = WHITE;
}

void draw_cursor() {
    float x1 = cursor_x * TILE_SIZE;
    float y1 = cursor_y * TILE_SIZE;
    float x2 = x1 + TILE_SIZE;
    float y2 = y1 + TILE_SIZE;

    ALLEGRO_COLOR color = selected ? al_map_rgb(0, 255, 0) : al_map_rgb(255, 0, 0);
    al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgba(255, 0, 0, 60));
    al_draw_rectangle(x1 + 2, y1 + 2, x2 - 2, y2 - 2, color, 3.0);
}

void move_cursor_up()    { if (cursor_y > 0) cursor_y--; }
void move_cursor_down()  { if (cursor_y < 7) cursor_y++; }
void move_cursor_left()  { if (cursor_x > 0) cursor_x--; }
void move_cursor_right() { if (cursor_x < 7) cursor_x++; }

bool handle_input() {
    ALLEGRO_EVENT ev;
    ALLEGRO_TIMEOUT timeout;
    al_init_timeout(&timeout, 0.01);

    if (al_wait_for_event_until(get_event_queue(), &ev, &timeout)) {
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP:    move_cursor_up(); break;
                case ALLEGRO_KEY_DOWN:  move_cursor_down(); break;
                case ALLEGRO_KEY_LEFT:  move_cursor_left(); break;
                case ALLEGRO_KEY_RIGHT: move_cursor_right(); break;
                case ALLEGRO_KEY_SPACE: select_piece(); break;
                case ALLEGRO_KEY_ESCAPE: return false;
            }
        }
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return false;
        }
    }

    return true;
}

void select_piece() {
    Piece p = get_piece(cursor_x, cursor_y);

    if (!selected) {
        if (p.color == current_player) {
            selected = 1;
            selected_x = cursor_x;
            selected_y = cursor_y;
        }
    } else {
        int moved = move_selected_piece();
        if (!moved) {
            selected = 0;
            selected_x = -1;
            selected_y = -1;
        }
    }
}

int has_additional_jump(int x, int y, PieceColor color) {
    Piece p = get_piece(x, y);
    int directions[2] = { -1, 1 };

    for (int dx_dir = 0; dx_dir < 2; dx_dir++) {
        for (int dy_dir = 0; dy_dir < 2; dy_dir++) {
            int dx = directions[dx_dir];
            int dy = directions[dy_dir];

            int cx = x + dx;
            int cy = y + dy;

            if (p.isQueen) {
                int found = 0;
                while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8) {
                    Piece at = get_piece(cx, cy);

                    if (at.color == NONE) {
                        if (found) return 1;
                        cx += dx;
                        cy += dy;
                        continue;
                    }

                    if (at.color == color) break;

                    if (!found) {
                        found = 1;
                        cx += dx;
                        cy += dy;
                    } else {
                        break;
                    }
                }
            } else {
                int mx = x + dx;
                int my = y + dy;
                int nx = x + 2 * dx;
                int ny = y + 2 * dy;

                if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8) continue;

                Piece middle = get_piece(mx, my);
                Piece target = get_piece(nx, ny);

                if (middle.color != NONE && middle.color != color && target.color == NONE)
                    return 1;
            }
        }
    }

    return 0;
}

int any_jump_available(PieceColor color) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece p = get_piece(x, y);
            if (p.color == color && has_additional_jump(x, y, color)) {
                return 1;
            }
        }
    }
    return 0;
}

int move_selected_piece() {
    /* nie wolno wejść na zajęte pole */
    if (get_piece(cursor_x, cursor_y).color != NONE) return 0;

    int dx = cursor_x - selected_x;
    int dy = cursor_y - selected_y;
    Piece piece = get_piece(selected_x, selected_y);
    int expected_dy = piece.isQueen ? 0 : (piece.color == WHITE ? -1 : 1);

    /* --------------------------------------------------   1.  BICIE  */
    if (abs(dx) == abs(dy)) {
        // pionek nie może bić do tyłu
        if (!piece.isQueen) {
            if ((piece.color == WHITE && dy > 0) || (piece.color == BLACK && dy < 0)) {
                return 0;
            }
        }

        int sx = (dx > 0) ? 1 : -1;
        int sy = (dy > 0) ? 1 : -1;
        int x = selected_x + sx, y = selected_y + sy;

        int found = 0, mx = -1, my = -1;

        while ((x != cursor_x || y != cursor_y) && x>=0 && x<8 && y>=0 && y<8) {
            Piece p = get_piece(x, y);
            if (p.color != NONE) {
                if (p.color == current_player) return 0;
                if (found) return 0;
                found = 1; mx = x; my = y;
            }
            x += sx; y += sy;
        }

        if (x == cursor_x && y == cursor_y && found) {  /* mamy bicie */
            set_piece(cursor_x, cursor_y, piece);
            set_piece(selected_x, selected_y, (Piece){NONE,0});
            set_piece(mx, my, (Piece){NONE,0});          /* zbijamy   */

            /* awans */
            if ((piece.color==WHITE && cursor_y==0) || (piece.color==BLACK && cursor_y==7)) {
                piece.isQueen = 1; set_piece(cursor_x,cursor_y,piece);
            }

            /* czy jest kolejne bicie? */
            if (has_additional_jump(cursor_x, cursor_y, current_player)) {
                selected = 1; selected_x = cursor_x; selected_y = cursor_y;
            } else {
                selected = 0; selected_x = selected_y = -1;
                current_player = (current_player==WHITE)?BLACK:WHITE;
            }
            return 1;
        }
    }

    /* --------------------------------------------------   2.  RUCH DAMKĄ (bez bicia) */
    if (piece.isQueen && abs(dx)==abs(dy)) {
        int sx = (dx>0)?1:-1, sy = (dy>0)?1:-1;
        int x = selected_x+sx, y = selected_y+sy;
        while (x!=cursor_x && y!=cursor_y) {
            if (get_piece(x,y).color!=NONE) return 0;
            x+=sx; y+=sy;
        }
        if (!has_additional_jump(selected_x,selected_y,current_player) &&
            !any_jump_available(current_player)) {
            set_piece(cursor_x,cursor_y,piece);
            set_piece(selected_x,selected_y,(Piece){NONE,0});
            selected=0; selected_x=selected_y=-1;
            current_player = (current_player==WHITE)?BLACK:WHITE;
            return 1;
        }
        return 0;
    }

    /* --------------------------------------------------   3.  RUCH ZWYKŁYM PIONKIEM */
    if (!has_additional_jump(selected_x,selected_y,current_player) &&
        !any_jump_available(current_player)) {
        if (abs(dx)==1 && ((piece.isQueen&&abs(dy)==1) || dy==expected_dy)) {
            set_piece(cursor_x,cursor_y,piece);
            set_piece(selected_x,selected_y,(Piece){NONE,0});
            if ((piece.color==WHITE && cursor_y==0) || (piece.color==BLACK && cursor_y==7)) {
                piece.isQueen=1; set_piece(cursor_x,cursor_y,piece);
            }
            selected=0; selected_x=selected_y=-1;
            current_player = (current_player==WHITE)?BLACK:WHITE;
            return 1;
        }
    }

    return 0;   /* nic nie pasowało */
}

