// $Id$
// $Revision$  $Author$ $Date$

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

#ifdef USEMYSQL
#include <mysql/mysql.h>
#endif
#include <list2.h>
#include <rb_tree.h>
#include <bitfield.h>
#include <string2.h>
#include <LogStream.h>
#include <PtrArray.h>
#include "ServerConfig.h"

enum proc_types {
   PROCTYPE_GIVE,
   PROCTYPE_BOW,
   PROCTYPE_CURSE,
   PROCTYPE_SAY
};

enum shop_data_flags {
   SHOPFLAG_BUY0,
   SHOPFLAG_SELL0,
   SHOPFLAG_OFFER0,
   SHOPFLAG_PLAYER_OWNED,
   SHOPFLAG_NULL1, // 5
   SHOPFLAG_NULL2, // 6
   SHOPFLAG_NULL3, // 7
   SHOPFLAG_NULL4, // 8
   SHOPFLAG_NULL5, // 9
   SHOPFLAG_NULL6, // 10
   SHOPFLAG_NULL7, // 11
   SHOPFLAG_NULL8, // 12
   SHOPFLAG_NULL9, // 13
   SHOPFLAG_NULL10, // 14
   SHOPFLAG_NULL11, // 15
   SHOPFLAG_NULL12, // 16
   SHOPFLAG_NULL13, // 17
   SHOPFLAG_NULL14, // 18
   SHOPFLAG_NULL15, // 19
   SHOPFLAG_NULL16, // 20
   SHOPFLAG_NULL17, // 21
   SHOPFLAG_NULL18, // 22
   SHOPFLAG_NULL19, // 23
   SHOPFLAG_NULL20, // 24
   SHOPFLAG_NULL21, // 25
   SHOPFLAG_NULL22, // 26
   SHOPFLAG_NULL23, // 27
   SHOPFLAG_NULL24, // 28
   SHOPFLAG_NULL25, // 29
   SHOPFLAG_NULL26, // 30
   SHOPFLAG_NULL27, // 31
   SHOPFLAG_NULL28, // 32
   SHOPFLAG_NULL29, // 33
   SHOPFLAG_NULL30, // 34
   SHOPFLAG_NULL31, // 35
   SHOPFLAG_NULL32, // 36
   SHOPFLAG_NULL33, // 37
   SHOPFLAG_NULL34, // 38
   SHOPFLAG_NULL35, // 39
   SHOPFLAG_NON_WEAPON,
   SHOPFLAG_SLASH,
   SHOPFLAG_SMASH,
   SHOPFLAG_PIERCE,
   SHOPFLAG_WHIP,
   SHOPFLAG_NEEDS_AMMO,
   SHOPFLAG_DART_THROWER,
   SHOPFLAG_BOW,
   SHOPFLAG_DART,
   SHOPFLAG_ARROW,
   SHOPFLAG_JUNK,
   SHOPFLAG_WAND,
   SHOPFLAG_POTION,
   SHOPFLAG_SCROLL,
   SHOPFLAG_CONTAINER,
   SHOPFLAG_COINS,
   SHOPFLAG_ARMOR,
   SHOPFLAG_WEAPON,
   SHOPFLAG_LIGHT_SOURCE,
   SHOPFLAG_CANTEEN,
   SHOPFLAG_LIQUID,
   SHOPFLAG_FOOD,
   SHOPFLAG_BOAT,
   SHOPFLAG_HAS_SPEC_PROC_DATA,
   SHOPFLAG_TOOLBOX,
   SHOPFLAG_CAULDRON,
   SHOPFLAG_PEN,
   SHOPFLAG_CONSTRUCT_COMPONENT,
   SHOPFLAG_CONCOCT_COMPONENT,
   SHOPFLAG_PARCHMENT,
   SHOPFLAG_NULL36,
   SHOPFLAG_NULL37,
   SHOPFLAG_HERB,
   SHOPFLAG_VEND_MACHINE
};

enum crit_flags {
   CRITFLAG_CAN_SEE_INV,
   CRITFLAG_USING_LIGHT,
   CRITFLAG_NULL1,
   CRITFLAG_IS_FLYING,
   CRITFLAG_HAS_BOAT,
   CRITFLAG_CAN_CLIMB,
   CRITFLAG_GOSSIPS,
   CRITFLAG_YELLS,
   CRITFLAG_GRATZ,
   CRITFLAG_AUCTIONS,
   CRITFLAG_SHOUTS,
   CRITFLAG_SAYS,
   CRITFLAG_TELLS,
   CRITFLAG_WIZNETS,
   CRITFLAG_IS_PARALYZED,
   CRITFLAG_IS_PERM_SLEEPED,
   CRITFLAG_IS_DUAL_WIELDING,
   CRITFLAG_IS_SNEAK,
   CRITFLAG_IN_USE,
   CRITFLAG_CAN_DIVE,
   CRITFLAG_SPELL_TESTED_YET,
   CRITFLAG_IS_BLOCKED,
   CRITFLAG_IS_HIDE,
   CRITFLAG_IS_TAILING,
   CRITFLAG_NOT_COMPLETE,
   CRITFLAG_ALREADY_HURLED,
};

enum pc_data_flags {
   PCFLAG_FROZEN,
   PCFLAG_GAGGED,
   PCFLAG_HAS_IMM_DATA,
   PCFLAG_CLOAKED,
   PCFLAG_TANK_GRAPH,
   PCFLAG_USING_CLIENT,
   PCFLAG_AUTOEXIT,
   PCFLAG_NO_HASSLE,
   PCLFAG_BRIEF,
   PCFLAG_AUTOSPLIT,
   PCFLAG_DO_PROMPT,
   PCFLAG_IS_BUILDER,
   PCFLAG_AUTOLOOT,
   PCFLAG_OLC_REDO,
   PCFLAG_EXTRA_INFO,
   PCFLAG_CR_BEHIND,
   PCFLAG_DO_CARRIAGE_RETURN,
   PCFLAG_IS_BLOCKING_DOOR,
   PCFLAG_CAN_DET_MAGIC,
   PCFLAG_DETECT_INVENTORY,
   PCFLAG_SHOW_VNUMS,
   PCFLAG_HAS_POOFIN_POOFOUT_MSG,
   PCFLAG_PAGE_OUTPUT,
   PCFLAG_IN_PAGE_BREAK_MODE,
   PCFLAG_NO_WIZCHAT,
   PCFLAG_HAS_COLORS,
   PCFLAG_USE_COLOR,
   PCFLAG_HAS_LANGUAGE_CHOICE,
   PCFLAG_NO_SHOW_MOB_ENTRY,
   PCFLAG_NO_BEEP,
   PCFLAG_IS_REMORT,
   PCFLAG_HAS_SACRIFICED 
};

