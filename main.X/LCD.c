/*
 * File:   LCD.c
 * Author: thodo
 *
 * Created on March 15, 2026, 12:48 AM
 */

#include "LCD.h"

void LCD_command(char c)
{
    LCD_PORT = c;
    RS = 0;
    RW = 0;
    E = 1;
    __delay_ms(1);
    E = 0;
}

void LCD_write(char c)
{
    LCD_PORT = c;
    RS = 1;
    RW = 0;
    E = 1;
    __delay_ms(1);
    E = 0;
}

void LCD_init()
{
    E = 0;
    __delay_ms(100);
    LCD_command(LCD_WAKE_UP);
    __delay_ms(30);
    LCD_command(LCD_WAKE_UP);
    __delay_ms(10);
    LCD_command(LCD_WAKE_UP);
    __delay_ms(10);
    LCD_command(LCD_SET_8_BIT);
    LCD_command(LCD_SET_CURSOR);
    LCD_command(LCD_CURSOR_ON_DISPLAY_ON);
    LCD_command(LCD_ENTRY_MODE);
}

void LCD_print_string(char *str, int sec)
{
    if (sec == 0)
    {
        LCD_command(LCD_CLEAR);
        LCD_command(LCD_RET_HOME);
    }

    while(*str != 0)
    {
    LCD_write(*str);
    str++;
    }
}