// $Id: pet_spll.cc,v 1.14 2001/04/07 22:31:55 greear Exp $
// $Revision: 1.14 $  $Author: greear $ $Date: 2001/04/07 22:31:55 $

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

///**********************  pet_spll.cc  *******************************///


#include "commands.h"
#include "command2.h"
#include <stdio.h>
#include "misc.h"
#include "misc2.h"
#include "battle.h"
#include "classes.h"
#include "spells.h"
#include "skills.h"
#include <math.h>
#include "pet_spll.h"
#include "command3.h"
#include <PtrArray.h>
#include "load_wld.h"
#include "trv_spll.h"

void do_cast_create_greater_golem(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CREATE_GREATER_GOLEM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);
   int whichgolem = d(1,4)-1;

   const char nofollow[][110] = { "Your creature sinks back into the earth, unwilling to follow you!\n",
          "Your creature appears for a moment, but then disperses in a shower of sparks.\n",
          "A small cloud forms over your head and pelts you with hail!\n",
          "Your feet burn for a moment, but nothing terribly impressive seems to happen.\n"};
   const char golemnames[][11] = { "granite", "ice", "lightning", "hellfire"};
   const int golemnums[] = {config.graniteGolemMob, config.iceGolemMob,
                            config.lightningGolemMob, config.hellfireGolemMob};
   /*  Check for: lost concentration, did_hit, !mag room    */
   /*  Do: damage to vict, mana from agg, messages to all involved, */
   /*      increment pause_count, */

   if (!is_canned)
      lvl = pc.LEVEL;

   int lost_con = FALSE;

   critter* golem = NULL;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {
      /* create an elemental golem: earth */
      if (!is_canned)
         pc.MANA -= spell_mana;
      
      if (!mob_list[config.graniteGolemMob].isInUse() || 
          !mob_list[config.iceGolemMob].isInUse() || 
          !mob_list[config.lightningGolemMob].isInUse() || 
          !mob_list[config.hellfireGolemMob].isInUse()) {
         mudlog.log(ERROR, "ERROR: Greater Golem doesn't exist.\n");
         return;
      }//if
      
      
      if (pc.PETS.size() >= (pc.CHA/4 +1)) {
         show(nofollow[whichgolem], pc);
         return;
      }//if


      
      if (d(pc.INT/2, lvl) > 25) {

	
         golem = mob_to_smob(mob_list[golemnums[whichgolem]], pc.getCurRoomNum(), TRUE);
         golem->mob->setDisolvable(TRUE);
         golem->mob->setTicksTillFreedom(pc.getCharisma() + d(1, pc.getLevel()));
         ROOM.gainCritter(golem);
         recursive_init_loads(*golem);
         
         Sprintf(buf, "You create a golem of animated %s.\n", &golemnames[whichgolem]);
         show(buf, pc);
         Sprintf(buf, "creates a golem of animated %s!", &golemnames[whichgolem]);
         emote(buf, pc, ROOM, TRUE);
         
         /* now figure out it's strength, and HP and other stuff*/
         golem->HP = d(5, lvl * 3)+200+lvl*2;
         golem->setHP_MAX(golem->HP);
         golem->STR = d(2, lvl/3) + (lvl/5);
         golem->LEVEL = pc.LEVEL;
         golem->setHomeTown(ROOM.getZoneNum()); //belong to current zone

         if( lvl > 20) {
            golem->ATTACKS = 2;
         }
         else {
            golem->ATTACKS=1;
         }

         switch (whichgolem) {
          case 0:
             golem->STR += d(2,2) + 1;
             golem->DEX = lvl/3;
             golem->CON = lvl/2 + d(2,2) + 1;
             golem->HIT = lvl/3;
             golem->DAM = lvl/4;
             golem->DAM_GIV_MOD = 100;
             golem->DAM_REC_MOD = 100-(lvl+10);
             golem->AC = 50-d(10,lvl/5);
             golem->HEAT_RESIS = 50-d(10, lvl/5);
             golem->COLD_RESIS = 50-d(10, lvl/5);
             golem->SPEL_RESIS = 50-d(10, lvl/5);
             golem->ELEC_RESIS = 50-d(10, lvl/5);
             if (lvl/10 > 10)
                golem->mob->setSkillViolence(10);
             else
                golem->mob->setSkillViolence(lvl/10);
             golem->mob->setDefensiveness(-10);
             golem->mob->setBenevolence(-10);
             golem->mob->setBadAssedness(-10);
             golem->mob->setSocialAwareness(-10);
             golem->BH_DICE_COUNT = lvl/5;
             golem->BH_DICE_SIDES = lvl/4;
             break;

          case 1:
             golem->DEX = lvl/2;
             golem->HIT = lvl/3 + d(1,2);
             golem->DAM = lvl/4+d(2,2);
             golem->DAM_GIV_MOD = 100;
             golem->DAM_REC_MOD = 90;
             golem->AC = 50-d(10,lvl/5);
             golem->HEAT_RESIS = 50-d(10,lvl/5);
             golem->COLD_RESIS = 30-d(10,lvl/5);
             golem->SPEL_RESIS = 50-d(10,lvl/5);
             golem->ELEC_RESIS = 50-d(10,lvl/5);
             if (lvl/7 > 10)
                golem->mob->setSkillViolence(10);
             else
                golem->mob->setSkillViolence(lvl/7);
             golem->mob->setDefensiveness(-10);
             golem->mob->setBenevolence(-10);
             golem->mob->setBadAssedness(-10);
             golem->mob->setSocialAwareness(-10);
             golem->BH_DICE_COUNT = lvl/5;
             golem->BH_DICE_SIDES = lvl/4;
             break;
             

          case 2:
             golem->HP -=200;
             golem->MANA = d(5, lvl * 2)+(pc.INT/2)+lvl/2;
             golem->STR -= 6 ;
             golem->DEX = lvl/2;
             golem->INT = pc.INT/3+lvl/3 + d(2,2);
             golem->WIS = pc.INT/3+lvl/3 + d(2,2);
             golem->HIT = lvl/3;
             golem->DAM = lvl/4;
             golem->DAM_GIV_MOD = 100;
             golem->DAM_REC_MOD = 100;
             golem->AC = 50-d(10,lvl/5);
             golem->HEAT_RESIS = 50-d(10,lvl/5);
             golem->COLD_RESIS = 50-d(10,lvl/5);
             golem->SPEL_RESIS = 30-d(10,lvl/5);
             golem->ELEC_RESIS = 50-d(10,lvl/5);
             if (lvl/6 > 10)
                golem->mob->setSkillViolence(10);
             else
                golem->mob->setSkillViolence(lvl/6);
             golem->mob->setDefensiveness(-10);
             golem->mob->setBenevolence(-10);
             golem->mob->setBadAssedness(-10);
             golem->mob->setSocialAwareness(-10);
             golem->BH_DICE_COUNT = lvl/6;
             golem->BH_DICE_SIDES = lvl/5;
             break;
             
          case 3:
             golem->STR += 2;
             golem->DEX = lvl/2 + d(2,2) + 1;
             golem->HIT = lvl/3+d(1,2);
             golem->DAM = lvl/4 + d(2,2);
             golem->DAM_GIV_MOD = 100 + lvl/2;
             golem->DAM_REC_MOD = 100;
             golem->AC = 50-d(10,lvl/5);
             golem->HEAT_RESIS = 30-d(10,lvl/5);
             golem->COLD_RESIS = 50-d(10,lvl/5);
             golem->SPEL_RESIS = 50-d(10,lvl/5);
             golem->ELEC_RESIS = 50-d(10,lvl/5);
             if (lvl/6 > 10)
                golem->mob->setSkillViolence(10);
             else
                golem->mob->setSkillViolence(lvl/6);
             golem->mob->setDefensiveness(-10);
             golem->mob->setBenevolence(-10);
             golem->mob->setBadAssedness(-10);
             golem->mob->setSocialAwareness(-10);
             golem->BH_DICE_COUNT = lvl/5;
             golem->BH_DICE_SIDES = lvl/4;
             break;

          default: mudlog.log(DBG, "DEBUG: Default Golem in: do_cast_greater_golem:pet_spll.cc, setting golem stats");
         }//switch 
         golem->setHP_MAX(golem->HP);
         golem->MA_MAX = golem->MANA;
         /* now it follows and is a pet of the person */
         Put(golem, pc.PETS);
         golem->MASTER = &pc;
         
         golem->doFollow(pc); // golem starts following caster
      }//if golem explosion
      else {
         //do_exploding_golem(pc, whichgolem,lvl)
         emote("loses control of the energies he has summoned!!\n", pc, ROOM, TRUE);
         show("You lose control of the energy that forms the life of the golem!!",pc);
         
         switch(whichgolem){
          case 0:
             do_cast_quake(pc, TRUE, lvl*2);
             break;
          case 1:
             do_cast_icestorm(pc, TRUE, lvl*2);             
             break;
          case 2:
             do_cast_lightning_storm(pc, TRUE, lvl*2);
             break;
          case 3:
             do_cast_firestorm(pc, TRUE, lvl*2);
             break;
          default: mudlog.log(DBG, "DEBUG: Default Golem in: do_cast_greater_golem:pet_spll.cc, exploding golem");
         }//switch whichgolem
      }//else
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
      show(LOST_CONCENTRATION_MSG_SELF, pc);
      emote("obviously forgot part of the spell!\n", pc, ROOM, TRUE);
      if (!is_canned)
         pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1;
}//do_cast_create_golem


