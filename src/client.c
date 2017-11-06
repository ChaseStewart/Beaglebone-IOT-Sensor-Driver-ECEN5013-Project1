#include "common.h"
#include "main.h"
#include "client.h"



void handleSigint(int sig)
{
	signal(SIGINT, handleSigint);
	client_state = STATE_SHUTDOWN;
	raise(SIGUSR2);
}


/* main program for the project */
int main(int argc, char **argv)
{
	sigset_t set;
	int retval, sig;
	struct sigevent my_sigevent;
	message_t *in_message;
	char in_buffer[2048];
	char mychar, waste;
	mqd_t main_queue, logger_queue, temp_queue, light_queue, client_queue;
	
	client_state = STATE_RUNNING;
	signal(SIGINT, handleSigint);
	printf("beginning \n");
	printf("send SIGUSR1 to bring up command interface \n");
	printf("send SIGINT to close the program \n");

	/* Create queue for client thread */
	client_queue = mq_open(CLIENT_QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, 0755, NULL);
	if (client_queue == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	
	/* Create queue for main thread */
	main_queue = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if (main_queue == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	/* Create queue for main thread */
	logger_queue = mq_open(LOGGER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if (main_queue == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	/* Create queue for main thread */
	light_queue = mq_open(LIGHT_DRIVER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if (main_queue == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	/* Create queue for main thread */
	temp_queue = mq_open(TEMP_DRIVER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if (temp_queue == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	
	
	
	/* catch heartbeat signal */
	sigemptyset(&set);
	sigaddset(&set, CLIENT_SIGNO);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	retval = pthread_sigmask(SIG_BLOCK, &set, NULL);
	if (retval != 0)
	{
		printf("Failed to set sigmask.\n");
		return -1;
	}
	
	/* register to receive logger signals */
	my_sigevent.sigev_notify = SIGEV_SIGNAL;
	my_sigevent.sigev_signo  = CLIENT_SIGNO;
	if (mq_notify(client_queue, &my_sigevent) == -1 )
	{
		printf("Failed to notify!\n");
		return -1;
	}

	sigemptyset(&set);
	sigaddset(&set, CLIENT_SIGNO);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	while (client_state > STATE_SHUTDOWN)
	{
		sigwait(&set, &sig);
		/* NOTE: this call is allowed to fail */
		mq_notify(client_queue, &my_sigevent);

		if (sig == CLIENT_SIGNO)
		{
			printf("Client awake\n");
			in_message = (message_t *) malloc(sizeof(message_t));
			errno = 0;
			while(errno != EAGAIN){
				retval = mq_receive(logger_queue, in_buffer, SIZE_MAX, NULL);
				if (retval <= 0 && errno != EAGAIN)
				{
					continue;
				}
				in_message = (message_t *)in_buffer;
				clientLogMessage(in_message);
			}
		}
		else if (sig == SIGUSR1)
		{
			printf("Welcome to SIGUSR1\n");
			printf("Press 'a' to request temp, press 'b' to request light,\n");
			printf("Press 'c' to log a custom string, press 'd' to close \n");
			
			mychar = getchar();
			waste = getchar();
			switch( mychar)
			{
				case 'a':
					clientsendTemp(temp_queue);
					break;
				case 'b':
					clientSendLight(light_queue);
					break;
				case 'c':
					clientSendLog(logger_queue);
					break;
				case 'd':
					clientSendShutdown(main_queue);
					break;
				default:
					printf("Invalid selection %c, returning to wait\n", mychar);
					break;
			}
		}
		else if (sig == SIGUSR2)
		{
			continue;
		}
	}
	
	printf("Closing the Client\n");	

	/* close this instance of the queue */
	retval = mq_close(client_queue);
	if (retval == -1)
	{
		printf( "Failed to reap queue\n");
		return 1;
	}
	/* close this instance of the queue */
	retval = mq_close(main_queue);
	if (retval == -1)
	{
		printf( "Failed to reap queue\n");
		return 1;
	}
	/* close this instance of the queue */
	retval = mq_close(logger_queue);
	if (retval == -1)
	{
		printf( "Failed to reap queue\n");
		return 1;
	}
	/* close this instance of the queue */
	retval = mq_close(light_queue);
	if (retval == -1)
	{
		printf( "Failed to reap queue\n");
		return 1;
	}
	/* close this instance of the queue */
	retval = mq_close(temp_queue);
	if (retval == -1)
	{
		printf( "Failed to reap queue\n");
		return 1;
	}

	/* unlink the client queue*/
	retval = mq_unlink(CLIENT_QUEUE_NAME);
	if (retval == -1)
	{
		printf("Failed to unlink queue\n");
		return 1;
	}

	printf("Goodbye\n");	
	return 0;
}

int clientLogMessage(message_t *in_message)
{
	char dbg_lvl[16];
	char mySource[16];
	switch(in_message->id)
	{
		case TEMP_VALUE:
			strcpy(dbg_lvl, "INFO");
			break;
		case LIGHT_VALUE:
			strcpy(dbg_lvl, "DEBUG");
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

	printf("[%s][%s][%d] %s", dbg_lvl, mySource, in_message->timestamp, in_message->message);
	return 0;
}


/* send temperature request*/
int clientsendTemp(mqd_t temp_queue)
{
	int retval;
	message_t msg;

	printf("Attempting to send temp\n");
	char units=0;
	msg.id = TEMP_DATA_REQ;
	msg.timestamp = time(NULL);
	msg.length = sizeof(char);
	msg.source = CLIENT_ID;
	msg.message = &units;
	retval = mq_send(temp_queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send from main to queue! Exiting...\n");
		return 1;
	}

}

/* send light request */
int clientSendLight(mqd_t light_queue)
{
	int retval;
	message_t msg;

	printf("Attempting to send light\n");
	msg.id = LIGHT_DATA_REQ;
	msg.timestamp = time(NULL);
	msg.length = 0;
	msg.source = CLIENT_ID;
	msg.message = NULL;
	retval = mq_send(light_queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send from client to queue! Exiting...\n");
		return 1;
	}
	return 0;
}

/* send log request */
int clientSendLog(mqd_t logger_queue)
{
	int retval;
	message_t msg;
	char in_buf[QUEUE_MSG_SIZE];
	
	printf("Attempting to send log\n");
	printf("Write a string to send to logger:\n\t>");
	fgets(in_buf, QUEUE_MSG_SIZE, stdin);

	msg.id = LOGGER;
	msg.timestamp = time(NULL);
	msg.length = strlen(in_buf);
	msg.priority = LOG_DEBUG;
	msg.source = CLIENT_ID;
	msg.message = in_buf;
	retval = mq_send(logger_queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send from client to queue! Exiting...\n");
		return 1;
	}
	return 0;
}

/* send the shutdown*/
int clientSendShutdown(mqd_t main_queue)
{
	int retval;
	message_t msg;

	printf("Attempting to send shutdown\n");
	msg.id = CLIENT_SHUTDOWN;
	msg.timestamp = time(NULL);
	msg.length = 0;
	msg.source = CLIENT_ID;
	msg.message = NULL;
	retval = mq_send(main_queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send from client to queue! Exiting...\n");
		return 1;
	}
	return 0;

}



