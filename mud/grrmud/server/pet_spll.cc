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
   int spell_mana = get_mana_cost(spell_num);


   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;
   int did_hit = TRUE;

   if ((vict.LEVEL >= lvl) || (vict.pc && vict.pc->imm_data)) {
     show("You can't charm one so powerful!\n", pc);
     return;
   }//if

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
   int spell_mana = get_mana_cost(spell_num);

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
   int spell_mana = get_mana_cost(spell_num);

   if (!is_canned)
      lvl = pc.LEVEL;

   int lost_con = FALSE;

   object* corpse = NULL;
   critter* pet = NULL;

   if (!(corpse = have_obj_numbered(*(ROOM.getInv()), 1, CORPSE_OBJECT,
				    pc.SEE_BIT, ROOM))) {
      show("You need a corpse in order to animate it!\n", pc);
      return;
   }//if

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {
      
      if (!mob_list[RAISED_CORPSE_NUM].isInUse()) {
         mudlog.log(ERR, "ERROR:  need to create a RAISED_CORPSE_MOB.\n");
         return;
      }//if
      
      pet = mob_to_smob(mob_list[RAISED_CORPSE_NUM], pc.getCurRoomNum());
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

   if (!ok_to_do_action(NULL, "KMSNB", spell_num, pc)) {
     return;
   }//if

   do_cast_raise_undead(pc, FALSE, 0);
}//cast_raise_undead


void do_cast_create_golem(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CREATE_GOLEM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

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

     if (!mob_list[EARTH_GOLEM_NUMBER].isInUse()) {
       mudlog.log(ERR, "ERROR:  need to create an EARTH_GOLEM.\n");
       return;
     }//if
     
     if (pc.PETS.size() >= (pc.CHA/4 +1)) {
       show(
         "Your creature sinks back into the earth, unwilling to follow you!\n",
	 pc);
       return;
     }//if

     golem = mob_to_smob(mob_list[EARTH_GOLEM_NUMBER], pc.getCurRoomNum());
     ROOM.gainCritter(golem);
     recursive_init_loads(*golem);

     show("You create a golem of animated earth.\n", pc);
     emote("creates a golem of animated earth!", pc, 
	   ROOM, TRUE);

     /* not figure out it's strength, and HP */
     golem->HP = d(4, lvl * 5);
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

   if (!ok_to_do_action(NULL, "KMSNB", spell_num, pc)) {
     return;
   }//if

   do_cast_create_golem(pc, FALSE, 0);
}//cast_create_golem


