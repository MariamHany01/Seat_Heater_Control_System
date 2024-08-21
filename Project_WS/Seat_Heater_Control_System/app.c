 /******************************************************************************
 *
 * Module: Application
 *
 * File Name: app.c
 *
 * Description: Application source file containing tasks def
 *
 * Author:Mariam Bestawrous
 *
 *******************************************************************************/
#include "MCAL/ADC/adc.h"
#include "MCAL/PORT/Port.h"
#include "MCAL/DIO/Dio.h"
#include"GPTM.h"
#include"uart0.h"
#include"tm4c123gh6pm_registers.h"


//#include"../HAL/Temp_sensorLM35/temp_sensorLM35.h"
#include"../HAL/Heater/heater.h"
#include"../HAL/LEDS/leds.h"


/* Kernel includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"


#include "app.h"
/*an array where you can access the desired level based on the number of btn presses  */
const static APP_userDesiredHeatLevel APP_NumOfBtnpressToHeatLevel[4]={SEAT_HEATING_OFF,LOW_SEAT_HEAT_TEMP,MEDIUM_SEAT_HEAT_TEMP,HIGHSEAT_HEAT_TEMP};

/* Global variables for desired temperature by users */
static volatile APP_userDesiredHeatLevel enDriverSeatDesiredTemp = SEAT_HEATING_OFF;
static volatile APP_userDesiredHeatLevel enPassengerSeatDesiredTemp = SEAT_HEATING_OFF;

/*Global variables for intensity of heater*/
static volatile HEATER_heaterState enDriverHeatingIntenisty = HEATER_OFF;
static volatile HEATER_heaterState enPassengerHeatingIntenisty = HEATER_OFF;

/*Global variables for current temperature value */
volatile float32 u32DriverSeat_CurrTemp=0;
volatile float32 u32PassengerSeat_CurrTemp=0;

/*array containing the failure logs with there details */
volatile Failure FailureLogArr[5];
static uint8 failureLogIndex = 0;

volatile APP_UserState DriverState={DRIVER_ID , DRIVER_BTN , 0 , HEATER_OFF};
volatile APP_UserState PassengerState={PASSENGER_ID , Passenger_BTN , 0 , HEATER_OFF};

///* Runtime measurements */
//uint32 ullTasksOutTime[10];
//uint32 ullTasksInTime[10];
//uint32 ullTasksExecutionTime[10];
//uint8 u8CPU_Load=0;                                           /* Variable to hold CPU load */


/* Event group handle */
EventGroupHandle_t xEventGroup;

xSemaphoreHandle xDriverButtonSemaphore;
xSemaphoreHandle xPassengerButtonSemaphore;

xSemaphoreHandle xUARTMutex;

/* Arrays to store task execution times */
uint32 ullTasksOutTime[10];
uint32 ullTasksInTime[10];
uint32 ullTasksTotalTime[10];

/* Task handles */
TaskHandle_t BtnPressMangeTasDriverkHandle , BtnPressMangeTasPassengerkHandle;
TaskHandle_t TemperatureMonitorTaskHandle;
TaskHandle_t HeaterControlTaskDriverHandle, HeaterControlTaskPassengerHandle;
TaskHandle_t DisplayUpdateTaskHandle;
TaskHandle_t CpuLoadMeasurementTaskHandle;
TaskHandle_t FailureLoggingTaskHandle;

/* non-static fns implementation */

void vBtnPressMangeTask(void *pvParameters){
    /* pointer to APP_UserState which an input parameter to the task to handle both seats */
    APP_UserState* pUserState = (APP_UserState*)pvParameters;
    /*assign a the seat btn press semaphore to a local variable to the task */
    SemaphoreHandle_t semaphore = (pUserState->userId == DRIVER_ID) ? xDriverButtonSemaphore : xPassengerButtonSemaphore;

    for(;;){
        /* wait for the btn press event */
        if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
            /*increment btn press count */
            pUserState->btnPressCount++;
            /* assign the global variable of DesiredTemp based on number of btn presses */
            if (pUserState->userId == DRIVER_ID) {
                enDriverSeatDesiredTemp = APP_NumOfBtnpressToHeatLevel[pUserState->btnPressCount];
                xEventGroupSetBits(xEventGroup, DRIVER_HEAT_CONFIG_CHANGED_EVENT);
           }
            else {
                enPassengerSeatDesiredTemp = APP_NumOfBtnpressToHeatLevel[pUserState->btnPressCount];
                xEventGroupSetBits(xEventGroup, PASSENGER_HEAT_CONFIG_CHANGED_EVENT);

                }
           }
        }
}

