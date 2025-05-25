#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>

static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_EVENT_QUEUE *event_queue = NULL;

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
                ? al_map_rgb(240, 217, 181)    // jasny
                : al_map_rgb(181, 136, 99);    // ciemny

            al_draw_filled_rectangle(
                col * size, row * size,
                (col + 1) * size, (row + 1) * size,
                color
            );
        }
    }
    al_flip_display();
}
