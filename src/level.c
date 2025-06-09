#include "../include/level.h"
#include "../include/game.h" // For Game, Level, Platform, Entity, Portal types, constants
#include <stdio.h>    // For sprintf, fprintf
#include <stdlib.h>   // For malloc, free
#include <string.h>   // For strdup
#include <math.h>     // For sin in level generation

// Original init_level function from main.c
void init_level(Level* level, const char* name, const char* description, float width, int id) { // Added id parameter
    level->platforms = NULL;
    level->enemies = NULL;
    level->glucose_items = NULL; // Initialize glucose_items
    level->projectiles = NULL;   // Initialize projectiles
    level->particles = NULL;     // Initialize particles
    level->num_platforms = 0;
    level->num_enemies = 0;
    level->num_glucose_items = 0; // Initialize num_glucose_items
    level->num_projectiles = 0;   // Initialize num_projectiles
    level->num_particles = 0;     // Initialize num_particles
    level->background = NULL;
    // Initialize multi-background fields
    for (int i = 0; i < 4; i++) {
        level->backgrounds[i] = NULL;
    }
    level->num_backgrounds = 0;
    level->background_positions = NULL;
    level->scroll_x = 0;
    level->level_width = width;
    level->level_name = strdup(name);
    level->level_description = strdup(description);
    level->id = id; // Store the level id
    
    // Allocate projectile array
    level->projectiles = (Projectile*)malloc(sizeof(Projectile) * MAX_PROJECTILES);
    if (level->projectiles) {
        // Initialize all projectiles as inactive
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            level->projectiles[i].active = false;
        }
    } else {
        fprintf(stderr, "Failed to allocate memory for projectiles in level %d\n", id);
    }
    
    // Allocate particle array
    level->particles = (Particle*)malloc(sizeof(Particle) * MAX_PARTICLES);
    if (level->particles) {
        // Initialize all particles as inactive
        for (int i = 0; i < MAX_PARTICLES; i++) {
            level->particles[i].active = false;
        }
    } else {
        fprintf(stderr, "Failed to allocate memory for particles in level %d\n", id);
    }
    
    // Portal is initialized in init_level_content
}

// Original init_levels function from main.c
void init_levels(Game* game) {
    game->num_levels = 4; // Increased to 4 levels
    game->levels = (Level*)malloc(sizeof(Level) * game->num_levels);
    if (!game->levels) {
        fprintf(stderr, "Failed to allocate memory for levels!\n");
        return;
    }
    
    init_level(&game->levels[0], "The Blood Stream", 
        "Navigate through blood vessels while avoiding patrolling T-cells", 1600, 1); // Pass id 1
    init_level_content(&game->levels[0], 1);
    
    init_level(&game->levels[1], "The Lymph Node",
        "Survive the immune system\'s fortress", 2400, 2); // Pass id 2
    init_level_content(&game->levels[1], 2);
    
    init_level(&game->levels[2], "The Final Battle",
        "Face off against specialized killer cells", 3200, 3); // Pass id 3
    init_level_content(&game->levels[2], 3);
    
    // New level ONE with multi-background system
    init_level(&game->levels[3], "level ONE",
        "Journey through evolving cellular environments", 4800, 4); // Pass id 4, wider level
    init_level_content(&game->levels[3], 4);

    game->current_level_data = &game->levels[0];
    
    char path[256];
    // Load regular backgrounds for levels 1-3
    for (int i = 0; i < 3; i++) {
        sprintf(path, "resources/sprites/background_%d.png", i + 1);
        game->levels[i].background = al_load_bitmap(path);
        if (!game->levels[i].background) {
            fprintf(stderr, "Failed to load background: %s\n", path);
            // Game can continue without background, or handle error more strictly
        }
    }
    
    // Load multi-backgrounds for level ONE (index 3)
    Level* level_one = &game->levels[3];
    level_one->num_backgrounds = 4;
    level_one->background_positions = (float*)malloc(sizeof(float) * 4);
    
    // Set up background transition positions (each background covers 1200 pixels)
    level_one->background_positions[0] = 0.0f;     // scene_11.png from 0-1200
    level_one->background_positions[1] = 1200.0f;  // scene_12.png from 1200-2400
    level_one->background_positions[2] = 2400.0f;  // scene_13.png from 2400-3600
    level_one->background_positions[3] = 3600.0f;  // scene_14_1.png from 3600-4800
    
    // Load the scene backgrounds
    const char* scene_files[] = {"scene_11.png", "scene_12.png", "scene_13.png", "scene_14_1.png"};
    for (int i = 0; i < 4; i++) {
        sprintf(path, "resources/sprites/%s", scene_files[i]);
        level_one->backgrounds[i] = al_load_bitmap(path);
        if (!level_one->backgrounds[i]) {
            fprintf(stderr, "Failed to load scene background: %s\n", path);
        }
    }
}

