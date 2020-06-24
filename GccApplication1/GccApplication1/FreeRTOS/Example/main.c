/*
* FreeRTOS_ATMEGA.c
*
* Created: 15/10/2018 13:08:53
* Author : IHA
*/

#include <avr/io.h>
#include <avr/sfr_defs.h>

#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <mh_z19.h>

#include "../FreeRTOSTraceDriver/FreeRTOSTraceDriver.h"

// define two Tasks
void task1( void *pvParameters );
void task2( void *pvParameters );
void _co2_task(void *pvParameters);

// define semaphore handle
SemaphoreHandle_t sensorSemaphore;


/*-----------------------------------------------------------*/
void mh_z19_create(e_com_port_t com_port, NULL);

void create_tasks_and_semaphores(void)
{
	// Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
	// because it is sharing a resource, such as the Serial port.
	// Semaphores should only be used whilst the scheduler is running, but we can set it up here.
	if ( sensorSemaphore == NULL )  // Check to confirm that the Semaphore has not already been created.
	{
		sensorSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore.
		if ( ( sensorSemaphore ) != NULL )
		{
			xSemaphoreGive( ( sensorSemaphore ) );  // Make the mutex available for use, by initially "Giving" the Semaphore.
		}
	}

	xTaskCreate(
	task1
	,  (const portCHAR *)"Task1"  // A name just for humans
	,  configMINIMAL_STACK_SIZE  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  NULL
	,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );

	xTaskCreate(
	task2
	,  (const portCHAR *)"Task2"  // A name just for humans
	,  configMINIMAL_STACK_SIZE  // This stack size can be checked & adjusted by reading the Stack Highwater
	,  NULL
	,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
	,  NULL );
	
	xTaskCreate(
	_co2_task
	, "CO2 sensor"
	, configMINIMAL_STACK_SIZE
	, NULL
	, 3
	, NULL)

}

/*-----------------------------------------------------------*/
void task1( void *pvParameters )
{
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set task no to be used for tracing with R2R-Network
	vTaskSetApplicationTaskTag( NULL, ( void * ) 1 );
	#endif

	for(;;)
	{
		//xSemaphoreTake(xTestSemaphore,portMAX_DELAY);
		vTaskDelay(10);
		//xSemaphoreGive(xTestSemaphore);

		PORTA ^= _BV(PA0);
	}
}

/*-----------------------------------------------------------*/
void task2( void *pvParameters )
{
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set task no to be used for tracing with R2R-Network
	vTaskSetApplicationTaskTag( NULL, ( void * ) 2 );
	#endif

	for(;;)
	{
		vTaskDelay(50);
		PORTA ^= _BV(PA7);
	}
}
void _co2_task(void *pvParameters){
	printf("we fucked up");
	uint16_t lastCO2ppm;
	mh_z19_return_code_t return_code;
	mh_z19_create(ser_USART3, NULL);
	int count = 0;
	while(count < 5){
		return_code = mh_z19_get_co2_ppm(&lastCO2ppm);

		if(return_code == MHZ19_OK){
			puts("CO2 measured: %d" , lastCO2ppm);
		}
		else if (return_code == MHZ19_NO_MEASSURING_AVAILABLE){
			puts("no measurement found!");
			mh_z19_take_meassuring();
			puts("Performing a measurement!");
			mh_z19_get_co2_ppm(&lastCO2ppm);
			puts("CO2 measured: %d" , lastCO2ppm);
		}
		else if (return_code == MHZ19_NO_SERIAL){
			puts("we fucked up");
		}
		else if(return_code == MHZ19_PPM_MUST_BE_GT_999){
			puts("we fucked up some more");
		}
		count++;
		vTaskDelay(5000);
	}
}



/*-----------------------------------------------------------*/
int main(void)
{
	DDRA |= _BV(DDA0) | _BV(DDA7);
	trace_init();

	create_tasks_and_semaphores();
	vTaskStartScheduler(); // initialise and run the freeRTOS scheduler. Execution should never return here.

	/* Replace with your application code */
	while (1)
	{
	}
}

