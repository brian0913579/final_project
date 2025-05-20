#include "../include/input.h"
#include "../include/game.h" // For Game, Level, Menu, Entity, GameState, constants
#include "../include/game_logic.h" // Added for reset_player_and_level
#include <allegro5/keyboard.h> // Changed from allegro_keyboard.h
#include <allegro5/keycodes.h> // For

// Original handle_menu_input function from main.c
void handle_menu_input(Game* game, ALLEGRO_EVENT* event) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        Menu* current_menu = NULL;
        
        switch (game->state) {
            case WELCOME_SCREEN:
                if (event->keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    game->state = MAIN_MENU;
                }
                return;
            case MAIN_MENU:
                current_menu = &game->main_menu;
                break;
            case LEVEL_SELECT:
                current_menu = &game->level_menu;
                break;
            case SETTINGS:
                current_menu = &game->settings_menu;
                break;
            case VICTORY: 
                if (event->keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    game->state = MAIN_MENU; 
                }
                return; 
            default:
                // LEVEL_COMPLETE is handled by the main handle_input
                return;
        }

        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_UP:
                if (current_menu && current_menu->num_items > 0) { // Added null check for current_menu
                    do {
                        current_menu->selected_index--;
                        if (current_menu->selected_index < 0) 
                            current_menu->selected_index = current_menu->num_items - 1;
                    } while (!current_menu->items[current_menu->selected_index].enabled);
                }
                break;

            case ALLEGRO_KEY_DOWN:
                if (current_menu && current_menu->num_items > 0) { // Added null check
                    do {
                        current_menu->selected_index++;
                        if (current_menu->selected_index >= current_menu->num_items) 
                            current_menu->selected_index = 0;
                    } while (!current_menu->items[current_menu->selected_index].enabled);
                }
                break;

            case ALLEGRO_KEY_ENTER:
                if (current_menu) { // Added null check
                    switch (game->state) {
                        case MAIN_MENU:
                            switch (current_menu->selected_index) {
                                case 0: // Start Game
                                    // game->state = PLAYING; // State will be set by reset
                                    // game->current_level = 1; // Handled by reset
                                    // game->current_level_data = &game->levels[0]; // Handled by reset
                                    // game->player.x = SCREEN_WIDTH / 4.0f;
                                    // game->player.y = SCREEN_HEIGHT / 2.0f;
                                    // game->player.dx = 0;
                                    // game->player.dy = 0;
                                    // game->player.health = game->player.max_health;
                                    // game->player.is_on_ground = false;
                                    // game->player.jump_requested = false; // Reset on new level
                                    // game->score = 0; // Score should be reset here if it's per-game
                                    // game->levels[0].scroll_x = 0; // Handled by reset
                                    reset_player_and_level(game, INITIAL_LEVEL -1); // Reset for level 1 (index 0)
                                    game->score = 0; // Reset global score when starting a new game
                                    game->state = PLAYING;
                                    break;
                                case 1: game->state = LEVEL_SELECT; break;
                                case 2: game->state = SETTINGS; break;
                                case 3: game->running = false; break;
                            }
                            break;
                        case LEVEL_SELECT:
                            if (current_menu->selected_index == current_menu->num_items - 1) {
                                game->state = MAIN_MENU;
                            } else if (current_menu->items[current_menu->selected_index].enabled) {
                                // game->current_level = current_menu->selected_index + 1; // Handled by reset
                                // game->current_level_data = &game->levels[game->current_level - 1]; // Handled by reset
                                // game->player.x = SCREEN_WIDTH / 4.0f;
                                // game->player.y = SCREEN_HEIGHT / 2.0f;
                                // game->player.dx = 0;
                                // game->player.dy = 0;
                                // game->player.health = game->player.max_health;
                                // game->player.is_on_ground = false;
                                // game->player.jump_requested = false; // Reset on level select
                                // game->current_level_data->scroll_x = 0; // Handled by reset
                                reset_player_and_level(game, current_menu->selected_index); // Pass level index
                                game->state = PLAYING;
                            }
                            break;
                        case SETTINGS:
                            switch (current_menu->selected_index) {
                                case 0: // Difficulty
                                    game->settings.difficulty = (game->settings.difficulty % 3) + 1;
                                    sprintf(current_menu->items[0].text, "Difficulty: %s", 
                                        game->settings.difficulty == 1 ? "Easy" :
                                        game->settings.difficulty == 2 ? "Normal" : "Hard");
                                    break;
                                case 1: // Sound
                                    game->settings.sound_enabled = !game->settings.sound_enabled;
                                    sprintf(current_menu->items[1].text, "Sound: %s",
                                        game->settings.sound_enabled ? "On" : "Off");
                                    break;
                                case 2: // Music
                                    game->settings.music_enabled = !game->settings.music_enabled;
                                    sprintf(current_menu->items[2].text, "Music: %s",
                                        game->settings.music_enabled ? "On" : "Off");
                                    break;
                                case 3: game->state = MAIN_MENU; break;
                            }
                            break;
                        default: break;
                    }
                }
                break;
            case ALLEGRO_KEY_ESCAPE:
                if (game->state != MAIN_MENU && game->state != WELCOME_SCREEN) { // WELCOME_SCREEN has no ESCAPE to MAIN_MENU
                    game->state = MAIN_MENU;
                }
                break;
        }
    }
}

