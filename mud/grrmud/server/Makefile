# makefile template
include ../../MakeInclude




SRCS  = classes.cc object.cc critter.cc room.cc door.cc skills.cc command3.cc\
grrmud.cc commands.cc command2.cc socials.cc spells.cc\
olc.cc load_wld.cc misc.cc misc2.cc parse.cc battle.cc spec_prc.cc\
login.cc command4.cc olc2.cc command5.cc batl_prc.cc spells2.cc\
ez_spll.cc dam_spll.cc trv_spll.cc rm_spll.cc dam_skll.cc wep_skll.cc\
ez_skll.cc social2.cc cr_skll.cc ar_skll.cc pet_spll.cc vehicle.cc\
script.cc SkillSpell.cc zone.cc

OBJS = classes.o object.o critter.o room.o door.o skills.o command3.o\
grrmud.o commands.o command2.o socials.o spells.o olc.o\
load_wld.o misc.o misc2.o parse.o battle.o spec_prc.o login.o\
command4.o olc2.o command5.o batl_prc.o spells2.o\
ez_spll.o dam_spll.o trv_spll.o rm_spll.o dam_skll.o wep_skll.o\
ez_skll.o social2.o cr_skll.o ar_skll.o pet_spll.o vehicle.o const.o\
script.o SkillSpell.o zone.o

TARG = grrmud                                 #program to be created

all: ${TARG}

${TARG}: libs $(OBJS)
	$(CCC) $(CCFLAGS) $(LDFLAGS) -o ${TARG} $(OBJS) $(LDLIBS)

new_binary: ${TARG}
	rm -f ${MUD_HOME}/grrmud/gmud.new
	cp ${TARG} ${MUD_HOME}/grrmud/gmud.new

$(OBJS): %.o: %.cc %.h
	@echo " "
	@echo "Making $<"
	$(CC) $(CCFLAGS) \
	-c $<

libs:
	${MAKE} -C ${MUD_HOME}/lib all


depend:
	makedepend $(SRCS)

clean:
	rm -f *.o
	${MAKE} -C ../../lib clean

purge: clean
	rm -f *~
	rm -f *.flc
	rm -f ${TARG}


#DO NOT DELETE THIS LINE -- make depend depends on it.

# DO NOT DELETE






