

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
	cp -f ${INC_FILES} ../../include/

clean:
	rm -f *.o
	rm -f ../../include/PtrArray.h
	rm -f ../../include/PtrArray.cc
	rm -f ../../include/ProtectedList.h
	rm -f ../../include/list2.h
	rm -f ../../include/rb_tree.h
	rm -f ../../include/rb_tree.cc
	rm -f ../../include/tree2.h
	rm -f ../../include/KVPair.h

purge:
	rm -f *~
	rm -f *.flc