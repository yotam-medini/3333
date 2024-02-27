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
LIBS = -lboost_program_options

objs: ${OBJS}

obj.d/%.o: %.cc
	mkdir -p ${@D}
	g++ ${CFLAGS} -MMD -c -o $@ $<

3333-server: ${OBJS}
	g++ ${DBGFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}
	ls -ld $@

clean:
	rm -f 3333-server
	rm -rf obj.d

-include ${DEPS}
