// $Id: critter.h,v 1.30 1999/07/30 06:42:23 greear Exp $
// $Revision: 1.30 $  $Author: greear $ $Date: 1999/07/30 06:42:23 $

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
   
   immort_data();				// default constructor
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
   // 7 Cleric, 8 Bard
   
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

   short markup;       // %markup
   short buy_percentage;       // buy_percentage
   short open_time;
   short close_time;
   bitfield shop_data_flags; //0 buy_0, 1 sell_0, 2 offer_0
   // 3 Player-run shopkeeper
   // 40-73 type to trade flags, same as obj_flags
   //
   ObjectContainer perm_inv; //holds perm inventory

   // Holds extra info for player run shops.
   PtrList<PlayerShopData> ps_data_list;
   String manager;

public:

   shop_data();
   shop_data(shop_data& source);  //copy constructor
   shop_data& operator=(shop_data& src);
   virtual ~shop_data();

   int isPlayerRun() const { return shop_data_flags.get(3); }

   void Clear();
   void Read(ifstream& da_file, short read_all);
   void Write(ofstream& da_file) const ;

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



///***************************** critter ******************************///

class critter : public Entity {
private:
   static int _cnt;

protected:
   LStringCollection short_desc;
   LStringCollection in_room_desc;
   
   bitfield crit_flags;
   // 0 can_see_inv, 1 using_light_src, 2 NULL, 3 is_flying,           
   // 4 have_boat, 5 can_climb,
   // 6 gos, 7 yell, 8 gratz, 9 auc, 
   // 10 shout, 11 say, 12 tell, 13 wiznet, 14 is_paralyzed,
   // 15 is_perm_sleeped, 16 is_dual_wielding, 17 is_sneak,
   // 18 in_use, 19 can_dive, 20 spell_tested_yet, 21 is_blocked,
   // 22 is_hide, 23 is_tailing 24 !complete, 25 already_hurled

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
   
   PtrList<SpellDuration> mini_affected_by; //decreased every battle
   //round
   ObjectContainer inv; // collection of items in inventory
   
   critter* follower_of; //pointer to who one is following
   //NULL if following self
   critter* master;      //NULL if not charmed in some way
   ObjectContainer pets; 
   ObjectContainer followers;
   ObjectContainer groupees;
   ObjectContainer is_fighting;
   temp_crit_data* temp_crit;
   int mirrors;
   critter* possessing; //ptr to who we are possessing
   critter* possessed_by; //ptr to who is possessing us

public:
   critter();
   critter(critter& source); //copy constructor
   virtual ~critter();
   void Clear();
   critter& operator= (critter& source);
   
   int getCurWeight();
   int getMaxWeight();
   void Read(ifstream& da_file, short read_all);
   void Write(ofstream& da_file);

   int writeOutput();
   int readInput();
   static int createWithDescriptor(int socket);
   void startLogin();

   void checkForProc(String& cmd, String& arg1, critter& actor,
                     int targ, room& cur_room);

   object* findObjInEq(int i_th, const String& name, int see_bit,
                       room& rm, int& posn);
   
   int doLogOffActive();
   int doGoLinkdead();
   int doLogOffInactive();
   int doLogOffNewLogin();
   void unPossess();

   void notifyHasBeenHurled();
   int canBeHurled();

   int logOff();
   void doPrompt();
   void doLogin();
   void save();
   void emote(const char* msg);

   void emote(CSelectorColl& includes, CSelectorColl& denies,
              CSentryE cs_entry, .../*Sprintf args*/);

   void doSuicide();

   void remember(critter& pc);
   void trackToKill(critter& vict, int& is_dead);
   int travelToRoom(int targ_room_num, int num_steps, int& is_dead);
   int doesRemember(critter& pc);

   int doesOwnObject(object& obj);
   int doesOwnRoom(room& rm);
   int doesOwnCritter(critter& foo);
   int doesOwnDoor(door& dr);
   int doesOwnDoor(door_data& dd);

   int isInBattle() { return !(is_fighting.isEmpty()); }

   int haveObjNumbered(int count, int obj_num);

   /** If return is > 0, self is more powerful, if less than 0,
    * b is more powerful...  The larger/smaller the value, the more
    * extreme the difference.
    */
   int compareTo(critter& pc);

   int isWeakerThan(critter& pc) { return (compareTo(pc) < 0); }
   int isStrongerThan(critter& pc) { return (compareTo(pc) > 0); }
   
   void doHuntProc(int steps, int& is_dead); //found in batl_prc.cc
   int getIdNum() const;
   int getImmLevel();

   // Where the mob is from, not what area it belongs to.
   int getHomeTown() const { return short_cur_stats[20]; }

   // What area it belongs to, ie what file it's stored in.
   int getNativeZoneNum() const { return short_cur_stats[34]; }

   int getAge() const;
   int getXpToNextLevel() const;
   int getSeeBit() const { return cur_stats[1]; }
   String* getTrackingTarget();

   int doBecomeNonPet(); //misc2.cc
   int doUngroup(int i_th, const String* vict); //command2.cc
   int doFollow(critter& pc, int do_msgs = TRUE);
   int doUnShield();

