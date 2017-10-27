#include <stdio.h>
#include "main.h"
#include "temp_driver.h"


void *mainTempDriver(void *arg)
{
	mqd_t main_queue;
	/* Create queue for main thread */
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	main_queue = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_RDONLY, 0755, NULL);
	if (main_queue == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return (char *)1;
	}
	printf("Initializing Temp Driver\n");
	initTempDriver(&main_queue);
	printf("Destroyed Temp Driver\n");

	return NULL;
}

/* Function to configure the temp sensor */
int8_t initTempDriver(mqd_t *queue)
{
	printf("Setup Temp Driver\n");
	sendHeartbeatTemp(queue);
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


