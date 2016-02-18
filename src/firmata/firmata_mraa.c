/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>

#include "mraa_internal.h"
#include "firmata/firmata_mraa.h"
#include "firmata/firmata.h"

static t_firmata* firmata_dev;

static unsigned int
mraa_firmata_aio_read(mraa_aio_context dev)
{
    firmata_pull(firmata_dev);
    return (unsigned int) firmata_dev->pins[dev->channel].value;
}

static mraa_result_t
mraa_firmata_aio_init_internal_replace(mraa_aio_context dev, int aio)
{
    // firmata considers A0 pin0 as well as actual pin0 :/
    firmata_pinMode(firmata_dev, aio, MODE_ANALOG);
    // register for updates on that ADC channel
    firmata_analogRead(firmata_dev, aio);

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_firmata_gpio_init_internal_replace(mraa_gpio_context dev, int pin)
{
    dev->pin = pin;
    dev->phy_pin = pin;

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_firmata_gpio_mode_replace(mraa_gpio_context dev, mraa_gpio_mode_t mode)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static int
mraa_firmata_gpio_read_replace(mraa_gpio_context dev)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static mraa_result_t
mraa_firmata_gpio_write_replace(mraa_gpio_context dev, int write_value)
{
    if (write_value == 0) {
        firmata_digitalWrite(firmata_dev, dev->phy_pin, LOW);
    } else {
        firmata_digitalWrite(firmata_dev, dev->phy_pin, HIGH);
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_firmata_gpio_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    switch (dir) {
        case MRAA_GPIO_IN:
            firmata_pinMode(firmata_dev, dev->phy_pin, MODE_OUTPUT);
        case MRAA_GPIO_OUT:
            firmata_pinMode(firmata_dev, dev->phy_pin, MODE_INPUT);
        default:
            return MRAA_ERROR_INVALID_PARAMETER;
    }

    return MRAA_SUCCESS;
}

mraa_board_t*
mraa_firmata_init(const char* uart_dev)
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    b->platform_name = "firmata";
    // do we support 2.5? Or are we more 2.3?
    // or should we return the flashed sketch name?
    b->platform_version = "2.5";
    b->gpio_count = 14;
    b->aio_count = 6;
    b->adc_supported = 10;
    b->phy_pin_count = 20;

    b->pins = (mraa_pininfo_t*) calloc(b->phy_pin_count, sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        free(b);
        return NULL;
    }

    strncpy(b->pins[0].name, "IO0", 8);
    b->pins[0].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[0].gpio.pinmap = 0;
    strncpy(b->pins[1].name, "IO1", 8);
    b->pins[1].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[1].gpio.pinmap = 1;
    strncpy(b->pins[2].name, "IO2", 8);
    b->pins[2].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[2].gpio.pinmap = 2;
    strncpy(b->pins[3].name, "IO3", 8);
    b->pins[3].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[3].gpio.pinmap = 3;
    strncpy(b->pins[4].name, "IO4", 8);
    b->pins[4].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[4].gpio.pinmap = 4;
    strncpy(b->pins[5].name, "IO5", 8);
    b->pins[5].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[5].gpio.pinmap = 5;
    strncpy(b->pins[6].name, "IO6", 8);
    b->pins[6].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[6].gpio.pinmap = 6;
    strncpy(b->pins[7].name, "IO7", 8);
    b->pins[7].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[7].gpio.pinmap = 7;
    strncpy(b->pins[8].name, "IO8", 8);
    b->pins[8].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[8].gpio.pinmap = 8;
    strncpy(b->pins[9].name, "IO9", 8);
    b->pins[9].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[9].gpio.pinmap = 9;
    strncpy(b->pins[10].name, "IO10", 8);
    b->pins[10].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[10].gpio.pinmap = 10;
    strncpy(b->pins[11].name, "IO11", 8);
    b->pins[11].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[11].gpio.pinmap = 11;
    strncpy(b->pins[12].name, "IO12", 8);
    b->pins[12].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[12].gpio.pinmap = 12;
    strncpy(b->pins[13].name, "IO13", 8);
    b->pins[13].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[13].gpio.pinmap = 13;
    strncpy(b->pins[10].name, "A0", 8);
    b->pins[14].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[14].gpio.pinmap = 14;
    strncpy(b->pins[11].name, "A1", 8);
    b->pins[15].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[15].gpio.pinmap = 15;
    strncpy(b->pins[12].name, "A2", 8);
    b->pins[16].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[16].gpio.pinmap = 16;
    strncpy(b->pins[13].name, "A3", 8);
    b->pins[17].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[17].gpio.pinmap = 17;
    strncpy(b->pins[13].name, "A4", 8);
    b->pins[18].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[18].gpio.pinmap = 18;
    strncpy(b->pins[13].name, "A5", 8);
    b->pins[19].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 1, 0 };
    b->pins[19].gpio.pinmap = 19;


    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        free(b);
        return NULL;
    }

    b->adv_func->gpio_init_internal_replace = &mraa_firmata_gpio_init_internal_replace;
    b->adv_func->gpio_mode_replace = &mraa_firmata_gpio_mode_replace;
    b->adv_func->gpio_dir_replace = &mraa_firmata_gpio_dir_replace;
    b->adv_func->gpio_read_replace = &mraa_firmata_gpio_read_replace;
    b->adv_func->gpio_write_replace = &mraa_firmata_gpio_write_replace;

    b->adv_func->aio_init_internal_replace = &mraa_firmata_aio_init_internal_replace;
    b->adv_func->aio_read_replace = &mraa_firmata_aio_read;

    firmata_dev = firmata_new(uart_dev);

    // if this isn't working then we have an issue with our uart
    while (!firmata_dev->isReady) {
       firmata_pull(firmata_dev);
    }

    return b;
}

mraa_platform_t
mraa_firmata_platform(mraa_board_t* board, const char* uart_dev)
{
    /**
     * Firmata boards are not something we can detect so we just trust the user
     * to initialise them themselves and is the only platform type not to be
     * initialised from mraa_init(). Good luck!
     */
    mraa_board_t* sub_plat = NULL;

    sub_plat = mraa_firmata_init(uart_dev);
    if (sub_plat != NULL) {
        sub_plat->platform_type = MRAA_GENERIC_FIRMATA;
        board->sub_platform = sub_plat;
        return sub_plat->platform_type;
    }

    return MRAA_NULL_PLATFORM;
}