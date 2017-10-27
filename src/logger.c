#include <stdio.h>
#include <mqueue.h>
#include "logger.h"
#include "main.h"

void * mainLogger(void *arg)
{

	mqd_t logger_queue;

	logger_queue = mq_open(LOGGER_QUEUE_NAME, O_CREAT | O_RDONLY, 0755, NULL );
	if (logger_queue == (mqd_t) -1 )
	{
		printf("Failed to initialize logger queue! Exiting...\n");
		return NULL;
	}

	int8_t retval;

	printf("Created Logger\n");
	retval = initLogger();
	if (retval != 0)
	{
		printf("Failed to initialize logger\n");
		return NULL;
	}

	printf("Destroyed Logger\n");
	return NULL;
}


int8_t initLogger()
{
	return 0;
}

void logInt(int32_t data)
{
	printf("[logger] received %u\n", data);
}

void logString(uint8_t *string, size_t length)
{
	/* malloc buffer, sprintf string into buffer, print it */
	printf("[logger] received %s\n", string);

}

