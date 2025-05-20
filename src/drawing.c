#include "../include/drawing.h"
#include "../include/game.h" // For Game, Level, Menu, Entity, Portal, constants
#include <allegro5/allegro_primitives.h> // For drawing shapes
#include <allegro5/allegro_font.h>     // For drawing text
#include <allegro5/allegro_ttf.h>      // For ttf fonts (though game->font is already loaded)
#include <stdio.h>                   // For sprintf
#include <math.h>                    // For sin in welcome screen pulse

// Original draw_menu function from main.c
void draw_menu(Game* game, Menu* menu, const char* title) {
    al_clear_to_color(MENU_BACKGROUND_COLOR);
    
    al_draw_text(game->title_font, TITLE_TEXT_COLOR,
                SCREEN_WIDTH/2, MENU_TITLE_Y,
                ALLEGRO_ALIGN_CENTRE, title);

    for (int i = 0; i < menu->num_items; i++) {
        ALLEGRO_COLOR color = menu->items[i].enabled ? 
            (i == menu->selected_index ? MENU_SELECTED_TEXT_COLOR : MENU_TEXT_COLOR) :
            MENU_DISABLED_TEXT_COLOR;

        al_draw_text(game->font, color,
                    SCREEN_WIDTH/2, MENU_ITEM_START_Y + i * MENU_ITEM_SPACING,
                    ALLEGRO_ALIGN_CENTRE, menu->items[i].text);
    }
    al_flip_display();
}

