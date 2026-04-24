#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/i2c.h"

// Pines LEDs
#define LED_R GPIO_NUM_5
#define LED_G GPIO_NUM_17
#define LED_B GPIO_NUM_18

// ADC
#define ADC_CHANNEL ADC1_CHANNEL_4

// OLED I2C
#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define OLED_ADDR 0x3C

//CONFIGURACION DE LA PANTALLA
void i2c_master_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };

    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void oled_command(uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd};
    i2c_master_write_to_device(I2C_MASTER_NUM, OLED_ADDR, data, 2, pdMS_TO_TICKS(1000));
}

void oled_data(uint8_t *data, size_t len)
{
    uint8_t buffer[129];
    buffer[0] = 0x40;
    memcpy(&buffer[1], data, len);
    i2c_master_write_to_device(I2C_MASTER_NUM, OLED_ADDR, buffer, len + 1, pdMS_TO_TICKS(1000));
}

//INICIALIZACION DE LA PANTALLA
void oled_init(void)
{
    vTaskDelay(pdMS_TO_TICKS(100));

    oled_command(0xAE);
    oled_command(0xD5);
    oled_command(0x80);
    oled_command(0xA8);
    oled_command(0x1F);
    oled_command(0xD3);
    oled_command(0x00);
    oled_command(0x40);
    oled_command(0x8D);
    oled_command(0x14);
    oled_command(0x20);
    oled_command(0x00);
    oled_command(0xA1);
    oled_command(0xC8);
    oled_command(0xDA);
    oled_command(0x02);
    oled_command(0x81);
    oled_command(0xCF);
    oled_command(0xD9);
    oled_command(0xF1);
    oled_command(0xDB);
    oled_command(0x40);
    oled_command(0xA4);
    oled_command(0xA6);
    oled_command(0xAF);
}

void oled_clear(void)
{
    for (int page = 0; page < 4; page++)
    {
        oled_command(0xB0 + page);
        oled_command(0x00);
        oled_command(0x10);

        uint8_t data[128];
        memset(data, 0x00, 128);
        oled_data(data, 128);
    }
}

void oled_set_cursor(uint8_t page, uint8_t col)
{
    oled_command(0xB0 + page);
    oled_command(0x00 + (col & 0x0F));
    oled_command(0x10 + ((col >> 4) & 0x0F));
}

void get_char(char c, uint8_t font[5])
{
    memset(font, 0x00, 5);

//SWITCH PARA RELACIONAR PIXELES CON CADA CARACTER
    switch (c)
    {
        case '0': {uint8_t f[5]={0x3E,0x51,0x49,0x45,0x3E}; memcpy(font,f,5);} break;
        case '1': {uint8_t f[5]={0x00,0x42,0x7F,0x40,0x00}; memcpy(font,f,5);} break;
        case '2': {uint8_t f[5]={0x42,0x61,0x51,0x49,0x46}; memcpy(font,f,5);} break;
        case '3': {uint8_t f[5]={0x21,0x41,0x45,0x4B,0x31}; memcpy(font,f,5);} break;
        case '4': {uint8_t f[5]={0x18,0x14,0x12,0x7F,0x10}; memcpy(font,f,5);} break;
        case '5': {uint8_t f[5]={0x27,0x45,0x45,0x45,0x39}; memcpy(font,f,5);} break;
        case '6': {uint8_t f[5]={0x3C,0x4A,0x49,0x49,0x30}; memcpy(font,f,5);} break;
        case '7': {uint8_t f[5]={0x01,0x71,0x09,0x05,0x03}; memcpy(font,f,5);} break;
        case '8': {uint8_t f[5]={0x36,0x49,0x49,0x49,0x36}; memcpy(font,f,5);} break;
        case '9': {uint8_t f[5]={0x06,0x49,0x49,0x29,0x1E}; memcpy(font,f,5);} break;

        case 'A': {uint8_t f[5]={0x7E,0x11,0x11,0x11,0x7E}; memcpy(font,f,5);} break;
        case 'B': {uint8_t f[5]={0x7F,0x49,0x49,0x49,0x36}; memcpy(font,f,5);} break;
        case 'C': {uint8_t f[5]={0x3E,0x41,0x41,0x41,0x22}; memcpy(font,f,5);} break;
        case 'D': {uint8_t f[5]={0x7F,0x41,0x41,0x22,0x1C}; memcpy(font,f,5);} break;
        case 'E': {uint8_t f[5]={0x7F,0x49,0x49,0x49,0x41}; memcpy(font,f,5);} break;
        case 'F': {uint8_t f[5]={0x7F,0x09,0x09,0x09,0x01}; memcpy(font,f,5);} break;
        case 'G': {uint8_t f[5]={0x3E,0x41,0x49,0x49,0x7A}; memcpy(font,f,5);} break;
        case 'I': {uint8_t f[5]={0x00,0x41,0x7F,0x41,0x00}; memcpy(font,f,5);} break;
        case 'L': {uint8_t f[5]={0x7F,0x40,0x40,0x40,0x40}; memcpy(font,f,5);} break;
        case 'M': {uint8_t f[5]={0x7F,0x02,0x0C,0x02,0x7F}; memcpy(font,f,5);} break;
        case 'N': {uint8_t f[5]={0x7F,0x04,0x08,0x10,0x7F}; memcpy(font,f,5);} break;
        case 'O': {uint8_t f[5]={0x3E,0x41,0x41,0x41,0x3E}; memcpy(font,f,5);} break;
        case 'R': {uint8_t f[5]={0x7F,0x09,0x19,0x29,0x46}; memcpy(font,f,5);} break;
        case 'S': {uint8_t f[5]={0x46,0x49,0x49,0x49,0x31}; memcpy(font,f,5);} break;
        case 'T': {uint8_t f[5]={0x01,0x01,0x7F,0x01,0x01}; memcpy(font,f,5);} break;
        case 'U': {uint8_t f[5]={0x3F,0x40,0x40,0x40,0x3F}; memcpy(font,f,5);} break;
        case 'V': {uint8_t f[5]={0x1F,0x20,0x40,0x20,0x1F}; memcpy(font,f,5);} break;
        case 'Z': {uint8_t f[5]={0x61,0x51,0x49,0x45,0x43}; memcpy(font,f,5);} break;

        case ':': {uint8_t f[5]={0x00,0x36,0x36,0x00,0x00}; memcpy(font,f,5);} break;
        case '/': {uint8_t f[5]={0x20,0x10,0x08,0x04,0x02}; memcpy(font,f,5);} break;
        case '-': {uint8_t f[5]={0x08,0x08,0x08,0x08,0x08}; memcpy(font,f,5);} break;
        case ' ': default: break;
    }
}

