// $Id: spec_prc.cc,v 1.26 1999/08/29 01:17:17 greear Exp $
// $Revision: 1.26 $  $Author: greear $ $Date: 1999/08/29 01:17:17 $

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
int do_just_killed_procs(critter& agg) {

   if (agg.pc) {  //if do pc things
      if (agg.PC_FLAGS.get(12)) { //autoloot
         String all("all");
         String corpse("corpse");
         get(1, &all, 1, &corpse, agg, FALSE); //don't show msgs
         //agg.pc->input.Prepend("get all corpse\n");
         //agg.processInput(agg.pc->input, FALSE, TRUE);
      }//if
   }//if

   if (agg.mob && agg.mob->proc_data) {
     int bad_ass = agg.mob->getBadAssedness();
     String cmd("get all corpse\n");
     if (bad_ass > 7) {
        agg.processInput(cmd, FALSE, TRUE);
        cmd = "wear all\n";
        agg.processInput(cmd, FALSE, TRUE);
        cmd = "skin corpse\n";
        agg.processInput(cmd, FALSE, TRUE);
        cmd = "wear skin\n";
        agg.processInput(cmd, FALSE, TRUE);
        cmd = "growl\n";
        agg.processInput(cmd, FALSE, TRUE);
     }//if
     else if (bad_ass > 4) {
        agg.processInput(cmd, FALSE, TRUE); //Thanks, Gandolf!!
        cmd = "wear all\n";
        agg.processInput(cmd, FALSE, TRUE);
        cmd = "laugh\n";
        agg.processInput(cmd, FALSE, TRUE);
     }//if
     else if (bad_ass > 0) {
        cmd = "laugh\n";
        agg.processInput(cmd, FALSE, TRUE);
     }//if
   }//if do mob things
   return 0;
}//do_just_killed_procs


int do_shot_proc(critter& targ, critter& pc, short did_hit,
                  int& is_targ_dead) {
  /* targ is victim, but is retaliating agains the pc here */

   if (!targ.mob)
      return -1;
   if (targ.isMob()) {
     mudlog.log(ERR, "ERROR:  targ is a MOB in do_shot_proc.\n");
     return -1;
   }//if

   /* default action */
   if (!did_hit || !targ.hasAI()) {
      prone(targ);
      return 0;
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
         hit(1, pc.getFirstName(), targ);  //in case they fled to room of agg
   }//else no ranged attack
   else {
     prone(targ);
   }//else
   return 0;
}//do_shot_proc


int do_wand_scroll_proc(door* dr_ptr, int proc_num, critter& pc,
                        int spell_lvl) {
   String buf(100);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "do_wand_scroll_proc: (DOOR) " << proc_num << " pc: "
             << *(pc.getName()) << " spell_lvl: " << spell_lvl
             << " dr_ptr: " << dr_ptr << endl;
   }

   /*  Used for potions too btw.                             */
   /*  If targ is NOT NULL, then it is a SMOB or PC.         */
   /*  return value determines if should decrement the wand. */

   if (!dr_ptr) {
      mudlog << "ERROR:  dr_ptr is NULL in do_wand_scroll_proc"
             << endl;
      show("What door do you wish to target?\n", pc);
      return -1;
   }//if

   switch (proc_num)
     {
       case 21: 
         do_cast_distortion_wall(*dr_ptr, pc, TRUE, spell_lvl);
         return 0;
       case 181: 
         do_cast_passdoor(*dr_ptr, pc, TRUE, spell_lvl);
         return 0;
       default:
         Sprintf(buf, 
                 "ERROR:  default called in do_wand_scroll_proc(DR):  %i.\n", 
                 proc_num);
         mudlog.log(ERR, buf);
         show( "You call upon unknown forces, and nothing happens!\n",
              pc);
         return -1;
      }//switch
   return 0;
}//do_wand_scroll_proc (DOOR)



