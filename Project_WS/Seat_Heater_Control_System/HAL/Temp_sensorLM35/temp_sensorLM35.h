 /******************************************************************************
 *
 * Module: LM32_Sensor interface
 *
 * File Name: temp_sensorLM35.h
 *
 * Description: Header file for the LM35 Temperature Sensor Driver
 *
 * Author: Mariam Bestawrous
 *
 *******************************************************************************/

#ifndef HAL_TEMP_SENSORLM35_TEMP_SENSORLM35_H_
#define HAL_TEMP_SENSORLM35_TEMP_SENSORLM35_H_

#include"std_types.h"

#define LM35_ADC_CHANNEL1_ID                PE1_PIN
#define LM35_ADC_CHANNEL2_ID                PE1_PIN

#define LM35_MAX_VOLT                      (float32)1.5
#define LM35_MAX_Temp_Value                (uint8)150
#define LM35_MIN_TEMO_VAL                  (uint8)5
uint8 LM35_ReadTemp(uint8 channel);
#endif /* HAL_TEMP_SENSORLM35_TEMP_SENSORLM35_H_ */
