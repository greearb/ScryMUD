// $Id: dam_spll.cc,v 1.14 2002/01/31 15:01:03 gingon Exp $
// $Revision: 1.14 $  $Author: gingon $ $Date: 2002/01/31 15:01:03 $

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

#include "dam_spll.h"
#include "commands.h"
#include "command2.h"
#include <stdio.h>
#include "misc.h"
#include "misc2.h"
#include "battle.h"
#include "classes.h"
#include "spells.h"
#include "skills.h"
#include <PtrArray.h>

SpellSpearOfDarkness spellSpearOfDarkness;
SpellOrbOfPower spellOrbOfPower;
SpellHolyWord spellHolyWord;


void SpellSpearOfDarkness::doSpellEffects(){
    String buf(100);
    short do_join_in_battle = TRUE;
    short do_fatality = FALSE;
    critter& victim = *(spellSpearOfDarkness.victim);
    critter& agg = *(spellSpearOfDarkness.agg);
	
    if (did_spell_hit(victim, NORMAL, agg, clvl, TRUE)){

        float dmg = (float)(d(7, (10 + clvl/5 - agg.ALIGN/200)));
        if ((agg.ALIGN < -350) && (victim.ALIGN > 350)) dmg *= 1.5;

        exact_raw_damage((int)(dmg), NORMAL, victim, agg);

	    if (&victim == &agg) {
            if (victim.HP < 0) {
                show("You pierce your own heart!!\n", agg);
                Sprintf(buf, "pierces %s own heart with %s spear of darkness!\n",
                        get_his_her(agg), get_his_her(agg));
                emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
                do_fatality = TRUE;
            }//if
            else {
                show("You impale yourself with your spear of darkness!\n",
                     agg);
                Sprintf(buf, "impales %s with %s spear of dark energy.\n",
                        get_himself_herself(victim),
                        get_his_her(agg));
                emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            }//else
            do_join_in_battle = FALSE;
        }//if agg == victim
        else { //agg is NOT the victim
             if (victim.HP < 0) {
                Sprintf(buf,
                     "Your spear of dark energy protrudes from %S's unbeating heart!\n",
                     name_of_crit(victim, agg.SEE_BIT));
                show(buf, agg);
                Sprintf(buf,
                        "%S pierces your heart with %s spear of dark energy!\n",
                        name_of_crit(agg, victim.SEE_BIT),
                        get_his_her(agg));
                buf.Cap();
                show(buf, victim);
                Sprintf(buf, "impales %S's heart with %s spear of dark energy!\n",
                        name_of_crit(victim, ~0),
                        get_his_her(victim));
                emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &victim);
                do_fatality = TRUE;
             }//if fatality
        
	     else { //no fatality
		
                 Sprintf(buf, "You conjure a spear of pure darkness and impale %S with it!.\n",
                         name_of_crit(victim, agg.SEE_BIT));
                 show(buf, agg);
                 Sprintf(buf, "%S's spear of dark energy slams into your chest!!\n",
                     name_of_crit(agg, victim.SEE_BIT));
                 buf.Cap();
                 show(buf, victim);
                 Sprintf(buf, "impales %S with %s spear of dark energy!\n", name_of_crit(victim, ~0),
                         get_his_her(agg));
                 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &victim);
            }
        }
    } else if (&agg == &victim) {
        show("Your spear circles towards you but misses!\n", agg);
        Sprintf(buf, "misses %s with %s spear of dark energy!!\n",
                get_himself_herself(victim),
                get_his_her(victim));
        emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
        do_join_in_battle = FALSE;
    }//if
    else { //missed, and agg does NOT equal vict
        Sprintf(buf, "You miss %S with your spear of dark energy.\n",
               name_of_crit(victim, agg.SEE_BIT));
        show(buf, agg);String buf(100);
        Sprintf(buf,
                "You narrowly elude %S's spear of dark energy!\n",
                name_of_crit(agg, victim.SEE_BIT));
        buf.Cap();
        show(buf, victim);
        Sprintf(buf, "misses %S with %s spear of dark energy!\n",
               name_of_crit(victim, ~0),
               get_his_her(agg));
        emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &victim);
    }//else did miss AND vict NOT equal to agg

    if (!do_fatality && do_join_in_battle &&
       !HaveData(&victim, agg.IS_FIGHTING)) {
        join_in_battle(agg, victim);
    }//if

    if (do_fatality) {
        agg_kills_vict(&agg, victim);
    }//if
} //SpellSpearOfDarkness::onCast


