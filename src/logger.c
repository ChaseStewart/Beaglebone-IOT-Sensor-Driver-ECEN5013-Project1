#include "logger.h"
#include "common.h"

FILE *out_file;

void * mainLogger(void *arg)
{
	int8_t retval;
	char *logname;
	char in_buffer[4096];
	message_t *in_message;
	mqd_t main_queue, logger_queue;

	/* initialize the queue. */
	retval = initLoggerQueues(&main_queue, &logger_queue);
	if (retval != 0)
	{
		printf("Failed to init logger queues\n");
	}
	
	out_file = initLogger(logger_queue, arg);
	if(out_file)
	{
		logFromLogger(logger_queue, LOG_INFO, "Opened file!\n");
	}

	/* this is the main loop for the program */
	while(logger_state > STATE_SHUTDOWN)
	{
		pthread_cond_wait(&logger_cv, &logger_mutex);
		printf("logger waking up\n");
		in_message = (message_t *) malloc(sizeof(message_t));
		while(errno != EAGAIN){
			retval = mq_receive(logger_queue, in_buffer, SIZE_MAX, NULL);
			if (retval <= 0 && errno != EAGAIN)
			{
				continue;
			}
			in_message = (message_t *)in_buffer;

			/* process Log*/
			if (in_message->id == LOGGER)
			{
				logMessage(in_message);
			} 

			else if (in_message->id == HEARTBEAT_REQ) 
			{
				logFromLogger(logger_queue, LOG_INFO, "Got Heartbeat!\n");
				sendHeartbeat(main_queue, LOGGER_ID);
			}
		}
		retval = mq_receive(logger_queue, in_buffer, SIZE_MAX, NULL);
		if (retval <= 0 && errno != EAGAIN)
		{
			continue;
		}
		in_message = (message_t *)in_buffer;

		/* process Log*/
		if (in_message->id == LOGGER)
		{
			logMessage(in_message);
		} 

		else if (in_message->id == HEARTBEAT_REQ) 
		{
			sendHeartbeat(main_queue, LOGGER_ID);
		}
	}
	fclose(out_file);	
	printf("Destroyed Logger\n");
	return NULL;
}

int8_t initLoggerQueues(mqd_t *main_queue, mqd_t *logger_queue)
{
	/* Create main queue*/
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	(*main_queue) = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_WRONLY , 0755, NULL );
	if ((*main_queue) == (mqd_t) -1 )
	{
		return 1;
	}

	/* Create main queue*/
	printf("Creating queue \"%s\"\n", LOGGER_QUEUE_NAME);
	(*logger_queue) = mq_open(LOGGER_QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK , 0755, NULL );
	if ((*logger_queue) == (mqd_t) -1 )
	{
		return 1;
	}
	printf("Created Logger\n");
	return 0;
}

int8_t logMessage(message_t *in_message)
{
	int retval;
	char dbg_lvl[16];
	char mySource[16];
	char outMessage[1024];

	switch(in_message->priority)
	{
		case LOG_INFO:
			strcpy(dbg_lvl, "INFO");
			break;
		case LOG_DEBUG:
			strcpy(dbg_lvl, "DEBUG");
			break;
		case LOG_ERROR:
			strcpy(dbg_lvl, "ERROR");
			break;
		case LOG_CRITICAL:
			strcpy(dbg_lvl, "CRITICAL");
			break;
		default:
			return 1;
	}
	
	switch(in_message->source)
	{
		case TEMP_DRIVER_ID:
			strcpy(mySource, "TEMP_DRIVER");
			break;
		case LIGHT_DRIVER_ID:
			strcpy(mySource, "LIGHT_DRIVER");
			break;
		case LOGGER_ID:
			strcpy(mySource, "LOGGER");
			break;
		case MAIN_ID:
			strcpy(mySource, "MAIN");
			break;
		default:
			return 1;
	}

	retval = sprintf( outMessage ,"[%s][%s][%d] %s", dbg_lvl, mySource, in_message->timestamp, in_message->message);
	if (retval <= 0)
	{
		printf("Error logging output!");
	}
	fputs(outMessage, out_file);

	return 0;
}

FILE *initLogger(mqd_t queue, void *arg)
{
	char *logname;
	logger_args *my_log_args;	
	struct sigevent my_sigevent;
	
	/* register to receive logger signals */
	my_sigevent.sigev_notify = SIGEV_SIGNAL;
	my_sigevent.sigev_signo  = LOGGER_SIGNO;
	if (mq_notify(queue, &my_sigevent) == -1 )
	{
		printf("Failed to notify!\n");
		return NULL;
	}
	
	/* parse logger args */
	my_log_args = (logger_args *)malloc(sizeof(logger_args));
	my_log_args = (logger_args *) arg;
	logname = my_log_args->filename;
	return(fopen(logname, "a"));
}

int8_t logFromLogger(mqd_t queue, int prio, char *message)
{
	int retval;
	message_t msg;

	msg.id = LOGGER;
	msg.timestamp = time(NULL);
	msg.length = strlen(message);
	msg.priority = prio;
	msg.source = LOGGER_ID;
	msg.message = message;
	retval = mq_send(queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send to queue! Exiting...\n");
		return 1;
	}

}

