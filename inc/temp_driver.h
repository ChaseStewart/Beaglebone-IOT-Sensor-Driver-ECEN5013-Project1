#include <stdint.h>

#ifndef __MY_TEMP_DRIVER_H__
#define __MY_TEMP_DRIVER_H__


#define TEMP_SLAVE_ADDR 0x00 // TODO FIXME
#define TEMP_ID 9

/* Main function for the temperature driver*/
void *mainTempDriver(void *);

/* Function to configure the temp sensor */
int8_t initTempDriver(void);

/* Function to read from the temp register */
int8_t readTempRegisters(uint8_t regAddr, uint8_t *retval);

/* Function to read N-bytes from the temp register, put result in retval, return status */
int8_t readNTempRegisters(uint8_t regAddr, uint8_t *retval, uint8_t length);

/* Function to write into the Temp Sensor Registers */
int8_t writeTempRegisters(uint8_t regAddr, uint8_t data);

/*Send heartbeat to main*/
#define sendHeartbeatTemp() sendHeartbeat(TEMP_ID)

#endif 
