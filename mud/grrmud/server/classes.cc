// $Id: classes.cc,v 1.13 1999/08/20 06:20:04 greear Exp $
// $Revision: 1.13 $  $Author: greear $ $Date: 1999/08/20 06:20:04 $

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
#include "SkillSpell.h"





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
      ContainedObject* tmp_ptr = this;
      ptr->privRemoveObject__(tmp_ptr);
   }//while
}//destructor


int StatBonus::_cnt = 0;

String StatBonus::toString() const {
   String retval(100);
   Sprintf(retval, "stat: %i  bonus: %i", stat, bonus);
   return retval;
}

SpellDuration* Entity::isAffectedBy(int spell_num) {
   Cell<SpellDuration*> cll(affected_by);
   SpellDuration* ptr;

   while ((ptr = cll.next())) {
      if (ptr->spell == spell_num)
         return ptr;
   }//while
   return NULL;
}//is_affected_by



int Entity::affectedByToString(critter* viewer, String& rslt) {
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

