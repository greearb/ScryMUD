// $Id$
// $Revision: 1.9 $  $Author$ $Date$

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

#include "trv_spll.h"
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



void cast_group_heal(critter& pc) {
   int spell_num = GROUP_HEAL_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_group_heal(pc, FALSE, 0);

}//cast_group_heal



void do_cast_group_heal(critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_affects = FALSE;
   int spell_num = GROUP_HEAL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);


   if (is_canned) {
      do_affects = TRUE;
   }//if is_canned
   else { //not canned
      lvl = agg.LEVEL; 
      if (!lost_concentration(agg, spell_num)) { 
         do_affects = TRUE;
         agg.MANA -= spell_mana;
      }//if !lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
               room_list[agg.getCurRoomNum()], FALSE); 
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
      }//else lost concentration
   }//else !canned

   if (do_affects) {

      show("You heal the group.\n", agg);
      Cell<critter*> cll(room_list[agg.getCurRoomNum()].getCrits());
      critter* ptr;
      
      while ((ptr = cll.next())) {
         if (!is_grouped(*ptr, agg)) {
            continue;
         }//if
         
         ptr->HP += (d(8, (40 + lvl/2)));
         if (ptr->HP > ptr->HP_MAX)
            ptr->HP = ptr->HP_MAX;
         
         Sprintf(buf, "%S heals you and your companions.\n",
                 name_of_crit(agg, ptr->SEE_BIT));
         show(buf, *ptr);
      }//while
      agg.PAUSE += 1;   // increment pause_count
   }//if do_affects
}//do_cast_group_heal


void cast_flame_strike(critter& pc) {
   int spell_num = FLAME_STRIKE_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNV", spell_num, pc)) {
     return;
   }//if

   do_cast_flame_strike(pc, FALSE, 0);

}//cast_flame_strike



void do_cast_flame_strike(critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_affects = FALSE;
   int spell_num = FLAME_STRIKE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);


   if (is_canned) {
      do_affects = TRUE;
   }//if is_canned
   else { //not canned
      lvl = agg.LEVEL; 
      if (!lost_concentration(agg, spell_num)) { 
         do_affects = TRUE;
         agg.MANA -= spell_mana;
      }//if !lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
               room_list[agg.getCurRoomNum()], FALSE); 
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
      }//else lost concentration
   }//else !canned

   if (do_affects) {
      critter* ptr;
      room* rm = room_list.elementAt(agg.getCurRoomNum());

      rm->makeReadyForAreaSpell();

      show_all("Pillars of crimson fire spurt up from below!!\n", 
               room_list[agg.getCurRoomNum()]);

      while ((ptr = rm->findNextSpellCritter())) {
         if ((ptr == &agg) || is_grouped(*ptr, agg)) {
            continue;
         }//if
       
         if (ptr->isMob()) {
            mudlog << "ERROR:  was MOB in do_cast_flame_strike, mob: "
                   << ptr->getName() << " agg: " << agg.getName() << endl;
            continue;
         }//if

         if (TRUE) { 
            exact_raw_damage(d(4, lvl), FIRE, *ptr, agg);
            if (ptr->HP < 0) {
               Sprintf(buf, "You are consumed by %S's pillar of flames.\n",
                       name_of_crit(agg, ptr->SEE_BIT));
               show(buf, *ptr);
               Sprintf(buf, "is consumed by %S's pillar of flames!", 
                       name_of_crit(agg, ~0));
               emote(buf, *ptr, room_list[agg.getCurRoomNum()], TRUE);
               agg_kills_vict(&agg, *ptr);
            }//if
            else {
               Sprintf(buf, "You are burned by %S's pillar of flames.\n",
                       name_of_crit(agg, ptr->SEE_BIT));
               show(buf, *ptr);
               Sprintf(buf, "is burned by %S's pillar of fire!", 
                       name_of_crit(agg, ~0));
               emote(buf, *ptr, room_list[agg.getCurRoomNum()], TRUE);
               if (!HaveData(&agg, ptr->IS_FIGHTING)) {
                  join_in_battle(*ptr, agg);
               }//if
            }//else
         }//if TRUE
      }//while
      agg.PAUSE += 1;   // increment pause_count
   }//if do_affects
}//do_cast_flame_strike



