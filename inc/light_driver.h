#include <stdint.h>
#include <mqueue.h>


#ifndef __MY_LIGHT_DRIVER_H__
#define __MY_LIGHT_DRIVER_H__

#define I2C_FILE 				"/dev/i2c-2"	/*I2C device location*/
#define LIGHT_SLAVE_ADDRESS 	0x39			/*Slave Address*/

/*Address of registers*/
#define LIGHT_CTRL_REG			0x00
#define LIGHT_TIMING_REG		0x01
#define LIGHT_THR_LOW_LOW_REG	0x02
#define LIGHT_THR_LOW_HIGH_REG	0x03
#define LIGHT_THR_HIGH_LOW_REG	0x04
#define LIGHT_THR_HIGH_HIGH_REG	0x05
#define LIGHT_INT_REG			0x06
#define LIGHT_ID_REG			0x0A
#define LIGHT_DATA0LOW_REG		0x0C
#define LIGHT_DATA0HIGH_REG		0x0D
#define LIGHT_DATA1LOW_REG		0x0E
#define LIGHT_DATA1HIGH_REG		0x0F

/*Light Sensor Commands*/
#define BIT_WORD_CMD 			0x20
#define BIT_CMD_SELECT_REG		0x80
#define BIT_POWER_UP			0x03
#define BIT_POWER_DOWN			0x00
#define BIT_INT_TIME_14ms		0x00
#define BIT_INT_TIME_101ms		0x01
#define BIT_INT_TIME_402ms		0x10
#define BIT_INT_DISABLE			0x00
#define BIT_INT_ENABLE			0x01

typedef enum
{
	INT_TIME_14ms = 0,
	INT_TIME_101ms,
	INT_TIME_402ms
}INTTIME_T;

/* Main function for the light driver */
void *mainLightDriver(void *);

/* Function to configure the queues */
int8_t initLightQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *light_queue);

/* Function to configure the light sensor */
int8_t initLightDriver(void);

/* Function to read from the light register */
int8_t readLightRegisters(uint8_t regAddr, uint8_t *retval);

/* Function to read N-bytes from the light register, put result in retval, return status */
int8_t readNLightRegisters(uint8_t regAddr, uint8_t *retval, uint8_t length);

/* Function to write into the Light Sensor Registers */
int8_t writeLightRegisters(uint8_t regAddr, uint8_t data);

/*Send heartbeat to main*/
#define sendHeartbeatLight(x) sendHeartbeat(x, LIGHT_DRIVER_ID)

#endif
