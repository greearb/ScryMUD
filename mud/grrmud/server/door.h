// $Id: door.h,v 1.22 1999/09/06 02:24:28 greear Exp $
// $Revision: 1.22 $  $Author: greear $ $Date: 1999/09/06 02:24:28 $

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

 
///**************************  door data   ********************///

#ifndef GRRMUD_DOOR_INCLUDE_H
#define GRRMUD_DOOR_INCLUDE_H


#include <bitfield.h>
#include <list2.h>
#include <string2.h>
#include "classes.h"
#include "room.h"

class room;

class door_data : virtual public Entity, virtual public Closable {
private:
   static int _cnt;

public:
   door_data (); //default constructor
   door_data(const door_data& source);  //copy constructor
   door_data& operator=(door_data& source);
   virtual ~door_data ();

   void clear();
   int read(istream& da_file, int read_all = TRUE);
   int write(ostream& da_file);
   static int getInstanceCount() { return _cnt; }

   virtual String* getName(int c_bit) { return getName(c_bit, English, 0); }
   virtual String* getName(critter* viewer, int dir);
   virtual String* getName(critter* viewer);
   virtual String* getName(int c_bit, LanguageE lang, int dest);
   
   String* getDirection(int dest, int c_bit = ~0, LanguageE lang = English);
   virtual void toStringStat(critter* viewer, String& rslt, ToStringTypeE st);

   virtual SafeList<object*>& getInv();

   virtual LEtypeE getEntityType() { return LE_DOOR_DATA; }

}; //door_data
 
 
 
//*********************** door **************************//
 
class critter;

/** Door is strange in that it contains much of it's information
 * in the door_data class, which may be shared by several doors.
 */
class door : virtual public ContainedObject, virtual public Serialized {
private:
   static int _cnt;

protected:
   door_data* dr_data;  
   int destination;
   short distance;
   critter* crit_blocking;
   int ticks_till_disolve; //for temp doors
   PtrList<SpellDuration> affected_by;
   Entity* container;

public:
   
   door();
   door(door& source); //copy constructor
   door& operator= (door& source);
   ~door();

   virtual LEtypeE getEntityType() { return LE_DOOR; }
   void clear();
   int write(ostream& da_file);
   int read(istream& da_file, int read_all = TRUE);
   virtual String* getName(critter* viewer) {
      return dr_data->getName(viewer, destination);
   }
   virtual String* getName(int c_bit = ~0) {
      return dr_data->getName(c_bit);
   }
   virtual int nameIsSecret(const String* name);
   virtual const char* getAbrevDir();

   virtual void setContainer(Entity* cont) { container = cont; }
   Entity* getContainer() { return container; }

   virtual door_data* getDrData() { return dr_data; }
   void setDrData(door_data* dr) { dr_data = dr; }

   critter* getCritBlocking() const { return crit_blocking; }
   void setCritBlocking(critter* pc) { crit_blocking = pc; }
   int getDestination() const { return destination; }
   void setDestination(int val) { destination = val; }
   int getDistance() const { return distance; }
   void setDistance(int val) { distance = val; }

   virtual PtrList<SpellDuration>& getAffectedBy() { return affected_by; }

   virtual SpellDuration* isAffectedBy(int spell_num);
   virtual int isAffected() { return !(affected_by.isEmpty()); }
   virtual int affectedByToString(critter* viewer, String& rslt);
   virtual void addAffectedBy(SpellDuration* ptr) { affected_by.prepend(ptr); }

   void setTicksTillDisolve(int val) { ticks_till_disolve = val; }
   int getTicksTillDisolve() const { return ticks_till_disolve; }
   int decrementTicksTillDisolve() { return --ticks_till_disolve; }

   int getVisBit() const { if (dr_data) return dr_data->getVisBit(); return 0; }
   int isInUse() const { return (dr_data && dr_data->isInUse()); }
   int isOpen() const { return (dr_data && dr_data->isOpen()); }
   int isSecret() const { return dr_data && dr_data->isSecret(); }
   int isClosed() const { return dr_data && dr_data->isClosed(); }
   int canOpen() const { return dr_data && dr_data->canOpen(); }
   int isLocked() const { return dr_data && dr_data->isLocked(); }
   int isBlocked() const { return dr_data && dr_data->isBlocked(); }
   int isMagLocked() const { return dr_data && dr_data->isMagLocked(); }
   int isVehicleExit() const { return dr_data && dr_data->isVehicleExit(); }
   int canClose() const { return canOpen(); }
   int isSecretWhenOpen() const { return dr_data && dr_data->isSecretWhenOpen(); }
   int consumesKey() const { return dr_data && dr_data->consumesKey(); }
   int isNoPassdoor() const { return dr_data && dr_data->isNoPassdoor(); }
   int isDestructable() const { return dr_data && dr_data->isDestructable(); }
   int isFlippable() const { return dr_data && dr_data->isFlippable(); }
   int isPickable() const { return dr_data && dr_data->isPickable(); }

   virtual int flipFlag(int fnum) { return dr_data && dr_data->flipFlag(fnum); }

   void lock() { if (dr_data) dr_data->lock(); }
   void unlock() { if (dr_data) dr_data->unlock(); }
   void open() { if (dr_data) dr_data->open(); }
   void close() { if (dr_data) dr_data->close(); }

   void setDestructable(int val) { dr_data->setDestructable(val); }
   void setLockable(int val) { dr_data->setLockable(val); }
   void setBlocked(int val) { dr_data->setBlocked(val); }
   void setLocked(int val) { dr_data->setLocked(val); }

   virtual void toStringStat(critter* viewer, String& rslt, ToStringTypeE st);

   int setKey(int nkey) { return dr_data && dr_data->setKey(nkey); }
   int setToken(int nkey) { return dr_data && dr_data->setToken(nkey); }

   int getKeyNum() const {
      if (dr_data) 
         return dr_data->getKey();
      return 0;
   }

   int getTokenNum() const {
      if (dr_data) 
         return dr_data->getToken();
      return 0;
   }

   int getCurRoomNum();

   room* getDestRoom();
   String* getDirection();

   ///******************  Static Functions ***************************///
   /** Viewer can be null. */
   static door* findDoor(SafePtrList<door> &lst, const int i_th,
                         const String* name, int& count_sofar,
                         critter* viewer);

   /** Viewer can be null. */
   static door* findDoor(SafePtrList<door> &lst, const int i_th,
                         const String* name, critter* viewer);

   static door* findDoorByDest(SafePtrList<door>& lst, int dest_room);

   static int getInstanceCount() { return _cnt; }
};//struct door   

#endif
