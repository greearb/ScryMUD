# $Id$
# $Revision: 1.3 $  $Author$ $Date$

# This should grow to contain almost all constant strings and patterns
# given to Sprintf.  To add support for a new language, then, you will
# just have to modify the code_gen files a bit and then add the
# translation here.
#
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
# Languages currently supported by the code_generation process are:
# English, Spanish, Portugues, Italian

# This is used as a kind of NULL enum.  It should never be used
# where a real CSentry entity is desired.

# See translations.spec for more details...

CS_OLC_INTRO
	eng "Welcome to OnLine Creation.\n\n
You can always type 'quit_olc' to temporarily exit, but be sure to complete
the item before you log off, or it will be lost.  To delete a work in 
progress, you can quit out of olc and then use the 'reset_olc' command.
Check the help if you have questions.

NOTE:  You can only do olc while IN a zone you own now.  Shouldn't be much
of a change for most of you though.
[[press enter to continue]]\n\n"
	
	ser "Dobrodosli u OnLine Creation.\n\n
Mozes uvek da otkucas 'quit_olc' da privremeno izadjes, ali budi sigurand da 
zavrsis sto si poceo pre nego sto se izlogujes ili ce tvoj rad biti izgubljen.
Da izbrises ono sto si zapoceo da radis mozes da prekines i onda da koristis
'reset_olc' komandu.  Pogledaj 'help' ako imas jos neka pitanja.

NAPOMENA: Mozes da koristis olc samo u zonama koje ti posedujes. 
[[pritisni enter da nastavis]]\n\n"

	ger "Willkommen im OLC (OnLine Creation) Bereich!\n
Sie können Ihre Arbeit jederzeit durch Eingabe von 'quit_olc' zeitweise
beenden, aber vergessen Sie nicht, das bearbeitete Objekt fertigzustellen,
bevor Sie sich aus dem System abmelden, da Ihre Arbeit sonst verloren geht.
Um ein begonnenes Werk endgültig abzubrechen, verlassen Sie den OLC-Bereich
mit 'quit_olc' und geben dann das 'reset_olc' Kommando.
Weitere Hinweise finden Sie in der Hilfe!

Hinweis: OLC ist nur möglich, während Sie sich in einer von Ihnen selbst
verwalteten Zone befinden!\n
[[Bitte Return drücken]]\n"
	~

CS_OBJ_NOT_CREATED_YET
	eng "This object has not been created yet.\n"
	~