void cast_create_greater_golem(critter& pc) {
   int spell_num = CREATE_GOLEM_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNBb", spell_num, pc)) {
      return;
   }//if

   do_cast_create_greater_golem(pc, FALSE, 0);
}//cast_create_golem


void cast_charm(int i_th, const String* vict, critter& pc) {
   critter* ptr;
   int spell_num = CHARM_SKILL_NUM;


   ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

   if (!ptr) {
      show("Who do you wish to charm?\n", pc);
      return;
   }//if

   if (ptr->isMob()) {
      ptr = mob_to_smob(*ptr, pc.getCurRoomNum(), TRUE, i_th, vict,
                        pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(ptr, "KMSNV", spell_num, pc)) {
     return;
   }//if

   do_cast_charm(*ptr, pc, FALSE, 0);
}//cast_charm


void do_cast_charm(critter& vict, critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CHARM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;
   int did_hit = TRUE;

   if ((vict.LEVEL >= lvl) || (vict.isImmort())) {
      show("You can't charm one so powerful!\n", pc);
      return;
   }//if

   // Make sure that vict is not already charmed.
   if (vict.master) {
      Sprintf(buf, "%S already has a master.\n", vict.getName(pc.SEE_BIT));
      buf.Cap();
      pc.show(buf);
      return;
   }

   if ((is_canned && (did_hit = 
                      did_spell_hit(vict, COERCION, pc, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(pc, spell_num)) && 
         (did_hit = did_spell_hit(pc, COERCION, vict)))) {
      
      if (pc.PETS.size() < (pc.CHA/4 +1)) {
         if (!is_canned)
           pc.MANA -= spell_mana;

         /* now it follows and is a pet of the person */
         Put(&vict, pc.PETS);
         vict.MASTER = &pc;

         if (vict.mob) {
            vict.mob->setTicksTillFreedom(pc.getCharisma() + d(1, pc.getLevel()));
         }

         Sprintf(buf, "You charm %S into following you!\n",
                 name_of_crit(vict, pc.SEE_BIT));
         show(buf, pc);
         Sprintf(buf, 
                 "After talking with %S for a bit, you decide you will follow %s anywhere!\n",
                 name_of_crit(pc, vict.SEE_BIT), get_him_her(pc));
         show(buf, vict);
         
         Sprintf(buf, "is overcome by %S's charm!", name_of_crit(pc, ~0));
         emote(buf, vict, room_list[vict.getCurRoomNum()], TRUE, &pc);
         
         vict.doFollow(pc); 
      }//
      else {
         pc.show("You already have all the pets you can control!\n");
         if (!is_canned)
           pc.MANA -= spell_mana;
      }
   }//if
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, pc);
      emote("seems to have forgotten part of the spell!\n", pc, ROOM,
            TRUE);
      if (!is_canned)
        pc.MANA -= spell_mana / 2;
   }//else lost concentration
   else { //didn't hit
      if (!is_canned)
        pc.MANA -= spell_mana;
      
      Sprintf(buf, "You fail to bring %S under your spell!\n", 
              name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);
      Sprintf(buf, "fails to bring %S under %s spell.",
              name_of_crit(vict, ~0), get_his_her(pc));
      emote(buf, pc, ROOM, TRUE, &vict);
      Sprintf(buf, 
              "%S tries to charm you, but you see through %s silken words.\n",
              name_of_crit(pc, vict.SEE_BIT), get_his_her(pc));
      show(buf, vict);
   }//else if didn't hit
   pc.PAUSE += 1; 
}//do_cast_charm