enum spec_flag {
   SPECFLAG_NULL1,
   SPECFLAG_SHOPKEEPER,
   SPECFLAG_TEACHER,
   SPECFLAG_LET_SAME_CLASS_PASS,
   SPECFLAG_LET_SAME_RACE_PASS,
   SPECFLAG_HAS_MOB_GIVE_PROC,
   SPECFLAG_HAS_MOB_SAY_PROC,
   SPECFLAG_HAS_MOB_BOW_PROC,
   SPECFLAG_HAS_MOB_CURSE_PROC,
   SPECFLAG_PROC_WITH_SAME_RACE,
   SPECFLAG_PROC_WITH_SAME_ALIGN,
   SPECFLAG_PROC_WITH_SAME_CLASS,
   SPECFLAG_NULL2,
   SPECFLAG_HAS_AI
};

enum mob_data_flag {
   MOBFLAG_HAS_PROC_DATA,
   MOBFLAG_SCAVENGE,
   MOBFLAG_WANDER,
   MOBFLAG_SHOULD_DO_PROCS,
   MOBFLAG_NEED_RESETTING,
   MOBFLAG_EDIBLE_CORPSE,
   MOBFLAG_IS_BANKER,
   MOBFLAG_SESSILE,
   MOBFLAG_NOT_HOMING,
   MOBFLAG_DISOLVABLE,
   MOBFLAG_NULL1,
   MOBFLAG_NULL2,
   MOBFLAG_NULL3,
   MOBFLAG_NULL4,
   MOBFLAG_NULL5,
   MOBFLAG_NULL6,
   MOBFLAG_HAS_SKIN,
   MOBFLAG_HAS_MOB_SCRIPT,
   MOBFLAG_IS_GUARD
};
enum obj_flags {
   OBJFLAG_NO_REST,
   OBJFLAG_ANTI_EVIL,
   OBJFLAG_ANTI_NEUTRAL,
   OBJFLAG_ANTI_GOOD,
   OBJFLAG_ANTI_DONATE,
   OBJFLAG_ANTI_DROP,
   OBJFLAG_ANTI_REMOVE,
   OBJFLAG_ANTI_MORTAL,
   OBJFLAG_ANTI_IMMORTAL,
   OBJFLAG_ANTI_DEMI,
   OBJFLAG_IN_USE,
   OBJFLAG_ANTI_WARRIOR,
   OBJFLAG_ANTI_SAGE,
   OBJFLAG_ANTI_WIZARD,
   OBJFLAG_ANTI_RANGER,
   OBJFLAG_ANTI_THIEF,
   OBJFLAG_ANTI_ALCHEMIST,
   OBJFLAG_ANTI_CLERIC,
   OBJFLAG_ANTI_BARD,
   OBJFLAG_ANTI_MOB,
   OBJFLAG_ANTI_PC,
   OBJFLAG_NOT_WORN,
   OBJFLAG_HEAD,
   OBJFLAG_NECK1,
   OBJFLAG_NECK2,
   OBJFLAG_AROUND_BODY,
   OBJFLAG_ARMS,
   OBJFLAG_WRIST1,
   OBJFLAG_WRIST2,
   OBJFLAG_HANDS,
   OBJFLAG_WIELDED,
   OBJFLAG_HELD,
   OBJFLAG_LIGHT,
   OBJFLAG_BODY,
   OBJFLAG_BELT,
   OBJFLAG_LEGS,
   OBJFLAG_FEET,
   OBJFLAG_FINGER1,
   OBJFLAG_FINGER2,
   OBJFLAG_SHIELD,
   OBJFLAG_NOT_A_WEAPON,
   OBJFLAG_SLASH,
   OBJFLAG_SMASH,
   OBJFLAG_PIERCE,
   OBJFLAG_WHIP,
   OBJFLAG_NEEDS_AMMO,
   OBJFLAG_DART_THROWER,
   OBJFLAG_BOW,
   OBJFLAG_DART,
   OBJFLAG_ARROW,
   OBJFLAG_JUNK,
   OBJFLAG_WAND,
   OBJFLAG_POTION,
   OBJFLAG_SCROLL,
   OBJFLAG_CONTAINER,
   OBJFLAG_COINS,
   OBJFLAG_ARMOR,
   OBJFLAG_WEAPON,
   OBJFLAG_LIGHT_SOURCE,
   OBJFLAG_CANTEEN,
   OBJFLAG_LIQUID,
   OBJFLAG_FOOD,
   OBJFLAG_BOAT,
   OBJFLAG_HAS_SPEC_PROC_DATA,
   OBJFLAG_TOOLBOX,
   OBJFLAG_CAULDRON,
   OBJFLAG_PEN,
   OBJFLAG_CONSTRUCT_COMPONENT,
   OBJFLAG_CONCOCT_COMPONENT,
   OBJFLAG_PARCHMENT,
   OBJFLAG_NEEDS_RESETTING,
   OBJFLAG_NOT_COMPLETE,
   OBJFLAG_HERB,
   OBJFLAG_VENDING_MACHINE,
   OBJFLAG_BULLETIN_BOARD,
   OBJFLAG_IS_BUTCHERABLE,
   OBJFLAG_HAS_OBJ_SCRIPT,
   OBJFLAG_IS_TWO_HANDED,
   OBJFLAG_ANTI_NECROMANCER
};

enum bag_flags {
   BAGFLAG_UNUSED1,
   BAGFLAG_UNUSED2,
   BAGFLAG_IS_CLOSED,
   BAGFLAG_IS_LOCKED,
   BAGFLAG_IS_PICKABLE,
   BAGFLAG_IS_MAG_LOCKABLE,
   BAGFLAG_IS_MAG_LOCKED,
   BAGFLAG_IS_DESTRUCTABLE,
   BAGFLAG_IS_CORPSE,
   BAGFLAG_NOT_CLOSABLE,
   BAGFLAG_PLAYER_OWNED,
   BAGFLAG_CONSUMES_KEY
};

enum door_data_flags {
   DDFLAG_OPEN_EXIT,
   DDFLAG_MAGIC_LOCKABLE,
   DDFLAG_CLOSED,
   DDFLAG_LOCKED,
   DDFLAG_PICKABLE,
   DDFLAG_LOCKABLE,
   DDFLAG_MAG_LOCKED,
   DDFLAG_DESTRUCTABLE,
   DDFLAG_CLOSABLE,
   DDFLAG_FLIPPABLE,
   DDFLAG_IN_USE,
   DDFLAG_PC_CANT_OPEN,
   DDFLAG_VEHICLE_EXIT,
   DDFLAG_SECRET,
   DDFLAG_BLOCKED,
   DDFLAG_NOT_COMPLETE,
   DDFLAG_SECRET_WHEN_OPEN,
   DDFLAG_CONSUMES_KEY,
   DDFLAG_NO_PASSDOOR
};

