/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nrf51dongle nRF51 Dongle
 * @ingroup     boards
 * @brief       Board specific files for the Nordic nRF51 Dongle
 * @{
 *
 * @file
 * @brief       Board specific configuration for the nRF51 Dongle
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Xtimer configuration
 * @{
 */
#define XTIMER                      (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_MASK                 (0xff000000)
#define XTIMER_SHIFT_ON_COMPARE     (2)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define LED_RED_PIN         (1 << 21)
#define LED_GREEN_PIN       (1 << 22)
#define LED_BLUE_PIN        (1 << 23)
/** @} */

/**
 * @name    Macros for controlling the on-board LEDs
 * @{
 */
#define LED_RED_ON          (NRF_GPIO->OUTCLR = LED_RED_PIN)
#define LED_RED_OFF         (NRF_GPIO->OUTSET = LED_RED_PIN)
#define LED_RED_TOGGLE      (NRF_GPIO->OUT ^= LED_RED_PIN)
#define LED_GREEN_ON        (NRF_GPIO->OUTCLR = LED_GREEN_PIN)
#define LED_GREEN_OFF       (NRF_GPIO->OUTSET = LED_GREEN_PIN)
#define LED_GREEN_TOGGLE    (NRF_GPIO->OUT ^= LED_GREEN_PIN)
#define LED_BLUE_ON         (NRF_GPIO->OUTCLR = LED_BLUE_PIN)
#define LED_BLUE_OFF        (NRF_GPIO->OUTSET = LED_BLUE_PIN)
#define LED_BLUE_TOGGLE     (NRF_GPIO->OUT ^= LED_BLUE_PIN)
/** @} */

/**
 * @brief   Initialize the board, also triggers the CPU initialization
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** BOARD_H */
/** @} */
