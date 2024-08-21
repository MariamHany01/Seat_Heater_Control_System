 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.c
 *
 * Description: Source file for TM4C123GH6PM Microcontroller - Port Driver.
 *
 * Author: Mariam Bestawrous
 ******************************************************************************/

#include "Port.h"
#include "Port_Regs.h"
#include "Det.h"

/* global static variable containing state of port module weather it's initialized or not */
static uint8 g_port_status=PORT_NOT_INITIALIZED;
static Port_ConfigType * g_initialized_array_ptr = NULL_PTR ; /* ptr to the array in the PORT_PBcfg.c */
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
void Port_Init(const Port_ConfigType * ConfigPtr ){
    uint8 pin_num = 0 ;/*counter indicating initialized pins */
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* check if the input configuration pointer is not a NULL_PTR */
    if (NULL_PTR == ConfigPtr){
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT_SID,PORT_E_PARAM_CONFIG);
    }
    else
#endif
    
    /* loop on the content of port_config array to initialize each pin in it */
    for (;pin_num<TOTAL_NUMBER_OF_PINS;pin_num++){
       volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */

       switch(ConfigPtr[pin_num].port_num)
            {
              case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
                       break;
              case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
                       break;
              case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
                       break;
              case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
                       break;
              case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
                       break;
              case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
                       break;
            }


            if( ((ConfigPtr[pin_num].port_num == 3) && (ConfigPtr[pin_num].pin_num == 7)) || ((ConfigPtr[pin_num].port_num == 5) && (ConfigPtr[pin_num].pin_num == 0)) ) /* PD7 or PF0 */
            {
                *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;                     /* Unlock the GPIOCR register */
                SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , ConfigPtr[pin_num].pin_num);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
            }
            else if( (ConfigPtr[pin_num].port_num == 2) && (ConfigPtr[pin_num].pin_num <= 3) ) /* PC0 to PC3 */
            {
                /* Do Nothing ...  this is the JTAG pins */
            }
            else
            {
                /* Do Nothing ... No need to unlock the commit register for this pin */
            }

            if(ConfigPtr[pin_num].direction == OUTPUT)
            {
              SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , ConfigPtr[pin_num].pin_num);               /* Set the corresponding bit in the GPIODIR register to configure it as output pin */

              /* open drain settings  */
              *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ODR_REG_OFFSET)                    /* accessing the register */
                                     =(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ODR_REG_OFFSET)&( 0XFFFFFFFF & ~(1 << ConfigPtr[pin_num].pin_num)))    /* clearing the pin bits */
                                     |(ConfigPtr->open_drain_select << ConfigPtr[pin_num].pin_num) ;      /*  insert the analog digital mode */

                if(ConfigPtr[pin_num].initial_value == STD_HIGH)
                {
                    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , ConfigPtr[pin_num].pin_num);          /* Set the corresponding bit in the GPIODATA register to provide initial value 1 */
                }
                else
                {
                    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , ConfigPtr[pin_num].pin_num);        /* Clear the corresponding bit in the GPIODATA register to provide initial value 0 */
                }
            }
            else if(ConfigPtr[pin_num].direction == INPUT)
            {
                CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , ConfigPtr[pin_num].pin_num);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */

                if(ConfigPtr[pin_num].resistor == PULL_UP)
                {
                    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , ConfigPtr[pin_num].pin_num);       /* Set the corresponding bit in the GPIOPUR register to enable the internal pull up pin */
                }
                else if(ConfigPtr[pin_num].resistor == PULL_DOWN)
                {
                    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , ConfigPtr[pin_num].pin_num);     /* Set the corresponding bit in the GPIOPDR register to enable the internal pull down pin */
                }
                else
                {
                    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , ConfigPtr[pin_num].pin_num);     /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
                    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , ConfigPtr[pin_num].pin_num);   /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
                }
            }
            else
            {
                /* Do Nothing */
            }

            /* Setup the pin mode  */

            if (ConfigPtr[pin_num].digital_analog_mode == DIGITAL){

                SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , ConfigPtr[pin_num].pin_num);             /* Enable digital */
                CLEAR_BIT( *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , ConfigPtr[pin_num].pin_num);             /* Disable analog */

            }
            else {
                CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , ConfigPtr[pin_num].pin_num);             /* disable digital */
                SET_BIT( *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , ConfigPtr[pin_num].pin_num);             /* enable analog */

            }
             /* for alternate and mode registers */
            if (ConfigPtr[pin_num].pin_mode == DEFAULT_GPIO_MODE) /* in case of GPIO */
            {
                CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , ConfigPtr[pin_num].pin_num);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
                *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (ConfigPtr[pin_num].pin_num * 4));     /* Clear the PMCx bits for this pin */
            }
            else /* in case of other modes */
            {
                   SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , ConfigPtr[pin_num].pin_num);             /* Enable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */


                   *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) =               /* accessing the register */
                            (*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &~ (0x0000000F << (ConfigPtr[pin_num].pin_num * 4) ))     /* clearing the corresponding bits */
                            | ( ConfigPtr[pin_num].pin_mode << (ConfigPtr[pin_num].pin_num * 4));     /* inserting the mode */

            }

    }
    g_port_status=PORT_INITIALIZED;
    g_initialized_array_ptr= ConfigPtr;
}
/* keep fn if it's enabled */
#if (PORT_SET_PIN_DIRECTION_API == STD_ON)

