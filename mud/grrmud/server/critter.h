// $Id: critter.h,v 1.64 2004/07/09 11:59:56 eroper Exp $
// $Revision: 1.64 $  $Author: eroper $ $Date: 2004/07/09 11:59:56 $

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

///**********************  critter.h  ***************************///

#ifndef GRRMUD_CRITTER_INCLUDE_H
#define GRRMUD_CRITTER_INCLUDE_H

#include <bitfield.h>
#include <string2.h>
#include "classes.h"
#include "object.h"
#include "door.h"
#include "room.h"
#include "const.h"
#include "lang_strings.h"

///********************  temp crit data  *****************************///

extern int bound(int l, int h, int v); //misc.h

class critter;
class room;
class CSelectorColl;

// MODES

enum PcMode {
   MODE_NORMAL = 0,
   MODE_OLC = 1,
   MODE_LOGGING_IN = 2,
   MODE_CH_DESC = 3,
   MODE_ADD_MOB_SCRIPT = 4,
   MODE_WRITING_POST = 5,
   MODE_GO_LINKDEAD_PLEASE = 6, // socket problem only
   MODE_QUIT_ME_PLEASE = 7,         // purposeful quit
   MODE_LOGOFF_NEWBIE_PLEASE = 9,    // pre-login socket problem
   MODE_LOG_OFF_LINKDEAD_PLEASE = 10, //to deal with chars on the linkdead list
   MODE_ADD_ROOM_SCRIPT = 11,
   MODE_ADD_OBJECT_SCRIPT = 12,
   MODE_ADD_BUG_COMMENT = 13,
   MODE_ADD_IDEA_COMMENT = 14
};//PcMode

enum PcPosition {
   POS_LOW_VAL,
   POS_STAND = POS_LOW_VAL,
   POS_SIT,
   POS_REST,
   POS_SLEEP,
   POS_MED,
   POS_STUN,
   POS_DEAD,
   POS_PRONE,
   POS_HIGH_VAL
};//PcPosition enum

extern const char* PcPositionStrings[];


class temp_crit_data {
protected:
   static int _cnt;
   
public:
   critter* guarded_by;
   critter* guarding;
   critter* shielded_by;
   critter* shielding;
   
   temp_crit_data();
   temp_crit_data(const temp_crit_data& source);
   ~temp_crit_data();

   int doUnShield();

   static int getInstanceCount() { return _cnt; }

   void Clear();
};//temp_crit_data


class MobScript;

class immort_data {
protected:
   static int _cnt;

public:
   int olc_counter;
   int temp_olc_int;
   critter* olc_mob;
   room* olc_room;
   door_data* olc_door;
   object* olc_obj;
   PtrList<String> tmplist;
   short imm_level;
   
   String* edit_string;
   GenScript* tmp_proc_script;
   int tmp_script_entity_num;
   String proc_script_buffer;
   
   immort_data();                                // default constructor
   immort_data(const immort_data& source);   // copy constructor
   ~immort_data();

   void Read(ifstream& dafile);
   void Write(ofstream& dafile);
   void Clear();
   static int getInstanceCount() { return _cnt; }
};//immort_data



///********************** teacher_data  **************************///

class teacher_data {
protected:
   static int _cnt;

public:
   bitfield teach_data_flags; // Same as classes
   // 1 Warrior, 2 Sage, 3 Wizard, 4 Ranger, 5 Thief, 6 Alchemist
   // 7 Cleric, 8 Bard 9 Necromancer
   
   teacher_data(); //constructor
   teacher_data(const teacher_data& source); // copy constructor
   ~teacher_data() { _cnt--; }

   int togTeachFlag(int flag) {
      teach_data_flags.flip(flag);
      return 0;
   }

   void Read(ifstream& da_file);
   void Write(ofstream& da_file) const;
   static int getInstanceCount() { return _cnt; }
}; //teacher_data


class PlayerShopData {
private:
   static int _cnt;

protected:
   int object_num;
   int sell_price; /* -1 is display only */
   int buy_price;  /* -1 is do not buy */

public:
   PlayerShopData() : object_num(0), sell_price(0), buy_price(0) {
      _cnt++;
   }

   PlayerShopData(int o, int s, int b) : object_num(o), sell_price(s), buy_price(b) {
      _cnt++;
   }

   PlayerShopData(const PlayerShopData& src) 
         : object_num(src.object_num), sell_price(src.sell_price),
           buy_price(src.buy_price) {
      _cnt++;
   }

   // No need to do an operator-equal now.

   ~PlayerShopData() { _cnt--; }

   int getObjNum() const { return object_num; }
   void setObjNum(int i) {
      if ((i >= 0) && (i < NUMBER_OF_ITEMS)) {
         object_num = i;
      }//if
   }//setObjNum

   int getSellPrice() const { return sell_price; }
   void setSellPrice(int i) { sell_price = i; }

   int getBuyPrice() const { return buy_price; }
   void setBuyPrice(int i) { buy_price = i; }

   static int getInstanceCount() { return _cnt; }

