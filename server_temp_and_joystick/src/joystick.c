#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define LIMIAR_BAIXO 1500
#define LIMIAR_CENTRO_MIN 1500
#define LIMIAR_CENTRO_MAX 2500
#define LIMIAR_ALTO 2500

void setup() {
    stdio_init_all();        // Inicializa USB para printf()
    adc_init();              // Inicializa ADC
    adc_gpio_init(26);       // GP26 = eixo Y
    adc_gpio_init(27);       // GP27 = eixo X
}

void rosa_dos_ventos(uint16_t x, uint16_t y) {
    if (x < LIMIAR_BAIXO && y > LIMIAR_ALTO) {
        printf("↖️  Noroeste (NO)\n");
    } else if (x > LIMIAR_ALTO && y > LIMIAR_ALTO) {
        printf("↗️  Nordeste (NE)\n");
    } else if (x < LIMIAR_BAIXO && y < LIMIAR_BAIXO) {
        printf("↙️  Sudoeste (SO)\n");
    } else if (x > LIMIAR_ALTO && y < LIMIAR_BAIXO) {
        printf("↘️  Sudeste (SE)\n");
    } else if (x < LIMIAR_BAIXO) {
        printf("⬅️  Oeste (O)\n");
    } else if (x > LIMIAR_ALTO) {
        printf("➡️  Leste (L)\n");
    } else if (y > LIMIAR_ALTO) {
        printf("⬆️  Norte (N)\n");
    } else if (y < LIMIAR_BAIXO) {
        printf("⬇️  Sul (S)\n");
    } else {
        printf("🔘 Centro\n");
    }
}

const char *direcao_string(uint16_t x, uint16_t y)
{
    if (x < LIMIAR_BAIXO && y > LIMIAR_ALTO)
    {
        return "noroeste";
    }
    else if (x > LIMIAR_ALTO && y > LIMIAR_ALTO)
    {
        return "nordeste";
    }
    else if (x < LIMIAR_BAIXO && y < LIMIAR_BAIXO)
    {
        return "sudoeste";
    }
    else if (x > LIMIAR_ALTO && y < LIMIAR_BAIXO)
    {
        return "sudeste";
    }
    else if (x < LIMIAR_BAIXO)
    {
        return "oeste";
    }
    else if (x > LIMIAR_ALTO)
    {
        return "leste";
    }
    else if (y > LIMIAR_ALTO)
    {
        return "norte";
    }
    else if (y < LIMIAR_BAIXO)
    {
        return "sul";
    }
    else
    {
        return "centro";
    }
}

