# $Id$
# $Revision: 1.24 $  $Author$ $Date$

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
obj_cmds.cc BugEntry.cc MudStats.cc clients.cc ServerConfig.cc

OBJS = parse_gen.o lang_strings.o classes.o object.o critter.o \
Filters.o room.o door.o skills.o command3.o \
grrmud.o commands.o command2.o socials.o spells.o olc.o \
load_wld.o misc.o misc2.o parse.o battle.o spec_prc.o login.o \
command4.o olc2.o command5.o batl_prc.o spells2.o \
ez_spll.o dam_spll.o trv_spll.o rm_spll.o dam_skll.o wep_skll.o \
ez_skll.o social2.o cr_skll.o ar_skll.o pet_spll.o vehicle.o const.o \
script.o SkillSpell.o zone.o rm_parse.o rm_cmds.o obj_parse.o \
obj_cmds.o BuildInfo.o BugEntry.o MudStats.o clients.o ServerConfig.o

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
	@echo " "
	@echo "Making $<"
	$(CC) $(CCFLAGS) \
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
	rm -f Subversion-Revision
	${MAKE} -C ../../lib clean

purge: clean
	rm -f *~
	rm -f *.flc
	rm -f ${TARG} ${CODE_GEN_TARG} ${AUDIT_TARG}


#DO NOT DELETE THIS LINE -- make depend depends on it.

# DO NOT DELETE

