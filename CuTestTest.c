#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "CuTest.h"

/*-------------------------------------------------------------------------*
 * CuString Test
 *-------------------------------------------------------------------------*/

void TestCuStringNew(CuTest* tc)
{
	CuString* str = CuStringNew();
	CuAssertTrue(tc, 0 == str->length);
	CuAssertTrue(tc, 0 != str->size);
	CuAssertStrEquals(tc, "", str->buffer);
}


void TestCuStringAppend(CuTest* tc)
{
	CuString* str = CuStringNew();
	CuStringAppend(str, "hello");
	CuAssertIntEquals(tc, 5, str->length);
	CuAssertStrEquals(tc, "hello", str->buffer);
	CuStringAppend(str, " world");
	CuAssertIntEquals(tc, 11, str->length);
	CuAssertStrEquals(tc, "hello world", str->buffer);
}


void TestCuStringAppendChar(CuTest* tc)
{
	CuString* str = CuStringNew();
	CuStringAppendChar(str, 'a');
	CuStringAppendChar(str, 'b');
	CuStringAppendChar(str, 'c');
	CuStringAppendChar(str, 'd');
	CuAssertIntEquals(tc, 4, str->length);
	CuAssertStrEquals(tc, "abcd", str->buffer);
}


void TestCuStringResizes(CuTest* tc)
{
	CuString* str = CuStringNew();
	int i;
	for(i = 0 ; i < STRING_MAX ; ++i) 
	{
		CuStringAppend(str, "aa");
	}
	CuAssertTrue(tc, STRING_MAX * 2 == str->length);
	CuAssertTrue(tc, STRING_MAX * 2 <= str->size);
}
	
CuSuite* CuStringGetSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestCuStringNew);
	SUITE_ADD_TEST(suite, TestCuStringAppend);
	SUITE_ADD_TEST(suite, TestCuStringAppendChar);
	SUITE_ADD_TEST(suite, TestCuStringResizes);

	return suite;
}

/*-------------------------------------------------------------------------*
 * CuTest Test
 *-------------------------------------------------------------------------*/

void TestPasses(CuTest* tc)
{
	CuAssert(tc, "test should pass", 1 == 0 + 1);
}

void TestFails(CuTest* tc)
{
	CuAssert(tc, "test should fail", 1 == 1 + 1);
}


void TestCuTestNew(CuTest* tc)
{
	CuTest* tc2 = CuTestNew("MyTest", TestPasses);
	CuAssertStrEquals(tc, "MyTest", tc2->name);
	CuAssertTrue(tc, !tc2->failed);
	CuAssertTrue(tc, tc2->message == NULL);
	CuAssertTrue(tc, tc2->function == TestPasses);
	CuAssertTrue(tc, tc2->ran == 0);
	CuAssertTrue(tc, tc2->jumpBuf == NULL);
}


void TestCuTestInit(CuTest *tc)
{
	CuTest tc2;
	CuTestInit(&tc2, "MyTest", TestPasses);
	CuAssertStrEquals(tc, "MyTest", tc2.name);
	CuAssertTrue(tc, !tc2.failed);
	CuAssertTrue(tc, tc2.message == NULL);
	CuAssertTrue(tc, tc2.function == TestPasses);
	CuAssertTrue(tc, tc2.ran == 0);
	CuAssertTrue(tc, tc2.jumpBuf == NULL);
}

void TestCuAssert(CuTest* tc)
{
	CuTest tc2;
	CuTestInit(&tc2, "MyTest", TestPasses);

	CuAssert(&tc2, "test 1", 5 == 4 + 1);
	CuAssertTrue(tc, !tc2.failed);
	CuAssertTrue(tc, tc2.message == NULL);

	CuAssert(&tc2, "test 2", 0);
	CuAssertTrue(tc, tc2.failed);
	CuAssertStrEquals(tc, "test 2", tc2.message);

	CuAssert(&tc2, "test 3", 1);
	CuAssertTrue(tc, tc2.failed);
	CuAssertStrEquals(tc, "test 2", tc2.message);

	CuAssert(&tc2, "test 4", 0);
	CuAssertTrue(tc, tc2.failed);
	CuAssertStrEquals(tc, "test 4", tc2.message);
}

void TestCuTestRun(CuTest* tc)
{
	CuTest tc2;
	CuTestInit(&tc2, "MyTest", TestFails);
	CuTestRun(&tc2);
	
	CuAssertStrEquals(tc, "MyTest", tc2.name);
	CuAssertTrue(tc, tc2.failed);
	CuAssertTrue(tc, tc2.ran);
	CuAssertStrEquals(tc, "test should fail", tc2.message);
}

/*-------------------------------------------------------------------------*
 * CuSuite Test
 *-------------------------------------------------------------------------*/

void TestCuSuiteInit(CuTest* tc)
{
	CuSuite ts;
	CuSuiteInit(&ts);
	CuAssertTrue(tc, ts.count == 0);
	CuAssertTrue(tc, ts.failCount == 0);
}

void TestCuSuiteNew(CuTest* tc)
{
	CuSuite* ts = CuSuiteNew();
	CuAssertTrue(tc, ts->count == 0);
	CuAssertTrue(tc, ts->failCount == 0);
}