void Port_SetPinDirection(const Port_PinType Pin , const  Port_PinDirection Direction ){
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* check if Port module is not initialized */
    if (g_port_status == PORT_NOT_INITIALIZED){
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID,PORT_E_UNINIT);
    }
    else{
        /* no action needed */
    }
#endif

#if (PORT_DEV_ERROR_DETECT == STD_ON)
        /* check if pin is withing range */
     if (Pin < TOTAL_NUMBER_OF_PINS){
         Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID,PORT_E_PARAM_PIN);
     }
     else {
         /* no action needed */
     }
#endif
#if (PORT_DEV_ERROR_DETECT == STD_ON)
        /* check if pin changeable or not */
     if (g_initialized_array_ptr[Pin].pin_changable == STD_OFF){
         Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID,PORT_E_DIRECTION_UNCHANGEABLE);
     }
     else {
         /* no action needed */
     }
#endif
//     g_initialized_array_ptr=g_initialized_array_ptr[Pin];
     volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */

     switch(g_initialized_array_ptr[Pin].port_num)
     {
         case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
             break;
         case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
             break;
         case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
             break;
         case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
             break;
         case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
              break;
         case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
             break;
      }


     if( ((g_initialized_array_ptr[Pin].port_num == 3) && (g_initialized_array_ptr[Pin].pin_num == 7)) || ((g_initialized_array_ptr[Pin].port_num == 5) && (g_initialized_array_ptr[Pin].pin_num == 0)) ) /* PD7 or PF0 */
         {
            *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;                     /* Unlock the GPIOCR register */
              SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , Pin);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
          }
     else if( (g_initialized_array_ptr[Pin].port_num == 2) && (g_initialized_array_ptr[Pin].pin_num <= 3) ) /* PC0 to PC3 */
          {
              /* Do Nothing ...  this is the JTAG pins */
          }
     else
          {
             /* Do Nothing ... No need to unlock the commit register for this pin */
          }


     if(Direction == OUTPUT)
       {
              SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , g_initialized_array_ptr[Pin].pin_num);               /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
       }
    else if(Direction == INPUT)
        {
                     CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , g_initialized_array_ptr[Pin].pin_num);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
        }
   else
        {
           /* Do Nothing */
        }
  g_initialized_array_ptr[Pin].direction=Direction;

}

#endif


#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo(Std_VersionInfoType *versioninfo)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* Check if input pointer is not Null pointer */
    if(NULL_PTR == versioninfo)
    {
        /* Report to DET  */
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_GET_VERSION_INFO_SID,PORT_E_PARAM_POINTER);

    }
    else
#endif /* (PORT_DEV_ERROR_DETECT == STD_ON) */
    {
        /* Copy the vendor Id */
        versioninfo->vendorID = (uint16)PORT_VENDOR_ID;
        /* Copy the module Id */
        versioninfo->moduleID = (uint16)PORT_MODULE_ID;
        /* Copy Software Major Version */
        versioninfo->sw_major_version = (uint8)PORT_SW_MAJOR_VERSION;
        /* Copy Software Minor Version */
        versioninfo->sw_minor_version = (uint8)PORT_SW_MINOR_VERSION;
        /* Copy Software Patch Version */
        versioninfo->sw_patch_version = (uint8)PORT_SW_PATCH_VERSION;
    }
}
#endif

