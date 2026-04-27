#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"

// ===== DEFINICIÓN DE PINES =====

#define LED_R GPIO_NUM_5
#define LED_G GPIO_NUM_17
#define LED_B GPIO_NUM_18

#define ADC_CHANNEL ADC1_CHANNEL_4  // GPIO32

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

    // ===== CONFIGURACIÓN DEL ADC (LDR) =====
    
    adc1_config_width(ADC_WIDTH_BIT_12);          // Valores de 0 a 4095.
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_12);

 
    while (1)
    {
        // ===============================
        // MEDICIÓN CON LUZ ROJA
        // ===============================
        
        gpio_set_level(LED_R, 1);
        gpio_set_level(LED_G, 0);
        gpio_set_level(LED_B, 0);

        vTaskDelay(pdMS_TO_TICKS(500)); // tiempo para estabilizar luz. Se subió un poco para que los leds no vayan tan rápido.

        R = adc1_get_raw(ADC_CHANNEL);

        // ===============================
        // MEDICIÓN CON LUZ VERDE
        // ===============================
        gpio_set_level(LED_R, 0);
        gpio_set_level(LED_G, 1);
        gpio_set_level(LED_B, 0);

        vTaskDelay(pdMS_TO_TICKS(500));

        G = adc1_get_raw(ADC_CHANNEL);

        // ===============================
        // MEDICIÓN CON LUZ AZUL
        // ===============================
        gpio_set_level(LED_R, 0);
        gpio_set_level(LED_G, 0);
        gpio_set_level(LED_B, 1);

        vTaskDelay(pdMS_TO_TICKS(500));

        B = adc1_get_raw(ADC_CHANNEL);

        // ===============================
        // APAGAR TODOS LOS LEDs
        // ===============================
        
        gpio_set_level(LED_R, 0);
        gpio_set_level(LED_G, 0);
        gpio_set_level(LED_B, 0);

        // ===============================
        // MOSTRAR VALORES
        // ===============================
        
        printf("R: %d | G: %d | B: %d\n", R, G, B);

        // ===============================
        // CLASIFICACIÓN DE COLOR
        // ===============================

        // AZUL 
        if (B > R && B > G)
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
        

        printf("-----------------------------\n");

        // Tiempo entre mediciones.
      
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
