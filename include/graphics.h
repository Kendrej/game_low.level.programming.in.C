#pragma once

#include <stdbool.h>
#include <allegro5/allegro.h> 
#include "board.h"
ALLEGRO_EVENT_QUEUE *get_event_queue(void);

void draw_game_over(PieceColor winner);
void draw_turn(PieceColor player);
void load_font(void);
void destroy_font(void);
bool init_display(int width, int height);
void draw_board(void);
void wait_for_exit(void);
void destroy_display(void);
ALLEGRO_EVENT_QUEUE *get_event_queue(void); 