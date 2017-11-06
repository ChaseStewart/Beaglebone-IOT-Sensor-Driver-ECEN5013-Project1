/**
 * @file tst_temp.c
 * @brief cmocka unit testing of all the functions of project1
 * @author Raj Kumar Subramaniam
 * @date Nov 5, 2017
 **/	
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdint.h>
#include "common.h"
#include "temp_driver.h"

void test_currentTemperature_celcius(void **state)
{
	int16_t temp = 0;
	int8_t ret;
	ret = currentTemperature(&temp, UNIT_CELCIUS);
	assert_int_equal(ret, 0);
	assert_int_equal(temp,20);
}

void test_currentTemperature_Fah(void **state)
{
	int16_t temp = 0;
	int8_t ret;
	ret = currentTemperature(&temp, UNIT_FAHRENHEIT);
	assert_int_equal(ret, 0);
	assert_int_equal(temp,68);
}

void test_currentTemperature_kelvin(void **state)
{
	int16_t temp = 0;
	int8_t ret;
	ret = currentTemperature(&temp, UNIT_KELVIN);
	assert_int_equal(ret, 0);
	assert_int_equal(temp,293);
}

int16_t tempConversionPositive(void **state)
{
	int16_t temp = 0x0170;
	temp = tempConversion(temp);
	assert_int_equal(temp,23);
}


int main(int argc, char **argv)	  /*this is main()*/
{
  const struct CMUnitTest tests[] = {
  	/*memory.c Tests*/
    cmocka_unit_test(test_currentTemperature_celcius),
    cmocka_unit_test(test_currentTemperature_Fah),
    cmocka_unit_test(test_currentTemperature_kelvin),
    cmocka_unit_test(tempConversionPositive)
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}


