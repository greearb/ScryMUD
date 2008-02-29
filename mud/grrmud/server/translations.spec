# $Id$
# $Revision$  $Author$ $Date$

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
CS_NONE
        eng "Should Never See This"
        spa "Nunca Deberias Ver Esto"
        ser "Nebi ovo nikada trebao da vidis"
	ger "Wrongness in the fabric of space"
        ~

# Shown when you try to communicate while gagged.
CS_YOU_GAGGED
        English "You have been gagged."
        Spanish "Esta no hace hablar." 
        Serbian "Ne mozes da govoris."
	German  "Sie wurden geknebelt."
        French "Tu ne peux pas parler."
        ~

CS_SAY_SPRINTF
        eng "%S\\n%S says, '%S'\\n%S"
        spa "%S\\n%S dice, '%S'\\n%S"
        ser "%S\\n%S kaze, '%S'\\n%S"
        ger "%S\\n%S sagt: '%S'\\n%S"
        fre "%S\\n%S dit, '%S'\\n%S"
        ~ 

CS_OSAY_SPRINTF
        eng "%S\\n[OOC] %S says, '%S'\\n%S"
        spa "%S\\n[OOC] %S dice, '%S'\\n%S"
        ser "%S\\n[OOC] %S kaze, '%S'\\n%S"
        ger "%S\\n[OOC] %S sagt: '%S'\\n%S"
        fre "%S\\n%S dit, '%S'\\n%S"
        ~ 

# Shown when you try to communicate in your sleep.
CS_MUTTER_SLEEP
        Eng "You mutter in your sleep."
        Spa "Murmuras mientras duermes."
        Ser "Mumlas u snu."
	Ger "Sie murmeln im Schlaf."
        ~

CS_NEW_PLAYER_INFO_NC
        eng "\007\007\007 INFO: [NEW PLAYER]  %S connecting from: %S\n"
        spa "\007\007\007 INFO: [NEW PLAYER]  %S conectandose desde: %S\n"
        ser "\007\007\007 INFO: [NEW PLAYER]  %S konektovan sa: %S\n"
        ger "\007\007\007 INFO: [NEUER SPIELER] %S nimmt Verbindung auf von: %S\n"
	~

CS_NEW_PLAYER_INFO_C
        eng "<BEEP> INFO: [NEW PLAYER]  %S connecting from: %S\n"
        spa "<BEEP> INFO: [NEW PLAYER]  %S conectandose desde: %S\n"
        ser "<BEEP> INFO: [NEW PLAYER]  %S konektovan sa: %S\n"
        ger "<BEEP> INFO: [NEUER SPIELER] %S nimmt Verbindung auf von: %S\n"
        ~

CS_PLAYER_REGAINED_INFO
        eng "  INFO:  %S has regained connection from: %S\n"
        spa "  INFO:  %S ha recuperado la conexion desde: %S\n"
        ser "  INFO:  %S ponovo se konektovao sa: %S\n"
	ger "  INFO:  %S nimmt erneut Verbindung auf von: %S\n"
        ~

CS_PLAYER_HAS_CONNECTED_INFO
        eng "   INFO:  %S has connected from: %S\n"
        spa "   INFO:  %S se ha conectado desde: %S\n"
        ser "   INFO:  %S se konektovao sa: %S\n"
	ger "   INFO:  %S hat Verbindung aufgenommen von: %S\n"
        ~

CS_PLAYER_OFF_LD_LIST_INFO
        eng " INFO:  %S has been taken off the link-dead list.\n"
        spa " INFO:  %S ha sido borrado de la lista de conexiones muertas.\n"
        ser " INFO:  %S je skinut sa liste mrvih linkova.\n"
	ger " INFO:  %S wurde von der Liste unterbrochener Verbindungen entfernt.\n"
        ~

CS_PLAYER_LOST_CON_INFO
        eng " INFO:  %S has lost connection.\n"
        spa " INFO:  %S ha perdido la conexion.\n"
        ser " INFO:  %S je izgubio konekciju.\n"
	ger " INFO:  %S hat die Verbindung verloren.\n"
	  ~

CS_PKILLED_INFO
        eng "  INFO:  %S has P-KILLED %S in room %i!!\n"
        spa "  INFO:  %S ha ASESINADO-AL-JUGADOR %S en la habitacion %i!!\n"
        ser "  INFO:  %S je ubio %S u sobi %i!!\n"
	ger "  INFO:  %S hat %S in Raum %i ermordet!\n"
	  ~

CS_KILLED_INFO
        eng "  INFO:  %S was killed by %S in room: %i\n"
        spa "  INFO:  %S fue muerto por %S en la habitacion: %i\n"
        ser "  INFO:  %S je ubijen. %S ga je ubio u sobi: %i\n"
	ger "  INFO:  %S wurde von %S in Raum %i ermordet.\n"
	~

CS_PERM_DEATH_INFO
        eng "%S has permanently died for lack of constitution!\n"
        spa "%S ha muerto permanentemente por falta de constitucion!\n"
        ser "%S je mrtav zbog slabe fizicke gradje!\n"
	ger "%S ist aufgrund von mangelnder Konstitution endgültig verschieden!\n"
        ~

CS_YELL_SLEEP
        eng "You roar in your sleep!\n"
        spa "Ruges mientras duermes!\n"
        ser "Deres se u snu!\n"
	ger "Sie schreien im Schlaf!\n"
 	~

CS_SAY_OBJ_SPRINTF
        eng "\n####%S says, '%S'\n"
        spa "\n####%S dice, '%S'\n"
        ser "\n####%S kaze, '%S'\n"
        ger "\n####%S sagt: '%S'\n"
	~

CS_YELL
        eng "%S\n%S yells: '%S'\n%S"
        spa "%S\n%S grita: '%S'\n%S"
        ser "%S\n%S vice: '%S'\n%S" 
        ger "%S\n%S ruft: '%S'\n%S" 
	~

CS_YOU_YELL
        eng "%SYou yell, '%S'\n%S"
        spa "%SGritas, '%S'\n%S"
        ser "%STi vices, '%S'\n%S"
	ger "%SSie rufen: '%S'\n%S"
        ~

CS_GOSSIP_WHAT
        eng "Gossip what??\n"
        spa "Chismear que??\n"
        ser "Sta hoces da kazes????\n"
	ger "Was wollen Sie babbeln?\n"
	~

CS_SAY_WHAT
	eng "Say what??\n"
	ser "Sta da kazes??\n"
	ger "Was möchten Sie sagen?\n"
	~

