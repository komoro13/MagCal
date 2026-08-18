/*
 * File:   newmainXC16.c
 * Author: thodo
 *
 * Created on March 11, 2026, 10:11 PM
 */

#define SAMPLING 500
#define CAL_FAR 6850
#define CAL_ZERO 21050
#define CAL_SPAN (CAL_ZERO - CAL_FAR)

#include "LCD.h"
#include "ADC.h"
#include <stdio.h>

void init()
{
    TRISB = 0;
    ANSELE = 0;
    TRISE = 0;
    //LATB = 0xFF;
    TRISD = 0;
    TRISFbits.TRISF5 = 1;
    ANSFbits.ANSELF5 = 1;
    
}

short int mean_signed_short(const short int arr[], int size)
{
    int i;
    long sum = 0;

    for(i = 0; i < size; i++)
    {
        sum += arr[i];
    }

    return sum / size;
}

short int getBaselineDifference(short int min, short int max)
{   
   return (min + max) / 2;
}

void boot()
{
      init();
    __delay_ms(1000);
    RED_LED = 1;    
    LCD_init();
    ADC16_init();
    TRISEbits.TRISE7 = 1;
    TRISEbits.TRISE6 = 1;
}

void warmup()
{
     LCD_print_string("Warming up...",0);
    
     /*for (int i = 0; i < 450; i++)
     {
        YELLOW_LED = 0;
        __delay_ms(1000);
        YELLOW_LED = 1;
        __delay_ms(1000);
        ClrWdt();
     }
      */
     __delay_ms(15000);
}

short int getAveragedReadings()
{
    signed short int readings_array[SAMPLING];
    
    for (int i = 0; i < SAMPLING; i++)
    {
        readings_array[i] = ADC16_read();
        __delay_ms(1);
     }
     return mean_signed_short(readings_array, SAMPLING);
}

short int getBaseline(int a)
{
    
    YELLOW_LED = 1;
    
    signed short int readings_array[SAMPLING];
   
    
    for (int i = 0; i < SAMPLING; i++)
    {
        readings_array[i] = ADC16_read();
        if ((readings_array[i] < 6550 || readings_array[i] > 6950) && a ==1) 
        {
            YELLOW_LED = 0;
            return -1;
        }
        __delay_ms(1);
     }
    GREEN_LED = 1;
    __delay_ms(115);
    GREEN_LED = 0;
    GREEN_LED = 1;
    __delay_ms(115);
    GREEN_LED = 0;
    GREEN_LED = 1;
    __delay_ms(115);
    GREEN_LED = 0;
     YELLOW_LED = 0;
     return mean_signed_short(readings_array, SAMPLING);
}


void update_lcd(char *maxres, char *minres, char *res)
{
    LCD_print_string(res, 0);
    LCD_command(0xc0);
    LCD_print_string("l:",1);
    LCD_print_string(minres,1);
    LCD_print_string("h:",1);
    LCD_print_string(maxres,1);
}

float value_map_1(float reading)
{
    if      (reading >= 0.985) return 0.00;
    else if (reading >= 0.945) return 0.25;
    else if (reading >= 0.930) return 0.50;
    else if (reading >= 0.920) return 0.75;
    else if (reading >= 0.900) return 1.00;

    else if (reading >= 0.885) return 1.25;
    else if (reading >= 0.870) return 1.50;
    else if (reading >= 0.855) return 1.75;
    else if (reading >= 0.840) return 2.00;

    else if (reading >= 0.825) return 2.25;
    else if (reading >= 0.810) return 2.50;
    else if (reading >= 0.795) return 2.75;
    else if (reading >= 0.780) return 3.00;

    else if (reading >= 0.765) return 3.25;
    else if (reading >= 0.750) return 3.50;
    else if (reading >= 0.735) return 3.75;
    else if (reading >= 0.720) return 4.00;

    else if (reading >= 0.705) return 4.25;
    else if (reading >= 0.690) return 4.50;
    else if (reading >= 0.675) return 4.75;

    else return 5.00;
}

float value_map_990(float reading)
{
    if      (reading >= 0.991) return 0.00;
    else if (reading >= 0.980) return 0.25;
    else if (reading >= 0.955) return 0.50;
    else if (reading >= 0.921) return 0.75;
    else if (reading >= 0.910) return 1.00;

    else if (reading >= 0.895) return 1.25;
    else if (reading >= 0.888) return 1.50;
    else if (reading >= 0.880) return 1.75;
    else if (reading >= 0.865) return 2.00;

    else if (reading >= 0.850) return 2.25;
    else if (reading >= 0.835) return 2.50;
    else if (reading >= 0.820) return 2.75;
    else if (reading >= 0.807) return 3.00;

    else if (reading >= 0.795) return 3.25;
    else if (reading >= 0.783) return 3.50;
    else if (reading >= 0.767) return 3.75;

    else return 4.00;
}

float value_map_980(float reading)
{
    if      (reading >= 0.985) return 0.00;
    else if (reading >= 0.975) return 0.25;
    else if (reading >= 0.955) return 0.50;
    else if (reading >= 0.925) return 0.75;
    else if (reading >= 0.910) return 1.00;

    else if (reading >= 0.895) return 1.25;
    else if (reading >= 0.888) return 1.50;
    else if (reading >= 0.867) return 1.75;
    else if (reading >= 0.852) return 2.00;

    else if (reading >= 0.810) return 2.25;
    else if (reading >= 0.832) return 2.50;
    else if (reading >= 0.814) return 2.75;
    else if (reading >= 0.792) return 3.00;

    else if (reading >= 0.785) return 3.25;
    else if (reading >= 0.770) return 3.50;
    else if (reading >= 0.755) return 3.75;

    else return 4.00;
}