void cast_meteorstorm(critter& pc) {
   int spell_num = METEORSTORM_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNV", spell_num, pc)) {
     return;
   }//if

   do_cast_meteorstorm(pc, FALSE, 0);  //does no error checking
}//cast_meteorstorm



void do_cast_meteorstorm(critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_affects = FALSE;
   int spell_num = METEORSTORM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);


   if (is_canned) {
      do_affects = TRUE;
   }//if is_canned
   else { //not canned
      lvl = agg.LEVEL; 
      if (!lost_concentration(agg, spell_num)) { 
         do_affects = TRUE;
         agg.MANA -= spell_mana;
      }//if !lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
               room_list[agg.getCurRoomNum()], FALSE); 
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
      }//else lost concentration
   }//else !canned

   if (do_affects) {
      critter* ptr;
      room* rm = room_list.elementAt(agg.getCurRoomNum());

      rm->makeReadyForAreaSpell();

      show_all("Sonic booms rend the air asunder!!\n", 
               room_list[agg.getCurRoomNum()]);

      while ((ptr = rm->findNextSpellCritter())) {
         if ((ptr == &agg) || is_grouped(*ptr, agg)) {
            continue;
         }//if
         
         
         if (ptr->isMob()) {
            mudlog << "ERROR:  was MOB in do_cast_meteor_storm, mob: "
              << ptr->getName() << " agg: " << agg.getName() << endl;
            continue;
         }//if

         if (TRUE) { 
            exact_raw_damage(d(5, lvl), NORMAL, *ptr, agg);
            if (ptr->HP < 0) {
               Sprintf(buf, "You are crushed beyond repair by %S's meteor.\n",
                       name_of_crit(agg, ptr->SEE_BIT));
               show(buf, *ptr);
               Sprintf(buf, "is crushed by %S's meteor!", 
                       name_of_crit(agg, ~0));
               emote(buf, *ptr, room_list[agg.getCurRoomNum()], TRUE);
               agg_kills_vict(&agg, *ptr);
            }//if
            else {
               Sprintf(buf, "You are struck by %S's meteor.\n",
                       name_of_crit(agg, ptr->SEE_BIT));
               show(buf, *ptr);
               Sprintf(buf, "is struck by %S's meteor!", 
                       name_of_crit(agg, ~0));
               emote(buf, *ptr, room_list[agg.getCurRoomNum()], TRUE);
               if (!HaveData(&agg, ptr->IS_FIGHTING)) {
                  join_in_battle(*ptr, agg);
               }//if
            }//else
         }//if TRUE
      }//while
      agg.PAUSE += 1;   // increment pause_count
   }//if do_affects
}//do_cast_meteorstorm



void cast_icestorm(critter& pc) {
   int spell_num = ICESTORM_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNV", spell_num, pc)) {
     return;
   }//if

   do_cast_icestorm(pc, FALSE, 0);  //does no error checking
}//cast_icestorm



void do_cast_icestorm(critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_affects = FALSE;
   int spell_num = ICESTORM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);


   if (is_canned) {
      do_affects = TRUE;
   }//if is_canned
   else { //not canned
      lvl = agg.LEVEL; 
      if (!lost_concentration(agg, spell_num)) { 
         do_affects = TRUE;
         agg.MANA -= spell_mana;
      }//if !lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
               room_list[agg.getCurRoomNum()], FALSE); 
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
      }//else lost concentration
   }//else !canned

   if (do_affects) {
      critter* ptr;
      room* rm = room_list.elementAt(agg.getCurRoomNum());

      rm->makeReadyForAreaSpell();

      show_all("Large shards of ice start falling from the sky!!\n", 
               room_list[agg.getCurRoomNum()]);

      while ((ptr = rm->findNextSpellCritter())) {
         if ((ptr == &agg) || is_grouped(*ptr, agg)) {
            continue;
         }//if

         if (ptr->isMob()) {
            mudlog << "ERROR:  was MOB in do_cast_meteor_storm, mob: "
                   << ptr->getName() << " agg: " << agg.getName() << endl;
            continue;
         }//if
         
         if (TRUE) { 
            exact_raw_damage(d(4, lvl), NORMAL, *ptr, agg);
            if (ptr->HP < 0) {
               Sprintf(buf,
                       "You are crushed beyond repair by %S's ice shard.\n",
                       name_of_crit(agg, ptr->SEE_BIT));
               show(buf, *ptr);
               Sprintf(buf, "is crushed by %S's ice shard!", 
                       name_of_crit(agg, ~0));
               emote(buf, *ptr, room_list[agg.getCurRoomNum()], TRUE);
               agg_kills_vict(&agg, *ptr);
            }//if
            else {
               Sprintf(buf, "You are struck by %S's ice shard.\n",
                       name_of_crit(agg, ptr->SEE_BIT));
               show(buf, *ptr);
               Sprintf(buf, "is struck by %S's ice shard!", 
                       name_of_crit(agg, ~0));
               emote(buf, *ptr, room_list[agg.getCurRoomNum()], TRUE);
               if (!HaveData(&agg, ptr->IS_FIGHTING)) {
                  join_in_battle(*ptr, agg);
               }//if
            }//else
         }//if TRUE
      }//while
      agg.PAUSE += 1;   // increment pause_count
   }//if do_affects
}//do_cast_icestorm