CS_GOSSIP_NOT_ON_CHANNEL
        eng "You are not on the gossip channel.\n"
        spa "No estas en el canal de chismes.\n"
        ser "Nisi na kanalu za ogovaranje.\n"
	ger "Sie beteiligen sich zur Zeit nicht am 'gossip channel'.\n"
	~

CS_GOSSIP
        eng "%S\n%S gossips, '%S'\n%S"
        spa "%S\n%S chismea, '%S'\n%S"
        ser "%S\n%S brblja, '%S'\n%S"
        ger "%S\n%S babbelt: '%S'\n%S"
	~

CS_YOU_GOSSIP
        eng "%SYou gossip, '%S'\n%S"
        spa "%SChismeas, '%S'\n%S"
        ser "%STi brbljas, '%S'\n%S"
        ger "%SSie babbeln: '%S'\n%S"
	~

CS_GS
        eng "%S\n%S tells the group, '%S'\n%S"
        spa "%S\n%S le dice al grupo, '%S'\n%S"
        ser "%S\n%S govori grupi, '%S'\n%S"
        ger "%S\n%S teilt der Gruppe mit: '%S'\n%S"
	~

CS_YOU_GS
        eng "%SYou tell the group, '%S'\n%S"
        spa "%SLe dices al grupo, '%S'\n%S"
        ser "%SLe Ti govoris grupi, '%S'\n%S"
        ger "%SSie teilen der Gruppe mit: '%S'\n%S"
	 ~

CS_NO_WIZCHAT_CHANNEL
        eng "You are not on the wizchat channel.\n"
        spa "No estas en el canal de wizchat.\n"
        ser "Ti si sada na wizchat  kanalu.\n"
	ger "Sie beteiligen sich zur Zeit nicht am 'wizchat channel'.\n"
	~

CS_WC
        eng "%S\n%S wizchats, '%S'\n%S"
        spa "%S\n%S wizchats, '%S'\n%S"
        ser "%S\n%S wizchats, '%S'\n%S"
        ger "%S\n%S wizchattet, '%S'\n%S"
	~

CS_YOU_WC
        eng "%SYou wizchat, '%S'\n%S"
        spa "%STu wizchat, '%S'\n%S"
        ser "%STi wizchat-ujes, '%S'\n%S"
        ger "%SSie wizchatten: '%S'\n%S"
	~

CS_GS_NOONE_AROUND
        eng "There is no one in your group to talk to now!\n"
        spa "No hay nadie en tu grupo a quien hablarle ahora!\n"
        ser "Nikoga nema u tvojoj grupi sa kim bi mogao da pricas!\n"
        ger "Es gibt zur Zeit niemanden in der Gruppe, mit dem Sie reden könnten!\n"
	~

CS_NO_AUCTION_CHANNEL
        eng "You are not on the auction channel.\n"
        spa "No estas en el canal auction.\n"
        ser "Nisi na kanalu za aukcije.\n"
	ger "Sie beteiligen sich zur Zeit nicht am 'auction channel'.\n"
	~

CS_AUCTION
        eng "%S\n%S auctions, '%S'\n%S"
        spa "%S\n%S auctions, '%S'\n%S"
        ser "%S\n%S aukcija, '%S'\n%S"
        ger "%S\n%S auctions: '%S'\n%S"
	~

CS_YOU_AUCTION
        eng "%SYou auction, '%S'\n%S"
        spa "%STu auction, '%S'\n%S"
        ser "%STi stavljas na aukciju, '%S'\n%S"
        ger "%SYou auction: '%S'\n%S"
	~

CS_NO_SHOUT_CHANNEL
        eng "You are not on the shout channel.\n"
        spa "No estas en el canal de griterio.\n"
        ser "Nisi na kanalu za vikanje.\n"
	ger "Sie beteiligen sich zur Zeit nicht am 'shout channel'.\n"
	~

CS_SHOUT
        eng "%S\n%S shouts, '%S'\n%S"
        spa "%S\n%S grita en el canal, '%S'\n%S"
        ser "%S\n%S vice, '%S'\n%S"
	ger "%S\n%S schreit: '%S'\n%S"
	~

CS_YOU_SHOUT
        eng "%SYou shout, '%S'\n%S"
        spa "%SGritas en el canal, '%S'\n%S"
        ser "%STi vices, '%S'\n%S"
        ger "%SSie schreien: '%S'\n%S"
	~

CS_PEMOTE_OBJ_SPRINTF
        eng "#####%S's %S\n"
        spa "#####%S's %S\n"
        ser "#####%S %S\n"
        ger "#####%Ss %S\n"
	~

CS_EMOTE_OBJ_SPRINTF
        eng "#####%S %S\n"
        spa "#####%S %S\n"
        ser "#####%S %S\n"
        ger "#####%S %S\n"
	~

CS_PEMOTE_OBJ_SOMEONE
        eng "#####Someone's %S\n"
        spa "#####Someone's %S\n"
        ser "#####Necije %S\n"
        ger "#####Jemandes %S\n"
	~

CS_EMOTE_OBJ_SOMEONE
        eng "#####Someone %S\n"
        spa "#####Someone %S\n"
        ser "#####Necije %S\n"
        ger "#####Jemand %S\n"
	~

CS_PEMOTE_SOMEONE
        eng "Someone's %S\n"
        spa "Someone's %S\n"
        ser "Necije %S\n"
        ger "Jemandes %S\n"
	~

CS_EMOTE_SOMEONE
        eng "Someone %S\n"
        spa "Alguien %S\n"
        ser "necije %S\n"
        ger "Jemand %S\n"
	~

CS_PEMOTE
        eng "%S's %S\n"
        spa "%S's %S\n"
        ser "%S's %S\n"
        ger "%Ss %S\n"
	~

CS_EMOTE
        eng "%S %S\n"
        spa "%S %S\n"
        ser "%S %S\n"
        ger "%S %S\n"
	~

CS_SAY_SPRINTF_YOU
        eng "%SYou say, '%S'\\n%S"
        spa "%SSe habla, '%S'\\n%S"
	ser "%STi kazes, '%S'\\n%S"
	ser "%SSie sagen: '%S'\\n%S"
        ~

CS_OSAY_SPRINTF_YOU
        eng "%S[OOC] You say, '%S'\\n%S"
        spa "%S[OOC] Se habla, '%S'\\n%S"
	ser "%S[OOC] Ti kazes, '%S'\\n%S"
	ser "%S[OOC] Sie sagen: '%S'\\n%S"
        ~

