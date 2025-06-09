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
    game->num_levels = 3; // Three levels now
    game->levels = (Level*)malloc(sizeof(Level) * game->num_levels);
    if (!game->levels) {
        fprintf(stderr, "Failed to allocate memory for levels!\n");
        return;
    }
    
    // Level ONE (multi-background system)
    init_level(&game->levels[0], "level ONE",
        "Journey through evolving cellular environments", 4800, 1); // Pass id 1, wider level
    init_level_content(&game->levels[0], 1);

    // Level TWO (multi-background system)
    init_level(&game->levels[1], "level TWO", 
        "Navigate through the blood stream", 4800, 2); // Pass id 2, wider level
    init_level_content(&game->levels[1], 2);

    // Level THREE (multi-background system)
    init_level(&game->levels[2], "level THREE",
        "Face the final cellular challenge", 5120, 3); // Pass id 3, wider level (4 backgrounds)
    init_level_content(&game->levels[2], 3);

    game->current_level_data = &game->levels[0];
    
    char path[256];
    
    // Load multi-backgrounds for level ONE (index 0)
    Level* level_one = &game->levels[0];
    level_one->num_backgrounds = 4;
    level_one->background_positions = (float*)malloc(sizeof(float) * 4);
    
    // Set up background transition positions (each background covers 1280 pixels for 1280x720 screen)
    level_one->background_positions[0] = 0.0f;     // scene_11_scaled.png from 0-1280
    level_one->background_positions[1] = 1280.0f;  // scene_12_scaled.png from 1280-2560
    level_one->background_positions[2] = 2560.0f;  // scene_13_scaled.png from 2560-3840
    level_one->background_positions[3] = 3840.0f;  // scene_14_1_scaled.png from 3840-5120
    
    // Load the scaled scene backgrounds (1280x720) for level ONE
    const char* scene_files_level1[] = {"scene_11_scaled.png", "scene_12_scaled.png", "scene_13_scaled.png", "scene_14_1_scaled.png"};
    for (int i = 0; i < 4; i++) {
        sprintf(path, "resources/sprites/%s", scene_files_level1[i]);
        level_one->backgrounds[i] = al_load_bitmap(path);
        if (!level_one->backgrounds[i]) {
            fprintf(stderr, "Failed to load scene background: %s\n", path);
        }
    }

    // Load multi-backgrounds for level TWO (index 1)
    Level* level_two = &game->levels[1];
    level_two->num_backgrounds = 3;
    level_two->background_positions = (float*)malloc(sizeof(float) * 3);
    
    // Set up background transition positions for level TWO
    level_two->background_positions[0] = 0.0f;     // scene_21_scaled.png from 0-1280
    level_two->background_positions[1] = 1280.0f;  // scene_22_scaled.png from 1280-2560
    level_two->background_positions[2] = 2560.0f;  // scene_23_1_scaled.png from 2560-3840
    
    // Load the scaled scene backgrounds for level TWO
    const char* scene_files_level2[] = {"scene_21_scaled.png", "scene_22_scaled.png", "scene_23_1_scaled.png"};
    for (int i = 0; i < 3; i++) {
        sprintf(path, "resources/sprites/%s", scene_files_level2[i]);
        level_two->backgrounds[i] = al_load_bitmap(path);
        if (!level_two->backgrounds[i]) {
            fprintf(stderr, "Failed to load scene background: %s\n", path);
        }
    }

    // Load multi-backgrounds for level THREE (index 2)
    Level* level_three = &game->levels[2];
    level_three->num_backgrounds = 4;
    level_three->background_positions = (float*)malloc(sizeof(float) * 4);
    
    // Set up background transition positions for level THREE
    level_three->background_positions[0] = 0.0f;     // scene_31_scaled.png from 0-1280
    level_three->background_positions[1] = 1280.0f;  // scene_32_scaled.png from 1280-2560
    level_three->background_positions[2] = 2560.0f;  // scene_33_scaled.png from 2560-3840
    level_three->background_positions[3] = 3840.0f;  // scene_34_1_scaled.png from 3840-5120
    
    // Load the scaled scene backgrounds for level THREE
    const char* scene_files_level3[] = {"scene_31_scaled.png", "scene_32_scaled.png", "scene_33_scaled.png", "scene_34_1_scaled.png"};
    for (int i = 0; i < 4; i++) {
        sprintf(path, "resources/sprites/%s", scene_files_level3[i]);
        level_three->backgrounds[i] = al_load_bitmap(path);
        if (!level_three->backgrounds[i]) {
            fprintf(stderr, "Failed to load scene background: %s\n", path);
        }
    }
}

// Original init_level_content function from main.c
void init_level_content(Level* level, int level_number) {
    switch (level_number) {
        case 1: // level ONE - Multi-background transitioning level (clean, no obstacles)
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
            level->portal.y = SCREEN_HEIGHT - 40.0f - PORTAL_HEIGHT;
            level->portal.is_active = true;
            break;

        case 2: // level TWO - Blood stream navigation (background viewing)
            // Just a simple ground platform for the player to walk on
            level->num_platforms = 1;
            level->platforms = malloc(sizeof(Platform) * level->num_platforms);
            if (!level->platforms) { 
                fprintf(stderr, "Failed to allocate platforms for level TWO\n"); 
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
            level->portal.y = SCREEN_HEIGHT - 40.0f - PORTAL_HEIGHT;
            level->portal.is_active = true;
            break;

        case 3: // level THREE - Final cellular challenge (background viewing)
            // Just a simple ground platform for the player to walk on
            level->num_platforms = 1;
            level->platforms = malloc(sizeof(Platform) * level->num_platforms);
            if (!level->platforms) { 
                fprintf(stderr, "Failed to allocate platforms for level THREE\n"); 
                return; 
            }
            
            // Single long ground platform across the entire level
            level->platforms[0] = (Platform){
                .x = 0.0f,
                .y = SCREEN_HEIGHT - 40.0f,
                .width = 5120.0f, // Full level width
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
            level->portal.y = SCREEN_HEIGHT - 40.0f - PORTAL_HEIGHT;
            level->portal.is_active = true;
            break;

        default:
            fprintf(stderr, "Invalid level number: %d\n", level_number);
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
