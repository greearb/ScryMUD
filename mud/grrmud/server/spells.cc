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

/* spells.cc:  My first try at coding spells.  These are quite messy
    code-wise, but served as templates to second generation spells
    (see the other files) */

#include "commands.h"
#include "command2.h"
#include "command3.h"
#include <stdio.h>
#include "misc.h"
#include "misc2.h"
#include "battle.h"
#include "classes.h"
#include "spells.h"
#include "skills.h"
#include "ez_spll.h"
#include "spells2.h"
#include "dam_spll.h"
#include "pet_spll.h"
#include "rm_spll.h"
#include "trv_spll.h"
#include <PtrArray.h>
#include "SkillSpell.h"


int get_mana_cost(int spell_num) {
   return SSCollection::instance().getSS(spell_num).getManaCost();
}


void rem_effects_crit(int spell_num, critter &pc, short do_msg) {
   String buf(100);

   if (spell_num == ARMOR_SKILL_NUM) {  
     pc.AC += 30;  //increases pc's ac by 30.
     if (do_msg)
       show("You feel more vulnerable.\n", pc);
   }//if
   else if (spell_num == MAGIC_SHIELD_SKILL_NUM) {
     if (do_msg)
       show("Your magical shield dissapates.\n", pc);
   }//if
   else if (spell_num == POISON_SKILL_NUM) {
     pc.HP_REGEN -= POISON_HP_REGEN_AUGMENTATION;
     pc.MA_REGEN -= POISON_MA_REGEN_AUGMENTATION;
     pc.MV_REGEN -= POISON_MV_REGEN_AUGMENTATION;
     if (do_msg)
       show("Your bowels no longer scream in pain!", pc);
   }//if
   else if (spell_num == WEB_SKILL_NUM) {
     pc.MV_REGEN -= WEB_MV_REGEN_AUGMENTATION;
     pc.DEX -= WEB_DEX_AUGMENTATION;
     if (do_msg)
       show("You finally work your way out of the web.\n", pc);
   }//if
   else if (spell_num == SHADOWS_BLESSING_SKILL_NUM) {
     pc.AC -= SHADOWS_BLESSING_EFFECT;
     if (do_msg)
       show("Your protective shadows fade away.\n", pc);
   }//if
   else if (spell_num == DETECT_MAGIC_SKILL_NUM) {
     if (pc.pc) {
       pc.PC_FLAGS.turn_off(18);
       if (do_msg)
	 show("You finally work your way out of the web.\n", pc);
     }//if
   }//if
   else if (spell_num == DETECT_HIDDEN_SKILL_NUM) {
     pc.VIS_BIT &= ~(4);
     if (do_msg)
       show("You no longer detect hidden things.\n", pc);
   }//if
   else if (spell_num == DETECT_INVISIBILITY_SKILL_NUM) {
     pc.VIS_BIT &= ~(2);
     if (do_msg)
       show("You no longer detect invisible things.\n", pc);
   }//if
   else if (spell_num == WEAKEN_SKILL_NUM) {
     pc.STR -= WEAKEN_EFFECT;
     if (do_msg)
       show("You feel stronger!\n", pc);
   }//if
   else if (spell_num == SLEEP_SKILL_NUM) {
     pc.CRIT_FLAGS.turn_off(15);
     wake(pc);
   }//if
   else if (spell_num == FLY_SKILL_NUM) {
     pc.CRIT_FLAGS.turn_off(3);
     lose_fly(pc, do_msg); //more dire effects??
   }//if
   else if (spell_num == ENTANGLE_SKILL_NUM) {
     pc.MV_REGEN -= ENTANGLE_MV_REGEN_AUGMENTATION;
     pc.DEX -= ENTANGLE_DEX_AUGMENTATION;
     if (do_msg)
       show("You finally work your way out of the entanglement.\n", pc);
   }//if
   else if (spell_num == FIREPROOF_SKILL_NUM) {
     pc.HEAT_RESIS -= FIRE_PROOF_EFFECT;
     if (do_msg)
       show("You feel less protected from heat.\n", pc);
   }//if
   else if (spell_num == STRENGTH_SKILL_NUM) {
     pc.STR -= STRENGTH_EFFECT;
     if (do_msg)
       show("You feel weaker.\n", pc);
   }//if
   else if (spell_num == HASTE_SKILL_NUM) {
     pc.ATTACKS--;
     if (do_msg)
       show("You lose a little quickness.\n", pc);
   }//if
   else if (spell_num == PFE_SKILL_NUM) {
     pc.CRIT_FLAGS.turn_off(24);
     if (do_msg)
       show("You no longer feel so protected.\n", pc);
   }//if
   else if (spell_num == PFG_SKILL_NUM) {
     pc.CRIT_FLAGS.turn_off(25);
     if (do_msg)
       show("You no longer feel so protected.\n", pc);
   }//if
   else if (spell_num == DIVINE_PROTECTION_SKILL_NUM) {
     pc.AC -= DIVINE_PROTECTION_EFFECT;
     if (do_msg)
       show("A divine force quits protecting you.\n", pc);
   }//if
   else if (spell_num == STONE_SKIN_SKILL_NUM) {
     pc.DEX -= STONE_SKIN_EFFECT_DEX;
     pc.DAM_REC_MOD -= STONE_SKIN_EFFECT_DRM;
     if (do_msg)
       show("Your skin softens.\n", pc);
   }//if
   else if (spell_num == ARMOR_SKILL_NUM) {
     pc.AC -= ARMOR_EFFECT;
     if (do_msg)
       show("You feel less protected from physical threats.\n", pc);
   }//if
   else if (spell_num == BLINDNESS_SKILL_NUM) {
     pc.SEE_BIT |= 1024;
     if (do_msg)
       show("You a vail of darkness is lifted from thine eyes.\n", pc);
   }//if
   else if (spell_num == ABSORB_BLOWS_SKILL_NUM) {
     if (do_msg)
       show("Your interdimensional lattice frays and falls apart.\n", pc);
   }//if
   else if (spell_num == PRISMATIC_GLOBE_SKILL_NUM) {
     pc.AC -= PRISMATIC_GLOBE_EFFECT_AC;
     pc.MOV -= PRISMATIC_GLOBE_EFFECT_MOV;
     if (do_msg)
       show("Your prismatic shield folds in upon itself and dissappears.\n",
	    pc);
   }//if
   else if (spell_num == SANCTUARY_SKILL_NUM) {
     pc.DAM_REC_MOD -= SANCTUARY_EFFECT_DRM;
     if (do_msg)
       show("Your protective light fades.\n", pc);
   }//if
   else if (spell_num == BIND_WOUND_SKILL_NUM) {
     pc.HP_REGEN -= BIND_WOUND_EFFECT;
     if (do_msg)
       show("Your bandage wears away.\n", pc);
   }//if
   else if ((spell_num == CHARM_SKILL_NUM) || 
	    (spell_num == MASS_CHARM_SKILL_NUM)) {
     follow(1, &NULL_STRING, pc, do_msg);
   }//if
   else if (spell_num == INVISIBILITY_SKILL_NUM) {
      pc.show("Your cloak of invisibility fades away.\n");
      pc.VIS_BIT &= ~2;
   }//if
   else {
      Sprintf(buf, "ERROR:  rem_effects_crit DEFAULT: spll# [%i] %s.\n",
              spell_num,
              (const char*)(SSCollection::instance().getNameForNum(spell_num)));
      mudlog.log(ERR, buf);
   }//switch
}// rem_effects_crit()


