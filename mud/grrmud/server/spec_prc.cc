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

///*********************  spec_prc.cc  *****************************///
///******  This holds special procedures for computer driven *******///
///******  characters.                                       *******///
///*****************************************************************///


#include "spells.h"
#include "spells2.h"
#include "skills.h"
#include "pet_spll.h"
#include "rm_spll.h"
#include "dam_spll.h"
#include "ez_spll.h"
#include "trv_spll.h"
#include "batl_prc.h"
#include "spec_prc.h"
#include "misc.h"
#include "misc2.h"
#include "classes.h"
#include "spells.h"
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include "command4.h"
#include "command5.h"
#include "socials.h"
#include "const.h"
#include "battle.h"
#include <PtrArray.h>
#include "load_wld.h"
#include "SkillSpell.h"



/** Execute these after a mob has just killed someone. */
void do_just_killed_procs(critter& agg) {

   if (agg.pc) {  //if do pc things
      if (agg.PC_FLAGS.get(12)) { //autoloot
         agg.pc->input.Prepend("get all corpse\n");
         agg.processInput(agg.pc->input, FALSE);
      }//if
   }//if

   if (agg.mob && agg.mob->proc_data) {
     int bad_ass = agg.mob->getBadAssedness();
     String cmd("get all corpse\n");
     if (bad_ass > 7) {
       agg.processInput(cmd, FALSE);
       cmd = "wear all\n";
       agg.processInput(cmd, FALSE);
       cmd = "skin corpse\n";
       agg.processInput(cmd, FALSE);
       cmd = "wear skin\n";
       agg.processInput(cmd, FALSE);
       cmd = "growl\n";
       agg.processInput(cmd, FALSE);
     }//if
     else if (bad_ass > 4) {
       cmd = "wear all\n";
       agg.processInput(cmd, FALSE);
       cmd = "laugh\n";
       agg.processInput(cmd, FALSE);
     }//if
     else if (bad_ass > 0) {
       cmd = "laugh\n";
       agg.processInput(cmd, FALSE);
     }//if
   }//if do mob things
}//do_just_killed_procs



/* assumes both are either SMOB's or PC's */
void do_domob_give_proc(critter& targ, critter& pc, object& obj) {
   if (!targ.isSmob())
      return;
   
   if (targ.mob && targ.mob->proc_data && 
       targ.mob->proc_data->give_proc) {  //if so, got a live one!
      
      if (obj.OBJ_NUM == targ.DOMOB_GIVE_NUM) {  //if gave right item
         
         if (targ.GIVE_RIGHT_MSG.Strlen()) { //if has a message
            show(targ.GIVE_RIGHT_MSG, pc);
         }//if

         /* now lets do any procs that need doing */
         if (check_l_range(targ.GIVE_RIGHT_ITEM, 1, 
                           NUMBER_OF_ITEMS, pc, FALSE)) {
            recursive_init_loads(obj_list[targ.GIVE_RIGHT_ITEM], 0);
            do_give(pc, targ, obj_list[targ.GIVE_RIGHT_ITEM]);
         }//if
         if (targ.GIVE_TEACH.Strlen()) { //if should teach something
            int spell_num = SSCollection::instance().getNumForName(targ.GIVE_TEACH);
            if (spell_num == -1) { // if it didn't exist
               mudlog.log(ERR, "ERROR:  spell unknown in do_domob_give.\n");
               mudlog.log(ERR, targ.GIVE_TEACH);
            }//if
            else {
               int retval;
               if (!pc.SKILLS_KNOWN.Find(spell_num, retval)) {
                  pc.SKILLS_KNOWN.Insert(spell_num, 1);
               }//if
            }//else
         }//if should teach

         if (check_l_range(targ.GIVE_TRANSPORT_ROOM, 1,
                           NUMBER_OF_ROOMS, pc, FALSE)) { //if should transprt
            do_transport(pc, targ, room_list[targ.GIVE_TRANSPORT_ROOM]);
         }//if
         
         // Now, delete the object,
         targ.loseInv(&obj);
         recursive_init_unload(obj, 0);
         if (obj.IN_LIST) {
            delete &obj;
            return;
         }//if
         
      }//if gave right object
      else {
         if (targ.GIVE_WRONG_MSG.Strlen()) {
            show(targ.GIVE_WRONG_MSG, pc);
         }//if
      }//else
   }//if targ has such a special procedure
}//do_domob_give_proc

      
 
/* assumes both are either SMOB's or PC's */
/* this is triggered by 'discuss', not 'say' */
void do_domob_say_proc(critter& targ, critter& pc, const String& msg) {  
  say_proc_cell* ptr;

  if (!targ.isSmob())
    return;

  if (targ.mob && targ.mob->proc_data && 
      !IsEmpty(targ.mob->proc_data->topics)) {  //if so, got a live one!

    if (!pass_domob_checks(targ, pc))
      return;

    if ((ptr = have_topic_named(targ.TOPICS, msg))) {  
      if (ptr->msg.Strlen()) { //if has a message
	show(ptr->msg, pc);
      }//if
      /* now lets do any procs that need doing */
      if (check_l_range(ptr->obj_num, 1, NUMBER_OF_ITEMS, pc, FALSE)) { 
	do_give(pc, targ, obj_list[ptr->obj_num]);
      }//if
      if (ptr->skill_name.Strlen()) { //if should teach something
         int spell_num = SSCollection::instance().getNumForName(ptr->skill_name);
         if (spell_num == -1) { // if it didn't exist
            mudlog.log(ERR, "ERROR:  spell unknown in do_domob_say.\n");
            mudlog.log(ERR, ptr->skill_name);
         }//if
         else {
            int retval;
            if (!pc.SKILLS_KNOWN.Find(spell_num, retval)) {
               pc.SKILLS_KNOWN.Insert(spell_num, 1);
            }//if
         }//else
      }//if should teach
      if (check_l_range(ptr->trans_to_room, 1, NUMBER_OF_ROOMS, pc, FALSE)) {
	do_transport(pc, targ, room_list[ptr->trans_to_room]);
      }//if
    }//if gave right object
    else {
      do_tell(targ, 
	      "Well, sounds interesting, but I don't know much of it.\n",
	      pc, FALSE, targ.getCurRoomNum());
    }//else
  }//if targ has such a special procedure
}//do_domob_say_proc


