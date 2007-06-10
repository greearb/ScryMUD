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

//*********************************************************************// 
///**************************  struct door data   ********************///

#include "door.h"
#include "misc.h"
#include "misc2.h"
#include "commands.h"
#include "room.h"
#include <PtrArray.h>

int door_data::_cnt = 0;

door_data::door_data () { //default constructor
   _cnt++;
   Clear();
}//door_data constructor

door_data::door_data (const door_data& source) {
   _cnt++;
   *this = source; 
}//door

door_data::~door_data () {
   _cnt--;
   Clear();
} //destructor

door_data& door_data::operator= (const door_data& source) {

   if (this == &source)
      return *this;

   vis_bit = source.vis_bit;
   door_num = source.door_num;
   token_num = source.token_num;
   key_num = source.token_num;
   in_zone = source.in_zone;
   
   door_data_flags = source.door_data_flags; //bitfield overloaded =

   names.becomeDeepCopyOf(source.names);
   
   long_desc = source.long_desc;
   return *this;
}//operator=


void door_data::Clear() {
   vis_bit = door_num = key_num = token_num = in_zone = 0;
   names.clearAndDestroy();
   long_desc.Clear();
   door_data_flags.off_all();
}//clear

#ifdef USEMYSQL
void door_data::dbRead(int door_num) {
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select * from Doors where DOOR_NUM=";
   query += door_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In door_data::dbRead(int):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      row=mysql_fetch_row(result);

      vis_bit = atoi(row[DOORTBL_VIS_BIT]);
      door_num = atoi(row[DOORTBL_DOOR_NUM]);
      token_num = atoi(row[DOORTBL_TOKEN_NUM]);
      in_zone = atoi(row[DOORTBL_IN_ZONE]);

      vis_bit |= 1024;

      door_data_flags.set(DDFLAG_OPEN_EXIT, atoi(row[DOORTBL_OPEN_EXIT]));
      door_data_flags.set(DDFLAG_MAGIC_LOCKABLE, atoi(row[DOORTBL_MAGIC_LOCKABLE]));
      door_data_flags.set(DDFLAG_CLOSED, atoi(row[DOORTBL_CLOSED]));
      door_data_flags.set(DDFLAG_LOCKED, atoi(row[DOORTBL_LOCKED]));
      door_data_flags.set(DDFLAG_PICKABLE, atoi(row[DOORTBL_PICKABLE]));
      door_data_flags.set(DDFLAG_LOCKABLE, atoi(row[DOORTBL_LOCKABLE]));
      door_data_flags.set(DDFLAG_MAG_LOCKED, atoi(row[DOORTBL_MAG_LOCKED]));
      door_data_flags.set(DDFLAG_DESTRUCTABLE, atoi(row[DOORTBL_DESTRUCTABLE]));
      door_data_flags.set(DDFLAG_CLOSABLE, atoi(row[DOORTBL_CLOSABLE]));
      door_data_flags.set(DDFLAG_FLIPPABLE, atoi(row[DOORTBL_FLIPPABLE]));
      door_data_flags.set(DDFLAG_PC_CANT_OPEN, atoi(row[DOORTBL_PC_CANT_OPEN]));
      door_data_flags.set(DDFLAG_VEHICLE_EXIT, atoi(row[DOORTBL_VEHICLE_EXIT]));
      door_data_flags.set(DDFLAG_SECRET, atoi(row[DOORTBL_SECRET]));
      door_data_flags.set(DDFLAG_BLOCKED, atoi(row[DOORTBL_BLOCKED]));
      door_data_flags.set(DDFLAG_SECRET_WHEN_OPEN, atoi(row[DOORTBL_SECRET_WHEN_OPEN]));
      door_data_flags.set(DDFLAG_CONSUMES_KEY, atoi(row[DOORTBL_CONSUMES_KEY]));
      door_data_flags.set(DDFLAG_NO_PASSDOOR, atoi(row[DOORTBL_NO_PASSDOOR]));

      door_data_flags.turn_on(DDFLAG_IN_USE);
      setComplete();
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In door_data::dbRead(int):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   // names
   query = "select NAME from DoorNames where DOOR_NUM=";
   query += door_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In door_data::dbRead(int):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      while ((row=mysql_fetch_row(result))) {
         names.append(new String(row[0]));
      }
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In door_data::dbRead(int):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }
}
#endif

