# $Id$
# $Revision$  $Author$ $Date$

include ../../MakeInclude

ARCHIVE      = $(PROG_HOME)/lib/libbitfield.a

# ADD YOUR .o FILE to those listed here and
# then add a target/dependency for it at the
# end of the file.
#
# -----------------------

LIBOBJS      = bitfield.o


#CCFLAGS= -I/home/greear/mud/include

# Example of a library that is built
#  directly in the lib directory
#-----------------------

all: ${ARCHIVE} ${PROG_HOME}/include/bitfield.h

${ARCHIVE}:     $(LIBOBJS)
	@echo " "
	@echo "Making Archive Library $@ ..."
	rm -f $@
	$(AR) $(ARFLAGS) $@ $^ 
	@echo "done"        #public_include.

${PROG_HOME}/include/bitfield.h: bitfield.h
	cp $< $@

.PHONY: clean
clean:
	@echo "Cleaning"
	@rm -f *.o
	@rm -f *~
	@rm -f ${ARCHIVE}
	@rm -f ${PROG_HOME}/include/bitfield.h
	@rm -f *.P

%.o : %.cc
	@echo "Compiling $<"
	@$(CCC) $(CCFLAGS) -c -MD -o $@ $<
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		    -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
		rm -f $*.d

-include $(OBJS:%.o=%.P)