enum room_flags {
   ROOMFLAG_NO_RESTRICTIONS,
   ROOMFLAG_NO_IMM,
   ROOMFLAG_NO_GOD,
   ROOMFLAG_IS_PERM_DARK,
   ROOMFLAG_WEATHER,
   ROOMFLAG_NO_SHOUT,
   ROOMFLAG_NO_MAGICAL_EXIT,
   ROOMFLAG_IS_HAVEN,
   ROOMFLAG_NO_PK,
   ROOMFLAG_NO_MAGIC,
   ROOMFLAG_NO_MOB,
   ROOMFLAG_NO_POTIONS,
   ROOMFLAG_NO_STAFFS,
   ROOMFLAG_NO_MORTALS,
   ROOMFLAG_NORMALLY_DARK,
   ROOMFLAG_SHALLOW_WATER,
   ROOMFLAG_DEEP_WATER,
   ROOMFLAG_SWAMP,
   ROOMFLAG_NEED_FLY,
   ROOMFLAG_NEED_BOAT,
   ROOMFLAG_NEED_CLIMB,
   ROOMFLAG_IS_ZLOCKED,
   ROOMFLAG_IS_TOTAL_LOADED,
   ROOMFLAG_IS_USED,
   ROOMFLAG_NO_MAGICAL_ENTRY,
   ROOMFLAG_NO_VEHICLES,
   ROOMFLAG_CRAMPED,
   ROOMFLAG_NO_RANGED_ATTACK,
   ROOMFLAG_NEED_DIVE,
   ROOMFLAG_USED_IN_TRACK,
   ROOMFLAG_CAN_CAMP,
   ROOMFLAG_NOT_COMPLETE,
   ROOMFLAG_HAS_KEYWORDS,
   ROOMFLAG_NO_WANDERING_MOBS,
   ROOMFLAG_NO_FOREIGN_WANDERING_MOBS,
   ROOMFLAG_HAS_PROC_SCRIPT
};

enum vehicle_flags {
   VEHFLAG_SELF_GUIDED,
   VEHFLAG_STEALTHY,
   VEHFLAG_UNLIMITED_FUEL,
   VEHFLAG_FLIES,
   VEHFLAG_CLIMBS,
   VEHFLAG_FLOATS,
   VEHFLAG_DIVES,
   VEHFLAG_CAN_SEE_OUT
};

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

/* Used by critter:show() to be passed on to colorize() */
enum hilite_type {
   HL_GOSSIP,
   HL_SAY,
   HL_YELL,
   HL_TELL,
   HL_DESC,
   HL_OBJ_LIST,
   HL_MOB_LIST,
   HL_DEF,
   HL_BAK,
   HL_BATTLE,
   HL_USR1,
   HL_USR2,
   HL_USR3,
   HL_ROOM
};

extern ServerConfig config;

#ifdef USEMYSQL
extern MYSQL* database;

enum ObjTableFieldIndex {
   OBJTBL_OBJ_NUM,
   OBJTBL_SOBJ_NUM,
   OBJTBL_SHORT_DESC,
   OBJTBL_IN_ROOM_DESC,
   OBJTBL_LONG_DESC,
   OBJTBL_CHARGES,
   OBJTBL_TIMES_RECHARGABLE,
   OBJTBL_PERCENT_LOAD,
   OBJTBL_VIS_BIT,
   OBJTBL_PRICE,
   OBJTBL_FROM_ZONE,
   OBJTBL_SKIN_NUMBER,
   OBJTBL_MAX_IN_GAME,
   OBJTBL_WEIGHT,
   OBJTBL_DICE_SIDES,
   OBJTBL_DICE_COUNT,
   OBJTBL_LEVEL,
   OBJTBL_KEY_NUM,
   OBJTBL_MAX_WEIGHT,
   OBJTBL_PERCENT_WEIGHT,
   OBJTBL_TIME_TILL_DISSOLVE,
   OBJTBL_NO_REST,
   OBJTBL_ANTI_EVIL,
   OBJTBL_ANTI_NEUTRAL,
   OBJTBL_ANTI_GOOD,
   OBJTBL_ANTI_DONATE,
   OBJTBL_ANTI_DROP,
   OBJTBL_ANTI_REMOVE,
   OBJTBL_ANTI_MORTAL,
   OBJTBL_ANTI_IMMORTAL,
   OBJTBL_ANTI_DEMI,
   OBJTBL_IN_USE,
   OBJTBL_ANTI_WARRIOR,
   OBJTBL_ANTI_SAGE,
   OBJTBL_ANTI_WIZARD,
   OBJTBL_ANTI_RANGER,
   OBJTBL_ANTI_THIEF,
   OBJTBL_ANTI_ALCHEMIST,
   OBJTBL_ANTI_CLERIC,
   OBJTBL_ANTI_BARD,
   OBJTBL_ANTI_MOB,
   OBJTBL_ANTI_PC,
   OBJTBL_NOT_WORN,
   OBJTBL_HEAD,
   OBJTBL_NECK1,
   OBJTBL_NECK2,
   OBJTBL_AROUND_BODY,
   OBJTBL_ARMS,
   OBJTBL_WRIST1,
   OBJTBL_WRIST2,
   OBJTBL_HANDS,
   OBJTBL_WIELDED,
   OBJTBL_HELD,
   OBJTBL_LIGHT,
   OBJTBL_BODY,
   OBJTBL_BELT,
   OBJTBL_LEGS,
   OBJTBL_FEET,
   OBJTBL_FINGER1,
   OBJTBL_FINGER2,
   OBJTBL_SHIELD,
   OBJTBL_NOT_A_WEAPON,
   OBJTBL_SLASH,
   OBJTBL_SMASH,
   OBJTBL_PIERCE,
   OBJTBL_WHIP,
   OBJTBL_NEEDS_AMMO,
   OBJTBL_DART_THROWER,
   OBJTBL_BOW,
   OBJTBL_DART,
   OBJTBL_ARROW,
   OBJTBL_JUNK,
   OBJTBL_WAND,
   OBJTBL_POTION,
   OBJTBL_SCROLL,
   OBJTBL_CONTAINER,
   OBJTBL_COINS,
   OBJTBL_ARMOR,
   OBJTBL_WEAPON,
   OBJTBL_LIGHT_SOURCE,
   OBJTBL_CANTEEN,
   OBJTBL_LIQUID,
   OBJTBL_FOOD,
   OBJTBL_BOAT,
   OBJTBL_HAS_SPEC_PROC_DATA,
   OBJTBL_TOOLBOX,
   OBJTBL_CAULDRON,
   OBJTBL_PEN,
   OBJTBL_CONSTRUCT_COMPONENT,
   OBJTBL_CONCOCT_COMPONENT,
   OBJTBL_PARCHMENT,
   OBJTBL_NEEDS_RESETTING,
   OBJTBL_HERB,
   OBJTBL_VENDING_MACHINE,
   OBJTBL_BULLETIN_BOARD,
   OBJTBL_IS_BUTCHERABLE,
   OBJTBL_HAS_OBJ_SCRIPT,
   OBJTBL_IS_CLOSED,
   OBJTBL_IS_PICKABLE,
   OBJTBL_IS_LOCKED,
   OBJTBL_IS_MAG_LOCKABLE,
   OBJTBL_IS_MAG_LOCKED,
   OBJTBL_IS_DESCTRUCTABLE,
   OBJTBL_IS_CORPSE,
   OBJTBL_NOT_CLOSABLE,
   OBJTBL_IS_PLAYER_OWNED,
   OBJTBL_CONSUMES_KEY,
   OBJTBL_CONSUME_TELEPORT,
   OBJTBL_HAS_CONSTRUCT_DATA,
   OBJTBL_HAS_SKIN,
   OBJTBL_CONSUME_POISON,
   OBJTBL_CASTS_SPELLS
};

