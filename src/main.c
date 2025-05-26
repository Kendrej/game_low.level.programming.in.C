#include <allegro5/allegro.h>
#include "graphics.h"
#include "board.h"
#include "input.h"

int main() {
    if (!init_display(800, 800)) {
        return 1;
    }

    init_board();
    init_cursor();
    load_font();
    bool running = true;
        while (running) {
        al_clear_to_color(al_map_rgb(0, 0, 0));

        draw_board();
        draw_pieces();
        draw_cursor();
        draw_turn(get_current_player()); 

        if (!any_piece_left(BLACK) || !any_piece_left(WHITE) ||
            (!any_move_available(WHITE) && !any_move_available(BLACK))) {

            PieceColor winner;
            if (!any_piece_left(BLACK))       winner = WHITE;
            else if (!any_piece_left(WHITE))  winner = BLACK;
            else                              winner = (get_current_player()==WHITE)?BLACK:WHITE;

            draw_game_over(winner);
            al_flip_display();
            al_rest(5.0);
            return 0;
        }
        al_flip_display();
        running = handle_input();
    }

    destroy_font();
    destroy_display();
    return 0;
}
