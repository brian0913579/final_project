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
#define PLATFORM_JUMP_TOLERANCE 8.0f // Pixels tolerance for standing on a platform, increased and made float
#define PORTAL_WIDTH 50
#define PORTAL_HEIGHT 80

// Common Colors
#define COLOR_BLACK al_map_rgb(0, 0, 0)
#define COLOR_WHITE al_map_rgb(255, 255, 255)
#define COLOR_YELLOW al_map_rgb(255, 255, 0)
#define COLOR_RED al_map_rgb(255, 0, 0)
#define COLOR_GREEN al_map_rgb(0, 255, 0)
#define COLOR_BLUE al_map_rgb(0, 0, 255)
#define COLOR_GRAY al_map_rgb(128, 128, 128)
#define COLOR_LIGHT_GRAY al_map_rgb(200, 200, 200)
#define COLOR_MEDIUM_GRAY al_map_rgb(150, 150, 150)
#define COLOR_SKY_BLUE al_map_rgb(135, 206, 235)
#define COLOR_PURPLE al_map_rgb(128, 0, 128)
#define COLOR_PINKISH_RED al_map_rgb(255, 100, 100) // For player

// UI Element Colors
#define MENU_TEXT_COLOR COLOR_WHITE
#define MENU_SELECTED_TEXT_COLOR COLOR_YELLOW
#define MENU_DISABLED_TEXT_COLOR COLOR_GRAY
#define MENU_BACKGROUND_COLOR COLOR_BLACK
#define TITLE_TEXT_COLOR COLOR_WHITE

// Alpha values for overlays (0-255)
#define ALPHA_OVERLAY_MEDIUM 128
#define ALPHA_OVERLAY_DARK 192

// UI Layout Constants
#define MENU_TITLE_Y 50
#define MENU_ITEM_START_Y 200
#define MENU_ITEM_SPACING 50

#define WELCOME_TITLE_Y_DIVISOR 3
#define WELCOME_SUBTEXT_Y_DIVISOR 2
#define WELCOME_VERSION_OFFSET_Y 50

#define PAUSE_TITLE_Y_DIVISOR 3
#define PAUSE_TEXT_Y_DIVISOR 2
#define PAUSE_TEXT_SPACING 40

#define GAMEOVER_TITLE_Y_DIVISOR 3
#define GAMEOVER_TEXT_Y_DIVISOR 2
#define GAMEOVER_TEXT_SPACING_1 50
#define GAMEOVER_TEXT_SPACING_2 90

#define LEVELCOMPLETE_TITLE_Y_DIVISOR 3
#define LEVELCOMPLETE_TEXT_Y_DIVISOR 2
#define LEVELCOMPLETE_TEXT_SPACING_1 50
#define LEVELCOMPLETE_TEXT_SPACING_2 90

#define VICTORY_TITLE_Y_DIVISOR 3
#define VICTORY_TEXT_Y_DIVISOR 2
#define VICTORY_TEXT_SPACING 50

#define PORTAL_BORDER_THICKNESS 2.0f

#define ENEMY_HEALTH_BAR_OFFSET_Y 10
#define ENEMY_HEALTH_BAR_HEIGHT 5

#define PLAYER_HUD_HEALTH_X 10
#define PLAYER_HUD_HEALTH_Y 40
#define PLAYER_HUD_HEALTH_WIDTH_MAX 200
#define PLAYER_HUD_HEALTH_HEIGHT 10

#define HUD_TEXT_X 10
#define HUD_TEXT_Y 10

// Game Mechanics Constants
#define GRAVITY 0.5f // Added f suffix for consistency
#define JUMP_SPEED -10.0f
#define MOVE_SPEED 5.0f

// Player Initial Stats
#define PLAYER_INITIAL_X_FACTOR (1.0f / 4.0f)
#define PLAYER_INITIAL_Y_FACTOR (1.0f / 2.0f)
#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 30
#define PLAYER_INITIAL_HEALTH 100
#define PLAYER_INITIAL_MAX_HEALTH 100
#define PLAYER_INITIAL_ATTACK_POWER 10
#define PLAYER_ATTACK_SPEED 0.5f // Attacks per second (lower is faster if used as a cooldown)
#define PLAYER_INVINCIBILITY_FRAMES 30 // Frames of invincibility after taking damage
#define PLAYER_KNOCKBACK_FORCE 10.0f

// Glucose Item Constants
#define GLUCOSE_WIDTH 20
#define GLUCOSE_HEIGHT 20
#define GLUCOSE_HEALTH_RECOVERY 25
#define COLOR_GLUCOSE al_map_rgb(255, 105, 180) // A pinkish color for glucose

#define MAX_STEP_UP_HEIGHT 8.0f    // Maximum height the player can automatically step up

// Game Progression
#define INITIAL_LEVEL 1
#define LEVEL_COMPLETE_SCORE_BONUS 1000

// Hazards
#define DEADLY_PLATFORM_DAMAGE 10

// Enemy Behavior
#define ENEMY_PATROL_DETECT_RANGE 200.0f
#define ENEMY_CHASE_SPEED 3.0f
#define ENEMY_CHASE_BREAK_RANGE 300.0f // Distance at which enemy stops chasing
#define ENEMY_PATROL_SPEED 2.0f

// Camera/Scrolling
#define SCROLL_X_PLAYER_OFFSET_FACTOR (1.0f / 3.0f) // Player position on screen before scrolling starts

// Resources
#define AUDIO_RESERVE_SAMPLES 8
#define FONT_SIZE_NORMAL 24
#define FONT_SIZE_TITLE 48
#define DEFAULT_FONT_PATH "/System/Library/Fonts/Helvetica.ttc" // Reverted to system font for testing

// Settings
#define DIFFICULTY_NORMAL 2
#define DIFFICULTY_EASY 1
#define DIFFICULTY_HARD 3


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

// Glucose Item structure
typedef struct {
    float x, y;
    float width, height;
    bool active;
} GlucoseItem;

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
    bool jump_requested;  // True if jump input was made
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
    GlucoseItem* glucose_items; // Added for glucose items
    int num_platforms;
    int num_enemies;
    int num_glucose_items; // Added for glucose items
    ALLEGRO_BITMAP* background;
    float scroll_x;
    float level_width;
    char* level_name;
    char* level_description;
    Portal portal;
    int id; // Added to store the level number (e.g., 1, 2, 3)
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

// Function declarations for core game setup and cleanup, if not in game_logic.h
// bool init_game(Game* game); // Moved to game_logic.h
// void cleanup_game(Game* game); // Moved to game_logic.h

// Declarations for other modules are now in their respective .h files
// e.g., drawing.h, entity.h, input.h, level.h, game_logic.h

#endif /* GAME_H */
