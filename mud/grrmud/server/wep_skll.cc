// $Id: wep_skll.cc,v 1.10 2002/08/30 06:22:27 eroper Exp $
// $Revision: 1.10 $  $Author: eroper $ $Date: 2002/08/30 06:22:27 $

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

//*********************** wep_skll.cc ********************//

#include "misc.h"
#include "misc2.h"
#include "classes.h"
#include "spells.h"
#include "battle.h"
#include "skills.h"
#include "commands.h"
#include "batl_prc.h"
#include "wep_skll.h"
#include <PtrArray.h>


int circle(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   if (victim->Strlen() == 0)
      crit_ptr = Top(pc.IS_FIGHTING);
   else
      crit_ptr = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);

   if (crit_ptr) {

      if (!ok_to_do_action(crit_ptr, "mSVFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
         return -1;
      }

      if (!HaveData(crit_ptr, pc.IS_FIGHTING)) {
         show("You must be fighting a person to circle him.\n", pc);
         return -1;
      }//if

      if (crit_ptr->IS_FIGHTING.peekFront() == &pc) {
         pc.show("You cannot break free to circle around.\n");
         return -1;
      }

      if (!(pc.EQ[9] && pc.EQ[9]->OBJ_FLAGS.get(43)) &&
         !(pc.EQ[10] && pc.EQ[10]->OBJ_FLAGS.get(43))) {
         show("You must be holding or wielding a piercing weapon to circle.\n", pc);
         return -1;
      }//if

      return do_circle(*crit_ptr, pc);
   }//if
   else {
      show("Circle who?\n", pc);
      return -1;
   }
}//circle


int do_circle(critter& vict, critter& pc) {
   String buf(100);
   String* weap_name;
   short do_fatality = FALSE;
   int wd;

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERROR, "ERROR:  smob sent in to do_circle.\n");
      return -1;
   }//if

   if (skill_did_hit(pc, CIRCLE_SKILL_NUM, vict)) { 

      if(pc.EQ[9] && pc.EQ[9]->OBJ_FLAGS.get(43)) {
         wd = pc.DAM + d(pc.EQ[9]->OBJ_DAM_DICE_COUNT, 
                         pc.EQ[9]->OBJ_DAM_DICE_SIDES);
         weap_name = long_name_of_obj(*(pc.EQ[9]), ~0);
      }
      else if(pc.EQ[10] && pc.EQ[10]->OBJ_FLAGS.get(43)) {
         // People with dual wield can circle sometimes.
         if (!(d(1,100) < 
               d(1, (int)(((float)(get_percent_lrnd(DUAL_WIELD_SKILL_NUM,pc)) * 
                    (float)(pc.DEX) / 10.0))))) {
            Sprintf(buf, "You whirl just in time to avoid %S's blade in your back.\n", 
                 name_of_crit(pc, vict.SEE_BIT));
            show(buf, vict);

            Sprintf(buf, "You nick your forearm trying to circle %S.\n",
                 name_of_crit(vict, pc.SEE_BIT));
            show(buf, pc);

            Sprintf(buf, "avoids %S's attempt to circle around %s.",
                    name_of_crit(pc, ~0), get_his_her(vict));
            emote(buf, vict, ROOM, TRUE, &pc);

            pc.PAUSE += 3;
            return 0;
         }
         wd = pc.DAM + d(pc.EQ[10]->OBJ_DAM_DICE_COUNT, 
                         pc.EQ[10]->OBJ_DAM_DICE_SIDES);
         weap_name = long_name_of_obj(*(pc.EQ[10]), ~0);
      }
      else {
         mudlog.log(ERROR, "ERROR:  do_circle: pc.EQ[9] or pc.EQ[10] isn't a dagger\n");
         return -1;
      }
      exact_raw_damage(((pc.LEVEL / 5 + 1) * wd / 2), NORMAL, vict, pc);

      if (vict.HP < 0) { //do fatality
         show("You collapse as a blade enters your back!\n", vict);
         Sprintf(buf, 
        "places %S in %S's back, producing a lot of blood and a corpse!\n",
                 weap_name, name_of_crit(vict, ~0));
         emote(buf, pc, ROOM, TRUE, &vict);
         Sprintf(buf, "%S's body goes limp in your arms.\n", 
                 name_of_crit(vict, pc.SEE_BIT));
         show(buf, pc);
         do_fatality = TRUE;
      }//if fatality
      else { //no fatality
         Sprintf(buf, "%S circles you and places a blade in your back!\n", 
                 name_of_crit(pc, vict.SEE_BIT));
         show(buf, vict);
         Sprintf(buf, "circles and places %S in %S's back.",
                 weap_name, name_of_crit(vict, ~0));
         emote(buf, pc, ROOM, TRUE, &vict);
         Sprintf(buf, "You circle and place %S in %S's back.", 
                 weap_name, name_of_crit(vict, pc.SEE_BIT));
         show(buf, pc);
      }//else
   }//if hit
   else {  //missed
      Sprintf(buf, 
        "You whirl just in time to avoid %S's blade in your back.\n", 
                 name_of_crit(pc, vict.SEE_BIT));
      show(buf, vict);

      Sprintf(buf, "You nick your forearm trying to circle %S.\n",
                 name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);

      Sprintf(buf, "avoids %S's attempt to circle around %s.",
              name_of_crit(pc, ~0), get_him_her(vict));
      emote(buf, vict, ROOM, TRUE, &pc);
   }//else

   if (do_fatality) {
      agg_kills_vict(&pc, vict);
   }//if

   pc.PAUSE += 3;
   return 0;
}//do_circle



