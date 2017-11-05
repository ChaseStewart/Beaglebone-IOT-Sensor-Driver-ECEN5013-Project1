#include <getopt.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
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

/* general vars */
#define HEARTBEAT_PERIOD 3


/* command-line options */
static struct option options[] = {
	{"filename", required_argument, NULL, 'f'},
	{"help", no_argument, NULL, 'h'},
	{NULL, 0, 0, 0}
};

/* print a help message*/
void my_print_help(void);

/* create the main queues */
int8_t initMainQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *light_queue, mqd_t *temp_queue);

/* main program for the project */
int main(int argc, char **argv);

/* ensure all heartbeats are received*/
int8_t processHeartbeats(mqd_t main_queue, mqd_t logger_queue);

/* request other threads to send a heartbeat to the heartbeat queue */
int8_t reqHeartbeats(mqd_t logger_queue, mqd_t temp_queue, mqd_t light_queue);

/* handle the alarm to kick off a round of processHeartbeats */
void heartbeatAlarm(int sig);

/* catch various signals */
void handleCtrlC(int sig);

/* send message to logger with priority prio */
int8_t logFromMain(mqd_t queue, int prio, char *message);

#endif
