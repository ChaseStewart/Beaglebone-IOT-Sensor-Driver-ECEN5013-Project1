#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mqueue.h>
#include <errno.h>
#include <pthread.h>

#include "../inc/common.h"
#include "unity.h"
#include "unit_test.h"

#define WEIRD_INVALID_QUEUE 1345 
void test_queue(void)
{
        mqd_t main_queue ;
	int retval; 
	printf("Close all queues by running sudo rm -rf /dev/mqueue/*\n");
        printf("press enter to continue\n");
        getchar();

        printf("First ensure project 1 is running!\n");
        printf("if it is, close it!\n");
        printf("press enter to continue\n");
        getchar();

        printf("Test open w wrong name\n");
        main_queue = mq_open("definitely wrong name", O_CREAT | O_WRONLY | O_NONBLOCK, 0755, NULL);
        TEST_ASSERT_EQUAL_INT32(-1, (int32_t)main_queue);
        TEST_ASSERT_EQUAL_INT32(EINVAL, (int32_t ) errno);

        printf("Test open w different wrong name, should fail\n");
        main_queue = mq_open("almostright\x00", O_WRONLY | O_NONBLOCK, 0755, NULL);
        TEST_ASSERT_EQUAL_INT32(-1, (int32_t)main_queue);
        
	printf("Test open w right name\n");
        main_queue = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_RDWR | O_NONBLOCK, 0755, NULL);
        TEST_ASSERT_GREATER_THAN(-1, (int32_t)main_queue);
        
	printf("test close invalid queue \n");
	retval = mq_close(WEIRD_INVALID_QUEUE);	
        TEST_ASSERT_EQUAL_INT32(-1, (int32_t)retval);
        TEST_ASSERT_EQUAL_INT32(EBADF, (int32_t ) errno);
	
	printf("test close valid queue \n");
	retval = mq_close(main_queue);	
        TEST_ASSERT_EQUAL_INT32(0, (int32_t)retval);

}

void test_messages(void)
{
        mqd_t main_queue ;
	int retval, mylen; 
	char message[] = "Hello";	
	char in_buffer[1024];
	
	mylen= strlen(message);

	TEST_ASSERT_GREATER_THAN(0, 1);
	printf("Test open w right name\n");
        main_queue = mq_open(MAIN_QUEUE_NAME, O_CREAT | O_RDWR | O_NONBLOCK, 0755, NULL);
        TEST_ASSERT_GREATER_THAN(-1, (int32_t)main_queue);
       
	retval = mq_send(main_queue, (const char *) &message, mylen, 0 );
        TEST_ASSERT_EQUAL_INT32(0, (int32_t)retval);
	
	retval = mq_receive(main_queue, in_buffer, SIZE_MAX, NULL );
        TEST_ASSERT_GREATER_THAN(0, (int32_t)retval);
 
}


int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_queue);
	RUN_TEST(test_messages);
	return UNITY_END();
}
