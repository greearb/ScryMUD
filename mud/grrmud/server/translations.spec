# $Id: translations.spec,v 1.5 1999/06/06 18:15:42 greear Exp $
# $Revision: 1.5 $  $Author: greear $ $Date: 1999/06/06 18:15:42 $

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

# This is used as a kind of NULL enum.  It should never be used
# where a real CSentry entity is desired.
CS_NONE
	eng "Should Never See This"
	~

# Shown when you try to communicate while gagged.
CS_YOU_GAGGED
	English "You have been gagged."
	Spanish "Esta no hace hablar." ~

# Shown when you try to communicate in your sleep.
CS_MUTTER_SLEEP
	Eng "You mutter in your sleep."
	~

CS_YELL_SLEEP
	eng "You roar in your sleep!\n"
	~

CS_SAY_SPRINTF
	eng "%S\\n%S says, '%S'\\n%S"
	spa "%S\\n%S habla, '%S'\\n%S"
	~ 

CS_SAY_OBJ_SPRINTF
	eng "\n####%S says, '%S'\n"
	~

CS_YELL
	eng "%S\n%S yells: '%S'\n%S"
	~

CS_YOU_YELL
	eng "%SYou yell, '%S'\n%S"
	~

CS_GOSSIP_WHAT
	eng "Gossip what??\n"
	~

CS_GOSSIP_NOT_ON_CHANNEL
	eng "You are not on the gossip channel.\n"
	~

CS_GOSSIP
	eng "%S\n%S gossips, '%S'\n%S"
	~

CS_YOU_GOSSIP
	eng "%SYou gossip, '%S'\n%S"
	~

CS_GS
	eng "%S\n%S tells the group, '%S'\n%S"
	~

CS_YOU_GS
	eng "%SYou tell the group, '%S'\n%S"
	~

CS_NO_WIZCHAT_CHANNEL
	eng "You are not on the wizchat channel.\n"
	~

CS_WC
	eng "%S\n%S wizchats, '%S'\n%S"
	~

CS_YOU_WC
	eng "%SYou wizchat, '%S'\n%S"
	~

CS_GS_NOONE_AROUND
	eng "There is no one in your group to talk to now!\n"
	~

CS_NO_AUCTION_CHANNEL
	eng "You are not on the auction channel.\n"
	~

CS_AUCTION
	eng "%S\n%S auctions, '%S'\n%S"
	~

CS_YOU_AUCTION
	eng "%SYou auction, '%S'\n%S"
	~

CS_NO_SHOUT_CHANNEL
	eng "You are not on the shout channel.\n"
	~

CS_SHOUT
	eng "%S\n%S shouts, '%S'\n%S"
	~

CS_YOU_SHOUT
	eng "%SYou shout, '%S'\n%S"
	~

CS_PEMOTE_OBJ_SPRINTF
	eng "#####%S's %S\n"
	~

CS_EMOTE_OBJ_SPRINTF
	eng "#####%S %S\n"
	~

CS_PEMOTE_OBJ_SOMEONE
	eng "#####Someone's %S\n"
	~

CS_EMOTE_OBJ_SOMEONE
	eng "#####Someone %S\n"
	~

CS_PEMOTE_SOMEONE
	eng "Someone's %S\n"
	~

CS_EMOTE_SOMEONE
	eng "Someone %S\n"
	~

CS_PEMOTE
	eng "%S's %S\n"
	~

CS_EMOTE
	eng "%S %S\n"
	~

CS_SAY_SPRINTF_YOU
	eng "%SYou say, '%S'\\n%S"
	spa "%SSe habla, '%S'\\n%S"
	~

CS_YOUR_INV
	eng "Your inventory:  \n"
	~

CS_DROP_WHAT
	eng "Drop what??\n"
	~

CS_OK
	eng "Ok.\n"
	~

CS_DONT_SEEM_TO_HAVE_THAT
	eng "You don't seem to have that.\n"
	~

CS_DONT_SEEM_TO_HAVE
	eng "You don't seem to have the %S.\n"
	~

CS_AINT_GOT
	eng "You neither have nor see the %S.\n"
	~

CS_CANT_FIND
	eng "You can't find the %S.\n"
	~

CS_DONATE_WHAT
	eng "Donate what??\n"
	~

CS_YOU_ARE_WEARING
	eng "You are wearing:\n"
	~

CS_YOU_CANT_FIND_THE
	eng "You can't find the %S.\n"
	~

CS_ITS_CLOSED
	eng "Its closed!\n"
	~

CS_CONTAINS
	eng "%S contains:\n\n"
	~

