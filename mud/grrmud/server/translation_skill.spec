# $Id$
# $Revision: 1.2 $  $Author$ $Date$

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

CS_TRACK_DUH
        eng "You really just have no idea where to begin!!\n"
	ser "Ti stvarno ne znas gdje da pocnes."
	ger "Sie haben wirklich keine Ahnung, wo Sie anfangen sollen!\n"
        ~


CS_YOU_NOT_MGR	
	eng "You are not the manager of this shopkeeper.\n"
	~

CS_SNEAKING_DUH
   eng "You couldn't sneak up on a sleeping Ogrue!!\n"
   ~

CS_START_SNEAKING
   eng "You start sneaking.\n"
   ~

CS_STOP_SNEAKING
   eng "You stop sneaking around.\n"
   ~

CS_YOU_SNEAKING
   eng "You are sneaking.\n"
   ~