void cast_firestorm(critter& pc) {
   int spell_num = FIRESTORM_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNV", spell_num, pc)) {
      return;
   }//if

   do_cast_firestorm(pc, FALSE, 0);  //does no error checking
}//cast_firestorm



void do_cast_firestorm(critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_affects = FALSE;
   int spell_num = FIRESTORM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);


   if (is_canned) {
      do_affects = TRUE;
   }//if is_canned
   else { //not canned
      lvl = agg.LEVEL; 
      if (!lost_concentration(agg, spell_num)) { 
         do_affects = TRUE;
         agg.MANA -= spell_mana;
      }//if !lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
               room_list[agg.getCurRoomNum()], FALSE); 
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
      }//else lost concentration
   }//else !canned

   if (do_affects) {
      critter* ptr;
      room* rm = room_list.elementAt(agg.getCurRoomNum());

      rm->makeReadyForAreaSpell();

      show_all("Globs of flaming vitrol start falling from the sky!!\n", 
               room_list[agg.getCurRoomNum()]);
      
      while ((ptr = rm->findNextSpellCritter())) {
         if ((ptr == &agg) || is_grouped(*ptr, agg)) {
            continue;
         }//if
       
         if (ptr->isMob()) {
            mudlog << "ERROR:  was MOB in do_cast_fire_storm, mob: "
              << ptr->getName() << " agg: " << agg.getName() << endl;
            continue;
         }//if
         
         if (TRUE) { 
            exact_raw_damage(d(4, lvl), FIRE, *ptr, agg);
            if (ptr->HP < 0) {
               Sprintf(buf, "You are burned to a crisp by %S's fire storm.\n",
                       name_of_crit(agg, ptr->SEE_BIT));
               show(buf, *ptr);
               Sprintf(buf, "is burned to a crisp by %S's fire storm!", 
                       name_of_crit(agg, ~0));
               emote(buf, *ptr, room_list[agg.getCurRoomNum()], TRUE);
               agg_kills_vict(&agg, *ptr);
            }//if
            else {
               Sprintf(buf, "You are burned by %S's fire storm.\n",
                       name_of_crit(agg, ptr->SEE_BIT));
               show(buf, *ptr);
               Sprintf(buf, "is burned by %S's fire storm!", 
                       name_of_crit(agg, ~0));
               emote(buf, *ptr, room_list[agg.getCurRoomNum()], TRUE);
               if (!HaveData(&agg, ptr->IS_FIGHTING)) {
                  join_in_battle(*ptr, agg);
               }//if
            }//else
         }//if TRUE
      }//while
      agg.PAUSE += 1;   // increment pause_count
   }//if do_affects
}//do_cast_firestorm



