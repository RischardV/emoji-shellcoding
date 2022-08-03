/*
 * SPDX-FileCopyrightText: 2021-2022 Hadrien Barral
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "led_strip.h"
#include "esp_err.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"


esp_err_t example_configure_stdin_stdout(void)
{
    // Initialize VFS & UART so we can use std::cout/cin
    setvbuf(stdin, NULL, _IONBF, 0);
    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install( (uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM,
            256, 0, 0, NULL, 0) );
    /* Tell VFS to use UART driver */
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
    esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);
    return ESP_OK;
}


bool scw_debug = 0;

static void imemcpy(void *dest, const void *start, size_t len)
{
    uint32_t *d = (uint32_t *) dest;
    uint32_t *s = (uint32_t *) start;
    while (len > 4) {
        *d++ = *s++;
        len -= 4;
    }
}

#define BLINK_GPIO 8
static led_strip_handle_t led_strip;

static void configure_led(void)
{
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

static void next_color(int delay_ms, uint32_t r, uint32_t g, uint32_t b)
{
    /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
    led_strip_set_pixel(led_strip, 0, r, g, b);
    /* Refresh the strip to send data */
    led_strip_refresh(led_strip);
    vTaskDelay(pdMS_TO_TICKS(delay_ms));
}

void led_pattern(void)
{
    configure_led();

    uint32_t M = 2;
    int color_ms = 150;

    for(;;) {
        next_color(       1, M, M, M);
        next_color(color_ms, M, 0, 0);
        next_color(color_ms, 0, M, 0);
        next_color(color_ms, 0, 0, M);
        next_color(color_ms, M/2, M/2, 0);
        next_color(color_ms, M/2, 0, M/2);
        next_color(color_ms, 0, M/2, M/2);
    }
}

static const char *private_key =
    "5HuALLURKEYSAREBELUNG2USx1WANTARA1SEPLZhTVqvbVBbpGq";

void debug_display_private_key(void)
{
    printf("Dumping the shitcoin private key...\n%s\n", private_key);
    led_pattern();
}

static void print_info(void)
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("\033[2J" "\033[H");
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    uint32_t flash_size = 0;
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
    }

    printf("%luMB %s flash\n", flash_size / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("\n");
    if (scw_debug) {
        debug_display_private_key();
    }
}

#define ALLOC_SIZE 78000

IRAM_ATTR char buffer[ALLOC_SIZE];
char input[ALLOC_SIZE];

void app_main(void)
{
    example_configure_stdin_stdout();
    print_info();
    printf("shitcoinWallet™\n");
    printf("Please enter your shitcoinWallet™ passphrase:\n");
    printf("  (protip: paste your Emoji shellcode here)\n");
    fgets(input,sizeof(input),stdin);

    printf("%.2000s ...\n", input);

    imemcpy(buffer, input, sizeof(buffer));
    typedef void exec_t(void);
    ((exec_t *) buffer)();

    for(;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
