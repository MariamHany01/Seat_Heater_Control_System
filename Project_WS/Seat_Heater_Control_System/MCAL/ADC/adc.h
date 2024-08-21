 /******************************************************************************
 *
 * Module: ADC
 *
 * File Name: adc.h
 *
 * Description: header file for the TM4C123GH6PM ADC driver for TivaC Analog to Digital converter
 *
* Author:Mariam Bestawrous
 *******************************************************************************/

#ifndef MCAL_ADC_ADC_H_
#define MCAL_ADC_ADC_H_

#include "std_types.h"

//#define ADC_MAXIMUM_VALUE           0xFFF
//#define ADC_REF_VOLT_VALUE          3.3

/* Enumeration for Port_PinDirectionType used by the PORT APIs */
typedef enum
{
    ADC_PROCESSOR,
    ADC_ANALOG_COMPARATOR0,
    ADC_ANALOG_COMPARATOR1,
    ADC_GPIO_PINS,
    ADC_TIMER,
    ADC_PWM_GENERATOR1,
    ADC_PWM_GENERATOR2,
    ADC_PWM_GENERATOR3
}ADC_EventMultiplexerSelect;

/*******************************************************************************
 *                             Important Definitions                             *
 *******************************************************************************/
#define ADC_ASEN0 0
#define ADC_EM0 0
#define ADC_EM1 4
#define ADC_EM2 8
#define ADC_EM3 12
#define ADC_DITHER 6
#define ADC_VREF 0
#define ADC_AIN0 0

/*******************************************************************************
 *                            Function Prototypes                            *
 *******************************************************************************/

void ADC0_Init(void);
uint16 ADC0_ReadChannel();
void ADC1_Init(void);
uint16 ADC1_ReadChannel();


#endif /* MCAL_ADC_ADC_H_ */