void rem_effects_obj(int spell_num, object &obj) {
  String buf(100);

  if (spell_num == RUNE_EDGE_SKILL_NUM) {
     obj.OBJ_DAM_DICE_SIDES -= RUNE_EDGE_EFFECTS;
  }//if
  else if ((spell_num == FROST_BLADE_SKILL_NUM) ||
	   (spell_num == FIRE_BLADE_SKILL_NUM)) {
    ; //do nothing, losing spell is enough
  }//if
  else {
    Sprintf(buf, "ERROR:  rem_effects_obj, not supported, spll# [%i] %s.\n",
	    spell_num, (const char*)(SSCollection::instance().getNameForNum(spell_num)));
    mudlog.log(ERR, buf);
  }//else
}// rem_effects_obj()


void rem_effects_door(int spell_num, door& dr, room& rm1,
		      room& rm2, short do_msg) {
  String buf(100);
  
  if (spell_num == FIREWALL_SKILL_NUM) {
     if (do_msg) {
        show_all("The flames on the exit die down.\n", rm1);
        show_all("The flames on the exit die down.\n", rm2);
     }//if
  }//if
  else if (spell_num == DISTORTION_WALL_SKILL_NUM) {
    if (do_msg) {
      show_all("The distortion field fades.\n", rm1);
      show_all("The distortion field fades.\n", rm2);
    }//if
  }//if
  else {
     Sprintf(buf, "ERROR:  rem_effects_door, spll# [%i] %s.\n",
	     spell_num, 
             (const char*)(SSCollection::instance().getNameForNum(spell_num)));
     mudlog.log(ERR, buf);
  }//else
}// rem_effects_door()
 

void rem_effects_room(int spell_num, room &rm, short do_msg) {
   String buf(100);

   if (spell_num == ILLUMINATE_SKILL_NUM) {
     rm.checkLight(do_msg);
   }//if
   else {
     Sprintf(buf, 
	     "ERROR:  spell_num(%i) out of range in rem_effects_room.\n",
             spell_num);
     mudlog.log(ERR, buf);
   }//else
}// rem_effects_room()


