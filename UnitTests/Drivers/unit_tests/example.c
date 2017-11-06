/**
 * @file testCB.c
 * @brief cmocka unit testing of all the functions of project1, project2 and Homework
 * @author Raj Kumar Subramaniam
 * @date July 13,2017
 **/
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdint.h>
#include "circbuff.h"


void test_circBuff_alloc(void **state)
{
	CB_t * cPtr = (CB_t*)malloc(sizeof(CB_t));
	CB_Status status;
	status = CirBuffInit(cPtr, 5);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	assert_non_null((void*) cPtr->buff);	/*Checks not NULL*/
	status = CirBuffDestroy(cPtr);
	assert_int_equal(status,CB_SUCCESS);
}

void test_circBuff_free(void **state)
{
	CB_t * cPtr = (CB_t*)malloc(sizeof(CB_t));
	CB_Status status;
	status = CirBuffInit(cPtr, 5);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	assert_non_null((void*) cPtr->buff);	/*Checks not NULL*/
	status = CirBuffDestroy(cPtr);
	assert_int_equal(status,CB_SUCCESS);
	assert_null((void*) cPtr->buff);	/*Checks for NULL*/
}

void test_circBuff_null(void **state)
{
	uint8_t data = 10;
	CB_t * cPtr = NULL;
	CB_Status status;
	status = CirBuffAddItem(cPtr,&data);	/*cPtr is Null*/
	assert_int_equal(status,CB_NULLPTR);	/*Checks Null*/
	cPtr = (CB_t*)malloc(sizeof(CB_t));
	status = CirBuffAddItem(cPtr,&data);	/*cPtr->buff is NULL*/
	assert_int_equal(status,CB_NULLPTR);	/*Checks Null*/
}

void test_circBuff_initialization(void **state)
{
	CB_t * cPtr = (CB_t*)malloc(sizeof(CB_t));
	CB_Status status;
	status = CirBuffInit(cPtr, 5);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	assert_non_null((void*) cPtr->buff);	/*Checks not NULL*/
	assert_int_equal(cPtr->count,0);	/*Count should be 0*/
	assert_int_equal(cPtr->size,5);		/*Checks the size*/
	assert_ptr_equal((void*)cPtr->buff, (void*) cPtr->head); /*Head is at Start*/
	assert_ptr_equal((void*)cPtr->buff, (void*) cPtr->tail); /*Tail is at Start*/
	status = CirBuffDestroy(cPtr);
	assert_int_equal(status,CB_SUCCESS);
}

void test_circBuff_add_remove(void **state)
{
	CB_t * cPtr = (CB_t*)malloc(sizeof(CB_t));
	CB_Status status;
	uint8_t a = 10;
	uint8_t *data = &a;
	status = CirBuffInit(cPtr, 5);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/

	/*Adding Data*/
	status = CirBuffAddItem(cPtr, data);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	a++;
	status = CirBuffAddItem(cPtr, data);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	a++;
	status = CirBuffAddItem(cPtr, data);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	
	/*Removing Data*/
	status = CirBuffRemoveItem(cPtr,data);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	assert_int_equal(*data,10);		/*Check the data*/
	status = CirBuffRemoveItem(cPtr,data);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	assert_int_equal(*data,11);		/*Check the data*/
	status = CirBuffRemoveItem(cPtr,data);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	assert_int_equal(*data,12);		/*Check the data*/

	/*Destroy the buffer*/
	status = CirBuffDestroy(cPtr);
	assert_int_equal(status,CB_SUCCESS);
}

void test_circBuff_full(void **state)
{
	CB_t * cPtr = (CB_t*)malloc(sizeof(CB_t));
	CB_Status status;
	uint8_t a = 10;
	status = CirBuffInit(cPtr, 3);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/

	/*Adding Data*/
	for(size_t i=0; i<3;i++)
	{
		status = CirBuffAddItem(cPtr, &a);
		assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	}
	status = CirBuffIsFull(cPtr);	
	assert_int_equal(status,CB_FULL);	/*Check Full*/

	/*Destroy the buffer*/
	status = CirBuffDestroy(cPtr);
	assert_int_equal(status,CB_SUCCESS);
}

