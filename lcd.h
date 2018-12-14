#ifndef LCD_H
#define LCD_H
#include "msp.h"
#include "delay.h"

#define RS BIT5
#define RW BIT6
#define E BIT7

#define DB4 BIT4
#define DB5 BIT5
#define DB6 BIT6
#define DB7 BIT7

#define UPPER_4_BIT 0xF0
#define LOWER_4_BIT 0x0F
#define FUNC_4_BIT_SET 0x28
#define FUNC_8_BIT_SET 0x38
#define CLEAR_DISPLAY 0x01
#define RET_HOME 0x02
#define DISPLAY_ON 0x0F
#define CURSOR_ON 0x10
#define ENTRY_MODE_SET_I_S 0x07
#define ENTRY_MODE_SET_I_NS 0x06
#define ENTRY_MODE_SET_D_NS 0x04
#define SET_DDRAM_ADDR_40H 0xA8
#define SET_DDRAM_ADDR_00H 0x80

#define WRITE_SPACE 0x20
#define WRITE_EXCL 0x21
#define WRITE_DBLQQT 0x22
#define WRITE_HASH 0x23
#define WRITE_DOLLAR 0x24
#define WRITE_PERCENT 0x25
#define WRITE_AMPERSAND 0x26
#define WRITE_SNGLQT 0x27
#define WRITE_PARENOPEN 0x28
#define WRITE_PARENCLOSE 0x29
#define WRITE_ASTERISK 0x2a
#define WRITE_PLUS 0x2b
#define WRITE_COMA 0x2c
#define WRITE_MINUS 0x2d
#define WRITE_DOT 0x2e
#define WRITE_SLASH 0x2f
#define WRITE_0 0x30
#define WRITE_1 0x31
#define WRITE_2 0x32
#define WRITE_3 0x33
#define WRITE_4 0x34
#define WRITE_5 0x35
#define WRITE_6 0x36
#define WRITE_7 0x37
#define WRITE_8 0x38
#define WRITE_9 0x39
#define WRITE_COLON 0x3a
#define WRITE_SEMICOLON 0x3b
#define WRITE_LESS 0x3c
#define WRITE_EQUALS 0x3d
#define WRITE_GREATER 0x3e
#define WRITE_QUESTION 0x3f
#define WRITE_AT 0x40
#define WRITE_A 0x41
#define WRITE_B 0x42
#define WRITE_C 0x43
#define WRITE_D 0x44
#define WRITE_E 0x45
#define WRITE_F 0x46
#define WRITE_G 0x47
#define WRITE_H 0x48
#define WRITE_I 0x49
#define WRITE_J 0x4a
#define WRITE_K 0x4b
#define WRITE_L 0x4c
#define WRITE_M 0x4d
#define WRITE_N 0x4e
#define WRITE_O 0x4f
#define WRITE_P 0x50
#define WRITE_Q 0x51
#define WRITE_R 0x52
#define WRITE_S 0x53
#define WRITE_T 0x54
#define WRITE_U 0x55
#define WRITE_V 0x56
#define WRITE_W 0x57
#define WRITE_X 0x58
#define WRITE_Y 0x59
#define WRITE_Z 0x5a
#define WRITE_SQRBRKTOPEN 0x5b
#define WRITE_BACKSLASH 0x5c
#define WRITE_SQRBRKTCLOSE 0x5d
#define WRITE_PWR 0x5e
#define WRITE_UNDERSCORE 0x5f
#define WRITE_APOSTROPH 0x60
#define WRITE_a 0x61
#define WRITE_b 0x62
#define WRITE_c 0x63
#define WRITE_d 0x64
#define WRITE_e 0x65
#define WRITE_f 0x66
#define WRITE_g 0x67
#define WRITE_h 0x68
#define WRITE_i 0x69
#define WRITE_j 0x6a
#define WRITE_k 0x6b
#define WRITE_l 0x6c
#define WRITE_m 0x6d
#define WRITE_n 0x6e
#define WRITE_o 0x6f
#define WRITE_p 0x70
#define WRITE_q 0x71
#define WRITE_r 0x72
#define WRITE_s 0x73
#define WRITE_t 0x74
#define WRITE_u 0x75
#define WRITE_v 0x76
#define WRITE_w 0x77
#define WRITE_x 0x78
#define WRITE_y 0x79
#define WRITE_z 0x7a
#define WRITE_CURLBRCOPEN 0x7b
#define WRITE_VERTICALBAR 0x7c
#define WRITE_CURLBRCCLOSE 0x7d
#define WRITE_TILDA 0x7e

void clear_display();
void initialize_lcd();
void initialize_lcd_pins();
void nybble();
void command_4bit(uint8_t word);
void write_4bit(uint8_t word);
uint8_t read_ac_4bit();

#endif
