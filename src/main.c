#include "common.h"
#include "main.h"

volatile int hbt_rsp[NUM_TASKS];
volatile sig_atomic_t alrm_flag = 0;

volatile int main_state   = STATE_RUNNING;
volatile int logger_state = STATE_RUNNING;
volatile int temp_state   = STATE_RUNNING;
volatile int light_state  = STATE_RUNNING;

/* init condition vars */
pthread_cond_t  heartbeat_cv;
pthread_cond_t  logger_cv;
pthread_cond_t  temp_cv;
pthread_cond_t  light_cv;
pthread_mutex_t heartbeat_mutex;
pthread_mutex_t logger_mutex;
pthread_mutex_t temp_mutex;
pthread_mutex_t light_mutex;

void my_print_help(void)
{
	printf("Usage: project1 [-f] filename [-h]\n");
}

void heartbeatAlarm(int sig)
{
	signal(SIGALRM, heartbeatAlarm);
	alarm(1);
	alrm_flag = 1;
}

void handleCtrlC(int sig)
{
	logger_state = STATE_SHUTDOWN;
	pthread_cond_signal(&logger_cv);

	temp_state   = STATE_SHUTDOWN;
	pthread_cond_signal(&temp_cv);

	light_state  = STATE_SHUTDOWN;
	pthread_cond_signal(&light_cv);

	main_state   = STATE_SHUTDOWN;
}

void handleSigLogger(int sig)
{
	signal(LOGGER_SIGNO, handleSigLogger);
	pthread_cond_signal(&logger_cv);
}

void handleSigHbt(int sig)
{
	signal(HEARTBEAT_SIGNO, handleSigHbt);
	pthread_cond_signal(&heartbeat_cv);
}

void handleSigLight(int sig)
{
	signal(LIGHT_DRIVER_SIGNO, handleSigLight);
	pthread_cond_signal(&light_cv);
}

void handleSigTemp(int sig)
{
	signal(TEMP_DRIVER_SIGNO, handleSigTemp);
	pthread_cond_signal(&temp_cv);
}


int main(int argc, char **argv)
{
	int retval, counter, curr_arg;
	char out_file_name[MAX_FILELEN];
	logger_args *my_log_args;

	pthread_t temp_thread, light_thread, logger_thread;
	message_t msg;
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
	signal(LOGGER_SIGNO, handleSigLogger);
	signal(TEMP_DRIVER_SIGNO, handleSigTemp);
	signal(LIGHT_DRIVER_SIGNO, handleSigLight);
	signal(HEARTBEAT_SIGNO, handleSigHbt);

	/* instantiate SIGUSR cond/mutex */
	pthread_cond_init(&heartbeat_cv, NULL);
	pthread_cond_init(&logger_cv, NULL);
	pthread_cond_init(&temp_cv, NULL);
	pthread_cond_init(&light_cv, NULL);

	pthread_mutex_init(&heartbeat_mutex, NULL);
	pthread_mutex_init(&logger_mutex, NULL);
	pthread_mutex_init(&temp_mutex, NULL);
	pthread_mutex_init(&light_mutex, NULL);


	/* do argc argv */
	curr_arg = 0;
	while(curr_arg >= 0)
	{
		curr_arg = getopt_long(argc, argv, "f:h", options, NULL);
		if (curr_arg < 0){ continue;}
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
		printf("Failed to initialize Main queues!\n");
	}

	
	/* create the logger thread */
	my_log_args = (logger_args *)malloc(sizeof(logger_args));
	my_log_args->filename = out_file_name;
	my_log_args->length = strlen(out_file_name);
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
	
	/* Test message send to main thread */
	msg.id = 0;
	msg.timestamp = time(NULL);
	msg.length = 0;
	msg.source = MAIN_ID;
	msg.message = NULL;
	retval = mq_send(logger_queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send to queue! Exiting...\n");
		//return 1;
	}

	/* prepare main loop */
	printf("Initialized children, Now moving to main loop\n");
	counter = 0;
	for (int idx = 0; idx< NUM_TASKS; idx++)
	{
		hbt_rsp[idx] = 0;
	}
		
	signal(SIGALRM, heartbeatAlarm);
	alarm(1);

	/* main loop */
	while (main_state == STATE_RUNNING)
	{
		if (alrm_flag == 1)
		{
			alrm_flag = 0;
			printf("Received alarm %d!\n", counter);
			counter++;
		}
		sleep(2);
	}
	printf("exiting program\n");
	
	/* on close, reap logger_thread */
	if (pthread_join(logger_thread, NULL) != 0)
	{
		printf("failed to reap logger_thread\n");
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
	
	printf("All Threads reaped!\n");

	/* close this instance of the queue */
	retval = mq_close(main_queue);
	if (retval == -1)
	{
		printf("Failed to reap queue\n");
		return 1;
	}
	/* close this instance of the queue */
	retval = mq_close(logger_queue);
	if (retval == -1)
	{
		printf("Failed to reap queue\n");
		return 1;
	}
	/* close this instance of the queue */
	retval = mq_close(temp_queue);
	if (retval == -1)
	{
		printf("Failed to reap queue\n");
		return 1;
	}
	/* close this instance of the queue */
	retval = mq_close(light_queue);
	if (retval == -1)
	{
		printf("Failed to reap queue\n");
		return 1;
	}
	printf("All main queues closed!\n");

	/* now unlink the queue for all */
	retval = mq_unlink(MAIN_QUEUE_NAME);
	if (retval == -1)
	{
		printf("Failed to unlink queue\n");
		return 1;
	}

	/* now unlink the queue for all */
	retval = mq_unlink(LOGGER_QUEUE_NAME);
	if (retval == -1)
	{
		printf("Failed to unlink queue\n");
		return 1;
	}
	
	/* now unlink the queue for all */
	retval = mq_unlink(TEMP_DRIVER_QUEUE_NAME);
	if (retval == -1)
	{
		printf("Failed to unlink queue\n");
		return 1;
	}

	/* now unlink the queue for all */
	retval = mq_unlink(LIGHT_DRIVER_QUEUE_NAME);
	if (retval == -1)
	{
		printf("Failed to unlink queue\n");
		return 1;
	}
	
	/* instantiate SIGUSR cond/mutex */
	pthread_cond_destroy(&heartbeat_cv);
	pthread_cond_destroy(&logger_cv);
	pthread_cond_destroy(&temp_cv);
	pthread_cond_destroy(&light_cv);

	pthread_mutex_destroy(&heartbeat_mutex);
	pthread_mutex_destroy(&logger_mutex);
	pthread_mutex_destroy(&temp_mutex);
	pthread_mutex_destroy(&light_mutex);

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
			main_state = STATE_SHUTDOWN;
		}
		hbt_rsp[idx] = 0;
	}
	if (main_state == STATE_SHUTDOWN)
	{
		return 1;
	}
	return 0;
}


int8_t initMainQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *light_queue, mqd_t *temp_queue)
{
	/* Create queue for main thread */
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	(*main_queue) = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_RDONLY, 0755, NULL);
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
	msg.source = MAIN_ID; 
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
int8_t sendHeartbeat(mqd_t queue, Task_Id id)
{
	int retval;
	message_t msg;
	
	/* craft heartbeat request */
	msg.id = HEARTBEAT_RSP;
	msg.timestamp = time(NULL);
	msg.length    = 0;
	msg.source    = id;
	msg.message   = NULL;

	/* */
	retval = mq_send(queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval != 0)
	{
		printf("Failed to send with retval %d\n", retval);
		return 1;
	}
	return 0;
}

