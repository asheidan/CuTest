
CUTEST_TESTBIN := CuTest_test


CFLAGS += -Wall -pedantic --std=c99

all: $(CUTEST_TESTBIN)

run_test: $(CUTEST_TESTBIN)
	./$(CUTEST_TESTBIN)

$(CUTEST_TESTBIN): CuTest.o CuTestTest.o AllTests.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

CuTest.o: CuTest.c CuTest.h

clean:
	rm -f *.o $(CUTEST_TESTBIN)
