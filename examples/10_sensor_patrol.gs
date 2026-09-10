# Example 10: Sensor-Based Patrol
# Demonstrates environmental sensing during a patrol

set patrol_steps = 3

repeat patrol_steps:
    if enemy nearby:
        player attack
    else:
        player move forward 1
