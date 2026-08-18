/*
 * File:   ADC.c
 * Author: thodo
 *
 * Created on March 14, 2026, 12:11 AM
 */

#include "ADC.h"

void ADC16_init()
{
        // Configure the SD A/D module
    SD1CON1bits.PWRLVL = 0; // Low power, normal bandwidth
    SD1CON1bits.SDREFP = 0; // Positive Voltage Reference is SVDD
    SD1CON1bits.SDREFN = 0; // Negative Voltage Reference is SVSS
    SD1CON1bits.VOSCAL = 0; // Internal Offset Measurement is disabled
    SD1CON1bits.DITHER = 1; // Low Dither
    SD1CON1bits.SDGAIN = 0; // Gain is 1:1
    // Channel #0 conversion result.
    SD1CON2bits.RNDRES = 2; // Round result to 16-bit
    SD1CON2bits.SDWM   = 1; // SDxRESH/SDxRESL updated on every Interrupt
    SD1CON2bits.CHOP   = 3; // Chopping should be enabled
    SD1CON3bits.SDCH   = 2; // Channel 0 (see the specific device data sheet)
    SD1CON3bits.SDCS   = 1; // Clock Source is a 8 MHz FRC
    SD1CON3bits.SDOSR  = 0; // Oversampling Ratio (OSR) is 1024 (best quality)
    SD1CON3bits.SDDIV  = 1; // Input Clock Divider is 2 (SD ADC clock is 4MHz)
             // Configure SD A/D interrupt
       SD1CON2bits.SDINT = 3; // Interrupt on every data output
    IFS6bits.SDA1IF = 0; // Clear interrupt flag
    // Turn on the SD A/D module
    SD1CON1bits.SDON   = 1;
   
    
}

signed short int ADC16_read()
{
    signed short int count;
    signed short int result;
    

    
    for(count=0; count<8; count++) 
    {
        //Clear interrupt flag.
        IFS6bits.SDA1IF = 0;
        //Wait for the result ready.
        //while(IFS6bits.SDA1IF == 0);
        
    }
    
    result = SD1RESH;
    return result;
}

void ADC_init()
{
    ADCON1=0;
    ADCON2=0;
    ADCON3=0;
    
    ADCON2bits.PVCFG = 0;
    ADCON2bits.NVCFG = 0;
    
     
    ADCON3bits.ADRC = 0 ;
    ADCON3bits.ADCS = 15;
    
    ADCON1bits.FORM = 0;
    ADCON2bits.BUFORG = 1;
   
    ADCON1bits.PWRLVL = 1;
    
    ADCON1bits.ADSIDL = 1;
    ADCON1bits.ADSLP = 0;
   
    ADCON1bits.PUMPEN = 0;
    
    ADL0CONL = 0;
    ADL0CONH = 0;
   
    
    ADCON2bits.RFPUMP = 0;
   
    
    
    ADL0CONHbits.ASEN = 0;
    ADL0CONHbits.SLINT = 1; 
    ADL0CONHbits.WM = 0;
    ADL0CONHbits.CM = 0;
    ADL0CONHbits.CTMEN = 0;
    ADL0CONHbits.MULCHEN = 0;
    ADL0CONHbits.SAMC = 15;
    
    
    ADTBL0bits.ADCH = 10;
    ADCON3bits.SLEN0 = 1;
    ADL0CONLbits.SAMP = 1; 
ADCON1bits.ADON = 1;

while(ADSTATHbits.ADREADY == 0);

//ADCON1bits.ADCAL = 1;              // start calibration

//while(ADSTATHbits.ADREADY == 0);
// wait until calibration bit clears
    


//YELLOW_LED = 0;
ADL0CONLbits.SAMP = 1; // Close sample switch.
ADL0CONLbits.SLEN = 1;
    
}

unsigned short int ADC_read()
{
    unsigned short int result;
       
    // CONVERSION
    IFS0bits.AD1IF = 0; // Start conversion.
    ADL0CONLbits.SAMP = 0; // Wait for the result.
    while(IFS0bits.AD1IF == 0);
    ADL0CONLbits.SAMP = 1; // Close the sample switch.
    result = ADRES0; // Read result for the channel #10.
    return result;
}