   void clear() { object_num = buy_price = sell_price = 0; }

   // Returns false when there are no more, object_num otherwise.
   int read(ifstream& da_file);
      
   void write(ofstream& da_file) const ;

}; // PlayerShopData


///*********************  shop_data  ***********************///

class shop_data {
private:
   static int _cnt;

public:
   short markup;       // %markup
   short buy_percentage;       // buy_percentage
   short open_time;
   short close_time;
   bitfield shop_data_flags; //0 buy_0, 1 sell_0, 2 offer_0
   // 3 Player-run shopkeeper
   // 40-73 type to trade flags, same as obj_flags
   //
   PtrList<object> perm_inv; //holds perm inventory

   // Holds extra info for player run shops.
   PtrList<PlayerShopData> ps_data_list;
   String manager;


   shop_data();
   shop_data(const shop_data& source);  //copy constructor
   ~shop_data();

   int isPlayerRun() const { return shop_data_flags.get(3); }

   void Clear();
   void Read(ifstream& da_file, short read_all);
   void Write(ofstream& da_file) const ;
   shop_data& operator=(const shop_data& src);

   static int getInstanceCount() { return _cnt; }

   void valueRem(int idx, critter& manager);
   void valueAdd(object& obj, critter& manager);
   void valueList(int i_th, const String* targ, critter& keeper, critter& manager);
   void valueSet(int val_idx, int sell_val, int buy_val, critter& manager);

   int isPlayerShopKeeper() { return shop_data_flags.get(3); }

   int findItemSalePrice(object& obj);
   int findItemBuyPrice(object& obj);

   int togShopFlag(int flag_num) {
      if ((flag_num >= 0) && (flag_num < 74)) {
         shop_data_flags.flip(flag_num);
         return 0;
      }
      return -1;
   }

   void makePSO() { shop_data_flags.turn_on(3); }

   PlayerShopData* getPsdFor(object& obj);
}; //shop_data


///****************  temp proc data  **************************///

class temp_proc_data {
protected:
   static int _cnt;

public:
   PtrList<String> hunting;
   String tracking;
   
   temp_proc_data();
   temp_proc_data(const temp_proc_data& source); //copy constructor
   ~temp_proc_data();

   void Clear();
   temp_proc_data& operator=(const temp_proc_data& source);
   static int getInstanceCount() { return _cnt; }
};//temp_proc_data



///**********************  say_proc_cell  ****************************///
///**  This is used to hold data for different topics in        *******///
///**  action_proc_data class.                                  *******///
///********************************************************************///

class say_proc_cell {
protected:
   static int _cnt;

public:
   String topic;
   String msg;        //msg to who is trying to discuss
   String skill_name; //skill to be taught upon this discussion
   int obj_num;       //to be given upon this discussion
   int trans_to_room; //number of room to be trans'd to upon this discussion
   
   say_proc_cell();
   say_proc_cell(const say_proc_cell& source);
   ~say_proc_cell() { _cnt--; }
   // say_proc_cell& operator=(const say_proc_cell& source);
   
   void Clear();
   void Read(ifstream& da_file);
   void Write(ofstream& da_file);
   static int getInstanceCount() { return _cnt; }
};//say_proc_cell



///**********************  action_proc_data  ****************************///
///**  This holds data for procs that are triggered by some action.  ****///
///**  For example, bow, curse, give, say...                         ****///
///**********************************************************************///

class action_proc_data {
protected:
   static int _cnt;

public:

   int test_num;                 //used with give
   String correct_msg;           //msg shown to pc upon correct gift/action
   String skill_name;            //skill to be taught upon a correct gift
   int obj_num;                  //to be given upon a correct gift
   int trans_to_room;            //room# to be trans'd to upon correct gift
   
   String wrong_gift_msg;
   
   action_proc_data();
   action_proc_data(const action_proc_data& source);
   action_proc_data& operator=(const action_proc_data& source); //op overload
   ~action_proc_data();
   
   void Clear();
   void Read(ifstream& da_file);
   void Write(ofstream& da_file);
   static int getInstanceCount() { return _cnt; }
};//action_proc_data



///*************************************************************///
///***                   spec data                         *****///
///***  Holds special info and procedures for NPC players  *****///
///*************************************************************///

class spec_data {
protected:
   short skill_violence;
   short benevolence;
   short defensiveness;
   short bad_assedness;
   short social_awareness;
   static int _cnt;

public:
   bitfield flag1; // 0 NULL 1 shopkeeper, 2 teacher
          //3 let_same_class_pass, 4 let_same_race_pass
          //5 has_mob_give_proc, 6 has_mob_say_proc, 7 has_mob_bow_proc,
          //8 has_mob_curse_proc, 9 proc_with_same_race, 
          //10 proc_with_same_align, 11 proc_with_same_class
          //12 NULL, 13 has_AI
          //
   short int1; //direction guarding, for sentinels
   shop_data* sh_data;
   teacher_data* teach_data;
   temp_proc_data* temp_proc;   //holds tracking and other info
   
