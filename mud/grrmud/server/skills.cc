// $Id: skills.cc,v 1.18 1999/09/06 02:24:28 greear Exp $
// $Revision: 1.18 $  $Author: greear $ $Date: 1999/09/06 02:24:28 $

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
#include "socials.h"
#include "social2.h"
#include <PtrArray.h>
#include "load_wld.h"
#include "SkillSpell.h"


int track(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   //   log("In track\n");

   /* check for miss on skill */
   if ((d(1, get_percent_lrnd(TRACK_SKILL_NUM, pc)) * 4) < d(1,100)) {
      String my_dir(regular_directions[d(1, 8) - 1]);
      Sprintf(buf, "You sense a trail to the %S.\n", 
              &my_dir);
      show(buf, pc);
      return 0;
   }//if


   if (!victim) {
      mudlog.log(ERR, "ERROR:  NULL(s) sent to track().\n");
   }//if
   else if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  mob trying to track..\n");
   }//if
   else if (pc.POS != POS_STAND) {
      show("You must be standing in order to track.\n", 
            pc);
   }//if
   else if (!pc.IS_FIGHTING.isEmpty()) {
      show("You can't track while you're fighting!!\n", pc);
   }//if
   else if (!victim->Strlen()) {
      show("Track who??\n", pc);
   }//if
   else { //ok, should be good to go 
      int znum = pc.getCurZoneNum();
      int in_room;
      // this fn sets in_room btw.
      crit_ptr = room::haveCritNamedInZone(ZoneCollection::instance().elementAt(znum),
                                           i_th, victim, pc.SEE_BIT, in_room);
      if (!crit_ptr) {
	show("That person is not found in these parts.\n", pc);
      }//if
      else {
        if (pc.getCurRoomNum() == in_room) {
	  show("You're already here!\n", pc);
        }//if
	else {
	  List<int> path(0);
	  path_from_a_to_b(pc.getCurRoomNum(), in_room, path);

          /*log("TRACK:  HERE IS PATH.\n");
          Cell<int> tcll(path);
          int tint;
          while (tint = Next(tcll)) {
	    Sprintf(buf, "TRACK:  %i ", tint);
	    log(buf);
	  }//while
	  */

	  if (path.isEmpty()) {
             show("You can't get there from here!\n", pc);
          }//if
	  else {
             SCell<door*> cll(ROOM.DOORS);
             door* ptr;
             path.popFront(); //get rid of first one
             while ((ptr = cll.next())) {
                if (abs(ptr->getDestination()) == path.peekFront()) {
                   Sprintf(buf, "You sense a trail leading %S.\n", 
                           ptr->getDirection());
                   show(buf, pc);
                   return 0;
                }//if
             }//while
             Sprintf(buf, "TRACK:  no exit to this room:  %i\n",
                     path.peekFront());
             //log(buf);
             show(buf, pc);
	  }//else got some kinda path
	}//else not already in same room
      }//else found a crit ptr
   }//else, good to go
   return -1;
}//track


int trip(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   if (!victim->Strlen()) {
      if (!(crit_ptr = pc.IS_FIGHTING.peekFront())) {
         show("Trip who?\n", pc);
         return -1;
      }//if
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   }//if

   if (crit_ptr) {

      if (crit_ptr == &pc) {
	show("You trip over your own two feet!\n", pc);
	return 0;
      }//if

      if (!ok_to_do_action(crit_ptr, "SVPFA", -1, pc)) {
         return -1;
      }//if

      if (!(crit_ptr = check_for_diversions(*crit_ptr, "GM", pc)))
         return -1;

      return do_trip(*crit_ptr, pc);
   }//if
   else {
     show("Trip who??\n", pc);
   }//else
   return -1;
}//trip


int do_trip(critter& vict, critter& pc) {
   String buf(100);
   short do_fatality = FALSE;


   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_trip.\n");
      return -1;
   }//if

   if (!pc.IS_FIGHTING.haveData(&vict)) {
      join_in_battle(pc, vict);
   }//if

   if (skill_did_hit(pc, TRIP_SKILL_NUM, vict)) {
      pc.setPosn(POS_SIT);
      pc.PAUSE += 2; //increment pause_count
      exact_raw_damage(d(4, 5), NORMAL, vict, pc);
      vict.PAUSE += d(1,3);

      ROOM.doEmoteN(pc, CS_TRIPS_S, vict);

      Sprintf(buf, cstr(CS_TRIPS_YOU, vict), pc.getName(&vict));
      buf.Cap();
      vict.show(buf);
      Sprintf(buf, cstr(CS_YOU_TRIP, pc), vict.getName(&pc));
      pc.show(buf);

      if (vict.HP < 0) {
	Sprintf(buf, "breaks %s neck as %s hits the ground!! ***CRACK***\n",
		get_his_her(vict), get_he_she(vict));
	emote(buf, vict, ROOM, TRUE);
	do_fatality = TRUE;
      }//if fatality
   }//if
   else {  //missed
      vict.showN(CS_DODGE_TRIP, pc);

      Sprintf(buf, "dodges %S's attempt to trip %s.",
	      pc.getName(), get_him_her(vict));
      emote(buf, vict, ROOM, TRUE, &pc);

      Sprintf(buf, cstr(CS_DODGE_TRIP_HIM, pc),
              vict.getName(&pc), get_him_her(vict));
      pc.show(buf);

      pc.PAUSE += 2; //increment pause_count
   }//else
   if (do_fatality) {
      agg_kills_vict(pc, vict);
   }//if
   return 0;
}//do_trip()


