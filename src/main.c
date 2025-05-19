#include "../include/game.h"      // Core game definitions, Allegro setup
#include "../include/game_logic.h" // For init_game, update_game, cleanup_game
#include "../include/input.h"    // For handle_input
#include "../include/drawing.h"  // For draw_game

int main(int argc, char **argv) {
    Game game;
    bool redraw = true; // Flag to manage redrawing efficiently

    // Initialize all game components, display, timer, player, levels, etc.
    // init_game now resides in game_logic.c
    if (!init_game(&game)) {
        // init_game should handle its own error messages.
        return -1; // Exit if initialization fails
    }

    // The timer is started within init_game.

    // Main game loop
    while (game.running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(game.event_queue, &event); // Wait for an event

        // Process input events
        // handle_input (from input.c) will manage game state changes based on input
        handle_input(&game, &event);

        // Handle specific event types for game logic and rendering
        if (event.type == ALLEGRO_EVENT_TIMER) {
            // Timer event: update game state
            // update_game (from game_logic.c) handles player movement, physics, AI, etc.
            update_game(&game);
            redraw = true; // Signal that a redraw is needed
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // Display close event: exit the game loop
            game.running = false;
        }

        // Redraw the screen if needed and the event queue is empty
        if (redraw && al_is_event_queue_empty(game.event_queue)) {
            redraw = false;
            // draw_game (from drawing.c) handles all rendering for the current game state
            // This function also calls al_flip_display()
            draw_game(&game);
        }
    }

    // Clean up resources before exiting
    // cleanup_game (from game_logic.c) frees memory, destroys Allegro objects, etc.
    cleanup_game(&game);

    return 0; // Successful exit
}