   action_proc_data* give_proc;
   action_proc_data* bow_proc;
   action_proc_data* curse_proc;
   
   PtrList<say_proc_cell> topics; //used for say_procs (discuss)
   
   String wrong_align_msg; // these 3 are used with action/say procs
   String wrong_class_msg;
   String wrong_race_msg;
   
   spec_data();
   spec_data(const spec_data& source);   //copy constructor
   ~spec_data();

   void Clear();
   void Read(ifstream& da_file, short read_all);
   void Write(ofstream& da_file);
   spec_data& operator=(const spec_data& source);

   static int getInstanceCount() { return _cnt; }
   int getBenevolence() const { return benevolence; }
   int getSocialAwareness() const { return social_awareness; }
   int getSkillViolence() const { return skill_violence; }
   int getDefensiveness() const { return defensiveness; }
   int getBadAssedness() const { return bad_assedness; }

   int isSentinel() const { return (int1 != 0); }
   void auditAI();
   int hasAI() const { return flag1.get(13); }
   int hasShopData() { return (sh_data != 0); }

   // Higher is 'more'
   void setSocialAwareness(int i) { social_awareness = i; auditAI(); }
   void setBenevolence(int i) { benevolence = i; auditAI(); }
   void setSkillViolence(int i) { skill_violence = i; auditAI(); }
   void setDefensiveness(int i) { defensiveness = i; auditAI(); }
   void setBadAssedness(int i) { bad_assedness = i; auditAI(); }

   int togTeachFlag(int flag) {
      if (teach_data) {
         return teach_data->togTeachFlag(flag);
      }
      return -1;
   }

   int togShopFlag(int flag) {
      if (sh_data)
         return sh_data->togShopFlag(flag);
      return -1;
   }

}; //spec_data


///*********************** mob data ***************************///
 
class mob_data {
protected:
   int cur_in_game;
   int max_in_game;
   static int _cnt;
   int ticks_old;
   int ticks_till_freedom;

public:
   int mob_num;
   int tmp_num; //not used now

   // When modifying this, modify MOB_DATA_FLAGS_NAMES in const.cc
   // Also check the read method for mob_data, and the tog_mflag
   // methods to make sure the flag is not reset or ignored.
   bitfield mob_data_flags; //all the proc bits
     // 0 has_proc_data, 1 scavenge, 2 wander, 3 should_do_procs,
     // 4 need_resetting, 5 edible_corpse, 6 is_banker,
     // 7 sessile (won't track out of current room)
     // 8 !homing (won't come home after tracking),
     // 9 disolvable (ie summoned critters),
     // 10 !victim_flee, 11 NULL, 12 NULL, 13 NULL, 14 NULL, 15 NULL,
     // 16 has_skin, 17 has_mob_script, 18 is_guard
     //

   spec_data* proc_data;
   int skin_num;

   PtrList<MobScript> mob_proc_scripts;
   MobScript* cur_script; // a pointer into the List of MobScripts.
   PtrList<MobScript> pending_scripts;

   int home_room; /* set when first placed in a room. */

   mob_data();
   mob_data(mob_data& source);  //copy constructor
   ~mob_data();

   int getCurInGame() { return cur_in_game; }
   int setCurInGame(int i) { cur_in_game = i; return cur_in_game; }
   int getMaxInGame() { return max_in_game; }
   int setMaxInGame(int i) { max_in_game = i; return max_in_game; }
   int incrementCurInGame() { cur_in_game++; return cur_in_game; }
   int decrementCurInGame() { cur_in_game--; return cur_in_game; }
   int incrementMaxInGame() { max_in_game++; return max_in_game; }
   int decrementMaxInGame() { max_in_game--; return max_in_game; }

   int getBenevolence() const;
   int getBadAssedness() const;
   int getSocialAwareness() const;
   int getSkillViolence() const;
   int getDefensiveness() const;
   int getTicksOld() const { return ticks_old; }
   void setTicksOld(int i) { ticks_old = i; }
   void incrementTicksOld() { ticks_old++; }
   int getTicksTillFreedom() const { return ticks_till_freedom; }
   void setTicksTillFreedom(int i) { ticks_till_freedom = i; }
   void decrementTicksTillFreedom() { ticks_till_freedom--; }
   void setDisolvable(int val) { mob_data_flags.set(9, val); }
   int isDisolvable() const { return mob_data_flags.get(9); }

   // Higher is 'more'
   void setBenevolence(int i);
   void setBadAssedness(int i);
   void setSocialAwareness(int i);
   void setSkillViolence(int i);
   void setDefensiveness(int i);

   void addProcData(); //ensure there is a proc_data
   void auditAI(); //update flag according to internal values...

   int isSentinel() const;
   int hasProcData() const { return mob_data_flags.get(0); } 
   int hasMobScript() const { return mob_data_flags.get(17); }
   int isRunningScript() const { return cur_script != 0; }
   int isNoHoming() const { return mob_data_flags.get(8); }

