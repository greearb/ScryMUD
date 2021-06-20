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

/************************  batl_prc.cc  ***************************/

#include "battle.h"
#include "batl_prc.h"
#include "classes.h"
#include <bitfield.h>
#include <string2.h>
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include "command4.h"
#include "command5.h"
#include <tree2.h>
#include "misc.h"
#include "misc2.h"
#include "spells.h"
#include "trv_spll.h"
#include "skills.h"
#include "rm_spll.h"
#include "spells2.h"
#include "wep_skll.h"
#include "dam_spll.h"
#include "dam_skll.h"
#include "ez_spll.h"
#include "socials.h"
#include "social2.h"
#include "ar_skll.h"
#include <PtrArray.h>
#include "necromancer.h"


void do_entered_room_procs(critter& pc, door* dr, const char* from_dir,
                           room& rm, int& is_dead, int sanity) {

   if (mudlog.ofLevel(TRC)) {
      mudlog << __FUNCTION__ << " pc: " << pc.getName() << " room: "
             << rm.getIdNum() << endl;
   }

   String buf(100);
   Cell<stat_spell_cell*> cll;
   stat_spell_cell* ptr;
   critter* crit = &pc;

   is_dead = FALSE;
   /* check for a firewall spell */

   if (dr) {
      dr->affected_by.head(cll);
      while ((ptr = cll.next())) {
         if (crit->isMob()) { //assume they will be affected!
            crit = mob_to_smob(*crit, rm.getRoomNum());
         }//if
         if (ptr->stat_spell == FIREWALL_SKILL_NUM) {
            do_firewall_effects(*crit, is_dead);
         }
         else if (ptr->stat_spell == DISTORTION_WALL_SKILL_NUM) {
            // Increment sanity here because this may re-curse through
            // this method again. --BEN
            do_distortion_wall_effects(*crit, is_dead, sanity + 1);
         }
      }//while
   }//if

   if (!is_dead) {
      rm.checkLight();

      pc.checkForBattle(rm);
      
      if (rm.haveCritter(&pc)) {
         String cmd = "enter";
         if (from_dir) {
            String from_dir_string(from_dir);
            rm.checkForProc(cmd, from_dir_string, pc, -1);
         }
         else {
            rm.checkForProc(cmd, NULL_STRING, pc, -1);
         }
      }//if
   }//if isn't dead yet
}//do_entered_room_procs



void do_entered_room_procs(object& obj, door* dr, const char* from_dir,
                           room& rm, int sanity) {
   if (sanity < 20) {
      rm.checkLight();
   }
}//do_entered_room_procs (objects)


