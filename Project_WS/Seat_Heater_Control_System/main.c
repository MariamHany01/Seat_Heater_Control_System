/******************************************************************************
 *
 * File Name: main.c
 *
 * Description: Main source file
 *
 *******************************************************************************/
/*MCA: includes*/
#include "MCAL/ADC/adc.h"
#include "MCAL/PORT/Port.h"
#include "MCAL/DIO/Dio.h"
#include "MCAL/GPIO/gpio.h"
#include"GPTM.h"
#include"uart0.h"
#include"tm4c123gh6pm_registers.h"

/*HAL includes*/
//#include"../HAL/Temp_sensorLM35/temp_sensorLM35.h"
#include"../HAL/Heater/heater.h"
#include "../HAL/Buttons/buttons.h"
#include "../HAL/LEDS/leds.h"

/* Kernel includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"

/* application includes */
#include"app.h"

/* The HW setup function */
static void prvSetupHardware( void );


/* Task handles */
extern TaskHandle_t BtnPressMangeTasDriverkHandle , BtnPressMangeTasPassengerkHandle;
extern TaskHandle_t TemperatureMonitorTaskHandle;
extern TaskHandle_t HeaterControlTaskDriverHandle, HeaterControlTaskPassengerHandle;
extern TaskHandle_t DisplayUpdateTaskHandle;
extern TaskHandle_t CpuLoadMeasurementTaskHandle;
extern TaskHandle_t FailureLoggingTaskHandle;

/* Event group handle */
extern EventGroupHandle_t xEventGroup;

extern xSemaphoreHandle xDriverButtonSemaphore;
extern xSemaphoreHandle xPassengerButtonSemaphore;

extern xSemaphoreHandle xUARTMutex;

extern APP_UserState DriverState;
extern APP_UserState PassengerState;


/* Define a constant for delay iterations */
#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 369          /* Define macro for delay iterations */

/* Function to create a delay in milliseconds */
void Delay_MS(unsigned long long n)                           /* Function to create delay */
{
    volatile unsigned long long count = 0;                    /* Volatile to prevent optimization */
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) ); /* Delay loop */
}

int main()
{

    /* Setup the hardware for use with the Tiva C board. */
    prvSetupHardware();

    /*synch tasks initialization */
    xEventGroup = xEventGroupCreate();

    xDriverButtonSemaphore =  xSemaphoreCreateCounting(3, 0);
    xPassengerButtonSemaphore =xSemaphoreCreateCounting(3, 0);

    xUARTMutex = xSemaphoreCreateMutex();

    /* Configure GPIO interrupts for buttons */
    GPIO_SW1EdgeTriggeredInterruptInit();
    GPIO_SW2EdgeTriggeredInterruptInit();

    /* tasks creation  */
    xTaskCreate(vBtnPressMangeTask, "BtnPressMange_Driver", 150, (void*)&DriverState, 4, &BtnPressMangeTasDriverkHandle);
    xTaskCreate(vBtnPressMangeTask, "BtnPressMange_Passenger", 150, (void*)&PassengerState, 4, &BtnPressMangeTasPassengerkHandle);
    xTaskCreate(vTemperatureMonitorTask, "TemperatureMonitor", 150, NULL, 2, &TemperatureMonitorTaskHandle);
    xTaskCreate(vHeaterControlTask, "HeaterControl_Driver", 150, (void*)&DriverState, 3, &HeaterControlTaskDriverHandle);
    xTaskCreate(vHeaterControlTask, "HeaterControl_Passenger", 150, (void*)&PassengerState, 3, &HeaterControlTaskPassengerHandle);
    xTaskCreate(vDisplayUpdateTask, "DisplayUpdate", 150, NULL, 1, &DisplayUpdateTaskHandle);
    xTaskCreate(vcpuLoadMeasurementTask, "cpuLoadMeasurement", 150, NULL, 1, &CpuLoadMeasurementTaskHandle);
    xTaskCreate(vFailureLoggingTask, "FailureLogging", 150, NULL, 2, &FailureLoggingTaskHandle);

    /*Set task tags for runtime measurments  */
    vTaskSetApplicationTaskTag(CpuLoadMeasurementTaskHandle, (TaskHookFunction_t)1);
    vTaskSetApplicationTaskTag(DisplayUpdateTaskHandle, (TaskHookFunction_t)2);
    vTaskSetApplicationTaskTag(HeaterControlTaskDriverHandle, (TaskHookFunction_t)3);
    vTaskSetApplicationTaskTag(HeaterControlTaskPassengerHandle, (TaskHookFunction_t)4);
    vTaskSetApplicationTaskTag(TemperatureMonitorTaskHandle, (TaskHookFunction_t)5);
    vTaskSetApplicationTaskTag(BtnPressMangeTasDriverkHandle, (TaskHookFunction_t)6);
    vTaskSetApplicationTaskTag(BtnPressMangeTasPassengerkHandle, (TaskHookFunction_t)7);
//    vTaskSetApplicationTaskTag(FailureLoggingTaskHandle, (TaskHookFunction_t)8);


	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for (;;);

}


static void prvSetupHardware( void )
{
	/* Place here any needed HW initialization such as GPIO, UART, etc.  */

    /* Enable clock for All PORTs and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x3F;
    while(!(SYSCTL_PRGPIO_REG & 0x3F));


    Port_Init(port_config);             /* Initialize Port Driver module */
    Dio_Init(&Dio_Configuration);       /* Initialize Dio Driver module */
    ADC0_Init();                        /* Initialize ADC0 */
    ADC1_Init();                        /* Initialize ADC1 */
    UART0_Init();                       /* Initialize UART0 */
    GPTM_WTimer0Init();                 /* Initialize General Purpose Timer Module WTimer0 */
}
/*ISR implementation  */
void GPIOPortF_Handler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // Initialize to pdFALSE

    if (GPIO_PORTF_RIS_REG & (1<<0)) {
        xSemaphoreGiveFromISR(xDriverButtonSemaphore, &xHigherPriorityTaskWoken);
        GPIO_PORTF_ICR_REG   |= (1<<0);//clear the flag

    }
    /*SW!*/
    if (GPIO_PORTF_RIS_REG & (1<<4)) {
        xSemaphoreGiveFromISR(xPassengerButtonSemaphore, &xHigherPriorityTaskWoken);
        GPIO_PORTF_ICR_REG   |= (1<<4);//clear the flag

    }
}
