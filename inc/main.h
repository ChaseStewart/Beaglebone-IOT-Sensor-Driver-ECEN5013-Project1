#include <getopt.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include "light_driver.h"
#include "temp_driver.h"
#include "logger.h"
#include "common.h"

#ifndef __MY_MAIN_H__
#define __MY_MAIN_H__


/* command-line options */
static struct option options[] = {
	{"filename", required_argument, NULL, 'f'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, 0, 0}
};

/* print a help message*/
void my_print_help(void);

int8_t initMainQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *light_queue, mqd_t *temp_queue);

/* main program for the project */
int main(int argc, char **argv);

/* ensure all heartbeats are received*/
int8_t processHeartbeats(void);

/* request other thread to send heartbeat */
int8_t reqHeartbeat(mqd_t *queue);

/* send heartbeat to the heartbeat queue */
int8_t sendHeartbeat(mqd_t queue, Task_Id id);

/* handle the alarm to kick off a round of processHeartbeats */
void heartbeatAlarm(int sig);


/* catch various signals */
void handleCtrlC(int sig);

void handleSigLogger(int sig);

void handleSigHbt(int sig);

void handleSigLight(int sig);

void handleSigTemp(int sig);


#endif
