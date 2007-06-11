// $Id$

//
//ScryMUD Server Code
//Copyright (C) 2007  Edward Roper
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

#include "necromancer.h"
#include "skills.h"
#include "spells.h"
#include "misc.h"
#include "misc2.h"
#include "load_wld.h" //for recursive_init_loads()

void do_cast_rust(critter &vict, critter &agg, int is_canned, int lvl)
{
   String buf(100);
   int spell_num = RUST_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   int lost_con = FALSE;

   if (!is_canned)
      lvl = agg.getLevel();

   // Time to rock n' roll.
   if ( is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

      stat_spell_cell *ptr = is_affected_by(spell_num, vict);

      if ( ! is_canned )
          agg.adjMana(-spell_mana);

      if ( ptr ) {
         ptr->bonus_duration += lvl / 2;
      } else {
         vict.affected_by.append(new stat_spell_cell(spell_num, lvl));
         vict.AC += RUST_EFFECT;
          
         // Cast on self
         if ( &vict == &agg ) {
            vict.show("You have rusted your own armor.", HL_DEF);
         } else {
            Sprintf(buf, "You rust %S's armor!\n",
                  name_of_crit(vict, agg.SEE_BIT));
            agg.show(buf, HL_DEF);
            Sprintf(buf, "Your armor rusts before your very eyes!\n",
                  name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            vict.show(buf, HL_DEF);
            Sprintf(buf, "armor rusts before your very eyes.\n",
                  name_of_crit(vict, ~0));
            pemote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }
      }
   }// end of "if - it - worked"
   else { // !canned && lost concentration
      agg.show(LOST_CONCENTRATION_MSG_SELF, HL_DEF);
      Sprintf(buf, "seems to have lost %s concentration.", get_his_her(agg));
      emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
      if ( ! is_canned )
          agg.adjMana(-spell_mana/2);
   }

}

void cast_rust(int i_th, const String *victim, critter &pc)
{
   critter *vict = NULL;
   int spell_num = RUST_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      pc.show("Whose armor do you wish to rust?", HL_DEF);
      return;
   }

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th, victim, pc.SEE_BIT);
   } // if

   if (!ok_to_do_action(vict, "KMVN", spell_num, pc)) {
      return;
   }

   do_cast_rust(*vict, pc, FALSE, 0);
}

void do_cast_disfavor(critter &vict, critter &agg, int is_canned, int lvl)
{
   String buf(100);
   int spell_num = DISFAVOR_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   int lost_con = FALSE;

   if (!is_canned)
      lvl = agg.getLevel();

   // Time to rock n' roll.
   if ( is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

      stat_spell_cell *ptr = is_affected_by(spell_num, vict);

      if ( ! is_canned )
          agg.adjMana(-spell_mana);

      if ( ptr ) {
         ptr->bonus_duration += lvl / 2;
      } else {
         vict.affected_by.append(new stat_spell_cell(spell_num, lvl));
         vict.HEAT_RESIS += DISFAVOR_EFFECT;
         vict.COLD_RESIS += DISFAVOR_EFFECT;
         vict.ELEC_RESIS += DISFAVOR_EFFECT;
         vict.SPEL_RESIS += DISFAVOR_EFFECT;
          
         // Cast on self
         if ( &vict == &agg ) {
            vict.show(
                  "You call the spirits of the underworld to plague yourself.",
                  HL_DEF);
         } else {
            Sprintf(buf, "You call the spirits of the underworld to plague %S.\n",
                  name_of_crit(vict, agg.SEE_BIT));
            agg.show(buf, HL_DEF);
            Sprintf(buf, "You feel plagued by the spirits of the underworld.\n",
                  name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            vict.show(buf, HL_DEF);
            Sprintf(buf, "seems bothered.\n",
                  name_of_crit(vict, ~0));
            emote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }
      }
   }// end of "if - it - worked"
   else { // !canned && lost concentration
      agg.show(LOST_CONCENTRATION_MSG_SELF, HL_DEF);
      Sprintf(buf, "seems to have lost %s concentration.", get_his_her(agg));
      emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
      if ( ! is_canned )
          agg.adjMana(-spell_mana/2);
   }

}

void cast_disfavor(int i_th, const String *victim, critter &pc)
{
   critter *vict = NULL;
   int spell_num = DISFAVOR_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      pc.show("Who do you wish to plague?", HL_DEF);
      return;
   }

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th, victim, pc.SEE_BIT);
   } // if

   if (!ok_to_do_action(vict, "KMVN", spell_num, pc)) {
      return;
   }

   do_cast_disfavor(*vict, pc, FALSE, 0);
}

