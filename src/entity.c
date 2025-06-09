#include "../include/entity.h"
#include "../include/game.h" // For Game, Level, Platform, Entity types
#include <math.h> // For sqrt
#include <stdio.h> // For printf in case of debugging, can be removed later

// Original update_enemy function from main.c
void update_enemy(Entity* enemy, Game* game) {
    // Handle knockback effects first
    if (enemy->knockback_timer > 0) {
        enemy->x += enemy->knockback_dx;
        enemy->y += enemy->knockback_dy;
        enemy->knockback_timer--;
        
        // Reduce knockback force over time
        enemy->knockback_dx *= 0.8f;
        enemy->knockback_dy *= 0.8f;
        
        if (enemy->knockback_timer <= 0) {
            enemy->knockback_dx = 0.0f;
            enemy->knockback_dy = 0.0f;
        }
        return; // Don't process normal AI while being knocked back
    }
    
    // Update combo timer
    if (enemy->combo_timer > 0) {
        enemy->combo_timer--;
        if (enemy->combo_timer <= 0) {
            enemy->combo_count = 0;
        }
    }
    
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
                // Complex boss behavior with multiple phases and special abilities
                float dx_boss = game->player.x - enemy->x;
                float dy_boss = game->player.y - enemy->y;
                float distance_to_player = sqrt(dx_boss * dx_boss + dy_boss * dy_boss);
                float health_percentage = enemy->health / enemy->max_health;
                
                if (health_percentage > ENEMY_BOSS_PHASE_HEALTH) {
                    // Phase 1: Aggressive chase with occasional pauses and special attacks
                    if (enemy->frame_timer <= 0) {
                        // Chase phase with enhanced speed
                        if (distance_to_player > 0) {
                            enemy->dx = (dx_boss / distance_to_player) * (ENEMY_CHASE_SPEED + 2.0f);
                            enemy->dy = (dy_boss / distance_to_player) * (ENEMY_CHASE_SPEED + 2.0f);
                        }
                        enemy->x += enemy->dx;
                        enemy->y += enemy->dy;
                        
                        enemy->frame_timer = 90; // Chase for 1.5 seconds
                    } else if (enemy->frame_timer == 45) {
                        // Brief pause with area attack
                        enemy->dx = 0;
                        enemy->dy = 0;
                        
                        // Boss special: triple shot spread
                        if (enemy->last_attack <= 0) {
                            for (int shot = 0; shot < 3; shot++) {
                                float angle_offset = (shot - 1) * 0.3f; // -0.3, 0, 0.3 radians
                                float shot_target_x = game->player.x + cos(angle_offset) * 100.0f;
                                float shot_target_y = game->player.y + sin(angle_offset) * 100.0f;
                                
                                create_projectile(game->current_level_data, 
                                                enemy->x + enemy->width/2, 
                                                enemy->y + enemy->height/2,
                                                shot_target_x, shot_target_y,
                                                enemy->type);
                            }
                            if (game->settings.sound_enabled && game->shoot_sound) {
                                al_play_sample(game->shoot_sound, 0.8, 0.0, 0.8, ALLEGRO_PLAYMODE_ONCE, NULL);
                            }
                            enemy->last_attack = ENEMY_SHOOT_COOLDOWN;
                            printf("Boss triple shot attack!\n");
                        }
                    }
                } else {
                    // Phase 2: Desperate behavior - enhanced abilities and faster movement
                    if (distance_to_player > 120.0f) {
                        // Very fast approach with prediction
                        float pred_x, pred_y;
                        predict_player_movement(game, 30, &pred_x, &pred_y);
                        
                        float pred_dx = pred_x - enemy->x;
                        float pred_dy = pred_y - enemy->y;
                        float pred_distance = sqrt(pred_dx * pred_dx + pred_dy * pred_dy);
                        
                        if (pred_distance > 0) {
                            enemy->dx = (pred_dx / pred_distance) * (ENEMY_CHASE_SPEED + 3.0f);
                            enemy->dy = (pred_dy / pred_distance) * (ENEMY_CHASE_SPEED + 3.0f);
                        }
                        enemy->x += enemy->dx;
                        enemy->y += enemy->dy;
                    } else {
                        // Close combat - erratic movement pattern
                        float angle = enemy->frame_timer * 0.15f;
                        enemy->dx = cos(angle) * 4.0f + cos(angle * 2.3f) * 2.0f; // Complex pattern
                        enemy->dy = sin(angle) * 3.0f + sin(angle * 1.7f) * 1.5f;
                        enemy->x += enemy->dx;
                        enemy->y += enemy->dy;
                    }
                    
                    // Enhanced rapid fire in phase 2
                    if (enemy->last_attack <= 0 && distance_to_player <= ENEMY_SHOOT_RANGE * 1.5f) {
                        // Boss special: burst fire
                        for (int burst = 0; burst < 2; burst++) {
                            create_projectile(game->current_level_data, 
                                            enemy->x + enemy->width/2, 
                                            enemy->y + enemy->height/2,
                                            game->player.x + game->player.width/2 + (burst * 20 - 10), 
                                            game->player.y + game->player.height/2,
                                            enemy->type);
                        }
                        // Play enemy shooting sound with higher pitch for rapid fire
                        if (game->settings.sound_enabled && game->shoot_sound) {
                            al_play_sample(game->shoot_sound, 0.7, 0.0, 1.3, ALLEGRO_PLAYMODE_ONCE, NULL);
                        }
                        enemy->last_attack = ENEMY_SHOOT_COOLDOWN / 3; // Much faster shooting
                        printf("Boss burst fire!\n");
                    }
                }
                
                // Update timers
                if (enemy->frame_timer > 0) enemy->frame_timer--;
                if (enemy->last_attack > 0) enemy->last_attack--;
            }
            break;
        case BEHAVIOR_NONE:
            break;
        case BEHAVIOR_FLANK:
            apply_flanking_behavior(enemy, &game->player, game);
            break;
        case BEHAVIOR_COORDINATE:
            execute_coordinate_behavior(game, enemy);
            break;
        case BEHAVIOR_AMBUSH:
            execute_ambush_behavior(game, enemy);
            break;
        case BEHAVIOR_RETREAT:
            execute_retreat_behavior(game, enemy);
            break;
        case BEHAVIOR_SURROUND:
            execute_surround_behavior(game, enemy);
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

