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
    level->num_platforms = 0;
    level->num_enemies = 0;
    level->num_glucose_items = 0; // Initialize num_glucose_items
    level->background = NULL;
    level->scroll_x = 0;
    level->level_width = width;
    level->level_name = strdup(name);
    level->level_description = strdup(description);
    level->id = id; // Store the level id
    // Portal is initialized in init_level_content
}

// Original init_levels function from main.c
void init_levels(Game* game) {
    game->num_levels = 3;
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

    game->current_level_data = &game->levels[0];
    
    char path[256];
    for (int i = 0; i < game->num_levels; i++) {
        sprintf(path, "resources/sprites/background_%d.png", i + 1);
        game->levels[i].background = al_load_bitmap(path);
        if (!game->levels[i].background) {
            fprintf(stderr, "Failed to load background: %s\n", path);
            // Game can continue without background, or handle error more strictly
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
                    .x = 300.0f + (i-16) * 600.0f,
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
                level->enemies[i] = (Entity){
                    .x = 400.0f + i * 300.0f,
                    .y = SCREEN_HEIGHT/2.0f,
                    .width = 40.0f,
                    .height = 40.0f,
                    .dx = 2.0f,
                    .dy = 0,
                    .active = true,
                    .type = T_CELL,
                    .state = MOVING,
                    .behavior = BEHAVIOR_PATROL,
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
                    .jump_requested = false
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
            
        case 2: // Lymph Node level
            level->num_platforms = 1;
            level->platforms = malloc(sizeof(Platform) * level->num_platforms);
            if (!level->platforms) { fprintf(stderr, "Failed to allocate platforms for level 2\n"); return; }
            level->platforms[0] = (Platform){0, SCREEN_HEIGHT - 40.0f, level->level_width, 40.0f, al_map_rgb(0,100,0), false};
            level->num_enemies = 0;
            level->enemies = NULL;
            level->portal.width = PORTAL_WIDTH;
            level->portal.height = PORTAL_HEIGHT;
            level->portal.x = level->level_width - PORTAL_WIDTH - 20.0f;
            level->portal.y = SCREEN_HEIGHT - 40.0f - PORTAL_HEIGHT;
            level->portal.is_active = true;
            break;
            
        case 3: // Final Battle level
            level->num_platforms = 1;
            level->platforms = malloc(sizeof(Platform) * level->num_platforms);
            if (!level->platforms) { fprintf(stderr, "Failed to allocate platforms for level 3\n"); return; }
            level->platforms[0] = (Platform){0, SCREEN_HEIGHT - 40.0f, level->level_width, 40.0f, al_map_rgb(50,50,50), false};
            level->num_enemies = 0;
            level->enemies = NULL;
            level->portal.width = PORTAL_WIDTH;
            level->portal.height = PORTAL_HEIGHT;
            level->portal.x = level->level_width - PORTAL_WIDTH - 20.0f;
            level->portal.y = SCREEN_HEIGHT - 40.0f - PORTAL_HEIGHT;
            level->portal.is_active = true;
            break;
    }
}

// Original cleanup_level function from main.c
void cleanup_level(Level* level) {
    if (level->platforms) free(level->platforms);
    if (level->enemies) free(level->enemies);
    if (level->glucose_items) free(level->glucose_items); // Free glucose_items
    if (level->background) al_destroy_bitmap(level->background);
    if (level->level_name) free(level->level_name);
    if (level->level_description) free(level->level_description);
    // Set pointers to NULL after freeing to prevent double free issues
    level->platforms = NULL;
    level->enemies = NULL;
    level->glucose_items = NULL; // Set glucose_items to NULL
    level->background = NULL;
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
