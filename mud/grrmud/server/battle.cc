// $Id: battle.cc,v 1.25 1999/08/10 07:06:17 greear Exp $
// $Revision: 1.25 $  $Author: greear $ $Date: 1999/08/10 07:06:17 $

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

///**********************  Battle and related functions ******************///

#include "battle.h"
#include "misc.h"
#include "misc2.h"
#include "spec_prc.h"
#include <iostream.h>
#include <fstream.h>
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include <stdio.h>
#include "classes.h"
#include <string2.h>
#include <bitfield.h>
#include "spells.h"
#include "skills.h"
#include <PtrArray.h>
#include "load_wld.h"
#include "const.h"
#include "Filters.h"
#include "batl_prc.h"


short can_start_battle(critter& targ, critter& pc, short do_msg) {
   String buf(100);

   if ((targ.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  can_start battle sent SMOB's.\n");
      return FALSE;
   }//if

   if (room_list[targ.getCurRoomNum()].isHaven()) {
      if (do_msg) {
         Sprintf(buf, "%S is in a peaceful room.\n", 
	         name_of_crit(targ, pc.SEE_BIT));
         buf.Cap();
         show(buf, pc);
      }//if
      return FALSE;
   }//if
   else if (room_list[targ.getCurRoomNum()].isNoPK() && 
	   (pc.pc && targ.pc)) {
      if (do_msg) {
         Sprintf(buf, "%S is in a safe room.\n", 
		 name_of_crit(targ, pc.SEE_BIT));
         buf.Cap();
         show(buf, pc);
      }//if
      return FALSE;
   }//if
   else if (ROOM.isHaven()) {
      if (do_msg) {
	 show("But you are in such peaceful room!\n", pc);
      }//if
      return FALSE;
   }//if
   else if (ROOM.isNoPK() && (pc.pc && targ.pc)) {
      if (do_msg) {
	 show("You are in a safe room.\n", pc);
      }//if
      return FALSE;
   }//if

   return TRUE;
}//can_start_battle()



void do_battle() {
   Cell<room*> rcell;
   embattled_rooms.head(rcell);
   room* r_ptr;
   Cell<critter*> crit_cell;
   critter* crit_ptr, *vict_ptr;
   short is_embattled, i;
   
   //log("In do_battle()\n");
   //   batlog << "Size of embattled_rooms:  " << Size(embattled_rooms) 
   //          << endl;

   r_ptr = rcell.next();
   while (r_ptr) {
      is_embattled = FALSE;
      List<critter*> tmp_list(r_ptr->getCrits());
      tmp_list.head(crit_cell);

      while ((crit_ptr = crit_cell.next())) {

         if (!r_ptr->haveCritter(crit_ptr)) {
            continue;
         }

         if (mudlog.ofLevel(DBG)) {
            mudlog << "In large while, crit_ptr:  " << crit_ptr << flush
                   << " name: "
                   << crit_ptr->getName() << " size of IS_FIGHTING:  "
                   << crit_ptr->IS_FIGHTING.size() << endl << endl;
         }//if


         // NOTE:  The aggressor can never die, but the HP can go
         // as low as needed, so that the next round may kill.

         if (!crit_ptr->IS_FIGHTING.isEmpty()) {
            is_embattled = TRUE;
            
            if (crit_ptr->isUsingClient()) {
               show("<BATTLE>", *crit_ptr);
            }
            else if (crit_ptr->isUsingColor()) {
               show(*(crit_ptr->getBattleColor()), *crit_ptr);
            }

            int atks = crit_ptr->ATTACKS;
            if (crit_ptr->ATTACKS > 4)
               atks = 4;
            if (crit_ptr->ATTACKS < 1)
               atks = 1;

            for (i = 0; i< atks; i++) {
	             /* check for second attack */
	       if ((i == 1) && (crit_ptr->pc)) {
                  if ((crit_ptr->PAUSE > 0) ||
                      (d(1, 100) >
                       d(1, (get_percent_lrnd(SECOND_ATTACK_SKILL_NUM,
                                              *(crit_ptr)) + crit_ptr->LEVEL) 
                         * (int)((float)(crit_ptr->DEX) / 9.0)))) {
                     continue; //didn't get second hit
		  }//if
	       }//if

               if (!crit_ptr->IS_FIGHTING.isEmpty()) { 
                      //if first hit kills, abort
            
		  vict_ptr = Top(crit_ptr->IS_FIGHTING);

                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "In do_battle, within for loop, i:  "
                            << i << " aggressor:  " 
                            << *(name_of_crit(*crit_ptr, ~0)) << " addr:  "
                            << crit_ptr << flush << " victim addr:  "
                            << vict_ptr << " vict_name:  "
                            << *(name_of_crit(*vict_ptr, ~0)) << endl;
                  }

                  int show_vict_tags = TRUE;
		  if ((vict_ptr = check_for_diversions(*vict_ptr, "GM", 
						      *crit_ptr))) {

                     if (vict_ptr->isUsingClient()) {
                        vict_ptr->show("<BATTLE>");
                     }
                     else if (vict_ptr->isUsingColor()) {
                        show(*(vict_ptr->getBattleColor()), *vict_ptr);
                     }

                     //do reg attack
                     do_battle_round(*crit_ptr, *vict_ptr, 9, show_vict_tags);
                     
                     if (crit_ptr->isNpc()) {
                        do_battle_proc(*crit_ptr);
                     }//if
		  }//if not a mirror

                  // NOTE:  vict_ptr could be deleted at this point!!
                  if (vict_ptr && r_ptr->haveCritter(vict_ptr)) {
                     //Vict CANNOT be deleted at this point..or you're screwed!
                     if (vict_ptr->isUsingClient()) {
                        show("</BATTLE>", *vict_ptr);
                     }
                     else if (vict_ptr->isUsingColor()) {
                        show(*(vict_ptr->getDefaultColor()), *vict_ptr);
                     }
                  }//if
               }//if
            }//for		/* now check for dual wield */

            if (!IsEmpty(crit_ptr->IS_FIGHTING) && 
                crit_ptr->CRIT_FLAGS.get(16) && (crit_ptr->PAUSE <= 0)) {
               int val = (int)(((float)(get_percent_lrnd(DUAL_WIELD_SKILL_NUM, 
                                                         *crit_ptr)) *
                                (float)(crit_ptr->DEX) / 10.0));
               if (d(1,100) < d(1, val)) {
                  vict_ptr = Top(crit_ptr->IS_FIGHTING);

                  if (vict_ptr->isUsingClient()) {
                     show("<BATTLE>", *vict_ptr);
                  }
                  else if (vict_ptr->isUsingColor()) {
                     show(*(vict_ptr->getBattleColor()), *vict_ptr);
                  }

                  int show_vict_tags = TRUE; //Show tags if needed.
                  do_battle_round(*crit_ptr, *vict_ptr, 10, show_vict_tags);

                  // Tags will only be shown in do_battle_round if
                  // the victim died.
                  if (show_vict_tags) {
                     if (vict_ptr->isUsingClient()) {
                        show("</BATTLE>", *vict_ptr);
                     }
                     else if (vict_ptr->isUsingColor()) {
                        show(*(vict_ptr->getDefaultColor()), *vict_ptr);
                     }
                  }//if
              }//if
            }//if

            if (crit_ptr->isUsingClient()) {
               show("</BATTLE>", *crit_ptr);
            }
            else if (crit_ptr->isUsingColor()) {
               show(*(crit_ptr->getDefaultColor()), *crit_ptr);
            }
         }//if
      }//while
      if (!is_embattled) {
         r_ptr = embattled_rooms.lose(rcell);
      }//if
      else
         r_ptr = rcell.next();
   }//while    
}//do_battle


