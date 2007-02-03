# $Id$
# $Revision$  $Author$ $Date$
#
# TODO: I'm pretty sure there's a more sane way to do this. Re-evaluate when
# I'm not so sleepy.

include ../../MakeInclude

TARGS = ${PROG_HOME}/include/KVPair.h \
		${PROG_HOME}/include/PtrArray.h \
		${PROG_HOME}/include/PtrArray.cc \
		${PROG_HOME}/include/ProtectedList.h \
		${PROG_HOME}/include/list2.h \
		${PROG_HOME}/include/rb_tree.h \
		${PROG_HOME}/include/rb_tree.cc \
		${PROG_HOME}/include/tree2.h \

.PHONY: all
all: $(TARGS)
		
${PROG_HOME}/include/KVPair.h: KVPair.h
	cp $< $@

${PROG_HOME}/include/PtrArray.h: PtrArray.h
	cp $< $@

${PROG_HOME}/include/PtrArray.cc: PtrArray.cc
	cp $< $@

${PROG_HOME}/include/ProtectedList.h: ProtectedList.h
	cp $< $@

${PROG_HOME}/include/list2.h: list2.h
	cp $< $@

${PROG_HOME}/include/rb_tree.h: rb_tree.h
	cp $< $@

${PROG_HOME}/include/rb_tree.cc: rb_tree.cc
	cp $< $@

${PROG_HOME}/include/tree2.h: tree2.h
	cp $< $@

.PHONY: clean
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

.PHONY: purge
purge: clean
	rm -f *~
	rm -f *.flc