void do_cast_lightning_storm(critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_affects = FALSE;
   int spell_num = LIGHTNING_STORM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   if (is_canned) {
      do_affects = TRUE;
   }//if is_canned
   else { //not canned
      lvl = agg.LEVEL; 
      if (!lost_concentration(agg, spell_num)) { 
         do_affects = TRUE;
         agg.MANA -= spell_mana;
      }//if !lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
               room_list[agg.getCurRoomNum()], FALSE); 
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
      }//else lost concentration
   }//else !canned

   if (do_affects) {
      critter* ptr;
      room* rm = room_list.elementAt(agg.getCurRoomNum());

      rm->makeReadyForAreaSpell();

      show("A huge broiling cloud gathers overhead!!\n", agg);
      
      while ((ptr = rm->findNextSpellCritter())) {
         if ((ptr == &agg) || is_grouped(*ptr, agg)) {
            continue;
         }//if
       
         if (ptr->isMob()) {
            mudlog << "ERROR:  was MOB in do_cast_lightning_storm, mob: "
                   << ptr->getName() << " agg: " << agg.getName() << endl;
            continue;
         }//if
         
         if (TRUE) { 
            emote("is toasted by a blinding bolt of lightning!", *ptr, 
                  room_list[agg.getCurRoomNum()], TRUE);
            exact_raw_damage(d(5, lvl), ELECTRICITY, *ptr, agg);
            if (ptr->HP < 0) {
               show("You are exploded by the bolt of lightning.\n", *ptr);
               emote("explodes in the bolt of lightning.", *ptr, 
                     room_list[ptr->getCurRoomNum()], TRUE);
               agg_kills_vict(&agg, *ptr);
            }//if
            else {
               show("You are fried by the bolt of lightning!\n", *ptr);
               emote("is struck by the lightning bolt.", *ptr,
                     room_list[ptr->getCurRoomNum()], TRUE);
               if (!HaveData(&agg, ptr->IS_FIGHTING)) {
                  join_in_battle(*ptr, agg);
               }//if
            }//else
         }//if TRUE
      }//while
      agg.PAUSE += 1;   // increment pause_count
   }//if do_affects
}//do_cast_lightning_storm
 

void cast_lightning_storm(critter& pc) {
   int spell_num = LIGHTNING_STORM_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNV", spell_num, pc)) {
     return;
   }//if

   do_cast_lightning_storm(pc, FALSE, 0);  //does no error checking
}//cast_lightning_storm


void do_cast_quake(critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_affects = FALSE;
   int spell_num = QUAKE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   if (is_canned) {
      do_affects = TRUE;
   }//if is_canned
   else { //not canned
      lvl = agg.LEVEL; 
      if (!lost_concentration(agg, spell_num)) { 
         do_affects = TRUE;
         agg.MANA -= spell_mana;
      }//if !lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
               room_list[agg.getCurRoomNum()], FALSE);
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
         return;
      }//else lost concentration
   }//else !canned

   if (do_affects) {
      critter* ptr;
      room* rm = room_list.elementAt(agg.getCurRoomNum());

      rm->makeReadyForAreaSpell();

      show_all("The earth heaves violently!!\n",
               room_list[agg.getCurRoomNum()]);
      
      while ((ptr = rm->findNextSpellCritter())) {
         if ((ptr == &agg) || is_grouped(*ptr, agg)) {
            continue;
         }//if

         if (ptr->isMob()) {
            mudlog << "ERROR:  was MOB in do_cast_lightning_storm, mob: "
                   << ptr->getName() << " agg: " << agg.getName() << endl;
            continue;
         }//if
         
         if (!ptr->CRIT_FLAGS.get(3)) { //if not flying
            exact_raw_damage(d(5, lvl), NORMAL, *ptr, agg);
            if (ptr->HP < 0) {
               show("You are mortally injured by the heaving earth.\n", *ptr);
               emote("is mortally injured by the heaving earth.", *ptr, 
                     room_list[ptr->getCurRoomNum()], TRUE);
               agg_kills_vict(&agg, *ptr);
            }//if
            else {
               show("You are injured by the earthquake!\n", *ptr);
               emote("is injured by the earthquake.", *ptr,
                     room_list[ptr->getCurRoomNum()], TRUE);
               if (!HaveData(&agg, ptr->IS_FIGHTING)) {
                  join_in_battle(*ptr, agg);
               }//if
            }//else
         }//if flying
         else {
            show("You fly lightly over the tossing ground.\n", *ptr);
            emote("flies clear of the heaving ground.", *ptr,
                  room_list[ptr->getCurRoomNum()], TRUE);
         }//else
      }//while
      agg.PAUSE += 1;   // increment pause_count
   }//if do_affects
}//do_cast_quake
 

void cast_quake(critter& pc) {
   int spell_num = QUAKE_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNV", spell_num, pc)) {
     return;
   }//if

   if (ROOM.isSmallWater() || ROOM.isBigWater()) {
      show("Earthquakes don't work on open water.\n", pc);
      return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_quake(pc, FALSE, 0);  //does no error checking
}//cast_quake


