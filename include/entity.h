#ifndef ENTITY_H
#define ENTITY_H

#include "game.h" // For Entity struct, Game struct, etc.
#include <math.h>  // For sqrt in enemy logic if needed directly

// Function declarations for entity management
void update_enemy(Entity* enemy, Game* game);
bool check_collision(Entity* a, Entity* b);
void handle_collisions(Game* game);

#endif /* ENTITY_H */
