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
 
extern String* long_name_of_obj(const object& obj, int see_bit);
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
   void Read(ifstream& da_file);
   void Write(ofstream& da_file) const;
   static int getInstanceCount() { return _cnt; }
}; //obj_construct_data


///**********************  obj spec data  *****************///

class obj_spec_data {
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
   List<stat_spell_cell*> casts_these_spells;
   
   
   obj_spec_data();
   obj_spec_data(const obj_spec_data& source);  //copy constructor
   ~obj_spec_data();

   obj_spec_data& operator=(const obj_spec_data& source);

   void Clear();
   void Read(ifstream& da_file);
   void Write(ofstream& da_file) const;
   static int getInstanceCount() { return _cnt; }
};//obj_spec_data



///********************** bag struct ***********************************/// 
class bag_struct {
protected:
   static int _cnt;

public:
   bitfield bag_flags;  // 0 NULL
                 // 1 NULL, 2 is_closed, 3 is_locked, 4 is_pickable,
		 // 5 is_mag_lockable, 6 is_mag_locked 
		 // 7 is_destructable 
		 //    {bash, fireball may open it till reset}
		 // 8 is_corpse, 9 !close, 17 consume_key
   int key_num;  //0 if no key needed
   short max_weight;
   short percentage_weight; //1 to 100, ie 50 bag makes objs half as heavy
   short time_till_disolve; //for corpse, in ticks, -1 if not disolving

   bag_struct(const bag_struct& source); //copy constructor
   bag_struct(); //default constructor
   ~bag_struct() { _cnt--; }

   void Read(ifstream& da_file);
   void Write(ofstream& da_file) const;

   void open() { bag_flags.turn_off(2); }
   void close() { bag_flags.turn_on(2); }
   void lock() { bag_flags.turn_on(3); }
   void unlock() { bag_flags.turn_off(3); }

   int consumesKey() const { return bag_flags.get(17); }

   int isOpen() const { return !bag_flags.get(2); }
   int isLocked() const { return bag_flags.get(3); }
   static int getInstanceCount() { return _cnt; }
}; // bag_class
 
 
///**************************  obj  ******************************///
 
class obj {
protected:
   static int _cnt;

public:
   List<String*> names;
   String short_desc;
   String in_room_desc;
   String long_desc;
   
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
           // 75 is_butcherable
   /* modify OLC when assigning 72 again --BEN */
   
   short extras[OBJ_MAX_EXTRAS + 1]; 
		// 0 charges, light_count, 1 rechargeable, (wands), 2 %load,
		// 3 cur_in_game, 4 max_in_game, 
		// 5 weight, 6 dice_sides, 7 dice_count, 8 level
	
   int cur_stats[OBJ_CUR_STATS + 1]; 
	   // 0 vis_bit;
	   // 1 price; 
	   // 2 item_num;
	   // 3 in_zone  //as in olc purposes, its origin

   List<object*> *in_list; /* the list the object is in, for disolving
                              NULL if not in someone's inv, a rooms inv,
                              or another object's inv
                              ALWAYS NULL when not a SOBJ */

   bag_struct* bag; //NULL if not a bag
  
   List<stat_spell_cell*> affected_by; 
 
   List<object*> inv; //empty if not a bag
 
   List<stat_spell_cell*> stat_affects;  

   obj_spec_data* obj_proc;

   
   obj ();                     //default constructor
   obj (obj& source);    //copy constructor
   ~obj();                     //destructor

   void Clear();
   obj& operator=(obj& source);

   int getLevel() const { return extras[8]; }
   int max_weight();
   int cur_weight();
   void Read(ifstream& da_file, short read_all);
   void Write(ofstream& da_file);
   static int getInstanceCount() { return _cnt; }
}; //class obj


///*********************  object  ******************************///

//static objs_in_game = 0;

class object {
protected:
   static int _cnt;

public:
   obj* ob; 
   
   object (const object& source); //copy constructor
   object();			     //default constructor
   ~object();                     //destructor
   object& operator= (const object& source);

   void Clear();
   void Read(ifstream& da_file, short read_all);
   void Write(ofstream& da_file) const;
   int is_magic();

   object* loseInv(object* obj);
   void gainInv(object* obj);
   int getIdNum();
   int getKeyNum();
   
   void lock();
   void unlock();
   void open();
   int consumesKey();

   int getZoneNum() const;
   const String* getName() { return getName(~0); };
   const String* getName(int c_bit) { return single_obj_name(*this, c_bit); }
   const String* getLongName() { return getLongName(~0); }
   const String* getLongName(int c_bit) { return long_name_of_obj(*this,
                                                                  c_bit); }
   int getLevel() const { if (ob) return ob->getLevel(); return 0; }
   int getDefaultPrice() { return ob->cur_stats[1]; }

   void makeComponent(int targ, int comp1, int comp2, int comp3, int comp4,
                      int comp5, ComponentEnum con_type);

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
   static int getInstanceCount() { return _cnt; }
}; // class object


#endif
