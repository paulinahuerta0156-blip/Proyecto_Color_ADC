#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"

// ===== CONFIGURACIÓN =====
#define LED_1 GPIO_NUM_5
#define LDR_CHANNEL ADC1_CHANNEL_4  // GPIO32

int umbral = 300; 

void app_main(void)
{
    // ===== CONFIGURAR LED =====
    
    gpio_reset_pin(LED_1);
    gpio_set_direction(LED_1, GPIO_MODE_OUTPUT);

    // ===== CONFIGURAR ADC =====
    
    adc1_config_width(ADC_WIDTH_BIT_12); // Rango 0–4095.
    adc1_config_channel_atten(LDR_CHANNEL, ADC_ATTEN_DB_12);

    while (1)
    {
        int valor = adc1_get_raw(LDR_CHANNEL);

        printf("LDR: %d\n", valor);

        // ===== LÓGICA =====
        
        if (valor < umbral)  // tapado = valor bajo
        {
            gpio_set_level(LED_PIN, 1); // LED ON.
        }
        else
        {
            gpio_set_level(LED_PIN, 0); // LED OFF.
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
