# $Id: translations.spec,v 1.12 1999/06/28 05:35:27 greear Exp $
# $Revision: 1.12 $  $Author: greear $ $Date: 1999/06/28 05:35:27 $

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
        spa "Nunca Deberias Ver Esto"
        ~

# Shown when you try to communicate while gagged.
CS_YOU_GAGGED
        English "You have been gagged."
        Spanish "Esta no hace hablar." ~

CS_SAY_SPRINTF
        eng "%S\\n%S says, '%S'\\n%S"
        spa "%S\\n%S dice, '%S'\\n%S"
        ~ 

# Shown when you try to communicate in your sleep.
CS_MUTTER_SLEEP
        Eng "You mutter in your sleep."
        Spa "Murmuras mientras duermes."
        ~

CS_NEW_PLAYER_INFO_NC
        eng "\007\007\007 INFO: [NEW PLAYER]  %S connecting from: %S\n"
        spa "\007\007\007 INFO: [NEW PLAYER]  %S conectandose desde: %S\n"
        ~

CS_NEW_PLAYER_INFO_C
        eng "<BEEP> INFO: [NEW PLAYER]  %S connecting from: %S\n"
        spa "<BEEP> INFO: [NEW PLAYER]  %S conectandose desde: %S\n"
        ~

CS_PLAYER_REGAINED_INFO
        eng "  INFO:  %S has regained connection from: %S\n"
        spa "  INFO:  %S ha recuperado la conexion desde: %S\n"
        ~

CS_PLAYER_HAS_CONNECTED_INFO
        eng "   INFO:  %S has connected from: %S\n"
        spa "   INFO:  %S se ha conectado desde: %S\n"
        ~

CS_PLAYER_OFF_LD_LIST_INFO
        eng " INFO:  %S has been taken off the link-dead list.\n"
        spa " INFO:  %S ha sido borrado de la lista de conexiones muertas.\n"
        ~

CS_PLAYER_LOST_CON_INFO
        eng " INFO:  %S has lost connection.\n"
        spa " INFO:  %S ha perdido la conexion.\n"
        ~

CS_PKILLED_INFO
        eng "  INFO:  %S has P-KILLED %S in room %i!!\n"
        spa "  INFO:  %S ha ASESINADO-AL-JUGADOR %S en la habitacion %i!!\n"
        ~

CS_KILLED_INFO
        eng "  INFO:  %S was killed by %S in room: %i\n"
        spa "  INFO:  %S fue muerto por %S en la habitacion: %i\n"
        ~

CS_PERM_DEATH_INFO
        eng "%S has permanently died for lack of constitution!\n"
        eng "%S ha muerto permanentemente por falta de constitucion!\n"
        ~

CS_YELL_SLEEP
        eng "You roar in your sleep!\n"
        spa "Ruges mientras duermes!\n"
        ~

CS_SAY_OBJ_SPRINTF
        eng "\n####%S says, '%S'\n"
        spa "\n####%S dice, '%S'\n"
        ~

CS_YELL
        eng "%S\n%S yells: '%S'\n%S"
        spa "%S\n%S grita: '%S'\n%S"
        ~

CS_YOU_YELL
        eng "%SYou yell, '%S'\n%S"
        spa "%SGritas, '%S'\n%S"
        ~

CS_GOSSIP_WHAT
        eng "Gossip what??\n"
        spa "Chismear que??\n"
        ~

CS_GOSSIP_NOT_ON_CHANNEL
        eng "You are not on the gossip channel.\n"
        spa "No estas en el canal de chismes.\n"
        ~

CS_GOSSIP
        eng "%S\n%S gossips, '%S'\n%S"
        spa "%S\n%S chismea, '%S'\n%S"
        ~

CS_YOU_GOSSIP
        eng "%SYou gossip, '%S'\n%S"
        spa "%SChismeas, '%S'\n%S"
        ~

CS_GS
        eng "%S\n%S tells the group, '%S'\n%S"
        spa "%S\n%S le dice al grupo, '%S'\n%S"
        ~

