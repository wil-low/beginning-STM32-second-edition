#include <stdint.h>

#define TM1638_NUM_KEYS 8

typedef struct {
    uint8_t stable; // debounced key state (bitmask)
    uint8_t counter[TM1638_NUM_KEYS];
} TM1638_Debounce_t;

void TM1638_DebounceUpdate(TM1638_Debounce_t *db, uint8_t raw);
