#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef __MY_COMMON_H__
#define __MY_COMMON_H__

#define NUM_TASKS 3
#define MAX_FILELEN 1024

/* queue vars */
#define QUEUE_MSG_SIZE          100 
#define QUEUE_NUM_MSGS          1000
#define MAIN_QUEUE_NAME         "/heartbeat1\x00"
#define LOGGER_QUEUE_NAME       "/logger\x00"
#define TEMP_DRIVER_QUEUE_NAME  "/tempdriver\x00"
#define LIGHT_DRIVER_QUEUE_NAME "/lightdriver\x00"

/* signal variables */
#define HEARTBEAT_SIGNO    20
#define LOGGER_SIGNO       21
#define TEMP_DRIVER_SIGNO  22
#define LIGHT_DRIVER_SIGNO 23

/* message state vars */
#define STATE_RUNNING  0
#define STATE_ERROR    1
#define STATE_SHUTDOWN 2

/* all mutexes and condition variables */
extern pthread_cond_t  heartbeat_cv;
extern pthread_cond_t  logger_cv;
extern pthread_cond_t  temp_cv;
extern pthread_cond_t  light_cv;
extern pthread_mutex_t heartbeat_mutex;
extern pthread_mutex_t logger_mutex;
extern pthread_mutex_t temp_mutex;
extern pthread_mutex_t light_mutex;


extern volatile int main_state;
extern volatile int logger_state;
extern volatile int temp_state;
extern volatile int light_state;


/* define message_type */
typedef enum message_type {HEARTBEAT_REQ, HEARTBEAT_RSP, TEMP_DRIVER, LIGHT_DRIVER, LOGGER} Message_Type;

/* enumerate each task */
typedef enum task_id {MAIN_ID, TEMP_DRIVER_ID, LIGHT_DRIVER_ID, LOGGER_ID} Task_Id;

/* Structure for message Queues */
typedef struct
{
	Message_Type id;	/*Log ID - will be an enum*/
	uint32_t timestamp;	/*Timestamp*/
	Task_Id  source;	/*Source*/
	size_t length;		/*Size of the message*/
	uint8_t* message;	/*Message Payload*/
}message_t;

/* Structure for logger arguments */
typedef struct 
{
	size_t length;
	char *filename;
}logger_args;



#endif
