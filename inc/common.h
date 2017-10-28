#include <stdint.h>


#ifndef __MY_COMMON_H__
#define __MY_COMMON_H__

#define NUM_TASKS 3
#define MAX_FILELEN 1024
#define QUEUE_MSG_SIZE          100 
#define QUEUE_NUM_MSGS          1000
#define MAIN_QUEUE_NAME         "/heartbeat1\x00"
#define LOGGER_QUEUE_NAME       "/logger\x00"
#define TEMP_DRIVER_QUEUE_NAME  "/tempdriver\x00"
#define LIGHT_DRIVER_QUEUE_NAME "/lightdriver\x00"

#define STATE_RUNNING  0
#define STATE_ERROR    1
#define STATE_SHUTDOWN 2

/* define message_type */
typedef enum message_type {HEARTBEAT_REQ, HEARTBEAT_RSP, TEMP_DRIVER, LIGHT_DRIVER, LOGGER} Message_Type;
typedef enum task_id {MAIN_ID, TEMP_DRIVER_ID, LIGHT_DRIVER_ID, LOGGER_ID} Task_Id;

/* Structure for message Queues */
typedef struct
{
	Message_Type id;	/*Log ID - will be an enum*/
	uint32_t timestamp;	/*Timestamp*/
	size_t length;		/*Size of the message*/
	uint8_t* message;	/*Message Payload*/
}message_t;

typedef struct 
{
	size_t length;
	char *filename;
}logger_args;


#endif
