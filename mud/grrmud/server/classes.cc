// $Id: classes.cc,v 1.9 1999/08/01 08:40:22 greear Exp $
// $Revision: 1.9 $  $Author: greear $ $Date: 1999/08/01 08:40:22 $

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

ContainedObject::~ContainedObject() {
   SafeList<ContainedObject*>* ptr;
   while (!contained_by.isEmpty()) {
      ptr = contained_by.popFront();
      ptr->privRemoveObject__((ContainedObject*)(this));
   }//while
}//destructor


int StatBonus::_cnt = 0;

String StatBonus::toString() const {
   String retval(100);
   Sprintf(retval, "stat: %i  bonus: %i", stat, bonus);
   return retval;
}
