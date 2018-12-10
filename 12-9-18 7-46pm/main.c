#include "setDCO.h"
#include "sdmm.h"
#include <stdio.h>

int card_detect = 0;
int init_timeout = 0;
int response_timeout = 0;
BYTE response[6] = {255, 0, 0, 0, 0, 0};
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


//int init_sequence1();
int init_sequence2();
void card_ok();
void wait_for_card();
void uninitialize_spi()
{
    P3->SEL0 &= ~(BIT5|BIT6|BIT7);
    P3->SEL1 &= ~(BIT5|BIT6|BIT7);
    P3->OUT &= ~(BIT5|BIT6|BIT7);
    P3->DIR |= (BIT5|BIT6|BIT7);
    EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_SWRST;
}
void initialize_spi()
{
    P3->SEL0 |= BIT5|BIT6|BIT7;
    P3->SEL1 &= ~(BIT5|BIT6|BIT7);


    EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_SWRST;

    EUSCI_B2->BRW = 10;
    EUSCI_B2->CTLW0  = EUSCI_B_CTLW0_SWRST | EUSCI_B_CTLW0_CKPH
                      |EUSCI_B_CTLW0_MSB  |EUSCI_B_CTLW0_MST
                      |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2;

    EUSCI_B2->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    setDCO(freq3mhz);
    init_pins();
    initialize_spi();
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
            if(init_sequence2() == 0)
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
void send_cmd(BYTE *cmd)
{
    int i;
    CS_L();
    for(i = 0; i < 6; i++)
    {
        write_spi(cmd[i]);
        receive_spi();
    }
    write_spi(0xff);
    receive_spi();
    for(i = 0; i < 6; i++)
    {
        write_spi(0xff);
        response[i] = receive_spi();
    }
    CS_H();
}

void write_spi(BYTE d)
{
    while(!EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG);
    EUSCI_B2->TXBUF = d;
}

BYTE receive_spi()
{
    while(!EUSCI_B2->IFG & EUSCI_B_IFG_RXIFG);
    return EUSCI_B2->RXBUF;
}

int init_sequence2()
{
    int i;
    BYTE resp;
    DL_H();
    CS_H();
    delay_us(45000);

    BYTE cmd0resp[6];
    BYTE cmd0resp2[6];
    BYTE cmd8resp[6];
    BYTE cmd55resp[6];
    BYTE acmd41resp[6];
    BYTE cmd55resp2[6];
    BYTE acmd41resp2[6];
    BYTE cmd55resp3[6];
    BYTE acmd41resp3[6];
    for(i = 0; i < 10; i++)
    {
        write_spi(0xFF);
        resp = receive_spi();
    }

    //while(response[0] == 255 && !response_timeout)
        send_cmd(cmd0);
        for(i = 0; i < 6; i++)
            cmd0resp[i] = response[i];
        send_cmd(cmd0);
        for(i = 0; i < 6; i++)
            cmd0resp2[i] = response[i];
    //response[0] = 255;
    //while(response[0] == 255 && !response_timeout)
        send_cmd(cmd8);
        for(i = 0; i < 6; i++)
            cmd8resp[i] = response[i];
    //while(response[0] == 1 && !response_timeout)
    //{
        send_cmd(cmd55);
        for(i = 0; i < 6; i++)
            cmd55resp[i] = response[i];
        send_cmd(acmd41);
        for(i = 0; i < 6; i++)
            acmd41resp[i] = response[i];

        send_cmd(cmd55);
        for(i = 0; i < 6; i++)
            cmd55resp2[i] = response[i];
        send_cmd(acmd41);
        for(i = 0; i < 6; i++)
            acmd41resp2[i] = response[i];

        send_cmd(cmd55);
        for(i = 0; i < 6; i++)
            cmd55resp3[i] = response[i];
        send_cmd(acmd41);
        for(i = 0; i < 6; i++)
            acmd41resp3[i] = response[i];
    //}
        printf("Cmd0 response\n%02X ", cmd0resp[0]);
        printf("%02X ", cmd0resp[1]);
        printf("%02X ", cmd0resp[2]);
        printf("%02X ", cmd0resp[3]);
        printf("%02X ", cmd0resp[4]);
        printf("%02X\n", cmd0resp[5]);

        printf("Cmd0 response 2\n%02X ", cmd0resp2[0]);
        printf("%02X ", cmd0resp2[1]);
        printf("%02X ", cmd0resp2[2]);
        printf("%02X ", cmd0resp2[3]);
        printf("%02X ", cmd0resp2[4]);
        printf("%02X\n", cmd0resp2[5]);

        printf("Cmd8 response\n%02X ", cmd8resp[0]);
        printf("%02X ", cmd8resp[1]);
        printf("%02X ", cmd8resp[2]);
        printf("%02X ", cmd8resp[3]);
        printf("%02X ", cmd8resp[4]);
        printf("%02X\n", cmd8resp[5]);

        printf("Cmd55 response\n%02X ", cmd55resp[0]);
        printf("%02X ", cmd55resp[1]);
        printf("%02X ", cmd55resp[2]);
        printf("%02X ", cmd55resp[3]);
        printf("%02X ", cmd55resp[4]);
        printf("%02X\n", cmd55resp[5]);

        printf("ACmd41 response\n%02X ", acmd41resp[0]);
        printf("%02X ", acmd41resp[1]);
        printf("%02X ", acmd41resp[2]);
        printf("%02X ", acmd41resp[3]);
        printf("%02X ", acmd41resp[4]);
        printf("%02X\n", acmd41resp[5]);

        printf("Cmd55 response 2\n%02X ", cmd55resp2[0]);
        printf("%02X ", cmd55resp2[1]);
        printf("%02X ", cmd55resp2[2]);
        printf("%02X ", cmd55resp2[3]);
        printf("%02X ", cmd55resp2[4]);
        printf("%02X\n", cmd55resp2[5]);

        printf("ACmd41 response 2\n%02X ", acmd41resp2[0]);
        printf("%02X ", acmd41resp2[1]);
        printf("%02X ", acmd41resp2[2]);
        printf("%02X ", acmd41resp2[3]);
        printf("%02X ", acmd41resp2[4]);
        printf("%02X\n", acmd41resp2[5]);

        printf("Cmd55 response 3\n%02X ", cmd55resp3[0]);
        printf("%02X ", cmd55resp3[1]);
        printf("%02X ", cmd55resp3[2]);
        printf("%02X ", cmd55resp3[3]);
        printf("%02X ", cmd55resp3[4]);
        printf("%02X\n", cmd55resp3[5]);

        printf("ACmd41 response 3\n%02X ", acmd41resp3[0]);
        printf("%02X ", acmd41resp[1]);
        printf("%02X ", acmd41resp[2]);
        printf("%02X ", acmd41resp[3]);
        printf("%02X ", acmd41resp[4]);
        printf("%02X\n", acmd41resp[5]);
    return response[0];
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

/*void write_spi(BYTE d)
{
    if (d & 0x80)
    {
        DI_H();
    }
    else {
        DI_L();
    }
    CK_H();
    CK_L();
    if (d & 0x40)
    {
        DI_H();
    }
    else {
        DI_L();
    }
    CK_H();
    CK_L();
    if (d & 0x20)
    {
        DI_H();
    }
    else {
        DI_L();
    }
    CK_H();
    CK_L();
    if (d & 0x10)
    {
        DI_H();
    }
    else {
        DI_L();
    }
    CK_H();
    CK_L();
    if (d & 0x08)
    {
        DI_H();
    }
    else {
        DI_L();
    }
    CK_H();
    CK_L();
    if (d & 0x04)
    {
        DI_H();
    }
    else {
        DI_L();
    }
    CK_H();
    CK_L();
    if (d & 0x02)
    {
        DI_H();
    }
    else {
        DI_L();
    }
    CK_H();
    CK_L();
    if (d & 0x01) {
        DI_H();
    }
    else {
        DI_L();
    }
    CK_H();
    CK_L();
}
BYTE receive_spi()
{
    BYTE r;
    r = 0;
    if (DO)
        r++;
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;
    CK_H();
    CK_L();
    r <<= 1;
    if (DO)
        r++;
    CK_H();
    CK_L();
    return r;
}*/
void init_pins()
{
    //SPI
    //P1.6OUT - DI
    //P1->SEL0 &= ~BIT6;
    //P1->SEL1 &= ~BIT6;
    //P1->OUT &= ~BIT6; //initially GND
    //P1->DIR |= BIT6;

    //P1.7IN - DO
    //P1->SEL0 &= ~BIT7;
    //P1->SEL1 &= ~BIT7;
    //P1->DIR &= ~BIT7;

    //P5.0 - SCLK
    //P5->SEL0 &= ~BIT0;
    //P5->SEL1 &= ~BIT0;
    //P5->OUT &= ~BIT0; //initially GND
    //P5->DIR |= BIT0;
    //----------------------------------//
    //uninitialize_spi();
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

void card_ok()
{

}
void wait_for_card()
{
    card_detect = 0;
    //ground reader pins
    DL_L();
    CS_L();
    //DI_L();
    __deep_sleep();
}
/*int init_sequence1()
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
    //works up to here
    while(response != 0)
    {
        CS_L();
        delay_us(100);
        write_spi(0x77);
        write_spi(0x0);
        write_spi(0x0);
        write_spi(0x0);
        write_spi(0x0);
        write_spi(0x65);
        write_spi(0xff);
        response = receive_spi();


        write_spi(0x69);
        write_spi(0x40);
        write_spi(0x0);
        write_spi(0x0);
        write_spi(0x0);
        write_spi(0x77);
        write_spi(0xff);
        response  = receive_spi();

        CS_H();
        delay_us(100);
    }


    send_cmd(cmd0);
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
        return 0;
    return 1;

}*/
