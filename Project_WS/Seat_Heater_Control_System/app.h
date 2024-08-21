 /******************************************************************************
 *
 * Module: Application
 *
 * File Name: app.h
 *
 * Description: Application header file containing tasks prototypes
 *
 * Author:Mariam Bestawrous
 *
 *******************************************************************************/


#ifndef APPLICATION_APP_H_
#define APPLICATION_APP_H_

#include "std_types.h"

#define DRIVER_ID                                     0x00
#define PASSENGER_ID                                  0x01

#define DRIVER_BTN                                    4/*SW1*/
#define Passenger_BTN                                 0/*SW2*/

#define MAX_VALID_TEMP                                  45
#define MIN_VALID_TEMP                                   5
#define ADC_FULL_SCALE_VALUE                            4095

#define RUNTIME_MEASUREMENTS_TASK_PERIODICITY         (2000u)

/* defining the bits for event group */
#define DRIVER_HEAT_CONFIG_CHANGED_EVENT                ( 1UL << 0UL )
#define PASSENGER_HEAT_CONFIG_CHANGED_EVENT              ( 1UL << 1UL )


typedef enum{
    SEAT_HEATING_OFF,
    LOW_SEAT_HEAT_TEMP=25,
    MEDIUM_SEAT_HEAT_TEMP=30,
    HIGHSEAT_HEAT_TEMP=35

}APP_userDesiredHeatLevel;



/*struct for failure logging system */
typedef struct{
  uint8 *failureMsg;
  uint32 timeStamp; /*time where the failure occures */
  uint8 heatLevel;
}Failure;

/*struct for monitoring task to display the current state of the system on a screen via UART*/
typedef struct{
   const uint8 userId;      /*driver / passenger id*/
   const uint8 btnId;       /*pin of the btn */
    uint8 btnPressCount;    /*number of btn presses */
    uint8 currentHeatVal;   /* current heat temperature value */

}APP_UserState;

/* Description: Manage the heating level based on number of btn presses
 * Parameters:Pointer to TaskInformation structure
 * return:nothing
 * Instance:this Task has 2 instances one for driver and other for passenger
 * Event base Fn*/
void vBtnPressMangeTask(void *pvParameters);

/* Description:Read the current temperature
 * Parameters:Pointer to TaskInformation structure
 * return:nothing
 * Instances: 1 instance
 * Periodic Task : every 300 ms */
void vTemperatureMonitorTask(void *pvParameters);

/* Description:Control heater intensity
 * Parameters:Pointer to TaskInformation structure
 * return:nothing
 * Instance:this Task has 2 instances one for driver and other for passenger
 * Periodic Task : every  500 ms */
void vHeaterControlTask(void *pvParameters);

/* Description:update the displayed screen on the monitor
 * Parameters:Pointer to TaskInformation structure
 * return:nothing
 * Instances: 1 instance
 * Periodic Task : every 3 sec */
void vDisplayUpdateTask(void *pvParameters);

/* Description:Cpu load calc
 * Parameters:Pointer to TaskInformation structure
 * return:nothing
 * Instances: 1 instance
 * Periodic Task : every 2 sec */
void vcpuLoadMeasurementTask(void *pvParameters);

/* Description:Display on the screen if there is any failure logs
 * Parameters:Pointer to TaskInformation structure
 * return:nothing
 * Instances: 1 instance
 * Periodic Task : every 1.5 s */
void vFailureLoggingTask(void *pvParameters);

#endif /* APPLICATION_APP_H_ */
