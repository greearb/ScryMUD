// $Id: classes.h,v 1.9 1999/07/28 05:57:05 greear Exp $
// $Revision: 1.9 $  $Author: greear $ $Date: 1999/07/28 05:57:05 $

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
#include "lang_strings.h"


class critter;  //foward declaration


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
   LE_BAG_OBJ,
   LE_ROOM,
   LE_VEHICLE,
   LE_DOOR_DATA,
   LE_DOOR,
   LE_UNKNOWN,
   LE_ANY
};


class ContainedObject;

/**  This class contains objects.  The container knows all of its
 * objects, and the objects know all of their containers, so hopefully
 * memory management will be cleaner and wrapped up at this lower layer.
 */
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

/** Keeps a list of what contains it so it can clean up easily upon
 * destruction and hopefully not leak memory or dangle pointers.
 */
class ContainedObject {
   friend class ObjectContainer;
public:
   ContainedObject() { }

   /** Remove from all containers. */
   virtual ~ContainedObject();
   List<ObjectContainer*>& getContainerList() { return contained_by; }

private:
   void privAddToContainer(ObjectContainer* ptr) { contained_by.gainData(ptr); }
   void privRemoveFromContainer(ObjectContainer* ptr) { contained_by.loseData(ptr); }
   List<ObjectContainer*> contained_by;

};//ContainedObject


/** This will be the base class for all objects, doors, critters, and rooms.
 */
class Entity : public ContainedObject {
protected:
   Entity* container;
   int vis_bit;
   int id_num;

public:
   Entity() : container(NULL) { }
   virtual ~Entity() { }

   virtual LEtypeE getEntityType() const = 0;
   virtual getEntityCount(LEtypeE type, int id_num, int sanity) = 0;
   virtual void gainObject(Entity* le) = 0;
   virtual Entity* loseObject(Entity* le) = 0;
   /** Returns zero if we can't find anything better. */
   virtual int getCurRoomNum() const = 0;


   virtual void setContainer(Entity* cont) { container = cont; }
   Entity* getContainer() { return container; }

   int isEntityType(LEtypeE le);
   int getVisBit() const { return vis_bit; }
   int getIdNum() const { return id_num; }
   void setIdNum(int i) { id_num = i; }
};


/**  This class holds a bitfield that concentrates all of the flags
 * that relate to all closable objects, including doors and containers.
 */
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
   

///************************  StatBonus  ************************///
 
/** A pairing of Stat and Bonus */
class StatBonus {
private:
   static int _cnt;

public:
   int stat;
   int bonus;
   
   StatBonus() : stat(0), bonus(0) { _cnt++; }
   StatBonus(int ss, int bd) :
         stat(ss), bonus(bd) { _cnt++; }
   StatBonus(const StatBonus& src) :
         stat(src.stat),
         bonus_duration(src.bonus) { _cnt++; }
   ~StatBonus() { _cnt--; }

   String toString() const ;

   static int getInstanceCount() { return _cnt; }
};//StatBonus

#endif
