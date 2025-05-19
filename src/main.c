#include "../include/game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GRAVITY 0.5
#define JUMP_SPEED 10.0
#define MOVE_SPEED 5.0

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

    game->font = al_load_ttf_font("/System/Library/Fonts/Helvetica.ttc", 24, 0);
    game->title_font = al_load_ttf_font("/System/Library/Fonts/Helvetica.ttc", 48, 0);
    if (!game->font || !game->title_font) {
        fprintf(stderr, "Failed to load fonts!\n");
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
        return false;
    }

    game->event_queue = al_create_event_queue();
    if (!game->event_queue) {
        fprintf(stderr, "Failed to create event queue!\n");
        return false;
    }

    al_register_event_source(game->event_queue, al_get_display_event_source(game->display));
    al_register_event_source(game->event_queue, al_get_timer_event_source(game->timer));
    al_register_event_source(game->event_queue, al_get_keyboard_event_source());

    // Initialize audio
    if (!al_install_audio()) {
        fprintf(stderr, "Failed to initialize audio!\n");
        return false;
    }
    
    if (!al_init_acodec_addon()) {
        fprintf(stderr, "Failed to initialize audio codecs!\n");
        return false;
    }
    
    if (!al_reserve_samples(8)) {
        fprintf(stderr, "Failed to reserve audio samples!\n");
        return false;
    }

    // Initialize player
    game->player.x = SCREEN_WIDTH / 4;
    game->player.y = SCREEN_HEIGHT / 2;
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

    // Initialize game state
    game->state = WELCOME_SCREEN;
    game->running = true;
    game->score = 0;

    // Initialize menus and levels
    init_menus(game);
    init_levels(game);

    al_start_timer(game->timer);
    return true;
}

void init_menus(Game* game) {
    // Initialize main menu
    game->main_menu.num_items = 4;
    game->main_menu.items = malloc(sizeof(MenuItem) * game->main_menu.num_items);
    game->main_menu.items[0] = (MenuItem){"Start Game", true, true};
    game->main_menu.items[1] = (MenuItem){"Level Select", true, true};
    game->main_menu.items[2] = (MenuItem){"Settings", true, true};
    game->main_menu.items[3] = (MenuItem){"Exit", true, true};
    game->main_menu.selected_index = 0;

    // Initialize level select menu
    game->level_menu.num_items = 4;
    game->level_menu.items = malloc(sizeof(MenuItem) * game->level_menu.num_items);
    game->level_menu.items[0] = (MenuItem){"Level 1: The Blood Stream", true, true};
    game->level_menu.items[1] = (MenuItem){"Level 2: The Lymph Node", true, false};
    game->level_menu.items[2] = (MenuItem){"Level 3: The Final Battle", true, false};
    game->level_menu.items[3] = (MenuItem){"Back", true, true};
    game->level_menu.selected_index = 0;

    // Initialize settings menu
    game->settings_menu.num_items = 4;
    game->settings_menu.items = malloc(sizeof(MenuItem) * game->settings_menu.num_items);
    game->settings_menu.items[0] = (MenuItem){"Difficulty: Normal", true, true};
    game->settings_menu.items[1] = (MenuItem){"Sound: On", true, true};
    game->settings_menu.items[2] = (MenuItem){"Music: On", true, true};
    game->settings_menu.items[3] = (MenuItem){"Back", true, true};
    game->settings_menu.selected_index = 0;

    // Initialize game settings
    game->settings.difficulty = 2; // Normal
    game->settings.sound_enabled = true;
    game->settings.music_enabled = true;
    game->current_level = 1;
}

Level* create_level(const char* name, const char* description, float width) {
    Level* level = (Level*)malloc(sizeof(Level));
    level->platforms = NULL;
    level->enemies = NULL;
    level->num_platforms = 0;
    level->num_enemies = 0;
    level->background = NULL;
    level->scroll_x = 0;
    level->level_width = width;
    level->level_name = strdup(name);
    level->level_description = strdup(description);
    return level;
}

