#include <xc.h>
#include <stdio.h>
#include "UART.h"
#define _XTAL_FREQ 20000000
#define BaudRate 9600
void UART_init()
{   
    TRISC6 = 0;   // TX direction
    TRISC7 = 1;   // RX direction
    SPBRG =((_XTAL_FREQ/16)/BaudRate)-1; // BaudRate generator
    BRGH = 1;  //High BaudRate select bit
    SYNC = 0;
    SPEN = 1;
    TXEN=1;
    CREN=1;
    TX9=0;
    RX9=0;
}
void putch(char bt)
{
    while(!TXIF);
    TXREG=bt;
}
//char getch()
//{
   
   // {
   //     CREN=0;
   //     CREN=1;
   // }
    
    // while(!RCIF);   
  //   return RCREG;
   

void UART_send_string(char* str)
{
    while(*str)
        putch(*str++);
}

void UART_print(unsigned const char *ptr)
{
    while (*ptr != 0)
    {
        putch(*ptr++);
    }
}