int steal(int i_th, const String* obj, int j_th, const String* victim,
          critter& pc) {
   critter* crit_ptr = NULL;
   object* objptr = NULL;
   int gold = FALSE;

   String buf(100);

   crit_ptr = ROOM.haveCritNamed(j_th, victim, pc.SEE_BIT);

   if (crit_ptr) {
      if (crit_ptr == &pc) {
	show("There's a bright idea!\n", pc);
	return 0;
      }//if

      if (!ok_to_do_action(crit_ptr, "mSVPF", -1, pc)) {
         return -1;
      }//if

      if (!obj->Strlen() || (strcasecmp(*obj, "gold") == 0)) {
	gold = TRUE;
      }//if
      else {
	objptr = crit_ptr->haveObjNamed(i_th, obj, &pc);      
      }//else
      return do_steal(objptr, *crit_ptr, pc, gold);
   }//if
   else {
     show("Steal from who??\n", pc);
   }//else
   return -1;
}//steal


int do_steal(object* obj, critter& vict, critter& pc, 
	      short steal_gold = FALSE) {
   String buf(100);
   int amt;


   if (skill_did_hit(pc, STEAL_SKILL_NUM, vict)) {
      /* it worked */
      pc.PAUSE += 1; //increment pause_count
      if (obj) {
         vict.loseInv(obj);
         pc.gainInv(obj);
         /* no explicit penalties if you don't get caught! */
         
         Sprintf(buf, cstr(CS_YOU_STEAL_SUCC, pc),
                 obj->getLongName(&pc), vict.getName(&pc));
         show(buf, pc);
      }//if an object
      else if (steal_gold) {
         pc.GOLD += (amt = (d(1,vict.GOLD) / 4));
         vict.GOLD -= amt;
         /* no explicit penalties if you don't get caught! */
         
         Sprintf(buf, cstr(CS_YOU_STEAL_GOLD, pc),
                 amt, vict.getName(&pc));
         show(buf, pc);
      }//if
      else { //!gold AND !obj, bad guess!!
         pc.show(CS_NO_STEAL_TARG);
         return 0;
      }//else
      return 0;
   }//if
   else {  //got caught!!!
      
      Sprintf(buf, cstr(CS_CAUGHT_STEALING, pc),
              vict.getName(&pc), get_his_her(vict));
      show(buf, pc);
      
      Sprintf(buf, "is caught trying to steal from %S.\n",
              vict.getName());
      emote(buf, pc, ROOM, TRUE, &vict);
      
      Sprintf(buf, cstr(CS_YOU_CATCH_STEALING, vict),
              pc.getName(&vict), get_his_her(pc));
      show(buf, vict);
      

      if (!vict.pc) {
         if (!vict.mob->proc_data || (vict.mob->getBenevolence() < 5)) {
            /* gonna be a whuppin! */
            do_hit(pc, vict); //start battle
         }//
         else {
            sob(1, &NULL_STRING, vict, room_list[vict.getCurRoomNum()]);
         }//else
      }//if
      else {
         if (pc.pc) { //P_STEAL
            pc.PC_FLAGS.turn_on(20);
         }//if
      }//else
      return -1;
   }//else
   return -1;
}//do_steal


int kick(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   if (!victim->Strlen()) {
      if (!(crit_ptr = pc.IS_FIGHTING.peekFront())) {
         show("Kick who?\n", pc);
         return -1;
      }//if
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   }//if

   if (crit_ptr) {

      if (crit_ptr == &pc) {
         show("You kick yourself for your stupidity!\n", pc);
         Sprintf(buf, "kicks %s.", get_himself_herself(pc));
         emote(buf, pc, ROOM, TRUE);
         return 0;
      }//if

      if (!ok_to_do_action(crit_ptr, "SVPF", -1, pc)) {
         return -1;
      }//if

      if (!(crit_ptr = check_for_diversions(*crit_ptr, "GM", pc)))
         return -1;

      return do_kick(*crit_ptr, pc);
   }//if
   else {
      show("Kick who??\n", pc);
   }//else
   return -1;
}//kick


int do_kick(critter& vict, critter& pc) {
   String buf(100);
   short do_fatality = FALSE;


   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_kick.\n");
      return -1;
   }//if

   if (!pc.IS_FIGHTING.haveData(&vict)) {
      join_in_battle(pc, vict);
   }//if

   if (skill_did_hit(pc, KICK_SKILL_NUM, vict)) {

      pc.PAUSE += d(1,2); //increment pause_count
      float d1 = (((float)(pc.STR)) * (float)(pc.getLevel()) + 5.0) / 60.0;
      exact_raw_damage(d((int)d1, 5), NORMAL, vict, pc);
      vict.PAUSE += d(1,3);

      if (vict.HP < 0) {
         Sprintf(buf, "kicks in %S's temple with %s foot!!\n", 
                 vict.getName(), get_his_her(pc));
         emote(buf, pc, ROOM, TRUE, &vict);
         Sprintf(buf, cstr(CS_KICK_FATALITY, vict), pc.getName(&pc));
         show(buf, vict);
         Sprintf(buf, cstr(CS_YOU_CRUSH_KICK, pc), vict.getName(&pc));
         show(buf, pc);
         do_fatality = TRUE;
      }//if fatality
      else {
         Sprintf(buf, cstr(CS_KICKS_YOU, vict), pc.getName(&vict));
         buf.Cap();
         show(buf, vict);

         Sprintf(buf, cstr(CS_YOU_KICK, pc), vict.getName(&pc));
         show(buf, pc);

         Sprintf(buf, "kicks %S!\n", vict.getName());
         emote(buf, pc, ROOM, TRUE, &vict);
      }//else
   }//if
   else {  //missed
      Sprintf(buf, "kicks mightily at %S, missing by a foot!\n",
              vict.getName());
      emote(buf, pc, ROOM, FALSE, &vict);

      Sprintf(buf, cstr(CS_KICK_MISS, vict), pc.getName(&vict), get_his_her(pc));
      buf.Cap();
      show(buf, vict);

      Sprintf(buf, cstr(CS_YOU_MISS_KICK, pc), vict.getName(&pc));
      show(buf, pc);

      pc.PAUSE += 2; //increment pause_count
   }//else
   if (do_fatality) {
      agg_kills_vict(pc, vict);
   }//if
   return 0;
}//do_kick()


