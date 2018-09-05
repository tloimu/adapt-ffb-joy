#include "test.h"
#include <avr/io.h>

int gFailures = 0;

void fail(void)
{
	gFailures++;
}

void expect(long expected, long actual)
{
	if (expected != actual)
		fail();
}

void expectF(float expected, float actual)
{
	if (expected != actual)
		fail();
}

void expectFin(float expectedMin, float expectedMax, float actual)
{
	if (actual < expectedMin || actual > expectedMax)
		fail();
}


void expectNot(long notExpected, long actual)
{
	if (notExpected == actual)
		fail();
}

void expectNull(void* actual)
{
	if (actual != 0)
		fail();
}

void expectNotNull(void* actual)
{
	if (actual == 0)
		fail();
}

typedef struct
{
	testFunc func;
	const char *name;
	uint8_t result;
} TestFunction;

#define MAX_TESTS 100
int gTestCount = 0;
TestFunction gTests[MAX_TESTS];

int gHasFTests = 0;

TestFunction *currentTest = 0;

void define(testFunc func, const char *name)
{
	gTests[gTestCount].func = func;
	gTests[gTestCount].name = name;
	gTests[gTestCount].result = 0;
	gTestCount++;
}

void fdefine(testFunc func, const char *name)
{
	gTests[gTestCount].func = func;
	gTests[gTestCount].name = name;
	gTests[gTestCount].result = 1;
	gTestCount++;
	gHasFTests = 1;
}

void runAllTests()
{
	for (int i = 0; i < gTestCount; i++)
		{
		currentTest = &gTests[i];
		if (!gHasFTests || currentTest->result != 0)
			{
			int failures = gFailures;
			currentTest->func();
			if (failures != gFailures)
				currentTest->result = 2;
			}
		}
}