void oled_write_char(char c)
{
    uint8_t font[5];
    get_char(c, font);

    uint8_t data[6];
    data[0] = font[0];
    data[1] = font[1];
    data[2] = font[2];
    data[3] = font[3];
    data[4] = font[4];
    data[5] = 0x00;

    oled_data(data, 6);
}

void oled_write_string(const char *str)
{
    while (*str)
    {
        oled_write_char(*str++);
    }
}

void oled_print_line(uint8_t line, const char *text)
{
    oled_set_cursor(line, 0);

    uint8_t clear[128];
    memset(clear, 0x00, 128);
    oled_data(clear, 128);

    oled_set_cursor(line, 0);
    oled_write_string(text);
}

//LECTURA DE COLORES MEDIANTE ADC
void app_main(void)
{
    gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_B, GPIO_MODE_OUTPUT);

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);

    i2c_master_init();
    oled_init();
    oled_clear();

    int ambiente, r, g, b;
    int r_real, g_real, b_real;

    char linea1[25];
    char linea2[25];
    char linea3[25];
    char linea4[25];

    while (1)
    {
        gpio_set_level(LED_R, 0);
        gpio_set_level(LED_G, 0);
        gpio_set_level(LED_B, 0);
        vTaskDelay(pdMS_TO_TICKS(200));

        ambiente = adc1_get_raw(ADC_CHANNEL);

        gpio_set_level(LED_R, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        r = adc1_get_raw(ADC_CHANNEL);
        gpio_set_level(LED_R, 0);

        gpio_set_level(LED_G, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        g = adc1_get_raw(ADC_CHANNEL);
        gpio_set_level(LED_G, 0);

        gpio_set_level(LED_B, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        b = adc1_get_raw(ADC_CHANNEL);
        gpio_set_level(LED_B, 0);

//RESULTADOS
        r_real = r - ambiente;
        g_real = g - ambiente;
        b_real = b - ambiente;

        int max = r_real;
        int min = r_real;

        if (g_real > max) max = g_real;
        if (b_real > max) max = b_real;

        if (g_real < min) min = g_real;
        if (b_real < min) min = b_real;

        const char *resultado;

        if (r_real < 80 && g_real < 80 && b_real < 80)
        {
            resultado = "MUY OSCURO";
        }
        else if ((max - min) < 250)
        {
            resultado = "VERDE";
        }
        else if (r_real > b_real)
        {
            resultado = "ROJO";
        }
        else
        {
            resultado = "AZUL";
        }

//IMPRESION DE RESULTADOS
        printf("\n-----------\n");
        printf("Ambiente: %d\n", ambiente);
        printf("R: %d | V: %d | A: %d\n", r, g, b);
        printf("R real: %d | V real: %d | A real: %d\n", r_real, g_real, b_real);
        printf("Resultado: %s\n", resultado);

        sprintf(linea1, "COLOR: %s", resultado);
        sprintf(linea2, "R:%d", r_real);
        sprintf(linea3, "V:%d", g_real);
        sprintf(linea4, "A:%d AMB:%d", b_real, ambiente);

        oled_print_line(0, linea1);
        oled_print_line(1, linea2);
        oled_print_line(2, linea3);
        oled_print_line(3, linea4);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}