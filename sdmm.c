#include "sdmm.h"

int response_timeout = 0;
BYTE response[16] = {255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

BYTE cmd0[6] = {0x40, 0, 0, 0, 0, 0x95}; //begin initialization
BYTE cmd1[6] = {0x41, 0x40, 0, 0, 0, 0xFF};
BYTE cmd8[6] = {0x48, 0, 0, 0x01, 0xAA, 0x87}; //send host's voltage range
BYTE cmd55[6] = {0x77, 0, 0, 0, 0, 0x01}; //next is acmd
BYTE acmd41[6] = {0x69, 0x40, 0, 0, 0, 0x01};
BYTE cmd58[6] = {0x7A, 0, 0, 0, 0, 0xFD}; //send operating conditions register
BYTE acmd51[6] = {0x73, 0, 0, 0, 0, 0xC7};
BYTE cmd9[6] = {0x49, 0, 0, 0, 0, 0xAF}; //send csd (card specific data) 128B
BYTE cmd10[6] = {0x4A, 0, 0, 0, 0, 0x1b}; //send cid (card id) 128B
BYTE cmd13[6] = {0x4D, 0, 0, 0, 0, 0xd}; //send card status 512B

void print_response(int cmd, int n)
{
    int i;
    printf("\nCMD%d response: ", cmd);
    for(i = 0; i < n; i++)
    {
        printf("%02X ", response[i]);
    }
    printf("\n");
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

void send_cmd(BYTE *cmd)
{
    //need to get the correct sequence cs h/l
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
    //need to get the correct sequence cs h/l
    int i;
    for(i = 0; i < n; i++)
    {
        write_spi(0xff);
        response[i] = receive_spi();
    }
    write_spi(0xff);
    receive_spi();
    CS_H();
}

int init_sequence2()
{
    int i;
    VCC_ON();
    delay_us(2000);

    P1->OUT |= BIT7;

    enable_reader();

    delay_us(45000);

    //start_timer3();
    for(i = 0; i < 10; i++)
    {
        write_spi(0xFF);
        receive_spi();
    }
    delay_us(40);
    /*    i = 10;
        send_cmd(cmd0);
        get_response(1);
        print_response(0, 1);

        send_cmd(cmd8);
        get_response(5);
        print_response(8, 5);

        send_cmd(cmd55);
        get_response(1);
        print_response(55, 1);
        send_cmd(acmd41);
        get_response(1);
        print_response(41, 1);

        send_cmd(cmd55);
        get_response(1);
        print_response(55, 1);
        send_cmd(acmd41);
        get_response(1);
        print_response(41, 1);
    */

    if(poll_sd(cmd0, 1) == 1 || response[0] != 1)
        return 1;
    response[0] = 255;
    if(poll_sd(cmd8, 1) == 1 || response[0] != 1)
        return 1;
    response[0] = 255;
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
    return 0;
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