int bash(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   if (!victim->Strlen()) {
      if (!(crit_ptr = pc.getFirstFighting())) {
         show("Bash who?\n", pc);
         return -1;
      }//if
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, victim, pc);
   }//if

   if (crit_ptr) {

      if (crit_ptr == &pc) {
         show("You shouldn't be bashing yourself..\n", pc);
         return -1;
      }//if

      if (!ok_to_do_action(crit_ptr, "SVPFA", -1, pc)) {
         return -1;
      }//if

      if (!(crit_ptr = check_for_diversions(*crit_ptr, "GM", pc)))
         return -1;

      return do_bash(*crit_ptr, pc);
   }//if
   else if (door* dptr = ROOM.findDoor(i_th, victim, pc))
      return do_bash(*dptr, pc);
   else
      pc.show(CS_NO_CAN_SEE);
   return -1;
}//bash


int do_bash(door& vict, critter& pc) { //bash for doors
   String buf(100);

   if (pc.isMob()) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  MOB sent to do_bash, name:  " 
                << *(name_of_crit(pc, ~0)) << "  address:  " << &pc << endl;
      }

      return -1;
   }//if

   if (!vict.isClosed()) {
      show("It isn't even closed.\n", pc);
      return -1;
   }//if

   if (!vict.isLocked()) {
      show("Its not locked, just open it!\n", pc);
      return -1;
   }//if

   if (!vict.isDestructable()) {
      pc.show("You nearly break your hand on the door.  It doesn't budge!\n");
      Sprintf(buf, 
              "nearly breaks %s hand on the door.  It doesn't budge!",
              get_his_her(pc));
      emote(buf, pc, ROOM, TRUE);
   }//if
   else if (d(1, 100) < get_percent_lrnd(BASH_SKILL_NUM, pc)) {
      Sprintf(buf, "You bust down the %S.\n", vict.getName(&pc));
      show(buf, pc);
      vict.open();
      vict.setLockable(FALSE);
      Sprintf(buf, "busts open the %S!\n", vict.getName());
      emote(buf, pc, ROOM, TRUE);
   }//else
   else {
      Sprintf(buf, "The %S gives a little as you hit it.\n",
              vict.getName(&pc));
      show(buf, pc);
      Sprintf(buf, "budges %S with %s hit.\n", vict.getName(),
              get_his_her(pc));
      emote(buf, pc, ROOM, TRUE);
   }//else  
   
   pc.PAUSE += d(2,2); //increment pause_count
   return 0;
}//do_bash()//door



int do_bash(critter& vict, critter& pc) {
   String buf(100);
   short do_fatality = FALSE;

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_bash.\n");
      return -1;
   }//if

   if (!pc.IS_FIGHTING.haveData(&vict)) {
      join_in_battle(pc, vict);
   }//if

   if (skill_did_hit(pc, BASH_SKILL_NUM, vict)) {

      pc.PAUSE += d(1,3); //increment pause_count
      exact_raw_damage(d(pc.STR/2, 5) + pc.DEX/2, NORMAL, vict, pc);
      vict.PAUSE += d(1,3);

      if (vict.HP < 0) {
         Sprintf(buf, "crushes the skull of %S with %s fist!!\n", 
                 name_of_crit(vict, ~0), get_his_her(pc));
         emote(buf, pc, ROOM, TRUE, &vict);
         Sprintf(buf, 
          "The last thing you feel is %S's fist caving in your temple!!\n",
          name_of_crit(pc, vict.SEE_BIT));
         show(buf, vict);
         Sprintf(buf, 
          "With a mighty blow of your fist you crush %S's skull.\n",
          name_of_crit(vict, pc.SEE_BIT));
         show(buf, pc);
         do_fatality = TRUE;
      }//if fatality
      else {
         Sprintf(buf, "%S rings your bell!\n",
                 name_of_crit(pc, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);

         Sprintf(buf, "You bash %S!\n", name_of_crit(vict, vict.SEE_BIT));
         show(buf, pc);

         Sprintf(buf, "bashes %S!\n", name_of_crit(vict, ~0));
         emote(buf, pc, ROOM, TRUE, &vict);
      }//else
   }//if
   else {  //missed
      Sprintf(buf, "nearly falls on %s butt trying to hit %S!\n",
              get_his_her(pc), vict.getName());
      emote(buf, pc, ROOM, FALSE, &vict);

      Sprintf(buf, "%S takes a big swing at you but misses!\n", 
                 name_of_crit(pc, vict.SEE_BIT));
      buf.Cap();
      show(buf, vict);

      Sprintf(buf, "You try to bash %S but fall on your ass!\n",
                 name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);

      pc.PAUSE += d(2,2); //increment pause_count
      return -1;
   }//else
   if (do_fatality) {
      agg_kills_vict(pc, vict);
   }//if
   return 0;
}//do_bash()


int block(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   if (!victim->Strlen()) {
      if (!(crit_ptr = pc.getFirstFighting())) {
         show("Block who?\n", pc);
         return -1;
      }//if
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, victim, pc);
   }//if

   if (crit_ptr) {
     if (!pc.IS_FIGHTING.haveData(crit_ptr)) {
       show("You aren't fighting that person.\n", pc);
       return -1;
     }//if

     if (!ok_to_do_action(crit_ptr, "PFA", -1, pc)) {
        return -1;
     }//if

     return do_block(*crit_ptr, pc);
   }//if
   else if (door* dptr = ROOM.findDoor(i_th, victim, pc))
      return do_block(*dptr, pc);
   else
      pc.show(CS_NO_CAN_SEE);
   return -1;
}//block


