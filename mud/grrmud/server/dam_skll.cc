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


void hurl(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   if (!victim) {
      mudlog.log(ERR, "ERROR:  NULL(s) sent to bash().\n");
      return;
   }//if

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  smob trying to bash..\n");
      return;
   }//if

   if (ROOM.isHaven()) {
      show("You can't bring yourself to perform violence here.\n", pc);
      return;
   }//if

   if (pc.POS != POS_STAND) {
      show("You aren't in a position to hurl anyone.\n", pc);
      return;
   }//if
   else if (!victim->Strlen()) {
      crit_ptr = Top(pc.IS_FIGHTING);
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   }//if

   if (crit_ptr) {
      if (ROOM.isNoPK() && crit_ptr->isPc()) {
         show("This room is too nice for murder.\n", pc);
         return;
      }//if

      if (crit_ptr == &pc) {
         show("You can't hurl yourself!!\n", pc);
         return;
      }//if

      if (!crit_ptr->canBeHurled()) {
         Sprintf(buf, "%S is wary to that particular trick now!\n",
                 crit_ptr->getLongName());
         pc.show(buf);
         return;
      }//if

      if (crit_ptr->isMob()) {
         crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum(), TRUE, i_th, victim,
                               pc.SEE_BIT);
      }//if

      do_hurl(*crit_ptr, pc);

   }//if
   else 
      show("Hurl who?\n", pc);
}//hurl


void do_hurl(critter& vict, critter& pc) {
   String buf(100);
   short do_fatality = FALSE;

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_hurl.\n");
      return;
   }//if

   if (!HaveData(&vict, pc.IS_FIGHTING)) {
      join_in_battle(pc, vict);
   }//if

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  smob sent in to do_bash.\n");
      return;
   }//if

   if (skill_did_hit(pc, HURL_SKILL_NUM, vict)) { 

      Sprintf(buf, "lifts %S over %s head!!\n",
	      name_of_crit(vict, ~0), get_his_her(pc));
      emote(buf, pc, ROOM, TRUE, &vict);
      Sprintf(buf, "%S lifts you over %s head!!\n",
	      name_of_crit(pc, vict.SEE_BIT), get_his_her(pc));
      show(buf, vict);

      Sprintf(buf, "You lift %S over your head.\n",
	      name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);

			/* affects */
      door* dptr;

      vict.notifyHasBeenHurled();
      vict.doRemoveFromBattle();
      vict.PAUSE += d(1,4) + 2;
      pc.PAUSE += d(1,3);

      if ((dptr = door::findDoor(ROOM.DOORS, 1, 
                                 Top(door_list[d(1,10)].names), ~0, ROOM)) && 
     	  !(dptr->dr_data->door_data_flags.get(2))) {
	 Sprintf(buf, "hurls %S out of the room.\n", name_of_crit(vict, ~0));
	 emote(buf, pc, ROOM, TRUE, &vict);
	 Sprintf(buf, "You hurl %s out of the room.\n", get_him_her(vict));
	 show(buf, pc);
	 Sprintf(buf, "%S hurls you out of the room.\n", 
		 name_of_crit(pc, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);

	 emote("is hurtled into the room.\n", vict,
               room_list[abs(dptr->destination)], TRUE);

         door* opposite_door = 
            door::findDoorByDest(room_list[abs(dptr->destination)].doors,
                                 pc.getCurRoomNum());
         String from_dir(30);
         int is_custom = FALSE;
         if (opposite_door) {
            from_dir = *(opposite_door->getDirection());
            if (strcasecmp(from_dir, "up") == 0) {
               is_custom = TRUE;
               from_dir = "above";
            }
            else if (strcasecmp(from_dir, "down") == 0) {
               is_custom = TRUE;
               from_dir = "below";
            }
         }//if

         int is_dead;
         vict.doGoToRoom(abs(dptr->destination), from_dir, dptr, is_dead,
                         vict.getCurRoomNum());
         
         if (is_dead) {
            return;
         }

         exact_raw_damage(d(8, pc.STR) * (dptr->distance + 1), NORMAL,
                             vict, pc);
         
      }//if hurled into another room
      else { //hurled against the wall
	 Sprintf(buf, "hurls %S into the wall.\n", name_of_crit(vict, ~0));
	 emote(buf, pc, ROOM, TRUE, &vict);
	 Sprintf(buf, "You hurl %s into the wall.\n", get_him_her(vict));
	 show(buf, pc);
	 Sprintf(buf, "%S hurls you into the wall.\n", 
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
      Sprintf(buf, "%S tries but fails to lift you over %s head!\n", 
                 name_of_crit(pc, vict.SEE_BIT), get_his_her(pc));
      buf.Cap();
      show(buf, vict);

      Sprintf(buf, "You fail to lift %S over your head!\n",
                 name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);

      Sprintf(buf, "fails to lift %S over %s head.\n",
	      name_of_crit(vict, ~0), get_his_her(pc));
      emote(buf, pc, ROOM, TRUE, &vict);
   }//else

   if (do_fatality) {
      agg_kills_vict(pc, vict);
   }//if
}//do_hurl



void body_slam(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   if (!victim) {
      mudlog.log(ERR, "ERROR:  NULL(s) sent to bash().\n");
      return;
   }//if

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  smob trying to bash..\n");
      return;
   }//if

   if (ROOM.isHaven()) {
      show("You can't bring yourself to perform violence here.\n", pc);
      return;
   }//if

   if (pc.POS != POS_STAND) {
      show("You aren't in a position to bodyslam anyone.\n",
            pc);
      return;
   }//if
   else if (!victim->Strlen()) {
      crit_ptr = Top(pc.IS_FIGHTING);
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   }//if

   if (crit_ptr) {
      if (ROOM.isNoPK() && crit_ptr->isPc()) {
         show("This room is too nice for murder.\n", pc);
         return;
      }//if

      if (crit_ptr == &pc) {
         show("You shouldn't be slamming yourself..\n", pc);
         return;
      }//if

      if (crit_ptr->isMob()) {
         crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum(), TRUE, i_th, victim,
                               pc.SEE_BIT);
      }//if

      do_body_slam(*crit_ptr, pc);

   }//if
   else 
      show("Bodyslam who?\n", pc);
}//body_slam


