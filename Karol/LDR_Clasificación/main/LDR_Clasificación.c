#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"

// ===== DEFINICIÓN DE PINES =====

#define LED_R GPIO_NUM_5
#define LED_G GPIO_NUM_17
#define LED_B GPIO_NUM_18

#define BTN_1 GPIO_NUM_25  // Botón rojo.
#define BTN_2 GPIO_NUM_13  // Botón verde.
#define BTN_3 GPIO_NUM_14  // Botón azul.

#define ADC_CHANNEL ADC1_CHANNEL_4  // GPIO32.

// ===== VARIABLES =====
int R = 0, G = 0, B = 0;

void app_main(void)
{
    // ===== CONFIGURACIÓN DE LEDs =====
    
    gpio_reset_pin(LED_R);
    gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_G);
    gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_B);
    gpio_set_direction(LED_B, GPIO_MODE_OUTPUT);

    // ===== CONFIGURACIÓN DE BOTONES =====
    
    gpio_reset_pin(BTN_1);
    gpio_set_direction(BTN_1, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_1, GPIO_PULLUP_ONLY);

    gpio_reset_pin(BTN_2);
    gpio_set_direction(BTN_2, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_2, GPIO_PULLUP_ONLY);

    gpio_reset_pin(BTN_3);
    gpio_set_direction(BTN_3, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_3, GPIO_PULLUP_ONLY);

    // ===== CONFIGURACIÓN ADC (LDR) =====
    
    adc1_config_width(ADC_WIDTH_BIT_12);          // Rango 0–4095.
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_12);

  
    while (1)
    {
        // Leer botones.
        int b1 = gpio_get_level(BTN_1);
        int b2 = gpio_get_level(BTN_2);
        int b3 = gpio_get_level(BTN_3);

        // ===== MEDICIÓN POR COLOR =====

        // ROJO
        if (b1 == 0)
        {
            gpio_set_level(LED_R, 1);
            gpio_set_level(LED_G, 0);
            gpio_set_level(LED_B, 0);

            vTaskDelay(pdMS_TO_TICKS(100)); // tiempo para estabilizar luz.

            R = adc1_get_raw(ADC_CHANNEL);
        }

        // VERDE
        else if (b2 == 0)
        {
            gpio_set_level(LED_R, 0);
            gpio_set_level(LED_G, 1);
            gpio_set_level(LED_B, 0);

            vTaskDelay(pdMS_TO_TICKS(100));

            G = adc1_get_raw(ADC_CHANNEL);
        }

        // AZUL
        else if (b3 == 0)
        {
            gpio_set_level(LED_R, 0);
            gpio_set_level(LED_G, 0);
            gpio_set_level(LED_B, 1);

            vTaskDelay(pdMS_TO_TICKS(100));

            B = adc1_get_raw(ADC_CHANNEL);
        }

        else
        {
            // Apagar LEDs si no se presiona ningún botón.
            gpio_set_level(LED_R, 0);
            gpio_set_level(LED_G, 0);
            gpio_set_level(LED_B, 0);
        }

        // ===== MOSTRAR VALORES =====
        
        printf("R: %d | G: %d | B: %d\n", R, G, B);

        // ===== CLASIFICACIÓN MEJORADA =====
        
        // Se usa margen para evitar errores por ruido.

        // AZUL 
        if (B > R + 40 && B > G + 40)
        {
            printf("COLOR DETECTADO: AZUL\n");
        }

        // VERDE
        else if (G > R && G > B)
        {
            printf("COLOR DETECTADO: VERDE\n");
        }

        // ROJO
        else if (R > G && R > B)
        {
            printf("COLOR DETECTADO: ROJO\n");
        }

        // Caso no claro
        else
        {
            printf("COLOR DETECTADO: DESCONOCIDO\n");
        }

        printf("-----------------------------\n");

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