CS_YOU_GS
        eng "%SYou tell the group, '%S'\n%S"
        spa "%SLe dices al grupo, '%S'\n%S"
        ~

CS_NO_WIZCHAT_CHANNEL
        eng "You are not on the wizchat channel.\n"
        spa "No estas en el canal de wizchat.\n"
        ~

CS_WC
        eng "%S\n%S wizchats, '%S'\n%S"
        spa "%S\n%S wizchats, '%S'\n%S"
        ~

CS_YOU_WC
        eng "%SYou wizchat, '%S'\n%S"
        spa "%STu wizchat, '%S'\n%S"
        ~

CS_GS_NOONE_AROUND
        eng "There is no one in your group to talk to now!\n"
        spa "No hay nadie en tu grupo a quien hablarle ahora!\n"
        ~

CS_NO_AUCTION_CHANNEL
        eng "You are not on the auction channel.\n"
        spa "No estas en el canal auction.\n"
        ~

CS_AUCTION
        eng "%S\n%S auctions, '%S'\n%S"
        spa "%S\n%S auctions, '%S'\n%S"
        ~

CS_YOU_AUCTION
        eng "%SYou auction, '%S'\n%S"
        spa "%STu auction, '%S'\n%S"
        ~

CS_NO_SHOUT_CHANNEL
        eng "You are not on the shout channel.\n"
        spa "No estas en el canal de griterio.\n"
        ~

CS_SHOUT
        eng "%S\n%S shouts, '%S'\n%S"
        spa "%S\n%S grita en el canal, '%S'\n%S"
        ~

CS_YOU_SHOUT
        eng "%SYou shout, '%S'\n%S"
        spa "%SGritas en el canal, '%S'\n%S"
        ~

CS_PEMOTE_OBJ_SPRINTF
        eng "#####%S's %S\n"
        spa "#####%S's %S\n"
        ~

CS_EMOTE_OBJ_SPRINTF
        eng "#####%S %S\n"
        spa "#####%S %S\n"
        ~

CS_PEMOTE_OBJ_SOMEONE
        eng "#####Someone's %S\n"
        spa "#####Someone's %S\n"
        ~

CS_EMOTE_OBJ_SOMEONE
        eng "#####Someone %S\n"
        spa "#####Someone %S\n"
        ~

CS_PEMOTE_SOMEONE
        eng "Someone's %S\n"
        spa "Someone's %S\n"
        ~

CS_EMOTE_SOMEONE
        eng "Someone %S\n"
        spa "Alguien %S\n"
        ~

CS_PEMOTE
        eng "%S's %S\n"
        spa "%S's %S\n"
        ~

CS_EMOTE
        eng "%S %S\n"
        spa "%S %S\n"
        ~

CS_SAY_SPRINTF_YOU
        eng "%SYou say, '%S'\\n%S"
        spa "%SSe habla, '%S'\\n%S"
        ~

CS_YOUR_INV
        eng "Your inventory:  \n"
        spa "Tu inventario:  \n"
        ~

CS_DROP_WHAT
        eng "Drop what??\n"
        spa "Dejar que??\n"
        ~

CS_OK
        eng "Ok.\n"
        spa "Hecho.\n"
        ~

CS_UNKNOWN
        eng "UNKNOWN"
        spa "DESCONOCIDO"
        ~

CS_DONT_SEEM_TO_HAVE_THAT
        eng "You don't seem to have that.\n"
        spa "No pareces tener eso.\n"
        ~

CS_DONT_SEEM_TO_HAVE
        eng "You don't seem to have the %S.\n"
        spa "No pareces tener %S.\n"
        ~

CS_AINT_GOT
        eng "You neither have nor see the %S.\n"
        spa "No tienes ni ves %S.\n"
        ~

CS_CANT_FIND
        eng "You can't find the %S.\n"
        spa "No puedes encontrar %S.\n"
        ~

CS_DONATE_WHAT
        eng "Donate what??\n"
        spa "Donar que??\n"
        ~

