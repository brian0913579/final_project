# Cancer Cell Adventure

An educational game where you play as a cancer cell navigating through the human body while avoiding the immune system.

## Game Description
Navigate through different environments of the human body while avoiding various types of immune cells:
- T-cells that actively chase you
- Macrophages that patrol specific areas
- B-cells that shoot antibodies
- NK cells as boss enemies

## Features
- Three unique levels with increasing difficulty
- Multiple enemy types with different AI behaviors
- Health and score system
- Settings customization (difficulty, sound, music)
- Smooth platformer physics
- Level progression system

## Controls
- Arrow Keys: Move left/right
- Space: Jump
- ESC: Pause game
- M (while paused): Return to main menu

## Building from Source

### Prerequisites
```bash
# On macOS
brew install allegro pkg-config

# On Linux (Ubuntu/Debian)
sudo apt-get install liballegro5-dev
```

### Building
```bash
make clean && make
```

### Running
```bash
./cancer_cell_game
```

## Game Structure
- `src/main.c`: Main game implementation
- `include/game.h`: Game structures and declarations
- `resources/`: Game resources (sprites, sounds)

## Development Status
Currently implementing:
- Level designs
- Enemy AI behaviors
- Score system
- Sound effects and music