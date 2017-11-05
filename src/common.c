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
	printf(" Attempt for %d \n", my_id);
	retval = mq_send(queue, (const char *) msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send from %d  with retval %d\n", my_id, retval);
		return 1;
	}
	printf("Send succeeded for %d  with retval %d\n", my_id, retval);
	return 0;
}
