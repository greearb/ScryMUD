//
//ScryMUD Server Code
//Copyright (C) 1998  Ben Greear
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// To contact the Author, Ben Greear:  greear@cyberhighway.net, (preferred)
//                                     greearb@agcs.com
//

#include <string2.h>
#include "const.h"
#include "critter.h"
#include "room.h"
#include "object.h"


/** All the words and phrases that one deems useless to the
 * cause of humane communication goes here.
 */
const char* CensoredStrings[] = {
   "asshole",
   "cunt",
   "dick",
   "fag", //This may catch too many??
   "fuck",
   "pussy",
   NULL  /* don't forget this!! */
};

const char* osd_names[] = {
   "consume_teleport", 
   "has_construct_data",
   "has_skin",
   "consume_poison",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "casts_spells"
};
const BitfieldNames OBJ_SPEC_DATA_FLAGS_NAMES(sizeof(osd_names)/sizeof(const char*),
                                              osd_names, "Obj Spec Data Flags:");

const char* bf_names[] = { 
   "NULL",
   "NULL",
   "is_closed",
   "is_locked",
   "is_pickable",
   "is_mag_lockable",
   "is_mag_locked",
   "is_destructable {fireball may open it till reset}",
   "is_corpse",
   "!close",
   "NULL",//10
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "consume_key"//17
};
const BitfieldNames BAG_FLAGS_NAMES(sizeof(bf_names)/sizeof(const char*),
                                    bf_names, "Bag Flags:");

const char* sd_names[] = {
   "buy_0",//0
   "sell_0",//1
   "offer_0",//2
   "Player-run shopkeeper",//3
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",//10
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",//20
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",//30
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NULL",
   "NON weapon",//40
   "slash",
   "smash",
   "pierce",
   "whip", 
   "needs_ammo",
   "dart_thrower",
   "bow", 
   "dart",
   "arrow",
   "junk", //50
   "wand",
   "potion",
   "scroll",
   "container",
   "coins",//55
   "armor",
   "weapon",
   "light_source",
   "canteen",
   "liquid",//60
   "food",
   "boat",
   "has_spec_proc_data",
   "toolbox",
   "cauldron",//65
   "pen", 
   "construct_component",
   "concoct_component",
   "parchment", //69
   "NULL",
   "NULL",
   "herb", //72
   "vend_machine"//73
};
const BitfieldNames SHOP_DATA_FLAGS_NAMES(sizeof(sd_names)/sizeof(const char*),
                                          sd_names, "Shop Data Flags (Trades):");

const char* td_names[] = { "NULL" };
const BitfieldNames TEACH_DATA_FLAGS_NAMES(sizeof(osd_names)/sizeof(const char*),
                                           td_names, "Teach Data Flags:");

/** FLAG1 */
const char* mp_names[] = {
   "NULL",
   "shopkeeper",
   "teacher",
   "let_same_class_pass",
   "let_same_race_pass",
   "has_mob_give_proc",
   "has_mob_say_proc",
   "has_mob_bow_proc",
   "has_mob_curse_proc",
   "proc_with_same_race", 
   "proc_with_same_align",
   "proc_with_same_class",
   "NULL",
   "has_AI"
};
const BitfieldNames MOB_PROC_DATA_FLAGS_NAMES(sizeof(mp_names)/sizeof(const char*),
                                              mp_names, "Mob Proc Flags:");

const char* c_names[] = {
   "can_see_inv",
   "using_light_src",
   "is_flying",           
   "have_boat",
   "can_climb",
   "gos",
   "yell",
   "gratz",
   "auc", 
   "shout",
   "say",
   "tell",
   "wiznet",
   "is_paralyzed",
   "is_perm_sleeped",
   "is_dual_wielding",
   "is_sneak",
   "in_use",
   "can_dive",
   "spell_tested_yet",
   "is_blocked",
   "is_hide",
   "is_tailing",
   "!complete",
   "already_hurled"
};
const BitfieldNames CRIT_FLAGS_NAMES(sizeof(c_names)/sizeof(const char*),
                                     c_names, "Critter Flags:");

const char* md_names[] = {
   "has_proc_data", //0
   "scavenge", //1
   "wander", //2
   "should_do_procs", //3
   "need_resetting", //4
   "edible_corpse", //5
   "is_banker", //6,
   "NULL", //7
   "NULL", //8
   "NULL", //9
   "NULL", //10
   "NULL", //11
   "NULL", //12
   "NULL", //13
   "NULL", //14
   "NULL", //15
   "has_skin", //16
   "has_mob_script" /*17*/
};
const BitfieldNames MOB_DATA_FLAGS_NAMES(sizeof(md_names)/sizeof(const char*),
                                         md_names, "Mob Data Flags:");

const char* pcd_names[] = {
   "frozen",//0
   "gagged",//1
   "has_imm_data", //2
   "cloaked",//3
   "tank_graph",//4
   "using_client",//5
   "auto_exit",//6
   "!hassle",//7
   "brief",//8
   "autosplit",//9 
   "do_prompt",//10
   "is_builder",//11
   "autoloot",//12
   "olc_redo",//13
   "extra_info",//14
   "cr_behind",//15
   "do_carriage_return",//16
   "is_blocking_door",//17
   "can_det_magic",//18
   "detect_inventory",//19
   "show_vnums",//20
   "has_poofin_poofout_msg",//21
   "page_output",//22
   "in_page_break_mode",//23
   "!wizchat",//24
   "has_colors",//25
   "use_color" //26
};
const BitfieldNames PC_DATA_FLAGS_NAMES(sizeof(pcd_names)/sizeof(const char*),
                                        pcd_names, "PC Data Flags:");

