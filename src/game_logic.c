#include "../include/game_logic.h"
#include "../include/game.h"      // For Game struct, constants, Allegro headers
#include "../include/level.h"      // For init_levels, cleanup_levels
#include "../include/input.h"      // For handle_input (though not directly called by these funcs)
#include "../include/drawing.h"    // For draw_game (though not directly called by these funcs)
#include "../include/entity.h"     // For update_enemy, handle_collisions
#include <stdio.h>               // For fprintf, sprintf
#include <stdlib.h>              // For malloc, free
#include <allegro5/allegro.h>
#include <allegro5/path.h> // For ALLEGRO_PATH, al_get_standard_path, al_set_path_filename, al_path_cstr, al_change_directory, al_destroy_path
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

// Original init_game function from main.c
bool init_game(Game* game) {
    if (!al_init()) {
        fprintf(stderr, "Failed to initialize Allegro!\n");
        return false;
    }

    // Initialize other Allegro addons and game components as before
    al_init_primitives_addon();
    al_install_keyboard();
    al_install_mouse();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(10); // Reserve some samples

    game->timer = al_create_timer(1.0 / FPS);
    if (!game->timer) {
        fprintf(stderr, "Failed to create timer!\n");
        return false;
    }

    game->event_queue = al_create_event_queue();
    if (!game->event_queue) {
        fprintf(stderr, "Failed to create event_queue!\n");
        al_destroy_timer(game->timer);
        return false;
    }

    game->display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!game->display) {
        fprintf(stderr, "Failed to create display!\n");
        al_destroy_timer(game->timer);
        al_destroy_event_queue(game->event_queue);
        return false;
    }

    // Try to set the working directory to the resources directory
    ALLEGRO_PATH* resources_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
    if (resources_path) {
        const char* dir_cstr = al_path_cstr(resources_path, ALLEGRO_NATIVE_PATH_SEP);
        if (al_change_directory(dir_cstr)) {
        } else {
            fprintf(stderr, "Failed to change working directory to: %s\nAttempting to use executable's directory as fallback.\n", dir_cstr);
            // Fallback: try to set CWD to executable's directory
            ALLEGRO_PATH* exe_path = al_get_standard_path(ALLEGRO_EXENAME_PATH);
            if (exe_path) {
                al_drop_path_tail(exe_path); // Remove filename to get directory
                const char* exe_dir_cstr = al_path_cstr(exe_path, ALLEGRO_NATIVE_PATH_SEP);
                if (al_change_directory(exe_dir_cstr)) {
                } else {
                    fprintf(stderr, "Failed to change working directory to exe path: %s\n", exe_dir_cstr);
                }
                al_destroy_path(exe_path);
            }
        }
        al_destroy_path(resources_path);
    } else {
        fprintf(stderr, "Failed to get standard resources path.\n");
    }

    game->font = al_load_ttf_font(DEFAULT_FONT_PATH, FONT_SIZE_NORMAL, 0);
    game->title_font = al_load_ttf_font(DEFAULT_FONT_PATH, FONT_SIZE_TITLE, 0);
    if (!game->font || !game->title_font) {
        fprintf(stderr, "Failed to load fonts!\n");
        // al_shutdown_font_addon(); // Consider cleanup on failure
        return false;
    }

    game->timer = al_create_timer(1.0 / FPS);
    if (!game->timer) {
        fprintf(stderr, "Failed to create timer!\n");
        return false;
    }

    game->display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!game->display) {
        fprintf(stderr, "Failed to create display!\n");
        // al_destroy_timer(game->timer); // Consider cleanup
        return false;
    }

    game->event_queue = al_create_event_queue();
    if (!game->event_queue) {
        fprintf(stderr, "Failed to create event queue!\n");
        // al_destroy_display(game->display); // Consider cleanup
        // al_destroy_timer(game->timer);
        return false;
    }

    al_register_event_source(game->event_queue, al_get_display_event_source(game->display));
    al_register_event_source(game->event_queue, al_get_timer_event_source(game->timer));
    al_register_event_source(game->event_queue, al_get_keyboard_event_source());

    if (!al_install_audio()) {
        fprintf(stderr, "Failed to initialize audio!\n");
        return false;
    }
    if (!al_init_acodec_addon()) {
        fprintf(stderr, "Failed to initialize audio codecs!\n");
        return false;
    }
    if (!al_reserve_samples(AUDIO_RESERVE_SAMPLES)) {
        fprintf(stderr, "Failed to reserve audio samples!\n");
        return false;
    }
    // Load sound effects
    game->jump_sound = al_load_sample("resources/sounds/jump.wav");
    game->hit_sound = al_load_sample("resources/sounds/hit.wav");
    game->death_sound = al_load_sample("resources/sounds/death.wav");
    game->collect_sound = al_load_sample("resources/sounds/collect.wav");
    game->shoot_sound = al_load_sample("resources/sounds/shoot.wav");
    
    // For now, no background music to keep it simple
    game->music_instance = NULL;
    
    if (!game->jump_sound) {
        fprintf(stderr, "Warning: Failed to load jump.wav\n");
    }
    if (!game->hit_sound) {
        fprintf(stderr, "Warning: Failed to load hit.wav\n"); 
    }
    if (!game->death_sound) {
        fprintf(stderr, "Warning: Failed to load death.wav\n");
    }
    if (!game->collect_sound) {
        fprintf(stderr, "Warning: Failed to load collect.wav\n");
    }
    if (!game->shoot_sound) {
        fprintf(stderr, "Warning: Failed to load shoot.wav\n");
    }

    game->player.x = SCREEN_WIDTH * PLAYER_INITIAL_X_FACTOR;
    game->player.y = SCREEN_HEIGHT * PLAYER_INITIAL_Y_FACTOR;
    game->player.width = PLAYER_WIDTH;
    game->player.height = PLAYER_HEIGHT;
    game->player.dx = 0;
    game->player.dy = 0;
    game->player.active = true;
    game->player.type = CANCER_CELL;
    game->player.state = IDLE;
    game->player.behavior = BEHAVIOR_NONE;
    
    // Adjust player stats based on difficulty
    switch (game->settings.difficulty) {
        case DIFFICULTY_EASY:
            game->player.health = PLAYER_INITIAL_HEALTH + 50;
            game->player.max_health = PLAYER_INITIAL_MAX_HEALTH + 50;
            game->player.attack_power = PLAYER_INITIAL_ATTACK_POWER + 5;
            break;
        case DIFFICULTY_HARD:
            game->player.health = PLAYER_INITIAL_HEALTH - 25;
            game->player.max_health = PLAYER_INITIAL_MAX_HEALTH - 25;
            game->player.attack_power = PLAYER_INITIAL_ATTACK_POWER - 2;
            break;
        default: // DIFFICULTY_NORMAL
            game->player.health = PLAYER_INITIAL_HEALTH;
            game->player.max_health = PLAYER_INITIAL_MAX_HEALTH;
            game->player.attack_power = PLAYER_INITIAL_ATTACK_POWER;
            break;
    }
    
    game->player.attack_speed = PLAYER_ATTACK_SPEED;
    game->player.last_attack = 0;
    game->player.last_shot = 0; // Initialize shooting cooldown
    game->player.sprite = NULL;
    game->player.sprite_sheet = NULL;
    game->player.current_frame = 0;
    game->player.frame_timer = 0;
    game->player.is_on_ground = false;
    game->player.jump_requested = false; // Initialize jump_requested

    game->state = WELCOME_SCREEN;
    game->running = true;
    game->score = 0;
    game->current_level = INITIAL_LEVEL; // Start at level 1

    // Initialize screen shake
    game->screen_shake.intensity = 0;
    game->screen_shake.duration = 0;
    game->screen_shake.offset_x = 0;
    game->screen_shake.offset_y = 0;

    // It's important that init_levels is called *before* reset_player_and_level
    // so that all level data (including original glucose states) is loaded first.
    init_menus(game);
    init_levels(game); 

    // Now, reset player and the current level to its initial state (including glucose items)
    reset_player_and_level(game, game->current_level); 

    al_start_timer(game->timer);
    return true;
}