CS_YOU_ARE_WEARING
        eng "You are wearing:\n"
        spa "Estas vistiendo:\n"
        ~

CS_YOU_CANT_FIND_THE
        eng "You can't find the %S.\n"
        spa "No puedes encontrar %S.\n"
        ~

CS_ITS_CLOSED
        eng "It's closed!\n"
        spa "Esta cerrado!\n"
        ~

CS_CONTAINS
        eng "%S contains:\n\n"
        spa "%S contiene:\n\n"
        ~

CS_WEAR_WHAT
        eng "Wear what??\n"
        spa "Vestir que??\n"
        ~

CS_NOTHING_TO_WEAR
        eng "You have nothing to wear!\n"
        spa "No tienes nada que vestir!\n"
        ~

CS_WEAR_IT_WHERE
        eng "Wear it where??\n"
        spa "Vestir esto donde??\n"
        ~

CS_ALREADY_WEARING_THERE
        eng "You are already using something there.\n"
        spa "Ya estas usando algo ahi.\n"
        ~

CS_REMOVE_WHAT
        eng "Remove what??\n"
        spa "Quitar que??\n"
        ~

CS_NOT_WEARING_THAT
        eng "You don't seem to be wearing that.\n"
        spa "No parece que estuvieras vistiendo eso.\n"
        ~

CS_LOOK_TOO_RELAXED
        eng "\nYou aren't in a lively enough position to look something.\n"
        spa "\nNo estas en una posicion suficientemente vivida para ver algo.\n"
        ~

CS_LOOK_ONLY_DARKNESS
        eng "\nAll you see is darkness...\n"
        spa "\nTodo lo que ves es oscuridad...\n"
        ~

CS_LOOK_CANT_SEE
        eng "\nYou should not be able to see at all!\n"
        spa "\nNo estas capacitado para ver en absoluto!\n"
        ~

CS_LOOK_GLOWS_BRIGHTLY
        eng "\t$s glows brightly.\n\n"
        spa "\t$s brilla intensamente.\n\n"
        ~

CS_LOOK_SKY_CAN_SEE
        eng "You look off into the great blue yonder!\n"
        spa "Miras el gran cielo azul!\n"
        ~

CS_LOOK_SKY_CANT_SEE
        eng "You can't see the sky from here!\n"
        spa "No puedes ver el cielo desde aqui!\n"
        ~

CS_NO_CAN_SEE
        eng "You do not see that here.\n"
        spa "No ves eso aqui.\n"
        ~

CS_NO_CAN_SEE_ITEM
        eng "You don't see that item here.\n"
        spa "No ves ese objeto aqui.\n"
        ~

CS_NO_CAN_SEE_PERSON
        eng "You do not see that person.\n"
        spa "No ves a esa persona.\n"
        ~

CS_FIGHT_WHO
        eng "Fight who??\n"
        spa "Pelear con quien??\n"
        ~

CS_ASSIST_WHO
        eng "Assist who??\n"
        spa "Asistir a quien??\n"
        ~

CS_ASSIST_ALREADY_TRYING
        eng "You're already giving it your all!\n"
        spa "Ya estas dandolo todo!\n"
        ~

CS_ASSIST_YOURSELF
        eng "You start giving yourself a hard time!\n"
        spa "Empiezas a tener un momento duro!\n"
        ~

CS_ASSIST_START
        eng "You assist %S in fighting against %S!\n"
        spa "Asistes a %S en pelear contra %S!\n"
        ~

CS_ASSIST_CANT
        eng "You cannot assist someone who is not fighting.\n"
        spa "No puedes asistir a alguien que no es peleando.\n"
        ~

CS_HIT_SELF
        eng "You rare back and smack the s**t out of yourself!\n"
        spa "You rare back and smack the s**t out of yourself!\n"
        ~

CS_HIT_BATTLE_STARTS
        eng "Battle envelops you!\n"
        spa "La batalla te envuelve!\n"
        ~

CS_CAST_NO_CAN_DO
        eng "You can't be ordered to cast a spell.\n"
        spa "You can't be ordered to cast a spell.\n"
        ~

