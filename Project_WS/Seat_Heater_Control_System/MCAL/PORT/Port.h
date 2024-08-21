 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.h
 *
 * Description: Header file for TM4C123GH6PM Microcontroller - Port Driver.
 *
 * Author: Mariam Bestawrous
 ******************************************************************************/

#ifndef PORT_H
#define PORT_H

/* Id for the company in the AUTOSAR
 * for example Mariam Bestawrous's ID = 1000 :) */
#define PORT_VENDOR_ID    (1000U)

/* PORT Module Id */
#define PORT_MODULE_ID    (124U)

/* PORT Instance Id */
#define PORT_INSTANCE_ID  (0U)

/* total number of pins excluding JTAG pins */
#define TOTAL_NUMBER_OF_PINS    (39u)

/*PORT IDs */
#define PORTA   (0u)
#define PORTB   (1u)
#define PORTC   (2u)
#define PORTD   (3u)
#define PORTE   (4u)
#define PORTF   (5u)

/*PIN IDs */
#define PIN0   (0u)
#define PIN1   (1u)
#define PIN2   (2u)
#define PIN3   (3u)
#define PIN4   (4u)
#define PIN5   (5u)
#define PIN6   (6u)
#define PIN7   (7u)

/*GPIO Modes */
#define DEFAULT_GPIO_MODE (0u)


/*
 * Module Version 1.0.0
 */
#define PORT_SW_MAJOR_VERSION           (1U)
#define PORT_SW_MINOR_VERSION           (0U)
#define PORT_SW_PATCH_VERSION           (0U)

/*
 * AUTOSAR Version 4.0.3
 */
#define PORT_AR_RELEASE_MAJOR_VERSION   (4U)
#define PORT_AR_RELEASE_MINOR_VERSION   (0U)
#define PORT_AR_RELEASE_PATCH_VERSION   (3U)


/*
 * Macros for PORT Status
 */
#define PORT_INITIALIZED                (1U)
#define PORT_NOT_INITIALIZED            (0U)

/* Standard AUTOSAR types */
#include "Std_Types.h"

/* AUTOSAR checking between Std Types and PORT Modules */
#if ((STD_TYPES_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Std_Types.h does not match the expected version"
#endif

/* PORT Pre-Compile Configuration Header file */
#include "Port_Cfg.h"

/* AUTOSAR Version checking between PORT_Cfg.h and PORT.h files */
#if ((PORT_CFG_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Port_Cfg.h does not match the expected version"
#endif

/* Software Version checking between Dio_Cfg.h and Dio.h files */
#if ((PORT_CFG_SW_MAJOR_VERSION != PORT_SW_MAJOR_VERSION)\
 ||  (PORT_CFG_SW_MINOR_VERSION != PORT_SW_MINOR_VERSION)\
 ||  (PORT_CFG_SW_PATCH_VERSION != PORT_SW_PATCH_VERSION))
  #error "The SW version of PortCfg.h does not match the expected version"
#endif

/* Non AUTOSAR files */
#include "Common_Macros.h"


/******************************************************************************
 *                      API Service Id Macros                                 *
 ******************************************************************************/
/* service ID for Port_init fn */
#define PORT_INIT_SID                                (uint8)0x00

/* service ID for Port_SetPinDirection fn */
#define PORT_SET_PIN_DIRECTION_SID                   (uint8)0x01

/* service ID for Port_RefreshPortDirection  fn */
#define PORT_REFRESH_PORT_DIRECTION_SID              (uint8)0x02

/* service ID for Port_init fn */
#define PORT_GET_VERSION_INFO_SID                    (uint8)0x03

/* service ID for Port_SetPinMode fn */
#define PORT_SET_PIN_MODE_SID                        (uint8)0x04

/*******************************************************************************
 *                      DET Error Codes                                        *
 *******************************************************************************/
/* DET code to report Invalid Port Pin ID */
#define PORT_E_PARAM_PIN                    (uint8)0x0A

/* DET code to report Port Pin not configured as changeable */
#define PORT_E_DIRECTION_UNCHANGEABLE        (uint8)0x0B

/* DET code to report API Port_Init service called with wrong parameter*/
#define PORT_E_PARAM_CONFIG                  (uint8)0x0C

/* DET code to report API Port_SetPinModeservice called when mode is unchangeable*/
#define PORT_E_PARAM_INVALID_MODE            (uint8)0x0D
#define PORT_E_MODE_UNCHANGEABLE             (uint8)0X0E

