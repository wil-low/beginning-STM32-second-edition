Below is a **strictly minimal, playable ruleset** for the **One-Dimensional Rogue-like**, designed so you can implement it quickly, verify the concept on real hardware, and then extend it safely.

This version deliberately limits mechanics while preserving the **core symbolic language** of the seven-segment display.

---

# Minimal One-Dimensional Rogue-like (MVP Ruleset)

## 1. World

* Linear corridor of **24 cells**
* Only **8 cells visible** at a time
* Player starts at cell **0**
* Exit is at cell **23**

No scrolling tricks yet: viewport scrolls automatically when player moves.

---

## 2. Display Encoding (reduced)

Only **5 segments are used** in MVP.

```
Segment usage (MVP):

 a → Enemy
 b → Trap
 c → Loot
 f → Unknown (fog)
 g → Player
```

Unused segments: d, e

This keeps logic simple while still expressive.

---

## 3. Cell States

Each cell may contain **at most one feature**:

* Empty
* Enemy
* Trap
* Loot
* Exit (special case)

Initial state:

* All cells except start are `f` (unknown)
* Contents are revealed when entered

---

## 4. Player State

* **Health (HP):** starts at 5
* **Attack:** fixed at 1
* No inventory system yet

Health is shown:

* Temporarily on digits when changed **or**
* On side LEDs (recommended)

---

## 5. Turn Model (very important)

The game is **strictly turn-based**.

One turn = exactly **one button action**.

No timers, no background movement.

---

## 6. Input (minimal)

### Required buttons

* LEFT
* RIGHT
* OK

(IR remote can map to these later.)

---

## 7. Player Actions

### LEFT / RIGHT

* Attempt to move one cell
* Costs 1 turn

### OK

Context action:

* Enemy → attack
* Loot → collect
* Trap → disarm attempt

---

## 8. Movement Rules

* Player may move into:

  * Empty cell
  * Loot cell
  * Trap cell
  * Enemy cell (combat triggers)
* Exit cell ends the game (win)

Movement into a cell **reveals it** (clears `f`).

---

## 9. Combat (minimal, deterministic)

### Enemy

* HP = 2
* Damage = 1

### Combat resolution

When player enters or presses OK on enemy cell:

1. Enemy takes 1 damage
2. If enemy HP > 0:

   * Player takes 1 damage
3. If enemy HP reaches 0:

   * Enemy is removed
   * Cell becomes empty

No randomness.

---

## 10. Trap Rules

* Trap triggers when entering cell
* Player loses **1 HP**
* Trap is then removed

OK on trap:

* Disarm attempt
* 50% success (simple RNG)

  * Success → trap removed
  * Failure → trap triggers

(If you want **no RNG at all**, remove disarm and always trigger.)

---

## 11. Loot Rules

* Loot restores **+1 HP**
* Cannot exceed max HP (5)
* Loot disappears after collection

---

## 12. Fog of War (`f`)

* All cells start as unknown
* When player enters a cell:

  * `f` is cleared permanently
* No re-darkening yet

This gives exploration value without complexity.

---

## 13. Win / Lose Conditions

### Win

* Player enters exit cell

### Lose

* Player HP reaches 0

Game ends immediately.

---

## 14. Example Visible Frame

```
[f][f][ ][P][a][b][ ][c]
```

Meaning:

* Player at current digit
* Enemy ahead
* Trap after enemy
* Loot further ahead
* Unexplored beyond

---

## 15. World Generation (fixed for MVP)

Hardcode a simple map, e.g.:

| Cell | Content |
| ---: | ------- |
|    3 | Loot    |
|    5 | Enemy   |
|    7 | Trap    |
|   10 | Enemy   |
|   14 | Loot    |
|   18 | Enemy   |
|   23 | Exit    |

No procedural generation yet.

---

## 16. Rendering Rules (minimal)

* For each visible cell:

  * If unknown → show `f`
  * Else if player → show `g`
  * Else show segment matching content
* Never combine segments in MVP

This keeps rendering trivial.

---

## 17. Why this MVP is correct

This minimal ruleset already exercises:

* Bitmask-based display modeling
* Viewport scrolling
* Deterministic turn handling
* Context-sensitive input
* State persistence per cell

And it is **fully playable** with:

* 8 seven-segment digits
* 3 buttons

---

## Natural next extensions (after MVP works)

In safe order:

1. Allow **enemy + loot** combination
2. Add doors (`d`)
3. Add hazard over time (`e`)
4. Add inventory
5. Add procedural generation
6. Add FreeRTOS port

---

If you want next, I can:

* Provide **exact C structs and bitmasks**
* Define **render loop vs input loop**
* Draft **state machine diagram**
* Convert this into a **FreeRTOS task split**

Tell me which one to do next.
