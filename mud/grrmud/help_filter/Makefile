# makefile template

include ../../MakeInclude


LDLIBS =  -lm -lstring2
 
HTML_OBJS = help2html.o
IDX_OBJS  = idx_bldr.o

HELP2HTML = help2html                                 #program to be created
IDX_BLDR = idx_bldr

all: ${HELP2HTML} ${IDX_BLDR}

 
${HELP2HTML}: $(HTML_OBJS)
	$(CCC) $(CCFLAGS) $(LDFLAGS) -o $(HELP2HTML) $(HTML_OBJS) $(LDLIBS)
	cp ${HELP2HTML} $(TOOL_BIN_DIR)

${IDX_BLDR}: $(IDX_OBJS)
	$(CCC) $(CCFLAGS) $(LDFLAGS) -o $(IDX_BLDR) $(IDX_OBJS) $(LDLIBS)
	cp ${IDX_BLDR} $(TOOL_BIN_DIR)


$(HTML_OBJS): %.o: %.C %.h libs
	@echo " "
	@echo "Making $<"
	$(CC) $(CCFLAGS) \
	-c $<

$(IDX_OBJS): %.o: %.C %.h libs
	@echo " "
	@echo "Making $<"
	$(CC) $(CCFLAGS) \
	-c $<

libs:
	${MAKE} -C ${MUD_HOME}/lib all

clean:
	rm -f *.o

purge: clean
	rm -f *.flc
	rm -f *~





