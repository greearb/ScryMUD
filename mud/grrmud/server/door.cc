// $Id: door.cc,v 1.11 1999/09/06 02:24:27 greear Exp $
// $Revision: 1.11 $  $Author: greear $ $Date: 1999/09/06 02:24:27 $

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

door_data& door_data::operator= (door_data& source) {

   if (this == &source)
      return *this;

   Entity* hack = &source;
   *((Entity*)(this)) = *hack;
   *((Closable*)(this)) = (Closable)(source);
   
   return *this;
}//operator=


void door_data::clear() {
   Entity::clear();
   Closable::clear();
}//clear

SafeList<object*>& door_data::getInv() {
   ::core_dump("door_data::getInv called.\n");
   return dummy_inv;
}

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

String* door_data::getDirection(int dest, int c_bit = ~0, LanguageE lang = English) {
   KeywordEntry* ls_coll = getNamesCollection(lang);

   if (!ls_coll || ls_coll->isEmpty()) {
      return &UNKNOWN;
   }
   if (!detect(c_bit, getVisBit())) {
      return &SOMETHING;
   }//if

   if (dest >= 0) {
      return getFirstName(c_bit, lang);
   }//else
   else {
      return getLastName(c_bit, lang);
   }
}//getDirection

String* door_data::getName(critter* viewer, int dir) {
   return getName(viewer->getSeeBit(), viewer->getLanguage(), dir);
}

String* door_data::getName(critter* viewer) {
   return getName(viewer->getSeeBit(), viewer->getLanguage(), 0);
}

String* door_data::getName(int c_bit, LanguageE lang, int dest) {
   KeywordEntry* ls_coll = getNamesCollection(lang);

   if (!ls_coll || ls_coll->isEmpty()) {
      return &UNKNOWN;
   }

   Cell<LString*> cell(*ls_coll);
   String *str, *str2;

   if (!detect(c_bit, getVisBit())) {
      return &SOMETHING;
   }//if

   if (dest >= 0) {
      str = cell.next();
      str2 = cell.next();

      if (str2)
	 if (*str2 != "#")
	    return str2;
	 else
	    return str;
      else  // no specific name, just the direction
	 return str;
    }//if
    else {
      str = cell.prev();
      str2 = cell.prev();

      if (str2)
	 if (*(str2) != "#")
	    return str2;
	 else
	    return str;
      else  // no specific name, just the direction
	 return str;
   }//else
}//getName(critter* viewer, int direction)...



//*******************************************************//
//*********************** door **************************//
 
int door::_cnt = 0;

door::door() : dr_data(NULL), destination(0), distance(0),
               crit_blocking(NULL), ticks_till_disolve(-1),
               container(NULL) {
   _cnt++;
}//door constructor

door::door(door& source) : dr_data(source.dr_data),
                           destination(source.destination),
                           distance(source.distance),
                           crit_blocking(NULL), // Don't copy this. 
                           ticks_till_disolve(-1),
                           container(NULL) {
   _cnt++;
}//door copy constructor

door::~door() {
   affected_by.clearAndDestroy();
   container = NULL;
   _cnt--;
}//destructor


SpellDuration* door::isAffectedBy(int spell_num) {
   Cell<SpellDuration*> cll(affected_by);
   SpellDuration* ptr;

   while ((ptr = cll.next())) {
      if (ptr->spell == spell_num)
         return ptr;
   }//while
   return NULL;
}//is_affected_by

int door::affectedByToString(critter* viewer, String& rslt) {
   String buf(100);
   rslt.clear();
   if (!affected_by.isEmpty()) {
      rslt.append(cstr(CS_AFFECTED_BY, *viewer));
      Cell<SpellDuration*> cll(affected_by);
      SpellDuration* sd_ptr;
      while ((sd_ptr = cll.next())) {
         Sprintf(buf, "\t%s.\n", 
                 (const char*)(SSCollection::instance().getNameForNum(sd_ptr->spell)));
         rslt.append(buf);
      }//while
   }//if
   else {
      rslt.append(cstr(CS_NOT_AFFECTED_SPELLS, *viewer));
   }//else
   return 0;
}//affectedByToString


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
   affected_by.clearAndDestroy();
   container = NULL;
}//Clear()


