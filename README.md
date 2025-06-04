# Cancer Cell Adventure

A thrilling 2D platformer where you play as a cancer cell navigating through the human immune system!

## 🎮 Game Overview

In **Cancer Cell Adventure**, you control a cancer cell that must survive and navigate through three challenging levels representing different parts of the human immune system. Collect glucose items to restore health while avoiding deadly immune cells and environmental hazards.

## 🎯 Objective

- **Survive** encounters with immune system defenders
- **Collect glucose** items to maintain your health  
- **Navigate** through complex biological environments
- **Reach the portal** at the end of each level to progress

## 🕹️ Controls

| Key | Action |
|-----|--------|
| `A` | Move Left |
| `D` | Move Right |
| `W` / `Space` | Jump |
| `J` / `X` | Melee Attack (damages nearby enemies) |
| `Q` | Shoot Projectile (ranged attack) |
| `ESC` | Pause/Resume Game |
| `M` | Return to Main Menu (when paused) |
| `Enter` | Confirm selections |
| `↑/↓` | Navigate menus |

### 🎯 Combat System
- **Melee Attack**: Short-range attack with high damage (10 damage)
- **Projectile Attack**: Long-range cyan projectiles (20 damage)
- **Cooldowns**: Visual indicators show attack and shooting readiness
- **Targeting**: Projectiles travel in the direction you're facing

## 🏥 Levels

### Level 1: The Blood Stream
- Navigate through flowing blood vessels
- Avoid patrolling T-cells
- Master the basic movement mechanics
- **Enemies**: T-cells with patrol behavior
- **Glucose**: 3 items strategically placed

### Level 2: The Lymph Node  
- Navigate a maze-like immune fortress
- Face more aggressive immune responses
- Vertical platforming challenges
- **Enemies**: Mix of T-cells and Macrophages
- **Glucose**: 4 items in challenging locations

### Level 3: The Final Battle
- Multi-level arena combat
- Face the ultimate immune response
- Boss-level NK cells with advanced AI
- **Enemies**: All immune cell types including NK cell bosses
- **Glucose**: 5 items for the final challenge

## 🦠 Enemy Types

- **T-cells** (Yellow): Basic patrol behavior, moderate threat
- **Macrophages** (Dark Yellow): Stronger and more persistent  
- **B-cells** (Cyan): Defensive specialists
- **NK-cells** (Red): Elite killers with boss-level intelligence

## 💊 Game Mechanics

- **Health System**: Start with 100 HP, lose health from enemy contact and hazards
- **Combat System**: Attack nearby enemies with the S key for 10 damage  
- **Glucose Collection**: Each glucose item restores 25 HP
- **Invincibility Frames**: Brief protection after taking damage  
- **Step-up Mechanics**: Automatically climb small ledges
- **Progressive Difficulty**: Each level introduces new challenges
- **Scoring System**: Earn points for collecting glucose, damaging enemies, and completing levels

## 🛠️ Building & Running

### Prerequisites
- **Allegro 5** library installed
- **GCC** compiler
- **pkg-config** for dependency management

### macOS Installation
```bash
# Install Allegro 5 using Homebrew
brew install allegro

# Clone and build the project
git clone <repository-url>
cd final_project
make

# Run the game
./cancer_cell_game
```

### Linux Installation  
```bash
# Ubuntu/Debian
sudo apt-get install liballegro5-dev

# Build and run
make
./cancer_cell_game
```

## 📁 Project Structure

```
cancer_cell_game/
├── src/                # Source code files
│   ├── main.c         # Main game loop
│   ├── game_logic.c   # Core game mechanics
│   ├── input.c        # Input handling
│   ├── drawing.c      # Rendering system
│   ├── entity.c       # Enemy AI and collision
│   └── level.c        # Level management
├── include/           # Header files
├── resources/         # Game assets
│   ├── sprites/      # Background images
│   ├── sounds/       # Audio files (placeholder)
│   └── fonts/        # Font files (placeholder)
├── obj/              # Compiled object files
├── Makefile          # Build configuration
└── README.md         # This file
```

## 🎨 Technical Features

- **Modular Architecture**: Clean separation of concerns across multiple modules
- **Event-Driven Design**: Responsive input handling and game state management
- **Advanced Physics**: Gravity, collision detection, and step-up mechanics
- **Smart AI**: Multiple enemy behavior patterns (Patrol, Chase, Shoot, Boss)
- **Memory Management**: Proper allocation/deallocation with safety checks
- **Cross-Platform**: Supports macOS and Linux

## 🔧 Customization

The game is built with extensibility in mind:

- **Constants**: Easy to modify game parameters in `include/game.h`
- **Levels**: Add new levels by extending the level system
- **Enemies**: Create new enemy types and behaviors
- **Assets**: Replace placeholder graphics and add sound effects

## 🐛 Known Issues & Future Enhancements

- Audio system framework present but sounds not implemented
- Basic geometric rendering (ready for sprite replacement)
- Projectile system framework in place for shooting enemies

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Development

Developed as a C programming and game development project showcasing:
- Advanced C programming techniques
- Game engine architecture
- Real-time systems programming  
- Memory management best practices
- Cross-platform development

---

**Survive the immune system. Collect glucose. Become the ultimate cancer cell!** 🦠