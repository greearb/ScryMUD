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

///************ this is to save repetitive typing ***********///
#ifndef BEN_CONST_INCLUDE
#define BEN_CONST_INCLUDE

#include <list2.h>
#include <rb_tree.h>
#include <bitfield.h>
#include <string2.h>
#include <LogStream.h>
#include <PtrArray.h>

enum entity_type {
   ENTITY_CRITTER,
   ENTITY_ROOM,
   ENTITY_DOOR,
   ENTITY_OBJECT
};// entity_type

/** Used in affect_crit_stat, script.h */
enum StatTypeE {
   STAT_HP,
   STAT_MANA,
   STAT_MOV
};

enum ComparisonTypeE {
   CT_EQUALS,
   CT_GT,
   CT_LT
};

enum BugTypeE {
   BT_IDEAS,
   BT_BUGS
};

extern const BitfieldNames MOB_DATA_FLAGS_NAMES;
extern const BitfieldNames MOB_PROC_DATA_FLAGS_NAMES;
extern const BitfieldNames SHOP_DATA_FLAGS_NAMES;
extern const BitfieldNames TEACH_DATA_FLAGS_NAMES;
extern const BitfieldNames PC_DATA_FLAGS_NAMES;
extern const BitfieldNames CRIT_FLAGS_NAMES;
extern const BitfieldNames DOOR_DATA_FLAGS_NAMES;
extern const BitfieldNames VEHICLE_FLAGS_NAMES;
extern const BitfieldNames ROOM_FLAGS_NAMES;
extern const BitfieldNames OBJ_FLAGS_NAMES;
extern const BitfieldNames OBJ_SPEC_DATA_FLAGS_NAMES;
extern const BitfieldNames BAG_FLAGS_NAMES;

extern const BitfieldNames STAT_AFFECT_NAMES;

extern LogStream mudlog;

extern bitfield Obj_Wear_Flags_Mask;
extern bitfield Obj_Consume_Procs_Mask;
extern bitfield Obj_Wear_Procs_Mask;
extern bitfield Obj_Remove_Procs_Mask;


extern const char* Opening_Screen;
extern char* olc_prompts[];
extern char* login_prompts[];

extern int do_shutdown;
extern int Cur_Max_Obj_Num;
extern int Cur_Max_Door_Num;
extern int Cur_Max_Room_Num;
extern int Cur_Max_Crit_Num;
extern int grr_reboot;
extern int Year;
extern int Day;
extern int Hour;
extern int BOOT_TIME; //time in seconds that it booted.
extern int DFLT_PORT;
extern int MAX_DESCRIPTORS_AVAILABLE;
extern int MAX_PLAYERS;
extern int Boot_Load_Modifier;
extern int Load_Modifier;  //initialized in main.cc
extern int Regular_Load_Modifier;
extern int OPT_USEC;   
extern int First_Room;	/* used in pulsed spec procs */
extern int Last_Room;        /* '' */

