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
        eng "%pS\\n%pS says, '%pS'\\n%pS"
        spa "%pS\\n%pS dice, '%pS'\\n%pS"
        ser "%pS\\n%pS kaze, '%pS'\\n%pS"
        ger "%pS\\n%pS sagt: '%pS'\\n%pS"
        fre "%pS\\n%pS dit, '%pS'\\n%pS"
        ~ 

CS_OSAY_SPRINTF
        eng "%pS\\n[OOC] %pS says, '%pS'\\n%pS"
        spa "%pS\\n[OOC] %pS dice, '%pS'\\n%pS"
        ser "%pS\\n[OOC] %pS kaze, '%pS'\\n%pS"
        ger "%pS\\n[OOC] %pS sagt: '%pS'\\n%pS"
        fre "%pS\\n%pS dit, '%pS'\\n%pS"
        ~ 

# Shown when you try to communicate in your sleep.
CS_MUTTER_SLEEP
        Eng "You mutter in your sleep."
        Spa "Murmuras mientras duermes."
        Ser "Mumlas u snu."
	Ger "Sie murmeln im Schlaf."
        ~

CS_NEW_PLAYER_INFO_NC
        eng "\007\007\007 INFO: [NEW PLAYER]  %pS connecting from: %pS\n"
        spa "\007\007\007 INFO: [NEW PLAYER]  %pS conectandose desde: %pS\n"
        ser "\007\007\007 INFO: [NEW PLAYER]  %pS konektovan sa: %pS\n"
        ger "\007\007\007 INFO: [NEUER SPIELER] %pS nimmt Verbindung auf von: %pS\n"
	~

CS_NEW_PLAYER_INFO_C
        eng "<BEEP> INFO: [NEW PLAYER]  %pS connecting from: %pS\n"
        spa "<BEEP> INFO: [NEW PLAYER]  %pS conectandose desde: %pS\n"
        ser "<BEEP> INFO: [NEW PLAYER]  %pS konektovan sa: %pS\n"
        ger "<BEEP> INFO: [NEUER SPIELER] %pS nimmt Verbindung auf von: %pS\n"
        ~

CS_PLAYER_REGAINED_INFO
        eng "  INFO:  %pS has regained connection from: %pS\n"
        spa "  INFO:  %pS ha recuperado la conexion desde: %pS\n"
        ser "  INFO:  %pS ponovo se konektovao sa: %pS\n"
	ger "  INFO:  %pS nimmt erneut Verbindung auf von: %pS\n"
        ~

CS_PLAYER_HAS_CONNECTED_INFO
        eng "   INFO:  %pS has connected from: %pS\n"
        spa "   INFO:  %pS se ha conectado desde: %pS\n"
        ser "   INFO:  %pS se konektovao sa: %pS\n"
	ger "   INFO:  %pS hat Verbindung aufgenommen von: %pS\n"
        ~

CS_PLAYER_OFF_LD_LIST_INFO
        eng " INFO:  %pS has been taken off the link-dead list.\n"
        spa " INFO:  %pS ha sido borrado de la lista de conexiones muertas.\n"
        ser " INFO:  %pS je skinut sa liste mrvih linkova.\n"
	ger " INFO:  %pS wurde von der Liste unterbrochener Verbindungen entfernt.\n"
        ~

CS_PLAYER_LOST_CON_INFO
        eng " INFO:  %pS has lost connection.\n"
        spa " INFO:  %pS ha perdido la conexion.\n"
        ser " INFO:  %pS je izgubio konekciju.\n"
	ger " INFO:  %pS hat die Verbindung verloren.\n"
	  ~

CS_PKILLED_INFO
        eng "  INFO:  %pS has P-KILLED %pS in room %i!!\n"
        spa "  INFO:  %pS ha ASESINADO-AL-JUGADOR %pS en la habitacion %i!!\n"
        ser "  INFO:  %pS je ubio %pS u sobi %i!!\n"
	ger "  INFO:  %pS hat %pS in Raum %i ermordet!\n"
	  ~

