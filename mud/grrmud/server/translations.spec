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
# English, Spanish, and Portugues

# Shown when you try to communicate while gagged.
CS_YOU_GAGGED
	English "You have been gagged."
	Spanish "Esta no hace hablar." ~

# Shown when you try to communicate in your sleep.
CS_MUTTER_SLEEP
	Eng "You mutter in your sleep."
	~

CS_SAY_SPRINTF
	eng "%S\\n%S says, '%S'\\n%S"
	spa "%S\\n%S habla, '%S'\\n%S"
	~ 

CS_SAY_SPRINTF_YOU
	eng "%SYou say, '%S'\\n%S"
	spa "%SSe habla, '%S'\\n%S"
	~