extern int GUARD_SKILL_NUM;
extern int SHIELD_SKILL_NUM;
extern int SECOND_ATTACK_SKILL_NUM; 
extern int BLOCK_SKILL_NUM;
extern int DUAL_WIELD_SKILL_NUM;
extern int DODGE_SKILL_NUM;
extern int PARRY_SKILL_NUM;
extern int CRITICAL_STRIKE_SKILL_NUM;
extern int ENHANCED_DAMAGE_SKILL_NUM;
extern int FIRE_BLADE_SKILL_NUM;
extern int FROST_BLADE_SKILL_NUM;
extern int BLINDNESS_SKILL_NUM;
extern int ABSORB_BLOWS_SKILL_NUM;
extern
int
   ABSORB_BLOWS_SKILL_NUM,
   ACROBATICS_SKILL_NUM,
   ANCIENT_LANGUAGES_SKILL_NUM,
   ALCHEMY_SKILL_NUM,
   ARCHERY_SKILL_NUM,
   ARMOR_SKILL_NUM,
   BACKSTAB_SKILL_NUM,
   BALISTICS_SKILL_NUM,
   BASH_DOOR_SKILL_NUM,
   BASH_SKILL_NUM,
   BERZERK_SKILL_NUM,
   BIND_WOUND_SKILL_NUM,
   BLACKSMITHING_SKILL_NUM,
   BLEND_SKILL_NUM,
   BLESS_SKILL_NUM,
   BLINDNESS_SKILL_NUM,
   BLOCK_SKILL_NUM,
   BLOCK_DOOR_SKILL_NUM,
   BODYSLAM_SKILL_NUM,
   BOW_SKILL_NUM,
   BRAWLING_SKILL_NUM,
   BREW_SKILL_NUM,
   BURNING_HANDS_SKILL_NUM,
   BUTCHER_SKILL_NUM,
   CAMPING_SKILL_NUM,
   CALM_SKILL_NUM,
   CAMOUFLAGE_SKILL_NUM,
   CAUSE_CRITICAL_SKILL_NUM,
   CAUSE_SICKNESS_SKILL_NUM,
   CHARM_SKILL_NUM,
   CHANNELLING_SKILL_NUM,
   CIRCLE_SKILL_NUM,
   CLAW_SKILL_NUM,
   CLIMBING_SKILL_NUM,
   COMMERCE_SKILL_NUM,
   CONSTRUCT_SKILL_NUM,
   CONJURING_SKILL_NUM,
   CONJURE_HORDE_SKILL_NUM,
   CONJURE_MINION_SKILL_NUM,
   CREATE_GOLEM_SKILL_NUM,
   CREATE_LIGHT_SKILL_NUM,
   CREATE_FOOD_SKILL_NUM,
   CREATE_WATER_SKILL_NUM,
   CREATION_SKILL_NUM,
   CRITICAL_STRIKE_SKILL_NUM,
   CURE_BLINDNESS_SKILL_NUM,
   CURE_CRITICAL_SKILL_NUM,
   CURE_SERIOUS_SKILL_NUM,
   CURE_SKILL_NUM,
   CURSE_SKILL_NUM,
   DAGGER_SKILL_NUM,
   DETECTION_SKILL_NUM,
   DETECT_ALIGNMENT_SKILL_NUM,
   DETECT_HIDDEN_SKILL_NUM, 
   DETECT_INVISIBILITY_SKILL_NUM,
   DETECT_MAGIC_SKILL_NUM,
   DETECT_POISON_SKILL_NUM,
   DISARM_SKILL_NUM,
   DISTORTION_WALL_SKILL_NUM,
   DISPEL_MAGIC_SKILL_NUM,
   DISPEL_GOOD_SKILL_NUM,
   DISPEL_EVIL_SKILL_NUM,
   DIVINE_PROTECTION_SKILL_NUM,
   DODGE_SKILL_NUM,
   DOD_SKILL_NUM,
   DOOR_BASH_SKILL_NUM,
   DUAL_WIELD_SKILL_NUM,
   EARTHMELD_SKILL_NUM,
   ELEMENTALISM_SKILL_NUM,
   ENCHANTMENT_SKILL_NUM,
   ENCHANT_ARMOR_SKILL_NUM,
   ENCHANT_WEAPON_SKILL_NUM,
   ENHANCED_DAMAGE_SKILL_NUM,
   ENTANGLE_SKILL_NUM,
   FAERIE_FIRE_SKILL_NUM,
   FENCING_SKILL_NUM,
   FIRE_BLADE_SKILL_NUM,
   FIREBALL_SKILL_NUM,
   FIREPROOF_SKILL_NUM,
   FIRESTORM_SKILL_NUM,
   FIREWALL_SKILL_NUM,
   FLAME_STRIKE_SKILL_NUM,
   FLY_SKILL_NUM,
   FORESTRY_SKILL_NUM,
   FROST_BLADE_SKILL_NUM,
   GATE_SKILL_NUM,
   GROUP_HEAL_SKILL_NUM,
   GUARD_SKILL_NUM,
   HARM_SKILL_NUM,
   HARMING_SKILL_NUM,
   HASTE_SKILL_NUM,
   HEAL_SKILL_NUM,
   HEALING_SKILL_NUM,
   HERBALISM_SKILL_NUM,
   HEROS_FEAST_SKILL_NUM,
   HIDE_SKILL_NUM,
   HONOR_CODE_SKILL_NUM,
   HOLY_WORD_SKILL_NUM,
   HURL_SKILL_NUM,
   ICESTORM_SKILL_NUM,
   IDENTIFY_SKILL_NUM,
   ILLUMINATE_SKILL_NUM,
   ILLUSION_SKILL_NUM,
   INVISIBILITY_SKILL_NUM,
   KICK_SKILL_NUM,
   LEADERSHIP_SKILL_NUM,
   LIGHTNING_SKILL_NUM,
   LIGHTNING_STORM_SKILL_NUM,
   LITERACY_SKILL_NUM,
   LOCATE_SKILL_NUM,
   LOGIC_SKILL_NUM,
   LORE_SKILL_NUM,
   MACE_SKILL_NUM,
   MAGIC_SHIELD_SKILL_NUM,
   MANA_SKILL_NUM,
   MARTIAL_ARTS_SKILL_NUM,
   MASS_CHARM_SKILL_NUM,
   MEDITATION_SKILL_NUM,
   METEORSTORM_SKILL_NUM,
   MIRROR_IMAGE_SKILL_NUM,
   NECROMANCY_SKILL_NUM,
   PARRY_SKILL_NUM,
   PASSDOOR_SKILL_NUM,
   PHYSICAL_ARTS_SKILL_NUM,
   PHYSIK_SKILL_NUM,
   PFE_SKILL_NUM,
   PFG_SKILL_NUM,
   PHILOSOPHY_SKILL_NUM,
   PICKLOCK_SKILL_NUM,
   PLAGUE_SKILL_NUM,
   POISON_SKILL_NUM,
   PORTAL_SKILL_NUM,
   PRISMATIC_GLOBE_SKILL_NUM,
   PROTECTION_SKILL_NUM,
   QUAKE_SKILL_NUM,
   QUICKFOOT_SKILL_NUM,
   RAISE_UNDEAD_SKILL_NUM,
   RAINBOW_SKILL_NUM,
   RECALL_SKILL_NUM,
   RECHARGE_SKILL_NUM,
   RELIGION_SKILL_NUM,
   REMOVE_POISON_SKILL_NUM,
   REMOVE_CURSE_SKILL_NUM,
   RESCUE_SKILL_NUM,
   RESTORE_SKILL_NUM,
   RUNE_EDGE_SKILL_NUM,
   SANCTUARY_SKILL_NUM,
   SCAN_SKILL_NUM,
   SCRIBE_SKILL_NUM,
   SCROLLS_SKILL_NUM,
   SCRYING_SKILL_NUM,
   SECOND_ATTACK_SKILL_NUM, 
   SHADOWS_BLESSING_SKILL_NUM,
   SHIELcD_SKILL_NUM,
   SHOCKING_GRASP_SKILL_NUM,
   SKIN_SKILL_NUM,
   SLEEP_SKILL_NUM,
   SNEAK_SKILL_NUM,
   STEAL_SKILL_NUM,
   STRENGTH_SKILL_NUM,
   STRENGTH_CONDITIONING_SKILL_NUM,
   STONE_SKIN_SKILL_NUM,
   SUMMON_SKILL_NUM,
   SWORD_SKILL_NUM,
   SWORDBOND_SKILL_NUM,
   TAIL_SKILL_NUM,
   TELEPORT_SKILL_NUM,
   THROWING_SKILL_NUM,
   TORNADO_SKILL_NUM,
   TRACK_SKILL_NUM,
   TRIP_SKILL_NUM,
   TYPHOON_SKILL_NUM,
   WEAKEN_SKILL_NUM,
   WEAPON_MASTERY_SKILL_NUM,
   WEB_SKILL_NUM,
   WHIP_SKILL_NUM,
   WIZARD_EYE_SKILL_NUM,
   WRESTLING_SKILL_NUM;


