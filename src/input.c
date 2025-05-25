#include "input.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "graphics.h"
#include "board.h"
#include <stdio.h>

#define TILE_SIZE 100

static int cursor_x = 0;
static int cursor_y = 0;

static int selected = 0;
static int selected_x = -1;
static int selected_y = -1;

static PieceColor current_player = WHITE;

extern Piece get_piece(int x, int y); // jeśli nie masz

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
            // cofnięcie zaznaczenia jeśli nieudany ruch
            selected = 0;
            selected_x = -1;
            selected_y = -1;
        }
        // zmiana tury odbywa się wyłącznie w move_selected_piece()
    }
}

int move_selected_piece() {
    if (get_piece(cursor_x, cursor_y).color != NONE)
        return 0;

    int dx = cursor_x - selected_x;
    int dy = cursor_y - selected_y;
    int expected_dy = (current_player == WHITE) ? -1 : 1;

    // NORMALNY RUCH (tylko jeśli NIE jesteśmy po biciu)
    if (!has_additional_jump(selected_x, selected_y, current_player)) {
        if ((dx == -1 || dx == 1) && dy == expected_dy) {
            set_piece(cursor_x, cursor_y, get_piece(selected_x, selected_y));
            set_piece(selected_x, selected_y, (Piece){NONE, 0});
            selected = 0;
            selected_x = -1;
            selected_y = -1;
            current_player = (current_player == WHITE) ? BLACK : WHITE;
            return 1;
        }
    }

    // BICIE
    if ((dx == -2 || dx == 2) && dy == 2 * expected_dy) {
        int mid_x = selected_x + dx / 2;
        int mid_y = selected_y + dy / 2;

        Piece middle = get_piece(mid_x, mid_y);
        if (middle.color != NONE && middle.color != current_player) {
            // bijemy
            set_piece(cursor_x, cursor_y, get_piece(selected_x, selected_y));
            set_piece(selected_x, selected_y, (Piece){NONE, 0});
            set_piece(mid_x, mid_y, (Piece){NONE, 0});

            // sprawdzamy, czy można bić dalej
            if (has_additional_jump(cursor_x, cursor_y, current_player)) {
                selected = 1;
                selected_x = cursor_x;
                selected_y = cursor_y;
            } else {
                selected = 0;
                selected_x = -1;
                selected_y = -1;
                current_player = (current_player == WHITE) ? BLACK : WHITE;
            }

            return 1;
        }
    }

    // RUCH NIELEGALNY
    return 0;
}


int has_additional_jump(int x, int y, PieceColor color) {
    int dy = (color == WHITE) ? -1 : 1;

    for (int dx = -2; dx <= 2; dx += 4) {
        int mid_x = x + dx / 2;
        int mid_y = y + dy;
        int new_x = x + dx;
        int new_y = y + 2 * dy;

        if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7)
            continue;

        Piece middle = get_piece(mid_x, mid_y);
        Piece target = get_piece(new_x, new_y);

        if (middle.color != NONE && middle.color != color && target.color == NONE)
            return 1;
    }

    return 0;
}

int any_jump_available(PieceColor color) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            Piece p = get_piece(x, y);
            if (p.color == color) {
                if (has_additional_jump(x, y, color)) {
                    return 1; // jest bicie
                }
            }
        }
    }
    return 0;
}