void do_battle_proc(critter& pc) {

  if (pc.PAUSE > 0)
    return;

  if (!pc.mob)
    return;

  // Animals won't do spec procs now. (a kicking snake looks funny!)
  if (pc.isAnimal())
     return;

  // Replaced isStunned with this, should let you perm-sleep and trip critters to
  // make them stop kicking, bashing, casting spells...
  if (!pc.isStanding())
     return;

  if (pc.isParalyzed())
     return;

  String buf(100);

  /* these integers make this more efficient, as it will be called a lot */
  int cls = pc.CLASS;
  int lvl = pc.LEVEL;
  int num_fighting = pc.IS_FIGHTING.size();
  int violence, defense, chance;

  if (pc.mob->proc_data) {
    violence = pc.mob->getSkillViolence();
    defense = pc.mob->getDefensiveness();
  }//if
  else {
    violence = defense = 0;
  }//else

  critter* primary_targ = pc.IS_FIGHTING.peekFront();

  if (!primary_targ)
    return; //why are we here??

  if (violence < -8)
    return;

  chance = d(1, 10); //expand this for more cases            

  if (lvl > 25) {
     if (chance > 5) {
        if (pc.IS_FIGHTING.size() > 6) {
           if (violence > 4) {
              if (pc.MOV > 0) {
                 do_berserk(pc);
                 pc.PAUSE--; // evil, eh? :)
              }
           }
        }
     }
  }

  if (lvl > 20) {
    if (cls == THIEF) {
      if (violence > 7) { //lets kill the weaklings/clerics!
        if ((chance > 7) && (!is_tank(pc)) && pc.EQ[9] && 
            pc.EQ[9]->OBJ_FLAGS.get(43)) {
          critter* weakest = find_weakest(pc.IS_FIGHTING);
          do_circle(*weakest, pc);
        }//if
        else if (chance > 5) {
          do_claw(*primary_targ, pc);
        }
      }//if very violent
      else if (violence > 3) {
        if (chance > 8) {
          critter* weakest = find_weakest(pc.IS_FIGHTING);
          do_bash(*weakest, pc);
        }//if
        else if (chance > 5) {
          if (num_fighting > 5) {
            do_trip(*primary_targ, pc);
          }//if
        }
      }//if not so violent
      else if (violence >= 0) {
        if (chance > 7) {
          do_trip(*primary_targ, pc);
        }//if
      }//if not so violent
    }//if a thief
    else if ((cls == WIZARD) || (cls == ALCHEMIST) || (cls == CLERIC)) {
      if (pc.MANA < 0)
        return;

      if (defense <= 0) {
        if (violence > 8) { //lets kill the weaklings/clerics!
          if (chance > 8) {
            critter* weakest = find_weakest(pc.IS_FIGHTING);
            do_cast_fireball(*weakest, pc, FALSE, 0);
          }//if
          else if (chance > 6) {
            if (num_fighting > 3) {
              do_cast_flame_strike(pc, FALSE, 0);
            }//if
            else {
              do_cast_rainbow(*primary_targ, pc, FALSE, 0);
            }//else
          }//else
          else if (chance > 4) {
            do_cast_lightning(*primary_targ, pc, FALSE, 0);
          }//if
          else if (chance > 2) {
            if (cls == CLERIC)
	     spellHolyWord.onCast(*primary_targ, pc, FALSE, 0);
            else
              spellHarm.onCast(*primary_targ, pc, FALSE, 0);            
          }//if
        }//if very violent
        else if (violence > 3) {
          if (chance > 8) {
            critter* weakest = find_weakest(pc.IS_FIGHTING);
            do_cast_rainbow(*weakest, pc, FALSE, 0);
          }//if
          else if (chance > 6) {
            if (num_fighting > 5) {
              do_cast_meteorstorm(pc, FALSE, 0);
            }//if
            else {
              do_cast_lightning(*primary_targ, pc, FALSE, 0);
            }//else
          }//if
          else if (chance > 4) {
            spellHarm.onCast(*primary_targ, pc, FALSE, 0);
          }//if
        }//if not so violent
        else if (violence >= 0) {
          if (chance > 7) {
            do_cast_fireball(*primary_targ, pc, FALSE, 0);
          }//if
          else if (chance > 4) {
            do_cast_weaken(*primary_targ, pc, FALSE, 0);
          }//if
        }//if not so violent
      }//if more offensive
      else {//is more DEFENSIVE
        if (violence > 8) { //lets kill the weaklings/clerics!
          if (chance > 8) {
            if (!is_affected_by(SANCTUARY_SKILL_NUM, pc)) {
              do_cast_sanctuary(pc, pc, FALSE, 0);
            }//if
            else {
              if ((pc.getHP() + 300) < pc.getHP_MAX()) {
                do_cast_restore(pc, pc, FALSE, 0);
              }//if
            }//else
          }//if
          else if (chance > 5) {
            do_cast_dispel_magic(*primary_targ, pc, FALSE, 0);
          }//else
          else if (chance > 3) {
            do_cast_calm(pc, FALSE, 0);
            smirk(1, &NULL_STRING, pc, ROOM);
            do_cast_teleport(pc, pc, FALSE, 0);
          }//if
          else if (chance > 2) {
            if (cls == CLERIC)
	      spellHolyWord.onCast(*primary_targ, pc, FALSE, 0);
            else
              spellHarm.onCast(*primary_targ, pc, FALSE, 0);            
          }//if
        }//if very violent
        else if (violence > 3) {
          if (chance > 8) {
            critter* weakest = find_weakest(pc.IS_FIGHTING);
            do_cast_blindness(*weakest, pc, FALSE, 0);
          }//if
          else if ((chance > 6) && ((pc.getHP() + 300) < pc.getHP_MAX())) {
            if (cls == CLERIC) {
              do_cast_restore(pc, pc, FALSE, 0);
            }//if
            else if (pc.HP < 60) {
              do_cast_teleport(pc, pc, FALSE, 0);
            }//else
          }//if
          else if ((chance > 4) && ((pc.getHP() + 100) < pc.getHP_MAX())) {
            spellHarm.onCast(*primary_targ, pc, FALSE, 0);
          }//if
        }//if not so violent
        else if (violence >= 0) {
          if ((chance > 7) && ((pc.getHP() + 300) < pc.getHP_MAX())) {
            do_cast_restore(pc, pc, FALSE, 0);
          }//if
          else if (chance > 4) {
            do_cast_armor(pc, pc, FALSE, 0);
          }//if
        }//if not so violent
      }//if more defensive
    }//if a magic user
    else {
      /* bards, warriors, rangers and all others */
      if (violence > 7) { //lets kill the weaklings/clerics!
        chance = d(1, 10); //expand this for more cases            
        if (chance > 7) {
          critter* weakest = find_weakest(pc.IS_FIGHTING);
          do_body_slam(*weakest, pc);
        }//if
        else if ((chance > 5) && (pc.MOV > 0)) {
          if (num_fighting > 3) {
            do_berserk(pc);
          }//if
          else {
            do_hurl(*primary_targ, pc);
          }//else
        }
        else if ((chance > 4) && (primary_targ->EQ[9])) {
          do_disarm(*primary_targ, pc);
        }//if
        else if (chance > 2) {
          do_bash(*primary_targ, pc);
        }//if
      }//if very violent
      else if (violence > 3) {
        chance = d(1, 10); //expand this for more cases            
        if (chance > 8) {
          critter* weakest = find_weakest(pc.IS_FIGHTING);
          do_bash(*weakest, pc);
        }//if
        else if ((chance > 6) && (pc.MOV > 0)) {
          if (num_fighting > 5) {
            do_berserk(pc);
          }//if
          else {
            do_trip(*primary_targ, pc);
          }//else
        }
        else if (chance > 4) {
          do_kick(*primary_targ, pc);
        }//if
      }//if not so violent
      else if (violence >= 0) {
        chance = d(1, 10); //expand this for more cases            
        if (chance > 7) {
          do_bash(*primary_targ, pc);
        }//if
        else if ((chance > 4) && (primary_targ->EQ[9])) {
          do_disarm(*primary_targ, pc);
        }//if
      }//if not so violent
    }//if a warrior or ranger or generic
  }//if level 20 and above
  else if (lvl > 10) {
    if (cls == THIEF) {
      if (violence > 7) { //lets kill the weaklings/clerics!
        if ((chance > 7) && (!is_tank(pc)) && pc.EQ[9] && 
            pc.EQ[9]->OBJ_FLAGS.get(43)) {
          critter* weakest = find_weakest(pc.IS_FIGHTING);
          do_circle(*weakest, pc);
        }//if
        else if (chance > 5) {
          do_claw(*primary_targ, pc);
        }
      }//if very violent
      else if (violence > 3) {
        if (chance > 8) {
          critter* weakest = find_weakest(pc.IS_FIGHTING);
          do_bash(*weakest, pc);
        }//if
        else if (chance > 5) {
          if (num_fighting > 5) {
            do_trip(*primary_targ, pc);
          }//if
        }
      }//if not so violent
      else if (violence >= 0) {
        if (chance > 7) {
          do_trip(*primary_targ, pc);
        }//if
      }//if not so violent
    }//if a thief
    else if ((cls == WIZARD) || (cls == ALCHEMIST) || (cls == CLERIC)) {
      if (pc.MANA < 0)
        return;
      if (defense <= 0) {
        if (violence > 8) { //lets kill the weaklings/clerics!
          if (chance > 8) {
            critter* weakest = find_weakest(pc.IS_FIGHTING);
            do_cast_rainbow(*weakest, pc, FALSE, 0);
          }//if
          else if (chance > 6) {
            if (num_fighting > 3) {
              do_cast_icestorm(pc, FALSE, 0);
            }//if
            else {
              do_cast_shocking_grasp(*primary_targ, pc, FALSE, 0);
            }//else
          }//else
          else if (chance > 4) {
            do_cast_lightning(*primary_targ, pc, FALSE, 0);
          }//if
          else if (chance > 2) {
            if (cls == CLERIC)
              spellHarm.onCast(*primary_targ, pc, FALSE, 0);
            else
              do_cast_blindness(*primary_targ, pc, FALSE, 0);            
          }//if
        }//if very violent
        else if (violence > 3) {
          if (chance > 8) {
            critter* weakest = find_weakest(pc.IS_FIGHTING);
            do_cast_rainbow(*weakest, pc, FALSE, 0);
          }//if
          else if (chance > 6) {
            if (num_fighting > 5) {
              do_cast_lightning_storm(pc, FALSE, 0);
            }//if
            else {
              do_cast_lightning(*primary_targ, pc, FALSE, 0);
            }//else
          }//if
          else if (chance > 4) {
            do_cast_weaken(*primary_targ, pc, FALSE, 0);
          }//if
        }//if not so violent
        else if (violence >= 0) {
          if (chance > 7) {
            do_cast_poison(*primary_targ, pc, FALSE, 0);
          }//if
          else if (chance > 4) {
            do_cast_burning_hands(*primary_targ, pc, FALSE, 0);
          }//if
        }//if not so violent
      }//if more offensive
      else {//is more DEFENSIVE
        if (violence > 8) { //lets kill the weaklings/clerics!
          if (chance > 8) {
            if (!is_affected_by(ARMOR_SKILL_NUM, pc)) {
              do_cast_armor(pc, pc, FALSE, 0);
            }//if
            else {
              if ((pc.getHP() + 100) < pc.getHP_MAX()) {
                do_cast_heal(pc, pc, FALSE, 0);
              }//if
            }//else
          }//if
          else if (chance > 5) {
            do_cast_blindness(*primary_targ, pc, FALSE, 0);
          }//else
          else if (chance > 3) {
            if (!is_affected_by(STONE_SKIN_SKILL_NUM, pc))
              do_cast_stone_skin(pc, pc, FALSE, 0);
          }//if
          else if (chance > 2) {
            if (cls == CLERIC)
              spellCauseCritical.onCast(*primary_targ, pc, FALSE, 0);
            else
              do_cast_burning_hands(*primary_targ, pc, FALSE, 0);    
          }//if
        }//if very violent
        else if (violence > 3) {
          if (chance > 8) {
            critter* weakest = find_weakest(pc.IS_FIGHTING);
            do_cast_blindness(*weakest, pc, FALSE, 0);
          }//if
          else if ((chance > 6) && ((pc.getHP() + 100) < pc.getHP_MAX())) {
            if (cls == CLERIC) {
              do_cast_heal(pc, pc, FALSE, 0);
            }//if
            else if (pc.HP < 60) {
              do_cast_cure_critical(pc, pc, FALSE, 0);
            }//else
          }//if
          else if (chance > 4) {
            do_cast_shocking_grasp(*primary_targ, pc, FALSE, 0);
          }//if
        }//if not so violent
        else if (violence >= 0) {
          if ((chance > 7) && ((pc.getHP() + 100) < pc.getHP_MAX())) {
            do_cast_heal(pc, pc, FALSE, 0);
          }//if
          else if (chance > 4) {
            do_cast_armor(pc, pc, FALSE, 0);
          }//if
        }//if not so violent
      }//if more defensive
    }//if a magic user
    else {
      /* bards, warriors, rangers and all others */
      if (violence > 7) { //lets kill the weaklings/clerics!
        chance = d(1, 10); //expand this for more cases            
        if (chance > 7) {
          critter* weakest = find_weakest(pc.IS_FIGHTING);
          do_bash(*weakest, pc);
        }//if
        else if ((chance > 5) && (pc.MOV > 0)) {
          if (num_fighting > 3) {
            do_berserk(pc);
          }//if
        }
        else if ((chance > 4) && (primary_targ->EQ[9])) {
          do_trip(*primary_targ, pc);
        }//if
        else if (chance > 2) {
          do_bash(*primary_targ, pc);
        }//if
      }//if very violent
      else if (violence > 3) {
        chance = d(1, 10); //expand this for more cases            
        if (chance > 8) {
          critter* weakest = find_weakest(pc.IS_FIGHTING);
          do_bash(*weakest, pc);
        }//if
        else if (chance > 6) {
          do_trip(*primary_targ, pc);
        }
      }//if not so violent
      else if (violence >= 0) {
        chance = d(1, 10); //expand this for more cases            
        if (chance > 7) {
          do_bash(*primary_targ, pc);
        }//if
        else if (chance > 5) {
          do_kick(*primary_targ, pc);
        }//if
      }//if not so violent
    }//if a warrior or ranger or generic
  }//if level 11 - 20
  else { //class 1-10
    if (cls == THIEF) {
      if (violence > 7) { //lets kill the weaklings/clerics!
        if ((chance > 7) && (!is_tank(pc)) && pc.EQ[9] && 
            pc.EQ[9]->OBJ_FLAGS.get(43)) {
          critter* weakest = find_weakest(pc.IS_FIGHTING);
          do_kick(*weakest, pc);
        }//if
      }//if very violent
      else if (violence >= 0) {
        if (chance > 8) {
          do_trip(*primary_targ, pc);
        }//if
      }//if not so violent
    }//if a thief
    else if ((cls == WIZARD) || (cls == ALCHEMIST) || (cls == CLERIC)) {
      if (pc.MANA < 0)
        return;
      if (defense <= 0) {
        if (violence > 8) { //lets kill the weaklings/clerics!
          if (chance > 8) {
            critter* weakest = find_weakest(pc.IS_FIGHTING);
            do_cast_dark_dart(*weakest, pc, FALSE, 0);
          }//if
          else if (chance > 6) {
            do_cast_shocking_grasp(*primary_targ, pc, FALSE, 0);
          }//if
        }//if very violent
        else if (violence > 3) {
          if (chance > 8) {
            critter* weakest = find_weakest(pc.IS_FIGHTING);
            do_cast_burning_hands(*weakest, pc, FALSE, 0);
          }//if
          else if (chance > 4) {
            do_cast_cure_serious(pc, pc, FALSE, 0);
          }//if
        }//if not so violent
        else if (violence >= 0) {
          if (chance > 9) {
            do_cast_dark_dart(*primary_targ, pc, FALSE, 0);
          }//if
          else if (chance > 6) {
            do_cast_cure_serious(pc, pc, FALSE, 0);
          }//if
        }//if not so violent
      }//if more offensive
      else {//is more DEFENSIVE
        if (violence > 8) { //lets kill the weaklings/clerics!
          if (chance > 8) {
            if (!is_affected_by(BLESS_SKILL_NUM, pc)) {
              do_cast_bless(pc, pc, FALSE, 0);
            }//if
            else {
              do_cast_cure_serious(pc, pc, FALSE, 0);
            }//else
          }//if
        }//if very violent
        else if (violence > 3) {
          if (chance > 8) {
            do_cast_cure_serious(pc, pc, FALSE, 0);
          }//if
        }//if not so violent
        else if (violence >= 0) {
          if ((chance > 9) && ((pc.getHP() + 10) < pc.getHP_MAX())) {
            do_cast_cure_serious(pc, pc, FALSE, 0);
          }//if
        }//if not so violent
      }//if more defensive
    }//if a magic user
    else {
      /* bards, warriors, rangers and all others */
      if (violence > 7) { //lets kill the weaklings/clerics!
        if (chance > 8) {
          critter* weakest = find_weakest(pc.IS_FIGHTING);
          do_kick(*weakest, pc);
        }//if
        else if (chance > 5) {
          do_kick(*primary_targ, pc);
        }//if
      }//if very violent
      else if (violence >= 0) {
        if (chance > 8) {
          do_kick(*primary_targ, pc);
        }//if
      }//if not so violent
    }//if a warrior or ranger or generic
  }//if level 0-10
}//do_battle_proc *whew*