void do_cast_typhoon(critter& agg, int is_canned, int lvl) {
   String buf(100);
   int new_room_num = FALSE;
   short done = FALSE, do_affects = FALSE;
   int spell_num = TYPHOON_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   if (agg.getCurRoom()->isBigWater()) {
      agg.show("Your typhoon fails to find water and fizzles!\n");
      return;
   }//if

   if (is_canned) {
      do_affects = TRUE;
   }//if is_canned
   else { //not canned
      lvl = agg.LEVEL; 
      if (!lost_concentration(agg, spell_num)) { 
         do_affects = TRUE;
         agg.MANA -= spell_mana;
      }//if !lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
               room_list[agg.getCurRoomNum()], FALSE);
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
      }//else lost concentration
   }//else !canned
   
   if (do_affects) {
      critter* ptr;
      room* rm = room_list.elementAt(agg.getCurRoomNum());

      rm->makeReadyForAreaSpell();

      show_all("The skies grow dark and the wind begins to howl!\n", 
               room_list[agg.getCurRoomNum()]);
      
      short count = 0;
      
      int begin_rm, end_rm;
      int zon = room_list[agg.getCurRoomNum()].getZoneNum();
      begin_rm = ZoneCollection::instance().elementAt(zon).getBeginRoomNum();
      end_rm = ZoneCollection::instance().elementAt(zon).getEndRoomNum();

      while ((ptr = rm->findNextSpellCritter())) {
         if (ptr == &agg) {
            if (d(1,50) > 25) {
               continue;
            }//if
         }//if
       
         done = FALSE;
         int sanity = 0;
         while (!done) {

            if (sanity++ > 10)
               break;
            
            new_room_num = begin_rm + d(1, (end_rm - begin_rm));
            
            if (!room_list.elementAtNoCreate(new_room_num) ||
                (!room_list[new_room_num].hasWeather())) {
               continue;
            }
            
            if (mob_can_enter(*ptr, room_list[new_room_num], FALSE)) {
               done = TRUE;
               if ((ptr->POS == POS_PRONE) ||
                   (d(1, (ptr->CRIT_WT_CARRIED - lvl)) > 50)) {
                  break; //do nothing
               }//if
               if (ptr->isMob()) {
                  mudlog << "ERROR:  was MOB in do_cast_typhoon, mob: "
                         << ptr->getName() << " agg: " << agg.getName() << endl;
                  break;
               }//if
               
               emote("is hurled into the sky out of sight!", *ptr, 
                     room_list[agg.getCurRoomNum()], TRUE);
               
               int is_dead;
               ptr->doGoToRoom(new_room_num, NULL, NULL, is_dead,
                               ptr->getCurRoomNum(), 1);
               if (is_dead)
                  return;
               
               if (!ptr->CRIT_FLAGS.get(3)) { //if not flying
                  exact_raw_damage(abs(agg.getCurRoomNum() -
                                       ptr->getCurRoomNum()),
                                   NORMAL, *ptr, agg);
                  if (ptr->HP < 0) {
                     show("You are crushed as you are once more reunited with the earth.\n",
                          *ptr);
                     emote("is crushed into the earth.", *ptr, 
                           room_list[ptr->getCurRoomNum()], TRUE);
                     agg_kills_vict(&agg, *ptr);
                     ptr = NULL;
                  }//if
                  else {
                     show("You land painfully on the ground.\n", *ptr);
                     emote("lands painfully on the ground.", *ptr,
                           room_list[ptr->getCurRoomNum()], TRUE);
                  }//else
               }//if
               else {
                  show("You land lightly on the ground.\n", *ptr);
                  emote("lands lightly on the ground.", *ptr,
                        room_list[ptr->getCurRoomNum()], TRUE);
               }//else
               // Make sure they haven't died
               if ( ptr ) {
                  look(1, &NULL_STRING, *ptr, TRUE);
               }
            }//if
            if (count++ > 9) { //can't spin for too long
               break;
            }//if
         }//while
      }//while
      agg.PAUSE += 1;   // increment pause_count
   }//if do_affects
}//do_cast_typhoon
 

