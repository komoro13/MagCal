/*
 * File:   newmainXC16.c
 * Author: thodo
 *
 * Created on March 11, 2026, 10:11 PM
 */

#define SAMPLING 500
#define CAL_FAR 6850
#define CAL_CLOSE 21050
#define CAL_SPAN (CAL_CLOSE - CAL_FAR)
#define FAR_BASELINE_CALIBRATE_TRIGGER_THRESHOLD 10000
#define CLOSE_CALIBRATION_REQUESTED() (PORTEbits.RE7 == 1)
#define FAR_BASELINE_MIN 6400
#define FAR_BASELINE_MAX 7000

#include "LCD.h"
#include "ADC.h"
#include "HLVD.h"
#include <stdio.h>

void init_GPIO()
{
    TRISB = 0;
    ANSELE = 0;
    TRISE = 0;
    TRISD = 0;
    TRISFbits.TRISF5 = 1;
    ANSFbits.ANSELF5 = 1;
    
}

void initialize_system()
{
    init_GPIO();
    __delay_ms(1000);
    RED_LED = 1;    
    HLVD_init();
    LCD_init();
    ADC16_init();
    TRISEbits.TRISE7 = 1;
    TRISEbits.TRISE6 = 1;
}

volatile int low_battery = 0;

void __attribute__((interrupt, no_auto_psv)) _LVDInterrupt(void)
{
    IEC4bits.HLVDIE = 0;      // prevent another HLVD interrupt
    IFS4bits.HLVDIF = 0;
    low_battery = 1;
}

void lowBatteryMessage()
{
    LCD_print_string("Change battery", 0);

    while(1)
    {
        ClrWdt();
    }
}

short int getAveragedReadings(void)
{
    long sum = 0;

    for (int i = 0; i < SAMPLING; i++)
    {
        sum += ADC16_read();
        __delay_ms(1);
    }

    return (short int)(sum / SAMPLING);
}

void calibrationSuccessSignal()
{
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
}

short int getBaseline(int validate_range)
{
    
    YELLOW_LED = 1;
    
    long sum = 0;
    short int read;
     
    for (int i = 0; i < SAMPLING; i++)
    {
        read = ADC16_read();
        sum += read;
        
        if ((read < FAR_BASELINE_MIN || read > FAR_BASELINE_MAX) && validate_range) 
        {
            YELLOW_LED = 0;
            return -1;
        }
        __delay_ms(1);
     }
     calibrationSuccessSignal();
     return (short int)(sum / SAMPLING);
}



typedef struct
{
    float threshold;
    float thickness;
} CalibrationPoint;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* corrected_baseline > 1.000 */
static const CalibrationPoint calibration_1[] =
{
    {0.985f, 0.00f},
    {0.945f, 0.25f},
    {0.930f, 0.50f},
    {0.920f, 0.75f},
    {0.900f, 1.00f},
    {0.885f, 1.25f},
    {0.870f, 1.50f},
    {0.855f, 1.75f},
    {0.840f, 2.00f},
    {0.825f, 2.25f},
    {0.810f, 2.50f},
    {0.795f, 2.75f},
    {0.780f, 3.00f},
    {0.765f, 3.25f},
    {0.750f, 3.50f},
    {0.735f, 3.75f},
    {0.720f, 4.00f},
    {0.705f, 4.25f},
    {0.690f, 4.50f},
    {0.675f, 4.75f}
};

/* corrected_baseline <= 1.000 */
static const CalibrationPoint calibration_990[] =
{
    {0.991f, 0.00f},
    {0.980f, 0.25f},
    {0.955f, 0.50f},
    {0.921f, 0.75f},
    {0.910f, 1.00f},
    {0.895f, 1.25f},
    {0.888f, 1.50f},
    {0.880f, 1.75f},
    {0.865f, 2.00f},
    {0.850f, 2.25f},
    {0.835f, 2.50f},
    {0.820f, 2.75f},
    {0.807f, 3.00f},
    {0.795f, 3.25f},
    {0.783f, 3.50f},
    {0.767f, 3.75f}
};

/* corrected_baseline <= 0.990 */
static const CalibrationPoint calibration_980[] =
{
    {0.985f, 0.00f},
    {0.975f, 0.25f},
    {0.955f, 0.50f},
    {0.925f, 0.75f},
    {0.910f, 1.00f},
    {0.895f, 1.25f},
    {0.888f, 1.50f},
    {0.867f, 1.75f},
    {0.852f, 2.00f},

    /* Verify these three against your calibration data:
       they are not in descending threshold order. */
    {0.810f, 2.25f},
    {0.832f, 2.50f},
    {0.814f, 2.75f},

    {0.792f, 3.00f},
    {0.785f, 3.25f},
    {0.770f, 3.50f},
    {0.755f, 3.75f}
};