void cast_mass_charm(critter& pc) {
   critter* ptr;
   int spell_num = MASS_CHARM_SKILL_NUM;

   if (!ok_to_do_action(ptr, "KMSNV", spell_num, pc)) {
      return;
   }//if

   do_cast_mass_charm(ROOM, pc, FALSE, 0);
}//


void do_cast_mass_charm(room& rm, critter& pc, int is_canned,
                        int lvl) {
   String buf(100);
   int spell_num = MASS_CHARM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);

   int base_mana = pc.MANA;

   critter* ptr;
   
   List<critter*> tmp_lst(rm.getCrits());
   Cell<critter*> cll(tmp_lst);

   while ((ptr = cll.next())) {
      if (ptr != &pc) {
         if (!ptr->isPc()) {
            do_cast_charm(*ptr, pc, is_canned, lvl);
            if ((pc.MANA + spell_mana) < base_mana)
               pc.MANA = base_mana - spell_mana;
         }//if
     }//if
   }//while
}//do_cast_mass_charm


void do_cast_raise_undead(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = RAISE_UNDEAD_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);

   if (!is_canned)
      lvl = pc.LEVEL;

   int lost_con = FALSE;

   object* corpse = NULL;
   critter* pet = NULL;

   if (!(corpse = have_obj_numbered(*(ROOM.getInv()), 1, config.corpseObject,
                                    pc.SEE_BIT, ROOM))) {
      show("You need a corpse in order to animate it!\n", pc);
      return;
   }//if

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {
      
      if (!mob_list[config.walkingCorpseMob].isInUse()) {
         mudlog.log(ERROR, "ERROR:  need to create a RAISED_CORPSE_MOB.\n");
         return;
      }//if
      
      pet = mob_to_smob(mob_list[config.walkingCorpseMob], pc.getCurRoomNum(), TRUE);
      pet->mob->setDisolvable(TRUE);
      pet->mob->setTicksTillFreedom(pc.getCharisma() + d(1, pc.getLevel()));
      pet->setHomeTown(ROOM.getZoneNum()); //belong to current zone
      ROOM.gainCritter(pet);
      
      recursive_init_loads(*pet);
      
      pet->inv = corpse->inv; //transfer inventory
      corpse->inv.clear();

      ROOM.loseInv(corpse);
      corpse->decrementCurInGame(); //no recursive unload, stuff transferred

      if (corpse->IN_LIST) { //this should be true in most cases btw
         delete corpse;
      }//if
      corpse = NULL; //completely shed of it

      if (!is_canned)
         pc.MANA -= spell_mana;
      
      if (pc.PETS.size() >= (pc.CHA/4 +1)) {
         show("Your monster stands, but shows no interest in following you!\n",
              pc);
         return;
      }//if
      
      show("You breathe a twisted form of life back into the corpse!\n", pc);
      emote("conjures life back into a gruesome corpse!", pc, 
            ROOM, TRUE);
      
      /* not figure out it's strength, and HP */
      pet->HP = d(4, lvl * 4);
      pet->setHP_MAX(pet->HP);
      pet->STR = d(2,2) + (lvl / 2);
      pet->LEVEL = pc.LEVEL;
      pet->ALIGN = pc.ALIGN;
      
      /* now it follows and is a pet of the person */
      Put(pet, pc.PETS);
      pet->MASTER = &pc;

      pet->doFollow(pc); // golem starts following caster     
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
      show(LOST_CONCENTRATION_MSG_SELF, pc);
      emote("mutters something about necrophilia and blushes!\n", pc, ROOM,
            TRUE);
      if (!is_canned)
         pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1; 
}//do_cast_raise_undead
 

