#include <stdio.h>
#include "light_driver.h"
#include "common.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

int32_t i2cHandle;	/*File Descriptor for I2C access*/

void * mainLightDriver(void *arg)
{
	mqd_t main_queue, logger_queue, light_queue;

	printf("Initializing Queues\n");	
	initLightQueues(&main_queue, &logger_queue, &light_queue);
	printf("Initializing LightDriver\n");
	initLightDriver();
	printf("Setup LightDriver\n");

	while(light_state == STATE_RUNNING)
	{
		pthread_cond_wait(&light_cv, &light_mutex);
		printf("Light Driver Awake!\n");
	}
	
	printf("Destroyed LightDriver\n");
	return NULL;
}

/* Function to configure the light sensor */
int8_t initLightQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *light_queue)
{

	/* Create queue for main thread */
	printf("Creating queue \"%s\"\n", MAIN_QUEUE_NAME);
	(*main_queue) = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*main_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	
	/* Create queue for logger thread */
	printf("Creating queue \"%s\"\n", LOGGER_QUEUE_NAME);
	(*logger_queue) = mq_open(LOGGER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*logger_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}

	/* Create queue for logger thread */
	printf("Creating queue \"%s\"\n", TEMP_DRIVER_QUEUE_NAME);
	(*light_queue) = mq_open(TEMP_DRIVER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*light_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}

	return 0;
}


/* Function to configure the light sensor */
int8_t initLightDriver(void)
{
	printf("Setup LightDriver\n");
	//sendHeartbeatLight();

	if ((i2cHandle = open(I2C_FILE,O_RDWR)) < 0)
	{
		printf("Error Opening I2C device - Light Senosr\n");
		exit(1);
	}

	if(ioctl(i2cHandle,I2C_SLAVE, SLAVE_ADDRESS) < 0)
	{
		printf("Failed to talk to Slave and Acquire bus access\n");
		exit(1);
	}

	return 0;
}

/* Function to write into the Light Sensor Registers */
int8_t writeLightRegisters(uint8_t data)
{
	if(write(i2cHandle, &data, 1) != 1)
	{
		printf("I2C Write error seen\n");
		return -1;
	}
	return 0;
}

/* Function to write N-bytes into the Light Sensor Registers */
int8_t writeNTempRegisters(uint8_t* data, size_t length)
{
	if(write(i2cHandle, data, length) != length)	/*Writes N-bytes into file*/
	{
		printf("I2C Write error seen\n");
		return -1;
	}
	return 0;
}

/* Function to write CTRL Register*/
int8_t writeCtrlReg(uint8_t data)
{
	int8_t status;
	int8_t buffer[2];
	buffer[0] = BIT_CMD_SELECT_REG | LIGHT_CTRL_REG;	/*Register Address*/
	buffer[1] = data;			/*Byte1*/
	status = writeNTempRegisters(buffer,2);	
	return status;
}
/* Function to read from the light register */
int8_t readLightRegisters(uint8_t regAddr, uint8_t *readData)
{
	regAddr = regAddr | BIT_CMD_SELECT_REG;
	writeLightRegisters(regAddr);		/*Writing into register*/	
	if(read(i2cHandle, readData, 1) != 1)
	{
		printf("Less Number of bytes are received/read error\n");
		return -1;
	}
	return 0;
}

/* Function to read N-bytes from the light register, put result in retval, return status */
int8_t readNLightRegisters(uint8_t regAddr, uint8_t *readData, uint8_t length)
{
	regAddr = regAddr | BIT_CMD_SELECT_REG | BIT_WORD_CMD;
	writeLightRegisters(regAddr);		/*Writing into register*/
	if(readData == NULL)
	{
		return -1;
	}
	if(read(i2cHandle, readData, length) != length)	/*Stores read value*/
	{
		printf("Less Number of bytes are received/read error\n");
		return -1;
	}
	return 0;
}


/* Function to read from the ID register */
int8_t readIDRegister(uint8_t* id)
{
	int8_t status;
	status = readLightRegisters(LIGHT_ID_REG, id);
	return status;
}

/* Function to read from the CTRL register */
int8_t readCtrlReg(uint8_t* ctrl)
{
	int8_t status;
	status = readLightRegisters(LIGHT_CTRL_REG, ctrl);
	return status;
}

/* Function to read from the ADC0 register */
int8_t readADC0(uint16_t* lux)
{
	int8_t status;
	status = readNLightRegisters(LIGHT_DATA0LOW_REG, (uint8_t*)lux,2);
	return status;
}

/* Function to read from the ADC1 register */
int8_t readADC1(uint16_t* lux)
{
	int8_t status;
	status = readNLightRegisters(LIGHT_DATA1LOW_REG, (uint8_t*)lux,2);
	return status;
}

/*Function to know the Sensor Lux Value*/
int8_t lightSensorLux(float* intensity)
{
	
}

