# $Id: translation4.spec,v 1.1 1999/08/22 07:16:20 greear Exp $
# $Revision: 1.1 $  $Author: greear $ $Date: 1999/08/22 07:16:20 $

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

CS_TITLE_CHANGED
	eng "Title changed.\n"
	~

CS_TITLE_80_ONLY
	eng "Your title must be less than 80 characters.\n"
	~

CS_MESSAGE
	eng "message"
	~

CS_ENTER_MSG_POST
	eng "Enter your message to be posted.
Use a solitary '~' on a line by itself to end.\n"
	~