// Original init_level_content function from main.c
void init_level_content(Level* level, int level_number) {
    switch (level_number) {
        case 1: // Blood Stream level
            level->num_platforms = 18;
            level->platforms = malloc(sizeof(Platform) * level->num_platforms);
            if (!level->platforms) {
                fprintf(stderr, "Failed to allocate memory for platforms in level %d\n", level_number);
                level->num_platforms = 0;
                // Do not return immediately, try to load other things like glucose
            }
            
            for (int i = 0; i < 8; i++) {
                level->platforms[i] = (Platform){
                    .x = i * 200.0f,
                    .y = SCREEN_HEIGHT - 100.0f + sin(i * 0.5f) * 50.0f,
                    .width = 200.0f,
                    .height = 20.0f,
                    .color = al_map_rgb(200, 0, 0),
                    .is_deadly = false
                };
                level->platforms[i + 8] = (Platform){
                    .x = i * 200.0f,
                    .y = 100.0f + sin(i * 0.5f) * 50.0f,
                    .width = 200.0f,
                    .height = 20.0f,
                    .color = al_map_rgb(200, 0, 0),
                    .is_deadly = false
                };
            }
            for (int i = 16; i < level->num_platforms; i++) {
                level->platforms[i] = (Platform){
                    .x = 500.0f + (i-16) * 600.0f,  // Moved from 300.0f to 500.0f to avoid spawn area
                    .y = SCREEN_HEIGHT/2.0f,
                    .width = 30.0f,
                    .height = 100.0f,
                    .color = al_map_rgb(255, 255, 0),
                    .is_deadly = true
                };
            }

            float last_floor_y_l1 = SCREEN_HEIGHT - 100.0f + sin(7 * 0.5f) * 50.0f; 
            level->portal.width = PORTAL_WIDTH;
            level->portal.height = PORTAL_HEIGHT;
            level->portal.x = level->level_width - PORTAL_WIDTH - 20.0f;
            level->portal.y = last_floor_y_l1 - PORTAL_HEIGHT;
            level->portal.is_active = true;
            
            level->num_enemies = 5;
            level->enemies = malloc(sizeof(Entity) * level->num_enemies);
             if (!level->enemies) {
                fprintf(stderr, "Failed to allocate memory for enemies in level %d\n", level_number);
                level->num_enemies = 0;
                // Do not return immediately
            }
            for (int i = 0; i < level->num_enemies; i++) {
                EntityBehavior enemy_behavior;
                EntityType enemy_type;
                
                // Mix patrol and shooting enemies
                if (i == 2 || i == 4) {
                    enemy_behavior = BEHAVIOR_SHOOT;
                    enemy_type = MACROPHAGE; // Macrophages shoot
                } else {
                    enemy_behavior = BEHAVIOR_PATROL;
                    enemy_type = T_CELL; // T-cells patrol
                }
                
                level->enemies[i] = (Entity){
                    .x = 400.0f + i * 300.0f,
                    .y = SCREEN_HEIGHT/2.0f,
                    .width = 40.0f,
                    .height = 40.0f,
                    .dx = 2.0f,
                    .dy = 0,
                    .active = true,
                    .type = enemy_type,
                    .state = MOVING,
                    .behavior = enemy_behavior,
                    .health = 50,
                    .max_health = 50,
                    .attack_power = 10,
                    .attack_speed = 1.0f,
                    .last_attack = 0,
                    .sprite = NULL,
                    .sprite_sheet = NULL,
                    .current_frame = 0,
                    .frame_timer = 0,
                    .is_on_ground = false,
                    .jump_requested = false,
                    .coyote_time = 0,
                    .jump_buffer = 0,
                    .wall_contact_left = 0,
                    .wall_contact_right = 0,
                    
                    // Enhanced Combat System Fields
                    .combo_count = 0,
                    .combo_timer = 0,
                    .knockback_dx = 0.0f,
                    .knockback_dy = 0.0f,
                    .knockback_timer = 0
                };
            }

            // Initialize glucose items for level 1
            level->num_glucose_items = 3;
            level->glucose_items = malloc(sizeof(GlucoseItem) * level->num_glucose_items);
            if (!level->glucose_items) {
                fprintf(stderr, "Failed to allocate memory for glucose items in level %d\n", level_number);
                level->num_glucose_items = 0;
            } else {
                level->glucose_items[0] = (GlucoseItem){
                    .x = 250.0f,
                    .y = SCREEN_HEIGHT - 150.0f,
                    .width = GLUCOSE_WIDTH,
                    .height = GLUCOSE_HEIGHT,
                    .active = true
                };
                level->glucose_items[1] = (GlucoseItem){
                    .x = 750.0f,
                    .y = 150.0f,
                    .width = GLUCOSE_WIDTH,
                    .height = GLUCOSE_HEIGHT,
                    .active = true
                };
                level->glucose_items[2] = (GlucoseItem){
                    .x = 1250.0f,
                    .y = SCREEN_HEIGHT - 150.0f,
                    .width = GLUCOSE_WIDTH,
                    .height = GLUCOSE_HEIGHT,
                    .active = true
                };
            }
            break;
            
        case 2: // Lymph Node level - Maze-like with vertical challenges
            level->num_platforms = 20;
            level->platforms = malloc(sizeof(Platform) * level->num_platforms);
            if (!level->platforms) { fprintf(stderr, "Failed to allocate platforms for level 2\n"); return; }
            
            // Ground platforms
            for (int i = 0; i < 6; i++) {
                level->platforms[i] = (Platform){
                    .x = i * 400.0f,
                    .y = SCREEN_HEIGHT - 40.0f,
                    .width = 200.0f,
                    .height = 40.0f,
                    .color = al_map_rgb(0, 100, 0),
                    .is_deadly = false
                };
            }
            
            // Vertical maze platforms
            for (int i = 6; i < 14; i++) {
                level->platforms[i] = (Platform){
                    .x = 200.0f + (i-6) * 250.0f,
                    .y = SCREEN_HEIGHT - 200.0f - (i % 3) * 80.0f,
                    .width = 150.0f,
                    .height = 20.0f,
                    .color = al_map_rgb(0, 150, 0),
                    .is_deadly = false
                };
            }
            
            // Deadly spikes
            for (int i = 14; i < level->num_platforms; i++) {
                level->platforms[i] = (Platform){
                    .x = 150.0f + (i-14) * 350.0f,
                    .y = SCREEN_HEIGHT - 60.0f,
                    .width = 20.0f,
                    .height = 20.0f,
                    .color = al_map_rgb(255, 0, 0),
                    .is_deadly = true
                };
            }
            
            level->num_enemies = 8;
            level->enemies = malloc(sizeof(Entity) * level->num_enemies);
            if (!level->enemies) {
                fprintf(stderr, "Failed to allocate memory for enemies in level %d\n", level_number);
                level->num_enemies = 0;
            } else {
                for (int i = 0; i < level->num_enemies; i++) {
                    EntityBehavior enemy_behavior;
                    EntityType enemy_type;
                    
                    // Mix different behaviors: patrol, chase, and shoot
                    if (i == 1 || i == 4 || i == 6) {
                        enemy_behavior = BEHAVIOR_SHOOT;
                        enemy_type = MACROPHAGE; // Macrophages shoot
                    } else if (i == 7) {
                        enemy_behavior = BEHAVIOR_CHASE;
                        enemy_type = T_CELL; // One T-cell chases
                    } else {
                        enemy_behavior = BEHAVIOR_PATROL;
                        enemy_type = (i % 2 == 0) ? T_CELL : MACROPHAGE;
                    }
                    
                    level->enemies[i] = (Entity){
                        .x = 300.0f + i * 280.0f,
                        .y = SCREEN_HEIGHT - 300.0f,
                        .width = 35.0f,
                        .height = 35.0f,
                        .dx = 1.5f,
                        .dy = 0,
                        .active = true,
                        .type = enemy_type,
                        .state = MOVING,
                        .behavior = enemy_behavior,
                        .health = 60,
                        .max_health = 60,
                        .attack_power = 15,
                        .attack_speed = 1.2f,
                        .last_attack = 0,
                        .sprite = NULL,
                        .sprite_sheet = NULL,
                        .current_frame = 0,
                        .frame_timer = 0,
                        .is_on_ground = false,
                        .jump_requested = false,
                        .coyote_time = 0,
                        .jump_buffer = 0,
                        .wall_contact_left = 0,
                        .wall_contact_right = 0,
                        
                        // Enhanced Combat System Fields
                        .combo_count = 0,
                        .combo_timer = 0,
                        .knockback_dx = 0.0f,
                        .knockback_dy = 0.0f,
                        .knockback_timer = 0
                    };
                }
            }
            
            // Initialize glucose items for level 2
            level->num_glucose_items = 4;
            level->glucose_items = malloc(sizeof(GlucoseItem) * level->num_glucose_items);
            if (!level->glucose_items) {
                fprintf(stderr, "Failed to allocate memory for glucose items in level %d\n", level_number);
                level->num_glucose_items = 0;
            } else {
                level->glucose_items[0] = (GlucoseItem){.x = 450.0f, .y = SCREEN_HEIGHT - 250.0f, .width = GLUCOSE_WIDTH, .height = GLUCOSE_HEIGHT, .active = true};
                level->glucose_items[1] = (GlucoseItem){.x = 950.0f, .y = SCREEN_HEIGHT - 180.0f, .width = GLUCOSE_WIDTH, .height = GLUCOSE_HEIGHT, .active = true};
                level->glucose_items[2] = (GlucoseItem){.x = 1450.0f, .y = SCREEN_HEIGHT - 320.0f, .width = GLUCOSE_WIDTH, .height = GLUCOSE_HEIGHT, .active = true};
                level->glucose_items[3] = (GlucoseItem){.x = 1950.0f, .y = SCREEN_HEIGHT - 200.0f, .width = GLUCOSE_WIDTH, .height = GLUCOSE_HEIGHT, .active = true};
            }
            
            level->portal.width = PORTAL_WIDTH;
            level->portal.height = PORTAL_HEIGHT;
            level->portal.x = level->level_width - PORTAL_WIDTH - 20.0f;
            level->portal.y = SCREEN_HEIGHT - 40.0f - PORTAL_HEIGHT;
            level->portal.is_active = true;
            break;
            
        case 3: // Final Battle level - Boss arena with complex layout
            level->num_platforms = 15;
            level->platforms = malloc(sizeof(Platform) * level->num_platforms);
            if (!level->platforms) { fprintf(stderr, "Failed to allocate platforms for level 3\n"); return; }
            
            // Arena floor with gaps
            level->platforms[0] = (Platform){0, SCREEN_HEIGHT - 40.0f, 600.0f, 40.0f, al_map_rgb(50,50,50), false};
            level->platforms[1] = (Platform){800.0f, SCREEN_HEIGHT - 40.0f, 600.0f, 40.0f, al_map_rgb(50,50,50), false};
            level->platforms[2] = (Platform){1600.0f, SCREEN_HEIGHT - 40.0f, 800.0f, 40.0f, al_map_rgb(50,50,50), false};
            level->platforms[3] = (Platform){2600.0f, SCREEN_HEIGHT - 40.0f, 600.0f, 40.0f, al_map_rgb(50,50,50), false};
            
            // Multi-level arena platforms
            for (int i = 4; i < 10; i++) {
                level->platforms[i] = (Platform){
                    .x = 200.0f + (i-4) * 450.0f,
                    .y = SCREEN_HEIGHT - 150.0f - (i % 2) * 100.0f,
                    .width = 200.0f,
                    .height = 20.0f,
                    .color = al_map_rgb(80, 80, 80),
                    .is_deadly = false
                };
            }
            
            // Deadly lava pits
            for (int i = 10; i < level->num_platforms; i++) {
                level->platforms[i] = (Platform){
                    .x = 650.0f + (i-10) * 400.0f,
                    .y = SCREEN_HEIGHT - 30.0f,
                    .width = 100.0f,
                    .height = 30.0f,
                    .color = al_map_rgb(255, 100, 0),
                    .is_deadly = true
                };
            }
            
            level->num_enemies = 12; // More challenging final level
            level->enemies = malloc(sizeof(Entity) * level->num_enemies);
            if (!level->enemies) {
                fprintf(stderr, "Failed to allocate memory for enemies in level %d\n", level_number);
                level->num_enemies = 0;
            } else {
                // Mix of different enemy types for final challenge
                for (int i = 0; i < level->num_enemies; i++) {
                    EntityType enemy_type;
                    if (i < 4) enemy_type = T_CELL;
                    else if (i < 8) enemy_type = MACROPHAGE;
                    else if (i < 10) enemy_type = B_CELL;
                    else enemy_type = NK_CELL; // Strongest enemies
                    
                    level->enemies[i] = (Entity){
                        .x = 500.0f + i * 220.0f,
                        .y = SCREEN_HEIGHT - 200.0f,
                        .width = (enemy_type == NK_CELL) ? 50.0f : 40.0f,
                        .height = (enemy_type == NK_CELL) ? 50.0f : 40.0f,
                        .dx = (enemy_type == NK_CELL) ? 3.0f : 2.0f,
                        .dy = 0,
                        .active = true,
                        .type = enemy_type,
                        .state = MOVING,
                        .behavior = (enemy_type == NK_CELL) ? BEHAVIOR_CHASE : BEHAVIOR_PATROL,
                        .health = (enemy_type == NK_CELL) ? 100 : 70,
                        .max_health = (enemy_type == NK_CELL) ? 100 : 70,
                        .attack_power = (enemy_type == NK_CELL) ? 25 : 20,
                        .attack_speed = 1.5f,
                        .last_attack = 0,
                        .sprite = NULL,
                        .sprite_sheet = NULL,
                        .current_frame = 0,
                        .frame_timer = 0,
                        .is_on_ground = false,
                        .jump_requested = false,
                        .coyote_time = 0,
                        .jump_buffer = 0,
                        .wall_contact_left = 0,
                        .wall_contact_right = 0,
                        
                        // Enhanced Combat System Fields
                        .combo_count = 0,
                        .combo_timer = 0,
                        .knockback_dx = 0.0f,
                        .knockback_dy = 0.0f,
                        .knockback_timer = 0
                    };
                }
            }
            
            // Strategic glucose placement for final battle
            level->num_glucose_items = 5;
            level->glucose_items = malloc(sizeof(GlucoseItem) * level->num_glucose_items);
            if (!level->glucose_items) {
                fprintf(stderr, "Failed to allocate memory for glucose items in level %d\n", level_number);
                level->num_glucose_items = 0;
            } else {
                level->glucose_items[0] = (GlucoseItem){.x = 350.0f, .y = SCREEN_HEIGHT - 200.0f, .width = GLUCOSE_WIDTH, .height = GLUCOSE_HEIGHT, .active = true};
                level->glucose_items[1] = (GlucoseItem){.x = 1000.0f, .y = SCREEN_HEIGHT - 300.0f, .width = GLUCOSE_WIDTH, .height = GLUCOSE_HEIGHT, .active = true};
                level->glucose_items[2] = (GlucoseItem){.x = 1650.0f, .y = SCREEN_HEIGHT - 200.0f, .width = GLUCOSE_WIDTH, .height = GLUCOSE_HEIGHT, .active = true};
                level->glucose_items[3] = (GlucoseItem){.x = 2200.0f, .y = SCREEN_HEIGHT - 350.0f, .width = GLUCOSE_WIDTH, .height = GLUCOSE_HEIGHT, .active = true};
                level->glucose_items[4] = (GlucoseItem){.x = 2800.0f, .y = SCREEN_HEIGHT - 100.0f, .width = GLUCOSE_WIDTH, .height = GLUCOSE_HEIGHT, .active = true};
            }
            
            level->portal.width = PORTAL_WIDTH;
            level->portal.height = PORTAL_HEIGHT;
            level->portal.x = level->level_width - PORTAL_WIDTH - 20.0f;
            level->portal.y = SCREEN_HEIGHT - 40.0f - PORTAL_HEIGHT;
            level->portal.is_active = true;
            break;
            
        case 4: // level ONE - Multi-background transitioning level (clean, no obstacles)
            // Just a simple ground platform for the player to walk on
            level->num_platforms = 1;
            level->platforms = malloc(sizeof(Platform) * level->num_platforms);
            if (!level->platforms) { 
                fprintf(stderr, "Failed to allocate platforms for level ONE\n"); 
                return; 
            }
            
            // Single long ground platform across the entire level
            level->platforms[0] = (Platform){
                .x = 0.0f,
                .y = SCREEN_HEIGHT - 40.0f,
                .width = 4800.0f, // Full level width
                .height = 40.0f,
                .color = al_map_rgb(139, 69, 19), // Brown ground
                .is_deadly = false
            };
            
            // No enemies - just background viewing
            level->num_enemies = 0;
            level->enemies = NULL;
            
            // No glucose items - just pure background experience
            level->num_glucose_items = 0;
            level->glucose_items = NULL;
            
            level->portal.width = PORTAL_WIDTH;
            level->portal.height = PORTAL_HEIGHT;
            level->portal.x = level->level_width - PORTAL_WIDTH - 20.0f;
            level->portal.y = SCREEN_HEIGHT - 40.0f - PORTAL_HEIGHT; // Align with ground platform
            level->portal.is_active = true;
            break;
    }
}

