# $Id: Makefile,v 1.3 1999/06/05 23:29:16 greear Exp $
# $Revision: 1.3 $  $Author: greear $ $Date: 1999/06/05 23:29:16 $

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
	g++ -g -o test1 test1.cc


clean:
	rm -f test1
	rm -f *~
	rm -f ${PROG_HOME}/include/LogStream.h