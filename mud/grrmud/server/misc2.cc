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

///*************************** misc2.cc ****************************///
///********  Headers for this file are found in misc2.h   ************///
///********  Bitfield masks defined here as well.         ************///

#include "misc.h"
#include "misc2.h"
#include <stdio.h>
#include <string.h>
#include "classes.h"
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include "spec_prc.h"
#include "batl_prc.h"
#include "spells.h"
#include "skills.h"
#include <PtrArray.h>
#include "vehicle.h"
#include <sys/types.h>
#include <signal.h>
#include <LogStream.h>
#include "SkillSpell.h"
#include <string.h>

// Attempt to load the player-run shop owner of that number.
// Returns a newly allocated SMOB, or NULL if a problem is
// encountered.
critter* load_player_shop_owner(int mob_num) {
   switch (config.useMySQL) {
#ifdef USEMYSQL
      case true:
         return db_load_player_shop_owner(mob_num);
         break;
#endif
      case false:
         return file_load_player_shop_owner(mob_num);
         break;
   }
   return NULL;
}
#ifdef USEMYSQL
critter* db_load_player_shop_owner(int mob_num) {
   critter* sk = new critter();
   sk->dbRead(mob_num, 0, true);
   sk->short_cur_stats[26] = 1;
   affected_mobs.append(sk);
   return sk;
}
#endif
critter* file_load_player_shop_owner(int mob_num) {
   String buf(100);

   if (mob_list[mob_num].isInUse()) {
      // Find the file name of our shopkeeper.
      Sprintf(buf, "./PlayerShops/%S_%i", mob_list[mob_num].getShortName(),
              mob_num);

      ifstream dafile(buf);
      
      if (dafile) {
         critter* sk = new critter();
         sk->fileRead(dafile, TRUE); //read all inventory.

         // Make sure it's labeled as a SMOB
         sk->short_cur_stats[26] = 1; /* make it a SMOB */
         affected_mobs.append(sk);

         return sk;
      }//if
      else {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  Could not open Player Shop Keeper -:"
                   << buf << ":- for reading." << endl;
         }//if
         return NULL;
      }//else
   }//if
   else {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  load_player_shop_owner, mob_num: "
                << mob_num << " is not in use." << endl;
      }//if
      return NULL;
   }//else
}//load_player_shop_owner


int save_player_shop_owner(critter& pc) {
   String buf(100);

   if (!pc.isPlayerShopKeeper()) {
      mudlog << "ERROR:  save_player_shop_owner, mob: " 
             << pc.getName() << " id_num: " << pc.getIdNum()
             << " is not a shop keeper." << endl;
      return FALSE;
   }

   if (mob_list[pc.getIdNum()].isInUse()) {
      // Find the file name of our shopkeeper.
      Sprintf(buf, "./PlayerShops/%S_%i", mob_list[pc.getIdNum()].getShortName(),
              pc.getIdNum());

      ofstream dafile(buf);
      
      if (dafile) {
         pc.Write(dafile);
         return TRUE;
      }//if
      else {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  Could not open Player Shop Keeper -:"
                   << buf << ":- for writing." << endl;
         }//if
         return FALSE;
      }//else
   }//if
   else {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  save_player_shop_owner, mob_num: "
                << pc.getIdNum() << " is not in use." << endl;
      }//if
      return FALSE;
   }//else
}//save_player_shop_owner

object* load_player_box(int box_num) {
#ifdef USEMYSQL
   if (config.useMySQL)
      return db_load_player_box(box_num);
   else
#endif
      return file_load_player_box(box_num);
}

#ifdef USEMYSQL
object* db_load_player_box(int box_num) {
   object* box = new object();
   box->dbRead(box_num, -1, true);
   return box;
}
#endif

object* file_load_player_box(int box_num) {
   String buf(100);

   if (obj_list[box_num].isInUse()) {
      Sprintf(buf, "./PlayerSacks/%i", box_num);

      ifstream dafile(buf);
      
      if (dafile) {
         object* box = new object();
         box->fileRead(dafile, TRUE);

         return box;
      }
      else {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  Could not open Player Box -:"
                   << buf << ":- for reading." << endl;
         }
         return NULL;
      }
   }
   else {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  load_player_box, box_num: "
                << box_num << " is not in use." << endl;
      }
      return NULL;
   }
}

int save_player_box(object& box) {
   String buf(100);

   if(obj_list[box.getIdNum()].isInUse()) {
      Sprintf(buf, "./PlayerSacks/%i", box.getIdNum());

      ofstream dafile(buf);

      if(dafile) {
         box.Write(dafile);
         return TRUE;
      }
      else {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  Could not open Player Owned Box -:"
                   << box.getIdNum() << ":- for writing." << endl;
         }
         return FALSE;
      }
   }
   else {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  save_player_box, obj_num: "
                << box.getIdNum() << " is not in use." << endl;
      }
      return FALSE;
   }
}

int core_dump(const char* msg) {
   cerr << "In my_assert, msg:  " << msg << endl;
   mudlog << "ERROR:  Dumping core on purpose..." << endl
          << flush;
   //From the great Katrina McClelan!
   //TODO:  Add waidpid etc...
   kill(getpid(),SIGSEGV); /* this'll dump core */
   sleep(10); /* make sure it stops here */

   return TRUE;

//    if (!fork()) {
//       /* child copy starts here */
//       /* dead by here */
//       kill(getpid(),SIGSEGV); /* this'll dump core */
//       sleep(10); /* make sure it stops here */
//    }
//    else {
//       cerr << "fork failed:  " << strerror(errno) << endl;
//    }
//    return TRUE;
}//core_dump
        /* parent continues unaware */


short a_will_help_b_against_c(critter& a, critter& b, critter& c) {
  if (!a.mob || !a.mob->proc_data || a.INT1)
    return FALSE;

  if (a.mob->getSocialAwareness() < 2)
    return FALSE;

  int should = a.mob->getSocialAwareness() * 10 + 5;
  /* should a like b? */
  if (a.CLASS == b.CLASS)
    should += 50;

  if (a.RACE == b.RACE)
    should += 75;

  if (abs(abs(a.ALIGN) - (abs(b.ALIGN))) < 300) 
    should += 90;

  if (a.ALIGN > 300)
    should += 35;

  /* now figure out why a should like c */
  if (a.CLASS == c.CLASS)
    should -= 50;

  if (a.RACE == c.RACE)
    should -= 75;

  if (abs(abs(a.ALIGN) - (abs(b.ALIGN))) < 300) 
    should -= 90;

  should -= d(1, (40 - a.LEVEL) * 4); //generally don't wanna help out..

  if (should > 0)
    return TRUE;
  else
    return FALSE;
}//a_helps_b_against_c

  
void lose_fly(critter& pc, short do_msg) {
  int is_dead;
  if (do_msg) {
    show("You glance down just in time to see the ground rushing up to meet you.\n", pc);
    pc.emote("suddenly falls out of the sky.\n");
  }

  if (ROOM.needsFly() && (ROOM.getFallTo() != 0)) {
    pc.doGoToRoom(ROOM.getFallTo(), "from the sky", NULL, is_dead, 
                  pc.getCurRoomNum(), 1, TRUE);
  }

  if (!is_dead)
    look(1, &NULL_STRING, pc, TRUE);
}//lose_fly

void unpetrify(critter& pc, short do_msg) {
  if (do_msg)
    show("Your skin returns to its origional rubbery texture.\n", pc);
  pc.CRIT_FLAGS.turn_off(14);
}//unpetrify

