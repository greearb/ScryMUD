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

#include "vehicle.h"
#include "const.h"
#include <PtrArray.h>
#include "room.h"
#include "misc.h"
#include "misc2.h"
#include "commands.h"

vehicle::vehicle() {
   path_cells.head(cll);
   cll.next(); //move to first one.

   ticks_till_next_stop = in_room = cur_fuel = max_fuel = 0;
   ticks_between_stops = 0;
}

vehicle::vehicle(int num) : room(num) {
   path_cells.head(cll);
   cll.next(); //move to first one.

   ticks_till_next_stop = in_room = cur_fuel = max_fuel = 0;
   ticks_between_stops = 0;
}


vehicle::~vehicle() {
   clear_ptr_list(path_cells);
}

int vehicle::canEnter(const room* dest_room, int do_msg) const {
   int retval = FALSE;
   String buf(100);

   if (!(hasUnlimitedFuel() || (cur_fuel > 0))) {
      if (do_msg) {
         Sprintf(buf, "%S is out of fuel.\n", &(short_desc));
         showAllCept(buf);
      }//if
   }//if
   else if (!dest_room->isInUse()) { //if not used
      if (do_msg)
         showAllCept("That room doesn't really exist!!\n");
   }//if
   else if (dest_room->isNoVehicle()) { //!vehicle
      if (do_msg)
         showAllCept("Vehicles cannot travel there.\n");
   }//if
   else if (dest_room->needsBoat() && !(canFloat() || canFly())) {
      if (do_msg)
         showAllCept("This vehicle can't travel on water.\n");
   }//if
   else if ((dest_room->needsClimb()) && (!(canClimb() || canFly()))) {
      if (do_msg)
         showAllCept("This vehicle can't climb.\n");
   }//if
   else if ((dest_room->needsDive()) && (!canDive())) {
      if (do_msg)
         showAllCept("This vehicle can't dive.\n");
   }//if
   else if ((dest_room->needsFly()) && (!canFly())) {
      if (do_msg)
         showAllCept("This vehicle can't fly.\n");
   }//if
   else if (dest_room->isZlocked()) { //is zlocked
      if (do_msg)
         showAllCept("That direction has been locked to vehicles.\n");
   }//if
   else
      retval = TRUE;
   return retval;
}//canEnter

void vehicle::setInRoom(int i) {
   if (i < 0)
      i = 0;
   if (i > NUMBER_OF_ROOMS)
      i = 0;

   in_room = i;
}


int vehicle::move() {
   return move(~0, getExitNum(), getExitDirection());
}

void vehicle::stat(critter& pc) {
   room::stat(pc);
   show("\n\t\t\tIS A VEHICLE\nVehicle flags:\n", pc);
   out_field(vehicle_flags, pc);

   String buf(200);

   Sprintf(buf, "ticks_till_next_stop:  %i, in_room:  %i, cur_fuel:  %i\n",
           ticks_till_next_stop, in_room, cur_fuel);
   show(buf, pc);

   Sprintf(buf, "Max fuel:  %i, ticks_between_stops:  %i\n",
           max_fuel, ticks_between_stops);
   show(buf, pc);
}//stat