void do_firewall_effects(critter& pc, int& is_dead) {
   String buf(100);
   is_dead = FALSE;

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_firewall_effects.\n");
      return;
   }//if

   exact_raw_damage(d(10,5), FIRE, pc);

   if (pc.HP < 0) {
      emote("is burned to death!!", pc, ROOM, TRUE);
      show("You have been burned to death!\n", pc);
      crit_dies(pc);
      is_dead = TRUE;
   }//if
   else {
      is_dead = FALSE;
      show("OUCH!!  You are singed as you enter.\n", pc);
      emote("momentarily catches on fire!", pc, ROOM, TRUE);
   }//else
}//do_firewall_effects(critter&


void do_distortion_wall_effects(critter& pc, int& is_dead) {
   String buf(100);
   is_dead = FALSE;

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_distortion_wall_effects.\n");
      return;
   }//if

   show("WOW, as you step into the next room you encounter a strange silvery\
curtain of....energy!", pc);
   emote("blinks out of sight!", pc, ROOM, TRUE);
   relocate_within_zone(pc); //silent teleport
   emote("materializes in front of you!", pc, ROOM, TRUE); //in new room now
}//do_distortion_wall_effects


void do_cast_fireball(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_join_in_battle = TRUE;
   short do_fatality = FALSE;
   int spell_num = FIREBALL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   /*  Check for: lost concentration, did_hit, !mag room    */
   /*  Do: damage to vict, mana from agg, messages to all involved, */
   /*      increment pause_count, */

   if (room_list[agg.getCurRoomNum()].isHaven()) {
      show(NOT_IN_HAVEN_MSG, agg);
      return;
   }//if

   if ((room_list[agg.getCurRoomNum()].isNoPK()) && 
       (vict.pc && agg.pc)) { 
      show(NO_PK_HERE_MSG, agg);
      return;
   }//if

   if (is_canned) {
      if (did_spell_hit(vict, FIRE, agg, lvl, TRUE)) {
         vict.HP -= d(4, 20) + 
                lvl - (int)((100.0 / (vict.HEAT_RESIS + 100)) * 20);
         agg.PAUSE += 1;   // increment pause_count
         if (&vict == &agg) {
            if (vict.HP < 0) {
               show("You burn the flesh from your bones!\n", agg);
               Sprintf(buf, "burns the flesh from %s living bones!\n",
                       get_his_her(agg));
               emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
               do_fatality = TRUE;
            }//if
            else {
               show("You give yourself a taste of life after death!\n", 
                    agg);
               Sprintf(buf, "blasts %s with %s crimson inferno.\n", 
                      get_hisself_herself(vict), 
                      get_his_her(agg));
               emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            }//else
            do_join_in_battle = FALSE;
         }//if
         else {
            if (vict.HP < 0) {
               Sprintf(buf, "You burn the flesh from %S's living bones!!\n",
                       name_of_crit(vict, agg.SEE_BIT));
               show(buf, agg);
               Sprintf(buf, 
                   "%S burns the flesh from your once living bones!\n",
                   name_of_crit(agg, vict.SEE_BIT));
               show(buf, vict);
               Sprintf(buf, "burns the flesh from %S's living bones!\n",
                       name_of_crit(vict, ~0));
               emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
               do_fatality = TRUE;
            }//if
            else {
               Sprintf(buf, "You give %S a taste of life after death!\n", 
                       name_of_crit(vict, agg.SEE_BIT));
               show(buf, agg);
               Sprintf(buf, "%S gives you a brief taste of hell!\n", 
                       name_of_crit(agg, vict.SEE_BIT));
               buf.Cap();
               show(buf, vict);
               Sprintf(buf, "blasts %S with %s crimson inferno.\n", 
                      name_of_crit(vict, ~0), 
                      get_his_her(agg));
               emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            }//else
         }//else
      }//if did_hit
      else { //missed
         agg.PAUSE += 1;   // increment pause_count
         if (&agg == &vict) {
            show("You miss yourself with your sulpherous inferno.\n", agg);
            Sprintf(buf, "misses %s with %s fireball.\n", 
                    get_hisself_herself(vict),
                    get_his_her(vict));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            do_join_in_battle = FALSE;
         }//if
         else {
            Sprintf(buf, "You miss %S with your fireball.\n", 
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);

            Sprintf(buf, "%S narrowly misses you with a fireball!\n", 
                       name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
 
            Sprintf(buf, "misses %S with %s fireball!\n", 
                       name_of_crit(vict, ~0), 
                       get_his_her(agg));
            emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
         }//else
      }//else, missed with canned magic   
   }//if canned
   else { //not canned 
      if (!lost_concentration(agg, spell_num)) { 
         if (&agg == &vict) {
            show("It would be stupid to try to burn yourself!!\n", agg);
            return;
         }//if
         if (did_spell_hit(agg, FIRE, vict)) {
            exact_raw_damage(d(8, agg.LEVEL), FIRE, vict, agg);
            if (vict.HP < 0) {
               Sprintf(buf, "You burn the flesh from %S's living bones!!\n",
                       name_of_crit(vict, agg.SEE_BIT));
               show(buf, agg);
               Sprintf(buf, 
                   "%S burns the flesh from your once living bones!\n",
                   name_of_crit(agg, vict.SEE_BIT));
               show(buf, vict);
               Sprintf(buf, "burns the flesh from %S's living bones!\n",
                       name_of_crit(vict, ~0));
               emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
               do_fatality = TRUE;
            }//if
            else {
               Sprintf(buf, "You give %S a taste of life after death!\n", 
                       name_of_crit(vict, agg.SEE_BIT));
               show(buf, agg);
               Sprintf(buf, "%S gives you a brief taste of hell!\n", 
                       name_of_crit(agg, vict.SEE_BIT));
               buf.Cap();
               show(buf, vict);
               Sprintf(buf, "blasts %S with %s crimson inferno.\n", 
                      name_of_crit(vict, ~0), 
                      get_his_her(agg));
               emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            }//else not fatality
         }//if did_hit
         else { //missed
            exact_raw_damage(d(3, agg.LEVEL) + agg.LEVEL, FIRE, vict, agg);
            if (vict.HP < 0) {
               Sprintf(buf, "You burn the flesh from %S's living bones!!\n",
                       name_of_crit(vict, agg.SEE_BIT));
               show(buf, agg);
               Sprintf(buf, 
                   "%S burns the flesh from your once living bones!\n",
                   name_of_crit(agg, vict.SEE_BIT));
               show(buf, vict);
               Sprintf(buf, "burns the flesh from %S's living bones!\n",
                       name_of_crit(vict, ~0));
               emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
               do_fatality = TRUE;
            }//if
            else {
               Sprintf(buf, "You singe %S with your fireball.\n", 
                       name_of_crit(vict, agg.SEE_BIT));
               show(buf, agg);
  
               Sprintf(buf, "%S singes you with a fireball!\n", 
                       name_of_crit(agg, vict.SEE_BIT));
               buf.Cap();
               show(buf, vict);
 
               Sprintf(buf, "singes %S with %s fireball!\n", 
                       name_of_crit(vict, ~0), 
                       get_his_her(agg));
               emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
            }//else no fatality
         }//missed
         agg.MANA -= spell_mana;
         agg.PAUSE += 1;   // increment pause_count
      }//if !lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);

         emote("obviously forgot something!\n", agg, 
               room_list[agg.getCurRoomNum()], TRUE);
 
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
      }//else lost concentration
   }//else !canned

   if (do_join_in_battle && !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if

   if (do_fatality) {
      agg_kills_vict(agg, vict);
   }//if
}//do_cast_fireball
 

void cast_fireball(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = FIREBALL_SKILL_NUM;

   if (victim->Strlen() == 0) 
      vict = Top(pc.IS_FIGHTING);
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Whom do you wish to barbeque??\n", pc);
      return;
   }//if
   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
              victim, pc.SEE_BIT);
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
   
   if (!(vict = check_for_diversions(*vict, "SGM", pc)))
     return;						       
                 /* all checks have been passed, lets do it */

   do_cast_fireball(*vict, pc, FALSE, 0);
}//cast_fireball