void do_cast_create_light(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CREATE_LIGHT_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {

      if (!is_canned)
         pc.MANA -= spell_mana;
      
      if (!obj_list[CREATE_LIGHT_NUMBER].OBJ_FLAGS.get(10)) {
         mudlog.log(ERR, "ERROR:  need to create an CREATE_LIGHT object.\n");
         return;
      }//if
      
      Sprintf(buf, "You produce %S out of thin air!\n",
              long_name_of_obj(obj_list[CREATE_LIGHT_NUMBER], ~0));
      show(buf, pc);
      Sprintf(buf, "cups %s hands and produces %S!",
              get_his_her(pc),
              long_name_of_obj(obj_list[CREATE_LIGHT_NUMBER], ~0));
      emote(buf, pc, ROOM, TRUE);
      
      pc.gainInv(&(obj_list[CREATE_LIGHT_NUMBER])); 
      
      recursive_init_loads(obj_list[CREATE_LIGHT_NUMBER], 0);
      
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
   int spell_mana = get_mana_cost(spell_num);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {

      if (!is_canned)
         pc.MANA -= spell_mana;
      
      if (!obj_list[CREATE_FOOD_NUMBER].OBJ_FLAGS.get(10)) {
         mudlog.log(ERR, "ERROR:  need to create an CREATE_FOOD object.\n");
         return;
      }//if
      
      Sprintf(buf, "You grab %S out of mid air!\n",
              long_name_of_obj(obj_list[CREATE_FOOD_NUMBER], ~0));
      show(buf, pc);
      Sprintf(buf, "reaches somewhere and pulls out %S!", 
              long_name_of_obj(obj_list[CREATE_FOOD_NUMBER], ~0));
      emote(buf, pc, ROOM, TRUE);
      
      pc.gainInv(&(obj_list[CREATE_FOOD_NUMBER]));

      recursive_init_loads(obj_list[CREATE_FOOD_NUMBER], 0);
      
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
   int spell_mana = get_mana_cost(spell_num);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {

     if (!is_canned)
       pc.MANA -= spell_mana;

     if ((!obj_list[CREATE_FOOD_NUMBER].isInUse()) ||
         (!obj_list[CREATE_FOOD2_NUMBER].isInUse()) || 
         (!obj_list[CREATE_FOOD3_NUMBER].isInUse()) || 
         (!obj_list[CREATE_FOOD4_NUMBER].isInUse())) {
        pc.show("ERROR:  Tell your sysadmin, food objects not created.\n");
        mudlog.log(ERR, "ERROR:  need to create an CREATE_FOOD[x] objects.\n");
        return;
     }//if

     int rnd;
     int count = d(1, lvl/3) + 1;
     for (int i = 0; i<count; i++) {
        rnd = d(1,4);
        if (rnd == 1) {
           ROOM.gainInv(&(obj_list[CREATE_FOOD_NUMBER]));
           recursive_init_loads(obj_list[CREATE_FOOD_NUMBER], 0);
        }
        else if (rnd == 2) {
           ROOM.gainInv(&(obj_list[CREATE_FOOD2_NUMBER]));
           recursive_init_loads(obj_list[CREATE_FOOD2_NUMBER], 0);
        }
        else if (rnd == 3) {
           ROOM.gainInv(&(obj_list[CREATE_FOOD3_NUMBER]));
           recursive_init_loads(obj_list[CREATE_FOOD3_NUMBER], 0);
        }
        else if (rnd == 4) {
           ROOM.gainInv(&(obj_list[CREATE_FOOD4_NUMBER]));
           recursive_init_loads(obj_list[CREATE_FOOD4_NUMBER], 0);
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
   int spell_mana = get_mana_cost(spell_num);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = FALSE;

   critter* golem = NULL;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {
      /* create an elemental golem: earth */
      if (!is_canned)
         pc.MANA -= spell_mana;
      
      if (!mob_list[ILLUSION_NUMBER].isInUse()) {
         mudlog << "ERROR:  need to create an ILLUSION_MOB# " << ILLUSION_NUMBER << endl;
         return;
      }//if
      
      golem = mob_to_smob(mob_list[ILLUSION_NUMBER], pc.getCurRoomNum());
      ROOM.gainCritter(golem);

      recursive_init_loads(mob_list[ILLUSION_NUMBER]);
      
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
                 critter_positions[pc.getPosn()]);
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

   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_illusion(pc, FALSE, 0);
}//cast_illusion



void do_cast_conjure_minion(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CONJURE_MINION_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   if (!is_canned)
      lvl = pc.LEVEL;
   
   int lost_con = FALSE;
   
   critter* golem = NULL;
   
   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {
      /* create a random minion */
      if (!is_canned)
         pc.MANA -= spell_mana / 2;
      
      int which_un = d(1,4); //for choices
      
      switch (which_un) {
      case 1:
         which_un = MINION_ONE_NUMBER;
         break;
      case 2:
         which_un = MINION_TWO_NUMBER;
         break;
      case 3:
         which_un = MINION_THREE_NUMBER;
         break;
      case 4:
         which_un = MINION_FOUR_NUMBER;
         break;
      }//switch
      
      if (!mob_list[which_un].CRIT_FLAGS.get(10)) {
         mudlog << "ERROR: need to create MINION of mob number: "
                << which_un << endl;
         return;
      }//if

      golem = mob_to_smob(mob_list[which_un], pc.getCurRoomNum());
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

   if (!ok_to_do_action(NULL, "KMSNB", spell_num, pc)) {
     return;
   }//if

   do_cast_conjure_minion(pc, FALSE, 0);
}//cast_conjure_minion


void cast_conjure_horde(critter& pc) {
   int spell_num = CONJURE_HORDE_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNB", spell_num, pc)) {
     return;
   }//if

   do_cast_conjure_horde(pc, FALSE, 0);
}//cast_conjure_horde


void do_cast_conjure_horde(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CONJURE_HORDE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

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

     int which_un;
     int times = d(1, lvl/6);
     for (int i = 0; i<times; i++) {
       which_un = d(1,4); //for choices
     
       switch (which_un) {
       case 1:
	 which_un = MINION_ONE_NUMBER;
	 break;
       case 2:
	 which_un = MINION_TWO_NUMBER;
	 break;
       case 3:
	 which_un = MINION_THREE_NUMBER;
	 break;
       case 4:
	 which_un = MINION_FOUR_NUMBER;
	 break;
       }//switch

       if (!mob_list[which_un].CRIT_FLAGS.get(10)) {
          if (mudlog.ofLevel(ERR)) {
             mudlog << "ERROR:  need to create MINION# " << which_un << endl;
          }
          return;
       }//if

       golem = mob_to_smob(mob_list[which_un], pc.getCurRoomNum());
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
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, pc);
     emote("garbles the name of a particular pit of Hell!\n", pc, ROOM, TRUE);
     if (!is_canned)
       pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1; 
}//do_cast_summon_horde