void init_levels(Game* game) {
    game->num_levels = 3;
    game->levels = (Level*)malloc(sizeof(Level) * game->num_levels);
    
    // Level 1: The Blood Stream
    game->levels[0] = *create_level("The Blood Stream", 
        "Navigate through blood vessels while avoiding patrolling T-cells", 1600);
    init_level_content(&game->levels[0], 1);
    
    // Level 2: The Lymph Node
    game->levels[1] = *create_level("The Lymph Node",
        "Survive the immune system's fortress", 2400);
    init_level_content(&game->levels[1], 2);
    
    // Level 3: The Final Battle
    game->levels[2] = *create_level("The Final Battle",
        "Face off against specialized killer cells", 3200);
    init_level_content(&game->levels[2], 3);

    // Initialize the current level
    game->current_level_data = &game->levels[0];
    
    // Load level resources
    char path[256];
    for (int i = 0; i < game->num_levels; i++) {
        sprintf(path, "resources/sprites/background_%d.png", i + 1);
        game->levels[i].background = al_load_bitmap(path);
    }
}

void init_level_content(Level* level, int level_number) {
    // Allocate memory for platforms based on level
    switch (level_number) {
        case 1: // Blood Stream level
            level->num_platforms = 15;
            level->platforms = malloc(sizeof(Platform) * level->num_platforms);
            
            // Create main blood vessel walls (floor and ceiling)
            for (int i = 0; i < 8; i++) {
                // Floor platform
                level->platforms[i] = (Platform){
                    .x = i * 200,
                    .y = SCREEN_HEIGHT - 100 + sin(i * 0.5) * 50,
                    .width = 200,
                    .height = 20,
                    .color = al_map_rgb(200, 0, 0),
                    .is_deadly = false
                };
                
                // Ceiling platform
                level->platforms[i + 8] = (Platform){
                    .x = i * 200,
                    .y = 100 + sin(i * 0.5) * 50,
                    .width = 200,
                    .height = 20,
                    .color = al_map_rgb(200, 0, 0),
                    .is_deadly = false
                };
            }
            
            // Add some obstacles (antibodies)
            for (int i = 14; i < level->num_platforms; i++) {
                level->platforms[i] = (Platform){
                    .x = 300 + i * 400,
                    .y = SCREEN_HEIGHT/2,
                    .width = 30,
                    .height = 100,
                    .color = al_map_rgb(255, 255, 0),
                    .is_deadly = true
                };
            }
            
            // Initialize enemies
            level->num_enemies = 5;
            level->enemies = malloc(sizeof(Entity) * level->num_enemies);
            
            // Add patrolling T-cells
            for (int i = 0; i < level->num_enemies; i++) {
                level->enemies[i] = (Entity){
                    .x = 400 + i * 300,
                    .y = SCREEN_HEIGHT/2,
                    .width = 40,
                    .height = 40,
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
                    .frame_timer = 0
                };
            }
            break;
            
        case 2: // Lymph Node level
            // More platforms and enemies for level 2
            break;
            
        case 3: // Final Battle level
            // Boss fight setup
            break;
    }
}

void update_game(Game* game) {
    if (game->state != PLAYING) {
        return;
    }
    
    // Update player position
    game->player.x += game->player.dx;
    game->player.y += game->player.dy;
    
    // Apply gravity
    game->player.dy += GRAVITY;
    
    // Check platform collisions
    for (int i = 0; i < game->current_level_data->num_platforms; i++) {
        Platform* platform = &game->current_level_data->platforms[i];
        
        // Basic AABB collision detection
        if (game->player.x < platform->x + platform->width &&
            game->player.x + game->player.width > platform->x &&
            game->player.y < platform->y + platform->height &&
            game->player.y + game->player.height > platform->y) {
            
            if (platform->is_deadly) {
                game->player.health -= 25;
                if (game->player.health <= 0) {
                    game->state = GAME_OVER;
                }
                // Bounce back from obstacle
                game->player.dy = -8.0f;
                game->player.y = platform->y - game->player.height - 1;
            } else {
                // Normal platform collision
                if (game->player.dy > 0) {
                    game->player.dy = 0;
                    game->player.y = platform->y - game->player.height;
                }
            }
        }
    }
    
    // Update enemies
    for (int i = 0; i < game->current_level_data->num_enemies; i++) {
        if (game->current_level_data->enemies[i].active) {
            update_enemy(&game->current_level_data->enemies[i], game);
        }
    }
    
    // Handle collisions between entities
    handle_collisions(game);
    
    // Keep player in bounds
    if (game->player.x < 0) game->player.x = 0;
    if (game->player.x > game->current_level_data->level_width - game->player.width) {
        game->player.x = game->current_level_data->level_width - game->player.width;
    }
    if (game->player.y < 0) game->player.y = 0;
    if (game->player.y > SCREEN_HEIGHT - game->player.height) {
        game->player.y = SCREEN_HEIGHT - game->player.height;
        game->player.dy = 0;
    }
    
    // Update camera scroll
    game->current_level_data->scroll_x = game->player.x - SCREEN_WIDTH / 3;
    if (game->current_level_data->scroll_x < 0) {
        game->current_level_data->scroll_x = 0;
    }
    if (game->current_level_data->scroll_x > game->current_level_data->level_width - SCREEN_WIDTH) {
        game->current_level_data->scroll_x = game->current_level_data->level_width - SCREEN_WIDTH;
    }
}