   void Clear();
   mob_data& operator= (mob_data& source);
   void Write(ofstream& ofile);
   void Read(ifstream& ofile, short read_all);
   int isInProcNow() { return (cur_script && (cur_script->isInProgress())); }
   void addProcScript(const String& txt, MobScript* script_data);
   void finishedMobProc();
   void doScriptJump(int abs_offset);
   static int getInstanceCount() { return _cnt; }

   int togShopFlag(int flag) {
      if (proc_data)
         return proc_data->togShopFlag(flag);
      return -1;
   }

   int togTeachFlag(int flag) {
      if (proc_data)
         return proc_data->togTeachFlag(flag);
      return -1;
   }

}; //mob_data


///*********************** pc data  ***********************///
 
class pc_data {
protected:
   static int _cnt;

public:
   String password;
   String input;
   String output;
   String last_input;
   String rep_to;
   int descriptor;    //socket descriptor

   // When modifying this, modify PC_DATA_FLAGS_NAMES in const.cc
   bitfield pc_data_flags;
      // 0 frozen, 1 gagged, 2 has_imm_data, 3 cloaked, 4 tank_graph,
      // 5 using_client, 6 auto_exit, 7 !hassle, 8 brief, 9 autosplit, 
      // 10 do_prompt, 11 is_builder, 12 autoloot, 13 olc_redo 
      // 14 extra_info, 15 cr_behind, 16 do_carriage_return
      // 17 is_blocking_door, 18 can_det_magic, 19 detect_inventory
      // 20 show_vnums, 21 has_poofin_poofout_msg, 22 page_output
      // 23 in_page_break_mode, 24 wizchat, 25 has_colors, 26 use_color
      // 27 has_language_choice, 28 !show_mob_entry, 29 no_beep,
      // 30 is_remort, 31 has_sacrificed, 32 is_roleplaying,
      // 33 is_afk, 34 gold_only

   // The following is used for fixing nerf screwups on remorting, etc.
   short short_base_stats[PC_BASE_STATS + 1];
   // 0 str, 1 int, 2 con, 3 cha, 4 wis, 5 dex, 6 hit, 7 dam,
   // 8 ac, 9 attacks, 10 hp_max, 11 mana_max, 12 mov_max,
   // 13 dam_received_divisor, 14 dam_given_multiplier,
   // 15 heat_resis, 16 cold_resis, 17 elect_resis, 18 spell_resis,
   // 19 bare_hand_dice_count, 20 bare_hand_dice_sides, 21 hp_regen,
   // 22 mana_regen, 23 mov_regen

   // Is our PC wanted in any zones?
   int wanted_in[NUMBER_OF_ZONES +1];

   short birth_day; //day born
   short birth_year; //year born
   short rent_year; //year of last rent
   short rent_day; //day of last rent
   short lines_on_page; //used to page output
   int age;
   String host;
   short link_condition; //1 CON_PLAYING, 2 CON_LINKDEAD, 3 CON_LOGGING_IN
   critter* snoop_by; //prt to who is snooping this critter
   critter* snooping; //prt to who is being snooped
   immort_data* imm_data; //olc and other godly stuff, NULL in scrubs
   PcMode mode; //is used to determine parsing, olc, help, etc.
   int index;  //used for loging in...
   short hunger;
   short thirst;
   short drugged;
   short violence_timer;
   String prompt;
   String poofin;
   String poofout;
   short pk_count;
   short died_count;
   short quest_points;
   short idle_ticks;

   int last_login_time; //in seconds, since 1970 etc...
   int total_time_online; //in seconds

   Tree<int, int> skills_spells_known; 
   object* w_eye_obj; //wizard's eye object

   // Ansi color codes.
   String gos_str;
   String say_str;
   String yell_str;
   String tell_str;
   String desc_str;
   String obj_list_str;
   String mob_list_str;
   String dflt_str;
   String bk_str;
   String battle_str;
   String user1_str;
   String user2_str;
   String user3_str;
   String room_str;

   LanguageE preferred_language; //ie English, spanish...

   // Volatile variables, ie not saved to disk.
   int bug_num; //used when commenting on bugs or ideas.
   object* post_msg;
   String bug_comment;

   pc_data();
   pc_data(const pc_data& source);  //copy constructor
   ~pc_data();

   pc_data& operator= (const pc_data& source);

   int canBeBeeped() const { return (!(pc_data_flags.get(29))); }

   void Clear();
   void Write(ofstream& ofile);
   void Read(ifstream& ofile);
   static int getInstanceCount() { return _cnt; }

   int client;
};//class pc_data


///***************************** critter ******************************///

class critter {
protected:
   static int _cnt;

public:
   int spam_cnt;
   PtrList<String> names;
   String short_desc;
   String in_room_desc;
   String long_desc;
   
   bitfield crit_flags;
   // 0 can_see_inv, 1 using_light_src, 2 NULL, 3 is_flying,           
   // 4 have_boat, 5 can_climb,
   // 6 gos, 7 yell, 8 gratz, 9 auc, 
   // 10 shout, 11 say, 12 tell, 13 NULL, 14 is_paralyzed,
   // 15 is_perm_sleeped, 16 is_dual_wielding, 17 is_sneak,
   // 18 in_use, 19 can_dive, 20 spell_tested_yet, 21 is_blocked,
   // 22 is_hide, 23 is_tailing 24 !complete, 25 already_hurled,

