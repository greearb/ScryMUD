#  Makefile for Hegemon and ScryMUD


#You should not have to edit anything besides the MakeInclude file
include MakeInclude

CORE_FILES = \
	grrmud/Help/IMM*_1\
	grrmud/Help/[a-z]*_1 \
	grrmud/server/*.cc grrmud/server/*.h \
	grrmud/server/Makefile MakeInclude Makefile \
	lib/bitfield/*.h lib/bitfield/*.cc lib/bitfield/Makefile \
	lib/log/*.h lib/log/*.cc lib/log/Makefile \
	lib/containers/*.h lib/containers/*.cc \
	lib/containers/Makefile grrmud/World/Makefile \
	grrmud/World/SS_DESCS grrmud/World/SKILLS_SPELLS \
	lib/string2/*.cc lib/string2/*.h lib/string2/Makefile \
	grrmud/truncate_log License Credits grrmud/startup \
	include/README lib/Makefile README grrmud/server/gen_cmds.spec

MISC_FILES = \
	grrmud/Boards/board_32 \
	grrmud/Pfiles/admin grrmud/PlayerShops \
	grrmud/grrmud.ini grrmud/shutdown_msg grrmud/startup



#NOTE:  if you are installing for the first time, you will probably want
#       to use the install target, example:  make install

all: scry_dist_all hegemon


# This will create a tarbal of everything one would need to
# untar, compile, and start it up!  This would be used to transfer
# one MUD to another machine.
# source, world, boards, help, log, etc


scry_dist_all: scry_dist_misc scry_dist_core scry_dist_world
	rm -f scry_dist_all$(SCRY_VERSION).tar.gz
	tar -cvf scry_dist_all$(SCRY_VERSION).tar \
		scry_dist_misc$(SCRY_VERSION).tar.gz \
		scry_dist_core$(SCRY_VERSION).tar.gz \
		grrmud/World/scry_dist_world${SCRY_VERSION}.tar.gz \
		License
	gzip scry_dist_all${SCRY_VERSION}.tar


scry_dist_misc:
	rm -f scry_dist_misc$(SCRY_VERSION).tar.gz
	tar -cvf scry_dist_misc$(SCRY_VERSION).tar ${MISC_FILES}
	gzip scry_dist_misc$(SCRY_VERSION).tar

scry_dist_core: build_html_pages
	rm -f scry_dist_core$(SCRY_VERSION).tar.gz
	tar -cvf scry_dist_core$(SCRY_VERSION).tar ${CORE_FILES}
	gzip scry_dist_core$(SCRY_VERSION).tar


scry_dist_world:
	${MAKE} -C grrmud/World dist

scry_archive_world:
	${MAKE} -C grrmud/World archive


archive: scry_archive_world
	tar -cvf scry_archive.${CUR_DATE}.tar ${CORE_FILES} \
		grrmud/World/wrld.${CUR_DATE}.tar.gz \
		${MISC_FILES}
	gzip scry_archive.${CUR_DATE}.tar

new_rel:
	${MAKE} -C grrmud/server new_binary
	${MAKE} -C grrmud/World new_db


install: build_scry_bin
	@echo "If you fail after this step, please read this makefile"
	@echo "and figure out what to do.  It is not complex, just creating"
	@echo "a few directories and moving the binary into place."
	@echo ""
	mkdir -p grrmud/World/OLC_DIR
	mkdir -p grrmud/log/Ideas
	mkdir -p grrmud/log/Bugs
	mkdir -p grrmud/log/Errors
	mkdir -p grrmud/log/Warnings
	cp grrmud/grrmud.ini grrmud/grrmud.ini.bak
	mv -f grrmud/server/grrmud grrmud/gmud
	mv -f grrmud/World/dist_zone_file grrmud/World/ZONE_FILE
	@echo "You may want to check the grrmud/grrmud.ini file to make sure"
	@echo "the settings are agreeable, especially the port."
	@echo "" 
	@echo "If everything worked as planned, you may cd to ./grrmud and type"
	@echo "startup&, or nohup startup& if you are dialing in.."
	@echo ""

build_lib_includes:
	${MAKE} -C lib all


build_scry_bin: build_lib_includes
	${MAKE} -C grrmud/server all


build_html_pages: build_help_filter
	${MAKE} -C grrmud/Help all


build_help_filter:
	${MAKE} -C grrmud/help_filter all




##################################################################
########  Hegemon (Java Client) Related rules ####################
##################################################################

hegemon: hegemon_dos hegemon_unix hegemon_applet hegemon_src

hegemon_dos:  hegemon_update
	rm -f hegemon$(HEGEMON_VERSION).zip
	zip hegemon$(HEGEMON_VERSION).zip  hegemon/*.class \
		hegemon/hegemon.properties \
		hegemon/data/colors.def hegemon/data/alias.def \
		hegemon/data/hosts.def \
		hegemon/data/help/battle/* hegemon/data/help/client/* \
		hegemon/data/help/communications/* \
		hegemon/data/help/general/* hegemon/data/help/immortal/* \
		hegemon/data/help/olc/* hegemon/data/help/skills/* \
		hegemon/data/help/spells/* hegemon/data/help/*.bml


hegemon_unix:  hegemon_update
	rm -f hegemon$(HEGEMON_VERSION).tar.gz
	tar -cvf hegemon$(HEGEMON_VERSION).tar  hegemon/*.class \
		hegemon/hegemon.properties \
		hegemon/data/colors.def hegemon/data/alias.def \
		hegemon/data/hosts.def \
		hegemon/data/help/battle/* hegemon/data/help/client/* \
		hegemon/data/help/communications/* \
		hegemon/data/help/general/* hegemon/data/help/immortal/* \
		hegemon/data/help/olc/* hegemon/data/help/skills/* \
		hegemon/data/help/spells/* hegemon/data/help/*.bml
	gzip hegemon$(HEGEMON_VERSION).tar


hegemon_archive: hegemon_update
	rm -f hegemon_archive$(CUR_DATE).tar.gz
	tar -cvf hegemon_archive.$(CUR_DATE).tar \
		Hegemon/*.java Hegemon/Makefile Hegemon/*.html \
		Hegemon/*.properties \
		hegemon
	gzip hegemon_archive.$(CUR_DATE).tar

hegemon_src:
	rm -f hegemon$(HEGEMON_VERSION).src.tar.gz
	tar -cvf hegemon$(HEGEMON_VERSION).src.tar Hegemon/*.java License \
		Hegemon/*.html Hegemon/Makefile
	gzip hegemon$(HEGEMON_VERSION).src.tar


hegemon_applet:  hegemon_update
	rm -f heg_applet$(HEGEMON_VERSION).tar.gz
	rm -fr heg_applet        #clean out the directory
	mkdir heg_applet
	cp Hegemon/heg.jar heg_applet/
	cp Hegemon/hegemon.html heg_applet/
	cp Hegemon/hegemon_local.html heg_applet/
	cp Hegemon/local_applet.properties heg_applet/
	cp Hegemon/scry.gif heg_applet/
	cp Hegemon/applet.properties heg_applet/hegemon.properties
	cp -r hegemon/data heg_applet/data
	tar -cvf heg_applet$(HEGEMON_VERSION).tar heg_applet/*
	gzip heg_applet$(HEGEMON_VERSION).tar


hegemon_update:
	${MAKE} -C Hegemon all

clean:
	${MAKE} -C grrmud/server clean
	rm -f hegemon$(HEGEMON_VERSION).tar.gz
	rm -f hegemon$(HEGEMON_VERSION).src.tar.gz
	rm -f hegemon$(HEGEMON_VERSION).zip
	rm -f heg_applet$(HEGEMON_VERSION).tar.gz
	rm -f scry_dist*
	rm -f scry_core$(SCRY_VERSION).tar

purge: clean
	find . -name "*~" -exec rm {} \;
	find . -name "*.flc" -exec rm {} \;
