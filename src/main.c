#include <mqueue.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

int main(int argc, char **argv)
{
	int state, ret;
	pthread_t temp_thread, light_thread, logger_thread;
	/* do argc argv */

	/* create initial queue */
	message_t msg;
	msg.id = 0;
	msg.timestamp = time(NULL);
	msg.length = 0;
	msg.message = NULL;

	mqd_t main_queue; 
	//struct mq_attr main_attr;
	//main_attr.mq_msgsize  = MAIN_NUM_MSGS;
	//main_attr.mq_maxmsg   = MAIN_MSG_SIZE;

	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);

	main_queue = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_RDWR, 0755, NULL );
	if (main_queue == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}

	ret = mq_send(main_queue, (const char *) &msg, sizeof(message_t), 0);
	if (ret == -1)
	{
		printf("Failed to send to queue! Exiting...\n");
		return 1;
	}

	state = STATE_RUNNING;

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

	printf("Initialized children\n");

	printf("Now moving to main loop\n");
	while (state == STATE_ERROR)
	{ // TODO request heartbeats and process
	}

	ret = mq_close(main_queue);
	if (ret == -1)
	{
		printf("Failed to reap queue\n");
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
int8_t reqHeartbeat(int8_t id)
{
	/* craft heartbeat request */


	return 0;
}

/* send heartbeat to the heartbeat queue */
int8_t sendHeartbeat(int8_t id)
{
	printf("Would have sent heartbeat from %d\n", id);
	return 0;
}

