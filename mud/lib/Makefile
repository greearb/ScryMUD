# $Id$
# $Revision$  $Author$ $Date$

include ../MakeInclude

.PHONY: all
all:
	@${MAKE} -C log all
	@${MAKE} -C string2 all
	@${MAKE} -C containers all
	@${MAKE} -C bitfield all
	@${MAKE} -C mt19937ar all

.PHONY: clean
clean:
	@${MAKE} -C containers clean
	@${MAKE} -C bitfield clean
	@${MAKE} -C log clean
	@${MAKE} -C string2 clean
	@${MAKE} -C mt19937ar clean
