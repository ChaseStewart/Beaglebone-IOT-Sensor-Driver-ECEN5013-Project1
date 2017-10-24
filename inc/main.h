#include "light_driver.h"
#include "temp_driver.h"
#include "logger.h"

#ifndef __MY_MAIN_H__
#define __MY_MAIN_H__


#define STATE_RUNNING  0
#define STATE_ERROR    1
#define STATE_SHUTDOWN 2

/* main program for the project */
int main(int argc, char **argv);

/* ensure all heartbeats are received*/
int8_t processHeartbeats(void);

/* request other thread to send heartbeat */
int8_t reqHeartbeat(int8_t id);

/* send heartbeat to the heartbeat queue */
int8_t sendHeartbeat(int8_t id);

#endif