// Ripped off from some PennMUSH code someone posted to the
// mud-dev list.

/* ansi.h */

/* ANSI control codes for various neat-o terminal effects
 * Some older versions of Ultrix don't appear to be able to
 * handle these escape sequences. If lowercase 'a's are being
 * stripped from @doings, and/or the output of the ANSI flag
 * is screwed up, you have the Ultrix problem.
 *
 * To fix the ANSI problem, try replacing the '\x1B' with '\033'.
 * To fix the problem with 'a's, replace all occurrences of '\a'
 * in the code with '\07'.
 *
 */


//#define ANSI_ECHO_ON  "\n\x1B\x4D\x31\x32\x6C"
#define ANSI_ECHO_ON    ""


#ifndef OLD_ANSI

#define BEEP_CHAR     '\a'
#define ESC_CHAR      '\x1B'

#define ANSI_NORMAL   "\x1B[0m"

#define ANSI_HILITE   "\x1B[1m"
#define ANSI_INVERSE  "\x1B[7m"
#define ANSI_BLINK    "\x1B[5m"
#define ANSI_UNDERSCORE "\x1B[4m"

#define ANSI_INV_BLINK         "\x1B[7;5m"
#define ANSI_INV_HILITE        "\x1B[1;7m"
#define ANSI_BLINK_HILITE      "\x1B[1;5m"
#define ANSI_INV_BLINK_HILITE  "\x1B[1;5;7m"

