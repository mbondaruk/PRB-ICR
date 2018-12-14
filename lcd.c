#include "lcd.h"

void initialize_lcd_pins()
{
    /*
     * 4 bit mode:
     * P3.5 - RS,4
     * P3.6 - RW,5
     * P3.7 - E,6
     * P4.4-P4.7 - DB4-7,11-14
     */
    P3->SEL0 &= ~(RS | RW | E); //GPIO mode
    P3->SEL1 &= ~(RS | RW | E); //GPIO mode

    P4->SEL0 &= ~(DB4 | DB5 | DB6 | DB7 );
    P4->SEL1 &= ~(DB4 | DB5 | DB6 | DB7 ); //GPIO mode

    P3->DIR |= (RS | RW | E); //out
    P4->DIR |= (DB4 | DB5 | DB6 | DB7 ); //out

    P3->OUT &= ~(RS | RW | E); //E,RS,RW low
}

void nybble()
{
    P3->OUT |= E; //E = 1
    delay_us(600);
    P3->OUT &= ~E; // E = 0
}
void command_8bit(uint8_t word)
{
    P4->OUT = word;
    P3->OUT &= ~(RS | RW);
    nybble();
}
void command_4bit(uint8_t word)
{
    P4->OUT = (UPPER_4_BIT & word);
    P3->OUT &= ~(RS | RW);
    nybble();
    P4->OUT = (UPPER_4_BIT & (word << 4));
    nybble();
}
void write_4bit(uint8_t word)
{
    P4->OUT = (UPPER_4_BIT & word);
    P3->OUT |= RS;
    P3->OUT &= ~RW;
    nybble();
    P4->OUT = (UPPER_4_BIT & (word << 4));
    nybble();
}
uint8_t read_ac_4bit()
{
    uint8_t word = 0x7F;
    P4->DIR &= ~(DB4 | DB5 | DB6 | DB7);
    P3->OUT &= RS;
    P3->OUT |= RW;
    delay_us(10); // Tah = address hold  time
    P3->OUT |= E;
    delay_us(100); // Tddr = data setup time
    word &= P4->IN; //read high 4 bits
    delay_us(380); // finish Tpw = enable pulse width
    P3->OUT &= E;
    delay_us(720); // finish enable cycle

    P3->OUT |= E;
    delay_us(100);
    word &= (P4->IN >> 4); //read low 4 bits
    delay_us(380);
    P3->OUT &= E;
    delay_us(720);

    P4->DIR |= (DB4 | DB5 | DB6 | DB7);
    return word;
}
void clear_display()
{
    command_4bit(CLEAR_DISPLAY);
    delay_us(20000);
}
void initialize_lcd()
{
    initialize_lcd_pins();
    delay_us(100000);
    P4->OUT = 0x30;
    delay_us(30000);
    nybble();
    delay_us(10000);
    nybble();
    delay_us(10000);
    nybble();
    delay_us(10000);
    P4->OUT = 0x20;
    nybble();
    command_4bit(FUNC_4_BIT_SET);
    command_4bit(CURSOR_ON);
    command_4bit(DISPLAY_ON);
    delay_us(40);
    clear_display();
    command_4bit(ENTRY_MODE_SET_I_NS);
}
/*void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	activate_mclk_as_DCO(FREQ_12_MHZ);
	delay_us(50000);
	initialize_lcd();
}*/
