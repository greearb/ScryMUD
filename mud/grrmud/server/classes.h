// $Id: classes.h,v 1.7 1999/07/18 21:16:18 greear Exp $
// $Revision: 1.7 $  $Author: greear $ $Date: 1999/07/18 21:16:18 $

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

///********************  classes.h  *****************************///



#ifndef __CLASSES_INCLUDE
#define __CLASSES_INCLUDE


#include <string2.h>
#include <bitfield.h>
#include <list2.h>
#include "script.h"

class critter;  //foward declaration


// This is used for reading in overloaded objects
enum class_id_enum {
   ROOM_CLASS_ID = 0,
   VEHICLE_CLASS_ID = 1
};//class_id_enum


class HegemonMarkup {
public:
   /**  Turn any hot characters, such as <,> into Hegemon
    * markup characters.  This will make this string safe for
    * inclusion in hegemon markup text.
    */
   static String makeSafeForHegTag(const char* string);
}; 

class ContainedObject;

class ObjectContainer {
private:
   List<ContainedObject*> inv;

public:
   int appendObject(ContainedObject* o) {
      inv.append(o);
      return TRUE;
   }

   int prependObject(ContainedObject* o) {
      inv.prepend(o);
      return TRUE;
   }

   ContainedObject* removeObject(ContainedObject* o) {
      return inv.loseData(o);
   }

   List<ContainedObject*>& getInv() { return inv; }
};//ObjectContainer


class ContainedObject {
private:
   ObjectContainer* contained_by;

public:
   ContainedObject() : contained_by(NULL) { };
   ContainedObject(ObjectContainer* ptr) : contained_by(ptr) { };

   int appendContainer(ObjectContainer* ptr) {
      if (ptr->appendObject(this)) {
         contained_by->removeObject(this);
         contained_by = ptr;
         return TRUE;
      }
      return FALSE;
   }//appendContainer

   int prependContainer(ObjectContainer* ptr) {
      if (ptr->prependObject(this)) {
         contained_by->removeObject(this);
         contained_by = ptr;
         return TRUE;
      }
      return FALSE;
   }//appendContainer

   ObjectContainer* getContainer() {
      return contained_by;
   }

};//ContainedObject


///************************  stat_cell  ************************///
 
class stat_spell_cell {
private:
   static int _cnt;

public:
   int stat_spell;
   int bonus_duration;
   
   stat_spell_cell() : stat_spell(0), bonus_duration(0) { _cnt++; }
   stat_spell_cell(int ss, int bd) :
         stat_spell(ss),
         bonus_duration(bd) { _cnt++; }
   stat_spell_cell(const stat_spell_cell& src) :
         stat_spell(src.stat_spell),
         bonus_duration(src.bonus_duration) { _cnt++; }
   ~stat_spell_cell() { _cnt--; }

   String toString() const ;

   static int getInstanceCount() { return _cnt; } //how many ZoneList objects exist?
};//stat_spell 
 


///***********************  PathCell  *****************************///

class PathCell {
private:
   String desc;
   String dir_to_next;
   int i_th_dir;
   int is_destination;
   static int _cnt;
   PathCell(const PathCell& src);

public:
   PathCell(String& description, String& direction_to_next, int is_dest);
   PathCell();

   ~PathCell() { _cnt--; }

   static int getInstanceCount() { return _cnt; } //how many ZoneList objects exist?

   void setDesc(String& description) { desc = description; }
   String& getDesc() { return desc; }
   String* getDescPtr() { return &desc; }

   void setDir(int i_th, const String& dir) { i_th_dir = i_th; dir_to_next = dir; }
   String& getDir() { return dir_to_next; }
   int getDirNum() const { return i_th_dir; }

   void setIsDest(int i);
   int isDest();

   //need the veh_num and cell_num to display appropriately.
   void stat(int veh_num, int cell_num, critter& pc);

   void Clear();
   void Read(ifstream& da_file);
   void Write(ofstream& da_file);
};//PathCell

#endif