float value_map_970(float reading)
{
    if      (reading >= 0.965) return 0.00;
    else if (reading >= 0.950) return 0.25;
    else if (reading >= 0.925) return 0.50;
    else if (reading >= 0.940) return 0.75;
    else if (reading >= 0.910) return 1.00; //OK

    else if (reading >= 0.920) return 1.25;
    else if (reading >= 0.910) return 1.50;
    else if (reading >= 0.900) return 1.75;
    else if (reading >= 0.890) return 2.00;

    else if (reading >= 0.880) return 2.25;
    else if (reading >= 0.870) return 2.50;
    else if (reading >= 0.860) return 2.75;
    else if (reading >= 0.850) return 3.00;

    else if (reading >= 0.840) return 3.25;
    else if (reading >= 0.830) return 3.50;
    else if (reading >= 0.820) return 3.75;
    else if (reading >= 0.810) return 4.00;

    else if (reading >= 0.800) return 4.25;
    else if (reading >= 0.790) return 4.50;
    else if (reading >= 0.780) return 4.75;

    else return 5.00;
}

float value_map_960(float reading)
{
    if      (reading >= 0.950) return 0.00;
    else if (reading >= 0.940) return 0.25;
    else if (reading >= 0.930) return 0.50;
    else if (reading >= 0.920) return 0.75;
    else if (reading >= 0.910) return 1.00;

    else if (reading >= 0.900) return 1.25;
    else if (reading >= 0.890) return 1.50;
    else if (reading >= 0.880) return 1.75;
    else if (reading >= 0.870) return 2.00;

    else if (reading >= 0.860) return 2.25;
    else if (reading >= 0.850) return 2.50;
    else if (reading >= 0.840) return 2.75;
    else if (reading >= 0.830) return 3.00;

    else if (reading >= 0.820) return 3.25;
    else if (reading >= 0.810) return 3.50;
    else if (reading >= 0.800) return 3.75;
    else if (reading >= 0.790) return 4.00;

    else if (reading >= 0.780) return 4.25;
    else if (reading >= 0.770) return 4.50;
    else if (reading >= 0.760) return 4.75;

    else return 5.00;
}

int main(void) 
{
    boot();
    
    signed short int result; 
    char res[15];
    char maxres[20] = "0";
    char minres[20] = "0";
    signed short int min = 30000;
    signed short int max = 0;
    signed short int baseline = 0;
    signed short int baseline1 = 10;
    signed short int bresult = 0;
    signed short int zero = 0;
    char bline[19] = "0";
    char value[10] = "0";
    
    float normalized_min = 3000;
    float normalized_max = 0;
    float normalized_result = 0;
    float gain_correction = 0;
    
    float corrected_baseline = 0;
    
    
    
    //warmup();     
   
 
        
    int counter = 0;
        
    YELLOW_LED = 0;
    GREEN_LED = 0;
        
    while(1)
    {  
        result = getAveragedReadings();
        //result = getAveragedReadings() - baseline;
        normalized_result = (float)(getAveragedReadings() - baseline) /(float)(baseline1 - baseline);
        gain_correction = (baseline1 - baseline) / (float)CAL_SPAN;
        normalized_result = normalized_result*gain_correction;
        
        
        
       if (normalized_result < normalized_min) 
        {
            normalized_min = normalized_result;
            sprintf(minres,"%.3f ",normalized_min);
        }
        if (normalized_result > normalized_max)
        {
          normalized_max = normalized_result;
          sprintf(maxres,"%.3f",normalized_max);
        }
        /*
         if (result < min) 
        {
            min = result;
            sprintf(minres,"%d ",min);
        }
        if (result > max)
        {
          max = result;
          sprintf(maxres,"%d",max);
        }*/
        //sprintf(res,"v:%d b:%d",result,baseline);
        //sprintf(res,"v:%d",result);
        
        //sprintf(res,"v:%.3f b:%d",normalized_result,baseline);
        
        if (PORTEbits.RE7 == 1)
        {
            baseline1 = getBaseline(0);
        }
        
        
        //update_lcd(maxres, minres, res);
        //update_lcd(maxres, minres, res);
        
        
        corrected_baseline = (baseline1 - baseline) / (float)CAL_SPAN;
        if (corrected_baseline<=0.970)
            sprintf(res,"t: %.3f a:%.3f",value_map_960(normalized_result), corrected_baseline);
        else if (corrected_baseline <= 0.980)
            sprintf(res,"t: %.3f b:%.3f",value_map_970(normalized_result), corrected_baseline);
        else if (corrected_baseline <= 0.990)
            sprintf(res,"t: %.3f c:%.3f",value_map_980(normalized_result), corrected_baseline);
        else if (corrected_baseline <= 1)
            sprintf(res,"t: %.3f d:%.3f",value_map_990(normalized_result), corrected_baseline);
        else sprintf(res,"t: %.3f e:%.3f",value_map_1(normalized_result), corrected_baseline);
        //sprintf(res,"r: %d",result);
    
        
        
        LCD_print_string(res, 0);
        
        LCD_command(0xc0);
        
        sprintf(res,"v: %.3f",normalized_result);
        
        LCD_print_string(res, 1);
        
        if (getAveragedReadings() < 10000)
        {        
            bresult = getBaseline(1);
            if (bresult != -1)
                baseline = bresult;
        }
        
        if (PORTEbits.RE6 == 1)
        {
            normalized_max = 0;
            normalized_min = 30000;
        }
        ClrWdt();
    }    
    return 0;
}