/* Foreground colors */

#define ANSI_BLACK	"\x1B[30m"
#define ANSI_RED	"\x1B[31m"
#define ANSI_GREEN	"\x1B[32m"
#define ANSI_YELLOW	"\x1B[33m"
#define ANSI_BLUE	"\x1B[34m"
#define ANSI_MAGENTA	"\x1B[35m"
#define ANSI_CYAN	"\x1B[36m"
#define ANSI_WHITE	"\x1B[37m"

/* Background colors */

#define ANSI_BBLACK	"\x1B[40m"
#define ANSI_BRED	"\x1B[41m"
#define ANSI_BGREEN	"\x1B[42m"
#define ANSI_BYELLOW	"\x1B[43m"
#define ANSI_BBLUE	"\x1B[44m"
#define ANSI_BMAGENTA	"\x1B[45m"
#define ANSI_BCYAN	"\x1B[46m"
#define ANSI_BWHITE	"\x1B[47m"

#else

#define BEEP_CHAR     '\07'
#define ESC_CHAR      '\033'

#define ANSI_NORMAL   "\033[0m"


#define ANSI_HILITE   "\033[1m"
#define ANSI_INVERSE  "\033[7m"
#define ANSI_BLINK    "\033[5m"
#define ANSI_UNDERSCORE "\033[4m"

#define ANSI_INV_BLINK         "\033[7;5m"
#define ANSI_INV_HILITE        "\033[1;7m"
#define ANSI_BLINK_HILITE      "\033[1;5m"
#define ANSI_INV_BLINK_HILITE  "\033[1;5;7m"

/* Foreground colors */

#define ANSI_BLACK	"\033[30m"
#define ANSI_RED	"\033[31m"
#define ANSI_GREEN	"\033[32m"
#define ANSI_YELLOW	"\033[33m"
#define ANSI_BLUE	"\033[34m"
#define ANSI_MAGENTA	"\033[35m"
#define ANSI_CYAN	"\033[36m"
#define ANSI_WHITE	"\033[37m"

/* Background colors */

#define ANSI_BBLACK	"\033[40m"
#define ANSI_BRED	"\033[41m"
#define ANSI_BGREEN	"\033[42m"
#define ANSI_BYELLOW	"\033[43m"
#define ANSI_BBLUE	"\033[44m"
#define ANSI_BMAGENTA	"\033[45m"
#define ANSI_BCYAN	"\033[46m"
#define ANSI_BWHITE	"\033[47m"

#endif
// End of penn-mush ansi color stuff...

extern const String ANSI_BLACK_S;
extern const String ANSI_RED_S;
extern const String ANSI_GREEN_S;
extern const String ANSI_YELLOW_S;
extern const String ANSI_BLUE_S;
extern const String ANSI_MAGENTA_S;
extern const String ANSI_CYAN_S;
extern const String ANSI_WHITE_S;

extern const String ANSI_BBLACK_S;
extern const String ANSI_BRED_S;
extern const String ANSI_BGREEN_S;
extern const String ANSI_BYELLOW_S;
extern const String ANSI_BBLUE_S;
extern const String ANSI_BMAGENTA_S;
extern const String ANSI_BCYAN_S;
extern const String ANSI_BWHITE_S;


#define NUMBER_OF_ZONES                 80
#define NUMBER_OF_ROOMS                 6000
#define NUMBER_OF_MOBS                  800
#define NUMBER_OF_ITEMS                 800
#define NUMBER_OF_DOORS                 200

