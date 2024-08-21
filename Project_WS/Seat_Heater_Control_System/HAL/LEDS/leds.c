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

#include "tm4c123gh6pm_registers.h"


void GPIO_RedLedOn(void)
{
    GPIO_PORTF_DATA_REG |= (1<<1);  /* Red LED ON */
}

void GPIO_BlueLedOn(void)
{
    GPIO_PORTF_DATA_REG |= (1<<2);  /* Blue LED ON */
}

void GPIO_GreenLedOn(void)
{
    GPIO_PORTF_DATA_REG |= (1<<3);  /* Green LED ON */
}

void GPIO_RedLedOff(void)
{
    GPIO_PORTF_DATA_REG &= ~(1<<1);  /* Red LED OFF */
}

void GPIO_BlueLedOff(void)
{
    GPIO_PORTF_DATA_REG &= ~(1<<2);  /* Blue LED OFF */
}

void GPIO_GreenLedOff(void)
{
    GPIO_PORTF_DATA_REG &= ~(1<<3);  /* Green LED OFF */
}

void GPIO_RedLedToggle(void)
{
    GPIO_PORTF_DATA_REG ^= (1<<1);  /* Red LED is toggled */
}

void GPIO_BlueLedToggle(void)
{
    GPIO_PORTF_DATA_REG ^= (1<<2);  /* Blue LED is toggled */
}

void GPIO_GreenLedToggle(void)
{
    GPIO_PORTF_DATA_REG ^= (1<<3);  /* Green LED is toggled */
}