CS_WEAR_WHAT
	eng "Wear what??\n"
	~

CS_NOTHING_TO_WEAR
	eng "You have nothing to wear!\n"
	~

CS_WEAR_IT_WHERE
	eng "Wear it where??\n"
	~

CS_ALREADY_WEARING_THERE
	eng "You are already using something there.\n"
	~

CS_REMOVE_WHAT
	eng "Remove what??\n"
	~

CS_NOT_WEARING_THAT
	eng "You don't seem to be wearing that.\n"
	~

CS_LOOK_TOO_RELAXED
	eng "\nYou aren't in a lively enough position to look something.\n"
	~

CS_LOOK_ONLY_DARKNESS
	eng "\nAll you see is darkness...\n"
	~

CS_LOOK_CANT_SEE
	eng "\nYou should not be able to see at all!\n"
	~

CS_LOOK_GLOWS_BRIGHTLY
	eng "\t$s glows brightly.\n\n"
	~

CS_LOOK_SKY_CAN_SEE
	eng "You look off into the great blue yonder!\n"
	~

CS_LOOK_SKY_CANT_SEE
	eng "You can't see the sky from here!\n"
	~

CS_NO_CAN_SEE
	eng "You do not see that here.\n"
	~

CS_NO_CAN_SEE_ITEM
	eng "You don't see that item here.\n"
	~

CS_NO_CAN_SEE_PERSON
	eng "You do not see that person.\n"
	~

CS_FIGHT_WHO
	eng "Fight who??\n"
	~

CS_ASSIST_WHO
	eng "Assist who??\n"
	~

CS_ASSIST_ALREADY_TRYING
	eng "You're already giving it your all!\n"
	~

CS_ASSIST_YOURSELF
	eng "You start giving yourself a hard time!\n"
	~

CS_ASSIST_START
	eng "You assist %S in fighting against %S!\n"
	~

CS_ASSIST_CANT
	eng "You cannot assist someone who is not fighting.\n"
	~

CS_HIT_SELF
	eng "You rare back and smack the s**t out of yourself!\n"
	~

CS_HIT_BATTLE_STARTS
	eng "Battle envelops you!\n"
	~

CS_CAST_NO_CAN_DO
	eng "You can't be ordered to cast a spell.\n"
	~

CS_SPELL_RESEARCH
	eng "That spell is in need of research.\n"
	~

CS_NO_CONTAINER	
	eng "You don't seem to have that container.\n"
	~

CS_TOO_LITTLE_GOLD
	eng "You don't have that much gold.\n"
	~

CS_MUST_DROP_MORE_GOLD
	eng "You must drop more than that!!\n"
	~
	
CS_DROP_I_COINS
	eng "You drop %i coins."
	~

CS_NOT_CONTAINER_PUT
	eng "How are you gonna put something in THAT?\n"
	~

CS_NOT_CONTAINER
	eng "That isn't a container.\n"
	~

CS_NO_BULLETIN_BOARD
	eng "You can't get things from that!\n"
	~

CS_NOT_OPEN
	eng "You need to open it first.\n"
	~

CS_TRY_EMPTY
	eng "Try empty instead!\n"
	~

CS_TARG_EMPTY
	eng "The %S appears to be empty.\n"
	~

CS_NO_SELF_CONTAINMENT
	eng "Attempt to self contain denied!!.\n"
	~

CS_NO_SELF_REMOVAL
	eng "Attempt to self extract denied.\n"
	~

CS_PUT_NO_SEE
	eng "You neither see nor have the %S.\n"
	~

CS_NO_SEE_THAT
	eng "You don't see that here.\n"
	~

CS_GET_WHAT
	eng "Get what??\n"
	~

CS_NO_DETECT_ANYTHING
	eng "You can't detect anything...\n"
	~

CS_NO_CONTAIN_OBJ
	eng "The %S doesn't seem to contain the %S.\n"
	~

CS_DEAF_YELL
	eng "You are deaf to yells.\n"
	~

CS_RESTING_ALREADY
	eng "You're already resting.\n"
	~

CS_DO_REST
	eng "You take off your boots and make yourself comfortable.\n"
	~

CS_REST_EMOTE
	eng "sits down to take a breather.\n"
	~

CS_REST_FIGHTING
	eng "You can't rest now, you're fighting!!\n"
	~

CS_BEYOND_RESTING
	eng "You're already way beyond resting!\n"
	~

CS_DO_SIT
	eng "You sit on your haunches, resting but wary.\n"
	~

CS_SIT_EMOTE
	eng "sits down.\n"
	~