#define HUNGER_MAX                      100
#define THIRST_MAX                      75
#define DRUGGED_MAX                     24

#define OBJ_MAX_EXTRAS 			9
#define OBJ_CUR_STATS			4
#define MOB_LONG_DATA			3
#define MOB_SHORT_CUR_STATS		43
#define MOB_CUR_STATS			3
#define ROOM_CUR_STATS			4

// Flags sent to the do_emote method.
#define EMOTE_POSSESSIVE         1
#define EMOTE_NON_POSSESSIVE     2


#define IMM_LEVEL	getImmLevel()
#define HUNTING		mob->proc_data->temp_proc->hunting
#define TRACKING       	mob->proc_data->temp_proc->tracking
#define SNOOPED_BY	pc->snoop_by
#define SNOOPING	pc->snooping
#define MODE		getMode()
#define BIRTH_DAY	pc->birth_day
#define BIRTH_MONTH	pc->birth_month
#define BIRTH_YEAR	pc->birth_year
#define RENT_DAY	pc->rent_day
#define RENT_YEAR	pc->rent_year

#define WRONG_ALIGN_MSG mob->proc_data->wrong_align_msg
#define WRONG_RACE_MSG  mob->proc_data->wrong_race_msg
#define WRONG_CLASS_MSG mob->proc_data->wrong_class_msg

#define GIVE_PROC       mob->proc_data->give_proc
#define DOMOB_GIVE_NUM  mob->proc_data->give_proc->test_num
#define GIVE_RIGHT_MSG  mob->proc_data->give_proc->correct_msg
#define GIVE_RIGHT_ITEM mob->proc_data->give_proc->obj_num
#define GIVE_TEACH      mob->proc_data->give_proc->skill_name
#define GIVE_WRONG_MSG  mob->proc_data->give_proc->wrong_gift_msg
#define GIVE_TRANSPORT_ROOM  	mob->proc_data->give_proc->trans_to_room


#define BOW_PROC                 mob->proc_data->bow_proc
#define BOW_MSG  		 mob->proc_data->bow_proc->correct_msg
#define BOW_ITEM 		 mob->proc_data->bow_proc->obj_num
#define BOW_TEACH      		 mob->proc_data->bow_proc->skill_name
#define BOW_TRANSPORT_ROOM  	 mob->proc_data->bow_proc->trans_to_room


#define CURSE_PROC               mob->proc_data->curse_proc
#define CURSE_MSG  		 mob->proc_data->curse_proc->correct_msg
#define CURSE_ITEM 		 mob->proc_data->curse_proc->obj_num
#define CURSE_TEACH		 mob->proc_data->curse_proc->skill_name
#define CURSE_TRANSPORT_ROOM  	 mob->proc_data->curse_proc->trans_to_room

#define TOPICS		 mob->proc_data->topics
#define BUY_PERCENTAGE   mob->proc_data->sh_data->buy_percentage
#define OPEN_TIME	 mob->proc_data->sh_data->open_time
#define CLOSE_TIME	 mob->proc_data->sh_data->close_time
#define MARKUP		 mob->proc_data->sh_data->markup
#define PC_FLAGS	 pc->pc_data_flags
#define GUARDING         temp_crit->guarding
#define SHIELDING        temp_crit->shielding
#define SHIELDED_BY      temp_crit->shielded_by
#define GUARDED_BY       temp_crit->guarded_by
#define MOB_FLAGS        mob->mob_data_flags
#define IS_FIGHTING	 is_fighting
#define IS_BRIEF	 isBrief()
#define USING_CLIENT	 isUsingClient()
#define IS_SAY		 crit_flags.get(11)
#define IS_YELL		 crit_flags.get(7)
#define IS_GOSSIP	 crit_flags.get(6)
#define IS_SHOUT	 crit_flags.get(10)
#define IS_AUCTION	 crit_flags.get(9)
#define CRIT_FLAGS	 crit_flags
#define GROUPEES	 groupees
#define PETS		 pets
#define FOLLOWER_OF	 follower_of
#define MASTER		 master
#define EQ		 eq
#define SKILLS_KNOWN	 pc->skills_spells_known
#define FOLLOWERS	 followers
#define FLAG1		 mob->proc_data->flag1
#define INT1		 mob->proc_data->int1
#define SHOP_DATA_FLAGS	 mob->proc_data->sh_data->shop_data_flags
#define TEACH_DATA_FLAGS \
         mob->proc_data->teach_data->teach_data_flags
