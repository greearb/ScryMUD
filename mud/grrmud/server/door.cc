// $Id: door.cc,v 1.7 1999/08/16 07:31:24 greear Exp $
// $Revision: 1.7 $  $Author: greear $ $Date: 1999/08/16 07:31:24 $

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

//*********************************************************************// 
///**************************  struct door data   ********************///

#include "door.h"
#include "misc.h"
#include "misc2.h"
#include "commands.h"
#include "room.h"
#include <PtrArray.h>

int door_data::_cnt = 0;

door_data::door_data () : Entity(), Closable() { //default constructor
   _cnt++;
}//door_data constructor

door_data::door_data (const door_data& source) 
      : Entity((Entity)(source)),
        Closable((Closable)(source)) {
   _cnt++;
}//door

door_data::~door_data () {
   _cnt--;
} //destructor

door_data& door_data::operator= (const door_data& source) {

   if (this == &source)
      return *this;

   *((Entity*)(this)) = (Entity)(source);
   *((Closable*)(this)) = (Closable)(source);
   
   return *this;
}//operator=


void door_data::clear() {
   Entity::clear();
   Closable::clear();
}//clear

int door_data::read(istream& da_file, int read_all = TRUE) {
   String tmp_str(80);
   
   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in door_data read." << endl;
      }
      return -1;
   }

   da_file >> tmp_str;
   if (isnum(tmp_str)) { // _v2 read
      setVisBit(atoi(tmp_str) | 1024);

      int test;
      int key_num;
      int door_num;
      int in_zone;
      int token_num;

      da_file >> door_num >> token_num >> key_num >> in_zone;
      tmp_str.getLine(da_file, 80);

      setKey(key_num);
      setIdNum(door_num);
      setToken(token_num);
      setZoneNum(in_zone);

      bitfield door_data_flags;
      door_data_flags.read(da_file);
      door_data_flags.turn_on(10); //set in use flag no matter what
      Closable::setFlags(door_data_flags);

      setComplete();  //if we can read it, it's complete!
      
      test = TRUE;
      while (test) {
         if (!da_file) {
            if (mudlog.ofLevel(ERR)) {
               mudlog << "ERROR:  da_file FALSE in door_data read." << endl;
            }
            return -1;
         }
         
         da_file >> tmp_str;
         if (strcmp(tmp_str, "~") == 0) {
            test = FALSE;
         }//if
         else {
            addName(tmp_str);
         }//else
      }//while            
      tmp_str.getLine(da_file, 80);
   
      tmp_str.termedRead(da_file);
      addLongDesc(tmp_str);
   }// if _v2
   else {
      MetaTags mt(tmp_str, da_file);
      Entity::read(da_file, read_all);
      Closable::read(da_file, read_all);
   }
   return 0;
}//Read

void door_data::toStringStat(critter* viewer, String& rslt, ToStringTypeE st) {
   String buf(500);

   if (viewer->isUsingClient()) {
      Sprintf(rslt, "<DOOR_DATA %i>\n", getIdNum());
   }
   else {
      Sprintf(rslt, "Door Data: %i", getIdNum());
   }

   Entity::toStringStat(viewer, buf, st);
   rslt.append(buf);

   Closable::toStringStat(viewer, buf, st);
   rslt.append(buf);

   if (viewer->isUsingClient()) {
      rslt.append("</DOOR_DATA>\n");
   }
}//toStringStat


int door_data::write(ostream& da_file) {
   MetaTags mt(*this);
   mt.write(da_file);

   Entity::write(da_file);
   return Closable::write(da_file);
}//Write



//*******************************************************//
//*********************** door **************************//
 
int door::_cnt = 0;

door::door() : dr_data(NULL), destination(0), distance(0),
               crit_blocking(NULL), ticks_till_disolve(-1) {
   _cnt++;
}//door constructor

door::door(door& source) : dr_data(source.dr_data),
                           destination(source.destination),
                           distance(source.distance),
                           crit_blocking(NULL), // Don't copy this. 
                           ticks_till_disolve(-1) {
   _cnt++;
}//door copy constructor

door::~door() {
   _cnt--;
}//destructor

int door::getCurRoomNum() {
   if (getContainer()) {
      return getContainer()->getCurRoomNum();
   }
   if (mudlog.ofLevel(WRN)) {
      mudlog << "WARNING:  door::getCurRoomNum failed\n";
   }
   return 0;
}//getCurRoomNum


void door::clear() {
   dr_data = NULL; //do not delete data pointed too, its in static arrays
   crit_blocking = NULL;
   ticks_till_disolve = -1;
   Entity::clear();
}//Clear()


door& door::operator= (door& source) {

   if (this == &source)
      return *this;
   
   *((Entity*)(this)) = (Entity)(source);

   //Shallow copy is preferred here.
   dr_data = source.dr_data;

   destination = source.destination;
   distance = source.distance;
   crit_blocking = source.crit_blocking;
   ticks_till_disolve = source.ticks_till_disolve;
   return *this;
}//operator=

