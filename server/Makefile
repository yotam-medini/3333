# -*- Makefile -*-

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

CPPS := $(wildcard *.cc)
OBJS = $(patsubst %.cc, obj.d/%.o, ${CPPS})
DEPS = $(patsubst %.cc, obj.d/%.d, ${CPPS})
DBGFLAGS = -g
CFLAGS = ${DBGFLAGS} -Wall -std=c++20

LDFLAGS =
LIBS = -lboost_program_options -lfmt

objs: ${OBJS}

obj.d/%.o: %.cc
	@mkdir -p ${@D}
	g++ ${CFLAGS} -MMD -c -o $@ $<

3333-server: ${OBJS}
	g++ ${DBGFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}
	ls -ld $@

./test-card: card.cc card.h
	g++ -DTEST_CARD=1 ${CFLAGS} card.cc -lfmt -o $@

run-test-card: ./test-card
	./test-card
clean:
	rm -f 3333-server
	rm -rf obj.d

-include ${DEPS}
