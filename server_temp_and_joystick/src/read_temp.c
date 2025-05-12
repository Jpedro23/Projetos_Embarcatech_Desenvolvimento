#include "read_temp.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"


float ler_temperatura()
{
    adc_select_input(4);
    sleep_us(10);
    uint16_t raw = adc_read();
    const float conversion = 3.3f / (1 << 12);
    float voltage = raw * conversion;
    return 27.0f - (voltage - 0.706f) / 0.001721f;
}

float media_temperatura()
{
    float soma = 0;
    for (int i = 0; i < 10; i++)
    {
        soma += ler_temperatura();
        sleep_ms(10);
    }
    return soma / 10.0f;
}