/* path is empty upon failure, otherwise holds path */
void path_from_a_to_b(int start_room, int targ_room, List<int>& path) {
   Cell<door*> cll;
   door* ptr;
   int counter = 0;
   int tmp_rm;
   String buf(100);

   //log("In path_from_a_to_b.\n");

   Tree2<int> tree(start_room);

   Tree2Node<int>* tmp_child;
   Tree2Cell<int> par(tree);
   Tree2Cell<int> tcll;

   path.clear();

   if (targ_room == start_room) {
      return;  //already here
   }//if

   while (par) { //break out when found, or determined unfindable
      if (counter++ > 500) { //insanity check
         mudlog << "WARNING:  TRACK: Busted insanity check!, start_room: "
                << start_room << " targ_room: " << targ_room << endl;
         tcll.Head(tree);
         while (tcll) {
            counter = tcll.Next_Breadth();
            room_list[counter].setFlag(29, FALSE); //no longer in a path
               }//while
         path.clear(); //sorry, didn't find it!
         return;
      }//if

      /* par is a cell, pointing to a tree node, which contains the
         number of the current room.  This is par.Data(). */

      // Sprintf(buf, "TRACK:  Here is current room#:  %i\n", par.Data());
      // log(buf);

                 /* cll is a List<door*> cell, it will take us through
                    all of the possible exits in the room numbered 
                    by par.Data() */

      room_list[par.Data()].DOORS.head(cll);
      while ((ptr = cll.next())) {
         tmp_rm = abs(ptr->destination);

         if (!room_list[tmp_rm].getFlag(29)) {//not already in a path

            /* tmp_child will point to the newly inserted tree node */
            tmp_child = par.Push_Child(tmp_rm);   //add to tree

            // tree2.Log(); //write tree to logfile

            room_list[tmp_rm].setFlag(29, TRUE);   //now its in a path

            if (tmp_rm == targ_room) { //if found it
              //log("TRACK:  FOUND IT, here is tree:");
              //Tree2.Log();

               tcll.Head(*tmp_child); //tcll now points to the tmp_child

               while (tcll) {
                  tmp_rm = tcll.Parent();
                  path.prepend(tmp_rm);
               }//while


               tcll.Head(tree);
               while (tcll) {
                  counter = tcll.Next_Breadth();
                  room_list[counter].setFlag(29, FALSE);
                     }//while
               return;
            }//if
         }//if
      }//while, goes through each door
      par.Next_Breadth();
   }//while not done or count > 500

   // if here, then the search failed,
   // clean up mess and go home!

   //   log("TRACK:  Can't find path, or sanity check failed.\n");

   tcll.Head(tree);
   while (tcll) {
      counter = tcll.Next_Breadth();
      room_list[counter].setFlag(29, FALSE);
   }//while
   path.clear();
}//path from a to b


