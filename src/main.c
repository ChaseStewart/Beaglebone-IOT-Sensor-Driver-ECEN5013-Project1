#include "common.h"
#include "main.h"

volatile int hbt_rsp[NUM_TASKS];
volatile sig_atomic_t alrm_flag = 0;

volatile int logger_state = STATE_RUNNING;
volatile int main_state   = STATE_STARTUP;
volatile int temp_state   = STATE_RUNNING;
volatile int light_state  = STATE_RUNNING;

void my_print_help(void)
{
	printf("Usage: project1 [-f] filename [-h]\n");
}

/* set all threads to SHUTDOWN state one by one then reap all resources */
void handleCtrlC(int sig)
{
	main_state   = STATE_SHUTDOWN;
	raise(HEARTBEAT_SIGNO);
	
}


int main(int argc, char **argv)
{
	int retval, curr_arg, sig;
	char out_file_name[MAX_FILELEN];
	logger_args *my_log_args;
	message_t msg;
	sigset_t set;
	pthread_t temp_thread, light_thread, logger_thread;
	mqd_t main_queue, logger_queue, temp_queue, light_queue; 

	/* if no args provided, print help and exit */
	if (argc == 1)
	{
		my_print_help();
		return 0;
	}
	
	/* catch Necessary Signals */
	signal(SIGINT, handleCtrlC);
	signal(SIGTERM, handleCtrlC);
	
	retval = blockAllSigs();
	if (retval != 0)
	{
		printf("Failed to set sigmask.\n");
		return 1;
	}

	/* do argc argv */
	curr_arg = 0;
	while(curr_arg >= 0)
	{
		curr_arg = getopt_long(argc, argv, "f:h", options, NULL);
		if (curr_arg < 0)
		{
			continue;
		}
		switch( curr_arg)
		{
			/* get output filename from -f arg */
			case 'f':
				strcpy(out_file_name, optarg);
				break;

			/* print help for */
			case 'h':
			default:
				my_print_help();
				return 0;
		}
	}

	/* create initial queues */
	retval = initMainQueues(&main_queue, &logger_queue, &light_queue, &temp_queue);
	if (retval != 0)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to initialize Main queues!\n");
	}

	/* create the logger thread */
	my_log_args = (logger_args *)malloc(sizeof(logger_args));
	my_log_args->filename = out_file_name;
	my_log_args->length = strlen(out_file_name);
	if(pthread_create(&logger_thread, NULL, mainLogger, my_log_args) != 0)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to create logger thread!\n");
		return 1;
	}
	
	/* create the temp thread */
	if(pthread_create(&temp_thread, NULL, mainTempDriver, NULL) != 0)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to create temperature thread!\n");
		return 1;
	}
	
	/* create the light thread */
	if(pthread_create(&light_thread, NULL, mainLightDriver, NULL) != 0)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to create light thread!\n");
		return 1;
	}
	

	/* prepare main loop */
	for (int idx = 0; idx< NUM_TASKS; idx++)
	{
		hbt_rsp[idx] = 0;
	}

	/* begin recurring heartbeat timer */		
	alarm(HEARTBEAT_PERIOD);

	logFromMain(logger_queue, LOG_INFO, "Starting main loop\n");

	sigemptyset(&set);
	sigaddset(&set, HEARTBEAT_SIGNO);
	sigaddset(&set, SIGALRM);

	/* main loop- go until STATE_ERROR or STATE_SHUTDOWN */
	while (main_state > STATE_SHUTDOWN)
	{
		sigwait(&set, &sig);
		if (sig == HEARTBEAT_SIGNO)
		{
			continue;	
		}

		else if (sig == SIGALRM)
		{
			/* for the first time, only request a heartbeat*/
			if (main_state == STATE_STARTUP)
			{
				main_state = STATE_REQ_RSP;
				reqHeartbeats(logger_queue, temp_queue, light_queue);
			}

			/* for the second time, read current heartbeats then request more*/
			else if (main_state == STATE_REQ_RSP)
			{
				processHeartbeats(main_queue, logger_queue);
				reqHeartbeats(logger_queue, temp_queue, light_queue);
			}
		}
		else
		{
			logFromMain(logger_queue, LOG_ERROR, "Something bad happened!\n");
		}

		alarm(HEARTBEAT_PERIOD);
	}
	logFromMain(logger_queue, LOG_INFO, "exiting program\n");

	temp_state  = STATE_SHUTDOWN;
	pthread_kill(temp_thread, TEMP_DRIVER_SIGNO);
	
	light_state = STATE_SHUTDOWN;
	pthread_kill(light_thread, LIGHT_DRIVER_SIGNO);
	
	logger_state = STATE_SHUTDOWN;
	pthread_kill(logger_thread, LOGGER_SIGNO);
	
	/* close this instance of the queue */
	retval = mq_close(main_queue);
	if (retval == -1)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to reap queue\n");
		return 1;
	}
	
	/* close this instance of the queue */
	retval = mq_close(logger_queue);
	if (retval == -1)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to reap queue\n");
		return 1;
	}
	/* close this instance of the queue */
	retval = mq_close(temp_queue);
	if (retval == -1)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to reap queue\n");
		return 1;
	}
	/* close this instance of the queue */
	retval = mq_close(light_queue);
	if (retval == -1)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to reap queue\n");
		return 1;
	}
	printf("All main queues closed!\n");

	/* now unlink the queue for all */
	retval = mq_unlink(MAIN_QUEUE_NAME);
	if (retval == -1)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to unlink queue\n");
		return 1;
	}
	retval = mq_unlink(LOGGER_QUEUE_NAME);
	if (retval == -1)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to unlink queue\n");
		return 1;
	}
	retval = mq_unlink(TEMP_DRIVER_QUEUE_NAME);
	if (retval == -1)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to unlink queue\n");
		return 1;
	}
	retval = mq_unlink(LIGHT_DRIVER_QUEUE_NAME);
	if (retval == -1)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "Failed to unlink queue\n");
		return 1;
	}
	
	/* on close, reap temp_thread */
	if (pthread_join(temp_thread, NULL) != 0)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "failed to reap temp_thread\n");
		return 1;
	}
	/* on close, reap light_thread */
	if (pthread_join(light_thread, NULL) != 0)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "failed to reap light_thread\n");
		return 1;
	}
	/* on close, reap logger_thread */
	if (pthread_join(logger_thread, NULL) != 0)
	{
		logFromMain(logger_queue, LOG_CRITICAL, "failed to reap logger_thread\n");
		return 1;
	}

	printf("Goodbye!\n");
	return 0;
}

