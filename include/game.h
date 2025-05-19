#ifndef GAME_H
#define GAME_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/keyboard.h> // Changed to keyboard.h based on directory listing

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600
#define FPS            60.0
#define PLATFORM_JUMP_TOLERANCE 5 // Pixels tolerance for standing on a platform
#define PORTAL_WIDTH 50
#define PORTAL_HEIGHT 80

#define GRAVITY 0.5
#define JUMP_SPEED -10.0
#define MOVE_SPEED 5.0

// Game states
typedef enum {
    WELCOME_SCREEN,
    MAIN_MENU,
    LEVEL_SELECT,
    SETTINGS,
    PLAYING,
    PAUSED,
    GAME_OVER,
    LEVEL_COMPLETE, // Added new state
    VICTORY
} GameState;

// Entity types
typedef enum {
    CANCER_CELL,
    T_CELL,
    MACROPHAGE,
    B_CELL,
    NK_CELL
} EntityType;

// Entity states
typedef enum {
    IDLE,
    MOVING,
    JUMPING,
    ATTACKING,
    DAMAGED,
    DEAD
} EntityState;

// Entity behavior types
typedef enum {
    BEHAVIOR_NONE,
    BEHAVIOR_PATROL,    // Move back and forth
    BEHAVIOR_CHASE,     // Chase the player
    BEHAVIOR_SHOOT,     // Shoot projectiles
    BEHAVIOR_BOSS       // Complex boss behavior
} EntityBehavior;

// Portal structure
typedef struct {
    float x, y;
    float width, height;
    bool is_active; // Might be useful later
} Portal;

// Structure for game entities (player and enemies)
typedef struct {
    float x, y;           // Position
    float dx, dy;         // Velocity
    float width, height;  // Size
    bool active;          // Is entity active?
    EntityType type;      // Type of entity
    EntityState state;    // Current state
    EntityBehavior behavior; // AI behavior type
    float health;        // Current health
    float max_health;    // Maximum health
    float attack_power;  // Damage dealt
    float attack_speed;  // Attack rate
    float last_attack;   // Time since last attack
    ALLEGRO_BITMAP* sprite;
    ALLEGRO_BITMAP* sprite_sheet; // For animations
    int current_frame;   // Current animation frame
    float frame_timer;   // Animation timer
    bool is_on_ground;    // True if the entity is on a platform
} Entity;

// Structure for game state
#define MAX_MENU_TEXT 64

// Menu item structure
typedef struct {
    char text[MAX_MENU_TEXT];
    bool selected;
    bool enabled;
} MenuItem;

// Menu structure
typedef struct {
    MenuItem* items;
    int num_items;
    int selected_index;
} Menu;

// Platform structure
typedef struct {
    float x, y;
    float width, height;
    ALLEGRO_COLOR color;
    bool is_deadly;     // Spikes or other hazards
} Platform;

// Level structure
typedef struct {
    Platform* platforms;
    Entity* enemies;
    int num_platforms;
    int num_enemies;
    ALLEGRO_BITMAP* background;
    float scroll_x;      // For scrolling levels
    float level_width;   // Total level width
    char* level_name;
    char* level_description;
    Portal portal;       // Added portal to level structure
} Level;

// Game settings
typedef struct {
    int difficulty;      // 1: Easy, 2: Normal, 3: Hard
    bool sound_enabled;
    bool music_enabled;
} GameSettings;

// Game structure
typedef struct {
    GameState state;
    Entity player;
    Level* levels;       // Array of levels
    int num_levels;
    Level* current_level_data;  // Pointer to current level
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* event_queue;
    ALLEGRO_TIMER* timer;
    ALLEGRO_FONT* font;
    ALLEGRO_FONT* title_font;
    ALLEGRO_SAMPLE* jump_sound;
    ALLEGRO_SAMPLE* hit_sound;
    ALLEGRO_SAMPLE* death_sound;
    ALLEGRO_SAMPLE_INSTANCE* music_instance;
    bool running;
    int score;
    int current_level;
    Menu main_menu;
    Menu level_menu;
    Menu settings_menu;
    GameSettings settings;
} Game;

// Function declarations
bool init_game(Game* game);
void init_menus(Game* game);
void init_levels(Game* game);
void init_level(Level* level, const char* name, const char* description, float width); // Changed from create_level
void init_level_content(Level* level, int level_number);
void update_game(Game* game);
void update_enemy(Entity* enemy, Game* game);
bool check_collision(Entity* a, Entity* b);
void handle_collisions(Game* game);
void draw_game(Game* game);
void draw_menu(Game* game, Menu* menu, const char* title);
void draw_welcome_screen(Game* game);
void draw_main_menu(Game* game);
void draw_level_select(Game* game);
void draw_settings_menu(Game* game);
void draw_pause_screen(Game* game);
void handle_input(Game* game, ALLEGRO_EVENT* event);
void handle_menu_input(Game* game, ALLEGRO_EVENT* event);
void cleanup_menus(Game* game);
void cleanup_level(Level* level);
void cleanup_levels(Game* game);
void cleanup_game(Game* game);

#endif /* GAME_H */
