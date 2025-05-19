#include "../include/drawing.h"
#include "../include/game.h" // For Game, Level, Menu, Entity, Portal, constants
#include <allegro5/allegro_primitives.h> // For drawing shapes
#include <allegro5/allegro_font.h>     // For drawing text
#include <allegro5/allegro_ttf.h>      // For ttf fonts (though game->font is already loaded)
#include <stdio.h>                   // For sprintf
#include <math.h>                    // For sin in welcome screen pulse

// Original draw_menu function from main.c
void draw_menu(Game* game, Menu* menu, const char* title) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    al_draw_text(game->title_font, al_map_rgb(255, 255, 255),
                SCREEN_WIDTH/2, 50,
                ALLEGRO_ALIGN_CENTRE, title);

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

// Original draw_welcome_screen function from main.c
void draw_welcome_screen(Game* game) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    
    float pulse = (1 + sin(al_get_time() * 2)) * 0.5f;
    ALLEGRO_COLOR title_color = al_map_rgb(255, (unsigned char)(100 + pulse * 155), (unsigned char)(100 + pulse * 155));
    
    al_draw_text(game->title_font, title_color,
                SCREEN_WIDTH/2, SCREEN_HEIGHT/3,
                ALLEGRO_ALIGN_CENTRE, "Cancer Cell Adventure");
    
    al_draw_text(game->font, al_map_rgb(200, 200, 200),
                SCREEN_WIDTH/2, SCREEN_HEIGHT/2,
                ALLEGRO_ALIGN_CENTRE, "Press ENTER to start");
    
    al_draw_text(game->font, al_map_rgb(150, 150, 150),
                SCREEN_WIDTH/2, SCREEN_HEIGHT - 50,
                ALLEGRO_ALIGN_CENTRE, "Version 1.0");
    
    al_flip_display();
}

// Original draw_main_menu function from main.c
void draw_main_menu(Game* game) {
    draw_menu(game, &game->main_menu, "Main Menu");
}

// Original draw_level_select function from main.c
void draw_level_select(Game* game) {
    draw_menu(game, &game->level_menu, "Select Level");
}

// Original draw_settings_menu function from main.c
void draw_settings_menu(Game* game) {
    draw_menu(game, &game->settings_menu, "Settings");
}

