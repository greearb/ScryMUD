// $Id: olc2.cc,v 1.12 1999/07/18 05:10:17 greear Exp $
// $Revision: 1.12 $  $Author: greear $ $Date: 1999/07/18 05:10:17 $

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

///**********************  olc2.cc  ******************************///
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include "command4.h"
#include "command5.h"
#include "misc.h"
#include "misc2.h"
#include <stdio.h>
#include "classes.h"
#include "battle.h"
#include "spec_prc.h"
#include "spells.h"
#include "skills.h"
#include "olc2.h"
#include "olc.h"
#include <PtrArray.h>
#include "vehicle.h"
#include "SkillSpell.h"


int normalize_obj(object& obj) {
   if (obj.OBJ_NUM != obj_list[obj.OBJ_NUM].OBJ_NUM) {
      mudlog << "ERROR:  normalize_obj, object numbers not in sync:  "
             << " obj.OBJ_NUM:  " << obj.OBJ_NUM << "  other:  "
             << obj_list[obj.OBJ_NUM].OBJ_NUM << endl;
   }//if

   Cell<stat_spell_cell*> cll(obj.stat_affects);
   stat_spell_cell* ptr, *tptr;
   int ss, bd;
   ptr = cll.next();

   while (ptr) {
      ss = ptr->stat_spell;
      if ((ss == 12) || (ss == 13) || (ss == 14) ||
          ((ss >= 0) && (ss <= 6)) || (ss == 11) ||
          (ss == 19) || (ss == 21) || 
          (ss == 20) || (ss == 26) || (ss == 33) ||
          (ss == 34) || (ss == 40) || (ss <= 0) ||
          ((ss >= MOB_SHORT_CUR_STATS) && (ss != 100) &&
           (ss != 101) && (ss != 102))) {
         tptr = ptr;
         ptr = obj.stat_affects.lose(cll);
         delete tptr;
      }//if
      else {
         bd = ptr->bonus_duration;
         if ((ss >= 1) && (ss <= 8)) {
            bd = bound(-4, 4, bd);
         }
         else if (ss == 9) { //ac
            bd = bound(-60, 60, bd);
         }
         else if (ss == 10) { //attacks
            bd = bound(-1, 2, bd);
         }
         else if (ss == 12) { //pause
            bd = bound(-10, 10, bd);
         }
         else if ((ss >= 15) && (ss <= 17)) {
            bd = bound(-500, 500, bd);
         }
         else if (ss == 18) {
            bd = bound(-50, 50, bd);
         }
         else if (ss == 22) {
            bd = bound(0, 2, bd);
         }
         else if ((ss >= 23) && (ss <= 25)) {
            bd = bound(-60, 60, bd);
         }
         else if ((ss == 27) || (ss == 28)) {
            bd = bound(-30, 30, bd);
         }
         else if ((ss >= 27) && (ss <= 32)) {
            bd = bound(-60, 60, bd);
         }
         else if ((ss == 35) || (ss == 36)) {
            bd = bound(-5, 5, bd);
         }
         else if ((ss >= 37) && (ss <= 39)) {
            bd = bound(-50, 50, bd);
         }
         else if ((ss >= 100) && (ss <= 102)) {
            bd = bound(-50, 50, bd);
         }

         ptr->bonus_duration = bd;

         ptr = cll.next();
      }//else
   }//while

   // no more than 5
   while (obj.stat_affects.size() > 5) {
      delete (obj.stat_affects.popBack());
   }

   // If it can 'disolve', then it must not be wearable, because it
   // makes a real mess when a corpse disolves and you are holding it!!
   // It could be made to work, but not worth the trouble IMHO. --BEN
   if (obj.getIdNum() == CORPSE_OBJECT) {
      // Make sure it can't be worn.
      obj.OBJ_FLAGS.turn_on(21);
   }

   // If not worn is on, then zero out the other worn flags just in case.
   if (obj.OBJ_FLAGS.get(21)) { 
      for (int i = 22; i<40; i++) {
         obj.OBJ_FLAGS.turn_off(i);
      }//for
   }//if

   return 0;
}//normalize_obj



int normalize_mob(critter& crit) {
   crit.CRIT_FLAGS.turn_on(18); //in_use
   crit.VIS_BIT |= 1024;
   crit.SEE_BIT |= 1024;

   crit.ATTACKS = bound(1, 4, crit.ATTACKS);
   crit.setPosn(bound(POS_STAND, POS_PRONE, crit.POS));

   int i;
   for (i = 1; i<9; i++) {
      crit.short_cur_stats[i] = bound(1, 100, crit.short_cur_stats[i]);
   }//for

   crit.AC = bound(-200, 200, crit.AC);
   crit.PAUSE = 0;
   crit.CLASS = bound(1, 8, crit.CLASS);
   crit.RACE = bound(1, 14, crit.RACE);
   crit.LEVEL = bound(1, 40, crit.LEVEL);
   crit.setNativeZoneNum(bound(0, NUMBER_OF_ZONES, crit.getNativeZoneNum()));
   crit.setHP_MAX(bound(1, 32000, crit.HP_MAX));
   crit.MV_MAX = bound(1, 32000, crit.MV_MAX);
   crit.MA_MAX = bound(1, 32000, crit.MA_MAX);
   crit.DAM_REC_MOD = bound(1, 1000, crit.DAM_REC_MOD);
   crit.DAM_GIV_MOD = bound(1, 1000, crit.DAM_GIV_MOD);

   for (i = 29; i<33; i++) {
      crit.short_cur_stats[i] = bound(-200, 200, crit.short_cur_stats[i]);
   }//for

   crit.BH_DICE_SIDES = bound(1, 100, crit.BH_DICE_SIDES);
   crit.BH_DICE_COUNT = bound(1, 100, crit.BH_DICE_COUNT);

   for (i = 38; i<41; i++) {
      crit.short_cur_stats[i] = bound(5, 1000, crit.short_cur_stats[i]);
   }//for

   return 0;
}//normalize_mob
 

int normalize_door(door_data& dr_data) {
  dr_data.vis_bit |= 1024;
  return 0;
}//normalize_door(data)

