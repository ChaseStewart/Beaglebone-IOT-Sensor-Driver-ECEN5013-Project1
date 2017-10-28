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

#include "common.h"
#include "main.h"

volatile int hbt_rsp[NUM_TASKS];
volatile sig_atomic_t alrm_flag = 0;
volatile int state;


void my_print_help(void)
{
	printf("Usage: project1 [-f] filename [-h]\n");
}

void heartbeatAlarm(int sig)
{
	alrm_flag = 1;
}

void handleCtrlC(int sig)
{
	state = STATE_SHUTDOWN;
}

int main(int argc, char **argv)
{
	int ret, counter, curr_arg;
	char out_file_name[MAX_FILELEN];
	logger_args *my_log_args;

	pthread_t temp_thread, light_thread, logger_thread;
	message_t msg;
	mqd_t main_queue; 
	

	if (argc == 1)
	{
		my_print_help();
		return 0;
	}
	
	/* catch Ctrl-C */
	signal(SIGINT, handleCtrlC);

	/* do argc argv */
	curr_arg = 0;
	while(curr_arg >= 0)
	{
		curr_arg = getopt_long(argc, argv, "f:h", options, NULL);
		if (curr_arg < 0){ continue;}
		switch( curr_arg)
		{
			case 'f':
				strcpy(out_file_name, optarg);
				break;
			case 'h':
			default:
				my_print_help();
				return 0;
		}
	}

	/* create initial queue */
	msg.id = 0;
	msg.timestamp = time(NULL);
	msg.length = 0;
	msg.message = NULL;

	
	/* Create queue for main thread */
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	main_queue = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if (main_queue == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}

	/* Test message send to main thread */
	ret = mq_send(main_queue, (const char *) &msg, sizeof(message_t), 0);
	if (ret == -1)
	{
		printf("Failed to send to queue! Exiting...\n");
		return 1;
	}

	my_log_args = (logger_args *)malloc(sizeof(logger_args));
	my_log_args->filename = out_file_name;
	my_log_args->length = strlen(out_file_name);
	
	/* create the logger thread */
	if(pthread_create(&logger_thread, NULL, mainLogger, my_log_args) != 0)
	{
		printf("Failed to create logger thread!\n");
		return 1;
	}
	
	/* create the temp thread */
	if(pthread_create(&temp_thread, NULL, mainTempDriver, NULL) != 0)
	{
		printf("Failed to create temperature thread!\n");
		return 1;
	}
	
	/* create the light thread */
	if(pthread_create(&light_thread, NULL, mainLightDriver, NULL) != 0)
	{
		printf("Failed to create light thread!\n");
		return 1;
	}

	/* start main loop */
	printf("Initialized children, Now moving to main loop\n");
	state = STATE_RUNNING;
	counter = 0;

	for (int idx = 0; idx< NUM_TASKS; idx++)
	{
		hbt_rsp[idx] = 0;
	}

	while (state == STATE_RUNNING)
	{ 
		if (alrm_flag == 1)
		{
			alrm_flag = 0;
			printf("Received alarm %d!\n", counter);
			counter++;
		}
		signal(SIGALRM, heartbeatAlarm);
		alarm(1);
		sleep(2);
	}
	printf("Exiting program\n");

	/* close this instance of the queue */
	ret = mq_close(main_queue);
	if (ret == -1)
	{
		printf("Failed to reap queue\n");
		return 1;
	}
	
	/* now unlink the queue for all */
	ret = mq_unlink(MAIN_QUEUE_NAME);
	if (ret == -1)
	{
		printf("Failed to unlink queue\n");
		return 1;
	}

	/* on close, reap temp_thread */
	if (pthread_join(temp_thread, NULL) != 0)
	{
		printf("failed to reap temp_thread\n");
		return 1;
	}

	/* on close, reap light_thread */
	if (pthread_join(light_thread, NULL) != 0)
	{
		printf("failed to reap light_thread\n");
		return 1;
	}
	
	/* on close, reap logger_thread */
	if (pthread_join(logger_thread, NULL) != 0)
	{
		printf("failed to reap logger_thread\n");
		return 1;
	}
	printf("All Threads reaped!\n");
	return 0;
}

/* ensure all heartbeats are received*/
int8_t processHeartbeats(void)
{
	for (int idx = 0; idx< NUM_TASKS; idx++)
	{
		if (hbt_rsp[idx] != 0)
		{
			/* TODO log error*/
			printf("Error with task %d", idx);
			state = STATE_SHUTDOWN;
		}
		hbt_rsp[idx] = 0;
	}
	if (state == STATE_SHUTDOWN)
	{
		return 1;
	}
	return 0;
}

/* request other thread to send heartbeat */
int8_t reqHeartbeat(mqd_t *queue)
{
	int retval;
	message_t msg;
	mqd_t my_queue;

	my_queue = (*queue);

	/* craft heartbeat request */
	msg.id = HEARTBEAT_REQ;
	msg.timestamp = time(NULL);
	msg.length = 0;
	msg.message = NULL;

	/* Attempt to send to the queue	*/
	retval = mq_send(my_queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send with retval %d\n", retval);
		return 1;
	}
	printf("Send succeeded with retval %d\n", retval);
	return 0;
}

/* send heartbeat to the heartbeat queue */
int8_t sendHeartbeat(mqd_t *queue, Task_Id id)
{
	int retval;
	message_t msg;
	
	uint8_t *heartbeat_message;
	heartbeat_message = (uint8_t *) malloc(sizeof(uint8_t *));
	*heartbeat_message = id;

	/* craft heartbeat request */
	msg.id = HEARTBEAT_RSP;
	msg.timestamp = time(NULL);
	msg.length = sizeof(uint8_t *);
	msg.message = heartbeat_message;

	retval = mq_send(*queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval != 0)
	{
		printf("Failed to send with retval %d\n", retval);
		return 1;
	}
	return 0;
}

