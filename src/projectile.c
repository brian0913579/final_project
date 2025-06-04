#include "../include/game.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Create a new projectile
void create_projectile(Level* level, float x, float y, float target_x, float target_y, EntityType source) {
    if (!level || !level->projectiles) return;
    
    // Find an inactive projectile slot
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!level->projectiles[i].active) {
            Projectile* proj = &level->projectiles[i];
            
            // Set position
            proj->x = x;
            proj->y = y;
            proj->width = PROJECTILE_WIDTH;
            proj->height = PROJECTILE_HEIGHT;
            
            // Calculate direction and velocity
            float dx = target_x - x;
            float dy = target_y - y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance > 0) {
                proj->dx = (dx / distance) * PROJECTILE_SPEED;
                proj->dy = (dy / distance) * PROJECTILE_SPEED;
            } else {
                // Default direction if target is exactly at source position
                proj->dx = PROJECTILE_SPEED;
                proj->dy = 0;
            }
            
            // Set projectile properties
            proj->active = true;
            proj->lifetime = PROJECTILE_LIFETIME;
            proj->damage = PROJECTILE_DAMAGE;
            proj->source = source;
            
            // Increment active projectile count
            level->num_projectiles++;
            
            printf("Created projectile from %.0f,%.0f to %.0f,%.0f\n", x, y, target_x, target_y);
            return;
        }
    }
    
    printf("Warning: No available projectile slots\n");
}

// Create a player projectile with direct velocity (for directional shooting)
void create_player_projectile(Level* level, float x, float y, float dx, float dy) {
    if (!level || !level->projectiles) return;
    
    // Find an inactive projectile slot
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!level->projectiles[i].active) {
            Projectile* proj = &level->projectiles[i];
            
            // Set position
            proj->x = x;
            proj->y = y;
            proj->width = PROJECTILE_WIDTH;
            proj->height = PROJECTILE_HEIGHT;
            
            // Set velocity directly
            proj->dx = dx;
            proj->dy = dy;
            
            // Set projectile properties
            proj->active = true;
            proj->lifetime = PROJECTILE_LIFETIME;
            proj->damage = PLAYER_PROJECTILE_DAMAGE;
            proj->source = CANCER_CELL; // Player is cancer cell
            
            // Increment active projectile count
            level->num_projectiles++;
            
            printf("Created player projectile at %.0f,%.0f with velocity %.1f,%.1f\n", x, y, dx, dy);
            return;
        }
    }
    
    printf("Warning: No available projectile slots for player\n");
}

// Update all projectiles
void update_projectiles(Level* level, Game* game) {
    if (!level || !level->projectiles) return;
    
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile* proj = &level->projectiles[i];
        
        if (!proj->active) continue;
        
        // Update position
        proj->x += proj->dx;
        proj->y += proj->dy;
        
        // Create trail effect for moving projectiles
        create_projectile_trail(level, proj->x + proj->width/2, proj->y + proj->height/2, proj->source);
        
        // Decrease lifetime
        proj->lifetime--;
        
        // Check if projectile should be destroyed
        bool should_destroy = false;
        
        // Destroy if lifetime expired
        if (proj->lifetime <= 0) {
            should_destroy = true;
        }
        
        // Destroy if out of level bounds
        if (proj->x < -100 || proj->x > level->level_width + 100 || 
            proj->y < -100 || proj->y > SCREEN_HEIGHT + 100) {
            should_destroy = true;
        }
        
        // Check collision with platforms
        for (int p = 0; p < level->num_platforms; p++) {
            Platform* platform = &level->platforms[p];
            
            if (proj->x < platform->x + platform->width &&
                proj->x + proj->width > platform->x &&
                proj->y < platform->y + platform->height &&
                proj->y + proj->height > platform->y) {
                
                // Create impact particle effect
                create_particle_burst(level, proj->x + proj->width/2, proj->y + proj->height/2, 
                                    al_map_rgb(200, 200, 200), 5);
                should_destroy = true;
                break;
            }
        }
        
        // Destroy projectile if needed
        if (should_destroy) {
            proj->active = false;
            level->num_projectiles--;
        }
    }
}