void cast_raise_undead(critter& pc) {
   int spell_num = RAISE_UNDEAD_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNBb", spell_num, pc)) {
      return;
   }//if

   do_cast_raise_undead(pc, FALSE, 0);
}//cast_raise_undead


void do_cast_create_golem(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CREATE_GOLEM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);
   int whichgolem = d(1,4)-1;

   const char nofollow[][77] = { "Your creature sinks back into the earth, unwilling to follow you!\n", 
          "Your creature appears for a moment, but then disperses into the atmosphere.\n",
          "A small cloud of condensation forms over your head and drenches you!\n",
          "A few hot sparks drift by, but nothing terribly impressive sems to happen.\n"};
   const char golemnames[][6] = { "earth", "water", "mist", "fire"};
   const int golemnums[] = {config.earthGolemMob, config.waterGolemMob,
                            config.airGolemMob, config.fireGolemMob};
   /*  Check for: lost concentration, did_hit, !mag room    */
   /*  Do: damage to vict, mana from agg, messages to all involved, */
   /*      increment pause_count, */

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;

   critter* golem = NULL;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {
     /* create an elemental golem: earth */
     if (!is_canned)
       pc.MANA -= spell_mana;

     if (!mob_list[config.earthGolemMob].isInUse() ||
         !mob_list[config.waterGolemMob].isInUse() ||
         !mob_list[config.airGolemMob].isInUse() ||
         !mob_list[config.fireGolemMob].isInUse()) {
        mudlog.log(ERROR, "ERROR: Greater Golem doesn't exist.\n");
        return;
     }//if
     
     if (pc.PETS.size() >= (pc.CHA/4 +1)) {
        show(nofollow[whichgolem], pc);
        return;
     }//if

     golem = mob_to_smob(mob_list[golemnums[whichgolem]], pc.getCurRoomNum(), TRUE);
     golem->mob->setDisolvable(TRUE);
     golem->mob->setTicksTillFreedom(pc.getCharisma() + d(1, pc.getLevel()));
     golem->setHomeTown(ROOM.getZoneNum()); //belong to current zone

     ROOM.gainCritter(golem);
     recursive_init_loads(*golem);

     Sprintf(buf, "You create a golem of animated %s.\n", &golemnames[whichgolem]);
     show(buf, pc);
     Sprintf(buf, "creates a golem of animated %s!", &golemnames[whichgolem]);
     emote(buf, pc, ROOM, TRUE);

     /* now figure out it's strength, and HP */
     golem->HP = d(4, lvl * 3);
     golem->setHP_MAX(golem->HP);
     golem->STR = d(2,2) + (lvl / 2);
     golem->LEVEL = pc.LEVEL;

     /* now it follows and is a pet of the person */
     Put(golem, pc.PETS);
     golem->MASTER = &pc;

     golem->doFollow(pc); // golem starts following caster     
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, pc);
     emote("obviously forgot part of the spell!\n", pc, ROOM, TRUE);
     if (!is_canned)
       pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1; 
}//do_cast_create_golem
 

