# Example 4: Subroutines and Reusable Behaviors
# Demonstrates function declarations and procedural calls

function ambush:
    player move forward 2
    player attack
    player move backward 1
    player defend

# Main sequence
set cycles = 2
repeat cycles:
    call ambush
    player turn right 90
