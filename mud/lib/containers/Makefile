include ../../MakeInclude

INC_FILES = \
	KVPair.h \
	PtrArray.h \
	PtrArray.cc \
	ProtectedList.h \
	list2.h \
	rb_tree.h \
	rb_tree.cc \
	tree2.h



all: ${INC_FILES}
	cp -f ${INC_FILES} ${PROG_HOME}/include/

clean:
	rm -f *.o
	rm -f ${PROG_HOME}/include/PtrArray.h
	rm -f ${PROG_HOME}/include/PtrArray.cc
	rm -f ${PROG_HOME}/include/ProtectedList.h
	rm -f ${PROG_HOME}/include/list2.h
	rm -f ${PROG_HOME}/include/rb_tree.h
	rm -f ${PROG_HOME}/include/rb_tree.cc
	rm -f ${PROG_HOME}/include/tree2.h
	rm -f ${PROG_HOME}/include/KVPair.h

purge: clean
	rm -f *~
	rm -f *.flc