enum ObjScriptTableFieldsIndex {
   OSCRTB_OBJ_NUM,
   OSCRTBL_SOBJ_NUM,
   OSCRTBL_TRIGGER_CMD,
   OSCRTBL_TARGET,
   OSCRTBL_ACTOR,
   OSCRTBL_DISCRIMINATOR,
   OSCRTBL_PRECEDENCE,
   OSCRTBL_SCRIPT_COMMANDS
};

enum ObjInvTableFieldsIndex {
   OBJINVTBL_OBJ_NUM,
   OBJINVTBL_SOBJ_NUM,
   OBJINVTBL_CONTENT_OBJ_NUM,
   OBJINVTBL_CONTENT_SOBJ_NUM,
   OBJINVTBL_QUANTITY
};

enum DoorTableFieldsIndex {
   DOORTBL_DOOR_NUM,
   DOORTBL_LONG_DESC,
   DOORTBL_VIS_BIT,
   DOORTBL_TOKEN_NUM,
   DOORTBL_IN_ZONE,
   DOORTBL_OPEN_EXIT,
   DOORTBL_MAGIC_LOCKABLE,
   DOORTBL_CLOSED,
   DOORTBL_LOCKED,
   DOORTBL_PICKABLE,
   DOORTBL_LOCKABLE,
   DOORTBL_MAG_LOCKED,
   DOORTBL_DESTRUCTABLE,
   DOORTBL_CLOSABLE,
   DOORTBL_FLIPPABLE,
   DOORTBL_PC_CANT_OPEN,
   DOORTBL_VEHICLE_EXIT,
   DOORTBL_SECRET,
   DOORTBL_BLOCKED,
   DOORTBL_SECRET_WHEN_OPEN,
   DOORTBL_CONSUMES_KEY,
   DOORTBL_NO_PASSDOOR
};

enum RoomTableFieldsIndex {
   ROOMTBL_ROOM_NUM,
   ROOMTBL_SHORT_DESC,
   ROOMTBL_LONG_DESC,
   ROOMTBL_VIS_BIT,
   ROOMTBL_MOVEMENT_COST,
   ROOMTBL_FALL_TO,
   ROOMTBL_IN_ZONE,
   ROOMTBL_NO_RESTRICTIONS,
   ROOMTBL_NO_IMM,
   ROOMTBL_NO_GOD,
   ROOMTBL_IS_PERM_DARK,
   ROOMTBL_WEATHER,
   ROOMTBL_NO_SHOUT,
   ROOMTBL_NO_MAGICAL_EXIT,
   ROOMTBL_IS_HAVEN,
   ROOMTBL_NO_PK,
   ROOMTBL_NO_MAGIC,
   ROOMTBL_NO_MOB,
   ROOMTBL_NO_POTIONS,
   ROOMTBL_NO_STAFFS,
   ROOMTBL_NO_MORTALS,
   ROOMTBL_NORMALLY_DARK,
   ROOMTBL_SHALLOW_WATER,
   ROOMTBL_DEEP_WATER,
   ROOMTBL_SWAMP,
   ROOMTBL_NEED_FLY,
   ROOMTBL_NEED_BOAT,
   ROOMTBL_NEED_CLIMB,
   ROOMTBL_NO_MAGICAL_ENTRY,
   ROOMTBL_NO_VEHICLES,
   ROOMTBL_CRAMPED,
   ROOMTBL_NO_RANGED_ATTACK,
   ROOMTBL_NEED_DIVE,
   ROOMTBL_USED_IN_TRACK,
   ROOMTBL_CAN_CAMP,
   ROOMTBL_HAS_KEYWORDS,
   ROOMTBL_NO_WANDERING_MOBS,
   ROOMTBL_NO_FOREIGN_WANDERING_MOBS,
   ROOMTBL_HAS_PROC_SCRIPT
};

enum RoomScriptTableFieldsIndex {
   RMSCRTBL_ROOM_NUM,
   RMSCRTBL_TRIGGER_CMD,
   RMSCRTBL_TARGET,
   RMSCRTBL_ACTOR,
   RMSCRTBL_DISCRIMINATOR,
   RMSCRTBL_PRECEDENCE,
   RMSCRTBL_SCRIPT_COMMANDS
};

enum VehicleTableFieldsIndex {
   VEHTBL_ROOM_NUM,
   VEHTBL_IN_ROOM,
   VEHTBL_TICKS_BETWEEN_STOPS,
   VEHTBL_CUR_FUEL,
   VEHTBL_MAX_FUEL,
   VEHTBL_SELF_GUIDED,
   VEHTBL_STEALTHY,
   VEHTBL_UNLIMITED_FUEL,
   VEHTBL_FLIES,
   VEHTBL_CLIMBS,
   VEHTBL_FLOATS,
   VEHTBL_DIVES,
   VEHTBL_CAN_SEE_OUT
};

enum VPathTableFieldIndex {
   VPATHTBL_ROOM_NUM,
   VPATHTBL_STOP_NUM,
   VPATHTBL_DESCRIPTION,
   VPATHTBL_DIR,
   VPATHTBL_I_TH,
   VPATHTBL_IS_DESTINATION
};