// Check projectile collisions with player
void check_projectile_collisions(Level* level, Game* game) {
    if (!level || !level->projectiles || !game) return;
    
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile* proj = &level->projectiles[i];
        
        if (!proj->active) continue;
        
        // Check player projectiles hitting enemies
        if (proj->source == CANCER_CELL) {
            // Player projectile - check collision with enemies
            for (int e = 0; e < level->num_enemies; e++) {
                Entity* enemy = &level->enemies[e];
                if (!enemy->active) continue;
                
                if (proj->x < enemy->x + enemy->width &&
                    proj->x + proj->width > enemy->x &&
                    proj->y < enemy->y + enemy->height &&
                    proj->y + proj->height > enemy->y) {
                    
                    // Create hit particle effect
                    create_particle_burst(level, proj->x + proj->width/2, proj->y + proj->height/2, 
                                        al_map_rgb(255, 255, 0), 6);
                    
                    // Enemy takes damage
                    enemy->health -= proj->damage;
                    game->score += 15; // Score for projectile hit
                    
                    // Check if enemy is defeated
                    if (enemy->health <= 0) {
                        // Create spectacular death effect
                        create_enemy_death_effect(level, enemy->x + enemy->width/2, enemy->y + enemy->height/2, enemy->type);
                        enemy->active = false;
                        game->score += 150; // Bonus for defeating enemy with projectile
                        printf("Enemy defeated by player projectile! Score: %d\n", game->score);
                    }
                    
                    // Destroy projectile
                    proj->active = false;
                    level->num_projectiles--;
                    
                    printf("Player projectile hit enemy! Enemy health: %.0f\n", enemy->health);
                    break; // Exit enemy loop since projectile is destroyed
                }
            }
        } else {
            // Enemy projectile - check collision with player
            if (proj->x < game->player.x + game->player.width &&
                proj->x + proj->width > game->player.x &&
                proj->y < game->player.y + game->player.height &&
                proj->y + proj->height > game->player.y) {
                
                // Create hit particle effect
                create_particle_burst(level, proj->x + proj->width/2, proj->y + proj->height/2, 
                                    al_map_rgb(255, 0, 0), 8);
                
                // Player takes damage
                game->player.health -= proj->damage;
                
                // Create screen shake effect
                create_screen_shake(game, 3.0f, 15);
                
                // Play hit sound if enabled
                if (game->settings.sound_enabled && game->hit_sound) {
                    al_play_sample(game->hit_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                
                // Destroy projectile
                proj->active = false;
                level->num_projectiles--;
                
                printf("Player hit by projectile! Health: %.0f\n", game->player.health);
                
                // Check if player died
                if (game->player.health <= 0) {
                    game->state = GAME_OVER;
                    
                    // Play death sound if enabled
                    if (game->settings.sound_enabled && game->death_sound) {
                        al_play_sample(game->death_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    }
                    
                    printf("Player died from projectile damage!\n");
                }
            }
        }
    }
}

// Create a burst of particles for visual effects
void create_particle_burst(Level* level, float x, float y, ALLEGRO_COLOR color, int count) {
    if (!level || !level->particles) return;
    
    int particles_created = 0;
    for (int i = 0; i < MAX_PARTICLES && particles_created < count; i++) {
        if (!level->particles[i].active) {
            Particle* p = &level->particles[i];
            
            // Random velocity for burst effect
            float angle = (rand() % 360) * M_PI / 180.0f;
            float speed = 1.0f + (rand() % 3); // Random speed 1-3
            
            p->x = x;
            p->y = y;
            p->dx = cos(angle) * speed;
            p->dy = sin(angle) * speed - 1.0f; // Slight upward bias
            p->color = color;
            p->lifetime = PARTICLE_LIFETIME_SHORT + (rand() % PARTICLE_LIFETIME_SHORT);
            p->max_lifetime = p->lifetime;
            p->active = true;
            
            level->num_particles++;
            particles_created++;
        }
    }
}

// Create spectacular death effect for enemies
void create_enemy_death_effect(Level* level, float x, float y, EntityType enemy_type) {
    if (!level || !level->particles) return;
    
    // Choose colors based on enemy type
    ALLEGRO_COLOR primary_color, secondary_color;
    switch (enemy_type) {
        case T_CELL:
            primary_color = COLOR_YELLOW;
            secondary_color = al_map_rgb(255, 255, 150);
            break;
        case MACROPHAGE:
            primary_color = al_map_rgb(200, 200, 0);
            secondary_color = al_map_rgb(255, 255, 100);
            break;
        case B_CELL:
            primary_color = al_map_rgb(0, 200, 255);
            secondary_color = al_map_rgb(150, 225, 255);
            break;
        case NK_CELL:
            primary_color = COLOR_RED;
            secondary_color = al_map_rgb(255, 150, 150);
            break;
        default:
            primary_color = COLOR_WHITE;
            secondary_color = COLOR_LIGHT_GRAY;
    }
    
    int particles_created = 0;
    for (int i = 0; i < MAX_PARTICLES && particles_created < ENEMY_DEATH_PARTICLES; i++) {
        if (!level->particles[i].active) {
            Particle* p = &level->particles[i];
            
            // Create explosion-like effect
            float angle = (rand() % 360) * M_PI / 180.0f;
            float speed = 2.0f + (rand() % 4); // Random speed 2-5
            
            p->x = x;
            p->y = y;
            p->dx = cos(angle) * speed;
            p->dy = sin(angle) * speed - 0.5f; // Slight upward bias
            
            // Alternate between primary and secondary colors
            p->color = (particles_created % 2 == 0) ? primary_color : secondary_color;
            p->lifetime = PARTICLE_LIFETIME_LONG + (rand() % PARTICLE_LIFETIME_MEDIUM);
            p->max_lifetime = p->lifetime;
            p->active = true;
            
            level->num_particles++;
            particles_created++;
        }
    }
}

// Create subtle trail effect for projectiles
void create_projectile_trail(Level* level, float x, float y, EntityType source) {
    if (!level || !level->particles) return;
    
    // Only create trail occasionally to avoid overwhelming the screen
    if (rand() % 3 != 0) return;
    
    ALLEGRO_COLOR trail_color;
    if (source == CANCER_CELL) {
        trail_color = al_map_rgba(0, 255, 255, 150); // Semi-transparent cyan for player
    } else {
        trail_color = al_map_rgba(255, 100, 100, 120); // Semi-transparent red for enemies
    }
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!level->particles[i].active) {
            Particle* p = &level->particles[i];
            
            p->x = x + (rand() % 6) - 3; // Small random offset
            p->y = y + (rand() % 6) - 3;
            p->dx = 0;
            p->dy = 0.5f; // Gentle downward drift
            p->color = trail_color;
            p->lifetime = PARTICLE_LIFETIME_SHORT;
            p->max_lifetime = p->lifetime;
            p->active = true;
            
            level->num_particles++;
            break; // Only create one trail particle per call
        }
    }
}

// Update all particles
void update_particles(Level* level) {
    if (!level || !level->particles) return;
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &level->particles[i];
        
        if (!p->active) continue;
        
        // Update position
        p->x += p->dx;
        p->y += p->dy;
        
        // Apply gravity to particles
        p->dy += 0.1f;
        
        // Fade color based on lifetime
        float alpha = (float)p->lifetime / (float)p->max_lifetime;
        p->color.a = alpha;
        
        // Decrease lifetime
        p->lifetime--;
        
        // Destroy particle if lifetime expired
        if (p->lifetime <= 0) {
            p->active = false;
            level->num_particles--;
        }
    }
}