/* assumes both are either SMOB's or PC's */
void do_domob_bow_proc(critter& targ, critter& pc) {
  if (!targ.isSmob())
    return;

  if (targ.mob && targ.mob->proc_data && 
      targ.mob->proc_data->bow_proc) {  //if so, got a live one!

    /*  Don't think checks are usefull here
    if (!pass_domob_checks(targ, pc))
      return;
      */

    if (targ.BOW_MSG.Strlen()) { //if has a message
      show(targ.BOW_MSG, pc);
    }//if
      /* now lets do any procs that need doing */
    if (check_l_range(targ.BOW_ITEM, 1, NUMBER_OF_ITEMS, pc, FALSE)) {
      do_give(pc, targ, obj_list[targ.BOW_ITEM]);
    }//if
    if (targ.BOW_TEACH.Strlen()) { //if should teach something
      int spell_num = SSCollection::instance().getNumForName(targ.BOW_TEACH);
      if (spell_num == -1) { // if it didn't exist
	mudlog.log(ERR, "ERROR:  spell unknown in do_domob_bow.\n");
	mudlog.log(ERR, targ.BOW_TEACH);
      }//if
      else {
	int retval;
	if (!pc.SKILLS_KNOWN.Find(spell_num, retval)) {
	  pc.SKILLS_KNOWN.Insert(spell_num, 1);
	}//if
      }//else
    }//if should teach
    if (check_l_range(targ.BOW_TRANSPORT_ROOM, 1, NUMBER_OF_ROOMS, 
		      pc, FALSE)) {
      do_transport(pc, targ, room_list[targ.BOW_TRANSPORT_ROOM]);
    }//if
  }//if has bow special procedure
}//do_domob_bow_proc


/* assumes both are either SMOB's or PC's */
void do_domob_curse_proc(critter& targ, critter& pc) {
  if (!targ.isSmob())
    return;

  if (targ.mob && targ.mob->proc_data && 
      targ.mob->proc_data->curse_proc) {  //if so, got a live one!

    /* I don't think checks are usefull here
    if (!pass_domob_checks(targ, pc))
      return;
    */

    if (targ.CURSE_MSG.Strlen()) { //if has a message
      show(targ.CURSE_MSG, pc);
    }//if
      /* now lets do any procs that need doing */
    if (check_l_range(targ.CURSE_ITEM, 1, NUMBER_OF_ITEMS, pc, FALSE)) {
      do_give(pc, targ, obj_list[targ.CURSE_ITEM]);
    }//if
    if (targ.CURSE_TEACH.Strlen()) { //if should teach something
      int spell_num = SSCollection::instance().getNumForName(targ.CURSE_TEACH);
      if (spell_num == -1) { // if it didn't exist
	mudlog.log(ERR, "ERROR:  spell unknown in do_domob_curse.\n");
	mudlog.log(ERR, targ.CURSE_TEACH);
      }//if
      else {
	int retval;
	if (!pc.SKILLS_KNOWN.Find(spell_num, retval)) {
	  pc.SKILLS_KNOWN.Insert(spell_num, 1);
	}//if
      }//else
    }//if should teach
    if (check_l_range(targ.CURSE_TRANSPORT_ROOM, 1, NUMBER_OF_ROOMS, 
		      pc, FALSE)) { //if should transport
      do_transport(pc, targ, room_list[targ.CURSE_TRANSPORT_ROOM]);
    }//if
  }//if has curse special procedure
}//do_domob_curse_proc


/* assumes existance of targ.FLAG1, what a stupid fn name **doh**  */
short pass_domob_checks(critter& targ, critter& pc) {
  if (targ.FLAG1.get(9) && (targ.RACE != pc.RACE)) {
    show(targ.WRONG_RACE_MSG, pc);
    return FALSE;
  }//if
  else if (targ.FLAG1.get(10)) { //align check
    if ((targ.ALIGN < -350) && (pc.ALIGN >= -350)) {
      show(targ.WRONG_ALIGN_MSG, pc);
      return FALSE;
    }//if
    else if ((targ.ALIGN >= -350) && (targ.ALIGN <= 350) && 
	     ((pc.ALIGN < -350) || (pc.ALIGN > 350))) {
      show(targ.WRONG_ALIGN_MSG, pc);
      return FALSE;
    }//if
    else if ((targ.ALIGN > 350) && (pc.ALIGN <= 350)){
      show(targ.WRONG_ALIGN_MSG, pc);
      return FALSE;
    }//if
  }//if align check
  else if (targ.FLAG1.get(11) && (targ.CLASS != pc.CLASS)) {
    show(targ.WRONG_CLASS_MSG, pc);
    return FALSE;
  }//if
  return TRUE;
}//pass_domob_checks


