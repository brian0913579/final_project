#include "../include/drawing.h"
#include "../include/game.h" // For Game, Level, Menu, Entity, Portal, constants
#include "../include/game_logic.h" // For star calculation functions
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
            char stars_text[64];
            sprintf(stars_text, "Total Stars: %d/%d", game->total_stars, MAX_STARS_PER_LEVEL * TOTAL_LEVELS);
            al_draw_text(game->font, COLOR_WHITE,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/GAMEOVER_TEXT_Y_DIVISOR, ALLEGRO_ALIGN_CENTRE, stars_text);
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
            
            // Show current level stars and total stars
            char level_stars_text[64];
            // Use current level progress for the stars (not yet finalized)
            int current_level_stars = calculate_stars(&game->current_level_progress);
            sprintf(level_stars_text, "Level %d Stars Earned:", game->current_level + 1);
            al_draw_text(game->font, COLOR_WHITE,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/LEVELCOMPLETE_TEXT_Y_DIVISOR + 60, ALLEGRO_ALIGN_CENTRE, level_stars_text);
            
            // Draw visual stars for current level
            draw_end_screen_stars(game, SCREEN_WIDTH/2, SCREEN_HEIGHT/LEVELCOMPLETE_TEXT_Y_DIVISOR + 100, current_level_stars, MAX_STARS_PER_LEVEL, game->current_level + 1);
            
            if (game->current_level < game->num_levels - 1) {
                 al_draw_text(game->font, COLOR_LIGHT_GRAY,
                            SCREEN_WIDTH/2, SCREEN_HEIGHT/LEVELCOMPLETE_TEXT_Y_DIVISOR + LEVELCOMPLETE_TEXT_SPACING_1 + 80, ALLEGRO_ALIGN_CENTRE, "Press N for Next Level");
            } else {
                 al_draw_text(game->font, COLOR_LIGHT_GRAY,
                            SCREEN_WIDTH/2, SCREEN_HEIGHT/LEVELCOMPLETE_TEXT_Y_DIVISOR + LEVELCOMPLETE_TEXT_SPACING_1 + 80, ALLEGRO_ALIGN_CENTRE, "Press N for Victory Screen");
            }
            al_draw_text(game->font, COLOR_LIGHT_GRAY,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/LEVELCOMPLETE_TEXT_Y_DIVISOR + LEVELCOMPLETE_TEXT_SPACING_2 + 80, ALLEGRO_ALIGN_CENTRE, "Press M for Main Menu");
            al_flip_display();
            break;
        case VICTORY:
            al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, al_map_rgba(255, 215, 0, ALPHA_OVERLAY_DARK)); // Gold-ish overlay
            al_draw_text(game->title_font, COLOR_WHITE,
                        SCREEN_WIDTH/2, SCREEN_HEIGHT/VICTORY_TITLE_Y_DIVISOR, ALLEGRO_ALIGN_CENTRE, "VICTORY!");
            
            // Show visual star breakdown by level
            float level_display_y = SCREEN_HEIGHT/VICTORY_TEXT_Y_DIVISOR + 50;
            float level_spacing = 80.0f;
            
            for (int level = 0; level < TOTAL_LEVELS; level++) {
                char level_text[32];
                sprintf(level_text, "Level %d", level + 1);
                al_draw_text(game->font, COLOR_WHITE,
                            SCREEN_WIDTH/2, level_display_y + level * level_spacing, ALLEGRO_ALIGN_CENTRE, level_text);
                
                // Draw visual stars for this level
                draw_end_screen_stars(game, SCREEN_WIDTH/2, level_display_y + level * level_spacing + 35, 
                                    game->level_stars[level].stars_earned, MAX_STARS_PER_LEVEL, level + 1);
            }
            
            al_draw_text(game->font, al_map_rgb(220, 220, 220), // Slightly off-white for variety
                        SCREEN_WIDTH/2, level_display_y + TOTAL_LEVELS * level_spacing + 20, ALLEGRO_ALIGN_CENTRE, "Press ENTER to return to menu");
            al_flip_display();
            break;
        case PLAYING:
            al_clear_to_color(COLOR_SKY_BLUE);
            Level* current = game->current_level_data;
            
            // Apply screen shake offset to camera
            float shake_offset_x = game->screen_shake.offset_x;
            float shake_offset_y = game->screen_shake.offset_y;
            
            // Draw backgrounds - check if level has multi-backgrounds
            if (current->num_backgrounds > 0 && current->backgrounds[0] != NULL) {
                // Multi-background system for level ONE
                for (int bg = 0; bg < current->num_backgrounds; bg++) {
                    if (current->backgrounds[bg]) {
                        float bg_start = current->background_positions[bg];
                        float bg_end = (bg < current->num_backgrounds - 1) ? 
                                      current->background_positions[bg + 1] : 
                                      current->level_width;
                        
                        // Check if this background section is visible
                        if (current->scroll_x < bg_end && current->scroll_x + SCREEN_WIDTH > bg_start) {
                            float draw_x = bg_start - current->scroll_x + shake_offset_x;
                            al_draw_bitmap(current->backgrounds[bg], draw_x, shake_offset_y, 0);
                        }
                    }
                }
            } else if (current->background) {
                // Single background system for regular levels
                al_draw_bitmap(current->background, -current->scroll_x + shake_offset_x, shake_offset_y, 0);
            }
            for (int i = 0; i < current->num_platforms; i++) {
                Platform* p = &current->platforms[i];
                float screen_x = p->x - current->scroll_x + shake_offset_x;
                float screen_y = p->y + shake_offset_y;
                if (screen_x + p->width >= 0 && screen_x <= SCREEN_WIDTH) {
                    al_draw_filled_rectangle(screen_x, screen_y, screen_x + p->width, screen_y + p->height, p->color);
                }
            }
            if (current->portal.is_active) {
                float portal_screen_x = current->portal.x - current->scroll_x + shake_offset_x;
                float portal_screen_y = current->portal.y + shake_offset_y;
                if (portal_screen_x + current->portal.width >= 0 && portal_screen_x <= SCREEN_WIDTH) {
                    al_draw_filled_rectangle(portal_screen_x, portal_screen_y,
                        portal_screen_x + current->portal.width, portal_screen_y + current->portal.height,
                        COLOR_PURPLE);
                    al_draw_rectangle(portal_screen_x, portal_screen_y,
                        portal_screen_x + current->portal.width, portal_screen_y + current->portal.height,
                        COLOR_WHITE, PORTAL_BORDER_THICKNESS);
                }
            }
            for (int i = 0; i < current->num_enemies; i++) {
                Entity* e = &current->enemies[i];
                if (!e->active) continue;
                float screen_x = e->x - current->scroll_x + shake_offset_x;
                float screen_y = e->y + shake_offset_y;
                if (screen_x + e->width >= 0 && screen_x <= SCREEN_WIDTH) {
                    ALLEGRO_COLOR enemy_color;
                    switch (e->type) {
                        case T_CELL: enemy_color = COLOR_YELLOW; break; // Bright Yellow
                        case MACROPHAGE: enemy_color = al_map_rgb(200, 200, 0); break; // Darker Yellow
                        case B_CELL: enemy_color = al_map_rgb(0, 200, 255); break; // Cyan/Light Blue
                        case NK_CELL: enemy_color = COLOR_RED; break; // Bright Red
                        default: enemy_color = COLOR_WHITE;
                    }
                    al_draw_filled_circle(screen_x + e->width/2, screen_y + e->height/2, e->width/2, enemy_color);
                    if (e->health < e->max_health) {
                        float health_percent = e->health / e->max_health;
                        al_draw_filled_rectangle(screen_x, screen_y - ENEMY_HEALTH_BAR_OFFSET_Y, 
                            screen_x + e->width * health_percent, screen_y - ENEMY_HEALTH_BAR_OFFSET_Y + ENEMY_HEALTH_BAR_HEIGHT,
                            al_map_rgb((unsigned char)(255 * (1-health_percent)), (unsigned char)(255 * health_percent), 0)); // Green to Red gradient
                    }
                }
            }

            // Draw Projectiles
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                Projectile* p = &current->projectiles[i];
                if (!p->active) continue;
                float screen_x = p->x - current->scroll_x + shake_offset_x;
                float screen_y = p->y + shake_offset_y;
                // Check if the projectile is on screen before drawing
                if (screen_x + p->width >= 0 && screen_x <= SCREEN_WIDTH) {
                    // Choose color based on source
                    ALLEGRO_COLOR projectile_color;
                    switch (p->source) {
                        case T_CELL: 
                        case MACROPHAGE: 
                        case B_CELL: 
                        case NK_CELL: 
                            projectile_color = al_map_rgb(255, 100, 100); // Red-ish for enemy projectiles
                            break;
                        case CANCER_CELL: 
                            projectile_color = al_map_rgb(0, 255, 255); // Cyan for player projectiles
                            break;
                        default: 
                            projectile_color = COLOR_WHITE;
                    }
                    
                    // Draw projectile as a small filled circle
                    al_draw_filled_circle(screen_x + p->width/2, screen_y + p->height/2, p->width/2, projectile_color);
                    
                    // Add different border colors for better distinction
                    ALLEGRO_COLOR border_color = (p->source == CANCER_CELL) ? al_map_rgb(255, 255, 255) : al_map_rgb(255, 0, 0);
                    al_draw_circle(screen_x + p->width/2, screen_y + p->height/2, p->width/2, border_color, 1.5f);
                }
            }

            // Draw Particles
            for (int i = 0; i < MAX_PARTICLES; i++) {
                Particle* p = &current->particles[i];
                if (!p->active) continue;
                float screen_x = p->x - current->scroll_x + shake_offset_x;
                float screen_y = p->y + shake_offset_y;
                // Check if the particle is on screen before drawing
                if (screen_x >= -10 && screen_x <= SCREEN_WIDTH + 10) {
                    // Draw particle as a small filled circle with fading alpha
                    al_draw_filled_circle(screen_x, screen_y, 2.0f, p->color);
                }
            }

            // Draw Glucose Items with pulsing effect
            for (int i = 0; i < current->num_glucose_items; i++) {
                GlucoseItem* g = &current->glucose_items[i];
                if (!g->active) continue;
                float screen_x = g->x - current->scroll_x + shake_offset_x;
                float screen_y = g->y + shake_offset_y;
                // Check if the glucose item is on screen before drawing
                if (screen_x + g->width >= 0 && screen_x <= SCREEN_WIDTH) {
                    // Create pulsing effect
                    float pulse = (1 + sin(al_get_time() * 4)) * 0.3f + 0.7f; // Pulse between 0.7 and 1.0
                    ALLEGRO_COLOR glucose_color = al_map_rgb(
                        (unsigned char)(255 * pulse), 
                        (unsigned char)(105 * pulse), 
                        (unsigned char)(180 * pulse)
                    );
                    
                    // Draw with slight size variation for pulsing effect
                    float size_mod = pulse * 2.0f;
                    al_draw_filled_rectangle(screen_x - size_mod, screen_y - size_mod, 
                                             screen_x + g->width + size_mod, screen_y + g->height + size_mod, 
                                             glucose_color);
                    
                    // Add a bright border for visibility
                    al_draw_rectangle(screen_x - size_mod, screen_y - size_mod,
                                     screen_x + g->width + size_mod, screen_y + g->height + size_mod,
                                     COLOR_WHITE, 1.0f);
                }
            }

            float player_screen_x = game->player.x - current->scroll_x + shake_offset_x;
            float player_screen_y = game->player.y + shake_offset_y;
            
            // Draw attack range indicator when attacking
            if (game->player.state == ATTACKING) {
                al_draw_circle(player_screen_x + game->player.width/2, 
                              player_screen_y + game->player.height/2,
                              PLAYER_ATTACK_RANGE, COLOR_RED, 2.0f);
            }
            
            // Draw shooting readiness indicator
            if (game->player.last_shot <= 0) {
                // Small green circle above player when ready to shoot
                al_draw_filled_circle(player_screen_x + game->player.width/2, 
                                    player_screen_y - 8, 3.0f, al_map_rgb(0, 255, 0));
            } else {
                // Red circle showing cooldown
                float cooldown_ratio = (float)game->player.last_shot / PLAYER_PROJECTILE_COOLDOWN;
                al_draw_filled_circle(player_screen_x + game->player.width/2, 
                                    player_screen_y - 8, 3.0f * cooldown_ratio, al_map_rgb(255, 0, 0));
            }
            
            // Draw combo indicator
            if (game->player.combo_count > 0 && game->player.combo_timer > 0) {
                // Combo chain indicator - growing glow around player
                float combo_intensity = (float)game->player.combo_count / MAX_COMBO_COUNT;
                ALLEGRO_COLOR combo_color = al_map_rgba(255, 255, 0, 
                                                      (unsigned char)(100 + combo_intensity * 155));
                
                for (int j = 0; j < game->player.combo_count && j < 5; j++) {
                    al_draw_circle(player_screen_x + game->player.width/2, 
                                 player_screen_y + game->player.height/2,
                                 game->player.width/2 + 5 + j * 3, combo_color, 2.0f);
                }
                
                // Combo counter text
                char combo_text[16];
                sprintf(combo_text, "x%d COMBO", game->player.combo_count);
                al_draw_text(game->font, al_map_rgb(255, 255, 0), 
                           player_screen_x + game->player.width/2, 
                           player_screen_y - 25, ALLEGRO_ALIGN_CENTER, combo_text);
            }
            
            // Draw player with state-based coloring
            ALLEGRO_COLOR player_color = COLOR_PINKISH_RED;
            if (game->player.last_attack > 0 && ((int)game->player.last_attack % 6) < 3) {
                // Invincibility flashing effect
                player_color = al_map_rgb(255, 150, 150);
            }
            
            al_draw_filled_circle(player_screen_x + game->player.width/2, 
                                player_screen_y + game->player.height/2, 
                                game->player.width/2, player_color);
            // Player Health Bar
            float health_percent = game->player.health / game->player.max_health;
            al_draw_filled_rectangle(PLAYER_HUD_HEALTH_X, PLAYER_HUD_HEALTH_Y, 
                                     PLAYER_HUD_HEALTH_X + PLAYER_HUD_HEALTH_WIDTH_MAX * health_percent, 
                                     PLAYER_HUD_HEALTH_Y + PLAYER_HUD_HEALTH_HEIGHT,
                                     al_map_rgb((unsigned char)(255 * (1-health_percent)), (unsigned char)(255 * health_percent), 0)); // Green to Red gradient
            
            // HUD Text (Level and Total Stars)
            char level_text[64];
            sprintf(level_text, "Level: %d  Total: %d/%d", 
                   game->current_level, game->total_stars, MAX_STARS_PER_LEVEL * TOTAL_LEVELS);
            al_draw_text(game->font, COLOR_WHITE, HUD_TEXT_X, HUD_TEXT_Y, ALLEGRO_ALIGN_LEFT, level_text);
            
            // Draw visual star display for current level progress (repositioned to top right)
            int current_level_progress = calculate_stars(&game->current_level_progress);
            draw_star_display(game, SCREEN_WIDTH - 100, 10, current_level_progress, MAX_STARS_PER_LEVEL);
            
            al_flip_display();
            break;
        default:
            break;
    }
}