void cast_summon(int i_th, const String* targ, critter& pc) {
   int spell_num = SUMMON_SKILL_NUM;

   critter* vict = have_crit_named(pc_list, i_th, targ,
                                   pc.SEE_BIT, ROOM);
   if (!vict) {
     show("You don't see that person!\n", pc);
     return;
   }//if

   if (!ok_to_do_action(vict, "KMSNB", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_summon(*vict, pc, FALSE, 0);  //does no error checking
}//cast_summon



void do_cast_summon(critter& vict, critter& pc, int is_canned, int lvl) {
   String buf(100);
   short do_affects = FALSE;
   int spell_num = SUMMON_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;
   int did_hit = TRUE;

   if (!is_canned)
      lvl = pc.LEVEL;
   
   if (is_canned || (!(lost_con = lost_concentration(vict, spell_num)) &&
		     (did_hit = did_spell_hit(pc, SUMMON, vict)))){ 
      do_affects = TRUE;
      if (!is_canned)
         pc.MANA -= spell_mana;
   }//else !canned
   else if (lost_con) {
      show(LOST_CONCENTRATION_MSG_SELF, pc);
      
      emote(LOST_CONCENTRATION_MSG_OTHER, pc, ROOM, FALSE);
      if (!is_canned)
         pc.MANA -= spell_mana / 2;
   }//else lost concentration
   else {
      Sprintf(buf, "You failed to summon %S.\n", 
              name_of_crit(vict, pc.SEE_BIT));
      show(buf, pc);
      
      Sprintf(buf, "%S tried but failed to summon you!\n", 
              name_of_crit(pc, vict.SEE_BIT));
      show(buf, vict);
      
      Sprintf(buf, "fails to summon %S.", name_of_crit(vict, ~0));
      emote(buf, pc, ROOM, TRUE, &vict);
      if (!is_canned)
         pc.MANA -= spell_mana;
   }//else
   
   if (do_affects) {
      if (!mob_can_enter(vict, ROOM, FALSE)) {
         show("That person can't enter this room!\n", pc);
         return;
      }//if
      
      if (vict.getCurRoom()->isNoMagExit()) {
         pc.show("That person's area is impervious to your spell.\n");
         return;
      }

      Sprintf(buf, 
	      "%S opens a portal above your head and yanks you through!\n",
	      name_of_crit(pc, vict.SEE_BIT));
      buf.Cap();
      show(buf, vict);
      
      Sprintf(buf, 
	      "is suddenly yanked through a portal that opened over %s head!",
	      get_his_her(vict));
      emote(buf, vict, room_list[vict.getCurRoomNum()], TRUE); 
      
      Sprintf(buf, "has been summoned by %S.", name_of_crit(pc, ~0));
      emote(buf, vict, ROOM, TRUE);

      int is_dead;
      vict.doGoToRoom(pc.getCurRoomNum(), NULL, NULL, is_dead,
                      vict.getCurRoomNum()); //vict is always a PC
      
   }//if do_affects
   pc.PAUSE += 1; 
}//do_cast_summon
 

void cast_passdoor(int i_th, const String* drr, critter& pc) {
   int spell_num = PASSDOOR_SKILL_NUM;
   door* dr = door::findDoor(ROOM.DOORS, i_th, drr, pc.SEE_BIT, ROOM);

   if (!dr) {
      show("Which door do you wish to pass?\n", pc);
      return;
   }//if

   if (!dr->isClosed()) {
      show("It isn't even closed!\n", pc);
      return;
   }//if

   if (dr->isMagLocked()) {
      show("You can feel a power emanating from the door which would make", pc);
      show("your spell fail horribly!\n", pc);
      return;
   }//if

   if (dr->isNoPassdoor()) {
      pc.show("This door is imbued with properties that blocks your spell!\n");
      return;
   }

   if (!ok_to_do_action(NULL, "KMSNB", spell_num, pc)) {
     return;
   }//if

   if (!(mob_can_enter(pc, room_list[abs(dr->destination)], TRUE)))
     return;

   do_cast_passdoor(*dr, pc, FALSE, 0);  //does no error checking
}//cast_passdoor


void do_cast_passdoor(door& dr, critter& vict, int is_canned, int lvl) {
   String buf(100);
   short do_affects = FALSE;
   int spell_num = PASSDOOR_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   if (!is_canned)
      lvl = vict.LEVEL;

   if (vict.isMob()) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "WARNING:  MOB in do_cast_passdor." << endl;
      }
      return;
   }//if   


   int lost_con = TRUE;

   if (is_canned || !(lost_con = lost_concentration(vict, spell_num))) { 
      do_affects = TRUE;
      if (!is_canned)
         vict.MANA -= spell_mana;
   }//else !canned
   else { //lost con
      show(LOST_CONCENTRATION_MSG_SELF, vict);     
      emote(LOST_CONCENTRATION_MSG_OTHER, vict, 
            room_list[vict.getCurRoomNum()], FALSE);
      if (!is_canned)
         vict.MANA -= spell_mana / 2;
   }//else lost concentration
   
   if (do_affects) {
      show("You briefly step into another dimension!\n", vict);
      emote("steps into another dimension.", vict,
            room_list[vict.getCurRoomNum()], TRUE); 
      
      int is_dead;
      vict.doGoToRoom(abs(dr.destination), NULL, NULL, is_dead,
                      vict.getCurRoomNum());
      
      if (!is_dead) {
         emote("steps back into your dimension.", vict,
               room_list[vict.getCurRoomNum()], TRUE);
      }
   }//if do_affects
   vict.PAUSE += 1;   // increment pause_count
}//do_cast_passdoor


