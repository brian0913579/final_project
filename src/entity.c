#include "../include/entity.h"
#include "../include/game.h" // For Game, Level, Platform, Entity types
#include <math.h> // For sqrt
#include <stdio.h> // For printf in case of debugging, can be removed later

// Original update_enemy function from main.c
void update_enemy(Entity* enemy, Game* game) {
    switch (enemy->behavior) {
        case BEHAVIOR_PATROL:
            enemy->x += enemy->dx;
            if (enemy->x <= 0 || enemy->x + enemy->width >= game->current_level_data->level_width) {
                 enemy->dx *= -1;
            }
            
            float dx_patrol = game->player.x - enemy->x;
            float dy_patrol = game->player.y - enemy->y;
            float detect_distance_patrol = sqrt(dx_patrol * dx_patrol + dy_patrol * dy_patrol);
            
            if (detect_distance_patrol < 200) { // Detection range
                enemy->behavior = BEHAVIOR_CHASE;
            }
            break;
            
        case BEHAVIOR_CHASE:
            {
                float dx_chase = game->player.x - enemy->x;
                float dy_chase = game->player.y - enemy->y;
                float distance = sqrt(dx_chase * dx_chase + dy_chase * dy_chase);
                
                if (distance > 0) {
                    enemy->dx = (dx_chase / distance) * 3.0f; // Chase speed
                    enemy->dy = (dy_chase / distance) * 3.0f;
                }
                
                enemy->x += enemy->dx;
                enemy->y += enemy->dy;
            }
            
            float dx_return = game->player.x - enemy->x;
            float dy_return = game->player.y - enemy->y;
            float current_detect_distance = sqrt(dx_return*dx_return + dy_return*dy_return);

            if (current_detect_distance > 300) {
                enemy->behavior = BEHAVIOR_PATROL;
                enemy->dy = 0; 
                enemy->dx = (enemy->dx > 0) ? 2.0f : -2.0f; 
            }
            break;
        case BEHAVIOR_SHOOT:
            // TODO: Implement shooting behavior
            break;
        case BEHAVIOR_BOSS:
            // TODO: Implement boss behavior
            break;
        case BEHAVIOR_NONE:
            break;
    }
}

// Original check_collision function from main.c
bool check_collision(Entity* a, Entity* b) {
    return (a->x < b->x + b->width &&
            a->x + a->width > b->x &&
            a->y < b->y + b->height &&
            a->y + a->height > b->y);
}

// Original handle_collisions function from main.c
void handle_collisions(Game* game) {
    for (int i = 0; i < game->current_level_data->num_enemies; i++) {
        Entity* enemy = &game->current_level_data->enemies[i];
        if (enemy->active && check_collision(&game->player, enemy)) {
            if (game->player.last_attack == 0) {
                game->player.health -= enemy->attack_power;
                game->player.last_attack = 30; // Invincibility frames
                
                float dx = game->player.x - enemy->x;
                float dy = game->player.y - enemy->y;
                float distance = sqrt(dx * dx + dy * dy);
                if (distance > 0) {
                    game->player.dx = (dx / distance) * 10.0f;
                    game->player.dy = (dy / distance) * 10.0f;
                }
                
                if (game->player.health <= 0) {
                    game->state = GAME_OVER;
                }
            }
        }
    }
    
    if (game->player.last_attack > 0) {
        game->player.last_attack--;
    }
}
