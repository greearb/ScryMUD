# $Id: Makefile,v 1.22 1999/08/22 07:16:19 greear Exp $
# $Revision: 1.22 $  $Author: greear $ $Date: 1999/08/22 07:16:19 $

# makefile template
include ../../MakeInclude


CODE_GEN_SRCS = code_gen.cc
CODE_GEN_OBJS = code_gen.o
CODE_GEN_TARG = ./code_gen

AUDIT_SRCS = audit.cc
AUDIT_OBJS = audit.o
AUDIT_TARG = ./audit

GEN_TARGS = lang_strings.h parse_gen.h lang_strings.cc parse_gen.cc

SRCS  = parse_gen.cc lang_strings.cc classes.cc object.cc critter.cc \
Filters.cc room.cc door.cc skills.cc command3.cc Markup.cc \
grrmud.cc commands.cc command2.cc socials.cc spells.cc \
olc.cc load_wld.cc misc.cc misc2.cc parse.cc battle.cc spec_prc.cc \
login.cc command4.cc olc2.cc command5.cc batl_prc.cc spells2.cc \
ez_spll.cc dam_spll.cc trv_spll.cc rm_spll.cc dam_skll.cc wep_skll.cc \
ez_skll.cc social2.cc cr_skll.cc ar_skll.cc pet_spll.cc vehicle.cc \
script.cc SkillSpell.cc zone.cc rm_parse.cc rm_cmds.cc obj_parse.cc \
obj_cmds.cc BugEntry.cc MudStats.cc Serialized.cc Scriptable.cc

OBJS = parse_gen.o lang_strings.o classes.o object.o critter.o \
Filters.o room.o door.o skills.o command3.o Markup.o \
grrmud.o commands.o command2.o socials.o spells.o olc.o \
load_wld.o misc.o misc2.o parse.o battle.o spec_prc.o login.o \
command4.o olc2.o command5.o batl_prc.o spells2.o \
ez_spll.o dam_spll.o trv_spll.o rm_spll.o dam_skll.o wep_skll.o \
ez_skll.o social2.o cr_skll.o ar_skll.o pet_spll.o vehicle.o const.o \
script.o SkillSpell.o zone.o rm_parse.o rm_cmds.o obj_parse.o \
obj_cmds.o BugEntry.o MudStats.o Serialized.o \
Scriptable.o BuildInfo.o

GEN_SPEC = gen_cmds.spec
LANG_SPEC = translations.spec translation2.spec translation_olc.spec \
translations_classes.spec translation_misc.spec translation_spell.spec \
translation_battle.spec translation4.spec


TARG = grrmud                                 #program to be created

all: ${TARG} ${AUDIT_TARG}

${TARG}: libs ${GEN_TARGS} $(OBJS) 
	$(CCC) $(CCFLAGS) $(LDFLAGS) -o ${TARG} $(OBJS) $(LDLIBS)

# This should really depend on libs too, but my libs make is lazy,
# and that causes this to be rebuilt each time.  By making TARG,
# as above, I think libs will be up to date, but you may have to
# specifically re-build the CODE_GEN_TARG if changes to the libs
# would have affected it. --BEN
${CODE_GEN_TARG}: ${CODE_GEN_OBJS}
	$(CCC) $(CCFLAGS) $(LDFLAGS) -o ${CODE_GEN_TARG} $(CODE_GEN_OBJS) $(LDLIBS)

${AUDIT_TARG}: libs ${AUDIT_OBJS}
	$(CCC) $(CCFLAGS) $(LDFLAGS) -o ${AUDIT_TARG} $(AUDIT_OBJS) $(LDLIBS)
	cp ${AUDIT_TARG} ${TOOL_BIN_DIR}/

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

$(AUDIT_OBJS): %.o: %.cc %.h
	@echo " "
	@echo "Making $<"
	$(CC) $(CCFLAGS) -c $<

libs:
	${MAKE} -C ${MUD_HOME}/lib all


depend:
	makedepend $(SRCS)

clean:
	rm -f *.o ${GEN_TARGS} BuildInfo.cc parse_gen.cc parse_gen.h
	rm -f lang_strings.h lang_strings.cc
	${MAKE} -C ../../lib clean

purge: clean
	rm -f *~
	rm -f *.flc
	rm -f ${TARG} ${CODE_GEN_TARG} ${AUDIT_TARG}


#DO NOT DELETE THIS LINE -- make depend depends on it.

# DO NOT DELETE






