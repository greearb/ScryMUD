# $Id$
# $Revision$  $Author$ $Date$

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
Filters.cc room.cc door.cc skills.cc command3.cc \
grrmud.cc commands.cc command2.cc socials.cc spells.cc \
olc.cc load_wld.cc misc.cc misc2.cc parse.cc battle.cc spec_prc.cc \
login.cc command4.cc olc2.cc command5.cc batl_prc.cc spells2.cc \
ez_spll.cc dam_spll.cc trv_spll.cc rm_spll.cc dam_skll.cc wep_skll.cc \
ez_skll.cc social2.cc cr_skll.cc ar_skll.cc pet_spll.cc vehicle.cc \
script.cc SkillSpell.cc zone.cc rm_parse.cc rm_cmds.cc obj_parse.cc \
obj_cmds.cc BugEntry.cc MudStats.cc clients.cc ServerConfig.cc mapper.cc \
regex.cc

OBJS = parse_gen.o lang_strings.o classes.o object.o critter.o \
Filters.o room.o door.o skills.o command3.o \
grrmud.o commands.o command2.o socials.o spells.o olc.o \
load_wld.o misc.o misc2.o parse.o battle.o spec_prc.o login.o \
command4.o olc2.o command5.o batl_prc.o spells2.o \
ez_spll.o dam_spll.o trv_spll.o rm_spll.o dam_skll.o wep_skll.o \
ez_skll.o social2.o cr_skll.o ar_skll.o pet_spll.o vehicle.o const.o \
script.o SkillSpell.o zone.o rm_parse.o rm_cmds.o obj_parse.o \
obj_cmds.o BuildInfo.o BugEntry.o MudStats.o clients.o ServerConfig.o \
mapper.o regex.o

GEN_SPEC = gen_cmds.spec
LANG_SPEC = translations.spec translations_classes.spec translation2.spec \
translation4.spec translation_olc.spec translation_spell.spec \
translation_battle.spec translation_skill.spec translation5.spec

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
	@echo "Making $<"
	@$(CC) $(CCFLAGS) \
	-c $<

Subversion-Revision:
	svn info |grep Revision |awk '{print $$2}' >Subversion-Revision

BuildInfo.cc: $(SRCS) Makefile BuildInfo.prefix Subversion-Revision
	cat BuildInfo.prefix > BuildInfo.cc
	echo "const char* BuildInfo::getBuildMachine() { return \"`uname -mrspn`\"; }" >> BuildInfo.cc
	echo "" >> BuildInfo.cc
	echo "const char* BuildInfo::getBuilder() { return \"`whoami`\"; }" >> BuildInfo.cc
	echo "" >> BuildInfo.cc
	echo "const char* BuildInfo::getBuildDate() { return \"`date`\"; }" >> BuildInfo.cc
	echo "" >> BuildInfo.cc
	echo "const char* BuildInfo::getVersion() { return \"${SCRY_VERSION}\"; }" >> BuildInfo.cc
	echo "" >> BuildInfo.cc
	echo "const char* BuildInfo::getRevision() { return \"`cat Subversion-Revision`\"; }" >> BuildInfo.cc


parse_gen.cc: ${CODE_GEN_TARG} ${GEN_SPEC}
parse_gen.h: ${CODE_GEN_TARG} ${GEN_SPEC}
	@${CODE_GEN_TARG} -C ${GEN_SPEC} parse_gen.cc parse_gen.h


lang_strings.cc: ${CODE_GEN_TARG} ${LANG_SPEC}
lang_strings.h: ${CODE_GEN_TARG} ${LANG_SPEC}
	${CODE_GEN_TARG} -L ${LANG_SPEC} lang_strings.cc lang_strings.h


$(CODE_GEN_OBJS): %.o: %.cc %.h
	@echo "Making $<"
	@$(CC) $(CCFLAGS) -c $<

$(AUDIT_OBJS): %.o: %.cc %.h
	@echo "Making $<"
	@$(CC) $(CCFLAGS) -c $<

libs:
	${MAKE} -C ${MUD_HOME}/lib all


#depend:
#makedepend $(SRCS)