void test_circBuff_empty(void **state)
{
	CB_t * cPtr = (CB_t*)malloc(sizeof(CB_t));
	CB_Status status;

	status = CirBuffInit(cPtr, 3);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/

	status = CirBuffIsEmpty(cPtr);	
	assert_int_equal(status,CB_EMPTY);	/*Check Empty*/

	/*Destroy the buffer*/
	status = CirBuffDestroy(cPtr);
	assert_int_equal(status,CB_SUCCESS);
}

void test_circBuff_wrap_add(void **state)
{
	CB_t * cPtr = (CB_t*)malloc(sizeof(CB_t));
	CB_Status status;
	status = CirBuffInit(cPtr, 3);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
	uint8_t a = 0;
	uint8_t *data = &a;
	/*Adding Data*/
	for(size_t i=0; i<3;i++)
	{
		status = CirBuffAddItem(cPtr, data);
		assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
		a++;
	}

	assert_ptr_equal((void*)cPtr->buff, (void*) cPtr->head); /*Head is at Start*/
	/*Destroy the buffer*/
	status = CirBuffDestroy(cPtr);
	assert_int_equal(status,CB_SUCCESS);
}

void test_circBuff_wrap_remove(void **state)
{
	CB_t * cPtr = (CB_t*)malloc(sizeof(CB_t));
	CB_Status status;
	uint8_t a = 0;
	uint8_t *data = &a;
	status = CirBuffInit(cPtr, 3);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/

	/*Adding Data*/
	for(size_t i=0; i<3;i++)
	{
		status = CirBuffAddItem(cPtr, data);
		assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
		a++;
	}
	a=0;
	for(size_t i=0; i<3;i++)
	{
		status = CirBuffRemoveItem(cPtr,data);
		assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
		
	}
	assert_ptr_equal((void*)cPtr->buff, (void*) cPtr->tail); /*Tail is at Start*/
	/*Destroy the buffer*/
	status = CirBuffDestroy(cPtr);
	assert_int_equal(status,CB_SUCCESS);
}

void test_circBuff_overfill(void **state)
{
	CB_t * cPtr = (CB_t*)malloc(sizeof(CB_t));
	CB_Status status;
	uint8_t a = 0;
	status = CirBuffInit(cPtr, 3);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/

	/*Adding Data*/
	for(size_t i=0; i<3;i++)
	{
		status = CirBuffAddItem(cPtr, &a);
		assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/
		a++;
	}

	status = CirBuffAddItem(cPtr, &a);
	assert_int_equal(status,CB_FULL);	/*Checks Success*/

	/*Destroy the buffer*/
	status = CirBuffDestroy(cPtr);
	assert_int_equal(status,CB_SUCCESS);
}

void test_circBuff_overempty(void **state)
{
	CB_t * cPtr = (CB_t*)malloc(sizeof(CB_t));
	CB_Status status;
	uint8_t a;
	uint8_t *data = &a;
	status = CirBuffInit(cPtr, 3);
	assert_int_equal(status,CB_SUCCESS);	/*Checks Success*/

	status = CirBuffRemoveItem(cPtr,data);	
	assert_int_equal(status,CB_EMPTY);	/*Check Empty*/

	/*Destroy the buffer*/
	status = CirBuffDestroy(cPtr);
	assert_int_equal(status,CB_SUCCESS);
}


int main(int argc, char **argv)	  /*this is main()*/
{
  const struct CMUnitTest tests[] = {
    /*circbuff.c Tests*/
    cmocka_unit_test(test_circBuff_alloc),
    cmocka_unit_test(test_circBuff_free),
    cmocka_unit_test(test_circBuff_null),
    cmocka_unit_test(test_circBuff_initialization),
    cmocka_unit_test(test_circBuff_add_remove),
    cmocka_unit_test(test_circBuff_full),
    cmocka_unit_test(test_circBuff_empty),
    cmocka_unit_test(test_circBuff_wrap_add),
    cmocka_unit_test(test_circBuff_wrap_remove),
    cmocka_unit_test(test_circBuff_overfill),
    cmocka_unit_test(test_circBuff_overempty),
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}