int do_block(critter& vict, critter& pc) {
   String buf(100);

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_block.\n");
      return -1;
   }//if

   if (!pc.isFighting(vict)) {
      return -1;
   }//if

   if (skill_did_hit(pc, BLOCK_SKILL_NUM, vict)) {
      pc.PAUSE += d(1,3); //increment pause_count
      vict.CRIT_FLAGS.turn_on(21); //now is blocked

      Sprintf(buf, "You block %S.\n", name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);
      Sprintf(buf, "blocks %S.", name_of_crit(vict, ~0));
      emote(buf, pc, ROOM, TRUE, &vict);
      Sprintf(buf, "%S blocks you.\n", name_of_crit(pc, vict.SEE_BIT));
      show(buf, vict);
      return 0;
   }//if
   else {  //missed
      show("You miss the block.\n", pc);
      emote("misses the block.", pc, ROOM, TRUE, &vict);
      Sprintf(buf, "%S tries but fails to block you.\n", 
	      name_of_crit(pc, vict.SEE_BIT));
      show(buf, vict);
      return -1;
   }//else
}//do_block()



int do_block(door& vict, critter& pc) {
   String buf(100);

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_block.\n");
      return -1;
   }//if

   if (ROOM.isHaven() || ROOM.isNoPK()) {
     show("Aggression is not allowed here.\n", pc);
     return -1;
   }//if

   if (pc.isInBattle()) {
     show("You can't block an entrance while fighting.\n", pc);
     return -1;
   }//if

   if (vict.isBlocked()) {
     show("Someone is already blocking that door!\n", pc);
     return -1;
   }//if

   if (d(1, 100) < get_percent_lrnd(BLOCK_SKILL_NUM, pc)) {
      Sprintf(buf, "You start blocking the %S.\n", vict.getName(&pc));
      show(buf, pc);
      vict.setBlocked(TRUE);
      vict.setCritBlocking(&pc);
      Sprintf(buf, "starts blocking the %S!\n", vict.getName());
      emote(buf, pc, ROOM, TRUE);
      return 0;
   }//else
   else {
     pc.PAUSE += d(1, 2); //increment pause_count
     show(
    "You can't find a suitable spot, maybe if you tried a little harder.\n",
          pc);
   }//else      
   return -1;
}//do_block()//door


int claw(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   if (pc.EQ[9] && pc.EQ[10]) {
      show("You don't have a free hand!\n", pc);
      return -1;
   }//if

   if (!victim->Strlen()) {
      crit_ptr = pc.getFirstFighting();
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, victim, pc);
   }//if

   if (crit_ptr) {

      if (crit_ptr == &pc) {
         show("You shouldn't be clawing yourself..\n", pc);
         return -1;
      }//if

      if (!ok_to_do_action(crit_ptr, "SVPF", -1, pc)) {
         return -1;
      }//if

      if (!(crit_ptr = check_for_diversions(*crit_ptr, "GM", pc)))
         return -1;

      return do_claw(*crit_ptr, pc);

   }//if
   else 
      show("Claw who?\n", pc);
   return -1;
}//claw


int do_claw(critter& vict, critter& pc) {
   String buf(100);
   short do_fatality = FALSE;

   mudlog.log(TRC, "In do_claw.\n");

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_claw.\n");
      return -1;
   }//if

   if (!pc.isFighting(vict)) {
      join_in_battle(pc, vict);
   }//if

   pc.PAUSE += d(1,3); //increment pause_count

   if (skill_did_hit(pc, CLAW_SKILL_NUM, vict)) {

      exact_raw_damage((d(pc.STR/2, 5) + pc.LEVEL), NORMAL, vict);

      if (vict.HP < 0) {
         Sprintf(buf, "rips %S's throat out with %s curled hand!!\n", 
                 name_of_crit(vict, ~0), get_his_her(pc));
         emote(buf, pc, ROOM, TRUE);
         Sprintf(buf, 
                 "You are supprised to see %S holding your larynx!!\n",
                 name_of_crit(pc, vict.SEE_BIT));
         show(buf, vict);
         Sprintf(buf, 
          "You rip out %S's larynx with a quick swipe of your hand.\n",
          name_of_crit(vict, pc.SEE_BIT));
         show(buf, pc);
         do_fatality = TRUE;
      }//if fatality
      else {
         Sprintf(buf, 
		"%S's fingers leave bloody streaks across your body!\n",
                name_of_crit(pc, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);

         Sprintf(buf, 
		"Your fingers leave bloody streaks across %S's body!\n", 
		name_of_crit(vict, vict.SEE_BIT));
         show(buf, pc);

         Sprintf(buf, "claws %S with a curled hand!\n",
		 name_of_crit(vict,~0));
         emote(buf, pc, ROOM, TRUE);
      }//else
   }//if
   else {  //missed
      Sprintf(buf, "%S takes a big swipe at you but misses!\n", 
                 name_of_crit(pc, vict.SEE_BIT));
      buf.Cap();
      show(buf, vict);

      Sprintf(buf, "You try to claw %S but narrowly miss.\n",
             name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);
      return -1;
   }//else
   if (do_fatality) {
      agg_kills_vict(pc, vict);
   }//if
   return 0;
}//do_claw()