enum CritterTableFieldsIndex {
   CRITTBL_MOB_NUMBER,
   CRITTBL_PC_NUMBER,
   CRITTBL_SHORT_DESC,
   CRITTBL_IN_ROOM_DESC,
   CRITTBL_LONG_DESC,
   CRITTBL_EXP_WORTH,
   CRITTBL_GOLD,
   CRITTBL_BANK_GOLD,
   CRITTBL_POSITION,
   CRITTBL_STR,
   CRITTBL_INTELLIGENCE,
   CRITTBL_CON,
   CRITTBL_CHA,
   CRITTBL_WIS,
   CRITTBL_DEX,
   CRITTBL_HIT,
   CRITTBL_DAM,
   CRITTBL_AC,
   CRITTBL_ATTACKS,
   CRITTBL_BHD_COUNT,
   CRITTBL_BHD_SIDES,
   CRITTBL_PAUSE_COUNT,
   CRITTBL_SEX,
   CRITTBL_CLASS,
   CRITTBL_RACE,
   CRITTBL_ALIGNMENT,
   CRITTBL_LEVEL,
   CRITTBL_IMM_LEVEL,
   CRITTBL_HOMETOWN,
   CRITTBL_WIMPY,
   CRITTBL_PRACTICES,
   CRITTBL_MOB_TYPE,
   CRITTBL_RELIGION,
   CRITTBL_GUILD,
   CRITTBL_FROM_ZONE,
   CRITTBL_VIS_BITS,
   CRITTBL_SEE_BITS,
   CRITTBL_IN_ROOM,
   CRITTBL_SKIN,
   CRITTBL_HP,
   CRITTBL_MANA,
   CRITTBL_MOV,
   CRITTBL_HP_MAX,
   CRITTBL_MANA_MAX,
   CRITTBL_MOV_MAX,
   CRITTBL_HP_REGEN,
   CRITTBL_MANA_REGEN,
   CRITTBL_MOV_REGEN,
   CRITTBL_DAM_REC_MOD,
   CRITTBL_DAM_GIV_MOD,
   CRITTBL_HEAT_RESIS,
   CRITTBL_COLD_RESIS,
   CRITTBL_ELECT_RESIS,
   CRITTBL_SPELL_RESIS,
   CRITTBL_WEIGHT,
   CRITTBL_MAX_IN_GAME,
   CRITTBL_BIRTH_DAY,
   CRITTBL_BIRTH_YEAR,
   CRITTBL_RENT_DAY,
   CRITTBL_RENT_YEAR,
   CRITTBL_LAST_LOGIN_TIME,
   CRITTBL_TOTAL_TIME_ONLINE,
   CRITTBL_AGE,
   CRITTBL_HUNGER,
   CRITTBL_THIRST,
   CRITTBL_DRUGGEDNESS,
   CRITTBL_PKILLS,
   CRITTBL_DEATHS,
   CRITTBL_QUEST_POINTS,
   CRITTBL_POOFIN_MSG,
   CRITTBL_POOFOUT_MSG,
   CRITTBL_PREFERRED_LANGUAGE,
   CRITTBL_PASSWORD,
   CRITTBL_LINES_ON_PAGE,
   CRITTBL_PROMPT,
   CRITTBL_GOS_COLOR,
   CRITTBL_SAY_COLOR,
   CRITTBL_YELL_COLOR,
   CRITTBL_TELL_COLOR,
   CRITTBL_DESC_COLOR,
   CRITTBL_OBJ_LIST_COLOR,
   CRITTBL_MOB_LIST_COLOR,
   CRITTBL_DEFAULT_COLOR,
   CRITTBL_BACKGROUND_COLOR,
   CRITTBL_BATTLE_COLOR,
   CRITTBL_USER1_COLOR,
   CRITTBL_USER2_COLOR,
   CRITTBL_USER3_COLOR,
   CRITTBL_ROOM_NAME_COLOR,
   CRITTBL_GUARDING_DIRECTION,
   CRITTBL_STORE_MANAGER,
   CRITTBL_WRONG_ALIGN_MSG,
   CRITTBL_WRONG_CLASS_MSG,
   CRITTBL_WRONG_RACE_MSG,
   CRITTBL_MARKUP,
   CRITTBL_BUY_PERCENTAGE,
   CRITTBL_OPEN_TIME,
   CRITTBL_CLOSE_TIME,
   CRITTBL_SKILL_VIOLENCE,
   CRITTBL_BENEVOLENCE,
   CRITTBL_DEFENSIVENESS,
   CRITTBL_BAD_ASSEDNESS,
   CRITTBL_SOCIAL_AWARENESS,
   CRITTBL_CAN_SEE_INVENTORY,
   CRITTBL_USING_LIGHT_SOURCE,
   CRITTBL_IS_FLYING,
   CRITTBL_HAS_BOAT,
   CRITTBL_CAN_CLIMB,
   CRITTBL_GOSSIPS,
   CRITTBL_YELLS,
   CRITTBL_GRATZ,
   CRITTBL_AUCTIONS,
   CRITTBL_SHOUTS,
   CRITTBL_SAYS,
   CRITTBL_TELLS,
   CRITTBL_WIZNETS,
   CRITTBL_IS_PARALYZED,
   CRITTBL_IS_PERM_SLEEPED,
   CRITTBL_IS_DUAL_WIELDING,
   CRITTBL_IS_SNEAKING,
   CRITTBL_IN_USE,
   CRITTBL_CAN_DIVE,
   CRITTBL_IS_HIDE,
   CRITTBL_IS_FROZEN,
   CRITTBL_IS_GAGGED,
   CRITTBL_HAS_IMM_DATA,
   CRITTBL_CLOAKED,
   CRITTBL_TANK_GRAPH,
   CRITTBL_USING_CLIENT,
   CRITTBL_AUTO_EXIT,
   CRITTBL_NO_HASSLE,
   CRITTBL_BRIEF,
   CRITTBL_IS_BUILDER,
   CRITTBL_AUTOLOOT,
   CRITTBL_AUTOSPLIT,
   CRITTBL_EXTRA_INFO,
   CRITTBL_CR_BEHIND,
   CRITTBL_DO_CARRIAGE_RETURN,
   CRITTBL_CAN_DETECT_MAGIC,
   CRITTBL_DETECT_INVENTORY,
   CRITTBL_SHOW_VNUMS,
   CRITTBL_HAS_POOFIN_POOFOUT_MSG,
   CRITTBL_PAGE_OUTPUT,
   CRITTBL_NO_WIZCHAT,
   CRITTBL_HAS_COLORS,
   CRITTBL_USE_COLOR,
   CRITTBL_HAS_LANGUAGE_CHOICE,
   CRITTBL_NO_SHOW_NPC_ENTRY,
   CRITTBL_NO_BEEP,
   CRITTBL_IS_REMORT,
   CRITTBL_HAS_SACRIFICED,
   CRITTBL_HAS_PROC_DATA,
   CRITTBL_SCAVENGE,
   CRITTBL_WANDER,
   CRITTBL_SHOULD_DO_PROCS,
   CRITTBL_EDIBLE,
   CRITTBL_IS_BANKER,
   CRITTBL_IS_SESSILE,
   CRITTBL_NO_HOMING,
   CRITTBL_DISOLVABLE,
   CRITTBL_HAS_SKIN,
   CRITTBL_HAS_MOB_SCRIPTS,
   CRITTBL_IS_SHOPKEEPER,
   CRITTBL_IS_TEACHER,
   CRITTBL_LET_SAME_CLASS_PASS,
   CRITTBL_LET_SAME_RACE_PASS,
   CRITTBL_HAS_MOB_GIV_PROC,
   CRITTBL_HAS_MOB_SAY_PROC,
   CRITTBL_HAS_MOB_BOW_PROC,
   CRITTBL_HAS_MOB_CURSE_PROC,
   CRITTBL_PROC_WITH_SAME_RACE,
   CRITTBL_PROC_WITH_SAME_ALIGN,
   CRITTBL_PROC_WITH_SAME_CLASS,
   CRITTBL_HAS_AI,
   CRITTBL_BUY_0,
   CRITTBL_SELL_0,
   CRITTBL_OFFER_0,
   CRITTBL_IS_PLAYER_OWNED,
   CRITTBL_BUYS_NON_WEAPONS,
   CRITTBL_BUYS_SLASHING_WEAPONS,
   CRITTBL_BUYS_SMASHING_WEAPONS,
   CRITTBL_BUYS_PIERCING_WEAPONS,
   CRITTBL_BUYS_WHIPS,
   CRITTBL_BUYS_AMMO_NEEDING_WEAPONS,
   CRITTBL_BUYS_DART_THROWERS,
   CRITTBL_BUYS_BOWS,
   CRITTBL_BUYS_DARTS,
   CRITTBL_BUYS_ARROWS,
   CRITTBL_BUYS_JUNK,
   CRITTBL_BUYS_WANDS,
   CRITTBL_BUYS_POTIONS,
   CRITTBL_BUYS_SCROLLS,
   CRITTBL_BUYS_CONTAINERS,
   CRITTBL_BUYS_COINS,
   CRITTBL_BUYS_ARMOR,
   CRITTBL_BUYS_WEAPONS,
   CRITTBL_BUYS_LIGHTS,
   CRITTBL_BUYS_CANTEENS,
   CRITTBL_BUYS_LIQUIDS,
   CRITTBL_BUYS_FOOD,
   CRITTBL_BUYS_BOATS,
   CRITTBL_BUYS_TOOLBOXES,
   CRITTBL_BUYS_CAULDRENS,
   CRITTBL_BUYS_PENS,
   CRITTBL_BUYS_CONSTRUCT_COMPONENTS,
   CRITTBL_BUYS_CONCOCT_COMPONENTS,
   CRITTBL_BUYS_PARCHMENTS,
   CRITTBL_BUYS_HERBS,
   CRITTBL_BUYS_VENDING_MACHINES,
   CRITTBL_TEACHES_WARRIORS,
   CRITTBL_TEACHES_SAGES,
   CRITTBL_TEACHES_WIZARDS,
   CRITTBL_TEACHES_RANGERS,
   CRITTBL_TEACHES_THIEVES,
   CRITTBL_TEACHES_ALCHEMISTS,
   CRITTBL_TEACHES_CLERICS,
   CRITTBL_TEACHES_BARDS,
};

