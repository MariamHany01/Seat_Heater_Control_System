/******************************************************************************
 *
 * Module: Buttons  module
 *
 * File Name: buttons_Cfg.h
 *
 * Description: configuration  file for the Buttons driver
 *
 * Author: Mariam Bestawrous
 *
 *******************************************************************************/
#ifndef HAL_BUTTONS_BUTTONS_CFG_H_
#define HAL_BUTTONS_BUTTONS_CFG_H_

/* Button State according to its configuration PULL UP/Down */
#define BUTTON_PRESSED  STD_LOW
#define BUTTON_RELEASED STD_HIGH

/* Set the Button Port */
#define SW1_BUTTON_PORT DioConf_SW1_PORT_NUM
#define SW2_BUTTON_PORT DioConf_SW2_PORT_NUM

/* Set the Button Pin Number */
#define SW1_BUTTON_PIN_NUM DioConf_SW1_CHANNEL_NUM
#define SW1_BUTTON_PIN_NUM_INDEX DioConf_SW1_CHANNEL_ID_INDEX
#define SW2_BUTTON_PIN_NUM DioConf_SW2_CHANNEL_NUM
#define SW2_BUTTON_PIN_NUM_INDEX DioConf_SW2_CHANNEL_ID_INDEX


#endif /* HAL_BUTTONS_BUTTONS_CFG_H_ */
