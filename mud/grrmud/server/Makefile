# $Id$
# $Revision$  $Author$ $Date$

# makefile template
include ../../MakeInclude

generated_TARGS = lang_strings.h parse_gen.h lang_strings.cc parse_gen.cc

code_gen_OBJS = code_gen.o
code_gen_TARG = code_gen

audit_OBJS = audit.o
audit_TARG = audit

grrmud_OBJS = parse_gen.o lang_strings.o classes.o object.o critter.o \
Filters.o room.o door.o skills.o command3.o \
grrmud.o commands.o command2.o socials.o spells.o olc.o \
load_wld.o misc.o misc2.o parse.o battle.o spec_prc.o login.o \
command4.o olc2.o command5.o batl_prc.o spells2.o \
ez_spll.o dam_spll.o trv_spll.o rm_spll.o dam_skll.o wep_skll.o \
ez_skll.o social2.o cr_skll.o ar_skll.o pet_spll.o vehicle.o const.o \
script.o SkillSpell.o zone.o rm_parse.o rm_cmds.o obj_parse.o \
obj_cmds.o BuildInfo.o BugEntry.o MudStats.o clients.o ServerConfig.o \
mapper.o regex.o protocol_handler.o telnet_handler.o hegemon_handler.o
grrmud_TARG = grrmud

GEN_SPEC = gen_cmds.spec
LANG_SPEC = translations.spec translations_classes.spec translation2.spec \
translation4.spec translation_olc.spec translation_spell.spec \
translation_battle.spec translation_skill.spec translation5.spec

${grrmud_TARG}: libs ${generated_TARGS} $(grrmud_OBJS) 
	@echo Linking $@
	@$(CCC) $(CCFLAGS) $(LDFLAGS) -o ${grrmud_TARG} $(grrmud_OBJS) $(LDLIBS)
	@rm -f Subversion-Revision 

# TODO (eroper): I haven't tracked down why this note is here yet
# This should really depend on libs too, but my libs make is lazy,
# and that causes this to be rebuilt each time.  By making TARG,
# as above, I think libs will be up to date, but you may have to
# specifically re-build the code_gen_TARG if changes to the libs
# would have affected it. --BEN
${code_gen_TARG}: ${code_gen_OBJS}
	@echo Linking $@
	@$(CCC) $(CCFLAGS) $(LDFLAGS) -o ${code_gen_TARG} $(code_gen_OBJS) $(LDLIBS)

${audit_TARG}: libs ${audit_OBJS}
	@echo Linking $@
	@$(CCC) $(CCFLAGS) $(LDFLAGS) -o ${audit_TARG} $(audit_OBJS) $(LDLIBS)
# this should go in an install target
#@cp ${audit_TARG} ${TOOL_BIN_DIR}/

Subversion-Revision: 
	@echo Generating $@
	@svn info |grep Revision |awk '{print $$2}' >Subversion-Revision

BuildInfo.cc: $(SRCS) Makefile BuildInfo.prefix Subversion-Revision
	@echo "Generating $@"
	@cat BuildInfo.prefix > BuildInfo.cc
	@echo "const char* BuildInfo::getBuildMachine() { return \"`uname -mrspn`\"; }" >> BuildInfo.cc
	@echo "" >> BuildInfo.cc
	@echo "const char* BuildInfo::getBuilder() { return \"`whoami`\"; }" >> BuildInfo.cc
	@echo "" >> BuildInfo.cc
	@echo "const char* BuildInfo::getBuildDate() { return \"`date`\"; }" >> BuildInfo.cc
	@echo "" >> BuildInfo.cc
	@echo "const char* BuildInfo::getVersion() { return \"${SCRY_VERSION}\"; }" >> BuildInfo.cc
	@echo "" >> BuildInfo.cc
	@echo "const char* BuildInfo::getRevision() { return \"`cat Subversion-Revision`\"; }" >> BuildInfo.cc

parse_gen.cc: ${code_gen_TARG} ${GEN_SPEC}
parse_gen.h: ${code_gen_TARG} ${GEN_SPEC}
	@echo Generating $@
	@./${code_gen_TARG} -C ${GEN_SPEC} parse_gen.cc parse_gen.h


lang_strings.cc: ${code_gen_TARG} ${LANG_SPEC}
lang_strings.h: ${code_gen_TARG} ${LANG_SPEC}
	@echo Generating $@
	@./${code_gen_TARG} -L ${LANG_SPEC} lang_strings.cc lang_strings.h

%.o : %.cc
	@echo "Compiling $<"
	@$(CCC) $(CCFLAGS) -c -MD -o $@ $<
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
		    -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
		rm -f $*.d

-include $(OBJS:%.o=%.P)

.PHONY: libs
libs:
	${MAKE} -C ${MUD_HOME}/lib all

.PHONY: clean
clean:
	@echo "Cleaning"
	@rm -f *.o ${generated_TARGS} BuildInfo.cc parse_gen.cc parse_gen.h
	@rm -f lang_strings.h lang_strings.cc
	@rm -f Subversion-Revision
	@rm -f *.P
	@echo "Cleaning ../../lib"
	@${MAKE} -C ../../lib clean

.PHONY: purge
purge: clean
	@rm -f *~
	@rm -f *.flc
	@rm -f ${grrmud_TARG} ${code_gen_TARG} ${audit_TARG}

.PHONY: all
all: ${grrmud_TARG} ${audit_TARG}

.PHONY: new_binary
new_binary: ${grrmud_TARG}
	# again, maybe an install target
	#@rm -f ${MUD_HOME}/grrmud/gmud.new
	#@cp ${grrmud_TARG} ${MUD_HOME}/grrmud/gmud.new

