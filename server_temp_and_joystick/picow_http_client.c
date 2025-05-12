#include <stdio.h>
#include "pico/stdio.h"
#include "pico/cyw43_arch.h"
#include "pico/async_context.h"
#include "lwip/altcp_tls.h"
#include "example_http_client_util.h"
#include "lwipopts_examples_common.h"
#include "lwipopts.h"
#include "mbedtls_config_examples_common.h"
#include "mbedtls_config.h"
#include "hardware/adc.h"
#include "joystick.h"
#include "wifi.h"
#include "read_temp.h"

#define HOST "192.168.137.199"
#define PORT 5000
#define INTERVALO_MS 50
#define BUTTON_LEFT 5
#define BUTTON_RIGHT 6
#define LED_BLUE 12
#define LED_RED 13
#define LED_GREEN 11

#define ADC_Y 0 // ADC0 = GP26
#define ADC_X 1 // ADC1 = GP27

#define DEADZONE 300

#define LIMIAR_BAIXO 1500
#define LIMIAR_CENTRO_MIN 1500
#define LIMIAR_CENTRO_MAX 2500
#define LIMIAR_ALTO 2500

// =================== Funções auxiliares direcionamento do rosa dos ventos ======================

void rosa_dos_ventos(uint16_t x, uint16_t y);

const char *direcao_string(uint16_t x, uint16_t y);

// =================== Programa principal ======================

int main()
{
    stdio_init_all();
    adc_init();
    adc_set_temp_sensor_enabled(true);

    gpio_init(BUTTON_LEFT);
    gpio_set_dir(BUTTON_LEFT, GPIO_IN);
    gpio_pull_up(BUTTON_LEFT);

    gpio_init(BUTTON_RIGHT);
    gpio_set_dir(BUTTON_RIGHT, GPIO_IN);
    gpio_pull_up(BUTTON_RIGHT);

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    adc_gpio_init(26); // GP26 = eixo Y
    adc_gpio_init(27); // GP27 = eixo X

    // =================== Iniciação da comunição wi-fi ======================

    printf("\nIniciando cliente HTTP...\n");

    if (iniciar_wifi(WIFI_SSID, WIFI_PASSWORD) != 0)
    {
        return 1; // Erro de conexão...
    }

    // =================== inicio do loop principal ======================

    while (1)
    {
        float temperature = media_temperatura();
        char temp_path[128];
        snprintf(temp_path, sizeof(temp_path), "/temperature?value=%.2f", temperature);

        EXAMPLE_HTTP_REQUEST_T temp_req = {0};
        temp_req.hostname = HOST;
        temp_req.url = temp_path;
        temp_req.port = PORT;
        temp_req.headers_fn = http_client_header_print_fn;
        temp_req.recv_fn = http_client_receive_print_fn;

        printf("Enviando temperatura: %s\n", temp_path);
        http_client_request_sync(cyw43_arch_async_context(), &temp_req);


        adc_select_input(ADC_Y);
        sleep_us(10);
        uint16_t y = adc_read();

        adc_select_input(ADC_X);
        sleep_us(10);
        uint16_t x = adc_read();

        rosa_dos_ventos(x, y); // imprime a direção no terminal

        const char *direcao = direcao_string(x, y);
        char direction_path[128];
        const char *path = NULL;

        if (strcmp(direcao, "centro") != 0)
        {
            snprintf(direction_path, sizeof(direction_path), "/direction?dir=%s", direcao);
            path = direction_path;
            gpio_put(LED_RED, 1);
            sleep_ms(50);
            gpio_put(LED_RED, 0);
        }

        else if (gpio_get(BUTTON_LEFT) == 0)
        {
            path = "/pressed_button_a";
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_GREEN, 1);
            sleep_ms(50);
            gpio_put(LED_BLUE, 0);
            gpio_put(LED_GREEN, 0);
        }

        else if (gpio_get(BUTTON_RIGHT) == 0)
        {
            path = "/pressed_button_b";
            gpio_put(LED_BLUE, 1);
            gpio_put(LED_RED, 1);
            sleep_ms(50);
            gpio_put(LED_BLUE, 0);
            gpio_put(LED_RED, 0);
        }

        else if (gpio_get(BUTTON_RIGHT) != 0 && gpio_get(BUTTON_LEFT) != 0)
        {
            path = "/unpressed";
            sleep_ms(50);
        }


        // =================== IF que constroi o path que sera mandado para a rota do servirdor ======================

        if (path != NULL)
        {
            EXAMPLE_HTTP_REQUEST_T req = {0};
            req.hostname = HOST;
            req.url = path;
            req.port = PORT;
            req.headers_fn = http_client_header_print_fn;
            req.recv_fn = http_client_receive_print_fn;

            printf("Enviando comando: %s\n", path);
            int result = http_client_request_sync(cyw43_arch_async_context(), &req);

            if (result == 0)
            {
                printf("Comando enviado com sucesso!\n");
            }
            else
            {
                printf("Erro ao enviar comando: %d\n", result);
            }

            sleep_ms(50);
        }

        sleep_ms(INTERVALO_MS);
    }
}
