#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <string.h>
#include <stdio.h>
#include "board.h"

static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_EVENT_QUEUE *event_queue = NULL;
static ALLEGRO_FONT *font24  = NULL;  
static ALLEGRO_FONT *font48  = NULL;   

ALLEGRO_EVENT_QUEUE *get_event_queue() {
    return event_queue;
}

bool init_display(int width, int height) {
    if (!al_init()) return false;
    al_init_primitives_addon();
    al_install_keyboard();
    display = al_create_display(width, height);
    if (!display) return false;

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    return true;
}

void wait_for_exit() {
    ALLEGRO_EVENT ev;
    bool running = true;

    while (running) {
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE ||
            (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
            running = false;
        }
    }
}

void destroy_display() {
    if (event_queue) al_destroy_event_queue(event_queue);
    if (display) al_destroy_display(display);
}
void draw_board() {
    int size = 100;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            ALLEGRO_COLOR color = ((row + col) % 2 == 0)
                ? al_map_rgb(240, 217, 181)   
                : al_map_rgb(181, 136, 99);    

            al_draw_filled_rectangle(
                col * size, row * size,
                (col + 1) * size, (row + 1) * size,
                color
            );
        }
    }
    al_flip_display();
}


void load_font(void)
{
    al_init_font_addon();
    al_init_ttf_addon();
    font24 = al_load_ttf_font("assets/arial.ttf", 24, 0);
    font48 = al_load_ttf_font("assets/arial.ttf", 48, 0);
    if (!font24 || !font48) {
        fprintf(stderr, "Nie udało się załadować czcionki!\n");
        exit(1);
    }
}

void destroy_font(void)
{
    if (font24) al_destroy_font(font24);
    if (font48) al_destroy_font(font48);
}

void draw_turn(PieceColor player)
{
    const char *txt = (player == WHITE) ? "TURA: BIAŁE" : "TURA: CZARNE";
    ALLEGRO_COLOR g = al_map_rgb(0,200,0);                      
    int w = al_get_text_width (font24, txt);
    al_draw_text(font24, g,
                 (al_get_display_width(display) - w)/2, 10, 0, txt);
}

void draw_game_over(PieceColor winner)
{
    const char *g1 = "KONIEC GRY";
    const char *g2 = (winner == WHITE) ? "WYGRAŁY BIAŁE" : "WYGRAŁY CZARNE";
    ALLEGRO_COLOR c = al_map_rgb(255, 50, 50);                  
    int sw = al_get_display_width(display);
    int sh = al_get_display_height(display);
    int w1 = al_get_text_width(font48, g1);
    int w2 = al_get_text_width(font48, g2);

    al_draw_text(font48, c, (sw - w1)/2, sh/2 - 60, 0, g1);
    al_draw_text(font48, c, (sw - w2)/2, sh/2 + 10, 0, g2);
}