enum CritScriptTableFieldsIndex {
   CRSTBL_MOB_NUMBER,
   CRSTBL_TRIGGER_CMD,
   CRSTBL_TARGET,
   CRSTBL_ACTOR,
   CRSTBL_DISCRIMINATOR,
   CRSTBL_PRECEDENCE,
   CRSTBL_SCRIPT_COMMANDS
};

enum CritProcTableFieldsIndex {
   CRPTBL_MOB_NUMBER,
   CRPTBL_PROC_TYPE,
   CRPTBL_PROC_TOPIC,
   CRPTBL_CORRECT_MSG,
   CRPTBL_INCORRECT_MSG,
   CRPTBL_TEST_ONUM,
   CRPTBL_AWARD_ONUM,
   CRPTBL_SKILL_NAME,
   CRPTBL_TRANS_ROOM
};
#endif

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
extern LogStream obj_ptr_log;

extern bitfield Obj_Wear_Flags_Mask;
extern bitfield Obj_Consume_Procs_Mask;
extern bitfield Obj_Wear_Procs_Mask;
extern bitfield Obj_Remove_Procs_Mask;


extern char* olc_prompts[];
extern char* login_prompts[];

extern int do_shutdown;
extern int Cur_Max_Obj_Num;
extern int Cur_Max_Door_Num;
extern int Cur_Max_Room_Num;
extern int Cur_Max_Crit_Num;
extern int grr_reboot;
extern int BOOT_TIME; //time in seconds that it booted.
extern int DFLT_PORT;
extern int First_Room;        /* used in pulsed spec procs */
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
   BALLISTICS_SKILL_NUM,
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
   CREATE_GREATER_GOLEM_SKILL_NUM,
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
   DETECT_RESISTANCES_SKILL_NUM,
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
   FLESH_TO_STONE_SKILL_NUM,
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
   INFRAVISION_SKILL_NUM,
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
   ORB_OF_POWER_SKILL_NUM,
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
   SOBER_SKILL_NUM,
   SOD_SKILL_NUM,
   STEAL_SKILL_NUM,
   STRENGTH_SKILL_NUM,
   STRENGTH_CONDITIONING_SKILL_NUM,
   STONE_SKIN_SKILL_NUM,
   SUMMON_SKILL_NUM,
   SWORD_SKILL_NUM,
   SWORDBOND_SKILL_NUM,
   TAIL_SKILL_NUM,
   TAMMUZ_SKILL_NUM,
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
   WRESTLING_SKILL_NUM,
// New skills for the Avian/Dragon race
// [RJY] - Trice
   HOVER_SKILL_NUM,
   WING_POWER_SKILL_NUM,
   SECRET_FEATHER_SKILL_NUM,
   SHRIEK_SKILL_NUM,
   CARRY_SKILL_NUM,
   PECK_SKILL_NUM,
   TAILSWEEP_SKILL_NUM,
   VISION_SKILL_NUM,
   DIVE_SKILL_NUM,
   GLARE_SKILL_NUM,
   WINDZONE_SKILL_NUM,
   PLUCK_SKILL_NUM,
   BIRDSEYE_SKILL_NUM,
   // New skills for the Necromancer class
   CURSING_SKILL_NUM,
   RUST_SKILL_NUM,
   DISFAVOR_SKILL_NUM,
   REMOVE_SOUL_SKILL_NUM,
   REMOVE_HOPE_SKILL_NUM,
   REMOVE_KARMA_SKILL_NUM,
   SANCTUM_OF_THE_VICTIM_SKILL_NUM,
   FEAR_SKILL_NUM,
   NECROPHILIA_SKILL_NUM;
   


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

#define ANSI_BLACK        "\x1B[30m"
#define ANSI_RED        "\x1B[31m"
#define ANSI_GREEN        "\x1B[32m"
#define ANSI_YELLOW        "\x1B[33m"
#define ANSI_BLUE        "\x1B[34m"
#define ANSI_MAGENTA        "\x1B[35m"
#define ANSI_CYAN        "\x1B[36m"
#define ANSI_WHITE        "\x1B[37m"

/* Foreground colors w/o escape or terminator */
#define BB_ANSI_BLACK        "30"
#define BB_ANSI_RED          "31"
#define BB_ANSI_GREEN        "32"
#define BB_ANSI_YELLOW       "33"
#define BB_ANSI_BLUE         "34"
#define BB_ANSI_MAGENTA      "35"
#define BB_ANSI_CYAN         "36"
#define BB_ANSI_WHITE        "37"

/* Background colors */

#define ANSI_BBLACK        "\x1B[40m"
#define ANSI_BRED        "\x1B[41m"
#define ANSI_BGREEN        "\x1B[42m"
#define ANSI_BYELLOW        "\x1B[43m"
#define ANSI_BBLUE        "\x1B[44m"
#define ANSI_BMAGENTA        "\x1B[45m"
#define ANSI_BCYAN        "\x1B[46m"
#define ANSI_BWHITE        "\x1B[47m"

/* Background colors w/o escape or terminator */
#define BB_ANSI_BBLACK       "40"
#define BB_ANSI_BRED         "41"
#define BB_ANSI_BGREEN       "42"
#define BB_ANSI_BYELLOW      "43"
#define BB_ANSI_BBLUE        "44"
#define BB_ANSI_BMAGENTA     "45"
#define BB_ANSI_BCYAN        "46"
#define BB_ANSI_BWHITE       "47"