const char* d_names[] = {
   "open exit basically no door",//0
   "is_mag_lockable",//1
   "is_closed",//2
   "is_locked",//3
   "is_pckabl",//4
   "is_lockable", //5
   "mag_locked (spell only can open it)",//6
   "is_destructable",//7
   "is_closeable",//8
   "is_flippable",//9
   "in_use",//10
   "is_unopenable (by players, auto only)",//11
   "is_vehicle_exit",//12
   "is_secret",//13
   "is_blocked",//14
   "!complete",//15
   "secret_when_open_too",//16
   "consume_key",//17
   "!passdoor"//18
};
const BitfieldNames DOOR_DATA_FLAGS_NAMES(sizeof(d_names)/sizeof(const char*),
                                          d_names, "Door Data Flags:");

const char* v_names[] = {
   "is_self_guided",
   "is_stealthy",
   "unlimited_fuel",
   "can_fly",
   "can_climb", 
   "can_float",
   "can_dive",
   "can_see_out (show room descs to passengers)"
};
const BitfieldNames VEHICLE_FLAGS_NAMES(sizeof(v_names)/sizeof(const char*),
                                        v_names, "Vehicle Flags:");
    
const char* r_names[] = {
   "no_rest",//0
   "!imm",//1
   "!god",//2
   "is_perm_dark",//3
   "weather",//4
   "!shout",//5
   "!mag_exit",//6
   "is_haven", //7
   "!PK", //8
   "!magic", //9
   "!mob",//10
   "!potions",//11
   "!staffs",//12
   "!mort",//13
   "normally_dark",//14
   "river/small lake",//15
   "ocean/big lake",//16
   "swamp", //17
   "need_fly",//18
   "need_boat",//19
   "need_climb",//20
   "is_zlocked (for olc)",//21
   "is_totally_loaded",//22
   "is_used",//23
   "!magic_entry",//24
   "!vehicle (vehicles can't drive here)",//25
   "cramped (!huge)",//26
   "!ranged",//27
   "need_dive_ability",//28
   "used_in_track",//29
   "can_camp",//30
   "!complete (olc wise)",//31
   "has_keywords",//32
   "!mob_wander",//33
   "!foreign_mob_wander",//34
   "has_proc_script"//35
};
const BitfieldNames ROOM_FLAGS_NAMES(sizeof(r_names)/sizeof(const char*),
                                     r_names, "Room Flags:");

const char* o_names[] = {
   "no_rest",//0
   "!evil", 
   "!neutral",
   "!good", 
   "!donate",
   "!drop",
   "!remove",
   "!mortal",
   "!imm",
   "!demi",
   "in_use",//10
   "!warrior",
   "!sage",
   "!wizard",
   "!ranger",
   "!thief",
   "!alchemist", 
   "!cleric",
   "!bard",
   "!mob",
   "!pc",//20
   "not worn",
   "head",
   "neck",
   "neck",
   "around body",
   "arms", 
   "wrist1",
   "wrist2", 
   "hands",
   "wielded",//30
   "held",
   "light",
   "body",
   "belt",
   "legs",
   "feet",
   "finger1", 
   "finger2",
   "shield", //39
   "not a weapon",//40
   "slash",
   "smash",
   "pierce",
   "whip", 
   "needs_ammo",
   "dart_thrower",
   "bow", 
   "dart",
   "arrow",
   "junk", //50
   "wand",
   "potion",
   "scroll",
   "container",
   "coins",//55
   "armor",
   "weapon",
   "light_source",
   "canteen",
   "liquid",//60
   "food",
   "boat",
   "has_spec_proc_data",
   "toolbox",
   "cauldron",
   "pen", 
   "construct_component",
   "concoct_component",
   "parchment", 
   "needs_resetting",//70
   "!complete", 
   "herb", //72
   "vend_machine",
   "bulletin_board",
   "is_butcherable",
   "has_obj_script"//76
};

const BitfieldNames OBJ_FLAGS_NAMES(sizeof(o_names)/sizeof(const char*),
                                    o_names, "Object Flags:");

const char* DARK_MSG = "It is too dark to see.\n";
const char* SLEEP_MSG = "Dream on, my friend.\n";
const char* BLIND_MSG = "You are blind as a bat, and your ears are small.\n";
const char* NO_CAN_SEE_MSG = "You do not see that here.\n";
const char* NOT_IN_HAVEN_MSG =
            "This is much to nice a room to fill with gore!!\n";
const char* NOT_IN_NOMAG_RM_MSG = 
            "This room is somehow insulated from magical energies!\n";
const char* SPELL_MUST_BE_STANDING_MSG = 
            "You must be standing to cast this spell.\n";

const char* NO_PK_HERE_MSG = 
            "If you want to MURDER someone, go somewhere else!!\n";

const char* NO_MANA_MSG = "You feel too tired to cast this spell.\n";
const char* PARSE_ERR_MSG = "Do what??\n";
const char* NO_KNOW_SPELL_MSG = "You don't know where to begin!\n";
const char* LOST_CONCENTRATION_MSG_SELF =
            "You lost your concentration.\n";
const char* LOST_CONCENTRATION_MSG_OTHER =
            "can't seem to remember how it goes.\n";
