// $Id: door.h,v 1.9 1999/07/28 05:57:05 greear Exp $
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
protected:
   static int _cnt;
   int in_zone; //owner zone
   List<String*> names; //the direction e, w... will be first 
   String long_desc;

   int token_num;
   int key_num;

public:

   virtual LEtypeE getEntityType() const { return LE_DOOR_DATA; }

   int isInZone(int zn) { return zn == in_zone; }
   int getZoneNum() const { return in_zone; }
   int getKeyNum() const { return key_num; }
   int getTokenNum() const { return token_num; }
   int getVisBit() const { return vis_bit; }

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
 
class door {
protected:
   static int _cnt;

public:
   door_data* dr_data;  
   int destination;
   short distance;
   List<stat_spell_cell*> affected_by;
   critter* crit_blocking;
   int ticks_till_disolve; //for temp doors
   int in_room;
   
   door();
   door(const door& source); //copy constructor
   door& operator= (const door& source);
   ~door();

   void Clear();
   void Write(ofstream& da_file);
   void Read(ifstream& da_file);

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

   room* getDestRoom();
   String* getDirection();

   ///******************  Static Functions ***************************///
   static door* findDoor(const List<door*> &lst, const int i_th,
                         const String* name, const int see_bit,
                         const room& rm);

   static door* findDoor(const List<door*>& lst, const int i_th,
                         const String* name, const int see_bit,
                         const int rm_vis_bit);

   static door* findDoorByDest(const List<door*>& lst, int dest_room);

   static int getInstanceCount() { return _cnt; }
};//struct door   

#endif
