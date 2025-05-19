#include "../include/game_logic.h"
#include "../include/game.h"      // For Game struct, constants, Allegro headers
#include "../include/level.h"      // For init_levels, cleanup_levels
#include "../include/input.h"      // For handle_input (though not directly called by these funcs)
#include "../include/drawing.h"    // For draw_game (though not directly called by these funcs)
#include "../include/entity.h"     // For update_enemy, handle_collisions
#include <stdio.h>               // For fprintf, sprintf
#include <stdlib.h>              // For malloc, free
#include <allegro5/allegro.h>
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
    if (!al_install_keyboard()) {
        fprintf(stderr, "Failed to initialize keyboard!\n");
        return false;
    }
    if (!al_init_primitives_addon()) {
        fprintf(stderr, "Failed to initialize primitives addon!\n");
        return false;
    }
    if (!al_init_font_addon() || !al_init_ttf_addon()) {
        fprintf(stderr, "Failed to initialize font addons!\n");
        return false;
    }

    // TODO: Make font paths configurable or use relative paths
    game->font = al_load_ttf_font("/System/Library/Fonts/Helvetica.ttc", 24, 0);
    game->title_font = al_load_ttf_font("/System/Library/Fonts/Helvetica.ttc", 48, 0);
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
        return false; // Or handle gracefully (e.g., disable sound)
    }
    if (!al_init_acodec_addon()) {
        fprintf(stderr, "Failed to initialize audio codecs!\n");
        return false;
    }
    if (!al_reserve_samples(8)) {
        fprintf(stderr, "Failed to reserve audio samples!\n");
        return false;
    }
    // game->jump_sound = al_load_sample("resources/sounds/jump.wav"); // Example
    // game->hit_sound = al_load_sample("resources/sounds/hit.wav");
    // game->death_sound = al_load_sample("resources/sounds/death.wav");
    // game->music_instance = al_load_sample_instance("resources/sounds/music.ogg");
    // if (game->music_instance) al_attach_sample_instance_to_mixer(game->music_instance, al_get_default_mixer());

    game->player.x = SCREEN_WIDTH / 4.0f;
    game->player.y = SCREEN_HEIGHT / 2.0f;
    game->player.width = 30;
    game->player.height = 30;
    game->player.dx = 0;
    game->player.dy = 0;
    game->player.active = true;
    game->player.type = CANCER_CELL;
    game->player.state = IDLE;
    game->player.behavior = BEHAVIOR_NONE;
    game->player.health = 100;
    game->player.max_health = 100;
    game->player.attack_power = 10;
    game->player.attack_speed = 1.0;
    game->player.last_attack = 0;
    game->player.sprite = NULL;
    game->player.sprite_sheet = NULL;
    game->player.current_frame = 0;
    game->player.frame_timer = 0;
    game->player.is_on_ground = false;

    game->state = WELCOME_SCREEN;
    game->running = true;
    game->score = 0;
    game->current_level = 1; // Start at level 1

    init_menus(game);
    init_levels(game); // This will set game->current_level_data

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

    game->settings.difficulty = 2;
    game->settings.sound_enabled = true;
    game->settings.music_enabled = true;
}

// Original update_game function from main.c
void update_game(Game* game) {
    if (game->state != PLAYING) {
        return;
    }
    
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
                game->player.health -= 1; // Reduced damage for testing, make constant later
                if (game->player.health <= 0) {
                    game->state = GAME_OVER;
                }
                // Land on deadly platform too
                game->player.y = platform->y - game->player.height;
                game->player.dy = 0;
                game->player.is_on_ground = true;
            } else {
                if (game->player.dy >= 0 &&
                    game->player.y + game->player.height - game->player.dy <= platform->y + PLATFORM_JUMP_TOLERANCE &&
                    game->player.y + game->player.height > platform->y) {
                    game->player.dy = 0;
                    game->player.y = platform->y - game->player.height;
                    game->player.is_on_ground = true;
                } else if (game->player.dx > 0 && game->player.x + game->player.width - game->player.dx <= platform->x) {
                    game->player.x = platform->x - game->player.width;
                    game->player.dx = 0;
                } else if (game->player.dx < 0 && game->player.x - game->player.dx >= platform->x + platform->width) {
                    game->player.x = platform->x + platform->width;
                    game->player.dx = 0;
                } else if (game->player.dy < 0 && game->player.y - game->player.dy >= platform->y + platform->height) {
                    game->player.y = platform->y + platform->height;
                    game->player.dy = 0;
                }
            }
        }
    }
    
    for (int i = 0; i < game->current_level_data->num_enemies; i++) {
        if (game->current_level_data->enemies[i].active) {
            update_enemy(&game->current_level_data->enemies[i], game);
        }
    }
    
    handle_collisions(game);
    
    if (game->player.x < 0) game->player.x = 0;
    if (game->player.x > game->current_level_data->level_width - game->player.width) {
        game->player.x = game->current_level_data->level_width - game->player.width;
    }
    // Allow falling off bottom of screen for game over, or handle differently
    if (game->player.y > SCREEN_HEIGHT) { // Fell off bottom
         game->player.health = 0;
         game->state = GAME_OVER; // Or a specific "fell off" game over
    } else if (game->player.y < 0 && game->player.dy < 0) { // Hit ceiling
        game->player.y = 0;
        game->player.dy = 0;
    }
    
    game->current_level_data->scroll_x = game->player.x - SCREEN_WIDTH / 3.0f;
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
        game->score += 1000; // Bonus for completing level
    }
}

// Original cleanup_menus function from main.c
void cleanup_menus(Game* game) {
    if (game->main_menu.items) free(game->main_menu.items);
    game->main_menu.items = NULL;
    if (game->level_menu.items) free(game->level_menu.items);
    game->level_menu.items = NULL;
    if (game->settings_menu.items) free(game->settings_menu.items);
    game->settings_menu.items = NULL;
}

// Original cleanup_game function from main.c
void cleanup_game(Game* game) {
    cleanup_menus(game);
    cleanup_levels(game); // This is now in level.c but called from here
    
    if (game->jump_sound) al_destroy_sample(game->jump_sound);
    if (game->hit_sound) al_destroy_sample(game->hit_sound);
    if (game->death_sound) al_destroy_sample(game->death_sound);
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
