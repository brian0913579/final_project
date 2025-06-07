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

#define SCREEN_WIDTH    1280
#define SCREEN_HEIGHT   720
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
#define PLAYER_ATTACK_RANGE 50.0f        // Range for player attacks
#define PLAYER_ATTACK_COOLDOWN 30        // Frames between attacks (0.5 seconds at 60 FPS)

// Player Projectile System
#define PLAYER_PROJECTILE_SPEED 8.0f     // Player projectiles are faster than enemy ones
#define PLAYER_PROJECTILE_DAMAGE 20      // Player projectiles do more damage than melee
#define PLAYER_PROJECTILE_COOLDOWN 20    // Frames between shots (1/3 second at 60 FPS)
#define PLAYER_PROJECTILE_RANGE 400.0f   // Maximum range for player projectiles

// Advanced Jump Mechanics
#define COYOTE_TIME_FRAMES 8             // Frames player can still jump after leaving ground
#define JUMP_BUFFER_FRAMES 6             // Frames to buffer jump input before landing
#define WALL_JUMP_FRAMES 12              // Frames to detect wall contact for wall jumping
#define WALL_JUMP_HORIZONTAL_SPEED 7.0f  // Horizontal speed when wall jumping
#define WALL_JUMP_VERTICAL_SPEED -8.0f   // Vertical speed when wall jumping (slightly less than normal jump)

// Enhanced Combat System
#define COMBO_WINDOW_FRAMES 60           // Frames within which combos can be chained (1 second)
#define COMBO_DAMAGE_MULTIPLIER 1.5f     // Damage multiplier for combo attacks
#define MAX_COMBO_COUNT 5                // Maximum combo chain length
#define CRITICAL_HIT_CHANCE 0.15f        // 15% chance for critical hits
#define CRITICAL_HIT_MULTIPLIER 2.0f     // Damage multiplier for critical hits
#define KNOCKBACK_FORCE 8.0f            // Force applied to enemies when hit
#define KNOCKBACK_DURATION 15            // Frames enemies are knocked back
#define ATTACK_MOMENTUM_BOOST 2.0f       // Speed boost when attacking in movement direction

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
#define ENEMY_SHOOT_RANGE 250.0f       // Range for shooting behavior
#define ENEMY_SHOOT_COOLDOWN 90        // Frames between shots (1.5 seconds at 60 FPS)
#define ENEMY_BOSS_PHASE_HEALTH 0.5f   // Health percentage for boss phase change

// Advanced AI Behavior Constants
#define AI_FLANK_DISTANCE 150.0f       // Distance to maintain when flanking
#define AI_COORDINATION_RANGE 300.0f   // Range for enemy coordination
#define AI_PREDICTION_FRAMES 45        // Frames to predict player movement
#define AI_TACTICAL_PAUSE_CHANCE 0.3f  // 30% chance to pause tactically
#define AI_RETREAT_HEALTH_THRESHOLD 0.25f // Health % when enemies retreat
#define AI_GROUP_ATTACK_MIN_ENEMIES 2  // Minimum enemies for group tactics
#define AI_ADAPTIVE_DIFFICULTY_FACTOR 0.1f // How much AI adapts per player success
#define AI_PATHFINDING_LOOKAHEAD 5     // Steps to look ahead for pathfinding
#define AI_AGGRESSION_BASE 1.0f        // Base aggression multiplier
#define AI_LEARNING_RATE 0.05f         // How fast AI adapts to player behavior
#define AI_AMBUSH_WAIT_FRAMES 180      // Frames to wait before ambush (3 seconds)
#define AI_SURROUND_ANGLE_OFFSET 90.0f // Degrees offset for surrounding player

// Projectile System
#define MAX_PROJECTILES 50             // Maximum projectiles on screen
#define PROJECTILE_SPEED 5.0f          // Projectile movement speed
#define PROJECTILE_DAMAGE 15           // Damage dealt by projectiles
#define PROJECTILE_LIFETIME 300        // Frames before projectile expires (5 seconds at 60 FPS)
#define PROJECTILE_WIDTH 8.0f          // Projectile size
#define PROJECTILE_HEIGHT 8.0f

// Particle System
#define MAX_PARTICLES 100              // Maximum particles on screen
#define PARTICLE_LIFETIME_SHORT 30     // Short particle effect (0.5 seconds)
#define PARTICLE_LIFETIME_MEDIUM 60    // Medium particle effect (1 second)

// Enhanced particle effects constants
#define PARTICLE_LIFETIME_LONG 120         // Long particle effect (2 seconds)
#define ENEMY_DEATH_PARTICLES 15           // Number of particles when enemy dies
#define PROJECTILE_TRAIL_PARTICLES 2       // Particles per frame for projectile trails

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
    BEHAVIOR_BOSS,      // Complex boss behavior
    BEHAVIOR_FLANK,     // Flank around the player
    BEHAVIOR_COORDINATE, // Coordinate with other enemies
    BEHAVIOR_AMBUSH,    // Wait and ambush player
    BEHAVIOR_RETREAT,   // Retreat when low health
    BEHAVIOR_SURROUND   // Surround the player
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

// Projectile structure
typedef struct {
    float x, y;           // Position
    float dx, dy;         // Velocity
    float width, height;  // Size
    bool active;          // Is projectile active?
    int lifetime;         // Frames remaining before expiration
    int damage;           // Damage dealt by projectile
    EntityType source;    // Who fired the projectile
} Projectile;