void cast_typhoon(critter& pc) {
   int spell_num = TYPHOON_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNV", spell_num, pc)) {
     return;
   }//if

   if (!ROOM.isBigWater()) {
      show("Typhoons only work on open water.\n", pc);
      return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_typhoon(pc, FALSE, 0);  //does no error checking
}//cast_typhoon



void do_cast_tornado(critter& agg, int is_canned, int lvl) {
   String buf(100);
   int new_room_num = FALSE;
   short done = FALSE, do_affects = FALSE;
   int spell_num = TORNADO_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   if (is_canned) {
      do_affects = TRUE;
   }//if is_canned
   else { //not canned
      lvl = agg.LEVEL; 
      if (!lost_concentration(agg, spell_num)) { 
         do_affects = TRUE;
         agg.MANA -= spell_mana;
      }//if !lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
               room_list[agg.getCurRoomNum()], FALSE);
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
      }//else lost concentration
   }//else !canned
   
   if (do_affects) {
      critter* ptr;
      room* rm = room_list.elementAt(agg.getCurRoomNum());

      rm->makeReadyForAreaSpell();

      show_all("The skies grow dark and the wind begins to howl!\n",
               room_list[agg.getCurRoomNum()]);
      
      int begin_rm, end_rm;
      int zon = room_list[agg.getCurRoomNum()].getZoneNum();
      begin_rm = ZoneCollection::instance().elementAt(zon).getBeginRoomNum();
      end_rm = ZoneCollection::instance().elementAt(zon).getEndRoomNum();
      
      while ((ptr = rm->findNextSpellCritter())) {
         if (ptr == &agg) {
            if (d(1,50) > 25) {
               continue;
            }//if
         }//if
         
         done = FALSE;
         int sanity = 0;
         while (!done) {
            if (sanity++ > 10) {
               break;
            }

            new_room_num = begin_rm + d(1, (end_rm - begin_rm));

            if (!room_list.elementAtNoCreate(new_room_num) ||
                (!room_list[new_room_num].hasWeather())) {
               continue;
            }

            if (mob_can_enter(*ptr, room_list[new_room_num], FALSE)) {
               done = TRUE;
               if ((ptr->POS == POS_PRONE) ||
                   (d(1, ptr->CRIT_WT_CARRIED - lvl) > 50)) {
                  break; //do nothing
               }//if
               if (ptr->isMob()) {
                  mudlog << "ERROR:  was MOB in do_cast_lightning_storm, mob: "
                         << ptr->getName() << " agg: " << agg.getName() << endl;
                  break;
               }//if

               emote("is hurled into the sky out of sight!", *ptr, 
                     room_list[agg.getCurRoomNum()], TRUE);

               int is_dead;
               ptr->doGoToRoom(new_room_num, NULL, NULL, is_dead,
                               ptr->getCurRoomNum(), 1);
               if (is_dead) {
                  return;
               }

               if (!ptr->CRIT_FLAGS.get(3)) { //if not flying
                  exact_raw_damage(abs(agg.getCurRoomNum() 
                                       - ptr->getCurRoomNum()),
                                   NORMAL, *ptr, agg);
                  if (ptr->HP < 0) {
                     show("You are crushed as you are once more reunited with the earth.\n",
                          *ptr);
                     emote("is crushed into the earth.", *ptr, 
                           room_list[ptr->getCurRoomNum()], TRUE);
                     agg_kills_vict(&agg, *ptr);
                     ptr = NULL;
                  }//if
                  else {
                     show("You land painfully on the ground.\n", *ptr);
                     emote("lands painfully on the ground.", *ptr,
                           room_list[ptr->getCurRoomNum()], TRUE);
                  }//else
               }//if
               else {
                  show("You land lightly on the ground.\n", *ptr);
                  emote("lands lightly on the ground.", *ptr,
                        room_list[ptr->getCurRoomNum()], TRUE);
               }//else
               // Make sure they haven't died
               if ( ptr ) {
                  look(1, &NULL_STRING, *ptr, TRUE);
               }
            }//if
         }//while
      }//while
      agg.PAUSE += 1;   // increment pause_count
   }//if do_affects
}//do_cast_tornado
 

void cast_tornado(critter& pc) {
   int spell_num = TORNADO_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNV", spell_num, pc)) {
     return;
   }//if

   if (ROOM.isBigWater()) {
      show("Tornado's don't work on open water.\n", pc);
      return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_tornado(pc, FALSE, 0);  //does no error checking
}//cast_tornado