void a_summons_help_against_b(critter& vict, critter& pc) {
   List<critter*> tmp_lst(NULL);
   if (!vict.mob)
      return;

   /* first check room that C is in */
   Cell<critter*> cll(ROOM.getCrits());
   critter* ptr;

   while ((ptr = cll.next())) {
      if (a_will_help_b_against_c(*ptr, vict, pc)) {
         tmp_lst.append(ptr);
      }//if
   }//while
   
   tmp_lst.head(cll);
   while ((ptr = cll.next())) {
      if (ptr->isMob()) {
         ptr = mob_to_smob(*ptr, pc.getCurRoomNum());
      }//if
      do_hit(pc, *ptr);
   }//while
   
   tmp_lst.clear();
   
   int cur_room = vict.getCurRoomNum();
   int targ_room;
   Cell<door*> dcll(room_list[cur_room].DOORS);
   door* dptr;
   while ((dptr = dcll.next())) {
      if (door::findDoorByDest(room_list[(targ_room = 
                                          abs(dptr->destination))].DOORS,
                               cur_room)) {
         List<critter*> room_crits(room_list[targ_room].getCrits());
         room_crits.head(cll);
         while ((ptr = cll.next())) {
            if (a_will_help_b_against_c(*ptr, vict, pc)) {
               tmp_lst.append(ptr);
            }//if
         }//while
         
         tmp_lst.head(cll);
         while ((ptr = cll.next())) {
            if (ptr->isMob()) {
               ptr = mob_to_smob(*ptr, targ_room);
            }//if
            int is_dead = FALSE;
            ptr->trackToKill(pc, is_dead);
         }//while
         tmp_lst.clear();
      }//if
   }//while
}//a summons help agains b