void cast_recall(int i_th, const String* victim, critter& pc) {
   int spell_num = RECALL_SKILL_NUM;
   critter* vict = NULL;
   if (victim->Strlen() == 0)
     vict = &pc;
   else {
     vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   }//else

   if (!vict) {
     show("Recall who?\n", pc);
     return;
   }//if

   if (!ok_to_do_action(vict, "KMSNB", spell_num, pc)) {
     return;
   }//if

   do_cast_recall(*vict, pc, FALSE, 0);  //does no error checking
}//cast_recall


void do_cast_recall(critter& vict, critter& pc, int is_canned, int lvl) {
   String buf(100);
   short do_affects = FALSE;
   int spell_num = RECALL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = TRUE;

   if (pc.isMob()) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "WARNING:  MOB in do_cast_recall." << endl;
      }
      return;
   }//if
   
   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) { 
      do_affects = TRUE;
      if (!is_canned)
	pc.MANA -= spell_mana;
   }//else !canned
   else { //lost con
      show(LOST_CONCENTRATION_MSG_SELF, pc);     
      emote(LOST_CONCENTRATION_MSG_OTHER, pc, ROOM, FALSE);
      if (!is_canned)
         pc.MANA -= spell_mana / 2;
   }//else lost concentration
   
   if (do_affects) {
      show("The world shimmers!\n", vict);
      emote("grows opaque and disappears.", vict,
            room_list[vict.getCurRoomNum()], TRUE); 
      
      int is_dead;
      vict.doGoToRoom(RECALL_ROOM, NULL, NULL, is_dead, vict.getCurRoomNum());

      if (!is_dead) {
         emote("blinks into existence.", vict, room_list[vict.getCurRoomNum()],
               TRUE);
      }//if

   }//if do_affects
   vict.PAUSE += 1;   // increment pause_count
}//do_cast_recall
 