int rm_give_proc(int mnum, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(mnum, 0, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[mnum].isInUse()) {
      show("This mob has not been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[mnum])) {
      show("You don't own this mob.\n", pc);
      return -1;
   }//if
   if (O_COUNT != 0) {
      show("You must complete your OLC project first.\n", pc);
      return -1;
   }//if

   if (mob_list[mnum].mob && mob_list[mnum].mob->proc_data &&
       mob_list[mnum].mob->proc_data->give_proc) {
      delete mob_list[mnum].mob->proc_data->give_proc;
      mob_list[mnum].mob->proc_data->give_proc = NULL;
      mob_list[mnum].mob->proc_data->flag1.turn_off(5);
      show("Give proc removed.\n", pc);
      show("Must asave to make changes permanent.\n", pc);
      return 0;
   }//if
   else {
      show("Failed to remove, did not exist.\n", pc);
      return -1;
   }//else
}//rm_give_proc


int rm_keyword(int kwd_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFPRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   Cell<KeywordPair*> cll(ROOM.keywords);
   KeywordPair* ptr;
   int cnt = 0;
   ptr = cll.next();
   while (ptr) {
      if (cnt == kwd_num) {
         delete ptr;
         ROOM.keywords.lose(cll);
         pc.show("Keyword Removed from this room.\n");
         ROOM.normalize();
         return 0;
      }
      else {
         ptr = cll.next();
         cnt++;
      }
   }//while

   pc.show("This room did not have a keyword numbered thusly.\n");
   return -1;
}//rm_keyword


int add_keyword(critter& pc) {
   if (!ok_to_do_action(NULL, "IFPRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   ROOM.keywords.append(new KeywordPair());
   ROOM.normalize();
   pc.show("Added a new Keyword at index 0.\n");
   return 0;
}//add_keyword


int clear_keyword(int kwd_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFPRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   Cell<KeywordPair*> cll(ROOM.keywords);
   KeywordPair* ptr;
   int cnt = 0;
   ptr = cll.next();
   while (ptr) {
      if (cnt == kwd_num) {
         ptr->clear();
         pc.show("Keyword cleared.\n");
         return 0;
      }
      else {
         ptr = cll.next();
         cnt++;
      }
   }//while

   pc.show("This room did not have a keyword numbered thusly.\n");
   return -1;
}//clear_keyword


int add_kname(int kwd_num, const String* name, critter& pc) {
   if (!ok_to_do_action(NULL, "IFPRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   Cell<KeywordPair*> cll(ROOM.keywords);
   KeywordPair* ptr;
   int cnt = 0;
   ptr = cll.next();
   while (ptr) {
      if (cnt == kwd_num) {
         ptr->names.append(new String(*name));
         pc.show("Name added.\n");
         return 0;
      }
      else {
         ptr = cll.next();
         cnt++;
      }
   }//while

   pc.show("This room did not have a keyword numbered thusly.\n");
   return -1;
}//clear_keyword



int ch_kdesc(int kwd_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFPRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   Cell<KeywordPair*> cll(ROOM.keywords);
   KeywordPair* ptr;
   int cnt = 0;
   ptr = cll.next();
   while (ptr) {
      if (cnt == kwd_num) {
         show("Enter a new room description for this Keyword.\n", pc);
         show("Use a solitary '~' on a line by itself to end.\n", pc);
         show("You may now paste descs up to 2k, ';' acts as a newline btw.\n",
              pc);

         ptr->desc.Clear();
         pc.pc->imm_data->edit_string = &(ptr->desc);
         pc.setMode(MODE_CH_DESC);
         return 0;
      }
      else {
         ptr = cll.next();
         cnt++;
      }
   }//while

   pc.show("This room did not have a keyword numbered thusly.\n");
   return -1;
}//ch_kdesc


int stat_keyword(int kwd_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   Cell<KeywordPair*> cll(ROOM.keywords);
   KeywordPair* ptr;
   int cnt = 0;
   while ((ptr = cll.next())) {
      if (cnt == kwd_num) {
         ptr->show(cnt, pc);
         return 0;
      }
      else {
         cnt++;
      }
   }//while

   pc.show("This room did not have a keyword numbered thusly.\n");
   return -1;
}//stat_keyword


int rm_stat_affect(int onum, int stat_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFPR", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(onum, 2, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;
   object* obj_ptr = &(obj_list[onum]);
   if (!obj_ptr->isInUse()) {
      show("This mob has not been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(*obj_ptr)) {
      show("You don't own this object.\n", pc);
      return -1;
   }//if

   Cell<stat_spell_cell*> cll(obj_ptr->stat_affects);
   stat_spell_cell* ptr;
   ptr = cll.next();
   while (ptr) {
      if (ptr->stat_spell == stat_num) {
         delete ptr;
         pc.show("Removed that stat affect...\n");
         pc.show("Don't forget to aosave.\n");
         ptr = obj_ptr->stat_affects.lose(cll);
         return 0;
      }
      else {
         ptr = cll.next();
      }
   }//while

   show("Failed to remove, did not exist.\n", pc);
   return -1;
}//rm_stat_affect


int add_stat_affect(int onum, int stat_num, int val, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(onum, 2, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;

   object* obj_ptr = &(obj_list[onum]);
   if (!obj_ptr->isInUse()) {
      show("This mob has not been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(*obj_ptr)) {
      show("You don't own this object.\n", pc);
      return -1;
   }//if

   obj_ptr->stat_affects.append(new stat_spell_cell(stat_num, val));
   normalize_obj(*obj_ptr);
   pc.show("Was added..now normalizing object to weed out bad values...\n");
   pc.show("Don't forget to aosave.\n");
   return 0;
}//add_stat_affect



int rm_casts_spell(int onum, int spell_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(onum, 2, NUMBER_OF_ITEMS, pc, TRUE, "Object Num"))
      return -1;

   object* obj_ptr = &(obj_list[onum]);
   if (!obj_ptr->isInUse()) {
      show("This mob has not been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(*obj_ptr)) {
      show("You don't own this object.\n", pc);
      return -1;
   }//if

   if (!obj_ptr->obj_proc) {
      pc.show("That object cannot have any spells to remove.\n");
      return -1;
   }

   Cell<stat_spell_cell*> cll(obj_ptr->CASTS_THESE_SPELLS);
   stat_spell_cell* ptr;
   ptr = cll.next();
   while (ptr) {
      if (ptr->stat_spell == spell_num) {
         delete ptr;
         pc.show("Removed that spell...\n");
         pc.show("Don't forget to aosave.\n");
         ptr = obj_ptr->CASTS_THESE_SPELLS.lose(cll);
         return 0;
      }
      else {
         ptr = cll.next();
      }
   }//while

   show("Failed to remove, did not exist.\n", pc);
   return -1;
}//rm_casts_spell


int add_casts_spell(int onum, int level, int spell, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(onum, 2, NUMBER_OF_ITEMS, pc, TRUE, "Obj Number"))
      return -1;

   if (!check_l_range(level, 0, MOB_SHORT_CUR_STATS, pc, TRUE,
                      "Spell Level"))
      return -1;

   if (!check_l_range(spell, 0, NUMBER_OF_SKILL_SPELLS, pc, TRUE,
                      "Spell Number"))
      return -1;

   object* obj_ptr = &(obj_list[onum]);
   if (!obj_ptr->isInUse()) {
      show("This mob has not been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(*obj_ptr)) {
      show("You don't own this object.\n", pc);
      return -1;
   }//if

   if (!obj_ptr->obj_proc) {
      obj_ptr->obj_proc = new obj_spec_data();
   }

   //turn on appropriate flags
   obj_ptr->OBJ_SPEC_FLAGS.turn_on(10);
   stat_spell_cell* ptr = new stat_spell_cell;
   ptr->bonus_duration = level;
   ptr->stat_spell = spell;
   obj_ptr->CASTS_THESE_SPELLS.append(ptr);
   obj_ptr->OBJ_FLAGS.turn_on(63); //so it will write out spec procs

   pc.show("Spell was added, don't forget to aosave.\n");
   return 0;
}//add_casts_spell


int rm_discuss_proc(int mnum, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(mnum, 0, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[mnum].isInUse()) {
      show("This mob has not been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[mnum])) {
      show("You don't own this mob.\n", pc);
      return -1;
   }//if

   if (O_COUNT != 0) {
      show("You must complete your OLC project first.\n", pc);
      return -1;
   }//if

   if (mob_list[mnum].mob && mob_list[mnum].mob->proc_data &&
       !IsEmpty(mob_list[mnum].mob->proc_data->topics)) {
      clear_ptr_list(mob_list[mnum].mob->proc_data->topics);
      mob_list[mnum].mob->proc_data->flag1.turn_off(6);
      show("Disscuss procs removed.\n", pc);
      show("Must asave to make changes permanent.\n", pc);
      return 0;
   }//if
   else {
      show("Failed to remove, did not exist.\n", pc);
   }//else
   return -1;
}//rm_discuss_proc


int rm_curse_proc(int mnum, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(mnum, 0, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[mnum].isInUse()) {
      show("This mob has not been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[mnum])) {
      show("You don't own this mob.\n", pc);
      return -1;
   }//if

   if (O_COUNT != 0) {
      show("You must complete your OLC project first.\n", pc);
      return -1;
   }//if

   if (mob_list[mnum].mob && mob_list[mnum].mob->proc_data &&
       mob_list[mnum].mob->proc_data->curse_proc) {
      delete mob_list[mnum].mob->proc_data->curse_proc;
      mob_list[mnum].mob->proc_data->curse_proc = NULL;
      mob_list[mnum].mob->proc_data->flag1.turn_off(8);
      show("Curse proc removed.\n", pc);
      show("Must asave to make changes permanent.\n", pc);
      return 0;
   }//if
   else {
      show("Failed to remove, did not exist.\n", pc);
      return -1;
   }//else
}//rm_curse_proc

int rm_bow_proc(int mnum, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(mnum, 0, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[mnum].CRIT_FLAGS.get(18)) {
      show("This mob has not been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[mnum])) {
      show("You don't own this mob.\n", pc);
      return -1;
   }//if

   if (O_COUNT != 0) {
      show("You must complete your OLC project first.\n", pc);
      return -1;
   }//if

   if (mob_list[mnum].mob && mob_list[mnum].mob->proc_data &&
       mob_list[mnum].mob->proc_data->bow_proc) {
      delete mob_list[mnum].mob->proc_data->bow_proc;
      mob_list[mnum].mob->proc_data->bow_proc = NULL;
      mob_list[mnum].mob->proc_data->flag1.turn_off(7);
      show("Bow proc removed.\n", pc);
      show("Must asave to make changes permanent.\n", pc);
      return 0;
   }//if
   else {
      show("Failed to remove, did not exist.\n", pc);
      return -1;
   }//else
}//rm_bow_proc



int add_proc(int mnum, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(mnum, 0, NUMBER_OF_MOBS, pc, TRUE))
      return -1;
   
   if (!mob_list[mnum].CRIT_FLAGS.get(18)) {
      show("This mob has not been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[mnum])) {
      show("You don't own this mob.\n", pc);
      return -1;
   }//if

   if (O_COUNT != 0) {
      show("You must complete your OLC project first.\n", pc);
      return -1;
   }//if

   int znum = ROOM.getZoneNum();

   if (!ZoneCollection::instance().elementAt(znum).isOwnedBy(pc)) {
     show("You must be in your own zone in order to build.\n", pc);
     show("You will now be exited from OLC.  Go back to your zone!\n", pc);
     pc.pc->input = NULL_STRING; //discard all input
     return -1;
   }//if
        
   OLC_MOB = &(mob_list[mnum]);
   O_COUNT = 30;
   pc.PC_FLAGS.turn_on(13);
   OLC_MOB->setNotComplete();

   show("You now have the ability to somewhat change this mob.  Be careful
that you don't add a proc that is ALREADY THERE.  This will result in 
strange things.  It should NOT be fatal however, so you can just remove
the proc later with the appropriate command(s).\n", pc);
   pc.setMode(MODE_OLC);
   return 0;
}//add_proc


int reset_olc(critter& pc) {
   mudlog.log(TRC, "In reset_olc.\n");
   
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

  
   if (pc.PC_FLAGS.get(13)) {
      show("You must complete your current project.  You are in a special 
case of OLC.\n", pc);
      return -1;
   }//if
   //log("About to delete.\n");

   pc.PC_FLAGS.turn_off(13); //olc_redo flag to FALSE
  
   int tmp = pc.IMM_LEVEL; //need to save this!!
   pc.pc->imm_data->Clear();
   pc.setImmLevel(tmp);
  
   pc.setMode(MODE_NORMAL);
   show("OLC has been reset, type 'olc' to enter OLC.\n", pc);
   return 0;
}//reset_olc


int ch_rdesc(critter& pc) {
   if (!ok_to_do_action(NULL, "IFPZR", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   show("Enter a new room description for this room.\n", pc);
   show("Use a solitary '~' on a line by itself to end.\n", pc);
   show("You may now paste descs up to 2k, ';' acts as a newline btw.\n",
	pc);

   ROOM.long_desc.Clear();
   pc.pc->imm_data->edit_string = &(ROOM.long_desc);
   pc.setMode(MODE_CH_DESC);
   return 0;
}//ch_rdesc


int list_paths(int veh_id, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if ((veh_id < 0) || (veh_id > NUMBER_OF_ROOMS)) {
      pc.show("That vehicle cannot be defined. (id number out of range)");
      return -1;
   }

   if (veh_id == 1) {
      veh_id = pc.getCurRoomNum();
   }

   if (!room_list[veh_id].isVehicle()) {
      pc.show("That room is not a vehicle.");
      return -1;
   }

   vehicle* veh = (vehicle*)(room_list.elementAt(veh_id));

   veh->showPaths(pc);
   return 0;
}//list_paths


int stat_path(int veh_id, int path_cell_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if ((veh_id < 0) || (veh_id > NUMBER_OF_ROOMS)) {
      pc.show("That vehicle cannot be defined. (id number out of range)");
      return -1;
   }

   if (!room_list[veh_id].isVehicle()) {
      pc.show("That room is not a vehicle.");
      return -1;
   }

   vehicle* veh = (vehicle*)(room_list.elementAt(veh_id));

   PathCell* pcell;
   if (!(pcell = veh->hasPathCell(path_cell_num))) {
      pc.show("That path is not defined for that room (vehicle) id.\n");
      return -1;
   }

   pcell->stat(veh_id, path_cell_num, pc);
   return 0;
}//stat_path


int set_path_dir(int veh_id, int path_cell_num, int i_th,
                  const String* dir, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if ((veh_id < 0) || (veh_id > NUMBER_OF_ROOMS)) {
      pc.show("That vehicle cannot be defined. (id number out of range)");
      return -1;
   }

   if (!room_list[veh_id].isZlocked()) {
      show("Your zone must be locked to change the vehicle.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnRoom(room_list[veh_id])) {
      show("You don't own this room.\n", pc);
      return -1;
   }//if
   
   if (!room_list[veh_id].isVehicle()) {
      pc.show("That room is not a vehicle.");
      return -1;
   }

   vehicle* veh = (vehicle*)(room_list.elementAt(veh_id));

   PathCell* pcell;
   if (!(pcell = veh->hasPathCell(path_cell_num))) {
      pc.show("That path is not defined for that room (vehicle) id.\n");
      return -1;
   }
   
   
   pcell->setDir(i_th, *dir);

   String buf(100);
   Sprintf(buf, "Setting path num to:  %i  direction to -:%S:-\n",
           i_th, dir);
   pc.show(buf);
   return 0;
}//set_path_dir



int set_path_pointer(int veh_id, int index, critter& pc) {

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if ((veh_id < 0) || (veh_id > NUMBER_OF_ROOMS)) {
      pc.show("That vehicle cannot be defined. (id number out of range)");
      return -1;
   }

   if (!room_list[veh_id].isZlocked()) {
      show("Your zone must be locked to change the vehicle.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnRoom(room_list[veh_id])) {
      show("You don't own this room.\n", pc);
      return -1;
   }//if
   
   if (!room_list[veh_id].isVehicle()) {
      pc.show("That room is not a vehicle.");
      return -1;
   }

   vehicle* veh = (vehicle*)(room_list.elementAt(veh_id));

   veh->setPathPointer(index);
   pc.show("Pointer set..");
   return 0;
}//set_path_pointer


//insert a blank path cell at the given index
int add_path(int veh_id, int path_cell_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if ((veh_id < 0) || (veh_id > NUMBER_OF_ROOMS)) {
      pc.show("That vehicle cannot be defined. (id number out of range)");
      return -1;
   }

   if (!room_list[veh_id].isZlocked()) {
      show("Your zone must be locked to change the vehicle.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnRoom(room_list[veh_id])) {
      show("You don't own this room.\n", pc);
      return -1;
   }//if
   
   if (!room_list[veh_id].isVehicle()) {
      pc.show("That room is not a vehicle.");
      return -1;
   }

   vehicle* veh = (vehicle*)(room_list.elementAt(veh_id));

   veh->insertPathCell(path_cell_num, new PathCell());

   pc.show("Path inserted...");
   return 0;
}//add_path


int tog_veh_stop(int veh_id, int path_cell_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if ((veh_id < 0) || (veh_id > NUMBER_OF_ROOMS)) {
      pc.show("That vehicle cannot be defined. (id number out of range)");
      return -1;
   }

   if (!room_list[veh_id].isZlocked()) {
      show("Your zone must be locked to change the vehicle.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnRoom(room_list[veh_id])) {
      show("You don't own this room.\n", pc);
      return -1;
   }//if
   
   if (!room_list[veh_id].isVehicle()) {
      pc.show("That room is not a vehicle.");
      return -1;
   }

   vehicle* veh = (vehicle*)(room_list.elementAt(veh_id));

   PathCell* pcell = veh->hasPathCell(path_cell_num);
   
   if (pcell) {
      pcell->setIsDest(!(pcell->isDest()));
      pc.show("Stop toggled...\n");
      return 0;
   }
   else {
      pc.show("That path cell does not exist in that vehicle.\n");
      return -1;
   }
}//tog_veh_stop


int set_veh_stop(int veh_id, int path_cell_num,
                  const String* yes_no, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if ((veh_id < 0) || (veh_id > NUMBER_OF_ROOMS)) {
      pc.show("That vehicle cannot be defined. (id number out of range)");
      return -1;
   }

   if (!room_list[veh_id].isZlocked()) {
      show("Your zone must be locked to change the vehicle.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnRoom(room_list[veh_id])) {
      show("You don't own this room.\n", pc);
      return -1;
   }//if
   
   if (!room_list[veh_id].isVehicle()) {
      pc.show("That room is not a vehicle.");
      return -1;
   }

   vehicle* veh = (vehicle*)(room_list.elementAt(veh_id));

   PathCell* pcell;
   if (!(pcell = veh->hasPathCell(path_cell_num))) {
      pc.show("That path is not defined for that room (vehicle) id.\n");
      return -1;
   }

   if (strncasecmp(*yes_no, "yes", yes_no->Strlen()) == 0) {
      pcell->setIsDest(TRUE);
      pc.show("Setting isDestination to TRUE.");
   }
   else {
      pcell->setIsDest(FALSE);
      pc.show("Setting isDestination to FALSE.");
   }
   return 0;
}//set_path_stop


int rem_path(int veh_id, int path_cell_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if ((veh_id < 0) || (veh_id > NUMBER_OF_ROOMS)) {
      pc.show("That vehicle cannot be defined. (id number out of range)");
      return -1;
   }

   if (!room_list[veh_id].isZlocked()) {
      show("Your zone must be locked to change the vehicle.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnRoom(room_list[veh_id])) {
      show("You don't own this room.\n", pc);
      return -1;
   }//if
   
   if (!room_list[veh_id].isVehicle()) {
      pc.show("That room is not a vehicle.");
      return -1;
   }

   vehicle* veh = (vehicle*)(room_list.elementAt(veh_id));

   veh->remPathCell(path_cell_num);

   pc.show("Path cell deleted...");
   return 0;
}//rem_path


int ch_path_desc(int veh_id, int path_cell_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if ((veh_id < 0) || (veh_id > NUMBER_OF_ROOMS)) {
      pc.show("That vehicle cannot be defined. (id number out of range)");
      return -1;
   }

   if (!room_list[veh_id].isZlocked()) {
      show("Your zone must be locked to change the vehicle.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnRoom(ROOM)) {
      show("You don't own this room.\n", pc);
      return -1;
   }//if
   
   if (!room_list[veh_id].isVehicle()) {
      pc.show("That room is not a vehicle.");
      return -1;
   }

   vehicle* veh = (vehicle*)(room_list.elementAt(veh_id));

   PathCell* pcell;
   if (!(pcell = veh->hasPathCell(path_cell_num))) {
      pc.show("That path is not defined for that room (vehicle) id.\n");
      return -1;
   }

   pc.show("Enter a new description for this path segemnt.");
   pc.show("Use a solitary '~' on a line by itself to end.\n");
   pc.show("You may now paste descs up to 2k, ';' acts as a newline btw.\n");

   String* desc = pcell->getDescPtr();
   desc->Clear();
   pc.pc->imm_data->edit_string = desc;
   pc.setMode(MODE_CH_DESC);
   return 0;
}//ch_path_desc


int add_mob_script(critter& pc, int mob_num, String& trigger_cmd,
                    int actor_num, String& descriminator, int target_num,
                    int takes_precedence) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (mob_num < 2 || mob_num > NUMBER_OF_MOBS) {
      show("That mob number is out of range.", pc);
      return -1;
   }

   if (!pc.doesOwnCritter(mob_list[mob_num])) {
      show("You don't own this mob.\n", pc);
      return -1;
   }//if

   if (!GenScript::validTrigger(trigger_cmd)) {
      show("That is not a valid trigger command.", pc);
      return -1;
   }

   // We prepend text too it so it's never a number as far as parsing
   // is concerned.  It's embarrasing, I know! Can't spell either!
   if (strncasecmp(descriminator, "DISCRIM_", 8) == 0) {
      String tmp = (((const char*)(descriminator)) + 8);
      descriminator = tmp;
   }

   if (descriminator == "NA") {
      descriminator.Clear();
   }

   String buf(200);
   Sprintf(buf, "Creating a script for mob:  %i, trigger -:%S:-, actor:  %i, \ndescriminator -:%S:-, target:  %i, takes_precedence:  %i\n",
           mob_num, &trigger_cmd, actor_num, &descriminator, target_num,
           takes_precedence);
   show(buf, pc);

   show("Enter a script for this mob. If the mob already has a\n", pc);
   show("proc for the specified trigger, then it will be over-written.\n", pc);
   show("Remember to end each statement with a double semicolon,\n", pc);
   show("which will be treated as a single semicolon by the parser.\n", pc);
   show("Use a solitary '~' on a line by itself to end.\n", pc);

   pc.pc->imm_data->tmp_proc_script
      = new MobScript(trigger_cmd, target_num, actor_num, descriminator,
                      takes_precedence);
   pc.pc->imm_data->proc_script_buffer.Clear();
   pc.pc->imm_data->tmp_script_entity_num = mob_num;

   pc.pc->imm_data->edit_string 
      = &(pc.pc->imm_data->proc_script_buffer);

   pc.setMode(MODE_ADD_MOB_SCRIPT);
   return 0;
}//add_mob_script


int add_room_script(critter& pc, int rm_num, String& trigger_cmd,
                     int actor_num, String& descriminator, int target_num,
                     int takes_precedence) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if ((rm_num < 2) || (rm_num > NUMBER_OF_ROOMS)) {
      show("That room number is out of range.", pc);
      return -1;
   }

   if (!pc.doesOwnRoom(room_list[rm_num])) {
      show("You don't own that room.\n", pc);
      return -1;
   }//if

   if (!GenScript::validTrigger(trigger_cmd)) {
      show("That is not a valid trigger command.", pc);
      return -1;
   }

   // We prepend text to it so it's never a number as far as parsing
   // is concerned.  It's embarrasing, I know! Can't spell either!
   if (strncasecmp(descriminator, "DISCRIM_", 8) == 0) {
      String tmp = (((const char*)(descriminator)) + 8);
      descriminator = tmp;
   }

   if (descriminator == "NA") {
      descriminator.Clear();
   }

   String buf(200);
   Sprintf(buf, "Creating a script for room:  %i, trigger -:%S:-, actor:  %i, \ndescriminator -:%S:-, target:  %i, takes_precedence:  %i\n",
           rm_num, &trigger_cmd, actor_num, &descriminator, target_num,
           takes_precedence);
   show(buf, pc);

   show("Enter a script for this room. If the room already has a\n", pc);
   show("proc for the specified trigger, then it will be over-written.\n", pc);
   show("Remember to end each statement with a double semicolon,\n", pc);
   show("which will be treated as a single semicolon by the parser.\n", pc);
   show("Use a solitary '~' on a line by itself to end.\n", pc);

   pc.pc->imm_data->tmp_proc_script
      = new RoomScript(trigger_cmd, target_num, actor_num, descriminator,
                       takes_precedence);
   pc.pc->imm_data->proc_script_buffer.Clear();
   pc.pc->imm_data->tmp_script_entity_num = rm_num;

   pc.pc->imm_data->edit_string 
      = &(pc.pc->imm_data->proc_script_buffer);

   pc.setMode(MODE_ADD_ROOM_SCRIPT);
   return 0;
}//add_room_script


int add_obj_script(critter& pc, int obj_num, String& trigger_cmd,
                   int actor_num, String& descriminator, int target_num,
                   int takes_precedence) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if ((obj_num < 2) || (obj_num > NUMBER_OF_ITEMS)) {
      show("That object number is out of range.", pc);
      return -1;
   }

   if (!pc.doesOwnObject(obj_list[obj_num])) {
      show("You don't own that object.\n", pc);
      return -1;
   }//if

   if (!GenScript::validTrigger(trigger_cmd)) {
      show("That is not a valid trigger command.", pc);
      return -1;
   }

   // We prepend text to it so it's never a number as far as parsing
   // is concerned.  It's embarrasing, I know! Can't spell either!
   if (strncasecmp(descriminator, "DISCRIM_", 8) == 0) {
      String tmp = (((const char*)(descriminator)) + 8);
      descriminator = tmp;
   }

   if (descriminator == "NA") {
      descriminator.Clear();
   }

   String buf(200);
   Sprintf(buf, "Creating a script for object:  %i, trigger -:%S:-, actor:  %i, \ndescriminator -:%S:-, target:  %i, takes_precedence:  %i\n",
           obj_num, &trigger_cmd, actor_num, &descriminator, target_num,
           takes_precedence);
   show(buf, pc);

   show("Enter a script for this object. If the object already has a\n", pc);
   show("proc for the specified trigger, then it will be over-written.\n", pc);
   show("Remember to end each statement with a double semicolon,\n", pc);
   show("which will be treated as a single semicolon by the parser.\n", pc);
   show("Use a solitary '~' on a line by itself to end.\n", pc);

   pc.pc->imm_data->tmp_proc_script
      = new ObjectScript(trigger_cmd, target_num, actor_num, descriminator,
                         takes_precedence);
   pc.pc->imm_data->proc_script_buffer.Clear();
   pc.pc->imm_data->tmp_script_entity_num = obj_num;

   pc.pc->imm_data->edit_string 
      = &(pc.pc->imm_data->proc_script_buffer);

   pc.setMode(MODE_ADD_OBJECT_SCRIPT);
   return 0;
}//add_obj_script


int create_concoction(int rslt, int comp1, int comp2, int comp3, int comp4,
                       int comp5, critter& pc) {

   return do_create_construction(rslt, comp1, comp2, comp3, comp4, comp5, pc,
                                 CONCOCTION);
}//create_concoction

int create_construction(int rslt, int comp1, int comp2, int comp3, int comp4,
                        int comp5, critter& pc) {

   return do_create_construction(rslt, comp1, comp2, comp3, comp4, comp5, pc,
                                 CONSTRUCTION);
}//create_construction


int do_create_construction(int rslt, int comp1, int comp2, int comp3, int comp4,
                           int comp5, critter& pc, ComponentEnum con_type) {

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   /*  First, make sure all object numbers are in range. */
   if (!check_l_range(rslt, 1, NUMBER_OF_ITEMS, pc, TRUE) ||
       !check_l_range(comp1, 1, NUMBER_OF_ITEMS, pc, TRUE) ||
       !check_l_range(comp2, 1, NUMBER_OF_ITEMS, pc, TRUE) ||
       !check_l_range(comp3, 1, NUMBER_OF_ITEMS, pc, TRUE) ||
       !check_l_range(comp4, 1, NUMBER_OF_ITEMS, pc, TRUE) ||
       !check_l_range(comp5, 1, NUMBER_OF_ITEMS, pc, TRUE)) {
      return -1;
   }//if

   if (rslt == 1) {
      pc.show("You must specify a resulting object, other than 1.\n");
      return -1;
   }

   if (comp1 == 1) {
      pc.show("Your components may not be 1, and you must specify at least one.\n");
      return -1;
   }

   /* First, make sure all objects are already created */
   if (!obj_list[rslt].isInUse() || !obj_list[comp1].isInUse()) {
      pc.show("You must create the objects first.\n");
      return -1;
   }//if

   if ((comp2 != 1) && (!obj_list[comp2].isInUse())) {
      pc.show("The second component is not created yet.\n");
      return -1;
   }

   if ((comp3 != 1) && (!obj_list[comp3].isInUse())) {
      pc.show("The third component is not created yet.\n");
      return -1;
   }

   if ((comp4 != 1) && (!obj_list[comp4].isInUse())) {
      pc.show("The fourth component is not created yet.\n");
      return -1;
   }

   if ((comp5 != 1) && (!obj_list[comp5].isInUse())) {
      pc.show("The fifth component is not created yet.\n");
      return -1;
   }

   /*  So, all numbers in bounds, got a valid result, and at least one
    * valid component. */

   /* Now check ownership, must own all components. */
   if (!pc.doesOwnObject(obj_list[rslt])) {
      pc.show("You don't own the resulting object.\n");
      return -1;
   }

   if (!pc.doesOwnObject(obj_list[comp1])) {
      pc.show("You don't own the first component.\n");
      return -1;
   }

   if ((comp2 != 1) && !pc.doesOwnObject(obj_list[comp2])) {
      pc.show("You don't own the second component.\n");
      return -1;
   }

   if ((comp3 != 1) && !pc.doesOwnObject(obj_list[comp3])) {
      pc.show("You don't own the third component.\n");
      return -1;
   }

   if ((comp4 != 1) && !pc.doesOwnObject(obj_list[comp4])) {
      pc.show("You don't own the fourth component.\n");
      return -1;
   }

   if ((comp5 != 1) && !pc.doesOwnObject(obj_list[comp5])) {
      pc.show("You don't own the fifth component.\n");
      return -1;
   }

   /*  Ownership has been satisfied, now lets do it! */
   /* Note that nothing needs to be done to the resulting object. */

   int my_comp1 = comp1;
   int my_comp2 = 0;
   int my_comp3 = 0;
   int my_comp4 = 0;
   int my_comp5 = 0;

   if (comp2 != 1)
      my_comp2 = comp2;

   if (comp3 != 1)
      my_comp3 = comp3;

   if (comp4 != 1)
      my_comp4 = comp4;

   if (comp5 != 1)
      my_comp5 = comp5;

   
   /* now, do comp1 */
   obj_list[comp1].makeComponent(rslt, my_comp1, my_comp2, my_comp3,
                                 my_comp4, my_comp5, con_type);
   if (my_comp2 != 0)
      obj_list[my_comp2].makeComponent(rslt, my_comp1, my_comp2, my_comp3,
                                       my_comp4, my_comp5, con_type);

   if (my_comp3 != 0)
      obj_list[my_comp3].makeComponent(rslt, my_comp1, my_comp2, my_comp3,
                                       my_comp4, my_comp5, con_type);
   if (my_comp4 != 0)
      obj_list[my_comp4].makeComponent(rslt, my_comp1, my_comp2, my_comp3,
                                       my_comp4, my_comp5, con_type);

   if (my_comp5 != 0)
      obj_list[my_comp5].makeComponent(rslt, my_comp1, my_comp2, my_comp3,
                                       my_comp4, my_comp5, con_type);

   pc.show("All done, enjoy!\n");
   return 0;
}//do_create_construction



int ch_odesc(int which_un, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if (which_un == 1) {
     show("USAGE:  ch_odesc [obj_number]\n", pc);
     return -1;
   }//if

   if (!check_l_range(which_un, 2, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;

   if (!obj_list[which_un].OBJ_FLAGS.get(10)) {
      show("This object has not been created yet!\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(obj_list[which_un])) {
      show("You don't own this object.\n", pc);
      return -1;
   }//if

   show("Enter a new long_description for this object.\n", pc);
   show("Use a solitary '~' on a line by itself to end.\n", pc);
   show("You may now paste descs up to 2k, ';' acts as a newline btw.\n",
	pc);

   obj_list[which_un].long_desc.Clear();
   pc.pc->imm_data->edit_string = &(obj_list[which_un].long_desc);
   pc.setMode(MODE_CH_DESC);
   return 0;
}//ch_odesc



int save_mob(int i_th, String* mob_name, critter& pc) {

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   critter* crit = ROOM.haveCritNamed(i_th, mob_name, pc.SEE_BIT);

   if (!crit) {
      show("Which mob?\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(*crit)) {
      show("You don't own this mobile.\n", pc);
      return -1;
   }//if

   mob_list[crit->MOB_NUM] = *crit; //operator overload!!
   mob_list[crit->MOB_NUM].CRITTER_TYPE = 2;

   show("OK, mob as been saved as is.  "
        "Amsave will make the changes permanent.\n", pc);
   return 0;
}//mob_save


int save_obj(int i_th, String* obj_name, critter& pc) {

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   object* vict;
   vict = have_obj_named(pc.inv, i_th, obj_name, pc.SEE_BIT, 
			 ROOM);
   
   if (!vict) {
      vict = ROOM.haveObjNamed(i_th, obj_name, pc.SEE_BIT);
   }

   if (!vict) {
     show("Which obj?  You must have it in your inventory.\n", pc);
     return -1;
   }//if

   if (!pc.doesOwnObject(*vict)) {
      show("You don't own this object.\n", pc);
      return -1;
   }//if

   obj_list[vict->OBJ_NUM].IN_LIST = &(pc.inv); //minor hack
   obj_list[vict->OBJ_NUM] = *vict; //operator overload!!
   obj_list[vict->OBJ_NUM].IN_LIST = NULL;

   show("OK, object as been saved as is.  
        Aosave will make the changes permanent.\n", pc);
   return 0;
}//obj_save

int make_pso(int i_th, const String* shop_keeper, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   critter* crit_ptr = ROOM.haveCritNamed(i_th, shop_keeper, pc.SEE_BIT);

   if (!crit_ptr) {
      pc.show("Which NPC shall I make into a player-run shop owner??\n");
      return -1;
   }//if

   if (!pc.doesOwnCritter(*crit_ptr)) {
      show("You don't own this critter.\n", pc);
      return -1;
   }//if

   // See if we got a shopkeeper...   
   if (!crit_ptr->mob || !crit_ptr->mob->proc_data ||
       !crit_ptr->mob->proc_data->sh_data) {
      pc.show("This is not a shopkeeper, can't make it a player-run shop owner.\n");
      return -1;
   }//if

   if (crit_ptr->isPlayerShopKeeper()) {
      pc.show("Already a Player-run Shop keeper!\n");
      return -1;
   }

   // Ok, do the work...
   
   crit_ptr->mob->proc_data->sh_data->makePSO();
   mob_list[crit_ptr->getIdNum()].mob->proc_data->sh_data->makePSO();

   save_player_shop_owner(*crit_ptr);

   pc.show("Ok, done, don't forget to amsave!!\n");
   return 0;
}//make_pso


int ch_mdesc(int which_un, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if (which_un == 1) {
     show("USAGE:  ch_mdesc [mob_number]\n", pc);
     return -1;
   }//if

   if (!check_l_range(which_un, 2, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[which_un].CRIT_FLAGS.get(18)) {
      show("This critter has not been created yet!\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[which_un])) {
      show("You don't own this critter.\n", pc);
      return -1;
   }//if

   show("Enter a new long_description for this critter.\n", pc);
   show("Use a solitary '~' on a line by itself to end.\n", pc);
   show("You may now paste descs up to 2k, ';' acts as a newline btw.\n",
	pc);

   mob_list[which_un].long_desc.Clear();
   pc.pc->imm_data->edit_string = &(mob_list[which_un].long_desc);
   pc.setMode(MODE_CH_DESC);
   return 0;
}//ch_mdesc


int ch_ddesc(int which_un, critter& pc) {
   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
  
   if (which_un == 1) {
      show("USAGE:  ch_ddesc [door_number]\n", pc);
      return -1;
   }//if

   if (!check_l_range(which_un, 11, NUMBER_OF_DOORS, pc, TRUE))
      return -1;

   if (!door_list[which_un].isInUse()) {
      show("This door has not been created yet!\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnDoor(door_list[which_un])) {
      show("You don't own this door.\n", pc);
      return -1;
   }//if

   show("Enter a new description for this door.\n", pc);
   show("Use a solitary '~' on a line by itself to end.\n", pc);
   show("You may now paste descs up to 2k, ';' acts as a newline btw.\n",
	pc);

   door_list[which_un].long_desc.Clear();
   pc.pc->imm_data->edit_string = &(door_list[which_un].long_desc);
   pc.setMode(MODE_CH_DESC);
   return 0;
}//ch_ddesc



int do_ch_desc(critter& pc) {
   String buf = pc.pc->input.Get_Rest();
   String* edit_str;
   int is_pc_desc = FALSE;

   if (pc.isImmort() && pc.pc->imm_data->edit_string) {
      edit_str = pc.pc->imm_data->edit_string;
   }
   else {
      is_pc_desc = TRUE;
      edit_str = &(pc.long_desc);
   }

   while (TRUE) {
      if (buf.Strlen() == 0) {
         return 0;
      }//if

      if ((buf == "~") || (is_pc_desc && (edit_str->Contains('\n') > 10))) {
         show("Description changed.\n", pc);
         pc.setMode(MODE_NORMAL);
         parse_for_max_80(*(edit_str));

         if (pc.isImmort() && pc.pc->imm_data->edit_string) {
            pc.pc->imm_data->edit_string = NULL;
         }

         return 0;
      }//if

      *(edit_str) += buf;  //append the line to desc
      *(edit_str) += "\n";

      buf = pc.pc->input.Get_Rest();
   }//while
   return 0;
}//do_ch_desc


int do_add_mob_script(critter& pc) {
   String buf = pc.pc->input.Get_Rest();
   mudlog.log(TRC, "In do_add_mob_script...");

   while (TRUE) {
      if (buf.Strlen() == 0) {
         return 0;
      }//if

      if (buf == "~") {
         mudlog.log(TRC, "Script completed...");
         show("Script completed.\n", pc);
         pc.setMode(MODE_NORMAL);
         parse_for_max_80(*(pc.pc->imm_data->edit_string));

         if (GenScript::checkScript(*(pc.pc->imm_data->edit_string))) {

            show("It's too hard to check the script, we'll assume you", pc);
            show("\nknow what you're doing.  If not, edit it again!", pc);

            // Now, lets add it to the mob already!
            // First, make sure the mob still exists
            int mob_num = pc.pc->imm_data->tmp_script_entity_num;
            if (mob_list[mob_num].isInUse()) {
               mudlog.log(TRC, "About to addProcScript");

               mob_list[mob_num].
                  addProcScript(*(pc.pc->imm_data->edit_string),
                                (MobScript*)(pc.pc->imm_data->tmp_proc_script));
               mudlog.log(TRC, "Done with addProcScript");
               pc.pc->imm_data->tmp_proc_script = NULL;
            }//if
         }//if script checks out ok
         else {
            mudlog.log(TRC, "Script failed check!!!");

            show("There was an ERROR IN THE SCRIPT.\n", pc);
            show(*(pc.pc->imm_data->edit_string), pc);

            delete pc.pc->imm_data->tmp_proc_script;
            pc.pc->imm_data->tmp_proc_script = NULL;
         }

         mudlog.log(TRC, "setting edit string to NULL");

	 pc.pc->imm_data->edit_string = NULL;
         return 0;
      }//if

      *(pc.pc->imm_data->edit_string) += buf;  //append the line to desc
      *(pc.pc->imm_data->edit_string) += "\n";
      buf = pc.pc->input.Get_Rest();
   }//while
   return 0;
}//do_add_mob_script


int do_add_room_script(critter& pc) {
   String buf = pc.pc->input.Get_Rest();
   mudlog.log(TRC, "In do_add_room_script...");

   while (TRUE) {
      if (buf.Strlen() == 0) {
         return 0;
      }//if

      if (buf == "~") {
         mudlog.log(TRC, "Script completed...");
         show("Script completed.\n", pc);
         pc.setMode(MODE_NORMAL);
         parse_for_max_80(*(pc.pc->imm_data->edit_string));

         if (GenScript::checkScript(*(pc.pc->imm_data->edit_string))) {
            show("It's too hard to check the script, we'll assume you", pc);
            show("\nknow what you're doing.  If not, edit it again!", pc);

            // Now, lets add it to the mob already!
            // First, make sure the mob still exists
            int rm_num = pc.pc->imm_data->tmp_script_entity_num;
            if (room_list[rm_num].isInUse()) {
               mudlog.log(TRC, "About to addProcScript");

               room_list[rm_num].
                  addProcScript(*(pc.pc->imm_data->edit_string),
                                (RoomScript*)(pc.pc->imm_data->tmp_proc_script));
               pc.pc->imm_data->tmp_proc_script = NULL;
            }//if
         }//if script checks out ok
         else {
            mudlog.log(TRC, "Script failed check!!!");

            show("There was an ERROR IN THE SCRIPT.\n", pc);
            show(*(pc.pc->imm_data->edit_string), pc);

            delete pc.pc->imm_data->tmp_proc_script;
            pc.pc->imm_data->tmp_proc_script = NULL;
         }

         mudlog.log(TRC, "setting edit string to NULL");

	 pc.pc->imm_data->edit_string = NULL;
         return 0;
      }//if

      *(pc.pc->imm_data->edit_string) += buf;  //append the line to desc
      *(pc.pc->imm_data->edit_string) += "\n";
      buf = pc.pc->input.Get_Rest();
   }//while
   return 0;
}//do_add_room_script


int do_add_obj_script(critter& pc) {
   String buf = pc.pc->input.Get_Rest();
   mudlog.log(TRC, "In do_add_obj_script...");

   while (TRUE) {
      if (buf.Strlen() == 0) {
         return 0;
      }//if

      if (buf == "~") {
         mudlog.log(TRC, "Script completed...");
         show("Script completed.\n", pc);
         pc.setMode(MODE_NORMAL);
         parse_for_max_80(*(pc.pc->imm_data->edit_string));

         if (GenScript::checkScript(*(pc.pc->imm_data->edit_string))) {
            show("It's too hard to check the script, we'll assume you", pc);
            show("\nknow what you're doing.  If not, edit it again!", pc);

            // Now, lets add it to the mob already!
            // First, make sure the obj still exists
            int obj_num = pc.pc->imm_data->tmp_script_entity_num;
            if (obj_list[obj_num].isInUse()) {
               mudlog.log(TRC, "About to addProcScript");

               obj_list[obj_num].
                  addProcScript(*(pc.pc->imm_data->edit_string),
                                (ObjectScript*)(pc.pc->imm_data->tmp_proc_script));
               pc.pc->imm_data->tmp_proc_script = NULL;
            }//if
         }//if script checks out ok
         else {
            mudlog.log(TRC, "Script failed check!!!");

            show("There was an ERROR IN THE SCRIPT.\n", pc);
            show(*(pc.pc->imm_data->edit_string), pc);

            delete pc.pc->imm_data->tmp_proc_script;
            pc.pc->imm_data->tmp_proc_script = NULL;
         }

         mudlog.log(TRC, "setting edit string to NULL");

	 pc.pc->imm_data->edit_string = NULL;
         return 0;
      }//if

      *(pc.pc->imm_data->edit_string) += buf;  //append the line to desc
      *(pc.pc->imm_data->edit_string) += "\n";
      buf = pc.pc->input.Get_Rest();
   }//while
   return 0;
}//do_add_obj_script


int ch_rname(const String* rname, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFPR", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (rname->Strlen() < 1) {
      show("See help for ch_rname.\n", pc);
      return -1;
   }//if

   ROOM.short_desc = *rname;

   show("Room's short desc changed.\n", pc);
   return 0;
}//ch_rname


int ch_osdesc(int which_un, const String* name, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (name->Strlen() < 1) {
      show("See help for ch_osdesc.\n", pc);
      return -1;
   }//if


   if (!check_l_range(which_un, 2, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;

   if (!obj_list[which_un].OBJ_FLAGS.get(10)) {
      show("That object hasn't been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(obj_list[which_un])) {
      show("You do not own this object.\n", pc);
      return -1;
   }//if

   obj_list[which_un].short_desc = *name;

   show("Object's short desc changed.\n", pc);
   return 0;
}//ch_osdesc


int ch_ondesc(int which_un, const String* rname, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (rname->Strlen() < 1) {
      show("See help for ch_osdesc.\n", pc);
      return -1;
   }//if

   if (!check_l_range(which_un, 2, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;

   if (!obj_list[which_un].OBJ_FLAGS.get(10)) {
      show("That object hasn't been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(obj_list[which_un])) {
      show("You do not own this object.\n", pc);
      return -1;
   }//if

   obj_list[which_un].in_room_desc = *rname;

   show("Object's in_room desc changed.\n", pc);
   return 0;
}//ch_osdesc


int ch_msdesc(int which_un, const String* rname, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (rname->Strlen() < 1) {
      show("See help for ch_msdesc.\n", pc);
      return -1;
   }//if

   if (!check_l_range(which_un, 2, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[which_un].CRIT_FLAGS.get(18)) {
      show("That critter hasn't been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[which_un])) {
      show("You do not own this critter.\n", pc);
      return -1;
   }//if

   mob_list[which_un].short_desc = *rname;

   show("Mob's short desc changed.\n", pc);
   return 0;
}//ch_msdesc


int ch_mndesc(int which_un, const String* rname, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (rname->Strlen() < 1) {
      show("See help for ch_mndesc.\n", pc);
      return -1;
   }//if

   if (!check_l_range(which_un, 2, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[which_un].isInUse()) {
      show("That critter hasn't been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[which_un])) {
      show("You do not own this critter.\n", pc);
      return -1;
   }//if

   mob_list[which_un].in_room_desc = *rname;

   show("Mob's in room desc changed.\n", pc);
   return 0;
}//ch_mndesc


int add_mname(int which_un, const String* name, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (name->Strlen() < 1) {
      show("See help for add_mname.\n", pc);
      return -1;
   }//if

   if (!check_l_range(which_un, 2, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[which_un].isInUse()) {
      show("That critter hasn't been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[which_un])) {
      show("You do not own this critter.\n", pc);
      return -1;
   }//if

   Put(new String(*name), mob_list[which_un].names);

   show("Added a name to the critter.\n", pc);
   return 0;
}//add_mname


int clear_mnames(int which_un, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(which_un, 2, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[which_un].isInUse()) {
      show("That critter hasn't been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[which_un])) {
      show("You do not own this critter.\n", pc);
      return -1;
   }//if

   clear_ptr_list(mob_list[which_un].names); //all gone!

   show("Cleared the names from the critter.\n", pc);
   return 0;
}//clear_mnames


int add_oname(int which_un, const String* name, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (name->Strlen() < 1) {
      show("See help for add_oname.\n", pc);
      return -1;
   }//if

   if (!check_l_range(which_un, 2, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;

   if (!obj_list[which_un].isInUse()) {
      show("That object hasn't been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(obj_list[which_un])) {
      show("You do not own this object.\n", pc);
      return -1;
   }//if

   obj_list[which_un].names.append(new String(*name));

   show("Added a name to the object.\n", pc);
   return 0;
}//add_oname


int clear_onames(int which_un, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(which_un, 2, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;

   if (!obj_list[which_un].isInUse()) {
      show("That object hasn't been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(obj_list[which_un])) {
      show("You do not own this object.\n", pc);
      return -1;
   }//if

   clear_ptr_list(obj_list[which_un].names); //all gone!

   show("Cleared the names from the object.\n", pc);
   return 0;
}//clear_onames


int rem_oname(int which_un, const String* name, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (name->Strlen() < 1) {
      show("See help for add_oname.\n", pc);
      return -1;
   }//if

   if (!check_l_range(which_un, 2, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;

   if (!obj_list[which_un].isInUse()) {
      show("That object hasn't been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(obj_list[which_un])) {
      show("You do not own this object.\n", pc);
      return -1;
   }//if

   Cell<String*> cll(obj_list[which_un].names);
   String* ptr;

   while ((ptr = cll.next())) {
      if (*ptr == *name) {
         show("OK, name removed.\n", pc);
         delete ptr;
         ptr = obj_list[which_un].names.lose(cll);
         return 0;
      }//if
   }//while
   show("Name not found.\n", pc);
   return -1;
}//rem_oname


int rem_mname(int which_un, const String* name, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (name->Strlen() < 1) {
      show("See help for add_mname.\n", pc);
      return -1;
   }//if

   if (!check_l_range(which_un, 2, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[which_un].isInUse()) {
      show("That critter hasn't been created yet.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnCritter(mob_list[which_un])) {
      show("You do not own this critter.\n", pc);
      return -1;
   }//if

   Cell<String*> cll(mob_list[which_un].names);
   String* ptr;

   while ((ptr = cll.next())) {
     if (*ptr == *name) {
       show("OK, name removed.\n", pc);
       delete ptr;
       ptr = mob_list[which_un].names.lose(cll);
       return 0;
     }//if
   }//while
   show("Name not found.\n", pc);
   return -1;
}//rem_mname