void gain_xp(critter& crit, const long exp, const short show_output) {
   crit.EXP += exp;
   if (show_output) {
      String buf(100);
      Sprintf(buf, "You gain %i experience points.\n", exp);
      show(buf, crit);
   }//if
   if (crit.EXP >= levels[crit.LEVEL]) {
      gain_level(crit);
   }//if
}//gain_xp


void do_battle_round(critter& agg, critter& vict, int posn_of_weapon,
                     int& show_vict_tags) {
   float damage, weapon_dam, pos_mult;
   short  is_wielding = FALSE;
   short tp, td;
   String buf(81);
   String aggbuf(25);
   String victbuf(25);
   String wmsg(25);
   String aggendbuf(50);
   String victendbuf(50);
   String otherbuf(50);
   int i;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In do_battle_round, agg addr:  " << &agg << "  name:  "
             << *(name_of_crit(agg, ~0)) << "  vict addr:  " << &vict
             << "  vict name:  " << *(name_of_crit(vict, ~0))
             << "  posn of weapon:  " << posn_of_weapon << endl;
   }

   if (agg.isParalyzed()) {
      agg.emote("is paralyzed, unable to fight back!");
      agg.show("You are paralyzed, unable to move a muscle!\n");
      return;
   }//if

   /* its cheaper to set than test, then set if needed */
   agg.CRIT_FLAGS.turn_off(17); //no sneak
   agg.CRIT_FLAGS.turn_off(22); //no hide

   if ((agg.POS == POS_SIT) || (agg.POS == POS_REST) || (agg.POS == POS_SLEEP)
       || (agg.POS == POS_MED)) {
      if (agg.CRIT_FLAGS.get(15)) { //if is perm_sleeped
          if (d(1,4) == 2) {
             agg.CRIT_FLAGS.turn_off(15);
             agg.emote("seems to come out of a deep slumber!\n");
             agg.setPosn(POS_STAND);
          }//if

          // TODO:  Is this spell different from perm sleep??
          stat_spell_cell* ss_ptr = is_affected_by(SLEEP_SKILL_NUM, agg);
          if (ss_ptr) {
             agg.affected_by.loseData(ss_ptr);
             delete ss_ptr;
          }//if
      }//if
      else if (!agg.isParalyzed() && (agg.PAUSE <= 0)) {
         agg.emote("stands ready to fight.\n");
         agg.setPosn(POS_STAND); //auto stand if possible
      }//if
   }//if

   if (agg.CRIT_FLAGS.get(21)) { //if blocked
      agg.CRIT_FLAGS.turn_off(21);
      return;
   }//if

   if (agg.EQ[posn_of_weapon]) { //wielded
      is_wielding = TRUE;
   }//if

   if (posn_of_weapon == 10) { //if doing dual_wield
      if (!agg.eq[posn_of_weapon])
         return;
      else if (!(agg.eq[posn_of_weapon]->OBJ_FLAGS.get(57)))
         return;
   }//if

   int j = (vict.DEX * 3) - (vict.AC / 10) + vict.LEVEL + (agg.POS * 5);
   i = (agg.DEX * 3) + (agg.HIT * 2) + agg.LEVEL + (vict.POS * 5);

   if ((!vict.isStunned() && (d(1, j) > d(1, i))) ||
       (agg.POS == POS_STUN)) {  //missed, stunned
      if (agg.POS == POS_STUN) {
         show("You lie immobilized by the thought of imminent death.\n", 
              agg);
      }//if
      else {
         //         log("In the else, missed.\n");
         Sprintf(buf, "%S misses you.\n", name_of_crit(agg, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);

         Sprintf(buf, "You miss %S.\n", name_of_crit(vict, agg.SEE_BIT));
         show(buf, agg);

         Sprintf(buf, "misses %S.", name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
      }//else

      return;
   }//if

   td = tp = FALSE;
   float chance = 0.0;
   if (vict.getPause() <= 0) {
      // bigger means less likely to hit, should range from around 1-600
      chance = (float)(d(1, 600)) * (float)agg.DEX / 18.0;
      if ((chance < 100.0) && vict.isStanding()) {
         td = (d(1, get_percent_lrnd(DODGE_SKILL_NUM, vict, TRUE) + vict.DEX * 2) > 50);
      }//if
      if ((!td && vict.EQ[9]) && (vict.isStanding() || vict.isSitting())) {
         // bigger means less likely to hit, should range from around 1-600
         chance = (float)(d(1, 800)) * (float)agg.DEX / 18.0;
         if (chance < 100.0) {
            tp = (d(1, get_percent_lrnd(PARRY_SKILL_NUM, vict, TRUE) + vict.DEX) > 50);
         }
      }//if
   }//if

   if (td) {
      Sprintf(buf, "dodges %S's blow.", name_of_crit(agg, ~0));
      emote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &agg);
     
      Sprintf(buf, "%S dodges your blow.\n", name_of_crit(vict, agg.SEE_BIT));
      buf.Cap();
      show(buf, agg);
     
      Sprintf(buf, "You dodge %S's blow.\n", name_of_crit(agg, vict.SEE_BIT));
      show(buf, vict);
      
      return;
   }//if td

   if (tp) {
      Sprintf(buf, "parries %S's blow.", name_of_crit(agg, ~0));
      emote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &agg);
      
      Sprintf(buf, "%S parries your blow.\n", 
              name_of_crit(vict, agg.SEE_BIT));
      buf.Cap();
      show(buf, agg);
      
      Sprintf(buf, "You parry %S's blow.\n", 
              name_of_crit(agg, vict.SEE_BIT));
      show(buf, vict);
      
      return;
   }//if tp

   /* else, did hit */