// Original init_menus function from main.c
void init_menus(Game* game) {
    game->main_menu.num_items = 4;
    game->main_menu.items = malloc(sizeof(MenuItem) * game->main_menu.num_items);
    if(game->main_menu.items) { // Check malloc result
        game->main_menu.items[0] = (MenuItem){"Start Game", true, true};
        game->main_menu.items[1] = (MenuItem){"Level Select", true, true};
        game->main_menu.items[2] = (MenuItem){"Settings", true, true};
        game->main_menu.items[3] = (MenuItem){"Exit", true, true};
        game->main_menu.selected_index = 0;
    } else {
        fprintf(stderr, "Failed to allocate memory for main menu items!\n");
        game->main_menu.num_items = 0; // Prevent access if allocation failed
    }

    game->level_menu.num_items = 4;
    game->level_menu.items = malloc(sizeof(MenuItem) * game->level_menu.num_items);
    if(game->level_menu.items) {
        game->level_menu.items[0] = (MenuItem){"Level 1: The Blood Stream", true, true};
        game->level_menu.items[1] = (MenuItem){"Level 2: The Lymph Node", true, true}; // Initially enable for testing
        game->level_menu.items[2] = (MenuItem){"Level 3: The Final Battle", true, true}; // Initially enable for testing
        game->level_menu.items[3] = (MenuItem){"Back", true, true};
        game->level_menu.selected_index = 0;
    } else {
        fprintf(stderr, "Failed to allocate memory for level menu items!\n");
        game->level_menu.num_items = 0;
    }

    game->settings_menu.num_items = 4;
    game->settings_menu.items = malloc(sizeof(MenuItem) * game->settings_menu.num_items);
    if(game->settings_menu.items) {
        game->settings_menu.items[0] = (MenuItem){"Difficulty: Normal", true, true};
        game->settings_menu.items[1] = (MenuItem){"Sound: On", true, true};
        game->settings_menu.items[2] = (MenuItem){"Music: On", true, true};
        game->settings_menu.items[3] = (MenuItem){"Back", true, true};
        game->settings_menu.selected_index = 0;
    } else {
        fprintf(stderr, "Failed to allocate memory for settings menu items!\n");
        game->settings_menu.num_items = 0;
    }

    game->settings.difficulty = DIFFICULTY_NORMAL;
    game->settings.sound_enabled = true;
    game->settings.music_enabled = true;
}

