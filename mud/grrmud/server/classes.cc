// $Id: classes.cc,v 1.8 1999/07/30 06:42:23 greear Exp $
// $Revision: 1.8 $  $Author: greear $ $Date: 1999/07/30 06:42:23 $

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
   return ((le == LE_ANY) || (getEntityType() == le));
}


int StatBonus::_cnt = 0;

String StatBonus::toString() const {
   String retval(100);
   Sprintf(retval, "stat: %i  bonus: %i", stat, bonus);
   return retval;
}
