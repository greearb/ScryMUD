// $Id: object.h,v 1.23 1999/08/10 07:06:20 greear Exp $
// $Revision: 1.23 $  $Author: greear $ $Date: 1999/08/10 07:06:20 $

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
#include "Scriptable.h"

class object;
 
extern String* long_name_of_obj(object& obj, int see_bit); //misc.h
extern const String* single_obj_name(object& obj, int see_bit); //misc2.h

enum ComponentEnum {
   CONSTRUCTION = 0,
   CONCOCTION = 1
};//ComponentEnum


///********************  obj construct data  **********************///

class obj_construct_data {
protected:
   static int _cnt;

public:
   int target_object; //obj_num of object it is component of
   /* companion components */
   int item1;
   int item2;
   int item3;
   int item4;
   int item5;
   
   obj_construct_data();
   obj_construct_data(const obj_construct_data& source); //copy constuctr
   ~obj_construct_data() { _cnt--; }

   //void Clear();
   void read(istream& da_file, int read_all = TRUE);
   void write(ostream& da_file);
   static int getInstanceCount() { return _cnt; }
}; //obj_construct_data


///**********************  obj spec data  *****************///

class obj_spec_data : public Serialized {
protected:
   static int _cnt;

public:
   bitfield obj_spec_data_flags; // 0 consume_teleport, 
   // 1 has_construct_data, 2 has_skin, 3 consume_poison,
   // 4 NULL, 5 NULL, 6 NULL,
   // 7 NULL, 8 NULL, 9 NULL,
   // 10 casts_spells
   //
   obj_construct_data* construct_data;
   object* skin_ptr;
   critter* w_eye_owner;
   PtrList<SpellDuration> casts_these_spells;
   
   
   obj_spec_data();
   obj_spec_data(const obj_spec_data& source);  //copy constructor
   virtual ~obj_spec_data();
   
   obj_spec_data& operator=(const obj_spec_data& source);

   void clear();
   virtual int read(istream& da_file, int read_all = TRUE);
   virtual int write(ostream& da_file);
   virtual LEtypeE getEntityType() { return LE_OBJ_PROC; }
   
   static int getInstanceCount() { return _cnt; }
};//obj_spec_data



///********************** bag struct ***********************************/// 
class bag_struct : public Closable {
private:
   static int _cnt;
protected:
   short max_weight;
   short percentage_weight; //1 to 100, ie 50 bag makes objs half as heavy
   short time_till_disolve; //for corpse, in ticks, -1 if not disolving

public:
   bag_struct(const bag_struct& source); //copy constructor
   bag_struct(); //default constructor
   virtual ~bag_struct() { _cnt--; }

   int getMaxWeight() const { return max_weight; }
   int getPercentWeight() const { return percentage_weight; }
   int getTimeTillDisolve() const { return time_till_disolve; }

   void setMaxWeight(int i) { max_weight = i; }
   void setPercentWeight(int i) { percentage_weight = i; }
   void setTimeTillDisolve(int i) { time_till_disolve = i; }

   virtual int read(istream& da_file, int read_all = TRUE);
   virtual int write(ostream& da_file);
   virtual LEtypeE getEntityType() { return LE_BAG_OBJ; }

   static int getInstanceCount() { return _cnt; }
}; // bag_class
 

///*********************  object  ******************************///

class object : public Entity, public Scriptable {
private:
   static int _cnt;

protected:
   int is_modified;

public:
   LStringCollection short_desc;
   LStringCollection in_room_desc;
   
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

   bag_struct* bag; //NULL if not a bag
  
   SafeList<object*> inv; //empty if not a bag
 
   PtrList<StatBonus> stat_affects;  

   obj_spec_data* obj_proc;
   
   object (object& source); //copy constructor
   object();			     //default constructor
   ~object();                     //destructor
   object& operator= (object& source);

   void clear();
   int read(istream& da_file, int read_all = TRUE);
   int write(ostream& da_file);
   int read_v3(istream& da_file, int read_all);
   int read_v2(istream& da_file, String& name, short read_all);

   int isMagic();

   object* loseInv(object* obj);
   void gainInv(object* obj);

   int getIdNum();
   int getKeyNum();
   int getLevel() const { return extras[8]; }
   int getMaxWeight() const;
   int getCurWeight();
   int getEmptyWeight() const { return extras[5]; }
   
   /** ptr will be consumed by this call, ie takes charge of memory. */
   void addStatAffect(StatBonus* ptr);
   void setEmptyWeight(int wt) { extras[5] = wt; }
   
   void lock();
   void unlock();
   void open();
   int consumesKey();

   int doGoToRoom(int dest_room, const char* from_dir, door* by_door,
                  int cur_room, int sanity);

   int getZoneNum() const;

   /** These default to english. */
   void addShortDesc(String& new_val);
   void addInRoomDesc(String& new_val);

   void addShortDesc(LString& new_val);
   void addInRoomDesc(LString& new_val);

   int getDefaultPrice() { return cur_stats[1]; }

   void makeComponent(int targ, int comp1, int comp2, int comp3, int comp4,
                      int comp5, ComponentEnum con_type);

   int isNamed(const String& name) const;
   int isLocked() const;
   int isMagLocked() const;
   int isClosed() const;
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
   int isModified() const { return is_modified; }
   int hasScript() const { return obj_flags.get(76); }
   int isLiquid() const;
   int isCanteen() const;

   void setModified(int val) { is_modified = val; }
   void setComplete();
   void setIncomplete();
   void setIdNum(int i);
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

   int getObjCountByNumber(int onum, int sanity);

   void setCurRoomNum(int i, int sanity);
   int getCurRoomNum();

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

}; // class object


#endif