int construct(critter& pc, short do_mob = FALSE) {
   String buf(100);
   object* toolbox, *ptr, *item1, *item2, *item3, *item4, *item5;
   SCell<object*> cll;
   //   log("In construct.\n");

   item1 = item2 = item3 = item4 = item5 = NULL;

   if (!pc.pc && !do_mob) //no ordering pets to do yer dirty-work
      return -1;

   if (!ok_to_do_action(NULL, "mBFPSK", CONSTRUCT_SKILL_NUM, pc)) {
      return -1;
   }

   pc.inv.head(cll);
   toolbox = NULL;
   while ((ptr = cll.next())) {
      if (ptr->OBJ_FLAGS.get(64)) { //if a toolbox
         toolbox = ptr;
         break;
      }//if
   }//while

   if (!toolbox) {
      show("You must have a toolbox in order to construct something.", 
           pc);
      return -1;
   }//if

   if (toolbox->inv.isEmpty()) {
      show("You need components in your toolbox to work with!\n", 
           pc);
      return -1;
   }//if

		/* found a tool box, make sure its clear of junk */
   toolbox->inv.head(cll);
   while ((ptr = cll.next())) {
      if (!(ptr->OBJ_FLAGS.get(67))) {
         Sprintf(buf, "%S is not a component to any construction.\n",
                 ptr->getShortDesc(&pc));
         buf.Cap();
         show(buf, pc);
         show("You have to have a clean toolbox in order to work!!\n", 
              pc);
         return -1;
      }//if
      if (ptr->OBJ_LEVEL > pc.LEVEL) {
         Sprintf(buf, "%S is too advanced for you to work with it.\n",
                 ptr->getShortDesc(&pc));
         buf.Cap();
         show(buf, pc);
         show("You must remove it from your work ", pc);
         show("area before you can continue.\n", pc);
         return -1;
      }//if
   }//while

   ptr = toolbox->getInv().peekFront();

   if (!ptr->obj_proc) {
      Sprintf(buf, "ERROR:  %S is COMPONENT w/NULL obj_proc.\n", 
              ptr->getShortDesc(&pc));
      mudlog.log(ERR, buf);
      return -1;
   }//if
   if (!ptr->obj_proc->construct_data) {
      Sprintf(buf, "ERROR:  %S is COMPONENT w/NULL construct_data.\n", 
              ptr->getShortDesc(&pc));
      mudlog.log(ERR, buf);
      return -1;
   }//if

                  /* check for all components available */
   if (check_l_range(ptr->COMPONENT_ITEM1, 1, NUMBER_OF_ITEMS, pc, 
       FALSE)) {
      if (!(item1 = toolbox->haveObjNumbered(1, ptr->COMPONENT_ITEM1,
                                             pc.getSeeBit(), ROOM))) {
         Sprintf(buf, "You need %S in order to construct %S.\n",
                 obj_list[ptr->COMPONENT_ITEM1].getLongName(&pc),
                 obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
         show(buf, pc);
         return -1;
      }//if
   }//if

   if (check_l_range(ptr->COMPONENT_ITEM2, 1, NUMBER_OF_ITEMS, pc, 
       FALSE)) {
      if (!(item2 = toolbox->haveObjNumbered(1, ptr->COMPONENT_ITEM2,
                                            pc.SEE_BIT, ROOM))) {
         Sprintf(buf, "You need %S in order to construct %S.\n",
                 obj_list[ptr->COMPONENT_ITEM2].getLongName(&pc),
                 obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
         show(buf, pc);
         return -1;
      }//if
   }//if

   if (check_l_range(ptr->COMPONENT_ITEM3, 1, NUMBER_OF_ITEMS, pc, 
       FALSE)) {
      if (!(item3 = toolbox->haveObjNumbered(1, ptr->COMPONENT_ITEM3,
                                             pc.SEE_BIT, ROOM))) {
         Sprintf(buf, "You need %S in order to construct %S.\n",
                 obj_list[ptr->COMPONENT_ITEM3].getLongName(&pc),
                 obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
         show(buf, pc);
         return -1;
      }//if
   }//if

   if (check_l_range(ptr->COMPONENT_ITEM4, 1, NUMBER_OF_ITEMS, pc, 
       FALSE)) {
      if (!(item4 = toolbox->haveObjNumbered(1, ptr->COMPONENT_ITEM4,
                                            pc.SEE_BIT, ROOM))) { 
         Sprintf(buf, "You need %S in order to construct %S.\n",
                 obj_list[ptr->COMPONENT_ITEM4].getLongName(&pc),
                 obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
         show(buf, pc);
         return -1;
      }//if
   }//if

   if (check_l_range(ptr->COMPONENT_ITEM5, 1, NUMBER_OF_ITEMS, pc, 
       FALSE)) {
      if (!(item5 = toolbox->haveObjNumbered(1, ptr->COMPONENT_ITEM5,
                                             pc.SEE_BIT, ROOM))) { 
         Sprintf(buf, "You need %S in order to construct %S.\n",
                 obj_list[ptr->COMPONENT_ITEM5].getLongName(&pc),
                 obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
         show(buf, pc);
         return -1;
      }//if
   }//if

		/*  have all items and levels are compatable */

   if (item1) {
      toolbox->loseInv(item1);
      drop_eq_effects(*item1, pc, FALSE, FALSE);
      recursive_init_unload(*item1, 0);
      if (item1->isModified()) {
         delete item1;
         item1 = NULL;
      }//if
   }//if
   
   if (item2) {
      toolbox->loseInv(item2);
      drop_eq_effects(*item2, pc, FALSE, FALSE);
      recursive_init_unload(*item2, 0);
      if (item2->isModified()) {
         delete item2;
         item2 = NULL;
      }//if
   }//if
   
   if (item3) {
      toolbox->loseInv(item3);
      drop_eq_effects(*item3, pc, FALSE, FALSE);
      recursive_init_unload(*item3, 0);
      if (item3->isModified()) {
         delete item3;
         item3 = NULL;
      }//if
   }//if
   
   if (item4) {
      toolbox->loseInv(item4);
      drop_eq_effects(*item4, pc, FALSE, FALSE);
      recursive_init_unload(*item4, 0);
      if (item4->isModified()) {
         delete item4;
         item4 = NULL;
      }//if
   }//if
   
   if (item5) {
      toolbox->loseInv(item5);
      drop_eq_effects(*item5, pc, FALSE, FALSE);
      recursive_init_unload(*item5, 0);
      if (item5->isModified()) {
         delete item5;
         item5 = NULL;
      }//if
   }//if
   
   if (skill_did_hit(pc, CONSTRUCT_SKILL_NUM, pc)) {
      pc.gainInv(&(obj_list[ptr->COMPONENT_TARG]));
      recursive_init_loads(obj_list[ptr->COMPONENT_TARG], 0);
      Sprintf(buf, "You have successfully constructed %S.\n",
              obj_list[ptr->COMPONENT_TARG].getShortDesc(&pc));
      show(buf, pc);
   }//if skill_did_hit, ie if pc knew it well enuf not to fail
   else {
      show("You fiddle with your components, but all you can seem to", pc);
      show(" do is bugger them up completely.\n", pc);
   }//else
   pc.PAUSE += 4;
   return 0;
}//construct()



int concoct(critter& pc, short do_mob = FALSE) {
   String buf(100);
   object* cauldron, *ptr, *item1, *item2, *item3, *item4, *item5;
   SCell<object*> cll;
   //   log("In concoct.\n");

   if (!pc.pc && !do_mob) //no ordering pets to do yer dirty-work
     return -1;

   item1 = item2 = item3 = item4 = item5 = NULL;

   if (!ok_to_do_action(NULL, "mBFPSK", BREW_SKILL_NUM, pc)) {
      return -1;
   }

   pc.inv.head(cll);
   cauldron = NULL;
   while ((ptr = cll.next())) {
      if (ptr->OBJ_FLAGS.get(65)) { //if a cauldron
         cauldron = ptr;
         break;
      }//if
   }//while

   if (!cauldron) {
      show("You must have a cauldron in order to brew something.", 
           pc);
      return -1;
   }//if

   if (cauldron->getInv().isEmpty()) {
      show("You need ingredients in your cauldron to brew with!\n", 
           pc);
      return -1;
   }//if

		/* found a cauldron, make sure its clear of junk */
   cauldron->inv.head(cll);
   while ((ptr = cll.next())) {
      if (!(ptr->OBJ_FLAGS.get(68))) { //concoct component
         Sprintf(buf, 
             "%S will certainly distort the properties of your brew.  ",
             ptr->getLongName(&pc));
         buf.Cap();
         show(buf, pc);
         show("You'd better take it out.\n", pc);
         return -1;
      }//if
      if (ptr->OBJ_LEVEL > pc.LEVEL) {
         Sprintf(buf, "You do not yet fully comprehend the power of %S.\n",
                 ptr->getLongName(&pc));
         buf.Cap();
         show(buf, pc);
         show("Until you better understand it, you shouldn't include", pc);
         show(" it in your cauldron.\n", pc);
         return -1;
      }//if
   }//while

   ptr = cauldron->getInv().peekFront();

   if (!ptr->obj_proc) {
      Sprintf(buf, "ERROR:  Object# %i is COMPONENT w/NULL obj_proc.\n", 
             ptr->getIdNum());
      mudlog.log(ERR, buf);
      return -1;
   }//if
   if (!ptr->obj_proc->construct_data) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  object# " << ptr->getIdNum()
                << " is COMPONENT w/NULL construct_data.\n";
      }
      return -1;
   }//if

                  /* check for all components available */
   if (check_l_range(ptr->COMPONENT_ITEM1, 1, NUMBER_OF_ITEMS, pc, 
       FALSE)) {
      if (!(item1 = cauldron->haveObjNumbered(1, ptr->COMPONENT_ITEM1,
                                              pc.SEE_BIT, ROOM))) {
         Sprintf(buf, "You need %S in order to brew %S.\n",
                 obj_list[ptr->COMPONENT_ITEM1].getLongName(&pc),
                 obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
         show(buf, pc);
         return -1;
      }//if
   }//if

   if (check_l_range(ptr->COMPONENT_ITEM2, 1, NUMBER_OF_ITEMS, pc, 
       FALSE)) {
      if (!(item2 = cauldron->haveObjNumbered(1, ptr->COMPONENT_ITEM2,
                                              pc.SEE_BIT, ROOM))) {
         Sprintf(buf, "You need %S in order to brew %S.\n",
                 obj_list[ptr->COMPONENT_ITEM2].getLongName(&pc),
                 obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
         show(buf, pc);
         return -1;
      }//if
   }//if

   if (check_l_range(ptr->COMPONENT_ITEM3, 1, NUMBER_OF_ITEMS, pc, 
       FALSE)) {
      if (!(item3 = cauldron->haveObjNumbered(1, ptr->COMPONENT_ITEM3,
                                              pc.SEE_BIT, ROOM))) {
         Sprintf(buf, "You need %S in order to brew %S.\n",
                 obj_list[ptr->COMPONENT_ITEM3].getLongName(&pc),
                 obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
         show(buf, pc);
         return -1;
      }//if
   }//if

   if (check_l_range(ptr->COMPONENT_ITEM4, 1, NUMBER_OF_ITEMS, pc, 
       FALSE)) {
      if (!(item4 = cauldron->haveObjNumbered(1, ptr->COMPONENT_ITEM4,
                                              pc.SEE_BIT, ROOM))) { 
         Sprintf(buf, "You need %S in order to brew %S.\n",
                 obj_list[ptr->COMPONENT_ITEM4].getLongName(&pc),
                 obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
         show(buf, pc);
         return -1;
      }//if
   }//if

   if (check_l_range(ptr->COMPONENT_ITEM5, 1, NUMBER_OF_ITEMS, pc, 
       FALSE)) {
      if (!(item5 = cauldron->haveObjNumbered(1, ptr->COMPONENT_ITEM5,
                                              pc.SEE_BIT, ROOM))) { 
         Sprintf(buf, "You need %S in order to brew %S.\n",
                 obj_list[ptr->COMPONENT_ITEM5].getLongName(&pc),
                 obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
         show(buf, pc);
         return -1;
      }//if
   }//if

		/*  have all items, and levels are compatable */

   if (item1) {
      cauldron->loseInv(item1);
      drop_eq_effects(*item1, pc, FALSE);
      recursive_init_unload(*item1, 0);
      if (item1->isModified()) {
         delete item1;
         item1 = NULL;
      }//if
   }//if
   
   if (item2) {
      cauldron->loseInv(item2);
      drop_eq_effects(*item2, pc, FALSE);
      recursive_init_unload(*item2, 0);
      if (item2->isModified()) {
         delete item2;
         item2 = NULL;
      }//if
   }//if
   
   if (item3) {
      cauldron->loseInv(item3);
      drop_eq_effects(*item3, pc, FALSE);
      recursive_init_unload(*item3, 0);
      if (item3->isModified()) {
         delete item3;
         item3 = NULL;
      }//if
   }//if
   
   if (item4) {
      cauldron->loseInv(item4);
      drop_eq_effects(*item4, pc, FALSE);
      recursive_init_unload(*item4, 0);
      if (item4->isModified()) {
         delete item4;
         item4 = NULL;
      }//if
   }//if
   
   if (item5) {
      cauldron->loseInv(item5);
      drop_eq_effects(*item5, pc, FALSE);
      recursive_init_unload(*item5, 0);
      if (item5->isModified()) {
         delete item5;
         item5 = NULL;
      }//if
   }//if
   
   if (skill_did_hit(pc, BREW_SKILL_NUM, pc)) {
      pc.gainInv(&(obj_list[ptr->COMPONENT_TARG]));
      recursive_init_loads(obj_list[ptr->COMPONENT_TARG], 0);
      Sprintf(buf, "You have successfully brewed %S.\n",
              obj_list[ptr->COMPONENT_TARG].getLongName(&pc));
      show(buf, pc);
   }//if skill_did_hit, ie if pc knew it well enuf not to fail
   else {
      show("You try to remember the recipe, but you must have", pc);
      show(" forgotten something because this stuff stinks and your ", pc);
      show("ingredients are ruined!!\n", pc);
   }//else
   pc.PAUSE += 4;
   return 0;
}//concoct()


int scribe(const String* spell, critter& pc, short do_mob = FALSE) {
   String buf(100);
   object* pen, *parchment; 
   int spell_num;

   /* default is that mob's can't scribe, ie no 'ordering' them to scribe */
   if (!pc.pc && !do_mob)
     return -1;

   //   log("In scribe.\n");

	// level of spell can't be higher than parchment level 
        // and or pen level.
        // Can only scribe spells the pc knows of course, unless its 
        // a SMOB
        // Must be wielding pen
        // Must be holding parchment
        // Resulting scroll goes to inventory

   if (spell->Strlen() == 0) {
      show("Please specify which spell you wish to scribe.\n", pc);
      return -1;
   }//if

   if (!ok_to_do_action(NULL, "mBFPr", spell_num, pc)) {
      return -1;
   }

   pen = pc.EQ[9];
   if (!pen || !pen->OBJ_FLAGS.get(66)) {
      show("You must be wielding a pen in order to scribe.\n", pc);
      return -1;
   }//if

		// pen is valid, check for parchment //

   parchment = pc.EQ[10];
   if (!parchment || !parchment->OBJ_FLAGS.get(69)) {
      show("You must be holding a parchment in order to scribe.\n", pc);
      return -1;
   }//if

		// parchment and pen are valid, check on spell //

   spell_num = SSCollection::instance().getNumForName(*spell);
   if (spell_num == -1) {
      show("That spell is in need of research.\n", pc);
      return -1;
   }//if

   /* does pc know the spell?? */

   int p_learned = get_percent_lrnd(spell_num, pc);
   if (p_learned == -1) {
     show("You don't know of that spell.\n", pc);
     return -1;
   }//if

		// have valid spell number, now check levels

   int spell_lvl = SSCollection::instance().getSS(spell_num).getMinLevel();
   
   if (spell_lvl > pen->OBJ_LEVEL) {
      show("Your pen is too weak to convey such magic.\n", pc);
      return -1;
   }//if

   if (spell_lvl > parchment->OBJ_LEVEL) {
      show("Your parchment cannot hold such strong words of magic.\n", pc);
      return -1;
   }//if

            // have all items, levels are compatable //
   int scroll_num;
   if (!lost_concentration(pc, spell_num)) {
      if ((scroll_num = SSCollection::instance().getSS(spell_num).getScrollNum()) != -1) {

         recursive_init_unload(*(pc.EQ[10]), 0);
         if (pc.EQ[10]->isModified()) 
            delete pc.EQ[10];
         remove_eq_effects(*(pc.EQ[10]), pc, FALSE, FALSE, 10);
         pc.EQ[10] = NULL; //no more parchment

         pc.gainInv(&(obj_list[scroll_num]));

         recursive_init_loads(obj_list[scroll_num], 0);

         Sprintf(buf, "You have successfully scribed %S.\n",
              obj_list[scroll_num].getLongName(&pc));
         show(buf, pc);
         pc.MANA -= get_mana_cost(spell_num);
         pc.PAUSE += 4;
      }//if
      else { //scroll object not created yet
         Sprintf(buf, "ERROR:  need to create scroll for spell:  %i.\n",
                 spell_num);
         mudlog.log(ERR, buf);
         show("As you finish the last phrase the parchement grows hot and",
              pc);
         show(" bursts into flame.\nThis spell just doesn't take to paper", 
              pc);
         show(" very well!\n", pc);
         if (pc.EQ[10]->isModified()) 
            delete pc.EQ[10];
         pc.EQ[10] = NULL; //no more parchment
      }//else
   }//if !lost concentration, if he/she knew it well enuf
   else {
      show("You suddenly lose your train of thought, and as you mark", pc);
      show(" through that last line, you know the parchment is useless.\n",
           pc);
      if (pc.EQ[10]->isModified()) 
         delete pc.EQ[10];
      pc.EQ[10] = NULL; //no more parchment
      pc.PAUSE += 4;
      pc.MANA -= (get_mana_cost(spell_num) / 2);
   }//else
   return 0;
}//scribe()



///*********************************************************************///
//************************ auxillary functions **************************//
///*********************************************************************///


short did_spell_hit(const critter& agg, const int spell_type,
		    const critter& vict, int lvl = -1, 
		    short is_canned = FALSE) {

  int j = 100;
  switch (spell_type)
     {
     case NORMAL:
        j = vict.AC;
        break;
     case FIRE: case D_BREATH:
        j =  vict.HEAT_RESIS;
        break;
     case ICE:
        j = vict.COLD_RESIS;
        break;
     case ELECTRICITY:
        j = vict.ELEC_RESIS;
        break;
     case CRONIC:
        j = ((18 - vict.CON) * 5 + 25);
        break;
     case AGILITY:
        j = ((18 - vict.DEX) * 5 + 25);
        break;
     case COERCION:
     case CHARM:
        j = ((18 - vict.INT) * 5 + 25);
        break;
     default:
        if (mudlog.ofLevel(ERR)) {
           mudlog << "ERROR:  default in did_spell_hit, spelltype:  "
                  << spell_type << endl;
        }
        j = 100;
     }//switch   
  
  j += vict.SPEL_RESIS; //gen resistance to spells
      
  if (is_canned) {
    j -= (vict.LEVEL - lvl) * 10;
  }//if
  else {
    j -= (vict.LEVEL - agg.LEVEL) * 10;
  }//else

  j += 200; 

  if (j < 0)
    j = 0;
  else if (j > 400)
    j = 400;

  return (d(1, 75) < d(1, j));
}//did_spell_hit


short skill_did_hit(critter& agg, int spell_num, critter& vict) {
   int percent_lrnd = 0;

   if (agg.pc) {
      agg.SKILLS_KNOWN.Find(spell_num, percent_lrnd);
   }//if
   else { //else not a pc
      percent_lrnd = 100;
   }//else
   
   if ((spell_num == CONSTRUCT_SKILL_NUM) || (spell_num == BREW_SKILL_NUM))
     return (d(1, 100) < d(1, (percent_lrnd * 2 + 2 * (agg.DEX + agg.INT))));

   else if ((spell_num == BODYSLAM_SKILL_NUM) ||
	    (spell_num == HURL_SKILL_NUM))
     return (d(1, vict.getCurWeight()) < d(1, percent_lrnd + agg.STR * 10));
   else if ((spell_num == BACKSTAB_SKILL_NUM) ||
	    (spell_num == CIRCLE_SKILL_NUM)) {
      int rnd1 = d(1, (agg.HIT * 2 + agg.DEX * 5 + vict.AC - 80));
      int rnd2  = d(1, (2 * vict.DEX + vict.LEVEL - agg.LEVEL + 100));
      return (((float)(rnd1) * (float)(percent_lrnd) / 100.0) >
              ((float)(rnd2)));
   }
   else {
     return (d(1, (agg.HIT + agg.DEX + 2 * agg.LEVEL * percent_lrnd)) >
	     d(1, (vict.DEX + 2 * vict.LEVEL * 100)));
   }
}//skill_did_hit


int sneak(critter& pc, int smob_too = FALSE) {
   String buf(100);
   
   if (pc.isMob())
      return -1; //no MOB's allowed!!
   
   if (!smob_too && !pc.pc) {
      return -1;
   }//if 
   
   
   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return -1;
   }//if
   
   if (pc.CRIT_FLAGS.get(17)) {
      pc.CRIT_FLAGS.turn_off(17);
      show("You stop sneaking around.\n", pc);
   }//if
   else {
      if (get_percent_lrnd(SNEAK_SKILL_NUM, pc) > 0) {
         pc.CRIT_FLAGS.turn_on(17);
         show("You start sneaking.\n", pc);
      }//if
      else {
         show("You couldn't sneak up on a sleeping Ogrue!!\n", pc);
      }//else
   }//else, not sneaking right now
   return 0;
}//sneak


int hide(critter& pc, int smob_too = FALSE) {
   String buf(100);

   if (pc.isMob())
      return -1; //no MOB's allowed!!
   
   if (!smob_too && !pc.pc) {
      return -1;
   }//if 
   
   if (pc.isFrozen()) {
      show("You are too frozen to hide.\n", pc);
      return -1;
   }//if
   
   if (pc.CRIT_FLAGS.get(22)) {
      pc.CRIT_FLAGS.turn_off(22);
      show("You stop hiding.\n", pc);
   }//if
   else {
      if (get_percent_lrnd(HIDE_SKILL_NUM, pc) > 0) {
         pc.CRIT_FLAGS.turn_on(22);
         show("You start hiding.\n", pc);
      }//if
      else {
         show("You are not skilled in this stealthy art!!\n", pc);
      }//else
   }//else, not hiding right now
   return 0;
}//hide

/*  Basically, just hide for rangers. */
int blend(critter& pc, int smob_too = FALSE) {
   String buf(100);

   if (pc.isMob())
      return -1; //no MOB's allowed!!
   
   if (!smob_too && !pc.pc) {
      return -1;
   }//if 
   
   if (pc.isFrozen()) {
      show("You are too frozen to hide.\n", pc);
      return -1;
   }//if
   
   if (pc.CRIT_FLAGS.get(22)) {
      pc.CRIT_FLAGS.turn_off(22);
      show("You step out of hiding.\n", pc);
   }//if
   else {
      if (get_percent_lrnd(BLEND_SKILL_NUM, pc) > 0) {
         pc.CRIT_FLAGS.turn_on(22);
         show("You blend into the background.\n", pc);
      }//if
      else {
         show("You are not skilled in this stealthy art!!\n", pc);
      }//else
   }//else, not hiding right now
   return 0;
}//blend
	  

int scan(critter& pc) {

   if (!ok_to_do_action(NULL, "KSPF", SCAN_SKILL_NUM, pc)) {
      return -1;
   }//if

   return ROOM.doScan(pc);
}//scan









