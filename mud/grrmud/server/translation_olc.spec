# $Id: translation_olc.spec,v 1.2 1999/08/22 07:16:20 greear Exp $
# $Revision: 1.2 $  $Author: greear $ $Date: 1999/08/22 07:16:20 $

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
	eng "Welcome go OnLine Creation.\n\n
You can always type 'quit_olc' to temporarily exit, but be sure to complete
the item before you log off, or it will be lost.  To delete a work in 
progress, you can quit out of olc and then use the 'reset_olc' command.
Check the help if you have questions.

NOTE:  You can only do olc while IN a zone you own now.  Shouldn't be much
of a change for most of you though.
[[press enter to continue]]\n\n"
	~

CS_NAME_NOT_FOUND
	eng "That entity does not have that name (keyword).\n"
	~

CS_NAME_REMOVED
	eng "The name (keyword) was removed from that entity.\n"
	~

CS_NAME_ADDED
	eng "Name added.\n"
	~