void update_enemy(Entity* enemy, Game* game) {
    switch (enemy->behavior) {
        case BEHAVIOR_PATROL:
            // Simple patrol behavior - move back and forth
            enemy->x += enemy->dx;
            
            // Check for collision with walls and reverse direction
            for (int i = 0; i < game->current_level_data->num_platforms; i++) {
                Platform* platform = &game->current_level_data->platforms[i];
                if (enemy->x < platform->x || 
                    enemy->x + enemy->width > platform->x + platform->width) {
                    enemy->dx *= -1;
                    break;
                }
            }
            
            // Check for player proximity to switch to chase behavior
            float dx = game->player.x - enemy->x;
            float dy = game->player.y - enemy->y;
            float detect_distance = sqrt(dx * dx + dy * dy);
            
            if (detect_distance < 200) { // Detection range
                enemy->behavior = BEHAVIOR_CHASE;
            }
            break;
            
        case BEHAVIOR_CHASE:
            {
                // Move towards player
                float dx = game->player.x - enemy->x;
                float dy = game->player.y - enemy->y;
                float distance = sqrt(dx * dx + dy * dy);
                
                if (distance > 0) {
                    enemy->dx = (dx / distance) * 3.0f; // Chase speed
                    enemy->dy = (dy / distance) * 3.0f;
                }
                
                enemy->x += enemy->dx;
                enemy->y += enemy->dy;
            }
            
            // Return to patrol if player is too far
            if (detect_distance > 300) {
                enemy->behavior = BEHAVIOR_PATROL;
                enemy->dy = 0;
                enemy->dx = 2.0f;
            }
            break;
    }
}

bool check_collision(Entity* a, Entity* b) {
    return (a->x < b->x + b->width &&
            a->x + a->width > b->x &&
            a->y < b->y + b->height &&
            a->y + a->height > b->y);
}

void handle_collisions(Game* game) {
    // Check collisions between player and enemies
    for (int i = 0; i < game->current_level_data->num_enemies; i++) {
        Entity* enemy = &game->current_level_data->enemies[i];
        if (enemy->active && check_collision(&game->player, enemy)) {
            // Player takes damage
            if (game->player.last_attack == 0) {
                game->player.health -= enemy->attack_power;
                game->player.last_attack = 30; // Invincibility frames
                
                // Knockback effect
                float dx = game->player.x - enemy->x;
                float dy = game->player.y - enemy->y;
                float distance = sqrt(dx * dx + dy * dy);
                if (distance > 0) {
                    game->player.dx = (dx / distance) * 10.0f;
                    game->player.dy = (dy / distance) * 10.0f;
                }
                
                // Check if player died
                if (game->player.health <= 0) {
                    game->state = GAME_OVER;
                }
            }
        }
    }
    
    // Update invincibility frames
    if (game->player.last_attack > 0) {
        game->player.last_attack--;
    }
}