CS_SPELL_RESEARCH
        eng "That spell is in need of research.\n"
        spa "Ese hechizo necesita aprenderse.\n"
        ~

CS_NO_CONTAINER 
        eng "You don't seem to have that container.\n"
        spa "No pareces tener ese contenedor.\n"
        ~

CS_TOO_LITTLE_GOLD
        eng "You don't have that much gold.\n"
        spa "No tienes esa cantidad de oro.\n"
        ~

CS_MUST_DROP_MORE_GOLD
        eng "You must drop more than that!!\n"
        spa "Debes dejar mas que eso!!\n"
        ~
        
CS_DROP_I_COINS
        eng "You drop %i coins."
        spa "Dejas %i monedas."
        ~

CS_NOT_CONTAINER_PUT
        eng "How are you gonna put something in THAT?\n"
        spa "Como vas a poner algo en ESO?\n"
        ~

CS_NOT_CONTAINER
        eng "That isn't a container.\n"
        spa "Eso no es un contenedor.\n"
        ~

CS_NO_BULLETIN_BOARD
        eng "You can't get things from that!\n"
        spa "No puedes sacar cosas de eso!\n"
        ~

CS_NOT_OPEN
        eng "You need to open it first.\n"
        spa "Necesitas abrirlo primero.\n"
        ~

CS_TRY_EMPTY
        eng "Try empty instead!\n"
        spa "Intenta vaciarlo en todo caso!\n"
        ~

CS_TARG_EMPTY
        eng "The %S appears to be empty.\n"
        spa "%S parece estar vacio.\n"
        ~

CS_NO_SELF_CONTAINMENT
        eng "Attempt to self contain denied!!.\n"
        spa "Attempt to self contain denied!!.\n"
        ~

CS_NO_SELF_REMOVAL
        eng "Attempt to self extract denied.\n"
        spa "Attempt to self extract denied.\n"
        ~

CS_PUT_NO_SEE
        eng "You neither see nor have the %S.\n"
        spa "No ves ni tienes %S.\n"
        ~

CS_NO_SEE_THAT
        eng "You don't see that here.\n"
        spa "No ves eso aqui.\n"
        ~

CS_GET_WHAT
        eng "Get what??\n"
        spa "Tomar que??\n"
        ~

CS_NO_DETECT_ANYTHING
        eng "You can't detect anything...\n"
        spa "No puedes detectar nada...\n"
        ~

CS_NO_CONTAIN_OBJ
        eng "The %S doesn't seem to contain the %S.\n"
        spa "%S no parece contener %S.\n"
        ~

CS_DEAF_YELL
        eng "You are deaf to yells.\n"
        spa "Estas sordo para los gritos.\n"
        ~

CS_RESTING_ALREADY
        eng "You're already resting.\n"
        spa "Ya estas descanzando.\n"
        ~

CS_DO_REST
        eng "You take off your boots and make yourself comfortable.\n"
        spa "Te quitas las botas y te pones comodo.\n"
        ~

CS_REST_EMOTE
        eng "sits down to take a breather.\n"
        spa "se sienta para tomar un respiro.\n"
        ~

CS_REST_FIGHTING
        eng "You can't rest now, you're fighting!!\n"
        spa "No puedes descanzar ahora, estas peleando!!\n"
        ~

CS_BEYOND_RESTING
        eng "You're already way beyond resting!\n"
        spa "Ya estas mas alla de descanzar!\n"
        ~

CS_DO_SIT
        eng "You sit on your haunches, resting but wary.\n"
        spa "Te sientas cruzado de piernas, descanzando pero alerta.\n"
        ~

CS_SIT_EMOTE
        eng "sits down.\n"
        spa "se sienta.\n"
        ~

CS_SIT_FIGHTING
        eng "You can't sit now, you're fighting!!\n"
        spa "No puedes sentarte ahora, estas peleando!!\n"
        ~

CS_BEYOND_SITTING
        eng "You're already way beyond sitting!\n"
        spa "Ya estas mas alla de sentarte!\n"
        ~