clean:
	rm -f *.o ${GEN_TARGS} BuildInfo.cc parse_gen.cc parse_gen.h
	rm -f lang_strings.h lang_strings.cc
	rm -f Subversion-Revision
	${MAKE} -C ../../lib clean

purge: clean
	rm -f *~
	rm -f *.flc
	rm -f ${TARG} ${CODE_GEN_TARG} ${AUDIT_TARG}


#DO NOT DELETE THIS LINE -- make depend depends on it.

# DO NOT DELETE

BugEntry.o: BugEntry.cc BugEntry.h critter.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h door.h room.h \
  commands.h lang_strings.h battle.h misc2.h
Filters.o: Filters.cc Filters.h critter.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h door.h room.h \
  commands.h lang_strings.h battle.h misc2.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h
MudStats.o: MudStats.cc MudStats.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  critter.h classes.h script.h \
  ${MUD_HOME}/include/KVPair.h object.h door.h room.h \
  commands.h lang_strings.h battle.h
ServerConfig.o: ServerConfig.cc ServerConfig.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h
SkillSpell.o: SkillSpell.cc const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  SkillSpell.h object.h classes.h script.h \
  ${MUD_HOME}/include/KVPair.h critter.h door.h room.h \
  commands.h lang_strings.h battle.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h MudStats.h \
  misc2.h dam_spll.h spells.h
ar_skll.o: ar_skll.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spells.h ar_skll.h skills.h batl_prc.h
audit.o: audit.cc audit.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h
batl_prc.o: batl_prc.cc battle.h classes.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h room.h critter.h \
  object.h door.h lang_strings.h commands.h batl_prc.h command2.h \
  command3.h command4.h command5.h \
  ${MUD_HOME}/include/tree2.h misc.h zone.h BugEntry.h \
  SkillSpell.h MudStats.h misc2.h spells.h trv_spll.h skills.h rm_spll.h \
  spells2.h wep_skll.h dam_spll.h dam_skll.h ez_spll.h socials.h \
  social2.h ar_skll.h
battle.o: battle.cc battle.h classes.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h room.h critter.h \
  object.h door.h lang_strings.h commands.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spec_prc.h command2.h command3.h spells.h spells2.h \
  skills.h load_wld.h Filters.h batl_prc.h clients.h
classes.o: classes.cc classes.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h misc.h object.h \
  critter.h door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h command5.h command4.h BuildInfo.h
clients.o: clients.cc clients.h \
  ${MUD_HOME}/include/LogStream.h
code_gen.o: code_gen.cc ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h code_gen.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.cc
command2.o: command2.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h command2.h \
  command3.h command4.h command5.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spec_prc.h spells.h skills.h parse.h parse_gen.h \
  ar_skll.h cr_skll.h social2.h ez_skll.h dam_skll.h olc.h olc2.h \
  socials.h wep_skll.h load_wld.h clients.h
command3.o: command3.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h command2.h \
  command3.h command4.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spec_prc.h spells.h skills.h command5.h load_wld.h
command4.o: command4.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h command2.h \
  command3.h command4.h command5.h olc2.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spec_prc.h spells.h skills.h load_wld.h vehicle.h \
  regex.h
command5.o: command5.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h command2.h \
  command3.h command4.h command5.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spec_prc.h spells.h skills.h batl_prc.h load_wld.h \
  olc.h BuildInfo.h clients.h vehicle.h
commands.o: commands.cc rm_spll.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  command4.h room.h critter.h object.h door.h lang_strings.h battle.h \
  commands.h dam_spll.h spells.h ez_spll.h trv_spll.h command2.h \
  command5.h misc.h zone.h ${MUD_HOME}/include/tree2.h \
  BugEntry.h SkillSpell.h MudStats.h misc2.h spec_prc.h batl_prc.h \
  spells2.h skills.h pet_spll.h load_wld.h Filters.h clients.h
const.o: const.cc ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  critter.h classes.h script.h \
  ${MUD_HOME}/include/KVPair.h object.h door.h room.h \
  commands.h lang_strings.h battle.h
cr_skll.o: cr_skll.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spells.h skills.h batl_prc.h cr_skll.h load_wld.h
critter.o: critter.cc script.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h ${MUD_HOME}/include/KVPair.h critter.h \
  object.h door.h room.h commands.h lang_strings.h battle.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h command2.h command3.h command5.h batl_prc.h \
  clients.h
