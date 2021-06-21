// $Id$
// $Revision$  $Author$ $Date$

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
//To contact the maintainer, Edward Roper: edro+scrymud [at] wanfear.net
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
   ticks_at_stops = ticks_between_stops = 0;
   veh_stopped.Clear();
}

vehicle::vehicle(int num) : room(num) {
   path_cells.head(cll);
   cll.next(); //move to first one.

   ticks_till_next_stop = in_room = cur_fuel = max_fuel = 0;
   ticks_at_stops = ticks_between_stops = 0;
   veh_moving.Clear();
}


vehicle::~vehicle() {
   path_cells.clearAndDestroy();
}

int vehicle::canEnter(const room* dest_room, int do_msg) const {
   int retval = FALSE;
   String buf(100);

   if (!(hasUnlimitedFuel() || (cur_fuel > 0))) {
      if (do_msg) {
         Sprintf(buf, "%pS is out of fuel.\n", &(short_desc));
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
   String buf(200);
   room::stat(pc);

   pc.show("\n\t\t\tIS A VEHICLE\n");

   Sprintf(buf, "In Room Stopped:\n%pS\n", &veh_stopped);
   pc.show(buf);
   Sprintf(buf, "In Room Moving:\n%pS\n", &veh_moving);
   pc.show(buf);

   pc.show("Vehicle flags:\n");
   out_field(vehicle_flags, pc, VEHICLE_FLAGS_NAMES);

   Sprintf(buf, "ticks_till_next_stop:  %i, in_room:  %i, cur_fuel:  %i\n",
           ticks_till_next_stop, in_room, cur_fuel);
   show(buf, pc);

   Sprintf(buf, "Max fuel:  %i, ticks_between_stops:  %i, ticks_at_stops:  %i\n",
           max_fuel, ticks_between_stops, ticks_at_stops);
   show(buf, pc);
}//stat

int vehicle::move(int see_bit, int i_th, const String& exit_direction) {
   int dest;
   int i_th_incr = 0;
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

   //if our vehicle creates a south in room and then tries to go south we end
   //up self-containing. Now of course one can set i_th in the path, but then
   //again, this gets ridiculously annoying on very long paths. It's also
   //highly prone to human error. In the event that we attempt to
   //self-contain, walk through all the doors (increasingly from i_th as
   //passed) until we (hopefully) find one that isn't us. In the event that we
   //fail to locate one, things should work as they did before (see below).
   //Please note that we only attempt doors in the direction requested.
   while ( exit_dr_ptr && ( abs(exit_dr_ptr->destination) == getRoomNum() ) ) {
      exit_dr_ptr = door::findDoor(room_list[in_room].DOORS, i_th+i_th_incr,
            &exit_direction, see_bit, room_list[in_room].getVisBit());
      i_th_incr++;
   }

   if (!exit_dr_ptr) {
      mudlog.log(WRN, "WARNING:  destination dr_ptr is NULL in move_room.\n");
      Sprintf(buf, "Your vehicle cannot go %pS.\n", &exit_direction);
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
      mudlog.log(LS_ERROR, "ERROR:  tmp_ptr is NULL in vehicle::move()\n");
      return FALSE;
   }//if

   if (!tmp_ptr->dr_data) {
      mudlog.log(LS_ERROR, "ERROR:  dr_data is null.\n");
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
          tmp_ptr->dr_data->isCloseable()) {
         if (dr_ptr) {
            Sprintf(buf, "The %pS closes.\n", name_of_door(*dr_ptr, ~0));
            showAllCept(buf);
         }
         else {
            mudlog.log(LS_ERROR, "ERROR:  dr_ptr is NULL...\n");
         }
         
         Sprintf(buf, "The %pS closes.\n", name_of_door(*tmp_ptr, ~0));
         room_list[in_room].showAllCept(buf);

         tmp_ptr->dr_data->close(); //make it closed
      }//if

      Sprintf(buf, "%pS leaves towards the %pS.\n", &(short_desc),
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


   mudlog.log(LS_ERROR, "Vehicle door updated.\n");

             /* add door to destination room */
   room_list[in_room].DOORS.prepend(tmp_ptr);

   advancePathIterator(); /* advance the internal state, let this object
                             know that it's actually been moved. */

   mudlog.log(LS_ERROR, "In new room..doing messages.\n");

   /* now in new room, do some messages if needed */
   if (!isStealthy()) { //if not stealty
      Sprintf(buf, "%pS has arrived.\n", &(short_desc));
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

   buf = getPassengerMessage();
   if (buf.Strlen() > 0) {
      showAllCept(buf);
   }//if

   if (vehicle_flags.get(7)) { //if can see out
      critter* cptr;
      Cell<critter*> ccll(CRITTERS);

      while ((cptr = ccll.next())) {
         do_look(1, &NULL_STRING, *cptr, room_list[in_room],
                 TRUE); /* ignore brief */
      }//while
   }//if can see out

   if (isAtDestination()) { //in other words, the one its in NOW

      //Eventually I'd like this configurable on the vehicle itself. If it's
      //not here and we have a relatively fast vehicle it's close to
      //impossible to catch it with the door open.
      ticks_till_next_stop += 15;

      if (!isStealthy() && tmp_ptr->dr_data->isClosed() &&
            tmp_ptr->dr_data->isOpenable() ) {
         if (dr_ptr) {
            Sprintf(buf, "The %pS opens.\n", name_of_door(*dr_ptr, ~0));
            showAllCept(buf);
         }
         else {
            mudlog.log(LS_ERROR, "ERROR:  dr_ptr is NULL, end of move.\n");
         }

         if (tmp_ptr) {
            Sprintf(buf, "The %pS opens.\n", name_of_door(*tmp_ptr, ~0));
            room_list[in_room].showAllCept(buf);
         }
         else {
            mudlog.log(LS_ERROR, "ERROR:  tmp_ptr is NULL, end of move.\n");
         }

      }//if
      tmp_ptr->dr_data->open(); //make it open
   }//if

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
int vehicle::canClimb() const { return (vehicle_flags.get(4) || canFly()); }
int vehicle::canFloat() const { return (vehicle_flags.get(5) || canFly()); }
int vehicle::canDive() const { return vehicle_flags.get(6); }
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
   veh_stopped.Clear();
   veh_moving.Clear();
   path_cells.clearAndDestroy();
   path_cells.head(cll);
   ticks_till_next_stop = in_room = cur_fuel = max_fuel = 0;
   ticks_between_stops = 0;
   ticks_at_stops = 0;
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
   
// Must use the number as it's stored in the text files for veh_num
#ifdef USEMYSQL
void vehicle::dbRead(int veh_num, short read_all) {
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select * from Vehicles where ROOM_NUM=";
   query+=veh_num;

   Clear();
   room::dbRead(veh_num, read_all);
   cur_stats[2] = cur_stats[2] & ~(0x01000000);

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In vehicle::dbRead(int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      row=mysql_fetch_row(result);

      in_room = atoi(row[VEHTBL_IN_ROOM]);
      cur_fuel = atoi(row[VEHTBL_CUR_FUEL]);
      max_fuel = atoi(row[VEHTBL_MAX_FUEL]);
      ticks_between_stops = atoi(row[VEHTBL_TICKS_BETWEEN_STOPS]);
      ticks_till_next_stop = ticks_between_stops;

      vehicle_flags.set(VEHFLAG_SELF_GUIDED, atoi(row[VEHTBL_SELF_GUIDED]));
      vehicle_flags.set(VEHFLAG_STEALTHY, atoi(row[VEHTBL_STEALTHY]));
      vehicle_flags.set(VEHFLAG_UNLIMITED_FUEL, atoi(row[VEHTBL_UNLIMITED_FUEL]));
      vehicle_flags.set(VEHFLAG_FLIES, atoi(row[VEHTBL_FLIES]));
      vehicle_flags.set(VEHFLAG_CLIMBS, atoi(row[VEHTBL_CLIMBS]));
      vehicle_flags.set(VEHFLAG_FLOATS, atoi(row[VEHTBL_FLOATS]));
      vehicle_flags.set(VEHFLAG_DIVES, atoi(row[VEHTBL_DIVES]));
      vehicle_flags.set(VEHFLAG_CAN_SEE_OUT, atoi(row[VEHTBL_CAN_SEE_OUT]));

      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In vehicle::dbRead(int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query = "select * from VehiclePaths where ROOM_NUM = " + (cur_stats[2] | 0x01000000);
   if (mysql_real_query(database, query, query.Strlen())==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In vehicle::dbRead():\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      while((row=mysql_fetch_row(result))) {
         PathCell* ptr = new PathCell;
         String desc(row[VPATHTBL_DESCRIPTION]);
         ptr->setDesc(desc);
         ptr->setDir(atoi(row[VPATHTBL_I_TH]), row[VPATHTBL_DIR]);
         ptr->setIsDest(atoi(row[VPATHTBL_IS_DESTINATION]));
         path_cells.append(ptr);
      }
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In vehicle::dbRead():\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }
}
#endif

void vehicle::fileRead(ifstream& da_file, short read_all) {
   Clear();
   room::fileRead(da_file, read_all);
   int format_version = 0;


   char tmp[81];
   //now, read in vehicle specific stuff...

   // look for a file-version identifier
   {
      char tst_char;
      da_file.get(tst_char);
      if ( tst_char != '%' ) {
         // no file version 
         da_file.putback(tst_char);
         format_version = 0;
      } else {
         da_file >> format_version;
         da_file.getline(tmp, 80);
      }
   }

   if ( format_version > 0 ) {
      veh_stopped.termedRead(da_file);
      veh_moving.termedRead(da_file);
   }

   vehicle_flags.Read(da_file);

   da_file.getline(tmp, 80);

   int sentinel;
   PathCell* ptr;
   da_file >> sentinel;
   da_file.getline(tmp, 80);
   while ((sentinel != -1) && da_file) {
      ptr = new PathCell();
      ptr->Read(da_file);
      path_cells.append(ptr);

      da_file >> sentinel;
      da_file.getline(tmp, 80);
   }

   path_cells.head(cll);
   cll.next();
   //da_file.getline(tmp, 80);

   da_file >> ticks_till_next_stop >> in_room >> cur_fuel >> max_fuel;
   da_file >> ticks_between_stops;
   
   if ( format_version > 0 ) {
      da_file >> ticks_at_stops;
   }
      
   da_file.getline(tmp, 80);
}//Read

#ifdef USEMYSQL
void vehicle::dbWrite() {
   String query;
   String fields;
   String values;
   cur_stats[2] = cur_stats[2] | 0x01000000;
   room::dbWrite();

   query = "insert into vehicles ";
   fields = "(ROOM_NUM, IN_ROOM, TICKS_BETWEEN_STOPS, CUR_FUEL, MAX_FUEL, SELF_GUIDED, STEALTHY, UNLIMITED_FUEL, FLIES, CLIMBS, FLOATS, DIVES, CAN_SEE_OUT)";

   values  = "values (";
   values += cur_stats[2] + ", ";
   values += in_room + ", ";
   values += ticks_between_stops + ", ";
   values += cur_fuel + ", ";
   values += max_fuel + ", ";
   values += vehicle_flags.get(0) + ", ";
   values += vehicle_flags.get(1) + ", ";
   values += vehicle_flags.get(2) + ", ";
   values += vehicle_flags.get(3) + ", ";
   values += vehicle_flags.get(4) + ", ";
   values += vehicle_flags.get(5) + ", ";
   values += vehicle_flags.get(6) + ", ";
   values += vehicle_flags.get(7) + ")";

   query += fields + values;

   if (mysql_real_query(database, query, query.Strlen())!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In vehicle::dbWrite():\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query = "insert into VehiclePaths ";
   fields = "ROOM_NUM, STOP_NUM, DESCRIPTION, DIR, I_TH, IS_DESTINATION ";
   values = "values ";

   Cell<PathCell*> pcell(path_cells);
   PathCell* ptr;
   char* buf;
   int i=0;
   while ((ptr = pcell.next())) {
      buf = (char*) malloc(ptr->getDesc().Strlen()*2+1);
      mysql_escape_string(buf, ptr->getDesc(), ptr->getDesc().Strlen());
      values += "(";
      values += cur_stats[2] + ", ";
      values += i + ", ";
      values += *"'" + String(buf) + *"', ";
      values += *"'" + ptr->getDir() + *"', ";
      values += ptr->getDirNum() + *"', ";
      values += ptr->isDest() + *"), ";
      i++;
      free(buf);
   }

   query += fields + values;

   cur_stats[2] = cur_stats[2] & ~(0x01000000);
}
#endif


void vehicle::fileWrite(ofstream& da_file) {
   room::fileWrite(da_file);

   da_file << "%" << VEHICLE_FORMAT_VERSION << "\tvehicle format version"
      << endl;

   parse_for_max_80(veh_stopped);
   da_file << veh_stopped << endl << "~" << endl;
   parse_for_max_80(veh_moving);
   da_file << veh_moving << endl << "~" << endl;

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
           << " " << max_fuel << " " << ticks_between_stops << " "
           << ticks_at_stops
           << "  End of Vehicle" << endl;
}//write

void vehicle::normalize() {
   room::normalize();
   path_cells.head(cll);
}//normalize_room
