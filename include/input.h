#ifndef INPUT_H
#define INPUT_H

#include "game.h" // For Game struct, ALLEGRO_EVENT

// Function declarations for input handling
void handle_input(Game* game, ALLEGRO_EVENT* event);
void handle_menu_input(Game* game, ALLEGRO_EVENT* event);

#endif /* INPUT_H */
