/******************************************************************************
 *
 * Module: Leds  module
 *
 * File Name: leds.h
 *
 * Description: source file for the Leds driver
 *
 * Author: Mariam Bestawrous
 *
 *******************************************************************************/

#ifndef HAL_LEDS_LEDS_H_
#define HAL_LEDS_LEDS_H_

void GPIO_RedLedOn(void);
void GPIO_BlueLedOn(void);
void GPIO_GreenLedOn(void);

void GPIO_RedLedOff(void);
void GPIO_BlueLedOff(void);
void GPIO_GreenLedOff(void);

void GPIO_RedLedToggle(void);
void GPIO_BlueLedToggle(void);
void GPIO_GreenLedToggle(void);
#endif /* HAL_LEDS_LEDS_H_ */
