#include <inttypes.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "driver/spi_common.h"

#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"

#include "config.h"
#include "ui.h"

void spi_init() {
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
}

void app_main(void) {

    spi_init();
    ui_init();

    ui_start();

    printf("Hello world!\n");

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