dam_skll.o: dam_skll.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spells.h skills.h batl_prc.h dam_skll.h
dam_spll.o: dam_spll.cc dam_spll.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  spells.h room.h critter.h object.h door.h lang_strings.h battle.h \
  commands.h command2.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h skills.h
door.o: door.cc door.h ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/list2.h classes.h script.h \
  const.h ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h room.h critter.h \
  object.h lang_strings.h battle.h commands.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h
ez_skll.o: ez_skll.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spells.h skills.h batl_prc.h ez_skll.h
ez_spll.o: ez_spll.cc ez_spll.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  commands.h critter.h object.h door.h room.h lang_strings.h battle.h \
  command2.h misc.h zone.h ${MUD_HOME}/include/tree2.h \
  BugEntry.h SkillSpell.h MudStats.h misc2.h spells.h skills.h spec_prc.h \
  clients.h
grrmud.o: grrmud.cc ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.cc classes.h script.h \
  const.h ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h misc.h object.h \
  critter.h door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h command2.h command3.h command4.h spec_prc.h grrmud.h \
  load_wld.h login.h BuildInfo.h Filters.h spells.h telnet.h
lang_strings.o: lang_strings.cc lang_strings.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h
load_wld.o: load_wld.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h olc2.h load_wld.h vehicle.h misc2.h
login.o: login.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h spells.h skills.h command2.h command5.h load_wld.h Filters.h \
  clients.h
mapper.o: mapper.cc mapper.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h
misc.o: misc.cc command4.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  room.h critter.h object.h door.h lang_strings.h battle.h commands.h \
  misc.h zone.h ${MUD_HOME}/include/tree2.h BugEntry.h \
  SkillSpell.h MudStats.h misc2.h spells.h vehicle.h load_wld.h \
  command3.h skills.h Filters.h
misc2.o: misc2.cc misc.h ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h command2.h command3.h spec_prc.h batl_prc.h spells.h \
  skills.h vehicle.h
obj_cmds.o: obj_cmds.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h rm_cmds.h
obj_parse.o: obj_parse.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h rm_parse.h obj_cmds.h parse.h parse_gen.h ar_skll.h \
  spells.h cr_skll.h social2.h ez_skll.h dam_skll.h skills.h command2.h \
  command3.h command4.h command5.h olc.h olc2.h socials.h wep_skll.h
object.o: object.cc object.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h door.h room.h commands.h lang_strings.h battle.h misc.h \
  zone.h ${MUD_HOME}/include/tree2.h BugEntry.h \
  SkillSpell.h MudStats.h misc2.h batl_prc.h load_wld.h
olc.o: olc.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h command2.h command3.h command4.h command5.h misc2.h olc.h \
  olc2.h vehicle.h load_wld.h
olc2.o: olc2.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h command2.h \
  command3.h command4.h command5.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spec_prc.h spells.h skills.h olc2.h olc.h vehicle.h
parse.o: parse.cc ar_skll.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  spells.h room.h critter.h object.h door.h lang_strings.h battle.h \
  commands.h cr_skll.h social2.h ez_skll.h dam_skll.h skills.h misc.h \
  zone.h ${MUD_HOME}/include/tree2.h BugEntry.h \
  SkillSpell.h MudStats.h misc2.h command2.h command3.h command4.h \
  command5.h olc.h olc2.h socials.h wep_skll.h parse.h parse_gen.h
parse_gen.o: parse_gen.cc code_gen.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/list2.h parse.h classes.h \
  ${MUD_HOME}/include/bitfield.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h parse_gen.h ar_skll.h \
  spells.h room.h critter.h object.h door.h lang_strings.h battle.h \
  commands.h cr_skll.h social2.h ez_skll.h dam_skll.h skills.h misc.h \
  zone.h ${MUD_HOME}/include/tree2.h BugEntry.h \
  SkillSpell.h MudStats.h misc2.h command2.h command3.h command4.h \
  command5.h olc.h olc2.h socials.h wep_skll.h
pet_spll.o: pet_spll.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h command2.h \
  misc.h zone.h ${MUD_HOME}/include/tree2.h BugEntry.h \
  SkillSpell.h MudStats.h misc2.h spells.h skills.h pet_spll.h command3.h \
  load_wld.h trv_spll.h
