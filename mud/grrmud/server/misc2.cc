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

///*************************** misc2.cc ****************************///
///********  Headers for this file are found in misc2.h   ************///
///********  Bitfield masks defined here as well.	 ************///

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


// Attempt to load the player-run shop owner of that number.
// Returns a newly allocated SMOB, or NULL if a problem is
// encountered.
critter* load_player_shop_owner(int mob_num) {
   String buf(100);

   if (mob_list[mob_num].isInUse()) {
      // Find the file name of our shopkeeper.
      Sprintf(buf, "./PlayerShops/%S_%i", mob_list[mob_num].getShortName(),
              mob_num);

      ifstream dafile(buf);
      
      if (dafile) {
         critter* sk = new critter();
         sk->Read(dafile, TRUE); //read all inventory.

         // Make sure it's labeled as a SMOB
         sk->short_cur_stats[26] = 1; /* make it a SMOB */
         return sk;
      }//if
      else {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  Could not open Player Shop Keeper -:"
                   << buf << ":- for reading." << endl;
         }//if
         return NULL;
      }//else
   }//if
   else {
      if (mudlog.ofLevel(ERR)) {
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
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  Could not open Player Shop Keeper -:"
                   << buf << ":- for writing." << endl;
         }//if
         return FALSE;
      }//else
   }//if
   else {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  save_player_shop_owner, mob_num: "
                << pc.getIdNum() << " is not in use." << endl;
      }//if
      return FALSE;
   }//else
}//save_player_shop_owner


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

  
void lose_fly(critter& pc, short do_msg = FALSE) {
  if (do_msg)
    show("Good thing gravity doesn't work all that well yet!\n", pc);
}//lose_fly

