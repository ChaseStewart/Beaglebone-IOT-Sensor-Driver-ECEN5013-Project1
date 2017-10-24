#include <stdint.h>
#include <stdlib.h>

typedef enum message_type {HEARTBEAT, TEMP, LIGHT} Message_Type;


/*Structure for message Queues*/
typedef struct
{
	Message_Type id;	/*Log ID - will be an enum*/
	uint32_t timeStamp;	/*Timestamp*/
	size_t length;		/*Size of the message*/
	uint8_t* message;	/*Message Payload*/
}message_t;



void *mainLogger(void *);

/* set up the logger to run*/
int8_t initLogger(void);

/*Function to print the logger structure onto the log file*/
void logStruct(message_t logs);

/*Function to log integers by converting them to ascii values*/
void logInt(int32_t data);

/*Function to log strings*/
void logString(uint8_t* string, size_t length);

/*Send heartbeat to main*/
int8_t sendHeartbeat(void);