void do_shot_proc(critter& targ, critter& pc, short did_hit,
                  int& is_targ_dead) {
  /* targ is victim, but is retaliating agains the pc here */

   if (!targ.mob)
      return;
   if (targ.isMob()) {
     mudlog.log(ERR, "ERROR:  targ is a MOB in do_shot_proc.\n");
     return;
   }//if

   /* default action */
   if (!did_hit || !targ.hasAI()) {
      prone(targ);
      return;
   }//if

	   /* these have AI */
   if (targ.INT1) { /* sentinals go down no matter what */
      prone(targ);
   }//if

   int benev = targ.mob->getBenevolence();
   int bad_ass = targ.mob->getBadAssedness();

   if ((benev < -7) || (bad_ass > 6)) {
     if (targ.EQ[9] && (targ.EQ[9]->OBJ_FLAGS.get(47) ||
			targ.EQ[9]->OBJ_FLAGS.get(46))) { //with ranged
       do_shoot(pc, targ);  
     }//if have ranged attack
     targ.trackToKill(pc, is_targ_dead);  //go after the bastard!!
   }//if
   else if ((benev < -3) || (bad_ass > 4)) {
     if (targ.EQ[9] && (targ.EQ[9]->OBJ_FLAGS.get(47) ||
			targ.EQ[9]->OBJ_FLAGS.get(46))) { //with ranged
       do_shoot(pc, targ);  
     }//if have ranged attack
     targ.remember(pc); //targ remembers pc, will be aggressive
     prone(targ);
   }//if
   else if (benev < 3) {
     targ.remember(pc);
     prone(targ);
   }//
   else if (bad_ass < -5) {
     a_summons_help_against_b(targ, pc);
   }//if
   else if (bad_ass < 3) {
      flee_to_safety(targ, is_targ_dead);
      if (!is_targ_dead)
         hit(1, Top(pc.names), targ);  //in case they fled to room of agg
   }//else no ranged attack
   else {
     prone(targ);
   }//else
}//do_shot_proc


short do_wand_scroll_proc(door* dr_ptr, int proc_num, critter& pc,
                          int spell_lvl) {
   String buf(100);

   /*  Used for potions too btw.                             */
   /*  If targ is NOT NULL, then it is a SMOB or PC.         */
   /*  return value determines if should decrement the wand. */

   if (!dr_ptr) {
      mudlog << "ERROR:  otarg is NULL in do_wand_scroll_proc"
             << endl;
      show("What do you wish to target?\n", pc);
      return FALSE;
   }//if

   switch (proc_num)
     {
       case 21: 
         do_cast_distortion_wall(*dr_ptr, pc, TRUE, spell_lvl);
         return TRUE;
       case 181: 
         do_cast_passdoor(*dr_ptr, pc, TRUE, spell_lvl);
         return TRUE;
       default:
         Sprintf(buf, 
                 "ERROR:  default called in do_wand_scroll_proc(DR):  %i.\n", 
                 proc_num);
         mudlog.log(ERR, buf);
         show( "You call upon unknown forces, and nothing happens!\n",
              pc);
         return FALSE;
      }//switch
}//do_scroll_proc (DOOR)



short do_wand_scroll_proc(int proc_num, critter& pc,
                          int spell_lvl) {
   /*  Used for potions too btw.                             */
   /*  If targ is NOT NULL, then it is a SMOB or PC.         */
   /*  return value determines if should decrement the wand. */

   switch (proc_num)
     {
       case 22: 
         do_cast_lightning_storm(pc, TRUE, spell_lvl);
         return TRUE;
       case 25: 
         do_cast_meteorstorm(pc, TRUE, spell_lvl);
         return TRUE;
       case 145: 
         do_cast_create_golem(pc, TRUE, spell_lvl);
         return TRUE;
       case 146: 
         do_cast_conjure_minion(pc, TRUE, spell_lvl);
         return TRUE;
       case 147: 
         do_cast_conjure_horde(pc, TRUE, spell_lvl);
         return TRUE;
       case 149: 
         do_cast_raise_undead(pc, TRUE, spell_lvl);
         return TRUE;
       case 150: 
         do_cast_illusion(pc, TRUE, spell_lvl);
         return TRUE;
       case 153: 
         do_cast_mirror_image(pc, TRUE, spell_lvl);
         return TRUE;
       case 200: 
         do_cast_group_heal(pc, TRUE, spell_lvl);
         return TRUE;
       case 210: 
         do_cast_create_food(pc, TRUE, spell_lvl);
         return TRUE;
       case 211: 
         do_cast_heros_feast(pc, TRUE, spell_lvl);
         return TRUE;
       case 218: 
         do_cast_create_light(pc, TRUE, spell_lvl);
         return TRUE;

         // Those used on rooms, always current room
       case 4: 
         do_cast_illuminate(ROOM, pc, TRUE, spell_lvl);
         return TRUE;
       case 6: 
         do_cast_firewall(ROOM, pc, TRUE, spell_lvl);
         return TRUE;
       case 12: 
         do_cast_calm(pc, TRUE, spell_lvl);
         return TRUE;
       case 14: 
         do_cast_mass_charm(ROOM, pc, TRUE, spell_lvl);
         return TRUE;
       case 17: 
         do_cast_typhoon(pc, TRUE, spell_lvl);
         return TRUE;
       case 18: 
         do_cast_tornado(pc, TRUE, spell_lvl);
         return TRUE;
       case 20: 
         do_cast_quake(pc, TRUE, spell_lvl);
         return TRUE;
       case 198: 
         do_cast_flame_strike(pc, TRUE, spell_lvl);
         return TRUE;
       case 215: 
         do_cast_icestorm(pc, TRUE, spell_lvl);
         return TRUE;
       case 220: 
         do_cast_firestorm(pc, TRUE, spell_lvl);
         return TRUE;

       default: 
       {
         String buf(100);
         Sprintf(buf, 
                 "ERROR:  dflt called in do_wand_scroll_proc(NONE):  %i.\n", 
                 proc_num);
         mudlog.log(ERR, buf);
         show( "You call upon unknown forces, and nothing happens!\n",
              pc);
         return FALSE;
       }//default
      }//switch
}//do_scroll_proc (NONE)


