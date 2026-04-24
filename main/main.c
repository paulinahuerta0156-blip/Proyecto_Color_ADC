#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"

// Pines LEDs
#define LED_R GPIO_NUM_5
#define LED_G GPIO_NUM_17
#define LED_B GPIO_NUM_18

// ADC (GPIO 32)
#define ADC_CHANNEL ADC1_CHANNEL_4

void app_main(void)
{
    // Configurar LEDs como salida
    gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_B, GPIO_MODE_OUTPUT);

    // Configurar ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);

    int ambiente, r, g, b;
    int r_real, g_real, b_real;

    while (1)
    {
        //Ambiente (todo apagado)
        gpio_set_level(LED_R, 0);
        gpio_set_level(LED_G, 0);
        gpio_set_level(LED_B, 0);
        vTaskDelay(pdMS_TO_TICKS(200));

        ambiente = adc1_get_raw(ADC_CHANNEL);

        //Rojo
        gpio_set_level(LED_R, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        r = adc1_get_raw(ADC_CHANNEL);
        gpio_set_level(LED_R, 0);

        //Verde
        gpio_set_level(LED_G, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        g = adc1_get_raw(ADC_CHANNEL);
        gpio_set_level(LED_G, 0);

        //Azul
        gpio_set_level(LED_B, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        b = adc1_get_raw(ADC_CHANNEL);
        gpio_set_level(LED_B, 0);

        //Valores reales
        r_real = r - ambiente;
        g_real = g - ambiente;
        b_real = b - ambiente;

        //Mostrar resultados
        printf("\n-----------\n");
        printf("Ambiente: %d\n", ambiente);

        printf("R: %d | V: %d | A: %d\n", r, g, b);
        printf("R real: %d | V real: %d | A real: %d\n", r_real, g_real, b_real);

        //Detección (comparacion para definir max y min)
        int max = r_real;
        int min = r_real;

        if (g_real > max) max = g_real;
        if (b_real > max) max = b_real;

        if (g_real < min) min = g_real;
        if (b_real < min) min = b_real;

        //Resultados
        if (r_real < 80 && g_real < 80 && b_real < 80) //Umbral para cuando no hay objeto
        {
            printf("Resultado: SIN COLOR / MUY OSCURO\n");
        }
        else if ((max - min) < 250) //VERDE (diferencia porque asi funciona bien)
        {
            printf("Resultado: VERDE\n");
        }
        else if (r_real > b_real) //ROJO
        {
            printf("Resultado: ROJO\n");
        }
        else //AZUL
        {
            printf("Resultado: AZUL\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}