void cast_create_golem(critter& pc) {
   int spell_num = CREATE_GOLEM_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNBb", spell_num, pc)) {
      return;
   }//if

   do_cast_create_golem(pc, FALSE, 0);
}//cast_create_golem


void do_cast_create_light(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CREATE_LIGHT_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {

      if (!is_canned)
         pc.MANA -= spell_mana;
      
      if (!obj_list[config.createLightObject].OBJ_FLAGS.get(10)) {
         mudlog.log(ERROR, "ERROR:  need to create an CREATE_LIGHT object.\n");
         return;
      }//if
      
      Sprintf(buf, "You produce %S out of thin air!\n",
              long_name_of_obj(obj_list[config.createLightObject], ~0));
      show(buf, pc);
      Sprintf(buf, "cups %s hands and produces %S!",
              get_his_her(pc),
              long_name_of_obj(obj_list[config.createLightObject], ~0));
      emote(buf, pc, ROOM, TRUE);
      
      pc.gainInv(&(obj_list[config.createLightObject])); 
      
      recursive_init_loads(obj_list[config.createLightObject], 0);
      
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, pc);
     emote("obviously forgot part of the spell!\n", pc, ROOM, TRUE);
     if (!is_canned)
        pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1; 
}//do_cast_create_light


void cast_create_light(critter& pc) {
   int spell_num = CREATE_LIGHT_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_create_light(pc, FALSE, 0);
}//cast_create_light