// Original draw_welcome_screen function from main.c
void draw_welcome_screen(Game* game) {
    al_clear_to_color(MENU_BACKGROUND_COLOR);
    
    float pulse = (1 + sin(al_get_time() * 2)) * 0.5f; // Pulse factor between 0 and 1
    // Pulsating color for the title, from a darker red to a brighter red
    ALLEGRO_COLOR title_color = al_map_rgb((unsigned char)(150 + pulse * 105), (unsigned char)(pulse * 100), (unsigned char)(pulse * 100));
    
    al_draw_text(game->title_font, title_color,
                SCREEN_WIDTH/2, SCREEN_HEIGHT/WELCOME_TITLE_Y_DIVISOR,
                ALLEGRO_ALIGN_CENTRE, "Cancer Cell Adventure");
    
    al_draw_text(game->font, COLOR_LIGHT_GRAY,
                SCREEN_WIDTH/2, SCREEN_HEIGHT/WELCOME_SUBTEXT_Y_DIVISOR,
                ALLEGRO_ALIGN_CENTRE, "Press ENTER to start");
    
    al_draw_text(game->font, COLOR_MEDIUM_GRAY,
                SCREEN_WIDTH/2, SCREEN_HEIGHT - WELCOME_VERSION_OFFSET_Y,
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
                           al_map_rgba(0, 0, 0, ALPHA_OVERLAY_MEDIUM));
    
    al_draw_text(game->title_font, COLOR_WHITE,
                SCREEN_WIDTH/2, SCREEN_HEIGHT/PAUSE_TITLE_Y_DIVISOR,
                ALLEGRO_ALIGN_CENTRE, "PAUSED");
    
    al_draw_text(game->font, COLOR_LIGHT_GRAY,
                SCREEN_WIDTH/2, SCREEN_HEIGHT/PAUSE_TEXT_Y_DIVISOR,
                ALLEGRO_ALIGN_CENTRE, "Press ESC to resume");
    al_draw_text(game->font, COLOR_LIGHT_GRAY,
                SCREEN_WIDTH/2, SCREEN_HEIGHT/PAUSE_TEXT_Y_DIVISOR + PAUSE_TEXT_SPACING,
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
            al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(0, 0, 0, ALPHA_OVERLAY_DARK));
            al_draw_text(game->title_font, COLOR_RED,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/GAMEOVER_TITLE_Y_DIVISOR, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
            char score_text[64];
            sprintf(score_text, "Final Score: %d", game->score);
            al_draw_text(game->font, COLOR_WHITE,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/GAMEOVER_TEXT_Y_DIVISOR, ALLEGRO_ALIGN_CENTRE, score_text);
            al_draw_text(game->font, COLOR_LIGHT_GRAY,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/GAMEOVER_TEXT_Y_DIVISOR + GAMEOVER_TEXT_SPACING_1, ALLEGRO_ALIGN_CENTRE, "Press ENTER to return to menu");
            al_draw_text(game->font, COLOR_LIGHT_GRAY,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/GAMEOVER_TEXT_Y_DIVISOR + GAMEOVER_TEXT_SPACING_2, ALLEGRO_ALIGN_CENTRE, "Press R to retry level");
            al_flip_display();
            break;
        case LEVEL_COMPLETE: 
            al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(0, 0, 100, ALPHA_OVERLAY_DARK)); 
            al_draw_text(game->title_font, COLOR_WHITE,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/LEVELCOMPLETE_TITLE_Y_DIVISOR, ALLEGRO_ALIGN_CENTRE, "LEVEL COMPLETE!");
            char score_text_lc[64];
            sprintf(score_text_lc, "Score: %d", game->score);
            al_draw_text(game->font, COLOR_WHITE,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/LEVELCOMPLETE_TEXT_Y_DIVISOR, ALLEGRO_ALIGN_CENTRE, score_text_lc);
            if (game->current_level < game->num_levels) {
                 al_draw_text(game->font, COLOR_LIGHT_GRAY,
                            SCREEN_WIDTH/2, SCREEN_HEIGHT/LEVELCOMPLETE_TEXT_Y_DIVISOR + LEVELCOMPLETE_TEXT_SPACING_1, ALLEGRO_ALIGN_CENTRE, "Press N for Next Level");
            } else {
                 al_draw_text(game->font, COLOR_LIGHT_GRAY,
                            SCREEN_WIDTH/2, SCREEN_HEIGHT/LEVELCOMPLETE_TEXT_Y_DIVISOR + LEVELCOMPLETE_TEXT_SPACING_1, ALLEGRO_ALIGN_CENTRE, "Press N for Victory Screen");
            }
            al_draw_text(game->font, COLOR_LIGHT_GRAY,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/LEVELCOMPLETE_TEXT_Y_DIVISOR + LEVELCOMPLETE_TEXT_SPACING_2, ALLEGRO_ALIGN_CENTRE, "Press M for Main Menu");
            al_flip_display();
            break;
        case VICTORY:
            al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(255, 215, 0, ALPHA_OVERLAY_DARK)); // Gold-ish overlay
            al_draw_text(game->title_font, COLOR_WHITE,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/VICTORY_TITLE_Y_DIVISOR, ALLEGRO_ALIGN_CENTRE, "VICTORY!");
            char score_text_victory[64];
            sprintf(score_text_victory, "Final Score: %d", game->score);
            al_draw_text(game->font, COLOR_WHITE,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/VICTORY_TEXT_Y_DIVISOR, ALLEGRO_ALIGN_CENTRE, score_text_victory);
            al_draw_text(game->font, al_map_rgb(220, 220, 220), // Slightly off-white for variety
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/VICTORY_TEXT_Y_DIVISOR + VICTORY_TEXT_SPACING, ALLEGRO_ALIGN_CENTRE, "Press ENTER to return to menu");
            al_flip_display();
            break;
        case PLAYING:
            al_clear_to_color(COLOR_SKY_BLUE);
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
                        COLOR_PURPLE);
                    al_draw_rectangle(portal_screen_x, current->portal.y,
                        portal_screen_x + current->portal.width, current->portal.y + current->portal.height,
                        COLOR_WHITE, PORTAL_BORDER_THICKNESS);
                }
            }
            for (int i = 0; i < current->num_enemies; i++) {
                Entity* e = &current->enemies[i];
                if (!e->active) continue;
                float screen_x = e->x - current->scroll_x;
                if (screen_x + e->width >= 0 && screen_x <= SCREEN_WIDTH) {
                    ALLEGRO_COLOR enemy_color;
                    switch (e->type) {
                        case T_CELL: enemy_color = COLOR_YELLOW; break; // Bright Yellow
                        case MACROPHAGE: enemy_color = al_map_rgb(200, 200, 0); break; // Darker Yellow
                        case B_CELL: enemy_color = al_map_rgb(0, 200, 255); break; // Cyan/Light Blue
                        case NK_CELL: enemy_color = COLOR_RED; break; // Bright Red
                        default: enemy_color = COLOR_WHITE;
                    }
                    al_draw_filled_circle(screen_x + e->width/2, e->y + e->height/2, e->width/2, enemy_color);
                    if (e->health < e->max_health) {
                        float health_percent = e->health / e->max_health;
                        al_draw_filled_rectangle(screen_x, e->y - ENEMY_HEALTH_BAR_OFFSET_Y, 
                            screen_x + e->width * health_percent, e->y - ENEMY_HEALTH_BAR_OFFSET_Y + ENEMY_HEALTH_BAR_HEIGHT,
                            al_map_rgb((unsigned char)(255 * (1-health_percent)), (unsigned char)(255 * health_percent), 0)); // Green to Red gradient
                    }
                }
            }
            float player_screen_x = game->player.x - current->scroll_x;
            al_draw_filled_circle(player_screen_x + game->player.width/2, 
                                game->player.y + game->player.height/2, 
                                game->player.width/2, COLOR_PINKISH_RED); // Player color
            // Player Health Bar
            float health_percent = game->player.health / game->player.max_health;
            al_draw_filled_rectangle(PLAYER_HUD_HEALTH_X, PLAYER_HUD_HEALTH_Y, 
                                     PLAYER_HUD_HEALTH_X + PLAYER_HUD_HEALTH_WIDTH_MAX * health_percent, 
                                     PLAYER_HUD_HEALTH_Y + PLAYER_HUD_HEALTH_HEIGHT,
                                     al_map_rgb((unsigned char)(255 * (1-health_percent)), (unsigned char)(255 * health_percent), 0)); // Green to Red gradient
            // HUD Text (Level and Score)
            char level_text[32];
            sprintf(level_text, "Level: %d  Score: %d", game->current_level, game->score);
            al_draw_text(game->font, COLOR_WHITE, HUD_TEXT_X, HUD_TEXT_Y, ALLEGRO_ALIGN_LEFT, level_text);
            al_flip_display();
            break;
        default:
            break;
    }
}