void show_stat_affects(object& obj, critter& pc) {
   Cell<stat_spell_cell*> cll(obj.ob->stat_affects);
   stat_spell_cell* ptr;
   String buf(100);
   
   while ((ptr = cll.next())) {
      if (ptr->stat_spell == 1) {
         Sprintf(buf, "Strength:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 2) {
         Sprintf(buf, "Inteligence:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 3) {
         Sprintf(buf, "Constitution:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 4) {
         Sprintf(buf, "Charisma:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 5) {
         Sprintf(buf, "Wisdom:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 6) {
         Sprintf(buf, "Dexterity:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 7) {
         Sprintf(buf, "Hit (chance):%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 8) {
         Sprintf(buf, "Damage:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 9) {
         Sprintf(buf, "AC (armor):%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 10) {
         Sprintf(buf, "Attacks:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 15) {
         Sprintf(buf, "HP (current):%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 16) {
         Sprintf(buf, "Mana (current):%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 17) {
         Sprintf(buf, "Movement (current):%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 18) {
         Sprintf(buf, "Alignment:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 22) {
         Sprintf(buf, "Practices:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 23) {
         Sprintf(buf, "HP (max):%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 24) {
         Sprintf(buf, "Mana (max):%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 25) {
         Sprintf(buf, "Movement (max):%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 27) {
         Sprintf(buf, "Damage Recieved Modifier:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 28) {
         Sprintf(buf, "Damage Given Modifier:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 29) {
         Sprintf(buf, "Heat Resistance:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 30) {
         Sprintf(buf, "Cold Resistance:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 31) {
         Sprintf(buf, "Electrical Resistance:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 32) {
         Sprintf(buf, "Spell Resistance:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 35) {
         Sprintf(buf, "Bare Hand Dice Count:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 36) {
         Sprintf(buf, "Bare Hand Dice Sides:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 37) {
         Sprintf(buf, "HP Regeneration:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 38) {
         Sprintf(buf, "Mana Regeneration:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 39) {
         Sprintf(buf, "Movement Regeneration:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 100) {
         Sprintf(buf, "Hunger:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 101) {
         Sprintf(buf, "Thirst:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else if (ptr->stat_spell == 102) {
         Sprintf(buf, "Drugged:%P25 %i\n", ptr->bonus_duration);
         show(buf, pc);
      }//if
      else {
         Sprintf(buf, "Unknown [%i]:, value: [%i]\n", ptr->stat_spell,
                 ptr->bonus_duration);
         show(buf, pc);
      }//if

  }//while
}//show_stat_affects


/** Tests this performs:  K(know spell), M(has mana), S(is_standing),
 *                        V(!violence), N(!magic), B(!pc in_battle),
 *                        P(paralyzed), F(frozen)
 *  Case matters.
 */
short ok_to_cast_spell(critter* vict, const char* flags, int spell_num,
		       critter& pc, int do_msg = TRUE) {
  String buf(100);
  int len = strlen(flags);  
  char chr;
  int mana_cost = 0;

  mana_cost = SSCollection::instance().getSS(spell_num).getManaCost();
  
  if (pc.isMob()) {
     Sprintf(buf, "ERROR:  smob casting spell# %i.\n", spell_num);
     mudlog.log(ERR, buf);
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
     else if (chr == 'K') {
        if (get_percent_lrnd(spell_num, pc) <= 0) {
           if (do_msg) {
              show("You don't know where to begin!\n", pc);
           }//if
           return FALSE;
        }//if
     }//if    
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
     else if (chr == 'F') {
        if (pc.pc && pc.CRIT_FLAGS.get(0)) {
           if (do_msg) {
              show("You are frozen!\n", pc);
           }//if
           return FALSE;
        }//if
     }//if
     else if (chr == 'P') {
        if (pc.CRIT_FLAGS.get(14)) {
           if (do_msg) {
              show("You are completely paralyzed!\n", pc);
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
     else if (chr == 'V') {
        if (ROOM.isHaven()) {
           if (do_msg) {
              show("Violence is not permitted here.\n", pc);
           }//if
           return FALSE;
        }//if
        else if (ROOM.isNoPK() && vict && vict->pc) {
           if (do_msg) {
              show("Player killing is not allowed here.\n", pc);
           }//if
           return FALSE;
        }//if
     }//if
  }//for
  return TRUE;
}//ok_to_cast_spell

      

critter* check_for_diversions(critter& pc, char* tests, critter& agg) {
  char chr;
  int i;
  String buf(100);
   
  if (!tests)
    return &pc;

  for (i = 0; (chr = tests[i]); i++) {
    if (chr == 'S') {
      if (pc.temp_crit && pc.SHIELDED_BY) {
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
	if (pc.GUARDED_BY) {
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


stat_spell_cell* is_affected_by(int spell_num, critter& pc) {
  Cell<stat_spell_cell*> cll(pc.affected_by);
  stat_spell_cell* ptr;

  while ((ptr = cll.next())) {
    if (ptr->stat_spell == spell_num)
      return ptr;
  }//while

  return NULL;
}//is_affected_by


stat_spell_cell* is_affected_by(int spell_num, object& obj) {
  Cell<stat_spell_cell*> cll(obj.ob->affected_by);
  stat_spell_cell* ptr;

  while ((ptr = cll.next())) {
    if (ptr->stat_spell == spell_num)
      return ptr;
  }//while

  return NULL;
}//is_affected_by (obj)


stat_spell_cell* has_stat_affect(int stat_num, object& obj) {
  Cell<stat_spell_cell*> cll(obj.ob->stat_affects);
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


void leave_room_effects(room& rm, critter& pc) {
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

   String cmd = "exit";
   rm.checkForProc(cmd, NULL_STRING, pc, -1);

}//leave_room_effects


void leave_room_effects(room& rm, object& obj) {

   rm.checkLight(FALSE);

   //String cmd = "exit";
   //rm.checkForProc(cmd, NULL_STRING, pc, -1);

}//leave_room_effects (Objects)



const String* single_obj_name(object& obj, int see_bit) {
   if (detect(see_bit, obj.OBJ_VIS_BIT))
      return Top(obj.ob->names);
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
      return "chic";
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



void parse_communication(String& str) {
   int sofar = 0;
   int len = str.Strlen();
   char ch;
   int max_len = 80;
   
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
               max_len = 80;
               sofar = 0;
               break;
            }//if
         }//for
         if (sofar > 0) { //didn't find a space
            retval += '\n';
            max_len = 80;
            sofar = 0;
         }
      }//if
      if (ch == '<') {
         retval += "<<";
      }
      else {
         if (ch == '\n') {
            max_len = 80;
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
   Cell<object*> cll(board.ob->inv);
   object* ptr;
   int sofar = 0;
   int i;

   while ((ptr = cll.next())) {
      i = atoi(*(ptr->ob->names.peekFront()));
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


int mob_can_enter(critter& pc, room& rm, short do_msg) {
   int retval = FALSE;

   if (!rm.isInUse()) { //if not used
      if (do_msg)
         show("That room doesn't really exist!!\n", pc);
   }//if
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
         pc.PAUSE++; //punish them a bit
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
   else if (rm.isZlocked()) {
      if (pc.isImmort()) {
         if (rm.getZoneNum() != 0) {
            if ((pc.getImmLevel() <= 5) &&
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



void out_stat_spell_list(const List<stat_spell_cell*>& lst, critter& pc) {
   Cell<stat_spell_cell*> cll(lst);
   stat_spell_cell* ptr;
   String buf(100);

   while ((ptr = cll.next())) {
      Sprintf(buf, "%i %i ", ptr->stat_spell, ptr->bonus_duration);
      show(buf, pc);
   }//while
   show("\n", pc);
}//out_stat_spell


critter* find_next_proc_mob(List<critter*>& lst) {
   Cell<critter*> cll(lst);
   critter* ptr;

   while ((ptr = cll.next())) {
      if (!(ptr->pc)) {
         if (ptr->mob) {
            if (!ptr->master) {
               if (ptr->MOB_FLAGS.get(3)) {
                  ptr->MOB_FLAGS.turn_off(3);
                  return ptr;
               }//if haven't done procs yet
            }//if
         }//if a mob
      }//needed if obj has BOTH mob and pc data fields
   }//while

   return NULL;
}//find_next_proc_mob


critter* find_next_spell_mob(List<critter*>& lst) {
   Cell<critter*> cll(lst);
   critter* ptr;

   while ((ptr = cll.next())) {
      if (!ptr->CRIT_FLAGS.get(20)) { //if spell_tested
         ptr->CRIT_FLAGS.turn_on(20);
         return ptr;
      }//if not affected yet
   }//while

   return NULL;
}//find_next_spell_mob




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
   return Hour;
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
      mudlog.log(ERR, "ERROR:  day is > 365, in get_month.\n");
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
      mudlog.log(ERR, "ERROR:  day is > 365, get_day_of_month.\n");
      return 32;
   }//else
}//get_day_of_month

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
      return "12 am";
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
      return "12 pm";
   else {
      mudlog.log(ERR, "ERROR:  m_time out of range, military_to_am.\n");
      return "0 am";
   }//else
}//military_to_am


room* get_next_room(int zone_num) {

   for (int i = ZoneCollection::instance().elementAt(zone_num).getBeginRoomNum();
        i <= ZoneCollection::instance().elementAt(zone_num).getEndRoomNum();
        i++) {
      if (!room_list[i].isInUse()) { //if room is not used
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


int get_percent_lrnd(int skill_num, critter& pc, short automatic = FALSE) {
  if (!pc.pc) {
    if (automatic && pc.mob && pc.mob->proc_data) {
      if (pc.FLAG1.get(12)) { //evasive maneuvers flag
	if (skill_num == DODGE_SKILL_NUM)
	  return (70 + pc.LEVEL);
	if (skill_num == PARRY_SKILL_NUM)
	  return (70 + pc.LEVEL);
      }//if
    }//if automatic (ie should search)
    return (70 + pc.LEVEL);
  }//if not a pc

  int retval;
  if (pc.SKILLS_KNOWN.Find(skill_num, retval)) {
    return retval;
  }//if

  return -1;
}//%lrnd_skill


void increment_percent_lrnd(int skill_num, critter& pc) {

   if (!pc.pc)
     return;

   float inc;
   int dif = SSCollection::instance().getSS(skill_num).getDifficulty();
   
   inc = (dif/50.0) * ((pc.INT/4.0) * (pc.INT/4.0) + pc.INT/2 + 10.0);

   int p_lrnt;
   if (pc.SKILLS_KNOWN.Find(skill_num, p_lrnt)) {
      p_lrnt += (int)inc;
      if (p_lrnt > 100)
        p_lrnt = 100;
      pc.SKILLS_KNOWN.Insert(skill_num, p_lrnt); //update value
      return;
   }//if
   mudlog.log(ERR,
              "ERROR:  tried to increment_%_lrnd on a skill unknown by pc.\n");
}//inc_skill


void init_masks() {
            /*  OBJ_WEAR_FLAGS  */
   int i;
   for (i = 22; i < 21 + MAX_EQ; i++) { 
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
   else return "??";
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


void clear_crit_list(List<critter*>& lst) {
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

void clear_obj_list(List<object*>& lst) {
   Cell<object*> cll(lst);
   object* ptr;

   //log("In clear_obj_list.\n");

   ptr = cll.next();
   while (ptr) {
      //log("In while loop.\n");
      if (ptr->ob) {  //some wierd bug? requires this.
         if (ptr->ob->in_list) { //if its a SOBJ 
            delete ptr;   //delete it for sure
         }//if
      }//if
      ptr = lst.lose(cll);
   }//while
}//clear_obj_list

void out_field(const bitfield& field, critter& pc) {
   int k = field.max_bit();
   String buf(100);

   for (int i = 0; i <= k; i++) {
      if (field.get(i)) {
         buf.Append(i);
         buf.Append(" ");
      }//if
   }//for
   buf.Append("\n");
   show(buf, pc);
}//out_field

   
void critter::doBecomeNonPet() {
   String buf(100);

   if (isMob()) {
      mudlog.log(ERR, "ERROR:  mob sent to unpet.\n");
      return;
   }//if

   if (!MASTER) {
      return;
   }//if

   MASTER->PETS.loseData(this); //master no longer has pc as pet...
   MASTER = NULL;   // slave of none

   doUngroup(1, &NULL_STRING);
}//unpet


const char* class_of_crit(critter& pc) {
   return get_class_name(pc.CLASS);
}//class_of_crit

