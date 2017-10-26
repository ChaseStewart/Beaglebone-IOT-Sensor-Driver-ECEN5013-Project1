#include "light_driver.h"
#include "temp_driver.h"
#include "logger.h"

#ifndef __MY_MAIN_H__
#define __MY_MAIN_H__

#define MAIN_QUEUE_NAME "/heartbeat\x00"
#define MAIN_MSG_SIZE  100 
#define MAIN_NUM_MSGS 1000

#define STATE_RUNNING  0
#define STATE_ERROR    1
#define STATE_SHUTDOWN 2

/* define message_type */
typedef enum message_type {HEARTBEAT, TEMP, LIGHT} Message_Type;

/* Structure for message Queues */
typedef struct
{
	Message_Type id;	/*Log ID - will be an enum*/
	uint32_t timestamp;	/*Timestamp*/
	size_t length;		/*Size of the message*/
	uint8_t* message;	/*Message Payload*/
}message_t;



/* main program for the project */
int main(int argc, char **argv);

/* ensure all heartbeats are received*/
int8_t processHeartbeats(void);

/* request other thread to send heartbeat */
int8_t reqHeartbeat(int8_t id);

/* send heartbeat to the heartbeat queue */
int8_t sendHeartbeat(int8_t id);

#endif
