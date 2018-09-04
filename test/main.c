#include <avr/io.h>
#include "../ffb-abacus.h"
#include "test.h"
#include "testsuites.h"

int main(void)
{
	FfbAcabus_Init();
	testSuiteSetEffect();
	testSuiteConstantForce();
	testSuiteSpringForce();
	testSuiteSineForce();

	runAllTests();

	int failures = gFailures;
	failures++;
	return gFailures;
}