void vTemperatureMonitorTask(void *pvParameters) {

    /* Task periodicity of 300 ms */
    const TickType_t xPeriodicity = pdMS_TO_TICKS(300);
    /* Initialize xLastWakeTime to the current tick count */
     TickType_t xLastWakeTime = xTaskGetTickCount();
     /*for measurement purpose */

     for(;;) {

         /* Read temperature from ADC and map to 0°C - 45°C range */
         u32DriverSeat_CurrTemp = (ADC0_ReadChannel() * MAX_VALID_TEMP) /( (float32) ADC_FULL_SCALE_VALUE );        /* Read and calculate driver seat temperature */
         DriverState.currentHeatVal=u32DriverSeat_CurrTemp;
         u32PassengerSeat_CurrTemp = (ADC1_ReadChannel() * MAX_VALID_TEMP) / ( (float32) ADC_FULL_SCALE_VALUE );    /* Read and calculate passenger seat temperature */
         PassengerState.currentHeatVal=u32PassengerSeat_CurrTemp;
         /* Check if the temperature is out of  the valid range */
         if ( (u32DriverSeat_CurrTemp < 5 || u32DriverSeat_CurrTemp > 50 ) ||
                 (u32PassengerSeat_CurrTemp < 5 || u32PassengerSeat_CurrTemp > 40 ) ) {
             /* Log failure and update the state */
             FailureLogArr[failureLogIndex].failureMsg = "Sensor Failure";
             FailureLogArr[failureLogIndex].timeStamp = xTaskGetTickCount()/configTICK_RATE_HZ;/*divide by configTICK_RATE_HZ to convert to secs*/

             /*check where the source of failure */
             if((u32DriverSeat_CurrTemp < 5 || u32DriverSeat_CurrTemp > 45 ) ){
                 enDriverHeatingIntenisty = HEATER_OFF;
                 FailureLogArr[failureLogIndex].heatLevel = APP_NumOfBtnpressToHeatLevel[DriverState.btnPressCount];
                 /* Turn on the red LED */
                 Dio_WriteChannel(DioConf_GREEN_LED_CHANNEL_ID_INDEX, STD_OFF);
                 Dio_WriteChannel(DioConf_BLUE_LED_CHANNEL_ID_INDEX, STD_OFF);
                 Dio_WriteChannel(DioConf_RED_LED_CHANNEL_ID_INDEX, STD_ON);

             }
             else {
                 enPassengerHeatingIntenisty = HEATER_OFF;
                 FailureLogArr[failureLogIndex].heatLevel = APP_NumOfBtnpressToHeatLevel[PassengerState.btnPressCount];
                 /* Turn on the red LED */
                 Dio_WriteChannel(DioConf_GREEN_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
                 Dio_WriteChannel(DioConf_BLUE_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
                 Dio_WriteChannel(DioConf_RED_LED_OUT_CHANNEL_ID_INDEX, STD_ON);

             }
             /*update the failure index */
             if (failureLogIndex < 4){
                 failureLogIndex++;
             }
             else {
                 failureLogIndex=0;
             }


             }
         vTaskDelayUntil(&xLastWakeTime, xPeriodicity);

         }
 }

void vHeaterControlTask(void *pvParameters) {
    const TickType_t xPeriodicity = pdMS_TO_TICKS(500);
    /* Initialize xLastWakeTime to the current tick count */
     TickType_t xLastWakeTime = xTaskGetTickCount();
    /* create a pointer to UserState which is an input parameter to the Task */
    APP_UserState *pUserState = (APP_UserState *) pvParameters;
    HEATER_heaterState enHeatingIntensity = HEATER_OFF;
    EventBits_t uxBitsToWaitFor;
    EventBits_t xEventGroupValue;
    uxBitsToWaitFor = (pUserState->userId == DRIVER_ID) ? DRIVER_HEAT_CONFIG_CHANGED_EVENT : PASSENGER_HEAT_CONFIG_CHANGED_EVENT;
    for(;;){

        xEventGroupValue= xEventGroupWaitBits(xEventGroup,uxBitsToWaitFor,TRUE,FALSE,portMAX_DELAY);

        /* wait for the event */
        if ( (xEventGroupValue & uxBitsToWaitFor) != 0){
            /*update the heating internsity of the heater based on the desired and current temperature */
            if (pUserState->currentHeatVal <= (APP_NumOfBtnpressToHeatLevel[pUserState->btnPressCount] - 10) ) {
                enHeatingIntensity = HEATER_HIGH;
                /* leds for testing purpose only */
                /* Cyan color for high intensity */
                if (pUserState->userId == DRIVER_ID){
                    Dio_WriteChannel(DioConf_GREEN_LED_CHANNEL_ID_INDEX, STD_ON);
                    Dio_WriteChannel(DioConf_BLUE_LED_CHANNEL_ID_INDEX, STD_ON);
                }
                else if (pUserState->userId == PASSENGER_ID){
                    Dio_WriteChannel(DioConf_GREEN_LED_OUT_CHANNEL_ID_INDEX, STD_ON);
                    Dio_WriteChannel(DioConf_BLUE_LED_OUT_CHANNEL_ID_INDEX, STD_ON);
                }
            }

            else if (pUserState->currentHeatVal <= (APP_NumOfBtnpressToHeatLevel[pUserState->btnPressCount] - 5) ) {
                enHeatingIntensity = HEATER_MED;
                /* leds for testing purpose only */
               /* Blue color for medium intensity */
                if (pUserState->userId == DRIVER_ID){
                    Dio_WriteChannel(DioConf_GREEN_LED_CHANNEL_ID_INDEX, STD_OFF);
                    Dio_WriteChannel(DioConf_BLUE_LED_CHANNEL_ID_INDEX, STD_ON);
                }
                else if (pUserState->userId == PASSENGER_ID){
                    Dio_WriteChannel(DioConf_GREEN_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
                    Dio_WriteChannel(DioConf_BLUE_LED_OUT_CHANNEL_ID_INDEX, STD_ON);
                }
            }
            else if (pUserState->currentHeatVal <= (35 - 2) ) {
                enHeatingIntensity = HEATER_LOW;
                /* leds for testing purpose only */
               /* Green color for Low intensity */
                if (pUserState->userId == DRIVER_ID){
                    Dio_WriteChannel(DioConf_GREEN_LED_CHANNEL_ID_INDEX, STD_ON);
                    Dio_WriteChannel(DioConf_BLUE_LED_CHANNEL_ID_INDEX, STD_OFF);
                }
                else if (pUserState->userId == PASSENGER_ID){
                    Dio_WriteChannel(DioConf_GREEN_LED_OUT_CHANNEL_ID_INDEX, STD_ON);
                    Dio_WriteChannel(DioConf_BLUE_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
                }
            }
            else if (pUserState->currentHeatVal >= APP_NumOfBtnpressToHeatLevel[pUserState->btnPressCount] ) {
                enHeatingIntensity = HEATER_OFF;
                /* leds for testing purpose only */
               /* Leds off */
                if (pUserState->userId == DRIVER_ID){
                    Dio_WriteChannel(DioConf_GREEN_LED_CHANNEL_ID_INDEX, STD_OFF);
                    Dio_WriteChannel(DioConf_BLUE_LED_CHANNEL_ID_INDEX, STD_OFF);
                }
                else if (pUserState->userId == PASSENGER_ID){
                    Dio_WriteChannel(DioConf_GREEN_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
                    Dio_WriteChannel(DioConf_BLUE_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
                }
            }
            if(pUserState->userId==DRIVER_ID){
                enDriverHeatingIntenisty =enHeatingIntensity;
            }
            else if(pUserState->userId==PASSENGER_ID){
                enPassengerHeatingIntenisty=enHeatingIntensity;
            }
            if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {

            UART0_SendString("Heating level changed\n\r");

            }
            xSemaphoreGive(xUARTMutex);

        }
        vTaskDelayUntil(&xLastWakeTime, xPeriodicity);


    }
}
void vDisplayUpdateTask(void *pvParameters) {
    /* Task periodicity of 1 sec */
    const TickType_t xPeriodicity = pdMS_TO_TICKS(3000);
    /* Initialize xLastWakeTime to the current tick count */
    TickType_t xLastWakeTime= xTaskGetTickCount();
    for(;;) {
        if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {

            /* prints for Driver*/
            UART0_SendString("Driver: ");
            UART0_SendString("\r\n");
            UART0_SendString(" Seat Desired Heat Level: ");
            switch (enDriverSeatDesiredTemp)
                    {
                    case 0:
                        UART0_SendString("SEAT_HEATING_OFF");
                        break;
                    case 25:
                        UART0_SendString("LOW_SEAT_HEAT_TEMP");
                        break;
                    case 30:
                        UART0_SendString("MED_SEAT_HEAT_TEMP");
                        break;
                    case 35:
                        UART0_SendString("HIGH_SEAT_HEAT_TEMP");
                        break;
                    default:
                        break;
                    }
            UART0_SendString("\r\n");
            UART0_SendString(" Seat Heater Intensity: ");
            switch (enDriverHeatingIntenisty)
                    {
                    case 0:
                        UART0_SendString("HEATER_OFF");
                        break;
                    case 1:
                        UART0_SendString("HEATER_LOW");
                        break;
                    case 2:
                        UART0_SendString("HEATER_MED");
                        break;
                    case 3:
                        UART0_SendString("HEATER_HIGH");
                        break;
                    default:
                        break;
                    }
            UART0_SendString("\r\n");
            UART0_SendString("Seat Current Temperature:");
            UART0_SendInteger( (uint32)u32DriverSeat_CurrTemp);
            UART0_SendString("\r\n");

            /*Separator */
            UART0_SendString("///////////////////////////////////");
            UART0_SendString("\r\n");

            /* prints for Driver*/
            UART0_SendString("Passenger: ");
            UART0_SendString("\r\n");
            UART0_SendString(" Seat Desired Heat Level: ");
            switch (enPassengerSeatDesiredTemp)
                            {
                            case 0:
                                UART0_SendString("SEAT_HEATING_OFF");
                                break;
                            case 25:
                                UART0_SendString("LOW_SEAT_HEAT_TEMP");
                                break;
                            case 30:
                                UART0_SendString("MED_SEAT_HEAT_TEMP");
                                break;
                            case 35:
                                UART0_SendString("HIGH_SEAT_HEAT_TEMP");
                                break;
                            default:
                                break;
                            }
            UART0_SendString("\r\n");
            UART0_SendString(" Seat Heater Intensity: ");
                    switch (enPassengerHeatingIntenisty)
                            {
                            case 0:
                                UART0_SendString("HEATER_OFF");
                                break;
                            case 1:
                                UART0_SendString("HEATER_LOW");
                                break;
                            case 2:
                                UART0_SendString("HEATER_MED");
                                break;
                            case 3:
                                UART0_SendString("HEATER_HIGH");
                                break;
                            default:
                                break;
                            }
                    UART0_SendString("\r\n");
                    UART0_SendString("Seat Current Temperature:");
                     UART0_SendInteger( (uint32)u32PassengerSeat_CurrTemp);
                     UART0_SendString("\r\n");


            /*Separator */
                    UART0_SendString("///////////////////////////////////");
                    UART0_SendString("\r\n");
        }
        xSemaphoreGive(xUARTMutex);


        vTaskDelayUntil(&xLastWakeTime, xPeriodicity);

    }
}

void vcpuLoadMeasurementTask(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {

            uint8_t ucCounter, ucCPU_Load;
            uint32_t ullTotalTasksTime = 0;

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(RUNTIME_MEASUREMENTS_TASK_PERIODICITY ) );
            for(ucCounter = 1; ucCounter < 9; ucCounter++)
            {
                ullTotalTasksTime += ullTasksTotalTime[ucCounter];
            }
            ucCPU_Load = (ullTotalTasksTime * 100) /  GPTM_WTimer0Read();

            if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {

                UART0_SendString(" CPU Load is ");
                UART0_SendInteger(ucCPU_Load);
                UART0_SendString("% \r\n");
            }
           xSemaphoreGive(xUARTMutex);



    }
}


void vFailureLoggingTask(void *pvParameters) {
    /* Task periodicity of 1000 mseconds */
    const TickType_t xPeriodicity = pdMS_TO_TICKS(1500);
    /* Initialize xLastWakeTime to the current tick count */
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint8 i ;
    for(;;) {
       if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {

            /* Check for failures and log them */
            if (failureLogIndex > 0) {
                UART0_SendString("Failure Log: ");
                UART0_SendString("\r\n");
                for (i=0; (i <= failureLogIndex) && (i<5); i++) {
                    UART0_SendString("Failure: ");
                    UART0_SendString(FailureLogArr[i].failureMsg);
                    UART0_SendString("\r\n");
                    UART0_SendString("Timestamp: ");
                    UART0_SendInteger(FailureLogArr[i].timeStamp);
                    UART0_SendString("\r\n");
                    UART0_SendString("Heat Level: ");
                    UART0_SendInteger(FailureLogArr[i].heatLevel);
                    UART0_SendString("\r\n");
                }
                failureLogIndex = 0; // Reset the index after logging
            }
       }
        xSemaphoreGive(xUARTMutex);


        vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
    }
}