   long long_data[MOB_LONG_DATA + 1]; 
   // 0 gold 
   // 1 exp;
   // 2 bank_gold
   
   short short_cur_stats[MOB_SHORT_CUR_STATS + 1];
   // 0 position   {0 stand, 1 sit, 2 rest, 3 sleep, 4 med,
   //               5 stun, 6 dead, 7 prone}
   // 1 str, 2 int, 3 con, 4 cha, 5 wis,
   // 6 dex, 7 hit, 8 dam, 9 ac, 10 attacks, 11 pause_count
   // 12 sex {0 female, 1 male, 2 neuter}
   // 13 class, {1 warrior, 2 sage, 3 wizard, 4 ranger, 5 thief,
   //            6 alchemist, 7 cleric, 8 bard}
   // 14 race, {1 human, 2 anitre, 4 darkling, 7 dwarf, 9 ogrue,
   //           11 elf, 14 sombrian}
   // 15 hp, 16 mana, 17 mov, 18 align,
   // 19 level, 20 hometowns, zone# mob is from,  21 wimpy
   // 22 practices, 23 hp_max,
   // 24 mana_max, 25 mov_max, 26 type_of_mob, { 0 pc, 1 smob 2 mob} 
   // 27 dam_received_divisor, 28 dam_given_multiplier,
   // 29 heat_resis, 30 cold_resis, 31 elect_resis, 
   // 32 spell_resis 33 religion,  34 Belongs to zone#.
   // 35 bare_hand_dice_count, 36 bare_hand_dice_sides,
   // 37 hp_regen, 38 mana_regen, 39 mov_regen
   // 40 guild, 41 naked_weight, 42 NULL
   
   int cur_stats[MOB_CUR_STATS + 1]; //array of length 
   // 0 vis_bit {1 dark, 2 invis, 4 hidden,
   //            8 imm_invis1, 16 imm_invis3, 32 link_dead_invis,
   //            64 imm_invis5, 128 imm_invis7, 256 imm_invis9,
   //            512 imm_invis10  1024 NORMAL
   // 1 see_bit {1 dark, 2 invis, 4 hidden,
   //            8 imm_invis1, 16 imm_invis3, 32 link_dead_detect
   //            64 imm_invis5, 128 imm_invis7, 256 imm_invis9,
   //            512 imm_invis10, 1024  NORMAL
   // 2 in_room,
   
   object* eq[MAX_EQ + 1];    //0 do_not_use
   //1 head, 2 neck, 3 neck, 4 around body, 5 arms, 
   //6 wrist, 7 wrist, 8 hands, 9 wielded, 10 held, 11 light,
   //12 body, 13 belt, 14 legs, 15 feet, 16 finger, 17 finger
   //18 shield 
   
   PtrList<stat_spell_cell> affected_by;  
   PtrList<stat_spell_cell> mini_affected_by; //decreased every battle
   //round
   PtrList<object> inv; // array of items in inventory
   
   pc_data* pc; //NULL if not a pc
   mob_data* mob; //null if not a mob
   
   critter* follower_of; //pointer to who one is following
   //NULL if following self
   critter* master;      //NULL if not charmed in some way
   PtrList<critter> pets; 
   PtrList<critter> followers;
   PtrList<critter> groupees; // List of everyone in your group.
   PtrList<critter> is_fighting;
   temp_crit_data* temp_crit;
   int mirrors;
   critter* possessing; //ptr to who we are possessing
   critter* possessed_by; //ptr to who is possessing us
   

   critter();
   critter(critter& source); //copy constructor
   ~critter();
   void Clear();
   critter& operator= (critter& source);
   
   int getCurWeight();
   int getMaxWeight();
   void dbRead(int crit_num, int pc_num, short read_all);
   void fileRead(ifstream& da_file, short read_all);
   void Write(ofstream& da_file);

   /** NOTE:  This may normalize the real value.
    */
   int getDamRecMod();

   /** For scripts, the script_targ is always *this.  The script_owner
    * will be null (for non-script specific stuff) and should be specified
    * if it relates to scripting of course.
    */
   int processInput(String& input, short do_sub, int script_driven, 
                    critter* c_script_owner = NULL,
                    room* r_script_owner = NULL, int was_ordered = FALSE);

   /**  Run a command after it has been parsed by processInput.  Also called
    * from the script_jump method.
    */
   int executeCommand(String* cooked_strs, int* cooked_ints,
                      int sanity, critter* c_script_owner,
                      room* r_script_owner, int do_sub, int was_ordered);

   int writeOutput();
   int readInput();
   static int createWithDescriptor(int socket);
   void startLogin();

   void checkForProc(String& cmd, String& arg1, critter& actor,
                     int targ, room& cur_room);

   object* findObjInEq(int i_th, const String& name, int see_bit,
                       room& rm, int& posn, int& count_sofar);
   