CS_YOUR_INV
        eng "Your inventory:  \n"
        spa "Tu inventario:  \n"
        ser "Tvoj inventar:  \n"
        ger "Ihr Inventar:\n"
	~

CS_DROP_WHAT
        eng "Drop what??\n"
        spa "Dejar que??\n"
        ser "Ispusti sta??\n"
	ger "WAS ablegen?\n"
	~

CS_JUNK_ALL
	eng "You junk everything you're holding. Time to start fresh!\n"		~

CS_CANT_JUNK
	eng "You can't junk %S.\n"
	~

CS_NOTHING_IN_INV_TO_JUNK
	eng "You aren't holding anything you can junk.\n"
	~

CS_NOTHING_TO_JUNK
	eng "You don't have anything like that.\n"
	~

CS_JUNK_REWARD
	eng "The gods reward you.\n"
	~

CS_OK
        eng "Ok.\n"
        spa "Hecho.\n"
        ser "Ok.\n"
        ger "Ok.\n"
	~

CS_UNKNOWN
        eng "UNKNOWN"
        spa "DESCONOCIDO"
        ser "NEPOZNATO"
        ger "UNBEKANNT"
	~

CS_DONT_SEEM_TO_HAVE_THAT
        eng "You don't seem to have that.\n"
        spa "No pareces tener eso.\n"
        ser "Nemas taj predmed.\n"
        fre "Tu n'y as pas.\n"
	~

CS_DONT_SEEM_TO_HAVE
        eng "You don't seem to have the %S.\n"
        spa "No pareces tener %S.\n"
        ser "Nemas %S.\n"
        fre "Tu n'as pas le %S.\n"
	  ~

CS_AINT_GOT
        eng "You neither have nor see the %S.\n"
        spa "No tienes ni ves %S.\n"
        ser "Izgleda da nemas ili nevidis %S.\n"
        ~

CS_CANT_FIND
        eng "You can't find the %S.\n"
        spa "No puedes encontrar %S.\n"
        ser "Nemozes da nadjes %S.\n"
        fre "Tu ne trouves pas le %S.\n"
	  ~
	

CS_DONATE_WHAT
        eng "Donate what??\n"
        spa "Donar que??\n"
        ser "Pokloni sta??\n"
	  ~

CS_YOU_ARE_WEARING
        eng "You are wearing:\n"
        spa "Estas vistiendo:\n"
        ser "Ti nosis:\n"
	~

CS_YOU_CANT_FIND_THE
        eng "You can't find the %S.\n"
        spa "No puedes encontrar %S.\n"
        ser "Nemozes da nadjes %S.\n"
        fre "Tu ne trouves pas le %S.\n"
	~

CS_ITS_CLOSED
        eng "It's closed!\n"
        spa "Esta cerrado!\n"
        ser "Zatvoreno!\n"
        fre "C'est couvert!\n"
	~

CS_CONTAINS
        eng "%S contains:\n\n"
        spa "%S contiene:\n\n"
        ser "%S sadrzi:\n\n"
	~

CS_WEAR_WHAT
        eng "Wear what??\n"
        spa "Vestir que??\n"
        ser "Obuci sta??\n"
	~

CS_NOTHING_TO_WEAR
        eng "You have nothing to wear!\n"
        spa "No tienes nada que vestir!\n"
        ser "Nemas sta da obuces!\n"
        fre "Tu n'as rien pour mettre!\n"
	~

CS_WEAR_IT_WHERE
        eng "Wear it where??\n"
        spa "Vestir esto donde??\n"
        ser "Kako da obuces??\n"
	~

CS_ALREADY_WEARING_THERE
        eng "You are already using something there.\n"
        spa "Ya estas usando algo ahi.\n"
        ser "Vec nosis nesto tamo.\n"
	~

CS_NEEDS_TWO_HANDS
        eng "This weapon requires both hands.\n"
        spa "This weapon requires both hands.\n"
        ser "This weapon requires both hands.\n"
	~

CS_USING_TWO_HANDER
       eng "The weapon you are using requires both hands.\n"
       spa "The weapon you are using requires both hands.\n"
       ser "The weapon you are using requires both hands.\n"
       ~

CS_REMOVE_WHAT
        eng "Remove what??\n"
        spa "Quitar que??\n"
        ser "Sta da skines??\n"
	~

CS_NOT_WEARING_THAT
        eng "You don't seem to be wearing that.\n"
        spa "No parece que estuvieras vistiendo eso.\n"
        ser "Izgleda da nenosis to.\n"
	~

CS_LOOK_TOO_RELAXED
        eng "\nYou aren't in a lively enough position to look at anything.\n"
        spa "\nNo estas en una posicion suficientemente vivida para ver algo.\n"
        ser "\nPrevise si opusten da bi gledao u bilo sta.\n"
	~

CS_LOOK_ONLY_DARKNESS
        eng "\nAll you see is darkness...\n"
        spa "\nTodo lo que ves es oscuridad...\n"
        ser "\nSve sto vidis je tama...\n"
	~

CS_LOOK_CANT_SEE
        eng "\nYou should not be able to see at all!\n"
        spa "\nNo estas capacitado para ver en absoluto!\n"
        ser "\nNisi u stanju da vidis bilo sta!\n"
        fre "Tu ne dois pas pouvoir pour voir rien!\n"
	~

CS_LOOK_GLOWS_BRIGHTLY
        eng "\t%s glows brightly.\n\n"
        spa "\t%s brilla intensamente.\n\n"
        ser "\t%s blistavo svetli.\n\n"
	~

CS_LOOK_SKY_CAN_SEE
        eng "You look off into the great blue yonder!\n"
        spa "Miras el gran cielo azul!\n"
        ser "Gledas u veliko plavo nebo!\n"
	~

CS_LOOK_SKY_CANT_SEE
        eng "You can't see the sky from here!\n"
        spa "No puedes ver el cielo desde aqui!\n"
        ser "Nemozes da vidis nebo odavde!\n"
	~

CS_NO_CAN_SEE
        eng "You do not see that here.\n"
        spa "No ves eso aqui.\n"
        ser "Nevidis to ovde.\n"
	~

CS_NO_CAN_SEE_ITEM
        eng "You don't see that item here.\n"
        spa "No ves ese objeto aqui.\n"
        ser "Nevidis taj objekat ovde.\n"
	~