short do_wand_scroll_proc(object* otarg, int proc_num, critter& pc,
                          int spell_lvl) {
   /*  Used for potions too btw.                             */
   /*  If targ is NOT NULL, then it is a SMOB or PC.         */
   /*  return value determines if should decrement the wand. */
   if (!otarg) {
      mudlog << "ERROR:  otarg is NULL in do_wand_scroll_proc"
             << endl;
      show("What do you wish to target?\n", pc);
      return FALSE;
   }//if

   switch (proc_num)
     {
       case 124: 
         do_lore(*otarg, pc);
         return TRUE;
       case 156: 
         do_cast_detect_poison(*otarg, pc, TRUE, spell_lvl);
         return TRUE;
       case 165: 
         do_cast_identify(*otarg, pc, TRUE, spell_lvl);
         return TRUE;
       case 182: 
         do_cast_enchant_weapon(*otarg, pc, TRUE, spell_lvl);
         return TRUE;
       case 183: 
         do_cast_enchant_armor(*otarg, pc, TRUE, spell_lvl);
         return TRUE;
       case 184: 
         do_cast_rune_edge(*otarg, pc, TRUE, spell_lvl);
         return TRUE;
       case 185: 
         do_cast_frost_blade(*otarg, pc, TRUE, spell_lvl);
         return TRUE;
       case 186: 
         do_cast_fire_blade(*otarg, pc, TRUE, spell_lvl);
         return TRUE;
       case 196: 
         do_cast_curse(*otarg, pc, TRUE, spell_lvl);
         return TRUE;
       case 213: 
         do_cast_create_water(*otarg, pc, TRUE, spell_lvl);
         return TRUE;
       default:
         String buf(100);
         Sprintf(buf, 
                 "ERROR:  default called in do_wand_scroll_proc(OBJ):  %i.\n", 
                 proc_num);
         mudlog.log(ERR, buf);
         show( "You call upon unknown forces, and nothing happens!\n",
              pc);
         return FALSE;
      }//switch
}//do_scroll_proc (OBJECTS)