//   log("Did hit..going into messages.\n");

   if (is_wielding) { //wielded
      weapon_dam = d((agg.EQ[posn_of_weapon])->OBJ_DAM_DICE_COUNT, 
                     (agg.EQ[posn_of_weapon])->OBJ_DAM_DICE_SIDES);
   }//if
   else {
      weapon_dam = d(agg.BH_DICE_COUNT, agg.BH_DICE_SIDES);
      //batlog << "no weapon, weapon_dam:  " << weapon_dam << endl;
   }//else
     
   //  log("About to enter switch.\n");
   switch (vict.POS) {
     case POS_STAND:
       pos_mult = 1.0;
       break;
     case POS_PRONE: case POS_REST: case POS_SIT:
       pos_mult = 1.5;
       break;
     case POS_SLEEP:
       pos_mult = 2.0;
       break;
     case POS_MED: 
       pos_mult = 3.0;
       break;
     case POS_STUN:
       pos_mult = 4.0;
       break;

     default:
        if (mudlog.ofLevel(ERR)) {
           mudlog << "ERROR:  Default called on switch in battle.cc\n"
                  << "ERROR:  Here is bad vict.pos:  " << vict.POS << endl;
        }
        pos_mult = 1.0;
        break;
   }//switch

	/* no modifiers have been added yet, exact_raw... does that */
   damage = ((float)agg.STR/3.0 + (float)agg.DAM + weapon_dam) *
      pos_mult;

   if (is_wielding) {
      if (((agg.EQ[posn_of_weapon])->obj_flags).get(41))
         wmsg = "slash";
      else if (((agg.EQ[posn_of_weapon])->obj_flags).get(42))
         wmsg = "smash";
      else if (((agg.EQ[posn_of_weapon])->obj_flags).get(43))
         wmsg = "pierce";
      else if (((agg.EQ[posn_of_weapon])->obj_flags).get(44))
         wmsg = "whip";
     else if (((agg.EQ[posn_of_weapon])->obj_flags).get(45))
        wmsg = "shoot";
      else if (((agg.EQ[posn_of_weapon])->obj_flags).get(46))
         wmsg = "sting";
      else if (((agg.EQ[posn_of_weapon])->obj_flags).get(47))
         wmsg = "sting";
     else if (((agg.EQ[posn_of_weapon])->obj_flags).get(48))
        wmsg = "sting";
      else if (((agg.EQ[posn_of_weapon])->obj_flags).get(49))
         wmsg = "jab";
      else {
         wmsg = "injure";
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  got to default finding wmsg, battle.cc, obj#:  "
                   << agg.EQ[posn_of_weapon]->OBJ_NUM << endl
                   << "ERROR:  Name of offending critter:  "
                   << *(name_of_crit(agg, ~0)) << endl;
         }
      }//else
   }//if wielding
   else {
      wmsg = "hit"; //bare handed
   }//else
   
   if (agg.pc && (d(1,10) > 5)) { //1 in 5, !MOB chance of chance
      if (d(1,200) < 
          (agg.getLevel() / 3) *
          (get_percent_lrnd(CRITICAL_STRIKE_SKILL_NUM, agg))) {
         agg.show("You score a critical strike!!\n");
         damage *= 2.0;
      }//if
   }//if

   if (is_wielding && is_affected_by(FROST_BLADE_SKILL_NUM, 
				     *(agg.EQ[posn_of_weapon]))) {
      damage = (float)(exact_raw_damage((int)damage, ICE, vict, agg));
   }//if
   else if (is_wielding && is_affected_by(FIRE_BLADE_SKILL_NUM, 
				     *(agg.EQ[posn_of_weapon]))) {
      damage = (float)(exact_raw_damage((int)damage, FIRE, vict, agg));
   }//if
   else {
      damage = (float)(exact_raw_damage((int)damage, NORMAL, vict, agg));
   }//else

   if (damage < 3) {
      Sprintf(aggbuf, "You bruise %S ", name_of_crit(vict, agg.SEE_BIT));
      Sprintf(victbuf, "%S bruises you ", name_of_crit(agg, 
                                                       vict.SEE_BIT));
      Sprintf(otherbuf, "%S bruises %S ", name_of_crit(agg, ~0), 
              name_of_crit(vict, ~0));
   }//if
   else if (damage < 7) {
      Sprintf(aggbuf, "You barely %S %S ", &wmsg, 
              name_of_crit(vict, agg.SEE_BIT));
      Sprintf(victbuf, "%S barely %Ss you ", 
              name_of_crit(agg, vict.SEE_BIT), &wmsg);
      Sprintf(otherbuf, "%S barely %Ss %S ", name_of_crit(agg, ~0), 
              &wmsg, name_of_crit(vict, ~0));
   }//if
   else if (damage < 11) {
      Sprintf(aggbuf, "You %S %S ", &wmsg, name_of_crit(vict, 
                                                        agg.SEE_BIT));
      if (strcmp("slash", wmsg) == 0) {
         Sprintf(victbuf, "%S slashes you ", name_of_crit(agg, vict.SEE_BIT));
         Sprintf(otherbuf, "%S slashes %S ", name_of_crit(agg, ~0),
                 name_of_crit(vict, ~0));
      }
      else {
         Sprintf(victbuf, "%S %Ss you ", name_of_crit(agg, vict.SEE_BIT), 
                 &wmsg);
         Sprintf(otherbuf, "%S %Ss %S ", name_of_crit(agg, ~0), &wmsg,
                 name_of_crit(vict, ~0));
      }
   }//if
   else if (damage < 15) {
      Sprintf(aggbuf, "You %S %S hard ", &wmsg, name_of_crit(vict, 
                                                             agg.SEE_BIT));
      if (strcmp("slash", wmsg) == 0) {
         Sprintf(victbuf, "%S slashes you hard ",
                 name_of_crit(agg, vict.SEE_BIT));
         Sprintf(otherbuf, "%S slashes %S hard ", name_of_crit(agg, ~0), 
                 name_of_crit(vict, ~0));
      }
      else {
         Sprintf(victbuf, "%S %Ss you hard ",
                 name_of_crit(agg, vict.SEE_BIT), &wmsg);
         Sprintf(otherbuf, "%S %Ss %S hard ", name_of_crit(agg, ~0), &wmsg, 
                 name_of_crit(vict, ~0));
      }
   }//if
   else if (damage < 20) {
      Sprintf(aggbuf, "You %S %S very hard ", &wmsg, 
              name_of_crit(vict, agg.SEE_BIT));
      
      if (strcmp("slash", wmsg) == 0) {
         Sprintf(victbuf, "%S slashes you very hard ", 
                 name_of_crit(agg, vict.SEE_BIT));
         Sprintf(otherbuf, "%S slashes %S very hard ", name_of_crit(agg, ~0), 
                 name_of_crit(vict, ~0));
      }
      else {
         Sprintf(victbuf, "%S %Ss you very hard ", 
                 name_of_crit(agg, vict.SEE_BIT), &wmsg);
         Sprintf(otherbuf, "%S %Ss %S very hard ", name_of_crit(agg, ~0), 
                 &wmsg, name_of_crit(vict, ~0));
      }
   }//if
   else if (damage < 25) {
      Sprintf(aggbuf, "You OBLITERATE %S ", 
              name_of_crit(vict, agg.SEE_BIT));
      Sprintf(victbuf, "%S OBLITERATES you ", 
              name_of_crit(agg, vict.SEE_BIT));
      Sprintf(otherbuf, "%S OBLITERATES %S ", name_of_crit(agg, ~0), 
              name_of_crit(vict, ~0));
   }//if
   else if (damage < 36) {
      Sprintf(aggbuf, "You LIQUIFY %S ", name_of_crit(vict, agg.SEE_BIT));
      Sprintf(victbuf, "%S LIQUIFIES you ", name_of_crit(agg, 
                                                         vict.SEE_BIT));
      Sprintf(otherbuf, "%S LIQUIFIES %S ", name_of_crit(agg, ~0), 
              name_of_crit(vict, ~0));
   }//if
   else if (damage >= 36) {
      Sprintf(aggbuf, "You ANNIHILATE %S ", name_of_crit(vict, 
                                                         agg.SEE_BIT));
      Sprintf(victbuf, "%S ANNIHILATES you ", name_of_crit(agg, 
                                                           vict.SEE_BIT));
      Sprintf(otherbuf, "%S ANNIHILATES %S ", name_of_crit(agg, ~0), 
              name_of_crit(vict, ~0));
   }//if
   
   if (agg.eq[posn_of_weapon]) {
      Sprintf(aggendbuf, "with your %S.\n", 
              name_of_obj(*(agg.EQ[posn_of_weapon]), agg.SEE_BIT));
      Sprintf(victendbuf, "with %s %S.\n", get_his_her(agg), 
              name_of_obj(*(agg.EQ[posn_of_weapon]), ~0));
   }
   else {
      if ((agg.RACE == DRAGON) || (agg.RACE == ANIMAL) || 
          (agg.RACE == MONSTER)) {
         aggbuf.dropFromEnd(1); //rid of trailing space
         aggendbuf = ".\n";
         victbuf.dropFromEnd(1); //rid of trailing space
         victendbuf = ".\n";
      }//if
      else {
         aggendbuf = "with your fist.\n";
         Sprintf(victendbuf, "with %s fist.\n", get_his_her(agg)); 
      }//else
   }//else

   gain_xp(agg, (int)(damage * 1.5), FALSE);  //fighting xp

   buf = aggbuf;
   buf+= aggendbuf;
   buf.Cap();
   show(buf, agg);
   
   buf = victbuf;
   buf+= victendbuf;
   buf.Cap();
   show(buf, vict);
   
   buf = otherbuf;
   buf+= victendbuf;
   buf.Cap();
   show_all_but_2(agg, vict, buf, room_list[agg.getCurRoomNum()]);
   
   //  log("Testing for consequences..\n");
   if ((vict.HP <= 0) && ((vict.POS == POS_SIT) || (vict.POS == POS_REST)
                          || (vict.POS == POS_PRONE) || (vict.POS == POS_SLEEP)
                          || (vict.POS == POS_STAND))) {
      vict.setPosn(POS_STUN);
      emote("is stunned.", vict, room_list[vict.getCurRoomNum()], TRUE);
      show("You are stunned!!\n", vict);
   }//if
   else if (vict.HP <= 0) {
      //emote("is dead!!", vict, room_list[vict.getCurRoomNum()], TRUE);
      agg_kills_vict(agg, vict, show_vict_tags);
   }//if
   else if (vict.HP <= vict.WIMPY) {
      int is_dead = FALSE;
      flee(vict, is_dead);
   }//if wimpy
}//do_battle_round