const char* SPELL_ONLY_ON_SELF_MSG = "You can't use this spell on others.\n";
const char* HUH_MSG = "Huh??\n";

// Used in socials heavily.
const char* DONT_SEE_PERSON = "You don't see that person.\n";
const char* NOT_IN_POSN = "You are not in a position to do that.\n";

String EAST("east");
String WEST("west");
String NORTH("north");
String SOUTH("south");
String NORTHEAST("northeast");
String SOUTHWEST("southwest");
String NORTHWEST("northwest");
String SOUTHEAST("southeast");
String UP("up");
String DOWN("down");

String SOMEONE("Someone");
String UNKNOWN("UNKNOWN");
String SOMETHING("something");
String SOMEWHERE("somewhere");
String UNKNOWN_DESC("You have reached a vague place that does not really exist!");

String NULL_STRING;

const String ANSI_BLACK_S("FG_BLACK");
const String ANSI_RED_S("FG_RED");
const String ANSI_GREEN_S("FG_GREEN");
const String ANSI_YELLOW_S("FG_YELLOW");
const String ANSI_BLUE_S("FG_BLUE");
const String ANSI_MAGENTA_S("FG_MAGENTA");
const String ANSI_CYAN_S("FG_CYAN");
const String ANSI_WHITE_S("FG_WHITE");

const String ANSI_BBLACK_S("BG_BLACK");
const String ANSI_BRED_S("BG_RED");
const String ANSI_BGREEN_S("BG_GREEN");
const String ANSI_BYELLOW_S("BG_YELLOW");
const String ANSI_BBLUE_S("BG_BLUE");
const String ANSI_BMAGENTA_S("BG_MAGENTA");
const String ANSI_BCYAN_S("BG_CYAN");
const String ANSI_BWHITE_S("BG_WHITE");


const unsigned int Max_Int = (1 << (sizeof(int) * 8 - 1));
const unsigned long Max_Long = (1 << (sizeof(long int) * 8 - 1));
const unsigned short Max_Short = (1 << (sizeof(short int) * 8 - 1));

const char* Opening_Screen = "<__SCRY__>\n\n\n


                                                       $$$
                                                       `$$
               pyppy$$$ pyppy$$$ pyppy$$$ pyp  $$$      $$
               $$$'`$$$ $$$'`$$$ $$$'`$$$ $$$  $$$      $$
    |   ,$$$$$ $$$  `\"' $$$  $$$ $$$  $$$ $$$  $$$ $$$$ $$             $$$$
  \\  ,$$$$$$$$ $$$      $$$  `\"' $$$  $$$ $$$  $$$ $$$$ $$ $$$ $$$ $$$ $$$'
--  $$$$$$'    $$$py$$$ $$$      $$$py$$' $$$py$$$   `$ $$ $$$ $$$ $$$ $$$
  /  `$$$$$$$$ `\"'  $$$ $$$  $$$ $$$'`$$y `\"'  $$$ $$$$ $$ $$$ $$$ $$$ $$$.
    |   `$$$$$ pyp  $$$ $$$  $$$ $$$  $$$ $$$  $$$ $$$$ $$             $$$$
               $$$py$$$ $$$py$$$ $$$  $$$ $$$py$$$      $$
               `\"'      `\"'      `\"'      `\"'           $$
                                                       ,$$
                          ScryMUD                      $$$

                     Original Code and World

        Written with DikuMUD and its derivatives in mind.

                Code:  Ben Greear AKA Grock
        Head Builder:  Shamu
            Web Page:  http://www.primenet.com/~greear/ScryMUD/scry.html
               Email:  greear@cyberhighway.net

       Thanks to the kind folks at primenet.com (www.primenet.com) for
       the site.  $21 a month gets unlimited usage, and they will give
       you a static login account for a one time fee.


";
//Opening_Screen


///****************  hardcoded global lists ****************///
char* critter_positions[10] = {
   "is standing here.",
   "is lying prone.",
   "is sitting here.",
   "is resting here quietly.",
   "is sleeping here.",
   "is meditating here.",
   "is lying here barely breathing.",
   "is lying here utterly dead!.",
   "is lying here prone.",
   NULL
};// critter_positions

char* critter_conditions[11] = {
   "is in horrible condition.",  // 0 - 10%
   "looks pretty much whipped.",  // 11 - 20%
   "is bleeding badly from numerous wounds.",  // 21 - 30%
   "isn't looking too good.",  // 31 - 40%
   "is bleeding from a few wounds.",  // 41 - 50%
   "is definitely going to feel it in the morning!",  // 51 - 60%
   "is looking quite beat up.",  // 61 - 70%
   "has bruises in several places.",  // 71 - 80%
   "is looking a little flustered.",  // 81 - 90%
   "looks perfectly healthy."  // 91 - 100%
};//critter_conditions

long levels[45] = { 
      500, 1000, 2000, 4000, 8000, 20000,
      50000, 100000, 150000, 200000,/*tenth*/ 300000, 400000, 500000, 
      750000, 1000000, 1500000, 2000000, 2750000, 3500000, 
      5000000,/*20_th*/ 7000000, 9000000, 11000000, 14000000, 17000000,
      20000000, 25000000, 30000000, 35000000, 50000000,/*30_th*/
      70000000, 90000000, 110000000, 130000000, 150000000, 170000000,
      190000000, 210000000, 230000000, 250000000, 300000000
   };//basic exp level cutoffs  