   int doLogOffActive();
   int doGoLinkdead();
   int doLogOffInactive();
   int doLogOffNewLogin();
   void unPossess();

   void notifyHasBeenHurled();
   int canBeHurled();
   int canBeBeeped() const { return pc && pc->canBeBeeped(); }

   int logOff();
   void doPrompt();
   void doLogin();
   void save();
   void emote(const char* msg);
   void pemote(const char* msg);

   void emote(CSelectorColl& includes, CSelectorColl& denies,
              CSentryE cs_entry, .../*Sprintf args*/);

   void doSuicide();

   void addProcScript(const String& txt, MobScript* script_data);
   void remember(critter& pc);
   void trackToKill(critter& vict, int& is_dead);
   int travelToRoom(int targ_room_num, int num_steps, int& is_dead);
   int doesRemember(critter& pc);

   int doesOwnObject(object& obj);
   int doesOwnRoom(room& rm);
   int doesOwnCritter(critter& foo);
   int doesOwnDoor(door& dr);
   int doesOwnDoor(door_data& dd);

   int isCloaked() { return (pc && PC_FLAGS.get(3)); }
   int isInProcNow() { return (mob && mob->isInProcNow()); }
   int isInBattle() { return !(is_fighting.isEmpty()); }

   int haveObjNumbered(int count, int obj_num);

   /** If return is > 0, self is more powerful, if less than 0,
    * b is more powerful...  The larger/smaller the value, the more
    * extreme the difference.
    */
   int compareTo(critter& pc);

   int isWeakerThan(critter& pc) { return (compareTo(pc) < 0); }
   int isStrongerThan(critter& pc) { return (compareTo(pc) > 0); }
   int isSessile() const { return mob && mob->mob_data_flags.get(7); }
   
   void doHuntProc(int steps, int& is_dead); //found in batl_prc.cc
   void finishedMobProc() { if (mob) mob->finishedMobProc();}
   int getIdNum() const;
   int getImmLevel() const;
   int getHomeRoom() { if (mob) return mob->home_room; return 0; }
   int shouldBeHoming();
   int showTime();

   // Where the mob is from, not what area it belongs to.
   int getHomeTown() const { return short_cur_stats[20]; }
   
   // What area it belongs to, ie what file it's stored in.
   int getNativeZoneNum() const { return short_cur_stats[34]; }

   int getAge() const;
   int getXpToNextLevel() const;
   int getVisBit() const { return cur_stats[0]; }
   int getSeeBit() const { return cur_stats[1]; }
   String* getTrackingTarget();

   const String* getName() const { return getName(~0); }
   const String* getName(int see_bit) const ;
   const String* getName(critter& looker) { return getName(looker.getSeeBit()); }
   const String* getLongName() const { return getName(); }
   const String* getLongName(int see_bit) const { return getName(see_bit); }
   const String* getShortName() const { return getShortName(~0); }
   const String* getShortName(int see_bit) const ;
   int doBecomeNonPet(); //misc2.cc
   int doUngroup(int i_th, const String* vict); //command2.cc
   int doFollow(critter& pc, int do_msgs = TRUE);
   int doUnShield();

   critter* getFollowerOf() const { return follower_of; }
   int getNakedWeight() const;
   PcMode getMode() { return pc->mode; }
   int getDescriptor() { return pc->descriptor; }
   int getLinkCond() { return pc->link_condition; }
   room* getCurRoom();
   int getCurRoomNum();
   int getCurZoneNum(); //what are we in right now
   String* getInput() { return &(pc->input); }
   int getCharisma() const { return short_cur_stats[4]; }
   int getHP() const { return short_cur_stats[15]; }
   int getHP_MAX() const { return short_cur_stats[23]; }
   int getDEX(char include_modifiers);
   int getWIS(char include_modifiers);
   int getCHA(char include_modifiers);
   int getBHDC(char include_modifiers);
   int getBHDS(char include_modifiers);
   int getDAM(char include_modifiers);
   int getMana() const { return short_cur_stats[16]; }
   int getManaMax() const { return short_cur_stats[24]; }
   int getMov() const { return short_cur_stats[17]; }
   int getMovMax() const { return short_cur_stats[25]; }
   int getAlignment() const { return short_cur_stats[18]; }
   LanguageE getLanguageChoice() const ;
   int getWimpy() const { return short_cur_stats[21]; }

   const ScriptCmd* getNextScriptCmd() {
      if (mob) {
         return mob->cur_script->getNextCommand();
      }
      return NULL;
   }

   int getPause() const { return short_cur_stats[11]; }
   void setPause(int i) { short_cur_stats[11] = i; }

   void setWimpy(int i) { short_cur_stats[21] = i; }

   int getPosn() const { return short_cur_stats[0]; }
   void setPosn(int i) {
      if ((i >= POS_LOW_VAL) && (i < POS_HIGH_VAL)) {
         short_cur_stats[0] = i;
      }
   }//setPosn

   const char* getPosnStr(critter& for_this_pc);

   critter* getFirstFighting();
   String& getPoofin();
   String& getPoofout();
   String* getHostName() const;

