// $Id: ar_skll.cc,v 1.5 1999/06/05 23:29:13 greear Exp $
// $Revision: 1.5 $  $Author: greear $ $Date: 1999/06/05 23:29:13 $

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

//*********************** ar_sklls.cc ********************//

#include "misc.h"
#include "misc2.h"
#include "classes.h"
#include "spells.h"
#include "battle.h"
#include "ar_skll.h"
#include "skills.h"
#include "commands.h"
#include "batl_prc.h"
#include <PtrArray.h>




int berserk(critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mVSPF", 0, pc)) {
      return -1;
   }//if     

   return do_berserk(pc);
}//berserk


int do_berserk(critter& pc) {
   String buf(100);

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_berserk.\n");
      return -1;
   }//if

   if (pc.MOV < 35) {
      show("You are too exhausted!\n", pc);
      return -1;
   }//if

   critter* ptr;
   
   ROOM.makeReadyForAreaSpell();

   int flag = FALSE;

   if (skill_did_hit(pc, BERZERK_SKILL_NUM, pc)) {
      while ((ptr = ROOM.findNextSpellCritter())) {
         if (ptr != &pc) {
            if (ptr->mob || 
                 (ptr->pc && HaveData(ptr, pc.IS_FIGHTING))) {

               if (ptr->pc && ptr->PC_FLAGS.get(7)) { //if !hassle
                  continue;
               }//if
               
               if (ptr->isMob()) {
                  ptr = mob_to_smob(*ptr, pc.getCurRoomNum());
               }//if
               flag = TRUE;
               ptr = check_for_diversions(*ptr, "MB", pc);
               if (!ptr) { //mirror was hit probably
                  continue;
               }//if
               if (!HaveData(ptr, pc.IS_FIGHTING)) {
                  join_in_battle(pc, *ptr);
               }//if

               if (ptr->isUsingClient()) {
                  show("<BATTLE>", *ptr);
               }
               else if (ptr->isUsingColor()) {
                  show(*(ptr->getBattleColor()), *ptr);
               }
               
               int show_vict_tags = TRUE;

               do_battle_round(pc, *ptr, 10, show_vict_tags);

               // Tags will only be shown in do_battle_round if
               // the victim died (and was a PC)
               if (show_vict_tags) {
                  if (ptr->isUsingClient()) {
                     show("</BATTLE>", *ptr);
                  }
                  else if (ptr->isUsingColor()) {
                     show(*(ptr->getDefaultColor()), *ptr);
                  }
               }//if

            }//if
         }//
      }//while
      if (!flag) {
         show("There is no one here to go berserk on!!\n", pc);
         return -1;
      }//if
      else {
         pc.MOV -= 35;
      }//else
   }//if
   else {
      show("You trip and nearly break a leg!!\n", pc);
      emote("gets all riled up nearly trips!!\n", pc, ROOM, TRUE);
      return -1;
   }//else
   return 0;
}//do_berserk







