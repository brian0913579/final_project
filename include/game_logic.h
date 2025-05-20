#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "game.h" // For Game struct, etc.
#include <allegro5/allegro.h> // For Allegro types if needed directly

// Function declarations for core game logic, initialization, and cleanup
bool init_game(Game* game);
void init_menus(Game* game); // For initializing menu structures
void update_game(Game* game);
void cleanup_menus(Game* game);
void cleanup_game(Game* game);
void reset_player_and_level(Game* game, int level_idx); // Declaration for reset function

#endif /* GAME_LOGIC_H */