char* login_prompts[] = {
   "\nEnter thy name:  ",
			/* for new players */
   
   //"\n\x9B\x4D\x31\x32\x68\nChoose a Password:  ", 
   //"\n\x9B\x4D\x31\x32\x68\nEnter your password again for verification:  ",
   "\nChoose a Password (Will be Echoed):  ", 
   "\nEnter your password again for verification:  ",
   "(M)ale, (F)emale, or (N)euter:  ",
   "\n\nSelection of Classes: \n
   Your class may affect your stats and will have an influence on
   the skills you are able to acquire.
	Warrior		(1)	
	Sage		(2)
   	Wizard		(3)
   	Ranger		(4)
   	Thief		(5)
   	Cleric		(7)
   	Bard		(8)

   Your choice is:  ",
					/* for non-new players */

   //"\n\x9B\x31\x32\x68\nEnter your password:  ",
   "\nEnter your password (Will be echoed):  ",
   
   "\nChoose your Race, or you can choose help on the races.

		        Race		Help on Race
	Human		(1)			(101)
	Anitre		(2)			(102)
	Darkling	(4)			(104)
	Dwarf		(7)			(107)
	Ogrue		(9)			(109)
	Elf		(11)			(111)

	Your choice: "


}; //login_prompts

//  This race was never defined good enough to add.
//        Sombrian        (14)                    (114)


                         /* data for OLC */