CS_KILLED_INFO
        eng "  INFO:  %pS was killed by %pS in room: %i\n"
        spa "  INFO:  %pS fue muerto por %pS en la habitacion: %i\n"
        ser "  INFO:  %pS je ubijen. %pS ga je ubio u sobi: %i\n"
	ger "  INFO:  %pS wurde von %pS in Raum %i ermordet.\n"
	~

CS_PERM_DEATH_INFO
        eng "%pS has permanently died for lack of constitution!\n"
        spa "%pS ha muerto permanentemente por falta de constitucion!\n"
        ser "%pS je mrtav zbog slabe fizicke gradje!\n"
	ger "%pS ist aufgrund von mangelnder Konstitution endgültig verschieden!\n"
        ~

CS_YELL_SLEEP
        eng "You roar in your sleep!\n"
        spa "Ruges mientras duermes!\n"
        ser "Deres se u snu!\n"
	ger "Sie schreien im Schlaf!\n"
 	~

CS_SAY_OBJ_SPRINTF
        eng "\n####%pS says, '%pS'\n"
        spa "\n####%pS dice, '%pS'\n"
        ser "\n####%pS kaze, '%pS'\n"
        ger "\n####%pS sagt: '%pS'\n"
	~

CS_YELL
        eng "%pS\n%pS yells: '%pS'\n%pS"
        spa "%pS\n%pS grita: '%pS'\n%pS"
        ser "%pS\n%pS vice: '%pS'\n%pS" 
        ger "%pS\n%pS ruft: '%pS'\n%pS" 
	~

CS_YOU_YELL
        eng "%pSYou yell, '%pS'\n%pS"
        spa "%pSGritas, '%pS'\n%pS"
        ser "%pSTi vices, '%pS'\n%pS"
	ger "%pSSie rufen: '%pS'\n%pS"
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
        eng "%pS\n%pS gossips, '%pS'\n%pS"
        spa "%pS\n%pS chismea, '%pS'\n%pS"
        ser "%pS\n%pS brblja, '%pS'\n%pS"
        ger "%pS\n%pS babbelt: '%pS'\n%pS"
	~

CS_YOU_GOSSIP
        eng "%pSYou gossip, '%pS'\n%pS"
        spa "%pSChismeas, '%pS'\n%pS"
        ser "%pSTi brbljas, '%pS'\n%pS"
        ger "%pSSie babbeln: '%pS'\n%pS"
	~

CS_GS
        eng "%pS\n%pS tells the group, '%pS'\n%pS"
        spa "%pS\n%pS le dice al grupo, '%pS'\n%pS"
        ser "%pS\n%pS govori grupi, '%pS'\n%pS"
        ger "%pS\n%pS teilt der Gruppe mit: '%pS'\n%pS"
	~

CS_YOU_GS
        eng "%pSYou tell the group, '%pS'\n%pS"
        spa "%pSLe dices al grupo, '%pS'\n%pS"
        ser "%pSLe Ti govoris grupi, '%pS'\n%pS"
        ger "%pSSie teilen der Gruppe mit: '%pS'\n%pS"
	 ~

CS_NO_WIZCHAT_CHANNEL
        eng "You are not on the wizchat channel.\n"
        spa "No estas en el canal de wizchat.\n"
        ser "Ti si sada na wizchat  kanalu.\n"
	ger "Sie beteiligen sich zur Zeit nicht am 'wizchat channel'.\n"
	~

CS_WC
        eng "%pS\n%pS wizchats, '%pS'\n%pS"
        spa "%pS\n%pS wizchats, '%pS'\n%pS"
        ser "%pS\n%pS wizchats, '%pS'\n%pS"
        ger "%pS\n%pS wizchattet, '%pS'\n%pS"
	~

