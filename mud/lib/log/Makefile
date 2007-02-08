# $Id$
# $Revision$  $Author$ $Date$

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

.PHONY: all
all: test1 ${PROG_HOME}/include/LogStream.h

${PROG_HOME}/include/LogStream.h: LogStream.h
	cp $< $@

test1:
	@echo "Compiling $@"
	@$(CCC) $(CCFLAGS)  -o $@ test1.cc

clean:
	rm -f test1
	rm -f *~
	rm -f ${PROG_HOME}/include/LogStream.h
	rm -f *.P