#define MARKUP		 mob->proc_data->sh_data->markup
#define PERM_INV	 mob->proc_data->sh_data->perm_inv
#define MOB_NUM		getIdNum()
#define MINI_AFFECTED_BY	 mini_affected_by
#define USING_LIGHT_SOURCE	1
#define DO_PROMPT		10 //in pc_data
#define PROMPT_STRING		 pc->prompt
#define PC_IS_BUILDER		11 // ""
#define BH_DICE_COUNT	 short_cur_stats[35]
#define BH_DICE_SIDES	 short_cur_stats[36]
#define PK_COUNT	 pc->pk_count
#define DIED_COUNT	 pc->died_count
#define QUEST_POINTS	 pc->quest_points


#define MOB_GOSSIP	6
#define MOB_YELL	7
#define MOB_GRATZ	8
#define MOB_AUC		9
#define MOB_SHOUT	10
#define MOB_SAY		11
#define MOB_TELL	12
#define MOB_WIZNET	13


#define BAG_FLAGS       bag->bag_flags
#define OBJ_PRCNT_LOAD	extras[2]
//#define OBJ_CUR_IN_GAME extras[3]
#define OBJ_MAX_IN_GAME extras[4]
#define OBJ_IN_ZONE	cur_stats[3]
#define CASTS_THESE_SPELLS   obj_proc->casts_these_spells
#define COMPONENT_ITEM1 obj_proc->construct_data->item1
#define COMPONENT_ITEM2 obj_proc->construct_data->item2
#define COMPONENT_ITEM3 obj_proc->construct_data->item3
#define COMPONENT_ITEM4 obj_proc->construct_data->item4
#define COMPONENT_ITEM5 obj_proc->construct_data->item5
#define COMPONENT_TARG  obj_proc->construct_data->target_object
#define OBJ_PRICE	cur_stats[1]
#define OBJ_FLAGS	obj_flags
#define OBJ_SPEC_FLAGS	obj_proc->obj_spec_data_flags
#define OBJ_VIS_BIT	cur_stats[0]
#define IN_LIST		in_list
#define OBJ_CUR_WEIGHT	getCurWeight()
#define OBJ_MAX_WEIGHT	getMaxWeight()
#define OBJ_NUM		cur_stats[2]
#define CHARGES			extras[0]
#define RECHARGE       		extras[1]
#define OBJ_BASE_WEIGHT		extras[5]
#define OBJ_DAM_DICE_SIDES	extras[6]
#define OBJ_DAM_DICE_COUNT	extras[7]
#define OBJ_LEVEL		extras[8]
#define OBJ_CONTAINER		54
#define OBJ_LIGHT_SOURCE	58
#define PRICE			cur_stats[1]


#define RM_IS_HAVEN	7
#define RM_IS_NONPK	8
#define RM_IS_NOMAG	9
#define CRITTERS	critters
#define DOORS		doors

#define DOOR_VIS_BIT	dr_data->vis_bit
#define VIS_DARK	1 //regarding vis_bits in general

#define O_COUNT		pc.pc->imm_data->olc_counter
#define TEMP_OLC_INT	pc.pc->imm_data->temp_olc_int


extern int do_shutdown;   //in grrmud.cc
extern int grr_reboot;   //in grrmud.cc
extern int CONVERT_WORLD_FROM_DEV; //in grrmud.cc

extern List<String*>   banned_hosts;
extern class critter   mob_list[NUMBER_OF_MOBS + 1];
extern class object    obj_list[NUMBER_OF_ITEMS + 1];
extern class door_data door_list[NUMBER_OF_DOORS +1];

extern List<class door*>	affected_doors; 
extern List<critter*>   affected_mobs; /* list those affected*/ 
extern List<struct object*>    affected_objects; /* ditto for objs */
extern List<critter*>   pc_list;      //head of pc list  
extern List<critter*>   linkdead_list;  //holds link dead folks
extern PtrArray<critter>   proc_action_mobs; //holds mob-proc'ing mobs


extern const char* CensoredStrings[];


