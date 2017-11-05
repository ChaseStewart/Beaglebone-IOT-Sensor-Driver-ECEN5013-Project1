#include "common.h"


/* request other thread to send heartbeat */
int8_t sendHeartbeat(mqd_t queue, Task_Id my_id)
{
	int retval;
	message_t *msg;
	mqd_t my_queue;

	/* craft heartbeat request */
	msg = (message_t *) malloc(sizeof(message_t));
	msg->id = HEARTBEAT_RSP;
	msg->timestamp = time(NULL);
	msg->length = 0;
	msg->source = my_id; 
	msg->message = NULL;

	/* Attempt to send to the queue	*/
	retval = mq_send(queue, (const char *) msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send from %d  with retval %d\n", my_id, retval);
		return 1;
	}
	printf("send from %d  with retval %d\n", my_id, retval);
	return 0;
}


int8_t blockAllSigs(void)
{
	int retval;
	sigset_t set;
	
	/* catch heartbeat signal */
	sigemptyset(&set);
	sigaddset(&set, HEARTBEAT_SIGNO);
	sigaddset(&set, SIGALRM);
	sigaddset(&set, LOGGER_SIGNO);
	sigaddset(&set, TEMP_DRIVER_SIGNO);
	sigaddset(&set, LIGHT_DRIVER_SIGNO);
	retval = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (retval != 0)
	{
		printf("Failed to set sigmask.\n");
		return -1;
	}
	return 0;
}
	