void do_cast_remove_soul(critter &vict, critter &agg, int is_canned, int lvl)
{
   String buf(100);
   int spell_num = REMOVE_SOUL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   int lost_con = FALSE;

   if (!is_canned)
      lvl = agg.getLevel();

   // Time to rock n' roll.
   if ( is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

      stat_spell_cell *ptr = is_affected_by(spell_num, vict);

      if ( ! is_canned )
          agg.adjMana(-spell_mana);

      if ( ptr ) {
         ptr->bonus_duration += lvl / 2;
      } else {
         vict.affected_by.append(new stat_spell_cell(spell_num, lvl));
         vict.SPEL_RESIS += REMOVE_SOUL_EFFECT;
          
         // Cast on self
         if ( &vict == &agg ) {
            vict.show(
                  "Ok.",
                  HL_DEF);
         } else {
            Sprintf(buf,
                  "You cause %S to scream in agony as their soul wretches.\n",
                  name_of_crit(vict, agg.SEE_BIT));
            agg.show(buf, HL_DEF);
            Sprintf(buf,
                  "You scream in agony as your soul wretches.\n",
                  name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            vict.show(buf, HL_DEF);
            Sprintf(buf, "screams in agony as %s soul wretches.\n",
                  get_his_her(vict));
            emote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }
      }
   }// end of "if - it - worked"
   else { // !canned && lost concentration
      agg.show(LOST_CONCENTRATION_MSG_SELF, HL_DEF);
      Sprintf(buf, "seems to have lost %s concentration.", get_his_her(agg));
      emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
      if ( ! is_canned )
          agg.adjMana(-spell_mana/2);
   }

}

void cast_remove_soul(int i_th, const String *victim, critter &pc)
{
   critter *vict = NULL;
   int spell_num = REMOVE_SOUL_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      pc.show("Who's soul do you wish to remove?", HL_DEF);
      return;
   }

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th, victim, pc.SEE_BIT);
   } // if

   if (!ok_to_do_action(vict, "KMVN", spell_num, pc)) {
      return;
   }

   do_cast_remove_soul(*vict, pc, FALSE, 0);
}

void do_cast_remove_hope(critter &vict, critter &agg, int is_canned, int lvl)
{
   String buf(100);
   int spell_num = REMOVE_HOPE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   int lost_con = FALSE;

   if (!is_canned)
      lvl = agg.getLevel();

   // Time to rock n' roll.
   if ( is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

      stat_spell_cell *ptr = is_affected_by(spell_num, vict);

      if ( ! is_canned )
          agg.adjMana(-spell_mana);

      // Dont let the duration accumulate.
      if ( ptr ) {
         agg.show("You can do no more.", HL_DEF);
      } else {
         // Hardcoded in this lasts for 4 ticks.
         vict.affected_by.append(new stat_spell_cell(spell_num, 4));
          
         // Cast on self
         if ( &vict == &agg ) {
            vict.show("You become hopeless.", HL_DEF);
         } else {
            Sprintf(buf, "You are overcome with a feeling of hopelessness.\n");
            vict.show(buf, HL_DEF);
            Sprintf(buf,
                  "quivers and drops to %s knees with a frail cry of hopelessness.\n",
                  get_his_her(vict));
            emote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }
      }
   }// end of "if - it - worked"
   else { // !canned && lost concentration
      agg.show(LOST_CONCENTRATION_MSG_SELF, HL_DEF);
      Sprintf(buf, "seems to have lost %s concentration.", get_his_her(agg));
      emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
      if ( ! is_canned )
          agg.adjMana(-spell_mana/2);
   }

}

