/******************************************************************************
 *
 * Module: Buttons  module
 *
 * File Name: buttons.c
 *
 * Description: source file for the Buttons driver
 *
 * Author: Mariam Bestawrous
 *
 *******************************************************************************/

#include "../../MCAL/DIO/Dio.h"
#include "Buttons.h"

/*******************************************************************************************************************/
uint8 buttonCheckState(uint8 Button_PIN_NUM)
{
    uint8  state = Dio_ReadChannel(Button_PIN_NUM);

    return state;
}



