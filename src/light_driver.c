#include <stdio.h>
#include "light_driver.h"
#include "main.h"

void * mainLightDriver(void *arg)
{
	mqd_t main_queue;
	
	/* Create queue for main thread */
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	main_queue = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_RDONLY, 0755, NULL);
	if (main_queue == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return (char*)1;
	}

	printf("Initializing LightDriver\n");
	initLightDriver(&main_queue);
	printf("Destroyed LightDriver\n");
	return NULL;
}

/* Function to configure the light sensor */
int8_t initLightDriver(mqd_t *main_queue)
{



	printf("Setup LightDriver\n");
	sendHeartbeatLight(main_queue);
	return 0;
}

/* Function to read from the light register */
int8_t readLightRegisters(uint8_t regAddr, uint8_t *retval)
{
	return 0;
}

/* Function to read N-bytes from the light register, put result in retval, return status */
int8_t readNLightRegisters(uint8_t regAddr, uint8_t *retval, uint8_t length)
{
	return 0;
}

/* Function to write into the Light Sensor Registers */
int8_t writeLightRegisters(uint8_t regAddr, uint8_t data)
{
	return 0;
}


