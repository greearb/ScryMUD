// $Id: const.cc,v 1.26 1999/09/11 06:12:16 greear Exp $
// $Revision: 1.26 $  $Author: greear $ $Date: 1999/09/11 06:12:16 $

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

const char* regular_directions[] = {
   "east",
   "west",
   "north",
   "south",
   "southeast",
   "northeast",
   "northwest",
   "southwest",
   "up",
   "down"
};

CSentryE osd_names[] = {
   CS_CONSUME_TELEPORT_F,
   CS_CONSTR_DATA_F,
   CS_HAS_SKIN_F,
   CS_CONSUME_POISON_F,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_CASTS_SPELLS_F
};
const LstrArray OBJ_SPEC_DATA_FLAGS_NAMES(sizeof(osd_names)/sizeof(CSentryE),
                                              osd_names, CS_OBJ_SPEC_DATA_FLAGS);

CSentryE bf_names[] = { 
   CS_OPEN_EXIT_F,
   CS_MAG_LOCKABLE_F,
   CS_IS_CLOSED_F,
   CS_IS_LOCKED_F,
   CS_IS_PICKABLE_F,
   CS_IS_LOCKED_F, // 5
   CS_IS_MAG_LOCKED_F,
   CS_IS_DESTRUCTABLE_F, //7
   CS_IS_CLOSABLE_F, //8
   CS_IS_FLIPPABLE_F,
   CS_IS_IN_USE_F, //10
   CS_IS_NO_OPEN_F,
   CS_IS_VEHICLE_EXIT_F, //12
   CS_IS_SECRET_F,
   CS_IS_BLOCKED_F,
   CS_IS_NOT_COMPLETE_F,
   CS_SECRET_WHEN_OPEN_F,
   CS_CONSUME_KEY_F,
   CS_NO_PASSDOOR_F,
   CS_IS_CORPSE_F
};
const LstrArray CLOSABLE_FLAGS_NAMES(sizeof(bf_names)/sizeof(CSentryE),
                                     bf_names, CS_CONTAINER_FLAGS);

CSentryE sd_names[] = {
   CS_BUY_ZERO_F,
   CS_SELL_ZERO_F,
   CS_OFFER_ZERO_F,
   CS_PLAYER_RUN_SHOPKEEPER_F, //3
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,//10
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,//20
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,//30
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NON_WEAPON_F,
   CS_SLASH_F,
   CS_SMASH_F,
   CS_PIERCE_F,
   CS_WHIP_F,
   CS_NEEDS_AMMO_F,
   CS_DART_THROWER_F,
   CS_BOW_F,
   CS_DART_F,
   CS_ARROW_F,
   CS_JUNK_F, //50
   CS_WAND_F,
   CS_POTION_F,
   CS_SCROLL_F,
   CS_CONTAINER_F,
   CS_COINS_F,//55
   CS_ARMOR_F,
   CS_WEAPON_F,
   CS_LIGHT_SOURCE_F,
   CS_CANTEEN_F,
   CS_LIQUID_F,//60
   CS_FOOD_F,
   CS_BOAT_F,
   CS_NULL,
   CS_TOOLBOX_F,
   CS_CAULDRON_F,//65
   CS_PEN_F,
   CS_CONSTR_COMP_F,
   CS_CONCOCT_COMP_F,
   CS_PARCHMENT_F,
   CS_NULL,
   CS_NULL,
   CS_HERB_F, //72
   CS_VEND_MACHINE_F//73
};
const LstrArray SHOP_DATA_FLAGS_NAMES(sizeof(sd_names)/sizeof(CSentryE),
                                      sd_names, CS_SHOP_DATA_FLAGS);

CSentryE class_names[] = {
   CS_NULL,
   CS_WARRIOR,
   CS_SAGE,
   CS_WIZARD,
   CS_RANGER,
   CS_THIEF,
   CS_ALCHEMIST,
   CS_CLERIC,
   CS_BARD
};
const LstrArray TEACH_DATA_FLAGS_NAMES(sizeof(class_names)/sizeof(CSentryE),
                                       class_names, CS_TEACH_DATA_FLAGS);

/** FLAG1 */
CSentryE mp_names[] = {
   CS_NULL,
   CS_SHOPKEEPER_F,
   CS_TEACHER_F,
   CS_SAME_CLASS_PASS_F,
   CS_LET_SAME_RACE_PASS_F,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_NULL,
   CS_HAS_AI_F,
};
const LstrArray MOB_PROC_DATA_FLAGS_NAMES(sizeof(mp_names)/sizeof(CSentryE),
                                          mp_names, CS_MOB_PROC_FLAGS);

