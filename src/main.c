#include <mqueue.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include "main.h"

volatile sig_atomic_t alrm_flag = 0;
volatile int state;

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
	int ret, counter;
	pthread_t temp_thread, light_thread, logger_thread;
	message_t msg;
	mqd_t main_queue; 
	
	/* do argc argv */

	/* create initial queue */
	msg.id = 0;
	msg.timestamp = time(NULL);
	msg.length = 0;
	msg.message = NULL;

	/* catch Ctrl-C */
	signal(SIGINT, handleCtrlC);
	
	/* Create queue for main thread */
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	main_queue = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_RDWR, 0755, NULL);
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

	/* create the logger thread */
	if(pthread_create(&logger_thread, NULL, mainLogger, NULL) != 0)
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
	return 0;
}

/* ensure all heartbeats are received*/
int8_t processHeartbeats(void)
{
	return 0;
}

/* request other thread to send heartbeat */
int8_t reqHeartbeat(mqd_t *queue)
{
	int retval;
	message_t msg;

	/* craft heartbeat request */
	msg.id = HEARTBEAT_REQ;
	msg.timestamp = time(NULL);
	msg.length = 0;
	msg.message = NULL;

	retval = mq_send(*queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval != 0)
	{
		printf("Failed to send with retval %d\n", retval);
		return 1;
	}
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

