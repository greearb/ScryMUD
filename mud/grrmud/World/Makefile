# $Id: Makefile,v 1.4 1999/06/05 23:29:12 greear Exp $
# $Revision: 1.4 $  $Author: greear $ $Date: 1999/06/05 23:29:12 $


include ../../MakeInclude

WORLD_FILES = \
	DEFAULT_CRITTERS DEFAULT_DOORS DEFAULT_OBJECTS DEFAULT_ZONE \
	SKILLS_SPELLS ZONE_FILE SS_DESCS \
	critters_[0-9] critters_[0-9][0-9] \
	doors_[0-9] doors_[0-9][0-9] \
	objects_[0-9] objects_[0-9][0-9] \
	zone_[0-9] zone_[0-9][0-9]

DIST_WORLD_FILES = \
	DEFAULT_CRITTERS DEFAULT_DOORS DEFAULT_OBJECTS DEFAULT_ZONE \
	SKILLS_SPELLS dist_zone_file SS_DESCS \
	critters_0 doors_0 objects_0 zone_0


TAR_TARG = scry_dist_world${SCRY_VERSION}.tar

ZONE_NEATO_FILES=*.nto

all: dist


ss.ps: ss_ps.dot
	dot -Tps -o ss.ps ss_ps.dot

#  You get ss_gif.dot when you run ScryMUD with the option
#  CONVERT_WORLD_FROM_DEV set to non-zero.
ss.gif: ss_gif.dot
	dot -Tgif -o ss.gif ss_gif.dot

#This doesn't currently work too well..gotta do some more thinking
# on it... --BEN
zone_gifs: ${ZONE_NEATO_FILES}
	batch_neato_gifs ${ZONE_NEATO_FILES}

dist:
	rm -f ${TAR_TARG}
	rm -f ${TAR_TARG}.gz
	tar -cvf ${TAR_TARG} ${DIST_WORLD_FILES}
	gzip ${TAR_TARG}


archive:
	rm -f wrld.${CUR_DATE}.tar
	rm -f wrld.${CUR_DATE}.tar.gz
	tar -cvf wrld.${CUR_DATE}.tar ${WORLD_FILES}
	gzip wrld.${CUR_DATE}.tar

new_db: archive
	rm -f ../wrld.new.tar.gz
	mv wrld.${CUR_DATE}.tar.gz ../wrld.new.tar.gz
