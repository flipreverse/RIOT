/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup boards
 * @{
 * @file
 * @brief       msb-430 common board initialization
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "cpu.h"
#include "irq.h"
#include "board.h"
#include "stdio_base.h"
#include "periph_conf.h"
#include "msp430.h"
#include "debug.h"

static volatile uint32_t __msp430_cpu_speed = MSP430_INITIAL_CPU_SPEED;

void msp430_init_dco(void);

static void msb_ports_init(void)
{
    /* Port 1: Free port, for energy saving all outputs are set to zero. */
    P1SEL0 = 0x00;   /* Port1 I/O Function */
    P1OUT = 0x00;   /* Port1 Output register: 00000000 = 0x00 */
    P1DIR = 0xFF;   /* Port1 Direction: 11111111 = 0xFF */

    P2SEL0 = 0x20;   /* Port2 I/O Function */
    P2OUT = 0x00;   /* Port2 Output register: 00000000 = 0x00 */
    P2DIR = 0x1C;   /* Port2 Direction: 00011010 = 0x1C */
    /*   0 - P2.0 [IN ] - */
    /*   0 - P2.1 [OUT] - */
    /*   1 - P2.2 [IN ] - */
    /*   1 - P2.3 [OUT] - */
    /*   1 - P2.4 [OUT] - */
    /*   0 - P2.5 [IN ] - */
    /*   0 - P2.6 [IN ] - SDC Protect */
    /*   0 - P2.7 [IN ] - SDC Detect */

    P3SEL0 = 0x00;   /* Port3 Pins 6 & 7 for USART */
    P3OUT = 0x00;   /* Port3 Output register: 01001001: 0x49 */
    P3DIR = 0xFF;   /* Port3 Direction: 10101011: 0xAB */
    /*   1 - P3.0 */
    /*   1 - P3.1 */
    /*   0 - P3.2 */
    /*   1 - P3.3 */
    /*   0 - P3.4 [IN ] - SHT 11 DATA (OUT/IN) */
    /*   1 - P3.5 [OUT] - SHT 11 CLK */
    /*   0 - P3.6 [2-Funktion] - RS232_RxD */
    /*   1 - P3.7 [2-Funktion] - RS232_TxD */

    /* Port 4: Free port, for energy saving all outputs are set to zero. */
    P4SEL0 = 0x00;   /* Port4 I/O Function */
    P4OUT = 0x00;   /* Port4 Output register: 00000000 = 0x00 */
    P4DIR = 0xFF;   /* Port4 Direction: 11111111 = 0xFF */
    /*   1 - P4.0 [OUT] - unused */
    /*   1 - P4.1 [OUT] - unused */
    /*   1 - P4.2 [OUT] - unused */
    /*   1 - P4.3 [OUT] - unused */
    /*   1 - P4.4 [OUT] - unused */
    /*   1 - P4.5 [OUT] - unused */
    /*   1 - P4.6 [OUT] - unused */
    /*   1 - P4.7 [OUT] - unused */
}

void msp430_set_cpu_speed(uint32_t speed)
{
    irq_disable();
    __msp430_cpu_speed = speed;
    msp430_init_dco();
    irq_enable();
}

/*---------------------------------------------------------------------------*/
void msp430_init_dco(void)
{
#if 0
#if MSP430_HAS_EXTERNAL_CRYSTAL
    /*------------------ use external oszillator -----------------------*/
    uint16_t i;

    /* Stop watchdog */
    WDTCTL = WDTPW + WDTHOLD;

    /* Init crystal for mclk */
    /* XT2 = HF XTAL */
    BCSCTL1 = RSEL2;

    /* Wait for xtal to stabilize */
    do {
        IFG1 &= ~OFIFG;             /* Clear oscillator fault flag */

        for (i = 0xFF; i > 0; i--); /* Time for flag to set */
    }
    while ((IFG1 & OFIFG) != 0);    /* Oscillator fault flag still set? */

    BCSCTL2 = SELM_2 + SELS;        /* MCLK und SMCLK = XT2 (safe) */
#else
    unsigned int delta = __msp430_cpu_speed >> 12;
    unsigned int oldcapture = 0;
    unsigned int i;

//    BCSCTL1 = 0xa4; /* ACLK is devided by 4. RSEL=6 no division for MCLK
             //and SSMCLK. XT2 is off. */

    /* Init FLL to desired frequency using the 32762Hz crystal */
#if MSP430_HAS_DCOR
    BCSCTL2 = 0x01;
#else
    BCSCTL2 = 0x00;
#endif

    WDTCTL = WDTPW + WDTHOLD;             /* Stop WDT */
    BCSCTL1 |= DIVA1 + DIVA0;             /* ACLK = LFXT1CLK/8 */

    for (i = 0xffff; i > 0; i--);         /* Delay for XTAL to settle */

    CCTL2 = CCIS0 + CM0 + CAP;            /* Define CCR2, CAP, ACLK */
    TACTL = TASSEL1 + TACLR + MC1;        /* SMCLK, continous mode */


    while (1) {
        unsigned int compare;

        while ((CCTL2 & CCIFG) != CCIFG);   /* Wait until capture occured! */

        CCTL2 &= ~CCIFG;                    /* Capture occured, clear flag */
        compare = CCR2;                     /* Get current captured SMCLK */
        compare = compare - oldcapture;     /* SMCLK difference */
        oldcapture = CCR2;                  /* Save current captured SMCLK */

        if (delta == compare) {
            break;                            /* if equal, leave "while(1)" */
        }
        else if (delta < compare) {         /* DCO is too fast, slow it down */
            DCOCTL--;

            if (DCOCTL == 0xFF) {             /* Did DCO role under? */
                BCSCTL1--;
            }
        }
        else {                              /* -> Select next lower RSEL */
            DCOCTL++;

            if (DCOCTL == 0x00) {             /* Did DCO role over? */
                BCSCTL1++;
            }

            /* -> Select next higher RSEL  */
        }
    }

    CCTL2 = 0;                            /* Stop CCR2 function */
    TACTL = 0;                            /* Stop Timer_A */

    BCSCTL1 &= ~(DIVA1 + DIVA0);          /* remove /8 divisor from ACLK again */
#endif
#endif
}

void board_init(void)
{
    msp430_cpu_init();
    msb_ports_init();

    msp430_set_cpu_speed(CLOCK_CORECLOCK);

    /* finally initialize STDIO */
    stdio_init();
}
