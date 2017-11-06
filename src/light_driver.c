#include "common.h"
#include "light_driver.h"

#define FAKE_SENSORS 1 /* for chase who doesn't have the sensors */

int32_t i2cHandle;	/*File Descriptor for I2C access*/

void * mainLightDriver(void *arg)
{
	sigset_t set;
	int retval, sig;
	mqd_t main_queue, logger_queue, light_queue;
	char in_buffer[4096];
	message_t *in_message;
	struct sigevent my_sigevent;

	initLightQueues(&main_queue, &logger_queue, &light_queue);
	logFromLight(logger_queue, LOG_INFO, "Initialized LightQueues\n");	
	printf("Initializing LightDriver\n");
	
	/* register to receive logger signals */
	my_sigevent.sigev_notify = SIGEV_SIGNAL;
	my_sigevent.sigev_signo  = LIGHT_DRIVER_SIGNO;
	if (mq_notify(light_queue, &my_sigevent) == -1 )
	{
		printf("failed to light notify!\n");
		return NULL;
	}
	
	retval = blockAllSigs();
	if (retval != 0)
	{
		printf("Failed to set sigmask.\n");
		return NULL;
	}
	
	initLightDriver();
	logFromLight(logger_queue, LOG_INFO, "Initialized Light Driver\n");	

	sigemptyset(&set);
	sigaddset(&set, LIGHT_DRIVER_SIGNO);
	while(light_state > STATE_SHUTDOWN)
	{
		sigwait(&set, &sig);
		if (mq_notify(light_queue, &my_sigevent) == -1 )
		{
			printf("failed to light notify!\n");
			return NULL;
		}

		in_message = (message_t *) malloc(sizeof(message_t));
		errno = 0;
		while(errno != EAGAIN){
			retval = mq_receive(light_queue, in_buffer, SIZE_MAX, NULL);
			if (retval <= 0 && errno != EAGAIN)
			{
				continue;
			}
			in_message = (message_t *)in_buffer;

			/* process Light Driver Req */
			if (in_message->id == LIGHT_DATA_REQ )
			{
				logFromLight(logger_queue, LOG_INFO, "Got Light Driver message\n");	
			} 

			else if (in_message->id == HEARTBEAT_REQ) 
			{
				sendHeartbeat(main_queue, LIGHT_DRIVER_ID);
			}
		}
	}
	
	logFromLight(logger_queue, LOG_INFO, "Destroyed LightDriver\n");
	return NULL;
}

/* Function to configure the light sensor */
int8_t initLightQueues(mqd_t *main_queue, mqd_t *logger_queue, mqd_t *light_queue)
{

	/* Create queue for main thread */
	(*main_queue) = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*main_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}
	
	/* Create queue for logger thread */
	(*logger_queue) = mq_open(LOGGER_QUEUE_NAME, O_CREAT | O_WRONLY, 0755, NULL);
	if ((*logger_queue) == (mqd_t) -1)
	{
		printf("Failed to initialize queue! Exiting...\n");
		return 1;
	}

	/* Create queue for logger thread */
	(*light_queue) = mq_open(LIGHT_DRIVER_QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, 0755, NULL);
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
	if (FAKE_SENSORS)
	{
		printf("DID NOT ACTUALLY INIT LIGHT SENSOR\n");
		return 0;
	}

	printf("Setup LightDriver\n");
	if ((i2cHandle = open(I2C_FILE,O_RDWR)) < 0)
	{
		printf("Error Opening I2C device - Light Sensor\n");
		exit(1);
	}

	if(ioctl(i2cHandle,I2C_SLAVE, LIGHT_SLAVE_ADDRESS) < 0)
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
int8_t writeNLightRegisters(uint8_t* data, size_t length)
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
	status = writeNLightRegisters(buffer,2);	
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
	uint16_t iCH0 = 0;
	uint16_t iCH1 = 0;
	float CH0 = 0;
	float CH1 = 0;
	float selectFormula = 0;
	int8_t status;
	status = readADC0(&iCH0);
	status = readADC1(&iCH1);
	CH0 = iCH0;
	CH1 = iCH1;
	selectFormula = CH1/CH0;	/*Lux calculation depends on this ratio*/
	/*Formula Selection*/
	if(selectFormula <= 0.5)
	{
		*intensity =  (0.0304*CH0)-(0.062*CH0*((CH1/CH0)*1.4));
	}
	else if(selectFormula <= 0.61)
	{
		*intensity = (0.0224*CH0)-(0.031*CH1);
	}
	else if(selectFormula <= 0.8)
	{
		*intensity =  (0.0128*CH0)-(0.0153*CH1);
	}
	else if(selectFormula <= 1.3)
	{
		*intensity =  (0.00146*CH0)-(0.00112*CH1);
	}
	else
	{
		*intensity = 0;
	}
	return status;
}

/*Function to find if the light is dark*/
bool isDark()
{
	float intensity;
	if(lightSensorLux(&intensity))
	{
		return false;
	}
	if(intensity < 50)	/*Lux value of <50 is assumed night*/
	{
		return true;
	}
	else
	{
		return false;
	}
}


/*Function to find if the light is dark*/
bool isBright()
{
	float intensity;
	if(lightSensorLux(&intensity))
	{
		return false;
	}
	if(intensity > 50)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* Function to configure integration time */
int8_t configTiming(INTTIME_T data)
{
	int8_t status;
	int8_t buffer[2];
	if(data >2)		//If invalid value
		return -1;
	buffer[0] = BIT_CMD_SELECT_REG | LIGHT_TIMING_REG;	/*Register Address*/
	buffer[1] = data;			/*The Enum match the required value to use into the register*/
	status = writeNLightRegisters(buffer,2);	
	return status;
}

/* Function to read from the Timing register */
int8_t readTimingRegister(uint8_t* data)
{
	int8_t status;
	status = readLightRegisters(LIGHT_TIMING_REG, data);
	return status;
}

/*Function to disable Interrupt Register*/
int8_t lightDisableInt()
{
	int8_t status;
	int8_t buffer[2];
	buffer[0] = BIT_CMD_SELECT_REG | LIGHT_INT_REG;	/*Register Address*/
	buffer[1] = BIT_INT_DISABLE;					
	status = writeNLightRegisters(buffer,2);		
	return status;
}

/*Function to enable Interrupt Register*/
int8_t lightEnableInt()
{
	int8_t status;
	int8_t buffer[2];
	buffer[0] = BIT_CMD_SELECT_REG | LIGHT_INT_REG;	/*Register Address*/
	buffer[1] = BIT_INT_ENABLE;					
	status = writeNLightRegisters(buffer,2);		
	return status;
}

/* Function to read from the Timing register */
int8_t readInterruptRegister(uint8_t* data)
{
	int8_t status;
	status = readLightRegisters(LIGHT_INT_REG, data);
	return status;
}

/* send a message to logger */
int8_t logFromLight(mqd_t queue, int prio, char *message)
{
	int retval;
	message_t msg;

	msg.id = LOGGER;
	msg.timestamp = time(NULL);
	msg.length = strlen(message);
	msg.priority = prio;
	msg.source = LIGHT_DRIVER_ID;
	msg.message = message;
	retval = mq_send(queue, (const char *) &msg, sizeof(message_t), 0);
	if (retval == -1)
	{
		printf("Failed to send to queue! Exiting...\n");
		return 1;
	}

}