void SpellOrbOfPower::doSpellEffects() {
   String buf(100);
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   critter& victim = *(spellOrbOfPower.victim);
   critter& agg = *(spellOrbOfPower.agg);

   if (did_spell_hit(victim, NORMAL, agg, clvl, TRUE)){
      int dmg = d(10, clvl + 10);
      exact_raw_damage(dmg, FIRE, victim, agg);

      if (&victim == &agg) { // Critter attacked itself...
         if (victim.HP < 0) { // Fatality
            show("Your power consumes you!\n", agg);
            Sprintf(buf, "invokes powers %s cannot controll!", get_he_she(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         } // if fatality
         else { // Survived their foolish act
            show("Your orb crushes itself into your body!\n", agg);
            Sprintf(buf, "orb crushes itself into %s body!", get_his_her(agg));
            pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         } // survived
         do_join_in_battle = FALSE;
      } // if agg == vict
      else { // agg not the victim
         if (victim.HP < 0) { // Fatality
            Sprintf(buf, "Your orb tears the flesh from %S's body!\n",
                    name_of_crit(victim, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, "%S's orb tears the flesh from your body!\n",
                    name_of_crit(agg, victim.SEE_BIT));
            show(buf, victim);
            Sprintf(buf, "orb rips the flesh from %S's body!",
                    name_of_crit(victim, ~0));
            pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &victim);
            do_fatality = TRUE;
         } //if fatality
         else { // survived
            if (dmg <= 5*(clvl+10)) { // kinda wimpy, average or worse damage...
               Sprintf(buf, "Your orb slams into %S!\n",
                       name_of_crit(victim, agg.SEE_BIT));
               show(buf, agg);
               Sprintf(buf, "%S's orb slams into you!!\n",
                       name_of_crit(agg, victim.SEE_BIT));
               show(buf, victim);
               Sprintf(buf, "orb slams into %S!",
                       name_of_crit(victim, ~0));
               pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &victim);
 
            } // if kinda wimpy

            else {  // oh yeah, that's the stuff...
               Sprintf(buf, "Your orb crushes itself into %S's body!\n",
                       name_of_crit(victim, agg.SEE_BIT));
               show(buf, agg);
               Sprintf(buf, "%S's orb crushes itself into your body!\n",
                       name_of_crit(agg, victim.SEE_BIT));
               show(buf, victim);
               Sprintf(buf, "orb crushes itself into %S's body!",
                       name_of_crit(victim, ~0));
               pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &victim);

               // Kinda stops you in yer tracks, don't it?
               victim.PAUSE+=d(1,3);
            } // if nasty gaping wound time
         }
      } // agg not victim
   } // did hit
   else { //missed

      if (&agg == &victim) { // couldn't even hit yourself!?
         show("Your orb passes harmlessly through your body.\n",agg);
         Sprintf(buf, "orb passes harmlessly through %s body.", get_his_her(victim));
         pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         do_join_in_battle = FALSE;
      } // missed self
      else { // missed somebody else...
         Sprintf(buf, "%S dodges your orb just before %s is struck!\n",
                 name_of_crit(victim, agg.SEE_BIT), get_he_she(victim));
         show(buf, agg);
         Sprintf(buf, "You duck out of the path of %S's orb just in time!\n",
                 name_of_crit(agg, victim.SEE_BIT));
         show(buf, victim);
         Sprintf(buf, "orb blasts past %S, missing by mere inches!",
                 name_of_crit(victim, ~0));
         pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &victim);
      }
   }//missed
   if (!do_fatality && do_join_in_battle && !HaveData(&victim, agg.IS_FIGHTING))
      join_in_battle(agg, victim);
   
   if (do_fatality) {
      if (victim.mob && victim.MOB_FLAGS.get(16)) {
         victim.MOB_FLAGS.turn_off(16);
         room_list[victim.getCurRoomNum()].gainInv(&obj_list[victim.mob->skin_num]);
      }
      agg_kills_vict(&agg, victim);
   }
}


