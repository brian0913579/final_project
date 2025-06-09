#ifndef DRAWING_H
#define DRAWING_H

#include "game.h" // For Game struct, Menu struct, Level struct, etc.
#include <allegro5/allegro_font.h> // For ALLEGRO_FONT if used directly
#include <allegro5/allegro_primitives.h> // For drawing functions

// Function declarations for drawing operations
void draw_game(Game* game);
void draw_menu(Game* game, Menu* menu, const char* title);
void draw_welcome_screen(Game* game);
void draw_main_menu(Game* game);
void draw_level_select(Game* game);
void draw_settings_menu(Game* game);
void draw_pause_screen(Game* game);
void draw_star_display(Game* game, float x, float y, int stars_earned, int max_stars);
void draw_end_screen_stars(Game* game, float center_x, float center_y, int stars_earned, int max_stars, int level);
// Note: Specific drawing for GAME_OVER, VICTORY, LEVEL_COMPLETE are handled within draw_game

#endif /* DRAWING_H */