int door::read(istream& da_file, int read_all = TRUE) {
   int data_index;
   int i, tmp;
   String buf(100);
  
   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in door read." << endl;
      }
      return -1;
   }

   da_file >> buf;
   if (isnum(buf)) { //if _v2
      data_index = atoi(buf);
      if (!check_l_range(data_index, 0, NUMBER_OF_DOORS, mob_list[0], FALSE)) {
         mudlog.log(ERR, "ERROR:  door_index is out of range.\n");
         data_index = 1; //at least it won't crash this way
      }//if
      /* ok, got good data_index */
   
      dr_data = &(door_list[data_index]);
      da_file >> destination;
      da_file >> distance;
   
      /* comment this out for reading original, DB_UPGRADE */
      buf.getLine(da_file, 80);
      da_file >> i;
      int is_affected = FALSE;
      while (i != -1) { //affected by
         if (!da_file) {
            if (mudlog.ofLevel(ERR)) {
               mudlog << "ERROR:  da_file FALSE in door read." << endl;
            }
            return -1;
         }

         is_affected = TRUE;
         da_file >> tmp;
         SpellDuration* sd = new SpellDuration(i, tmp);
         addAffectedBy(sd);
         da_file >> i;
      }//while   
   }//if _v2
   else {
      MetaTags mt(buf, da_file);
      Entity::read(da_file, read_all);

      da_file >> data_index;
      if (!check_l_range(data_index, 0, NUMBER_OF_DOORS, mob_list[0], FALSE)) {
         mudlog.log(ERR, "ERROR:  door_index is out of range.\n");
         data_index = 1; //at least it won't crash this way
      }//if
      /* ok, got good data_index */
   
      dr_data = &(door_list[data_index]);
      da_file >> destination;
      da_file >> distance;
      buf.getLine(da_file, 80);

   }//else, _v3
   
   if (isAffected()) {
      door* hack = this;
      affected_doors.appendUnique(hack);
   }
   return 0;
}// read()


void door::toStringStat(critter* viewer, String& rslt, ToStringTypeE st) {
   String buf(500);

   if (viewer->isUsingClient()) {
      rslt = "<DOOR>\n";
   }
   else {
      rslt = "Door:\n";
   }

   if (viewer->isImmort()) {
      Sprintf(buf, "Destination: %i  Distance: %i  Ticks-till-Disolve: %i\n",
              destination, distance, ticks_till_disolve);
      rslt.append(buf);

      if (crit_blocking) {
         Sprintf(buf, "Blocked by: %S\n", crit_blocking->getName(viewer));
         rslt.append(buf);
      }
   }

   if (dr_data) {
      // This is where the interesting stuff is kept.
      dr_data->toStringStat(viewer, buf, st);
      rslt.append(buf);
   }
   if (viewer->isUsingClient()) {
      rslt.append("</DOOR>\n");
   }
}//toStringStat


int door::write(ostream& da_file) {
   MetaTags mt(*this);
   mt.write(da_file);

   Entity::write(da_file);

   da_file << dr_data->getIdNum() << " " << destination << " "
           << distance << "dr_data#, dest, dist\n";
   return 0;
}//Write()



   ///******************  Static Functions ***************************///

/* static */
door* door::findDoor(SafePtrList<door> &lst, const int i_th,
                     const String* name, critter* viewer) {
   int foo;
   return findDoor(lst, i_th, name, foo, viewer);
}

door* door::findDoor(SafePtrList<door> &lst, const int i_th,
                     const String* name, int& count_sofar,
                     critter* viewer) {

   Cell<LString*> names_cll;
   SCell<door*> cell(lst);
   door* door_ptr;
   int count = 0, ptr_v_bit, len;
   LString *string;
   LStringCollection* col_ptr;

   //log("in have_door_named");

   int rm_vis_bit = 0;
   LanguageE lang = English;
   int pc_see_bit = ~0;
   if (viewer) {
      rm_vis_bit = viewer->getCurRoom()->getVisBit();
      lang = viewer->getLanguage();
      pc_see_bit = viewer->getSeeBit();
   }

   if (!name) {
      mudlog.log(WRN, "ERROR:  name is NULL in have_door_named.\n");
      return NULL;
   }//if

   if ((len = name->Strlen()) == 0) {
      mudlog.log(WRN, "WARNING:  len was zero in have_door_named.\n");
      return NULL;
   }//if


   while ((door_ptr = cell.next())) {
      ptr_v_bit = (door_ptr->getVisBit() | rm_vis_bit); 
      if (detect(pc_see_bit, ptr_v_bit)) {
         col_ptr = door_ptr->dr_data->getNamesCollection(lang);
         col_ptr->head(names_cll);
	 if (door_ptr->destination < 0) {
	    string = names_cll.prev(); 
	    while (*string != "#") {
	       if (strncasecmp(*string, *name, len) == 0) {
		  count++;
                  count_sofar++;
		  if (count == i_th) {
		     return door_ptr;
		  }//if
                  break;
	       }//if
	       string = names_cll.prev();
	    }//while
	 }//if
	 else {
	    while ((string = names_cll.next())) {
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

door* door::findDoorByDest(SafePtrList<door>& lst, int dest_room) {
   SCell<door*> cll(lst);
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