CS_STANDING
        eng "is standing here."
        spa "esta parado aqui."
        ~

CS_PRONE
        eng "is lying prone."
        spa "esta tirado."
        ~

CS_SITTING
        eng "is sitting here."
        spa "esta sentado aqui."
        ~

CS_RESTING
        eng "is resting here quietly."
        spa "esta descanzando aqui tranquilamente."
        ~

CS_SLEEPING
        eng "is sleeping here."
        spa "esta durmiendo aqui."
        ~

CS_MEDITATING
        eng "is meditating here."
        spa "esta meditando aqui."
        ~

CS_STUNNED
        eng "is lying here barely breathing."
        spa "esta tirado aqui apenas respirando."
        ~

CS_DEAD
        eng "is lying here utterly dead!"
        spa "esta tirado aqui completamente muerto!"
        ~

CS_ALREADY_STANDING
        eng "You're already standing.\n"
        spa "Ya estas parado.\n"
        ~

CS_DO_STAND
        eng "You stand up ready to face the world once again.\n"
        spa "Te paras para enfrentar al mundo una vez mas.\n"
        ~

CS_STAND_EMOTE
        eng "stands up.\n"
        spa "se para.\n"
        ~

CS_NO_STAND
        eng "You can't even FEEL your legs, how can you stand?\n"
        spa "Nisiquiera puedes SENTIR tus piernas, como puedes pararte?\n"
        ~

CS_WAKE_FIRST
        eng "Maybe you should try to wake first.\n"
        spa "Quizas deberias primero tratar de depertar.\n"
        ~

CS_SLEEPING_ALREADY
        eng "You're already sleeping.\n"
        spa "Ya estas durmiendo.\n"
        ~

CS_DO_SLEEP
        eng "You lay yourself down to sleep.\n"
        spa "Te tiras a dormir.\n"
        ~

CS_EMOTE_SLEEP
        eng "goes to sleep.\n"
        spa "se va a dormir.\n"
        ~

CS_CANT_SLEEP
        eng "You can't sleep now, you're fighting!!\n"
        spa "No puedes dormir ahora, estas peleando!!\n"
        ~

CS_BEYOND_SLEEP
        eng "You're already way beyond sleeping!\n"
        spa "Ya estas mas alla de dormir!\n"
        ~

CS_ALREADY_MED
        eng "You're already meditating.\n"
        spa "Ya estas meditando.\n"
        ~

CS_DO_MED
        eng "You relax into a deep trance.\n"
        spa "Te relajas en un profundo trance.\n"
        ~

CS_MED_EMOTE
        eng "goes into a trance.\n"
        spa "se mete en un trance.\n"
        ~

CS_MED_NO_CONCENTRATE
        eng "You just can't seem to concentrate...\n"
        spa "Simplemente no puedes concentrarte...\n"
        ~

CS_MED_FIGHTING
        eng "You can't meditate now, you're fighting!!\n"
        spa "No puedes meditar ahora, estas peleando!!\n"
        ~

CS_MED_WAKE_FIRST
        eng "You must wake first.\n"
        spa "Primero debes despertar.\n"
        ~

CS_AWAKE_ALREADY
        eng "You're already awake.\n"
        spa "Ya estas despierto.\n"
        ~

CS_DO_WAKE
        eng "You wake, sit up and stretch.\n"
        spa "Despiertas, te paras y te estiras.\n"
        ~

CS_WAKE_EMOTE
        eng "wakes and sits.\n"
        spa "despierta y se para.\n"
        ~

CS_CANT_WAKE
        eng "You can't wake up, don't you just hate that??!!\n"
        spa "No puedes despertar, no odias eso??!!\n"
        ~

CS_WAKE_DEAD
        eng "You see a flickering...REDISH??? light coming towards you!\n"
        eng "You see a flickering...REDISH??? light coming towards you!\n"
        ~

CS_MOVE_NO_MOV
        eng "You can't take one more step!!\n"
        spa "No puedes dar un paso mas!!\n"
        ~

