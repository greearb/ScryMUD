# $Id: Makefile,v 1.8 2006/04/06 06:10:08 greear Exp $
# $Revision: 1.8 $  $Author: greear $ $Date: 2006/04/06 06:10:08 $

include ../../MakeInclude

ARCHIVE      = $(PROG_HOME)/lib/libstring2.a

#LDLIBS =  -lm -lstring2 -lbitfield -lgcont -lstdc++ -lgcc -lgcc_eh #-lnsl # -lsocket

ifeq "${WIN32}" "1"
LDLIBS += -lws2_32
endif

ifeq "${PLATFORM}" "SOLARIS"
BASIC_LDLIBS += -lsocket -lnsl
endif

# ADD YOUR .o FILE to those listed here and
# then add a target/dependency for it at the
# end of the file.
#
# -----------------------

LIBOBJS      = string2.o


TESTER = tester

# Example of a library that is built
#  directly in the lib directory
#-----------------------

all: ${ARCHIVE} ${PROG_HOME}/include/string2.h ${TESTER}

${TESTER}: ${PROG_HOME}/lib/libbitfield.a ${PROG_HOME}/lib/libgcont.a ${LIBOBJS} tester.o
	$(CCC) $(CCFLAGS) $(LDFLAGS) -o ${TESTER} $(LIBOBJS) tester.o $(BASIC_LDLIBS)
	-./${TESTER}

${PROG_HOME}/lib/libbitfield.a:
	${MAKE} -C ../bitfield

${PROG_HOME}/lib/libgcont.a:
	${MAKE} -C ../containers

${ARCHIVE}:     $(LIBOBJS) Makefile
	@echo " "
	@echo "Making Archive Library $@ ..."
	rm -f $@
	$(AR) $(ARFLAGS) $@ $(LIBOBJS)
	@echo "done"        #public_include.


${PROG_HOME}/include/string2.h: string2.h
	cp string2.h ${PROG_HOME}/include/string2.h

string2.o:   string2.cc string2.h
	@echo " "
	@echo "Making $@..."
	$(CCC) $(CCFLAGS) -c $<

tester.o: tester.cc
	@echo " "
	@echo "Making $@..."
	$(CCC) $(CCFLAGS) -c $<

clean:
	rm -f ${PROG_HOME}/include/string2.h
	rm -f *.o
	rm -f $(ARCHIVE)
	rm -f ${TESTER}
	rm -f *~

purge: clean