/* corrected_baseline <= 0.980 */
static const CalibrationPoint calibration_970[] =
{
    {0.965f, 0.00f},
    {0.950f, 0.25f},

    /* Verify this region against your calibration data:
       thresholds are not monotonically descending. */
    {0.925f, 0.50f},
    {0.940f, 0.75f},
    {0.910f, 1.00f},
    {0.920f, 1.25f},
    {0.910f, 1.50f},

    {0.900f, 1.75f},
    {0.890f, 2.00f},
    {0.880f, 2.25f},
    {0.870f, 2.50f},
    {0.860f, 2.75f},
    {0.850f, 3.00f},
    {0.840f, 3.25f},
    {0.830f, 3.50f},
    {0.820f, 3.75f},
    {0.810f, 4.00f},
    {0.800f, 4.25f},
    {0.790f, 4.50f},
    {0.780f, 4.75f}
};

/* corrected_baseline <= 0.970 */
static const CalibrationPoint calibration_960[] =
{
    {0.950f, 0.00f},
    {0.940f, 0.25f},
    {0.930f, 0.50f},
    {0.920f, 0.75f},
    {0.910f, 1.00f},
    {0.900f, 1.25f},
    {0.890f, 1.50f},
    {0.880f, 1.75f},
    {0.870f, 2.00f},
    {0.860f, 2.25f},
    {0.850f, 2.50f},
    {0.840f, 2.75f},
    {0.830f, 3.00f},
    {0.820f, 3.25f},
    {0.810f, 3.50f},
    {0.800f, 3.75f},
    {0.790f, 4.00f},
    {0.780f, 4.25f},
    {0.770f, 4.50f},
    {0.760f, 4.75f}
};

float mapReading(float reading,
                 const CalibrationPoint *table,
                 unsigned int size,
                 float maximum_thickness)
{
    for (unsigned int i = 0; i < size; i++)
    {
        if (reading >= table[i].threshold)
            return table[i].thickness;
    }

    return maximum_thickness;
}

void updateResultBuffer(char *res,
                        float normalized_result,
                        float corrected_baseline)
{
    float thickness;
    char calibration_id;

    if (corrected_baseline <= 0.970f)
    {
        thickness = mapReading(
            normalized_result,
            calibration_960,
            ARRAY_SIZE(calibration_960),
            5.00f
        );

        calibration_id = 'a';
    }
    else if (corrected_baseline <= 0.980f)
    {
        thickness = mapReading(
            normalized_result,
            calibration_970,
            ARRAY_SIZE(calibration_970),
            5.00f
        );

        calibration_id = 'b';
    }
    else if (corrected_baseline <= 0.990f)
    {
        thickness = mapReading(
            normalized_result,
            calibration_980,
            ARRAY_SIZE(calibration_980),
            4.00f
        );

        calibration_id = 'c';
    }
    else if (corrected_baseline <= 1.000f)
    {
        thickness = mapReading(
            normalized_result,
            calibration_990,
            ARRAY_SIZE(calibration_990),
            4.00f
        );

        calibration_id = 'd';
    }
    else
    {
        thickness = mapReading(
            normalized_result,
            calibration_1,
            ARRAY_SIZE(calibration_1),
            5.00f
        );

        calibration_id = 'e';
    }

    sprintf(
        res,
        "t: %.3f %c:%.3f",
        thickness,
        calibration_id,
        corrected_baseline
    );
}

int main(void) 
{
    
    initialize_system();
    
    signed short int result; 
    char res[32];
        
    signed short int baselineFar = 0;
    signed short int baselineClose = 10;
    signed short int newFarBaseline = 0;

    float normalized_result = 0;
    float gain_correction = 0;
    
    float corrected_baseline = 0;
               
    YELLOW_LED = 0;
    GREEN_LED = 0;

    while(1)
    {  
        
        if (low_battery)
            lowBatteryMessage();
        
        result = getAveragedReadings();
        
        if ((baselineClose - baselineFar) != 0)
        {
            normalized_result = (float)(result - baselineFar) /(float)(baselineClose - baselineFar);
            gain_correction = (baselineClose - baselineFar) / (float)CAL_SPAN;
            normalized_result *= gain_correction;
        }
        
        if (CLOSE_CALIBRATION_REQUESTED())
        {
            baselineClose = getBaseline(0);
        }
        
        if (result < FAR_BASELINE_CALIBRATE_TRIGGER_THRESHOLD)
        {        
            newFarBaseline = getBaseline(1);
            if (newFarBaseline != -1)
                baselineFar = newFarBaseline;
        }
          
        corrected_baseline = (baselineClose - baselineFar) / (float)CAL_SPAN;
        
        updateResultBuffer(res, normalized_result, corrected_baseline);
        
        LCD_print_string(res, 0);
        
        LCD_command(0xc0);
        
        sprintf(res,"v: %.3f",normalized_result);
        
        LCD_print_string(res, 1);
        
        ClrWdt();
    }    
    return 0;
}