// Original handle_input function from main.c
void handle_input(Game* game, ALLEGRO_EVENT* event) {
    if (game->state == WELCOME_SCREEN || game->state == MAIN_MENU || 
        game->state == LEVEL_SELECT || game->state == SETTINGS || game->state == VICTORY) {
        handle_menu_input(game, event);
        return;
    }

    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_W:
            case ALLEGRO_KEY_SPACE:
                if (game->state == PLAYING) { // Removed is_on_ground check here
                    game->player.jump_requested = true;
                }
                break;
            case ALLEGRO_KEY_ESCAPE:
                if (game->state == PLAYING)
                    game->state = PAUSED;
                else if (game->state == PAUSED)
                    game->state = PLAYING;
                break;
            case ALLEGRO_KEY_M:
                if (game->state == PAUSED || game->state == LEVEL_COMPLETE) {
                    game->state = MAIN_MENU;
                }
                break;
            case ALLEGRO_KEY_N: 
                if (game->state == LEVEL_COMPLETE) {
                    // game->current_level is 0-indexed (e.g., 0 for Level 1, 1 for Level 2)
                    // game->num_levels is the total count (e.g., 3)
                    // To go to the next level, we need to check if current_level < max_level_index (num_levels - 1)
                    if (game->current_level < game->num_levels - 1) {
                        // Load the next level: current_level + 1
                        reset_player_and_level(game, game->current_level + 1);
                        game->state = PLAYING;
                    } else {
                        // Player has completed the last level
                        game->state = VICTORY;
                    }
                }
                break;
            case ALLEGRO_KEY_ENTER:
                if (game->state == GAME_OVER) {
                    // Game state is reset to main menu, no need to call reset_player_and_level here
                    // as starting a new game from main menu will call it.
                    game->state = MAIN_MENU;
                }
                break;
            case ALLEGRO_KEY_R:
                if (game->state == GAME_OVER) {
                    // game->player.x = SCREEN_WIDTH / 4.0f;
                    // game->player.y = SCREEN_HEIGHT / 2.0f;
                    // game->player.dx = 0;
                    // game->player.dy = 0;
                    // game->player.health = game->player.max_health;
                    // game->player.last_attack = 0;
                    // game->player.jump_requested = false; // Reset on retry
                    // game->current_level_data->scroll_x = 0;
                    // for (int i = 0; i < game->current_level_data->num_enemies; i++) {
                    //     Entity* enemy = &game->current_level_data->enemies[i];
                    //     // Re-initialize enemy positions based on their initial setup if possible
                    //     // For now, using a generic reset. This might need to be level specific.
                    //     enemy->x = 400.0f + i * 300.0f; // Example reset position
                    //     enemy->y = SCREEN_HEIGHT/2.0f;
                    //     enemy->dx = 2.0f;
                    //     enemy->dy = 0;
                    //     enemy->health = enemy->max_health;
                    //     enemy->behavior = BEHAVIOR_PATROL; // Reset behavior
                    //     enemy->active = true; // Ensure enemy is active
                    // }
                    reset_player_and_level(game, game->current_level - 1); // Reset current level (current_level is 1-based)
                    game->state = PLAYING;
                }
                break;
        }
    }

    // Only update dx from key states on timer events when playing
    if (game->state == PLAYING && event->type == ALLEGRO_EVENT_TIMER) {
        ALLEGRO_KEYBOARD_STATE keyState;
        al_get_keyboard_state(&keyState); // Get fresh state on timer
        
        game->player.dx = 0; 
        if (al_key_down(&keyState, ALLEGRO_KEY_A)) {
            game->player.dx = -MOVE_SPEED;
        }
        if (al_key_down(&keyState, ALLEGRO_KEY_D)) {
            game->player.dx = MOVE_SPEED;
        }

        // Handle continuous jump if jump key is held
        if (al_key_down(&keyState, ALLEGRO_KEY_W) || al_key_down(&keyState, ALLEGRO_KEY_SPACE)) {
            if (game->player.is_on_ground) { // Only request another jump if currently on the ground
                game->player.jump_requested = true;
            }
        }
    }
}