// Original cleanup_level function from main.c
void cleanup_level(Level* level) {
    if (level->platforms) free(level->platforms);
    if (level->enemies) free(level->enemies);
    if (level->glucose_items) free(level->glucose_items); // Free glucose_items
    if (level->projectiles) free(level->projectiles);     // Free projectiles
    if (level->particles) free(level->particles);         // Free particles
    if (level->background) al_destroy_bitmap(level->background);
    
    // Cleanup multi-backgrounds
    for (int i = 0; i < 4; i++) {
        if (level->backgrounds[i]) {
            al_destroy_bitmap(level->backgrounds[i]);
        }
    }
    if (level->background_positions) free(level->background_positions);
    
    if (level->level_name) free(level->level_name);
    if (level->level_description) free(level->level_description);
    // Set pointers to NULL after freeing to prevent double free issues
    level->platforms = NULL;
    level->enemies = NULL;
    level->glucose_items = NULL; // Set glucose_items to NULL
    level->projectiles = NULL;   // Set projectiles to NULL
    level->particles = NULL;     // Set particles to NULL
    level->background = NULL;
    
    // Reset multi-background fields
    for (int i = 0; i < 4; i++) {
        level->backgrounds[i] = NULL;
    }
    level->background_positions = NULL;
    level->num_backgrounds = 0;
    
    level->level_name = NULL;
    level->level_description = NULL;
}

// Original cleanup_levels function from main.c
void cleanup_levels(Game* game) {
    if (game->levels) {
        for (int i = 0; i < game->num_levels; i++) {
            cleanup_level(&game->levels[i]);
        }
        free(game->levels);
        game->levels = NULL; // Set to NULL after freeing
    }
}
