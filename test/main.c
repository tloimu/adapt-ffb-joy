#include <avr/io.h>
#include "../ffb-abacus.h"
#include "test.h"
#include "testsuites.h"

void testSuiteConstantForce();

int main(void)
{
	FfbAcabus_Init();
	testSuiteConstantForce();

	runAllTests();

	int failures = gFailures;
	failures++;
	return gFailures;
}