CS_MOV_STANDING
        eng "You have to be standing in order to travel.\n"
        spa "Tienes que estas parado para poder viajar.\n"
        ~

CS_MOV_FIGHTING
        eng "You are busy fighting.\n"
        spa "Estas ocupado peleando.\n"
        ~

CS_MOV_NO_EXIT
        eng "You can't discern an exit in that direction.\n"
        spa "No puedes ver una salida en esa direccion.\n"
        ~

CS_MOV_CLOSED
        eng "The %S is closed.\n"
        spa "%S es cerrada.\n"
        ~

CS_MOV_BLOCKED
        eng "The %S is blocked by %S.\n"
        spa "%S esta bloqueado por %S.\n"
        ~

CS_BLOCKS_WAY
        eng "%S blocks your way.\n"
        spa "%S bloquea tu camino.\n"
        ~

CS_CONSUMES_KEY
        eng "You give up %S as you pass through.\n"
        spa "Das %S mientras pasas a su lado.\n"
        ~

CS_MOV_NEED_KEY
        eng "You must have %S in order to pass.\n"
        spa "Debes tener %S para poder pasar.\n"
        ~

CS_HAS_ARRIVED
        eng "has arrived.\n"
        spa "ha llegado.\n"
        ~

CS_YOU_FOLLOW
        eng "You attempt to follow %S %s.\n"
        spa "Intentas seguir a $S %s.\n"
        ~

CS_NO_FIND_EXIT
        eng "You can't discern an exit in that direction.\n"
        spa "No puedes ver una salida en esa direccion.\n"
        ~

CS_NO_WEAR
        eng "You can't wear %S.\n"
        spa "No puedes vestir %S.\n"
        ~

CS_ALREADY_WEARING
        eng "   %S:%P25You are already using something there.\n"
        spa "   %S:%P25Ya estas usando algo ahi.\n"
        ~

CS_WOH
        eng "     Worn on head:"
        spa "     En la cabeza:"
        ~

CS_WON
        eng "     Worn around neck:"
        spa "     Alrededor del cuello:"
        ~

CS_WAB
        eng "     Worn around body:"
        spa "     Alrededor del cuerpo:"
        ~

CS_WOA
        eng "     Worn on arms:"
        spa "     En los brazos:"
        ~

CS_WOW
        eng "     Worn on wrist:"
        spa "     En las mu~ecas:"
        ~

CS_WO_HANDS
        eng "     Worn on hands:"
        spa "     En las manos:"
        ~

CS_WIELDED
        eng "     Wielded:"
        spa "     Usando:"
        ~

CS_HELD
        eng "     Held:"
        spa "     Sosteniendo:"
        ~

CS_LIGHT
        eng "     Used as light:"
        spa "     Usando como luz:"
        ~

CS_WOB
        eng "     Worn on body:"
        spa "     En el cuerpo:"
        ~


CS_WAIST
        eng "     Worn around waist:"
        spa "     En la cintura:"
        ~

CS_WOL
        eng "     Worn on legs:"
        spa "     En las piernas:"
        ~

CS_WOF
        eng "     Worn on feet:"
        spa "     En los pies:"
        ~

CS_WO_FINGER
        eng "     Worn on finger:"
        spa "     En el dedo:"
        ~

CS_WO_SHIELD
        eng "     Used as shield:"
        spa "     Usando como escudo:"
        ~

CS_WO_UNDEFINED
        eng "Used somewhere undefined:\t"
        spa "Usando en un lugar indefinido:\t"
        ~

CS_WO_RSLT_VNUM
        eng "%S%P30 [%i]%P37 %S.\n"
        spa "%S%P30 [%i]%P37 %S.\n"
        ~

CS_WO_RSLT
        eng "%S%P30 %S.\n"
        spa "%S%P30 %S.\n"
        ~

CS_NOT_EXPERIENCED
        eng "You are not experienced enough to use %S.\n"
        spa "No tienes suficiente experiencia para usar %S.\n"
        ~