int do_wand_scroll_proc(int proc_num, critter& pc,
                        int spell_lvl) {
   /*  Used for potions too btw.                             */
   /*  If targ is NOT NULL, then it is a SMOB or PC.         */
   /*  return value determines if should decrement the wand. */

   if (mudlog.ofLevel(DBG)) {
      mudlog << "do_wand_scroll_proc: (NO TARG) " << proc_num << " pc: "
             << *(pc.getName()) << " spell_lvl: " << spell_lvl
             << endl;
   }

   switch (proc_num)
     {
       case 22: 
         do_cast_lightning_storm(pc, TRUE, spell_lvl);
         return 0;
       case 25: 
         do_cast_meteorstorm(pc, TRUE, spell_lvl);
         return 0;
       case 145: 
         do_cast_create_golem(pc, TRUE, spell_lvl);
         return 0;
       case 146: 
         do_cast_conjure_minion(pc, TRUE, spell_lvl);
         return 0;
       case 147: 
         do_cast_conjure_horde(pc, TRUE, spell_lvl);
         return 0;
       case 149: 
         do_cast_raise_undead(pc, TRUE, spell_lvl);
         return 0;
       case 150: 
         do_cast_illusion(pc, TRUE, spell_lvl);
         return 0;
       case 153: 
         do_cast_mirror_image(pc, TRUE, spell_lvl);
         return 0;
       case 200: 
         do_cast_group_heal(pc, TRUE, spell_lvl);
         return 0;
       case 210: 
         do_cast_create_food(pc, TRUE, spell_lvl);
         return 0;
       case 211: 
         do_cast_heros_feast(pc, TRUE, spell_lvl);
         return 0;
       case 218: 
         do_cast_create_light(pc, TRUE, spell_lvl);
         return 0;

         // Those used on rooms, always current room
       case 4: 
         do_cast_illuminate(ROOM, pc, TRUE, spell_lvl);
         return 0;
       case 6: 
         do_cast_firewall(ROOM, pc, TRUE, spell_lvl);
         return 0;
       case 12: 
         do_cast_calm(pc, TRUE, spell_lvl);
         return 0;
       case 14: 
         do_cast_mass_charm(ROOM, pc, TRUE, spell_lvl);
         return 0;
       case 17: 
         do_cast_typhoon(pc, TRUE, spell_lvl);
         return 0;
       case 18: 
         do_cast_tornado(pc, TRUE, spell_lvl);
         return 0;
       case 20: 
         do_cast_quake(pc, TRUE, spell_lvl);
         return 0;
       case 198: 
         do_cast_flame_strike(pc, TRUE, spell_lvl);
         return 0;
       case 215: 
         do_cast_icestorm(pc, TRUE, spell_lvl);
         return 0;
       case 220: 
         do_cast_firestorm(pc, TRUE, spell_lvl);
         return 0;

       default: 
       {
         String buf(100);
         Sprintf(buf, 
                 "ERROR:  dflt called in do_wand_scroll_proc(NONE):  %i.\n", 
                 proc_num);
         mudlog.log(ERR, buf);
         show( "You call upon unknown forces, and nothing happens!\n",
              pc);
         return -1;
       }//default
      }//switch
   return 0;
}//do_scroll_proc (NONE)