void draw_game(Game* game) {
    switch (game->state) {
        case WELCOME_SCREEN:
            draw_welcome_screen(game);
            break;
            
        case MAIN_MENU:
            draw_main_menu(game);
            break;
            
        case LEVEL_SELECT:
            draw_level_select(game);
            break;
            
        case SETTINGS:
            draw_settings_menu(game);
            break;
            
        case PAUSED:
            draw_pause_screen(game);
            break;
            
        case GAME_OVER:
            // Draw semi-transparent black overlay
            al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                                   al_map_rgba(0, 0, 0, 192));
            
            // Draw "GAME OVER" text
            al_draw_text(game->title_font, al_map_rgb(255, 0, 0),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/3,
                        ALLEGRO_ALIGN_CENTRE, "GAME OVER");
            
            // Draw score
            char score_text[64];
            sprintf(score_text, "Final Score: %d", game->score);
            al_draw_text(game->font, al_map_rgb(255, 255, 255),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/2,
                        ALLEGRO_ALIGN_CENTRE, score_text);
            
            // Draw instructions
            al_draw_text(game->font, al_map_rgb(200, 200, 200),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 50,
                        ALLEGRO_ALIGN_CENTRE, "Press ENTER to return to menu");
            al_draw_text(game->font, al_map_rgb(200, 200, 200),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 90,
                        ALLEGRO_ALIGN_CENTRE, "Press R to retry level");
            
            al_flip_display();
            break;
            
        case PLAYING:
            // Draw gameplay
            al_clear_to_color(al_map_rgb(135, 206, 235)); // Sky blue background

            Level* current = game->current_level_data;

            // Draw background if available
            if (current->background) {
                al_draw_bitmap(current->background, -current->scroll_x, 0, 0);
            }

            // Draw platforms
            for (int i = 0; i < current->num_platforms; i++) {
                Platform* p = &current->platforms[i];
                float screen_x = p->x - current->scroll_x;
                
                // Only draw platforms that are visible on screen
                if (screen_x + p->width >= 0 && screen_x <= SCREEN_WIDTH) {
                    al_draw_filled_rectangle(
                        screen_x, p->y,
                        screen_x + p->width, p->y + p->height,
                        p->color
                    );
                }
            }

            // Draw enemies
            for (int i = 0; i < current->num_enemies; i++) {
                Entity* e = &current->enemies[i];
                if (!e->active) continue;
                
                float screen_x = e->x - current->scroll_x;
                if (screen_x + e->width >= 0 && screen_x <= SCREEN_WIDTH) {
                    // Draw enemy based on type
                    ALLEGRO_COLOR enemy_color;
                    switch (e->type) {
                        case T_CELL:
                            enemy_color = al_map_rgb(255, 255, 0);
                            break;
                        case MACROPHAGE:
                            enemy_color = al_map_rgb(200, 200, 0);
                            break;
                        case B_CELL:
                            enemy_color = al_map_rgb(0, 200, 255);
                            break;
                        case NK_CELL:
                            enemy_color = al_map_rgb(255, 0, 0);
                            break;
                        default:
                            enemy_color = al_map_rgb(255, 255, 255);
                    }
                    
                    al_draw_filled_circle(
                        screen_x + e->width/2,
                        e->y + e->height/2,
                        e->width/2,
                        enemy_color
                    );

                    // Draw health bar if damaged
                    if (e->health < e->max_health) {
                        float health_percent = e->health / e->max_health;
                        al_draw_filled_rectangle(
                            screen_x, e->y - 10,
                            screen_x + e->width * health_percent, e->y - 5,
                            al_map_rgb(255 * (1-health_percent), 255 * health_percent, 0)
                        );
                    }
                }
            }

            // Draw player (cancer cell)
            float screen_x = game->player.x - current->scroll_x;
            al_draw_filled_circle(
                screen_x + game->player.width/2,
                game->player.y + game->player.height/2,
                game->player.width/2,
                al_map_rgb(255, 100, 100)
            );

            // Draw player health bar
            float health_percent = game->player.health / game->player.max_health;
            al_draw_filled_rectangle(
                10, 40,
                10 + 200 * health_percent, 50,
                al_map_rgb(255 * (1-health_percent), 255 * health_percent, 0)
            );

            // Draw current level and score
            char level_text[32];
            sprintf(level_text, "Level: %d  Score: %d", game->current_level, game->score);
            al_draw_text(game->font, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, level_text);

            al_flip_display();
            break;
            
        default:
            break;
    }
}

void draw_menu(Game* game, Menu* menu, const char* title) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    // Draw title
    al_draw_text(game->title_font, al_map_rgb(255, 255, 255),
                SCREEN_WIDTH/2, 50,
                ALLEGRO_ALIGN_CENTRE, title);

    // Draw menu items
    for (int i = 0; i < menu->num_items; i++) {
        ALLEGRO_COLOR color = menu->items[i].enabled ? 
            (i == menu->selected_index ? al_map_rgb(255, 255, 0) : al_map_rgb(255, 255, 255)) :
            al_map_rgb(128, 128, 128);

        al_draw_text(game->font, color,
                    SCREEN_WIDTH/2, 200 + i * 50,
                    ALLEGRO_ALIGN_CENTRE, menu->items[i].text);
    }

    al_flip_display();
}

