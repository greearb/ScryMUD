// $Id: classes.cc,v 1.6 1999/07/28 05:57:05 greear Exp $
// $Revision: 1.6 $  $Author: greear $ $Date: 1999/07/28 05:57:05 $

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

///********************  classes.cc  *****************************///

#include "classes.h" 
#include "misc.h"
#include "misc2.h"
#include "room.h"
#include "const.h"
#include "commands.h"    //for wear_eq_effects...
#include "battle.h"
#include <PtrArray.h>
#include "command5.h"
#include "command4.h"





/** Static method */
String HegemonMarkup::makeSafeForHegTag(const char* str) {
   int len = strlen(str);
   char c;
   String retval(len + 50);

   for (int i = 0; i<len; i++) {
      c = str[i];
      if (c == '>') {
         retval += ">>";
      }
      else {
         retval += c;
      }
   }//for
   return retval;
}//convertToHeg

ObjectContainer::~ObjectContainer() {
   ContainedObject* ptr;
   while (!inv.isEmpty()) {
      ptr = inv.popFront();
      ptr->privRemoveFromContainer(this);
   }//while
}//destructor


int ObjectContainer::append(ContainedObject* o) {
   o->privAddToContainer(this);
   inv.append(o);
   return TRUE;
}

int ObjectContainer::prepend(ContainedObject* o) {
   o->privAddToContainer(this);
   inv.prepend(o);
   return TRUE;
}

int ObjectContainer::insertUnique(ContainedObject* o) {
   o->privAddToContainer(this);
   inv.gainData(o); //puts in unique copy
   return TRUE;
}

ContainedObject* ObjectContainer::remove(ContainedObject* o) {
   o->privRemoveFromContainer(this);
   return inv.loseData(o);
}


ContainedObject::~ContainedObject() {
   ObjectContainer* ptr;
   while (!contained_by.isEmpty()) {
      ptr = contained_by.popFront();
      ptr->privRemoveObject(this);
   }//while
}//destructor


int Entity::isEntityType(LEtypeE le) {
   return ((le == LE_ALL) || (getEntityType() == le));
}


int stat_spell_cell::_cnt = 0;

String stat_spell_cell::toString() const {
   String retval(100);
   Sprintf(retval, "stat_spell: %i  bonus_duration: %i", stat_spell,
           bonus_duration);
   return retval;
}


int PathCell::_cnt = 0;

PathCell::PathCell(String& description, String& direction_to_next,
                   int is_dest) {
   _cnt++;
   desc = description;
   dir_to_next = direction_to_next;
   is_destination = is_dest;
}


PathCell::PathCell() {
   _cnt++;
   dir_to_next = "NOWHERE";
   is_destination = 0;
   desc = "GENERIC_DESC";
   i_th_dir = 1;
}//constructor


void PathCell::Clear() {
   desc.Clear();
   dir_to_next.Clear();
   is_destination = i_th_dir = 0;
}

void PathCell::Read(ifstream& da_file) {
   char tmp[81];

   desc.Termed_Read(da_file); //description

//   mudlog << "PathCell:  got desc:  -:" << desc << ":-" << endl;

   da_file >> is_destination >> i_th_dir >> dir_to_next;

//   mudlog << "Is_dest:  " << is_destination << "  i_th:  " << i_th_dir
//          << "  dir:  " << dir_to_next << endl;

   da_file.getline(tmp, 80);
//   mudlog << "Junk:  -:" << tmp << ":-" << endl;
}//Read


void PathCell::Write(ofstream& da_file) {
   parse_for_max_80(desc);
   da_file << desc << "\n~\n";

   da_file << is_destination << " " << i_th_dir << " " << dir_to_next 
           << " is_dest, i_th, dir, End of PathCell..\n";
}//Write


void PathCell::setIsDest(int i) {
   if (i)
      is_destination = 1;
   else 
      is_destination = 0;
}

int PathCell::isDest() {
   return is_destination;
}


void PathCell::stat(int veh_num, int path_cell_num, critter& pc) {
   String buf(1000);

   if (pc.isPc()) {
      if (pc.isUsingClient()) {
         Sprintf(buf, "<PSTAT %i %i>", veh_num, path_cell_num);
         pc.show(buf);

         pc.show(" <PSTAT_LD>");
         pc.show(desc);
         pc.show("</STAT> ");

         Sprintf(buf, "<PATH_DIR %i %S> ", i_th_dir, &dir_to_next);
         pc.show(buf);
         

         Sprintf(buf, "<PATH_IS_DEST %i> ", is_destination);
         pc.show(buf);
      }

      Sprintf(buf, "Path Cell [%i] for vehicle [%i].  Desc:\n",
              path_cell_num, veh_num);
      pc.show(buf);
      pc.show(desc);
      Sprintf(buf, "Direction:  i_th: %i  dir: %S\n", i_th_dir,
              &dir_to_next);
      pc.show(buf);
      
      if (is_destination) {
         pc.show("It IS a DESTINATION.\n");
      }
      else {
         pc.show("It is NOT a destination.\n");
      }
   }//if it's a pc
}//stat
