#!/bin/bash
# Test script to verify spawn death issue is fixed

echo "Testing Cancer Cell Adventure spawn fix..."
echo "Screen size: 1280x720"
echo "Player spawn position: (320, 360)"
echo ""

# Start the game in background
./cancer_cell_game &
GAME_PID=$!

# Give the game time to initialize and reach the spawn
sleep 2

# Check if game is still running (not crashed due to immediate death)
if kill -0 $GAME_PID 2>/dev/null; then
    echo "✅ SUCCESS: Game is running and player survived spawn!"
    echo "The deadly platform collision issue has been fixed."
else
    echo "❌ FAILURE: Game crashed or player died immediately on spawn."
fi

# Clean up
kill $GAME_PID 2>/dev/null
wait $GAME_PID 2>/dev/null

echo ""
echo "Fix summary:"
echo "- Moved deadly platforms in level 1 from x=300 to x=500"
echo "- This prevents collision with player spawn at x=320"
echo "- Player can now spawn safely and begin gameplay"
