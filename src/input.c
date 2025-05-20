#include "../include/input.h"
#include "../include/game.h" // For Game, Level, Menu, Entity, GameState, constants
#include <allegro5/keyboard.h> // Changed from allegro_keyboard.h
#include <allegro5/keycodes.h> // For ALLEGRO_KEY_*
#include <stdio.h> // For sprintf (used in settings menu)

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
                                    game->state = PLAYING;
                                    game->current_level = 1;
                                    game->current_level_data = &game->levels[0];
                                    game->player.x = SCREEN_WIDTH / 4.0f;
                                    game->player.y = SCREEN_HEIGHT / 2.0f;
                                    game->player.dx = 0;
                                    game->player.dy = 0;
                                    game->player.health = game->player.max_health;
                                    game->player.is_on_ground = false;
                                    game->score = 0;
                                    game->levels[0].scroll_x = 0;
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
                                game->current_level = current_menu->selected_index + 1;
                                game->current_level_data = &game->levels[game->current_level - 1];
                                game->player.x = SCREEN_WIDTH / 4.0f;
                                game->player.y = SCREEN_HEIGHT / 2.0f;
                                game->player.dx = 0;
                                game->player.dy = 0;
                                game->player.health = game->player.max_health;
                                game->player.is_on_ground = false;
                                game->current_level_data->scroll_x = 0;
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
            case ALLEGRO_KEY_W: // Added W for jump
            case ALLEGRO_KEY_SPACE:
                if (game->state == PLAYING && game->player.is_on_ground) {
                    game->player.dy = JUMP_SPEED;
                    game->player.is_on_ground = false;
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
                    if (game->current_level < game->num_levels) {
                        game->current_level++;
                        game->current_level_data = &game->levels[game->current_level - 1];
                        game->player.x = SCREEN_WIDTH / 4.0f;
                        game->player.y = SCREEN_HEIGHT / 2.0f;
                        game->player.dx = 0;
                        game->player.dy = 0;
                        game->player.health = game->player.max_health;
                        game->player.is_on_ground = false;
                        game->current_level_data->scroll_x = 0;
                        game->state = PLAYING;
                    } else {
                        game->state = VICTORY;
                    }
                }
                break;
            case ALLEGRO_KEY_ENTER:
                if (game->state == GAME_OVER) {
                    game->state = MAIN_MENU;
                }
                break;
            case ALLEGRO_KEY_R:
                if (game->state == GAME_OVER) {
                    game->player.x = SCREEN_WIDTH / 4.0f;
                    game->player.y = SCREEN_HEIGHT / 2.0f;
                    game->player.dx = 0;
                    game->player.dy = 0;
                    game->player.health = game->player.max_health;
                    game->player.last_attack = 0;
                    game->current_level_data->scroll_x = 0;
                    for (int i = 0; i < game->current_level_data->num_enemies; i++) {
                        Entity* enemy = &game->current_level_data->enemies[i];
                        // Re-initialize enemy positions based on their initial setup if possible
                        // For now, using a generic reset. This might need to be level specific.
                        enemy->x = 400.0f + i * 300.0f; // Example reset position
                        enemy->y = SCREEN_HEIGHT/2.0f;
                        enemy->dx = 2.0f;
                        enemy->dy = 0;
                        enemy->health = enemy->max_health;
                        enemy->behavior = BEHAVIOR_PATROL; // Reset behavior
                        enemy->active = true; // Ensure enemy is active
                    }
                    game->state = PLAYING;
                }
                break;
        }
    }

    if (game->state == PLAYING) { // Continuous movement only when playing
        ALLEGRO_KEYBOARD_STATE keyState;
        al_get_keyboard_state(&keyState);
        
        // Horizontal movement with A and D
        // Reset dx at the beginning of each input check if no movement key is pressed.
        // This allows jump to not interfere with horizontal movement intentions.
        game->player.dx = 0; 
        if (al_key_down(&keyState, ALLEGRO_KEY_A)) {
            game->player.dx = -MOVE_SPEED;
        }
        if (al_key_down(&keyState, ALLEGRO_KEY_D)) {
            // If A is also held, D takes precedence or they cancel out.
            // For simplicity, D overrides A if both are pressed.
            // If A should cancel D, then: if (game->player.dx == -MOVE_SPEED) game->player.dx = 0; else game->player.dx = MOVE_SPEED;
            game->player.dx = MOVE_SPEED;
        }

        // Allow jump with W (pressed event) or SPACE (pressed event)
        // The event-based jump is already handled above. 
        // If continuous jump while holding W is desired, it would be: 
        // if (al_key_down(&keyState, ALLEGRO_KEY_W) && game->player.is_on_ground) {
        //     game->player.dy = JUMP_SPEED;
        //     game->player.is_on_ground = false;
        // }
    }
}
