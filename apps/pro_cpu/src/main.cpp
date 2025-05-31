#include <stdio.h>
#include <stdlib.h>
#include <ctime>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>
#include <zephyr/drivers/gpio.h>

#include "pin_defines.hpp"

#define TFT_W           172
#define TFT_H           320
#define BYTES_PER_PIXEL 2

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)
#define TFT_LED_GPIO     GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, tft_led_gpios)

static uint16_t frame_buf[TFT_W * TFT_H];

int main()
{
    /* turn on TFT LED */
    const struct gpio_dt_spec tft_led = TFT_LED_GPIO;
    gpio_pin_configure_dt(&tft_led, GPIO_OUTPUT_ACTIVE | GPIO_ACTIVE_HIGH);

    const struct device* dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(dev)) {
        printk("Device %s not ready\n", dev->name);
        return 0;
    }

    if (display_set_pixel_format(dev, PIXEL_FORMAT_RGB_565) != 0) {
        printk("Failed to set required pixel format\n");
        return 0;
    }

    struct display_capabilities caps;
    display_get_capabilities(dev, &caps);

    int rc = display_blanking_off(dev);
    if (rc != 0) {
        printk("Failed to turn on display blanking: %d\n", rc);
    }

    while (1) {
        /* make a random colored band pattern in the frame buf */
        unsigned int band_len = 80u;
        for (size_t i = 0u; i < TFT_H; i += band_len) {
            for (size_t k = 0u; k < band_len; k++) {
                uint16_t color =
                    (uint16_t)(rand() % UINT64_MAX); // Random color
                for (size_t j = 0u; j < TFT_W; j++) {
                    size_t idx = (i + k) * TFT_W + j;
                    frame_buf[idx] = color;
                }
            }
        }

        struct display_buffer_descriptor _desc = { .buf_size =
                                                       sizeof(frame_buf),
                                                   .width = TFT_W,
                                                   .height = TFT_H,
                                                   .pitch = TFT_W,
                                                   .frame_incomplete = false };

        rc = display_write(dev, 0u, 0u, &_desc, (void*)frame_buf);
        if (rc != 0) {
            printk("Failed to write to display: %d\n", rc);
        }

        printk("written frame\n");
        k_sleep(K_MSEC(33u));
    }

    while (1) {
        printk("Welcome to ST7789 demo!\n");
        k_sleep(K_SECONDS(rand() % 30u));
    }

    return 0;
}