CS_NO_CAN_SEE_PERSON
        eng "You do not see that person.\n"
        spa "No ves a esa persona.\n"
        ser "Nevidis tu osobu.\n"
	~

CS_FIGHT_WHO
        eng "Fight who??\n"
        spa "Pelear con quien??\n"
        ser "Koga hoces da napadnes??\n"
	~

CS_ASSIST_WHO
        eng "Assist who??\n"
        spa "Asistir a quien??\n"
        ser "Kome hoces da asistiras??\n"
	~

CS_ASSIST_ALREADY_TRYING
        eng "You're already giving it your all!\n"
        spa "Ya estas dandolo todo!\n"
        ser "Vec dajes sve od sebe!\n"
        fre "Tu essaies deja bien!\n"
	~

CS_ASSIST_YOURSELF
        eng "You start giving yourself a hard time!\n"
        spa "Empiezas a tener un momento duro!\n"
        ser "Pocinjes da gnjavis sam sebe!\n"
	~

CS_ASSIST_START
        eng "You assist %S in fighting against %S!\n"
        spa "Asistes a %S en pelear contra %S!\n"
        ser "Asistiras %S u borbi protiv %S!\n"
	~

CS_ASSIST_CANT
        eng "You cannot assist someone who is not fighting.\n"
        spa "No puedes asistir a alguien que no es peleando.\n"
        ser "Nemozes da asistiras nekome ko se ne bori.\n"
	~

CS_HIT_SELF
        eng "You rare back and smack the s**t out of yourself!\n"
        spa "You rare back and smack the s**t out of yourself!\n"
        ser "Podizes ruku i razbijas sebe pod sred glave!\n"
	~

CS_HIT_BATTLE_STARTS
        eng "Battle envelops you!\n"
        spa "La batalla te envuelve!\n"
        ser "Pocinjes da se boris!\n"
	~

CS_CAST_NO_CAN_DO
        eng "You can't be ordered to cast a spell.\n"
        spa "You can't be ordered to cast a spell.\n"
        ser "Nemozes da koristis carolije.\n"
	~

CS_SPELL_RESEARCH
        eng "That spell is in need of research.\n"
        spa "Ese hechizo necesita aprenderse.\n"
        ser "Moras jos da proucis tu caroliju.\n"
	~

CS_NO_CONTAINER 
        eng "You don't seem to have that container.\n"
        spa "No pareces tener ese contenedor.\n"
        ser "Izgleda da nemas taj kontejner.\n"
	~

CS_TOO_LITTLE_GOLD
        eng "You don't have that much gold.\n"
        spa "No tienes esa cantidad de oro.\n"
        ser "Nemas toliko zlata.\n"
	~

CS_MUST_DROP_MORE_GOLD
        eng "You must drop more than that!!\n"
        spa "Debes dejar mas que eso!!\n"
        ser "Moras da ispustis vise od toga!!\n"
	~
        
CS_DROP_I_COINS
        eng "You drop %i coins.\n"
        spa "Dejas %i monedas.\n"
        ser "Ispustio si %i dukata.\n"
	~

CS_DROPS_I_COINS
        eng "drops %i coins.\n"
        ~

CS_PUT_I_COINS
        eng "You put %i coins in %S.\n"
	~

CS_AMT_MUST_BE_GREATER
	eng "The amount needs to be greater than zero!\n"
	~

CS_NO_HAVE_THAT_MUCH
	eng "You don't have that much!\n"
	~

CS_NOT_CONTAINER_PUT
        eng "How are you gonna put something in THAT?\n"
        spa "Como vas a poner algo en ESO?\n"
        ser "Kako planiras da stavis to ovde?\n"
	~

CS_NOT_CONTAINER
        eng "That isn't a container.\n"
        spa "Eso no es un contenedor.\n"
        ser "To nije kontejner.\n"
	~

CS_NO_BULLETIN_BOARD
        eng "You can't get things from that!\n"
        spa "No puedes sacar cosas de eso!\n"
        ser "Nemozes da uzmes stvari odatle!\n"
	~

CS_NOT_OPEN
        eng "You need to open it first.\n"
        spa "Necesitas abrirlo primero.\n"
        ser "Moras da otvoris prvo.\n"
	fre "Il faut que tu y ouvres premiere.\n"
        ~

CS_TRY_EMPTY
        eng "Try empty instead!\n"
        spa "Intenta vaciarlo en todo caso!\n"
        ser "Pokusaj da ispraznis prvo!\n"
	~

CS_TARG_EMPTY
        eng "The %S appears to be empty.\n"
        spa "%S parece estar vacio.\n"
        ser "%S je prazno.\n"
	~

CS_NO_SELF_CONTAINMENT
        eng "Attempt to self contain denied!!.\n"
        spa "Attempt to self contain denied!!.\n"
        ser "Pokusaj odbijen!!.\n"
	~

CS_NO_SELF_REMOVAL
        eng "Attempt to self extract denied.\n"
        spa "Attempt to self extract denied.\n"
        ser "Nemoguce!.\n"
	~

CS_PUT_NO_SEE
        eng "You neither see nor have the %S.\n"
        spa "No ves ni tienes %S.\n"
        ser "Ili nevidis ili nemas %S.\n"
	~

CS_NO_SEE_THAT
        eng "You don't see that here.\n"
        spa "No ves eso aqui.\n"
        ser "Nevidis to ovde.\n"
	~

CS_GET_WHAT
        eng "Get what??\n"
        spa "Tomar que??\n"
        ser "Sta da uzmes??\n"
	~

CS_NO_DETECT_ANYTHING
        eng "You can't detect anything...\n"
        spa "No puedes detectar nada...\n"
        ser "Nemozes da detektujes nista...\n"
	~

CS_NO_CONTAIN_OBJ
        eng "The %S doesn't seem to contain the %S.\n"
        spa "%S no parece contener %S.\n"
        ser "%S  ne sadrzi %S.\n"
	~

CS_DEAF_YELL
        eng "You are deaf to yells.\n"
        spa "Estas sordo para los gritos.\n"
        ser "Necujes vikanje.\n"
	~

CS_RESTING_ALREADY
        eng "You're already resting.\n"
        spa "Ya estas descanzando.\n"
        ser "Vec se odmaras.\n"
	~

CS_DO_REST
        eng "You take off your boots and make yourself comfortable.\n"
        spa "Te quitas las botas y te pones comodo.\n"
        ser "Skidas cizme i udobno se smestas.\n"
	~

CS_REST_EMOTE
        eng "sits down to take a breather."
        spa "se sienta para tomar un respiro."
        ser "seda na zemlju da se malo odmori."
        ~