/* called when a projectile comes through room, usually from throw or shoot. */
void alert_room_proc(int rm_num, int alert_type, critter& targ,
                     critter& agg) {
   List<critter*> tmp_lst(room_list[rm_num].getCrits());
   Cell<critter*> cll(tmp_lst);
   critter* ptr;
   String buf(100);

   while ((ptr = cll.next())) {
      if (alert_type == PROJECTILE_DEATH) {
         if (ptr->mob && ptr->mob->proc_data) {
            if (ptr->mob->getBadAssedness() > 3) {
               buf = "get all corpse";
               ptr->processInput(buf, FALSE, TRUE);
            }//if
            if (ptr->mob->getBadAssedness() > 8) {
               buf = "skin corpse";
               ptr->processInput(buf, FALSE, TRUE);
            }//if
            if (ptr->mob->getBadAssedness() > 6) {
               buf = "wear all";
               ptr->processInput(buf, FALSE, TRUE);
            }//if
         }//if
      }//if PROJECTILE_DEATH
   }//while
}//alert_room_proc


critter* find_weakest(List<critter*>& lst) {
   Cell<critter*> cll(lst);
   critter* ptr;
   critter* weakest = lst.peekFront();
   
   while ((ptr = cll.next())) {
      if (ptr == weakest)
         continue;
      if (ptr->isWeakerThan(*weakest)) {
         weakest = ptr;
      }//if
   }//while
   return weakest;
}//find_weakest
  