void draw_welcome_screen(Game* game) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    // Draw title with pulsing effect
    float pulse = (1 + sin(al_get_time() * 2)) * 0.5; // Creates a value between 0 and 1
    ALLEGRO_COLOR title_color = al_map_rgb(255, 100 + pulse * 155, 100 + pulse * 155);
    
    al_draw_text(game->title_font, title_color,
                SCREEN_WIDTH/2, SCREEN_HEIGHT/3,
                ALLEGRO_ALIGN_CENTRE, "Cancer Cell Adventure");
    
    // Draw subtitle with different color
    al_draw_text(game->font, al_map_rgb(200, 200, 200),
                SCREEN_WIDTH/2, SCREEN_HEIGHT/2,
                ALLEGRO_ALIGN_CENTRE, "Press ENTER to start");
    
    // Draw version and credits
    al_draw_text(game->font, al_map_rgb(150, 150, 150),
                SCREEN_WIDTH/2, SCREEN_HEIGHT - 50,
                ALLEGRO_ALIGN_CENTRE, "Version 1.0");
    
    al_flip_display();
}

void draw_main_menu(Game* game) {
    draw_menu(game, &game->main_menu, "Main Menu");
}

void draw_level_select(Game* game) {
    draw_menu(game, &game->level_menu, "Select Level");
}

void draw_settings_menu(Game* game) {
    draw_menu(game, &game->settings_menu, "Settings");
}

void draw_pause_screen(Game* game) {
    // Semi-transparent overlay
    al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                           al_map_rgba(0, 0, 0, 128));
    
    al_draw_text(game->title_font, al_map_rgb(255, 255, 255),
                SCREEN_WIDTH/2, SCREEN_HEIGHT/3,
                ALLEGRO_ALIGN_CENTRE, "PAUSED");
    
    al_draw_text(game->font, al_map_rgb(200, 200, 200),
                SCREEN_WIDTH/2, SCREEN_HEIGHT/2,
                ALLEGRO_ALIGN_CENTRE, "Press ESC to resume");
    al_draw_text(game->font, al_map_rgb(200, 200, 200),
                SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 40,
                ALLEGRO_ALIGN_CENTRE, "Press M for Main Menu");
    
    al_flip_display();
}

void handle_input(Game* game, ALLEGRO_EVENT* event) {
    if (game->state == WELCOME_SCREEN || game->state == MAIN_MENU || 
        game->state == LEVEL_SELECT || game->state == SETTINGS) {
        handle_menu_input(game, event);
        return;
    }

    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_SPACE:
                if (game->state == PLAYING && game->player.y + game->player.height >= SCREEN_HEIGHT) {
                    game->player.dy = JUMP_SPEED;
                }
                break;
            case ALLEGRO_KEY_ESCAPE:
                if (game->state == PLAYING)
                    game->state = PAUSED;
                else if (game->state == PAUSED)
                    game->state = PLAYING;
                break;
            case ALLEGRO_KEY_M:
                if (game->state == PAUSED)
                    game->state = MAIN_MENU;
                break;
            case ALLEGRO_KEY_ENTER:
                if (game->state == GAME_OVER) {
                    game->state = MAIN_MENU;
                }
                break;
            case ALLEGRO_KEY_R:
                if (game->state == GAME_OVER) {
                    // Reset player
                    game->player.x = SCREEN_WIDTH / 4;
                    game->player.y = SCREEN_HEIGHT / 2;
                    game->player.dx = 0;
                    game->player.dy = 0;
                    game->player.health = game->player.max_health;
                    game->player.last_attack = 0;
                    
                    // Reset level
                    game->current_level_data->scroll_x = 0;
                    
                    // Reset enemies
                    for (int i = 0; i < game->current_level_data->num_enemies; i++) {
                        Entity* enemy = &game->current_level_data->enemies[i];
                        enemy->x = 400 + i * 300;
                        enemy->y = SCREEN_HEIGHT/2;
                        enemy->dx = 2.0f;
                        enemy->dy = 0;
                        enemy->health = enemy->max_health;
                        enemy->behavior = BEHAVIOR_PATROL;
                    }
                    
                    game->state = PLAYING;
                }
                break;
        }
    }

    // Continuous movement
    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);
    
    if (al_key_down(&keyState, ALLEGRO_KEY_LEFT))
        game->player.dx = -MOVE_SPEED;
    else if (al_key_down(&keyState, ALLEGRO_KEY_RIGHT))
        game->player.dx = MOVE_SPEED;
    else
        game->player.dx = 0;
}

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
            default:
                return;
        }

        switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_UP:
                do {
                    current_menu->selected_index--;
                    if (current_menu->selected_index < 0) 
                        current_menu->selected_index = current_menu->num_items - 1;
                } while (!current_menu->items[current_menu->selected_index].enabled);
                break;

            case ALLEGRO_KEY_DOWN:
                do {
                    current_menu->selected_index++;
                    if (current_menu->selected_index >= current_menu->num_items) 
                        current_menu->selected_index = 0;
                } while (!current_menu->items[current_menu->selected_index].enabled);
                break;

            case ALLEGRO_KEY_ENTER:
                switch (game->state) {
                    case MAIN_MENU:
                        switch (current_menu->selected_index) {
                            case 0: // Start Game
                                game->state = PLAYING;
                                game->current_level = 1;
                                break;
                            case 1: // Level Select
                                game->state = LEVEL_SELECT;
                                break;
                            case 2: // Settings
                                game->state = SETTINGS;
                                break;
                            case 3: // Exit
                                game->running = false;
                                break;
                        }
                        break;

                    case LEVEL_SELECT:
                        if (current_menu->selected_index == current_menu->num_items - 1) {
                            // Back option
                            game->state = MAIN_MENU;
                        } else if (current_menu->items[current_menu->selected_index].enabled) {
                            game->current_level = current_menu->selected_index + 1;
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
                            case 3: // Back
                                game->state = MAIN_MENU;
                                break;
                        }
                        break;
                    default:
                        break;
                }
                break;

            case ALLEGRO_KEY_ESCAPE:
                if (game->state != MAIN_MENU) {
                    game->state = MAIN_MENU;
                }
                break;
        }
    }
}