CS_YOU_WC
        eng "%pSYou wizchat, '%pS'\n%pS"
        spa "%pSTu wizchat, '%pS'\n%pS"
        ser "%pSTi wizchat-ujes, '%pS'\n%pS"
        ger "%pSSie wizchatten: '%pS'\n%pS"
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
        eng "%pS\n%pS auctions, '%pS'\n%pS"
        spa "%pS\n%pS auctions, '%pS'\n%pS"
        ser "%pS\n%pS aukcija, '%pS'\n%pS"
        ger "%pS\n%pS auctions: '%pS'\n%pS"
	~

CS_YOU_AUCTION
        eng "%pSYou auction, '%pS'\n%pS"
        spa "%pSTu auction, '%pS'\n%pS"
        ser "%pSTi stavljas na aukciju, '%pS'\n%pS"
        ger "%pSYou auction: '%pS'\n%pS"
	~

CS_NO_SHOUT_CHANNEL
        eng "You are not on the shout channel.\n"
        spa "No estas en el canal de griterio.\n"
        ser "Nisi na kanalu za vikanje.\n"
	ger "Sie beteiligen sich zur Zeit nicht am 'shout channel'.\n"
	~

CS_SHOUT
        eng "%pS\n%pS shouts, '%pS'\n%pS"
        spa "%pS\n%pS grita en el canal, '%pS'\n%pS"
        ser "%pS\n%pS vice, '%pS'\n%pS"
	ger "%pS\n%pS schreit: '%pS'\n%pS"
	~

CS_YOU_SHOUT
        eng "%pSYou shout, '%pS'\n%pS"
        spa "%pSGritas en el canal, '%pS'\n%pS"
        ser "%pSTi vices, '%pS'\n%pS"
        ger "%pSSie schreien: '%pS'\n%pS"
	~

CS_PEMOTE_OBJ_SPRINTF
        eng "#####%pS's %pS\n"
        spa "#####%pS's %pS\n"
        ser "#####%pS %pS\n"
        ger "#####%pSs %pS\n"
	~

CS_EMOTE_OBJ_SPRINTF
        eng "#####%pS %pS\n"
        spa "#####%pS %pS\n"
        ser "#####%pS %pS\n"
        ger "#####%pS %pS\n"
	~

CS_PEMOTE_OBJ_SOMEONE
        eng "#####Someone's %pS\n"
        spa "#####Someone's %pS\n"
        ser "#####Necije %pS\n"
        ger "#####Jemandes %pS\n"
	~

CS_EMOTE_OBJ_SOMEONE
        eng "#####Someone %pS\n"
        spa "#####Someone %pS\n"
        ser "#####Necije %pS\n"
        ger "#####Jemand %pS\n"
	~

CS_PEMOTE_SOMEONE
        eng "Someone's %pS\n"
        spa "Someone's %pS\n"
        ser "Necije %pS\n"
        ger "Jemandes %pS\n"
	~

CS_EMOTE_SOMEONE
        eng "Someone %pS\n"
        spa "Alguien %pS\n"
        ser "necije %pS\n"
        ger "Jemand %pS\n"
	~

CS_PEMOTE
        eng "%pS's %pS\n"
        spa "%pS's %pS\n"
        ser "%pS's %pS\n"
        ger "%pSs %pS\n"
	~

CS_EMOTE
        eng "%pS %pS\n"
        spa "%pS %pS\n"
        ser "%pS %pS\n"
        ger "%pS %pS\n"
	~

CS_SAY_SPRINTF_YOU
        eng "%pSYou say, '%pS'\\n%pS"
        spa "%pSSe habla, '%pS'\\n%pS"
	ser "%pSTi kazes, '%pS'\\n%pS"
	ser "%pSSie sagen: '%pS'\\n%pS"
        ~

CS_OSAY_SPRINTF_YOU
        eng "%pS[OOC] You say, '%pS'\\n%pS"
        spa "%pS[OOC] Se habla, '%pS'\\n%pS"
	ser "%pS[OOC] Ti kazes, '%pS'\\n%pS"
	ser "%pS[OOC] Sie sagen: '%pS'\\n%pS"
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
	eng "You can't junk %pS.\n"
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
        eng "You don't seem to have the %pS.\n"
        spa "No pareces tener %pS.\n"
        ser "Nemas %pS.\n"
        fre "Tu n'as pas le %pS.\n"
	  ~