CSentryE c_names[] = {
   CS_CAN_SEE_INV_F,
   CS_USING_LIGHT_SRC_F,
   CS_NULL,
   CS_IS_FLYING_F,
   CS_HAVE_BOAT_F,
   CS_CAN_CLIMB_F,
   CS_GOS_F,
   CS_YELL_F,
   CS_GRATZ_F,
   CS_AUC_F,
   CS_SHOUT_F,
   CS_SAY_F,
   CS_TELL_F,
   CS_WIZNET_F,
   CS_PARALYZED_F,
   CS_PERM_SLEEPED_F,
   CS_DUAL_WIELDING_F,
   CS_IS_SNEAK_F,
   CS_IS_IN_USE_F,
   CS_CAN_DIVE_F,
   CS_SPELL_TESTED_YET_F,
   CS_IS_BLOCKED_F,
   CS_IS_HIDING_F,
   CS_IS_TAILING_F,
   CS_IS_NOT_COMPLETE_F,
   CS_ALREADY_HURLED_F
};
const LstrArray CRIT_FLAGS_NAMES(sizeof(c_names)/sizeof(CSentryE),
                                 c_names, CS_CRIT_FLAGS);

CSentryE md_names[] = {
   CS_HAS_PROC_DATA_F,
   CS_SCAVENGE_F,
   CS_WANDER_F,
   CS_SHOULD_DO_PROCS_F,
   CS_NEEDS_RESETTING_F, //4
   CS_EDIBLE_CORPSE_F,
   CS_IS_BANKER_F,
   CS_IS_SESSILE_F, //7
   CS_IS_NO_HOMING_F, //8
   CS_NULL, //9
   CS_NULL, //10
   CS_NULL, //11
   CS_NULL, //12
   CS_NULL, //13
   CS_NULL, //14
   CS_NULL, //15
   CS_HAS_SKIN_F,
   CS_HAS_SCRIPT_F, /*17*/
};
const LstrArray MOB_DATA_FLAGS_NAMES(sizeof(md_names)/sizeof(CSentryE),
                                     md_names, CS_MOB_DATA_FLAGS);

CSentryE pcd_names[] = {
   CS_FROZEN_F,
   CS_GAGGED_F,
   CS_HAS_IMM_DATA_F,
   CS_CLOAKED_F,
   CS_TANK_GRAPH_F,
   CS_USING_CLIENT_F,
   CS_AUTO_EXIT_F,
   CS_NO_HASSLE_F,
   CS_BRIEF_F,//8
   CS_AUTOSPLIT_F,
   CS_DO_PROMPT_F,//10
   CS_IS_BUILDER_F,
   CS_AUTOLOOT_F,//12
   CS_OLC_REDO_F,
   CS_EXTRA_INFO_F,//14
   CS_CR_BEHIND_F,
   CS_DO_CR_F,
   CS_IS_BLOCKING_DOOR_F,
   CS_CAN_DET_MAGIC_F,
   CS_DET_INV_F,
   CS_SHOW_VNUMS_F,//20
   CS_HAS_POOF_MSGS_F,
   CS_PAGE_OUTPUT_F,//22
   CS_PB_MODE_F,
   CS_NO_WIZCHAT_F,
   CS_HAS_COLORS_F,//25
   CS_USE_COLOR_F,
   CS_HAS_LANG_CHOICE_F,
   CS_NO_SHOW_NPC_ENTRY_F,
   CS_NO_BEEP_F,
   CS_SHOW_DBG_F
};
const LstrArray PC_DATA_FLAGS_NAMES(sizeof(pcd_names)/sizeof(CSentryE),
                                    pcd_names, CS_PC_DATA_FLAGS);


CSentryE v_names[] = {
   CS_IS_SELF_GUIDED_F,
   CS_STEALTHY_F,
   CS_UNLIMITED_FUEL_F,
   CS_IS_FLYING_F,
   CS_CAN_CLIMB_F,
   CS_CAN_FLOAT_F,
   CS_CAN_DIVE_F,
   CS_CAN_SEE_OUT_F,
};
const LstrArray VEHICLE_FLAGS_NAMES(sizeof(v_names)/sizeof(CSentryE),
                                    v_names, CS_VEH_FLAGS);
    
