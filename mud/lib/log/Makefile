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
all: test1
	@echo " "
	cp LogStream.h ../../include

test1:
	g++ -g -o test1 test1.cc


clean:
	rm -f test1
	rm -f *~
	rm -f ../../include/LogStream.h