void do_cast_create_food(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CREATE_FOOD_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {

      if (!is_canned)
         pc.MANA -= spell_mana;
      
      if (!obj_list[config.createFoodObject1].OBJ_FLAGS.get(10)) {
         mudlog.log(ERROR, "ERROR:  need to create an CREATE_FOOD object.\n");
         return;
      }//if
      
      Sprintf(buf, "You grab %S out of mid air!\n",
              long_name_of_obj(obj_list[config.createFoodObject1], ~0));
      show(buf, pc);
      Sprintf(buf, "reaches somewhere and pulls out %S!", 
              long_name_of_obj(obj_list[config.createFoodObject1], ~0));
      emote(buf, pc, ROOM, TRUE);
      
      pc.gainInv(&(obj_list[config.createFoodObject1]));

      recursive_init_loads(obj_list[config.createFoodObject1], 0);
      
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
      show(LOST_CONCENTRATION_MSG_SELF, pc);
      emote("obviously forgot part of the spell!\n", pc, ROOM, TRUE);
      if (!is_canned)
         pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1; 
}//do_cast_create_food


void cast_create_food(critter& pc) {
   int spell_num = CREATE_FOOD_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_create_food(pc, FALSE, 0);
}//cast_create_food



void do_cast_heros_feast(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = HEROS_FEAST_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {

     if (!is_canned)
       pc.MANA -= spell_mana;

     if ((!obj_list[config.createFoodObject1].isInUse()) ||
         (!obj_list[config.createFoodObject2].isInUse()) || 
         (!obj_list[config.createFoodObject3].isInUse()) || 
         (!obj_list[config.createFoodObject3].isInUse())) {
        pc.show("ERROR:  Tell your sysadmin, food objects not created.\n");
        mudlog.log(ERROR, "ERROR:  need to create an CREATE_FOOD[x] objects.\n");
        return;
     }//if

     int rnd;
     int count = d(1, lvl/3) + 1;
     for (int i = 0; i<count; i++) {
        rnd = d(1,4);
        if (rnd == 1) {
           ROOM.gainInv(&(obj_list[config.createFoodObject1]));
           recursive_init_loads(obj_list[config.createFoodObject1], 0);
        }
        else if (rnd == 2) {
           ROOM.gainInv(&(obj_list[config.createFoodObject2]));
           recursive_init_loads(obj_list[config.createFoodObject2], 0);
        }
        else if (rnd == 3) {
           ROOM.gainInv(&(obj_list[config.createFoodObject3]));
           recursive_init_loads(obj_list[config.createFoodObject3], 0);
        }
        else if (rnd == 4) {
           ROOM.gainInv(&(obj_list[config.createFoodObject4]));
           recursive_init_loads(obj_list[config.createFoodObject4], 0);
        }
     }//for

     emote("causes food to fall from the sky!\n", pc, ROOM, TRUE);
     show("You cause food to fall from the sky!\n", pc);
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, pc);
     emote("obviously forgot part of the spell!\n", pc, ROOM, TRUE);
     if (!is_canned)
       pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1; 
}//do_cast_heros_feast


void cast_heros_feast(critter& pc) {
   int spell_num = HEROS_FEAST_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_heros_feast(pc, FALSE, 0);
}//cast_heros_feast


void do_cast_illusion(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = ILLUSION_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;

   critter* golem = NULL;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {
      /* create an elemental golem: earth */
      if (!is_canned)
         pc.MANA -= spell_mana;
      
      if (!mob_list[config.illusionMob].isInUse()) {
         mudlog << "ERROR:  need to create an ILLUSION_MOB# " << config.illusionMob << endl;
         return;
      }//if
      
      golem = mob_to_smob(mob_list[config.illusionMob], pc.getCurRoomNum(), TRUE);
      golem->mob->setDisolvable(TRUE);
      golem->mob->setTicksTillFreedom(pc.getCharisma() + d(1, pc.getLevel()));
      golem->setHomeTown(ROOM.getZoneNum()); //belong to current zone
      ROOM.gainCritter(golem);

      recursive_init_loads(mob_list[config.illusionMob]);
      
      show("You create an illusion of yourself.\n", pc);
      emote("shimmers and then splits into two identical beings!", pc, 
            ROOM, TRUE);
      
      /* now figure out it's strength, and HP */
      golem->HP = 1;
      golem->setHP_MAX(1);
      golem->STR = 1;
      golem->LEVEL = pc.LEVEL;
      golem->setPosn(pc.getPosn());
      golem->SEX = pc.SEX;
      golem->CLASS = pc.CLASS;
      golem->RACE = pc.RACE;
      golem->ALIGN = pc.ALIGN;
      
      golem->names.prepend(new String(*(Top(pc.names)))); 
      if (pc.pc) {
         Sprintf(buf, "%S %S", name_of_crit(pc, ~0), &(pc.short_desc));
         golem->short_desc = buf;

         Sprintf(buf, "%S %S %s\n", 
                 pc.getShortName(), &(pc.short_desc), 
                 pc.getPosnStr(pc));
         golem->in_room_desc = buf;      
      }//if
      else {
         golem->short_desc = pc.short_desc;
         golem->in_room_desc = pc.in_room_desc;
      }//

      
      golem->long_desc = pc.long_desc;
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
      show(LOST_CONCENTRATION_MSG_SELF, pc);
      emote("obviously forgot part of the spell!\n", pc, ROOM, TRUE);
      if (!is_canned)
         pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1; 
}//do_cast_illusion
 

void cast_illusion(critter& pc) {
   int spell_num = ILLUSION_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNb", spell_num, pc)) {
      return;
   }//if

   do_cast_illusion(pc, FALSE, 0);
}//cast_illusion



void do_cast_conjure_minion(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CONJURE_MINION_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);

   if (!is_canned)
      lvl = pc.LEVEL;
   
   int lost_con = FALSE;
   
   critter* golem = NULL;
   
   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {
      /* create a random minion */
      if (!is_canned)
         pc.MANA -= spell_mana;
      
      int tmp = d(1,4); //for choices
      
      int which_un = 0;
      switch (tmp) {
      case 1:
         which_un = config.minionMob1;
         break;
      case 2:
         which_un = config.minionMob2;
         break;
      case 3:
         which_un = config.minionMob3;
         break;
      case 4:
         which_un = config.minionMob4;
         break;
      }//switch
      
      if ((which_un == 0) || (!mob_list[which_un].isInUse())) {
         mudlog << "ERROR: (conjure minion) need to create MINION of mob number: "
                << which_un << endl;
         Sprintf(buf, "DB Error (conjured minion# %i is NOT inUse.\nTell an IMM to mstat it and report to Grock.\n", which_un);
         pc.show(buf);
         return;
      }//if

      golem = mob_to_smob(mob_list[which_un], pc.getCurRoomNum(), TRUE);
      golem->mob->setDisolvable(TRUE);
      golem->mob->setTicksTillFreedom(pc.getCharisma() + d(1, pc.getLevel()));
      golem->setHomeTown(ROOM.getZoneNum()); //belong to current zone
      ROOM.gainCritter(golem);

      recursive_init_loads(mob_list[which_un]);
      
      show("You summon a minion from the pits of Hell!\n", pc);
      emote("summons a minion of Hell!", pc, ROOM, TRUE);

      if (pc.PETS.size() < (pc.CHA/4 + 1) && 
          (d(1, pc.CHA * 10 + lvl * 4) > (d(1, golem->LEVEL * 4)))) {
         /* now it follows and is a pet of the person */
         Put(golem, pc.PETS);
         golem->MASTER = &pc;       
         golem->doFollow(pc); // golem starts following caster     
      }//if it worked
      else {
         Sprintf(buf, "whirls around and attacks %S with a great fury!\n",
                 name_of_crit(pc, ~0));
         emote(buf, *golem, ROOM, TRUE, &pc);
         Sprintf(buf, "Uh-oh, %S has broken free of your spell!!\n",
                 name_of_crit(*golem, pc.SEE_BIT));
         show(buf, pc);
         join_in_battle(*golem, pc);
      }//else
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
      show(LOST_CONCENTRATION_MSG_SELF, pc);
      emote("garbles the name of a minion of Hell!\n", pc, ROOM, TRUE);
      if (!is_canned)
         pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1; 
}//do_cast_conjure_minion
 

void cast_conjure_minion(critter& pc) {
   int spell_num = CONJURE_MINION_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNBb", spell_num, pc)) {
      return;
   }//if

   do_cast_conjure_minion(pc, FALSE, 0);
}//cast_conjure_minion