// Visual star display function
void draw_star_display(Game* game, float x, float y, int stars_earned, int max_stars) {
    if (!game) return;
    
    float star_size = 24.0f; // Size of each star
    float star_spacing = 28.0f; // Spacing between stars
    
    // Use current level's star sprites (convert to 0-based index)
    int level_index = game->current_level - 1;
    if (level_index < 0 || level_index >= 3) level_index = 0; // Fallback to level 1 sprites
    
    for (int i = 0; i < max_stars && i < 3; i++) {
        float star_x = x + i * star_spacing;
        float star_y = y;
        
        // Use the same level sprite for all stars in that level
        ALLEGRO_BITMAP* star_bitmap = NULL;
        if (i < stars_earned) {
            // Star is earned - use filled sprite for this level
            star_bitmap = game->star_filled[level_index];
        } else {
            // Star not earned - use empty sprite for this level
            star_bitmap = game->star_empty[level_index];
        }
        
        if (star_bitmap) {
            // Scale the bitmap to our desired size
            al_draw_scaled_bitmap(star_bitmap,
                0, 0, // source start (top-left of image)
                al_get_bitmap_width(star_bitmap), // original width
                al_get_bitmap_height(star_bitmap), // original height
                star_x, star_y, // destination position on screen
                star_size, star_size, // scaled width and height
                0 // no special flags
            );
        } else {
            // Fallback to drawing simple colored circles if bitmaps failed to load
            ALLEGRO_COLOR star_color = (i < stars_earned) ? COLOR_YELLOW : COLOR_GRAY;
            al_draw_filled_circle(star_x + star_size/2, star_y + star_size/2, star_size/2 - 2, star_color);
            al_draw_circle(star_x + star_size/2, star_y + star_size/2, star_size/2 - 2, COLOR_WHITE, 1.0f);
        }
    }
}

