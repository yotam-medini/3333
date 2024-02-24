# -*- Makefile -*-

ifneq ($(emv),)
emv:
	@echo $($(emv))
endif

.DELETE_ON_ERROR:

CPPS := $(wildcard *.cc)
OBJS = $(patsubst %.cc, obj.d/%.o, ${CPPS})
DBGFLAGS = -g
LDFLAGS =
LIBS = -lboost_program_options

objs: ${OBJS}

obj.d/%.o: %.cc
	mkdir -p ${@D}
	g++ ${DBGFLAGS} -c -o $@ $<

3333-server: ${OBJS}
	g++ ${DBGFLAGS} ${LDFLAGS} -o $@ ${OBJS} ${LIBS}
	ls -ld $@

clean:
	rm -f 3333-server obj.d