CS_TOO_HEAVY
        eng "%S is just too heavy for you!\n"
        spa "%S es demadiado pesado para vos!\n"
        ~
        
CS_CANT_WEAR_THERE
        eng "You can't wear it there.\n"
        spa "No puedes usar eso ahi.\n"
        ~

CS_TOO_EVIL
        eng "You're too evil for your %S!\n"
        spa "Eres demasiado malvado para %S!\n"
        ~

CS_TOO_NEUTRAL
        eng "You're too neutral for your %S!\n"
        spa "Eres demasiado neutral para %S!\n"
        ~

CS_TOO_GOOD
        eng "You're too pleasant for your %S!\n"
        spa "Eres demasiado pacifico para %S!\n"
        ~

CS_IMM_ONLY
        eng "%S belongs to an immortal!\n"
        spa "%S pertenece a un inmortal!\n"
        ~

CS_DEMI_ONLY
        eng "%S belongs to a demi-god!\n"
        spa "%S pertenece a un semi-dios!\n"
        ~

CS_GOD_ONLY
        eng "%S belongs to a god!\n"
        spa "%S pertenece a un dios!\n"
        ~

CS_TOO_RELAXED
        eng "You are too relaxed!\n"
        spa "Estas demasiado relajado!\n"
        ~

CS_NO_WARRIOR
        eng "Silly warrior, %S is not for you!\n"
        spa "Guerrero tonto, %S no es para ti!\n"
        ~

CS_NO_SAGE
        eng "Silly sage, %S is not for you!\n"
        spa "Silly sage, %S is not for you!\n"
        ~

CS_NO_WIZARD
        eng "Silly wizard, %S is not for you!\n"
        spa "Mago tonto, %S no es para ti!\n"
        ~

CS_NO_RANGER
        eng "Silly ranger, %S is not for you!\n"
        spa "Guardabosque tonto, %S no es para ti!\n"
        ~

CS_NO_THIEF
        eng "Silly thief, %S is not for you!\n"
        spa "Ladron tonto, %S no es para ti!\n"
        ~

CS_NO_ALCHEMISTS
        eng "Alchemists can't use %S!\n"
        spa "Los alquimistas no pueden usar %S!\n"
        ~

CS_NO_CLERIC
        eng "Silly cleric, %S is not for you!\n"
        spa "Clerigo tonto, %S no es para ti!\n"
        ~

CS_NO_BARD
        eng "Silly bard, %S is not for you!\n"
        spa "Bardo tonto, %S no es para ti!\n"
        ~

CS_NO_MOB
        eng "%S is !MOB equipment!\n"
        spa "%S is !MOB equipment!\n"
        ~

CS_NO_PC
        eng "%S is for mobs only!\n"
        spa "%S es solamente para mobs!\n"
        ~

CS_TOO_MUCH_WT
        eng "%S:\tYou can't carry that much weight.\n"
        spa "%S:\tNo puedes cargar tanto peso.\n"
        ~

CS_NO_GET_BULLETIN
        eng "You shouldn't be getting bulletin boards, try opurge.\n"
        ~

CS_SGT_TOO_MUCH_WT
        eng "%S:\t%S can't carry that much weight.\n"
        spa "%S:\t%S no puede cargar tanto peso.\n"
        ~

CS_SGT_TOO_MUCH_WT_TARG
        eng "%S can't give you %S because you are carrying too much weight already!"
        spa "%S no puede darte %S porque ya estas cargando demasiado peso!"
        ~

CS_SGT_NO_MORTAL
        eng "%S is not for mortals!\n"
        spa "%S no es para mortales!\n"
        ~

CS_SGT_NO_DEMI
        eng "%S is not for demi-gods!\n"
        spa "%S no es para semi-dioses!\n"
        ~

CS_SGT_NO_GOD
        eng "%S is not for gods!\n"
        spa "%S no es para dioses!\n"
        ~

CS_SGT_PC_ONLY
        eng "%S is for players only!\n"
        spa "%S es para jugadores solamente!\n"
        ~

