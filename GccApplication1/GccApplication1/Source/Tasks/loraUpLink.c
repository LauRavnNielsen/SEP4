#include <stddef.h>
#include <stdio.h>

#include <ATMEGA_FreeRTOS.h>

#include <lora_driver.h>
#include "../Headers/loraUpLinkHandler.h"
#include <iled.h>
#include "../Headers/config.h"
#include <stdlib.h>


static char _out_buf[100];
static lora_payload_t _uplink_payload;

typedef struct LoraUpLink{
	EventGroupHandle_t loraReadyEventGroup;
	QueueHandle_t payloadQueue;
	
} LoraUpLink;

static void lora_sendPayload(LoraUpLink_t);

static void lora_emptyQueueOnStartUp(LoraUpLink_t);

LoraUpLink_t lora_createUpLink(EventGroupHandle_t loraReadyEventGroup, QueueHandle_t payloadQueue){
	LoraUpLink_t self = malloc(sizeof(LoraUpLink));
	
	self->loraReadyEventGroup = loraReadyEventGroup;
	self->payloadQueue = payloadQueue;
	
	return self;
}


static void _lora_setup(LoraUpLink_t self)
{
	e_LoRa_return_code_t rc;
	led_slow_blink(led_ST2); // OPTIONAL: Led the green led blink slowly while we are setting up LoRa

	// Factory reset the transceiver
	printf("FactoryReset >%s<\n", lora_driver_map_return_code_to_text(lora_driver_rn2483_factory_reset()));
	
	// Configure to EU868 LoRaWAN standards
	printf("Configure to EU868 >%s<\n", lora_driver_map_return_code_to_text(lora_driver_configure_to_eu868()));

	// Get the transceivers HW EUI
	rc = lora_driver_get_rn2483_hweui(_out_buf);
	printf("Get HWEUI >%s<: %s\n",lora_driver_map_return_code_to_text(rc), _out_buf);

	// Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
	printf("Set DevEUI: %s >%s<\n", _out_buf, lora_driver_map_return_code_to_text(lora_driver_set_device_identifier(_out_buf)));

	// Set Over The Air Activation parameters to be ready to join the LoRaWAN
	printf("Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<\n", LORA_appEUI, LORA_appKEY, _out_buf, lora_driver_map_return_code_to_text(lora_driver_set_otaa_identity(LORA_appEUI,LORA_appKEY,_out_buf)));

	// Save all the MAC settings in the transceiver
	printf("Save mac >%s<\n",lora_driver_map_return_code_to_text(lora_driver_save_mac()));

	// Enable Adaptive Data Rate
	printf("Set Adaptive Data Rate: ON >%s<\n", lora_driver_map_return_code_to_text(lora_driver_set_adaptive_data_rate(LoRa_ON)));

	// Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
	printf("Set Receiver Delay: %d ms >%s<\n", 500, lora_driver_map_return_code_to_text(lora_driver_set_receive_delay(500)));

	// Join the LoRaWAN
	uint8_t maxJoinTriesLeft = 10;
	
	do {
		rc = lora_driver_join(LoRa_OTAA);
		printf("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_map_return_code_to_text(rc));

		if ( rc != LoRa_ACCEPTED)
		{
			// Make the red led pulse to tell something went wrong
			led_long_puls(led_ST1); // OPTIONAL
			// Wait 5 sec and lets try again
			vTaskDelay(oneSecond*5);
		}
		else
		{
			break;
		}
	} while (--maxJoinTriesLeft);

	if (rc == LoRa_ACCEPTED)
	{
		lora_emptyQueueOnStartUp(self);
		
		// Connected to LoRaWAN :-)
		// Make the green led steady
		led_led_on(led_ST2); // OPTIONAL
		//xEventGroupSetBits(self->loraReadyEventGroup, LORA_CONNECTED_BIT);
	}
	else
	{
		// Something went wrong
		// Turn off the green led
		led_led_off(led_ST2); // OPTIONAL
		// Make the red led blink fast to tell something went wrong
		led_fast_blink(led_ST1); // OPTIONAL
		
		// Lets stay here
		while (1)
		{
			taskYIELD();
		}
	}
	
	
}

static void lora_startUp(LoraUpLink_t self){
	// Hardware reset of LoRaWAN transceiver
	lora_driver_reset_rn2483(1);
	vTaskDelay(oneSecond);
	lora_driver_reset_rn2483(0);
	// Give it a chance to wakeup
	vTaskDelay(oneSecond);

	lora_driver_flush_buffers(); // get rid of first version string from module after reset!

	_lora_setup(self);
}

static void lora_sendPayload(LoraUpLink_t self){
	static e_LoRa_return_code_t rc;
	
	if (self->payloadQueue != NULL){
		if (xQueuePeek(self->payloadQueue, &_uplink_payload, portMAX_DELAY));{
			rc = lora_driver_sent_upload_message(false, &_uplink_payload);
			
			printf("Upload Message >%s<\n", lora_driver_map_return_code_to_text(rc));
			
			led_short_puls(led_ST4);  // OPTIONAL
			if (rc == LoRa_NO_FREE_CH){
				xEventGroupClearBits(self->loraReadyEventGroup, LORA_CONNECTED_BIT);
				lora_startUp(self);
			}
			else if (rc == LoRa_MAC_TX_OK){
				xQueueReceive(self->payloadQueue, &_uplink_payload, portMAX_DELAY);
			}	
		}
	}
	
}

static void lora_emptyQueueOnStartUp(LoraUpLink_t self){
	while (uxQueueMessagesWaiting(self->payloadQueue) != 0){
		lora_sendPayload(self);
	}
	puts("payloadQueue is empty");
	xEventGroupSetBits(self->loraReadyEventGroup, LORA_CONNECTED_BIT);
}

/*-----------------------------------------------------------*/
void _lora_uplink_task( void *pvParameters )
{
	LoraUpLink_t self = (LoraUpLink_t)pvParameters;
	
	lora_startUp(self);
	
	for(;;)
	{
		lora_sendPayload(self);
		
	}
}