CS_REST_FIGHTING
        eng "You can't rest now, you're fighting!!\n"
        spa "No puedes descanzar ahora, estas peleando!!\n"
        ser "Nemozes sada da se odmaras, boris se!!!\n"
	~

CS_BEYOND_RESTING
        eng "You're already way beyond resting!\n"
        spa "Ya estas mas alla de descanzar!\n"
        ser "Vec si se naodmarao!\n"
	~

CS_DO_SIT
        eng "You sit on your haunches, resting but wary.\n"
        spa "Te sientas cruzado de piernas, descanzando pero alerta.\n"
        ser "Oprezno sedas da se odmoris.\n"
	~

CS_SIT_EMOTE
        eng "sits down."
        spa "se sienta."
        ser "seda na zemlju."
	~

CS_SIT_FIGHTING
        eng "You can't sit now, you're fighting!!\n"
        spa "No puedes sentarte ahora, estas peleando!!\n"
        ser "Nemozes da sednes sada, boris se!!!\n"
	~

CS_BEYOND_SITTING
        eng "You're already way beyond sitting!\n"
        spa "Ya estas mas alla de sentarte!\n"
        ser "Vec si se nasedeo dovoljno!\n"
	~

CS_STANDING
        eng "is standing here."
        spa "esta parado aqui."
        ser "stoji ovde."
	~

CS_PRONE
        eng "is lying prone."
        spa "esta tirado."
        ser "opruzen na zemlji lezi."
	~

CS_SITTING
        eng "is sitting here."
        spa "esta sentado aqui."
        ser "sedi ovde."
	~

CS_RESTING
        eng "is resting here quietly."
        spa "esta descanzando aqui tranquilamente."
        ser "se tiho odmara ovde."
	~

CS_SLEEPING
        eng "is sleeping here."
        spa "esta durmiendo aqui."
        ser "spava ovde."
	~

CS_MEDITATING
        eng "is meditating here."
        spa "esta meditando aqui."
        ser "meditira ovde."
	~

CS_STUNNED
        eng "is lying here barely breathing."
        spa "esta tirado aqui apenas respirando."
        ser "lezi ovde tesko disuci."
	~

CS_FLYING
        eng "is floating here."
        spa "esta flotando aqui."
        ser "is floating here."
	~

CS_DEAD
        eng "is lying here utterly dead!"
        spa "esta tirado aqui completamente muerto!"
        ser "lezi ovde potpuno mrtav!"
        ~

CS_PARALYZED
        eng "lies here frozen in stone."
        ser "lezi ovde zamrznut u kamenu."
        ~

CS_ALREADY_STANDING
        eng "You're already standing.\n"
        spa "Ya estas parado.\n"
        ser "Vec stojis.\n"
	~

CS_DO_STAND
        eng "You stand up ready to face the world once again.\n"
        spa "Te paras para enfrentar al mundo una vez mas.\n"
        ser "Ustajes spreman da se suocis sa svetom jos jednom.\n"
	~

CS_STAND_EMOTE
        eng "stands up."
        spa "se para."
        ser "ustaje."
	~

CS_NO_STAND
        eng "You can't even FEEL your legs, how can you stand?\n"
        spa "Nisiquiera puedes SENTIR tus piernas, como puedes pararte?\n"
        ser "Nemozes ni da osetis svoje noge, kako ces da ustanes?\n"
	~

CS_WAKE_FIRST
        eng "Maybe you should try to wake first.\n"
        spa "Quizas deberias primero tratar de depertar.\n"
        ser "Mozda bi trebalo da se probudis prvo?.\n"
        fre "Peut etre tu dois essayer te reveiller premiere.\n"
	~

CS_SLEEPING_ALREADY
        eng "You're already sleeping.\n"
        spa "Ya estas durmiendo.\n"
        ser "Vec spavas.\n"
	fre "Tu es deja dormi.\n"
        ~

CS_DO_SLEEP
        eng "You lay yourself down to sleep.\n"
        spa "Te tiras a dormir.\n"
        ser "Lezes da spavas.\n"
	fre "Tu te couches.\n"
        ~

CS_EMOTE_SLEEP
        eng "goes to sleep."
        spa "se va a dormir."
        ser "leze da spava."
	~

CS_CANT_SLEEP
        eng "You can't sleep now, you're fighting!!\n"
        spa "No puedes dormir ahora, estas peleando!!\n"
        ser "Nemozes da spavas sada, boris se!!!\n"
	~

CS_BEYOND_SLEEP
        eng "You're already way beyond sleeping!\n"
        spa "Ya estas mas alla de dormir!\n"
        ser "Vec si se dosta naspavao!\n"
	~

CS_ALREADY_MED
        eng "You're already meditating.\n"
        spa "Ya estas meditando.\n"
        ser "Vec meditiras.\n"
	~

CS_DO_MED
        eng "You relax into a deep trance.\n"
        spa "Te relajas en un profundo trance.\n"
        ser "Opustas se padas u duboki trans.\n"
	~

CS_MED_EMOTE
        eng "goes into a trance."
        spa "se mete en un trance."
        ser "pada u trans."
	~

CS_MED_NO_CONCENTRATE
        eng "You just can't seem to concentrate...\n"
        spa "Simplemente no puedes concentrarte...\n"
        ser "Nepolazi ti za rukom da se koncentrises...\n"
	~

CS_MED_FIGHTING
        eng "You can't meditate now, you're fighting!!\n"
        spa "No puedes meditar ahora, estas peleando!!\n"
        ser "Nemozes da meditiras sada, boris se!!!\n"
	~

CS_MED_WAKE_FIRST
        eng "You must wake first.\n"
        spa "Primero debes despertar.\n"
        ser "Moras da se probudis prvo.\n"
	fre "Il faut que tu te reveilles premiere.\n"
        ~

CS_AWAKE_ALREADY
        eng "You're already awake.\n"
        spa "Ya estas despierto.\n"
        ser "Vec si budan.\n"
	~

CS_DO_WAKE
        eng "You wake, sit up and stretch.\n"
        spa "Despiertas, te paras y te estiras.\n"
        ser "Budis se, sedas i teglis se.\n"
	~

CS_WAKE_EMOTE
        eng "wakes and sits.\n"
        spa "despierta y se para.\n"
        ser "se budi i seda.\n"
	~