// Convenience method for when we don't care about dealing
// with tags.
void agg_kills_vict(critter& agg, critter& vict) {
   int show_vict_tags = FALSE;
   agg_kills_vict(agg, vict, show_vict_tags);
}


void agg_kills_vict(critter& agg, critter& vict, int& show_vict_tags) {
   Cell<critter*> cll2(vict.IS_FIGHTING);
   critter *ptr2;
   String buf(100);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In agg_kills_vict, agg:  " << *(name_of_crit(agg, ~0))
             << "  vict:  " << *(name_of_crit(vict, ~0)) << endl;
   }

   emote("is dead!", vict, room_list[vict.getCurRoomNum()], TRUE);

   while ((ptr2 = cll2.next())) { // others no longer fighting vict.
      ptr2->IS_FIGHTING.loseData(&vict); 
   }//while

   if (agg.mob && agg.mob->proc_data && 
       agg.mob->proc_data->temp_proc) {  //rem from hunting, tracking

      Cell<String*> cll(agg.HUNTING);
      String* ptr;

      while ((ptr = cll.next())) {
	 if (vict.isNamed(*ptr)) {
            delete ptr;
            ptr = agg.HUNTING.lose(cll);
	    break;
	 }//if
      }//while

      if (vict.isNamed(agg.TRACKING)) {
         agg.setTrackingTarget(NULL_STRING);
      }
   }//if
   
   agg.ALIGN -= (int)(((float)(vict.ALIGN) / 10.0) *
                      ((float)(agg.ALIGN) + 1000.0) / 1500.0);

   if (agg.ALIGN < -1000)
      agg.ALIGN = -1000;
   else if (agg.ALIGN > 1000)
      agg.ALIGN = 1000;

   vict.IS_FIGHTING.clear(); //vict no longer fighting others

   if (vict.pc && agg.pc) {
      doShowList(&vict, Selectors::instance().CC_gets_info_allow,
                 Selectors::instance().CC_none, pc_list,
                 CS_PKILLED_INFO,
                 agg.getName(), vict.getName(), vict.getCurRoomNum());

      agg.pc->pk_count++;
      vict.pc->pk_count--;
   }//if
   else if (vict.pc) {
      doShowList(&vict, Selectors::instance().CC_gets_info_allow,
                 Selectors::instance().CC_none, pc_list,
                 CS_KILLED_INFO,
                 vict.getName(), agg.getName(), vict.getCurRoomNum());

      vict.pc->died_count++;
   }

   disburse_xp(agg, vict); //take care of xp
   dead_crit_to_corpse(vict, show_vict_tags); //remove all traces of vict
   do_just_killed_procs(agg); //autoloot etc
}//agg_kills_vict;


