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
            
            if (detect_distance_patrol < ENEMY_PATROL_DETECT_RANGE) { // Detection range
                enemy->behavior = BEHAVIOR_CHASE;
            }
            break;
            
        case BEHAVIOR_CHASE:
            {
                float dx_chase = game->player.x - enemy->x;
                float dy_chase = game->player.y - enemy->y;
                float distance = sqrt(dx_chase * dx_chase + dy_chase * dy_chase);
                
                if (distance > 0) {
                    enemy->dx = (dx_chase / distance) * ENEMY_CHASE_SPEED; // Chase speed
                    enemy->dy = (dy_chase / distance) * ENEMY_CHASE_SPEED;
                }
                
                enemy->x += enemy->dx;
                enemy->y += enemy->dy;
            }
            
            float dx_return = game->player.x - enemy->x;
            float dy_return = game->player.y - enemy->y;
            float current_detect_distance = sqrt(dx_return*dx_return + dy_return*dy_return);

            if (current_detect_distance > ENEMY_CHASE_BREAK_RANGE) {
                enemy->behavior = BEHAVIOR_PATROL;
                enemy->dy = 0; 
                enemy->dx = (enemy->dx > 0) ? ENEMY_PATROL_SPEED : -ENEMY_PATROL_SPEED; 
            }
            break;
        case BEHAVIOR_SHOOT:
            {
                // Stationary shooting enemy - aims and shoots at player
                float dx_shoot = game->player.x - enemy->x;
                float dy_shoot = game->player.y - enemy->y;
                float distance_to_player = sqrt(dx_shoot * dx_shoot + dy_shoot * dy_shoot);
                
                if (distance_to_player <= ENEMY_SHOOT_RANGE) {
                    // Face the player
                    enemy->dx = (dx_shoot > 0) ? 1.0f : -1.0f;
                    
                    // Shoot if cooldown is ready
                    if (enemy->last_attack <= 0) {
                        // Create a projectile aimed at the player
                        create_projectile(game->current_level_data, 
                                        enemy->x + enemy->width/2, 
                                        enemy->y + enemy->height/2,
                                        game->player.x + game->player.width/2, 
                                        game->player.y + game->player.height/2,
                                        enemy->type);
                        enemy->last_attack = ENEMY_SHOOT_COOLDOWN;
                        
                        // Play enemy shooting sound if enabled
                        if (game->settings.sound_enabled && game->shoot_sound) {
                            al_play_sample(game->shoot_sound, 0.4, 0.0, 1.2, ALLEGRO_PLAYMODE_ONCE, NULL);
                        }
                    }
                } else {
                    // Move slowly towards player if out of range
                    if (distance_to_player > 0) {
                        enemy->dx = (dx_shoot / distance_to_player) * 1.0f; // Slower than chase
                        enemy->x += enemy->dx;
                    }
                }
                
                // Decrease shoot cooldown
                if (enemy->last_attack > 0) {
                    enemy->last_attack--;
                }
            }
            break;
            
        case BEHAVIOR_BOSS:
            {
                // Complex boss behavior with multiple phases
                float dx_boss = game->player.x - enemy->x;
                float dy_boss = game->player.y - enemy->y;
                float distance_to_player = sqrt(dx_boss * dx_boss + dy_boss * dy_boss);
                float health_percentage = enemy->health / enemy->max_health;
                
                if (health_percentage > ENEMY_BOSS_PHASE_HEALTH) {
                    // Phase 1: Aggressive chase with occasional pauses
                    if (enemy->frame_timer <= 0) {
                        // Chase phase
                        if (distance_to_player > 0) {
                            enemy->dx = (dx_boss / distance_to_player) * (ENEMY_CHASE_SPEED + 1.0f);
                            enemy->dy = (dy_boss / distance_to_player) * (ENEMY_CHASE_SPEED + 1.0f);
                        }
                        enemy->x += enemy->dx;
                        enemy->y += enemy->dy;
                        
                        enemy->frame_timer = 120; // Chase for 2 seconds
                    } else if (enemy->frame_timer == 60) {
                        // Brief pause in the middle
                        enemy->dx = 0;
                        enemy->dy = 0;
                    }
                } else {
                    // Phase 2: Desperate behavior - faster movement and shooting
                    if (distance_to_player > 100.0f) {
                        // Fast approach
                        if (distance_to_player > 0) {
                            enemy->dx = (dx_boss / distance_to_player) * (ENEMY_CHASE_SPEED + 2.0f);
                            enemy->dy = (dy_boss / distance_to_player) * (ENEMY_CHASE_SPEED + 2.0f);
                        }
                        enemy->x += enemy->dx;
                        enemy->y += enemy->dy;
                    } else {
                        // Close combat - circular movement
                        float angle = enemy->frame_timer * 0.1f;
                        enemy->dx = cos(angle) * 3.0f;
                        enemy->dy = sin(angle) * 2.0f;
                        enemy->x += enemy->dx;
                        enemy->y += enemy->dy;
                    }
                    
                    // Rapid fire in phase 2
                    if (enemy->last_attack <= 0 && distance_to_player <= ENEMY_SHOOT_RANGE) {
                        create_projectile(game->current_level_data, 
                                        enemy->x + enemy->width/2, 
                                        enemy->y + enemy->height/2,
                                        game->player.x + game->player.width/2, 
                                        game->player.y + game->player.height/2,
                                        enemy->type);
                        // Play enemy shooting sound with higher pitch for rapid fire
                        al_play_sample(game->shoot_sound, 0.7, 0.0, 1.3, ALLEGRO_PLAYMODE_ONCE, NULL);
                        enemy->last_attack = ENEMY_SHOOT_COOLDOWN / 2; // Faster shooting
                        printf("Boss rapid fire!\n"); // Debug output
                    }
                }
                
                // Update timers
                if (enemy->frame_timer > 0) enemy->frame_timer--;
                if (enemy->last_attack > 0) enemy->last_attack--;
            }
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
                
                // Create screen shake effect for enemy damage
                create_screen_shake(game, 2.0f, 10);
                
                // Play hit sound if enabled
                if (game->settings.sound_enabled && game->hit_sound) {
                    al_play_sample(game->hit_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                
                game->player.last_attack = PLAYER_INVINCIBILITY_FRAMES; // Invincibility frames
                
                if (game->player.health <= 0) {
                    // Play death sound if enabled
                    if (game->settings.sound_enabled && game->death_sound) {
                        al_play_sample(game->death_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    }
                    game->state = GAME_OVER;
                }
            }
        }
    }
    
    if (game->player.last_attack > 0) {
        game->player.last_attack--;
    }
}
