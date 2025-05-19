#ifndef LEVEL_H
#define LEVEL_H

#include "game.h" // For Level, Platform, Entity, Game structs
#include <allegro5/allegro.h> // For ALLEGRO_BITMAP if used directly in these funcs
#include <stdio.h>  // For sprintf, fprintf
#include <stdlib.h> // For malloc, free
#include <string.h> // For strdup
#include <math.h>   // For sin (used in level 1 platform generation)

// Function declarations for level management
void init_level(Level* level, const char* name, const char* description, float width);
void init_levels(Game* game);
void init_level_content(Level* level, int level_number);
void cleanup_level(Level* level);
void cleanup_levels(Game* game);

#endif /* LEVEL_H */
