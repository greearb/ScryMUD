# $Id: translation_battle.spec,v 1.2 2001/03/29 03:02:36 eroper Exp $
# $Revision: 1.2 $  $Author: eroper $ $Date: 2001/03/29 03:02:36 $

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

CS_APPEARS_DEAD
	eng "%S appears in the room looking quite dead!\n"
	ser "%S se pojavio u sobi, izgleda da je mrtav!\n"
	ger "%S erscheint, einen ziemlich toten Eindruck hinterlassend!\n"
	~

CS_YOU_DEAD_NL
	eng "\n\n\n\nYOU ARE DEAD!!!!!!!!!!!\n"
	ser "\n\n\n\nMRTAV SI!!!!!!!!!!\n"
	ger "\n\n\n\nSie sind MAUSETOT!!!\n"
	~

CS_WELCOME_AGAIN
	eng "\nWelcome once again.\n"
	ser "\nDobro dosao jos jednom.\n" 
	ger "\nWillkommen zurück!\n"
	~

CS_FARE_BETTER
	eng "Perhaps this time you'll fare a little better!!\n"
	ser "Mozda ces ovoga puta da budes malo srecniji!!\n"
	ger "Vielleicht klappt's diesmal besser!\n"
	~

CS_END_OF_LUCK
	eng "You have reached the end of your luck! Perhaps your soul will endure longer 
next time. Please feel free to rejoin the Realm in another mortal coil!\n"
	ser "Stigao si do kraja tvoje srece!  Mozda ce tvoja dusa sledeci put da izdrzi duze. 
Osecaj se slobodan da se opet pridruzis i krenes u novu avanturu!\n"
	ger "Sie haben Ihr Glück einmal zuviel herausgefordert! Vielleicht hält Ihre arme Seele 
in Ihrer nächsten Inkarnation länger durch? Versuchen Sie es!\n"
	~

CS_GASP_FAREWELL
	eng "gasps a final farewell to the Realm in which %s has dwelt for %i years!"
	ser "oprasta se po poslednji put od sveta u kojem  %s je ziveo %i godina!"
	ger "scheidet mit einem letzten Lebewohl aus dem Reich, in dem %s %i Jahre lang gelebt hat!"
~

