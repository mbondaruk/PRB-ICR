#include "setDCO.h"
#include "sdmm.h"
#include <stdio.h>
#include "lcd.h"

int response_timeout = 0; //timeout flag

BYTE response[16] = {255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
BYTE cmd0[6] = {0x40, 0, 0, 0, 0, 0x95}; //begin initialization
BYTE cmd1[6] = {0x41, 0, 0, 0, 0, 0xF9};
BYTE cmd8[6] = {0x48, 0, 0, 0x01, 0xAA, 0x87}; //send host's voltage range
BYTE cmd55[6] = {0x77, 0, 0, 0, 0, 0x65}; //next is acmd
BYTE acmd41[6] = {0x69, 0x40, 0, 0, 0, 0x77};
BYTE cmd58[6] = {0x7A, 0, 0, 0, 0, 0xFD}; //send operating conditions register
BYTE acmd51[6] = {0x73, 0, 0, 0, 0, 0xC7};
BYTE cmd9[6] = {0x49, 0, 0, 0, 0, 0xAF}; //send csd (card specific data) 128B
BYTE cmd10[6] = {0x4A, 0, 0, 0, 0, 0x1b}; //send cid (card id) 128B
BYTE cmd13[6] = {0x4D, 0, 0, 0, 0, 0xd}; //send card status 512B

void initialize_timer3(); //TA3 - overall card timeout
void initialize_timer2(); //TA2 - one response timeout
void reset_timer3();
void reset_timer2();
void start_timer3();
void stop_timer3();
void start_timer2();
void stop_timer2();

void wait_for_card();
void disable_reader();
void enable_reader();
void init_pins();

int init_sequence2();
int poll_sd(BYTE *cmd, int respbytes);
void send_cmd(BYTE *cmd);
void get_response(int n);
void write_spi(BYTE d);
BYTE receive_spi();
int from_vrange();
int to_vrange();


void ground_unused_io()
{
    P1->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4);
    P1->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4);
    P1->DIR |= (BIT0|BIT1|BIT2|BIT3|BIT4);
    P1->OUT &= ~(BIT0|BIT1|BIT2|BIT3|BIT4);


    P2->SEL0 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
    P2->SEL1 &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
    P2->DIR |= (BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);
    P2->OUT &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT7);

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
}

void PORT6_IRQHandler(void)
{
    if(P6->IFG & BIT7)
    {
        delay_us(100000); //wait for bounce
        P6->IFG &= ~BIT7;

        /*if(card_detect)
        {
            card_detect = 0;
        }
        else
        {
            if(P6->IN & BIT7)
            {
                card_detect = 1;
                P6->IES |= BIT7;
            }
        }*/
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

void uninitialize_spi()
{
    /*
     * shut down SPI wire pulls to ground them properly
     */
    P1->SEL0 &= ~(BIT5|BIT6|BIT7);
    P1->SEL1 &= ~(BIT5|BIT6|BIT7);
    P1->OUT &= ~(BIT5|BIT6|BIT7);
    P1->DIR |= (BIT5|BIT6|BIT7);
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;
}

void initialize_spi()
{
    P1->SEL0 |= BIT5|BIT6|BIT7;
    P1->SEL1 &= ~(BIT5|BIT6|BIT7);

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;

    EUSCI_B0->BRW = 10;
    EUSCI_B0->CTLW0  = EUSCI_B_CTLW0_SWRST | EUSCI_B_CTLW0_CKPH
                      |EUSCI_B_CTLW0_MSB  |EUSCI_B_CTLW0_MST
                      |EUSCI_B_CTLW0_SYNC |EUSCI_B_CTLW0_UCSSEL_2;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;
}
void write_bad_card()
{
    write_4bit(WRITE_B);
    write_4bit(WRITE_a);
    write_4bit(WRITE_d);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_C);
    write_4bit(WRITE_a);
    write_4bit(WRITE_r);
    write_4bit(WRITE_d);
}
void write_timeout()
{
    write_4bit(WRITE_C);
    write_4bit(WRITE_a);
    write_4bit(WRITE_r);
    write_4bit(WRITE_d);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_T);
    write_4bit(WRITE_i);
    write_4bit(WRITE_m);
    write_4bit(WRITE_e);
    write_4bit(WRITE_o);
    write_4bit(WRITE_u);
    write_4bit(WRITE_t);
}
void write_checking_card()
{
    clear_display();
    write_4bit(WRITE_C);
    write_4bit(WRITE_h);
    write_4bit(WRITE_e);
    write_4bit(WRITE_c);
    write_4bit(WRITE_k);
    write_4bit(WRITE_i);
    write_4bit(WRITE_n);
    write_4bit(WRITE_g);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_c);
    write_4bit(WRITE_a);
    write_4bit(WRITE_r);
    write_4bit(WRITE_d);
    write_4bit(WRITE_DOT);
    write_4bit(WRITE_DOT);
    write_4bit(WRITE_DOT);
    command_4bit(SET_DDRAM_ADDR_40H);
}
void write_slot_empty()
{
    clear_display();
    write_4bit(WRITE_S);
    write_4bit(WRITE_l);
    write_4bit(WRITE_o);
    write_4bit(WRITE_t);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_e);
    write_4bit(WRITE_m);
    write_4bit(WRITE_p);
    write_4bit(WRITE_t);
    write_4bit(WRITE_y);
}
void write_voltage(int from, int to)
{
    int from1[9] = {2,2,2,3,3,3,3,3,3};
    int from2[9] = {7,8,9,0,1,2,3,4,5};
    int to1[9] = {2,2,3,3,3,3,3,3,3};
    int to2[9] = {8,9,0,1,2,3,4,5,6};

    clear_display();
    write_4bit(WRITE_V);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_r);
    write_4bit(WRITE_a);
    write_4bit(WRITE_n);
    write_4bit(WRITE_g);
    write_4bit(WRITE_e);
    write_4bit(WRITE_COLON);
    command_4bit(SET_DDRAM_ADDR_40H);
    write_4bit(WRITE_0+from1[from]);
    write_4bit(WRITE_DOT);
    write_4bit(WRITE_0+from2[from]);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_V);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_t);
    write_4bit(WRITE_o);
    write_4bit(WRITE_SPACE);
    write_4bit(WRITE_0 + to1[to]);
    write_4bit(WRITE_DOT);
    write_4bit(WRITE_0 + to2[to]);
    write_4bit(WRITE_V);
}

