#include "common.h"
#include "main.h"


int client_state = STATE_RUNNING;


void handleSigterm(int sig)
{
	client_state = STATE_SHUTDOWN;
	raise(CLIENT_SIGNO);
}


/* main program for the project */
int main(int argc, char **argv)
{
	sigset_t set;
	int retval, sig;
	struct sigevent my_sigevent;
	message_t *in_message;
	char in_buffer[2048];
	mqd_t main_queue, logger_queue, temp_queue, light_queue, client_queue;
	
	signal(SIGTERM, handleSigterm);

	printf("beginning \n");

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
	
	/* catch heartbeat signal */
	sigemptyset(&set);
	sigaddset(&set, CLIENT_SIGNO);
	sigaddset(&set, SIGINT);
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
		return NULL;
	}

	sigemptyset(&set);
	sigaddset(&set, CLIENT_SIGNO);
	sigaddset(&set, SIGINT);
	while (client_state > STATE_SHUTDOWN)
	{
		sigwait(&set, &sig);
		/* NOTE: this call is allowed to fail */
		mq_notify(client_queue, &my_sigevent);

		if (sig == CLIENT_SIGNO)
		{
		
			in_message = (message_t *) malloc(sizeof(message_t));
			errno = 0;
			while(errno != EAGAIN){
				retval = mq_receive(logger_queue, in_buffer, SIZE_MAX, NULL);
				if (retval <= 0 && errno != EAGAIN)
				{
					continue;
				}
				in_message = (message_t *)in_buffer;

				/* process Log*/
				if (in_message->id == TEMP_VALUE)
				{
					//logMessage(in_message);
				} 
				if (in_message->id == LIGHT_VALUE )
				{

				}

				else if (in_message->id == HEARTBEAT_REQ) 
				{
				}
			}
		}
		else if (sig == SIGINT)
		{
			printf("Welcome to SIGINT\n");
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

	/* unlink the client queue*/
	retval = mq_unlink(CLIENT_QUEUE_NAME);
	if (retval == -1)
	{
		printf("Failed to unlink queue\n");
		return 1;
	}
	
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