short is_tank(critter& pc) {
  if (pc.IS_FIGHTING.peekFront())
    return FALSE;

  Cell<critter*> cll(pc.IS_FIGHTING);
  critter* ptr;

  while ((ptr = cll.next())) {
    if (ptr->IS_FIGHTING.peekFront() == &pc) {
      return TRUE;
    }//if
  }//while

  return FALSE;
}//is_tank


void do_was_calmed_procs(critter& calmed, critter& calmer) {
   if (!calmer.isImmort() && calmed.mob && calmed.mob->proc_data) {

      //should range from 0-20, where higher is meaner.
      int i = abs(calmed.mob->getBenevolence() - 10);
      if (d(1, i) > 8) {
         emote("looks very pissed!!\n", calmed, room_list[calmed.getCurRoomNum()],
               TRUE);
         if (!calmed.isAnimal()) {
            if (calmed.LEVEL > 20) {
               do_body_slam(calmer, calmed);
            }//if
            else if (calmed.LEVEL > 10) {
               do_bash(calmer, calmed);
            }//else
            else {
               do_kick(calmer, calmed);
            }//else
         }
         else {
            do_hit(calmer, calmed);
         }
      }//if
        
      return;
  }//if it's a mob etc
}//do_calmed_procs


///**********************************************************************///
///************************  auxillary functions  ***********************///
///**********************************************************************///