void cast_remove_hope(int i_th, const String *victim, critter &pc)
{
   critter *vict = NULL;
   int spell_num = REMOVE_HOPE_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      pc.show("Who do you want to psychologically destroy?", HL_DEF);
      return;
   }

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th, victim, pc.SEE_BIT);
   } // if

   if (!ok_to_do_action(vict, "KMVN", spell_num, pc)) {
      return;
   }

   do_cast_remove_hope(*vict, pc, FALSE, 0);
}

void do_cast_remove_karma(critter &vict, critter &agg, int is_canned, int lvl)
{
   String buf(100);
   int spell_num = REMOVE_KARMA_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   int lost_con = FALSE;

   if (!is_canned)
      lvl = agg.getLevel();

   // Time to rock n' roll.
   if ( is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

      stat_spell_cell *ptr = is_affected_by(spell_num, vict);

      if ( ! is_canned )
          agg.adjMana(-spell_mana);

      // Dont let the duration accumulate.
      if ( ptr ) {
         agg.show("You can do no more.", HL_DEF);
      } else {
         // Hardcoded in this lasts for 4 ticks.
         vict.affected_by.append(new stat_spell_cell(spell_num, 4));
          
         // Cast on self
         if ( &vict == &agg ) {
            vict.show("You remove your ability to gain mana.", HL_DEF);
         } else {
            Sprintf(buf, "You feel a cold sensation as your skin glows briefly.\n");
            vict.show(buf, HL_DEF);
            Sprintf(buf, "glows a faint blue and then returns to normal.\n");
            emote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &vict);
         }
      }
   }// end of "if - it - worked"
   else { // !canned && lost concentration
      agg.show(LOST_CONCENTRATION_MSG_SELF, HL_DEF);
      Sprintf(buf, "seems to have lost %s concentration.", get_his_her(agg));
      emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
      if ( ! is_canned )
          agg.adjMana(-spell_mana/2);
   }

}

void cast_remove_karma(int i_th, const String *victim, critter &pc)
{
   critter *vict = NULL;
   int spell_num = REMOVE_KARMA_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      pc.show("Who do you to mentally destroy?", HL_DEF);
      return;
   }

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th, victim, pc.SEE_BIT);
   } // if

   if (!ok_to_do_action(vict, "KMVN", spell_num, pc)) {
      return;
   }

   do_cast_remove_karma(*vict, pc, FALSE, 0);
}

void do_cast_sanctum_of_the_victim(critter &vict, critter &agg, int is_canned, int lvl)
{
   String buf(100);
   int spell_num = SANCTUM_OF_THE_VICTIM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   int lost_con = FALSE;

   if (!is_canned)
      lvl = agg.getLevel();

   // Time to rock n' roll.
   if ( is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

      stat_spell_cell *ptr = is_affected_by(spell_num, vict);

      if ( ! is_canned )
          agg.adjMana(-spell_mana);

      if ( ptr ) {
         ptr->bonus_duration += 1;
      } else {
         vict.affected_by.append(new stat_spell_cell(spell_num, lvl));
         vict.DAM_REC_MOD += SANCTUM_EFFECT;
          
         // Cast on self
         if ( &vict == &agg ) {
            vict.show("You have made yourself frail.", HL_DEF);
         } else {
            Sprintf(buf, "You make %S frail!\n",
                  name_of_crit(vict, agg.SEE_BIT));
            agg.show(buf, HL_DEF);
            Sprintf(buf, "You feel frail!\n",
                  name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            vict.show(buf, HL_DEF);
         }
      }
   }// end of "if - it - worked"
   else { // !canned && lost concentration
      agg.show(LOST_CONCENTRATION_MSG_SELF, HL_DEF);
      Sprintf(buf, "seems to have lost %s concentration.", get_his_her(agg));
      emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
      if ( ! is_canned )
          agg.adjMana(-spell_mana/2);
   }

}

void cast_sanctum_of_the_victim(int i_th, const String *victim, critter &pc)
{
   critter *vict = NULL;
   int spell_num = SANCTUM_OF_THE_VICTIM_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      pc.show("Who is your victim?", HL_DEF);
      return;
   }

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th, victim, pc.SEE_BIT);
   } // if

   if (!ok_to_do_action(vict, "KMVN", spell_num, pc)) {
      return;
   }

   do_cast_sanctum_of_the_victim(*vict, pc, FALSE, 0);
}

