#include "periph.h"
#include <stdio.h>
void write_spi(BYTE d)
{
    while(EUSCI_B0->STATW & EUSCI_B_STATW_SPI_BUSY);
    //printf("wrote %02X\n", d);
    EUSCI_B0->TXBUF = d;
}

BYTE receive_spi()
{
    while(EUSCI_B0->STATW & EUSCI_B_STATW_SPI_BUSY);
    //printf("received %02X\n", EUSCI_B0->RXBUF);
    return EUSCI_B0->RXBUF;
}

void start_timer3()
{
    TIMER_A3->CTL |= TIMER_A_CTL_MC_1;
}

void start_timer2()
{
    TIMER_A2->CTL |= TIMER_A_CTL_MC_1;
}

void stop_timer3()
{
    TIMER_A3->CTL &= ~TIMER_A_CTL_MC_MASK;
}

void stop_timer2()
{
    TIMER_A2->CTL &= ~TIMER_A_CTL_MC_MASK;
}

void reset_timer3()
{
    TIMER_A3->R = 0;
}

void reset_timer2()
{
    TIMER_A2->R = 0;
}

void initialize_timer2()
{
    /* timer2 - single response timeout
     * same frequency as SPI - 300KHz
     */
    TIMER_A2->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_ID__2;
    TIMER_A2->CTL &= ~TIMER_A_CTL_MC_MASK;
    TIMER_A2->EX0 |= TIMER_A_EX0_IDEX__5;
    /* 8 bits per byte
     * 1 dummy + 6 bytes cmd + 1 dummy + 6 bytes resp + 1 dummy = 21B/attempt
     * give it 10 attempts 8*21*10 = 1680
     */
    TIMER_A2->CCR[0] = 1680;
    TIMER_A2->CCTL[0] |= TIMER_A_CCTLN_CCIE;
    NVIC->ISER[0] = (1 << TA2_0_IRQn);
}

void initialize_timer3()
{
    /* timer3 - count 1s init timeout
     * divided to the 46875Hz to fit into 16bit counter
     * gives the card 1 s to activate
     */
    TIMER_A3->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_ID__8;
    TIMER_A2->CTL &= ~TIMER_A_CTL_MC_MASK;
    TIMER_A3->EX0 |= TIMER_A_EX0_IDEX__8;
    TIMER_A3->CCR[0] = 46875;
    TIMER_A3->CCTL[0] |= TIMER_A_CCTLN_CCIE;
    NVIC->ISER[0] = (1 << TA3_0_IRQn);
}
void init_pins()
{
    //P6.7IN - card detect (w Glitch filter)
    P6->SEL0 &= ~BIT7;
    P6->SEL1 &= ~BIT7;
    P6->DIR &= ~BIT7;

    //P2.4OUT - vcc control
    P2->SEL0 &= ~BIT4;
    P2->SEL1 &= ~BIT4;
    P2->DIR |= BIT4;
    P2->OUT &= ~BIT4; //initially GND

    //P2.5OUT - switch control
    P2->SEL0 &= ~BIT5;
    P2->SEL1 &= ~BIT5;
    P2->DIR |= BIT5;
    P2->OUT &= ~BIT5; //initially GND


    //P2.6OUT - SD_CS
    P2->SEL0 &= ~BIT6;
    P2->SEL1 &= ~BIT6;
    P2->OUT &= ~BIT6; //initially GND
    P2->DIR |= BIT6;
}

void uninitialize_spi()
{
    /*
     * spi lines low
     */
    P1->SEL0 &= ~(BIT5|BIT6|BIT7);
    P1->SEL1 &= ~(BIT5|BIT6|BIT7);
    P1->OUT &= ~(BIT5|BIT6|BIT7);
    P1->DIR |= (BIT5|BIT6|BIT7);
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST;
}

