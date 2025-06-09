#include "include/game.h"
#include "include/game_logic.h"
#include <stdio.h>
#include <assert.h>

// Simple test to verify star system functions work correctly
int main() {
    Game game = {0}; // Initialize game structure
    
    printf("Testing star system implementation...\n");
    
    // Test initialization
    init_star_system(&game);
    assert(game.total_stars == 0);
    assert(game.current_level_progress.stars_earned == 0);
    printf("✓ Star system initialization works\n");
    
    // Test star calculation
    game.current_level_progress.killed_normal_enemy = true;
    game.current_level_progress.killed_boss = true;
    game.current_level_progress.killed_all_enemies = false;
    
    int stars = calculate_stars(&game.current_level_progress);
    assert(stars == 2);
    printf("✓ Star calculation works (2/3 stars)\n");
    
    // Test finalization
    game.current_level = 1;
    finalize_level_stars(&game);
    assert(game.level_stars[0].stars_earned == 2);
    assert(game.total_stars == 2);
    printf("✓ Level star finalization works\n");
    
    // Test total calculation
    int total = calculate_total_stars(&game);
    assert(total == 2);
    printf("✓ Total star calculation works\n");
    
    printf("\nAll star system tests passed! ⭐\n");
    return 0;
}