door& door::operator= (door& source) {

   if (this == &source)
      return *this;
   
   affected_by.becomeDeepCopyOf(source.affected_by);

   //Shallow copy is preferred here.
   dr_data = source.dr_data;

   destination = source.destination;
   distance = source.distance;
   crit_blocking = source.crit_blocking;
   ticks_till_disolve = source.ticks_till_disolve;
   container = source.container;
   return *this;
}//operator=

int door::read(istream& da_file, int read_all = TRUE) {
   int data_index;
   int i, tmp;
   String buf(100);
   
   clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in door read." << endl;
      }
      return -1;
   }

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
      while (i != -1) { //affected by
         if (!da_file) {
            if (mudlog.ofLevel(ERR)) {
               mudlog << "ERROR:  da_file FALSE in door read." << endl;
            }
            return -1;
         }

         da_file >> tmp;
         SpellDuration* sd = new SpellDuration(i, tmp);
         addAffectedBy(sd);
         da_file >> i;
      }//while   
   }//if _v2
   else {
      MetaTags mt(buf, da_file);

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

      da_file >> i;
      while (i != -1) { //affected by
         if (!da_file) {
            if (mudlog.ofLevel(ERR)) {
               mudlog << "ERROR:  da_file FALSE in door read." << endl;
            }
            return -1;
         }

         da_file >> tmp;
         SpellDuration* sd = new SpellDuration(i, tmp);
         addAffectedBy(sd);
         da_file >> i;
      }//while   
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

   da_file << dr_data->getIdNum() << " " << destination << " "
           << distance << "dr_data#, dest, dist\n";

   Cell<SpellDuration*> cll(affected_by);
   SpellDuration* ptr;
   while ((ptr = cll.next())) {
      da_file << ptr->spell << " " << ptr->duration << " ";
   }
   da_file << "-1  //affected-by\n";
   return 0;
}//Write()

int door::nameIsSecret(const String* name) {
   Cell<KeywordEntry*> col_cell(dr_data->getNames());
   String* ptr;
   int len = name->Strlen();

   if (len == 0)
      return FALSE;

   KeywordEntry* coll;
   while ((coll = col_cell.next())) {
      Cell<LString*> cll(*coll);

      if (destination >= 0) { //if positive, go from top
         ptr = cll.next();
         while ((ptr = cll.next())) {
            if (*ptr == "#")
               break; // didn't find it
            if (strncasecmp(*name, *ptr, len) == 0)
               return TRUE;
         }//while
         return FALSE;
      }//if
      else {
         ptr = cll.prev();
         while ((ptr = cll.prev())) {
            if (*ptr == "#")
               break; // didn't find it
            if (strncasecmp(*name, *ptr, len) == 0)
               return TRUE;
         }//while
         return FALSE;
      }//else
   }//while
   return FALSE;
}//nameIsSecret



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
   KeywordEntry* col_ptr;

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
   String* ptr = NULL;
   if (destination < 0)
      ptr = dr_data->getLastName();
   else
      ptr = dr_data->getFirstName();
   
   if (ptr)
      return ptr;
   else
      return &UNKNOWN;
}//getDirection


const char* door::getAbrevDir() {
   String* dir;

   if (destination < 0) 
      dir = dr_data->getLastName();
   else
      dir = dr_data->getFirstName();

   if (dir == NULL) {
      return "??";
   }//if

   if (strcasecmp(*dir, "north") == 0) 
      return "N";
   else if (strcasecmp(*dir, "northwest") == 0)
      return "NW";
   else if (strcasecmp(*dir, "northeast") == 0)
      return "NE";
   else if (strcasecmp(*dir, "east") == 0)
      return "E";
   else if (strcasecmp(*dir, "south") == 0)
      return "S";
   else if (strcasecmp(*dir, "southeast") == 0)
      return "SE";
   else if (strcasecmp(*dir, "southwest") == 0)
      return "SW";
   else if (strcasecmp(*dir, "west") == 0)
      return "W";
   else if (strcasecmp(*dir, "up") == 0)
      return "U";
   else if (strcasecmp(*dir, "down") == 0)
      return "D";
   else return "??";
}//abbrev_dir_of_door