void cast_conjure_horde(critter& pc) {
   int spell_num = CONJURE_HORDE_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNBb", spell_num, pc)) {
      return;
   }//if

   do_cast_conjure_horde(pc, FALSE, 0);
}//cast_conjure_horde


void do_cast_conjure_horde(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CONJURE_HORDE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;

   critter* golem = NULL;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {
     /* create a random minion */
     if (is_canned)
       pc.MANA -= spell_mana / 2;

     show("You open a portal to the pits of Hell!\n", pc);
     emote("opens a portal for the minions of Hell!", pc, ROOM, TRUE);

     int which_un = 0;
     int tmp;
     int times = d(1, lvl/6) + 1;
     for (int i = 0; i<times; i++) {
       tmp = d(1,4); //for choices
     
       switch (tmp) {
       case 1:
          which_un = config.minionMob1;
          break;
       case 2:
          which_un = config.minionMob2;
          break;
       case 3:
          which_un = config.minionMob3;
          break;
       case 4:
          which_un = config.minionMob4;
          break;
       }//switch

       if ((which_un == 0) || (!mob_list[which_un].isInUse())) {
          if (mudlog.ofLevel(ERROR)) {
             mudlog << "ERROR:  (conjure horde) need to create MINION# " 
                    << which_un << endl;
          }
          Sprintf(buf, "DB Error (conjure horde# %i is NOT inUse.\nTell an IMM to mstat it and report to Grock.\n", which_un);
          pc.show(buf);
          return;
       }//if

       golem = mob_to_smob(mob_list[which_un], pc.getCurRoomNum(), TRUE);
       golem->mob->setDisolvable(TRUE);
       golem->mob->setTicksTillFreedom(pc.getCharisma() + d(1, pc.getLevel()));
       golem->setHomeTown(ROOM.getZoneNum()); //belong to current zone
       ROOM.gainCritter(golem);
       recursive_init_loads(mob_list[which_un]);

       if (pc.PETS.size() < (pc.CHA/4 + 1) && 
           (d(1, pc.CHA * 10 + lvl * 4) > (d(1, golem->LEVEL * 2)))) {
         /* now it follows and is a pet of the person */
         Put(golem, pc.PETS);
         golem->MASTER = &pc;       
         golem->doFollow(pc); // golem starts following caster     
       }//if it worked
       else {
         Sprintf(buf, "whirls around and attacks %S with a great fury!\n",
                 name_of_crit(pc, ~0));
         emote(buf, *golem, ROOM, TRUE, &pc);
         Sprintf(buf, "Uh-oh, %S has broken free of your spell!!\n",
                 name_of_crit(*golem, pc.SEE_BIT));
         show(buf, pc);
         join_in_battle(*golem, pc);
       }//else
     }//for
     if (!is_canned)
       pc.MANA -= spell_mana;
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, pc);
     emote("garbles the name of a particular pit of Hell!\n", pc, ROOM, TRUE);
     if (!is_canned)
       pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1; 
}//do_cast_summon_horde
