# $Id: translations_classes.spec,v 1.2 1999/08/25 06:35:12 greear Exp $
# $Revision: 1.2 $  $Author: greear $ $Date: 1999/08/25 06:35:12 $

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
	~

CS_ROOM_BECOMES_DARK
	eng "The room becomes dark.\n"
	~

CS_ROOM_DOESNT_EXIST
	eng "That room doesn't really exist!!\n"
	~

CS_NO_VEHICLES_ALLOWED
	eng "Vehicles cannot travel there.\n"
	~

CS_NO_TRAVEL_WATER
	eng "This vehicle can't travel on water.\n"
	~

CS_NO_TRAVEL_CLIMB
	eng "This vehicle can't climb.\n"
	~

CS_NO_TRAVEL_DIVE
	eng "This vehicle can't dive.\n"
	~

CS_NO_TRAVEL_FLY
	eng "This vehicle can't fly.\n"
	~

CS_NO_TRAVEL_ZLOCKED
	eng "That direction has been locked to vehicles.\n"
	~