int exact_raw_damage(int damage, int damage_type, critter& vict,
		     critter& agg) {
   int armor_value = 100;
   float dam = (float)damage;
   
   switch (damage_type) {
    case FIRE: case D_BREATH:
      armor_value = vict.HEAT_RESIS;
      break;
    case ICE:
      armor_value = vict.COLD_RESIS;
      break;
    case ELECTRICITY:
      armor_value = vict.ELEC_RESIS;
      break;
   }//switch
   
   armor_value += 100;
   if (armor_value < 0) {
      armor_value = 0;
   }//if
   
   dam = dam * ((float)armor_value/200.0);
   
   dam = (dam * (((float)vict.DAM_REC_MOD)/100.0) * 
          ((float)(agg.DAM_GIV_MOD)/100.0));
   int pl;
   if (agg.pc && 
       (pl = get_percent_lrnd(ENHANCED_DAMAGE_SKILL_NUM, agg)) > 0) {
      dam += (float)(d(1, pl/20));
   }//if
   
   if ((agg.ALIGN < -350) && vict.CRIT_FLAGS.get(24)) {
      dam -= (float)(d(1, 10));
      if (dam < 0)
        dam = 0;
   }//if
   
   if ((agg.ALIGN > 350) && vict.CRIT_FLAGS.get(25)) {
      dam -= (float)(d(1, 10));
      if (dam < 0)
        dam = 0;
   }//if
   
   if (is_affected_by(ABSORB_BLOWS_SKILL_NUM, vict)) {
      vict.MANA -= (short)dam;
      if (vict.MANA < 0) {
         dam *= 2;
         vict.MANA = 0;
      }//if
      else {
         dam = -(dam/1.5);
      }//else
   }//if

   if (is_affected_by(EARTHMELD_SKILL_NUM, vict)) {
      if ((pl = get_percent_lrnd(EARTHMELD_SKILL_NUM, agg)) > d(1,115)) {
         dam *= (100.0 - ((float)(pl))/7.0) / 100.0;
      }//if
   }//if
   
   // Gonna make low level mobs a little weaker across the board to
   // help out the newbies!
   if (agg.isNPC() && (agg.getLevel() < 10)) {
      dam *= 0.75;
   }

   if (vict.isImmort() && vict.isNoHassle()
       && vict.getImmLevel() >= agg.getImmLevel()) {
      dam = 0.0;
      if (mudlog.ofLevel(INF)) {
         mudlog << "INFO:  setting damage to zero because of !hassle.\n"
                << endl;
      }//if
   }//if

  vict.HP -= (int)(dam);
  return abs((int)dam); //negative damages mess up error messages
}//exact_raw_damage


int exact_raw_damage(int damage, int damage_type, critter& vict) {
  int armor_value = 100;
  float dam = (float)damage;

  switch (damage_type) {
  case FIRE: case D_BREATH:
    armor_value = vict.HEAT_RESIS;
    break;
  case ICE:
    armor_value = vict.COLD_RESIS;
    break;
  case ELECTRICITY:
    armor_value = vict.ELEC_RESIS;
    break;
  }//switch

  armor_value += 100;
  if (armor_value < 0) {
     armor_value = 0;
  }//if
  
  dam *= ((float)armor_value/200.0);

  dam *= (((float)vict.DAM_REC_MOD)/100.0);
   
  // Give PC's a slight edge, maybe modify after the game has mature
  // players.
  if (!vict.isPc()) {
     dam *= 0.90;
  }

  vict.HP -= (int)(dam);
  return (int)dam;
}//exact_raw_damage


void crit_dies(critter& pc) {
   Cell<critter*> cll2(pc.IS_FIGHTING);
   critter *ptr2;

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB sent to crit_dies.\n");
      do_shutdown = TRUE;
      core_dump("crit_dies");
   }//if

   //log("In crit_dies.\n");

   while ((ptr2 = cll2.next())) { // others no longer fighting vict.
      ptr2->IS_FIGHTING.loseData(&pc); 
   }//while

   pc.is_fighting.clear(); //vict no longer fighting others
   int show_vict_tags = FALSE;
   dead_crit_to_corpse(pc, show_vict_tags); //remove all traces of vict
}//crit_dies;