CSentryE r_names[] = {
   CS_NO_REST_F,
   CS_NO_IMM_F,//1
   CS_NO_GOD_F,//2
   CS_IS_PERM_DARK_F,//3
   CS_HAS_WEATHER_F,
   CS_NO_SHOUT_F,//5
   CS_NO_MAG_EXIT_F,
   CS_HAVEN_F,
   CS_NO_PK_F, //8
   CS_NO_MAGIC_F,
   CS_ANTI_NPC_F,//10
   CS_ANTI_POTION_F,
   CS_ANTI_STAFF_F,
   CS_ANTI_MORT_F,
   CS_NORMAL_DARK_F,
   CS_RIVER_LAKE_F,
   CS_OCEAN_LAKE_F,
   CS_SWAMP_F, //17
   CS_NEED_FLY_F,//18
   CS_NEED_BOAT_F,
   CS_NEED_CLIMB_F,
   CS_ZLOCKED_F,
   CS_TOT_LOADED_F,
   CS_IS_IN_USE_F,
   CS_NO_MAG_ENTRY_F,//24
   CS_NO_VEH_F,
   CS_CRAMPED_F,
   CS_NO_RANGED_F,
   CS_NEED_DIVE_F,
   CS_USED_IN_TRACK_F,
   CS_CAN_CAMP_F,//30
   CS_IS_NOT_COMPLETE_F,
   CS_HAS_KEYWORDS_F,
   CS_NO_MOB_WANDER_F,
   CS_NO_FOREIGN_MOB_WANDER_F,
   CS_HAS_SCRIPT_F//35
};
const LstrArray ROOM_FLAGS_NAMES(sizeof(r_names)/sizeof(CSentryE),
                                 r_names, CS_ROOM_FLAGS);

CSentryE o_names[] = {
   CS_NO_REST_F,
   CS_NO_EVIL_F,
   CS_NO_NEUT_F,
   CS_NO_GOOD_F,
   CS_NO_DONATE_F,
   CS_NO_DROP_F,
   CS_NO_REM_F,
   CS_ANTI_MORT_F,
   CS_NO_IMM_F,
   CS_NO_DEMI_F,
   CS_IS_IN_USE_F, //10
   CS_NO_WARRIOR_F,
   CS_NO_SAGE_F,
   CS_NO_WIZ_F,
   CS_NO_RANGER_F,
   CS_NO_THIEF_F,
   CS_NO_ALCH_F,
   CS_NO_CLERIC_F,
   CS_NO_BARD_F,
   CS_ANTI_NPC_F,
   CS_NO_PC_F,//20
   CS_NOT_WORN_F,
   CS_WORN_HEAD_F,
   CS_WORN_NECK_F,
   CS_WORN_NECK_F,
   CS_ARND_BODY_F,
   CS_WORN_ARMS_F,
   CS_WORN_W1_F,
   CS_WORN_W2_F,
   CS_WORN_HANDS_F,
   CS_WIELDED_F,//30
   CS_HELD_F,
   CS_WORN_LIGHT_F,
   CS_WORN_BODY_F,
   CS_BELT_F,
   CS_WORN_LEGS_F,
   CS_WORN_FEET_F,
   CS_WORN_FINGER1_F,
   CS_WORN_FINGER2_F,
   CS_SHIELD_F, //39
   CS_NON_WEAPON_F,
   CS_SLASH_F,
   CS_SMASH_F,
   CS_PIERCE_F,
   CS_WHIP_F,
   CS_NEEDS_AMMO_F,
   CS_DART_THROWER_F,
   CS_BOW_F,
   CS_DART_F,
   CS_ARROW_F,
   CS_JUNK_F, //50
   CS_WAND_F,
   CS_POTION_F,
   CS_SCROLL_F,
   CS_CONTAINER_F,
   CS_COINS_F,//55
   CS_ARMOR_F,
   CS_WEAPON_F,
   CS_LIGHT_SOURCE_F,
   CS_CANTEEN_F,
   CS_LIQUID_F,//60
   CS_FOOD_F,
   CS_BOAT_F,
   CS_HAS_PROC_DATA_F,
   CS_TOOLBOX_F,
   CS_CAULDRON_F,//65
   CS_PEN_F,
   CS_CONSTR_COMP_F,
   CS_CONCOCT_COMP_F,
   CS_PARCHMENT_F,
   CS_NEEDS_RESETTING_F,
   CS_IS_NOT_COMPLETE_F,
   CS_HERB_F, //72
   CS_VEND_MACHINE_F,
   CS_BULLETIN_BOARD_F,
   CS_BUTCHERABLE_F,
   CS_HAS_SCRIPT_F//76
};

const LstrArray OBJ_FLAGS_NAMES(sizeof(o_names)/sizeof(CSentryE),
                                o_names, CS_OBJ_FLAGS);