extern const char* DARK_MSG;
extern const char* SLEEP_MSG;
extern const char* BLIND_MSG;
extern const char* NOT_IN_HAVEN_MSG;
extern const char* NOT_IN_NOMAG_RM_MSG;
extern const char* SPELL_MUST_BE_STANDING_MSG;
extern const char* NO_PK_HERE_MSG;
extern const char* NO_MANA_MSG;
extern const char* PARSE_ERR_MSG;
extern const char* NO_KNOW_SPELL_MSG;
extern const char* LOST_CONCENTRATION_MSG_SELF;
extern const char* LOST_CONCENTRATION_MSG_OTHER;
extern const char* SPELL_ONLY_ON_SELF_MSG;
extern const char* HUH_MSG;

extern int EXP_DIVISOR;   // critter.exp/exp_divisor =>exp gained per kill

// Used in buy, sell, list and offer commands/procs.
#define OUT_OF_TOWN_MODIFIER        1.25
#define COMMERCE_SKILL_EFFECT_BUY   0.86
#define COMMERCE_SKILL_EFFECT_SELL  1.05


//  RACES

#define OTHER_RACE		0
#define HUMAN			1
#define ANITRE			2
#define AVINTRE			3
#define DARKLING		4
#define DROW			5
#define DRAGON			6
#define DWARF			7
#define IRONCLAD		8
#define OGRUE			9
#define ROCKTROLL		10
#define ELF			11
#define FAERIE			12
#define ENTITY			13
#define SOMBRIAN                14
#define UNDEAD			15
#define ANIMAL			16
#define MONSTER			17

// PLAYER CLASSES

#define OTHER_CLASS		0
#define WARRIOR			1
#define SAGE			2
#define WIZARD			3
#define RANGER			4
#define THIEF			5
#define ALCHEMIST		6
#define CLERIC			7
#define BARD		        8

#define SEX_FEMALE              0
#define SEX_MALE                1
#define SEX_NEUTER              2


#define DOOR_NORTH              3
#define DOOR_SOUTH              4
#define DOOR_EAST               1
#define DOOR_WEST               2
#define DOOR_NORTHWEST          7
#define DOOR_NORTHEAST          6
#define DOOR_SOUTHEAST          5
#define DOOR_SOUTHWEST          8
#define DOOR_UP                 9
#define DOOR_DOWN               10


/******************* Types of Spells/Damages, for armor purposes ************/

#define FIRE            201
#define D_BREATH        202
#define ICE             203
#define ELECTRICITY     204
#define CRONIC          205		//poison,
#define NORMAL          206               //good ole hack/slash
#define SUMMON          207
#define CHARM           208
#define COERCION        209
#define AGILITY         210


#define HUNGER_ID  	100 			//these are for identification
#define THIRST_ID  	101			//of stat_affects
#define DRUGGED_ID	102

#define CON_PLAYING     1
#define CON_LINKDEAD    2
#define CON_LOGGING_IN	3

#define TRUE            1       
#define FALSE           0


#define RECALL_ROOM     1    /* may make multiple ones one day */
#define NEWBIE_ROOM     6
#define LOGIN_ROOM      1
#define DONATE_ROOM     7    /* may make multiple ones some day.. */
#define NEWBIE_BAG_NUM  358  /* Give to newbies when they enter the game. */

#define CORPSE_OBJECT   3    /* obj number of a corpse object bag thingie */
#define GOLD_OBJECT     5    /* ditto for gold */
#define PC_SKIN_OBJECT  8    /* ditto for the generic pc_skin object */
#define MEAT_OBJ_NUMBER 97             /* ditto */
#define EARTH_GOLEM_NUMBER   37
#define MINION_ONE_NUMBER    38        /* classy chick demon */
#define MINION_TWO_NUMBER    39        /* dwarven blacksmith */
#define MINION_THREE_NUMBER  41        /* fallen wizard      */
#define MINION_FOUR_NUMBER   42        /* thiefy woman       */
#define RAISED_CORPSE_NUM    57
#define ILLUSION_NUMBER      55
#define GATE_DOOR_NUM        44        /* gate door for gate spell */
#define CREATE_FOOD_NUMBER   94
#define CREATE_FOOD2_NUMBER  68
#define CREATE_FOOD3_NUMBER  13
#define CREATE_FOOD4_NUMBER  103
#define CREATE_LIGHT_NUMBER  91

