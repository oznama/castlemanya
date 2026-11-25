#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"

RGBColor interpolateColor(RGBColor c1, RGBColor c2, float t) {
    RGBColor resultado;
  
    resultado.r = (c1.r + t * (c2.r - c1.r));
    resultado.g = (unsigned char)(c1.g + t * (c2.g - c1.g));
    resultado.b = (unsigned char)(c1.b + t * (c2.b - c1.b));
    
    return resultado;
}

typedef struct {
    uint16_t value;
    uint16_t probability;
} MoneyDrop;

uint16_t drop_money(void)
{
    MoneyDrop moneys[3] = {
        {.value = 100, .probability = 20}, // 2%
        {.value = 50, .probability = 100}, // 10%
        {.value = 10, .probability = 400}  // 40%
    };

    uint16_t random_throw  = (rand() % 1000) + 1;
    for(uint8_t i=0; i<3; i++) {
        bool has_luck = random_throw  <= moneys[i].probability;
        if (has_luck) return moneys[i].value;
        random_throw -= moneys[i].probability;
    }

    return 0;
}