# $Id: Makefile,v 1.3 1999/06/05 23:29:16 greear Exp $
# $Revision: 1.3 $  $Author: greear $ $Date$

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

all: ${ARCHIVE} ${PROG_HOME}/include/string2.h

${ARCHIVE}:     $(LIBOBJS)
	@echo " "
	@echo "Making Archive Library $@ ..."
	rm -f $@
	$(AR) $(ARFLAGS) $@ $^ 
	cp string2.h ${PROG_HOME}/include
	@echo "done"        #public_include.


${PROG_HOME}/include/string2.h:
	cp string2.h ${PROG_HOME}/include/string2.h

string2.o:   string2.cc string2.h
	@echo " "
	@echo "Making $@..."
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f ${PROG_HOME}/include/string2.h
	rm -f *.o
	rm -f $(ARCHIVE)
	rm -f *~
