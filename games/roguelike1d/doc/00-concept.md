Below is a **complete, internally consistent rule set** for a **One-Dimensional Rogue-like** that is *natively compatible* with **8 seven-segment digits**, optional LEDs, and a **4×4 button matrix / IR remote**. This is designed as a **serious firmware project**, not a novelty.

I will specify **world model, encoding, turn rules, combat, progression, and failure states** with enough precision to implement directly.

---

# 1. World Model

### Spatial layout

* The world is a **linear dungeon corridor** of length **L** (e.g. 32–128 cells).
* Only **8 adjacent cells** are visible at once (mapped to the 8 digits).
* Player always occupies **exactly one cell**.

### Viewport

* The 8 digits show:

```
[D−3][D−2][D−1][P][D+1][D+2][D+3][D+4]
```

* The viewport scrolls as the player moves.

---

# 2. Cell Encoding (core idea)

Each cell is represented by **one seven-segment digit**.
Segments encode **orthogonal state**, not graphics.

### Segment semantics

```
Segment meaning (recommended):

 a  → Enemy present
 b  → Trap present
 c  → Loot present
 d  → Door / locked state
 e  → Environmental hazard
 f  → Darkness / unknown
 g  → Player marker (only on player cell)
```

This allows **simultaneous information** per cell.

Example:

* `a + c` → enemy guarding loot
* `b + e` → trapped hazard zone
* `f only` → unexplored

---

# 3. Player State Encoding

### Player cell

* Always has **segment g ON**
* Other segments encode **current threats**

### Health

* Stored numerically
* Displayed via:

  * Side LEDs **or**
  * Temporarily replacing digits on status screen

### Inventory

* Abstract, slot-based (no list rendering)
* Items modify rules, not visuals

---

# 4. Turn Model (deterministic)

The game is **turn-based**, not real-time.

### One turn consists of:

1. Player action
2. World resolution
3. Status update

No background timers are required.

---

# 5. Player Actions

### Movement

* **Left / Right**
* Costs 1 turn

### Interact (OK / A)

Context-dependent:

* Enemy → attack
* Loot → collect
* Door → unlock
* Trap → attempt disarm

### Wait

* Skip turn (useful for baiting enemies)

---

# 6. Combat Rules (simple but deep)

### Enemy stats

Each enemy has:

* HP (1–5)
* Damage (1–3)
* Behavior flag

### Combat resolution

* Combat is **deterministic**, no RNG:

```
Player attacks → enemy HP − player ATK
Enemy attacks → player HP − enemy ATK
```

* Order depends on:

  * Player initiative stat
  * Item effects

### Death

* Enemy death clears segment `a`
* Loot may appear (`c`)

---

# 7. Trap & Hazard Mechanics

### Traps (`b`)

* Trigger when entering cell
* Effects:

  * HP damage
  * Status effect
  * Item loss

### Environmental hazard (`e`)

* Persistent effect:

  * Poison
  * Radiation
  * Darkness spread

Hazards stack over time.

---

# 8. Exploration & Fog of War

### Darkness (`f`)

* Unexplored cells start with `f`
* Entering cell clears `f`
* Some enemies re-darken cells

This allows **map memory gameplay**.

---

# 9. Loot & Progression

### Loot (`c`)

Types:

* Heal
* Damage boost
* Trap disarm
* Vision (clears nearby `f`)

### Level progression

* Corridor divided into “zones”
* Each zone introduces:

  * New segment combinations
  * Harder enemies

---

# 10. Doors & Locks (`d`)

### Doors

* Block movement
* Require:

  * Key item
  * Health sacrifice
  * Puzzle solution

Doors act as **soft checkpoints**.

---

# 11. Enemy AI (simple but expressive)

Enemy behaviors:

* Static
* Patrol (left/right)
* Chase when visible
* Ambush (reveals late)

Movement is:

* One cell per turn
* Resolved after player move

---

# 12. Win & Lose Conditions

### Lose

* Player HP ≤ 0
* Certain status effect threshold

### Win

* Reach exit cell
* Defeat final enemy
* Collect required artifact

---

# 13. Example Turn (concrete)

Player moves right:

1. Cell has `a + b`
2. Trap triggers (HP −1)
3. Enemy attacks (HP −2)
4. Player counterattacks
5. Cell updates:

   * `b` cleared
   * `a` remains or clears

Display updates accordingly.

---

# 14. Optional Advanced Mechanics

### Corruption

* Over time, segments randomly flip
* Forces urgency

### Permadeath

* Save minimal stats to flash
* New run starts harder

### IR Remote Debug Mode

* Reveal map
* Spawn enemies
* Step turns manually

---

# 15. Why this works extremely well on 7-segment

* **Segments ≠ pixels**
* Every lit segment is meaningful
* Information density is high
* Player learns symbolic language

This is **true hardware-aware game design**.

---

# Next steps

I can:

1. Define **exact bitmask layouts**
2. Design **cell struct + world array**
3. Propose **input handling tables**
4. Draft **render/update pipeline**
5. Create a **minimal playable ruleset first**

Tell me which layer you want to implement first.