void show_stat_affects(object& obj, critter& pc) {
   Cell<stat_spell_cell*> cll(obj.stat_affects);
   stat_spell_cell* ptr;
   String buf(100);

   if (obj.stat_affects.isEmpty()) {
      pc.show("NONE\n");
   }
   
   while ((ptr = cll.next())) {
      if (ptr->stat_spell == 1) {
         Sprintf(buf, "Strength[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 2) {
         Sprintf(buf, "Inteligence[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 3) {
         Sprintf(buf, "Constitution[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 4) {
         Sprintf(buf, "Charisma[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 5) {
         Sprintf(buf, "Wisdom[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 6) {
         Sprintf(buf, "Dexterity[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 7) {
         Sprintf(buf, "Hit (chance)[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 8) {
         Sprintf(buf, "Damage[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 9) {
         Sprintf(buf, "AC (armor)[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 10) {
         Sprintf(buf, "Attacks[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 15) {
         Sprintf(buf, "HP (current)[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 16) {
         Sprintf(buf, "Mana (current)[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 17) {
         Sprintf(buf, "Movement (current)[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 18) {
         Sprintf(buf, "Alignment[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 22) {
         Sprintf(buf, "Practices[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 23) {
         Sprintf(buf, "HP (max)[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 24) {
         Sprintf(buf, "Mana (max)[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 25) {
         Sprintf(buf, "Movement (max)[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 27) {
         Sprintf(buf, "Damage Recieved Modifier[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 28) {
         Sprintf(buf, "Damage Given Modifier[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 29) {
         Sprintf(buf, "Heat Resistance[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 30) {
         Sprintf(buf, "Cold Resistance[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 31) {
         Sprintf(buf, "Electrical Resistance[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 32) {
         Sprintf(buf, "Spell Resistance[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 35) {
         Sprintf(buf, "Bare Hand Dice Count[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 36) {
         Sprintf(buf, "Bare Hand Dice Sides[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 37) {
         Sprintf(buf, "HP Regeneration[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 38) {
         Sprintf(buf, "Mana Regeneration[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 39) {
         Sprintf(buf, "Movement Regeneration[%i]:%P25 %i\n",
                 ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 100) {
         Sprintf(buf, "Hunger[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 101) {
         Sprintf(buf, "Thirst[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 102) {
         Sprintf(buf, "Drugged[%i]:%P25 %i\n", ptr->stat_spell, ptr->bonus_duration);
         show(buf, pc);
      }//if
      else {
         Sprintf(buf, "Unknown [%i]:, value: [%i]\n", ptr->stat_spell,
                 ptr->bonus_duration);
         show(buf, pc);
      }//if

  }//while
}//show_stat_affects


/** Tests this performs:  A(!animal)
 *                        B(!pc in_battle)  b (! in mob room)
 *                        C(owns aux_crit) F(frozen)
 *                        G(!gagged) I(is immort) K(know spell), M(has mana), 
 *                        m(!mob, smob ok), N(!magic),
 *                        P(paralyzed), R(owns aux_rm), r(resting or standing)
 *                        S(is_standing), V(!violence), Z(room is zlocked)
 *  Syntax is:  If test is TRUE, then action can be done.  Note that some
 *     of the conditions are negative logic.  Thus, 'A' will be TRUE if
 *     the critter is NOT an ANIMAL.
 *
 *  Case matters.
 */
int ok_to_do_action(critter* vict, const char* flags, int spell_num,
                    critter& pc, room* aux_rm, critter* aux_crit,
                    int do_msg) {
  String buf(100);
  int len = strlen(flags);  
  char chr;
  int mana_cost = 0;

  // mana_cost = SSCollection::instance().getSS(spell_num).getManaCost();
  mana_cost = get_mana_cost(spell_num, pc);

  if (pc.isMob()) {
     Sprintf(buf, "ERROR:  mob casting spell# %i.\n", spell_num);
     mudlog.log(ERROR, buf);
     return FALSE;
  }//if

  for (int i = 0; i<len; i++) {
     chr = flags[i];
     if (chr == 'M') {
        if (mana_cost > pc.MANA) {
           if (do_msg) {
              show("You don't have enough energy to cast this spell!\n", pc);
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'A') {
        if (pc.getClass() == ANIMAL) {
           if (do_msg) {
              show("Animals can't do that!\n", pc);
           }//if
           return FALSE;
        }//if
     }//if    
     else if (chr == 'K') {
        if (get_percent_lrnd(spell_num, pc) <= 0) {
           if (do_msg) {
              show("You don't know where to begin!\n", pc);
           }//if
           return FALSE;
        }//if
     }//if    
     else if (chr == 'm') {
        if (pc.isMob()) {
           if (do_msg) {
              mudlog << "ERROR: ok_to_do_action, got a MOB (not SMOB).\n";
              pc.show("ERROR:  mob trying to do an action.\n");
           }//if
           return FALSE;
        }//if
     }//if    
     else if (chr == 'G') {
        if (pc.isGagged()) {
           if (do_msg) {
              show("You have been gagged.\n", pc);
              return FALSE;
           }//if
        }
     }
     else if (chr == 'P') {
        if (pc.CRIT_FLAGS.get(14)) {
           if (do_msg) {
              show("You are paralyzed!\n", pc);
           }//if
           return FALSE;
        }//if
     }//if    
     else if (chr == 'F') {
        if (pc.pc && pc.PC_FLAGS.get(0)) {
           if (do_msg) {
              show("You are frozen!\n", pc);
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'r') {
        if (pc.POS > POS_REST) {
           if (do_msg) {
              show("You must be in a more lively position!\n", pc);
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'B') {
        if (!pc.IS_FIGHTING.isEmpty()) {
           if (do_msg) {
              show("You can't concentrate enough in battle!\n", pc);
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'b') {
        if (ROOM.isNoMob()) {
           if (do_msg) {
              show("NPC's cannot be here.\n", pc);
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'N') {
        if (ROOM.isNoMagic()) {
           if (do_msg) {
              show("Magic cannot function here!\n", pc);
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'S') {
        if (pc.POS != POS_STAND) {
           if (do_msg) {
              show("You must be standing in order to do this.\n", pc);
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'R') {
        if (!aux_rm || !pc.doesOwnRoom(*aux_rm)) {
           if (do_msg) {
              pc.show("You do not own that room.\n");
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'Z') {
        if (!aux_rm || !aux_rm->isZlocked()) {
           if (do_msg) {
              show("You cannot edit a room that is not locked.\n", pc);
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'C') {
        if (!pc.doesOwnCritter(*aux_crit)) {
           if (do_msg) {
              pc.show("You do not own that critter.\n");
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'I') {
        if (!pc.isImmort()) {
           if (do_msg) {
              pc.show("Ehh??\n");
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'V') {
        if (ROOM.isZlocked() && !(pc.doesOwnRoom(ROOM) || pc.getImmLevel() > 8)) {
           if (do_msg) {
              pc.show("You don't own this room, and the zone is locked.\n");
           }
           return FALSE;
        }
        if (!pc.isImmort()) {
           if (ROOM.isHaven() && (&pc != vict)) {
              if (do_msg) {
                 show("Violence is not permitted here.\n", pc);
              }//if
              return FALSE;
           }//if
           else if (ROOM.isNoPK() && vict && vict->pc && (&pc != vict)) {
              if (do_msg) {
                 show("Player killing is not allowed here.\n", pc);
              }//if
              return FALSE;
           }//if
        }//if
     }//if
  }//for
  return TRUE;
}//ok_to_do_action


critter* check_for_diversions(critter& pc, char* tests, critter& agg) {
  char chr;
  int i;
  String buf(100);
   
  if (!tests)
    return &pc;

  for (i = 0; (chr = tests[i]); i++) {
    if (chr == 'S') {
      if (pc.temp_crit && pc.SHIELDED_BY && !pc.SHIELDED_BY->isSleeping()) {
        if (d(1, 175) > pc.SHIELDED_BY->DEX + 
            get_percent_lrnd(SHIELD_SKILL_NUM, *(pc.SHIELDED_BY))) {
          if (ROOM.haveCritter(pc.SHIELDED_BY)) {
             return pc.SHIELDED_BY;
          }//if
        }//if
      }//if
    }//if
    else if (chr == 'G') {
      if (pc.temp_crit) {
        if (pc.GUARDED_BY && !pc.GUARDED_BY->isSleeping()) {
          if (d(1, 175) > pc.GUARDED_BY->DEX +
              get_percent_lrnd(GUARD_SKILL_NUM, *(pc.GUARDED_BY))) {
            if (ROOM.haveCritter(pc.GUARDED_BY)) {
               return pc.GUARDED_BY;
            }
          }//if
        }//if
      }//if
    }//if Guard
    else if (chr == 'M') {
      if (pc.mirrors > 0) {
        if (d(1, pc.mirrors) > 1) {
          pc.mirrors--;
          Sprintf(buf, "%S shatters a mirror image of you.\n",
                  name_of_crit(agg, pc.SEE_BIT));
          buf.Cap();
          show(buf, pc);
          Sprintf(buf, "breaks a mirror image of %S.",
                  name_of_crit(pc, ~0));
          emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &pc);
          return NULL;
        }//if hit a mirror
      }//if has mirrors
    }//if mirror check
  }//for 

  return &pc;
}//check_for_diversions


stat_spell_cell* is_affected_by(int spell_num, const critter& pc) {
  Cell<stat_spell_cell*> cll(pc.affected_by);
  stat_spell_cell* ptr;

  while ((ptr = cll.next())) {
    if (ptr->stat_spell == spell_num)
      return ptr;
  }//while

  return NULL;
}//is_affected_by


stat_spell_cell* is_affected_by(int spell_num, const object& obj) {
  Cell<stat_spell_cell*> cll(obj.affected_by);
  stat_spell_cell* ptr;

  while ((ptr = cll.next())) {
    if (ptr->stat_spell == spell_num)
      return ptr;
  }//while

  return NULL;
}//is_affected_by (obj)


stat_spell_cell* has_stat_affect(int stat_num, object& obj) {
  Cell<stat_spell_cell*> cll(obj.stat_affects);
  stat_spell_cell* ptr;

  while ((ptr = cll.next())) {
    if (ptr->stat_spell == stat_num)
      return ptr;
  }//while

  return NULL;
}//has_stat_affect


const char* get_opposite_dir(const char* dir) {
   int i = strlen(dir);

   if (strncasecmp(dir, "north", i) == 0)
      return "south";
   else if (strncasecmp(dir, "east", i) == 0)
      return "west";
   else if (strncasecmp(dir, "south", i) == 0)
      return "north";
   else if (strncasecmp(dir, "west", i) == 0)
      return "east";
   else if (strncasecmp(dir, "northeast", i) == 0)
      return "southwest";
   else if (strncasecmp(dir, "southeast", i) == 0)
      return "northwest";
   else if (strncasecmp(dir, "southwest", i) == 0)
      return "northeast";
   else if (strncasecmp(dir, "northwest", i) == 0)
      return "southeast";
   else if (strncasecmp(dir, "up", i) == 0)
      return "down";
   else if (strncasecmp(dir, "down", i) == 0)
      return "up";
   else 
      return "UNKNOWN";
}//get_opposite_dir


void leave_room_effects(room& rm, critter& pc, int check_proc) {
   Cell<door*> cll(rm.DOORS);
   door* ptr;

   while ((ptr = cll.next())) {
      if (ptr->crit_blocking == &pc) {
         ptr->dr_data->door_data_flags.turn_off(14); //no longer blocked
         ptr->crit_blocking = NULL;
      }//if
   }//while

   pc.CRIT_FLAGS.turn_off(22); //no longer hiding or melding

   if (pc.pc) {
      pc.PC_FLAGS.turn_off(17);
   }//if
   rm.checkLight(FALSE);

   if (check_proc) {
      String cmd = "exit";
      rm.checkForProc(cmd, NULL_STRING, pc, -1);
   }
}//leave_room_effects


void leave_room_effects(room& rm, object& obj) {

   rm.checkLight(FALSE);

   //String cmd = "exit";
   //rm.checkForProc(cmd, NULL_STRING, pc, -1);

}//leave_room_effects (Objects)



const String* single_obj_name(object& obj, int see_bit) {
   if (detect(see_bit, obj.OBJ_VIS_BIT))
      return obj.names.peekFront();
   else
      return &SOMETHING; //global 'someone' String
}//single_obj_name


const char* get_himself_herself(critter& pc) {
   if (pc.SEX == 0)
      return "herself";
   else if (pc.SEX == 1) 
      return "himself";
   else return "itself";
}//

const char* get_dude_chic(critter& pc) {
   if (pc.SEX == 0)
      return "chick";
   else if (pc.SEX == 1) 
      return "dude";
   else return "being";
}//

const char* get_fellow_lady(critter& pc) {
   if (pc.SEX == 0)
      return "lady";
   else if (pc.SEX == 1) 
      return "fellow";
   else return "being";
}//

const char* get_him_her(critter& pc) {
   if (pc.SEX == 0)
      return "her";
   else if (pc.SEX == 1) 
      return "him";
   else return "it";
}//


int is_grouped(critter& crit, critter& pc) {
   Cell<critter*> cll(pc.GROUPEES);
   critter* ptr;

   while ((ptr = cll.next())) {
      if (ptr == &crit)
         return TRUE;
   }//while

   crit.GROUPEES.head(cll);
   while ((ptr = cll.next())) {
      if (ptr == &pc)
         return TRUE;
   }//while
   return FALSE;
}//is_grouped



int get_race_num(const char* name) {
   if (strcasecmp(name, "undead") == 0)
      return UNDEAD;
   if (strcasecmp(name, "animal") == 0)
      return ANIMAL;
   if (strcasecmp(name, "monster") == 0)
      return MONSTER;
   if (strcasecmp(name, "human") == 0)
      return HUMAN;
   if (strcasecmp(name, "anitre") == 0)
      return ANITRE;
   if (strcasecmp(name, "avintre") == 0)
      return AVINTRE;
   if (strcasecmp(name, "darkling") == 0)
      return DARKLING;
   if (strcasecmp(name, "drow") == 0)
      return DROW;
   if (strcasecmp(name, "dragon") == 0)
      return DRAGON;
   if (strcasecmp(name, "dwarf") == 0)
      return DWARF;
   if (strcasecmp(name, "ironclad") == 0)
      return IRONCLAD;
   if (strcasecmp(name, "orgue") == 0)
      return OGRUE;
   if (strcasecmp(name, "rocktroll") == 0)
      return ROCKTROLL;
   if (strcasecmp(name, "elf") == 0)
      return ELF;
   if (strcasecmp(name, "faerie") == 0)
      return FAERIE;
   if (strcasecmp(name, "entity") == 0)
      return ENTITY;
   if (strcasecmp(name, "sombrian") == 0)
      return SOMBRIAN;
   if (strcasecmp(name, "avian") == 0)
      return AVIAN;
   return OTHER_RACE;
}//get race_num



int get_class_num(const char* name) {
   if (strcasecmp(name, "warrior") == 0) 
      return WARRIOR;
   if (strcasecmp(name, "sage") == 0) 
      return SAGE;
   if (strcasecmp(name, "wizard") == 0) 
      return WIZARD;
   if (strcasecmp(name, "ranger") == 0) 
      return RANGER;
   if (strcasecmp(name, "thief") == 0) 
      return THIEF;
   if (strcasecmp(name, "alchemist") == 0) 
      return ALCHEMIST;
   if (strcasecmp(name, "cleric") == 0) 
      return CLERIC;
   if (strcasecmp(name, "bard") == 0) 
      return BARD;
   if (strcasecmp(name, "necromancer") == 0) 
      return NECROMANCER;
   return OTHER_CLASS;
}//get class num



const char* get_race_name(int num) {
   if (num == OTHER_RACE)
      return "other";
   if (num == UNDEAD)
      return "undead";
   if (num == ANIMAL)
      return "animal";
   if (num == MONSTER)
      return "monster";
   if (num == HUMAN)
      return "human";
   if (num == ANITRE)
      return "anitre";
   if (num == AVINTRE)
      return "avintre";
   if (num == DARKLING)
      return "darkling";
   if (num == DROW)
      return "drow";
   if (num == DRAGON)
      return "dragon";
   if (num == DWARF)
      return "dwarf";
   if (num == IRONCLAD)
      return "ironclad";
   if (num == OGRUE)
      return "ogrue";
   if (num == ROCKTROLL)
      return "rocktroll";
   if (num == ELF)
      return "elf";
   if (num == FAERIE)
      return "faerie";
   if (num == ENTITY)
      return "entity";
   if (num == SOMBRIAN)
      return "sombrian";
   if (num == AVIAN)
      return "avian";
   return "UNKNOWN";
}//get race name

const char* get_class_name(int num) {
   if (num == OTHER_CLASS) 
      return "other";
   if (num == WARRIOR)
      return "warrior";
   if (num == SAGE)
      return "sage";
   if (num == WIZARD)
      return "wizard";
   if (num == RANGER)
      return "ranger";
   if (num == THIEF)
      return "thief";
   if (num == ALCHEMIST)
      return "alchemist";
   if (num == CLERIC)
      return "cleric";
   if (num == BARD)
      return "bard";
   if (num == NECROMANCER)
      return "necromancer";
   return "UNKNOWN";
}//get class name


int max(int i, int j) {
  if (i > j)
    return i;
  return j;
}

int min(int i, int j) {
   if (i < j)
      return i;
   return j;
}

void save_all() {
  Cell<critter*> cll(pc_list);
  critter* ptr;

  //log("In save_all.\n");

  while ((ptr = cll.next())) {
    if (ptr->MODE == MODE_NORMAL) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "save_all:  Name of crit -:" << *(name_of_crit(*ptr, ~0))
                << ":-" << endl;
      }
      ptr->save();
    }//if
  }//while

}//save all


say_proc_cell* have_topic_named(List<say_proc_cell*> lst, const String& msg) {
  Cell<say_proc_cell*> cll(lst);
  say_proc_cell* ptr;

  while ((ptr = cll.next())) {
    if (strcasecmp(ptr->topic, msg) == 0) {
      return ptr;
    }//if
  }//while

  return NULL;
}//have_topic_named


void strip_hegemon_tags(String& str) {
   // If we find a tag, delete it.  Maybe do more interesting things
   // later.
   int len = str.Strlen();
   String retval(len);
   char ch;
   int in_tag = false;
   int prev_was_lt = false;

   for (int i = 0; i<len; i++) {
      ch = str[i];
      if (ch == '<') {
         if (in_tag) {
            continue;
         }
         else {
            if (prev_was_lt) {
               prev_was_lt = FALSE;
               retval += ch;
            }
            else {
               prev_was_lt = TRUE;
            }
         }//else
      }//if
      else {
         if (prev_was_lt) {
            in_tag = TRUE;
         }

         if (ch == '>') {
            prev_was_lt = FALSE;
            if (in_tag) {
               in_tag = FALSE;
            }
            else {
               retval += ch;
            }
         }
         else {
            prev_was_lt = FALSE;
            if (in_tag) {
               continue;
            }
            else {
               retval += ch;
            }
         }//else
      }//else
   }//for
   str = retval;
}//strip_hegemon_tags


void parse_communication(String& str) {
   int sofar = 0;
   int len = str.Strlen();
   char ch;
   int max_len = 380;
   
   // First, check for censored strings.  This is a lot of work
   // for the computer, and I hate censoring, but experience has
   // shown it to be needed.  Edit the CensoredStrings in const.cc
   // to modify this...

   String tmp(str);
   tmp.Tolower(); //make it all lower-case

   //const char* incoming = tmp; //will cast to char*

   // Skip censorship for now. --BEN
   //const char* censored;
   //for (int i = 0; (censored = CensoredStrings[i]); i++) {
   //   if (strstr(incoming, censored)) {
   //      str = "CENSORED";
   //      return;
   //   }
   //}

   String retval(len + 5);

   for (int i = 0; i<len; i++) {
      ch = str[i];
      if (++sofar > max_len) {
         // retval needs to have a newline inserted before the current
         // spot. (retval.Strlen())
         int retval_len = retval.Strlen();
         for (int j = retval_len; j > (retval_len - max_len); j--) {
            if (isspace(retval[j])) {
               retval.setCharAt(j, '\n');
               max_len = 380;
               sofar = 0;
               break;
            }//if
         }//for
         if (sofar > 0) { //didn't find a space
            retval += '\n';
            max_len = 380;
            sofar = 0;
         }
      }//if
      if (ch == '<') {
         retval += "<<";
      }
      else {
         if (ch == '\n') {
            max_len = 380;
            sofar = 0;
         }
         retval += ch;
      }
   }//for
   str = retval;
}//parse_communication


void parse_for_max_80(String& str) {
   int start = 0;
   int sofar = 0;
   int len = str.Strlen();
   int i;
   short found_it;

   while (TRUE) {
      found_it = FALSE;
      i = start;
      if ((i + 79) >= len) {
         break;//done
      }//if
      while ((i < len) && (str[i] != '\n') && (i < (start + 80))) {
         i++;
      }//while
      sofar = i;
      if ((sofar - start) > 79) { //gotta fix it
         while (i > start) {
            i--;
            if (str[i] == ' ') {
               str.setCharAt(i, '\n');
               i++; //move past newline
               start = i;
               found_it = TRUE;
               break;
            }//if
         }//while
         if (!found_it) { //there wasn't a space
            str.setCharAt((i + 78), '\n'); 
            start = i + 79;
         }//if
      }//if need fixing
      else {
         start = sofar + 1;
      }//else
   }//while true

   if (len == 0)
      return; //hack

   for (i = 0; i < (len - 1); i++) {
      if ((str[i] == '~') && (isspace(str[i+1]))) {
         str.setCharAt(i, ' ');
      }//if
   }//for

   if (str[len - 1] == '~')
       str.setCharAt((len - 1), ' ');

}//parse_for_max_80()


short name_is_secret(const String* name, door& dr) {
   Cell<String*> cll(dr.dr_data->names);
   String* ptr;
   int len = name->Strlen();

   if (len == 0)
      return FALSE;

   if (dr.destination >= 0) { //if positive, go from top
      ptr = cll.next();
      while ((ptr = cll.next())) {
         if (*ptr == "#")
            break; // didn't find it
         if (strncasecmp(*name, *ptr, len) == 0)
            return TRUE;
      }//while
      return FALSE;
   }//if
   else {
      ptr = cll.prev();
      while ((ptr = cll.prev())) {
         if (*ptr == "#")
            break; // didn't find it
         if (strncasecmp(*name, *ptr, len) == 0)
            return TRUE;
      }//while
      return FALSE;
   }//else
}//name_is_secret

// May return a NULL!
String* dir_of_room(room& rm, int dest_rm_num) {
   Cell<door*> cll(rm.DOORS);
   door* ptr;

   while ((ptr = cll.next())) {
      if (abs(ptr->destination) == abs(dest_rm_num)) {
         return direction_of_door(*ptr);
      }//if
   }//while

   return NULL;
}//dir_of_room


int get_next_msg_num(object& board) {
   Cell<object*> cll(board.inv);
   object* ptr;
   int sofar = 0;
   int i;

   while ((ptr = cll.next())) {
      i = atoi(*(ptr->names.peekFront()));
      if (i > sofar)
         sofar = i;
   }//while

   return sofar + 1;
}//get_next_msg_num


String owner_of_zone(int rm_num) {
   int znum = room_list[rm_num].getZoneNum();

   return ZoneCollection::instance().elementAt(znum).getFirstOwner();

}//owner_of_zone


void do_vehicle_moves() {
   Cell<room*> cll;
   pulsed_proc_rooms.head(cll); //declared in grrmud.cc 
   room* rm_ptr;
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In do_vehicle_moves, pulsed_proc_rooms.Size():  "
             << pulsed_proc_rooms.size() << endl;
   }

   while ((rm_ptr = cll.next())) {
      //if (mudlog.ofLevel()) {
      //   mudlog << "Checking on room/vehicle number:  " 
      //          << rm_ptr->getRoomNum() << endl;
      //}
      if (rm_ptr->isVehicle() && rm_ptr->isInUse() && !rm_ptr->isZlocked()) {
         vehicle* veh_ptr = (vehicle*)(rm_ptr);
         if (veh_ptr->isSelfGuided()) { //its not pc-guided

            veh_ptr->decrementTicksTillNextStop();
            if (veh_ptr->getTicksTillNextStop() <= 0) {

               veh_ptr->move();
            }//if
         }//if
      }//if
   }//while
}//do_vehicle_moves()


int mob_can_enter(critter& pc, room& rm, short do_msg, int check_no_wander) {
   int retval = FALSE;

   if (!rm.isInUse()) { //if not used
      if (do_msg)
         show("That room doesn't really exist!!\n", pc);
   }//if
   else if ((rm.isNoMortal() && !pc.isImmort())) {
      if (do_msg) {
         pc.show("Mortals are not allowed there!!\n");
      }
   }
   else if ((rm.needsBoat()) && //need boat
            (!(pc.CRIT_FLAGS.get(4) || pc.CRIT_FLAGS.get(3)))) {
      if (do_msg)
         show("You need a boat to go there.\n", pc);
   }//if
   else if ((rm.needsDive()) && //need underwater ability
            (!pc.canDive())) {
      if (do_msg)
         show("You must be able to dive to go there.\n", pc);
   }//if
   else if ((rm.needsClimb()) && //need climb
            (!(pc.isFlying() || pc.canClimb()))) {
      if (do_msg) {
         show("You lose your footing and almost fall!!\n", pc);
         emote("slips and almost falls!!\n", pc, ROOM, TRUE);
         pc.PAUSE = 1; //punish them a bit
      }//if
   }//if
   else if ((rm.needsFly()) && //need fly
            (!(pc.CRIT_FLAGS.get(3)))) {
      if (do_msg)
         show("You have to be flying to go there.\n", pc);
   }//if
   else if ((rm.isNoMob()) && // !mob?
               (!pc.isPc())) {
      if (do_msg)
         show("Mobs are not allowed to move there.\n", pc); //never see prob.
   }//if !mob
   else if (!pc.isPc() && check_no_wander && 
            (rm.isNoWanderMob() || 
             (rm.isNoWanderForeignMob() && (pc.getHomeTown() != rm.getZoneNum())))) {
      if (do_msg)
         show("Mobs are not allowed to wander there.\n", pc); //never see prob.
   }//if !mob
   else if (rm.isZlocked()) {
      if (pc.isImmort()) {
         if (rm.getZoneNum() != 0) {
            if ((pc.getImmLevel() <= 2) &&
                (!ZoneCollection::instance().elementAt(rm.getZoneNum()).isOwnedBy(pc))) {
               if (do_msg) {
                  pc.show("Immorts may no longer visit other areas that are\n");
                  pc.show("locked.  If you have a legit reason, ask a higher\n");
                  pc.show("builder or coder for permission.\n");
               }//if
               return FALSE;
            }//if
            else {
               return TRUE;
            }
         }//if
         else {
            return TRUE;
         }
      }//if is Immort
      else {
         if (do_msg)
            show("That direction has been locked to your kind.\n", pc);
         return FALSE;
      }
   }//if
   else
      retval = TRUE;
   return retval;
}//mob_can_enter



void out_stat_list(const List<stat_spell_cell*>& lst, critter& pc,
                   const BitfieldNames& names) {
   Cell<stat_spell_cell*> cll(lst);
   stat_spell_cell* ptr;
   String buf(100);
   String buf2(100);

   if (lst.isEmpty()) {
      pc.show("NONE\n");
      return;
   }

   while ((ptr = cll.next())) {
      Sprintf(buf, "(%s %i) ", names.getName(ptr->stat_spell),
              ptr->bonus_duration);
      buf2.Append(buf);
   }//while
   buf2.Append("\n");
   pc.show(buf2);
}//out_stat_list


void out_spell_list(const List<stat_spell_cell*>& lst, critter& pc, int
        show_ostat_dat_flag) {
   Cell<stat_spell_cell*> cll(lst);
   stat_spell_cell* ptr;
   String buf(100);
   String buf2(100);

   if (lst.isEmpty()) {
      pc.show("NONE\n");
      return;
   }

   while ((ptr = cll.next())) {

       // If being called from ostat, show spell level. 
       if (show_ostat_dat_flag) {
           Sprintf(buf, "([snum: %d][lvl: %i] \"%s\") ",
                   ptr->stat_spell,
                   ptr->bonus_duration,
                   SSCollection::instance().getNameForNum(ptr->stat_spell));
       } else {
           Sprintf(buf, "(%s) ",
                   SSCollection::instance().getNameForNum(ptr->stat_spell));
       }

      buf2.Append(buf);
   }//while
   buf2.Append("\n");
   pc.show(buf2);
}//out_spell_list


object* have_obj_numbered(const List<object*>& lst, const int i_th,
                          const int obj_num, const int see_bit,
                          const room& rm) {
   Cell<object*> cll(lst);
   object* ptr;
   int count = 0;

   while ((ptr = cll.next())) {
      if (ptr->OBJ_NUM == obj_num) {
         if (detect(see_bit, (ptr->OBJ_VIS_BIT | rm.getVisBit()))) { 
            count++;
            if (count == i_th) {
               return ptr;
            }//if
         }//if detect
      }//if obj nums agree
   }//while
   return NULL;
}//have_obj_numbered


int get_game_time() {
   return config.hour;
}//get_game_time


const char* get_month(int day) {
   if (day <= 31) 
      return "January";
   else if (day <= 60)
      return "February";
   else if (day <= 91)
      return "March";
   else if (day <= 120)
      return "April";
   else if (day <= 150)
      return "May";
   else if (day <= 181)
      return "June";
   else if (day <= 211)
      return "July";
   else if (day <= 242)
      return "August";
   else if (day <= 273)
      return "September";
   else if (day <= 303)
      return "October";
   else if (day <= 334)
      return "November";
   else if (day <= 365)
      return "December";
   else {
      mudlog.log(ERROR, "ERROR:  day is > 365, in get_month.\n");
      return "Leap Month";
   }//else
}//get_month
      

const int get_day_of_month(int day) { //day of the year that is
   if (day <= 31) 
      return day;
   else if (day <= 60)
      return day - 31;
   else if (day <= 91)
      return day - 60;
   else if (day <= 120)
      return day - 91;
   else if (day <= 150)
      return day - 120;
   else if (day <= 181)
      return day - 150;
   else if (day <= 211)
      return day - 181;
   else if (day <= 242)
      return day - 211;
   else if (day <= 273)
      return day - 242;
   else if (day <= 303)
      return day - 273;
   else if (day <= 334)
      return day - 303;
   else if (day <= 365)
      return day - 334;
   else {
      mudlog.log(ERROR, "ERROR:  day is > 365, get_day_of_month.\n");
      return 32;
   }//else
}//get_day_of_month

SeasonType get_season(int day){
   if (day <= 31)
   return winter;
      else if (day <= 60)
   return winter;
	   else if (day <= 91)
   return spring;
      else if (day <= 120)
   return spring;
      else if (day <= 150)
   return spring;
      else if (day <= 181)
   return summer;
      else if (day <= 211)
   return summer;
      else if (day <= 242)
   return summer;
      else if (day <= 273)
   return fall;
      else if (day <= 303)
   return fall;
      else if (day <= 334)
   return fall;
      else if (day <= 365)
   return winter;
      else return winter; //leap day

}		

const char* military_to_am(int m_time) {
   if (m_time == 0)
      return "1 am";
   else if (m_time == 1)
      return "2 am";
   else if (m_time == 2)
      return "3 am";
   else if (m_time == 3)
      return "4 am";
   else if (m_time == 4)
      return "5 am";
   else if (m_time == 5)
      return "6 am";
   else if (m_time == 6)
      return "7 am";
   else if (m_time == 7)
      return "8 am";
   else if (m_time == 8)
      return "9 am";
   else if (m_time == 9)
      return "10 am";
   else if (m_time == 10)
      return "11 am";
   else if (m_time == 11)
      return "12 pm";
   else if (m_time == 12)
      return "1 pm";
   else if (m_time == 13)
      return "2 pm";
   else if (m_time == 14)
      return "3 pm";
   else if (m_time == 15)
      return "4 pm";
   else if (m_time == 16)
      return "5 pm";
   else if (m_time == 17)
      return "6 pm";
   else if (m_time == 18)
      return "7 pm";
   else if (m_time == 19)
      return "8 pm";
   else if (m_time == 20)
      return "9 pm";
   else if (m_time == 21)
      return "10 pm";
   else if (m_time == 22)
      return "11 pm";
   else if (m_time == 23)
      return "12 am";
   else {
      mudlog.log(ERROR, "ERROR:  m_time out of range, military_to_am.\n");
      return "0 am";
   }//else
}//military_to_am


room* get_next_room(int zone_num) {

   for (int i = ZoneCollection::instance().elementAt(zone_num).getBeginRoomNum();
        i <= ZoneCollection::instance().elementAt(zone_num).getEndRoomNum();
        i++) {
      if (!room_list[i].isInUse()) { //if room is not used
         room_list[i].setRoomNum(i); //This really shouldn't be needed!
         return &(room_list[i]); //ptr to that room
      }//if
   }//for
   return NULL; //there was no free room
}//get_next_room()


int get_next_obj() { 
   for (int i = 10; i<NUMBER_OF_ITEMS; i++) { 
      if (!(obj_list[i].OBJ_FLAGS.get(10))) {
         return i;
      }//if
   }//for
   return -1; //there was no free room
}//get_next_obj()


int get_next_door() { 
   for (int i = 10; i<NUMBER_OF_DOORS; i++) { 
      if (!(door_list[i].door_data_flags.get(10))) {
         return i;
      }//if
   }//for
   return -1; //there was no free door
}//get_next_door()


int get_next_mob() { 
   for (int i = 10; i<NUMBER_OF_MOBS; i++) { 
      if (!(mob_list[i].CRIT_FLAGS.get(18))) {
         return i;
      }//if
   }//for
   return -1; //there was no free mob
}//get_next_obj()


int get_percent_lrnd(int skill_num, const critter& pc, short automatic) {
   if (!pc.pc) {
      if (automatic && pc.mob && pc.mob->proc_data) {
         if (pc.FLAG1.get(12)) { //evasive maneuvers flag
            if (skill_num == DODGE_SKILL_NUM)
               return (70 + pc.LEVEL);
            if (skill_num == PARRY_SKILL_NUM)
               return (70 + pc.LEVEL);
         }//if
      }//if automatic (ie should search)
      return ((int)(2.5*pc.LEVEL));
   }//if not a pc
   
   int retval;
   if (static_cast< Tree<int,int> >(pc.SKILLS_KNOWN).Find(skill_num, retval)) {
      return retval;
   }//if
   
   return -1;
}//%lrnd_skill


void increment_percent_lrnd(int skill_num, critter& pc) {

   if (!pc.pc)
     return;

   float inc;
   int dif = SSCollection::instance().getSS(skill_num).getDifficulty();
   
   inc = (33.0/(float)dif) * (((float)(pc.INT)/4.0) * ((float)(pc.INT)/4.0) +
                              (float)(pc.INT)/2.0);
   inc = max((int)(inc), 1);

   int p_lrnt;
   if (pc.SKILLS_KNOWN.Find(skill_num, p_lrnt)) {
      p_lrnt += (int)inc;
      if (p_lrnt > 100)
        p_lrnt = 100;
      pc.SKILLS_KNOWN.Insert(skill_num, p_lrnt); //update value
      return;
   }//if
   mudlog.log(ERROR,
              "ERROR:  tried to increment_%_lrnd on a skill unknown by pc.\n");
}//inc_skill


void init_masks() {
            /*  OBJ_WEAR_FLAGS  */
   int i;
   for (i = 22; i < (21 + MAX_EQ); i++) { 
      Obj_Wear_Flags_Mask.turn_on(i);
   }

            /*  OBJ_CONSUME_PROCS_FLAGS  */
   Obj_Consume_Procs_Mask.turn_on(0); //teleport
   Obj_Consume_Procs_Mask.turn_on(3); //poison

            /*  OBJ_WEAR_PROCS  */
   //Obj_Wear_Procs_Mask.turn_on(??); //no special ones coded yet

            /*  OBJ_REMOVE_PROCS  */
   //Obj_Remove_Procs_Mask.turn_on(??); //no special ones coded yet

   //Shop_Data_Buy_Procs_Mask.turn_on(0);   //buy_proc_0

   //Shop_Data_Sell_Procs_Mask.turn_on(1);   //sell_proc_0

   //Shop_Data_Offer_Procs_Mask.turn_on(2);   //offer_proc_0

}//init masks


String* direction_of_door(const door& drr) {
   String* ptr = NULL;
   if (drr.destination < 0)
      ptr = drr.dr_data->names.peekRear();
   else
      ptr = drr.dr_data->names.peekFront();

   if (ptr)
      return ptr;
   else
      return &UNKNOWN;
}//direction_of_door


const char* abbrev_dir_of_door(const door& drr) {
   String* dir;

   if (drr.destination < 0) 
      dir = drr.dr_data->names.peekRear();
   else
      dir = drr.dr_data->names.peekFront();

   if (dir == NULL) {
      return "??";
   }//if

   if (strcasecmp(*dir, "north") == 0) 
      return "N";
   else if (strcasecmp(*dir, "northwest") == 0)
      return "NW";
   else if (strcasecmp(*dir, "northeast") == 0)
      return "NE";
   else if (strcasecmp(*dir, "east") == 0)
      return "E";
   else if (strcasecmp(*dir, "south") == 0)
      return "S";
   else if (strcasecmp(*dir, "southeast") == 0)
      return "SE";
   else if (strcasecmp(*dir, "southwest") == 0)
      return "SW";
   else if (strcasecmp(*dir, "west") == 0)
      return "W";
   else if (strcasecmp(*dir, "up") == 0)
      return "U";
   else if (strcasecmp(*dir, "down") == 0)
      return "D";
   else return *dir;
}//abbrev_dir_of_door


int obj_count(List<object*>& lst, object& src) {
   int retval = 0;
   Cell<object*> cll(lst);
   object* ptr;

   while ((ptr = cll.next())) {
      if (ptr->OBJ_NUM == src.OBJ_NUM) {
         retval++;
      }//if
   }//while
   return retval;
}//obj_count

int crit_count(List<critter*>& lst, critter& src) {
   int retval = 0;
   Cell<critter*> cll(lst);
   critter* ptr;

   if (!src.mob) {
      return retval;
   }//if

   while ((ptr = cll.next())) {
      if (ptr->mob) {
         if (ptr->MOB_NUM == src.MOB_NUM) {
            retval++;
         }//if
      }//if
   }//while
   return retval;
}//crit_count


void clear_crit_list(PtrList<critter>& lst) {
   Cell<critter*> cll(lst);
   critter* ptr;
   //log("In clear_crit_list.\n");

   ptr = cll.next();
   while (ptr) {
      if ((ptr->pc) || (ptr->isSmob())) { //if smob/pc 
         delete ptr;   //delete it for sure
      }//if
      ptr = lst.lose(cll);
   }//while
}//clear_crit_list


void clear_obj_list(PtrList<object>& lst) {
   Cell<object*> cll(lst);
   object* ptr;

   //log("In clear_obj_list.\n");

   ptr = cll.next();
   while (ptr) {
      //log("In while loop.\n");
      if (ptr->in_list) { //if its a SOBJ
         delete ptr;   //delete it for sure
      }//if
      
      ptr = lst.lose(cll);
   }//while
}//clear_obj_list


void out_field(const bitfield& field, critter& pc, const BitfieldNames& names) {
   int k = field.max_bit();
   String buf(100);
   String tmp(50);
   int sofar = 0;

   Sprintf(buf, "%S (SET)\n\t", &(names.getHeader()));
   pc.show(buf);
   buf = "";

   for (int i = 0; i <= k; i++) {
      if (field.get(i)) {
         Sprintf(tmp, "[%i] %s,  ", i, names.getName(i));
         if ((sofar + tmp.Strlen()) > 80) {
            buf += "\n\t";
            sofar = tmp.Strlen();
         }
         else {
            sofar += tmp.Strlen();
         }
         buf += tmp;
      }//if
   }//for
   buf.Append("\n");
   show(buf, pc);
}//out_field

   
int critter::doBecomeNonPet() {
   String buf(100);

   if (isMob()) {
      mudlog.log(ERROR, "ERROR:  mob sent to unpet.\n");
      return -1;
   }//if

   if (!MASTER) {
      return -1;
   }//if

   MASTER->PETS.loseData(this); //master no longer has pc as pet...
   MASTER = NULL;   // slave of none

   // Takes care of guarding/shielding, etc.
   if ( temp_crit ) {
       temp_crit->Clear();
   }

   return doUngroup(1, &NULL_STRING);
}//unpet


const char* class_of_crit(critter& pc) {
   return get_class_name(pc.CLASS);
}//class_of_crit

int find_and_delete_obj(object* obj_to_find, int room_num) {
   // Find an object in the rooms current inventory, also look in containers' and
   // critters' inventories.
   // Returns FALSE on error (room doesn't exist, removed wasn't found)
   //         TRUE  when an object has been removed
   object* obj = NULL;
   critter* crit = NULL;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "DEBUG:  in find_and_remove_obj(object* obj_to_find = " << obj_to_find
             << ", int rm_num = " << room_num << ")\n";
   }

   // Sanity checks, make sure this is a valid room
   if ((0 > room_num > NUMBER_OF_ROOMS) || !room_list[room_num].isInUse()) {
      mudlog.log(DBG, "DEBUG:  Room is invalid.\n");
      return FALSE;
   }

   Cell<object *> obj_cll(*(room_list[room_num].getInv()));
   Cell<critter*> crit_cll(room_list[room_num].getCrits());

   // Try to find obj_to_find in the room's inventory
   mudlog.log(DBG, "DEBUG:  checking objects in the room.\n");
   while((obj = obj_cll.next())) {
      if (obj == obj_to_find) {
         // Found it, so delete it
         mudlog.log(DBG, "DEBUG:  Found object in the room.\n");
         room_list[room_num].loseInv(obj_to_find);
         if (obj_to_find->isModified()) {
            delete obj_to_find;
         }
         return TRUE;
      }
      else if (find_and_delete_obj(obj_to_find, obj)) {
         mudlog.log(DBG, "DEBUG:  Found object in a container.\n");
         return TRUE;
      }
   }

   // Check the critters in the room
   mudlog.log(DBG, "DEBUG:  checking on critters in the room.\n");
   while((crit = crit_cll.next())) {
      if (find_and_delete_obj(obj_to_find, crit)) {
         mudlog.log(DBG, "DEBUG:  Found object on a critter.\n");
         return TRUE;
      }
   }

   // Guess we didn't find it
   mudlog.log(DBG, "DEBUG:  Didn't find object in room.\n");
   return FALSE;
}

int find_and_delete_obj(object* obj_to_find, critter* crit_ptr) {
   // Find an object in a critter's current inventory, also look in containers the
   // critter owns, and at the critter's equipment list
   // Returns FALSE on error (crit_ptr is bad, didn't find obj_to_find)
   //         TRUE  when an object has been removed
   object* obj = NULL;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "DEBUG:  in find_and_remove_obj(object* obj_to_find = " << obj_to_find
             << ", crit_ptr = " << crit_ptr << ")\n";
   }

   if (!crit_ptr) {
      mudlog.log(DBG, "DEBUG:  bad crit_ptr.\n");
      return FALSE;
   }

   Cell<object*> obj_cll(crit_ptr->inv);

   // Try to find obj_to_find in the critter's inventory
   mudlog.log(DBG, "DEBUG:  checking critter's inventory.\n");
   obj = obj_cll.next();
   while (obj) {
      if (obj == obj_to_find) {
         // Found it, so delete it
         mudlog.log(DBG, "DEBUG:  found object in critter's inventory.\n");
         crit_ptr->loseInv(obj_to_find);
         if (obj_to_find->isModified()) {
            delete obj_to_find;
         }
         return TRUE;
      }
      else {
         if (find_and_delete_obj(obj_to_find, obj)) {
            mudlog.log(DBG, "DEBUG:  Found object in a container (2).");
            return TRUE;
         }
      }//else
      obj = obj_cll.next();
   }

   // Check the critter's equipped items
   mudlog.log(DBG, "DEBUG:  checking critter's equipment.");
   for (int i=1; i < MAX_EQ; i++) {
      if (crit_ptr->EQ[i] == obj_to_find) {
         // Found it, so delete it
         mudlog.log(DBG, "DEBUG:  found object in critter's EQ[].");
         remove_eq_effects(*crit_ptr->EQ[i], *crit_ptr, FALSE, FALSE, i);
         crit_ptr->EQ[i] = NULL;
         if (obj_to_find->isModified()) {
            delete obj_to_find;
         }
         return TRUE;
      }
      else {
         if (crit_ptr->EQ[i] && find_and_delete_obj(obj_to_find, crit_ptr->EQ[i])) {
            mudlog.log(DBG, "DEBUG:  Found object in a container.\n");
            return TRUE;
         }
      }
   }

   // Guess we didn't find it
   mudlog.log(DBG, "DEBUG:  didn't find object on critter.\n");
   return FALSE;
}

int find_and_delete_obj(object* obj_to_find, object* find_in) {
   // Find an object in another object's inventory - infinately recursive
   // Returns FALSE on error (find_in isn't a container, obj_to_find isn't in find_in)
   //         TRUE  when an object has been removed
   object* obj = NULL;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "DEBUG:  in find_and_remove_obj(object* obj_to_find = " << obj_to_find
             << ", object* find_in = " << find_in << ")\n";
   }

   Cell<object *> cll(find_in->inv);

   // Try to find obj_to_find in find_in's inventory
   mudlog.log(DBG, "DEBUG:  checking object's inventory.\n");
   obj = cll.next();
   while (obj) {
      if (obj == obj_to_find) {
         // Found it, so, delete it
         mudlog.log(DBG, "DEBUG:  Found object in container.\n");
         find_in->loseInv(obj_to_find);
         if (obj_to_find->isModified()) {
            delete obj_to_find;
         }
         return TRUE;
      }
      else {
         if (find_and_delete_obj(obj_to_find, obj)) {
            mudlog.log(DBG, "DEBUG:  Found object in container in container.\n");
            return TRUE;
         }
      }
      obj = cll.next();
   }

   // Guess we didn't find it
   mudlog.log(DBG, "DEBUG:  Couldn't find object in container.\n");
   return FALSE;
}

// Swap one letter for another, preserving case.
char icharswap(char oldch, char newch) {
   if (isalpha(oldch) && isalpha(newch)) {
      char a = tolower(oldch);
      char b = tolower(newch);
      return oldch + (b - a);
   }
   else {
      return newch;
   }
}

String transform(const String &input,
      const PtrList<String> &wordpats, const PtrList<String> &wordreps,
      const PtrList<String> &fragpats, const PtrList<String> &fragreps) {
   String output;
   char lastalpha = 'a';
   for (unsigned int i = 0; i < input.Strlen(); i++) {
      // On a symbol, or in the middle of a word
      if (!isalnum(input[i]) || (i > 0 && isalnum(input[i-1]))) {
         Cell<String*> patcll(fragpats);
         String* pat;
         Cell<String*> repcll(fragreps);
         String* rep;

         while ((pat = patcll.next()) && (rep = repcll.next())) {
            if (input.Strlen() >= pat->Strlen() + i &&
                  strncasecmp(((const char*)input)+i, *pat, pat->Strlen()) == 0) {
               // output.Append(*rep);
               for (unsigned int j=0; j < rep->Strlen(); j++) {
                  if (j < pat->Strlen() && isalnum(input[i+j])) lastalpha = input[i+j];
                  output.Append(icharswap(lastalpha, (*rep)[j]));
               }
               i+=pat->Strlen() - 1;
               break;
            }
         }
         // Didn't find a match (or ran out of replacements before we ran out of patterns)
         if (!pat || !rep) output.Append(input[i]);
      }
      // Must be at the beginning of the string or a word, then
      else { // if (i == 0 || !isalnum(input[i-1]))
         Cell<String*> patcll(wordpats);
         String* pat;
         Cell<String*> repcll(wordreps);
         String* rep;

         while ((pat = patcll.next()) && (rep = repcll.next())) {
            if (input.Strlen() >= pat->Strlen() + i &&
                  (input.Strlen() == pat->Strlen() + i ||
                     !isalnum(input[pat->Strlen() + i ])) &&
                  strncasecmp(((const char*)input)+i, *pat, pat->Strlen()) == 0) {
               // output.Append(*rep);
               for (unsigned int j=0; j < rep->Strlen(); j++) {
                  if (j < pat->Strlen() && isalnum(input[i+j])) lastalpha = input[i+j];
                  output.Append(icharswap(lastalpha, (*rep)[j]));
               }
               i+=pat->Strlen() - 1;
               break;
            }
         }
         // Didn't find a word match, try a fragment
         if (!pat || !rep) {
            // fragpats.head(patcll);
            Cell<String*> patcll(fragpats);
            // fragreps.head(repcll);
            Cell<String*> repcll(fragreps);
            while ((pat = patcll.next()) && (rep = repcll.next())) {
               if (input.Strlen() >= pat->Strlen() + i &&
                     strncasecmp(((const char*)input)+i, *pat, pat->Strlen()) == 0) {
                  // output.Append(*rep);
                  for (unsigned int j=0; j < rep->Strlen(); j++) {
                     if (j < pat->Strlen() && isalnum(input[i+j])) lastalpha = input[i+j];
                     output.Append(icharswap(lastalpha, (*rep)[j]));
                  }
                  i+=pat->Strlen() - 1;
                  break;
               }
            }
            // Didn't find a match
            if (!pat || !rep) output.Append(input[i]);
         }
      }
   }
   return output;
}




critter* get_target_mob(int i_th, const String* target, critter& pc, char* diversions){
   critter* vict = NULL;
   if (target->Strlen() == 0)
      vict = pc.IS_FIGHTING.peekFront();
   else
      vict = ROOM.haveCritNamed(i_th, target, pc);
//   if (!vict) doFailureNoTarget();

   if(vict){
      vict = check_for_diversions(*vict, diversions, pc);
      return vict;
   }
   return NULL;
}


object* get_target_obj(int i_th, const String* target, critter& pc) {
    object* vict = NULL;

    if (!(vict = have_obj_named(pc.inv, i_th, target, pc.SEE_BIT, 
                               ROOM))) 
    vict = ROOM.haveObjNamed(i_th, target, pc.SEE_BIT);
  //  if (!vict) doFailureNoTarget();

    return vict;
    
}

door* get_target_door(int i_th, const String* target, critter& pc) {
    door* dptr = NULL;

    dptr = door::findDoor(ROOM.DOORS, i_th, target, pc.SEE_BIT, ROOM);
//    if (!dptr) doFailureNoTarget();
    
    return dptr;
      
}

room* get_target_room(critter& pc){
    //no error checking, mobs should always be in a room
    room* rm = room_list.elementAt(pc.getCurRoomNum());
    return rm;
    
}


/*template <typename T> void DistProbMatrix<T>::add(T value, unsigned int weight){
	size += weight;
	matrix.reserve(size);
	for(unsigned int i = 0; i < weight; i++){
		matrix.push_back(value);
	}

//	cout << "Beging DistProbMatrix::add matrix = " << matrix << " size:" << size << " weight:" << weight << " value:" << value << endl;
	if(matrix == NULL){ // we're a fresh instance
		matrix = new int[weight];
		cout << "new matrix = " << matrix;
		memset(matrix, value, weight);
		cout << "matrix data:" << endl;
//		for(unsigned int idx = 0; idx < size; idx++){
//			cout << "idx:" << idx << "matrix value: "<< *(matrix + (idx*sizeof(int)));
//		}
		size = weight;
		return;
	} else{ //adding to existing
		unsigned int tmp_size = size+weight; //size for new matrix
		int* tmp_matrix = new int[size+weight]; // new matrix
		cout << "old matrix: " << matrix << " new matrix:" << tmp_matrix << " old size:" << size << " new size:" << tmp_size << endl;
		memcpy(tmp_matrix, &value, weight); // copy the old one over
		memset((tmp_matrix+size-1), value, weight); //set the new values
		for(unsigned int idx = 0; idx < tmp_size; idx++){
			if(*(tmp_matrix + (idx*sizeof(int))) > 13){
			   cout << "idx: " << idx << "matrix value: "<< *(tmp_matrix + (idx*sizeof(int))) << endl;
			}
		}
		delete[] matrix; // kill the old one
		matrix = tmp_matrix;
		size = tmp_size;
	}
}	*/

		  
void combine_weights(int* target, const float* in, unsigned int length){

   unsigned int i =0;
   
   while(i < length){
         target[i] = int(target[i] * in[i]);
         ++i;
   }
}