void do_cast_fear(critter &vict, critter &agg, int is_canned, int lvl)
{
   String buf(100);
   int spell_num = FEAR_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   int lost_con = FALSE;

   if (!is_canned)
      lvl = agg.getLevel();

   // Time to rock n' roll.
   if ( is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

      if ( ! is_canned )
          agg.adjMana(-spell_mana);

      vict.MOV = 0;

      // Cast on self
      if ( &vict == &agg ) {
         vict.show("You idiot, now you can't move.", HL_DEF);
      } else {
         Sprintf(buf, "You put fear into the heart of %S\n",
               name_of_crit(vict, agg.SEE_BIT));
         agg.show(buf, HL_DEF);
         Sprintf(buf, "Your face turns a pale white. You are afraid.\n",
               name_of_crit(agg, vict.SEE_BIT));
         buf.Cap();
         vict.show(buf, HL_DEF);
         Sprintf(buf, "face turns a pale white.\n");
         pemote(buf, vict, room_list[agg.getCurRoomNum()], TRUE, &vict);
      }
   }// end of "if - it - worked"
   else { // !canned && lost concentration
      agg.show(LOST_CONCENTRATION_MSG_SELF, HL_DEF);
      Sprintf(buf, "seems to have lost %s concentration.", get_his_her(agg));
      emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
      if ( ! is_canned )
          agg.adjMana(-spell_mana/2);
   }

}

void cast_fear(int i_th, const String *victim, critter &pc)
{
   critter *vict = NULL;
   int spell_num = FEAR_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      pc.show("In whom do you wish to instill fear?", HL_DEF);
      return;
   }

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th, victim, pc.SEE_BIT);
   } // if

   if (!ok_to_do_action(vict, "KMVN", spell_num, pc)) {
      return;
   }

   do_cast_fear(*vict, pc, FALSE, 0);
}

