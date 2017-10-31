#include <stdio.h>
#include <mqueue.h>
#include <stdint.h>
#include <stdlib.h>
#include "common.h"

#ifndef __MY_LOGGER_H__
#define __MY_LOGGER_H__

#define LOGGER_QUEUE_NAME "/logger\x00"
#define LOGGER_MSG_SIZE   100
#define MAIN_NUM_MSGS  	  1000

/* main program for logger */
void *mainLogger(void *);

/* set up the logger to run*/
int8_t initLoggerQueues(mqd_t *main_queue, mqd_t *logger_queue);

/*Function to print the logger structure onto the log file*/
void logStruct(message_t logs);

/*Function to log integers by converting them to ascii values*/
void logInt(int32_t data);

/*Function to log strings*/
void logString(uint8_t* string, size_t length);

/*Send heartbeat to main*/
#define sendHeartbeatLogger() sendHeartbeat(LOGGER_ID)

#endif