// Advanced AI behavior implementations

// Check if there's a clear line of sight between two points
bool has_line_of_sight(Game* game, float x1, float y1, float x2, float y2) {
    // Simple line of sight check - can be enhanced with actual collision detection
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance == 0) return true;
    
    // Sample points along the line to check for collisions with platforms
    int steps = (int)(distance / 10); // Check every 10 units
    if (steps < 2) steps = 2;
    
    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;
        float check_x = x1 + dx * t;
        float check_y = y1 + dy * t;
        
        // Check collision with platforms
        for (int j = 0; j < game->current_level_data->num_platforms; j++) {
            Platform* platform = &game->current_level_data->platforms[j];
            if (check_x >= platform->x && check_x <= platform->x + platform->width &&
                check_y >= platform->y && check_y <= platform->y + platform->height) {
                return false;
            }
        }
    }
    
    return true;
}

// Predict where the player will be based on their current velocity
void predict_player_movement(Game* game, float prediction_time, float* pred_x, float* pred_y) {
    *pred_x = game->player.x + game->player.dx * prediction_time;
    *pred_y = game->player.y + game->player.dy * prediction_time;
    
    // Clamp to level boundaries
    if (*pred_x < 0) *pred_x = 0;
    if (*pred_x > game->current_level_data->level_width - game->player.width) {
        *pred_x = game->current_level_data->level_width - game->player.width;
    }
    if (*pred_y < 0) *pred_y = 0;
    if (*pred_y > game->current_level_data->level_height - game->player.height) {
        *pred_y = game->current_level_data->level_height - game->player.height;
    }
}