void SpellHolyWord::doSpellEffects() {
   String buf(100);
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   critter& vict = *(spellHolyWord.victim);
   critter& agg = *(spellHolyWord.agg);

   
   if( did_spell_hit(vict, NORMAL, agg, clvl, TRUE)){

      int dmg = d(7, clvl + 5);

      exact_raw_damage(dmg, NORMAL, vict, agg);

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("You god destroys you!\n", agg);
            Sprintf(buf, "is destroyed by %s own god!!",
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show("Your god wreaks vengeance upon you!\n", agg);
            Sprintf(buf, "god wreaks vengeance upon %s.", 
                    get_him_her(vict));
            pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, 
                  "Your god destroys %S at your behest!\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S's god destroys you at %s behest!!\n",
                    name_of_crit(agg, vict.SEE_BIT),
                    get_his_her(agg));
            show(buf, vict);
            Sprintf(buf, "god destroys %S at %s behest!",
                    name_of_crit(vict, ~0),
                    get_his_her(agg));
            pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, "Your god greatly injures %S at your behest!.\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S calls the wrath of %s god down upon you!!\n",
                    name_of_crit(agg, vict.SEE_BIT),
                    get_his_her(agg));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "god wreaks vengeance upon %S at %s behest!", 
                    name_of_crit(vict, ~0),
                    get_his_her(agg));
            pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else { //missed
     if (&agg == &vict) {
       show("Your god does not listen.\n", agg);
       Sprintf(buf, "prays in vain to %s god!!", 
               get_him_her(vict));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       do_join_in_battle = FALSE;
     }//if
     else { //missed, and agg does NOT equal vict
       show("Your god does not listen!\n", agg);
       Sprintf(buf, "%S prays for your demise in vain!!\n",
               name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "prays for %S's demise in vain!", 
               name_of_crit(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
     }//else did miss AND vict NOT equal to agg
   }//did_miss



   if (!do_fatality && do_join_in_battle && 
       !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }
}



/*
void do_cast_dark_spear(critter& vict, critter& agg, int is_canned,
                       int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = SOD_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   if (!is_canned)
      lvl = agg.LEVEL;

   int lost_con = FALSE;
   if ((is_canned && (did_hit =
     do_cast_dark_spear(critter& vict, critter& agg, int is_canned,
                       int lvl)                 did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) &&
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

      float dmg = (float)(d(7, (10 + lvl/5 - agg.ALIGN/200)));
      if ((agg.ALIGN < -350) && (vict.ALIGN > 350)) {
         dmg *= 1.5;
      }

      exact_raw_damage((int)(dmg), NORMAL, vict, agg);
      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("You pierce your own heart!!\n", agg);
            Sprintf(buf, "pierces %s own heart with %s spear of darkness!\n",
                    get_his_her(agg), get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show("You impale yourself with your spear of darkness!\n",
                 agg);
            Sprintf(buf, "impales %s with %s spear of dark energy.\n",
                    get_himself_herself(vict),
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf,
         "Your spear of dark energy protrudes from %S's unbeating heart!\n",
                 name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf,
                    "%S pierces your heart with %s spear of dark energy!\n",
                    name_of_crit(agg, vict.SEE_BIT),
                    get_his_her(agg));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "impales %S's throat with %s spear of dark energy!\n",
                    name_of_crit(vict, ~0),
                    get_his_her(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf,
                    "You conjure a spear of pure darkness and impale %S with it!.\n"
,
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, "%S's spear of dark energy slams into your chest!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "impales %S with %s spear of dark energy!\n",
                    name_of_crit(vict, ~0),
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
     if (!is_canned)
       agg.MANA -= spell_mana;
     if (&agg == &vict) {
       show("Your spear circles towards you but misses!\n", agg);
       Sprintf(buf, "misses %s with %s spear of dark energy!!\n",
               get_himself_herself(vict),
               get_his_her(vict));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       do_join_in_battle = FALSE;
     }//if
     else { //missed, and agg does NOT equal vict
       Sprintf(buf, "You miss %S with your spear of dark energy.\n",
               name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);String buf(100);
       Sprintf(buf,
               "You narrowly elude %S's spear of dark energy!\n",
               name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "misses %S with %s spear of dark energy!\n",
               name_of_crit(vict, ~0),
               get_his_her(agg));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
     }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg,
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
      do_join_in_battle = FALSE;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1;

   if (!do_fatality && do_join_in_battle &&
       !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_dark_spear


void cast_dark_spear(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = SOD_SKILL_NUM;

   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to impale??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if

   if (!(vict = check_for_diversions(*vict, "GSM", pc)))
     return;

   do_cast_dark_spear(*vict, pc, FALSE, 0);
}//cast_dark_spear */



/*void do_cast_orb_of_power(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int   spell_num = ORB_OF_POWER_SKILL_NUM;
   int   spell_mana = get_mana_cost(spell_num, agg);
   
   if (!is_canned)
      lvl = agg.LEVEL;
   
   int   lost_con = FALSE;
   if ((is_canned && (did_hit = did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
      (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) &&
        (did_hit = did_spell_hit(agg, NORMAL, vict)))) { // Did hit
      int dmg = d(10, lvl + 10);
      
      exact_raw_damage(dmg, FIRE, vict, agg);
      
      if (!is_canned)
         agg.MANA -= spell_mana;
      
      if (&vict == &agg) { // Critter attacked itself...
         if (vict.HP < 0) { // Fatality
            show("Your power consumes you!\n", agg);
            Sprintf(buf, "invokes powers %s cannot controll!", get_he_she(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         } // if fatality
         else { // Survived their foolish act
            show("Your orb crushes itself into your body!\n", agg);
            Sprintf(buf, "orb crushes itself into %s body!", get_his_her(agg));
            pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         } // survived
         do_join_in_battle = FALSE;
      } // if agg == vict
      else { // agg not the victim
         if (vict.HP < 0) { // Fatality
            Sprintf(buf, "Your orb tears the flesh from %S's body!\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, "%S's orb tears the flesh from your body!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            show(buf, vict);
            Sprintf(buf, "orb rips the flesh from %S's body!",
                    name_of_crit(vict, ~0));
            pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         } //if fatality
         else { // survived
            if (dmg <= 5*(lvl+10)) { // kinda wimpy, average or worse damage...
               Sprintf(buf, "Your orb slams into %S!\n",
                       name_of_crit(vict, agg.SEE_BIT));
               show(buf, agg);
               Sprintf(buf, "%S's orb slams into you!!\n",
                       name_of_crit(agg, vict.SEE_BIT));
               show(buf, vict);
               Sprintf(buf, "orb slams into %S!",
                       name_of_crit(vict, ~0));
               pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
 
            } // if kinda wimpy

            else {  // oh yeah, that's the stuff...
               Sprintf(buf, "Your orb crushes itself into %S's body!\n",
                       name_of_crit(vict, agg.SEE_BIT));
               show(buf, agg);
               Sprintf(buf, "%S's orb crushes itself into your body!\n",
                       name_of_crit(agg, vict.SEE_BIT));
               show(buf, vict);
               Sprintf(buf, "orb crushes itself into %S's body!",
                       name_of_crit(vict, ~0));
               pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);

               // Kinda stops you in yer tracks, don't it?
               vict.PAUSE+=d(1,3);
            } // if nasty gaping wound time
         }
      } // agg not victim
      agg.PAUSE++;
   } // did hit
   else if (!did_hit) { // missed
      if (!is_canned)
         agg.MANA -= spell_mana;
      if (&agg == &vict) { // couldn't even hit yourself!?
         show("Your orb passes harmlessly through your body.\n",agg);
         Sprintf(buf, "orb passes harmlessly through %s body.", get_his_her(vict));
         pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         do_join_in_battle = FALSE;
      } // missed self
      else { // missed somebody else...
         Sprintf(buf, "%S dodges your orb just before %s is struck!\n",
                 name_of_crit(vict, agg.SEE_BIT), get_he_she(vict));
         show(buf, agg);
         Sprintf(buf, "You duck out of the path of %S's orb just in time!\n",
                 name_of_crit(agg, vict.SEE_BIT));
         show(buf, vict);
         Sprintf(buf, "orb blasts past %S, missing by mere inches!",
                 name_of_crit(vict, ~0));
         pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
      } // missed somebody
   } // missed
   else if (lost_con) { // lost concentration
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, room_list[agg.getCurRoomNum()],
            TRUE);
      if (!is_canned)
         agg.MANA -= spell_mana / 2;
      do_join_in_battle = FALSE;
   }  // lost concentration
   else {
      mudlog.log(ERROR, "ERROR: in default of a spell.\n");
   }

   if (!do_fatality && do_join_in_battle && !HaveData(&vict, agg.IS_FIGHTING))
      join_in_battle(agg, vict);
   
   if (do_fatality) {
      if (vict.mob && vict.MOB_FLAGS.get(16)) {
         vict.MOB_FLAGS.turn_off(16);
         room_list[vict.getCurRoomNum()].gainInv(&obj_list[vict.mob->skin_num]);
      }
      agg_kills_vict(&agg, vict);
   }
} // do_cast_orb_of_power*/

/*void cast_orb_of_power(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = ORB_OF_POWER_SKILL_NUM;
   
   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else
      vict = ROOM.haveCritNamed(i_th, victim, pc);
   
   if (!vict) {
      show("Whom do you wish to disintegrate?\n", pc);
      return;
   }
  
   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc))
      return;

   do_cast_orb_of_power(*vict, pc, FALSE, 0);
} // cast_orb_of_power */
/*
void do_cast_holy_word(critter& vict, critter& agg, int is_canned,
                      int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = HOLY_WORD_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   int lost_con = FALSE;
   if ((is_canned && (did_hit = 
                      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

      int dmg = d(7, lvl + 5);

      exact_raw_damage(dmg, NORMAL, vict, agg);

      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("You god destroys you!\n", agg);
            Sprintf(buf, "is destroyed by %s own god!!",
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show("Your god wreaks vengeance upon you!\n", agg);
            Sprintf(buf, "god wreaks vengeance upon %s.", 
                    get_him_her(vict));
            pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, 
                  "Your god destroys %S at your behest!\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S's god destroys you at %s behest!!\n",
                    name_of_crit(agg, vict.SEE_BIT),
                    get_his_her(agg));
            show(buf, vict);
            Sprintf(buf, "god destroys %S at %s behest!",
                    name_of_crit(vict, ~0),
                    get_his_her(agg));
            pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, "Your god greatly injures %S at your behest!.\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S calls the wrath of %s god down upon you!!\n",
                    name_of_crit(agg, vict.SEE_BIT),
                    get_his_her(agg));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "god wreaks vengeance upon %S at %s behest!", 
                    name_of_crit(vict, ~0),
                    get_his_her(agg));
            pemote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
     if (!is_canned)
       agg.MANA -= spell_mana;
     if (&agg == &vict) {
       show("Your god does not listen.\n", agg);
       Sprintf(buf, "prays in vain to %s god!!", 
               get_him_her(vict));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       do_join_in_battle = FALSE;
     }//if
     else { //missed, and agg does NOT equal vict
       show("Your god does not listen!\n", agg);
       Sprintf(buf, "%S prays for your demise in vain!!\n",
               name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "prays for %S's demise in vain!", 
               name_of_crit(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
     }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
      do_join_in_battle = FALSE;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1; 

   if (!do_fatality && do_join_in_battle && 
       !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_holy_word
*/ 
/*
void cast_holy_word(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = HOLY_WORD_SKILL_NUM;

   if (victim->Strlen() == 0) 
      vict = Top(pc.is_fighting);
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to destroy??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
      return;
   }//if
   
   do_cast_holy_word(*vict, pc, FALSE, 0);
}//cast_holy_word
*/

void cast_dispel_evil(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = DISPEL_EVIL_SKILL_NUM;

   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to harm??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
   
   /* dispel good/evil messages and damage are the same */
   if ((pc.ALIGN < -350) || (vict->ALIGN > 350)) {
      show("OOPS!!\n", pc);
      do_cast_dispel_good(pc, pc, FALSE, 0);
   }//if
   else
     do_cast_dispel_good(*vict, pc, FALSE, 0);

}//cast_dispel_evil


void do_cast_dispel_good(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = DISPEL_GOOD_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = FALSE;
   if ((is_canned && (did_hit = did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, FIRE, vict)))) {

      int dmg = d(5, lvl + 5);

      exact_raw_damage(dmg, NORMAL, vict, agg);

      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("Your soul leaves your body in immortal pain!!\n", agg);
            Sprintf(buf, "makes %s own soul flee in immortal pain!!",
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show("You make your soul scream in immortal pain!\n", 
                 agg);
            Sprintf(buf, "makes %s soul scream in immortal anguish!", 
                   get_his_her(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, "You make %S's soul flee in immortal pain!\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S makes your soul flee in immortal anguish!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            show(buf, vict);
            Sprintf(buf, "makes %S's soul flee in immortal anguish!",
                    name_of_crit(vict, ~0));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, "You make %S's soul scream in pain!.\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, "%S makes your soul scream in anguish!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "makes %S's soul scream in pain!", 
                    name_of_crit(vict, ~0));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
     if (!is_canned)
       agg.MANA -= spell_mana;
     if (&agg == &vict) {
       show("You can't find your own soul!\n", agg);
       Sprintf(buf, "tries to harm %s own soul in vain!!", get_his_her(vict));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       do_join_in_battle = FALSE;
     }//if
     else { //missed, and agg does NOT equal vict
       if (vict.ALIGN < -350 && 1 == d(1,25))
          Sprintf(buf, "So you see, Evil will always triumph over Good, because Good is dumb.\n");
       else
          Sprintf(buf, "You can't get ahold of %S's immortal soul!\n",
                  name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
               "%S can't get ahold of your soul, thank god!!\n",
               name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "tries and fails to make %S's soul scream in pain!", 
               name_of_crit(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
     }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
      do_join_in_battle = FALSE;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1; 

   if (!do_fatality && do_join_in_battle && 
       !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_dispel_good
 

void cast_dispel_good(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = DISPEL_GOOD_SKILL_NUM;

   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to harm??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
   
   if ((pc.ALIGN > 350) || (vict->ALIGN < -350)) {
     show("OOPS!!\n", pc);
     do_cast_dispel_good(pc, pc, FALSE, 0);
   }//if
   else
     do_cast_dispel_good(*vict, pc, FALSE, 0);

}//cast_dispel_good



void do_cast_harm(critter& vict, critter& agg, int is_canned,
                      int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short lost_con = FALSE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = HARM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   if ((is_canned && (did_hit = 
                      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

      int dmg = d(5, lvl + 5);

      exact_raw_damage(dmg, NORMAL, vict, agg);

      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("You crush your internal organs!\n", agg);
            Sprintf(buf, "crushes %s internal organs!!",
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show(
             "You bruise your internal organs with the force of your will!\n", 
                 agg);
            Sprintf(buf, 
                    "bruises %s internal organs with the force of %s will!", 
                    get_his_her(vict),
                    get_his_her(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, 
                  "You pulverise %S's innards with the force of your will!\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S crushes your heart with the force of %s will!!\n",
                    name_of_crit(agg, vict.SEE_BIT),
                    get_his_her(agg));
            show(buf, vict);
            Sprintf(buf, "crushes %S's heart with the force of %s will!",
                    name_of_crit(vict, ~0),
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, "You bruise %S's organs with your will!.\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S bruises your organs with the force of %s will!!\n",
                    name_of_crit(agg, vict.SEE_BIT),
                    get_his_her(agg));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, 
                    "bruises %S's internal organs with the force of %s will!", 
                    name_of_crit(vict, ~0),
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
     if (!is_canned)
       agg.MANA -= spell_mana;
     if (&agg == &vict) {
       show("You fail to harm yourself.\n", agg);
       Sprintf(buf, "tries to harm %s in vain!!", 
               get_himself_herself(vict));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       do_join_in_battle = FALSE;
     }//if
     else { //missed, and agg does NOT equal vict
       Sprintf(buf, "You fail to harm %S!\n",
               name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
               "%S tries but fails to harm you!!\n",
               name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "tries and fails to make %S's soul scream in pain!", 
               name_of_crit(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
     }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
      do_join_in_battle = FALSE;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1; 

   if (!do_fatality && do_join_in_battle && 
       !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_harm
 

void cast_harm(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = HARM_SKILL_NUM;

   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to harm??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
   
   do_cast_harm(*vict, pc, FALSE, 0);

}//cast_harm


void do_cast_cause_critical(critter& vict, critter& agg, int is_canned,
                      int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short lost_con = FALSE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = CAUSE_CRITICAL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   if ((is_canned && (did_hit = 
                      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

      int dmg = d(5, lvl + 5);

      exact_raw_damage(dmg, CRONIC, vict, agg);

      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("You have a heart attack!\n", agg);
            Sprintf(buf, "gives %s a heart attack!!",
                    get_himself_herself(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show(
             "You cause an awful pain in your ass!\n", 
                 agg);
            Sprintf(buf, 
                    "causes an awful pain in %s side!", 
                    get_his_her(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, 
                    "You give %S a heart attack!\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S gives you a heart attack!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            show(buf, vict);
            Sprintf(buf, "gives %S a heart attack!",
                    name_of_crit(vict, ~0));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, "You give %S a mighty pain in %s arse!.\n",
                    name_of_crit(vict, agg.SEE_BIT),
                    get_his_her(vict));
            show(buf, agg);
            Sprintf(buf, 
                    "%S gives you an awful pain in your side!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, 
                    "causes an awful pain in %S's side!", 
                    name_of_crit(vict, ~0));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
     if (!is_canned)
       agg.MANA -= spell_mana;
     if (&agg == &vict) {
       show("You fail to harm yourself.\n", agg);
       Sprintf(buf, "tries to harm %s in vain!!", 
               get_himself_herself(vict));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       do_join_in_battle = FALSE;
     }//if
     else { //missed, and agg does NOT equal vict
       Sprintf(buf, "You fail to hurt %S!\n",
               name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
               "%S fails to hurt you!!\n",
               name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "fails to hurt %S!", 
               name_of_crit(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
     }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
      do_join_in_battle = FALSE;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1; 

   if (!do_fatality && do_join_in_battle && 
       !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_cause_critical
 

void cast_cause_critical(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = CAUSE_CRITICAL_SKILL_NUM;

   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to hurt??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
   
   do_cast_cause_critical(*vict, pc, FALSE, 0);

}//cast_cause_critical



void do_cast_cause_sickness(critter& vict, critter& agg, int is_canned,
                      int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = CAUSE_SICKNESS_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   int lost_con = FALSE;
   if ((is_canned && (did_hit = 
                      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

      int dmg = d(2, lvl);

      exact_raw_damage(dmg, CRONIC, vict, agg);

      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("You literally puke your guts out!\n", agg);
            Sprintf(buf, "retches so hard %s organs burst!!",
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show(
             "You make yourself sick!\n", 
                 agg);
            Sprintf(buf, 
                    "makes %s sick!", 
                    get_himself_herself(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, 
                    "You make %S literally puke %s guts out!!\n",
                    name_of_crit(vict, agg.SEE_BIT),
                    get_his_her(vict));
            show(buf, agg);
            Sprintf(buf, 
                    "%S makes you retch so hard you burst your organs!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            show(buf, vict);
            Sprintf(buf, "makes %S retch %s guts out!",
                    name_of_crit(vict, ~0),
                    get_his_her(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, "You make %S sick!.\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S makes you sick!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, 
                    "makes %S sick!", 
                    name_of_crit(vict, ~0),
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
     if (!is_canned)
       agg.MANA -= spell_mana;
     if (&agg == &vict) {
       show("You fail to make yourself puke.\n", agg);
       Sprintf(buf, "fails to make %s sick!!", 
               get_himself_herself(vict));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       do_join_in_battle = FALSE;
     }//if
     else { //missed, and agg does NOT equal vict
       Sprintf(buf, "You fail to make %S sick!\n",
               name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
               "%S makes you feel a little queasy!!\n",
               name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "makes %S feel a little queasy!", 
               name_of_crit(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
     }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
      do_join_in_battle = FALSE;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1; 

   if (!do_fatality && do_join_in_battle && 
       !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_cause_sickness
 

void cast_cause_sickness(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = CAUSE_SICKNESS_SKILL_NUM;

   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to harm??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
   
   do_cast_cause_sickness(*vict, pc, FALSE, 0);

}//cast_cause_sickness


void do_cast_lightning(critter& vict, critter& agg, int is_canned,
                      int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = LIGHTNING_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   int lost_con = FALSE;
   if ((is_canned && (did_hit = 
                      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

      int dmg = d(3, 7) + lvl;
      
      exact_raw_damage(dmg, ELECTRICITY, vict, agg);
      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("You blast yourself asunder with a lightning bolt!!\n", agg);
            Sprintf(buf, "blasts %s asunder with a lightning bolt!\n",
                    get_himself_herself(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show("You blast yourself with a lightning bolt!\n", 
                 agg);
            Sprintf(buf, "calls lightning down upon %s.", 
                   get_himself_herself(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, "You blast %S asunder with your lightning bolt!\n",
                 name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S blasts you asunder with %s lightning bolt!\n",
                    name_of_crit(agg, vict.SEE_BIT),
                    get_his_her(agg));
            show(buf, vict);
            Sprintf(buf, "blasts %S asunder with %s bolt of lightning!",
                    name_of_crit(vict, ~0),
                    get_his_her(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, 
                 "You call a bolt of lightning down on %S's head!.\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, "%S's bolt of lightning momentarily stuns you!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "fries %S with %s bolt of lightning!", 
                    name_of_crit(vict, ~0), 
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
     if (!is_canned)
       agg.MANA -= spell_mana;
     if (&agg == &vict) {
       Sprintf(buf, "You miss youself with your lightning bolt!\n", 
               name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, "misses %s with %s bolt of lighting!!", 
               get_himself_herself(vict),
               get_his_her(vict));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       do_join_in_battle = FALSE;
     }//if
     else { //missed, and agg does NOT equal vict
       Sprintf(buf, "You miss %S with your bolt of lightning.\n",
               name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
               "You narrowly elude %S's bolt of lightning!\n",
               name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "misses %S with %s bolt of lightning!", 
               name_of_crit(vict, ~0), 
               get_his_her(agg));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
     }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
      do_join_in_battle = FALSE;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1; 

   if (!do_fatality && do_join_in_battle && 
       !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_lightning
 

void cast_lightning(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = LIGHTNING_SKILL_NUM;

   if (victim->Strlen() == 0) 
      vict = Top(pc.is_fighting);
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to fry??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
   
   do_cast_lightning(*vict, pc, FALSE, 0);
}//cast_lightning



void do_cast_dark_dart(critter& vict, critter& agg, int is_canned,
                       int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = DOD_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   int lost_con = FALSE;
   if ((is_canned && (did_hit = 
                      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

      int dmg = d(2, 7) + lvl/2;
      if ((agg.ALIGN < 350) && (vict.ALIGN > 350))
         dmg *= 2;

      exact_raw_damage(dmg, NORMAL, vict, agg);
      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("You pierce your own throat!!\n", agg);
            Sprintf(buf, "pierces %s own throat with %s energy dart!\n",
                    get_his_her(agg), get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show("You pierce yourself with your dark dart of energy!\n", 
                 agg);
            Sprintf(buf, "pierces %s with %s dark energy dart.\n", 
                    get_himself_herself(vict), 
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, 
         "Your dark dart of energy protrudes from %S's unbreathing throat!\n",
                 name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                    "%S pierces your throat with %s dark energy dart!\n",
                    name_of_crit(agg, vict.SEE_BIT),
                    get_his_her(agg));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "pierces %S's throat with %s dark energy dart!\n",
                    name_of_crit(vict, ~0),
                    get_his_her(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, 
                    "You conjure a bolt of pure darkness and hit %S with it!.\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, "%S's dart of dark energy slams into your chest!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "hits %S with %s dart of dark energy!\n", 
                    name_of_crit(vict, ~0), 
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
     if (!is_canned)
       agg.MANA -= spell_mana;
     if (&agg == &vict) {
       show("Your dart circles towards you but misses!\n", agg);
       Sprintf(buf, "misses %s with %s dark dart of energy!!\n", 
               get_himself_herself(vict),
               get_his_her(vict));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       do_join_in_battle = FALSE;
     }//if
     else { //missed, and agg does NOT equal vict
       Sprintf(buf, "You miss %S with your dark energy dart.\n",
               name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
               "You narrowly elude %S's dark dart of energy!\n",
               name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "misses %S with %s dark energy dart!\n", 
               name_of_crit(vict, ~0), 
               get_his_her(agg));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
     }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
      do_join_in_battle = FALSE;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1; 

   if (!do_fatality && do_join_in_battle && 
       !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_dark_dart
 

void cast_dark_dart(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = DOD_SKILL_NUM;

   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to blast??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
   
   if (!(vict = check_for_diversions(*vict, "GSM", pc)))
     return;

   do_cast_dark_dart(*vict, pc, FALSE, 0);
}//cast_dark_dart


void do_cast_shocking_grasp(critter& vict, critter& agg, int is_canned,
                      int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = SHOCKING_GRASP_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);
 

   if (!is_canned)
      lvl = agg.LEVEL;

   int lost_con = FALSE;
   if ((is_canned && (did_hit = 
                      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

      exact_raw_damage((d(4, 8) + lvl), ELECTRICITY, vict, agg);
      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("Your just kill yourself!!\n", agg);
            Sprintf(buf, "shocks %s to death!\n",
                    get_himself_herself(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show("You shock yourself!\n", 
                 agg);
            Sprintf(buf, "shocks %s with %s crackling hands.\n", 
                   get_himself_herself(vict), 
                   get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, 
                 "The acrid smell of ozone rises from %S's charred body!\n",
                 name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                   "%S shocks you to death!\n",
                   name_of_crit(agg, vict.SEE_BIT));
            show(buf, vict);
            Sprintf(buf, "burns %S to death with %s crackling hands!\n",
                    name_of_crit(vict, ~0),
                    get_his_her(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, 
                 "Your fingers crackle with electricity as you shock %S!.\n",
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, "%S's crackling fingers burn into your side!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "shocks %S with %s crackling fingers!\n", 
                    name_of_crit(vict, ~0), 
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
      if (!is_canned)
        agg.MANA -= spell_mana;
      if (&agg == &vict) {
        Sprintf(buf, "You can't even grab yourself!!\n", 
                name_of_crit(vict, agg.SEE_BIT));
        show(buf, agg);
        Sprintf(buf, "tries to grab %s but misses!!\n", 
                get_himself_herself(vict));
        emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
        do_join_in_battle = FALSE;
      }//if
      else { //missed, and agg does NOT equal vict
         Sprintf(buf, "%S eludes your fearful grasp.\n",
                 name_of_crit(vict, agg.SEE_BIT));
         show(buf, agg);

         Sprintf(buf, 
                 "You narrowly evade %S's shocking grasp!\n",
                 name_of_crit(agg, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);
 
         Sprintf(buf, "misses %S with %s shocking grasp!\n", 
                    name_of_crit(vict, ~0), 
                    get_his_her(agg));
         emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
      }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
      do_join_in_battle = FALSE;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1; 

   if (!do_fatality && do_join_in_battle && 
       !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_shocking_grasp
 

void cast_shocking_grasp(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = SHOCKING_GRASP_SKILL_NUM;

   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to blast??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
   
   if (!(vict = check_for_diversions(*vict, "GM", pc)))
     return;

   do_cast_shocking_grasp(*vict, pc, FALSE, 0);
}//cast_shocking_grasp


void do_cast_rainbow(critter& vict, critter& agg, int is_canned,
                      int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = RAINBOW_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   int lost_con = FALSE;
   if ((is_canned && (did_hit = 
                      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

      exact_raw_damage((d(4, 15) + lvl), FIRE, vict, agg);
      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("Your colorful blast rips you in half!\n", agg);
            Sprintf(buf, "blasts %s in half with vivid color!\n",
                    get_himself_herself(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show("You blast yourself with vivid colors!\n", 
                 agg);
            Sprintf(buf, "blasts %s with %s vivid rainbow.\n", 
                   get_himself_herself(vict), 
                   get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, 
                 "Your fingers drip color as your blast rips %S in two!\n",
                 name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                   "%S rips you in half with a colorful blast!\n",
                   name_of_crit(agg, vict.SEE_BIT));
            show(buf, vict);
            Sprintf(buf, "rips %S in half with a colorful blast!\n",
                    name_of_crit(vict, ~0));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, 
          "Your fingers drip color as you blast %S with your rainbow!\n", 
                       name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, "%S's rainbow burns into your chest!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "blasts %S with %s vivid rainbow!\n", 
                    name_of_crit(vict, ~0), 
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
      if (!is_canned)
        agg.MANA -= spell_mana;
      if (&agg == &vict) {
         show("You miss yourself with your rainbow.\n", agg);
         Sprintf(buf, "misses %s with %s colored blast.\n", 
                 get_himself_herself(vict),
                 get_his_her(vict));
         emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         do_join_in_battle = FALSE;
      }//if
      else { //missed, and agg does NOT equal vict
         Sprintf(buf, "You miss %S with your rainbow.\n", 
                 name_of_crit(vict, agg.SEE_BIT));
         show(buf, agg);

         Sprintf(buf, 
                 "%S narrowly misses you with a rainbow blast!\n", 
                 name_of_crit(agg, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);
 
         Sprintf(buf, "misses %S with %s rainbow blast!\n", 
                    name_of_crit(vict, ~0), 
                    get_his_her(agg));
         emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
      }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1; 

   if (do_join_in_battle && !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_rainbow
 

void cast_rainbow(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = RAINBOW_SKILL_NUM;

   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to blast??\n", pc);
      return;
   }//if
   
   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if
   
   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
   
   if (!(vict = check_for_diversions(*vict, "GSM", pc)))
     return;

   do_cast_rainbow(*vict, pc, FALSE, 0);
}//cast_rainbow


void do_cast_burning_hands(critter& vict, critter& agg, int is_canned,
                      int lvl) {
   String buf(100);
   short did_hit = TRUE;
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = BURNING_HANDS_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   int lost_con = FALSE;
   if ((is_canned && (did_hit = 
                      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

      exact_raw_damage((d(2, 12) + lvl), FIRE, vict, agg);
      if (!is_canned)
        agg.MANA -= spell_mana;

      if (&vict == &agg) {
         if (vict.HP < 0) {
            show("You burn yourself to the bone!\n", agg);
            Sprintf(buf, "burns %s to death!\n",
                    get_himself_herself(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_fatality = TRUE;
         }//if
         else {
            show("You burn yourself horribly!\n", 
                 agg);
            Sprintf(buf, "burns %s with %s glowing hands.\n", 
                   get_himself_herself(vict), 
                   get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
         do_join_in_battle = FALSE;
      }//if agg == vict
      else { //agg is NOT the victim
         if (vict.HP < 0) {
            Sprintf(buf, 
                 "You find yourself holding a charred corpse!\n",
                 name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, 
                   "%S burns you to death with glowing hands!\n",
                   name_of_crit(agg, vict.SEE_BIT));
            show(buf, vict);
            Sprintf(buf, "burns %S to death with glowing hands!\n",
                    name_of_crit(vict, ~0));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
            do_fatality = TRUE;
         }//if fatality
         else { //no fatality
            Sprintf(buf, 
                    "%S howls in agony as you grab %s with your hands aflame!\n", 
                    name_of_crit(vict, agg.SEE_BIT), get_him_her(vict));
            show(buf, agg);
            Sprintf(buf, 
                    "%S's hands leave third degree burns on your body!!\n",
                    name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "burns %S with %s burning hands!\n", 
                    name_of_crit(vict, ~0), 
                    get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }//else, no fatality
      }//agg is NOT victim
   }//if all went well, else test these cases
   else if (!did_hit) { //missed
      if (!is_canned)
        agg.MANA -= spell_mana;
      if (&agg == &vict) {
         show("You evade your own hands!\n", agg);
         Sprintf(buf, "can't seem to grab %s with %s hot hands.\n", 
                 get_himself_herself(vict),
                 get_his_her(vict));
         emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         do_join_in_battle = FALSE;
      }//if
      else { //missed, and agg does NOT equal vict
         Sprintf(buf, "%S evades your grasp.\n", 
                 name_of_crit(vict, agg.SEE_BIT));
         show(buf, agg);

         Sprintf(buf, 
                 "You evade %S's burning hands!\n",
                 name_of_crit(agg, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);
 
         Sprintf(buf, "misses %S with %s burning hands!\n", 
                    name_of_crit(vict, ~0), 
                    get_his_her(agg));
         emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
      }//else did miss AND vict NOT equal to agg
   }//did_miss
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot something!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
        agg.MANA -= spell_mana / 2;
   }//else lost concentration
   else {
      mudlog.log(ERROR, "ERROR:  in default of a spell.\n");
   }//else
   agg.PAUSE += 1; 

   if (do_join_in_battle && !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(&agg, vict);
   }//if
}//do_cast_burning_hands
 

void cast_burning_hands(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = BURNING_HANDS_SKILL_NUM;

   if (victim->Strlen() == 0) {
      vict = Top(pc.is_fighting);
      if (vict && !detect(pc.SEE_BIT, vict->VIS_BIT))
         vict = NULL;
   }
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to blast??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if
   
   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if

   do_cast_burning_hands(*vict, pc, FALSE, 0);
}//cast_burning_hands