#if (PORT_SET_PIN_MODE_API == STD_ON)
void Port_SetPinMode(Port_PinType Pin, Port_PinModeType Mode){
#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* check if Port module is not initialized */
    if (g_port_status == PORT_NOT_INITIALIZED){
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID,PORT_E_UNINIT);
    }
    else{
        /* no action needed */
    }
#endif

#if (PORT_DEV_ERROR_DETECT == STD_ON)
        /* check if pin is withing range */
     if (Pin < TOTAL_NUMBER_OF_PINS){
         Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID,PORT_E_PARAM_PIN);
     }
     else {
         /* no action needed */
     }
#endif
#if (PORT_DEV_ERROR_DETECT == STD_ON)
        /* check if pin changeable or not */
     if (g_initialized_array_ptr[Pin].pin_changable == STD_OFF){
         Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_MODE_SID,PORT_E_DIRECTION_UNCHANGEABLE);
     }
     else {
         /* no action needed */
     }
#endif
     volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
     switch(g_initialized_array_ptr[Pin].port_num)
            {
            case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
                  break;
             case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
                   break;
             case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
                   break;
             case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
                   break;
             case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
                   break;
             case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
                   break;
            }
     /* for alternate and mode registers */
     if ( Mode== DEFAULT_GPIO_MODE) /* in case of GPIO */
          {
             CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) ,g_initialized_array_ptr[Pin].pin_num);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
             *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (g_initialized_array_ptr[Pin].pin_num * 4));     /* Clear the PMCx bits for this pin */
          }
     else /* in case of other modes */
          {
             SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) ,g_initialized_array_ptr[Pin].pin_num);             /* Enable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */

             *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) =               /* accessing the register */
                                 (*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &~ (0x0000000F << (Pin* 4) ))     /* clearing the corresponding bits */
                                 | ( Mode << (g_initialized_array_ptr[Pin].pin_num * 4));     /* inserting the mode */

                 }
     g_initialized_array_ptr[Pin].pin_mode=Mode;

}
#endif



void Port_RefreshPortDirection (void){

#if (PORT_DEV_ERROR_DETECT == STD_ON)
    /* check if Port module is not initialized */
    if (g_port_status == PORT_NOT_INITIALIZED){
        Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_REFRESH_PORT_DIRECTION_SID,PORT_E_UNINIT);
    }
    else{
        /* no action needed */
    }
#endif
    uint8 pin_num = 0 ;/*counter indicating initialized pins */

      /* loop on the content of port_config array to initialize each pin in it */
      for (;pin_num<TOTAL_NUMBER_OF_PINS;pin_num++){
         volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */

         switch(g_initialized_array_ptr[pin_num].port_num)
              {
                case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
                         break;
                case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
                         break;
                case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
                         break;
                case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
                         break;
                case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
                         break;
                case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
                         break;
              }


              if( ((g_initialized_array_ptr[pin_num].port_num == 3) && (g_initialized_array_ptr[pin_num].pin_num == 7)) || ((g_initialized_array_ptr[pin_num].port_num == 5) && (g_initialized_array_ptr[pin_num].pin_num == 0)) ) /* PD7 or PF0 */
              {
                  *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;                     /* Unlock the GPIOCR register */
                  SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , g_initialized_array_ptr[pin_num].pin_num);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
              }
              else if( (g_initialized_array_ptr[pin_num].port_num == 2) && (g_initialized_array_ptr[pin_num].pin_num <= 3) ) /* PC0 to PC3 */
              {
                  /* Do Nothing ...  this is the JTAG pins */
              }
              else
              {
                  /* Do Nothing ... No need to unlock the commit register for this pin */
              }

              if(g_initialized_array_ptr[pin_num].direction == OUTPUT)
              {
                SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , g_initialized_array_ptr[pin_num].pin_num);               /* Set the corresponding bit in the GPIODIR register to configure it as output pin */

              }
              else if(g_initialized_array_ptr[pin_num].direction == INPUT)
              {
                  CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , g_initialized_array_ptr[pin_num].pin_num);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
              }
              else
              {
                  /* Do Nothing */
              }

      }
}