void cast_teleport(critter& pc) {
   int spell_num = TELEPORT_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_teleport(pc, pc, FALSE, 0);  //does no error checking
}//cast_teleport


void do_cast_teleport(critter& vict, critter& pc, int is_canned, int lvl) {
   String buf(100);
   int new_room_num = FALSE;
   short done = FALSE, do_affects = FALSE;
   int spell_num = TELEPORT_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   //log("in do_cast_teleport\n");

   if (!is_canned)
     lvl = pc.LEVEL;

   int lost_con = TRUE;

   if (vict.getCurRoom()->isNoMagExit()) {
      vict.show("You cannot leave this place by magic means!\n");
      return;
   }

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) { 
      do_affects = TRUE;
      if (!is_canned)
	vict.MANA -= spell_mana;
   }//else !canned
   else { //lost con
     show(LOST_CONCENTRATION_MSG_SELF, pc);
     
     emote(LOST_CONCENTRATION_MSG_OTHER, pc, 
	   room_list[pc.getCurRoomNum()], FALSE);
     if (!is_canned)
       pc.MANA -= spell_mana / 2;
   }//else lost concentration

   if (do_affects) {
      show("The world shimmers!\n", vict);
      emote("grows opaque and disapears.", vict,
            room_list[vict.getCurRoomNum()], TRUE); 

      int sanity = 0;
      while (!done) {
         if (sanity++ > 10) {
            if (mudlog.ofLevel(DBG)) {
               mudlog << "WARNING:  Sanity check failed in do_cast_teleport."
                      << "  vict:  " << *(name_of_crit(vict, ~0)) << endl;
            }
            emote("disappears briefly and then shimmers back into existence.",
                  vict, room_list[vict.getCurRoomNum()], TRUE);

            break;
         }

         new_room_num = d(1, Cur_Max_Room_Num);
         if ((mob_can_enter(vict, room_list[new_room_num], FALSE)) &&
             (!room_list[new_room_num].isNoMagEntry())) {
            done = TRUE;
            emote("shimmers into existence.", vict, 
                  room_list[vict.getCurRoomNum()], TRUE);
            int is_dead;
            vict.doRemoveFromBattle();
            vict.doGoToRoom(new_room_num, NULL, NULL, is_dead,
                            vict.getCurRoomNum());
            if (is_dead)
               return;
         }//if
      }//while

      vict.doRemoveFromBattle();
   }//if do_affects
   vict.PAUSE += 1;   // increment pause_count
}//do_cast_teleport
 


void cast_poison(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = POISON_SKILL_NUM;

   if (victim->Strlen() == 0) 
      vict = Top(pc.IS_FIGHTING);
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);
   if (!vict) {
      show("Who do you wish to poison??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
                          victim, pc.SEE_BIT);
   }//if
 
   if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
  
   do_cast_poison(*vict, pc, FALSE, 0);  //does no error checking
}//cast_poison


