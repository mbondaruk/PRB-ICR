#include "setDCO.h"
#include "sdmm.h"
#define WAIT wait_card
int card_detect = 0;
int init_timeout = 0;
int response_timeout = 0;
//BYTE response[6] = {255, 0, 0, 0, 0, 0};
BYTE cmd0[6] = {0x40, 0, 0, 0, 0, 0x95};
BYTE cmd8[6] = {0x48, 0, 0, 0x01, 0xAA, 0x87};
BYTE cmd55[6] = {77, 0, 0, 0, 0, 0};
BYTE acmd41[6] = {0xA9, 0x40, 0, 0, 0, 0};
BYTE cmd58[6] = {0x7A, 0, 0, 0, 0, 0};

void PORT2_IRQHandler(void);
void TA3_0_IRQHandler(void);
void TA2_0_IRQHandler(void);
void initialize_timer3();
void initialize_timer2();
void write_spi(BYTE d);
BYTE receive_spi();
void init_pins();
void setMODCLK();
void send_cmd(BYTE *cmd);
int init_sequence1();
void card_ok();
void wait_for_card();

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    setDCO(freq12mhz);
    init_pins();
    /* card detect interrupt wake up */
    P2->IFG = 0;
    P2->IE = BIT7;
    NVIC->ISER[1] |= (1 << (PORT2_IRQn-32));
    __enable_interrupt();

    while(1)
    {
        if(card_detect == 0 && !(P2->IN & BIT7))
        {
            __deep_sleep();
        }
        else
        {
            delay_us(10000);
            if(init_sequence1() == 0)
            {
                card_ok();
            }
            else
            {
                wait_for_card();
            }
        }
    }
}
int init_sequence1()
{
    BYTE i, response = 255;
    BYTE response1, response2, response3, response4, response5;

    DL_H();
    CS_H();
    delay_us(45000); //ramp up Vdata + stabilize V

    //initialize_timer3(); //initialization timeout
    //initialize_timer2();
    for(i = 0; i < 10; i++)
    {
        write_spi(0xff);
    }

    while(response == 255)
    {
        CS_L();

        write_spi(0x40);
        write_spi(0);
        write_spi(0);
        write_spi(0);
        write_spi(0);
        write_spi(0x95);
        write_spi(0xff);
        response = receive_spi();
        response1 = receive_spi();
        response2 = receive_spi();
        response3 = receive_spi();
        response4 = receive_spi();
        response5 = receive_spi();
        CS_H();
    }

    response = 255;

    while(response == 255)
    {
        CS_L();

        write_spi(0x48);
        write_spi(0x0);
        write_spi(0x0);
        write_spi(0x1);
        write_spi(0xAA);
        write_spi(0x87);
        write_spi(0xff);
        //response = receive_spi();
        response  = receive_spi();
        response1 = receive_spi();
        response2 = receive_spi();
        response3 = receive_spi();
        response4 = receive_spi();
        response5 = receive_spi();
        CS_H();
    }



    /*send_cmd(cmd0);
    if(response[0] != 1 || response_timeout)
        return 1;
    send_cmd(cmd8);
    if(response[0] != 1 || response_timeout)
        return 1;
    while(response[0] == 1)
    {
        send_cmd(cmd55);
        send_cmd(acmd41);
    }
    if(response[0] == 0)
        return 0;*/
    return 1;

}
void PORT2_IRQHandler(void)
{
    if(P2->IFG & BIT7)
    {
        P2->IFG &= ~BIT7;

        if(card_detect)
        {

        }
        else
        {
            card_detect = 1;
            P2->IES |= BIT7;
        }
    }
}
void TA3_0_IRQHandler(void)
{
    TIMER_A3->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A3->CCR[0] = 0;
}

void TA2_0_IRQHandler(void)
{
    TIMER_A2->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; //turn off spi transmission
    TIMER_A2->CCR[0] = 0;
}

void initialize_timer3()
{
    //timer3 - count 1s init timeout
    TIMER_A3->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC_1 | TIMER_A_CTL_ID__8;
    TIMER_A3->EX0 |= TIMER_A_EX0_IDEX__8;
    TIMER_A3->CCR[0] = 46875;
    TIMER_A3->CCTL[0] |= TIMER_A_CCTLN_CCIE;
    NVIC->ISER[0] = (1 << TA3_N_IRQn);
}

