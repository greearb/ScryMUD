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
	cp bitfield.h ${PROG_HOME}/include
	@echo "done"        #public_include.

${PROG_HOME}/include/bitfield.h:
	cp bitfield.h ${PROG_HOME}/include/bitfield.h

bitfield.o:   bitfield.cc bitfield.h
	@echo " "
	@echo "Making $@..."
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o
	rm -f *~
	rm -f ${ARCHIVE}
	rm -f ${PROG_HOME}/include/bitfield.h