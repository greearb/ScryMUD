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




void berserk(critter& pc) {
   String buf(100);

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  smob trying to berserk..\n");
      return;
   }//if

   if (ROOM.isHaven()) {
      show("You can't bring yourself to perform violence here.\n", pc);
      return;
   }//if

   if (pc.POS != POS_STAND) {
      show("You can't hardly go berserk on your butt!!\n", 
            pc);
      return;
   }//if

   do_berserk(pc);

}//berserk


void do_berserk(critter& pc) {
   String buf(100);

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_berserk.\n");
      return;
   }//if

   if (pc.MOV < 35) {
      show("You are too exhausted!\n", pc);
      return;
   }//if

   List<critter*> tmp_lst(ROOM.getCrits());
   Cell<critter*> cll(tmp_lst);
   critter* ptr;

   while ((ptr = cll.next()))
      ptr->CRIT_FLAGS.turn_off(20); //turn off done_already 
                                    //flag for all in rm

   int flag = FALSE;

   if (skill_did_hit(pc, BERZERK_SKILL_NUM, pc)) {
      while ((ptr = find_next_spell_mob(tmp_lst))) {
         if (ptr != &pc) {
            if (ptr->mob || 
                 (ptr->pc && HaveData(ptr, pc.IS_FIGHTING))) {

               if (ptr->pc && ptr->PC_FLAGS.get(7)) { //if !hassle
                  continue;
               }//if
               
               if (!ROOM.haveCritter(ptr)) {
                  continue;
               }

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
               do_battle_round(pc, *ptr, 10);
            }//if
         }//
      }//while
      if (!flag) {
         show("There is no one here to go berserk on!!\n", pc);
      }//if
      else {
         pc.MOV -= 35;
      }//else
   }//if
   else {
      show("You trip and nearly break a leg!!\n", pc);
      emote("gets all riled up nearly trips!!\n", pc, ROOM, TRUE);
   }//else
}//do_berserk