CS_CANT_WAKE
        eng "You can't wake up, don't you just hate that?\n"
        spa "No puedes despertar, no odias eso?\n"
        ser "Nemozes da se probudis, zar nemrzis taj osecaj?\n"
	~

CS_WAKE_DEAD
        eng "You see a flickering...REDISH??? light coming towards you!\n"
        eng "You see a flickering...REDISH??? light coming towards you!\n"
        ser "Vidis svetlucanje...  svetlo ti se polako priblizava!\n"
	~

CS_MOVE_NO_MOV
        eng "You can't take one more step!!\n"
        spa "No puedes dar un paso mas!!\n"
        ser "Nemozes da napravis ni jedan korak vise!!\n"
	~

CS_MOV_STANDING
        eng "You have to be standing in order to travel.\n"
        spa "Tienes que estas parado para poder viajar.\n"
        ser "Moras da stojis ako hoces da hodas.\n"
	~

CS_MOV_FIGHTING
        eng "You are busy fighting.\n"
        spa "Estas ocupado peleando.\n"
        ser "Zauzet si borbom.\n"
	~

CS_MOV_NO_EXIT
        eng "You can't discern an exit in that direction.\n"
        spa "No puedes ver una salida en esa direccion.\n"
        ser "Nemozes da nadjes izlas u tom smeru.\n"
	~

CS_MOV_CLOSED
        eng "The %S is closed.\n"
        spa "%S es cerrada.\n"
        ser "%S su zatvorena.\n"
	fre "%S est couvert.\n"
        ~

CS_MOV_BLOCKED
        eng "The %S is blocked by %S.\n"
        spa "%S esta bloqueado por %S.\n"
        ser "%S je blokiran. %S blokira put.\n"
	~

CS_BLOCKS_WAY
        eng "%S blocks your way.\n"
        spa "%S bloquea tu camino.\n"
        ser "%S ti blokira put.\n"
	~

CS_CONSUMES_KEY
        eng "You give up %S as you pass through.\n"
        spa "Das %S mientras pasas a su lado.\n"
        ser "Dajes  %S u prolazu.\n"
	~

CS_MOV_NEED_KEY
        eng "You must have %S in order to pass.\n"
        spa "Debes tener %S para poder pasar.\n"
        ser "Moras da imas %S da bi prosao.\n"
	~

CS_HAS_ARRIVED
        eng "has arrived.\n"
        spa "ha llegado.\n"
        ser "je dosao.\n"
        fre "est arrive.\n"
	~

CS_YOU_FOLLOW
        eng "You attempt to follow %S %s.\n"
        spa "Intentas seguir a %S %s.\n"
        ser "Pokusavas da pratis %S %s.\n"
        fre "Tu essaies suivre %S %s.\n"
	~

CS_NO_FIND_EXIT
        eng "You can't discern an exit in that direction.\n"
        spa "No puedes ver una salida en esa direccion.\n"
        ser "Nemozes da nadjes izlaz u tom pravcu.\n"
	~

CS_NO_WEAR
        eng "You can't wear %S.\n"
        spa "No puedes vestir %S.\n"
        ser "Nemozes da obuces %S.\n"
	~

CS_ALREADY_WEARING
        eng "   %S:%P25You are already using something there.\n"
        spa "   %S:%P25Ya estas usando algo ahi.\n"
        ser "   %S:%P25Vec imas obuceno nesto tamo.\n"
	~

CS_WOH
        eng "     Worn on head:"
        spa "     En la cabeza:"
        ser "     Nosis na glavi:"
        fre "     Sur la tete:"
	~

CS_WON
        eng "     Worn around neck:"
        spa "     Alrededor del cuello:"
        ser "     Nosis oko vrata:"
	~

CS_WAB
        eng "     Worn around body:"
        spa "     Alrededor del cuerpo:"
        ser "     Nosis oko tela:"
	~

CS_WOA
        eng "     Worn on arms:"
        spa "     En los brazos:"
        ser "     Nosis oko ruku:"
        fre "     Sur les bras:"
	~

CS_WOW
        eng "     Worn on wrist:"
        spa "     En las mu~ecas:"
        ser "     Nosis oko zglobova:"
	~

CS_WO_HANDS
        eng "     Worn on hands:"
        spa "     En las manos:"
        ser "     Nosis na rukama:"
        fre "     Sur les mains:"
	~

CS_WIELDED
        eng "     Wielded:"
        spa "     Usando:"
        ser "     Drzis:"
	~

CS_HELD
        eng "     Held:"
        spa "     Sosteniendo:"
        ser "     Nosis u ruci:"
	~

CS_LIGHT
        eng "     Used as light:"
        spa "     Usando como luz:"
        ser "     Koristis za svetlost:"
        fre "     Utilise comme lumiere:"
	~

CS_WOB
        eng "     Worn on body:"
        spa "     En el cuerpo:"
        ser "     Nosis na telu:"
        fre "     Sur le corps:"
	~


CS_WAIST
        eng "     Worn around waist:"
        spa "     En la cintura:"
        ser "     Nosis oko pojasa:"
	~

CS_WOL
        eng "     Worn on legs:"
        spa "     En las piernas:"
        ser "     Nosis oko nogu:"
	~

CS_WOF
        eng "     Worn on feet:"
        spa "     En los pies:"
        ser "     Nosis na nogama:"
        fre "     Sur les pieds:"
	~

CS_WO_FINGER
        eng "     Worn on finger:"
        spa "     En el dedo:"
        ser "     Nosis na prstu:"
        fre "     Sur le doigt:"
	~

CS_WO_SHIELD
        eng "     Used as shield:"
        spa "     Usando como escudo:"
        ser "     Koristis kao stit:"
	~

CS_WO_UNDEFINED
        eng "Used somewhere undefined:\t"
        spa "Usando en un lugar indefinido:\t"
        ser "Nosis negde neidentifikovano gde:\t"
	~

CS_WO_RSLT_VNUM
        eng "%S%P30 [%i]%P37 %S.\n"
        spa "%S%P30 [%i]%P37 %S.\n"
        ser "%S%P30 [%i]%P37 %S.\n"
        fre "%S%P30 [%i]%P37 %S.\n"
	~

CS_WO_RSLT
        eng "%S%P30 %S.\n"
        spa "%S%P30 %S.\n"
        ser "%S%P30 %S.\n"
        fre "%S%P30 %S.\n"
	~

CS_NOT_EXPERIENCED
        eng "You are not experienced enough to use %S.\n"
        spa "No tienes suficiente experiencia para usar %S.\n"
        ser "Nisi dovoljno iskusan da bi koristio %S.\n"
	~