void TestCuSuiteAddTest(CuTest* tc)
{
	CuSuite ts;
	CuTest tc2;

	CuSuiteInit(&ts);
	CuTestInit(&tc2, "MyTest", TestFails);

	CuSuiteAdd(&ts, &tc2);
	CuAssertTrue(tc, ts.count == 1);
	
	CuAssertStrEquals(tc, "MyTest", ts.list[0]->name);
}

void TestCuSuiteAddSuite(CuTest* tc)
{
	CuSuite* ts1 = CuSuiteNew();
	CuSuite* ts2 = CuSuiteNew();

	CuSuiteAdd(ts1, CuTestNew("TestFails1", TestFails));
	CuSuiteAdd(ts1, CuTestNew("TestFails2", TestFails));

	CuSuiteAdd(ts2, CuTestNew("TestFails3", TestFails));
	CuSuiteAdd(ts2, CuTestNew("TestFails4", TestFails));

	CuSuiteAddSuite(ts1, ts2);
	CuAssertIntEquals(tc, 4, ts1->count);

	CuAssertStrEquals(tc, "TestFails1", ts1->list[0]->name);
	CuAssertStrEquals(tc, "TestFails2", ts1->list[1]->name);
	CuAssertStrEquals(tc, "TestFails3", ts1->list[2]->name);
	CuAssertStrEquals(tc, "TestFails4", ts1->list[3]->name);
}

void TestCuSuiteRun(CuTest* tc)
{
	CuSuite ts;
	CuTest tc1, tc2, tc3, tc4;

	CuSuiteInit(&ts);
	CuTestInit(&tc1, "TestPasses", TestPasses);
	CuTestInit(&tc2, "TestPasses", TestPasses);
	CuTestInit(&tc3, "TestFails",  TestFails);
	CuTestInit(&tc4, "TestFails",  TestFails);

	CuSuiteAdd(&ts, &tc1);
	CuSuiteAdd(&ts, &tc2);
	CuSuiteAdd(&ts, &tc3);
	CuSuiteAdd(&ts, &tc4);
	CuAssertTrue(tc, ts.count == 4);

	CuSuiteRun(&ts);
	CuAssertTrue(tc, ts.count - ts.failCount == 2);
	CuAssertTrue(tc, ts.failCount == 2);
}

void TestCuSuiteSummary(CuTest* tc)
{
	CuSuite ts;
	CuTest tc1, tc2;
	CuString summary;

	CuSuiteInit(&ts);
	CuTestInit(&tc1, "TestPasses", TestPasses);
	CuTestInit(&tc2, "TestFails",  TestFails);
	CuStringInit(&summary);

	CuSuiteAdd(&ts, &tc1);
	CuSuiteAdd(&ts, &tc2);
	CuSuiteRun(&ts);

	CuSuiteSummary(&ts, &summary);

	CuAssertTrue(tc, ts.count == 2);
	CuAssertTrue(tc, ts.failCount == 1);
	CuAssertStrEquals(tc, ".F\n\n", summary.buffer);
}


void TestCuSuiteDetails_SingleFail(CuTest* tc)
{
	CuSuite ts;
	CuTest tc1, tc2;
	CuString details;
	char* expected;

	CuSuiteInit(&ts);
	CuTestInit(&tc1, "TestPasses", TestPasses);
	CuTestInit(&tc2, "TestFails",  TestFails);
	CuStringInit(&details);

	CuSuiteAdd(&ts, &tc1);
	CuSuiteAdd(&ts, &tc2);
	CuSuiteRun(&ts);

	CuSuiteDetails(&ts, &details);

	CuAssertTrue(tc, ts.count == 2);
	CuAssertTrue(tc, ts.failCount == 1);

	expected = 
		"There was 1 failure:\n"
		"1) TestFails: test should fail\n"
		"\n!!!FAILURES!!!\n"
		"Runs: 2 Passes: 1 Fails: 1\n";

	CuAssertStrEquals(tc, expected, details.buffer);
}


void TestCuSuiteDetails_SinglePass(CuTest* tc)
{
	CuSuite ts;
	CuTest tc1;
	CuString details;
	char* expected;

	CuSuiteInit(&ts);
	CuTestInit(&tc1, "TestPasses", TestPasses);
	CuStringInit(&details);

	CuSuiteAdd(&ts, &tc1);
	CuSuiteRun(&ts);

	CuSuiteDetails(&ts, &details);

	CuAssertTrue(tc, ts.count == 1);
	CuAssertTrue(tc, ts.failCount == 0);

	expected = 
		"OK (1 test)\n";

	CuAssertStrEquals(tc, expected, details.buffer);
}

void TestCuSuiteDetails_MultiplePasses(CuTest* tc)
{
	CuSuite ts;
	CuTest tc1, tc2;
	CuString details;
	char* expected;

	CuSuiteInit(&ts);
	CuTestInit(&tc1, "TestPasses", TestPasses);
	CuTestInit(&tc2, "TestPasses", TestPasses);
	CuStringInit(&details);

	CuSuiteAdd(&ts, &tc1);
	CuSuiteAdd(&ts, &tc2);
	CuSuiteRun(&ts);

	CuSuiteDetails(&ts, &details);

	CuAssertTrue(tc, ts.count == 2);
	CuAssertTrue(tc, ts.failCount == 0);

	expected = 
		"OK (2 tests)\n";

	CuAssertStrEquals(tc, expected, details.buffer);
}