int do_wand_scroll_proc(object* otarg, int proc_num, critter& pc,
                        int spell_lvl) {

   if (mudlog.ofLevel(DBG)) {
      mudlog << "do_wand_scroll_proc: (OBJ) " << proc_num << " pc: "
             << *(pc.getName()) << " spell_lvl: " << spell_lvl
             << " otarg: " << otarg << endl;
      if (otarg)
         mudlog << "Targ's num " << otarg->getIdNum() << endl;
   }


   /*  Used for potions too btw.                             */
   /*  If targ is NOT NULL, then it is a SMOB or PC.         */
   /*  return value determines if should decrement the wand. */
   if (!otarg) {
      mudlog << "ERROR:  otarg is NULL in do_wand_scroll_proc"
             << endl;
      show("What object do you wish to target?\n", pc);
      return -1;
   }//if

   switch (proc_num)
     {
       case 124: 
         do_lore(*otarg, pc, FALSE);
         return 0;
       case 156: 
         do_cast_detect_poison(*otarg, pc, TRUE, spell_lvl);
         return 0;
       case 165: 
         do_cast_identify(*otarg, pc, TRUE, spell_lvl);
         return 0;
       case 182: 
         do_cast_enchant_weapon(*otarg, pc, TRUE, spell_lvl);
         return 0;
       case 183: 
         do_cast_enchant_armor(*otarg, pc, TRUE, spell_lvl);
         return 0;
       case 184: 
         do_cast_rune_edge(*otarg, pc, TRUE, spell_lvl);
         return 0;
       case 185: 
         do_cast_frost_blade(*otarg, pc, TRUE, spell_lvl);
         return 0;
       case 186: 
         do_cast_fire_blade(*otarg, pc, TRUE, spell_lvl);
         return 0;
       case 196: 
         do_cast_curse(*otarg, pc, TRUE, spell_lvl);
         return 0;
       case 213: 
         do_cast_create_water(*otarg, pc, TRUE, spell_lvl);
         return 0;
       default:
         String buf(100);
         Sprintf(buf, 
                 "ERROR:  default called in do_wand_scroll_proc(OBJ):  %i.\n", 
                 proc_num);
         mudlog.log(ERR, buf);
         show( "You call upon unknown forces, and nothing happens!\n",
              pc);
         return -1;
      }//switch
   return 0;
}//do_scroll_proc (OBJECTS)


int do_wand_scroll_proc(critter* targ, int proc_num, critter& pc,
                        int spell_lvl) {
   String buf(100);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "do_wand_scroll_proc: " << proc_num << " pc: "
             << *(pc.getName()) << " spell_lvl: " << spell_lvl
             << " targ: " << targ << endl;
      if (targ)
         mudlog << "Targ's name: " << *(targ->getName()) << endl;
   }