void do_body_slam(critter& vict, critter& pc) {
   String buf(100);
   short do_fatality = FALSE;

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_bash.\n");
      return;
   }//if

   if (!HaveData(&vict, pc.IS_FIGHTING)) {
      mudlog.log(DBG, "Calling join_in_battle in do_body_slam..\n");
      join_in_battle(pc, vict);
   }//if

   if (skill_did_hit(pc, BODYSLAM_SKILL_NUM, vict)) {

      pc.PAUSE += d(1,3); //increment pause_count
      exact_raw_damage(d(5, pc.STR) + pc.LEVEL, NORMAL, vict, pc);
      vict.PAUSE += (d(1,3) + 1);

      Sprintf(buf, "lifts %S over %s head!!\n",
	      name_of_crit(vict, ~0), get_his_her(pc));
      emote(buf, pc, ROOM, TRUE, &vict);
      Sprintf(buf, "%S lifts you over %s head!!\n",
	      name_of_crit(pc, vict.SEE_BIT), get_his_her(pc));
      show(buf, vict);
      Sprintf(buf, "You lift %S over your head with a mighty heave!\n",
	      name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);

      if (vict.HP < 0) {
         Sprintf(buf, "crushes %S's body into the ground!!\n", 
                 name_of_crit(vict, ~0), get_his_her(pc));
         emote(buf, pc, ROOM, TRUE, &vict);
         show("You meet the ground and your world goes black!\n", vict);

         Sprintf(buf, 
	         "You crush %S into the ground, %s moves no more\n",
        	 name_of_crit(vict, pc.SEE_BIT), get_he_she(vict));
         show(buf, pc);
         do_fatality = TRUE;
      }//if fatality
      else {
         Sprintf(buf, "%S slams you into the ground!\n",
                 name_of_crit(pc, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);

         Sprintf(buf, "You slam %S into the ground!\n",
		 name_of_crit(vict, vict.SEE_BIT));
         show(buf, pc);

         Sprintf(buf, "slams %S to the ground!\n",
		 name_of_crit(vict, ~0));
         emote(buf, pc, ROOM, TRUE, &vict);
      }//else
   }//if
   else {  //missed
      Sprintf(buf, "%S tries but fails to lift you over %s head!\n", 
                 name_of_crit(pc, vict.SEE_BIT), get_his_her(pc));
      buf.Cap();
      show(buf, vict);

      Sprintf(buf, "You fail to lift %S over your head!\n",
                 name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);

      Sprintf(buf, "fails to lift %S over %s head.\n",
	      name_of_crit(vict, ~0), get_his_her(pc));
      emote(buf, pc, ROOM, TRUE, &vict);

      pc.PAUSE += d(1,3); //increment pause_count
   }//else

   if (do_fatality) {
      agg_kills_vict(pc, vict);
   }//if
}//do_body_slam

