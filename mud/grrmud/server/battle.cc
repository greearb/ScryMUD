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

///**********************  Battle and related functions ******************///

#include "battle.h"
#include "misc.h"
#include "misc2.h"
#include "spec_prc.h"
#include <math.h>
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include <stdio.h>
#include "classes.h"
#include <string2.h>
#include <bitfield.h>
#include "spells.h"
#include "spells2.h"
#include "skills.h"
#include <PtrArray.h>
#include "load_wld.h"
#include "const.h"
#include "Filters.h"
#include "batl_prc.h"
#include "clients.h"

short can_start_battle(critter& targ, critter& pc, short do_msg) {
   String buf(100);

   if ((targ.isMob()) || (pc.isMob())) {
      mudlog.log(LS_ERROR, "ERROR:  can_start battle sent SMOB's.\n");
      return FALSE;
   }//if

   if (room_list[targ.getCurRoomNum()].isHaven()) {
      if (do_msg) {
         Sprintf(buf, "%pS is in a peaceful room.\n", 
                 name_of_crit(targ, pc.SEE_BIT));
         buf.Cap();
         pc.show(buf);
      }//if
      return FALSE;
   }//if
   else if (room_list[targ.getCurRoomNum()].isNoPK() && 
           (pc.pc && targ.pc)) {
      if (do_msg) {
         Sprintf(buf, "%pS is in a safe room.\n", 
                 name_of_crit(targ, pc.SEE_BIT));
         buf.Cap();
         pc.show(buf);
      }//if
      return FALSE;
   }//if
   else if (ROOM.isHaven()) {
      if (do_msg) {
         pc.show("But you are in such peaceful room!\n");
      }//if
      return FALSE;
   }//if
   else if (ROOM.isNoPK() && (pc.pc && targ.pc)) {
      if (do_msg) {
         pc.show("You are in a safe room.\n");
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
   critter* crit_ptr = NULL;
   critter* vict_ptr = NULL;
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
               crit_ptr->show(CTAG_BATTLE(crit_ptr->whichClient() ));
            }

            /* KHAAVREN DELETE MARKER
               else if (crit_ptr->isUsingColor()) {
               crit_ptr->show(*(crit_ptr->getBattleColor()));
               }
               */

            int atks = crit_ptr->ATTACKS;
            if (crit_ptr->ATTACKS > 4)
               atks = 4;
            if (crit_ptr->ATTACKS < 1)
               atks = 1;

            /* check for second attack */
            if ((crit_ptr->isPc())
                  && (d(1,100) < d(1, (get_percent_lrnd(SECOND_ATTACK_SKILL_NUM, *(crit_ptr)) + 
                           crit_ptr->LEVEL) * (int)((float)(crit_ptr->getDEX(true)) / 9.0))-(crit_ptr->PAUSE*60)) ) {
               atks++;
            }

            for (i = 0; i< atks; i++) {
               if (!crit_ptr->IS_FIGHTING.isEmpty()) { 
                  //if first hit kills, abort
                  vict_ptr = crit_ptr->IS_FIGHTING.peekFront();

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
                        vict_ptr->show(CTAG_BATTLE(vict_ptr->whichClient()));
                     }
                     /* KHAAVREN DELETE MARKER
                        else if (vict_ptr->isUsingColor()) {
                        show(*(vict_ptr->getBattleColor()), *vict_ptr);
                        }
                        */

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
                        vict_ptr->show(CTAG_END_BATTLE(vict_ptr->whichClient()));
                     }
                     /* KHAAVREN DELETE MARKER
                        else if (vict_ptr->isUsingColor()) {
                        vict_ptr->show(*(vict_ptr->getDefaultColor()));
                        }
                        */
                  }//if
               }//if
            }//for                /* now check for dual wield */

            if (crit_ptr->isFighting() && crit_ptr->isDualWielding()) {

               vict_ptr = crit_ptr->IS_FIGHTING.peekFront();

               if (vict_ptr->isUsingClient()) {
                  vict_ptr->show(CTAG_BATTLE(vict_ptr->whichClient()));
               }

               int show_vict_tags = TRUE; //Show tags if needed.
               do_battle_round(*crit_ptr, *vict_ptr, 10, show_vict_tags);

               // Tags will only be shown in do_battle_round if
               // the victim died.
               if (show_vict_tags) {
                  if (vict_ptr->isUsingClient()) {
                     vict_ptr->show(CTAG_END_BATTLE(vict_ptr->whichClient()));
                  }
               }//if
            }//if

            if (crit_ptr->isUsingClient()) {
               crit_ptr->show(CTAG_END_BATTLE(crit_ptr->whichClient()));
            }
            /* KHAAVREN DELETE MARKER
               else if (crit_ptr->isUsingColor()) {
               crit_ptr->show(*(crit_ptr->getDefaultColor()));
               }
               */
         }//if
      }//while

      // Take anyone affected by Flesh-to-Stone out of battle.
      List<critter*> tmp_list2(r_ptr->getCrits());
      tmp_list2.head(crit_cell);
      critter* walker;
      while ((walker = crit_cell.next())) {
         if (is_affected_by(FLESH_TO_STONE_SKILL_NUM, *walker)) {
            /* quit battle */
            Cell<critter*> cll(walker->IS_FIGHTING);
            critter* tmp_ptr;
            while ((tmp_ptr = cll.next())) {
               tmp_ptr->IS_FIGHTING.loseData(walker);
            }//while
            walker->IS_FIGHTING.clear();
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
      Sprintf(buf, "You gain %li experience points.\n", exp);
      crit.show(buf);
   }//if
   if (crit.EXP >= levels[crit.LEVEL]) {
      if (crit.LEVEL >= 30) {
         crit.EXP = levels[crit.LEVEL] - 1;
         //can't level past 30
      }
      else {
         gain_level(crit);
      }
   }//if
}//gain_xp