// Find the best flanking position around the player
void calculate_flank_position(Game* game, Entity* enemy, float* target_x, float* target_y) {
    float player_center_x = game->player.x + game->player.width / 2;
    float player_center_y = game->player.y + game->player.height / 2;
    
    // Try to position enemy at a flanking angle
    float flank_distance = AI_FLANK_DISTANCE;
    float flank_angle = AI_FLANK_ANGLE + (enemy->ai_timer * 0.02f); // Slowly rotate flank position
    
    // Determine which side to flank based on enemy position
    float enemy_center_x = enemy->x + enemy->width / 2;
    if (enemy_center_x < player_center_x) {
        flank_angle = -flank_angle; // Flank from left
    }
    
    *target_x = player_center_x + cos(flank_angle) * flank_distance - enemy->width / 2;
    *target_y = player_center_y + sin(flank_angle) * flank_distance - enemy->height / 2;
    
    // Ensure target position is within level bounds
    if (*target_x < 0) *target_x = 0;
    if (*target_x > game->current_level_data->level_width - enemy->width) {
        *target_x = game->current_level_data->level_width - enemy->width;
    }
    if (*target_y < 0) *target_y = 0;
    if (*target_y > game->current_level_data->level_height - enemy->height) {
        *target_y = game->current_level_data->level_height - enemy->height;
    }
}

// Check if an enemy should enter ambush mode
bool should_enter_ambush(Game* game, Entity* enemy) {
    float dx = game->player.x - enemy->x;
    float dy = game->player.y - enemy->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    // Enter ambush if player is approaching and within ambush range
    return (distance <= AI_AMBUSH_RANGE && distance > AI_AMBUSH_TRIGGER_RANGE &&
            has_line_of_sight(game, enemy->x, enemy->y, game->player.x, game->player.y));
}

// Check if an enemy should retreat
bool should_retreat(Game* game, Entity* enemy) {
    float health_percentage = enemy->health / enemy->max_health;
    float dx = game->player.x - enemy->x;
    float dy = game->player.y - enemy->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    // Retreat if low health or overwhelmed
    return (health_percentage < AI_RETREAT_HEALTH_THRESHOLD || 
            (distance < AI_RETREAT_DISTANCE && enemy->last_damage_time < AI_RETREAT_PANIC_TIME));
}

// Update global AI coordination state
void update_ai_coordination(Game* game) {
    game->ai_state.coordination_timer++;
    
    // Reset coordination every few seconds
    if (game->ai_state.coordination_timer >= AI_COORDINATION_UPDATE_RATE) {
        game->ai_state.coordination_timer = 0;
        game->ai_state.active_coordinators = 0;
        
        // Count active enemies for coordination
        for (int i = 0; i < game->current_level_data->num_enemies; i++) {
            Entity* enemy = &game->current_level_data->enemies[i];
            if (enemy->active && enemy->coordination_id > 0) {
                game->ai_state.active_coordinators++;
            }
        }
    }
    
    // Update player tracking
    game->ai_state.player_last_x = game->player.x;
    game->ai_state.player_last_y = game->player.y;
    
    // Update difficulty adaptation
    if (game->ai_state.adaptation_timer > 0) {
        game->ai_state.adaptation_timer--;
    }
}

// Adapt AI difficulty based on player performance
void adapt_difficulty(Game* game) {
    if (game->ai_state.adaptation_timer > 0) return; // Don't adapt too frequently
    
    // Increase difficulty if player is doing well (earning stars or high combos)
    if (game->combo_count > 5 || game->total_stars > game->ai_state.last_stars_check + 2) {
        game->ai_state.difficulty_multiplier += AI_DIFFICULTY_STEP;
        if (game->ai_state.difficulty_multiplier > AI_MAX_DIFFICULTY) {
            game->ai_state.difficulty_multiplier = AI_MAX_DIFFICULTY;
        }
    }
    // Decrease difficulty if player is struggling
    else if (game->player.health < game->player.max_health * 0.3f) {
        game->ai_state.difficulty_multiplier -= AI_DIFFICULTY_STEP;
        if (game->ai_state.difficulty_multiplier < AI_MIN_DIFFICULTY) {
            game->ai_state.difficulty_multiplier = AI_MIN_DIFFICULTY;
        }
    }
    
    game->ai_state.last_stars_check = game->total_stars;
    game->ai_state.adaptation_timer = AI_ADAPTATION_RATE;
}

