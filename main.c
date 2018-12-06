#include "msp.h"
#include "sdcard.h"

int card_detect = 0;
int timer_1s = 46875;

void initialize_timer3()
{
    //timer2 - count 1s init timeout
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC_2 |
    TIMER_A0->EX0 |= TIMER_A_EX0_IDEX__8;
    NVIC->ISER[0] = (1 << TA0_N_IRQn);
}

void initialize_timer2()
{
    //timer2 - count 74 sclk during init
    //TACLK @
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC_2;
    TIMER_A0->EX0 |= TIMER_A_EX0_IDEX__8;
    NVIC->ISER[0] = (1 << TA0_N_IRQn);
}

void enable_timer2()
{
    TIMER_A2->CTL |= TIMER_A_CTL_MC_2;
    TIMER_A2->CTL |= TIMER_A_CTL_IFG;
}

void enable_timer3()
{
    TIMER_A3->CTL |= TIMER_A_CTL_MC_2;
    TIMER_A3->CTL |= TIMER_A_CTL_IFG;
}

void disable_timer()
{
    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
}

void reset_timer()
{
    TIMER_A0->R = 0;
}

void reset_spi_400Khz()
{
    /* Reset EUSCIB0 */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;

    /* Configure EUSCIB0 */
    //CKPH = 0, CKPL = 0, SCLK @ 400KHz
    EUSCI_B0->BRW = 60;
    EUSCI_B0->CTLW0  = EUSCI_B_CTLW0_SWRST
                      |EUSCI_B_CTLW0_MSB  |EUSCI_B_CTLW0_MST
                      |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2;

    /* Clear SWRST */
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}

void reset_spi_48Mhz()
{
    /* Reset EUSCIB0 */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;

    /* Configure EUSCIB0 */
    EUSCI_B0->CTLW0  = EUSCI_B_CTLW0_SWRST|EUSCI_B_CTLW0_CKPL |EUSCI_B_CTLW0_MST
                      |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2;

    /* Clear SWRST */
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}

void init_pins()
{
    //1.5 SCLK, 1.6 MOSI(DI), 1.7 MISO(DO)
    P1->SEL0 |= BIT5|BIT6|BIT7; //select USCI B0
    P1->SEL1 &= ~(BIT5|BIT6|BIT7);
    //P1.0 - SD_CD with pulldown
    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;
    P1->DIR &= ~BIT0;
    P1->REN |= BIT0;
    P1->OUT &= ~BIT0;
    //P3.0 - buffer control -C
    P3->SEL0 &= ~BIT0;
    P3->SEL1 &= ~BIT0;
    P3->DIR |= BIT0;
    P3->OUT &= ~BIT0;
}

void init_sequence()
{
    //enable power to sd card
    P3->OUT |= BIT0;
    //set spi to sd-native mode
    reset_spi_400Khz();
    delay_us(1);
    WRITE_SPI(0xFF);
    WRITE_SPI(0xFF);
    WRITE_SPI(0xFF);
    WRITE_SPI(0xFF);
    WRITE_SPI(0xFF);
    WRITE_SPI(0xFF);
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    setDCO(freq3MHz);
    init_pins();

    //__wfi();

    while(1)
    {
        if(P1->IN & BIT0 && card_detect == 0)
        {
            card_detect = 1;
            init_sequence();
        }

    }
}