void do_cast_weaken(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short did_hit = TRUE;
   int spell_num = WEAKEN_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, agg);

   int lost_con = FALSE;

   if (!is_canned) {
     lvl = agg.getLevel();
   }

   if ((is_canned && (did_hit = 
                      did_spell_hit(agg, CRONIC, vict, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, CRONIC, vict)))) {

     stat_spell_cell* ptr = is_affected_by(spell_num, vict);
     
     if (!is_canned) {
         agg.adjMana(-spell_mana);
     }

     if (ptr) {
       ptr->bonus_duration += d(1,3);
       agg.show("Ok.\n");
     }//if
     else {
       // -10% of unmodified strength for 1-5 ticks.
       int weaken_by_value = (int)( - ( 0.10 * vict.getSTR(false) ) );
       vict.affected_by.append(new stat_spell_cell(spell_num, d(1,5), weaken_by_value ) );
       vict.STR += weaken_by_value;

       if (vict.pc) {
          if (vict.STR > 20) {
             if (d(1,13) == 5) { //ouch, lost some CON!!
                vict.show("You feel even worse than normal!\n");
                vict.STR--;
             }
          }//if
          else if (vict.DEX > 20) {
             if (d(1,13) == 7) { //ouch, lost some CON!!
                vict.show("You feel even worse than normal!\n");
                vict.DEX--;
             }
          }//if
          else if (vict.CON > 20) {
             if (d(1,13) == 7) { //ouch, lost some DAMCON!!
                vict.show("You feel even worse than normal!\n");
                vict.CON--;
             }
          }//if
       }


       if (&vict == &agg) {
         show("You feel weaker!\n", agg);
       }//if
       else {
         Sprintf(buf, "You drain some of %S's strength!\n", 
                 name_of_crit(vict, agg.SEE_BIT));
         show(buf, agg);
         Sprintf(buf, "%S drains your strength!\n", 
                 name_of_crit(agg, vict.SEE_BIT));
         buf.Cap();
         show(buf, vict);
         Sprintf(buf, "drains %S's strength!",
                 name_of_crit(vict, ~0));
         emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else if (!did_hit) {
     Sprintf(buf, "You fail to drain some of %S's strength!\n", 
             name_of_crit(vict, agg.SEE_BIT));
     show(buf, agg);
     Sprintf(buf, "%S fails to drain your strength!\n", 
             name_of_crit(agg, vict.SEE_BIT));
     buf.Cap();
     show(buf, vict);
     Sprintf(buf, "fails to drain %S's strength!",
             name_of_crit(vict, ~0));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
   }//did_hit
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("feels a little weak in the head!", agg, 
           room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
         agg.adjMana(-spell_mana/2);
   }//else lost concentration
   agg.PAUSE = 1; 
}//do_cast_weaken

void cast_weaken(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = WEAKEN_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to weaken??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if
   
   if (!ok_to_do_action(vict, "KMSVN", spell_num, pc)) {
     return;
   }//if

   do_cast_weaken(*vict, pc, FALSE, 0);
}//do_cast_weaken

void do_cast_raise_undead(critter& pc, int is_canned, int lvl) {
   String buf(100);
   int spell_num = RAISE_UNDEAD_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num, pc);

   if (!is_canned)
      lvl = pc.getLevel();

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
          pc.adjMana(-spell_mana);
      
      if (pc.PETS.size() >= (pc.CHA/4 +1)) {
         show("Your monster stands, but shows no interest in following you!\n",
              pc);
         return;
      }//if
      
      show("You breathe a twisted form of life back into the corpse!\n", pc);
      emote("conjures life back into a gruesome corpse!", pc, 
            ROOM, TRUE);
      
      /* not figure out it's strength, and HP */
      pet->setHP(d(4, lvl * 4));
      pet->setHP_MAX(pet->HP);
      pet->STR = d(2,2) + (lvl / 2);
      pet->setLevel(pc.getLevel());
      pet->ALIGN = pc.getAlignment();
      
      /* now it follows and is a pet of the person */
      pc.PETS.append(pet);
      pet->MASTER = &pc;

      pet->doFollow(pc); // golem starts following caster     
   }//if canned or didn't lose concentration
   else { //not canned AND lost concentration
      show(LOST_CONCENTRATION_MSG_SELF, pc);
      emote("mutters something about necrophilia and blushes!\n", pc, ROOM,
            TRUE);
      if (!is_canned)
         pc.adjMana(-(spell_mana/2));
   }//else lost concentration
   pc.PAUSE = 1; 
}//do_cast_raise_undead

void cast_raise_undead(critter& pc) {
   int spell_num = RAISE_UNDEAD_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSNBb", spell_num, pc)) {
      return;
   }//if

   do_cast_raise_undead(pc, FALSE, 0);
}//cast_raise_undead