// Execute flanking behavior
void execute_flank_behavior(Game* game, Entity* enemy) {
    float target_x, target_y;
    calculate_flank_position(game, enemy, &target_x, &target_y);
    
    // Move towards flanking position
    float dx = target_x - enemy->x;
    float dy = target_y - enemy->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance > AI_FLANK_MIN_DISTANCE) {
        float speed = ENEMY_CHASE_SPEED * game->ai_state.difficulty_multiplier;
        enemy->dx = (dx / distance) * speed;
        enemy->dy = (dy / distance) * speed;
        enemy->x += enemy->dx;
        enemy->y += enemy->dy;
    } else {
        // In position, switch to coordinated attack
        enemy->behavior = BEHAVIOR_COORDINATE;
        enemy->ai_timer = AI_COORDINATE_DURATION;
    }
    
    enemy->ai_timer++;
}

// Apply flanking behavior - implementation matching the declared signature
void apply_flanking_behavior(Entity* enemy, Entity* player, Game* game) {
    float target_x, target_y;
    calculate_flank_position(game, enemy, &target_x, &target_y);
    
    // Move towards flanking position
    float dx = target_x - enemy->x;
    float dy = target_y - enemy->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance > AI_FLANK_MIN_DISTANCE) {
        float speed = ENEMY_CHASE_SPEED * game->ai_state.difficulty_multiplier;
        enemy->dx = (dx / distance) * speed;
        enemy->dy = (dy / distance) * speed;
        enemy->x += enemy->dx;
        enemy->y += enemy->dy;
    } else {
        // In position, switch to coordinated attack
        enemy->behavior = BEHAVIOR_COORDINATE;
        enemy->ai_timer = AI_COORDINATE_DURATION;
    }
    
    enemy->ai_timer++;
}

// Stub implementations for missing AI behavior functions
void execute_coordinate_behavior(Game* game, Entity* enemy) {
    // Basic coordination behavior - move towards player with slight flanking
    float dx = game->player.x - enemy->x;
    float dy = game->player.y - enemy->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance > 0) {
        enemy->dx = (dx / distance) * ENEMY_CHASE_SPEED;
        enemy->dy = (dy / distance) * ENEMY_CHASE_SPEED;
        enemy->x += enemy->dx;
        enemy->y += enemy->dy;
    }
}

void execute_ambush_behavior(Game* game, Entity* enemy) {
    // Basic ambush behavior - wait then rush player
    if (enemy->ai_timer > 0) {
        enemy->ai_timer--;
        // Wait in position
        enemy->dx = 0;
        enemy->dy = 0;
    } else {
        // Rush towards player
        float dx = game->player.x - enemy->x;
        float dy = game->player.y - enemy->y;
        float distance = sqrt(dx * dx + dy * dy);
        
        if (distance > 0) {
            enemy->dx = (dx / distance) * (ENEMY_CHASE_SPEED * 1.5f);
            enemy->dy = (dy / distance) * (ENEMY_CHASE_SPEED * 1.5f);
            enemy->x += enemy->dx;
            enemy->y += enemy->dy;
        }
    }
}

void execute_retreat_behavior(Game* game, Entity* enemy) {
    // Basic retreat behavior - move away from player
    float dx = game->player.x - enemy->x;
    float dy = game->player.y - enemy->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance > 0) {
        // Move away from player
        enemy->dx = -(dx / distance) * ENEMY_CHASE_SPEED;
        enemy->dy = -(dy / distance) * ENEMY_CHASE_SPEED;
        enemy->x += enemy->dx;
        enemy->y += enemy->dy;
    }
}

void execute_surround_behavior(Game* game, Entity* enemy) {
    // Basic surround behavior - circle around player
    float dx = game->player.x - enemy->x;
    float dy = game->player.y - enemy->y;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance > AI_FLANK_DISTANCE) {
        // Move closer
        if (distance > 0) {
            enemy->dx = (dx / distance) * ENEMY_CHASE_SPEED;
            enemy->dy = (dy / distance) * ENEMY_CHASE_SPEED;
        }
    } else {
        // Circle around player
        float angle = atan2(dy, dx) + (AI_SURROUND_ANGLE_OFFSET * 3.14159f / 180.0f);
        enemy->dx = cos(angle) * ENEMY_PATROL_SPEED;
        enemy->dy = sin(angle) * ENEMY_PATROL_SPEED;
    }
    
    enemy->x += enemy->dx;
    enemy->y += enemy->dy;
}