// Visual star display function for end screens (centered)
void draw_end_screen_stars(Game* game, float center_x, float center_y, int stars_earned, int max_stars, int level) {
    if (!game) return;
    
    float star_size = 32.0f; // Larger stars for end screens
    float star_spacing = 40.0f; // More spacing between stars
    
    // Calculate starting position to center the stars
    float total_width = (max_stars - 1) * star_spacing + star_size;
    float start_x = center_x - total_width / 2.0f;
    
    // Use specified level's star sprites (convert to 0-based index)
    int level_index = level - 1;
    if (level_index < 0 || level_index >= 3) level_index = 0; // Fallback to level 1 sprites
    
    for (int i = 0; i < max_stars && i < 3; i++) {
        float star_x = start_x + i * star_spacing;
        float star_y = center_y - star_size / 2.0f;
        
        // Use the same level sprite for all stars in that level
        ALLEGRO_BITMAP* star_bitmap = NULL;
        if (i < stars_earned) {
            // Star is earned - use filled sprite for this level
            star_bitmap = game->star_filled[level_index];
        } else {
            // Star not earned - use empty sprite for this level
            star_bitmap = game->star_empty[level_index];
        }
        
        if (star_bitmap) {
            // Scale the bitmap to our desired size
            al_draw_scaled_bitmap(star_bitmap,
                0, 0, // source start (top-left of image)
                al_get_bitmap_width(star_bitmap), // original width
                al_get_bitmap_height(star_bitmap), // original height
                star_x, star_y, // destination position on screen
                star_size, star_size, // scaled width and height
                0 // no special flags
            );
        } else {
            // Fallback to drawing simple colored circles if bitmaps failed to load
            ALLEGRO_COLOR star_color = (i < stars_earned) ? COLOR_YELLOW : COLOR_GRAY;
            al_draw_filled_circle(star_x + star_size/2, star_y + star_size/2, star_size/2 - 2, star_color);
            al_draw_circle(star_x + star_size/2, star_y + star_size/2, star_size/2 - 2, COLOR_WHITE, 1.0f);
        }
    }
}
