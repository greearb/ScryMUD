// $Id: vehicle.cc,v 1.8 1999/09/07 07:00:27 greear Exp $
// $Revision: 1.8 $  $Author: greear $ $Date: 1999/09/07 07:00:27 $

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
#include "Markup.h"

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
   path_cells.clearAndDestroy();
}

int vehicle::canEnter(room* dest_room, int do_msg) {
   int retval = FALSE;
   String buf(100);

   if (!(hasUnlimitedFuel() || (cur_fuel > 0))) {
      if (do_msg) {
         Sprintf(buf, "%S is out of fuel.\n", getShortDesc());
         showAllCept(buf);
      }//if
   }//if
   else if (!dest_room->isInUse()) { //if not used
      if (do_msg)
         showAllCept(CS_ROOM_DOESNT_EXIST);
   }//if
   else if (dest_room->isNoVehicle()) { //!vehicle
      if (do_msg)
         showAllCept(CS_NO_VEHICLES_ALLOWED);
   }//if
   else if (dest_room->needsBoat() && !(canFloat() || canFly())) {
      if (do_msg)
         showAllCept(CS_NO_TRAVEL_WATER);
   }//if
   else if ((dest_room->needsClimb()) && (!(canClimb() || canFly()))) {
      if (do_msg)
         showAllCept(CS_NO_TRAVEL_CLIMB);
   }//if
   else if ((dest_room->needsDive()) && (!canDive())) {
      if (do_msg)
         showAllCept(CS_NO_TRAVEL_DIVE);
   }//if
   else if ((dest_room->needsFly()) && (!canFly())) {
      if (do_msg)
         showAllCept(CS_NO_TRAVEL_FLY);
   }//if
   else if (dest_room->isZlocked()) { //is zlocked
      if (do_msg)
         showAllCept(CS_NO_TRAVEL_ZLOCKED);
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

void vehicle::toStringStat(critter* viewer, String& rslt, ToStringTypeE st) {
   String buf(1000);

   room::toStringStat(viewer, buf, st);
   rslt.append(buf);

   if (!viewer || viewer->isImmort()) {
      rslt.append("\n\t\t\tIS A VEHICLE\nVehicle flags:\n");
      Markup::toString("", vehicle_flags, VEHICLE_FLAGS_NAMES, viewer, "", buf);
      rslt.append(buf);

      Sprintf(buf, "ticks_till_next_stop:  %i, in_room:  %i, cur_fuel:  %i\n",
              ticks_till_next_stop, in_room, cur_fuel);
      rslt.append(buf);

      Sprintf(buf, "Max fuel:  %i, ticks_between_stops:  %i\n",
              max_fuel, ticks_between_stops);
      rslt.append(buf);
   }
}//toStringStat

int vehicle::move(int see_bit, int i_th, const String& exit_direction) {
   int dest;
   String buf(200);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In vehicle::move, in_room:  " << in_room 
             << "  see_bit:  " << see_bit << "  i_th:  " << i_th
             << " exit dir -:" << exit_direction << ":-" << endl;
   }

   ticks_till_next_stop = ticks_between_stops;

   door* exit_dr_ptr = room_list[in_room].findDoor(i_th, &exit_direction);

   //log("Got exit_dr_ptr.\n");

   if (!exit_dr_ptr) {
      //mudlog.log(WRN, "WARNING:  destination dr_ptr is NULL in move_room.\n");
      Sprintf(buf, "Your vehicle cannot go %S.\n", &exit_direction);
      showAllCept(buf);
      return FALSE;
   }//if

   dest = abs(exit_dr_ptr->getDestination());

   if (mudlog.ofLevel(DBG)) {
      mudlog << "got dest:  " << dest << endl;
   }

   if (!canEnter(&room_list[dest], TRUE)) {
      mudlog.log(WRN, 
                 "WARNING:  vehicle::can_enter failed in vehicle::move\n");
      return FALSE;
   }//if

   ticks_till_next_stop += exit_dr_ptr->getDistance();

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Passed tests, looking for doors to vehicle, size list:  "
             << room_list[in_room].DOORS.size() << endl;
   }

   /* passed all tests, lets move it!! */

   /* delete door(s) TO the car */
   SCell<door*> dcll(room_list[in_room].DOORS);
   door* tmp_ptr = NULL;

   door* dr_ptr = dcll.next();
   while (dr_ptr) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "Comparing dest:  " << abs(dr_ptr->getDestination())
                << "  and this room num:  " << getRoomNum() << endl;
      }
      if (abs(dr_ptr->getDestination()) == getRoomNum()) {
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

   if (!tmp_ptr->getDrData()) {
      mudlog.log(ERR, "ERROR:  dr_data is null.\n");
      return FALSE;
   }//if

   mudlog.log(DBG, "About to do messages.\n");

   dr_ptr = NULL;
   // lets find the door from the train to the outside.
   DOORS.head(dcll); //head of vehicle doors
   while ((dr_ptr = dcll.next())) {
      if (dr_ptr->getDrData() == tmp_ptr->getDrData()) {
         break;
      }
   }//while
      

   /* show messages to room, the one BEFORE the move */
   if (!isStealthy()) { //if not stealth
      if (isAtDestination() && tmp_ptr->isOpen() && tmp_ptr->canClose()) {
         if (dr_ptr) {
            Sprintf(buf, "The %S closes.\n", dr_ptr->getName());
            showAllCept(buf);
         }
         else {
            mudlog.log(ERR, "ERROR:  dr_ptr is NULL...\n");
         }
         
         Sprintf(buf, "The %S closes.\n", tmp_ptr->getName());
         room_list[in_room].showAllCept(buf);

         tmp_ptr->close(); //make it closed
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
      if (abs(dr_ptr->getDestination()) == abs(in_room)) { //if is_vehicle_exit
         if (dr_ptr->getDestination() < 0)
            dr_ptr->setDestination(-(abs(dest)));
         else
            dr_ptr->setDestination(abs(dest)); 
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
      if (dr_ptr->getDrData() == tmp_ptr->getDrData()) {
         break;
      }
   }//while

   if (isAtDestination()) { //in other words, the one its in NOW
      if (!isStealthy() && tmp_ptr->isClosed()) {
         if (dr_ptr) {
            Sprintf(buf, "The %S opens.\n", dr_ptr->getName());
            showAllCept(buf);
         }
         else {
            mudlog.log(ERR, "ERROR:  dr_ptr is NULL, end of move.\n");
         }

         if (tmp_ptr) {
            Sprintf(buf, "The %S opens.\n", tmp_ptr->getName());
            room_list[in_room].showAllCept(buf);
         }
         else {
            mudlog.log(ERR, "ERROR:  tmp_ptr is NULL, end of move.\n");
         }

      }//if
      tmp_ptr->open(); //make it open
   }//if

   // TODO:  Must break this out per passenger....
   //buf = getPassengerMessage();
   //if (buf.Strlen() > 0) {
   //   showAllCept(buf);
   //}//if

   if (vehicle_flags.get(7)) { //if can see out
      critter* cptr;
      SCell<critter*> ccll(CRITTERS);

      while ((cptr = ccll.next())) {
         do_look(1, &NULL_STRING, *cptr, room_list[in_room],
                 TRUE); /* ignore brief */
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


String& vehicle::getPassengerMessage(critter* viewer) {
   PathCell* ptr = cll.item();
   if (ptr)
      return *(ptr->getDesc(viewer));
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


void vehicle::clear() {
   room::clear();
   vehicle_flags.clear();
   path_cells.clearAndDestroy();
   path_cells.head(cll);
   ticks_till_next_stop = in_room = cur_fuel = max_fuel = 0;
   ticks_between_stops = 0;
}



void vehicle::showPaths(critter& pc) {
   Cell<PathCell*> cell(path_cells);
   PathCell* ptr;
   int i = 0;
   String buf(500);

   while ((ptr = cell.next())) {
      ptr->toStringStat(getRoomNum(), i, pc, buf, ST_ALL);
      pc.show(buf);
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
   

int vehicle::read(istream& da_file, int read_all) {
   vehicle::clear();

   mudlog << __FUNCTION__ << endl;

   room::read(da_file, read_all);

   char tmp[81];
   //now, read in vehicle specific stuff...

   mudlog << "About to read vehicle flags...\n";
   vehicle_flags.read(da_file);

   da_file.getline(tmp, 80);

   int sentinel;
   PathCell* ptr;
   da_file >> sentinel;
   da_file.getline(tmp, 80);
   while (sentinel != -1) {
      mudlog << "top of while, Reading path, sent: " << sentinel << endl;
      if (!da_file) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR: da_file bad in vehicle::read()\n" << endl;
         }
         ::core_dump(__FUNCTION__);
         return -1;
      }

      ptr = new PathCell();
      ptr->read(da_file);
      path_cells.append(ptr);

      da_file >> sentinel;
      mudlog << "bottom of while, Reading path, sent: " << sentinel << endl;
      da_file.getline(tmp, 80);
   }

   path_cells.head(cll);
   cll.next();
   //da_file.getline(tmp, 80);

   da_file >> ticks_till_next_stop >> in_room >> cur_fuel >> max_fuel;
   da_file >> ticks_between_stops;
      
   da_file.getline(tmp, 80);

   if (mudlog.ofLevel(DB)) {
      mudlog << "\nXXXXXXXX-----------**************-----------*******\n";
      String buf(500);
      toStringStat(NULL, buf, ST_ALL);
      mudlog << buf << endl;
      mudlog << "XXXXXXXX-----------**************-----------*******\n\n";
   }

   return 0;
}//read


int vehicle::write(ostream& da_file) {
   room::write(da_file);
   vehicle_flags.write(da_file);

   da_file << endl;

   Cell<PathCell*> pcell(path_cells);
   PathCell* ptr;
   while ((ptr = pcell.next())) {
      da_file << "1  Beginning of PathCell\n";
      ptr->write(da_file);
   }

   da_file << "-1 End of PathCells..\n";

   da_file << ticks_till_next_stop << " " << in_room << " " << cur_fuel 
           << " " << max_fuel << " " << ticks_between_stops
           << "  End of Vehicle" << endl;
   return 0;
}//write

void vehicle::normalize() {
   room::normalize();
   path_cells.head(cll);
}//normalize_room


int PathCell::_cnt = 0;

PathCell::PathCell(LString& description, String& direction_to_next,
                   int is_dest) {
   _cnt++;
   desc.addLstring(description);
   dir_to_next = direction_to_next;
   is_destination = is_dest;
}


PathCell::PathCell() {
   _cnt++;
   dir_to_next = "NOWHERE";
   is_destination = 0;
   LString ls(English, "GENERIC_DESC");
   desc.addLstring(ls);
   i_th_dir = 1;
}//constructor


void PathCell::clear() {
   desc.clear();
   dir_to_next.clear();
   is_destination = i_th_dir = 0;
}

int PathCell::read(istream& da_file, int read_all = TRUE) {
   char tmp[81];
   String buf(100);

   PathCell::clear();

   da_file >> buf;
   if (strcmp(buf, "<META") == 0) { //then do v3 style read
      mudlog << __FUNCTION__ << ": v_03\n";
      MetaTags mt(buf, da_file);
      desc.read(da_file);
      da_file >> is_destination >> i_th_dir >> dir_to_next;
      da_file.getline(tmp, 80);
   }
   else { //v02 read
      mudlog << __FUNCTION__ << ": v_02\n";
      if (strcmp(buf, "~") != 0) {
         String tmpb(100);
         tmpb.termedRead(da_file);
         tmpb.prepend(" ");
         tmpb.prepend(buf);
         desc.addString(English, buf);
      }
      else {
         String tmp2(" ");
         desc.addString(English, tmp2);
      }

      da_file >> is_destination >> i_th_dir >> dir_to_next;

      //   mudlog << "Is_dest:  " << is_destination << "  i_th:  " << i_th_dir
      //          << "  dir:  " << dir_to_next << endl;

      da_file.getline(tmp, 80);
   }
   return 0;
//   mudlog << "Junk:  -:" << tmp << ":-" << endl;
}//read


int PathCell::write(ostream& da_file) {
   MetaTags mt(*this);
   mt.write(da_file);

   desc.write(da_file);

   da_file << is_destination << " " << i_th_dir << " " << dir_to_next 
           << " is_dest, i_th, dir, End of PathCell..\n";
   return 0;
}//write


void PathCell::setIsDest(int i) {
   if (i)
      is_destination = 1;
   else 
      is_destination = 0;
}

int PathCell::isDest() {
   return is_destination;
}


void PathCell::toStringStat(int veh_num, int path_cell_num, critter& pc,
                            String& rslt, ToStringTypeE st) {
   String buf(500);

   if (pc.isUsingClient()) {
      Sprintf(rslt, "<PSTAT %i %i>", veh_num, path_cell_num);

      desc.toStringStat("<DESC>", "</DESC>", &pc, buf);
      rslt.append(buf);

      Sprintf(buf, "<PATH_DIR %i %S> ", i_th_dir, &dir_to_next);
      rslt.append(buf);
         
      Sprintf(buf, "<PATH_IS_DEST %i> </PSTAT>", is_destination);
      rslt.append(buf);
   }
   else {
      if ((st == ST_ALL) || (pc.isImmort())) {
         Sprintf(buf, "Path Cell [%i] for vehicle [%i].  Desc:\n",
                 path_cell_num, veh_num);
         rslt.append(buf);

         desc.toStringStat("", "", &pc, buf);
         rslt.append(buf);

         Sprintf(buf, "Direction:  i_th: %i  dir: %S\n", i_th_dir,
                 &dir_to_next);
         rslt.append(buf);
      
         if (is_destination) {
            pc.show("It IS a DESTINATION.\n");
         }
         else {
            pc.show("It is NOT a destination.\n");
         }
      }
   }//if it's a pc
}//stat