CS_AINT_GOT
        eng "You neither have nor see the %pS.\n"
        spa "No tienes ni ves %pS.\n"
        ser "Izgleda da nemas ili nevidis %pS.\n"
        ~

CS_CANT_FIND
        eng "You can't find the %pS.\n"
        spa "No puedes encontrar %pS.\n"
        ser "Nemozes da nadjes %pS.\n"
        fre "Tu ne trouves pas le %pS.\n"
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
        eng "You can't find the %pS.\n"
        spa "No puedes encontrar %pS.\n"
        ser "Nemozes da nadjes %pS.\n"
        fre "Tu ne trouves pas le %pS.\n"
	~

CS_ITS_CLOSED
        eng "It's closed!\n"
        spa "Esta cerrado!\n"
        ser "Zatvoreno!\n"
        fre "C'est couvert!\n"
	~

CS_CONTAINS
        eng "%pS contains:\n\n"
        spa "%pS contiene:\n\n"
        ser "%pS sadrzi:\n\n"
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
        eng "You assist %pS in fighting against %pS!\n"
        spa "Asistes a %pS en pelear contra %pS!\n"
        ser "Asistiras %pS u borbi protiv %pS!\n"
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
        eng "You put %i coins in %pS.\n"
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
        eng "The %pS appears to be empty.\n"
        spa "%pS parece estar vacio.\n"
        ser "%pS je prazno.\n"
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
        eng "You neither see nor have the %pS.\n"
        spa "No ves ni tienes %pS.\n"
        ser "Ili nevidis ili nemas %pS.\n"
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
        eng "The %pS doesn't seem to contain the %pS.\n"
        spa "%pS no parece contener %pS.\n"
        ser "%pS  ne sadrzi %pS.\n"
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
        eng "The %pS is closed.\n"
        spa "%pS es cerrada.\n"
        ser "%pS su zatvorena.\n"
	fre "%pS est couvert.\n"
        ~

CS_MOV_BLOCKED
        eng "The %pS is blocked by %pS.\n"
        spa "%pS esta bloqueado por %pS.\n"
        ser "%pS je blokiran. %pS blokira put.\n"
	~

CS_BLOCKS_WAY
        eng "%pS blocks your way.\n"
        spa "%pS bloquea tu camino.\n"
        ser "%pS ti blokira put.\n"
	~

CS_CONSUMES_KEY
        eng "You give up %pS as you pass through.\n"
        spa "Das %pS mientras pasas a su lado.\n"
        ser "Dajes  %pS u prolazu.\n"
	~

CS_MOV_NEED_KEY
        eng "You must have %pS in order to pass.\n"
        spa "Debes tener %pS para poder pasar.\n"
        ser "Moras da imas %pS da bi prosao.\n"
	~

CS_HAS_ARRIVED
        eng "has arrived.\n"
        spa "ha llegado.\n"
        ser "je dosao.\n"
        fre "est arrive.\n"
	~

CS_YOU_FOLLOW
        eng "You attempt to follow %pS %s.\n"
        spa "Intentas seguir a %pS %s.\n"
        ser "Pokusavas da pratis %pS %s.\n"
        fre "Tu essaies suivre %pS %s.\n"
	~

CS_NO_FIND_EXIT
        eng "You can't discern an exit in that direction.\n"
        spa "No puedes ver una salida en esa direccion.\n"
        ser "Nemozes da nadjes izlaz u tom pravcu.\n"
	~

CS_NO_WEAR
        eng "You can't wear %pS.\n"
        spa "No puedes vestir %pS.\n"
        ser "Nemozes da obuces %pS.\n"
	~