/*  Used for potions too btw.                             */
/*  If targ is NOT NULL, then it is a SMOB or PC.         */
/*  return value determines if should decrement the wand. */
   if (!targ) {
      mudlog << "ERROR:  targ is NULL in do_wand_scroll_proc (critter target)"
             << endl;
      show("Who do you wish to target?\n", pc);
      return -1;
   }//if

   switch (proc_num)
     {
      case 1: //heal
         do_cast_heal(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 10: 
         do_cast_web(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 11: 
         do_cast_entangle(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 30: 
         do_cast_restore(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 38: 
         do_cast_burning_hands(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 39: 
         do_cast_rainbow(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 136: 
         do_cast_shocking_grasp(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 139: 
         do_cast_dark_dart(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 140: 
         do_cast_shadows_blessing(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 151: 
         do_cast_faerie_fire(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 152: 
         do_cast_invisibility(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 155: 
         do_cast_detect_magic(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 159:
         do_cast_gate(room_list[targ->getCurRoomNum()], pc, TRUE, spell_lvl);
         return 0;
       case 160: 
         do_cast_portal(room_list[targ->getCurRoomNum()], pc, TRUE, spell_lvl);
         return 0;
       case 162: 
         do_cast_detect_hidden(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 163: 
         do_cast_detect_invisibility(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 164:
         do_cast_detect_alignment(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 171: 
         do_cast_sleep(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 172: 
         do_cast_fly(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 174: 
         do_cast_fireproof(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 175: 
         do_cast_haste(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 176: 
         do_cast_dispel_magic(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 177: 
         do_cast_strength(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 178: 
         do_cast_charm(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 179:
         do_cast_teleport(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 180: 
         do_cast_recall(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 187: 
         do_cast_bind_wound(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 188: 
         do_cast_cure_serious(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 189: 
         do_cast_cure_critical(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 191: 
         do_cast_bless(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 192: 
         do_cast_pfe(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 193: 
         do_cast_pfg(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 194: 
         do_cast_remove_curse(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 195: 
         do_cast_dispel_good(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 197: 
         do_cast_harm(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 199: 
         do_cast_holy_word(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 201:
         do_cast_divine_protection(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 202: 
         do_cast_magic_shield(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 203: 
         do_cast_armor(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 204: 
         do_cast_stone_skin(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 205: 
         do_cast_sanctuary(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 206: 
         do_cast_prismatic_globe(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 208: 
         do_cast_absorb_blows(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 217:
         do_cast_lightning(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 219: //fireball
         do_cast_fireball(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 221: 
         do_cast_blindness(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 222:
         do_cast_cause_sickness(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 224: 
         do_cast_cause_critical(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 225: 
         do_cast_weaken(*targ, pc, TRUE, spell_lvl);
         return 0;
       case 226: 
         do_cast_poison(*targ, pc, TRUE, spell_lvl);
         return 0;
         //case 227: //TODO:  code this up! 
         //do_cast_plague(*targ, pc, TRUE, spell_lvl);
         //return 0;
       case 228: //this is not the error it looks like, see dam_spll.cc
         do_cast_dispel_good(*targ, pc, TRUE, spell_lvl);
         return 0;
       default:
         Sprintf(buf, 
                 "ERROR:  dflt called in do_wand_scroll_proc(CRIT):  %i.\n", 
                 proc_num);
         mudlog.log(ERR, buf);
         show( "You call upon unknown forces, and nothing happens!\n",
              pc);
         return 0;
      }//switch
   return 0;
}//do_scroll_proc


int do_pulsed_spec_procs(int first_room, int last_room) {
   Cell<critter*> cll;
   critter* ptr;
   int sz, i;
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

         if (mudlog.ofLevel(DBG2)) {
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
                      << *(ptr->getName()) << endl;
            }
            if (ptr->isInBattle()) {
               //Already pretty busy!!
               continue;
            }
            if (ptr->MOB_FLAGS.get(1)) { //scavenge
               if (d(1,100) <= 10) {
                  if ((sz = room_list[i].getInv().size())) { //objs to pick up?
                     
                     int attempted = d(1, sz) - 1; //translate to zero-based!
                     obj_ptr = room_list[i].getInv().elementAt(attempted);
 
                     if (!obj_ptr) {
                        if (mudlog.ofLevel(ERR)) {
                           mudlog << "ERROR:  obj_ptr NULL in scavenge proc, rm:"
                                  << i << " inv_size: " << sz << " attempted: "
                                  << attempted << endl;
                        }//if
                        return -1;
                     }//if
  
                     Sprintf(gtobj, "get %S\n", obj_ptr->getFirstName());
 
                     ptr->processInput(gtobj, FALSE, TRUE);
                     if (!ptr->isAnimal() && !ptr->isMonster()) {
                        ptr->processInput(wr_all, FALSE, TRUE);
                     }

                  }//if random chance
               }//if
            }// if scavenge
            if (ptr->isWanderer() && !ptr->isTracking() && !ptr->isRunningScript() &&
                room_list[i].haveCritter(ptr)) {
               if (d(1, 10) > 8) {
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

               }//if chance succeedes, 40% chance
            }//if wander
	    if (ptr->isTracking() && room_list[i].haveCritter(ptr)) {
               is_dead = FALSE;
	       ptr->doHuntProc(d(1, ptr->DEX/2), is_dead);
	    }//if hunting
            else if (ptr->shouldBeHoming() && room_list[i].haveCritter(ptr)) {
               is_dead = FALSE;
               //mudlog << "WARNING: doing travelToRoom from spec_prc.cc" << endl;
               ptr->travelToRoom( ptr->getHomeRoom(), d(1, ptr->DEX/2),
                                 is_dead);
            }
         }//while, won't leave a room untill all have been dealt with
      }//if room isn't locked
   }//for, loops through rooms

   First_Room = last_room;
   Last_Room = last_room + (Cur_Max_Room_Num / 10);
   if (First_Room > Cur_Max_Room_Num) {
      First_Room = 0;
      Last_Room = Cur_Max_Room_Num / 10;
   }//if
   return 0;
}//do_pulsed


// TODO:  Translations problems with the emotes here...
// Just does messages, no real actions.
int do_this_obj_proc(int type_of_proc, int proc_num, critter& pc,
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
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "starts wearing %S on %s head.\n", 
                    obj.getShortDesc(), get_his_her(pc));
         }//if
         else if (posn == 2 || posn == 3) {
            Sprintf(tmp_str, "You start wearing %S around your neck.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "starts wearing %S around %s neck.\n", 
                    obj.getShortDesc(), 
                    get_his_her(pc));
         }//if
         else if (posn == 4) {
            Sprintf(tmp_str, "You start wearing %S about your body.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "starts wearing %S about %s body.\n", 
                    obj.getShortDesc(), 
                    get_his_her(pc));
         }//if
         else if (posn == 5) {
            Sprintf(tmp_str, "You start wearing %S on your arms.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "starts wearing %S on %s arms.\n", 
                    obj.getShortDesc(),
                    get_his_her(pc));
         }//if
         else if (posn == 6 || posn == 7) {
            Sprintf(tmp_str, "You start wearing %S around your wrist.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "starts wearing %S around %s wrist.\n", 
                    obj.getShortDesc(), 
                    get_his_her(pc));
         }//if
         else if (posn == 8) {
            Sprintf(tmp_str, "You place %S on your hands.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "places %S on %s hands.\n", 
                    obj.getShortDesc(), 
                    get_his_her(pc));
         }//if
         else if (posn == 9) {
            Sprintf(tmp_str, "You wield %S.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "wields %S.\n", 
                    obj.getShortDesc());
         }//if
         else if (posn == 10) {
            Sprintf(tmp_str, "You grab %S.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "grabs %S.\n", 
                    obj.getShortDesc());
         }//if
         else if (posn == 11) {
            Sprintf(tmp_str, "You start using %S as a light.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "starts using %S as a light.\n", 
                    obj.getShortDesc());
         }//if
         else if (posn == 12) {
            Sprintf(tmp_str, "You wear %S on your body.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "wears %S on %s body.\n", 
                    obj.getShortDesc(), 
                    get_his_her(pc));
         }//if
         else if (posn == 13) {
            Sprintf(tmp_str, "You start wearing %S around your waist.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "starts wearing %S around %s waist.\n", 
                    obj.getShortDesc(), 
                    get_his_her(pc));
         }//if
         else if (posn == 14) {
            Sprintf(tmp_str, "You start wearing %S on your legs.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "starts wearing %S on %s legs.\n", 
                    obj.getShortDesc(), 
                    get_his_her(pc));
         }//if
         else if (posn == 15) {
            Sprintf(tmp_str, "You put %S on your feet.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "puts %S on %s feet.\n", 
                    obj.getShortDesc(), 
                    get_his_her(pc));
         }//if
         else if (posn == 16 || posn == 17) {
            Sprintf(tmp_str, "You place %S on your finger.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "places %S on %s finger.\n", 
                    obj.getShortDesc(), 
                    get_his_her(pc));
         }//if
         else if (posn == 18) {
            Sprintf(tmp_str, "You start using %S as a shield.\n", 
                    obj.getShortDesc(&pc));
            Sprintf(other_msg, "starts using %S as a shield.\n", 
                    obj.getShortDesc(), 
                    get_his_her(pc));
         }//if
         else {
            mudlog << "ERROR:  posn out of range in obj_wear_proc, obj#: "
                   << obj.getIdNum() << " posn: " << posn << endl;
            return -1;
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
		obj.getShortDesc(&pc));
         Sprintf(other_msg, "stops using %S.", 
              obj.getShortDesc());
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
   return 0;
}//do_this_obj_proc


int do_buy_proc(int prc_num, critter& keeper, int i_th, 
                const String* item, critter& pc) {
   object* obj_ptr = NULL;
   int price;
   short is_perm = FALSE;
   String buf(100);

   //log("In do_buy_proc.\n");

   if (keeper.isMob()) {
      mudlog.log(ERR, "ERROR:  keeper is MOB in do_buy_proc.\n");
      return -1;
   }//if

   if (keeper.mob) {
      if (keeper.mob->proc_data) {
         if (!(keeper.mob->proc_data->sh_data)) {
            mudlog.log(ERR, "ERROR:  keeper has no sh_data in do_buy_proc.\n");
            return -1;
         }//if
      }//if
      else {
         mudlog.log(ERR, "ERROR:  keeper has no proc_data in do_buy_proc.\n");
         return -1;
      }//else
   }//if
   else {
      mudlog.log(ERR, "ERROR:  keeper's mob is NULL in do_buy_proc.\n");
      return -1;
   }//else

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB trying to buy.\n");
      return -1;
   }//if

   // Only support one buy proc at this time, and probably forever!
   if (prc_num == 0) { //buy proc_0
      //log("Doing buy proc_0.\n");
      if (!keeper.isOpen(get_game_time(), TRUE, pc)) {
         return -1;
      }

      //log("Looking for obj ptr...k\n");
      object* tmp_optr = NULL;

      if (item->Strlen() > 0) {
         obj_ptr = have_obj_named(keeper.inv, i_th, item, pc.SEE_BIT,
                                  ROOM);

         if (!obj_ptr) {
            int first_cnt = obj_named_count(keeper.inv, item, pc.SEE_BIT, ROOM);
            obj_ptr = have_obj_named(keeper.PERM_INV, (i_th - first_cnt), item,
                                     pc.SEE_BIT, ROOM);
            is_perm = TRUE;
         }//if
     
      }
      else {
         // Maybe they are trying to get it by the index....gotta reproduce the code
         // that listed them like this...  Cut-And-Pasted from the list_merchandise()
         // method in command2.cc

         SCell<object*> cell(keeper.inv);
         static int item_counts[NUMBER_OF_ITEMS + 1];
         
         memset(item_counts, 0, sizeof(int) * (NUMBER_OF_ITEMS + 1));
         
         // Now, find the instance count.
         while ((tmp_optr = cell.next())) {
            item_counts[tmp_optr->getIdNum()]++;
         }//while
         
         keeper.PERM_INV.head(cell);
         while ((tmp_optr = cell.next())) {
            item_counts[tmp_optr->getIdNum()]++;
         }//while
         
         keeper.inv.head(cell);
         int id_num;
         int cnt = 0;
         while ((tmp_optr = cell.next())) {
            
            id_num = tmp_optr->getIdNum();
            
            if (!tmp_optr->isModified() &&
                (item_counts[id_num] == -1)) { //already done it
               continue;
            }
            
            if (detect(pc.SEE_BIT, (tmp_optr->OBJ_VIS_BIT | ROOM.getVisBit()))) {
               price = keeper.findItemSalePrice(*tmp_optr, pc);
               
               if (price < 0) {
                  continue; //buf = "  NOT FOR SALE NOW.";
               }//if
               
               cnt++;
               if (i_th == cnt) {
                  obj_ptr = tmp_optr;
                  break;
               }

               item_counts[id_num] = -1;               
            }//if detectable
         }//while

         keeper.PERM_INV.head(cell);
         while ((tmp_optr = cell.next())) {
            
            id_num = tmp_optr->getIdNum();
            
            if (!tmp_optr->isModified() &&
                (item_counts[id_num] == -1)) { //already done it
               continue;
            }
            
            if (detect(pc.SEE_BIT, (tmp_optr->OBJ_VIS_BIT | ROOM.getVisBit()))) {
               price = keeper.findItemSalePrice(*tmp_optr, pc);
               
               if (price < 0) {
                  continue; //buf = "  NOT FOR SALE NOW.";
               }//if
               
               cnt++;
               if (i_th == cnt) {
                  obj_ptr = tmp_optr;
                  break;
               }

               item_counts[id_num] = -1;
               
            }//if detectable

         }//while
      }//else, tried to get it by index only.

      if (!obj_ptr) {
         do_tell(keeper, "I don't have that in stock right now...", pc,
                 FALSE, pc.getCurRoomNum()); 
      }//if
      else {
         // This will do messages if needed.
         if (!obj_get_by(*obj_ptr, pc, TRUE)) {
            return -1;
         }//if

         price = keeper.findItemSalePrice(*obj_ptr, pc);
         
         if (price < 0) {
            do_tell(keeper, "I can't sell that to you now, inventory problem.\n",
                    pc, FALSE, pc.getCurRoomNum());
            return -1;
         }//if

         if (price > pc.GOLD) {
            do_tell(keeper, "I don't run a charity here!!", pc, FALSE, 
		    pc.getCurRoomNum()); 
            disdain(1, pc.getFirstName(), keeper, ROOM);
            return -1;
         }//if

         // Deal with gold.
         pc.GOLD -= price;

         // Only put the gold back in circulation for Player-run keepers.
         if (keeper.isPlayerShopKeeper()) {
            keeper.GOLD += price;
         }//if
         else {
            // Give half back into the game so others can sell their stuff.
            keeper.GOLD += price/2;
         }

         if (!is_perm) {
            keeper.loseInv(obj_ptr);
         }//if
         else {
            recursive_init_loads(*obj_ptr, 0);
         }

         Sprintf(buf, "That'll be %i gold coins, here's your %S.", price,
                 name_of_obj(*obj_ptr, pc.SEE_BIT));
         do_tell(keeper, buf, pc, FALSE, pc.getCurRoomNum());
         int del_obj = FALSE;
         gain_eq_effects(*obj_ptr, NULL, pc, FALSE, FALSE, del_obj);
         if (!del_obj) {
            pc.gainInv(obj_ptr);
         }

         if (keeper.isPlayerShopKeeper()) {
            save_player_shop_owner(keeper);
         }

      }//else
   }//if buy proc_0
   else {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  bad proc num sent to do_buy_proc: "
                << prc_num << endl;
      }//if
   }//else
   return 0;
}//do_buy_proc


int do_vend_buy(object& vendor, int i_th, const String* item, critter& pc) {
   object* obj_ptr;
   int price;
   String buf(100);

   mudlog.log(TRC, "In do_vend_buy.\n");

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB trying to buy.\n");
      return -1;
   }//if

   obj_ptr = have_obj_named(vendor.inv, i_th, item, pc.SEE_BIT, ROOM);
   if (!obj_ptr) {
      Sprintf(buf, "%S doesn't dispense that.\n",
              long_name_of_obj(vendor, pc.SEE_BIT));
      show(buf, pc);
   }//if
   else {

      if (!obj_get_by(*obj_ptr, pc, TRUE)) {
         return -1;
      }//if

      price = obj_ptr->PRICE;

      if (price > pc.GOLD) {
         show("You don't have enough gold.\n", pc);
         return -1;
      }//if

         	/* good to go I believe! */
      pc.GOLD -= price;
      int del_obj = FALSE;
      recursive_init_loads(*obj_ptr, 0);
      Sprintf(buf, "You insert %i coins and out pops %S.", price,
              obj_ptr->getLongName(&pc));
      show(buf, pc);
      gain_eq_effects(obj_list[obj_ptr->OBJ_NUM], NULL, pc, FALSE, FALSE, del_obj);
      if (!del_obj) {
         pc.gainInv(&(obj_list[obj_ptr->OBJ_NUM])); //only dispense OBJ's
      }
      return 0;
   }//else
   return -1;
}//do_vend_buy


int do_offer_proc(int prc_num, critter& keeper, int i_th, 
                  const String* item, critter& pc) {
   object* obj_ptr;
   int price;
   short will_buy = FALSE;
   String buf(100);
   
   //log("In do_buy_proc.\n");

   if (keeper.isMob()) {
      mudlog.log(ERR, "ERROR:  keeper is MOB in do_offer_proc.\n");
      return -1;
   }//if

   if (keeper.mob) {
      if (keeper.mob->proc_data) {
         if (!(keeper.mob->proc_data->sh_data)) {
            mudlog.log(ERR, "ERROR:  keeper has no sh_data in do_offer_proc.\n");
            return -1;
         }//if
      }//if
      else {
         mudlog.log(ERR, "ERROR:  keeper has no proc_data in do_offer_proc.\n");
         return -1;
      }//else
   }//if
   else {
      mudlog.log(ERR, "ERROR:  keeper's mob is NULL in do_offer_proc.\n");
      return -1;
   }//else
            

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB trying to offer.\n");
      return -1;
   }//if

   if (prc_num == 2) { //offer proc_0
      //log("Doing offer proc_2.\n");
      if (!keeper.isOpen(get_game_time(), TRUE, pc)) {
         return -1;
      }//if

      //log("Looking for obj ptr...k\n");
      obj_ptr = have_obj_named(pc.inv, i_th, item, pc.SEE_BIT,
                               ROOM);
      if (!obj_ptr) {
         Sprintf(buf, "You don't have the %S.", item);
	 show(buf, pc);
         do_tell(keeper, buf, pc, FALSE, pc.getCurRoomNum());
      }//if
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
            return -1;
         }//if

         price = keeper.findItemBuyPrice(*obj_ptr, pc);

         if (price < 0) {
            do_tell(keeper, "I don't buy that type of stuff.", pc,
                    FALSE, pc.getCurRoomNum());
            return -1;
         }//if

         //log("Good to go!\n");
         	/* good to go I believe! */
         Sprintf(buf, "I'd give you %i coins for that.", price);
         do_tell(keeper, buf, pc, FALSE, pc.getCurRoomNum());
         return 0;
      }//else
   }//if offer proc_2
   else {
      Sprintf(buf, "ERROR:  bad proc num sent to do_offer_proc: %i.\n", 
              prc_num);
      mudlog.log(ERR, buf);
   }//else        
   return -1;
}//do_offer_proc


int do_sell_proc(int prc_num, critter& keeper, int i_th, 
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
            return -1;
         }//if
      }//if
      else {
         mudlog.log(ERR, "ERROR:  keeper has no proc_data in do_sell_proc.\n");
         return -1;
      }//else
   }//if
   else {
      mudlog.log(ERR, "ERROR:  keeper's mob is NULL in do_sell_proc.\n");
      return -1;
   }//else
            
   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB trying to sell.\n");
      return -1;
   }//if

   if (keeper.isMob()) {
      mudlog.log(ERR, "ERROR:  keeper is MOB in do_sell_proc.\n");
      return -1;
   }//if

   if (prc_num == 1) { //sell proc_1
      //log("Doing sell proc_1.\n");
      if (!keeper.isOpen(get_game_time(), TRUE, pc)) {
         return -1;
      }

      //log("Looking for obj ptr...\n");
      obj_ptr = have_obj_named(pc.inv, i_th, item, pc.SEE_BIT,
                               ROOM);
      if (!obj_ptr) {
         Sprintf(buf, "You don't have the %S.", item);
         do_tell(keeper, buf, pc, FALSE, pc.getCurRoomNum());
      }//if
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
            return -1;
         }//if

         price = keeper.findItemBuyPrice(*obj_ptr, pc);
         
         if (price < 0) {
            do_tell(keeper, "I don't buy that type of stuff.", pc, FALSE, 
		    pc.getCurRoomNum());
            return -1;
         }//if

         //log("Found price.\n");
         if (price > keeper.GOLD) {
            do_tell(keeper, "I'm fresh out of money, perhaps later.", pc,
                    FALSE, pc.getCurRoomNum());
            return -1;
         }//if

         Sprintf(buf, "You now have %S.\n", 
		obj_ptr->getShortDesc(&keeper));
         show(buf, keeper);
         Sprintf(buf, "Here's %i coins.", price);
         do_tell(keeper, buf, pc, FALSE, pc.getCurRoomNum());

         pc.GOLD += price;
         keeper.GOLD -= price;

         drop_eq_effects(*obj_ptr, pc, FALSE, FALSE);
         pc.loseInv(obj_ptr);

         int del_obj = FALSE;
         gain_eq_effects(*obj_ptr, NULL, pc, FALSE, FALSE, del_obj);
         if (!del_obj) {
            pc.gainInv(&(obj_list[obj_ptr->OBJ_NUM])); //only dispense OBJ's
         }

         if (keeper.isPlayerShopKeeper()) {
            save_player_shop_owner(keeper);
         }
         return 0;
      }//else
   }//if sell proc_1
   else {
      Sprintf(buf, "ERROR:  bad proc num sent to do_sell_proc: %i.\n", 
              prc_num);
      mudlog.log(ERR, buf);
   }//else        
   return -1;
}//do_sell_proc

