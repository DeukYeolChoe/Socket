#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Server_v3.h"
#include "CuTest.h"

void TestgetFileExtension_validFile(CuTest* tc)
{
	char* input = strdup("Kartrider.png");
	int actual = getFileExtension(input);
	printf("%d\n", actual);
	int expected = 2;//Success
	CuAssertIntEquals(tc, expected, actual); 
}

void TestgetFileExtension_invalidFile(CuTest* tc)
{
	char* input = strdup("Kartirider.jpedg");
	int actual = getFileExtension(input);
	printf("%d\n", actual);
	int expected = 1;//failure
	CuAssertIntEquals(tc, expected, actual); 
}

void TestgetFileExtension_null(CuTest* tc)
{
	char* input = '\0';
	int actual = getFileExtension(input);
	printf("%d\n", actual);
	int expected = 0;//NULL
	CuAssertIntEquals(tc, expected, actual); 
}

void TestgetFilePath_validFilePath1(CuTest* tc)
{
	char* input = strdup("GET /index.html");
	int actual = getFilePath(input);
	printf("%d\n", actual);
	int expected = 1;
	CuAssertIntEquals(tc, expected, actual); 
}

void TestgetFilePath_validFilePath2(CuTest* tc)
{
	char* input = strdup("GET ////////////////////////////////");
	int actual = getFilePath(input);
	printf("%d\n", actual);
	int expected = 1;
	CuAssertIntEquals(tc, expected, actual); 
}

void TestgetFilePath_invalidFilePath1(CuTest* tc)
{
	char* input = strdup("GET /home/deukyeolchoe/Socket/index.html");
	int actual = getFilePath(input);
	printf("%d\n", actual);
	int expected = 0;
	CuAssertIntEquals(tc, expected, actual); 
}
	
void TestgetFilePath_invalidFilePath2(CuTest* tc)
{
	char* input = strdup("GET /////eddf///////adfsdf///adfaf//");
	int actual = getFilePath(input);
	printf("%d\n", actual);
	int expected = 0;
	CuAssertIntEquals(tc, expected, actual); 
}

CuSuite* CuGetSuite()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestgetFileExtension_validFile);
	SUITE_ADD_TEST(suite, TestgetFileExtension_invalidFile);
	SUITE_ADD_TEST(suite, TestgetFileExtension_null);
	SUITE_ADD_TEST(suite, TestgetFilePath_validFilePath1);
	SUITE_ADD_TEST(suite, TestgetFilePath_validFilePath2);
	SUITE_ADD_TEST(suite, TestgetFilePath_invalidFilePath1);
	SUITE_ADD_TEST(suite, TestgetFilePath_invalidFilePath2);
	
	return suite;
}