   // Where the mob is from, not what area it belongs to.
   void setHomeTown(int i) { short_cur_stats[20] = i; }

   // What area the mob belongs to, ie stored in this file.
   void setNativeZoneNum(int i) { short_cur_stats[34] = i; }


   void setDescriptor(int i) { pc->descriptor = i; }
   void setNakedWeight(int i) { short_cur_stats[41] = i; }
   void setIdNum(int i);
   void setDoPrompt(int i) { PC_FLAGS.set(10, i); }
   void setHP(int i);
   void setMana(int i);
   void setMov(int i);
   void setHP_MAX(int i);
   void setManaMax(int i);
   void setMovMax(int i);
   void setMode(PcMode val);
   void setImmLevel(int i);
   void setCurRoomNum(int i); //assign in_roomtoo
   void setTrackingTarget(const String& targ_name);
   void setPoofin(const char* pin);
   void setPoofout(const char* pout);
   void setComplete();
   void setNotComplete();
   void setNoClient();
   void setHasSacrificed(int i) { if (pc) PC_FLAGS.set(31, i); }
   void setIsRemort(int i) { if (pc) PC_FLAGS.set(30, i); }
   void setViolenceTimer() { pc->violence_timer = 8; }

   int isRemort() { return pc && PC_FLAGS.get(30); }
   int hasSacrificed() { return pc && PC_FLAGS.get(31); }   
   int hasAI();
   int shouldDoPrompt() { return pc && PC_FLAGS.get(10); }
   int shouldDoTank() { return pc && PC_FLAGS.get(4); }
   int canDetectMagic() { return (mob || (pc && PC_FLAGS.get(18))); }
   int canDetect(const critter& other) const;
   int canDive() const { return CRIT_FLAGS.get(19); }
   int isUsingClient() const ;
   bool setClient(int which);
   int whichClient();
   int isUsingColor();
   int isBrief();
   int shouldSeeInventory();
   int shouldShowVnums();
   int shouldDoAutoExits();
   int shouldDoPoofin();
   int shouldDoPoofout();
   int isNoHassle();
   int isSneaking() const;
   int isHiding() const;
   int isInvis() const;
   int isHungry() { return (HUNGER == 0); }
   int isThirsty() { return (THIRST == 0); }
   int isGettingHungry() { return (HUNGER <= 10 && HUNGER >= 0); }
   int isGettingThirsty() { return (THIRST <= 10 && THIRST >= 0); }
   int isDrugged() { return (DRUGGED > 0); }
   int isRolePlaying() { return pc && PC_FLAGS.get(32); }
   int isAFK() { return pc && PC_FLAGS.get(33); }
   int isGoldOnly() { return pc && PC_FLAGS.get(34); }
   int isViolent() { return (pc->violence_timer > 0); }
   
   int isMob() const { return (CRITTER_TYPE == 2); }
   int isNPC() const { return (isMob() || isSmob()); }
   int isSmob() const { return (CRITTER_TYPE == 1); }
   int isPc() const { return (CRITTER_TYPE == 0); }
   int isNpc() const { return !isPc(); }
   int isTracking() const { return mob && mob->proc_data &&
                               mob->proc_data->temp_proc &&
                               mob->proc_data->temp_proc->tracking.Strlen(); }
   int isGroupLeader();
   int isInGroupWith(critter* v);
   int makeGroupSane();

   int getBenevolence() const;
   int isSentinel() const;

   String* getSayColor();
   String* getYellColor();
   String* getTellColor();
   String* getGossipColor();
   String* getDescColor();
   String* getObjListColor();
   String* getMobListColor();
   String* getDefaultColor(); //for foreground
   String* getBackGroundColor();
   String* getBattleColor();
   String* getRoomColor();

   int isNamed(const String& str) const ;
   int isGagged();
   int isFrozen();
   int isImmort() const;
   int isFlying();
   int isTailing() { return CRIT_FLAGS.get(23); }
   int isDualWielding() { return crit_flags.get(16); }
   int canClimb();
   int isParalyzed();
   int isNotComplete() const ;
   int isWizchat() const;
   int isInUse() const;
   int isFemale() const { return SEX == SEX_FEMALE; }
   int isMale() const { return SEX == SEX_MALE; }
   int isNeuter() const { return SEX == SEX_NEUTER; }
   int isBard() const { return CLASS == BARD; }
   int isWarrior()  const { return CLASS == WARRIOR; }
   int isSage()  const { return CLASS == SAGE; }
   int isWizard() const  { return CLASS == WIZARD; }
   int isRanger() const  { return CLASS == RANGER; }
   int isAlchemist() const  { return CLASS == ALCHEMIST; }
   int isCleric() const  { return CLASS == CLERIC; }
   int isThief() const  { return CLASS == THIEF; }
   int isNecromancer() const { return CLASS == NECROMANCER; }
   int isFighting() const  { return !is_fighting.isEmpty(); }
   int isFighting(critter& da_pc) { return is_fighting.haveData(&da_pc); }
   int isGuard() { return MOB_FLAGS.get(MOBFLAG_IS_GUARD); }
   int hasBoat() { return crit_flags.get(CRITFLAG_HAS_BOAT); }
   
