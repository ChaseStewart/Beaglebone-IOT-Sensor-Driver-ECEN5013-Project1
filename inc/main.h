#include "light_driver.h"
#include "temp_driver.h"
#include "logger.h"


#define STATE_RUNNING  0
#define STATE_ERROR    1
#define STATE_SHUTDOWN 2

/* main program for the project */
int main(int argc, char **argv);

/* ensure all heartbeats are received*/
int8_t processHeartbeats(void);

/* request other thread to send heartbeat */
int8_t reqHeartbeat(int8_t id);

