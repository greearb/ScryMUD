// $Id: classes.h,v 1.8 1999/07/25 20:13:04 greear Exp $
// $Revision: 1.8 $  $Author: greear $ $Date: 1999/07/25 20:13:04 $

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

///********************  classes.h  *****************************///



#ifndef __CLASSES_INCLUDE
#define __CLASSES_INCLUDE


#include <string2.h>
#include <bitfield.h>
#include <list2.h>
#include "script.h"

class critter;  //foward declaration


// This is used for reading in overloaded objects
enum class_id_enum {
   ROOM_CLASS_ID = 0,
   VEHICLE_CLASS_ID = 1
};//class_id_enum


class HegemonMarkup {
public:
   /**  Turn any hot characters, such as <,> into Hegemon
    * markup characters.  This will make this string safe for
    * inclusion in hegemon markup text.
    */
   static String makeSafeForHegTag(const char* string);
}; 

/** I'd wrather not do it this way, but I can't find a better solution.
 * So, I'm going to make the LogicalEntity's aware of what kind of object
 * they are.
 */
enum LEtypeE {
   LE_CRITTER,
   LE_OBJECT,
   LE_ROOM,
   LE_DOOR_DATA,
   LE_DOOR,
   LE_UNKNOWN
};


class ContainedObject;

class ObjectContainer {
   friend class ContainedObject;
public:
   virtual ~ObjectContainer();

   virtual int append(ContainedObject* o);
   virtual int prepend(ContainedObject* o);
   virtual int insertUnique(ContainedObject* o);
   virtual ContainedObject* remove(ContainedObject* o);
   List<ContainedObject*>& getInv() { return inv; }
   const List<ContainedObject*>& peekInv() const { return inv; }

private:
   List<ContainedObject*> inv;
   ContainedObject* privRemoveObject(ContainedObject* o) { return inv.loseData(o); }

};//ObjectContainer

class ContainedObject {
   friend class ObjectContainer;
public:
   ContainedObject() { }

   /** Remove from all containers. */
   ~ContainedObject();
   List<ObjectContainer*>& getContainerList() { return contained_by; }

private:
   void privAddToContainer(ObjectContainer* ptr) { contained_by.gainData(ptr); }
   void privRemoveFromContainer(ObjectContainer* ptr) { contained_by.loseData(ptr); }
   List<ObjectContainer*> contained_by;

};//ContainedObject


class EntityContainer : public ObjectContainer {
public:
   int getEntityCountByNumber(int id_num, int sanity);
   List<Entity*>& getInv() { return (List<Entity*>&)(ObjectContainer::getInv()); }
};//EntityContainer


class LogicalContainer;

class Entity : public ContainedObject {
protected:
   LogicalContainer* container;
   int vis_bit;
   int id_num;

public:
   Entity() : container(NULL) { }

   virtual LEtypeE getEntityType() const = 0;

   virtual void setContainer(LogicalContainer* cont) { container = cont; }
   LogicalContainer* getContainer() { return container; }

   int getVisBit() const { return vis_bit; }
   int getIdNum() const { return id_num; }
   void setIdNum(int i) { id_num = i; }
};

class LogicalContainer {
public:
   virtual void gainObject(Entity* le) = 0;
   virtual LogicalEntity* loseObject(Entity* le) = 0;

   /** Returns zero if we can't find anything better. */
   virtual int getCurRoomNum() const = 0;
};


class Closable {
protected:
   // When modifying this, modify CLOSABLE_FLAGS_NAMES in const.cc
   bitfield flags;
	      // 0 open exit, basically no door
              // 1 is_mag_lockable, 2 is_closed, 3 is_locked, 4 is_pckabl,
	      // 5 is_lockable, 6 mag_locked (spell only can open it)
	      // 7 is_destructable, 8 is_closeable, 9 is_flippable
              // 10 in_use, 11 is_unopenable (by players, auto only)
	      // 12 is_vehicle_exit, 13 is_secret, 14 is_blocked,
              // 15 !complete, 16 secret_when_open_too, 17 consume_key
              // 18 !passdoor, 19 is_corpse

public:
   int isOpen() const { return !(flags.get(2)); }
   int canClose() const { return canOpen(); }
   int isVehicleExit() const { return flags.get(12); }
   int canOpen() const { return (!flags.get(11) 
                                  && flags.get(8)); }
   int canLock() const { return flags.get(5); }

   int isClosed() const { return flags.get(2); }
   int isLocked() const { return flags.get(3); }
   int isMagLocked() const { return flags.get(6); }
   int isSecret() const { return (flags.get(13) || flags.get(16)); }
   int isInUse() const { return flags.get(10); }
   int isNotComplete() const { return flags.get(15); }
   int isSecretWhenOpen()  const { return flags.get(16); }
   int consumesKey() const { return flags.get(17); }
   int isNoPassdoor() const { return flags.get(18); }

   void setComplete() { flags.turn_off(15); }
   void setNotComplete() { flags.turn_on(15); }
   void close() { flags.turn_on(2); }
   void open() { flags.turn_off(2); }
   void lock() { close(); flags.turn_on(3); }
   void unlock() { flags.turn_off(3); }
};//class Closable


class LString : public String {
protected:
   LanguageE lang;
public:
   LString(LanguageE language, int len) : String(len), lang(language) { }
   LanguageE getLanguage() const { return lang; }
   void setLanguage(LanguageE l) { lang = l; }
};

class LStringCollection : public List<LString*> {
public:
   LString& getString(LanguageE for_lang);
};
   

///************************  stat_cell  ************************///
 
class stat_spell_cell {
private:
   static int _cnt;

public:
   int stat_spell;
   int bonus_duration;
   
   stat_spell_cell() : stat_spell(0), bonus_duration(0) { _cnt++; }
   stat_spell_cell(int ss, int bd) :
         stat_spell(ss),
         bonus_duration(bd) { _cnt++; }
   stat_spell_cell(const stat_spell_cell& src) :
         stat_spell(src.stat_spell),
         bonus_duration(src.bonus_duration) { _cnt++; }
   ~stat_spell_cell() { _cnt--; }

   String toString() const ;

   static int getInstanceCount() { return _cnt; } //how many ZoneList objects exist?
};//stat_spell 
 


///***********************  PathCell  *****************************///

class PathCell {
private:
   String desc;
   String dir_to_next;
   int i_th_dir;
   int is_destination;
   static int _cnt;
   PathCell(const PathCell& src);

public:
   PathCell(String& description, String& direction_to_next, int is_dest);
   PathCell();

   ~PathCell() { _cnt--; }

   static int getInstanceCount() { return _cnt; } //how many ZoneList objects exist?

   void setDesc(String& description) { desc = description; }
   String& getDesc() { return desc; }
   String* getDescPtr() { return &desc; }

   void setDir(int i_th, const String& dir) { i_th_dir = i_th; dir_to_next = dir; }
   String& getDir() { return dir_to_next; }
   int getDirNum() const { return i_th_dir; }

   void setIsDest(int i);
   int isDest();

   //need the veh_num and cell_num to display appropriately.
   void stat(int veh_num, int cell_num, critter& pc);

   void Clear();
   void Read(ifstream& da_file);
   void Write(ofstream& da_file);
};//PathCell

#endif
