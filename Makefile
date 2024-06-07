
COMMON_CFLAGS=-g -Wall -lm  -Dgprintf_glibc_compat

testp: testp.c
	gcc -o $@ $^ $(COMMON_CFLAGS) 

testp-coverage: testp.c
	gcc -o $@ $^ --coverage $(COMMON_CFLAGS) 

testp.c: gp.c selftest.c
	cat $^ > $@

covreport: testp-coverage
	rm -f *.gcda
	./testp-coverage
	geninfo .
	genhtml testp.gcda.info -o report

clean:
	rm -f *.o testp testp-coverage *.info *.gcda *.gcov *.gcno