CS_ALREADY_WEARING
        eng "   %pS:%P25You are already using something there.\n"
        spa "   %pS:%P25Ya estas usando algo ahi.\n"
        ser "   %pS:%P25Vec imas obuceno nesto tamo.\n"
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
        eng "%pS%P30 [%i]%P37 %pS.\n"
        spa "%pS%P30 [%i]%P37 %pS.\n"
        ser "%pS%P30 [%i]%P37 %pS.\n"
        fre "%pS%P30 [%i]%P37 %pS.\n"
	~

CS_WO_RSLT
        eng "%pS%P30 %pS.\n"
        spa "%pS%P30 %pS.\n"
        ser "%pS%P30 %pS.\n"
        fre "%pS%P30 %pS.\n"
	~

CS_NOT_EXPERIENCED
        eng "You are not experienced enough to use %pS.\n"
        spa "No tienes suficiente experiencia para usar %pS.\n"
        ser "Nisi dovoljno iskusan da bi koristio %pS.\n"
	~

CS_TOO_HEAVY
        eng "%pS is just too heavy for you!\n"
        spa "%pS es demadiado pesado para vos!\n"
        ser "%pS je suvise tesko za tebe!\n"
	~
        
CS_CANT_WEAR_THERE
        eng "You can't wear it there.\n"
        spa "No puedes usar eso ahi.\n"
        ser "Nemozes da nosis to tu.\n"
	~

CS_TRY_LIGHT_OBJ
	eng "See 'help light' to light an object.\n"
	~

CS_TOO_EVIL
        eng "You're too evil for your %pS!\n"
        spa "Eres demasiado malvado para %pS!\n"
        ser "Ti si previse zao za  %pS!\n"
	~

CS_TOO_NEUTRAL
        eng "You're too neutral for your %pS!\n"
        spa "Eres demasiado neutral para %pS!\n"
        ser "Ti si previse neutralan za  %pS!\n"
	~

CS_TOO_GOOD
        eng "You're too pleasant for your %pS!\n"
        spa "Eres demasiado pacifico para %pS!\n"
        ser "Ti si previse fin za  %pS!\n"
	~

CS_IMM_ONLY
        eng "%pS belongs to an immortal!\n"
        spa "%pS pertenece a un inmortal!\n"
        ser "%pS pripada besmrtnicima!\n"
	~

CS_DEMI_ONLY
        eng "%pS belongs to a demi-god!\n"
        spa "%pS pertenece a un semi-dios!\n"
        ser "%pS pripada polu-bogovima!\n"
	~

CS_GOD_ONLY
        eng "%pS belongs to a god!\n"
        spa "%pS pertenece a un dios!\n"
        ser "%pS pripada bogu!\n"
	~

CS_TOO_RELAXED
        eng "You are too relaxed!\n"
        spa "Estas demasiado relajado!\n"
        ser "previse si opusten!\n"
	~

CS_NO_WARRIOR
        eng "Silly warrior, %pS is not for you!\n"
        spa "Guerrero tonto, %pS no es para ti!\n"
        ser "Glupi ratnice, %pS nije za tebe!\n"
	~

CS_NO_SAGE
        eng "Silly sage, %pS is not for you!\n"
        spa "Silly sage, %pS is not for you!\n"
        ser "Glupi Mudrace, %pS nije za tebe!\n"
	~

CS_NO_WIZARD
        eng "Silly wizard, %pS is not for you!\n"
        spa "Mago tonto, %pS no es para ti!\n"
        ser "Glupi carobnjaku %pS nije za tebe!\n"
	~

CS_NO_RANGER
        eng "Silly ranger, %pS is not for you!\n"
        spa "Guardabosque tonto, %pS no es para ti!\n"
        ser "Glupi rendzeru, %pS nije za tebe!\n"
	~

CS_NO_THIEF
        eng "Silly thief, %pS is not for you!\n"
        spa "Ladron tonto, %pS no es para ti!\n"
        ser "Glupi lopove, %pS nije za tebe!\n"
	~

CS_NO_ALCHEMISTS
        eng "Alchemists can't use %pS!\n"
        spa "Los alquimistas no pueden usar %pS!\n"
        ser "Alhemicar nemoze da koristi %pS!\n"
	~