// New reset_player_and_level function
void reset_player_and_level(Game* game, int level_idx) {
    // Reset player state
    game->player.x = SCREEN_WIDTH * PLAYER_INITIAL_X_FACTOR;
    game->player.y = SCREEN_HEIGHT * PLAYER_INITIAL_Y_FACTOR;
    game->player.dx = 0;
    game->player.dy = 0;
    game->player.health = PLAYER_INITIAL_HEALTH;
    game->player.is_on_ground = false;
    game->player.jump_requested = false;
    // Reset other player attributes as needed (e.g., score for the level if not global)

    // Ensure the level index is valid
    if (level_idx < 0 || level_idx >= game->num_levels) {
        fprintf(stderr, "Invalid level index for reset: %d\n", level_idx);
        level_idx = 0; // Default to first level
    }
    game->current_level = level_idx;
    game->current_level_data = &game->levels[level_idx];

    // Reload or reset the content of the current level to its original state
    // This includes reactivating all glucose items for that level.
    // We need to be careful if init_level_content reallocates memory.
    // A safer approach might be to have a separate reset_level_items function
    // or ensure init_level_content can be called multiple times safely.

    // For now, let's re-initialize the specific content for the current level.
    // This assumes init_level_content correctly sets up glucose items to active.
    // If init_level_content frees and reallocates, this is fine.
    // If it modifies in place, we need a dedicated reset for items.

    // Let's try re-running init_level_content for the specific level.
    // We need the original level_number (1, 2, or 3) not the index (0, 1, 2).
    // We added 'id' to the Level struct for this purpose.
    if (game->current_level_data) {
        // Before re-initializing content, free existing dynamic content within the level
        // to prevent memory leaks if init_level_content reallocates.
        if (game->current_level_data->platforms) free(game->current_level_data->platforms);
        game->current_level_data->platforms = NULL;
        game->current_level_data->num_platforms = 0;

        if (game->current_level_data->enemies) free(game->current_level_data->enemies);
        game->current_level_data->enemies = NULL;
        game->current_level_data->num_enemies = 0;

        if (game->current_level_data->glucose_items) free(game->current_level_data->glucose_items);
        game->current_level_data->glucose_items = NULL;
        game->current_level_data->num_glucose_items = 0;

        // Reset projectiles
        if (game->current_level_data->projectiles) {
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                game->current_level_data->projectiles[i].active = false;
            }
        }
        game->current_level_data->num_projectiles = 0;
        
        // Reset particles
        if (game->current_level_data->particles) {
            for (int i = 0; i < MAX_PARTICLES; i++) {
                game->current_level_data->particles[i].active = false;
            }
        }
        game->current_level_data->num_particles = 0;
        
        // Portal and background are handled by init_levels and init_level_content, 
        // but backgrounds are loaded once in init_levels. Portal is part of level struct.

        init_level_content(game->current_level_data, game->current_level_data->id);
    } else {
        fprintf(stderr, "Error: current_level_data is NULL during reset_player_and_level\n");
    }

    // Reset scroll position
    game->current_level_data->scroll_x = 0;
}