regex.o: regex.cc regex.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h
rm_cmds.o: rm_cmds.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h rm_cmds.h
rm_parse.o: rm_parse.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h rm_parse.h rm_cmds.h parse.h parse_gen.h ar_skll.h \
  spells.h cr_skll.h social2.h ez_skll.h dam_skll.h skills.h command2.h \
  command3.h command4.h command5.h olc.h olc2.h socials.h wep_skll.h
rm_spll.o: rm_spll.cc rm_spll.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  command4.h room.h critter.h object.h door.h lang_strings.h battle.h \
  commands.h command2.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spells.h skills.h
room.o: room.cc door.h ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/list2.h classes.h script.h \
  const.h ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h room.h critter.h \
  object.h lang_strings.h battle.h commands.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h load_wld.h Filters.h
script.o: script.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h command2.h \
  command3.h command4.h command5.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spec_prc.h spells.h skills.h batl_prc.h load_wld.h
skills.o: skills.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spells.h skills.h batl_prc.h socials.h social2.h \
  load_wld.h spells2.h
social2.o: social2.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h command2.h spec_prc.h social2.h
socials.o: socials.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h command2.h spec_prc.h
spec_prc.o: spec_prc.cc spells.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  room.h critter.h object.h door.h lang_strings.h battle.h commands.h \
  spells2.h skills.h pet_spll.h rm_spll.h command4.h dam_spll.h ez_spll.h \
  trv_spll.h batl_prc.h spec_prc.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h command2.h command3.h command5.h socials.h \
  load_wld.h
spells.o: spells.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h command2.h \
  command3.h misc.h zone.h ${MUD_HOME}/include/tree2.h \
  BugEntry.h SkillSpell.h MudStats.h misc2.h spells.h spells2.h skills.h \
  ez_spll.h dam_spll.h pet_spll.h rm_spll.h command4.h trv_spll.h \
  clients.h
spells2.o: spells2.cc commands.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  critter.h object.h door.h room.h lang_strings.h battle.h command2.h \
  misc.h zone.h ${MUD_HOME}/include/tree2.h BugEntry.h \
  SkillSpell.h MudStats.h misc2.h spells.h skills.h spells2.h socials.h
tmp_socials.o: tmp_socials.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h command2.h spec_prc.h
trv_spll.o: trv_spll.cc trv_spll.h const.h \
  ${MUD_HOME}/include/list2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  commands.h critter.h object.h door.h room.h lang_strings.h battle.h \
  command2.h misc.h zone.h ${MUD_HOME}/include/tree2.h \
  BugEntry.h SkillSpell.h MudStats.h misc2.h spells.h skills.h
vehicle.o: vehicle.cc vehicle.h room.h critter.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h door.h \
  lang_strings.h battle.h commands.h misc.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h
wep_skll.o: wep_skll.cc misc.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h classes.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/list2.h script.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc \
  ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/PtrArray.cc ServerConfig.h \
  ${MUD_HOME}/include/KVPair.h object.h critter.h \
  door.h room.h commands.h lang_strings.h battle.h zone.h \
  ${MUD_HOME}/include/tree2.h BugEntry.h SkillSpell.h \
  MudStats.h misc2.h spells.h skills.h batl_prc.h wep_skll.h
zone.o: zone.cc zone.h ${MUD_HOME}/include/PtrArray.h \
  ${MUD_HOME}/include/bitfield.h \
  ${MUD_HOME}/include/string2.h \
  ${MUD_HOME}/include/LogStream.h \
  ${MUD_HOME}/include/PtrArray.cc \
  ${MUD_HOME}/include/tree2.h \
  ${MUD_HOME}/include/list2.h const.h \
  ${MUD_HOME}/include/rb_tree.h \
  ${MUD_HOME}/include/rb_tree.cc ServerConfig.h misc.h \
  classes.h script.h ${MUD_HOME}/include/KVPair.h \
  object.h critter.h door.h room.h commands.h lang_strings.h battle.h \
  BugEntry.h SkillSpell.h MudStats.h misc2.h command4.h command5.h \
  mapper.h
