// $Id: olc2.cc,v 1.16 1999/08/27 03:10:04 greear Exp $
// $Revision: 1.16 $  $Author: greear $ $Date: 1999/08/27 03:10:04 $

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


// int normalize_obj(object& obj, bool just_check, critter* pc) {
//
//  If pc is !NULL, then send messages to it when possible.  This will
// allow the PC to find out what's wrong with the object and fix it, or
// complain about it.
//
// Note:  even if just_check is set, certain things (the 1024 vis bit,
// some of the weapon type and wearable flags, certain quantities if negative)
// can be altered.  I consider this a feature, but could be convinced to
// "fix" it if necessary.
//                                                     - angelbob
//  Sounds good to me...
//  Now we need to create some methods to audit and/or fix zones.. --Ben
//
#ifdef n_show
#warning Macro name already defined!  Change n_show in olc2.cc
#endif

#define n_show(string, pc)  if(pc) show(string, *pc)

int normalize_obj(object& obj, bool just_check, critter* pc) {
   //NOTE: Using a String object, and Sprintf will guarantee that
   // you never overflow (it grows). --Ben
  char buffer[4096];  // Pick a nice high value, just for safety

  sprintf(buffer,
	  "Normalizing object:  finding limits by object level %d.\n",
	  obj.OBJ_LEVEL);
  n_show(buffer, pc);

   if (obj.OBJ_NUM != obj_list[obj.OBJ_NUM].OBJ_NUM) {
      mudlog << "ERROR:  normalize_obj, object numbers not in sync:  "
             << " obj.OBJ_NUM:  " << obj.OBJ_NUM << "  other:  "
             << obj_list[obj.OBJ_NUM].OBJ_NUM << endl;
   }//if

   // TODO:  check charges, recharges, %load, CIG, max_in_game,
   //        weight(?), price (?)
   // TODO:  check in_room_desc for lack of period, and short_desc for starting
   //        capital letter.

   // If not worn is on, then zero out the other worn flags just in case.
   if (obj.OBJ_FLAGS.get(21)) { 
      for (int i = 22; i<40; i++) {
	if(obj.OBJ_FLAGS.get(i)) {
	  sprintf(buffer, "Not_worn is on:  turning off flag %d.\n", i);
	  n_show(buffer, pc);
	  obj.OBJ_FLAGS.turn_off(i);
	} // if
      }//for
   }//if

   // Turn on vis bit 1024
   if(!(obj.OBJ_VIS_BIT & 1024)) {
     n_show("Turning on vis bit 1024.\n", pc);
     obj.OBJ_VIS_BIT = obj.OBJ_VIS_BIT | 1024;
   }

   if (!obj.bag) {
      if (obj.inv.size() != 0) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "ERROR:  normalize_obj, object with inventory is not bag:  "
                   << " obj.OBJ_NUM:  " << obj.OBJ_NUM << endl;
         }
      }
   }

   int stat_plus;
   int other_plus;
   if(obj.OBJ_LEVEL < 0) {
     obj.OBJ_LEVEL = 0;
     n_show("You can't have a negative object level.  Making it 0.\n", pc);
   }
   if(obj.OBJ_LEVEL <= 10) {
     stat_plus = 2;
     other_plus = 20;
   } else if (obj.OBJ_LEVEL <= 20) {
     stat_plus = 3;
     other_plus = 50;
   } else if (obj.OBJ_LEVEL <= 30) {
     stat_plus = 4;
     other_plus = 75;
   } else if (obj.OBJ_LEVEL <= 40) {
     stat_plus = 5;
     other_plus = 100;
   } else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "ERROR: normalize_obj, object level not in <= 40 range:  "
                << " obj.OBJ_NUM:  " << obj.OBJ_NUM << endl;
      }
      n_show("Illegal object level, using level 1 bounds.\n", pc);
      if(!just_check) {
	obj.OBJ_LEVEL = 1;
      }
      stat_plus = 2;
      other_plus = 20;
   }

   // If obj is not a weapon, don't let it be any particular kind of weapon
   if(obj.OBJ_FLAGS.get(40)) {
      for (int i = 41; i<49; i++) {
	if(obj.OBJ_FLAGS.get(i)) {
	  sprintf(buffer, "Object is not_weapon, turning off flag %d.\n", i);
	  n_show(buffer, pc);
	  obj.OBJ_FLAGS.turn_off(i);
	}
      }//for
      obj.OBJ_FLAGS.turn_off(57);
   }

   // Figure out how stompy a weapon is
   if(obj.OBJ_DAM_DICE_COUNT < 0) {
     obj.OBJ_DAM_DICE_COUNT = 0;
     n_show("Weapon can't have negative dice count.  Fixing.\n", pc);
   }
   if(obj.OBJ_DAM_DICE_SIDES < 0) {
     obj.OBJ_DAM_DICE_SIDES = 0;
     n_show("Weapon can't have negative dice sides.  Fixing.\n", pc);
   }

   // TODO:
   // Actually, average damage might be a better thing to count.
   // After all, a 30d1 sword is much better than a 1d30. --Ben
   // NOTE:  there is a d(int cnt, int sides) method...

   // Average for 1 die is (1 + DIE_SIDES)/2...  Add an extra half to the
   // end before truncate, so we round 1/2 hp up on damage count.
   int dam_avg = int((float(obj.OBJ_DAM_DICE_COUNT)
		      * (float(obj.OBJ_DAM_DICE_SIDES) + 0.5)) + 0.5);

   int dam_max = obj.OBJ_DAM_DICE_COUNT * obj.OBJ_DAM_DICE_SIDES;
   int bracket = (obj.OBJ_LEVEL % 5) ? (obj.OBJ_LEVEL/5) + 1 : (obj.OBJ_LEVEL/5);
   int dam_max_allowed = 6 + 4*bracket;
   int dam_avg_allowed = 6 + 2*bracket;  // Leave bounds loose for now
   if((dam_max > dam_max_allowed) || (dam_avg > dam_avg_allowed)) {
     n_show("Damage average or max too high for object level.\n", pc);
   }
   while(!just_check
	 && ((dam_max > dam_max_allowed) || (dam_avg > dam_avg_allowed))) {
     if(obj.OBJ_DAM_DICE_COUNT > obj.OBJ_DAM_DICE_SIDES) {
       obj.OBJ_DAM_DICE_COUNT--;
     } else {
       obj.OBJ_DAM_DICE_SIDES--;
     }
     dam_max = obj.OBJ_DAM_DICE_COUNT * obj.OBJ_DAM_DICE_SIDES;
     dam_avg = int((float(obj.OBJ_DAM_DICE_COUNT)
		    * (float(obj.OBJ_DAM_DICE_SIDES) + 0.5)) + 0.5);
   }

   Cell<StatBonus*> cll(obj.stat_affects);
   StatBonus* ptr, *tptr;
   int ss, bd, old_bd;
   ptr = cll.next();
   int stat_goodies = 0;
   int max_bonus = stat_plus * 2;
   // is_bonus is the amount of bonus (from max_bonus total) that the
   // current entry amounts to.  For a bad stat affect, this is zero,
   // or it can be up to four or five for extremely good affects.
   int is_bonus;

   while (ptr) {
      is_bonus = 0;
      ss = ptr->stat;
      if ((ss == 12) || (ss == 13) || (ss == 14) ||  // Sex, class, race
          (ss == 11) || (ss == 0) ||                 // pause count, position
          (ss == 19) || (ss == 20) || (ss == 21) ||  // level, hometown, wimpy
          (ss == 22) || (ss == 26) || (ss == 33) ||  // pracs, mob type, religion
          (ss == 34) || (ss == 40) ||                // zone#, guild
          (ss < 0) ||                                // illegal (negative)
          ((ss >= MOB_SHORT_CUR_STATS) && (ss != 100) &&
           (ss != 101) && (ss != 102))) {            // illegal (not hunger/thirst/drug)
         tptr = ptr;
         ptr = obj.stat_affects.lose(cll);
         delete tptr;
      }//if
      else {
         old_bd = bd = ptr->bonus;
         if ((ss >= 1) && (ss <= 6)) {  // stats
            bd = bound(-2*stat_plus, stat_plus, bd);
	    bd = bound(-2*stat_plus, max_bonus - stat_goodies, bd);
	    if(bd > 0) is_bonus = bd;
	    if(bd != old_bd) {
	      sprintf(buffer, "Value %d for stat %d disallowed.\n",
		      old_bd, ss);
	      n_show(buffer, pc);
	    }
         }
	 else if ((ss == 7) || (ss == 8)) {             // To hit, damage
 	    bd = bound(-6, 3, bd);
	    if(bd > 0) is_bonus = 1;
	    if(bd != old_bd) {
	      sprintf(buffer, "Value %d for stat %d disallowed.\n",
		      old_bd, ss);
	      n_show(buffer, pc);
	    }
	 }
         else if (ss == 9) {            // ac
	    // TODO:  finish
	    // Note: armor in multiple categories (heavy/light) should be
	    // restricted as lightest category, to prevent abuse.
	    if (obj.OBJ_FLAGS.get(23)
	       || obj.OBJ_FLAGS.get(24)  // neck
	       || obj.OBJ_FLAGS.get(25)  // cloak (around body)
	       || obj.OBJ_FLAGS.get(27)
	       || obj.OBJ_FLAGS.get(28)  // bracelets (wrists)
	       || obj.OBJ_FLAGS.get(32)  // light source
	       || obj.OBJ_FLAGS.get(34)  // belt
	       || obj.OBJ_FLAGS.get(37)
	       || obj.OBJ_FLAGS.get(38)  // fingers
	       ) {
	      // This is light armor.
	      bd = bound(-bracket, 2*bracket, bd);
	      if(bd != old_bd) {
		sprintf(buffer,
			"Value %d for light armor disallowed.\n",
			old_bd);
		n_show(buffer, pc);
	      }
	    } else if (obj.OBJ_FLAGS.get(22)     // head
		      || obj.OBJ_FLAGS.get(26)  // arms
		      || obj.OBJ_FLAGS.get(29)  // gloves (hands)
		      || obj.OBJ_FLAGS.get(30)  // wielded
		      || obj.OBJ_FLAGS.get(31)  // held
		      || obj.OBJ_FLAGS.get(35)  // legs
		      || obj.OBJ_FLAGS.get(36)  // boots (feet)
		      ) {
	      // This is medium armor
	      bd = bound(-3*bracket, 6*bracket, bd);
	      if(bd != old_bd) {
		sprintf(buffer,
			"Value %d for medium armor disallowed.\n",
			old_bd);
		n_show(buffer, pc);
	      }
	    } else if (obj.OBJ_FLAGS.get(33) || obj.OBJ_FLAGS.get(39)) {
	      // This is body armor and/or a shield
	      bd = bound(-5*bracket, 10*bracket, bd);
	      if(bd != old_bd) {
		sprintf(buffer,
			"Value %d for heavy armor disallowed.\n",
			old_bd);
		n_show(buffer, pc);
	      }
	    } else {
	      // Don't know what it is :-(
	      bd = bound(-other_plus, 2*other_plus, bd);
	      if(bd != old_bd) {
		sprintf(buffer,
			"Value %d for unusual armor type disallowed.\n",
			old_bd);
		n_show(buffer, pc);
	      }
	    }
	    if (bd < 0)
               is_bonus = 1;
         }
         else if (ss == 10) {           // attacks
            bd = bound(-1, 2, bd);
	    bd = bound(-1, max_bonus - stat_goodies, bd);
	    if(bd > 0) is_bonus = bd;
	    if(old_bd != bd) {
	      n_show("Too many attacks added or subtracted!\n", pc);
	    }
         }
         else if ((ss >= 15) && (ss <= 17)) {
 	    bd = bound(-200*stat_plus, 100*stat_plus, bd);  // HP, mana, move
	    if(bd > 0) is_bonus = 1;
	    if(old_bd != bd) {
	      n_show("Too much current hp/mana/move added or subtracted.\n",
		     pc);
	    }
         }
         else if (ss == 18) {           // alignment
	    bd = bound(-50, 50, bd);    // I'll just leave this as is
         }
         else if ((ss >= 23) && (ss <= 25)) {  // hp, mana, move maxima
            bd = bound(-30*stat_plus, 15*stat_plus, bd);
	    if(bd > 0) is_bonus = 1;
	    if(old_bd != bd) {
	      n_show("Too much max hp/mana/move added or subtracted.\n", pc);
	    }
         }
         else if (ss == 27) {           // Damage received
            bd = bound((-other_plus)/3, 2*other_plus/3, bd);
	    bd = bound(-5*(max_bonus - stat_goodies), 2*other_plus/3, bd);
	    if(bd < 0) is_bonus = (-bd + 4)/5; // bonus of 1 for each 5%
	    if(old_bd != bd) {
	      n_show("Too much dam_received affected.\n", pc);
	    }
         }
	 else if (ss == 28) {           // Damage given
            bd = bound((-2*other_plus)/3, other_plus/3, bd);
	    bd = bound((-other_plus)/3, 5*(max_bonus - stat_goodies), bd);
	    if(bd > 0) is_bonus = (bd + 4)/5;  // bonus of 1 for each 5%
	    if(old_bd != bd) {
	      n_show("Too much dam_given affected.\n", pc);
	    }
	 }
         else if ((ss >= 29) && (ss <= 32)) {  // resistances
            bd = bound(-other_plus, 2*other_plus, bd);
	    if(bd < 0) is_bonus = 1;
	    if(old_bd != bd) {
	      sprintf(buffer, "Changed resistance %d by %d.  Disallowed.\n",
		      ss, old_bd);
	      n_show(buffer, pc);
	    }
         }
         else if ((ss == 35) || (ss == 36)) {  // Bare hand dice,
	                                       // bare hand sides
            bd = bound(-2*stat_plus, stat_plus, bd);
	    if(bd > 0) is_bonus = 1;
	    if(old_bd != bd) {
	      n_show("Barehand dice/sides affect out of range.\n", pc);
	    }
         }
         else if ((ss >= 37) && (ss <= 39)) {  // Hp/mana/move regen
            bd = bound(-other_plus, other_plus/2, bd);
	    if(bd > 0) is_bonus = 1;
	    if(old_bd != bd) {
	      n_show("Hp/mana/mov regen out of range.  Disallowed.\n", pc);
	    }
         }
         else if ((ss >= 100) && (ss <= 102)) { // Hunger, thirst, drugged
	    bd = bound(-50, 50, bd);
	    if(old_bd != bd) {
	      n_show("Hunger/Thirst/Drug out of range.\n", pc);
	    }
         }

	 stat_goodies += is_bonus;
	 if(!just_check) {
            ptr->bonus = bd;
	 }
	 if((bd == 0) || (stat_goodies > max_bonus)) {
	   // Remove ineffective stat mods, or ones that are too much
	   if(bd != 0) {
	     sprintf(buffer,
		     "Too many (or too high) bonuses, stat %d, amt %d.\n",
		     ss, old_bd);
	   } else {
	     sprintf(buffer, "Empty mod for %d.\n", ss);
	   }
	   n_show(buffer, pc);
	   if(!just_check) {
	     // Delete that puppy.
	     tptr = ptr;
	     ptr = obj.stat_affects.lose(cll);
	     delete tptr;
	   }
	 } // (bd == 0) || (stat_goodies > max_bonus)

         if(ptr) ptr = cll.next();
      }//else
   }//while

   // no more than 5
   while (obj.stat_affects.size() > 5) {
      delete (obj.stat_affects.popBack());
   }

   // If it can 'dissolve', then it must not be wearable, because it
   // makes a real mess when a corpse disolves and you are holding it!!
   // It could be made to work, but not worth the trouble IMHO. --BEN
   if (obj.getIdNum() == CORPSE_OBJECT) {
      // Make sure it can't be worn.
      obj.OBJ_FLAGS.turn_on(21);
   }

   return 0;
}//normalize_obj

