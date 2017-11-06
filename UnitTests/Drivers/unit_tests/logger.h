#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <mqueue.h>
#include <stdint.h>
#include <stdlib.h>
#include "common.h"

#ifndef __MY_LOGGER_H__
#define __MY_LOGGER_H__

/* logger args */
#define LOGGER_QUEUE_NAME "/logger\x00"
#define LOGGER_MSG_SIZE   100
#define MAIN_NUM_MSGS  	  1000

/* main program for logger */
void *mainLogger(void *);

/* set up the logger to run*/
int8_t initLoggerQueues(mqd_t *main_queue, mqd_t *logger_queue);

/* Set logger to notify and open file*/
FILE *initLogger(mqd_t queue, void *arg);

/*Function to print the logger structure onto the log file*/
int8_t logMessage(message_t *in_message);

/* 
 * put a message into the logfile from the logger
 * NOTICE queue is unused + only for compatibility here
 */
int8_t logFromLogger(mqd_t queue, int prio, char *message);

/* change the logfile */
FILE *logFileChange(message_t *newName);

#endif
