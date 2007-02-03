# $Id$
# $Revision$  $Author$ $Date$

include ../../MakeInclude

ARCHIVE      = $(PROG_HOME)/lib/libstring2.a

# ADD YOUR .o FILE to those listed here and
# then add a target/dependency for it at the
# end of the file.
#
# -----------------------

LIBOBJS      = string2.o

# Example of a library that is built
#  directly in the lib directory
#-----------------------

.PHONY: all
all: ${ARCHIVE} ${PROG_HOME}/include/string2.h

${ARCHIVE}:     $(LIBOBJS)
	@echo " "
	@echo "Making Archive Library $@ ..."
	rm -f $@
	$(AR) $(ARFLAGS) $@ $^ 
	@echo "done"        #public_include.


${PROG_HOME}/include/string2.h: string2.h
	cp string2.h ${PROG_HOME}/include/string2.h

.PHONY: clean
clean:
	rm -f ${PROG_HOME}/include/string2.h
	rm -f *.o
	rm -f $(ARCHIVE)
	rm -f *~
	rm -f *.P

%.o : %.cc
	@echo "Compiling $<"
	@$(CCC) $(CCFLAGS) -c -MD -o $@ $<
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		    -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
		rm -f $*.d

-include $(OBJS:%.o=%.P)
