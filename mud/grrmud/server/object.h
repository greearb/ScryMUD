// $Id$
// $Revision: 1.41 $  $Author$ $Date$

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
   // 10 casts_spells, 11 has_head
   //
   obj_construct_data* construct_data;
   object* skin_ptr;
   object* head_ptr;
   critter* w_eye_owner;
   PtrList<stat_spell_cell> casts_these_spells;
   
   
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
                 // 8 is_corpse, 9 !close, 10 player_owned, 17 consume_key
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
   int isPlayerOwned() const { return bag_flags.get(10); }
   static int getInstanceCount() { return _cnt; }
}; // bag_class
 

///*********************  object  ******************************///

class object {
private:
   static int _cnt;

protected:
   int pause;
   int in_room;
   ObjectScript* cur_script;  /* currently executing script */
   PtrList<ObjectScript> pending_scripts;  /* queue holds scripts to be run */
   PtrList<ObjectScript> obj_proc_scripts; /* Scripts this object has. */

public:
   PtrList<String> names;
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
           // 75 is_butcherable, 76 has_obj_script 77 is_two_handed
           // 78 !necromancer
   
   short extras[OBJ_MAX_EXTRAS + 1]; 
                // 0 charges, light_count, 1 rechargeable, (wands), 2 %load,
                // 3 cur_in_game, 4 max_in_game, 
                // 5 weight, 6 dice_sides, 7 dice_count, 8 level
        
   int cur_stats[OBJ_CUR_STATS + 1]; 
           // 0 vis_bit;
           // 1 price; 
           // 2 item_num;
           // 3 in_zone  //as in olc purposes, its origin

   PtrList<object> *in_list; /* the list the object is in, for disolving
                              NULL if not in someone's inv, a rooms inv,
                              or another object's inv
                              ALWAYS NULL when not a SOBJ */

   bag_struct* bag; //NULL if not a bag
  
   PtrList<stat_spell_cell> affected_by; 
 
   PtrList<object> inv; //empty if not a bag
 
   PtrList<stat_spell_cell> stat_affects;  

   obj_spec_data* obj_proc;

   
   object (object& source); //copy constructor
   object();                             //default constructor
   ~object();                     //destructor
   object& operator= (object& source);

   void Clear();
   void dbRead(long obj_num, long sobj_num, short read_all);
   void fileRead(ifstream& da_file, short read_all);
   void Write(ofstream& da_file);
   int isMagic();

   object* loseInv(object* obj);
   void gainInv(object* obj);

   int getIdNum();
   int getKeyNum();
   int getLevel() const { return extras[8]; }
   int getMaxWeight() const;
   int getCurWeight() const;
   int getEmptyWeight() const { return extras[5]; }
   
   void setEmptyWeight(int wt) { extras[5] = wt; }
   
   void lock();
   void unlock();
   void open();
   int consumesKey();

   int doJunk();
   int doMload(int i_th);

   int doGoToRoom(int dest_room, const char* from_dir, door* by_door,
                  int cur_room, int sanity);

   int getZoneNum() const;
   const String* getShortName() { return getName(); }
   const String* getShortName(int c_bit) { return getName(c_bit); }
   const String* getName() { return getName(~0); };
   const String* getName(int c_bit) { return single_obj_name(*this, c_bit); }
   const String* getLongName() { return getLongName(~0); }
   const String* getLongName(int c_bit) { return long_name_of_obj(*this, c_bit); }
   const String* getLongName(critter& pc);
   int getDefaultPrice() { return cur_stats[1]; }
   int getPause() const { return pause; }
   
   void setPause(int i) { pause = i; }
   void decrementPause() { if (pause > 0) pause--; }

   void makeComponent(int targ, int comp1, int comp2, int comp3, int comp4,
                      int comp5, ComponentEnum con_type);

   int isNamed(const String& name) const;
   int isLocked() const;
   int isMagLocked() const;
   int isClosed() const;
   int isPlayerOwned() const;
   int isPotion() const;
   int isFood() const ;
   int isInUse() const ;
   int isNotComplete() const ;
   int isLightSource() const;
   int isLit() const;
   int isBulletinBoard() const;
   int isScroll() const;
   int isCoins() const { return OBJ_FLAGS.get(55); }
   int isNoDonate() const { return OBJ_FLAGS.get(4); }
   int isHerb() const;
   int isPaused() const { return pause > 0; }
   int isModified() const { return (in_list && 1); } //ie is it a SOBJ
   int hasScript() const { return obj_flags.get(76); }
   int isLiquid() const;
   int isCanteen() const;
   int isTwoHanded() const;
   int isBoat() const { return OBJ_FLAGS.get(62); }
   int isContainer() const { return obj_flags.get(54); }

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
   int getDiceCnt() const { return extras[7]; }
   int getDiceSides() const { return extras[6]; }

   static int getInstanceCount() { return _cnt; }

   int getObjCountByNumber(int onum, int sanity);

   void setCurRoomNum(int i, int sanity);
   int getCurRoomNum() const { return in_room; }

   int getSacPoints(List<object*>& tmp_list, int sanity);
   int calcSacPoints(); //non-recursive

   const ScriptCmd* getNextScriptCmd() { return cur_script->getNextCommand(); }
   void finishedObjProc();
   int isInProcNow() { return (cur_script && (cur_script->isInProgress())); }
   void addProcScript(const String& txt, ObjectScript* script_data);
   void doScriptJump(int abs_offset);
   void listScripts(critter& pc);
   ObjectScript* getScriptAt(int idx) { return obj_proc_scripts.elementAt(idx); }
   void removeScript(String& trigger, int i_th, critter& pc);
   int insertNewScript(ObjectScript* script);

   int processInput(String& input, room& rm, int& was_deleted); /* for object scripts */

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
                     int targ, room& cur_room, int sanity = 0);

}; // class object


#endif
