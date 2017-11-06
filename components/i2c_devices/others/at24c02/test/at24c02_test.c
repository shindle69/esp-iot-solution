/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#define AT24C02_TEST_CODE 1
#if AT24C02_TEST_CODE

#include <stdio.h>
#include "unity.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "at24c02.h"
#include "i2c_bus.h"

#define I2C_MASTER_SCL_IO           21          /*!< gpio number for I2C master clock IO21*/
#define I2C_MASTER_SDA_IO           15          /*!< gpio number for I2C master data  IO15*/
#define I2C_MASTER_NUM              I2C_NUM_1   /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE   0           /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0           /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ          100000      /*!< I2C master clock frequency */

static i2c_bus_handle_t i2c_bus = NULL;
static at24c02_handle_t dev = NULL;

/**
 * @brief i2c master initialization
 */
static void i2c_bus_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_bus = i2c_bus_create(I2C_MASTER_NUM, &conf);
}

void at24c02_init()
{
    i2c_bus_init();
    dev = dev_at24c02_create(i2c_bus, 0x50);
}

void at24c02_test_task(void* pvParameters)
{
    uint8_t ret = 1;
    int cnt = 2;
    while (cnt--) {
        /****One data Test****/
        at24c02_write_byte(dev, 0x20, 0x55);
        vTaskDelay(100 / portTICK_RATE_MS);

        at24c02_read_byte(dev, 0x20, &ret);
        printf("Value of Address 0x20 Last:%x\n", ret);

        at24c02_write_byte(dev, 0x20, 0x23);
        vTaskDelay(100 / portTICK_RATE_MS);

        at24c02_read_byte(dev, 0x20, &ret);
        printf("Value of Address 0x20:%x\n", ret);

        /****** some data Test ****/
        uint8_t data[5] = { 0x10, 0x11, 0x12, 0x13, 0x14 };
        at24c02_write(dev, 0x20, sizeof(data), data);
        vTaskDelay(100 / portTICK_RATE_MS);

        at24c02_read(dev, 0x20, sizeof(data), data);
        printf("Value start Address 0x20 Last:%x,%x,%x,%x,%x,\n", data[0],
                data[1], data[2], data[3], data[4]);

        uint8_t data1[5] = { 0x22, 0x23, 0x24, 0x25, 0x26 };
        at24c02_write(dev, 0x20, sizeof(data1), data1);
        vTaskDelay(100 / portTICK_RATE_MS);

        at24c02_read(dev, 0x20, sizeof(data1), data1);
        printf("Value start Address 0x20:%x,%x,%x,%x,%x,\n", data1[0], data1[1],
                data1[2], data1[3], data1[4]);
    }
    dev_at24c02_delete(dev, true);
    vTaskDelete(NULL);
}

void at24c02_test()
{
    at24c02_init();
    xTaskCreate(at24c02_test_task, "at24c02_test_task", 1024 * 2, NULL, 10,
            NULL);
}

TEST_CASE("Device at24c02 test", "[at24c02][iot][device]")
{
    at24c02_test();
}

#endif