CS_SGT_MOB_ONLY
        eng "%S is not for players!\n"
        spa "%S no es para jugadores!\n"
        ~

CS_NO_REMOVE
        eng "The %S seems to have grown very attached to you!!\n"
        spa "The %S seems to have grown very attached to you!!\n"
        ~

CS_NO_DROP
        eng "Yeech, you can't let go of %S!!\n"
        spa "Yeech, you can't let go of %S!!\n"
        ~

CS_THERE_WERE_COINS
        eng "There were %i coins.\n"
        spa "Alli habia %i monedas.\n"
        ~

CS_DO_GET
        eng "You get %S.\n"
        spa "Agarras %S.\n"
        ~

CS_DO_GET_O
        eng "%S gets %S.\n"
        spa "%S agarra %S.\n"
        ~

CS_DO_GET_FROM
        eng "You get %S from %S.\n"
        spa "Agarras %S de %S.\n"
        ~

CS_DO_GET_FROM_O
        eng "%S gets %S from %s %S.\n"
        spa "%S agarra %S de %s %S.\n"
        ~

CS_DO_GET_YOUR
        eng "You get %S from your %S.\n"
        spa "Agarras %S de tu %S.\n"
        ~

CS_DO_GET_YOUR_O
        eng "%S gets %S from %S.\n"
        spa "%S agarra %S de %S.\n"
        ~

CS_LESS_THIRSTY
        eng "You feel less thirsty.\n"
        spa "Te sientes con menos sed.\n"
        ~

CS_MORE_THIRSTY
        eng "You feel even more thirsty.\n"
        spa "Te sientes aun con mas sed.\n"
        ~

CS_LESS_HUNGRY
        eng "You feel less hungry.\n"
        spa "Te sientes con menos hambre.\n"
        ~

CS_MORE_HUNGRY
        eng "You feel even hungrier.\n"
        spa "Te sientes aun con mas hambre.\n"
        ~

CS_MORE_DRUGGED
        eng "You feel a little higher.\n"
        spa "Te sientes un poco alto.\n"
        ~

CS_LESS_DRUGGED
        eng "Some feeling returns to your nose!.\n"
        spa "Algunos pensamientos vuelven a tu nariz!.n"
        ~

CS_YOU_DROP
        eng "You drop %S.\n"
        spa "Dejas %S.\n"
        ~

CS_DROPS
        eng "%S drops %S.\n"
        spa "%S deja %S.n"
        ~

CS_YOU_DONATE
        eng "You donate %S.\n"
        spa "Donas %S.\n"
        ~

CS_DONATES
        eng "%S donates %S.\n"
        spa "%S dona %S.\n"
        ~

CS_OBJ_APPEARS
        eng "%S appears in the room with a popping sound.\n"
        spa "%S aparece en la habitacion de un sonido a !POP!.\n"
        ~

CS_TRY_FILL
        eng "Try fill instead!\n"
        spa "Intenta llenar en todo caso!\n"
        ~

CS_TRY_POST
        eng "Try post instead.\n"
        spa "Intenta postear en todo caso.\n"
        ~

CS_CANT_HOLD_WT
        eng "The %S can't hold the %S.\n"
        spa "%S no puede sostener %S.\n"
        ~

CS_YOU_PUT
        eng "You put %S in %S.\n"
        spa "Pones %S en %S.\n"
        ~

CS_PUTS
        eng "%S puts %S in %S.\n"
        spa "%S pone %S en %S.\n"
        ~

CS_PUTS_IN_OWN
        eng "%S puts %S in %s %S.\n"
        spa "%S pone %S en %s %S.\n"
        ~

CS_LEAVES_SPRINTF
        eng "leaves %s.\n"
        spa "se marcha al %s.\n"
        ~

CS_ARRIVED_FROM_SPRINTF
        eng "has arrived from %S.\n"
        spa "ha llegado desde %S.\n"
        ~

CS_ARRIVED_FROM_CUST_SP
        eng "has arrived from the %S.\n"
        spa "ha llegado desde el %S.\n"
        ~


