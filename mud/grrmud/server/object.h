// $Id: object.h,v 1.19 1999/07/30 06:42:23 greear Exp $
// $Revision: 1.19 $  $Author: greear $ $Date: 1999/07/30 06:42:23 $

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

///************************  object.h  ********************************///

#ifndef GRRMUD_OBJECT_INCLUDE_H
#define GRRMUD_OBJECT_INCLUDE_H

#include "const.h"
#include <bitfield.h>
#include <string2.h>
#include "classes.h"
#include "critter.h"

class object;
class Pc;
 
enum ComponentEnum {
   CONSTRUCTION = 0,
   CONCOCTION = 1
};//ComponentEnum


///********************  obj construct data  **********************///

class ConstructData {
private:
   static int _cnt;

protected:
   int target_object; //obj_num of object it is component of
   /* companion components */
   int item1;
   int item2;
   int item3;
   int item4;
   int item5;
   
public:
   ConstructData();
   ConstructData(const ConstructData& source); //copy constuctr
   ~ConstructData() { _cnt--; }

   //void Clear();
   void Read(ifstream& da_file);
   void Write(ofstream& da_file) const;
   void makeComponent(int targ, int comp1, int comp2, int comp3, int comp4,
                      int comp5);
   int getTarget() const { return target_object; }
   int getItem1() const { return item1; }
   int getItem2() const { return item2; }
   int getItem3() const { return item3; }
   int getItem4() const { return item4; }
   int getItem5() const { return item5; }

   static int getInstanceCount() { return _cnt; }
}; //ConstructData



///*********************  object  ******************************///

class object : public Entity {
private:
   static int _cnt;

protected:
   int is_modified; //takes the place of what IN_LIST used to do for us.

   LString short_desc;
   LString in_room_desc;
   
   bitfield obj_flags; 
	   // 0 no_rest,  1 !evil, 2 !neutral, 3 !good, 
	   // 4 !donate, 5 !drop, 6 !remove, 7 !mortal, 8 !imm,
	   // 9 !demi, 10 in_use, 11 !warrior, 12 !sage,
	   // 13 !wizard, 14 !ranger, 15 !thief, 16 !alchemist, 
	   // 17 !cleric, 18 !bard, 19 !mob, 20 !pc
 
	   // 21 not worn, 22 head, 23 neck, 24 neck, 25 around body, 26 arms, 
	   // 27 wrist1, 28 wrist2, 29 hands, 30 wielded, 31 held, 32 light,
	   // 33 body, 34 belt, 35 legs, 36 feet, 37 finger1, 
	   // 38 finger2, 39 shield
	   // 40 not a weapon, 41 slash, 42 smash, 43 pierce,44 whip, 
	   // 45 needs_ammo,  46 dart_thrower, 47 bow, 48 dart, 49 arrow 
	   // 50 junk, 51 wand, 52 potion, 53 scroll, 54 container,
	   // 55 coins, 56 armor, 57 weapon, 58 light_source,
	   // 59 canteen, 60 liquid, 61 food, 62 boat, 63 has_spec_proc_data,
           // 64 toolbox, 65 cauldron, 66 pen, 67 construct_component
           // 68 concoct_component, 69 parchment, 70 needs_resetting
           // 71 !complete, 72 herb, 73 vend_machine, 74 bulletin_board
           // 75 is_butcherable, 76 has_obj_script
   
   short extras[OBJ_MAX_EXTRAS + 1]; 
		// 0 charges, light_count, 1 rechargeable, (wands), 2 %load,
		// 3 cur_in_game, 4 max_in_game, 
		// 5 weight, 6 dice_sides, 7 dice_count, 8 level
	
   int cur_stats[OBJ_CUR_STATS + 1]; 
	   // 0 vis_bit;
	   // 1 price; 
	   // 2 item_num;
	   // 3 in_zone  //as in olc purposes, its origin

   PtrList<StatBonus> stat_affects;  

public:
   object (object& source); //copy constructor
   object();			     //default constructor
   virtual ~object();                     //destructor
   object& operator= (object& source);

   void Clear();
   void Read(ifstream& da_file, short read_all);
   void Write(ofstream& da_file);
   int isMagic();

   virtual Entity* loseObject(Entity* le);
   virtual void gainObject(Entity* le);
   virtual int getCurRoomNum();

   int getLevel() const { return extras[8]; }

   virtual int getCurWeight() const { return getEmptyWeight(); }
   int getEmptyWeight() const { return extras[5]; }
   void setEmptyWeight(int wt) { extras[5] = wt; }
   virtual int getEntityCount(LEtypeE type, int id_num, int sanity) { return 0; }
   
   int doGoToRoom(int dest_room, const char* from_dir, door* by_door,
                  int cur_room, int sanity);

   int getZoneNum() const;
   int getDefaultPrice() { return cur_stats[1]; }
   
   int isPotion() const;
   int isFood() const ;
   int isInUse() const ;
   int isNotComplete() const ;
   int isLightSource() const;
   int isLit() const;
   int isBulletinBoard() const;
   int isScroll() const;
   int isCoins() const { return OBJ_FLAGS.get(55); }
   int isHerb() const;
   int isPaused() const { return pause > 0; }
   int isModified() const { return is_modified; } //ie isSOBJ
   int hasScript() const { return obj_flags.get(76); }
   int isLiquid() const;
   int isCanteen() const;