CS_SIT_FIGHTING
	eng "You can't sit now, you're fighting!!\n"
	~

CS_BEYOND_SITTING
	eng "You're already way beyond sitting!\n"
	~

CS_ALREADY_STANDING
	eng "You're already standing.\n"
	~

CS_DO_STAND
	eng "You stand up ready to face the world once again.\n"
	~

CS_STAND_EMOTE
	eng "stands up.\n"
	~

CS_NO_STAND
	eng "You can't even FEEL your legs, how can you stand?\n"
	~

CS_WAKE_FIRST
	eng "Maybe you should try to wake first.\n"
	~

CS_SLEEPING_ALREADY
	eng "You're already sleeping.\n"
	~

CS_DO_SLEEP
	eng "You lay yourself down to sleep.\n"
	~

CS_EMOTE_SLEEP
	eng "goes to sleep.\n"
	~

CS_CANT_SLEEP
	eng "You can't sleep now, you're fighting!!\n"
	~

CS_BEYOND_SLEEP
	eng "You're already way beyond sleeping!\n"
	~

CS_ALREADY_MED
	eng "You're already meditating.\n"
	~

CS_DO_MED
	eng "You relax into a deep trance.\n"
	~

CS_MED_EMOTE
	eng "goes into a trance.\n"
	~

CS_MED_NO_CONCENTRATE
	eng "You just can't seem to concentrate...\n"
	~

CS_MED_FIGHTING
	eng "You can't meditate now, you're fighting!!\n"
	~

CS_MED_WAKE_FIRST
	eng "You must wake first.\n"
	~

CS_AWAKE_ALREADY
	eng "You're already awake.\n"
	~

CS_DO_WAKE
	eng "You wake, sit up and stretch.\n"
	~

CS_WAKE_EMOTE
	eng "wakes and sits.\n"
	~

CS_CANT_WAKE
	eng "You can't wake up, don't you just hate that??!!\n"
	~

CS_WAKE_DEAD
	eng "You see a flickering...REDISH??? light coming towards you!\n"
	~

CS_MOVE_NO_MOV
	eng "You can't take one more step!!\n"
	~

CS_MOV_STANDING
	eng "You have to be standing in order to travel.\n"
	~

CS_MOV_FIGHTING
	eng "You are busy fighting.\n"
	~

CS_MOV_NO_EXIT
	eng "You can't discern an exit in that direction.\n"
	~

CS_MOV_CLOSED
	eng "The %S is closed.\n"
	~

CS_MOV_BLOCKED
	eng "The %S is blocked by %S.\n"
	~

CS_BLOCKS_WAY
	eng "%S blocks your way.\n"
	~

CS_CONSUMES_KEY
	eng "You give up %S as you pass through.\n"
	~

CS_MOV_NEED_KEY
	eng "You must have %S in order to pass.\n"
	~

CS_HAS_ARRIVED
	eng "has arrived."
	~

CS_YOU_FOLLOW
	eng "You attempt to follow %S %s."
	~

CS_NO_FIND_EXIT
	eng "You can't discern an exit in that direction.\n"
	~

CS_NO_WEAR
	eng "You can't wear %S.\n"
	~

CS_ALREADY_WEARING
	eng "   %S:%P25You are already using something there.\n"
	~

CS_WOH
	eng "     Worn on head:"
	~

CS_WON
	eng "     Worn around neck:"
	~

CS_WAB
	eng "     Worn around body:"
	~

CS_WOA
	eng "     Worn on arms:"
	~

CS_WOW
	eng "     Worn on wrist:"
	~

CS_WO_HANDS
	eng "     Worn on hands:"
	~

CS_WIELDED
	eng "     Wielded:"
	~

CS_HELD
	eng "     Held:"
	~

CS_LIGHT
	eng "     Used as light:"
	~

CS_WOB
	eng "     Worn on body:"
	~


CS_WAIST
	eng "     Worn around waist:"
	~

CS_WOL
	eng "     Worn on legs:"
	~

CS_WOF
	eng "     Worn on feet:"
	~

CS_WO_FINGER
	eng "     Worn on finger:"
	~

CS_WO_SHIELD
	eng "     Used as shield:"
	~

CS_WO_UNDEFINED
	eng "Used somewhere undefined:\t"
	~

CS_WO_RSLT_VNUM
	eng "%S%P30 [%i]%P37 %S.\n"
	~

CS_WO_RSLT
	eng "%S%P30 %S.\n"
	~

CS_NOT_EXPERIENCED
	eng "You are not experienced enough to use %S.\n"
	~

