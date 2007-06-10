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

///********************  Filters.cc  *****************************///

#include "Filters.h"
#include "misc2.h"
#include "misc.h"


/** returns true if pc cannot detect b's sneaking. */
int SelectActorSneakWorked::matches(const critter* pc, const critter* actor) {
   if (actor->isSneaking()) {
      if (pc->canDetect(*actor) &&
          d(1, (25 + (pc->LEVEL * 5) + (pc->INT * 5)) 
            > (d(1, get_percent_lrnd(SNEAK_SKILL_NUM, *actor) +
                 actor->DEX * 10 + actor->LEVEL * 5)))) {
         return TRUE;
      }
   }//if
   return FALSE;
}//matches


int CSelectorColl::matches(critter* pc, critter* actor) const {
   CritterSelector* ptr;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "CSelectorColl::matches, select_rule: " << (int)(select_rule)
             << " pc: " << *(pc->getName());
      if (actor) {
         mudlog << " actor: " << *(actor->getName()) << endl;
      }
      else {
         mudlog << " actor: NULL" << endl;
      }
   }//if logging

   if (!selections.isEmpty()) {
      for (int i = selections.getNextIdx(-1); i != -1;
           i = selections.getNextIdx(i)) {
         ptr = selections.elementAt(i);

         if (mudlog.ofLevel(DBG)) {
            mudlog << "CSelectorColl::matches, selector: " << ptr->name()
                   << " matches?: " << ptr->matches(pc, actor) << endl;
         }

         if (select_rule == SELECTOR_AND) {
            if (ptr->matches(pc, actor)) {
               continue;
            }
            else {
               return FALSE;
            }
         }
         else if (select_rule == SELECTOR_OR) {
            if (ptr->matches(pc, actor)) {
               return TRUE;
            }
            else {
               continue;
            }
         }
      }//for
   }//if

   if (select_rule == SELECTOR_AND) {
      return TRUE;
   }
   else {
      return FALSE;
   }
}//matches


/** Constructor */
Selectors::Selectors() 
      : CC_all(CSelectorColl::SELECTOR_OR, &selectAll),
        CC_none(CSelectorColl::SELECTOR_AND, &selectNone),
        
        // If ALL of these are true, try you _may_ show message.
        CC_mob_entry_allow(CSelectorColl::SELECTOR_AND, &selectActorEntryMsgs,
                           &selectCanDetectActor),

        // If ANY of these are true, don't show message.
        CC_mob_entry_deny(CSelectorColl::SELECTOR_OR,
                          &selectNPC_Possessed, &selectAreSame,
                          &selectIsSleeping, &selectIsMeditating,
                          &selectActorSneakWorked),
        CC_gets_info_allow(CSelectorColl::SELECTOR_AND, &selectGetsInfo),
        CC_using_client(CSelectorColl::SELECTOR_AND, &selectUsingClient),
        CC_not_using_client(CSelectorColl::SELECTOR_AND, &selectNotUsingClient)
{
}//Constructor

/** Singleton thingie. */
Selectors& Selectors::instance() {
   static Selectors self;
   return self;
}