/* Other BB (Building Block) Components */
#define BB_ANSI_NORMAL       "0"
#define BB_ANSI_HILITE       "1"
#define BB_ANSI_SEPERATE     ";"
#define ESC_SEQ              "\x1B["
#define END_ESC              "m"


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

#define ANSI_BLACK        "\033[30m"
#define ANSI_RED        "\033[31m"
#define ANSI_GREEN        "\033[32m"
#define ANSI_YELLOW        "\033[33m"
#define ANSI_BLUE        "\033[34m"
#define ANSI_MAGENTA        "\033[35m"
#define ANSI_CYAN        "\033[36m"
#define ANSI_WHITE        "\033[37m"

/* Background colors */

#define ANSI_BBLACK        "\033[40m"
#define ANSI_BRED        "\033[41m"
#define ANSI_BGREEN        "\033[42m"
#define ANSI_BYELLOW        "\033[43m"
#define ANSI_BBLUE        "\033[44m"
#define ANSI_BMAGENTA        "\033[45m"
#define ANSI_BCYAN        "\033[46m"
#define ANSI_BWHITE        "\033[47m"

#endif
// End of penn-mush ansi color stuff...
extern const String ANSI_NONE_S;

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


#define NUMBER_OF_ZONES                 160 
#define NUMBER_OF_ROOMS                 16000
#define NUMBER_OF_MOBS                  3000
#define NUMBER_OF_ITEMS                 3000
#define NUMBER_OF_DOORS                 500

#define OBJ_MAX_EXTRAS                         9
#define OBJ_CUR_STATS                        4
#define MOB_LONG_DATA                        3
#define MOB_SHORT_CUR_STATS                43
#define MOB_CUR_STATS                        3
#define ROOM_CUR_STATS                        4

// Flags sent to the do_emote method.
#define EMOTE_POSSESSIVE         1
#define EMOTE_NON_POSSESSIVE     2


#define IMM_LEVEL        getImmLevel()
#define HUNTING                mob->proc_data->temp_proc->hunting
#define TRACKING               mob->proc_data->temp_proc->tracking
#define SNOOPED_BY        pc->snoop_by
#define SNOOPING        pc->snooping
#define MODE                getMode()
#define BIRTH_DAY        pc->birth_day
#define BIRTH_MONTH        pc->birth_month
#define BIRTH_YEAR        pc->birth_year
#define RENT_DAY        pc->rent_day
#define RENT_YEAR        pc->rent_year

#define WRONG_ALIGN_MSG mob->proc_data->wrong_align_msg
#define WRONG_RACE_MSG  mob->proc_data->wrong_race_msg
#define WRONG_CLASS_MSG mob->proc_data->wrong_class_msg

#define GIVE_PROC       mob->proc_data->give_proc
#define DOMOB_GIVE_NUM  mob->proc_data->give_proc->test_num
#define GIVE_RIGHT_MSG  mob->proc_data->give_proc->correct_msg
#define GIVE_RIGHT_ITEM mob->proc_data->give_proc->obj_num
#define GIVE_TEACH      mob->proc_data->give_proc->skill_name
#define GIVE_WRONG_MSG  mob->proc_data->give_proc->wrong_gift_msg
#define GIVE_TRANSPORT_ROOM          mob->proc_data->give_proc->trans_to_room


#define BOW_PROC                 mob->proc_data->bow_proc
#define BOW_MSG                   mob->proc_data->bow_proc->correct_msg
#define BOW_ITEM                  mob->proc_data->bow_proc->obj_num
#define BOW_TEACH                       mob->proc_data->bow_proc->skill_name
#define BOW_TRANSPORT_ROOM           mob->proc_data->bow_proc->trans_to_room


#define CURSE_PROC               mob->proc_data->curse_proc
#define CURSE_MSG                   mob->proc_data->curse_proc->correct_msg
#define CURSE_ITEM                  mob->proc_data->curse_proc->obj_num
#define CURSE_TEACH                 mob->proc_data->curse_proc->skill_name
#define CURSE_TRANSPORT_ROOM           mob->proc_data->curse_proc->trans_to_room

#define TOPICS                 mob->proc_data->topics
#define BUY_PERCENTAGE   mob->proc_data->sh_data->buy_percentage
#define OPEN_TIME         mob->proc_data->sh_data->open_time
#define CLOSE_TIME         mob->proc_data->sh_data->close_time
#define MARKUP                 mob->proc_data->sh_data->markup
#define PC_FLAGS         pc->pc_data_flags
#define GUARDING         temp_crit->guarding
#define SHIELDING        temp_crit->shielding
#define SHIELDED_BY      temp_crit->shielded_by
#define GUARDED_BY       temp_crit->guarded_by
#define MOB_FLAGS        mob->mob_data_flags
#define IS_FIGHTING         is_fighting
#define IS_BRIEF         isBrief()
#define USING_CLIENT         isUsingClient()
#define IS_SAY                 crit_flags.get(11)
#define IS_YELL                 crit_flags.get(7)
#define IS_GOSSIP         crit_flags.get(6)
#define IS_SHOUT         crit_flags.get(10)
#define IS_AUCTION         crit_flags.get(9)
#define CRIT_FLAGS         crit_flags
#define GROUPEES         groupees
#define PETS                 pets
#define FOLLOWER_OF         follower_of
#define MASTER                 master
#define EQ                 eq
#define SKILLS_KNOWN      skills_spells_known
#define FOLLOWERS         followers
#define FLAG1                 mob->proc_data->flag1
#define INT1                 mob->proc_data->int1
#define SHOP_DATA_FLAGS         mob->proc_data->sh_data->shop_data_flags
#define TEACH_DATA_FLAGS \
         mob->proc_data->teach_data->teach_data_flags
#define MARKUP                 mob->proc_data->sh_data->markup
#define PERM_INV         mob->proc_data->sh_data->perm_inv
#define MOB_NUM                getIdNum()
#define MINI_AFFECTED_BY         mini_affected_by
#define USING_LIGHT_SOURCE        1
#define DO_PROMPT                10 //in pc_data
#define PROMPT_STRING                 pc->prompt
#define PC_IS_BUILDER                11 // ""
#define BH_DICE_COUNT         short_cur_stats[35]
#define BH_DICE_SIDES         short_cur_stats[36]
#define PK_COUNT         pc->pk_count
#define DIED_COUNT         pc->died_count
#define QUEST_POINTS         pc->quest_points


#define MOB_GOSSIP        6
#define MOB_YELL        7
#define MOB_GRATZ        8
#define MOB_AUC                9
#define MOB_SHOUT        10
#define MOB_SAY                11
#define MOB_TELL        12
#define MOB_WIZNET        13