CS_NO_CLERIC
        eng "Silly cleric, %pS is not for you!\n"
        spa "Clerigo tonto, %pS no es para ti!\n"
        ser "Glupi svestenice, %pS nije za tebe!\n"
	~

CS_NO_BARD
        eng "Silly bard, %pS is not for you!\n"
        spa "Bardo tonto, %pS no es para ti!\n"
        ser "Glupi guslaru, %pS nije za tebe!\n"
	~

CS_NO_MOB
        eng "%pS is !MOB equipment!\n"
        spa "%pS is !MOB equipment!\n"
        ser "%pS je !MOB oprema!\n"
	~

CS_NO_PC
        eng "%pS is for mobs only!\n"
        spa "%pS es solamente para mobs!\n"
        ser "%pS je za mobove samo!\n"
        fre "%pS est seulement pour les mobs!\n"
	~

CS_TOO_MUCH_WT
        eng "%pS:\tYou can't carry that much weight.\n"
        spa "%pS:\tNo puedes cargar tanto peso.\n"
        ser "%pS:\tNemozes da poneses toliko tesko.\n"
	~

CS_NO_GET_BULLETIN
        eng "You shouldn't be getting bulletin boards, try opurge.\n"
        ser "Nebi trebao da dobijas forume, pokusaj opurge. \n"
	~

CS_SGT_TOO_MUCH_WT
        eng "%pS:\t%pS can't carry that much weight.\n"
        spa "%pS:\t%pS no puede cargar tanto peso.\n"
        ser "%pS:\t%pS nemozes da nosis toliko tesko.\n"
	~

CS_SGT_TOO_MUCH_WT_TARG
        eng "%pS can't give you %pS because you are carrying too much weight already!"
        spa "%pS no puede darte %pS porque ya estas cargando demasiado peso!"
        ser "%pS nemoze da ti da %pS posto vec nosis maksimalnu tezinu!"
	~

CS_SGT_NO_MORTAL
        eng "%pS is not for mortals!\n"
        spa "%pS no es para mortales!\n"
        ser "%pS nije za smrtnike!\n"
        fre "%pS n'est pas pour les mortales!\n"
	~

CS_SGT_NO_DEMI
        eng "%pS is not for demi-gods!\n"
        spa "%pS no es para semi-dioses!\n"
        ser "%pS nije za polu bogove!\n"
	~

CS_SGT_NO_GOD
        eng "%pS is not for gods!\n"
        spa "%pS no es para dioses!\n"
        ser "%pS nije za bogove!\n"
        fre "%pS n'est pas pour les dieus!\n"
	~

CS_SGT_PC_ONLY
        eng "%pS is for players only!\n"
        spa "%pS es para jugadores solamente!\n"
        ser "%pS je samo za igrace!\n"
	~

CS_SGT_MOB_ONLY
        eng "%pS is not for players!\n"
        spa "%pS no es para jugadores!\n"
        ser "%pS nije za igrace!\n"
	~

CS_NO_REMOVE
        eng "The %pS seems to have grown very attached to you!!\n"
        spa "The %pS seems to have grown very attached to you!!\n"
        ser " %pS je postao previse privrzen tebi!!\n"
	~

CS_NO_DROP
        eng "Yeech, you can't let go of %pS!!\n"
        spa "Yeech, you can't let go of %pS!!\n"
        ser  "Yeeech, nemozes da ispustis %pS!!\n"
	~

CS_THERE_WERE_COINS
        eng "There were %i coins.\n"
        spa "Alli habia %i monedas.\n"
        ser "Nasao si  %i dukata.\n"
	~

CS_DO_GET
        eng "You get %pS.\n"
        spa "Agarras %pS.\n"
        ser "Dobio si %pS.\n"
	~

CS_DO_GET_O
        eng "%pS gets %pS.\n"
        spa "%pS agarra %pS.\n"
        ser "%pS uzima %pS.\n"
	~