#undef n_show


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
   crit.setHpMax(bound(1, 32000, crit.getHpMax()));
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
  dr_data.setVisBit(dr_data.getVisBit() | 1024);
  return 0;
}//normalize_door(data)

int rm_keyword(int kwd_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFPRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   SCell<KeywordPair*> cll(ROOM.keywords);
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
   KeywordPair* kp = new KeywordPair();
   ROOM.keywords.append(kp);
   ROOM.normalize();
   pc.show("Added a new Keyword at index 0.\n");
   return 0;
}//add_keyword


int clear_keyword(int kwd_num, critter& pc) {
   if (!ok_to_do_action(NULL, "IFPRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   SCell<KeywordPair*> cll(ROOM.keywords);
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
   
   SCell<KeywordPair*> cll(ROOM.keywords);
   KeywordPair* ptr;
   int cnt = 0;
   ptr = cll.next();
   while (ptr) {
      if (cnt == kwd_num) {
         LString ls(pc.getLanguage(), *name);
         ptr->addName(ls);
         pc.show(CS_NAME_ADDED);
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
   
   SCell<KeywordPair*> cll(ROOM.keywords);
   KeywordPair* ptr;
   int cnt = 0;
   ptr = cll.next();
   while (ptr) {
      if (cnt == kwd_num) {
         show("Enter a new room description for this Keyword.\n", pc);
         show("Use a solitary '~' on a line by itself to end.\n", pc);
         show("You may now paste descs up to 2k, ';' acts as a newline btw.\n",
              pc);

         LString ls(pc.getLanguage(), "");
         ptr->addLongDesc(ls); //clear it out...
         pc.pc->imm_data->edit_string = ptr->getLongDescColl();
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

   SCell<KeywordPair*> cll(ROOM.keywords);
   KeywordPair* ptr;
   int cnt = 0;
   while ((ptr = cll.next())) {
      if (cnt == kwd_num) {
         String buf(500);
         ptr->toStringStat(&pc, buf, ST_ALL);
         pc.show(buf);
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

   Cell<StatBonus*> cll(obj_ptr->stat_affects);
   StatBonus* ptr;
   ptr = cll.next();
   while (ptr) {
      if (ptr->stat == stat_num) {
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

   obj_ptr->stat_affects.append(new StatBonus(stat_num, val));
   if (!(pc.getImmLevel() >= 9)) {
      normalize_obj(*obj_ptr);
   }
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

   Cell<SpellDuration*> cll(obj_ptr->CASTS_THESE_SPELLS);
   SpellDuration* ptr;
   ptr = cll.next();
   while (ptr) {
      if (ptr->spell == spell_num) {
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
   SpellDuration* ptr = new SpellDuration();
   ptr->duration = level;
   ptr->spell = spell;
   obj_ptr->CASTS_THESE_SPELLS.append(ptr);
   obj_ptr->OBJ_FLAGS.turn_on(63); //so it will write out spec procs

   pc.show("Spell was added, don't forget to aosave.\n");
   return 0;
}//add_casts_spell


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
   pc.pc->imm_data->clear();
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

   LString ls(pc.getLanguage(), "");
   ROOM.addLongDesc(ls); //clear it out.
   pc.pc->imm_data->edit_string = ROOM.getLongDescColl();
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

   String buf(500);
   pcell->toStringStat(veh_id, path_cell_num, pc, buf, ST_ALL);
   pc.show(buf);
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

   LString ls(pc.getLanguage(), "");
   pcell->getDescColl()->addLstring(ls); //blank it out.
   pc.pc->imm_data->edit_string = pcell->getDescColl();
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
      = new GenScript(trigger_cmd, target_num, actor_num, descriminator,
                      takes_precedence);
   pc.pc->imm_data->proc_script_buffer.clearAndDestroy();
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
      = new GenScript(trigger_cmd, target_num, actor_num, descriminator,
                       takes_precedence);
   pc.pc->imm_data->proc_script_buffer.clearAndDestroy();
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
      = new GenScript(trigger_cmd, target_num, actor_num, descriminator,
                         takes_precedence);
   pc.pc->imm_data->proc_script_buffer.clearAndDestroy();
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

   LString ls(pc.getLanguage(), "");
   obj_list[which_un].addLongDesc(ls); //clear it out.
   pc.pc->imm_data->edit_string = obj_list[which_un].getLongDescColl();
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

   obj_list[vict->OBJ_NUM].setModified(TRUE);; //minor hack
   obj_list[vict->OBJ_NUM] = *vict; //operator overload!!
   obj_list[vict->OBJ_NUM].setModified(FALSE);

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

   LString ls(pc.getLanguage(), "");
   mob_list[which_un].addLongDesc(ls);
   pc.pc->imm_data->edit_string = mob_list[which_un].getLongDescColl();
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

   LString ls(pc.getLanguage(), "");
   door_list[which_un].addLongDesc(ls); //clear it.
   pc.pc->imm_data->edit_string = door_list[which_un].getLongDescColl();
   pc.setMode(MODE_CH_DESC);
   return 0;
}//ch_ddesc


int do_ch_desc(critter& pc) {
   String buf = pc.pc->input.Get_Rest();
   LStringCollection* edit_str;
   int is_pc_desc = FALSE;

   if (pc.isImmort() && pc.pc->imm_data->edit_string) {
      edit_str = pc.pc->imm_data->edit_string;
   }
   else {
      is_pc_desc = TRUE;
      edit_str = pc.getLongDescColl();
   }

   while (TRUE) {
      if (buf.Strlen() == 0) {
         return 0;
      }//if

      if ((buf == "~") || 
          (is_pc_desc && 
           (edit_str->getString(&pc)->contains('\n') > 10))) {
         show("Description changed.\n", pc);
         pc.setMode(MODE_NORMAL);
         parse_for_max_80(*(edit_str->getString(&pc)));

         if (pc.isImmort() && pc.pc->imm_data->edit_string) {
            pc.pc->imm_data->edit_string = NULL;
         }

         return 0;
      }//if
      LString ls(pc.getLanguage(), buf);
      ls.append("\n");
      edit_str->appendString(ls);
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
         parse_for_max_80(*(pc.pc->imm_data->edit_string->getString(&pc)));

         if (GenScript::checkScript(*(pc.pc->imm_data->edit_string->getString(&pc)))) {

            show("It's too hard to check the script, we'll assume you", pc);
            show("\nknow what you're doing.  If not, edit it again!", pc);

            // Now, lets add it to the mob already!
            // First, make sure the mob still exists
            int mob_num = pc.pc->imm_data->tmp_script_entity_num;
            if (mob_list[mob_num].isInUse()) {
               mudlog.log(TRC, "About to addProcScript");

               mob_list[mob_num].
                  addProcScript(*(pc.pc->imm_data->edit_string->getString(&pc)),
                                pc.pc->imm_data->tmp_proc_script);
               mudlog.log(TRC, "Done with addProcScript");
               pc.pc->imm_data->tmp_proc_script = NULL;
            }//if
         }//if script checks out ok
         else {
            mudlog.log(TRC, "Script failed check!!!");

            show("There was an ERROR IN THE SCRIPT.\n", pc);
            pc.show(*(pc.pc->imm_data->edit_string->getString(&pc)));

            delete pc.pc->imm_data->tmp_proc_script;
            pc.pc->imm_data->tmp_proc_script = NULL;
         }

         mudlog.log(TRC, "setting edit string to NULL");

	 pc.pc->imm_data->edit_string = NULL;
         return 0;
      }//if

      LString ls(pc.getLanguage(), buf);
      ls.append("\n");
      pc.pc->imm_data->edit_string->appendString(ls);
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
         parse_for_max_80(*(pc.pc->imm_data->edit_string->getString(&pc)));

         if (GenScript::checkScript(*(pc.pc->imm_data->edit_string->getString(&pc)))) {
            show("It's too hard to check the script, we'll assume you", pc);
            show("\nknow what you're doing.  If not, edit it again!", pc);

            // Now, lets add it to the mob already!
            // First, make sure the mob still exists
            int rm_num = pc.pc->imm_data->tmp_script_entity_num;
            if (room_list[rm_num].isInUse()) {
               mudlog.log(TRC, "About to addProcScript");

               room_list[rm_num].
                  addProcScript(*(pc.pc->imm_data->edit_string->getString(&pc)),
                                pc.pc->imm_data->tmp_proc_script);
               pc.pc->imm_data->tmp_proc_script = NULL;
            }//if
         }//if script checks out ok
         else {
            mudlog.log(TRC, "Script failed check!!!");

            show("There was an ERROR IN THE SCRIPT.\n", pc);
            pc.show(*(pc.pc->imm_data->edit_string->getString(&pc)));

            delete pc.pc->imm_data->tmp_proc_script;
            pc.pc->imm_data->tmp_proc_script = NULL;
         }

         mudlog.log(TRC, "setting edit string to NULL");

	 pc.pc->imm_data->edit_string = NULL;
         return 0;
      }//if

      LString ls(pc.getLanguage(), buf);
      ls.append("\n");

      pc.pc->imm_data->edit_string->appendString(ls);
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
         parse_for_max_80(*(pc.pc->imm_data->edit_string->getString(pc.getLanguage())));

         if (GenScript::checkScript(*(pc.pc->imm_data->edit_string->getString(pc.getLanguage())))) {
            show("It's too hard to check the script, we'll assume you", pc);
            show("\nknow what you're doing.  If not, edit it again!", pc);

            // Now, lets add it to the mob already!
            // First, make sure the obj still exists
            int obj_num = pc.pc->imm_data->tmp_script_entity_num;
            if (obj_list[obj_num].isInUse()) {
               mudlog.log(TRC, "About to addProcScript");

               obj_list[obj_num].
                  addProcScript(*(pc.pc->imm_data->edit_string->getString(pc.getLanguage())),
                                pc.pc->imm_data->tmp_proc_script);
               pc.pc->imm_data->tmp_proc_script = NULL;
            }//if
         }//if script checks out ok
         else {
            mudlog.log(TRC, "Script failed check!!!");

            show("There was an ERROR IN THE SCRIPT.\n", pc);
            pc.show(*(pc.pc->imm_data->edit_string->getString(pc.getLanguage())));

            delete pc.pc->imm_data->tmp_proc_script;
            pc.pc->imm_data->tmp_proc_script = NULL;
         }

         mudlog.log(TRC, "setting edit string to NULL");

	 pc.pc->imm_data->edit_string = NULL;
         return 0;
      }//if

      LString ls(pc.getLanguage(), buf);
      pc.pc->imm_data->edit_string->appendString(ls);
      buf = pc.pc->input.Get_Rest();
   }//while
   return 0;
}//do_add_obj_script


int ch_rname(String* rname, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFPR", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (rname->Strlen() < 1) {
      show("See help for ch_rname.\n", pc);
      return -1;
   }//if

   ROOM.addShortDesc(pc.getLanguage(), *rname);

   show("Room's short desc changed.\n", pc);
   return 0;
}//ch_rname


int ch_osdesc(int which_un, String* name, critter& pc) {
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

   obj_list[which_un].addShortDesc(pc.getLanguage(), *name);

   show("Object's short desc changed.\n", pc);
   return 0;
}//ch_osdesc


int ch_ondesc(int which_un, String* rname, critter& pc) {
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

   obj_list[which_un].addInRoomDesc(pc.getLanguage(), *rname);

   show("Object's in_room desc changed.\n", pc);
   return 0;
}//ch_osdesc


int ch_msdesc(int which_un, String* rname, critter& pc) {
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

   mob_list[which_un].addShortDesc(pc.getLanguage(), *rname);

   show("Mob's short desc changed.\n", pc);
   return 0;
}//ch_msdesc


int ch_mndesc(int which_un, String* rname, critter& pc) {
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

   mob_list[which_un].addInRoomDesc(pc.getLanguage(), *rname);

   show("Mob's in room desc changed.\n", pc);
   return 0;
}//ch_mndesc


int add_mname(int which_un, String* name, critter& pc) {
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

   mob_list[which_un].addName(pc.getLanguage(), *name);

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

   mob_list[which_un].getNames().clearLanguage(pc.getLanguage());

   show("Cleared the names from the critter.\n", pc);
   return 0;
}//clear_mnames


int add_oname(int which_un, String* name, critter& pc) {
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

   obj_list[which_un].addName(pc.getLanguage(), *name);

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

   obj_list[which_un].getNames().clearLanguage(pc.getLanguage()); //all gone!

   show("Cleared the names from the object.\n", pc);
   return 0;
}//clear_onames


int rem_oname(int which_un, String* name, critter& pc) {
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

   if (obj_list[which_un].removeName(pc.getLanguage(), *name)) {
      pc.show(CS_NAME_REMOVED);
      return 0;
   }
   else {
      pc.show(CS_NAME_NOT_FOUND);
      return -1;
   }
}//rem_oname


int rem_mname(int which_un, String* name, critter& pc) {
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

   if (mob_list[which_un].removeName(pc.getLanguage(), *name)) {
      pc.show(CS_NAME_REMOVED);
      return 0;
   }
   else {
      pc.show(CS_NAME_NOT_FOUND);
      return -1;
   }
}//rem_mname






