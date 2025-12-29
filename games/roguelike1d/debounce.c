#include "debounce.h"

#define DEBOUNCE_MAX 4 // 4 samples ≈ 20–40 ms depending on poll rate

void TM1638_DebounceUpdate(TM1638_Debounce_t *db, uint8_t raw) {
    for (uint8_t i = 0; i < TM1638_NUM_KEYS; i++) {
        uint8_t mask = 1 << i;

        if (raw & mask) {
            if (db->counter[i] < DEBOUNCE_MAX)
                db->counter[i]++;
        } else {
            if (db->counter[i] > 0)
                db->counter[i]--;
        }

        if (db->counter[i] == DEBOUNCE_MAX)
            db->stable |= mask;
        else if (db->counter[i] == 0)
            db->stable &= ~mask;
    }
}