   void setModified(int mod) { is_modified = mod; }
   void setComplete();
   void setIncomplete();
   void setButcherable(int val);

   // These return the current value after the operation
   int setCurInGame(int i);
   int getCurInGame();
   int decrementCurInGame();
   int incrementCurInGame();
   int getMaxInGame();
   int getDiceCnt() const { return extras[6]; }
   int getDiceSides() const { return extras[7]; }

   static int getInstanceCount() { return _cnt; }

   int getObjCountByNumber(LEtypeE type, int onum, int sanity);

   /* Found in obj_cmds.cc */
   /**  Move all in room out some door.  Does no checks, just puts em
    * through the door, even if it's closed??!!
    */
   int move_all(int i_th, const String* dir, room& rm);
   int omove_all(int i_th, const String* dir, room& rm);

   /**  Move all in room out some door.  Does no checks, just puts em
    * through the door, even if it's closed!!??
    */
   int move(int i_th, const String* pc, int j_th, const String* dir, room& rm);
   int omove(int i_th, const String* obj, int j_th, const String* dir, room& rm);

   /** Echo message into the room in this direction */
   int neighbor_echo(int i_th, const String* dir, const String& buf, room& rm);

   int tell(int i, const String* name, String& msg, room& rm);
   int do_tell(const char* msg, critter& targ); /* room tells targ */

   int com_recho(const String* msg, room& rm);
   int com_zecho(const String* msg, room& rm);
   int wizchat(const char* message, room& rm);
   int obj_pause(int ticks);

   void checkForProc(String& cmd, String& arg1, critter& actor,
                     int targ, room& cur_room);

   virtual LEtypeE getEntityType() const { return LE_OBJECT; }
}; // class object


/** Special Procedure extensions to an object. */
class ProcObject : public object, public Scriptable {
private:
   static int _cnt;

protected:
   bitfield proc_flags; // 0 consume_teleport, 
   // 1 has_construct_data, 2 has_skin, 3 consume_poison,
   // 4 NULL, 5 NULL, 6 NULL,
   // 7 NULL, 8 NULL, 9 NULL,
   // 10 casts_spells
   //
   ConstructData* construct_data;
   object* skin_ptr;
   Pc* w_eye_owner;
   PtrList<SpellDuration> casts_these_spells;

public:
   ProcObject();
   ProcObject(const ProcObject& source);  //copy constructor
   ~ProcObject();

   ProcObject& operator=(const ProcObject& source);

   void makeComponent(int targ, int comp1, int comp2, int comp3, int comp4,
                      int comp5, ComponentEnum con_type);

   int castsSpells() { return proc_flags.get(10); }
   int consumeTeleport() { return proc_flags.get(0); }
   int hasConstructData() { return proc_flags.get(1); }
   int hasSkin() { return proc_flags.get(2); }
   int consumePoison() { return proc_flags.get(3); }
   object* getSkin() { return skin_ptr; }
   void setSkin(object* sk) { skin_ptr = sk; }
   Pc* getWizardEyeOwner() { return w_eye_owner; }
   void setWizardEyeOwner(Pc* cr) { w_eye_owner = cr; }

   virtual int insertNewScript(GenScript* original);

   bitfield& getFlags() { return proc_flags; }
   PtrList<SpellDuration>& getSpellsCast() { return casts_these_spells; }

   void Clear();
   void Read(ifstream& da_file);
   void Write(ofstream& da_file) const;
   static int getInstanceCount() { return _cnt; }
};//ProcObject




///********************** Bag  ***********************************/// 
class BaseBag : public ObjectContainer, public Closable {
private:
   static int _cnt;

protected:
   // bag_flags are now in the Closable class.
   short max_weight;
   short percentage_weight; //1 to 100, ie 50 bag makes objs half as heavy
   short time_till_disolve; //for corpse, in ticks, -1 if not disolving

   virtual void loseObject_(object* obj);

public:
   BaseBag(BaseBag& source); //copy constructor
   BaseBag(); //default constructor
   BaseBag& operator=(BaseBag& src);
   ~BaseBag() { _cnt--; }

   void Read(ifstream& da_file);
   void Write(ofstream& da_file) const;

   virtual PtrList<object>& getInv() { return (PtrList<object>&)(getInv()); }
   int getMaxWeight() const { return max_weight; }
   int getPercentWeight() const { return percentage_weight; }
   int getTimeTillDisolve() const { return time_till_disolve; }

   void setMaxWeight(int i) { max_weight = i; }
   void setPercentWeight(int i) { percentage_weight = i; }
   void setTimeTillDisolve(int i) { time_till_disolve = i; }

   virtual int getEntityCount(LEtypeE type, int id_num, int sanity);

   virtual int getCurWeight();

   static int getInstanceCount() { return _cnt; }

   virtual LEtypeE getEntityType() const { return LE_BAG_OBJ; }
}; // BaseBag

class Bag : public object, public BaseBag {
protected:
   virtual void gainObject_(object* obj);

public:
   virtual int getCurWeight() { return BaseBag::getCurWeight() +
                                         object::getCurWeight(); }
   virtual void gainObject(Entity* obj);
};

class ProcBag : public ProcObject, public BaseBag {
protected:
   virtual void gainObject_(object* obj);

public:

   virtual int getCurWeight() { return BaseBag::getCurWeight() +
                                         object::getCurWeight(); }
   virtual void gainObject(Entity* obj);
};

#endif
