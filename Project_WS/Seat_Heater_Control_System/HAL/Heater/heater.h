 /******************************************************************************
 *
 * Module: Heater module
 *
 * File Name: heater.h
 *
 * Description: header file for the heater driver
 *
 * Author: Mariam Bestawrous
 *
 *******************************************************************************/

#ifndef HAL_HEATER_HEATER_H_
#define HAL_HEATER_HEATER_H_

#include "std_types.h"

#define DRIVER_HEATER_INSTANCE              0x00
#define PASSENGER_HEATER_INSTANCE           0x01
typedef enum {
    HEATER_OFF,
    HEATER_LOW,
    HEATER_MED,
    HEATER_HIGH
}HEATER_heaterState;


void HEATER_SetTempLevel(uint8 heater_instance, HEATER_heaterState intenisty);
#endif /* HAL_HEATER_HEATER_H_ */
