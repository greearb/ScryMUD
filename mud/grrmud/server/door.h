// $Id: door.h,v 1.12 1999/08/09 06:00:39 greear Exp $
// $Revision: 1.12 $  $Author: greear $ $Date: 1999/08/09 06:00:39 $

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

class door_data : public Entity, public Closable {
private:
   static int _cnt;

protected:
   int in_zone; //owner zone
   int token_num;
   int key_num;

public:

   virtual LEtypeE getEntityType() const { return LE_DOOR_DATA; }

   int isInZone(int zn) { return zn == in_zone; }
   int getZoneNum() const { return in_zone; }
   int getKeyNum() const { return key_num; }
   int getTokenNum() const { return token_num; }

   door_data (); //default constructor
   door_data(const door_data& source);  //copy constructor
   door_data& operator=(const door_data& source);
   virtual ~door_data ();

   void Clear();
   void Read(ifstream& da_file);
   void Write(ofstream& da_file);
   static int getInstanceCount() { return _cnt; }
}; //door_data
 
 
 
//*********************** door **************************//
 
class critter;

/** Door is strange in that it contains much of it's information
 * in the door_data class, which may be shared by several doors.
 * For that reason, though door is an Entity, it must over-ride
 * several of Entity's methods, especially those dealing with
 * name and description data.
 */
class door : public Entity {
private:
   static int _cnt;

protected:
   door_data* dr_data;  
   int destination;
   short distance;
   critter* crit_blocking;
   int ticks_till_disolve; //for temp doors

public:
   
   door();
   door(door& source); //copy constructor
   door& operator= (door& source);
   ~door();

   void clear();
   int write(ostream& da_file);
   int read(istream& da_file, int read_all = TRUE);
   
   int isOwnedBy(critter& pc);
   int getVisBit() const { if (dr_data) return dr_data->getVisBit(); return 0; }
   int isOpen() const { return (dr_data && dr_data->isOpen()); }
   int isSecret() const { return dr_data && dr_data->isSecret(); }
   int isClosed() const { return dr_data && dr_data->isClosed(); }
   int canOpen() const { return dr_data && dr_data->canOpen(); }
   int isLocked() const { return dr_data && dr_data->isLocked(); }
   int isMagLocked() const { return dr_data && dr_data->isMagLocked(); }
   int isVehicleExit() const { return dr_data && dr_data->isVehicleExit(); }
   int canClose() const { return canOpen(); }
   int isSecretWhenOpen() const { return dr_data && dr_data->isSecretWhenOpen(); }
   int consumesKey() const { return dr_data && dr_data->consumesKey(); }
   int isNoPassdoor() const { return (dr_data && dr_data->isNoPassdoor()); }

   void lock() { if (dr_data) dr_data->lock(); }
   void unlock() { if (dr_data) dr_data->unlock(); }
   void open() { if (dr_data) dr_data->open(); }
   void close() { if (dr_data) dr_data->close(); }

   int getKeyNum() const {
      if (dr_data) 
         return dr_data->getKeyNum();
      return 0;
   }

   int getTokenNum() const {
      if (dr_data) 
         return dr_data->getTokenNum();
      return 0;
   }

   int getCurRoomNum();

   room* getDestRoom();
   String* getDirection();

   ///******************  Static Functions ***************************///
   static door* findDoor(const SafeList<door*> &lst, const int i_th,
                         const String* name, const int see_bit,
                         const room& rm);

   static door* findDoor(const SafeList<door*>& lst, const int i_th,
                         const String* name, const int see_bit,
                         const int rm_vis_bit);

   static door* findDoorByDest(const SafeList<door*>& lst, int dest_room);

   static int getInstanceCount() { return _cnt; }
};//struct door   

#endif