void initialize_spi()
{
    P1->SEL0 |= BIT5|BIT6|BIT7;
    P1->SEL1 &= ~(BIT5|BIT6|BIT7);
    //pullup on DO

    P1->DIR &= BIT7;
    P1->REN |= BIT7;
    P1->OUT |= BIT7;

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST;

    EUSCI_B0->BRW = 10;
    EUSCI_B0->CTLW0  = EUSCI_B_CTLW0_SWRST | EUSCI_B_CTLW0_CKPH
                      //|EUSCI_B_CTLW0_CKPL
                      |EUSCI_B_CTLW0_MSB  |EUSCI_B_CTLW0_MST
                      |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}
void initialize_spi_mode0()
{
    P1->SEL0 |= BIT5|BIT6|BIT7;
    P1->SEL1 &= ~(BIT5|BIT6|BIT7);
    //pullup on DO

    P1->DIR &= BIT7;
    P1->REN |= BIT7;
    P1->OUT |= BIT7;

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST;

    EUSCI_B0->BRW = 10;
    EUSCI_B0->CTLW0  = EUSCI_B_CTLW0_SWRST //| EUSCI_B_CTLW0_CKPH
                      //|EUSCI_B_CTLW0_CKPL
                      |EUSCI_B_CTLW0_MSB  |EUSCI_B_CTLW0_MST
                      |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}
void initialize_spi_mode2()
{
    P1->SEL0 |= BIT5|BIT6|BIT7;
    P1->SEL1 &= ~(BIT5|BIT6|BIT7);
    //pullup on DO

    P1->DIR &= BIT7;
    P1->REN |= BIT7;
    P1->OUT |= BIT7;

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST;

    EUSCI_B0->BRW = 10;
    EUSCI_B0->CTLW0  = EUSCI_B_CTLW0_SWRST //| EUSCI_B_CTLW0_CKPH
                      |EUSCI_B_CTLW0_CKPL
                      |EUSCI_B_CTLW0_MSB  |EUSCI_B_CTLW0_MST
                      |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}
void initialize_spi_mode3()
{
    P1->SEL0 |= BIT5|BIT6|BIT7;
    P1->SEL1 &= ~(BIT5|BIT6|BIT7);
    //pullup on DO

    P1->DIR &= BIT7;
    P1->REN |= BIT7;
    P1->OUT |= BIT7;

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST;

    EUSCI_B0->BRW = 10;
    EUSCI_B0->CTLW0  = EUSCI_B_CTLW0_SWRST | EUSCI_B_CTLW0_CKPH
                      |EUSCI_B_CTLW0_CKPL
                      |EUSCI_B_CTLW0_MSB  |EUSCI_B_CTLW0_MST
                      |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}
/*void ground_unused_io()
{
    P1->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4);
    P1->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4);
    P1->DIR |= (BIT0|BIT1|BIT2|BIT3|BIT4);
    P1->OUT &= ~(BIT0|BIT1|BIT2|BIT3|BIT4);


    P2->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT7);
    P2->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT7);
    P2->DIR |= (BIT0|BIT1|BIT2|BIT3|BIT7);
    P2->OUT &= ~(BIT0|BIT1|BIT2|BIT3|BIT7);

    P3->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4);
    P3->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4);
    P3->DIR |= (BIT0|BIT1|BIT2|BIT3|BIT4);
    P3->OUT &= ~(BIT0|BIT1|BIT2|BIT3|BIT4);

    P4->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3);
    P4->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3);
    P4->DIR |= (BIT0|BIT1|BIT2|BIT3);
    P4->OUT &= ~(BIT0|BIT1|BIT2|BIT3);

    P5->SEL0 = 0;
    P5->SEL1 = 0;
    P5->DIR = 1;
    P5->OUT = 0;

    P6->SEL0 = 0;
    P6->SEL1 = 0;
    P6->DIR = 1;
    P6->OUT = 0;

    P7->SEL0 = 0;
    P7->SEL1 = 0;
    P7->DIR = 1;
    P7->OUT = 0;

    P8->SEL0 = 0;
    P8->SEL1 = 0;
    P8->DIR = 1;
    P8->OUT = 0;

    P9->SEL0 = 0;
    P9->SEL1 = 0;
    P9->DIR = 1;
    P9->OUT = 0;

    P10->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
    P10->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
    P10->DIR |= (BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
    P10->OUT &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
}*/