void do_battle_round(critter& agg, critter& vict, int posn_of_weapon,
                     int& show_vict_tags) {
   float damage, weapon_dam, pos_mult, xp_damage;
   short  is_wielding = FALSE;
   String buf(81);
   String aggbuf(25);
   String victbuf(25);
   String wmsg(25);
   String aggendbuf(50);
   String victendbuf(50);
   String otherbuf(50);
   char dam_str[50];

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In do_battle_round, agg addr:  " << &agg << "  name:  "
             << *(name_of_crit(agg, ~0)) << "  vict addr:  " << &vict
             << "  vict name:  " << *(name_of_crit(vict, ~0))
             << "  posn of weapon:  " << posn_of_weapon << endl;
   }

   // Set violence timer for pc vs. pc combat
   if (config.useViolenceTimer && agg.isPc() && vict.isPc()) {
      agg.setViolenceTimer();
      vict.setViolenceTimer();
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
       || (agg.POS == POS_MED) || (agg.POS == POS_PRONE)) {
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

   if (agg.isBlocked()) { //if blocked
      agg.CRIT_FLAGS.turn_off(21);
      return;
   }//if

   if (agg.EQ[posn_of_weapon]) { //wielded
      is_wielding = TRUE;
   }//if

   if (posn_of_weapon == 10) { //if doing dual_wield
      if (!agg.eq[posn_of_weapon])
         return;
      else if (!(agg.eq[posn_of_weapon]->isWeapon()))
         return;
   }//if

   int j = (int)(vict.combatBonusVal(false) - ((vict.AC - 100)/10.0));
   int i = (int)(agg.combatBonusVal(true) + agg.HIT);

   if ( (( ! vict.isStunned() ) && ( d(1,50+j) > d(1,50+i) ))
         || ( agg.POS == POS_STUN ) ) {

      if ( agg.POS == POS_STUN ) {
         agg.show("You lie immobilized by the thought of imminent death.\n",
               HL_BATTLE);
      }
      else {
         int lower_bound = 1;
         int miss_score = 50;
         int dodge_score = 0 
            + get_percent_lrnd(DODGE_SKILL_NUM, vict)
            + get_percent_lrnd(QUICKFOOT_SKILL_NUM, vict)
            + get_percent_lrnd(MARTIAL_ARTS_SKILL_NUM, vict);
         int block_score = 0;
         int parry_score = 0;

         if ( vict.isUsingShield() ) {
            block_score += get_percent_lrnd(BLOCK_SKILL_NUM, vict);
         } 

         if ( vict.eq[9] ||
               (vict.eq[10] && vict.isDualWielding() && (!vict.isUsingShield() )) ) {
            parry_score += get_percent_lrnd(PARRY_SKILL_NUM, vict);
         }

         int total_score = miss_score + dodge_score + block_score + parry_score;
         int random_num = d(1,total_score);

         if ( (random_num >= lower_bound) && (random_num < (lower_bound + miss_score)) ) {
            Sprintf(buf, "%pS misses you.\n", name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            vict.show(buf, HL_BATTLE);

            Sprintf(buf, "You miss %pS.\n", name_of_crit(vict, agg.SEE_BIT));
            agg.show(buf, HL_BATTLE);

            Sprintf(buf, "misses %pS.", name_of_crit(vict, ~0));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);

            return;
         }

         lower_bound += miss_score;

         if ( (random_num >= lower_bound) && (random_num < (lower_bound + dodge_score)) ) {
            Sprintf(buf, "dodges %pS's blow.", name_of_crit(agg, ~0));
            emote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &agg);

            Sprintf(buf, "%pS dodges your blow.\n", name_of_crit(vict, agg.SEE_BIT));
            buf.Cap();
            agg.show(buf, HL_BATTLE);

            Sprintf(buf, "You dodge %pS's blow.\n", name_of_crit(agg, vict.SEE_BIT));
            vict.show(buf, HL_BATTLE);

            return;
         }

         lower_bound += dodge_score;

         if ( (random_num >= lower_bound) && (random_num < (lower_bound + parry_score)) ) {

            Sprintf(buf, "parries %pS's blow.", name_of_crit(agg, ~0));
            emote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &agg);

            Sprintf(buf, "%pS parries your blow.\n", 
                  name_of_crit(vict, agg.SEE_BIT));
            buf.Cap();
            agg.show(buf, HL_BATTLE);

            Sprintf(buf, "You parry %pS's blow.\n", 
                  name_of_crit(agg, vict.SEE_BIT));
            vict.show(buf, HL_BATTLE);

            return;
         }

         lower_bound += parry_score;

         if ( (random_num >= lower_bound) && (random_num < (lower_bound + block_score)) ) {

            Sprintf(buf, "blocks %pS's blow.", name_of_crit(agg, ~0));
            emote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &agg);

            Sprintf(buf, "%pS blocks your blow.\n", 
                  name_of_crit(vict, agg.SEE_BIT));
            buf.Cap();
            agg.show(buf, HL_BATTLE);

            Sprintf(buf, "You block %pS's blow.\n", 
                  name_of_crit(agg, vict.SEE_BIT));
            vict.show(buf, HL_BATTLE);

            return;
         }
      }//agg isn't stunned.
      return;
   }//missed,blocked,parried,dodged,whatever...

   /* else, did hit */