void main(void)
{
    /*
     * RTC & WDT are off, hence
     * deep_sleep sends to LPM4
     */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    setDCO(freq3mhz);
    init_pins();
    initialize_lcd();
    initialize_spi();
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

void send_cmd(BYTE *cmd)
{
    int i;
    CS_L();
    write_spi(0xff);
    receive_spi();
    for(i = 0; i < 6; i++)
    {
        write_spi(cmd[i]);
        receive_spi();
    }
    write_spi(0xff);
    receive_spi();
}

void get_response(int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        write_spi(0xff);
        response[i] = receive_spi();
    }
    CS_H();
    write_spi(0xff);
    receive_spi();
}

void write_spi(BYTE d)
{
    while(EUSCI_B0->STATW & EUSCI_B_STATW_SPI_BUSY);
    EUSCI_B0->TXBUF = d;
}

BYTE receive_spi()
{
    while(EUSCI_B0->STATW & EUSCI_B_STATW_SPI_BUSY);
    return EUSCI_B0->RXBUF;
}

int from_vrange()
{
    if(response[3] & 0x80)
    {
        return 0;
    }
    else if(response[2] & 0x1)
    {
        return 1;
    }
    else if(response[2] & 0x2)
    {
        return 2;
    }
    else if(response[2] & 0x4)
    {
        return 3;
    }
    else if(response[2] & 0x8)
    {
        return 4;
    }
    else if(response[2] & 0x10)
    {
        return 5;
    }
    else if(response[2] & 0x20)
    {
        return 6;
    }
    else if(response[2] & 0x40)
    {
        return 7;
    }
    else if(response[2] & 0x80)
    {
        return 8;
    }
}
int to_vrange()
{
    if(response[2] & 0x80)
    {
        return 8;
    }
    else if(response[2] & 0x40)
    {
        return 7;
    }
    else if(response[2] & 0x20)
    {
        return 6;
    }
    else if(response[2] & 0x10)
    {
        return 5;
    }
    else if(response[2] & 0x08)
    {
        return 4;
    }
    else if(response[2] & 0x04)
    {
        return 3;
    }
    else if(response[2] & 0x02)
    {
        return 2;
    }
    else if(response[2] & 0x01)
    {
        return 1;
    }
    else if(response[3] & 0x80)
    {
        return 0;
    }
}
int poll_sd(BYTE *cmd, int respbytes)
{
    start_timer2();
    while(response[0] == 0xFF && !response_timeout)
    {
        send_cmd(cmd);
        get_response(respbytes);
    }
    stop_timer2();
    reset_timer2();

    if(response_timeout)
    {
        stop_timer3();
        reset_timer3();
        return 1;
    }
    return 0;
}
int init_sequence2()
{
    int i;
    response_timeout = 0;
    enable_reader();
    delay_us(45000);

    start_timer3();
    for(i = 0; i < 10; i++)
    {
        write_spi(0xFF);
        receive_spi();
    }

    if(poll_sd(cmd0, 6) == 1 || response[0] != 1)
        return 1;
    response[0] = 255;
    if(poll_sd(cmd8, 6) == 1 || response[0] != 1)
        return 1;
    response[0] = 255;

    //read OCR (operation condition register)
    if(poll_sd(cmd58, 6) == 1 || response[0] != 1)
        return 1;

    stop_timer3();
    reset_timer3();
    if(response[0] == 1)
    {

        int from = from_vrange();
        int to = to_vrange();
        write_voltage(from, to);
        return 0;
    }
    return 1;
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

void init_pins()
{
    //P6.7IN - card detect (w Glitch filter)
    P6->SEL0 &= ~BIT7;
    P6->SEL1 &= ~BIT7;
    P6->DIR &= ~BIT7;

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
void disable_reader()
{
    uninitialize_spi();
    DL_L();
    CS_L();
}
void enable_reader()
{
    initialize_spi();
    DL_H();
    CS_H();
}
void wait_for_card()
{
    //ground reader pins
    disable_reader();
    __deep_sleep();
    delay_us(1500000);
}
