// $Id: vehicle.h,v 1.9 2001/03/29 03:02:36 eroper Exp $
// $Revision: 1.9 $  $Author: eroper $ $Date: 2001/03/29 03:02:36 $

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

///*************************  Vehicle  *****************************///

#ifndef GRRMUD_VEHICLE_INCLUDE_H
#define GRRMUD_VEHICLE_INCLUDE_H

#include "room.h"
#include "misc.h"
#include "misc2.h"

class vehicle : public room {
protected:
   bitfield vehicle_flags;
   /* 0 is_self_guided, 1 is_stealthy, 2 unlimited_fuel,
    * 3 can_fly, 4 can_climb, 5 can_float, 6 can_dive,
    * 7 can_see_out (show room descs to passengers) 
    */

   Cell<PathCell*> cll; //this will iterate around and around
   PtrList<PathCell> path_cells;
   int ticks_till_next_stop;
   int in_room; //the room the vehicle is currently in
   int cur_fuel;
   int max_fuel;
   int ticks_between_stops;

public:

   vehicle();
   vehicle(int num);
   virtual ~vehicle();

   int isVehicle() { return TRUE; }

   int isSelfGuided() const;
   int isStealthy() const;
   int hasUnlimitedFuel() const;
   int canFly() const;
   int canClimb() const;
   int canFloat() const;
   int canDive() const;

   void toggleVehicleFlag(int i) {
      if ((i >=0) && (i<50)) {
         vehicle_flags.flip(i);
      }
   }

   void showVehicleFlags(critter& pc) {
      out_field(vehicle_flags, pc, VEHICLE_FLAGS_NAMES);
   }

   void showPaths(critter& pc);
   void insertPathCell(int index, PathCell* pcell);
   PathCell* hasPathCell(int path_cell_num);
   void remPathCell(int path_cell_num); //deletes it from memory
   void setPathPointer(int index);

   int isAtDestination();

   void advancePathIterator();

   void decrementTicksTillNextStop();
   int getTicksTillNextStop() const;

   void setInRoom(int i);
   void setCurFuel(int i) { cur_fuel = i; }
   void setMaxFuel(int i) { max_fuel = i; }
   void setTicksBetweenStops(int i) { ticks_between_stops = i; }

   String getPassengerMessage();
   String getExitDirection();
   int getExitNum();

   //figure out next destination and go there...
   int move(int see_bit, int i_th, const String& exit_direction);
   int move(); //figure direction and call other move function

   door* findNextExit(); //for non-self-guided vehicles
   int canEnter(const room* dest, int do_msg) const;
   virtual void stat(critter& pc);

   virtual void Clear();
#ifdef USEMYSQL
   virtual void dbWrite();
   virtual void dbRead(int veh_num, short read_all);
#endif
   virtual void fileRead(ifstream& da_file, short read_all);
   virtual void fileWrite(ofstream& da_file);

   virtual void normalize(); /* called after OLC to enforce as much state as
                              * possible. */


};//vehicle

#endif
