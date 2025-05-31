#include <stdio.h>
#include <stdlib.h>
#include <ctime>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/display/cfb.h>
#include <zephyr/drivers/gpio.h>

#include "pin_defines.hpp"
#include "gif_player.hpp"

int main()
{
    static gify::gif_player gif_player;

    while (1) {
        printk("Welcome to ST7789 demo!\n");
        k_sleep(K_SECONDS(rand() % 30u));
    }

    return 0;
}
