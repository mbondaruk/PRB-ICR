#include "setDCO.h"
#include "sdmm.h"
#include <stdio.h>
#include "lcd.h"
#include "gui.h"
#include "periph.h"
extern int response_timeout;
extern BYTE response[16];

void wait_for_card();

void PORT6_IRQHandler(void)
{
    if(P6->IFG & BIT7)
    {
        delay_us(75000);
        P6->IFG &= ~BIT7;
    }
}

void TA3_0_IRQHandler(void)
{
    TIMER_A3->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    response_timeout = 1;
}

void TA2_0_IRQHandler(void)
{
    TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    response_timeout = 1;
}

void main(void)
{
    /*
     * RTC & WDT are off, hence
     * deep_sleep sends to LPM4
     */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    setDCO(freq1_5mhz);
    init_pins();
    initialize_lcd();
    initialize_timer2();
    initialize_timer3();

    /* card detect interrupt wake up */
    P6->IFG = 0;
    P6->IE = BIT7;
    NVIC->ISER[1] |= (1 << (PORT6_IRQn-32));
    __enable_interrupt();

    while(1)
    {
        if(!(P6->IN & BIT7))
        {
            write_slot_empty();
            __deep_sleep();
        }
        else
        {
            write_checking_card();
            delay_us(1500000);

            if(init_sequence2() == 0)
            {
                //card OK
                wait_for_card();
            }
            else if(response_timeout)
            {
                //unresponsive card
                delay_us(1500000);
                write_timeout();
                response_timeout = 0;
                wait_for_card();
            }
            else
            {
                //incompatible card, respond with error
                delay_us(1500000);
                write_bad_card();
                wait_for_card();
            }
        }
    }
}

void wait_for_card()
{
    //ground reader pins
    response[0] = 255;
    response_timeout = 0;
    disable_reader();
    VCC_OFF();
    __deep_sleep();
}