/** Stat Affect names */
CSentryE sa_names[] = {
   CS_POSN_F,
   CS_STR_F,
   CS_INT_F,
   CS_CON_F,
   CS_CHA_F,
   CS_WIZ_F,
   CS_DEX_F,
   CS_HIT_F,
   CS_DAM_F,
   CS_AC_F,
   CS_ATTACKS_F, //10
   CS_PAUSE_F,
   CS_SEX_F,
   CS_CLASS_F,
   CS_RACE_F,
   CS_CUR_HP_F,
   CS_CUR_MA_F,
   CS_CUR_MOV_F,
   CS_ALIGN_F,
   CS_LVL_F,
   CS_HOMETOWN_F, //20
   CS_WIMPY_F,
   CS_PRAC_F,
   CS_MAX_HP_F,
   CS_MAX_MA_F,
   CS_MAX_MOV_F,
   CS_CTYPE_F,
   CS_DRM_F,
   CS_DGM_F,
   CS_HT_RESIS_F,
   CS_CLD_RESIS_F,
   CS_ELEC_RESIS_F,
   CS_SPELL_RESIS_F,
   CS_RELIGION_F,
   CS_HOME_ZONE_F,
   CS_BHDC_F,
   CS_BHDS_F,
   CS_HP_REGEN_F,
   CS_MA_REGEN_F,
   CS_MV_REGEN_F,
   CS_GUILD_F,
   CS_NAKED_WEIGHT_F,
   CS_NULL
};
const LstrArray STAT_AFFECT_NAMES(sizeof(sa_names)/sizeof(CSentryE),
                                  sa_names, CS_STAT_AFFECTS);


const char* DARK_MSG = "It is too dark to see.\n";
const char* SLEEP_MSG = "Dream on, my friend.\n";
const char* BLIND_MSG = "You are blind as a bat, and your ears are small.\n";
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
            Web Page:  http://scry.WANfear.com
               Email:  greear@cyberhighway.net

       Thanks to the kind folks at WANfear for hosting the ScryMUD site.
       WANfear offers:
          Virtual-Domain Web Site Hosting: (http://www.your-company.com/),
          CVS repositories, and other hosting and consulting services.
          Check them out at: http://www.WANfear.com
";
//Opening_Screen


///****************  hardcoded global lists ****************///

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

   Enter 'help' or (0) for descriptions of the classes.

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

	Your choice: ",
      
      /* case 6 */
      "
Choose your preferred language:
Lengua:

        English         (0)
        Espanol         (1)
        Portugues       (2)
        Italian         (3)

        Your choice:
        Selection: "

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

[2] is_flying, [4] can_climb, [5] gos, [6] yell, [7] gratz, [8] auc,
[9] shout, [10] say, [11] tell, [12] wiznet, [16] is_sneak, 
[18] can_dive, [21] is_hide
",

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
1 scavenge, 2 wander, 5 edible_corpse, 6 is_banker, 7 is_sessile,
8 !homing 16 has_skin. \n",

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
"NULL, error if you see this!!",

/* 56 proc_data:  say_proc (discuss) */
"NULL, error if you see this!!",

/* 57 proc_data:  bow_proc */
"NULL, error if you see this!!",

/* 58 proc_data:  curse_proc:  message */
"NULL, error if you see this!!",

/* 59 spec_data:  say proc, response */
"NULL, error if you see this!!",

/* 60 proc_data:  give_proc correct_msg */
"NULL, error if you see this!!",

/* 61 proc_data:  bow_proc skill_name */
"NULL, error if you see this!!",

/* 62 proc_data:  curse_proc skill_name */
"NULL, error if you see this!!",

/* 63 proc_data:  say_proc skill_name */
"NULL, error if you see this!!",

/* 64 proc_data:  give_proc skill_name */
"NULL, error if you see this!!",

/* 65 proc_data:  bow_proc, obj_num */
"NULL, error if you see this!!",

/* 66 proc_data:  curse_proc, obj_num */
"NULL, error if you see this!!",

/* 67 proc_data:  say_proc, obj_num */
"NULL, error if you see this!!",

/* 68 proc_data:  give_proc, obj_num */
"NULL, error if you see this!!",

/* 69 proc_data:  bow_proc, trans_to_room */
"NULL, error if you see this!!",

/* 70 proc_data:  curse_proc, trans_to_room */
"NULL, error if you see this!!",

/* 71 proc_data: say_proc, trans_to_room */
"NULL, error if you see this!!",

/* 72 proc_data: give_proc, trans_to_room */
"NULL, error if you see this!!",

/* 73 proc_data:  say_proc, prompt for another? */
"NULL, error if you see this!!",

/* 74 proc_data:  give_proc, wrong_gift_msg */
"NULL, error if you see this!!",

/* 75 proc_data: wrong_align_msg  */
"NULL, error if you see this!!",

/* 76 proc_data: wrong_class_msg  */
"NULL, error if you see this!!",

/* 77 proc_data: wrong_race_msg  */
"NULL, error if you see this!!",

/* 78 obj_spec_proc:  skin_num  */
"\nPlease enter the number of the object that is to be the skin.\n\n",

/* 79 casts_these_spells */
"\nNow enter the level of the spell it will cast, and the spell number it
will cast.  You can find the spell_numbers via the 'slist' command.
A negative one (-1) terminates.  (LEVEL SPELL_NUM) pairs, level first.

Example: 30 10 -1  /* This would make it cast a level 30 'web' spell. */\n"


}; //olc prompts