int vehicle::move(int see_bit, int i_th, const String& exit_direction) {
   int dest;
   String buf(200);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In vehicle::move, in_room:  " << in_room 
             << "  see_bit:  " << see_bit << "  i_th:  " << i_th
             << " exit dir -:" << exit_direction << ":-" << endl;
   }

   ticks_till_next_stop = ticks_between_stops;

   door* exit_dr_ptr = door::findDoor(room_list[in_room].DOORS, i_th,
                                      &exit_direction, see_bit,
                                      room_list[in_room].getVisBit());

   //log("Got exit_dr_ptr.\n");

   if (!exit_dr_ptr) {
      //mudlog.log(WRN, "WARNING:  destination dr_ptr is NULL in move_room.\n");
      Sprintf(buf, "Your vehicle cannot go %S.\n", &exit_direction);
      showAllCept(buf);
      return FALSE;
   }//if

   dest = abs(exit_dr_ptr->destination);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "got dest:  " << dest << endl;
   }

   if (!canEnter(&room_list[dest], TRUE)) {
      mudlog.log(WRN, 
                 "WARNING:  vehicle::can_enter failed in vehicle::move\n");
      return FALSE;
   }//if

   ticks_till_next_stop += exit_dr_ptr->distance;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Passed tests, looking for doors to vehicle, size list:  "
             << room_list[in_room].DOORS.size() << endl;
   }

   /* passed all tests, lets move it!! */

   /* delete door(s) TO the car */
   Cell<door*> dcll(room_list[in_room].DOORS);
   door* tmp_ptr = NULL;

   door* dr_ptr = dcll.next();
   while (dr_ptr) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "Comparing dest:  " << abs(dr_ptr->destination)
                << "  and this room num:  " << getRoomNum() << endl;
      }
      if (abs(dr_ptr->destination) == getRoomNum()) {
         //mudlog.log(DBG, "They were equal.\n");
         tmp_ptr = dr_ptr;
         dr_ptr = room_list[in_room].DOORS.lose(dcll);
      }//if
      else {
         //mudlog.log(DBG, "They were not equal.");
         dr_ptr = dcll.next();
      }
   }//if

   /* save tmp_ptr, it can be reused in next room */
   mudlog.log(DBG, "Found tmp_ptr.\n");

   if (!tmp_ptr) {  //the train door, the exterior one
      mudlog.log(ERR, "ERROR:  tmp_ptr is NULL in vehicle::move()\n");
      return FALSE;
   }//if

   if (!tmp_ptr->dr_data) {
      mudlog.log(ERR, "ERROR:  dr_data is null.\n");
      return FALSE;
   }//if

   mudlog.log(DBG, "About to do messages.\n");

   dr_ptr = NULL;
   // lets find the door from the train to the outside.
   DOORS.head(dcll); //head of vehicle doors
   while ((dr_ptr = dcll.next())) {
      if (dr_ptr->dr_data == tmp_ptr->dr_data) {
         break;
      }
   }//while
      

   /* show messages to room, the one BEFORE the move */
   if (!isStealthy()) { //if not stealth
      if (isAtDestination() && 
          tmp_ptr->dr_data->isOpen() &&
          tmp_ptr->dr_data->canClose()) {
         if (dr_ptr) {
            Sprintf(buf, "The %S closes.\n", name_of_door(*dr_ptr, ~0));
            showAllCept(buf);
         }
         else {
            mudlog.log(ERR, "ERROR:  dr_ptr is NULL...\n");
         }
         
         Sprintf(buf, "The %S closes.\n", name_of_door(*tmp_ptr, ~0));
         room_list[in_room].showAllCept(buf);

         tmp_ptr->dr_data->close(); //make it closed
      }//if

      Sprintf(buf, "%S leaves towards the %S.\n", &(short_desc),
              &exit_direction);
      room_list[in_room].showAllCept(buf);
   }//if not stealth

   /*  make vehicle door exit to new room */

   mudlog.log(DBG, "About to update vehicle door.\n");
   if (!hasUnlimitedFuel()) {
      cur_fuel -= room_list[in_room].getMovCost();
   }

   DOORS.head(dcll); //list of doors to/from vehicle
   while ((dr_ptr = dcll.next())) {
      if (abs(dr_ptr->destination) == abs(in_room)) { //if is_vehicle_exit
         if (dr_ptr->destination < 0)
            dr_ptr->destination = -(abs(dest));
         else
            dr_ptr->destination = abs(dest); 
         break; 
      }//if
   }//while
   /* vehicle door now points to next room */

   in_room = dest;  //guaranteed to be positive


   mudlog.log(ERR, "Vehicle door updated.\n");

             /* add door to destination room */
   room_list[in_room].DOORS.prepend(tmp_ptr);

   advancePathIterator(); /* advance the internal state, let this object
                             know that it's actually been moved. */

   mudlog.log(ERR, "In new room..doing messages.\n");

   /* now in new room, do some messages if needed */
   if (!isStealthy()) { //if not stealty
      Sprintf(buf, "%S has arrived.\n", &(short_desc));
      room_list[in_room].showAllCept(buf);
   }//if not stealth


   dr_ptr = NULL;
   // lets find the door from the train to the outside.
   DOORS.head(dcll); //head of vehicle doors
   while ((dr_ptr = dcll.next())) {
      if (dr_ptr->dr_data == tmp_ptr->dr_data) {
         break;
      }
   }//while

   if (isAtDestination()) { //in other words, the one its in NOW
      if (!isStealthy() && tmp_ptr->dr_data->isClosed()) {
         if (dr_ptr) {
            Sprintf(buf, "The %S opens.\n", name_of_door(*dr_ptr, ~0));
            showAllCept(buf);
         }
         else {
            mudlog.log(ERR, "ERROR:  dr_ptr is NULL, end of move.\n");
         }

         if (tmp_ptr) {
            Sprintf(buf, "The %S opens.\n", name_of_door(*tmp_ptr, ~0));
            room_list[in_room].showAllCept(buf);
         }
         else {
            mudlog.log(ERR, "ERROR:  tmp_ptr is NULL, end of move.\n");
         }

      }//if
      tmp_ptr->dr_data->open(); //make it open
   }//if

   buf = getPassengerMessage();
   if (buf.Strlen() > 0) {
      showAllCept(buf);
   }//if

   if (vehicle_flags.get(7)) { //if can see out
      critter* cptr;
      Cell<critter*> ccll(CRITTERS);

      while ((cptr = ccll.next())) {
         do_look(1, &NULL_STRING, *cptr, room_list[in_room]);
      }//while
   }//if can see out

   return TRUE;

}//move