void do_cast_poison(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_effects = FALSE;
   short do_join_in_battle = TRUE;
   int spell_num = POISON_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 

   if (!is_canned)
      lvl = agg.LEVEL;

   if (is_canned) {
      if (did_spell_hit(vict, CRONIC, agg, lvl, TRUE)) {
         do_effects = TRUE;

         if (&vict == &agg) {
            show("You feel sick!\n", vict);
            emote("turns green and looks quite sick.", agg, 
                  room_list[agg.getCurRoomNum()], TRUE); 
            do_join_in_battle = FALSE;
         }//if
         else {
            Sprintf(buf, "You poison %S!\n", 
                       name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, "%S has poisoned you!\n", 
                       name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
            Sprintf(buf, "%S poisons %S.\n", 
                      name_of_crit(agg, ~0), name_of_crit(vict, ~0));
            buf.Cap();
            show_all_but_2(agg, vict, buf, room_list[agg.getCurRoomNum()]);
         }//else
      }//if did_hit
      else { //missed
         if (&agg == &vict) {
            show("You can't even manage to poison yourself.\n", agg);
            do_join_in_battle = FALSE;
         }//if
         else {
            Sprintf(buf, "You fail to poison %S.\n", 
                    name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);

            Sprintf(buf, "%S fails to poison you!\n", 
                       name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
         }//else
         agg.PAUSE += 1;   // increment pause_count
      }//else, missed with canned magic   
   }//if canned
   else {//not canned 
      if (&agg == &vict) {
         show("You would feel stupid if you poisoned yourself on purpose.\n",
              agg);
         return;
      }//if
      if (!lost_concentration(agg, spell_num)) { 
         if (did_spell_hit(agg, CRONIC, vict)) {
            do_effects = TRUE;

            Sprintf(buf, "You poison %S!\n", 
                       name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);
            Sprintf(buf, "%S poisons you!\n", 
                       name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
         }//if did_hit
         else { //missed
            Sprintf(buf, "You fail to poison %S.\n", 
                       name_of_crit(vict, agg.SEE_BIT));
            show(buf, agg);

            Sprintf(buf, "%S tries and fails to poison you!\n", 
                       name_of_crit(agg, vict.SEE_BIT));
            buf.Cap();
            show(buf, vict);
 
         }//missed
         agg.MANA -= spell_mana;
         agg.PAUSE += 1;   // increment pause_count
      }//if ! lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);

         emote(LOST_CONCENTRATION_MSG_OTHER, agg, *(agg.getCurRoom()), TRUE);
 
         agg.MANA -= spell_mana / 2;
         agg.PAUSE += 1; 
      }//else
   }//else !canned

   if (do_effects) {
      stat_spell_cell* sp;

      Cell<stat_spell_cell*> cll(vict.affected_by);
      while ((sp = cll.next())) {
         if (sp->stat_spell == spell_num) {
            sp->bonus_duration += (int)((float)(lvl) / 3.0);
            return;
         }//if
      }//while

      Put(new stat_spell_cell(spell_num, lvl/3), vict.affected_by);
      vict.HP_REGEN += POISON_HP_REGEN_AUGMENTATION;
      vict.MV_REGEN += POISON_MV_REGEN_AUGMENTATION;
      vict.MA_REGEN += POISON_MA_REGEN_AUGMENTATION;
   }//if

   if (do_join_in_battle && !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if
}//do_cast_poison


void cast_illuminate(critter& pc) {
   int spell_num = ILLUMINATE_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_illuminate(ROOM, pc, FALSE, 0);  //does no error checking
}//cast_illuminate


void do_cast_illuminate(room& rm, critter& agg, int is_canned,
                        int lvl) {
   int spell_num = ILLUMINATE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
   String buf(100);
   short do_effects = FALSE;
 
   if (room_list[agg.getCurRoomNum()].isNoMagic()) {
      show(NOT_IN_NOMAG_RM_MSG, agg);
      return;
   }//if

   if (!is_canned)
      lvl = agg.LEVEL;

   agg.PAUSE += 1; 

   if (is_canned) {
      do_effects = TRUE;
      show("You enlighten the room!\n", agg); 
      emote("enlightens the room.", agg, rm, TRUE); 
   }//if canned
   else {//not canned 
      if (!lost_concentration(agg, spell_num)) { 
         do_effects = TRUE;
         show("You enlighten the room!\n", agg); 
         emote("enlightens the room.", agg, rm, TRUE); 
         agg.MANA -= spell_mana;
      }//if ! lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, rm, TRUE);
         agg.MANA -= spell_mana / 2;
      }//else
   }//else !canned

   if (do_effects) {
      mudlog.log(DBG, "Doing illuminate effects.\n");
      stat_spell_cell* sp;

      Cell<stat_spell_cell*> cll(rm.affected_by);
      while ((sp = cll.next())) {
         if (sp->stat_spell == spell_num) {
            sp->bonus_duration += (int)((float)(lvl) / 2.0);
            return;
         }//if
      }//while

      affected_rooms.gainData(&rm); //add to global affected list so
                                     //spell can wear off eventually
      Put(new stat_spell_cell(spell_num, lvl/2), rm.affected_by);
      rm.setVisBit(rm.getVisBit() & ~1);  //make it light
   }//if
}//do_cast_illuminate



//******************************************************************//
///**************   auxillary spell/skill functions   *************///
//******************************************************************//


int has_all_prereqs(int skill_num, critter& pc) {
   Cell<int> cll(SSCollection::instance().getSS(skill_num).prereqs);
   int tmp;
   int retval;

   while ((tmp = cll.next())) {
     if (pc.SKILLS_KNOWN.Find(tmp, retval)) {
       if (retval < 50)
	 return FALSE;
     }//if
     else
       return FALSE;
   }//while

   return TRUE;
}//has_all_prereqs

void update_skills(critter& pc) {
   int key;
   if (pc.SKILLS_KNOWN.Min(key)) {
      update_skill(key, pc);
      while (pc.SKILLS_KNOWN.Next(key)) {
         update_skill(key, pc);
      }//while
   }//if
}//update_skills


void update_skill(int last_learned, critter& pc) {
   
   if (!pc.pc) {
      return;
   }//if

   if (get_percent_lrnd(last_learned, pc) < 50) {
      return;
   }

   if (!check_l_range(last_learned, 0, NUMBER_OF_SKILL_SPELLS, pc, FALSE)) {
      mudlog.log(ERR,
                 "ERROR:  last_learned out of range, gain_spells_skills!!\n");
      return;
   }//if

   Cell<int> cll(SSCollection::instance().getSS(last_learned).enables);
   int tmp;
   while ((tmp = cll.next())) {
     if (has_all_prereqs(tmp, pc) && (get_percent_lrnd(tmp, pc) == -1)) {
        pc.SKILLS_KNOWN.Insert(tmp, 1);
	if (tmp == WEAPON_MASTERY_SKILL_NUM)
	   pc.DAM++;
	else if (tmp == HONOR_CODE_SKILL_NUM)
	   pc.WIS++;
	else if (tmp == LEADERSHIP_SKILL_NUM)
	   pc.CHA++;
	else if (tmp == QUICKFOOT_SKILL_NUM)
	   pc.DEX++;
	else if (tmp == WRESTLING_SKILL_NUM)
	   pc.BH_DICE_COUNT++;
	else if (tmp == SECOND_ATTACK_SKILL_NUM)
	   pc.ATTACKS++;
	else if (tmp == CLIMBING_SKILL_NUM)
	   pc.CRIT_FLAGS.turn_on(5);
	else if (tmp == DUAL_WIELD_SKILL_NUM)
	   pc.CRIT_FLAGS.turn_on(16);
	else if (tmp == LOGIC_SKILL_NUM)
 	   pc.CHA--; //logical people are boring!!
	else if (tmp == MARTIAL_ARTS_SKILL_NUM) {
	   pc.BH_DICE_COUNT++;
	   pc.BH_DICE_SIDES += 2;
	}//martail arts
     }//if
   }//while
}//gain_spells_skills


short lost_concentration(critter& agg, int spell_num) {
   int percent_lrned = 0, i;

   //   log("In lost_concentration\n");

   if (agg.pc) {
     agg.SKILLS_KNOWN.Find(spell_num, percent_lrned);
   }//if
   else { //not a pc
      percent_lrned = 50 + (2 * agg.LEVEL);
   }// else

   i = (int)(((float)percent_lrned / 75.0) * 
     (float)(agg.WIS + agg.INT + agg.LEVEL + 200));

   return (d(1, 100) >= d(1, i));
}//lost_concentration


int relocate_within_zone(critter& pc) {
   int zn = ROOM.getZoneNum();

   int begin_rm = ZoneCollection::instance().elementAt(zn).getBeginRoomNum();
   int end_rm = ZoneCollection::instance().elementAt(zn).getEndRoomNum();
   int count = 0;
   int new_room_num;

      while (TRUE) {   
         new_room_num = begin_rm + d(1, (end_rm - begin_rm));
	 if (count++ > 15) 
	    new_room_num = pc.getCurRoomNum();
         if (mob_can_enter(pc, room_list[new_room_num], FALSE)) {
            int pcs_room = pc.getCurRoomNum();
            ROOM.removeCritter(&pc);
	    leave_room_effects(room_list[pcs_room], pc);
            room_list[new_room_num].removeCritter(&pc);
            look(1, &NULL_STRING, pc);
	    return TRUE;
         }//if
	 if (count++ > 15) 
	    return FALSE; //should never get here I think
      }//while
}//relocate_within_zone



















