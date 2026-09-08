# Example 5: Compiler Optimization Demonstration
# Demonstrates Constant Folding, Constant Propagation, DCE, and Redundant Movement Elimination

# 1. Constant Folding: 4 * 2 + 2 -> evaluated at compile time to 10
set base_distance = 4 * 2 + 2

# 2. Constant Propagation: base_distance (10) substituted directly into movement
player move forward base_distance

# 3. Redundant Instruction Elimination: 0-step movement and 0-degree turns stripped out
player move forward 0
player turn left 0

# 4. Final action
player attack