void cast_blood_ritual(critter& agg) {
// Spell: Blood Ritual
//
// This spell converts 100% of the casters mana to hitpoints. If necessary,
// MAX_HP is temporarily increased for the duration of the spell.
//
// Misc: this spell severely lowers mana regeneration.
//
    int spell_num = BLOOD_RITUAL_SKILL_NUM;
    int mana_cost = get_mana_cost(spell_num, agg);

    if (!ok_to_do_action(NULL, "KMSN", spell_num, agg)) {
        return;
    }

    // Can't use again until the original debuff wears off.
    stat_spell_cell *ss_ptr = is_affected_by(spell_num, agg);
    if ( ss_ptr ) {
        agg.show("You can't do that yet.\n");
        return;
    }

    bool lost_con = agg.lost_concentration(spell_num);
    if ( lost_con ) {
      agg.show(LOST_CONCENTRATION_MSG_SELF);
      agg.adjMana(-mana_cost/2);
      return;
    }

    agg.adjMana(-mana_cost);

    int adj_value = agg.getMana();
    agg.adjHP(adj_value);
    agg.setMana(0);

    int max_hp = agg.getHP_MAX();
    int cur_hp = agg.getHP();

    if ( max_hp < cur_hp ) {
        adj_value = cur_hp - max_hp;
    } else {
        adj_value = 0;
    }

    agg.adjHP_MAX(adj_value);
    agg.affected_by.append(new stat_spell_cell(spell_num, 2, adj_value));

    agg.show("You emanate a deep red light as your magical energy flows into your blood.\n");

    String buf(100);
    Sprintf(buf,"emanates a deep red light as %s magical energy flows into %s blood.",
           get_his_her(agg), get_his_her(agg));
    agg.emote(buf);

}//cast_blood_ritual()

void cast_spirit_ritual(critter& agg) {
// Spell: Spirit Ritual
//
// This spell converts 50% of the casters maximum hitpoints to maximum mana.
//
    int spell_num = SPIRIT_RITUAL_SKILL_NUM;
    int mana_cost = get_mana_cost(spell_num, agg);

    if (!ok_to_do_action(NULL, "KMSN", spell_num, agg)) {
        return;
    }

    // Can't use again until the original debuff wears off.
    stat_spell_cell *ss_ptr = is_affected_by(spell_num, agg);
    if ( ss_ptr ) {
        agg.show("You can't do that yet.\n");
        return;
    }

    bool lost_con = agg.lost_concentration(spell_num);
    if ( lost_con ) {
      agg.show(LOST_CONCENTRATION_MSG_SELF);
      agg.adjMana(-mana_cost/2);
      return;
    }

    agg.adjMana(-mana_cost);

    int adj_value = int(agg.getHP_MAX()/2.0);
    agg.adjHP_MAX(-adj_value);
    agg.adjManaMax(adj_value);

    if ( agg.getHP() > agg.getHP_MAX() ) {
        agg.setHP(agg.getHP_MAX());
    }

    agg.affected_by.append(new stat_spell_cell(spell_num,int(agg.getLevel()/1.5), adj_value));

    agg.show("The world grows dim as black specks dance in your vision.\n");

    String buf(100);
    agg.pemote("eyes come to life as black specks begin an intricate within them.");

}//cast_spirit_ritual()

void cast_stamina_ritual(critter& agg) {
// Spell: Stamina Ritual
//
// This spell increases the casters mov max by 1.5x, at the expense of mov
// regen.
//
    int spell_num = STAMINA_RITUAL_SKILL_NUM;
    int mana_cost = get_mana_cost(spell_num, agg);

    if (!ok_to_do_action(NULL, "KMSN", spell_num, agg)) {
        return;
    }

    // Can't use again until the original debuff wears off.
    stat_spell_cell *ss_ptr = is_affected_by(spell_num, agg);
    if ( ss_ptr ) {
        agg.show("You can't do that yet.\n");
        return;
    }

    bool lost_con = agg.lost_concentration(spell_num);
    if ( lost_con ) {
      agg.show(LOST_CONCENTRATION_MSG_SELF);
      agg.adjMana(-mana_cost/2);
      return;
    }

    agg.adjMana(-mana_cost);

    int adj_value = int(agg.getMovMax()/2.0);
    agg.adjMovMax(adj_value);
    agg.adjMov(adj_value);

    agg.affected_by.append(new stat_spell_cell(spell_num,int(agg.getLevel()/1.5), adj_value));

    agg.show("Your heart begins to race.\n");

    String buf(100);
    agg.emote("looks eerily tense.");

}//cast_stamina_ritual()
