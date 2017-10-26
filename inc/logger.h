#include <stdint.h>
#include <stdlib.h>

#ifndef __MY_LOGGER_H__
#define __MY_LOGGER_H__

#define LOGGER_ID 1

/* main program for logger */
void *mainLogger(void *);

/* set up the logger to run*/
int8_t initLogger(void);

/*Function to print the logger structure onto the log file*/
// void logStruct(message_t logs);

/*Function to log integers by converting them to ascii values*/
void logInt(int32_t data);

/*Function to log strings*/
void logString(uint8_t* string, size_t length);

/*Send heartbeat to main*/
#define sendHeartbeatLogger() sendHeartbeat(LOGGER_ID)

#endif
