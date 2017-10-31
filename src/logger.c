#include "logger.h"
#include "common.h"

FILE *out_file;

void * mainLogger(void *arg)
{
	int8_t retval;
	char *logname;
	mqd_t main_queue, logger_queue;
	logger_args *my_log_args;	
	struct sigevent my_sigevent;

	/* initialize the queue. */
	retval = initLoggerQueues(&main_queue, &logger_queue);


	if (retval != 0)
	{
		printf("Failed to init logger queues\n");
	}

	my_sigevent.sigev_notify = SIGEV_SIGNAL;
	my_sigevent.sigev_signo  = LOGGER_SIGNO;
	if (mq_notify(logger_queue, &my_sigevent) == -1 )
	{
		printf("Failed to notify!\n");
		return NULL;
	}
	
	/* parse logger args */
	my_log_args = (logger_args *)malloc(sizeof(logger_args));
	my_log_args = (logger_args *) arg;
	logname = my_log_args->filename;
	out_file = fopen(logname, "a");
	if(out_file)
	{
		fputs("Opened file!\n", out_file);
	}

	while(logger_state == STATE_RUNNING)
	{
		pthread_cond_wait(&logger_cv, &logger_mutex);
		printf("Logger awake!\n");
	}

	fclose(out_file);	
	printf("Destroyed Logger\n");
	return NULL;
}




int8_t initLoggerQueues(mqd_t *main_queue, mqd_t *logger_queue)
{
	/* Create main queue*/
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	(*main_queue) = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL );
	if ((*main_queue) == (mqd_t) -1 )
	{
		return 1;
	}

	/* Create main queue*/
	printf("Creating queue \"%s\"\n", LOGGER_QUEUE_NAME);
	(*logger_queue) = mq_open(LOGGER_QUEUE_NAME, O_CREAT | O_RDONLY, 0755, NULL );
	if ((*logger_queue) == (mqd_t) -1 )
	{
		return 1;
	}
	printf("Created Logger\n");
	return 0;
}

int8_t logMessage(message_t *in_message)
{
	printf("[%d][%d] %s\n", in_message->source, in_message->timestamp, in_message->message);
	return 0;
}

int8_t initLogger(void)
{
	return 0;
}

void logInt(int32_t data)
{
	printf("[logger] received %u\n", data);
}

void logString(uint8_t *string, size_t length)
{
	/* malloc buffer, sprintf string into buffer, print it */
	printf("[logger] received %s\n", string);

}

