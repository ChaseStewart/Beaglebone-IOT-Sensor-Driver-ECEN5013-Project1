#include <stdio.h>
#include "temp_driver.h"
#include "common.h"


void *mainTempDriver(void *arg)
{
	mqd_t main_queue, logger_queue, temp_queue;
	/* Create queue for main thread */
	printf("Initializing Temp Queues\n");
	initTempQueues(&main_queue, &logger_queue, &temp_queue);
	
	printf("Initializing Temp Driver\n");
	initTempQueues(&main_queue, &logger_queue, &temp_queue);

	while(temp_state ==STATE_RUNNING)
	{
		pthread_cond_wait(&temp_cv, &temp_mutex);
		printf("Temp Driver Awake!\n");
	}

	printf("Destroyed Temp Driver\n");

	return NULL;
}

/* Function to configure the temp sensor */
int8_t initTempQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *temp_queue)
{
	/* Create main queue*/
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	(*main_queue) = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*main_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	
	/* Create main queue*/
	printf("Creating queue \"%s\"\n", LOGGER_QUEUE_NAME);
	(*logger_queue) = mq_open(LOGGER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*logger_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	/* Create main queue*/
	printf("Creating queue \"%s\"\n", TEMP_DRIVER_QUEUE_NAME);
	(*temp_queue) = mq_open(TEMP_DRIVER_QUEUE_NAME, O_CREAT | O_RDONLY, 0755, NULL);
	if ((*temp_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	return 0;
}

int8_t initTempDriver()
{
	printf("Setup Temp Driver\n");
	return 0;
}

/* Function to read from the temp register */
int8_t readTempRegisters(uint8_t regAddr, uint8_t *retval)
{
	return 0;
}

/* Function to read N-bytes from the temp register, put result in retval, return status */
int8_t readNTempRegisters(uint8_t regAddr, uint8_t *retval, uint8_t length)
{
	return 0;
}

/* Function to write into the Temp Sensor Registers */
int8_t writeTempRegisters(uint8_t regAddr, uint8_t data)
{
	return 0;
}