   int isResting() const { return POS == POS_REST; }
   int isStanding() const { return POS == POS_STAND; }
   int isSitting() const { return POS == POS_SIT; }
   int isSleeping() const { return POS == POS_SLEEP; }
   int isStunned() const { return POS == POS_STUN; }
   int isMeditating() const { return POS == POS_MED; }
   int isPossessed() const { return (!possessed_by); }
   int posnNonTalkative() const { return ((POS > POS_REST) && (POS != POS_PRONE)); }
   int isEdible() const { return mob && MOB_FLAGS.get(5); }
   
   int isHuman() { return RACE ==  HUMAN; }
   int isAnitre() { return ((RACE == ANITRE) || (RACE == AVINTRE)); }
   int isDarkling() { return ((RACE == DARKLING) || (RACE == DROW)); }
   int isDragon() { return RACE == DRAGON; }
   int isDwarf() { return ((RACE == DWARF) || (RACE == IRONCLAD)); }
   int isOgrue() { return ((RACE == OGRUE) || (RACE == ROCKTROLL)); }
   int isElf() { return ((RACE == ELF) || (RACE == FAERIE)); }
   int isSombrian() { return (RACE == SOMBRIAN); }
   int isUndead() { return RACE == UNDEAD; }
   int isAnimal() { return RACE == ANIMAL; }
   int isMonster() { return RACE == MONSTER; }
   int isCharmed() { return master != NULL; }
   int isWanderer() { return mob && MOB_FLAGS.get(2); }
   int isBanker() { return mob && MOB_FLAGS.get(6); }
   int isOpen(int cur_military_time, int do_msg, critter& pc) const;

   int isTeacher();
   int isShopKeeper();
   int isPlayerShopKeeper();
   int isManagedBy(critter& pc);
   stat_spell_cell* isAffectedBy(int spell_num);
   // return current value of cur_in_game after operation
   int getCurInGame();
   int setCurInGame(int i);
   int decrementCurInGame();
   int incrementCurInGame();
   int getMaxInGame();
   int setMaxInGame(int i);
   void clearIGFlags(); //clear the Inc/Dec Cur In Game checks

   int getLevel() const { return LEVEL; }
   int getClass() const { return CLASS; }
   int getRace() const { return RACE; }
   void setLevel(int i) { LEVEL = i; }
   void setRace(int i) { RACE = i; }
   void setClass(int i) { CLASS = i; }

   /**  Translates an asci string (like HP, MANA, MOV, ALIGN, etc)
    * and returns the integer value.  Returns 0 if it can't
    * resolve the field.
    */
   int getFieldValue(const char* field) const;
 
   /**  Get the attribute att, from this critter, used for
    * scripting mostly. */
   int getAttribute(String& att);

   int getLinesOnPage();
   void setLinesOnPage(int lines);

   void releasePageBreak() { if (pc) PC_FLAGS.turn_off(23); }
   int isInPageBreak() const { return (pc && PC_FLAGS.get(23)); }

   void show(const char* msg, hilite_type hl_type = HL_DEF);
   void show(CSentryE, hilite_type hl_type = HL_DEF); //Pick language of choice, if available.

   object* loseInv(object* obj); //returns the object removed. (or NULL)
   void loseObjectFromGame(object* obj);

   PtrList<object>& getInv() { return inv; }

   void gainInv(object* obj);

   void doLeaveRoom();
   int doEnterRoom(room* new_room);
   void doRemoveFromBattle();
   void doGoToRoom(int dest_room, const char* from_dir, door* by_door,
                   int& is_dead, int cur_room, int sanity,
                   int do_msgs = TRUE);
   void doScriptJump(int abs_index);
   int insertNewScript(MobScript* script);

   void breakEarthMeld();
   void doDie(int do_msg, int& show_tags, int leave_corpse = TRUE);

   // Assumes this is a shop-keeper.
   int findItemSalePrice(object& item, critter& pc);
   int findItemBuyPrice(object& item, critter& pc);

   void transferShopDataTo(critter& sink);
   void valueRem(int idx, critter& manager);
   void valueAdd(object& obj, critter& manager);
   void valueList(int i_th, const String* targ, critter& manager);
   void valueSet(int val_idx, int sell_val, int buy_val, critter& manager);

   int withdrawCoins(int count, critter& banker); //do messages
   int depositCoins(int count, critter& banker); //do messages
   int balanceCoins(critter& banker);

   /** Can fail if cnt is bad, does all messages. */
   int doDropCoins(int cnt);
   int doPutCoins(int cnt, object* bag);
   void split(int amt, int do_msg);

   /** Assume we are removing this object at this posn. */
   void checkLight(object* obj = NULL, int posn = -1);
   void drunkifyMsg(String& msg);
   void checkForBattle(room& rm);
   static int getInstanceCount() { return _cnt; }
};//class critter
 

#endif
