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

    bool running = true;
    while (running) {
        al_clear_to_color(al_map_rgb(0, 0, 0));

        draw_board();
        draw_pieces();
        draw_cursor();              // ← TO JEST KLUCZOWE

        al_flip_display();          // ← JEDYNE miejsce, gdzie to robimy

        running = handle_input();   // ← sterowanie
    }

    destroy_display();
    return 0;
}
