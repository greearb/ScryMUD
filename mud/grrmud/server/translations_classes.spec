# $Id: translations_classes.spec,v 1.4 2001/03/29 03:02:36 eroper Exp $
# $Revision: 1.4 $  $Author: eroper $ $Date: 2001/03/29 03:02:36 $

# The syntax is as follows:
# ENUM_NAME LANGUAGE "TEXT" LANGUAGE "TEXT" ... ~
#
# The parser is free form in that white-space does not matter.
# All strings should be in double quotes, and you must terminate
# with a ~ (tilde) as a 'language'.
#
# NOTE:  Order is not important, and you may abbreviate the language,
#        so long as you have enough significant characters to
#        distinguish amongst them.
#
# Comments must not be in the middle of a definition.  In other words,
# only right before the ENUM name.  Comments must not be longer than
# 200 characters per line!!
#
# Languages currently supported by the code_generation process are (at least):
# English, Spanish, Portugues, Italian

# This is used as a kind of NULL enum.  It should never be used
# where a real CSentry entity is desired.

# See translations.spec for more details...


CS_ROOM_BECOMES_LIGHT
	eng "The room becomes light.\n"
	ser "Soba postaje svetla.\n"
	ger "Der Raum erhellt sich wieder.\n"
	~

CS_ROOM_BECOMES_DARK
	eng "The room becomes dark.\n"
	ser "Soba postaje mracna.\n"
	ger "Es wird dunkel im Raum.\n"
	~

CS_ROOM_DOESNT_EXIST
	eng "That room doesn't really exist!!\n"
	ser "Ta soba ne postoji!!\n"
	ger "Diesen Raum gibt es nicht wirklich!\n"
	~

CS_NO_VEHICLES_ALLOWED
	eng "Vehicles cannot travel there.\n"
	ser "Vozila ne mogu da idu tamo.\n"
	ger "Transportmittel können dort nicht benutzt werden.\n"
	~

CS_NO_TRAVEL_WATER
	eng "This vehicle can't travel on water.\n"
	ser "Ovo vozilo nemoze da pliva po vodi.\n"
	ger "Dieses Transportmittel kann nicht schwimmen.\n"
	~

CS_NO_TRAVEL_CLIMB
	eng "This vehicle can't climb.\n"
	ser "Ovo vozilo nemoze da se penje.\n"
	ger "Dieses Transportmittel kann nicht klettern.\n"
	~

CS_NO_TRAVEL_DIVE
	eng "This vehicle can't dive.\n"
	ser "Ovo vozilo ne moze da roni.\n"
	ger "Dieses Transportmittel kann nicht tauchen.\n"
	~

CS_NO_TRAVEL_FLY
	eng "This vehicle can't fly.\n"
	ser "Ovo vozilo nemoze da leti.\n"
	ger "Dieses Transportmittel kann nicht fliegen.\n"
	~

CS_NO_TRAVEL_ZLOCKED
	eng "That direction has been locked to vehicles.\n"
	ser "Taj smer je zatvoren za vozila.\n"
	ger "Diese Richtung ist für Transportmittel gesperrt.\n"
	~

CS_DEFINED
	eng " (DEFINED)\n\t"
	ser " (DEFINED)\n\t"
	ger " (DEFINIERT)\n\t"
	~

