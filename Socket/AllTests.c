#include<stdio.h>

#include "CuTest.h"

CuSuite* CuGetSuite();

void RunAllTests()
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();
	
	CuSuiteAddSuite(suite, CuGetSuite());
	
	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}

int main (void)
{
	RunAllTests();
} 