#define MAX_INPUT_LEN   5000  //can input this many chars at one time
#define OUTPUT_MAX_LEN  25000  //can hold this many chars in output buffer
#define DFLT_DIR        .
#define MAX_EQ          19

#define ROOM_INV 0
#define OBJ_INV  1
#define CRIT_INV 2

#define ROOM             room_list[pc.getCurRoomNum()]
#define GOLD    	 long_data[0]
#define EXP     	 long_data[1]
#define BANK_GOLD    	 long_data[2]
#define AGE     	 pc->age
#define SEE_BIT 	 cur_stats[1]
#define VIS_BIT 	 cur_stats[0]
#define POS     	 getPosn()
#define STR     	 short_cur_stats[1]
#define INT     	 short_cur_stats[2]
#define CON     	 short_cur_stats[3]
#define CHA     	 short_cur_stats[4]
#define WIS     	 short_cur_stats[5]
#define DEX     	 short_cur_stats[6]
#define HIT     	 short_cur_stats[7]
#define DAM     	 short_cur_stats[8]
#define AC      	 short_cur_stats[9]
#define ATTACKS 	 short_cur_stats[10]
#define SEX     	 short_cur_stats[12]
#define CLASS   	 short_cur_stats[13]
#define RACE    	 short_cur_stats[14]
#define HP      	 short_cur_stats[15]
#define MANA    	 short_cur_stats[16]
#define MOV     	 short_cur_stats[17]
#define ALIGN   	 short_cur_stats[18]
#define LEVEL   	 short_cur_stats[19]
//#define MISC  		 short_cur_stats[20] //is hometown now
#define IN_ROOM 	 cur_stats[2]
#define WIMPY            short_cur_stats[21]
#define PRACS   	 short_cur_stats[22]
#define HP_MAX  	 getHP_MAX()
#define MA_MAX  	 short_cur_stats[24]
#define MV_MAX 		 short_cur_stats[25]
#define CRITTER_TYPE   	 short_cur_stats[26] /* was SMOB */
#define DAM_REC_MOD    	 short_cur_stats[27]
#define DAM_GIV_MOD    	 short_cur_stats[28]
#define HUNGER  	 pc->hunger
#define THIRST  	 pc->thirst
//#define CUR_IN_GAME  	 mob->cur_in_game
#define MAX_IN_GAME  	 mob->max_in_game
#define DRUGGED 	 pc->drugged
#define CRIT_WT_CARRIED  	 getCurWeight()
#define CRIT_MAX_WT_CARRY    	 getMaxWeight()
#define PAUSE   	 short_cur_stats[11]
#define HEAT_RESIS 	 short_cur_stats[29]
#define COLD_RESIS 	 short_cur_stats[30]
#define ELEC_RESIS 	 short_cur_stats[31]
#define SPEL_RESIS 	 short_cur_stats[32]
#define RELIGION	 short_cur_stats[33]
//#define MOB_FROM_ZONE 	 short_cur_stats[34]
#define HP_REGEN   	 short_cur_stats[37] 
#define MA_REGEN   	 short_cur_stats[38] 
#define MV_REGEN   	 short_cur_stats[39] 
#define GUILD		 short_cur_stats[40]


//   SPEC_PROCS

#define BATTLE_SPEC_PROC    	1
#define JUST_KILLED_PROC    	2
#define OBJ_WEAR_PROC    	3 
#define OBJ_REMOVE_PROC  	4
#define OBJ_CONSUME_PROC	5
#define JK_SPEC_PROC		6 


extern const char* DONT_SEE_PERSON;
extern const char* NOT_IN_POSN;


extern String UNKNOWN_DESC;
extern String SOMEONE;
extern String SOMETHING;
extern String SOMEWHERE;
extern String UNKNOWN;
extern String NULL_STRING;
extern String NORTH;
extern String SOUTH;
extern String WEST;
extern String EAST;
extern String NORTHWEST;
extern String NORTHEAST;
extern String SOUTHWEST;
extern String SOUTHEAST;
extern String UP;
extern String DOWN;

extern char* critter_positions[10];

extern char* critter_conditions[11];

#define MAX_LEVEL    45
extern long levels[MAX_LEVEL];

#endif