void disburse_xp(critter& agg, const critter& vict) {
   Cell<critter*> cll;
   critter* crit_ptr;
   int tot_levs = 0;
   long xp_to_be_gained = (vict.EXP/EXP_DIVISOR + 30 - agg.getLevel());
   long xp_per_level;

   //log("In disburse_xp.\n");
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In disburse_xp, agg: " << *(agg.getName())
             << " " << agg.getIdNum() << " vict: " << *(vict.getName())
             << " " << vict.getIdNum() << endl;
   }

   if (!agg.FOLLOWER_OF || (agg.FOLLOWER_OF == &agg)) {
      if (IsEmpty(agg.GROUPEES)) { //solitary person
         gain_xp(agg, xp_to_be_gained, TRUE);
      }//if
      else { //leader of a group then
         agg.GROUPEES.head(cll);
         while ((crit_ptr = cll.next())) {
            if (crit_ptr->getCurRoomNum() == agg.getCurRoomNum()) {
               tot_levs += crit_ptr->getLevel();
            }
         }//while
 
         xp_per_level = (xp_to_be_gained / tot_levs);
  
         agg.GROUPEES.head(cll);
         while ((crit_ptr = cll.next())) {
            if (crit_ptr->getCurRoomNum() == agg.getCurRoomNum()) {
               gain_xp(*crit_ptr, xp_per_level * (crit_ptr->LEVEL), TRUE);
            }
         }//while
      }//else
   }//if
   else { //following someone
      if (HaveData(&agg, agg.FOLLOWER_OF->GROUPEES)) { 
                  //if follower is in group
         agg.FOLLOWER_OF->GROUPEES.head(cll);
         while ((crit_ptr = cll.next())) {
            if (crit_ptr->getCurRoomNum() == agg.getCurRoomNum()) {
               tot_levs += crit_ptr->LEVEL;
            }
         }//while
 
         xp_per_level = (xp_to_be_gained / tot_levs);
  
         agg.FOLLOWER_OF->GROUPEES.head(cll);
         while ((crit_ptr = cll.next())) {
            if (crit_ptr->getCurRoomNum() == agg.getCurRoomNum()) {
               gain_xp(*crit_ptr, xp_per_level * (crit_ptr->LEVEL), TRUE);
            }
         }//while
      }//if
      else { //not in group
         gain_xp(agg, xp_to_be_gained, TRUE);
      }//else
   }//if following someone
   //log("Done w/disburse xp.\n");
}//disburse_xp


void gain_level(critter& crit) {
   if (crit.LEVEL >= 30) {
      show("You can learn no more from battle.\n", crit);
      if (crit.LEVEL > 40) {
         crit.LEVEL = 40; //for those over level 40 by some other means
      }//if
      return;
   }//if

   int hp_gain = d(2, crit.CON/2);
   int mana_gain = d(2, crit.WIS/2);

   int _class = crit.getClass();
   switch (_class)
      {
      case WARRIOR:
      case RANGER:
      case THIEF:
      case BARD:
         hp_gain += d(2, crit.CON/4);
         break;
      case SAGE:
      case WIZARD:
      case CLERIC:
      case ALCHEMIST:
         mana_gain += d(2, crit.WIS/4);
         break;
      default:
         if (mudlog.ofLevel(DBG)) {
            mudlog << "WARNING: default class in gain_level: " << _class
                   << endl;
         }
      }//switch

   crit.LEVEL++;
   crit.PRACS += d(2, (int)((float)(crit.INT)/6.0)) + 1;
   crit.MA_MAX += mana_gain;
   crit.setHP_MAX(crit.getHP_MAX() + hp_gain);
   crit.MV_MAX += d(1, crit.DEX);
   show("You rise a level.\n", crit);
}//gain_level


