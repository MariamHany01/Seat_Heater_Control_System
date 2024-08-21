 /******************************************************************************
 *
 * Module: ADC
 *
 * File Name: adc.c
 *
 * Description: source file for the TM4C123GH6PM ADC driver for TivaC Analog to Digital converter
 *
* Author:Mariam Bestawrous
 *******************************************************************************/
#include "adc0.h"
#include "tm4c123gh6pm_registers.h"

void ADC0_EnableCLock(){
    /* RCC Registers */
    ADC_ADCRCGCADC_REG  |= (1<<0);               /* Enable ADC0 Clock */
}


uint32 ADC0_ReadChannel(ADC_channel_no channel)
{

    /* Initialize the sequencer */
    ADC0_ADCACTSS_REG     = (0x00000000);         /* De-activate all the sample sequencers */
    ADC0_ADCSSMUX0_REG    = channel;           /* the first sequencer of sequencer0 is for AIN1 */
    ADC0_ADCSSCTL0_REG   |= 0x6;                  /* The first sequencer is the last sequencer - Enable interrupt */
    ADC0_ADCACTSS_REG    |=(1<<0);

    /* Start Conversion */
    ADC0_ADCPSSI_REG = (ADC0_ADCPSSI_REG & 0xFFFFFFF0 ) | ( (1<<0) );

    /* Check on the flag till the conversation is done */
    while(!(ADC0_ADCRIS_REG & 0x00000001));

    /* Clear the flag */
    ADC0_ADCISC_REG |= 0x00000001;

    /* return the data */
    return (uint32) (ADC0_ADCSSFIFO0_REG);
}