void door_data::fileRead(ifstream& da_file) {
   char tmp[81];
   String tmp_str(80);
   String* string;
   int test = TRUE;
   
   Clear();
   
   if (!da_file) {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  da_file FALSE in door_data read." << endl;
      }
      return;
   }

   da_file >> vis_bit >> door_num >> token_num >> key_num >> in_zone;
   da_file.getline(tmp, 80);         
   
   vis_bit |= 1024; //hack, forgot blindness bit :P
   
   door_data_flags.Read(da_file);
   door_data_flags.turn_on(10); //set in use flag no matter what

   setComplete();  //if we can read it, it's complete!

   test = TRUE;
   while (test) {
      if (!da_file) {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  da_file FALSE in door_data read." << endl;
         }
         return;
      }

      da_file >> tmp_str;
      if (strcmp(tmp_str, "~") == 0) {
         test = FALSE;
      }//if
      else {
         string = new String(tmp_str);
         names.append(string);
      }//else
   }//while            
   da_file.getline(tmp, 80);         
   
   long_desc.Termed_Read(da_file);
}//Read


void door_data::Write(ofstream& da_file) {
   Cell<String*> cll(names);
   String* ptr;
   
   da_file << vis_bit << " " << door_num << " "
           << token_num << " " << key_num << " " << in_zone
           << "\tvbit, door#, token#, key#\n";
   
   door_data_flags.Write(da_file);
   
   int len = 0;
   while ((ptr = cll.next())) {
      len += ptr->Strlen() + 1;
      if (len > 79) {
         da_file << endl;
         len = 0;
      }//if
      da_file << *ptr << " ";
   }//while            
   da_file << "~" << "\tnames\n";         
   
   parse_for_max_80(long_desc);
   da_file << long_desc << endl << "~" << endl;
}//Write



//*******************************************************//
//*********************** door **************************//
 
int door::_cnt = 0;

door::door() {
   _cnt++;
   dr_data = NULL;
   crit_blocking = NULL;
   destination = in_room = 0;
   distance = 0; /* number of battle rounds it takes to move
                    can be used to slow ppl/vehicles down when logical */
   ticks_till_disolve = -1; // don't disolve
}//door constructor

door::door(const door& source) {
   _cnt++;
   dr_data = NULL;
   crit_blocking = NULL;
   *this = source;
   ticks_till_disolve = -1;
}//door copy constructor

door::~door() {
   _cnt--;
   affected_doors.loseData(this);
   Clear();
}//destructor

void door::Clear() {
   dr_data = NULL; //do not delete data pointed too, its in static arrays
   crit_blocking = NULL;
   destination = in_room = 0;
   distance = 0;
   affected_by.clearAndDestroy();
   ticks_till_disolve = -1;
}//Clear()


door& door::operator= (const door& source) {

   if (this == &source)
      return *this;

   Clear();
   
   if (!source.dr_data) {
      mudlog.log(ERROR, "ERROR:  assigning with empty door as source.\n");
      return *this;
   }//if
   
   if (!dr_data) {
      dr_data = new door_data(*(source.dr_data));
   }//if 
   else {
      *dr_data = *(source.dr_data);
   }//else
   destination = source.destination;
   distance = source.distance;
   
   Cell<stat_spell_cell*> cll(source.affected_by);
   stat_spell_cell* ptr;
   while ((ptr = cll.next())) {
      affected_by.append(new stat_spell_cell(*ptr));
   }//while
   
   crit_blocking = source.crit_blocking;
   ticks_till_disolve = source.ticks_till_disolve;
   in_room = source.in_room;
   return *this;
}//operator=

void door::Read(ifstream& da_file) {
   int data_index;
   int i, tmp;
   char buf[82];
  
   Clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  da_file FALSE in door read." << endl;
      }
      return;
   }

   da_file >> data_index;
   if (!check_l_range(data_index, 0, NUMBER_OF_DOORS, mob_list[0], FALSE)) {
      mudlog.log(ERROR, "ERROR:  door_index is out of range.\n");
      data_index = 1; //at least it won't crash this way
   }//if
   /* ok, got good data_index */
   
   dr_data = &(door_list[data_index]);
   da_file >> destination;
   da_file >> distance;
   
   /* comment this out for reading original, DB_UPGRADE */
   da_file.getline(buf, 80);
   da_file >> i;
   int is_affected = FALSE;
   while (i != -1) { //affected by
      if (!da_file) {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  da_file FALSE in door read." << endl;
         }
         return;
      }

      is_affected = TRUE;
      da_file >> tmp;
      affected_by.append(new stat_spell_cell(i, tmp));
      da_file >> i;
   }//while
   
   if (is_affected) {
      affected_doors.gainData(this);
   }

   da_file.getline(buf, 80);
}// Read()


