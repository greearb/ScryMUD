# makefile template
include ../../MakeInclude


CODE_GEN_SRCS = code_gen.cc
CODE_GEN_OBJS = code_gen.o
CODE_GEN_TARG = ./code_gen

GEN_TARGS = lang_strings.h parse_gen.h lang_strings.cc parse_gen.cc

SRCS  = parse_gen.cc lang_strings.cc classes.cc object.cc critter.cc \
room.cc door.cc skills.cc command3.cc \
grrmud.cc commands.cc command2.cc socials.cc spells.cc \
olc.cc load_wld.cc misc.cc misc2.cc parse.cc battle.cc spec_prc.cc \
login.cc command4.cc olc2.cc command5.cc batl_prc.cc spells2.cc \
ez_spll.cc dam_spll.cc trv_spll.cc rm_spll.cc dam_skll.cc wep_skll.cc \
ez_skll.cc social2.cc cr_skll.cc ar_skll.cc pet_spll.cc vehicle.cc \
script.cc SkillSpell.cc zone.cc rm_parse.cc rm_cmds.cc obj_parse.cc \
obj_cmds.cc BugEntry.cc MudStats.cc

OBJS = parse_gen.o lang_strings.o classes.o object.o critter.o \
room.o door.o skills.o command3.o \
grrmud.o commands.o command2.o socials.o spells.o olc.o \
load_wld.o misc.o misc2.o parse.o battle.o spec_prc.o login.o \
command4.o olc2.o command5.o batl_prc.o spells2.o \
ez_spll.o dam_spll.o trv_spll.o rm_spll.o dam_skll.o wep_skll.o \
ez_skll.o social2.o cr_skll.o ar_skll.o pet_spll.o vehicle.o const.o \
script.o SkillSpell.o zone.o rm_parse.o rm_cmds.o obj_parse.o \
obj_cmds.o BuildInfo.o BugEntry.o MudStats.o

GEN_SPEC = gen_cmds.spec
LANG_SPEC = translations.spec

TARG = grrmud                                 #program to be created

all: ${TARG}

${TARG}: libs ${GEN_TARGS} $(OBJS) 
	$(CCC) $(CCFLAGS) $(LDFLAGS) -o ${TARG} $(OBJS) $(LDLIBS)

# This should really depend on libs too, but my libs make is lazy,
# and that causes this to be rebuilt each time.  By making TARG,
# as above, I think libs will be up to date, but you may have to
# specifically re-build the CODE_GEN_TARG if changes to the libs
# would have affected it. --BEN
${CODE_GEN_TARG}: ${CODE_GEN_OBJS}
	$(CCC) $(CCFLAGS) $(LDFLAGS) -o ${CODE_GEN_TARG} $(CODE_GEN_OBJS) $(LDLIBS)

new_binary: ${TARG}
	rm -f ${MUD_HOME}/grrmud/gmud.new
	cp ${TARG} ${MUD_HOME}/grrmud/gmud.new

$(OBJS): %.o: %.cc %.h
	@echo " "
	@echo "Making $<"
	$(CC) $(CCFLAGS) \
	-c $<

BuildInfo.cc: $(SRCS) Makefile BuildInfo.prefix
	cat BuildInfo.prefix > BuildInfo.cc
	echo "const char* BuildInfo::getBuildMachine() { return \"`uname -mrspn`\"; }" >> BuildInfo.cc
	echo "" >> BuildInfo.cc
	echo "const char* BuildInfo::getBuilder() { return \"`whoami`\"; }" >> BuildInfo.cc
	echo "" >> BuildInfo.cc
	echo "const char* BuildInfo::getBuildDate() { return \"`date`\"; }" >> BuildInfo.cc
	echo "" >> BuildInfo.cc
	echo "const char* BuildInfo::getVersion() { return \"${SCRY_VERSION}\"; }" >> BuildInfo.cc


parse_gen.cc: ${CODE_GEN_TARG} ${GEN_SPEC}
parse_gen.h: ${CODE_GEN_TARG} ${GEN_SPEC}
	${CODE_GEN_TARG} -C ${GEN_SPEC} parse_gen.cc parse_gen.h


lang_strings.cc: ${CODE_GEN_TARG} ${LANG_SPEC}
lang_strings.h: ${CODE_GEN_TARG} ${LANG_SPEC}
	${CODE_GEN_TARG} -L ${LANG_SPEC} lang_strings.cc lang_strings.h


$(CODE_GEN_OBJS): %.o: %.cc %.h
	@echo " "
	@echo "Making $<"
	$(CC) $(CCFLAGS) -c $<

libs:
	${MAKE} -C ${MUD_HOME}/lib all


depend:
	makedepend $(SRCS)

clean:
	rm -f *.o ${GEN_TARGS} BuildInfo.cc
	${MAKE} -C ../../lib clean

purge: clean
	rm -f *~
	rm -f *.flc
	rm -f ${TARG}


#DO NOT DELETE THIS LINE -- make depend depends on it.

# DO NOT DELETE






