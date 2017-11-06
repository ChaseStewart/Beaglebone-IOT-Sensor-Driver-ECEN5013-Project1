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
 #include "light_driver.h"

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

void tempConversionPositive(void **state)
{
	int16_t temp = 0x1700;
	temp = tempConversion(temp);
	assert_int_equal(temp,23);
}

void tempConversion0(void **state)
{
	int16_t temp = 0;
	temp = tempConversion(temp);
	assert_int_equal(temp,0);
}

void tempConversionNegative(void **state)
{
	int16_t temp = 0xFF00;
	temp = tempConversion(temp);
	assert_int_equal(temp,-1);	
}

void test_lightSensorLux1(void **state)
{
	float data;
	int8_t ret;
	ret = lightSensorLux(&data, 0x2345, 0x50);

}


int main(int argc, char **argv)	  /*this is main()*/
{
  const struct CMUnitTest tests[] = {
  	/*memory.c Tests*/
    cmocka_unit_test(test_currentTemperature_celcius),
    cmocka_unit_test(test_currentTemperature_Fah),
    cmocka_unit_test(test_currentTemperature_kelvin),
    cmocka_unit_test(tempConversionPositive),
    cmocka_unit_test(tempConversion0),
    cmocka_unit_test(tempConversionNegative),
    cmocka_unit_test(test_lightSensorLux1)
  };
  return cmocka_run_group_tests(tests, NULL, NULL);
}


