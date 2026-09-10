# Example 12: NPC Guard Behavior
# Demonstrates a combined behavior using movement, sensors,
# conditionals, combat, defense, and retreat

set patrol_steps = 2

repeat patrol_steps:
    if health low:
        player retreat
    else:
        if enemy nearby:
            player attack
            player move backward 1
        else:
            player move forward 1

player defend
