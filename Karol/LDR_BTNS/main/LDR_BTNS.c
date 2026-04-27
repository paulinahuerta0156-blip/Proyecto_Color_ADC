#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"

// ===== DEFINICIÓN DE PINES =====
#define LED_R GPIO_NUM_5
#define LED_G GPIO_NUM_17
#define LED_B GPIO_NUM_18

#define BTN_1 GPIO_NUM_25   // Botón para cambiar modo.

#define ADC_CHANNEL ADC1_CHANNEL_4  // GPIO32

// ===== VARIABLES =====

int R = 0, G = 0, B = 0;
int modo = 0; // 0 = medición, 1 = clasificación.

int estado_anterior = 1; // Para detectar flanco del botón.

void app_main(void)
{
    // ===== CONFIGURACIÓN DE LEDs =====
    
    gpio_reset_pin(LED_R);
    gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_G);
    gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED_B);
    gpio_set_direction(LED_B, GPIO_MODE_OUTPUT);

    // ===== CONFIGURACIÓN DEL BOTÓN =====
    
    gpio_reset_pin(BTN_1);
    gpio_set_direction(BTN_1, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_1, GPIO_PULLUP_ONLY);

    // ===== CONFIGURACIÓN DEL ADC =====
    
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_12);

    while (1)
    {
        // ===============================
        // LECTURA DEL BOTÓN
        // ===============================
        
        int b1 = gpio_get_level(BTN_1);

        // Detecta solo cuando se presiona.
        
        if (b1 == 0 && estado_anterior == 1)
        {
            modo = !modo;
            printf("CAMBIO DE MODO\n");
        }

        estado_anterior = b1;

        // ===============================
        // MEDICIÓN CON LUZ ROJA
        // ===============================
        
        gpio_set_level(LED_R, 1);
        gpio_set_level(LED_G, 0);
        gpio_set_level(LED_B, 0);

        vTaskDelay(pdMS_TO_TICKS(500));

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
        // APAGAR LEDs
        // ===============================
        
        gpio_set_level(LED_R, 0);
        gpio_set_level(LED_G, 0);
        gpio_set_level(LED_B, 0);

        // ===============================
        // MOSTRAR SEGÚN MODO
        // ===============================
        
        if (modo == 0)
        {
            // MODO MEDICIÓN
            printf("MODO: MEDICION\n");
            printf("R: %d | G: %d | B: %d\n", R, G, B);
        }
        else
        {
            // MODO CLASIFICACIÓN
            printf("MODO: CLASIFICACION\n");

            if (B > R && B > G)
            {
                printf("COLOR: AZUL\n");
            }
            else if (G > R && G > B)
            {
                printf("COLOR: VERDE\n");
            }
            else
            {
                printf("COLOR: ROJO\n");
            }
        }

        printf("-----------------------------\n");
        
        // Tiempo entre mediciones.

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