// If cll is currently 'pointing' to a room that
// is a destination, return true
int vehicle::isAtDestination() {
   PathCell* data = cll.item();
   
   return (data && data->isDest());
}

int vehicle::isSelfGuided() const { return vehicle_flags.get(0); }
int vehicle::isStealthy() const { return vehicle_flags.get(1); }
int vehicle::hasUnlimitedFuel() const { return vehicle_flags.get(2); }
int vehicle::canFly() const { return vehicle_flags.get(3); }
int vehicle::canClimb() const { return vehicle_flags.get(3); }
int vehicle::canFloat() const { return vehicle_flags.get(3); }
int vehicle::canDive() const { return vehicle_flags.get(3); }
void vehicle::decrementTicksTillNextStop() { ticks_till_next_stop--; }
int vehicle::getTicksTillNextStop() const { return ticks_till_next_stop; }

void vehicle::advancePathIterator() {
   //iterate past end of the list.
   if (!cll.next())
      cll.next();
}


String vehicle::getPassengerMessage() {
   PathCell* ptr = cll.item();
   if (ptr)
      return ptr->getDesc();
   else
      return UNKNOWN_DESC;
}

String vehicle::getExitDirection() {
   PathCell* ptr = cll.item();
   if (ptr)
      return ptr->getDir();
   else
      return UNKNOWN;
}

// get the i_th exit value
int vehicle::getExitNum() {
   PathCell* ptr = cll.item();
   if (ptr)
      return ptr->getDirNum();
   else
      return 1;
}


void vehicle::Clear() {
   room::Clear();
   vehicle_flags.Clear();
   clear_ptr_list(path_cells);
   path_cells.head(cll);
   ticks_till_next_stop = in_room = cur_fuel = max_fuel = 0;
   ticks_between_stops = 0;
}



void vehicle::showPaths(critter& pc) {
   Cell<PathCell*> cell(path_cells);
   PathCell* ptr;
   int i = 0;

   while ((ptr = cell.next())) {
      ptr->stat(getRoomNum(), i, pc);
      pc.show("\n");
      i++;
   }
}//showPaths

void vehicle::insertPathCell(int index, PathCell* pcell) {
   path_cells.insertAt(index, pcell);
}

PathCell* vehicle::hasPathCell(int path_cell_num) {
   return path_cells.elementAt(path_cell_num);
}

void vehicle::remPathCell(int path_cell_num) {//deletes it from memory
   PathCell* ptr = path_cells.removeAt(path_cell_num);
   if (ptr)
      delete ptr;

   //now, reset the cell pointing to the next direction...
   path_cells.head(cll);
   cll.next();
}

void vehicle::setPathPointer(int index) {
   if ((path_cells.size() > index) || (index < 0))
      index = 0;

   PathCell* ptr;
   path_cells.head(cll);
   int i = 0;

   while ((ptr = cll.next())) {
      if (i == index) {
         return;
      }//if
      i++;
   }//while
}//setPathPointer
   

void vehicle::Read(ifstream& da_file, short read_all) {
   Clear();
   room::Read(da_file, read_all);


   char tmp[81];
   //now, read in vehicle specific stuff...

   vehicle_flags.Read(da_file);

   da_file.getline(tmp, 80);

   int sentinel;
   PathCell* ptr;
   da_file >> sentinel;
   da_file.getline(tmp, 80);
   while ((sentinel != -1) && da_file) {
      ptr = new PathCell();
      ptr->Read(da_file);
      Put(ptr, path_cells);

      da_file >> sentinel;
      da_file.getline(tmp, 80);
   }

   path_cells.head(cll);
   cll.next();
   //da_file.getline(tmp, 80);

   da_file >> ticks_till_next_stop >> in_room >> cur_fuel >> max_fuel;
   da_file >> ticks_between_stops;
      
   da_file.getline(tmp, 80);
}//Read


void vehicle::Write(ofstream& da_file) {
   room::Write(da_file);
   vehicle_flags.Write(da_file);

   da_file << endl;

   Cell<PathCell*> pcell(path_cells);
   PathCell* ptr;
   while ((ptr = pcell.next())) {
      da_file << "1  Beginning of PathCell\n";
      ptr->Write(da_file);
   }

   da_file << "-1 End of PathCells..\n";

   da_file << ticks_till_next_stop << " " << in_room << " " << cur_fuel 
           << " " << max_fuel << " " << ticks_between_stops
           << "  End of Vehicle" << endl;
}//write

void vehicle::normalize() {
   room::normalize();
   path_cells.head(cll);
}//normalize_room