CS_DO_GET_FROM
        eng "You get %pS from %pS.\n"
        spa "Agarras %pS de %pS.\n"
        ser "Dobijas %pS od %pS.\n"
	~

CS_DO_GET_FROM_O
        eng "%pS gets %pS from %pS.\n"
        spa "%pS agarra %pS de %pS.\n"
        ser "%pS Dobija %pS od %pS.\n"
	~

CS_DO_GET_YOUR
        eng "You get %pS from your %pS.\n"
        spa "Agarras %pS de tu %pS.\n"
        ser "Dobijas %pS od svog %pS.\n"
	~

CS_DO_GET_YOUR_O
        eng "%pS gets %pS from %pS.\n"
        spa "%pS agarra %pS de %pS.\n"
        ser "%pS dobija %pS od %pS.\n"
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
        eng "You drop %pS.\n"
        spa "Dejas %pS.\n"
        ser "Ispustas %pS.\n"
	~

CS_DROPS
        eng "%pS drops %pS.\n"
        spa "%pS deja %pS.n"
        ser "%pS ispusta %pS.n"
	~

CS_YOU_DONATE
        eng "You donate %pS.\n"
        spa "Donas %pS.\n"
        ser "Poklanjas %pS.\n"
	~

CS_DONATES
        eng "%pS donates %pS.\n"
        spa "%pS dona %pS.\n"
        ser "%pS poklanja %pS.\n"
	~

CS_OBJ_APPEARS
        eng "%pS appears in the room with a popping sound.\n"
        spa "%pS aparece en la habitacion de un sonido a !POP!.\n"
        ser "%pS se pojavljuje na sred sobe uz pucketanje!.\n"
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
        eng "The %pS can't hold the %pS.\n"
        spa "%pS no puede sostener %pS.\n"
        ser "%pS nemoze da sadrzi %pS.\n"
	~

CS_BAG_CUR_MAX_WEIGHT
	eng  "It is holding %i out of %i pounds.\n"
	~

CS_BAG_EMPTY_WEIGHT
	eng "The %pS itself weighs %i pounds.\n"
	~

CS_BAG_PERCENT
	eng "It reduces its contents' weight to %i%%.\n"
	~

CS_OBJECT_WEIGHT
	eng "It weighs %i pounds.\n"
	~

CS_YOU_PUT
        eng "You put %pS in %pS.\n"
        spa "Pones %pS en %pS.\n"
        ser "Stavljas %pS u %pS.\n"  
        fre "Tu mettes %pS dans %pS.\n"
	~

CS_PUTS
        eng "%pS puts %pS in %pS.\n"
        spa "%pS pone %pS en %pS.\n"
        ser "%pS stavlja %pS u %pS.\n"
        fre "%pS mette %pS dans %pS.\n"
	~

CS_PUTS_IN_OWN
        eng "%pS puts %pS in %s %pS.\n"
        spa "%pS pone %pS en %s %pS.\n"
        ser "%pS stavlja %pS u %s %pS.\n"
        fre "%pS mette %pS dans %s %pS.\n"
	~

CS_LEAVES_SPRINTF
        eng "leaves %pS.\n"
        spa "se marcha al %pS.\n"
        ser "Ide na %pS.\n"
        fre "departe %pS.\n"
	~

CS_LEAVES_NOT_NORMAL_SPRINTF
        eng "leaves through the %pS.\n"
        spa "se marcha al %pS.\n"
        ser "Odlazi kroz %pS.\n"
	~

CS_ARRIVED_FROM_SPRINTF
        eng "has arrived from %pS.\n"
        spa "ha llegado desde %pS.\n"
        ser "je stigao sa %pS.\n"
        fre "est arrive du %pS.\n"
	~

CS_ARRIVED_FROM_CUST_SP
        eng "has arrived from the %pS.\n"
        spa "ha llegado desde el %pS.\n"
        ser "je stigao sa %pS.\n"
        fre "est arrive du %pS.\n"
	~