int backstab(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   crit_ptr = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   
   if (crit_ptr) {
      if (!ok_to_do_action(crit_ptr, "mBSVFP", 0, pc, pc.getCurRoom(), NULL,
                           TRUE)) {
         return -1;
      }

      if (crit_ptr == &pc) {
         show("You can't backstab yourself!!\n", pc);
         return -1;
      }//if

      if( (!(pc.isHiding())) || (!(pc.isInvis())) ) {
         pc.show("You must be hiding to backstab.\n") ;
         return -1;
      }

      if (!(pc.EQ[9] && pc.EQ[9]->OBJ_FLAGS.get(43)) &&
         !(pc.EQ[10] && pc.EQ[10]->OBJ_FLAGS.get(43))) {
            show("You must be holding or wielding a piercing weapon to backstab.\n", pc);
            return -1;
      } // else

      if (crit_ptr->isMob()) {
         crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum(), TRUE, i_th, victim,
                                pc.SEE_BIT);
      }//if

      if (!(crit_ptr = check_for_diversions(*crit_ptr, "GM", pc)))
         return -1;

      if (!IsEmpty(crit_ptr->IS_FIGHTING)) {
         show("The element of supprise has been lost.\n", pc);
         return do_hit(*crit_ptr, pc);
      }//if

      return do_backstab(*crit_ptr, pc);
   }//if
   else {
      show("Backstab who?\n", pc);
      return -1;
   }
}//backstab


int do_backstab(critter& vict, critter& pc) {
   String buf(100);
   String* weap_name;
   short do_fatality = FALSE;
   int wd;

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERROR, "ERROR:  MOB sent to do_backstab.\n");
      return -1;
   }//if

   if (!HaveData(&vict, pc.IS_FIGHTING)) {
      join_in_battle(pc, vict);
   }//if

   if (skill_did_hit(pc, BACKSTAB_SKILL_NUM, vict)) { 

      if(pc.EQ[9] && pc.EQ[9]->OBJ_FLAGS.get(43)) {
         wd = pc.DAM + d(pc.EQ[9]->OBJ_DAM_DICE_COUNT, 
                         pc.EQ[9]->OBJ_DAM_DICE_SIDES);
         weap_name = long_name_of_obj(*(pc.EQ[9]), ~0);
      }
      else if(pc.EQ[10] && pc.EQ[10]->OBJ_FLAGS.get(43)) {
         wd = pc.DAM + d(pc.EQ[10]->OBJ_DAM_DICE_COUNT, 
                         pc.EQ[10]->OBJ_DAM_DICE_SIDES);
         weap_name = long_name_of_obj(*(pc.EQ[10]), ~0);
      }
      else {
         mudlog.log(ERROR, "ERROR:  do_backstab: pc.EQ[9] or pc.EQ[10] isn't a dagger\n");
         return -1;
      }

      exact_raw_damage(((pc.LEVEL / 4 + 1) * wd), NORMAL, vict, pc);

      vict.PAUSE += d(1,2);

      if (vict.HP < 0) { //do fatality
         show("You collapse as a blade enters your back!\n", vict);
         Sprintf(buf, 
        "places %S in %S's back, producing a lot of blood and a corpse!\n",
                 weap_name, name_of_crit(vict, ~0));
         emote(buf, pc, ROOM, TRUE, &vict);
         Sprintf(buf, "%S's body goes limp in your arms.\n", 
                 name_of_crit(vict, pc.SEE_BIT));
         show(buf, pc);
         do_fatality = TRUE;
      }//if fatality
      else { //no fatality
         Sprintf(buf, "%S places a blade in your back!\n", 
                 name_of_crit(pc, vict.SEE_BIT));
         show(buf, vict);
         Sprintf(buf, "places %S in %S's back.",
                 weap_name, name_of_crit(vict, ~0));
         emote(buf, pc, ROOM, TRUE, &vict);
         Sprintf(buf, "You place %S in %S's back.\n", 
                 weap_name, name_of_crit(vict, pc.SEE_BIT));
         show(buf, pc);
      }//else
   }//if hit
   else {  //missed

           pc.PAUSE += d(1,3);

           Sprintf(buf, 
                           "You whirl just in time to avoid %S's blade in your back.\n", 
                           name_of_crit(pc, vict.SEE_BIT));
           show(buf, vict);

           Sprintf(buf, "You nearly cut your finger off trying to backstab %S.\n",
                           name_of_crit(vict, pc.SEE_BIT));
           show(buf, pc);

           Sprintf(buf, "avoids %S's attempt to backstab %s.",
                           name_of_crit(pc, ~0), get_him_her(vict));
           emote(buf, vict, ROOM, TRUE, &pc);
   }//else

   if (do_fatality) {
      agg_kills_vict(&pc, vict);
   }//if
   return 0;
}//do_backstab


