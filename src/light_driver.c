#include <stdio.h>
#include "light_driver.h"
#include "common.h"

void * mainLightDriver(void *arg)
{
	mqd_t main_queue, logger_queue, light_queue;

	printf("Initializing Queues\n");	
	initLightQueues(&main_queue, &logger_queue, &light_queue);
	printf("Initializing LightDriver\n");
	initLightDriver();
	printf("Setup LightDriver\n");
	
	printf("Destroyed LightDriver\n");
	return NULL;
}

/* Function to configure the light sensor */
int8_t initLightQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *light_queue)
{

	/* Create queue for main thread */
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	(*main_queue) = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*main_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	
	/* Create queue for logger thread */
	printf("Creating queue \"%s\"\n", LOGGER_QUEUE_NAME);
	(*logger_queue) = mq_open(LOGGER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*logger_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}

	/* Create queue for logger thread */
	printf("Creating queue \"%s\"\n", TEMP_DRIVER_QUEUE_NAME);
	(*light_queue) = mq_open(TEMP_DRIVER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*light_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}

	return 0;
}


int8_t initLightDriver(void)
{
	printf("Would have initialzed light driver!\n");
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


