# Example 3: Iteration with Repeat and While
# Demonstrates bounded repeat loops and condition-driven while loops

set combo_hits = 3

# Execute a rapid melee combo
repeat combo_hits:
    player attack
    player move forward 1

# Tactical repositioning
player turn left 90
player move forward 2

while not enemy nearby:
    player move forward 1