/* ensure all heartbeats are received */
int8_t processHeartbeats(mqd_t main_queue, mqd_t logger_queue)
{
	int hbt_source, retval;
	char in_buffer[4096];
	char heartbeat_msg[1024];
	message_t *in_message;
	
	in_message = (message_t *) malloc(sizeof(message_t));

	/* process all messages and set corresponding hbt_rsp entry */
	errno = 0;
	while(errno != EAGAIN){
		retval = mq_receive(main_queue, in_buffer, SIZE_MAX, NULL);
		if (retval <= 0 && errno != EAGAIN)
		{
			break;
		}

		/* process heartbeat_response messages */
		in_message = (message_t *)in_buffer;
		if (in_message->id == HEARTBEAT_RSP)
		{
			hbt_rsp[in_message->source] = 1;
		}
	}

	/* once all queue messages processed, ensure all heartbeats were there*/
	for (int idx = 0; idx< NUM_TASKS; idx++)
	{
		if (hbt_rsp[idx] == 0)
		{
			sprintf(heartbeat_msg, "Missing heartbeat response from %d\n", idx);
			logFromMain(logger_queue, LOG_ERROR, heartbeat_msg);
			main_state = STATE_ERROR;
		}
		hbt_rsp[idx] = 0;

	}
	if (main_state == STATE_ERROR)
	{
		printf("Missing heartbeats\n");
		raise(SIGINT);
		return 1;
	}
	else
	{
		sprintf(heartbeat_msg, "Missing heartbeat response\n");
		logFromMain(logger_queue, LOG_INFO, heartbeat_msg);
		return 0;
	}
}


int8_t initMainQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *light_queue, mqd_t *temp_queue)
{
	struct sigevent my_sigevent;

	/* Create queue for main thread */
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	(*main_queue) = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, 0755, NULL);
	if ((*main_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}

	/* Create queue for logger thread */
	printf("Creating queue \"%s\"\n", LOGGER_QUEUE_NAME);
	(*logger_queue) = mq_open(LOGGER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*logger_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	
	/* Create queue for light thread */
	printf("Creating queue \"%s\"\n", LIGHT_DRIVER_QUEUE_NAME);
	(*light_queue) = mq_open(LIGHT_DRIVER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*light_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	
	/* Create queue for main thread */
	printf("Creating queue \"%s\"\n", TEMP_DRIVER_QUEUE_NAME);
	(*temp_queue) = mq_open(TEMP_DRIVER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*temp_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	return 0;	
}


/* send heartbeat to the heartbeat queue */
int8_t reqHeartbeats(mqd_t logger_queue, mqd_t temp_queue, mqd_t light_queue)
{
	int idx, retval;
	message_t *msg;

	/* craft heartbeat request */
	msg = (message_t *) malloc(sizeof(message_t));
	msg->id        = HEARTBEAT_REQ;
	msg->timestamp = time(NULL);
	msg->length    = 0;
	msg->source    = MAIN_ID;
	msg->message   = NULL;

	/* send logger heartbeat */
	retval = mq_send(logger_queue, (const char *) msg, sizeof(message_t), 0);
	if (retval < 0)
	{
		printf("Failed to send to logger with retval %d and errno %d \n", retval, errno);
		return 1;
	}

	/* send temp heartbeat */
	retval = mq_send(temp_queue, (const char *) msg, sizeof(message_t), 0);
	if (retval < 0)
	{
		printf("Failed to send to temp with retval %d and errno %d \n", retval, errno);
		return 1;
	}
	
	/* send light heartbeat */
	retval = mq_send(light_queue, (const char *) msg, sizeof(message_t), 0);
	if (retval < 0)
	{
		printf("Failed to send to light  with retval %d and errno %d \n", retval, errno);
		return 1;
	}
	return 0;
}

int8_t logFromMain(mqd_t queue, int prio, char *message)
{
	int retval;
	message_t msg;

	msg.id = LOGGER;
	msg.timestamp = time(NULL);
	msg.length = strlen(message);
	msg.priority = prio;
	msg.source = MAIN_ID;
	msg.message = message;
	retval = mq_send(queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send from main to queue! Exiting...\n");
		return 1;
	}
}