   critter* getFollowerOf() const { return follower_of; }
   int getNakedWeight() const;
   room* getCurRoom();
   int getCurRoomNum();
   int getCurZoneNum(); //what are we in right now
   int getHP() const { return short_cur_stats[15]; }
   int getHP_MAX() const { return short_cur_stats[23]; }
   int getMana() const { return short_cur_stats[16]; }
   int getManaMax() const { return short_cur_stats[24]; }
   int getMov() const { return short_cur_stats[17]; }
   int getMovMax() const { return short_cur_stats[25]; }
   int getAlignment() const { return short_cur_stats[18]; }
   LanguageE getLanguageChoice() const ;

   int getPosn() const { return short_cur_stats[0]; }
   void setPosn(int i) {
      if ((i >= POS_LOW_VAL) && (i < POS_HIGH_VAL)) {
         short_cur_stats[0] = i;
      }
   }//setPosn

   const char* getPosnStr(critter& for_this_pc);

   critter* getFirstFighting();

   // Where the mob is from, not what area it belongs to.
   void setHomeTown(int i) { short_cur_stats[20] = i; }

   // What area the mob belongs to, ie stored in this file.
   void setNativeZoneNum(int i) { short_cur_stats[34] = i; }


   void setNakedWeight(int i) { short_cur_stats[41] = i; }
   void setIdNum(int i);
   void setHP(int i);
   void setMana(int i);
   void setMov(int i);
   void setHP_MAX(int i);
   void setMode(PcMode val);
   void setImmLevel(int i);
   void setTrackingTarget(const String& targ_name);
   void setPoofin(const char* pin);
   void setPoofout(const char* pout);
   void setComplete();
   void setNotComplete();
   void setNoClient();

   int hasAI();
   virtual int canDetectMagic() { return TRUE; }
   int canDetect(const critter& other) const;
   int canDive() const { return CRIT_FLAGS.get(19); }
   int isUsingClient() const ;
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
   
   virtual int isMob() const { return FALSE; }
   virtual int isNPC() const { return FALSE; }
   virtual int isPc() const { return FALSE; }
   virtual int isModified() const { return FALSE; }
   virtual int isNpc() const { return FALSE; }

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

   int isNamed(const String& str) const ;
   int isGagged();
   int isFrozen();
   int isImmort();
   int isFlying();
   int isTailing() { return CRIT_FLAGS.get(23); }
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
   int isFighting() const  { return !is_fighting.isEmpty(); }
   int isFighting(critter& da_pc) { return is_fighting.haveData(&da_pc); }

   int isStanding() const { return POS == POS_STAND; }
   int isSleeping() const { return POS == POS_SLEEP; }
   int isStunned() const { return POS == POS_STUN; }
   int isMeditating() const { return POS == POS_MED; }
   int isPossessed() const { return (!possessed_by); }

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

   int isTeacher();
   int isShopKeeper();
   int isPlayerShopKeeper();
   int isManagedBy(critter& pc);

   // return current value of cur_in_game after operation
   int getCurInGame();
   int setCurInGame(int i);
   int decrementCurInGame();
   int incrementCurInGame();
   int getMaxInGame();
   int setMaxInGame(int i);
   void clearIGFlags(); //clear the Inc/Dec Cur In Game checks

   int getLevel() { return LEVEL; }
   int getClass() const { return CLASS; }

   /**  Translates an asci string (like HP, MANA, MOV, ALIGN, etc)
    * and returns the integer value.  Returns 0 if it can't
    * resolve the field.
    */
   int getFieldValue(const char* field) const;
 
   /**  Get the attribute att, from this critter, used for
    * scripting mostly. */
   int getAttribute(String& att);

   void show(const char* msg);
   void show(CSentryE); //Pick language of choice, if available.

   void loseObjectFromGame(object* obj);

   Entity* loseObject(Entity* obj); //returns the object removed. (or NULL)
   void gainObject(Entity* obj);
   int getCurRoomNum() const;

   void doLeaveRoom();
   int doEnterRoom(room* new_room);
   void doRemoveFromBattle();
   void doGoToRoom(int dest_room, const char* from_dir, door* by_door,
                   int& is_dead, int cur_room, int sanity,
                   int do_msgs = TRUE);

   void breakEarthMeld();

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

   /** Assume we are removing this object at this posn. */
   void checkLight(object* obj = NULL, int posn = -1);
   void drunkifyMsg(String& msg);
   void checkForBattle(room& rm);
   static int getInstanceCount() { return _cnt; }
};//class critter


///*********************** mob data ***************************///
 
/** A computer controlled character. */
class Mobile : public critter, public Scriptable {
private:
   static int _cnt;
protected:
   int cur_in_game;
   int max_in_game;

   int mob_num;
   int tmp_num; //not used now