// Original draw_pause_screen function from main.c
void draw_pause_screen(Game* game) {
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

// Original draw_game function from main.c
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
            al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(0, 0, 0, 192));
            al_draw_text(game->title_font, al_map_rgb(255, 0, 0),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/3, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
            char score_text[64];
            sprintf(score_text, "Final Score: %d", game->score);
            al_draw_text(game->font, al_map_rgb(255, 255, 255),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/2, ALLEGRO_ALIGN_CENTRE, score_text);
            al_draw_text(game->font, al_map_rgb(200, 200, 200),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 50, ALLEGRO_ALIGN_CENTRE, "Press ENTER to return to menu");
            al_draw_text(game->font, al_map_rgb(200, 200, 200),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 90, ALLEGRO_ALIGN_CENTRE, "Press R to retry level");
            al_flip_display();
            break;
        case LEVEL_COMPLETE: 
            al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(0, 0, 100, 192)); 
            al_draw_text(game->title_font, al_map_rgb(255, 255, 255),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/3, ALLEGRO_ALIGN_CENTRE, "LEVEL COMPLETE!");
            char score_text_lc[64];
            sprintf(score_text_lc, "Score: %d", game->score);
            al_draw_text(game->font, al_map_rgb(255, 255, 255),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/2, ALLEGRO_ALIGN_CENTRE, score_text_lc);
            if (game->current_level < game->num_levels) {
                 al_draw_text(game->font, al_map_rgb(200, 200, 200),
                            SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 50, ALLEGRO_ALIGN_CENTRE, "Press N for Next Level");
            } else {
                 al_draw_text(game->font, al_map_rgb(200, 200, 200),
                            SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 50, ALLEGRO_ALIGN_CENTRE, "Press N for Victory Screen");
            }
            al_draw_text(game->font, al_map_rgb(200, 200, 200),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 90, ALLEGRO_ALIGN_CENTRE, "Press M for Main Menu");
            al_flip_display();
            break;
        case VICTORY:
            al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(255, 215, 0, 192));
            al_draw_text(game->title_font, al_map_rgb(255, 255, 255),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/3, ALLEGRO_ALIGN_CENTRE, "VICTORY!");
            char score_text_victory[64];
            sprintf(score_text_victory, "Final Score: %d", game->score);
            al_draw_text(game->font, al_map_rgb(255, 255, 255),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/2, ALLEGRO_ALIGN_CENTRE, score_text_victory);
            al_draw_text(game->font, al_map_rgb(220, 220, 220),
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 50, ALLEGRO_ALIGN_CENTRE, "Press ENTER to return to menu");
            al_flip_display();
            break;
        case PLAYING:
            al_clear_to_color(al_map_rgb(135, 206, 235));
            Level* current = game->current_level_data;
            if (current->background) {
                al_draw_bitmap(current->background, -current->scroll_x, 0, 0);
            }
            for (int i = 0; i < current->num_platforms; i++) {
                Platform* p = &current->platforms[i];
                float screen_x = p->x - current->scroll_x;
                if (screen_x + p->width >= 0 && screen_x <= SCREEN_WIDTH) {
                    al_draw_filled_rectangle(screen_x, p->y, screen_x + p->width, p->y + p->height, p->color);
                }
            }
            if (current->portal.is_active) {
                float portal_screen_x = current->portal.x - current->scroll_x;
                if (portal_screen_x + current->portal.width >= 0 && portal_screen_x <= SCREEN_WIDTH) {
                    al_draw_filled_rectangle(portal_screen_x, current->portal.y,
                        portal_screen_x + current->portal.width, current->portal.y + current->portal.height,
                        al_map_rgb(128, 0, 128));
                    al_draw_rectangle(portal_screen_x, current->portal.y,
                        portal_screen_x + current->portal.width, current->portal.y + current->portal.height,
                        al_map_rgb(255,255,255), 2.0f);
                }
            }
            for (int i = 0; i < current->num_enemies; i++) {
                Entity* e = &current->enemies[i];
                if (!e->active) continue;
                float screen_x = e->x - current->scroll_x;
                if (screen_x + e->width >= 0 && screen_x <= SCREEN_WIDTH) {
                    ALLEGRO_COLOR enemy_color;
                    switch (e->type) {
                        case T_CELL: enemy_color = al_map_rgb(255, 255, 0); break;
                        case MACROPHAGE: enemy_color = al_map_rgb(200, 200, 0); break;
                        case B_CELL: enemy_color = al_map_rgb(0, 200, 255); break;
                        case NK_CELL: enemy_color = al_map_rgb(255, 0, 0); break;
                        default: enemy_color = al_map_rgb(255, 255, 255);
                    }
                    al_draw_filled_circle(screen_x + e->width/2, e->y + e->height/2, e->width/2, enemy_color);
                    if (e->health < e->max_health) {
                        float health_percent = e->health / e->max_health;
                        al_draw_filled_rectangle(screen_x, e->y - 10, 
                            screen_x + e->width * health_percent, e->y - 5,
                            al_map_rgb((unsigned char)(255 * (1-health_percent)), (unsigned char)(255 * health_percent), 0));
                    }
                }
            }
            float player_screen_x = game->player.x - current->scroll_x;
            al_draw_filled_circle(player_screen_x + game->player.width/2, 
                                game->player.y + game->player.height/2, 
                                game->player.width/2, al_map_rgb(255, 100, 100));
            float health_percent = game->player.health / game->player.max_health;
            al_draw_filled_rectangle(10, 40, 10 + 200 * health_percent, 50,
                al_map_rgb((unsigned char)(255 * (1-health_percent)), (unsigned char)(255 * health_percent), 0));
            char level_text[32];
            sprintf(level_text, "Level: %d  Score: %d", game->current_level, game->score);
            al_draw_text(game->font, al_map_rgb(255, 255, 255), 10, 10, ALLEGRO_ALIGN_LEFT, level_text);
            al_flip_display();
            break;
        default:
            break;
    }
}
