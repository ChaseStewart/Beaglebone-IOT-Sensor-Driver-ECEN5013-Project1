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

/*Send heartbeat to main*/
#define sendHeartbeatLight(x) sendHeartbeat(x, LIGHT_DRIVER_ID)

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

/* Function to write into the Light Sensor Registers */
int8_t writeLightRegisters(uint8_t data);

/* Function to write N-bytes into the Light Sensor Registers */
int8_t writeNLightRegisters(uint8_t* data, size_t length);

/* Function to write CTRL Register*/
int8_t writeCtrlReg(uint8_t data);

/* Function to read from the light register */
int8_t readLightRegisters(uint8_t regAddr, uint8_t *readData);

/* Function to read N-bytes from the light register, put result in retval, return status */
int8_t readNLightRegisters(uint8_t regAddr, uint8_t *readData, uint8_t length);

/* Function to read from the ID register */
int8_t readIDRegister(uint8_t* id);

/* Function to read from the CTRL register */
int8_t readCtrlReg(uint8_t* ctrl);

/* Function to read from the ADC0 register */
int8_t readADC0(uint16_t* lux);

/* Function to read from the ADC1 register */
int8_t readADC1(uint16_t* lux);

/*Function to know the Sensor Lux Value*/
int8_t lightSensorLux(float* intensity);

/*Function to find if the light is dark*/
bool isDark();

/*Function to find if the light is dark*/
bool isBright();

/* Function to configure integration time */
int8_t configTiming(INTTIME_T data);

/* Function to read from the Timing register */
int8_t readTimingRegister(uint8_t* data);

/*Function to disable Interrupt Register*/
int8_t lightDisableInt();

/*Function to enable Interrupt Register*/
int8_t lightEnableInt();

/* Function to read from the Timing register */
int8_t readInterruptRegister(uint8_t* data);

#endif
