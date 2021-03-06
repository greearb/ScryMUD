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

//*********************** skills.cc ********************//

#include "misc.h"
#include "misc2.h"
#include "classes.h"
#include "spells.h"
#include "battle.h"
#include "skills.h"
#include "commands.h"
#include "batl_prc.h"
#include "dam_skll.h"
#include <PtrArray.h>


int hurl(int i_th, const String* victim, critter& pc, int was_ordered) {
   critter* crit_ptr;
   String buf(100);

   if (pc.isNpc() && was_ordered) {
      return -1;
   }

   if (!victim->Strlen()) {
      crit_ptr = pc.IS_FIGHTING.peekFront();
      if (crit_ptr && !detect(pc.SEE_BIT, crit_ptr->VIS_BIT))
         crit_ptr = NULL;
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(crit_ptr, "mSVFP", 0, pc,
                        pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (crit_ptr) {

      if (crit_ptr == &pc) {
         show("You can't hurl yourself!!\n", pc);
         return -1;
      }//if

      if (!crit_ptr->canBeHurled()) {
         Sprintf(buf, "%pS is wary to that particular trick now!\n",
                 crit_ptr->getLongName());
         buf.Cap();
         pc.show(buf);
         return -1;
      }//if

      if (pc.isPc() && (get_percent_lrnd(HURL_SKILL_NUM, pc) < 1)) {
         pc.show("You don't know that skill yet.\n");
         return -1;
      }

      if (crit_ptr->isMob()) {
         crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum(), TRUE,
                                i_th, victim, pc.SEE_BIT);
      }//if

      return do_hurl(*crit_ptr, pc);
   }//if
   else {
      show("Hurl who?\n", pc);
      return -1;
   }
}//hurl


int do_hurl(critter& vict, critter& pc) {
   String buf(100);
   short do_fatality = FALSE;

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(LS_ERROR, "ERROR:  MOB sent to do_hurl.\n");
      return -1;
   }//if

   if (!pc.IS_FIGHTING.haveData(&vict)) {
      join_in_battle(pc, vict);
   }//if

   if (skill_did_hit(pc, HURL_SKILL_NUM, vict)) { 

      Sprintf(buf, "lifts %pS over %s head!!\n",
              name_of_crit(vict, ~0), get_his_her(pc));
      emote(buf, pc, ROOM, TRUE, &vict);
      Sprintf(buf, "%pS lifts you over %s head!!\n",
              name_of_crit(pc, vict.SEE_BIT), get_his_her(pc));
      show(buf, vict);

      Sprintf(buf, "You lift %pS over your head.\n",
              name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);

                        /* affects */
      door* dptr;

      vict.notifyHasBeenHurled();
      vict.doRemoveFromBattle();
      vict.PAUSE = 1;
      pc.PAUSE = 1;

      if ((dptr = door::findDoor(ROOM.DOORS, 1, 
                                 door_list[d(1,10)].names.peekFront(),
                                 ~0, ROOM)) && 
               !(dptr->dr_data->door_data_flags.get(2))) {
         Sprintf(buf, "hurls %pS out of the room.\n", name_of_crit(vict, ~0));
         emote(buf, pc, ROOM, TRUE, &vict);
         Sprintf(buf, "You hurl %s out of the room.\n", get_him_her(vict));
         show(buf, pc);
         Sprintf(buf, "%pS hurls you out of the room.\n", 
                 name_of_crit(pc, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);

         emote("is hurtled into the room.\n", vict,
               room_list[abs(dptr->destination)], TRUE);

         door* opposite_door = 
            door::findDoorByDest(room_list[abs(dptr->destination)].doors,
                                 pc.getCurRoomNum());
         String from_dir(30);
         if (opposite_door) {
            from_dir = *(opposite_door->getDirection());
            if (strcasecmp(from_dir, "up") == 0) {
               from_dir = "above";
            }
            else if (strcasecmp(from_dir, "down") == 0) {
               from_dir = "below";
            }
         }//if

         int is_dead;
         vict.doGoToRoom(abs(dptr->destination), from_dir, dptr, is_dead,
                         vict.getCurRoomNum(), 1);
         
         if (is_dead) {
            return 0;
         }

         if (vict.isFlying()) {
            vict.show("Fortunately, you are flying, and only barely scrape the ground!\n");
         }
         else {
            exact_raw_damage(d(8, pc.STR) * (dptr->distance + 1), NORMAL,
                             vict, pc);
         }
      }//if hurled into another room
      else { //hurled against the wall
         Sprintf(buf, "hurls %pS into the wall.\n", name_of_crit(vict, ~0));
         emote(buf, pc, ROOM, TRUE, &vict);
         Sprintf(buf, "You hurl %s into the wall.\n", get_him_her(vict));
         show(buf, pc);
         Sprintf(buf, "%pS hurls you into the wall.\n", 
                 name_of_crit(pc, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);

         exact_raw_damage(d(8, pc.STR), NORMAL, vict, pc);
      }//else hurled into wall

      if (vict.HP < 0) { //do fatality
         show("You meet the ground and your world goes black!\n", vict);
         emote("hits the ground and moves no more!\n", vict,
               room_list[vict.getCurRoomNum()], TRUE);
         do_fatality = TRUE;
      }//if fatality
      //else { //no fatality
      //   emote("hits the ground hard!\n", vict,
      //         *(vict.getCurRoom()), TRUE);
      //}//else
   }//if hit
   else {  //missed
      Sprintf(buf, "%pS tries but fails to lift you over %s head!\n", 
                 name_of_crit(pc, vict.SEE_BIT), get_his_her(pc));
      buf.Cap();
      show(buf, vict);

      Sprintf(buf, "You fail to lift %pS over your head!\n",
                 name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);

      Sprintf(buf, "fails to lift %pS over %s head.\n",
              name_of_crit(vict, ~0), get_his_her(pc));
      emote(buf, pc, ROOM, TRUE, &vict);
   }//else

   if (do_fatality) {
      agg_kills_vict(&pc, vict);
   }//if
   return 0;
}//do_hurl



int body_slam(int i_th, const String* victim, critter& pc, int was_ordered) {
   critter* crit_ptr;
   String buf(100);

   if (pc.isNpc() && was_ordered) {
      return -1;
   }

   if (!victim->Strlen()) {
      crit_ptr = pc.IS_FIGHTING.peekFront();
      if (crit_ptr && !detect(pc.SEE_BIT, crit_ptr->VIS_BIT))
         crit_ptr = NULL;
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   }//if

   if (crit_ptr) { 
      if (!ok_to_do_action(crit_ptr, "mSVFPA", 0, pc, pc.getCurRoom(),
                           NULL, TRUE)) {
         return -1;
      }

      if (crit_ptr == &pc) {
         show("You shouldn't be slamming yourself..\n", pc);
         return -1;
      }//if

      if (pc.isPc() && (get_percent_lrnd(BODYSLAM_SKILL_NUM, pc) < 1)) {
         pc.show("You don't know that skill yet.\n");
         return -1;
      }

      if (crit_ptr->isMob()) {
         crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum(), TRUE, i_th, victim,
                               pc.SEE_BIT);
      }//if

      return do_body_slam(*crit_ptr, pc);
   }//if
   else {
      show("Bodyslam who?\n", pc);
      return -1;
   }
}//body_slam


int do_body_slam(critter& vict, critter& pc) {
   String buf(100);
   short do_fatality = FALSE;

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(LS_ERROR, "ERROR:  MOB sent to do_bash.\n");
      return -1;
   }//if

   if (!pc.IS_FIGHTING.haveData(&vict)) {
      mudlog.log(DBG, "Calling join_in_battle in do_body_slam..\n");
      join_in_battle(pc, vict);
   }//if

   if (skill_did_hit(pc, BODYSLAM_SKILL_NUM, vict)) {

      pc.PAUSE = 1; //increment pause_count
      exact_raw_damage(d(5, pc.STR) + pc.LEVEL, NORMAL, vict, pc);
      vict.PAUSE = 1;

      Sprintf(buf, "lifts %pS over %s head!!\n",
              name_of_crit(vict, ~0), get_his_her(pc));
      emote(buf, pc, ROOM, TRUE, &vict);
      Sprintf(buf, "%pS lifts you over %s head!!\n",
              name_of_crit(pc, vict.SEE_BIT), get_his_her(pc));
      show(buf, vict);
      Sprintf(buf, "You lift %pS over your head with a mighty heave!\n",
              name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);

      if (vict.HP < 0) {
         Sprintf(buf, "crushes %pS's body into the ground!!\n", 
                 name_of_crit(vict, ~0));
         emote(buf, pc, ROOM, TRUE, &vict);
         show("You meet the ground and your world goes black!\n", vict);

         Sprintf(buf, 
                 "You crush %pS into the ground, %s moves no more!\n",
                 name_of_crit(vict, pc.SEE_BIT), get_he_she(vict));
         show(buf, pc);
         do_fatality = TRUE;
      }//if fatality
      else {
         Sprintf(buf, "%pS slams you into the ground!\n",
                 name_of_crit(pc, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);

         Sprintf(buf, "You slam %pS into the ground!\n",
                 name_of_crit(vict, vict.SEE_BIT));
         show(buf, pc);

         Sprintf(buf, "slams %pS to the ground!\n",
                 name_of_crit(vict, ~0));
         emote(buf, pc, ROOM, TRUE, &vict);
      }//else
   }//if
   else {  //missed
      Sprintf(buf, "%pS tries but fails to lift you over %s head!\n", 
                 name_of_crit(pc, vict.SEE_BIT), get_his_her(pc));
      buf.Cap();
      show(buf, vict);

      Sprintf(buf, "You fail to lift %pS over your head!\n",
                 name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);

      Sprintf(buf, "fails to lift %pS over %s head.\n",
              name_of_crit(vict, ~0), get_his_her(pc));
      emote(buf, pc, ROOM, TRUE, &vict);

      pc.PAUSE = 1; //increment pause_count
   }//else

   if (do_fatality) {
      agg_kills_vict(&pc, vict);
   }//if
   return 0;
}//do_body_slam

