# Example 2: Sensory-Driven Combat Tactics
# Demonstrates sensory predicates, branching logic (if/else), and combat maneuvers

set advance_speed = 3
player move forward advance_speed

if enemy nearby:
    player attack
    player move backward 1
    player defend
else:
    player move forward 2

if health low:
    player retreat
else:
    player attack