void TestCuSuiteDetails_MultipleFails(CuTest* tc)
{
	CuSuite ts;
	CuTest tc1, tc2;
	CuString details;
	char* expected;

	CuSuiteInit(&ts);
	CuTestInit(&tc1, "TestFails1", TestFails);
	CuTestInit(&tc2, "TestFails2", TestFails);
	CuStringInit(&details);

	CuSuiteAdd(&ts, &tc1);
	CuSuiteAdd(&ts, &tc2);
	CuSuiteRun(&ts);

	CuSuiteDetails(&ts, &details);

	CuAssertTrue(tc, ts.count == 2);
	CuAssertTrue(tc, ts.failCount == 2);

	expected = 
		"There were 2 failures:\n"
		"1) TestFails1: test should fail\n"
		"2) TestFails2: test should fail\n"
		"\n!!!FAILURES!!!\n"
		"Runs: 2 Passes: 0 Fails: 2\n";

	CuAssertStrEquals(tc, expected, details.buffer);
}


/*-------------------------------------------------------------------------*
 * Misc Test
 *-------------------------------------------------------------------------*/

void TestCuStrCopy(CuTest* tc)
{
	char* old = "hello world";
	char* new = CuStrCopy(old);
	CuAssert(tc, "old is new", strcmp(old, new) == 0);
}


void TestCuStringAppendFormat(CuTest* tc)
{
	int i;
	char* text = CuStrAlloc(301);		/* long string */
	CuString* str = CuStringNew();
	for (i = 0 ; i < 300 ; ++i)
		text[i] = 'a';
	text[300] = '\0';
	CuStringAppendFormat(str, "%s", text);

	/* buffer limit raised to HUGE_STRING_LEN so no overflow */

	CuAssert(tc, "length of str->buffer is 300", 300 == strlen(str->buffer));
}

void TestFail(CuTest* tc)
{
	jmp_buf buf;
	int pointReached = 0;
	CuTest* tc2 = CuTestNew("TestFails", TestFails);
	tc2->jumpBuf = &buf;
	if (setjmp(buf) == 0)
	{
		CuFail(tc2, "hello world");
		pointReached = 1;
	}
	CuAssert(tc, "point was not reached", pointReached == 0);
}

void TestAssertStrEquals(CuTest* tc)
{
	jmp_buf buf;
	CuTest* tc2 = CuTestNew("TestFails", TestFails);
	char* expected = "expected <hello> but was <world>";
	tc2->jumpBuf = &buf;
	if (setjmp(buf) == 0)
	{
		CuAssertStrEquals(tc2, "hello", "world");
	}
	CuAssert(tc, "CuAssertStrEquals works", strcmp(expected, tc2->message) == 0);
}

void TestAssertIntEquals(CuTest* tc)
{
	jmp_buf buf;
	CuTest* tc2 = CuTestNew("TestFails", TestFails);
	char* expected = "expected <42> but was <32>";
	tc2->jumpBuf = &buf;
	if (setjmp(buf) == 0)
	{
		CuAssertIntEquals(tc2, 42, 32);
	}
	CuAssert(tc, "CuAssertStrEquals works", strcmp(expected, tc2->message) == 0);
}

/*-------------------------------------------------------------------------*
 * main
 *-------------------------------------------------------------------------*/

CuSuite* CuGetSuite(void)
{
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestCuStringAppendFormat);
	SUITE_ADD_TEST(suite, TestCuStrCopy);
	SUITE_ADD_TEST(suite, TestFail);
	SUITE_ADD_TEST(suite, TestAssertStrEquals);
	SUITE_ADD_TEST(suite, TestAssertIntEquals);

	SUITE_ADD_TEST(suite, TestCuTestNew);
	SUITE_ADD_TEST(suite, TestCuTestInit);
	SUITE_ADD_TEST(suite, TestCuAssert);
	SUITE_ADD_TEST(suite, TestCuTestRun);

	SUITE_ADD_TEST(suite, TestCuSuiteInit);
	SUITE_ADD_TEST(suite, TestCuSuiteNew);
	SUITE_ADD_TEST(suite, TestCuSuiteAddTest);
	SUITE_ADD_TEST(suite, TestCuSuiteAddSuite);
	SUITE_ADD_TEST(suite, TestCuSuiteRun);
	SUITE_ADD_TEST(suite, TestCuSuiteSummary);
	SUITE_ADD_TEST(suite, TestCuSuiteDetails_SingleFail);
	SUITE_ADD_TEST(suite, TestCuSuiteDetails_SinglePass);
	SUITE_ADD_TEST(suite, TestCuSuiteDetails_MultiplePasses);
	SUITE_ADD_TEST(suite, TestCuSuiteDetails_MultipleFails);

	return suite;
}