void dead_crit_to_corpse(critter& vict, int& show_vict_tags) {
   object *corpse, *gold;
   Cell<object*> ocell;
   object* o_ptr;
   String buf(81);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In dead_crit_to_corpse, vict:  " << *(name_of_crit(vict, ~0))
             << "  vict addr:  " << &vict << endl;
   }

   if (vict.isMob()) {
      mudlog.log(ERR, "ERROR:  mob sent to dead_crit_to_corpse.\n");
      return;
   }//if

   /* first, cut all links to critter */

   if (vict.temp_crit) {
      //Takes care of shielding and so on.
      vict.temp_crit->Clear();
   }//if

   int nrm = vict.getCurRoomNum();

   room_list[nrm].removeCritter(&vict); //decrement's cur in game if NPC

   leave_room_effects(room_list[nrm], vict); //takes care of blocking
   affected_mobs.loseData(&vict);
   proc_action_mobs.loseData(&vict);

   vict.doUngroup(1, &NULL_STRING);   //no more part of group
   vict.doBecomeNonPet();             //dead pets make no sense!

   // Get rid of any pets victim may own.
   critter* pptr;
   while (!vict.PETS.isEmpty()) {
      pptr = vict.PETS.popFront();
      pptr->doBecomeNonPet();
   }

   int ngame;
   if (vict.mob) {
      recursive_init_unload(vict);
      ngame = vict.getCurInGame();
      if (ngame < 0) {
         mudlog << "ERROR:  ngame is < zero: "
                << ngame << ", dead_crit_to_corpse, for mob: "
                << *(vict.getName()) << " number: " << vict.getIdNum()
                << endl;
         vict.setCurInGame(0); //try to correct things
      }//if
   }//if

   // If we are a Player-run Shop keeper, then have some special fun.
   if (vict.isPlayerShopKeeper()) {

      // we gotta first update all the player-manipulated
      // data, no use making them enter that again and again.
      vict.transferShopDataTo(mob_list[vict.getIdNum()]);

      // Now save it to disk.
      save_player_shop_owner(mob_list[vict.getIdNum()]);
   }//if

   /*  all links have been cut, I hope */
   /*  Now we will transfer all eq and gold and stuff to a corpse */

   // create corpse object.
   corpse = obj_to_sobj(obj_list[CORPSE_OBJECT], 
                        room_list[vict.getCurRoomNum()].getInv(),
                        vict.getCurRoomNum());

   recursive_init_loads(*corpse, 0);

   // Set edible flag on corpse if was on mob.
   if (vict.isEdible()) {
      corpse->setButcherable(TRUE);
   }

   corpse->bag->time_till_disolve = 10; // in ticks
   Sprintf(buf, "The corpse of %S lies here.",
                name_of_crit(vict, ~0));
   corpse->in_room_desc = buf;
   room_list[vict.getCurRoomNum()].gainInv(corpse);

   /* Set the weight to that of the living being. */
   corpse->setEmptyWeight(vict.getNakedWeight());

         /*  gold  */

   if (vict.GOLD > 0) {
      gold = obj_to_sobj(obj_list[GOLD_OBJECT], &(corpse->inv), vict.getCurRoomNum());
      
      obj_list[GOLD_OBJECT].incrementCurInGame();
      
      gold->cur_stats[1] = vict.GOLD; //transfer gold
      vict.GOLD = 0;
      
      Put(gold, corpse->inv);
   }//if

         /* eq  */
   for (int i = 0; i<MAX_EQ; i++) {
      if (vict.EQ[i]) {
         remove_eq_effects(*(vict.EQ[i]), vict, TRUE, FALSE, i);
         if (vict.EQ[i]->IN_LIST)
            vict.EQ[i]->IN_LIST = &(corpse->inv);
         Put(vict.EQ[i], corpse->inv);
         vict.EQ[i] = NULL;
      }//if
   }//for

      /*  inv  */
   vict.inv.head(ocell);
   while ((o_ptr = ocell.next())) {
      drop_eq_effects(*o_ptr, vict, FALSE);
      if (o_ptr->IN_LIST) 
         o_ptr->IN_LIST = &(corpse->inv);
      corpse->gainInv(o_ptr);
   }//while
   vict.inv.clear();

                  /*  perm inv for shopkeepers */
   if (vict.mob) {
      if (vict.mob->proc_data) {
         if (vict.mob->proc_data->sh_data) {
            vict.PERM_INV.head(ocell);
            while ((o_ptr = ocell.next())) {
               if (o_ptr->IN_LIST) 
                  o_ptr->IN_LIST = &(corpse->inv);
               Put(o_ptr, corpse->inv);
            }//while
            vict.PERM_INV.clear(); //should NEVER be SOBJ's
         }//if
      }//if
   }//if

   if (vict.mob && vict.MOB_FLAGS.get(16)) { //if has skin
      if (!corpse->obj_proc) {
         corpse->obj_proc = new obj_spec_data;
      }//if
      corpse->obj_proc->skin_ptr = &(obj_list[vict.mob->skin_num]);
      recursive_init_loads(*(corpse->obj_proc->skin_ptr), 0);
      corpse->obj_proc->obj_spec_data_flags.turn_on(2);
   }//if
   else {
      if (vict.pc) {
         if (!corpse->obj_proc) {
            corpse->obj_proc = new obj_spec_data;
         }//if
         object* ptr = corpse->obj_proc->skin_ptr = 
            obj_to_sobj(obj_list[PC_SKIN_OBJECT], &(corpse->inv),
                        vict.getCurRoomNum());

         recursive_init_loads(*ptr, 0);

         ptr->names.append(new String(*(Top(vict.names))));
         Sprintf(buf, "the tattered skin of %S", Top(vict.names));
         ptr->short_desc = buf;
         Sprintf(buf, "The tattered skin of %S lies here.", 
                 Top(vict.names));
         ptr->in_room_desc = buf;
         Sprintf(buf,
"This large piece of %s skin was recently hacked from the corpse of %S.
You wonder why anyone would want the skin of a %s, but perhaps it is just
a trophy--a symbol of %S's defeat.\n",
                 get_race_name(vict.RACE),
                 name_of_crit(vict, ~0), get_race_name(vict.RACE),
                 name_of_crit(vict, ~0));
         ptr->long_desc = buf;
         corpse->obj_proc->obj_spec_data_flags.turn_on(2);
      }//if
   }//else

            /* punish and re-enter the pc into the game, if its a pc  */
   if (vict.pc) {
      if (vict.CON > 0) {
         vict.MANA = 1;
         vict.HP   = 1;
         vict.MOV  = 1;
         vict.HUNGER = 20;
         vict.THIRST = 20;
         vict.setPosn(POS_REST);
         if (vict.EXP < 5000000)
            vict.EXP -= (vict.EXP/13);
         else
            vict.EXP -= 500000;

         if (show_vict_tags) {
            show_vict_tags = FALSE; //Tell calling code not to show more tags.
            if (vict.isUsingClient()) {
               show("</BATTLE>", vict);
            }
            else if (vict.isUsingColor()) {
               show(*(vict.getDefaultColor()), vict);
            }
         }//if we should show tags here...
         
         Cell<stat_spell_cell*> spcll(vict.affected_by);
         stat_spell_cell* sp_ptr;
         while ((sp_ptr = spcll.next())) {
            rem_effects_crit(sp_ptr->stat_spell, vict, FALSE);
         }//while
         clear_ptr_list(vict.affected_by);
         
         vict.mini_affected_by.head(spcll);
         while ((sp_ptr = spcll.next())) {
            rem_effects_crit(sp_ptr->stat_spell, vict, FALSE);
         }//while
         clear_ptr_list(vict.mini_affected_by);
         
         
         room_list[LOGIN_ROOM].gainCritter(&vict);
         Sprintf(buf, "%S appears in the room looking quite dead!\n", 
                 name_of_crit(vict, ~0));
         buf.Cap();
         room_list[LOGIN_ROOM].showAllCept(buf, &vict);
         show("\n\n\n\nYOU ARE DEAD!!!!!!!!!!!\n", vict);
         show("\nWelcome once again.\n", vict);
         show("Perhaps this time you'll fare a little better!!\n", vict);
         look(1, &NULL_STRING, vict); //sos ya can see the temple desc, etc.
         vict.save();
      }//if not con-death
      else { //PERM DEATH!!
         show(
"You have reached the end of your luck!.  Perhaps your soul will endure longer
next time.  Please feel free to rejoin the Realm in another mortal coil!\n",
              vict);

         Sprintf(buf,
"gasps a final farewell to the Realm in which %s has dwelt for %i years!",
                 get_he_she(vict), vict.getAge());
         emote(buf, vict, room_list[nrm], TRUE);

         buf = *(Top(vict.names));
         buf.Tolower();
         buf.Prepend("rm ./Pfiles/");
         system(buf);
       
         doShowList(&vict, Selectors::instance().CC_gets_info_allow,
                    Selectors::instance().CC_none, pc_list,
                    CS_PERM_DEATH_INFO,
                    vict.getName());

         vict.doSuicide(); //poor bastard!
      }//if perm con death
   }//if
   else { //then it was a smob
      if (vict.possessed_by) {
         vict.possessed_by->show("Your possessed victim has died.\n");
         vict.possessed_by->unPossess();
      }
      delete &vict;
   }//else
}//dead_crit_to_corpse