void initialize_timer2()
{
    //timer2 - count response timeout
    TIMER_A2->CTL |= TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC_1;
    TIMER_A2->CCR[0] = 16;
    TIMER_A2->CCTL[0] |= TIMER_A_CCTLN_CCIE;
    NVIC->ISER[0] = (1 << TA2_N_IRQn);
}

void write_spi(BYTE d)
{
    if (d & 0x80)
    {
        DI_H();
    }
    else {
        DI_L();  /* bit7 */
    }
    CK_H();
    CK_L();
    if (d & 0x40)
    {
        DI_H();
    }
    else {
        DI_L();  /* bit6 */
    }
    CK_H();
    CK_L();
    if (d & 0x20)
    {
        DI_H();
    }
    else {
        DI_L();  /* bit5 */
    }
    CK_H();
    CK_L();
    if (d & 0x10)
    {
        DI_H();
    }
    else {
        DI_L();  /* bit4 */
    }
    CK_H();
    CK_L();
    if (d & 0x08)
    {
        DI_H();
    }
    else {
        DI_L();  /* bit3 */
    }
    CK_H();
    CK_L();
    if (d & 0x04)
    {
        DI_H();
    }
    else {
        DI_L();  /* bit2 */
    }
    CK_H();
    CK_L();
    if (d & 0x02)
    {
        DI_H();
    }
    else {
        DI_L();  /* bit1 */
    }
    CK_H();
    CK_L();
    if (d & 0x01) {
        DI_H();
    }
    else {
        DI_L();  /* bit0 */
    }
    CK_H();
    CK_L();
}
BYTE receive_spi()
{
    BYTE r;
    r = 0;
    if (DO)
        r++;   /* bit7 */
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;   /* bit6 */
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;   /* bit5 */
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;   /* bit4 */
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;   /* bit3 */
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;   /* bit2 */
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;   /* bit1 */
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;   /* bit0 */
    CK_H();
    CK_L();
    return r;
}
void init_pins()
{
    //SPI
    //P1.6OUT - DI
    P1->SEL0 &= ~BIT6;
    P1->SEL1 &= ~BIT6;
    P1->OUT &= ~BIT6; //initially GND
    P1->DIR |= BIT6;

    //P1.7IN - DO
    P1->SEL0 &= ~BIT7;
    P1->SEL1 &= ~BIT7;
    P1->DIR &= ~BIT7;

    //P5.0 - SCLK
    P5->SEL0 &= ~BIT0;
    P5->SEL1 &= ~BIT0;
    P5->OUT &= ~BIT0; //initially GND
    P5->DIR |= BIT0;
    //----------------------------------//
    //P2.7IN - card detect
    P2->SEL0 &= ~BIT7;
    P2->SEL1 &= ~BIT7;
    P2->DIR &= ~BIT7;

    //P3.0OUT - switch control
    P3->SEL0 &= ~BIT0;
    P3->SEL1 &= ~BIT0;
    P3->DIR |= BIT0;
    P3->OUT &= ~BIT0; //initially GND

    //P2.6OUT - SD_CS
    P2->SEL0 &= ~BIT6;
    P2->SEL1 &= ~BIT6;
    P2->OUT &= ~BIT6; //initially GND
    P2->DIR |= BIT6;

}
void setMODCLK()
{
    CS->KEY = CS_KEY_VAL;  //Unlock CS registers
    CS->CTL0 = 0;       //clear register CTL0
    CS->CTL1 = (CS_CTL1_SELA_2 | CS_CTL1_SELS_4 | CS_CTL1_SELM_4);
    CS->KEY = 0;
}
/*void send_cmd(BYTE *cmd)
{
    int i;
    response[0] = 255;
    while(response[0] == 255 && !response_timeout)
    {
        CS_L();
        for(i = 0; i < 6; i++)
            write_spi(cmd[i]);

        write_spi(0xff);
        response[0] = receive_spi();
        //for(i = 0; i < 6; i++)
            //response[i] = receive_spi();
        CS_H();
    }
}*/

void card_ok()
{

}
void wait_for_card()
{
    card_detect = 0;
    //ground reader pins
    DL_L();
    CS_L();
    DI_L();
    CK_L();
    __deep_sleep();
}
