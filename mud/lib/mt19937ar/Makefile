include ../../MakeInclude

ARCHIVE = $(PROG_HOME)/lib/libmt19937ar.a
LIBOBJS = mt19937ar.o

.PHONY: all

all: ${ARCHIVE} ${PROG_HOME}/include/mt19937ar.h

${ARCHIVE}: $(LIBOBJS)
	@echo -n "Making Archive Library $@... "
	@rm -f $@
	@$(AR) $(ARFLAGS) $@ $^
	@echo "done."

${PROG_HOME}/include/mt19937ar.h: mt19937ar.h
	@cp mt19937ar.h ${PROG_HOME}/include/mt19937ar.h

.PHONY: clean
clean:
	@echo -n "Cleaning... "
	@rm -f *.o
	@rm -f *.P
	@rm -f $(ARCHIVE)
	@echo "done."

%.o : %.c
	@echo -n "Compiling $<... "
	@$(CC) $(CCFLAGS) -c -MD -o $@ $<
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		    -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
		rm -f $*.d
	@echo "done."

-include $(OBJS:%.o=%.P)