void cleanup_menus(Game* game) {
    if (game->main_menu.items) free(game->main_menu.items);
    if (game->level_menu.items) free(game->level_menu.items);
    if (game->settings_menu.items) free(game->settings_menu.items);
}

void cleanup_level(Level* level) {
    if (level->platforms) free(level->platforms);
    if (level->enemies) free(level->enemies);
    if (level->background) al_destroy_bitmap(level->background);
    if (level->level_name) free(level->level_name);
    if (level->level_description) free(level->level_description);
}

void cleanup_levels(Game* game) {
    for (int i = 0; i < game->num_levels; i++) {
        cleanup_level(&game->levels[i]);
    }
    free(game->levels);
}

void cleanup_game(Game* game) {
    // Cleanup menus
    cleanup_menus(game);
    
    // Cleanup levels
    cleanup_levels(game);
    
    // Cleanup audio resources
    if (game->jump_sound) al_destroy_sample(game->jump_sound);
    if (game->hit_sound) al_destroy_sample(game->hit_sound);
    if (game->death_sound) al_destroy_sample(game->death_sound);
    if (game->music_instance) al_destroy_sample_instance(game->music_instance);
    
    // Cleanup display and event resources
    if (game->display) al_destroy_display(game->display);
    if (game->timer) al_destroy_timer(game->timer);
    if (game->event_queue) al_destroy_event_queue(game->event_queue);
    if (game->font) al_destroy_font(game->font);
    if (game->title_font) al_destroy_font(game->title_font);
    
    // Cleanup player resources
    if (game->player.sprite) al_destroy_bitmap(game->player.sprite);
    if (game->player.sprite_sheet) al_destroy_bitmap(game->player.sprite_sheet);
}

int main() {
    Game game = {0};
    
    if (!init_game(&game)) {
        fprintf(stderr, "Failed to initialize game!\n");
        return -1;
    }

    init_menus(&game);

    ALLEGRO_EVENT event;

    while (game.running) {
        al_wait_for_event(game.event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            if (game.state == PLAYING) {
                update_game(&game);
                
                // Update enemy AI
                for (int i = 0; i < game.current_level_data->num_enemies; i++) {
                    Entity* enemy = &game.current_level_data->enemies[i];
                    if (enemy->active) {
                        update_enemy(enemy, &game);
                    }
                }
                
                // Handle collisions
                handle_collisions(&game);
            }
            draw_game(&game);
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            game.running = false;
        }
        else {
            handle_input(&game, &event);
        }
    }

    cleanup_game(&game);
    return 0;
}