CS_TOO_HEAVY
        eng "%S is just too heavy for you!\n"
        spa "%S es demadiado pesado para vos!\n"
        ser "%S je suvise tesko za tebe!\n"
	~
        
CS_CANT_WEAR_THERE
        eng "You can't wear it there.\n"
        spa "No puedes usar eso ahi.\n"
        ser "Nemozes da nosis to tu.\n"
	~

CS_TOO_EVIL
        eng "You're too evil for your %S!\n"
        spa "Eres demasiado malvado para %S!\n"
        ser "Ti si previse zao za  %S!\n"
	~

CS_TOO_NEUTRAL
        eng "You're too neutral for your %S!\n"
        spa "Eres demasiado neutral para %S!\n"
        ser "Ti si previse neutralan za  %S!\n"
	~

CS_TOO_GOOD
        eng "You're too pleasant for your %S!\n"
        spa "Eres demasiado pacifico para %S!\n"
        ser "Ti si previse fin za  %S!\n"
	~

CS_IMM_ONLY
        eng "%S belongs to an immortal!\n"
        spa "%S pertenece a un inmortal!\n"
        ser "%S pripada besmrtnicima!\n"
	~

CS_DEMI_ONLY
        eng "%S belongs to a demi-god!\n"
        spa "%S pertenece a un semi-dios!\n"
        ser "%S pripada polu-bogovima!\n"
	~

CS_GOD_ONLY
        eng "%S belongs to a god!\n"
        spa "%S pertenece a un dios!\n"
        ser "%S pripada bogu!\n"
	~

CS_TOO_RELAXED
        eng "You are too relaxed!\n"
        spa "Estas demasiado relajado!\n"
        ser "previse si opusten!\n"
	~

CS_NO_WARRIOR
        eng "Silly warrior, %S is not for you!\n"
        spa "Guerrero tonto, %S no es para ti!\n"
        ser "Glupi ratnice, %S nije za tebe!\n"
	~

CS_NO_SAGE
        eng "Silly sage, %S is not for you!\n"
        spa "Silly sage, %S is not for you!\n"
        ser "Glupi Mudrace, %S nije za tebe!\n"
	~

CS_NO_WIZARD
        eng "Silly wizard, %S is not for you!\n"
        spa "Mago tonto, %S no es para ti!\n"
        ser "Glupi carobnjaku %S nije za tebe!\n"
	~

CS_NO_RANGER
        eng "Silly ranger, %S is not for you!\n"
        spa "Guardabosque tonto, %S no es para ti!\n"
        ser "Glupi rendzeru, %S nije za tebe!\n"
	~

CS_NO_THIEF
        eng "Silly thief, %S is not for you!\n"
        spa "Ladron tonto, %S no es para ti!\n"
        ser "Glupi lopove, %S nije za tebe!\n"
	~

CS_NO_ALCHEMISTS
        eng "Alchemists can't use %S!\n"
        spa "Los alquimistas no pueden usar %S!\n"
        ser "Alhemicar nemoze da koristi %S!\n"
	~

CS_NO_CLERIC
        eng "Silly cleric, %S is not for you!\n"
        spa "Clerigo tonto, %S no es para ti!\n"
        ser "Glupi svestenice, %S nije za tebe!\n"
	~

CS_NO_BARD
        eng "Silly bard, %S is not for you!\n"
        spa "Bardo tonto, %S no es para ti!\n"
        ser "Glupi guslaru, %S nije za tebe!\n"
	~

CS_NO_MOB
        eng "%S is !MOB equipment!\n"
        spa "%S is !MOB equipment!\n"
        ser "%S je !MOB oprema!\n"
	~

CS_NO_PC
        eng "%S is for mobs only!\n"
        spa "%S es solamente para mobs!\n"
        ser "%S je za mobove samo!\n"
        fre "%S est seulement pour les mobs!\n"
	~

CS_TOO_MUCH_WT
        eng "%S:\tYou can't carry that much weight.\n"
        spa "%S:\tNo puedes cargar tanto peso.\n"
        ser "%S:\tNemozes da poneses toliko tesko.\n"
	~

CS_NO_GET_BULLETIN
        eng "You shouldn't be getting bulletin boards, try opurge.\n"
        ser "Nebi trebao da dobijas forume, pokusaj opurge. \n"
	~

CS_SGT_TOO_MUCH_WT
        eng "%S:\t%S can't carry that much weight.\n"
        spa "%S:\t%S no puede cargar tanto peso.\n"
        ser "%S:\t%S nemozes da nosis toliko tesko.\n"
	~

CS_SGT_TOO_MUCH_WT_TARG
        eng "%S can't give you %S because you are carrying too much weight already!"
        spa "%S no puede darte %S porque ya estas cargando demasiado peso!"
        ser "%S nemoze da ti da %S posto vec nosis maksimalnu tezinu!"
	~

CS_SGT_NO_MORTAL
        eng "%S is not for mortals!\n"
        spa "%S no es para mortales!\n"
        ser "%S nije za smrtnike!\n"
        fre "%S n'est pas pour les mortales!\n"
	~

CS_SGT_NO_DEMI
        eng "%S is not for demi-gods!\n"
        spa "%S no es para semi-dioses!\n"
        ser "%S nije za polu bogove!\n"
	~

CS_SGT_NO_GOD
        eng "%S is not for gods!\n"
        spa "%S no es para dioses!\n"
        ser "%S nije za bogove!\n"
        fre "%S n'est pas pour les dieus!\n"
	~

CS_SGT_PC_ONLY
        eng "%S is for players only!\n"
        spa "%S es para jugadores solamente!\n"
        ser "%S je samo za igrace!\n"
	~

CS_SGT_MOB_ONLY
        eng "%S is not for players!\n"
        spa "%S no es para jugadores!\n"
        ser "%S nije za igrace!\n"
	~

CS_NO_REMOVE
        eng "The %S seems to have grown very attached to you!!\n"
        spa "The %S seems to have grown very attached to you!!\n"
        ser " %S je postao previse privrzen tebi!!\n"
	~

CS_NO_DROP
        eng "Yeech, you can't let go of %S!!\n"
        spa "Yeech, you can't let go of %S!!\n"
        ser  "Yeeech, nemozes da ispustis %S!!\n"
	~

CS_THERE_WERE_COINS
        eng "There were %i coins.\n"
        spa "Alli habia %i monedas.\n"
        ser "Nasao si  %i dukata.\n"
	~

