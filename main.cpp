/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "badger2040.hpp"

#include "bsp/board.h"
#include "tusb.h"

#include "cdc_uart.h"
#include "get_serial.h"

// UART0 for Picoprobe debug
// UART1 for picoprobe to target device

using namespace pimoroni;

const int BUFF_SIZE = 296 * 128 / 8;
const int MAX_UART_PKT = 64;


uint8_t buf[BUFF_SIZE];

Badger2040 *badger;

int main(void) {
    badger = new Badger2040(buf);

    badger->init();
    badger->update_speed(2);

    for(auto x = 0u; x < 128; x++) {
        badger->pixel(x, x);
    }
    badger->font("bitmap8");
    badger->text("Ready", 0, 0, 4, 0, 1);
    badger->update(true);

    board_init(); // Wtf?
    usb_serial_init(); // ??
    //cdc_uart_init(); // From cdc_uart.c
    tusb_init(); // Tiny USB?

    uint bytes_total = 0;
    while (1) {
        tud_task(); // tinyusb device task
        uint bytes_recvd = cdc_task(buf + bytes_total, MAX_UART_PKT); // CDC serial task
        bytes_total += bytes_recvd;
        if(bytes_total >= BUFF_SIZE) {
            bytes_total = 0;
            badger->update();
        }
    }

    return 0;
}