# Unit Tester

The unit tests are ran in Atmel Studio 7.0 (AS7).

Open the solution file **unittest.atsln**. There, make sure the Device **ATmega32** and the Tool **Simulator** are set in the project properties and that you have the needed plugins installed in the AS7.


## Running Tests

To run unit tests and verify the results, after opening the unit test solution, do the following in Atmel Studio.

First, set a breakpoint to the last line in **main.c**

	int main(void)
	{
		...
		int failures = gFailures;
		failures++;
		return gFailures;	<-- Set the breakpoint here
	}


From there, you can see that how many tests failed by checking the value of the **gFailures** variable. If it is zero, all unit tests have passed.

If you get failures, you can easily get them by setting another breakpoint to function in file **test.c**

	void fail(void)
	{
		gFailures++;	<-- Set another breakpoint here
	}

If a test failure occurs, debugger stops here and from the Call Stack view, you can see where exactly the test failed.

## How to run only one test?

The test functions are registered using **define()**. Normally, all tests are ran when **runAllTests()** is called. However, to run a single test case only, modify the **define()** line into **fdefine()** and then, only that test function is ran.

For example, to run only the test function named **testTwo()** even when three (or more) are registered, change:

	void testSuiteMyTests(void)
	{
		define(testOne, "test one");
		fdefine(testTwo, "test two");	<--- using "fdefine" here results running this test function only
		define(testThree, "test three");
	}

(Those familiar with unit test tools like Jasmine should recognize some similarities here.)

## Adding tests

There is a hard limit to amount of test functions that can be registered in **test.c**

	#define MAX_TESTS 100

You may increase that up to amount that is allowed by the device memory limit.

## Test results

Calculating a single effect takes time (in debug mode)

| time | cycles | Effect |
-----------------------------
|   45 |    716 | Constant |
|  211 |   3378 | Sine |
|  |  | Square |
|  |  | Triangle |
|  |  | Sawtooth |
|  |  | Spring |
|  |  | Envelope |
