// $Id: object.h,v 1.17 1999/07/28 05:57:05 greear Exp $
// $Revision: 1.17 $  $Author: greear $ $Date: 1999/07/28 05:57:05 $

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

class ConstructData {
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
   
   ConstructData();
   ConstructData(const ConstructData& source); //copy constuctr
   ~ConstructData() { _cnt--; }

   //void Clear();
   void Read(ifstream& da_file);
   void Write(ofstream& da_file) const;
   static int getInstanceCount() { return _cnt; }
}; //ConstructData


///**********************  obj spec data  *****************///

/** Special Procedure data. */
class SpecProcData {
private:
   static int _cnt;

protected:
   bitfield flags; // 0 consume_teleport, 
   // 1 has_construct_data, 2 has_skin, 3 consume_poison,
   // 4 NULL, 5 NULL, 6 NULL,
   // 7 NULL, 8 NULL, 9 NULL,
   // 10 casts_spells
   //
   ConstructData* construct_data;
   object* skin_ptr;
   critter* w_eye_owner;
   List<SpellDuration*> casts_these_spells;

public:
   SpecProcData();
   SpecProcData(const SpecProcData& source);  //copy constructor
   ~SpecProcData();

   SpecProcData& operator=(const SpecProcData& source);

   int castsSpells() { return flags.get(10); }
   int consumeTeleport() { return flags.get(0); }
   int hasConstructData() { return flags.get(1); }
   int hasSkin() { return flags.get(2); }
   int consumePoison() { return flags.get(3); }
   object* getSkin() { return skin_ptr; }
   void setSkin(object* sk) { skin_ptr = sk; }
   critter* getWizardEyeOwner() { return w_eye_owner; }
   void setWizardEyeOwner(critter* cr) { w_eye_owner = cr; }

   bitfield& getFlags() { return flags; }
   List<SpellDuration*>& getSpellsCast() { return casts_these_spells; }

   void Clear();
   void Read(ifstream& da_file);
   void Write(ofstream& da_file) const;
   static int getInstanceCount() { return _cnt; }
};//SpecProcData



///*********************  object  ******************************///

class object : public Entity {
private:
   static int _cnt;

protected:
   int pause;
   ObjectScript* cur_script;  /* currently executing script */
   List<ObjectScript*> pending_scripts;  /* queue holds scripts to be run */
   List<ObjectScript*> obj_proc_scripts; /* Scripts this object has. */

   int is_modified; //takes the place of what IN_LIST used to do for us.

   void gainObject_(object* obj);
   object* loseObject_(object* obj);

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

   List<stat_spell_cell*> affected_by; 
 
   List<stat_spell_cell*> stat_affects;  

   SpecProcData* obj_proc;

public:
   
   object (object& source); //copy constructor
   object();			     //default constructor
   virtual ~object();                     //destructor
   object& operator= (object& source);

   void Clear();
   void Read(ifstream& da_file, short read_all);
   void Write(ofstream& da_file);
   int isMagic();

   Entity* loseObject(Entity* le);
   void gainObject(Entity* le);
   int getCurRoomNum() const;

   int getLevel() const { return extras[8]; }
   int getCurWeight() const;
   
   int getEmptyWeight() const { return extras[5]; }
   void setEmptyWeight(int wt) { extras[5] = wt; }
   
   int doGoToRoom(int dest_room, const char* from_dir, door* by_door,
                  int cur_room, int sanity);

   int getZoneNum() const;
   const String* getShortName() { return getName(); }
   const String* getShortName(int c_bit) { return getName(c_bit); }
   const String* getName() { return getName(~0); };
   const String* getName(int c_bit) { return single_obj_name(*this, c_bit); }
   const String* getLongName() { return getLongName(~0); }
   const String* getLongName(int c_bit) { return long_name_of_obj(*this, c_bit); }
   int getDefaultPrice() { return cur_stats[1]; }
   int getPause() const { return pause; }
   
   void setPause(int i) { pause = i; }
   void decrementPause() { if (pause > 0) pause--; }

   void makeComponent(int targ, int comp1, int comp2, int comp3, int comp4,
                      int comp5, ComponentEnum con_type);

   int isNamed(const String& name) const;
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

   const ScriptCmd* getNextScriptCmd() { return cur_script->getNextCommand(); }
   void finishedObjProc();
   int isInProcNow() { return (cur_script && (cur_script->isInProgress())); }
   void addProcScript(const String& txt, ObjectScript* script_data);
   void doScriptJump(int abs_offset);
   void listScripts(critter& pc);
   ObjectScript* getScriptAt(int idx) { return obj_proc_scripts.elementAt(idx); }
   void removeScript(String& trigger, int i_th, critter& pc);
   int insertNewScript(ObjectScript* script);

   int processInput(String& input, room& rm); /* for object scripts */

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


///********************** Bag  ***********************************/// 
class Bag : public ObjectContainer, public object, public Closable {
protected:
   static int _cnt;
   // bag_flags are now in the Closable class.
   int key_num;  //0 if no key needed
   short max_weight;
   short percentage_weight; //1 to 100, ie 50 bag makes objs half as heavy
   short time_till_disolve; //for corpse, in ticks, -1 if not disolving
public:

   Bag(Bag& source); //copy constructor
   Bag(); //default constructor
   Bag& operator=(Bag& src);
   ~Bag() { _cnt--; }

   void Read(ifstream& da_file);
   void Write(ofstream& da_file) const;

   List<object*>& getInv() { return (List<object*>&)(getInv()); }
   int getMaxWeight() const { return max_weight; }
   int getPercentWeight() const { return percentage_weight; }
   int getTimeTillDisolve() const { return time_till_disolve; }

   void setMaxWeight(int i) { max_weight = i; }
   void setPercentWeight(int i) { percentage_weight = i; }
   void setTimeTillDisolve(int i) { time_till_disolve = i; }

   static int getInstanceCount() { return _cnt; }

   virtual LEtypeE getEntityType() const { return LE_BAG_OBJ; }
}; // Bag


#endif