parse_gen.o: code_gen.h parse.h classes.h script.h const.h ServerConfig.h
parse_gen.o: /usr/include/string.h /usr/include/features.h
parse_gen.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
parse_gen.o: /usr/include/stdlib.h parse_gen.h ar_skll.h spells.h room.h
parse_gen.o: critter.h object.h door.h lang_strings.h commands.h cr_skll.h
parse_gen.o: social2.h ez_skll.h dam_skll.h skills.h misc.h zone.h BugEntry.h
parse_gen.o: /usr/include/errno.h /usr/include/bits/errno.h
parse_gen.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
parse_gen.o: MudStats.h misc2.h command2.h command3.h command4.h command5.h
parse_gen.o: olc.h olc2.h socials.h wep_skll.h
lang_strings.o: lang_strings.h
classes.o: classes.h script.h const.h ServerConfig.h /usr/include/string.h
classes.o: /usr/include/features.h /usr/include/sys/cdefs.h
classes.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h misc.h object.h
classes.o: critter.h door.h room.h commands.h lang_strings.h zone.h
classes.o: BugEntry.h /usr/include/errno.h /usr/include/bits/errno.h
classes.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
classes.o: MudStats.h misc2.h battle.h command5.h command4.h BuildInfo.h
classes.o: /usr/include/getopt.h
object.o: object.h const.h ServerConfig.h /usr/include/string.h
object.o: /usr/include/features.h /usr/include/sys/cdefs.h
object.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
object.o: critter.h door.h room.h commands.h lang_strings.h misc.h zone.h
object.o: BugEntry.h /usr/include/errno.h /usr/include/bits/errno.h
object.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
object.o: MudStats.h misc2.h batl_prc.h load_wld.h battle.h
critter.o: /usr/include/arpa/telnet.h script.h const.h ServerConfig.h
critter.o: /usr/include/string.h /usr/include/features.h
critter.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
critter.o: /usr/include/stdlib.h classes.h critter.h object.h door.h room.h
critter.o: commands.h lang_strings.h misc.h zone.h BugEntry.h
critter.o: /usr/include/errno.h /usr/include/bits/errno.h
critter.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
critter.o: MudStats.h misc2.h command2.h command3.h command5.h batl_prc.h
critter.o: battle.h clients.h
Filters.o: Filters.h critter.h classes.h script.h const.h ServerConfig.h
Filters.o: /usr/include/string.h /usr/include/features.h
Filters.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Filters.o: /usr/include/stdlib.h object.h door.h room.h commands.h
Filters.o: lang_strings.h misc2.h misc.h zone.h BugEntry.h
Filters.o: /usr/include/errno.h /usr/include/bits/errno.h
Filters.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
Filters.o: MudStats.h
room.o: door.h classes.h script.h const.h ServerConfig.h
room.o: /usr/include/string.h /usr/include/features.h
room.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
room.o: /usr/include/stdlib.h room.h critter.h object.h lang_strings.h
room.o: commands.h misc.h zone.h BugEntry.h /usr/include/errno.h
room.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
room.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h battle.h
room.o: load_wld.h Filters.h
door.o: door.h classes.h script.h const.h ServerConfig.h
door.o: /usr/include/string.h /usr/include/features.h
door.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
door.o: /usr/include/stdlib.h room.h critter.h object.h lang_strings.h
door.o: commands.h misc.h zone.h BugEntry.h /usr/include/errno.h
door.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
door.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h
skills.o: misc.h classes.h script.h const.h ServerConfig.h
skills.o: /usr/include/string.h /usr/include/features.h
skills.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
skills.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
skills.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
skills.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
skills.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h spells.h
skills.o: battle.h skills.h batl_prc.h socials.h social2.h load_wld.h
skills.o: spells2.h
command3.o: commands.h const.h ServerConfig.h /usr/include/string.h
command3.o: /usr/include/features.h /usr/include/sys/cdefs.h
command3.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
command3.o: critter.h object.h door.h room.h lang_strings.h command2.h
command3.o: command3.h command4.h misc.h zone.h BugEntry.h
command3.o: /usr/include/errno.h /usr/include/bits/errno.h
command3.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
command3.o: MudStats.h misc2.h /usr/include/stdio.h /usr/include/bits/types.h
command3.o: /usr/include/libio.h /usr/include/_G_config.h
command3.o: /usr/include/wchar.h /usr/include/bits/wchar.h
command3.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
command3.o: /usr/include/bits/sys_errlist.h battle.h spec_prc.h spells.h
command3.o: skills.h command5.h load_wld.h
grrmud.o: /usr/include/errno.h /usr/include/features.h
grrmud.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
grrmud.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
grrmud.o: /usr/include/asm/errno.h /usr/include/stdio.h
grrmud.o: /usr/include/bits/types.h /usr/include/libio.h
grrmud.o: /usr/include/_G_config.h /usr/include/wchar.h
grrmud.o: /usr/include/bits/wchar.h /usr/include/gconv.h
grrmud.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
grrmud.o: /usr/include/stdlib.h /usr/include/ctype.h /usr/include/endian.h
grrmud.o: /usr/include/bits/endian.h /usr/include/limits.h
grrmud.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
grrmud.o: /usr/include/stdint.h /usr/include/bits/wordsize.h
grrmud.o: /usr/include/bits/socket.h /usr/include/sys/types.h
grrmud.o: /usr/include/time.h /usr/include/bits/sockaddr.h
grrmud.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
grrmud.o: /usr/include/bits/in.h /usr/include/bits/byteswap.h
grrmud.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
grrmud.o: /usr/include/bits/uio.h /usr/include/sys/wait.h
grrmud.o: /usr/include/signal.h /usr/include/bits/sigset.h
grrmud.o: /usr/include/bits/signum.h /usr/include/sys/resource.h
grrmud.o: /usr/include/bits/resource.h /usr/include/bits/time.h
grrmud.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
grrmud.o: /usr/include/sys/time.h /usr/include/sys/select.h
grrmud.o: /usr/include/bits/select.h /usr/include/unistd.h
grrmud.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
grrmud.o: /usr/include/netdb.h /usr/include/bits/netdb.h /usr/include/fcntl.h
grrmud.o: /usr/include/bits/fcntl.h /usr/include/strings.h classes.h script.h
grrmud.o: const.h ServerConfig.h /usr/include/string.h misc.h object.h
grrmud.o: critter.h door.h room.h commands.h lang_strings.h zone.h BugEntry.h
grrmud.o: SkillSpell.h MudStats.h misc2.h battle.h command2.h command3.h
grrmud.o: command4.h spec_prc.h grrmud.h load_wld.h login.h BuildInfo.h
grrmud.o: Filters.h spells.h
commands.o: rm_spll.h const.h ServerConfig.h /usr/include/string.h
commands.o: /usr/include/features.h /usr/include/sys/cdefs.h
commands.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
commands.o: command4.h room.h critter.h object.h door.h lang_strings.h
commands.o: commands.h dam_spll.h spells.h ez_spll.h trv_spll.h command2.h
commands.o: command5.h misc.h zone.h BugEntry.h /usr/include/errno.h
commands.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
commands.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h battle.h
commands.o: /usr/include/stdio.h /usr/include/bits/types.h
commands.o: /usr/include/libio.h /usr/include/_G_config.h
commands.o: /usr/include/wchar.h /usr/include/bits/wchar.h
commands.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
commands.o: /usr/include/bits/sys_errlist.h spec_prc.h batl_prc.h spells2.h
commands.o: skills.h pet_spll.h load_wld.h Filters.h clients.h
command2.o: commands.h const.h ServerConfig.h /usr/include/string.h
command2.o: /usr/include/features.h /usr/include/sys/cdefs.h
command2.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
command2.o: critter.h object.h door.h room.h lang_strings.h command2.h
command2.o: command3.h command4.h command5.h misc.h zone.h BugEntry.h
command2.o: /usr/include/errno.h /usr/include/bits/errno.h
command2.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
command2.o: MudStats.h misc2.h /usr/include/stdio.h /usr/include/bits/types.h
command2.o: /usr/include/libio.h /usr/include/_G_config.h
command2.o: /usr/include/wchar.h /usr/include/bits/wchar.h
command2.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
command2.o: /usr/include/bits/sys_errlist.h battle.h spec_prc.h spells.h
command2.o: skills.h parse.h parse_gen.h ar_skll.h cr_skll.h social2.h
command2.o: ez_skll.h dam_skll.h olc.h olc2.h socials.h wep_skll.h load_wld.h
command2.o: clients.h
socials.o: misc.h classes.h script.h const.h ServerConfig.h
socials.o: /usr/include/string.h /usr/include/features.h
socials.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
socials.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
socials.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
socials.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
socials.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h
socials.o: command2.h battle.h spec_prc.h
spells.o: commands.h const.h ServerConfig.h /usr/include/string.h
spells.o: /usr/include/features.h /usr/include/sys/cdefs.h
spells.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
spells.o: critter.h object.h door.h room.h lang_strings.h command2.h
spells.o: command3.h /usr/include/stdio.h /usr/include/bits/types.h
spells.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
spells.o: /usr/include/bits/wchar.h /usr/include/gconv.h
spells.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
spells.o: misc.h zone.h BugEntry.h /usr/include/errno.h
spells.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
spells.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h battle.h
spells.o: spells.h spells2.h skills.h ez_spll.h dam_spll.h pet_spll.h
spells.o: rm_spll.h command4.h trv_spll.h clients.h
olc.o: commands.h const.h ServerConfig.h /usr/include/string.h
olc.o: /usr/include/features.h /usr/include/sys/cdefs.h
olc.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
olc.o: critter.h object.h door.h room.h lang_strings.h misc.h zone.h
olc.o: BugEntry.h /usr/include/errno.h /usr/include/bits/errno.h
olc.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
olc.o: MudStats.h command2.h command3.h command4.h command5.h misc2.h
olc.o: /usr/include/stdio.h /usr/include/bits/types.h /usr/include/libio.h
olc.o: /usr/include/_G_config.h /usr/include/wchar.h
olc.o: /usr/include/bits/wchar.h /usr/include/gconv.h
olc.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h battle.h
olc.o: olc.h olc2.h vehicle.h load_wld.h
load_wld.o: misc.h classes.h script.h const.h ServerConfig.h
load_wld.o: /usr/include/string.h /usr/include/features.h
load_wld.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
load_wld.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
load_wld.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
load_wld.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
load_wld.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h
load_wld.o: /usr/include/stdio.h /usr/include/bits/types.h
load_wld.o: /usr/include/libio.h /usr/include/_G_config.h
load_wld.o: /usr/include/wchar.h /usr/include/bits/wchar.h
load_wld.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
load_wld.o: /usr/include/bits/sys_errlist.h olc2.h load_wld.h vehicle.h
load_wld.o: misc2.h
misc.o: command4.h const.h ServerConfig.h /usr/include/string.h
misc.o: /usr/include/features.h /usr/include/sys/cdefs.h
misc.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
misc.o: room.h critter.h object.h door.h lang_strings.h commands.h
misc.o: /usr/include/time.h /usr/include/bits/types.h battle.h misc.h zone.h
misc.o: BugEntry.h /usr/include/errno.h /usr/include/bits/errno.h
misc.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
misc.o: MudStats.h misc2.h /usr/include/stdio.h /usr/include/libio.h
misc.o: /usr/include/_G_config.h /usr/include/wchar.h
misc.o: /usr/include/bits/wchar.h /usr/include/gconv.h
misc.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
misc.o: /usr/include/sys/types.h /usr/include/unistd.h
misc.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h spells.h
misc.o: vehicle.h load_wld.h command3.h skills.h Filters.h
misc.o: /usr/include/math.h /usr/include/bits/huge_val.h
misc.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
misc2.o: misc.h classes.h script.h const.h ServerConfig.h
misc2.o: /usr/include/string.h /usr/include/features.h
misc2.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
misc2.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
misc2.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
misc2.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
misc2.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h
misc2.o: /usr/include/stdio.h /usr/include/bits/types.h /usr/include/libio.h
misc2.o: /usr/include/_G_config.h /usr/include/wchar.h
misc2.o: /usr/include/bits/wchar.h /usr/include/gconv.h
misc2.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
misc2.o: command2.h command3.h spec_prc.h batl_prc.h spells.h skills.h
misc2.o: vehicle.h /usr/include/sys/types.h /usr/include/time.h
misc2.o: /usr/include/signal.h /usr/include/bits/sigset.h
misc2.o: /usr/include/bits/signum.h
parse.o: ar_skll.h const.h ServerConfig.h /usr/include/string.h
parse.o: /usr/include/features.h /usr/include/sys/cdefs.h
parse.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
parse.o: spells.h room.h critter.h object.h door.h lang_strings.h commands.h
parse.o: cr_skll.h social2.h ez_skll.h dam_skll.h skills.h misc.h zone.h
parse.o: BugEntry.h /usr/include/errno.h /usr/include/bits/errno.h
parse.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
parse.o: MudStats.h misc2.h command2.h command3.h command4.h command5.h olc.h
parse.o: olc2.h socials.h /usr/include/stdio.h /usr/include/bits/types.h
parse.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
parse.o: /usr/include/bits/wchar.h /usr/include/gconv.h
parse.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
parse.o: wep_skll.h parse.h parse_gen.h
battle.o: battle.h classes.h script.h const.h ServerConfig.h
battle.o: /usr/include/string.h /usr/include/features.h
battle.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
battle.o: /usr/include/stdlib.h room.h critter.h object.h door.h
battle.o: lang_strings.h commands.h misc.h zone.h BugEntry.h
battle.o: /usr/include/errno.h /usr/include/bits/errno.h
battle.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
battle.o: MudStats.h misc2.h spec_prc.h /usr/include/math.h
battle.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
battle.o: /usr/include/bits/mathcalls.h command2.h command3.h
battle.o: /usr/include/stdio.h /usr/include/bits/types.h /usr/include/libio.h
battle.o: /usr/include/_G_config.h /usr/include/wchar.h
battle.o: /usr/include/bits/wchar.h /usr/include/gconv.h
battle.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
battle.o: spells.h spells2.h skills.h load_wld.h Filters.h batl_prc.h
battle.o: clients.h
spec_prc.o: spells.h const.h ServerConfig.h /usr/include/string.h
spec_prc.o: /usr/include/features.h /usr/include/sys/cdefs.h
spec_prc.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
spec_prc.o: room.h critter.h object.h door.h lang_strings.h commands.h
spec_prc.o: spells2.h skills.h pet_spll.h rm_spll.h command4.h dam_spll.h
spec_prc.o: ez_spll.h trv_spll.h batl_prc.h spec_prc.h misc.h zone.h
spec_prc.o: BugEntry.h /usr/include/errno.h /usr/include/bits/errno.h
spec_prc.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
spec_prc.o: MudStats.h misc2.h command2.h command3.h command5.h socials.h
spec_prc.o: battle.h load_wld.h
login.o: commands.h const.h ServerConfig.h /usr/include/string.h
login.o: /usr/include/features.h /usr/include/sys/cdefs.h
login.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
login.o: critter.h object.h door.h room.h lang_strings.h misc.h zone.h
login.o: BugEntry.h /usr/include/errno.h /usr/include/bits/errno.h
login.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
login.o: MudStats.h /usr/include/stdio.h /usr/include/bits/types.h
login.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
login.o: /usr/include/bits/wchar.h /usr/include/gconv.h
login.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
login.o: spells.h skills.h command2.h command5.h load_wld.h
login.o: /usr/include/time.h /usr/include/unistd.h
login.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h Filters.h
login.o: clients.h
command4.o: commands.h const.h ServerConfig.h /usr/include/string.h
command4.o: /usr/include/features.h /usr/include/sys/cdefs.h
command4.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
command4.o: critter.h object.h door.h room.h lang_strings.h command2.h
command4.o: command3.h command4.h command5.h olc2.h misc.h zone.h BugEntry.h
command4.o: /usr/include/errno.h /usr/include/bits/errno.h
command4.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
command4.o: MudStats.h misc2.h /usr/include/stdio.h /usr/include/bits/types.h
command4.o: /usr/include/libio.h /usr/include/_G_config.h
command4.o: /usr/include/wchar.h /usr/include/bits/wchar.h
command4.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
command4.o: /usr/include/bits/sys_errlist.h battle.h spec_prc.h spells.h
command4.o: skills.h load_wld.h /usr/include/time.h /usr/include/unistd.h
command4.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
command4.o: vehicle.h
olc2.o: commands.h const.h ServerConfig.h /usr/include/string.h
olc2.o: /usr/include/features.h /usr/include/sys/cdefs.h
olc2.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
olc2.o: critter.h object.h door.h room.h lang_strings.h command2.h command3.h
olc2.o: command4.h command5.h misc.h zone.h BugEntry.h /usr/include/errno.h
olc2.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
olc2.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h
olc2.o: /usr/include/stdio.h /usr/include/bits/types.h /usr/include/libio.h
olc2.o: /usr/include/_G_config.h /usr/include/wchar.h
olc2.o: /usr/include/bits/wchar.h /usr/include/gconv.h
olc2.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
olc2.o: battle.h spec_prc.h spells.h skills.h olc2.h olc.h vehicle.h
command5.o: commands.h const.h ServerConfig.h /usr/include/string.h
command5.o: /usr/include/features.h /usr/include/sys/cdefs.h
command5.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
command5.o: critter.h object.h door.h room.h lang_strings.h command2.h
command5.o: command3.h command4.h command5.h misc.h zone.h BugEntry.h
command5.o: /usr/include/errno.h /usr/include/bits/errno.h
command5.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
command5.o: MudStats.h misc2.h /usr/include/stdio.h /usr/include/bits/types.h
command5.o: /usr/include/libio.h /usr/include/_G_config.h
command5.o: /usr/include/wchar.h /usr/include/bits/wchar.h
command5.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
command5.o: /usr/include/bits/sys_errlist.h battle.h spec_prc.h spells.h
command5.o: skills.h batl_prc.h /usr/include/unistd.h
command5.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
command5.o: load_wld.h olc.h BuildInfo.h clients.h vehicle.h
batl_prc.o: battle.h classes.h script.h const.h ServerConfig.h
batl_prc.o: /usr/include/string.h /usr/include/features.h
batl_prc.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
batl_prc.o: /usr/include/stdlib.h room.h critter.h object.h door.h
batl_prc.o: lang_strings.h commands.h batl_prc.h command2.h command3.h
batl_prc.o: command4.h command5.h misc.h zone.h BugEntry.h
batl_prc.o: /usr/include/errno.h /usr/include/bits/errno.h
batl_prc.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
batl_prc.o: MudStats.h misc2.h spells.h trv_spll.h skills.h rm_spll.h
batl_prc.o: spells2.h wep_skll.h dam_spll.h dam_skll.h ez_spll.h socials.h
batl_prc.o: social2.h ar_skll.h
spells2.o: commands.h const.h ServerConfig.h /usr/include/string.h
spells2.o: /usr/include/features.h /usr/include/sys/cdefs.h
spells2.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
spells2.o: critter.h object.h door.h room.h lang_strings.h command2.h
spells2.o: /usr/include/stdio.h /usr/include/bits/types.h
spells2.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
spells2.o: /usr/include/bits/wchar.h /usr/include/gconv.h
spells2.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
spells2.o: misc.h zone.h BugEntry.h /usr/include/errno.h
spells2.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
spells2.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h battle.h
spells2.o: spells.h skills.h /usr/include/math.h /usr/include/bits/huge_val.h
spells2.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
spells2.o: spells2.h socials.h
ez_spll.o: ez_spll.h const.h ServerConfig.h /usr/include/string.h
ez_spll.o: /usr/include/features.h /usr/include/sys/cdefs.h
ez_spll.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
ez_spll.o: commands.h critter.h object.h door.h room.h lang_strings.h
ez_spll.o: command2.h /usr/include/stdio.h /usr/include/bits/types.h
ez_spll.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
ez_spll.o: /usr/include/bits/wchar.h /usr/include/gconv.h
ez_spll.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
ez_spll.o: misc.h zone.h BugEntry.h /usr/include/errno.h
ez_spll.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
ez_spll.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h battle.h
ez_spll.o: spells.h skills.h spec_prc.h clients.h
dam_spll.o: dam_spll.h const.h ServerConfig.h /usr/include/string.h
dam_spll.o: /usr/include/features.h /usr/include/sys/cdefs.h
dam_spll.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
dam_spll.o: spells.h room.h critter.h object.h door.h lang_strings.h
dam_spll.o: commands.h command2.h /usr/include/stdio.h
dam_spll.o: /usr/include/bits/types.h /usr/include/libio.h
dam_spll.o: /usr/include/_G_config.h /usr/include/wchar.h
dam_spll.o: /usr/include/bits/wchar.h /usr/include/gconv.h
dam_spll.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
dam_spll.o: misc.h zone.h BugEntry.h /usr/include/errno.h
dam_spll.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
dam_spll.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h battle.h
dam_spll.o: skills.h
trv_spll.o: trv_spll.h const.h ServerConfig.h /usr/include/string.h
trv_spll.o: /usr/include/features.h /usr/include/sys/cdefs.h
trv_spll.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
trv_spll.o: commands.h critter.h object.h door.h room.h lang_strings.h
trv_spll.o: command2.h /usr/include/stdio.h /usr/include/bits/types.h
trv_spll.o: /usr/include/libio.h /usr/include/_G_config.h
trv_spll.o: /usr/include/wchar.h /usr/include/bits/wchar.h
trv_spll.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
trv_spll.o: /usr/include/bits/sys_errlist.h misc.h zone.h BugEntry.h
trv_spll.o: /usr/include/errno.h /usr/include/bits/errno.h
trv_spll.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
trv_spll.o: MudStats.h misc2.h battle.h spells.h skills.h
rm_spll.o: rm_spll.h const.h ServerConfig.h /usr/include/string.h
rm_spll.o: /usr/include/features.h /usr/include/sys/cdefs.h
rm_spll.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
rm_spll.o: command4.h room.h critter.h object.h door.h lang_strings.h
rm_spll.o: commands.h command2.h /usr/include/stdio.h
rm_spll.o: /usr/include/bits/types.h /usr/include/libio.h
rm_spll.o: /usr/include/_G_config.h /usr/include/wchar.h
rm_spll.o: /usr/include/bits/wchar.h /usr/include/gconv.h
rm_spll.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
rm_spll.o: misc.h zone.h BugEntry.h /usr/include/errno.h
rm_spll.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
rm_spll.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h battle.h
rm_spll.o: spells.h skills.h
dam_skll.o: misc.h classes.h script.h const.h ServerConfig.h
dam_skll.o: /usr/include/string.h /usr/include/features.h
dam_skll.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
dam_skll.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
dam_skll.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
dam_skll.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
dam_skll.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h spells.h
dam_skll.o: battle.h skills.h batl_prc.h dam_skll.h
wep_skll.o: misc.h classes.h script.h const.h ServerConfig.h
wep_skll.o: /usr/include/string.h /usr/include/features.h
wep_skll.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
wep_skll.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
wep_skll.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
wep_skll.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
wep_skll.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h spells.h
wep_skll.o: battle.h skills.h batl_prc.h wep_skll.h
ez_skll.o: misc.h classes.h script.h const.h ServerConfig.h
ez_skll.o: /usr/include/string.h /usr/include/features.h
ez_skll.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ez_skll.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
ez_skll.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
ez_skll.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
ez_skll.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h spells.h
ez_skll.o: battle.h skills.h batl_prc.h ez_skll.h
social2.o: misc.h classes.h script.h const.h ServerConfig.h
social2.o: /usr/include/string.h /usr/include/features.h
social2.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
social2.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
social2.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
social2.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
social2.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h
social2.o: command2.h battle.h spec_prc.h social2.h
cr_skll.o: misc.h classes.h script.h const.h ServerConfig.h
cr_skll.o: /usr/include/string.h /usr/include/features.h
cr_skll.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
cr_skll.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
cr_skll.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
cr_skll.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
cr_skll.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h spells.h
cr_skll.o: battle.h skills.h batl_prc.h cr_skll.h load_wld.h
ar_skll.o: misc.h classes.h script.h const.h ServerConfig.h
ar_skll.o: /usr/include/string.h /usr/include/features.h
ar_skll.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ar_skll.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
ar_skll.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
ar_skll.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
ar_skll.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h spells.h
ar_skll.o: battle.h ar_skll.h skills.h batl_prc.h
pet_spll.o: commands.h const.h ServerConfig.h /usr/include/string.h
pet_spll.o: /usr/include/features.h /usr/include/sys/cdefs.h
pet_spll.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
pet_spll.o: critter.h object.h door.h room.h lang_strings.h command2.h
pet_spll.o: /usr/include/stdio.h /usr/include/bits/types.h
pet_spll.o: /usr/include/libio.h /usr/include/_G_config.h
pet_spll.o: /usr/include/wchar.h /usr/include/bits/wchar.h
pet_spll.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
pet_spll.o: /usr/include/bits/sys_errlist.h misc.h zone.h BugEntry.h
pet_spll.o: /usr/include/errno.h /usr/include/bits/errno.h
pet_spll.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
pet_spll.o: MudStats.h misc2.h battle.h spells.h skills.h /usr/include/math.h
pet_spll.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
pet_spll.o: /usr/include/bits/mathcalls.h pet_spll.h command3.h load_wld.h
pet_spll.o: trv_spll.h
vehicle.o: vehicle.h room.h critter.h classes.h script.h const.h
vehicle.o: ServerConfig.h /usr/include/string.h /usr/include/features.h
vehicle.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
vehicle.o: /usr/include/stdlib.h object.h door.h lang_strings.h commands.h
vehicle.o: misc.h zone.h BugEntry.h /usr/include/errno.h
vehicle.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
vehicle.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h
script.o: commands.h const.h ServerConfig.h /usr/include/string.h
script.o: /usr/include/features.h /usr/include/sys/cdefs.h
script.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h classes.h script.h
script.o: critter.h object.h door.h room.h lang_strings.h command2.h
script.o: command3.h command4.h command5.h misc.h zone.h BugEntry.h
script.o: /usr/include/errno.h /usr/include/bits/errno.h
script.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
script.o: MudStats.h misc2.h /usr/include/stdio.h /usr/include/bits/types.h
script.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
script.o: /usr/include/bits/wchar.h /usr/include/gconv.h
script.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
script.o: battle.h spec_prc.h spells.h skills.h batl_prc.h
script.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
script.o: /usr/include/bits/confname.h load_wld.h
SkillSpell.o: SkillSpell.h object.h const.h ServerConfig.h
SkillSpell.o: /usr/include/string.h /usr/include/features.h
SkillSpell.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
SkillSpell.o: /usr/include/stdlib.h classes.h script.h critter.h door.h
SkillSpell.o: room.h commands.h lang_strings.h misc.h zone.h BugEntry.h
SkillSpell.o: /usr/include/errno.h /usr/include/bits/errno.h
SkillSpell.o: /usr/include/linux/errno.h /usr/include/asm/errno.h MudStats.h
SkillSpell.o: dam_spll.h spells.h
zone.o: zone.h /usr/include/stdlib.h /usr/include/features.h
zone.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h const.h
zone.o: ServerConfig.h /usr/include/string.h misc.h classes.h script.h
zone.o: object.h critter.h door.h room.h commands.h lang_strings.h BugEntry.h
zone.o: /usr/include/errno.h /usr/include/bits/errno.h
zone.o: /usr/include/linux/errno.h /usr/include/asm/errno.h SkillSpell.h
zone.o: MudStats.h misc2.h command4.h command5.h
rm_parse.o: misc.h classes.h script.h const.h ServerConfig.h
rm_parse.o: /usr/include/string.h /usr/include/features.h
rm_parse.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
rm_parse.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
rm_parse.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
rm_parse.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
rm_parse.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h
rm_parse.o: /usr/include/stdio.h /usr/include/bits/types.h
rm_parse.o: /usr/include/libio.h /usr/include/_G_config.h
rm_parse.o: /usr/include/wchar.h /usr/include/bits/wchar.h
rm_parse.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
rm_parse.o: /usr/include/bits/sys_errlist.h rm_parse.h rm_cmds.h parse.h
rm_parse.o: parse_gen.h ar_skll.h spells.h cr_skll.h social2.h ez_skll.h
rm_parse.o: dam_skll.h skills.h command2.h command3.h command4.h command5.h
rm_parse.o: olc.h olc2.h socials.h wep_skll.h
rm_cmds.o: misc.h classes.h script.h const.h ServerConfig.h
rm_cmds.o: /usr/include/string.h /usr/include/features.h
rm_cmds.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
rm_cmds.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
rm_cmds.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
rm_cmds.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
rm_cmds.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h rm_cmds.h
obj_parse.o: misc.h classes.h script.h const.h ServerConfig.h
obj_parse.o: /usr/include/string.h /usr/include/features.h
obj_parse.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
obj_parse.o: /usr/include/stdlib.h object.h critter.h door.h room.h
obj_parse.o: commands.h lang_strings.h zone.h BugEntry.h /usr/include/errno.h
obj_parse.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
obj_parse.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h
obj_parse.o: /usr/include/stdio.h /usr/include/bits/types.h
obj_parse.o: /usr/include/libio.h /usr/include/_G_config.h
obj_parse.o: /usr/include/wchar.h /usr/include/bits/wchar.h
obj_parse.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
obj_parse.o: /usr/include/bits/sys_errlist.h rm_parse.h obj_cmds.h parse.h
obj_parse.o: parse_gen.h ar_skll.h spells.h cr_skll.h social2.h ez_skll.h
obj_parse.o: dam_skll.h skills.h command2.h command3.h command4.h command5.h
obj_parse.o: olc.h olc2.h socials.h wep_skll.h
obj_cmds.o: misc.h classes.h script.h const.h ServerConfig.h
obj_cmds.o: /usr/include/string.h /usr/include/features.h
obj_cmds.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
obj_cmds.o: /usr/include/stdlib.h object.h critter.h door.h room.h commands.h
obj_cmds.o: lang_strings.h zone.h BugEntry.h /usr/include/errno.h
obj_cmds.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
obj_cmds.o: /usr/include/asm/errno.h SkillSpell.h MudStats.h misc2.h
obj_cmds.o: rm_cmds.h
BugEntry.o: BugEntry.h critter.h classes.h script.h const.h ServerConfig.h
BugEntry.o: /usr/include/string.h /usr/include/features.h
BugEntry.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
BugEntry.o: /usr/include/stdlib.h object.h door.h room.h commands.h
BugEntry.o: lang_strings.h /usr/include/errno.h /usr/include/bits/errno.h
BugEntry.o: /usr/include/linux/errno.h /usr/include/asm/errno.h misc2.h
MudStats.o: MudStats.h const.h ServerConfig.h /usr/include/string.h
MudStats.o: /usr/include/features.h /usr/include/sys/cdefs.h
MudStats.o: /usr/include/gnu/stubs.h /usr/include/stdlib.h critter.h
MudStats.o: classes.h script.h object.h door.h room.h commands.h
MudStats.o: lang_strings.h
clients.o: clients.h
ServerConfig.o: ServerConfig.h /usr/include/string.h /usr/include/features.h
ServerConfig.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
ServerConfig.o: /usr/include/stdlib.h