int disarm(int i_th, const String* victim, critter& pc) {
   critter* crit_ptr;
   String buf(100);

   crit_ptr = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);

   if (crit_ptr) {
      if (!ok_to_do_action(crit_ptr, "mSVFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
         return -1;
      }

      if (crit_ptr == &pc) {
         show("You slap your wrist and feel stupid!!\n", pc);
         return -1;
      }//if

      if (crit_ptr->isMob()) {
         crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum(), TRUE, i_th, victim,
                                pc.SEE_BIT);
      }//if


      if (!(crit_ptr = check_for_diversions(*crit_ptr, "GSMgos", pc)))
         return -1;

      return do_disarm(*crit_ptr, pc);
   }//if
   else {
      show("Disarm who?\n", pc);
      return -1;
   }
}//disarm


int do_disarm(critter& vict, critter& pc) {
   String buf(100);
   int posn = 9; //wielded

   if ((vict.isMob()) || (pc.isMob())) {
      mudlog.log(ERROR, "ERROR:  MOB sent to do_disarm.\n");
      return -1;
   }//if

   if (!pc.IS_FIGHTING.haveData(&vict)) {
      join_in_battle(pc, vict);
   }//if

   if (!vict.EQ[posn])
      posn = 10;

   if (!vict.EQ[posn]) {
      Sprintf(buf, "%s isn't wielding anything!\n", get_he_she(vict));
      buf.Cap();
      show(buf, pc);
      return -1;
   }//if

   int missed = FALSE;
   if (d(1, 100) > d(1, pc.STR * 10 + 
                     get_percent_lrnd(SWORDBOND_SKILL_NUM, pc))) {
      missed = TRUE;
   }//if
   
   pc.PAUSE += 2;
   vict.PAUSE += d(1,3);
   
   if (!missed && skill_did_hit(pc, DISARM_SKILL_NUM, vict)) { 
      Sprintf(buf, "%S disarms you!!\n",
              name_of_crit(pc, vict.SEE_BIT));
      buf.Cap();
      show(buf, vict);
      Sprintf(buf, "deftly disarms %S!!",
              name_of_crit(vict, ~0));
      emote(buf, pc, ROOM, TRUE, &vict);
      Sprintf(buf, "You deftly disarm %S.\n",
              name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);
      
      vict.gainInv(vict.EQ[posn]); //person does NOT lose eq
      remove_eq_effects(*(vict.EQ[posn]), vict, FALSE, FALSE, posn);
      vict.EQ[posn] = NULL;
   }//if
   else {  //missed
      Sprintf(buf, 
              "You keep a tight hold on your weapon as %S tries to disarm you.\n", 
              name_of_crit(pc, vict.SEE_BIT));
      show(buf, vict);
      
      Sprintf(buf, "You fail to disarm %S.\n",
              name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);
      
      Sprintf(buf, "fails to disarm %S!",
              name_of_crit(vict, ~0));
      emote(buf, pc, ROOM, TRUE, &vict);
   }//else
   return 0;
}//do_disarm

