#include <inttypes.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_lvgl_port.h"

#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"

#define DISP_BL_GPIO 4
#define DISP_RST_GPIO 5
#define DISP_DC_GPIO 6
#define DISP_SPI_CS_GPIO 10
#define SPI_CLK_GPIO 12
#define SPI_MOSI_GPIO 13

#define DISP_HEIGHT 320 // px
#define DISP_WIDTH 240  // px

#define DISP_PIXEL_CLOCK_HZ 40 * 1000 * 1000 // 40MHz
#define DISP_CMD_BITS 8
#define DISP_PARAM_BITS 8

void app_main(void) {
    // init display backlight pin
    gpio_config_t bl_config = {.mode = GPIO_MODE_OUTPUT, .pin_bit_mask = 1ULL << DISP_BL_GPIO};
    ESP_ERROR_CHECK(gpio_config(&bl_config));

    // init SPI bus
    spi_bus_config_t disp_buscfg = {
        .sclk_io_num     = SPI_CLK_GPIO,
        .mosi_io_num     = SPI_MOSI_GPIO,
        .miso_io_num     = -1,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = DISP_HEIGHT * DISP_WIDTH * 2 // reassess for memory optimizations and RGB format
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &disp_buscfg, SPI_DMA_CH_AUTO));

    // init lcd io and display
    esp_lcd_panel_io_handle_t io_handle     = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num       = DISP_DC_GPIO,
        .cs_gpio_num       = DISP_SPI_CS_GPIO,
        .pclk_hz           = DISP_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits      = DISP_CMD_BITS,
        .lcd_param_bits    = DISP_PARAM_BITS,
        .spi_mode          = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle     = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = DISP_RST_GPIO,
        .rgb_ele_order  = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // Turn off backlight during init
    ESP_ERROR_CHECK(gpio_set_level(DISP_BL_GPIO, 0));

    // Reset the display
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));

    // Initialize LCD panel
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // Turn on the screen
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));

    // Swap x and y axis (Different LCD screens may need different options)
    // ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));

    // init LVGL port
    lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    lvgl_port_display_cfg_t disp_cfg = {
        .io_handle     = io_handle,
        .panel_handle  = panel_handle,
        .buffer_size   = DISP_WIDTH * (DISP_HEIGHT / 10), // ~40 lines, ~25.6KB per buffer
        .trans_size    = DISP_WIDTH * 10 * 2,
        .double_buffer = true,
        .hres          = DISP_WIDTH,
        .vres          = DISP_HEIGHT,
        .monochrome    = false,
        .color_format  = LV_COLOR_FORMAT_RGB565,
        .rotation =
            {
                .swap_xy  = false,
                .mirror_x = false,
                .mirror_y = false,
            },
        .flags = {
            .buff_spiram  = true,  // draw buffer lives in PSRAM
            .buff_dma     = false, // PSRAM buffer isn't DMA-capable directly
            .swap_bytes   = true,
            .direct_mode  = false,
            .full_refresh = false,
        }
    };
    lv_display_t *disp = lvgl_port_add_disp(&disp_cfg);

    ESP_ERROR_CHECK(gpio_set_level(DISP_BL_GPIO, 1));

    if (lvgl_port_lock(0)) {
        lv_obj_t *label = lv_label_create(lv_scr_act());
        lv_label_set_text(label, "Hello, World!");
        lv_obj_center(label);
        lvgl_port_unlock();
    }

    printf("Hello world!\n");

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