CS_TOO_HEAVY
	eng "%S is just too heavy for you!\n"
	~
	
CS_CANT_WEAR_THERE
	eng "You can't wear it there.\n"
	~

CS_TOO_EVIL
	eng "You're too evil for your %S!\n"
	~

CS_TOO_NEUTRAL
	eng "You're too neutral for your %S!\n"
	~

CS_TOO_GOOD
	eng "You're too pleasant for your %S!\n"
	~

CS_IMM_ONLY
	eng "%S belongs to an immortal!\n"
	~

CS_DEMI_ONLY
	eng "%S belongs to a demi-god!\n"
	~

CS_GOD_ONLY
	eng "%S belongs to a god!\n"
	~

CS_TOO_RELAXED
	eng "You are too relaxed!\n"
	~

CS_NO_WARRIOR
	eng "Silly warrior, %S is not for you!\n"
	~

CS_NO_SAGE
	eng "Silly sage, %S is not for you!\n"
	~

CS_NO_WIZARD
	eng "Silly wizard, %S is not for you!\n"
	~

CS_NO_RANGER
	eng "Silly ranger, %S is not for you!\n"
	~

CS_NO_THIEF
	eng "Silly thief, %S is not for you!\n"
	~

CS_NO_ALCHEMISTS
	eng "Alchemists can't use %S!\n"
	~

CS_NO_CLERIC
	eng "Silly cleric, %S is not for you!\n"
	~

CS_NO_BARD
	eng "Silly bard, %S is not for you!\n"
	~

CS_NO_MOB
	eng "%S is !MOB equipment!\n"
	~

CS_NO_PC
	eng "%S is for mobs only!\n"
	~

CS_TOO_MUCH_WT
	eng "%S:\tYou can't carry that much weight.\n"
	~

CS_NO_GET_BULLETIN
	eng "You shouldn't be getting bulletin boards, try opurge.\n"
	~

CS_SGT_TOO_MUCH_WT
	eng "%S:\t%S can't carry that much weight.\n"
	~

CS_SGT_NO_MORTAL
	eng "%S is not for mortals!\n"
	~

CS_SGT_NO_DEMI
	eng "%S is not for demi-gods!\n"
	~

CS_SGT_NO_GOD
	eng "%S is not for gods!\n"
	~

CS_SGT_PC_ONLY
	eng "%S is for players only!\n"
	~

CS_SGT_MOB_ONLY
	eng "%S is not for players!\n"
	~

CS_NO_REMOVE
	eng "The %S seems to have grown very attached to you!!\n"
	~

CS_NO_DROP
	eng "Yeech, you can't let go of %S!!\n"
	~

CS_THERE_WERE_COINS
	eng "There were %i coins.\n"
	~

CS_DO_GET
	eng "You get %S.\n"
	~

CS_DO_GET_O
	eng "%S gets %S.\n"
	~

CS_DO_GET_FROM
	eng "You get %S from %S.\n"
	~

CS_DO_GET_FROM_O
	eng "%S gets %S from %s %S.\n"
	~

CS_DO_GET_YOUR
	eng "You get %S from your %S.\n"
	~

CS_DO_GET_YOUR_O
	eng "%S gets %S from %S.\n"
	~

CS_LESS_THIRSTY
	eng "You feel less thirsty.\n"
	~

CS_MORE_THIRSTY
	eng "You feel even more thirsty.\n"
	~

CS_LESS_HUNGRY
	eng "You feel less hungry.\n"
	~

CS_MORE_HUNGRY
	eng "You feel even hungrier.\n"
	~

CS_MORE_DRUGGED
	eng "You feel a little higher.\n"
	~

CS_LESS_DRUGGED
	eng "Some feeling returns to your nose!.\n"
	~

CS_YOU_DROP
	eng "You drop %S.\n"
	~

CS_DROPS
	eng "%S drops %S.\n"
	~

CS_YOU_DONATE
	eng "You donate %S.\n"
	~

CS_DONATES
	eng "%S donates %S.\n"
	~

CS_OBJ_APPEARS
	eng "%S appears in the room with a popping sound.\n"
	~

CS_TRY_FILL
	eng "Try fill instead!\n"
	~

CS_TRY_POST
	eng "Try post instead.\n"
	~

CS_CANT_HOLD_WT
	eng "The %S can't hold the %S.\n"
	~

CS_YOU_PUT
	eng "You put %S in %S.\n"
	~

CS_PUTS
	eng "%S puts %S in %S.\n"
	~

CS_PUTS_IN_OWN
	eng "%S puts %S in %s %S.\n"
	~