#define BAG_FLAGS       bag->bag_flags
#define OBJ_PRCNT_LOAD        extras[2]
//#define OBJ_CUR_IN_GAME extras[3]
#define OBJ_MAX_IN_GAME extras[4]
#define OBJ_IN_ZONE        cur_stats[3]
#define CASTS_THESE_SPELLS   obj_proc->casts_these_spells
#define COMPONENT_ITEM1 obj_proc->construct_data->item1
#define COMPONENT_ITEM2 obj_proc->construct_data->item2
#define COMPONENT_ITEM3 obj_proc->construct_data->item3
#define COMPONENT_ITEM4 obj_proc->construct_data->item4
#define COMPONENT_ITEM5 obj_proc->construct_data->item5
#define COMPONENT_TARG  obj_proc->construct_data->target_object
#define OBJ_PRICE        cur_stats[1]
#define OBJ_FLAGS        obj_flags
#define OBJ_SPEC_FLAGS        obj_proc->obj_spec_data_flags
#define OBJ_VIS_BIT        cur_stats[0]
#define IN_LIST                in_list
#define OBJ_CUR_WEIGHT        getCurWeight()
#define OBJ_MAX_WEIGHT        getMaxWeight()
#define OBJ_NUM                cur_stats[2]
#define CHARGES                        extras[0]
#define RECHARGE                       extras[1]
#define OBJ_BASE_WEIGHT                extras[5]
#define OBJ_DAM_DICE_SIDES        extras[6]
#define OBJ_DAM_DICE_COUNT        extras[7]
#define OBJ_LEVEL                extras[8]
#define OBJ_CONTAINER                54
#define OBJ_LIGHT_SOURCE        58
#define PRICE                        cur_stats[1]


#define RM_IS_HAVEN        7
#define RM_IS_NONPK        8
#define RM_IS_NOMAG        9
#define CRITTERS        critters
#define DOORS                doors

#define DOOR_VIS_BIT        dr_data->vis_bit
#define VIS_DARK        1 //regarding vis_bits in general

#define O_COUNT                pc.pc->imm_data->olc_counter
#define TEMP_OLC_INT        pc.pc->imm_data->temp_olc_int


extern int do_shutdown;   //in grrmud.cc
extern int grr_reboot;   //in grrmud.cc

extern PtrList<String>   banned_hosts;
extern class critter   mob_list[NUMBER_OF_MOBS + 1];
extern class object    obj_list[NUMBER_OF_ITEMS + 1];
extern class door_data door_list[NUMBER_OF_DOORS +1];

extern PtrList<class door>        affected_doors; 
extern PtrList<critter>   affected_mobs; /* list those affected*/ 
extern PtrList<struct object>    affected_objects; /* ditto for objs */
extern PtrList<critter>   pc_list;      //head of pc list  
extern PtrList<critter>   linkdead_list;  //holds link dead folks
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

//  RACES

#define OTHER_RACE      0
#define HUMAN           1
#define ANITRE          2
#define AVINTRE         3 
#define DARKLING        4
#define DROW            5
#define DRAGON          6
#define DWARF           7
#define IRONCLAD        8
#define OGRUE           9
#define ROCKTROLL       10
#define ELF             11
#define FAERIE          12
#define ENTITY          13
#define SOMBRIAN        14
#define UNDEAD          15
#define ANIMAL          16
#define MONSTER         17
#define AVIAN           18

// PLAYER CLASSES
#define NUMBER_OF_CLASSES       10

#define OTHER_CLASS             0
#define WARRIOR                 1
#define SAGE                    2
#define WIZARD                  3
#define RANGER                  4
#define THIEF                   5
#define ALCHEMIST               6
#define CLERIC                  7
#define BARD                    8
#define NECROMANCER             9

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
#define CRONIC          205                //poison,
#define NORMAL          206               //good ole hack/slash
#define SUMMON          207
#define CHARM           208
#define COERCION        209
#define AGILITY         210


#define HUNGER_ID          100                    //these are for identification
#define THIRST_ID          101                    //of stat_affects
#define DRUGGED_ID         102
#define STONED_ID          103

#define CON_PLAYING     1
#define CON_LINKDEAD    2
#define CON_LOGGING_IN  3

#define TRUE            1       
#define FALSE           0



#define MAX_INPUT_LEN   5000  //can input this many chars at one time
#define OUTPUT_MAX_LEN  25000  //can hold this many chars in output buffer
#define DFLT_DIR        .
#define MAX_EQ          19

#define ROOM_INV 0
#define OBJ_INV  1
#define CRIT_INV 2

#define ROOM             room_list[pc.getCurRoomNum()]
#define GOLD             long_data[0]
#define EXP              long_data[1]
#define BANK_GOLD             long_data[2]
#define AGE              pc->age
#define SEE_BIT          cur_stats[1]
#define VIS_BIT          cur_stats[0]
#define POS              getPosn()
#define STR              short_cur_stats[1]
#define INT              short_cur_stats[2]
#define CON              short_cur_stats[3]
#define CHA              short_cur_stats[4]
#define WIS              short_cur_stats[5]
#define DEX              short_cur_stats[6]
#define HIT              short_cur_stats[7]
#define DAM              short_cur_stats[8]
#define AC               short_cur_stats[9]
#define ATTACKS          short_cur_stats[10]
#define SEX              short_cur_stats[12]
#define CLASS            short_cur_stats[13]
#define RACE             short_cur_stats[14]
#define HP               short_cur_stats[15]
#define MANA             short_cur_stats[16]
#define MOV              short_cur_stats[17]
#define ALIGN            short_cur_stats[18]
#define LEVEL            short_cur_stats[19]
#define IN_ROOM          cur_stats[2]
#define WIMPY            short_cur_stats[21]
#define PRACS            short_cur_stats[22]
#define HP_MAX           getHP_MAX()
#define MA_MAX           short_cur_stats[24]
#define MV_MAX                  short_cur_stats[25]
#define CRITTER_TYPE            short_cur_stats[26] /* was SMOB */
#define DAM_REC_MOD             short_cur_stats[27]
#define DAM_GIV_MOD             short_cur_stats[28]
#define HUNGER           pc->hunger
#define THIRST           pc->thirst
#define MAX_IN_GAME           mob->max_in_game
#define DRUGGED          pc->drugged
#define CRIT_WT_CARRIED           getCurWeight()
#define CRIT_MAX_WT_CARRY             getMaxWeight()
#define PAUSE            short_cur_stats[11]
#define HEAT_RESIS          short_cur_stats[29]
#define COLD_RESIS          short_cur_stats[30]
#define ELEC_RESIS          short_cur_stats[31]
#define SPEL_RESIS          short_cur_stats[32]
#define RELIGION         short_cur_stats[33]
//#define MOB_FROM_ZONE          short_cur_stats[34]
#define HP_REGEN            short_cur_stats[37] 
#define MA_REGEN            short_cur_stats[38] 
#define MV_REGEN            short_cur_stats[39] 
#define GUILD                 short_cur_stats[40]


//   SPEC_PROCS

#define BATTLE_SPEC_PROC            1
#define JUST_KILLED_PROC            2
#define OBJ_WEAR_PROC            3 
#define OBJ_REMOVE_PROC          4
#define OBJ_CONSUME_PROC        5
#define JK_SPEC_PROC                6 


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

extern char* critter_conditions[11];

#define MAX_LEVEL    45
extern long levels[MAX_LEVEL];

extern const char* seasons[];

#endif
