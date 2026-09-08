# GameScript 2D Game Simulation Runtime

## 1. Overview
The GameScript runtime provides a deterministic 2D discrete grid simulation where compiled instructions alter game entities in real time.

```text
+------------------------+
| .  .  .  .  E  .  .  . |
| .  .  .  .  .  .  .  . |
| .  P^ .  .  .  #  .  . |
| .  .  .  .  .  #  .  . |
| .  .  .  .  E  .  .  . |
+------------------------+
```

### Legend:
- `P^`, `P>`, `Pv`, `P<` : Player entity indicating orientation (North, East, South, West).
- `E` : Enemy entity (with health and attack range).
- `#` : Impassable obstacle / terrain wall.
- `.` : Empty walkable cell.

---

## 2. Core Simulation Rules

1. **Grid Coordinates**:
   - Origin `(0, 0)` is at the top-left corner.
   - `+X` increases rightward (East), `+Y` increases downward (South).

2. **Player Movement & Boundaries**:
   - Movement forward moves in the direction of the current facing orientation.
   - Boundary checks prevent entities from stepping outside the `[0..Width-1, 0..Height-1]` grid.
   - Obstacles and alive enemy tiles block player step advancement.

3. **Turning**:
   - Degrees are normalized to 90-degree cardinal compass turns (North, East, South, West).

4. **Combat Resolution**:
   - Attack deals 25 base damage to enemies within an immediate 1-tile neighborhood or facing cone.
   - Defend raises a block stance, reducing incoming damage by 50%.
   - Defeated enemies (`HP <= 0`) are marked inactive and become passable tiles.