critter* mob_to_smob(critter& mob, const int room_num, int do_sub,
                     int i_th, const String* name, int see_bit) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In mob_to_smob:  " << *(mob.getName()) << endl;
   }

   if (!mob.isMob()) {
      mudlog.log(ERR, "ERROR:  mob_to_smob called on nonMOB.\n");
      do_shutdown = TRUE;
      exit(100);
   }//if

   if (mob.pc) {
      mudlog.log(ERR, "ERROR:  mob has pc_data in mob_to_smob.\n");
      do_shutdown = TRUE;
      exit(113);
   }//if

   mob.MOB_FLAGS.turn_on(4); //now it can be/should be reset when pulsed

   critter* crit_ptr = new critter(mob);

   obj_ptr_log << "CRI_CR " << mob.getIdNum() << " " << crit_ptr << "\n";

   if (!name || ((room_num < 0) || (room_num > NUMBER_OF_ROOMS))) {
      mudlog.log(ERR,
                 "ERROR:  name NULL  or room_num out of range in mob_to_smob.\n");
      do_shutdown = TRUE;
      exit(100);
   }//if

   crit_ptr->setCurRoomNum(room_num);

   if (do_sub) {
      if (!room_list[room_num].sub_a_4_b(crit_ptr, i_th, *name, see_bit)) {
         mudlog << "ERROR: crit_sub_a_4_b failed in mob_to_smob, rm# "
                << room_num << " i_th: " << i_th << " name: " << *name << endl;
      }//if
   }//if
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "WARNING:  mob_to_smob, not doing do_sub, mob#: "
                << crit_ptr->getIdNum() << endl;
      }//if
   }//else

   affected_mobs.gainData(crit_ptr);  //basically a list of smobs

   if (mudlog.ofLevel(DBG)) {
      mudlog << "mob_to_smob:  returning critter, address:  " << crit_ptr
             << endl;
   }

   return crit_ptr;
}//mob_to_smob


critter* mob_to_smob(critter& mob, const int room_num,
                     int suppress_msg) {
   if ((room_num > NUMBER_OF_ROOMS) || (room_num < 0)) {
      mudlog.log(ERR, "ERROR:  room_num out of range in mob_to_smob.\n");
      do_shutdown = TRUE;
      exit(100);
   }//if

   return mob_to_smob(mob, room_list[room_num], suppress_msg);
}

// Must use this one if we are reading in room, because we
// are using a temporary room to put them in.  Should migrate
// to this one, as opposed to the one above anyway..safer.
critter* mob_to_smob(critter& mob, room& rm, int suppress_sub_fail_msg) {

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In mob_to_smob:  " << *(mob.getName()) << endl;
   }

   /*  This function DOES DO SUBSTITUTION itself, calling code need not worry
       about it. */

   if (!mob.isMob()) {
      mudlog.log(ERR, "ERROR:  mob_to_smob called on nonMOB.\n");
      core_dump("mob_to_smob1");
   }//if

   if (mob.pc) {
      mudlog.log(ERR, "ERROR:  mob has pc_data in mob_to_smob.\n");
      core_dump("mob_to_smob2");
   }//if

   mob.MOB_FLAGS.turn_on(4); //now it can be/should be reset when pulsed

   critter* crit_ptr = new critter(mob);

   obj_ptr_log << "CRI_CR " << mob.getIdNum() << " " << crit_ptr << "\n";

   crit_ptr->setCurRoomNum(rm.getIdNum());

   if (!rm.sub_a_4_b(crit_ptr, (critter*)(&mob), 1)) {
      /* note:  create golem and others will fail on this now with no bad
         effects, so this error message has been commented out. */
      if (!suppress_sub_fail_msg) {
         mudlog << "ERROR:  SubstituteData failed in mob_to_smob, mob#: "
                << crit_ptr->getIdNum() << "  name: " << *(crit_ptr->getName())
                << " room# " << rm.getIdNum() << endl;
      }//if

      // If couldn't substitute, then we need to increment it's
      // CUR_IN_GAME
      crit_ptr->incrementCurInGame();
   }//if
   
   affected_mobs.gainData(crit_ptr);  //basically a list of smobs

   if (mudlog.ofLevel(DBG)) {
      mudlog << "mob_to_smob:  returning critter, address:  " << crit_ptr
             << endl;
   }

   return crit_ptr;
}//mob_to_smob



object* obj_to_sobj(object& obj, List<object*>* in_list,
                    int do_sub, int i_th, const String* name, int see_bit, 
                    room& rm) {
   //log("In obj_to_sobj.\n");

   if (!name || !in_list) {
      core_dump("ERROR:  name or in_list NULL in obj_to_sobj.\n");
   }//if

   if (obj.IN_LIST) {
      core_dump("ERROR:  obj_to_sobj called on SOBJ, version 1.\n");
   }//if

   if (!in_list) {
      core_dump("ERROR:  in_list is NULL in obj_to_sobj, version 1!!\n");
   }//if

   obj.OBJ_FLAGS.turn_on(70); //now it can be/should be reset when pulsed

   object* obj_ptr = new object(obj);

   obj_ptr_log << "OBJ_CR " << obj.getIdNum() << " " << obj_ptr << "\n";

   obj_ptr->IN_LIST = in_list;
   obj_ptr->setCurRoomNum(rm.getIdNum(), 0);

   if (do_sub) {
      if (!obj_sub_a_4_b(obj_ptr, *in_list, i_th, name, see_bit, rm)) {
         mudlog.log(ERR, "ERROR:  obj_sub_a_4_b failed in obj_to_sobj.\n");
      }//if
   }//if
   else {
      //mudlog << "WARNING:  obj_to_sobj, no sub:  increment CUR_IN_GAME?"
      //       << endl;
      //obj_list->incrementCurInGame();
   }

   affected_objects.gainData(obj_ptr);  //basically a list of sobjs
   return obj_ptr;
}//obj_to_sobj


object*  obj_to_sobj(object& obj, List<object*>* in_list, int rm_num) {
   //log("In obj_to_sobj, part two.\n");

/* WARNING:  this function does NO SUBSTITUTION, you must do it 
             explicitly in the calling code. */

   if (obj.IN_LIST) {
      core_dump("ERROR:  obj_to_sobj called on SOBJ, version 2.\n");
   }//if

   if (!in_list) {
      core_dump("ERROR:  in_list is NULL in obj_to_sobj, version 2!!\n");
   }//if

   obj.OBJ_FLAGS.turn_on(70); //now it can be/should be reset when pulsed

   object* obj_ptr = new object(obj);
   
   obj_ptr_log << "OBJ_CR " << obj.getIdNum() << " " << obj_ptr << "\n";
   
   obj_ptr->IN_LIST = in_list;
   obj_ptr->setCurRoomNum(rm_num, 0);

   affected_objects.gainData(obj_ptr);  //basically a list of sobjs
   return obj_ptr;
}//obj_to_sobj
