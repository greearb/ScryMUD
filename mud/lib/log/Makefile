# $Id: Makefile,v 1.4 2001/03/29 03:02:38 eroper Exp $
# $Revision: 1.4 $  $Author: eroper $ $Date$

#ARCHIVE      = /home/greear/mud/lib/liblog.a

# ADD YOUR .o FILE to those listed here and
# then add a target/dependency for it at the
# end of the file.
#
# -----------------------

#LIBOBJS      = string2.o

# Example of a library that is built
#  directly in the lib directory
#-----------------------

#$(ARCHIVE):     $(LIBOBJS)

include ../../MakeInclude

all: test1
	@echo " "
	cp LogStream.h ${PROG_HOME}/include

test1:
	g++ -g -fhuge-objects -o test1 test1.cc


clean:
	rm -f test1
	rm -f *~
	rm -f ${PROG_HOME}/include/LogStream.h