/* DET code to report Port API service called without module initialization */
#define PORT_E_UNINIT                        (uint8)0x0F

/* DET code to report APIs called with a Null Pointer*/
#define PORT_E_PARAM_POINTER                 (uint8)0x10

/*******************************************************************************
 *                              Module Data Types                              *
 *******************************************************************************/
/* Description: uint8 holds pin_number for a certain port ( 8 as max)  */
typedef uint8 Port_PinType;

/* Description: Enum to hold PIN direction */
typedef enum
{
    INPUT,OUTPUT
}Port_PinDirection;

/* Description: uint to hold mode of the pin ( 15 as max ) (10:13 not used for that device) */
typedef uint8 Port_PinModeType;

/* Description: Enum to hold internal resistor type for PIN */
typedef enum
{
    OFF,PULL_UP,PULL_DOWN
}Port_InternalResistor;

/* Description: Enum to hold mode of the pin  from analog vs digital perspective  */
typedef enum {
    DIGITAL,ANALOG
}Port_Digital_Analog_mode_select;

/* Description: Enum to hold mode in case of output pin : open_drain or push_pull */
typedef enum {
    DISABLE_OPEN_DRAIN,ENABLE_OPEN_DRAIN
}Port_Open_Drain_Select;

/* Description: Structure to configure each individual PIN:
 *  1. the PORT Which the pin belongs to. 0, 1, 2, 3, 4 or 5
 *  2. the number of the pin in the PORT.
 *      3. the direction of pin --> INPUT or OUTPUT
 *      4. the internal resistor --> Disable, Pull up or Pull down
 */
typedef struct 
{
    uint8 port_num;
    uint8 pin_num;
    Port_PinDirection direction;
    Port_PinModeType pin_mode;
    Port_InternalResistor resistor;
    uint8 initial_value;
    Port_Digital_Analog_mode_select digital_analog_mode ;
    Port_Open_Drain_Select open_drain_select;
    uint8 pin_changable;
}Port_ConfigType;

extern  Port_ConfigType port_config[TOTAL_NUMBER_OF_PINS];/*array containing all pins configurations in Port_PBcfg.c */

/*******************************************************************************
 *                      Function Prototypes                                    *
 *******************************************************************************/

/************************************************************************************
* Service Name: Port_SetupGpioPin
* Sync/Async: Synchronous
* Reentrancy: reentrant
* Parameters (in): ConfigPtr - Pointer to post-build configuration data
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Function to Setup the pin configuration:
*              - Setup the pin as Digital GPIO pin
*              - Setup the direction of the GPIO pin
*              - Setup the internal resistor for i/p pin
************************************************************************************/
void Port_SetupGpioPin(const Port_ConfigType *ConfigPtr );

/************************************************************************************
* Service Name: Port_Init
* Sync/Async: Synchronous
* Reentrancy: Non Reentrant
* Parameters (in): ConfigPtr - Pointer to post-build configuration set
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: initializes the Port Driver module.
************************************************************************************/
void Port_Init(const Port_ConfigType *ConfigPtr);

/************************************************************************************
* Service Name: Port_SetPinDirection
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): pin - Port Pin ID number  and Direction - Port Pin Direction
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Sets the port pin direction.
************************************************************************************/
void Port_SetPinDirection(const Port_PinType Pin , const  Port_PinDirection Direction );


/************************************************************************************
* Service Name: Port_SetPinMode
* Sync/Async: Synchronous
* Reentrancy: Reentrant
* Parameters (in): pin - Port Pin ID number  and Mode - Port Pin mode
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Sets the port pin mode.
************************************************************************************/
void Port_SetPinMode(
                    Port_PinType Pin,
                    Port_PinModeType Mode);


/************************************************************************************
* Service Name: Port_RefreshPortDirection
* Sync/Async: Synchronous
* Reentrancy: Non Reentrant
* Parameters (in): None
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Refreshes port direction.
************************************************************************************/
void Port_RefreshPortDirection (void);

/************************************************************************************
* Service Name: Port_SetPinDirection
* Sync/Async: Synchronous
* Reentrancy: on Reentrant
* Parameters (in): pin - Port Pin ID number  and Direction - Port Pin Direction
* Parameters (inout): None
* Parameters (out): None
* Return value: None
* Description: Sets the port pin direction.
************************************************************************************/
void Port_GetVersionInfo(
             Std_VersionInfoType* versioninfo
            );


#endif /* PORT_H */