// Particle structure for visual effects
typedef struct {
    float x, y;           // Position
    float dx, dy;         // Velocity
    ALLEGRO_COLOR color;  // Particle color
    int lifetime;         // Frames remaining
    int max_lifetime;     // For fading effect
    bool active;          // Is particle active?
} Particle;

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
    float last_attack;   // Time since last attack (also used for invincibility)
    float last_shot;     // Time since last projectile shot
    ALLEGRO_BITMAP* sprite;
    ALLEGRO_BITMAP* sprite_sheet; // For animations
    int current_frame;   // Current animation frame
    float frame_timer;   // Animation timer
    bool is_on_ground;    // True if the entity is on a platform
    bool jump_requested;  // True if jump input was made
    int coyote_time;      // Frames remaining for coyote time jump
    int jump_buffer;      // Frames remaining for jump buffer
    int wall_contact_left;  // Frames since touching left wall
    int wall_contact_right; // Frames since touching right wall
    
    // Enhanced Combat System Fields
    int combo_count;      // Current combo chain length
    int combo_timer;      // Frames since last combo attack
    float knockback_dx;   // Horizontal knockback velocity
    float knockback_dy;   // Vertical knockback velocity
    int knockback_timer;  // Frames remaining for knockback effect
    
    // Advanced AI Fields
    float ai_aggression;  // AI aggression multiplier (1.0 = normal)
    float ai_alertness;   // How aware this enemy is (0.0-1.0)
    int ai_state_timer;   // Timer for AI state changes
    float target_x, target_y; // AI target position
    int coordination_id;  // ID for coordinating with other enemies
    bool is_coordinating; // Whether this enemy is part of a group
    float flank_angle;    // Angle for flanking maneuvers
    int ambush_timer;     // Timer for ambush behavior
    bool player_spotted;  // Whether player has been detected
    float predicted_player_x, predicted_player_y; // Predicted player position
    int retreat_timer;    // Timer for retreat behavior
    EntityBehavior backup_behavior; // Behavior to return to after special actions
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
    Projectile* projectiles;    // Array of projectiles
    Particle* particles;        // Array of particles for visual effects
    int num_platforms;
    int num_enemies;
    int num_glucose_items; // Added for glucose items
    int num_projectiles;   // Active projectile count
    int num_particles;     // Active particle count
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

// Screen shake effect
typedef struct {
    float intensity;     // Current shake intensity
    int duration;        // Frames remaining
    float offset_x;      // Current screen offset
    float offset_y;
} ScreenShake;

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
    ALLEGRO_SAMPLE* collect_sound;
    ALLEGRO_SAMPLE* shoot_sound;
    ALLEGRO_SAMPLE_INSTANCE* music_instance;
    bool running;
    int score;
    int current_level;
    Menu main_menu;
    Menu level_menu;
    Menu settings_menu;
    GameSettings settings;
    ScreenShake screen_shake;    // Screen shake effect
    
    // Global AI State Tracking
    float global_ai_difficulty;   // Dynamic difficulty adjustment (starts at 1.0)
    int player_deaths;           // Track player deaths for AI adaptation
    int player_successful_dodges; // Track player skill for AI learning
    int enemies_killed_streak;   // Current kill streak for AI adaptation
    float ai_coordination_timer; // Global timer for enemy coordination
    bool ai_alert_mode;          // Whether all enemies are on high alert
    int ai_alert_timer;          // Timer for alert mode duration
} Game;

// Function declarations for core game setup and cleanup, if not in game_logic.h
// bool init_game(Game* game); // Moved to game_logic.h
// void cleanup_game(Game* game); // Moved to game_logic.h

// Declarations for other modules are now in their respective .h files
// e.g., drawing.h, entity.h, input.h, level.h, game_logic.h

// Projectile system function declarations
void create_projectile(Level* level, float x, float y, float target_x, float target_y, EntityType source);
void create_player_projectile(Level* level, float x, float y, float dx, float dy);
void update_projectiles(Level* level, Game* game);
void check_projectile_collisions(Level* level, Game* game);

// Particle system function declarations
void create_particle_burst(Level* level, float x, float y, ALLEGRO_COLOR color, int count);
void create_enemy_death_effect(Level* level, float x, float y, EntityType enemy_type);
void create_projectile_trail(Level* level, float x, float y, EntityType source);
void update_particles(Level* level);

// Screen shake effect function declarations
void create_screen_shake(Game* game, float intensity, int duration);
void update_screen_shake(Game* game);

// Advanced AI system function declarations
void init_ai_system(Game* game);
void update_global_ai_state(Game* game);
void predict_player_movement(Entity* enemy, Entity* player);
bool check_line_of_sight(Entity* enemy, Entity* player, Level* level);
void update_enemy_coordination(Game* game);
void apply_flanking_behavior(Entity* enemy, Entity* player, Game* game);
void apply_ambush_behavior(Entity* enemy, Entity* player, Game* game);
void apply_retreat_behavior(Entity* enemy, Entity* player, Game* game);
void apply_surround_behavior(Entity* enemy, Entity* player, Game* game, int enemy_index);
void adapt_ai_difficulty(Game* game, bool player_success);
float calculate_ai_aggression(Entity* enemy, Game* game);
bool should_coordinate_attack(Game* game, int enemy_index);
void trigger_ai_alert_mode(Game* game, int duration);

#endif /* GAME_H */