short do_wand_scroll_proc(critter* targ, int proc_num, critter& pc,
                          int spell_lvl) {
   String buf(100);

/*  Used for potions too btw.                             */
/*  If targ is NOT NULL, then it is a SMOB or PC.         */
/*  return value determines if should decrement the wand. */
   if (!targ) {
      mudlog << "ERROR:  targ is NULL in do_wand_scroll_proc"
             << endl;
      show("Who do you wish to target?\n", pc);
      return FALSE;
   }//if

   switch (proc_num)
     {
      case 1: //heal
         do_cast_heal(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 10: 
         do_cast_web(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 11: 
         do_cast_entangle(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 30: 
         do_cast_restore(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 38: 
         do_cast_burning_hands(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 39: 
         do_cast_rainbow(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 136: 
         do_cast_shocking_grasp(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 139: 
         do_cast_dark_dart(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 140: 
         do_cast_shadows_blessing(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 151: 
         do_cast_faerie_fire(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 152: 
         do_cast_invisibility(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 155: 
         do_cast_detect_magic(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 159:
         do_cast_gate(room_list[targ->getCurRoomNum()], pc, TRUE, spell_lvl);
         return TRUE;
       case 160: 
         do_cast_portal(room_list[targ->getCurRoomNum()], pc, TRUE, spell_lvl);
         return TRUE;
       case 162: 
         do_cast_detect_hidden(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 163: 
         do_cast_detect_invisibility(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 164:
         do_cast_detect_alignment(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 171: 
         do_cast_sleep(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 172: 
         do_cast_fly(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 174: 
         do_cast_fireproof(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 175: 
         do_cast_haste(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 176: 
         do_cast_dispell_magic(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 177: 
         do_cast_strength(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 178: 
         do_cast_charm(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 179:
         do_cast_teleport(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 180: 
         do_cast_recall(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 187: 
         do_cast_bind_wound(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 188: 
         do_cast_cure_serious(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 189: 
         do_cast_cure_critical(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 191: 
         do_cast_bless(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 192: 
         do_cast_pfe(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 193: 
         do_cast_pfg(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 194: 
         do_cast_remove_curse(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 195: 
         do_cast_dispell_good(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 197: 
         do_cast_harm(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 199: 
         do_cast_holy_word(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 201:
         do_cast_divine_protection(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 202: 
         do_cast_magic_shield(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 203: 
         do_cast_armor(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 204: 
         do_cast_stone_skin(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 205: 
         do_cast_sanctuary(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 206: 
         do_cast_prismatic_globe(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 208: 
         do_cast_absorb_blows(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 217:
         do_cast_lightning(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 219: //fireball
         do_cast_fireball(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 221: 
         do_cast_blindness(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 222:
         do_cast_cause_sickness(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 224: 
         do_cast_cause_critical(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 225: 
         do_cast_weaken(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       case 226: 
         do_cast_poison(*targ, pc, TRUE, spell_lvl);
         return TRUE;
         //case 227: //TODO:  code this up! 
         //do_cast_plague(*targ, pc, TRUE, spell_lvl);
         //return TRUE;
       case 228: //this is not the error it looks like, see dam_spll.cc
         do_cast_dispell_good(*targ, pc, TRUE, spell_lvl);
         return TRUE;
       default:
         Sprintf(buf, 
                 "ERROR:  dflt called in do_wand_scroll_proc(CRIT):  %i.\n", 
                 proc_num);
         mudlog.log(ERR, buf);
         show( "You call upon unknown forces, and nothing happens!\n",
              pc);
         return FALSE;
      }//switch
}//do_scroll_proc


void do_pulsed_spec_procs(int first_room, int last_room) {
   Cell<critter*> cll;
   critter* ptr;
   int sz, i;
   int i_th;
   Cell<object*> cll2;
   object* obj_ptr;


   String wr_all("wear all\n");
   //String d_all("donate all\n");
   String gtobj(50);
   String buf(100);

   //log("In do_pulsed_spec_procs.\n");

   if (first_room < 0)
      first_room = 0;

   if (last_room >= NUMBER_OF_ROOMS)
      last_room = NUMBER_OF_ROOMS -1;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In pulsed spec procs, first_room:  " << first_room
             << " last room:  " << last_room << endl;
   }

   for (i = first_room; i <= last_room; i++) {
      if (room_list.elementAtNoCreate(i) &&
          !room_list[i].isZlocked() && //if not locked
          room_list[i].isInUse()) { //and in use

         if (mudlog.ofLevel(DBG)) {
            mudlog << "Found an unlocked room:  " << i << endl;
         }
         
         room_list[i].resetProcMobs();
      }//if
   }//for

   int is_dead = FALSE;
   for (i = first_room; i <= last_room; i++) {
      if (room_list.elementAtNoCreate(i) &&
          !room_list[i].isZlocked()) { //if not locked
         while ((ptr = room_list[i].findNextProcMob())) {
            if (mudlog.ofLevel(DBG)) {
               mudlog << "Found ptr:  " << ptr << " name: "
                      << ptr->getName() << endl;
            }
            if (ptr->MOB_FLAGS.get(1)) { //scavenge
               if (d(1,100) <= 10) {
                  if ((sz = room_list[i].inv.size())) { //objs to pick up?
                     
                     if (ptr->isMob()) { //if its a MOB
                        ptr = mob_to_smob(*ptr, i);
                     }//if

                     i_th = d(1,sz);
                     room_list[i].inv.head(cll2);
                     for (int j = 0; j<i_th; j++) {
                        obj_ptr = cll2.next();
                     }//for
 
                     if (!obj_ptr) {
                        mudlog.log(ERR,
                                   "ERROR:  obj_ptr NULL in scavenge proc.\n");
                        return;
                     }//if
  
                     Sprintf(gtobj, "get %S\n", Top(obj_ptr->ob->names));
 
                     ptr->processInput(gtobj, FALSE);
                     ptr->processInput(wr_all, FALSE);

                  }//if random chance
               }//if
            }// if scavenge
            if (ptr->isWanderer() && !ptr->isTracking() &&
                room_list[i].haveCritter(ptr)) {
               if (d(1, 10) > 3) {
                  //log("Doing wander spec_prc.\n");
                  // This can be fatal..though rarely is.  So, don't use *ptr
                  // any more after this..as it could be deleted already.

                  //if (ptr->isMob()) { //if its a MOB
                  //   ptr = mob_to_smob(*ptr, i);
                  //}//if

                  is_dead = FALSE;
                  move(*ptr, 1, *(room_list[i].getRandomExitDir(*ptr)),
                       TRUE, room_list[i], is_dead);

                  if (is_dead)
                     continue; //with the while loop

               }//if chance succeedes, 70% chance
            }//if wander
	    if (ptr->isTracking() && room_list[i].haveCritter(ptr)) {
               is_dead = FALSE;
	       ptr->doHuntProc(d(1,10), is_dead);
	    }//if hunting
         }//while, won't leave a room untill all have been dealt with
      }//if room isn't locked
   }//for, loops through rooms

   First_Room = last_room;
   Last_Room = last_room + 100;
   if (First_Room >= (NUMBER_OF_ROOMS -1)) {
      First_Room = 0;
      Last_Room = 100;
   }//if
}//do_pulsed


void do_this_obj_proc(int type_of_proc, int proc_num, critter& pc,
                      object& obj, int posn) {
   //log("In do_this_obj_proc.\n");
   String tmp_str(100);
   String other_msg(100);

   if (type_of_proc == OBJ_WEAR_PROC) {
      //log("Proc is of type obj_wear_proc.\n");
      if (proc_num == -1) { //wear_prc1, default
         //log("Doing generic wear proc.\n");
         if (posn == 1) {
            Sprintf(tmp_str, "You start wearing %S on your head.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "starts wearing %S on %s head.\n", 
                    &(obj.ob->short_desc), get_his_her(pc));
         }//if
         else if (posn == 2 || posn == 3) {
            Sprintf(tmp_str, "You start wearing %S around your neck.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "starts wearing %S around %s neck.\n", 
                    &(obj.ob->short_desc), 
                    get_his_her(pc));
         }//if
         else if (posn == 4) {
            Sprintf(tmp_str, "You start wearing %S about your body.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "starts wearing %S about %s body.\n", 
                    &(obj.ob->short_desc), 
                    get_his_her(pc));
         }//if
         else if (posn == 5) {
            Sprintf(tmp_str, "You start wearing %S on your arms.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "starts wearing %S on %s arms.\n", 
                    &(obj.ob->short_desc),
                    get_his_her(pc));
         }//if
         else if (posn == 6 || posn == 7) {
            Sprintf(tmp_str, "You start wearing %S around your wrist.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "starts wearing %S around %s wrist.\n", 
                    &(obj.ob->short_desc), 
                    get_his_her(pc));
         }//if
         else if (posn == 8) {
            Sprintf(tmp_str, "You place %S on your hands.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "places %S on %s hands.\n", 
                    &(obj.ob->short_desc), 
                    get_his_her(pc));
         }//if
         else if (posn == 9) {
            Sprintf(tmp_str, "You wield %S.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "wields %S.\n", 
                    &(obj.ob->short_desc));
         }//if
         else if (posn == 10) {
            Sprintf(tmp_str, "You grab %S.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "grabs %S.\n", 
                    &(obj.ob->short_desc));
         }//if
         else if (posn == 11) {
            Sprintf(tmp_str, "You start using %S as a light.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "starts using %S as a light.\n", 
                    &(obj.ob->short_desc));
         }//if
         else if (posn == 12) {
            Sprintf(tmp_str, "You wear %S on your body.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "wears %S on %s body.\n", 
                    &(obj.ob->short_desc), 
                    get_his_her(pc));
         }//if
         else if (posn == 13) {
            Sprintf(tmp_str, "You start wearing %S around your waist.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "starts wearing %S around %s waist.\n", 
                    &(obj.ob->short_desc), 
                    get_his_her(pc));
         }//if
         else if (posn == 14) {
            Sprintf(tmp_str, "You start wearing %S on your legs.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "starts wearing %S on %s legs.\n", 
                    &(obj.ob->short_desc), 
                    get_his_her(pc));
         }//if
         else if (posn == 15) {
            Sprintf(tmp_str, "You put %S on your feet.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "puts %S on %s feet.\n", 
                    &(obj.ob->short_desc), 
                    get_his_her(pc));
         }//if
         else if (posn == 16 || posn == 17) {
            Sprintf(tmp_str, "You place %S on your finger.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "places %S on %s finger.\n", 
                    &(obj.ob->short_desc), 
                    get_his_her(pc));
         }//if
         else if (posn == 18) {
            Sprintf(tmp_str, "You start using %S as a shield.\n", 
                    &(obj.ob->short_desc));
            Sprintf(other_msg, "starts using %S as a shield.\n", 
                    &(obj.ob->short_desc), 
                    get_his_her(pc));
         }//if
         else {
            mudlog << "ERROR:  posn out of range in obj_wear_proc, obj#: "
                   << obj.getIdNum() << " posn: " << posn << endl;
            return;
         }//else
         emote(other_msg, pc, ROOM, TRUE);
         show(tmp_str, pc);
      }//if
      else {
         Sprintf(tmp_str, "ERROR:  uncoded obj_wear_proc called: %i.\n",
                 proc_num);
         mudlog.log(ERR, tmp_str);
      }//else
   }//if
   else if (type_of_proc == OBJ_REMOVE_PROC) {
      if (proc_num == -2) { //rem_prc1, default
         Sprintf(tmp_str, "You stop using %S.\n", 
		&(obj.ob->short_desc));
         Sprintf(other_msg, "stops using %S.", 
              &(obj.ob->short_desc));
         emote(other_msg, pc, ROOM, TRUE);
         show(tmp_str, pc);
      }//if proc -2
      else {
         Sprintf(tmp_str, "ERROR:  uncoded obj_rem_proc called: %i.\n",
                 proc_num);
         mudlog.log(ERR, tmp_str);
      }//else
   }//if OBJ_REMOVE_PROC      
   else if (type_of_proc == OBJ_CONSUME_PROC) {
      if (proc_num == 0) { //consume_teleport
         do_cast_teleport(pc, pc, TRUE, 25);
      }//if
      else if (proc_num == 3) { //poison
         do_cast_poison(pc, pc, TRUE, 15);
      }//if
      else {
         Sprintf(tmp_str, "ERROR:  uncoded obj_consume_proc called: %i.\n",
                 proc_num);
         mudlog.log(ERR, tmp_str);
      }//else
   }//if
}//do_this_obj_proc


void do_buy_proc(int prc_num, critter& keeper, int i_th, 
                 const String* item, critter& pc) {
   object* obj_ptr;
   int price;
   short is_perm = FALSE;
   String buf(100);

   //log("In do_buy_proc.\n");

   if (keeper.isMob()) {
      mudlog.log(ERR, "ERROR:  keeper is MOB in do_buy_proc.\n");
      return;
   }//if

   if (keeper.mob) {
      if (keeper.mob->proc_data) {
         if (!(keeper.mob->proc_data->sh_data)) {
            mudlog.log(ERR, "ERROR:  keeper has no sh_data in do_buy_proc.\n");
            return;
         }//if
      }//if
      else {
         mudlog.log(ERR, "ERROR:  keeper has no proc_data in do_buy_proc.\n");
         return;
      }//else
   }//if
   else {
      mudlog.log(ERR, "ERROR:  keeper's mob is NULL in do_buy_proc.\n");
      return;
   }//else

   if (!item) {
      mudlog.log(ERR, "ERROR:  item NULL in buy.\n");
      return;
   }//if

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB trying to buy.\n");
      return;
   }//if

   int cur_time;
   if (prc_num == 0) { //buy proc_0
      //log("Doing buy proc_0.\n");
      cur_time = get_game_time();
      if (keeper.OPEN_TIME < keeper.CLOSE_TIME) {
         if ((cur_time < keeper.OPEN_TIME) || (cur_time > keeper.CLOSE_TIME)) {
            Sprintf(buf, "Hours are from %s to %s.\n", 
                    military_to_am(keeper.OPEN_TIME),
                    military_to_am(keeper.CLOSE_TIME));
            show(buf, pc);
            return;
         }//if
      }//if
      else {
         if ((cur_time > keeper.CLOSE_TIME) &&
             (cur_time < keeper.OPEN_TIME)) {
            Sprintf(buf, "Hours are from %s to %s.\n", 
                    military_to_am(keeper.OPEN_TIME),
                    military_to_am(keeper.CLOSE_TIME));
            show(buf, pc);
            return;
         }//if
      }//else

      //log("Looking for obj ptr...k\n");
      obj_ptr = have_obj_named(keeper.inv, i_th, item, pc.SEE_BIT,
                               ROOM);
      if (!obj_ptr) {
         obj_ptr = have_obj_named(keeper.PERM_INV, i_th, item, pc.SEE_BIT,
                                  ROOM);
         is_perm = TRUE;
      }//if
     
      if (!obj_ptr) {
         do_tell(keeper, "I don't have that in stock right now...", pc,
                 FALSE, pc.getCurRoomNum()); 
      }//if
//              /* have the object now...check for city aliances... */
//                  //if outlaw
//       else if (((pc.MISC % 10) == 4) && ((keeper.MISC % 10) != 4)) { 
//          do_tell(keeper, "I don't deal with outlaws!!", pc,
//                  FALSE, pc.getCurRoomNum());
//       }//if
      else {
         if (!obj_get_by(*obj_ptr, pc, TRUE)) {
            return;
         }//if

         price = (int)((float)(obj_ptr->PRICE) * 
                       ((float)(keeper.MARKUP) / 100.0));
         if (pc.getHomeTown() != (keeper.getHomeTown())) {
            price = (int)((float)(price) * OUT_OF_TOWN_MODIFIER);
         }//if

	 if (pc.pc && (d(1, 100) <
                       get_percent_lrnd(COMMERCE_SKILL_NUM, pc))) {
	    price = (int)((float)price * COMMERCE_SKILL_EFFECT_BUY);
          }//if

         //log("Found price.\n");
         if (price > pc.GOLD) {
            do_tell(keeper, "I don't run a charity here!!", pc, FALSE, 
		    pc.getCurRoomNum()); 
            disdain(1, Top(pc.names), keeper, ROOM);
            return;
         }//if

         //log("Good to go!\n");
         	/* good to go I believe! */
         pc.GOLD -= price;
         if (!is_perm) {
            keeper.loseInv(obj_ptr);
         }//if
         else {
            recursive_init_loads(*obj_ptr, 0);
         }
         pc.gainInv(obj_ptr);
         Sprintf(buf, "That'll be %i gold coins, here's your %S.", price,
                 name_of_obj(*obj_ptr, pc.SEE_BIT));
         do_tell(keeper, buf, pc, FALSE, pc.getCurRoomNum());
      }//else
   }//if buy proc_0
   else {
      Sprintf(buf, "ERROR:  bad proc num sent to do_buy_proc: %i.\n", 
              prc_num);
      mudlog.log(ERR, buf);
   }//else        
}//do_buy_proc


void do_vend_buy(object& vendor, int i_th, const String* item, critter& pc) {
   object* obj_ptr;
   int price;
   String buf(100);

   mudlog.log(TRC, "In do_vend_buy.\n");

   if (!item) {
      mudlog.log(ERR, "ERROR:  item NULL in buy.\n");
      return;
   }//if

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB trying to buy.\n");
      return;
   }//if

   obj_ptr = have_obj_named(vendor.ob->inv, i_th, item, pc.SEE_BIT,
                               ROOM);
   if (!obj_ptr) {
      Sprintf(buf, "%S doesn't dispense that.\n",
              long_name_of_obj(vendor, pc.SEE_BIT));
      show(buf, pc);
   }//if
   else {

      if (!obj_get_by(*obj_ptr, pc, TRUE)) {
         return;
      }//if

      price = obj_ptr->PRICE;

      if (price > pc.GOLD) {
         show("You don't have enough credits.\n", pc);
         return;
      }//if

         	/* good to go I believe! */
      pc.GOLD -= price;
      Put(&(obj_list[obj_ptr->OBJ_NUM]), pc.inv); //only dispense OBJ's
      recursive_init_loads(*obj_ptr, 0);
      Sprintf(buf, "You insert %i credits and out pops your %S.", price,
              name_of_obj(*obj_ptr, pc.SEE_BIT));
      show(buf, pc);
   }//else
}//do_vend_buy


void do_offer_proc(int prc_num, critter& keeper, int i_th, 
                 const String* item, critter& pc) {
   object* obj_ptr;
   int price;
   short will_buy = FALSE;
   String buf(100);
   
   //log("In do_buy_proc.\n");

   if (keeper.isMob()) {
      mudlog.log(ERR, "ERROR:  keeper is MOB in do_offer_proc.\n");
      return;
   }//if

   if (keeper.mob) {
      if (keeper.mob->proc_data) {
         if (!(keeper.mob->proc_data->sh_data)) {
            mudlog.log(ERR, "ERROR:  keeper has no sh_data in do_offer_proc.\n");
            return;
         }//if
      }//if
      else {
         mudlog.log(ERR, "ERROR:  keeper has no proc_data in do_offer_proc.\n");
         return;
      }//else
   }//if
   else {
      mudlog.log(ERR, "ERROR:  keeper's mob is NULL in do_offer_proc.\n");
      return;
   }//else
            

   if (!item) {
      mudlog.log(ERR, "ERROR:  NULL item sent to do_offer_proc.\n");
      return;
   }//if

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB trying to offer.\n");
      return;
   }//if

   if (prc_num == 2) { //offer proc_0
      //log("Doing offer proc_2.\n");
      price = get_game_time();
      if ((price < keeper.OPEN_TIME) || (price > keeper.CLOSE_TIME)) {
         Sprintf(buf, "Hours are from %s to %s.\n", 
                 military_to_am(keeper.OPEN_TIME),
                 military_to_am(keeper.CLOSE_TIME));
         show(buf, pc);
         return;
      }//if

      //log("Looking for obj ptr...k\n");
      obj_ptr = have_obj_named(pc.inv, i_th, item, pc.SEE_BIT,
                               ROOM);
      if (!obj_ptr) {
         Sprintf(buf, "You don't have the %S.", item);
	 show(buf, pc);
         do_tell(keeper, buf, pc, FALSE, pc.getCurRoomNum());
      }//if
//              /* have the object now...check for city aliances... */
//                  //if outlaw
//       else if (((pc.MISC % 10) == 4) && ((keeper.MISC % 10) != 4)) { 
//          do_tell(keeper, "I don't deal with outlaws!", pc, FALSE, pc.getCurRoomNum());
//       }//if
      else {
         for(int q = 50; q < 63; q++) {
            if (keeper.SHOP_DATA_FLAGS.get(q) && obj_ptr->OBJ_FLAGS.get(q)) {
               will_buy = TRUE;
               break;
            }//if
         }//if

         if (!will_buy) {
            do_tell(keeper, "I don't buy that type of stuff.", pc, 
		    FALSE, pc.getCurRoomNum());
            return;
         }//if

         price = (int)((float)(obj_ptr->PRICE) * 
                       ((float)(keeper.BUY_PERCENTAGE) / 100.0));

         if (pc.getHomeTown() != keeper.getHomeTown()) 
            price = (int)(((float)(price))  * (1.0 / OUT_OF_TOWN_MODIFIER));

         if (pc.pc && (d(1, 100) <
                       get_percent_lrnd(COMMERCE_SKILL_NUM, pc))) {
            price = (int)((float)price * COMMERCE_SKILL_EFFECT_SELL);
         }//if

         //log("Found price.\n");
         if (price > keeper.GOLD) {
            do_tell(keeper, "I'm fresh out of money, perhaps later.", pc,
                    FALSE, pc.getCurRoomNum());
            return;
         }//if

         //log("Good to go!\n");
         	/* good to go I believe! */
         Sprintf(buf, "I'd give you %i coins for that.", price);
         do_tell(keeper, buf, pc, FALSE, pc.getCurRoomNum());
      }//else
   }//if offer proc_2
   else {
      Sprintf(buf, "ERROR:  bad proc num sent to do_offer_proc: %i.\n", 
              prc_num);
      mudlog.log(ERR, buf);
   }//else        
}//do_offer_proc


void do_sell_proc(int prc_num, critter& keeper, int i_th, 
                 const String* item, critter& pc) {
   object* obj_ptr;
   int price;
   short will_buy = FALSE;
   String buf(100);

   //log("In do_sell_proc.\n");

   if (keeper.mob) {
      if (keeper.mob->proc_data) {
         if (!(keeper.mob->proc_data->sh_data)) {
            mudlog.log(ERR, "ERROR:  keeper has no sh_data in do_sell_proc.\n");
            return;
         }//if
      }//if
      else {
         mudlog.log(ERR, "ERROR:  keeper has no proc_data in do_sell_proc.\n");
         return;
      }//else
   }//if
   else {
      mudlog.log(ERR, "ERROR:  keeper's mob is NULL in do_sell_proc.\n");
      return;
   }//else
            

   if (!item) {
      mudlog.log(ERR, "ERROR:  NULL item sent to do_sell_proc.\n");
      return;
   }//if

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB trying to sell.\n");
      return;
   }//if

   if (keeper.isMob()) {
      mudlog.log(ERR, "ERROR:  keeper is MOB in do_sell_proc.\n");
      return;
   }//if

   if (prc_num == 1) { //sell proc_1
      //log("Doing sell proc_1.\n");
      price = get_game_time();
      if ((price < keeper.OPEN_TIME) || (price > keeper.CLOSE_TIME)) {
         Sprintf(buf, "Hours are from %s to %s.\n", 
                 military_to_am(keeper.OPEN_TIME),
                 military_to_am(keeper.CLOSE_TIME));
         show(buf, pc);
         return;
      }//if

      //log("Looking for obj ptr...\n");
      obj_ptr = have_obj_named(pc.inv, i_th, item, pc.SEE_BIT,
                               ROOM);
      if (!obj_ptr) {
         Sprintf(buf, "You don't have the %S.", item);
         do_tell(keeper, buf, pc, FALSE, pc.getCurRoomNum());
      }//if
//              /* have the object now...check for city aliances... */
//                  //if outlaw
//       else if (((pc.MISC % 10) == 4) && ((keeper.MISC % 10) != 4)) {
//          do_tell(keeper, "I don't deal with outlaws!", pc, FALSE, pc.getCurRoomNum()); 
//       }//if
      else {
         for(int q = 50; q < 63; q++) {
            if (keeper.SHOP_DATA_FLAGS.get(q) && obj_ptr->OBJ_FLAGS.get(q)) {
               will_buy = TRUE;
               break;
            }//if
         }//if

         if (!will_buy) {
            do_tell(keeper, "I don't buy that type of stuff.", pc, FALSE, 
		    pc.getCurRoomNum());
            return;
         }//if

         price = (int)((float)(obj_ptr->PRICE) * 
                       ((float)(keeper.BUY_PERCENTAGE) / 100.0));

         if (pc.getHomeTown() != keeper.getHomeTown()) 
            price = (int)(((float)(price))  * (1.0 / OUT_OF_TOWN_MODIFIER));

         if (pc.pc && (d(1, 100) <
                       get_percent_lrnd(COMMERCE_SKILL_NUM, pc))) {
            price = (int)((float)price * COMMERCE_SKILL_EFFECT_SELL);
         }//if


         //log("Found price.\n");
         if (price > keeper.GOLD) {
            do_tell(keeper, "I'm fresh out of money, perhaps later.", pc,
                    FALSE, pc.getCurRoomNum());
            return;
         }//if

         //log("Good to go!\n");
         	/* good to go I believe! */
         pc.loseInv(obj_ptr);
         keeper.gainInv(obj_ptr);

         Sprintf(buf, "You now have %S.\n", 
		&(obj_ptr->ob->short_desc));
         show(buf, keeper);
         Sprintf(buf, "Here's %i coins.\n", price);
         do_tell(keeper, buf, pc, FALSE, pc.getCurRoomNum());
         pc.GOLD += price;
      }//else
   }//if sell proc_1
   else {
      Sprintf(buf, "ERROR:  bad proc num sent to do_sell_proc: %i.\n", 
              prc_num);
      mudlog.log(ERR, buf);
   }//else        
}//do_sell_proc