char* olc_prompts[] = {
/* 0 */
"\n\nWhich do you wish to create: \n
(1)object, (2)critter, (3)room, (4)door, (5)quit, (6)vehicle\n",
		
/* 1 */		/* here is output for the object. */
"\nInput the object_num:  
If you don't know, guess, the system will suggest one.\n",

/* 2 */
"\nInput a name, one word, '~', by itself, terminates.
Example:  garland rose ~\n",

/* 3 */
"\nInput the short description.
Example:  a garland\n",

/* 4 */
"\nInput the in_room_description, do capitalize:
Example:  A short sword sits here rusting.\n",

/* 5 */
"\nInput the long description, '~', ON LINE BY ITSELF, terminates:
Example:
    You see a sparsely descripted object lying here.
~\n", 

/* 6 */
"\nTime to set obj_flags.  The number of the flag will toggle it,
a -1 terminates.

0 no_rest,  1 !evil, 2 !neutral, 3 !good, 4 !donate, 5 !drop, 6 !remove, 
7 !mortal, 8 !imm, 9 !demi, 10 !god, 11 !Warrior, 12 !sage, 13 !wizard, 
14 !ranger, 15 !thief, 16 !alchemist, 17 !cleric, 18 !bard, 19 !mob, 20 !pc

21 not worn, 22 head, 23 neck, 24 neck, 25 around body, 26 arms, 27 wrist1,
28 wrist2, 29 hands, 30 wielded, 31 held, 32 light, 33 body, 34 belt,
35 legs, 36 feet, 37 finger1, 38 finger2, 39 shield,

40 not a weapon, 41 slash, 42 smash, 43 pierce, 44 whip, 45 needs_ammo,
46 dart_thrower, 47 bow, 48 dart, 49 arrow, 50 junk, 51 wand, 52 potion,
53 scroll, 54 container, 55 coins, 56 armor, 57 weapon, 58 light_source,
59 canteen, 60 liquid, 61 food, 62 boat, 
63 has_spec_proc_data { flag for scrolls, potions, wands...}
64 toolbox, 65 cauldron, 66 pen, 67 (DO NOT USE),
68 (DO NOT USE), 69 parchment (for scribing), 71 (DO NOT USE),
72 herb, 73 vend_machine, 74 bulletin_board

***  If you're making a weapon, please consider flagging a flag between
     41 and 49 inclusive.  This will help determine damage messages.
***  Remember to flag 63 if you want spec procs or it is a component. ***\n",

/* 7 */
"\nEnter all these extras at once please:
		
(1)charges or light_count, 	(2)times rechargeable (wands), 
(3)load percentage,		(4)max_in_game, 
(5)weight(lbs), 		(6)dice_sides, 
(7)dice_count,			(8)level
Example:  0 0 25 15 5 3 2 5

WARNING:  if you make the object too powerful it will be deleted.
7d4 is the max, and most should fall well under that.\n",

/* 8 */
"\nEnter all these cur_stats at once please:

vis_bit (To calculate add flags up:
	{2 invis, 4 hidden, 8 god_invis1},
price,  (for comparison:  a good sword 4d4 should be around 3k)\n",

/* 9 */
"\nReading in bag_specific information.  First, the bag_flags.
The number of the flag will toggle it, a -1 terminates.

0 NULL, 1 NULL, 2 is_closed, 3 is_locked, 4 is_pickable, 5 is_mag_lockable,
6 is_mag_locked, 7 is_destructable, 8 is_corpse (probably shouldn't use)
9 is_NON_closeable, 17 consumes_key\n",

/* 10 */
"\nEnter all of these bag data at once please:

(1)key number (zero if no key),		
(2)maximum weight(lbs) NOTE: this is ITSELF plus its INVENTORY
(3)percentage weight (normal is 100),	
(4)time till it disolves, (probably should make this -1)\n",

/* 11 */
"\nEnter any spells the item is to be Affected By:
spell number, followed by duration; -1 for duration is permanent.
A -1 for spell number terminates.  None in yet, choose -1.\n",

/* 12 */
"\nEnter the item's inventory, all at once, but ONLY IF ITS A CONTAINER:
A -1 will terminate.  Also, make sure the item exists!  'olist' can help
you figure out which you want to use.
Example:  1 6 5 -1\n",

/* 13 */
"\nNow for the Stat Affects:
stat number, followed by the change; A -1 for stat number terminates.
Example:  2 3 -1
Here are the stats you can fiddle with:

1 str, 2 int, 3 con, 4 cha, 5 wis, 6 dex, 7 hit, 8 dam, 9 ac, 10 attacks,
11 pause_count, 15 hp, 16 mana, 17 mov, 18 align, 19 level, 23 hp_max,
24 mana_max, 25 mov_max, 27 damage_received_modifier, 28 damage_given_modifier,
29 heat_resis, 30 cold_resis, 31 elect_resis, 32 spell_resis, 35 bare_hand_dice_count, 
36 bare_hand_dice_sides, 37 hp_regen, 38 mana_regen, 39 mov_regen,
100 hunger, 101 thirst, 102 drugged.

WARNING:  Once again, moderation is needed, if it is too extreme it will be
	  deleted.  This is NOT a twink MUD!\n",

/* 14 */ 
"\nTime to set obj_proc_flags.  The number of the flag will toggle it,
a -1 terminates.

0 consume_teleport, 1 (DO NOT USE, cannot be set here)
2 has_skin, 3 consume_poison

Other spells that it can CAST will be specified later.\n",

/* more obj prompts below */

//*********************************************************************//
///***********************  critter prompts  *************************///
//*********************************************************************//

/* 15 */		/* here is output for the object. */
"\nInput the critter number:  
If you don't know which one, try one, the game will suggest another if 
its bad.\n",

/* 16 */
"\nInput a name, one word, '~', by itself, terminates.
Example: guard portal ~\n",

/* 17 */
"\nInput the short description.
Example:  a portal guard\n",

/* 18 */
"\nInput the in_room_description, do capitalize:
Example:  A portal guard leans against the door.\n",

/* 19 */
"\nInput the long description, '~', ON LINE BY ITSELF, terminates:
Example:  
	You see a vague person standing here.
~\n", 

/* 20 */
"\nTime to set crit_flags.  The number of the flag will toggle it,
a -1 terminates.

3 is_flying, 5 can_climb, 6 gos, 7 yell, 8 gratz, 9 auc, 10 shout,
11 say, 12 tell, 13 wiznet, 14 is_paralyzed, 15 is_perm_sleeped,
16 is_dual_wielding, 17 is_sneak, 19 can_dive.\n",

/* 21 */ // long data
"\nEnter all these data at once please:
		
(1)gold		 (2)exp worth (lev 15 mob ~= 4k, lev 20 ~= 10k)
(3)bank_gold (Zero is normal)

Example:  566 2225 0\n",

/* 22 */ //first 15 short_cur_stats
"\nEnter all 15 of these short_cur_stats at once please:

(1)position  {0 stand, 1 prone, 2 sit, 3 rest, 4 sleep, 5 med, 6 stun, 7 dead}
(2)str, (3)int, (4)con, (5)cha, (6)wis, (7)dex, (8)hit, (9)dam, (10)ac,
(11)attacks per round, (12)pause_count {0 is good}, 
(13)sex {0 female, 1 male, 2 neuter}
(14)class, {1 Warrior, 2 Sage, 3 Wizard, 4 Ranger, 5 Thief, 6 Alchemist, 
          {7 Cleric, 8 Bard}
(15)race, {1 human, 2 anitre, 3 avintre, 4 darkling, 5 drow,
	   6 dragon, 7 dwarf, 8 ironclad, 9 ogrue, 10 rocktroll,
	   11 elf, 12 faerie, 13 entity, 14 sombrian, 15 undead,
	   16 animal, 17 monster}\n",

/* 23, second 15 short_cur_stats */
"\nEnter all 15 of these short_cur_stats at once please:

(1)hp, (2)mana, (3)mov, (4)align, (5)level, 
(6) hometowns: which zone# is the mob from??}
(7)wimpy (8)practices, (9)hp_max, (10)mana_max, (11)mov_max, 
(12)type_of_mob: {2 mob}, (13)damage_received_modifier (100 is normal),
(14)damage_given_modifier (100 is normal), (15)heat_resis\n",

/* 24, rest of short_cur_stats */
"\nEnter all of these short_cur_stats at once please:

 {0 is very resistant, 100 not very resistant }

(1)cold_resis, (2)elect_resis, (3)spell_resis
(4)religion { 0 none, 1 fire, 2 ice, 3 air, 4 earth, 5 beast, 6 blade }
(5)NULL, (6)bare_hand_dice_count, (7)bare_hand_dice_sides, (8)hp_regen, 
(9)mana_regen, (10)mov_regen, (11)NULL, (12)naked_weight, (13)NULL,

Remember, you should enter zero's for NULL fields, they may one day
be defined and zero will be default.\n",

/* 25, cur_stats */
"\n(1)vis_bit: add these up:
             {1 dark, 2 invis, 4 hidden, 8 god_invis1}
(2)see_bit:  add these up:
	     {0 normal, 1 dark, 2 invis, 4 hidden,
	      8 god_invis1, 16 god_invis2, 32 detect_link_dead}
(3)in_room:  0 is just fine here.\n",

/* 26 eq */
"\nEnter eq in (item#, posn) pairs.  Negative one terms.
Example:  8 1 -1     Make sure item exists btw.

Positions: 1 head, 2 neck, 3 neck, 4 around body, 5 arms, 6 wrist, 7 wrist,
8 hands, 9 wielded, 10 held, 11 light, 12 body, 13 belt, 14 legs,
15 feet, 16 finger, 17 finger, 18 shield. DO NOT USE ZERO.\n",

/* 27 affected by */
"\nEnter any spells the mob is to be Affected By:
spell number, followed by duration; -1 for duration is permanent.
A -1 for spell number terminates.

NOTE:  Check slist for a list of spells.\n",

/* 28 inventory */
"\nEnter the mob's inventory, all at once is fine.
A -1 will terminate.  Also, make sure the item exists!
Example:  1 6 5 -1\n",

/* 29 mob_data: max_in_game, skin_num*/
"\nEnter the max number of these mobs to be loaded in the game 
simultaneously AND the object number of the skin, if it has one.  If
it does not have a skin, enter a zero

Example:  4 0\n",

/* 30 mob_data flags */ 
"\nTime to set mob_data_flags.  The number of the flag will toggle it,
a -1 terminates.

0 has_proc_data, (teacher, shopkeeper, discuss, bow, curse, give procs,
                  PERSONALITY),
1 scavenge, 2 wander, 5 edible_corpse, 6 is_banker, 16 has_skin. \n",

/* 31 spec_data: int1 */
"\nIf mob is to be a sentinel, give direction guarding:
{1 east, 2 west, 3 north, 4 south, 5 southeast, 6 northeast, 7 northwest,
 8 southwest, 9 up, 10 down}

The mob will not let anyone pass through the door guarded.  You can specify
it to let certain classes/races past with the next bitfield.

If it is not a sentinel, enter zero.

So you should enter:
[1] sentinal direction
  These next 4 range in value from -10 to 10, they specify the mobs BEHAVIOR!
[2] skill_violence (how often should the mob use skills, -10 never, 10 always)
[3] benevolence (-10 very aggressive, 10 very peaceful)
[4] defensiveness (-10 must hurt others, 10 maximum self preservation)
[5] bad_assedness (-10 super wimp, 10 Snake Pliskin)
[6] social_awareness (-10 who cares, 10 I'll save the world!)

NOTE:  Zero is default value for all (but you must enter SOMETHING).\n",

/* 32 proc_data: proc_data_flags */
"\nTime to set proc_data flags.  The number of the flag will toggle it,
a -1 terminates.  You don't have to set any flags if you don't want to.

1 is_shopkeeper, 2 is_teacher,
3 let_same_class_pass, 4 let_same_race_pass (these are used with sentinels),
5 has_mob_give_proc, 6 has_mob_say_proc (discuss), 7 has_mob_bow_proc,
8 has_mob_curse_proc, 9 do_procs_with_same_race_only, 10 same_align,
11 same_class\n",

/* 33 shopkeeper, markup, buy% */
"\nEnter all these at once:
(1)markup_percentage 		(2)buy_percentage
(For example:  50 would be half, 200 would be twice.)
(3) open_time		(4)close_time		(military)\n",

/* 34 shopkeeper:  shop_data_flags */
/* NOTE:  Don't reuse the first 3, were buy, sell, list procs */
"\nTime to set shop_data flags.  The number of the flag will toggle it,
a -1 terminates.  Choose at least one buy, sell, and offer proc.  More
may be defined later, for now your choice is easy!

Choose what type of eq it trades in:
46 dart_thrower, 47 bow, 48 dart, 49 arrow,
50 junk, 51 wands, 52 potions, 53 scrolls, 54 containers,
56 armor, 57 weapons, 58 lights, 59 canteens, 61 foods, 62 boats,
64 toolbox, 65 cauldron, 66 pen, 67 construct_component,
68 concoct_component, 69 parchment(blank for scribing).

Note:  You can choose more than one.\n",

/* 35 shopkeeper:  perm_inv */
"\nNow enter any items (numbers) that the shopkeeper will ALWAYS be selling.
A negative one terminates.
Example:  8 -1\n",

/* 36 teacher:  teach_data_flags */
"\nTime to set teach_data flags.  The number of the flag will toggle it,
a -1 terminates.  Choose what classes it will teach to:

1 Warrior, 2 Sage, 3 Wizard, 4 Ranger, 5 Thief, 6 Alchemist, 7 Cleric, 8 Bard

Note:  You can choose more than one.\n",

/***********************************************************************/
/*  37 THIS IS AN OBJECT PROMPT FOR COMPONENTS */
/* Should no longer be used btw. */
"\nEnter the these construct data:

(1) target_object (what it can be constructed into)
(2) item1 (these items are fellow components, zero is NULL value)
(3) item2      (4) item3      (5) item4     (6) item5.\n",


//**********************************************************************//
//**************************  room prompts  ****************************//
//**********************************************************************//

/* 38 */	
"\nInput the room number (enter 1 for next available room):  ",

/* 39 */
"\nInput a name, one word, '~' terminates.  Note:  This will rarely be seen.
Example: temple ~\n",

/* 40 */
"\nInput the short description, you SHOULD capitalize:
Example:  The temple of Garland\n",

/* 41 */
"\nInput the long description, '~', ON LINE BY ITSELF, terminates:
Example:  You see a non-descript room.\n", 

/* 42 */
"\nTime to set room_flags.  The number of the flag will toggle it,
a -1 terminates.

0 no_rest, 1 !imm, 2 !god, 3 is_perm_dark, 4 weather, 5 !shout, 6 !mag_exit,
7 is_haven, 8 !pk, 9 !magic, 10 !mob, 11 !potions, 12 !staffs, 13 !mort
14 normally_dark, 15 river/small lake, 16 ocean/big lake, 17 swamp,
18 need_fly, 19 need_boat, 20 need_climb, 24 !magic_entry,
25 !vehicles (vehicles can't drive here), 26 is_cramped (!HUGE),
27 !ranged combat, 28 need_dive_ability, 30 can_camp 33 !mob_wander,
34 !foreign_mob_wander\n",

/* 43 */ // cur_stats data
"\nEnter all these data at once please:
		
(1)vis_bit {0 NORMAL, 1 dark, 2 invis, 4 hidden, 8 god_invis1,
            16 god_invis2}
(2)movement cost(in movement points)\n",

/* 44 affected by */
"\nEnter any spells the room is to be Affected By:
spell number, followed by duration; -1 for duration is permanent.
A -1 for spell number terminates.  Probably want to just enter -1.\n",
 
/* 45 inventory */
"\nEnter the room's inventory, all at once is fine.
A -1 will terminate.  Also, make sure the item exists!
Example:  1 6 5 -1\n",

/* 46  direction */
"\nEnter the distance, and the direction of the exit TO this new room.
Do not abbreviate, example:  0 north or 2 northwest or 1 up...

NOTE:  distance is a unit of battle rounds.  This lasts about 2 seconds.
For each distance unit you give, the mover will be paused for this amount
of time, just as the skill 'bash' works on this and other muds.\n",


//**********************************************************************//
//**************************  door prompts  ****************************//
//**********************************************************************//

/* 47 */	
"\nInput the door number:  ",

/* 48 */
"\nInput door names, following the example, '~' terminates
Example: east    door oak      #      oak door   west    ~
         ^^^^    ^^^^^^^^      ^      ^^^^^^^^   ^^^^
         dir     names     separater    names    opposite side's dir\n",

/* 49 */
"\nInput the long description, '~', ON LINE BY ITSELF,  terminates:
Example:  You see a non-descript door.\n", 

/* 50 */
"\nTime to set door_flags.  The number of the flag will toggle it,
a -1 terminates.

1 mag_lockable, 2 is_closed, 3 is_locked, 4 is_pickable, 5 is_lockable,
6 mag_locked, 7 is_destructable, 8 is_closeable, 
11 !open (by pc or mob, used for vehicle doors),
12 is_exit_from_vehicle (to the outside), 13 is_secret
16 secret_when_open_too, 17 consumes_key, 18 !passdoor\n",

/* 51 vis_bit, token_num, key_num */
"\nInput all of these at once please:

Add these up to get the door's vis_bit:
(1)vis_bit {0 NULL, 1 dark, 2 invis, 4 hidden, 8 god_invis1,
            16 god_invis2}
(2) token_number (zero if no token needed, for vehicle doors mostly)
(3) key_number   (zero if no key needed)\n",

/* 52 vehicle::route */
"Please enter the flags for this vehicle.  Entering the number of the
flag toggles it.  When complete, enter -1.

0 is_self_guided, 1 is_stealthy (arrival un-announced), 2 unlimited_fuel,
3 can_fly, 4 can_climb, 5 can_float, 6 can_dive,
7 can_see_out (show room descs to passengers) 
\n",

/* 53 fuel, time_between_stops, cur_fuel, max_fuel */
"\nEnter all these at once please:
NOTE:  Consider fuel units to be equal to movement points in size.
NOTE:  For unlimited fuel, you should toggle the appropriate flag (2).

(1) time_between_stops   (in mini_ticks/battle rounds, how fast it moves, 5-10)
(2) current fuel         (What the vehicle starts out with.)
(3) max_fuel             (What is the maximum amount of fuel it can hold.)\n",

/* 54 vehicle::number, */
"\nInput the vehicle number:  ",

/* 55 proc_data:  give_proc */
"\nPlease enter the object number that should TRIGGER this proc when given to
the mob.  Example:  55.  
Note that this object should be created already as usual.\n",

/* 56 proc_data:  say_proc (discuss) */
"\nPlease input the topic for this conversation.  For example, if you won't
this discussion to be triggered upon a 'discuss history <this_mob>', you would
enter 'history' here.  The quotes are NOT NEEDED.  Remember:  only an
exact match (case insensitive of course) will allow the discussion to take
place, so you will probably want to keep it simple.

Example:  history\n",

/* 57 proc_data:  bow_proc */
"\nPlease enter the message the person is to see when they bow to this mob.
TERMINATE with a ~ on a LINE BY ITSELF!
Example, for some priest:  

The priest tells you 'You're attitude is encouraging, you should go discuss
the brotherhood with the abbot in Elden Noir.'\n",

/* 58 proc_data:  curse_proc:  message */

"\nPlease enter the message you would like shown to the person who will
'curse' this mob.  TERMINATE with a ~ on a LINE BY ITSELF!
Example, for some priest:

The priest scowls and says, 'Perhaps THIS will teach you to respect the powers
of good!'

The priest may then transport the person somewhere...Hell perhaps??\n",

/* 59 spec_data:  say proc, response */
"\nNow enter the response to this topic of discussion.  
TERMINATE with a ~ on a LINE BY ITSELF!
For example, for a priest and \'brotherhood\' for a topic:

The abbot tells you, \'So you wish to join our brotherhood eh?  Consider
yourself an accolyte, but if you truly wish to gain favor, bring me a root of
the Golden Seal plant for my healing potions.  It grows in poplar shaded
groves in the mountains to the east.\'

Perhaps this priest will also have a give proc for the herb...\n",

/* 60 proc_data:  give_proc correct_msg */
"\nPlease enter the message to be shown upon a CORRECT gift.
TERMINATE with a ~ on a LINE BY ITSELF!
Example for an abbot:  

The abbot smiles and says, \'Thanks, because of your worthy quest I will tell
you something of the lore of herbs.\'

Then perhaps the abbot will teach a skill/spell 'herb lore' or something.\n",

/* 61 proc_data:  bow_proc skill_name */
"\nIf you wish to have the mob give the player a skill/spell, enter that here.
Example:  begging
   
Perhaps a skill an old begger would give a kind soul?
If you don\'t want anything, please enter NA or na for not applicable.\n",


/* 62 proc_data:  curse_proc skill_name */
"\nIf you wish to have the mob give the player a skill/spell, enter that here.
Example:  ancient tongues

Perhaps a skill a crusty old geezer would give a young upstart that shows
courage, if not tact.  Then perhaps the geezer would transport him to some
evil place?
If you don't want anything, please enter NA or na for not applicable.\n",

/* 63 proc_data:  say_proc skill_name */
"\nIf you wish to have the mob give the player a skill/spell, enter that here.
Example:  history   

Perhaps a skill a sage would give to a serious student?
If you don\'t want anything, please enter NA or na for not applicable.\n",

/* 64 proc_data:  give_proc skill_name */
"\nIf you wish to have the mob give the player a skill/spell, enter that here.

Example:  herb lore

If you don\'t want anything, please enter NA or na for not applicable.\n",

/* 65 proc_data:  bow_proc, obj_num */
"\nPlease enter the number of the object the mob is to give to who ever
bows to him/her.  If you don\'t want anything to be given, enter ZERO (0).
Example:  82 \n",

/* 66 proc_data:  curse_proc, obj_num */
"\nPlease enter the number of the object the mob is to give to who ever
curses him/her.  If you don\'t want anything to be given, enter ZERO (0).
Example:  66 \n",

/* 67 proc_data:  say_proc, obj_num */
"\nPlease enter the number of the object the mob is to give to who ever
discusses this topic.  If you don\'t want anything to be given, enter ZERO (0).
Example:  0 \n",

/* 68 proc_data:  give_proc, obj_num */
"\nPlease enter the number of the object the mob is to give to who ever
gives the RIGHT gift.  If you don\'t want anything to be given, enter ZERO (0).
Example:  24 \n",

/* 69 proc_data:  bow_proc, trans_to_room */
"\nPlease enter the number of the room the mob is to transport the person who 
bows to him/her to.  
If you don\'t want the person transported anywhere, enter ZERO (0).
Example:  0 \n",

/* 70 proc_data:  curse_proc, trans_to_room */
"\nPlease enter the number of the room the mob is transport the person who 
curses him/her to.  
If you don\'t want the person transported anywhere, enter ZERO (0).
Example:  666 \n",

/* 71 proc_data: say_proc, trans_to_room */
"\nPlease enter the number of the room which the mob is to transport the
person who discusses the given topic.  
If you don\'t want the person transported anywhere, enter ZERO (0).
Example:  1492 \n",

/* 72 proc_data: give_proc, trans_to_room */
"\nPlease enter the number of the room which the mob is transport the person
to who gives the CORRECT gift.  
If you don\'t want the person transported anywhere, enter ZERO (0).
Example:  1492 \n",

/* 73 proc_data:  say_proc, prompt for another? */
"\nWould you like to add another discussion topic?  Yes(1) No(0)
Example:  0 \n",

/* 74 proc_data:  give_proc, wrong_gift_msg */
"\nPlease enter the message you would like the player to see if he/she gives
a wrong item to this mob.  
TERMINATE with a ~ on a LINE BY ITSELF!

Example:  Thanks, but what I really need is a silver dragon scale. \n",


/* 75 proc_data: wrong_align_msg  */
"\nPlease enter the message you would like a player of the wrong alignment 
(as specified by flags set earlier (or not set)) to see.
TERMINATE with a ~ on a LINE BY ITSELF!

Example:  The priest frowns and tells you, 'If I wasn't a peaceful man
I'd banish your petulant soul to the nine rings of Hell!!' \n",

/* 76 proc_data: wrong_class_msg  */
"\nPlease enter the message you would like a player of the wrong class 
(as specified by flags set earlier (or not set)) to see.
TERMINATE with a ~ on a LINE BY ITSELF!

Example:  The wizzard laughs and says, 'Why would I want to talk to one of
your profession??'\n",

/* 77 proc_data: wrong_race_msg  */
"\nPlease enter the message you would like a player of the wrong race 
(as specified by flags set earlier (or not set)) to see.
TERMINATE with a ~ on a LINE BY ITSELF!

Example:  The dwarf laughs and says, 'You kind are not welcome in our
hallowed tunnels.'\n",

/* 78 obj_spec_proc:  skin_num  */
"\nPlease enter the number of the object that is to be the skin.\n\n",

/* 79 casts_these_spells */
"\nNow enter the level of the spell it will cast, and the spell number it
will cast.  You can find the spell_numbers via the 'slist' command.
A negative one (-1) terminates.  (LEVEL SPELL_NUM) pairs, level first.

Example: 30 10 -1  /* This would make it cast a level 30 'web' spell. */\n"


}; //olc prompts
