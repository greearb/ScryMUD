include ../../MakeInclude

ARCHIVE      = $(MUD_HOME)/lib/libstring2.a

# ADD YOUR .o FILE to those listed here and
# then add a target/dependency for it at the
# end of the file.
#
# -----------------------

LIBOBJS      = string2.o

# Example of a library that is built
#  directly in the lib directory
#-----------------------

all: ${ARCHIVE} ../../include/string2.h

${ARCHIVE}:     $(LIBOBJS)
	@echo " "
	@echo "Making Archive Library $@ ..."
	rm -f $@
	$(AR) $(ARFLAGS) $@ $^ 
	cp string2.h ../../include
	@echo "done"        #public_include.


../../include/string2.h:
	cp string2.h ../../include/string2.h

string2.o:   string2.cc string2.h
	@echo " "
	@echo "Making $@..."
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f ../../include/string2.h
	rm -f *.o
	rm -f $(ARCHIVE)
	rm -f *~