// Original update_game function from main.c
void update_game(Game* game) {
    if (game->state != PLAYING) {
        return;
    }

    // Process jump request
    if (game->player.jump_requested && game->player.is_on_ground) {
        game->player.dy = JUMP_SPEED;
        game->player.is_on_ground = false; // Prevent double jump in same frame
        
        // Play jump sound if enabled
        if (game->settings.sound_enabled && game->jump_sound) {
            al_play_sample(game->jump_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
    }
    game->player.jump_requested = false; // Consume the jump request
    
    game->player.x += game->player.dx;
    game->player.y += game->player.dy;
    
    game->player.dy += GRAVITY;
    game->player.is_on_ground = false;
    
    for (int i = 0; i < game->current_level_data->num_platforms; i++) {
        Platform* platform = &game->current_level_data->platforms[i];
        if (game->player.x < platform->x + platform->width &&
            game->player.x + game->player.width > platform->x &&
            game->player.y < platform->y + platform->height &&
            game->player.y + game->player.height > platform->y) {
            
            if (platform->is_deadly) {
                game->player.health -= DEADLY_PLATFORM_DAMAGE; 
                
                // Play hit sound if enabled
                if (game->settings.sound_enabled && game->hit_sound) {
                    al_play_sample(game->hit_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                
                if (game->player.health <= 0) {
                    // Play death sound if enabled
                    if (game->settings.sound_enabled && game->death_sound) {
                        al_play_sample(game->death_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    }
                    game->state = GAME_OVER;
                }
                // Land on deadly platform too
                game->player.y = platform->y - game->player.height;
                game->player.dy = 0;
                game->player.is_on_ground = true;
            } else {
                // Check vertical collision (landing on top or hitting bottom)
                if (game->player.dy >= 0 && // Moving downwards or still
                    game->player.y + game->player.height - game->player.dy <= platform->y + PLATFORM_JUMP_TOLERANCE && // Previous position was above or at platform top
                    game->player.y + game->player.height > platform->y) { // Current position is intersecting or below platform top
                    game->player.dy = 0;
                    game->player.y = platform->y - game->player.height;
                    game->player.is_on_ground = true;
                } else if (game->player.dy < 0 && // Moving upwards
                           game->player.y - game->player.dy >= platform->y + platform->height && // Previous position was below platform bottom
                           game->player.y < platform->y + platform->height) { // Current position is intersecting or above platform bottom
                    game->player.y = platform->y + platform->height;
                    game->player.dy = 0; // Stop upward movement
                }

                // Check horizontal collision (hitting sides), including step-up logic
                bool vertically_aligned_for_horizontal_check =
                    (game->player.y < platform->y + platform->height) &&
                    (game->player.y + game->player.height > platform->y);

                if (vertically_aligned_for_horizontal_check) {
                    // Moving right
                    if (game->player.dx > 0 && // Player is moving right
                        game->player.x + game->player.width - game->player.dx <= platform->x && // Previous right edge was left of or at platform's left edge
                        game->player.x + game->player.width > platform->x) { // Current right edge is past platform's left edge

                        float player_current_foot_y = game->player.y + game->player.height;
                        float step_height = player_current_foot_y - platform->y; // Height of the step relative to player's feet

                        // Condition for step-up:
                        // 1. Platform top is above player's current foot level (step_height > 0).
                        // 2. The step height is not too large (step_height <= MAX_STEP_UP_HEIGHT).
                        // 3. Player is on the ground (meaning they have support).
                        if (step_height > 0 && step_height <= MAX_STEP_UP_HEIGHT && game->player.is_on_ground) {
                            // Perform step-up
                            game->player.y = platform->y - game->player.height;
                            game->player.dy = 0; // Stop any residual vertical movement
                            game->player.is_on_ground = true; // Ensure player is considered on ground after step
                            // Player's x position is already advanced by player.dx.
                            // player.dx is not zeroed, allowing continued movement if key is held.
                        } else {
                            // Normal wall collision: stop and align
                            game->player.x = platform->x - game->player.width;
                            game->player.dx = 0;
                        }
                    }
                    // Moving left
                    else if (game->player.dx < 0 && // Player is moving left
                               game->player.x - game->player.dx >= platform->x + platform->width && // Previous left edge was right of or at platform's right edge
                               game->player.x < platform->x + platform->width) { // Current left edge is past platform's right edge

                        float player_current_foot_y = game->player.y + game->player.height;
                        float step_height = player_current_foot_y - platform->y;

                        if (step_height > 0 && step_height <= MAX_STEP_UP_HEIGHT && game->player.is_on_ground) {
                            // Perform step-up
                            game->player.y = platform->y - game->player.height;
                            game->player.dy = 0;
                            game->player.is_on_ground = true;
                        } else {
                            // Normal wall collision: stop and align
                            game->player.x = platform->x + platform->width;
                            game->player.dx = 0;
                        }
                    }
                } // End of horizontal collision check
            }
        }
    }
    
    for (int i = 0; i < game->current_level_data->num_enemies; i++) {
        if (game->current_level_data->enemies[i].active) {
            update_enemy(&game->current_level_data->enemies[i], game);
        }
    }
    
    // Update projectiles
    update_projectiles(game->current_level_data, game);
    check_projectile_collisions(game->current_level_data, game);
    
    // Update particles
    update_particles(game->current_level_data);
    
    // Update screen shake
    update_screen_shake(game);
    
    // Handle player attack
    if (game->player.state == ATTACKING) {
        // Check for enemies in attack range
        for (int i = 0; i < game->current_level_data->num_enemies; i++) {
            Entity* enemy = &game->current_level_data->enemies[i];
            if (!enemy->active) continue;
            
            float dx = enemy->x - game->player.x;
            float dy = enemy->y - game->player.y;
            float distance = sqrt(dx * dx + dy * dy);
            
            if (distance <= PLAYER_ATTACK_RANGE) {
                enemy->health -= game->player.attack_power;
                game->score += 25; // Score for damaging enemy
                
                if (enemy->health <= 0) {
                    // Create enhanced death effect for melee kills too
                    create_enemy_death_effect(game->current_level_data, 
                                            enemy->x + enemy->width/2, 
                                            enemy->y + enemy->height/2, 
                                            enemy->type);
                    enemy->active = false;
                    game->score += 100; // Bonus for defeating enemy
                    printf("Enemy defeated! Score: %d\n", game->score);
                }
                
                // Visual feedback - enemy flash or knockback could be added here
                printf("Player attacks enemy! Enemy health: %.0f\n", enemy->health);
            }
        }
        
        // Reset attack state after one frame
        game->player.state = MOVING;
    }
    
    // Decrease attack cooldown
    if (game->player.last_attack > 0) {
        game->player.last_attack--;
    }
    
    // Decrease shooting cooldown
    if (game->player.last_shot > 0) {
        game->player.last_shot--;
    }
    
    handle_collisions(game);
    
    // Check for collision with glucose items
    for (int i = 0; i < game->current_level_data->num_glucose_items; i++) {
        GlucoseItem* item = &game->current_level_data->glucose_items[i];
        if (item->active &&
            game->player.x < item->x + GLUCOSE_WIDTH &&
            game->player.x + game->player.width > item->x &&
            game->player.y < item->y + GLUCOSE_HEIGHT &&
            game->player.y + game->player.height > item->y) {
            
            item->active = false; // Deactivate the item
            game->player.health += GLUCOSE_HEALTH_RECOVERY;
            if (game->player.health > game->player.max_health) {
                game->player.health = game->player.max_health;
            }
            
            // Add score for collecting glucose
            game->score += 50;
            
            // Create collection particle effect
            create_particle_burst(game->current_level_data, 
                                item->x + GLUCOSE_WIDTH/2, 
                                item->y + GLUCOSE_HEIGHT/2, 
                                al_map_rgb(255, 105, 180), 12);
            
            // Play collect sound if enabled
            if (game->settings.sound_enabled && game->collect_sound) {
                al_play_sample(game->collect_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            }
            
            // Visual feedback could be added here (particle effect, score popup)
            printf("Glucose collected! Health: %.0f/%.0f, Score: %d\n", 
                   game->player.health, game->player.max_health, game->score);
        }
    }
    
    if (game->player.x < 0) game->player.x = 0;
    if (game->player.x > game->current_level_data->level_width - game->player.width) {
        game->player.x = game->current_level_data->level_width - game->player.width;
    }
    // Allow falling off bottom of screen for game over, or handle differently
    if (game->player.y > SCREEN_HEIGHT) { // Fell off bottom
        // Play death sound if enabled
        if (game->settings.sound_enabled && game->death_sound) {
            al_play_sample(game->death_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
        game->player.health = 0;
        game->state = GAME_OVER; // Or a specific "fell off" game over
    } else if (game->player.y < 0 && game->player.dy < 0) { // Hit ceiling
        game->player.y = 0;
        game->player.dy = 0;
    }
    
    game->current_level_data->scroll_x = game->player.x - SCREEN_WIDTH * SCROLL_X_PLAYER_OFFSET_FACTOR;
    if (game->current_level_data->scroll_x < 0) {
        game->current_level_data->scroll_x = 0;
    }
    if (game->current_level_data->level_width > SCREEN_WIDTH && // Only scroll if level is wider than screen
        game->current_level_data->scroll_x > game->current_level_data->level_width - SCREEN_WIDTH) {
        game->current_level_data->scroll_x = game->current_level_data->level_width - SCREEN_WIDTH;
    }
    if (game->current_level_data->level_width <= SCREEN_WIDTH) { // Don't scroll if level fits on screen
        game->current_level_data->scroll_x = 0;
    }

    Portal* portal = &game->current_level_data->portal;
    if (portal->is_active && 
        game->player.x < portal->x + portal->width &&
        game->player.x + game->player.width > portal->x &&
        game->player.y < portal->y + portal->height &&
        game->player.y + game->player.height > portal->y) {
        game->state = LEVEL_COMPLETE;
        game->score += LEVEL_COMPLETE_SCORE_BONUS; // Bonus for completing level
    }
}

// Screen shake effect functions
void create_screen_shake(Game* game, float intensity, int duration) {
    if (!game) return;
    
    // Only apply if new shake is stronger or current shake is ending
    if (intensity > game->screen_shake.intensity || game->screen_shake.duration < 10) {
        game->screen_shake.intensity = intensity;
        game->screen_shake.duration = duration;
    }
}

void update_screen_shake(Game* game) {
    if (!game) return;
    
    if (game->screen_shake.duration > 0) {
        // Calculate random offset based on intensity
        float max_offset = game->screen_shake.intensity;
        game->screen_shake.offset_x = ((rand() % 200) - 100) / 100.0f * max_offset;
        game->screen_shake.offset_y = ((rand() % 200) - 100) / 100.0f * max_offset;
        
        // Gradually reduce intensity and duration
        game->screen_shake.duration--;
        game->screen_shake.intensity *= 0.95f; // Gradual fade
        
        if (game->screen_shake.duration <= 0) {
            game->screen_shake.intensity = 0;
            game->screen_shake.offset_x = 0;
            game->screen_shake.offset_y = 0;
        }
    }
}

// Original cleanup_menus function from main.c
void cleanup_menus(Game* game) {
    if (game->main_menu.items) {
        free(game->main_menu.items);
        game->main_menu.items = NULL;
        game->main_menu.num_items = 0; // Reset num_items to prevent use after free
    }
    if (game->level_menu.items) {
        free(game->level_menu.items);
        game->level_menu.items = NULL;
        game->level_menu.num_items = 0; // Reset num_items
    }
    if (game->settings_menu.items) {
        free(game->settings_menu.items);
        game->settings_menu.items = NULL;
        game->settings_menu.num_items = 0; // Reset num_items
    }
}

// Original cleanup_game function from main.c
void cleanup_game(Game* game) {
    cleanup_menus(game);
    cleanup_levels(game); // This is now in level.c but called from here
    
    if (game->jump_sound) al_destroy_sample(game->jump_sound);
    if (game->hit_sound) al_destroy_sample(game->hit_sound);
    if (game->death_sound) al_destroy_sample(game->death_sound);
    if (game->collect_sound) al_destroy_sample(game->collect_sound);
    if (game->shoot_sound) al_destroy_sample(game->shoot_sound);
    if (game->music_instance) al_destroy_sample_instance(game->music_instance);
    
    if (game->font) al_destroy_font(game->font);
    if (game->title_font) al_destroy_font(game->title_font);
    // Allegro addons are shutdown by al_uninstall_system() implicitly if initialized
    // but specific resource destruction is good practice.

    if (game->event_queue) al_destroy_event_queue(game->event_queue);
    if (game->timer) al_destroy_timer(game->timer);
    if (game->display) al_destroy_display(game->display);

    // Player sprites are not currently loaded, but if they were:
    // if (game->player.sprite) al_destroy_bitmap(game->player.sprite);
    // if (game->player.sprite_sheet) al_destroy_bitmap(game->player.sprite_sheet);

    // Consider al_shutdown_primitives_addon(), al_shutdown_font_addon(), etc.
    // if not relying on al_uninstall_system(). For now, al_uninstall_system() in main is fine.
}
