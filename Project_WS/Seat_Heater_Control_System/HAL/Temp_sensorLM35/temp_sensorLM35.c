 /******************************************************************************
 *
 * Module: LM32_Sensor interface
 *
 * File Name: temp_sensorLM35.c
 *
 * Description: Source file for the LM35 Temperature Sensor Driver
 *
 * Author: Mariam Bestawrous
 *
 *******************************************************************************/

#include"temp_sensorLM35.h"
#include"../../MCAL/ADC/adc0.h"

static uint8 conversion_eqn(uint32 ADC_reading){
    return (uint8)(((uint64)LM35_MAX_Temp_Value * ADC_reading * ADC_REF_VOLT_VALUE)/((float)LM35_MAX_VOLT * ADC_MAXIMUM_VALUE));
}
uint8 LM35_ReadTemp(uint8 channel){

    /*initialize variables to carry the reading from adc and temp after conversion */
    volatile uint32 adc_reading;
    volatile uint8 temp_val;

    /* adc read */
    adc_reading=ADC0_ReadChannel((ADC_channel_no)channel);
    temp_val=conversion_eqn(adc_reading);

    return temp_val;
}