   // When modifying this, modify MOB_DATA_FLAGS_NAMES in const.cc
   // Also check the read method for Mobile, and the tog_mflag
   // methods to make sure the flag is not reset or ignored.
   bitfield mob_data_flags; //all the proc bits
     // 0 has_proc_data, 1 scavenge, 2 wander, 3 should_do_procs,
     // 4 need_resetting, 5 edible_corpse, 6 is_banker,
     // 7 sessile (won't track out of current room)
     // 8 !homing (won't come home after tracking),
     // 9 NULL, 10 NULL, 11 NULL, 12 NULL, 13 NULL, 14 NULL, 15 NULL,
     // 16 has_skin, 17 has_mob_script
     //

   int skin_num;
   int is_modified;

   int home_room; /* set when first placed in a room. */
public:

   Mobile();
   Mobile(Mobile& source);  //copy constructor
   ~Mobile();

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
   int getHomeRoom() { return home_room; }

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
   int isNoHoming() const { return mob_data_flags.get(8); }
   int isEdible() const { return mob_data_flags.get(5); }
   int isSessile() const { return mob_data_flags.get(7); }
   int isWanderer() { return mob_data_flags.get(2); }
   int isBanker() { return mob_data_flags.get(6); }
   int shouldBeHoming();

   void Clear();
   Mobile& operator= (Mobile& source);
   void Write(ofstream& ofile);
   void Read(ifstream& ofile, short read_all);
   void addProcScript(const String& txt, MobScript* script_data);
   void finishedMobProc();
   void doScriptJump(int abs_offset);
   static int getInstanceCount() { return _cnt; }

   virtual int isNPC() const { return TRUE; }
   virtual int isModified() const { return is_modified; }

}; //Mobile


/**  Holds data for the more active/inteligent mobiles, including shopkeepers
 * teachers and AI mobs.
 */
class ProcMobile : public Mobile {
protected:
   short skill_violence;
   short benevolence;
   short defensiveness;
   short bad_assedness;
   short social_awareness;
   static int _cnt;

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
   
public:
   
   ProcMobile();
   ProcMobile(const ProcMobile& source);   //copy constructor
   ~ProcMobile();

   void Clear();
   void Read(ifstream& da_file, short read_all);
   void Write(ofstream& da_file);
   ProcMobile& operator=(const ProcMobile& source);

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
   int isOpen(int cur_military_time, int do_msg, critter& pc) const;
   int isTracking() const { return (temp_proc &&
                                    temp_proc->tracking.Strlen()); }
   
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

}; //ProcMobile


///*********************** pc data  ***********************/// 
/** A player controlled character. */
class Pc : public critter {
private:
   static int _cnt;

protected:
   String password;
   String input;
   String output;
   String last_input;
   String rep_to;
   int descriptor;    //socket descriptor

   // When modifying this, modify PC_DATA_FLAGS_NAMES in const.cc
   bitfield pc_flags;
      // 0 frozen, 1 gagged, 2 has_imm_data, 3 cloaked, 4 tank_graph,
      // 5 using_client, 6 auto_exit, 7 !hassle, 8 brief, 9 autosplit, 
      // 10 do_prompt, 11 is_builder, 12 autoloot, 13 olc_redo 
      // 14 extra_info, 15 cr_behind, 16 do_carriage_return
      // 17 is_blocking_door, 18 can_det_magic, 19 detect_inventory
      // 20 show_vnums, 21 has_poofin_poofout_msg, 22 page_output
      // 23 in_page_break_mode, 24 !wizchat, 25 has_colors, 26 use_color
      // 27 has_language_choice, 28 !show_mob_entry, 29 no_beep

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

   LanguageE preferred_language; //ie English, spanish...

   // Volatile variables, ie not saved to disk.
   int bug_num; //used when commenting on bugs or ideas.
   object* post_msg;
   String bug_comment;
public:

   Pc();
   Pc(const Pc& source);  //copy constructor
   ~Pc();

   Pc& operator= (const Pc& source);

   void setDoPrompt(int i) { pc_flags.set(10, i); }

   int canBeBeeped() const { return (!(pc_flags.get(29))); }
   virtual int canDetectMagic() { return pc_flags.get(18); }
   int isCloaked() { return pc_flags.get(3); }
   virtual int isPc() const { return TRUE; }
   int shouldDoPrompt() { return pc_flags.get(10); }
   int shouldDoTank() { return pc_flags.get(4); }

   void Clear();
   void Write(ofstream& ofile);
   void Read(ifstream& ofile);
   void setDescriptor(int i) { descriptor = i; }

   int getLinesOnPage() { return lines_on_page; }
   void setLinesOnPage(int lines) { lines_on_page = lines; }
   void releasePageBreak() { pc_flags.turn_off(23); }
   int isInPageBreak() const { return pc_flags.get(23); }

   String& getPoofin();
   String& getPoofout();
   String* getHostName() const;

   void setWizardEyeObject(object* o) { w_eye_obj = o; }
   object* getWizardEyeObject() { return w_eye_obj; }

   String* getInput() { return &(input); }
   PcMode getMode() { return mode; }
   int getDescriptor() { return descriptor; }
   int getLinkCond() { return link_condition; }

   static int getInstanceCount() { return _cnt; }
};//class Pc



#endif
