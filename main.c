#include <xc.h>
#include "LCD.h"
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "UART.h"
#include "ADC.h"
#define _XTAL_FREQ 20000000

#define MODE LCD

#define RED
#define BLUE
#define GREEN

int main()
{
    
    
    TRISB = 0x00; //LCD port
    TRISC = 0x01; 
   
    __delay_ms(1000);
    RC4 = 0;
    __delay_ms(1000);
    RC4 = 1;
    __delay_ms(1000);
    RC4 = 0;
    __delay_ms(1000);
    RC4 = 1;
    __delay_ms(1000);
    RC4 = 0;
    __delay_ms(1000);
    
    ADC_init();
    //if MODE == UART
    //UART_init();
    //elif MODE == LCD
    LCD_init();
    //endif
      
    
           
    int outcome = 1111;
    char buffer[7];
    
    while(1)
    {   
        outcome = ADC_read();
        
        sprintf(buffer,sizeof(buffer),"%d", outcome);
        LCD_Write_String("ADC: ");
        LCD_Write_String(buffer);
        
        if(RC1 == 1)//hold button
        {
            while(RC1==1);
            while(RC1==0);
        }
        
        LCD_command(LCD_RETURN_HOME);
         
        //if MODE == UART  
        //UART_print("Outcome: ");
        //UART_print(buffer);
        //UART_print("\n\r");
        
        //elif MODE == LCD 
                
          
         //LCD_write_char('a');
         //UART_print(buffer);
         //UART_print( "\n\r");
         //endif
    }
    return 0;
}