void door::Write(ofstream& da_file) {
   int count = 0;
   
   da_file << dr_data->door_num << " " << destination << " "
           << distance << " door# dest# distance\n";
   
   Cell<stat_spell_cell*> cll(affected_by);
   stat_spell_cell* ptr;
   while ((ptr = cll.next())) {
      da_file << ptr->stat_spell << " " << ptr->bonus_duration << " ";
      if (++count > 20) {
         da_file << endl;
         count = 0;
      }//if
   }//while
   da_file << -1 << "\tdoor Affected_Bye (end of door)\n";
}//Write()



   ///******************  Static Functions ***************************///

/* static */
door* door::findDoor(const PtrList<door> &lst, const int i_th,
                     const String* name, const int see_bit,
                     const room& rm) {
   int foo = 0;
   return findDoor(lst, i_th, name, see_bit, rm, foo);
}

door* door::findDoor(const PtrList<door> &lst, const int i_th,
                     const String* name, const int see_bit,
                     const room& rm, int& count_sofar) {
   return door::findDoor(lst, i_th, name, see_bit, rm.getVisBit(), count_sofar);
}

door* door::findDoor(const PtrList<door> &lst, const int i_th,
                     const String* name, const int see_bit,
                     const int rm_vis_bit) {
   int foo = 0;
   return findDoor(lst, i_th, name, see_bit, rm_vis_bit, foo);
}

/* static */
door* door::findDoor(const PtrList<door> &lst, const int i_th,
                     const String* name, const int see_bit,
                     const int rm_vis_bit, int& count_sofar) {
   Cell<String*> char_cell;
   Cell<door*> cell(lst);
   door* door_ptr;
   int count = 0, ptr_v_bit, len;
   String *string;

   if ((len = name->Strlen()) == 0) {
      //mudlog.log(WRN, "WARNING:  len was zero in have_door_named.\n");
      return NULL;
   }//if

   while ((door_ptr = cell.next())) {
      ptr_v_bit = (door_ptr->DOOR_VIS_BIT | rm_vis_bit); 
      if (detect(see_bit, ptr_v_bit)) {
         door_ptr->dr_data->names.head(char_cell);
         if (door_ptr->destination < 0) {
            string = char_cell.prev();
            if (string) {
               while (*string != "#") {
                  if (strncasecmp(*string, *name, len) == 0) {
                     count++;
                     count_sofar++;
                     if (count == i_th) {
                        return door_ptr;
                     }//if
                     break;
                  }//if
                  string = char_cell.prev();
               }//while
            }
         }//if
         else {
            while ((string = char_cell.next())) {
               if (*string == "#") {
                  break;
               }//if
               else {
                  if (strncasecmp(*string, *name, name->Strlen()) == 0) {
                     count++;
                     count_sofar++;
                     if (count == i_th) {
                        return door_ptr;
                     }//if
                     break;
                  }//if
               }//else
            }//while
         }//if
      }//if
   }//while
   return NULL;
}//haveDoor

door* door::findDoorByDest(const PtrList<door>& lst, int dest_room) {
   Cell<door*> cll(lst);
   door* ptr;

   while ((ptr = cll.next())) {
      if (abs(ptr->destination) == dest_room) {
         return ptr;
      }//if
   }//while
   return ptr;
}

room* door::getDestRoom() {
   return room_list.elementAtNoCreate(bound(0, NUMBER_OF_ROOMS,
                                            abs(destination)));
}//getDestRoom


String* door::getDirection() {
   return direction_of_door(*this);
}


stat_spell_cell* door::isAffectedBy(int spell_num) {
   Cell<stat_spell_cell*> cll(affected_by);
   stat_spell_cell* sp;
   while ((sp = cll.next())) {
      if (sp->stat_spell == spell_num) {
         return sp;
      }
   }//while
   return NULL;
}