//   log("Did hit..going into messages.\n");

   //handles bhd too
   weapon_dam = agg.getWeapDAM(posn_of_weapon, true);

   //  log("About to enter switch.\n");
   switch (vict.POS) {
     case POS_STAND:
       pos_mult = 1.0;
       break;
     case POS_PRONE: case POS_REST: case POS_SIT:
       pos_mult = 1.25;
       break;
     case POS_SLEEP:
       pos_mult = 1.5;
       break;
     case POS_MED: 
       pos_mult = 1.75;
       break;
     case POS_STUN:
       pos_mult = 2.0;
       break;

     default:
        if (mudlog.ofLevel(LS_ERROR)) {
           mudlog << "ERROR:  Default called on switch in battle.cc\n"
                  << "ERROR:  Here is bad vict.pos:  " << vict.POS << endl;
        }
        pos_mult = 1.0;
        break;
   }//switch

        /* no modifiers have been added yet, exact_raw... does that */
   damage = weapon_dam * pos_mult;

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
         if (mudlog.ofLevel(LS_ERROR)) {
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
      if (d(1, 600) < 
          (agg.getLevel() / 5) *
          (get_percent_lrnd(CRITICAL_STRIKE_SKILL_NUM, agg))) {
         agg.show("You score a critical strike!!\n", HL_BATTLE);
         damage *= 2.0;
      }//if
   }//if

   xp_damage = vict.HP + 1;

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

   if ( xp_damage < damage ) { // if damage is more than enough to kill victim
      sprintf (dam_str, " [dam > victim hp: %d]", (int)damage); //let imms know hp was exceeded by damage
      xp_damage = damage;          // set xp gain damage to actual hp lost by victim
   }
   else {
      // Set up a string to let imms see exact damage delt/recieved
      sprintf(dam_str, " [%d]", (int)damage);
   }

   if (damage < (xp_damage * 0.02) ) {
      Sprintf(aggbuf, "You bruise%s %pS ",
              agg.isImmort()?dam_str:"", name_of_crit(vict, agg.SEE_BIT));
      Sprintf(victbuf, "%pS bruises%s you ", name_of_crit(agg, vict.SEE_BIT),
              vict.isImmort()?dam_str:"");
      Sprintf(otherbuf, "%pS bruises %pS ", name_of_crit(agg, ~0), 
              name_of_crit(vict, ~0));
   }//if
   else if (damage < (xp_damage * 0.05) ) {
      Sprintf(aggbuf, "You barely %pS%s %pS ", &wmsg, agg.isImmort()?dam_str:"",
              name_of_crit(vict, agg.SEE_BIT));
      Sprintf(victbuf, "%pS barely %pSs%s you ",
              name_of_crit(agg, vict.SEE_BIT), &wmsg, vict.isImmort()?dam_str:"");
      Sprintf(otherbuf, "%pS barely %pSs %pS ", name_of_crit(agg, ~0), 
              &wmsg, name_of_crit(vict, ~0));
   }//if
   else if (damage < (xp_damage * 0.10) ) {
      Sprintf(aggbuf, "You %pS%s %pS ", &wmsg, agg.isImmort()?dam_str:"",
              name_of_crit(vict, agg.SEE_BIT));
      if (strcmp("slash", wmsg) == 0) {
         Sprintf(victbuf, "%pS slashes%s you ", name_of_crit(agg, vict.SEE_BIT),
                 vict.isImmort()?dam_str:"");
         Sprintf(otherbuf, "%pS slashes %pS ", name_of_crit(agg, ~0),
                 name_of_crit(vict, ~0));
      }
      else {
         Sprintf(victbuf, "%pS %pSs%s you ", name_of_crit(agg, vict.SEE_BIT), 
                 &wmsg, vict.isImmort()?dam_str:"");
         Sprintf(otherbuf, "%pS %pSs %pS ", name_of_crit(agg, ~0), &wmsg,
                 name_of_crit(vict, ~0));
      }
   }//if
   else if (damage < (xp_damage * 0.15) ) {
      Sprintf(aggbuf, "You %pS%s %pS hard ", &wmsg, agg.isImmort()?dam_str:"",
             name_of_crit(vict, agg.SEE_BIT));
      if (strcmp("slash", wmsg) == 0) {
         Sprintf(victbuf, "%pS slashes you hard%s ", name_of_crit(agg, vict.SEE_BIT),
                 vict.isImmort()?dam_str:"");
         Sprintf(otherbuf, "%pS slashes %pS hard ", name_of_crit(agg, ~0), 
                 name_of_crit(vict, ~0));
      }
      else {
         Sprintf(victbuf, "%pS %pSs you hard%s ", name_of_crit(agg, vict.SEE_BIT), &wmsg,
                 vict.isImmort()?dam_str:"");
         Sprintf(otherbuf, "%pS %pSs %pS hard ", name_of_crit(agg, ~0), &wmsg, 
                 name_of_crit(vict, ~0));
      }
   }//if
   else if (damage < (xp_damage * 0.20) ) {
      Sprintf(aggbuf, "You %pS%s %pS very hard ", &wmsg, agg.isImmort()?dam_str:"",
              name_of_crit(vict, agg.SEE_BIT));
      
      if (strcmp("slash", wmsg) == 0) {
         Sprintf(victbuf, "%pS slashes you very hard%s ", name_of_crit(agg, vict.SEE_BIT),
                 vict.isImmort()?dam_str:"");
         Sprintf(otherbuf, "%pS slashes %pS very hard ", name_of_crit(agg, ~0), 
                 name_of_crit(vict, ~0));
      }
      else {
         Sprintf(victbuf, "%pS %pSs%s you very hard ", name_of_crit(agg, vict.SEE_BIT),
                 &wmsg, vict.isImmort()?dam_str:"");
         Sprintf(otherbuf, "%pS %pSs %pS very hard ", name_of_crit(agg, ~0), 
                 &wmsg, name_of_crit(vict, ~0));
      }
   }//if
   else if (damage < (xp_damage * 0.50) ) {
      Sprintf(aggbuf, "You OBLITERATE%s %pS ", agg.isImmort()?dam_str:"",
              name_of_crit(vict, agg.SEE_BIT));
      Sprintf(victbuf, "%pS OBLITERATES%s you ", name_of_crit(agg, vict.SEE_BIT),
              vict.isImmort()?dam_str:"");
      Sprintf(otherbuf, "%pS OBLITERATES %pS ", name_of_crit(agg, ~0), 
              name_of_crit(vict, ~0));
   }//if
   else if (damage < (xp_damage * 0.80) ) {
      Sprintf(aggbuf, "You LIQUIFY%s %pS ", agg.isImmort()?dam_str:"",
             name_of_crit(vict, agg.SEE_BIT));
      Sprintf(victbuf, "%pS LIQUIFIES%s you ", name_of_crit(agg, vict.SEE_BIT),
             vict.isImmort()?dam_str:"");
      Sprintf(otherbuf, "%pS LIQUIFIES %pS ", name_of_crit(agg, ~0), 
              name_of_crit(vict, ~0));
   }//if
   else { // >= 80% of the targets hitpoints
      Sprintf(aggbuf, "You ANNIHILATE%s %pS ", agg.isImmort()?dam_str:"",
             name_of_crit(vict, agg.SEE_BIT));
      Sprintf(victbuf, "%pS ANNIHILATES%s you ", name_of_crit(agg, vict.SEE_BIT),
             vict.isImmort()?dam_str:"");
      Sprintf(otherbuf, "%pS ANNIHILATES %pS ", name_of_crit(agg, ~0), 
              name_of_crit(vict, ~0));
   }//if

   if (agg.eq[posn_of_weapon]) {
      Sprintf(aggendbuf, "with your %pS.\n", 
              name_of_obj(*(agg.EQ[posn_of_weapon]), agg.SEE_BIT));
      Sprintf(victendbuf, "with %s %pS.\n", get_his_her(agg), 
              name_of_obj(*(agg.EQ[posn_of_weapon]), ~0));
   }
   else {
      if ((agg.RACE == DRAGON) || (agg.RACE == ANIMAL) || 
          (agg.RACE == MONSTER) || (agg.RACE == DRAGON)) {
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

   // changed to reflect only actual hp lost, not damage inflicted
   if (!agg.getCurRoom()->isColiseum()) { // no xp gain in a coliseum
      gain_xp(agg, (int)(damage * 1.5), FALSE);  //fighting xp 
   }

   buf = aggbuf;
   buf+= aggendbuf;
   buf.Cap();
   agg.show(buf, HL_BATTLE);
   
   buf = victbuf;
   buf+= victendbuf;
   buf.Cap();
   vict.show(buf, HL_BATTLE);
   
   buf = otherbuf;
   buf+= victendbuf;
   buf.Cap();
   show_all_but_2(agg, vict, buf, room_list[agg.getCurRoomNum()], HL_BATTLE);

   // In theory, any damage higher than 40 will have a chance of breaking
   // the spell, with damage much higher having a better chance
   if ((d(5, (int)damage) > 200) && vict.isParalyzed()) {
      Sprintf(buf, "The force of %pS's blow breaks the stone you are encased in!\n",
         name_of_crit(agg, vict.SEE_BIT));
      vict.show(buf);
      Sprintf(buf, "The force of your blow breaks the stone %pS is encased in!\n",
         name_of_crit(vict, agg.SEE_BIT));
      agg.show(buf);
      Sprintf(buf, "blow breaks the stone %pS is encased in!\n",
         name_of_crit(vict, agg.SEE_BIT));
      agg.pemote(buf);
      rem_effects_crit(FLESH_TO_STONE_SKILL_NUM, vict, FALSE);

      stat_spell_cell* ssc = vict.isAffectedBy(FLESH_TO_STONE_SKILL_NUM);
      if (ssc) {
         vict.affected_by.loseData(ssc);
         delete ssc;
      }
   }
   
   //  log("Testing for consequences..\n");
   if ((vict.HP <= 0) && ((vict.POS == POS_SIT) || (vict.POS == POS_REST)
                          || (vict.POS == POS_PRONE) || (vict.POS == POS_SLEEP)
                          || (vict.POS == POS_STAND))) {
      vict.setPosn(POS_STUN);
      emote("is stunned.", vict, room_list[vict.getCurRoomNum()], TRUE);
      vict.show("You are stunned!!\n", HL_BATTLE);
   }//if
   else if (vict.HP <= 0) {
      //emote("is dead!!", vict, room_list[vict.getCurRoomNum()], TRUE);
      agg_kills_vict(&agg, vict, show_vict_tags, TRUE);
   }//if
   else if ( vict.HP <= vict.getWimpy() ) {
      int is_dead = FALSE;
      flee(vict, is_dead);
   }//if wimpy
}//do_battle_round


// Convenience method for when we don't care about dealing
// with tags.
void agg_kills_vict(critter* agg, critter& vict, int do_msg) {
   int show_vict_tags = FALSE;
   agg_kills_vict(agg, vict, show_vict_tags, do_msg);
}


void agg_kills_vict(critter* agg, critter& vict, int& show_vict_tags,
                    int do_msg) {
   Cell<critter*> cll2(vict.IS_FIGHTING);
   critter *ptr2;
   String buf(100);

   while ((ptr2 = cll2.next())) { // others no longer fighting vict.
      ptr2->IS_FIGHTING.loseData(&vict); 
   }//while

   if (agg && agg->mob && agg->mob->proc_data && 
       agg->mob->proc_data->temp_proc) {  //rem from hunting, tracking

      Cell<String*> cll(agg->HUNTING);
      String* ptr;

      while ((ptr = cll.next())) {
         if (vict.isNamed(*ptr)) {
            delete ptr;
            ptr = agg->HUNTING.lose(cll);
            break;
         }//if
      }//while

      if (vict.isNamed(agg->TRACKING)) {
         agg->setTrackingTarget(NULL_STRING);
      }
   }//if
   
   float val = ((float)(vict.ALIGN) / 10.0);

   if (agg) {
      
      // Adjust so that the extremes are harder to reach.
      //
      // This causes lovely crashes when critters auto-expire i.e. minions of
      // hell and golems and therefore must be protected like so inside the if
      // (agg) loop... we always have a vict though.
      if ((vict.ALIGN < 0 && agg->ALIGN > 0) ||
            (vict.ALIGN > 0 && agg->ALIGN < 0)) {
         val *= 1.0/pow(10, fabs(agg->ALIGN/1000.0));
      }

      agg->adjAlignment(-((short)(val)));

   }

   if (vict.pc && agg && agg->pc) {
      doShowList(&vict, Selectors::instance().CC_gets_info_allow,
                 Selectors::instance().CC_none, pc_list,
                 CS_PKILLED_INFO,
                 agg->getName(), vict.getName(), vict.getCurRoomNum());

      agg->pc->pk_count++;
      vict.pc->pk_count--;
   }//if
   else if (vict.pc && agg) {
      doShowList(&vict, Selectors::instance().CC_gets_info_allow,
                 Selectors::instance().CC_none, pc_list,
                 CS_KILLED_INFO,
                 vict.getName(), agg->getName(), vict.getCurRoomNum());
   }
   
   if (agg) {
      disburse_xp(*agg, vict); //take care of xp
   }


   vict.doDie(do_msg, show_vict_tags);
   //dead_crit_to_corpse(vict, show_vict_tags); //remove all traces of vict
   if (agg) {
      do_just_killed_procs(*agg); //autoloot etc
   }
}//agg_kills_vict;

void damage_magic_shields(float damage, critter& vict) {
   float dam=(float)damage;
   int spells_used=0;

   if (is_affected_by(STONE_SKIN_SKILL_NUM, vict)) { spells_used++;
      dam*=(100.0+STONE_SKIN_EFFECT_DRM)/100.0; }
   if (is_affected_by(SANCTUARY_SKILL_NUM, vict)) { spells_used++;
      dam*=(100.0+SANCTUARY_EFFECT_DRM)/100.0; }
   if (is_affected_by(ARMOR_SKILL_NUM, vict)) spells_used++;
   if (is_affected_by(MAGIC_SHIELD_SKILL_NUM, vict)) spells_used++;
   if (is_affected_by(SHADOWS_BLESSING_SKILL_NUM, vict)) spells_used++;
   if (is_affected_by(DIVINE_PROTECTION_SKILL_NUM, vict)) spells_used++;
   if (is_affected_by(PRISMATIC_GLOBE_SKILL_NUM, vict)) spells_used++;

   if (spells_used >= 2) {
      vict.MOV -= (int)((damage-dam)/5);
      vict.MANA -= (int)((damage-dam)/5);
      if (vict.MOV < 0) vict.MOV=0;
      if (vict.MANA < 0) vict.MANA=0;
   }
   else if (spells_used == 1) {
      vict.MANA -= (int)((damage-dam)/5);
      if (vict.MANA < 0) vict.MANA=0;
   }
}

int exact_raw_damage(int damage, int damage_type, critter& vict, critter& agg) {
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

   // Special damages add an additional 10% owie at the 1:1 ratio.
   switch(damage_type) {
      case FIRE:
      case D_BREATH:
      case ICE:
      case ELECTRICITY:
         dam += ( dam*0.10 );
         break;
   }

   dam = dam * ((float)armor_value/200.0);
   
   damage_magic_shields(dam, vict);

   dam = (dam * (((float)vict.getDamRecMod(true))/100.0) * 
          ((float)(agg.getDamGivMod(true))/100.0));
   int pl;
   if (agg.pc && 
       (pl = get_percent_lrnd(ENHANCED_DAMAGE_SKILL_NUM, agg)) > 0) {
       // between 0.5% at 1%lrnd to 50% at 100%lrnd extra damage MAX
      dam += ( dam * ( ((float)d(1, (pl/2)+1))/100) );
      // old method dam += (float)(d(1, pl/20));
   }//if
   
   if ((agg.ALIGN < -350) && is_affected_by(PFE_SKILL_NUM, vict)) {
      dam -= (float)(d(1, 10));
      if (dam < 0)
        dam = 0;
   }//if
   
   if ((agg.ALIGN > 350) &&is_affected_by(PFG_SKILL_NUM, vict)) {
      dam -= (float)(d(1, 10));
      if (dam < 0)
        dam = 0;
   }//if
   
//DYS
   if (is_affected_by(DIVINE_RETRIBUTION_SKILL_NUM, vict)) {
       // example numbers:
       // at +/- 1000 alignment, and 100% learned, the victim returns 100% 
       // at +/- 500 alignment, and 100% learned, the victim returns 50%
       // at +/- 1000 alignment, and 50% learned, the victim returns 50%
       // at +/- 500 alignment, and 50% learned, the victim returns 25%
       float modifier = get_percent_lrnd(DIVINE_RETRIBUTION_SKILL_NUM, vict);
       modifier /= 1000;
       modifier *= abs(vict.getAlignment());
       modifier /= 100;

       agg.takeDamage( (int)( (dam * 0.40) * modifier), ELECTRICITY, vict);
   }

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
      if ((pl = get_percent_lrnd(EARTHMELD_SKILL_NUM, vict)) > d(1,115)) {
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

   // Special checks for Flesh-to-Stone
   if (is_affected_by(FLESH_TO_STONE_SKILL_NUM, vict)) {
      if (dam > 3.0) {
         dam = 3.0;
      }
   }

   if (is_affected_by(FLESH_TO_STONE_SKILL_NUM, agg)) {
      // Just in case someone lets some damage slip through...
      if (dam > 5.0) {
         dam = 5.0;
      }
   }

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

  damage_magic_shields(dam, vict);

  dam *= (((float)vict.getDamRecMod(true))/100.0);
   
  // Give PC's a slight edge, maybe modify after the game has mature
  // players.
  if (!vict.isPc()) {
     dam *= 0.90;
  }

  vict.HP -= (int)(dam);
  return (int)dam;
}//exact_raw_damage

void disburse_xp(critter& agg, const critter& vict) {
   Cell<critter*> cll;
   critter* crit_ptr;
   int tot_levs = 0;
   long xp_to_be_gained = (vict.EXP/config.experienceDivisor+30-agg.getLevel());
   long xp_per_level;

   //log("In disburse_xp.\n");
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In disburse_xp, agg: " << *(agg.getName())
             << " " << agg.getIdNum() << " vict: " << *(vict.getName())
             << " " << vict.getIdNum() << endl;
   }

   // no exp gain in a coliseum
   if (!agg.getCurRoom()->isColiseum()) {
      if (agg.GROUPEES.isEmpty()) { //solitary person
         gain_xp(agg, xp_to_be_gained, TRUE);
      }//if
      else { //in a group
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
      }//in a group
   }
}//disburse_xp


/*
 RJY [Trice] added this function for the sole reason of debugging.
 with this function I can set a br point in GDB directly and not
 have to weed through another functions crap ;)


 AsPer Bens request I tried to use crit->show and crit->emote but got compiler
 errors, dropped back to crit.show and crit.emote and got even more errors, not
 sure what I am doing wrong. Instead of commiting BS code I left in the legecy 
 C style code.
 -Ron [12/20/99]

*/
void avian_gain_level(critter& crit) {
   String buf;
   
   if ( crit.isAvian() ) { // For sanity reasons
      switch( crit.getLevel() )
      {
         case 5: {
                    Sprintf(buf, "writhes in agony as %s nose transforms into a beak.", get_his_her(crit));
                    crit.emote(buf);
                    crit.show("You writhe in agony as your nose hardens to form a beak!\n");
                 }
                 break;
         case 10: {
                     Sprintf(buf, "howls as wings sprout from %s back.", get_his_her(crit));
                     crit.emote(buf);
                     crit.show("Sharp pains shoot from your back as a pair of wings begin to form.\n");
                     if ( crit.EQ[12] ) { //on body
                        Sprintf(buf, "new wings destroy %pS, sending it to the ground",
                              long_name_of_obj(*(crit.EQ[12]), ~0));
                        crit.pemote(buf);
                        Sprintf(buf,"Your wings smash %pS, crumbling it to the ground",
                              long_name_of_obj(*(crit.EQ[12]), ~0));
                        crit.show(buf);
                        remove_eq_effects(*(crit.EQ[12]), crit, FALSE, FALSE, 12);
                        crit.gainInv(crit.EQ[12]);
                        crit.EQ[12] = NULL;
                     }//if wearing body armor
                  }
                  break;
         case 15: {
                     Sprintf(buf, "howls as %s wings grow to full length.", get_his_her(crit));
                     crit.emote(buf);
                     crit.show("Excruciating pain befalls you as your wings take full form.\n");
                     if( crit.EQ[4] ) { //on back
                        Sprintf(buf, "wings destroy %pS, sending it to the ground",
                              long_name_of_obj(*(crit.EQ[4]), ~0));
                        crit.pemote(buf);
                        Sprintf(buf, "Your wings smash %pS, crumbling it to the ground",
                              crit.EQ[4]->getLongName(crit));
                        crit.show(buf);
                        remove_eq_effects(*(crit.EQ[4]), crit, FALSE, FALSE, 4);
                        crit.gainInv(crit.EQ[4]);
                        crit.EQ[4] = NULL;
                     }//if wearing armor _around_ body (cloak, etc)
                  }
                  break;
         case 20: {
                     Sprintf(buf, "falls over as %s feet transform into wicked looking claws.",
                           get_his_her(crit));
                     crit.emote(buf);
                     crit.show("You fall over as your feet transform into mighty claws.\n");
                     if( crit.EQ[15] ) { //feet
                        Sprintf(buf, "claws destroy %pS sending them to the ground",
                              long_name_of_obj(*(crit.EQ[15]), ~0));
                        crit.pemote(buf);
                        Sprintf(buf, "Your claws smash %pS, sending them to the ground",
                              long_name_of_obj(*(crit.EQ[15]), ~0) );
                        crit.show(buf);
                        remove_eq_effects(*(crit.EQ[15]), crit, FALSE, FALSE, 15);
                        crit.gainInv(crit.EQ[15]);
                        crit.EQ[15] = NULL;
                     }//if wearing something on their feet
                  }
                  break;
         case 25: {
                     Sprintf(buf, "A magnificant tail erupts from %pS's back.\n", crit.getName());
                     show_all(buf,room_list[crit.getCurRoomNum()]);
                     crit.show("A magnificant tails forms behind you, the metamorphosis is complete!\n");
                  }
                  break;
      }
   }
}//avain_gain_level


void gain_level(critter& crit) {

   if (crit.LEVEL >= 30) {
      crit.show("You can learn no more from battle.\n");
      if (crit.LEVEL > 40) {
         crit.LEVEL = 40; //for those over level 40 by some other means
      }//if
      return;
   }//if

   int hp_gain = d(1, crit.getCON(true)/2);
   int mana_gain = d(1, crit.getWIS(true)/2);

   int _class = crit.getClass();
   switch (_class)
      {
      case WARRIOR:
      case RANGER:
      case THIEF:
         hp_gain += d(2, crit.getCON(true)/2);
         break;
      case BARD:
         hp_gain += d(3, crit.getCON(true)/2);
         break;
      case SAGE:
      case WIZARD:
      case CLERIC:
      case ALCHEMIST:
      case NECROMANCER:
         mana_gain += d(2, crit.getWIS(true)/2);
         break;
      default:
         if (mudlog.ofLevel(DBG)) {
            mudlog << "WARNING: default class in gain_level: " << _class
                   << endl;
         }
      }//switch

   crit.LEVEL++;
   crit.PRACS += d(2, (int)((float)(crit.getINT(true))/6.0)) + 1;
   crit.adjManaMax(mana_gain);
   crit.adjHP_MAX(hp_gain);
   crit.adjMovMax(d(1,crit.getDEX(true)));
   crit.show("You rise a level.\n");

   if ( crit.isAvian() ) {
      avian_gain_level(crit);
   }
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
      mudlog.log(LS_ERROR, "ERROR:  mob sent to dead_crit_to_corpse.\n");
      return;
   }//if

   /* first, cut all links to critter */

   if (vict.temp_crit) {
      //Takes care of shielding and so on.
      vict.temp_crit->Clear();
   }//if

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
   corpse = obj_to_sobj(obj_list[config.corpseObject], 
                        room_list[vict.getCurRoomNum()].getInv(),
                        vict.getCurRoomNum());

   recursive_init_loads(*corpse, 0);

   // Set edible flag on corpse if was on mob.
   if (vict.isEdible()) {
      corpse->setButcherable(TRUE);
   }

   corpse->bag->time_till_disolve = 10; // in ticks
   Sprintf(buf, "The corpse of %pS lies here.",
                name_of_crit(vict, ~0));
   corpse->in_room_desc = buf;
   room_list[vict.getCurRoomNum()].gainInv(corpse);

   /* Set the weight to that of the living being. */
   corpse->setEmptyWeight(vict.getNakedWeight());

         /*  gold  */

   if (vict.GOLD > 0) {
      gold = obj_to_sobj(obj_list[config.goldCoinsObject], &(corpse->inv), vict.getCurRoomNum());
      
      obj_list[config.goldCoinsObject].incrementCurInGame();
      
      gold->cur_stats[1] = vict.GOLD; //transfer gold
      vict.GOLD = 0;
      
      corpse->inv.append(gold);
   }//if

         /* eq  */
   for (int i = 0; i<MAX_EQ; i++) {
      if (vict.EQ[i]) {
         remove_eq_effects(*(vict.EQ[i]), vict, TRUE, FALSE, i);
         if (vict.EQ[i]->IN_LIST)
            vict.EQ[i]->IN_LIST = &(corpse->inv);
         corpse->inv.append(vict.EQ[i]);
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
               corpse->inv.append(o_ptr);
            }//while
            vict.PERM_INV.clear(); //should NEVER be SOBJ's
         }//if
      }//if
   }//if

   // behead stuff
   {
      if (!corpse->obj_proc) {
         corpse->obj_proc = new obj_spec_data;
      }//if
      object* ptr = corpse->obj_proc->head_ptr =
         obj_to_sobj(obj_list[config.HeadObject], &(corpse->inv),
               vict.getCurRoomNum());
      recursive_init_loads(*ptr, 0);

      ptr->names.append(new String(*(vict.names.peekFront())));

      Sprintf(buf, "the severed head of %pS",
            vict.getName());
      ptr->short_desc = buf;

      Sprintf(buf, "The severed head of %pS lies here rotting.",
            vict.getName());
      ptr->in_room_desc = buf;

      Sprintf(buf, "This is the severed and rotting head of %pS.",
            vict.getName());
      ptr->long_desc = buf;

      corpse->obj_proc->obj_spec_data_flags.turn_on(11);
   }
   // end behead stuff

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
            obj_to_sobj(obj_list[config.pcSkinObject], &(corpse->inv),
                        vict.getCurRoomNum());

         recursive_init_loads(*ptr, 0);

         ptr->names.append(new String(*(vict.names.peekFront())));
         Sprintf(buf, "the tattered skin of %pS", vict.names.peekFront());
         ptr->short_desc = buf;
         Sprintf(buf, "The tattered skin of %pS lies here.", 
               vict.names.peekFront());
         ptr->in_room_desc = buf;
         Sprintf(buf,
               "This large piece of %s skin was recently hacked from the corpse of %pS.\n"
               "You wonder why anyone would want the skin of a %s, but perhaps it is just\n"
               "a trophy--a symbol of %pS's defeat.\n",
               get_race_name(vict.RACE),
               name_of_crit(vict, ~0), get_race_name(vict.RACE),
               name_of_crit(vict, ~0));
         ptr->long_desc = buf;
         corpse->obj_proc->obj_spec_data_flags.turn_on(2);
      }//if
   }//else

            /* punish and re-enter the pc into the game, if its a pc  */
   if (vict.pc) {
      if (vict.CON > 0 || vict.getCurRoom()->isColiseum()) {
         vict.MANA = 1;
         vict.HP   = 1;
         vict.MOV  = 1;
         vict.HUNGER = 20;
         vict.THIRST = 20;
         vict.setPosn(POS_REST);

         // No loss of experience in a coliseum
         if (!vict.getCurRoom()->isColiseum()) {
            if (vict.EXP < 5000000) vict.EXP -= (vict.EXP/13);
            else vict.EXP -= 500000;
         }

         if (show_vict_tags) {
            show_vict_tags = FALSE; //Tell calling code not to show more tags.
            if (vict.isUsingClient()) {
               vict.show(CTAG_END_BATTLE(vict.whichClient()));
            }
            else if (vict.isUsingColor()) {
               vict.show(*(vict.getDefaultColor()));
            }
         }//if we should show tags here...
         
         Cell<stat_spell_cell*> spcll(vict.affected_by);
         stat_spell_cell* sp_ptr;
         while ((sp_ptr = spcll.next())) {
            rem_effects_crit(sp_ptr->stat_spell, vict, FALSE);
         }//while
         vict.affected_by.clearAndDestroy();
         
         vict.mini_affected_by.head(spcll);
         while ((sp_ptr = spcll.next())) {
            rem_effects_crit(sp_ptr->stat_spell, vict, FALSE);
         }//while
         vict.mini_affected_by.clearAndDestroy();
         
         
         int nrm = vict.getCurRoomNum();
         room_list[nrm].removeCritter(&vict); //decrement's cur in game if NPC
         leave_room_effects(room_list[nrm], vict, TRUE); //takes care of blocking

         room_list[config.loginRoom].gainCritter(&vict);
         Sprintf(buf, "%pS appears in the room looking quite dead!\n", 
                 name_of_crit(vict, ~0));
         buf.Cap();
         room_list[config.loginRoom].showAllCept(buf, &vict);
         vict.show(CS_YOU_DEAD_NL);
         vict.show(CS_WELCOME_AGAIN);
         vict.show(CS_FARE_BETTER);
         look(1, &NULL_STRING, vict); //sos ya can see the temple desc, etc.
         vict.PAUSE = 0;
         vict.save();
      }//if not con-death
      else { //PERM DEATH!!
         int nrm = vict.getCurRoomNum();
         vict.show(CS_END_OF_LUCK);

         Sprintf(buf,
"gasps a final farewell to the Realm in which %s has dwelt for %i years!",
                 get_he_she(vict), vict.getAge());
         emote(buf, vict, room_list[nrm], TRUE);

         doShowList(&vict, Selectors::instance().CC_gets_info_allow,
                    Selectors::instance().CC_none, pc_list,
                    CS_PERM_DEATH_INFO,
                    vict.getName());

         vict.doSuicide(); //poor bastard!
      }//if perm con death
   }//if
   else { //then it was a smob
      show_vict_tags = FALSE;
      int nrm = vict.getCurRoomNum();
      room_list[nrm].removeCritter(&vict); //decrement's cur in game if NPC
      leave_room_effects(room_list[nrm], vict, TRUE); //takes care of blocking

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
      mudlog.log(LS_ERROR, "ERROR:  mob_to_smob called on nonMOB.\n");
      do_shutdown = TRUE;
      exit(100);
   }//if

   if (mob.pc) {
      mudlog.log(LS_ERROR, "ERROR:  mob has pc_data in mob_to_smob.\n");
      do_shutdown = TRUE;
      exit(113);
   }//if

   mob.MOB_FLAGS.turn_on(4); //now it can be/should be reset when pulsed

   critter* crit_ptr = new critter(mob);

   obj_ptr_log << "CRI_CR " << mob.getIdNum() << " " << crit_ptr << "\n";

   if (!name || ((room_num < 0) || (room_num > NUMBER_OF_ROOMS))) {
      mudlog.log(LS_ERROR,
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
   crit_ptr->mob->setTicksOld(0);

   return crit_ptr;
}//mob_to_smob


critter* mob_to_smob(critter& mob, const int room_num,
                     int suppress_msg) {
   if ((room_num > NUMBER_OF_ROOMS) || (room_num < 0)) {
      mudlog.log(LS_ERROR, "ERROR:  room_num out of range in mob_to_smob.\n");
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
      mudlog.log(LS_ERROR, "ERROR:  mob_to_smob called on nonMOB.\n");
      core_dump("mob_to_smob1");
   }//if

   if (mob.pc) {
      mudlog.log(LS_ERROR, "ERROR:  mob has pc_data in mob_to_smob.\n");
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
   crit_ptr->mob->setTicksOld(0);

   return crit_ptr;
}//mob_to_smob



object* obj_to_sobj(object& obj, PtrList<object>* in_list,
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
         mudlog.log(LS_ERROR, "ERROR:  obj_sub_a_4_b failed in obj_to_sobj.\n");
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


object*  obj_to_sobj(object& obj, PtrList<object>* in_list, int rm_num) {
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