CS_DO_GET
        eng "You get %S.\n"
        spa "Agarras %S.\n"
        ser "Dobio si %S.\n"
	~

CS_DO_GET_O
        eng "%S gets %S.\n"
        spa "%S agarra %S.\n"
        ser "%S uzima %S.\n"
	~

CS_DO_GET_FROM
        eng "You get %S from %S.\n"
        spa "Agarras %S de %S.\n"
        ser "Dobijas %S od %S.\n"
	~

CS_DO_GET_FROM_O
        eng "%S gets %S from %S.\n"
        spa "%S agarra %S de %S.\n"
        ser "%S Dobija %S od %S.\n"
	~

CS_DO_GET_YOUR
        eng "You get %S from your %S.\n"
        spa "Agarras %S de tu %S.\n"
        ser "Dobijas %S od svog %S.\n"
	~

CS_DO_GET_YOUR_O
        eng "%S gets %S from %S.\n"
        spa "%S agarra %S de %S.\n"
        ser "%S dobija %S od %S.\n"
	~

CS_LESS_THIRSTY
        eng "You feel less thirsty.\n"
        spa "Te sientes con menos sed.\n"
        ser "Osecas se manje zedan.\n"
        fre "Tu n'as plus soif.\n"
	~

CS_MORE_THIRSTY
        eng "You feel even more thirsty.\n"
        spa "Te sientes aun con mas sed.\n"
        ser "Osecas se jos zedniji.\n"
        fre "Tu as plus de soif.\n"
	~

CS_LESS_HUNGRY
        eng "You feel less hungry.\n"
        spa "Te sientes con menos hambre.\n"
        ser "Osecas se manje gladan.\n"
        fre "Tu n'as plus faim.\n"
	~

CS_MORE_HUNGRY
        eng "You feel even hungrier.\n"
        spa "Te sientes aun con mas hambre.\n"
        ser "Jos si gladniji.\n"
        fre "Tu as plus de faim.\n"
	~

CS_MORE_DRUGGED
        eng "You feel a little higher.\n"
        spa "Te sientes un poco alto.\n"
        ser "Osecas se malo bolje i opustenije.\n"
	~

CS_LESS_DRUGGED
        eng "Some feeling returns to your nose!.\n"
        spa "Algunos pensamientos vuelven a tu nariz!.n"
        ser "Malo te popustaju lekovi!.n"
	~

CS_EAT_GAIN_HP
	eng "You feel much healthier!\n"
	ser "Osecas se malo zdravije!\n"
	~

CS_EAT_LOSE_HP
	eng "You feel much less healthy!\n"
	ser "Osecas se manje zdrav!\n"
	~

CS_EAT_GAIN_MANA
	eng "You feel more powerful and focused.\n"
	ser "Bolje si koncetrisan i osecas se mnogo jaci.\n"
	~

CS_EAT_LOSE_MANA
	eng "You feel dazed and confused.\n"
	ser "Osecas se osamucen i zbunjen.\n"
	~

CS_EAT_GAIN_MOV
	eng "You feel quick and energetic!\n"
	ser "Osecas se brz i energican!\n"
	~

CS_EAT_LOSE_MOV
	eng "You feel sluggish.\n"
	ser "Osecas se tunjavo.\n"
	~

CS_YOU_DROP
        eng "You drop %S.\n"
        spa "Dejas %S.\n"
        ser "Ispustas %S.\n"
	~

CS_DROPS
        eng "%S drops %S.\n"
        spa "%S deja %S.n"
        ser "%S ispusta %S.n"
	~

CS_YOU_DONATE
        eng "You donate %S.\n"
        spa "Donas %S.\n"
        ser "Poklanjas %S.\n"
	~

CS_DONATES
        eng "%S donates %S.\n"
        spa "%S dona %S.\n"
        ser "%S poklanja %S.\n"
	~

CS_OBJ_APPEARS
        eng "%S appears in the room with a popping sound.\n"
        spa "%S aparece en la habitacion de un sonido a !POP!.\n"
        ser "%S se pojavljuje na sred sobe uz pucketanje!.\n"
	~

CS_TRY_FILL
        eng "Try fill instead!\n"
        spa "Intenta llenar en todo caso!\n"
        ser "Pokusaj fill umesto toga!\n"
	~

CS_TRY_POST
        eng "Try post instead.\n"
        spa "Intenta postear en todo caso.\n"
        ser "Pokusaj post umesto toga.\n"
	~

CS_CANT_HOLD_WT
        eng "The %S can't hold the %S.\n"
        spa "%S no puede sostener %S.\n"
        ser "%S nemoze da sadrzi %S.\n"
	~

CS_BAG_CUR_MAX_WEIGHT
	eng  "It is holding %i out of %i pounds.\n"
	~

CS_BAG_EMPTY_WEIGHT
	eng "The %S itself weighs %i pounds.\n"
	~

CS_OBJECT_WEIGHT
	eng "It weighs %i pounds.\n"
	~

CS_YOU_PUT
        eng "You put %S in %S.\n"
        spa "Pones %S en %S.\n"
        ser "Stavljas %S u %S.\n"  
        fre "Tu mettes %S dans %S.\n"
	~

CS_PUTS
        eng "%S puts %S in %S.\n"
        spa "%S pone %S en %S.\n"
        ser "%S stavlja %S u %S.\n"
        fre "%S mette %S dans %S.\n"
	~

CS_PUTS_IN_OWN
        eng "%S puts %S in %s %S.\n"
        spa "%S pone %S en %s %S.\n"
        ser "%S stavlja %S u %s %S.\n"
        fre "%S mette %S dans %s %S.\n"
	~

CS_LEAVES_SPRINTF
        eng "leaves %S.\n"
        spa "se marcha al %S.\n"
        ser "Ide na %S.\n"
        fre "departe %S.\n"
	~

CS_LEAVES_NOT_NORMAL_SPRINTF
        eng "leaves through the %S.\n"
        spa "se marcha al %S.\n"
        ser "Odlazi kroz %S.\n"
	~

CS_ARRIVED_FROM_SPRINTF
        eng "has arrived from %S.\n"
        spa "ha llegado desde %S.\n"
        ser "je stigao sa %S.\n"
        fre "est arrive du %S.\n"
	~

CS_ARRIVED_FROM_CUST_SP
        eng "has arrived from the %S.\n"
        spa "ha llegado desde el %S.\n"
        ser "je stigao sa %S.\n"
        fre "est arrive du %S.\n"
	~
