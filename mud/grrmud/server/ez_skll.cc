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

//*********************** ez_skll.cc ********************//

#include "misc.h"
#include "misc2.h"
#include "classes.h"
#include "spells.h"
#include "battle.h"
#include "skills.h"
#include "commands.h"
#include "batl_prc.h"
#include "ez_skll.h"
#include <PtrArray.h>


int rescue(int i_th, const String* vict, critter& pc) {
   int skill_num = RESCUE_SKILL_NUM;
   critter* ptr = NULL;

   ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT); 

   if (!ptr) {
      show("Rescue who??\n", pc);
      return -1;
   }//if

   if (!ok_to_do_action(ptr, "mKSFP", skill_num,
                        pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (ptr->isMob()) {
      show("That person can't need rescuing!\n", pc);
      return -1;
   }//if

   return do_rescue(*ptr, pc); 
}//rescue


int do_rescue(critter& vict, critter& pc) {
  String buf(100);


  if (vict.isMob()) {
    return -1;
  }//if

  if (IsEmpty(vict.IS_FIGHTING)) {
     Sprintf(buf, "%s isn't fighting anyone!\n", get_his_her(vict));
     buf.Cap();
     show(buf, pc);
     return -1;
  }//if

  if (&vict == &pc) {
     show("There's no help for that poor fool!!\n", pc);
     return -1;
  }//if

  /*  good to go as far as I can tell */

  if (skill_did_hit(pc, RESCUE_SKILL_NUM, vict)) {
     Cell<critter*> cll(vict.IS_FIGHTING);
     critter* ptr;

     while ((ptr = cll.next())) {
        ptr->IS_FIGHTING.loseData(&vict); //rescued no longer in target list
        ptr->IS_FIGHTING.gainData(&pc);   //rescuer in target list
        pc.IS_FIGHTING.gainData(ptr);   //attacker in rescuer's list
     }//while
     /* all are attacking the rescue-er now */

     vict.doRemoveFromBattle(); //no more fighting for that poor loser!
     
     Sprintf(buf, "valiantly rescues %S.\n", name_of_crit(vict, ~0));
     emote(buf, pc, ROOM, TRUE, &vict);
     Sprintf(buf, "You rescue %S.\n", name_of_crit(vict, pc.SEE_BIT));
     show(buf, pc);
     Sprintf(buf, "You are rescued by %S.\n", name_of_crit(pc, vict.SEE_BIT));
     show(buf, vict);
  }//if
  else {
     Sprintf(buf, "fails to rescue %S.", name_of_crit(vict, ~0));
     emote(buf, pc, ROOM, TRUE, &vict);
     Sprintf(buf, "You fail to rescue %S.\n", name_of_crit(vict, pc.SEE_BIT));
     show(buf, pc);
     Sprintf(buf, "%S tries but fails to rescue you.\n", 
             name_of_crit(pc, vict.SEE_BIT));
     buf.Cap();
     show(buf, vict);
  }//else
  return 0;
}//do_rescue


int shield(int i_th, const String* vict, critter& pc) {
   int skill_num = SHIELD_SKILL_NUM;
   critter* ptr;

   if (!ok_to_do_action(NULL, "mSKFP", skill_num, pc, pc.getCurRoom(),
                        NULL, TRUE)) {
      return -1;
   }

   ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT); 

   if (!ptr) {
     show("Shield who??\n", pc);
     return -1;
   }//if

   if (ptr->isMob()) {
      ptr = mob_to_smob(*ptr, pc.getCurRoomNum(), TRUE, i_th, vict, pc.SEE_BIT);
   }//if

   return do_shield(*ptr, pc); 
}//shield


int do_shield(critter& vict, critter& pc) {
   String buf(100);
   
   if (vict.isMob()) {
      return -1;
   }//if
   
   if (pc.temp_crit && (pc.SHIELDING || pc.SHIELDED_BY)) {
      show("You are already shielding or shielded.\n", pc);
      return -1;
   }//if
   
   if (&vict == &pc) {
      show("You already shield yourself as best you can!!\n", pc);
      return -1;
   }//if
   
   if (!pc.temp_crit) {
      pc.temp_crit = new temp_crit_data;
   }//if
   if (!vict.temp_crit) {
      pc.temp_crit = new temp_crit_data;
   }//if
   
   if (vict.SHIELDED_BY || vict.SHIELDING) {
      show("That person is already shielded by or shielding someone.\n", pc);
      return -1;
   }//if
   
   /*  good to go as far as I can tell */
   
   pc.SHIELDING = &vict;
   vict.SHIELDED_BY = &pc;
   
   Sprintf(buf, "You start shielding %S.\n", name_of_crit(vict, pc.SEE_BIT));
   show(buf, pc);
   Sprintf(buf, "starts shielding %S.", name_of_crit(vict, ~0));
   emote(buf, pc, ROOM, TRUE, &vict);
   Sprintf(buf, 
           "%S starts shielding you.  You feel protected by %s strong arms!!\n", 
           name_of_crit(pc, vict.SEE_BIT), get_his_her(pc));
   show(buf, pc);
   return 0;
}//do_shield


int guard(int i_th, const String* vict, critter& pc) {
   int skill_num = GUARD_SKILL_NUM;
   critter* ptr;

   if (!ok_to_do_action(NULL, "mKSFP", skill_num, pc, pc.getCurRoom(),
                        NULL, TRUE)) {
      return -1;
   }

   ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

   if (!ptr) {
     show("Guard who??\n", pc);
     return -1;
   }//if

   if (ptr->isMob()) {
      ptr = mob_to_smob(*ptr, pc.getCurRoomNum(), TRUE, i_th, vict, pc.SEE_BIT);
   }//if

   return do_guard(*ptr, pc); 
}//guard


int do_guard(critter& vict, critter& pc) {
   String buf(100);

   if (vict.isMob()) {
      return -1; 
   }//if

   if (pc.temp_crit && (pc.GUARDING || pc.GUARDED_BY)) {
      show("You are already guarding or guarded.\n", pc);
      return -1;
   }//if

   if (&vict == &pc) {
      show("You already guard yourself as best you can!!\n", pc);
      return -1;
   }//if

   if (!pc.temp_crit) {
      pc.temp_crit = new temp_crit_data;
   }//if

   if (!vict.temp_crit) {
      vict.temp_crit = new temp_crit_data;
   }//if

   if (vict.GUARDED_BY || vict.GUARDING) {
      show("That person is already guarded by or guarding someone.\n", pc);
      return -1;
   }//if

   /*  good to go as far as I can tell */
   
   pc.GUARDING = &vict;
   vict.GUARDED_BY = &pc;
   
   Sprintf(buf, "You start guarding %S.\n", name_of_crit(vict, pc.SEE_BIT));
   show(buf, pc);
   Sprintf(buf, "starts guarding %S.", name_of_crit(vict, ~0));
   emote(buf, pc, ROOM, TRUE, &vict);
   Sprintf(buf, "%S starts guarding you.  You feel much safer!!\n", 
           name_of_crit(pc, vict.SEE_BIT));
   show(buf, pc);
   return 0;
}//do_guard


int picklock(int i_th, const String* vict, critter& pc) {
   int skill_num = PICKLOCK_SKILL_NUM;
   door* dr;
   object* obj;

   if (!ok_to_do_action(NULL, "mKBSFP", skill_num, pc, pc.getCurRoom(),
                        NULL, TRUE)) {
      return -1;
   }

   dr = door::findDoor(ROOM.DOORS, i_th, vict, pc.SEE_BIT, ROOM); 
   if (!dr) {
      obj = have_obj_named(pc.inv, i_th, vict, pc.SEE_BIT, ROOM);
      if (!obj) {
         obj = ROOM.haveObjNamed(i_th, vict, pc.SEE_BIT);
      }//if
      if (!obj) {
         show("You don't see that here.\n", pc); 
         return -1;
      }//if
      return do_picklock(*obj, pc);
   }//if

   return do_picklock(*dr, pc); 
}//picklock


int do_picklock(door& dr, critter& pc) {

   if (skill_did_hit(pc, PICKLOCK_SKILL_NUM, pc)) {
      if (!dr.dr_data->door_data_flags.get(2))
         show("It isn't even closed.\n", pc);
      else if (!dr.dr_data->door_data_flags.get(3))
         show("It isn't locked!!\n", pc);
      else if (!dr.dr_data->door_data_flags.get(4))
         show("It can't be opened so easily.\n", pc);
      else { //good to go
         show("You skillfully pick the lock.\n", pc);
         emote("skillfully picks the lock.", pc, ROOM, TRUE);
         dr.dr_data->door_data_flags.turn_off(3); //no longer locked
         return 0;
      }//else
   }//if it worked
   else {
      show("You strain your eyes and fingers, but accomplish nothing.\n", 
	    pc);
      emote("fiddles with the lock in vain.", pc, ROOM, TRUE);
      pc.PAUSE += 3;
   }//else
   return -1;
}//do_picklock


int do_picklock(object& obj, critter& pc) { //for objects

   if (skill_did_hit(pc, PICKLOCK_SKILL_NUM, pc)) {
      if (!obj.bag)
	 show("That is not a container.\n", pc);
      else if (!obj.BAG_FLAGS.get(2))
         show("It isn't even closed.\n", pc);
      else if (!obj.BAG_FLAGS.get(3))
         show("It isn't locked!!\n", pc);
      else if (!obj.BAG_FLAGS.get(4))
         show("It can't be opened so easily.\n", pc);
      else { //good to go
         show("You skillfully pick the lock.\n", pc);
         emote("skillfully picks the lock.", pc, ROOM, TRUE);
         obj.BAG_FLAGS.turn_off(3); //no longer locked
         return 0;
      }//else
   }//if it worked
   else {
      show("You strain your eyes and fingers, but accomplish nothing.\n", 
	    pc);
      emote("fiddles with the lock in vain.", pc, ROOM, TRUE);
      pc.PAUSE += 3;
   }//else
   return -1;
}//do_picklock, for objects


int earthmeld(critter& pc) {
   int spell_num = EARTHMELD_SKILL_NUM;

   if (!ok_to_do_action(NULL, "mKNBSFP", spell_num, pc, pc.getCurRoom(),
                        NULL, TRUE)) {
      return -1;
   }

   return do_earthmeld(pc);
}//earthmeld


int do_earthmeld(critter& pc) {
   stat_spell_cell* sp = NULL;
   int spell_num = EARTHMELD_SKILL_NUM; 

   // Check for a forrest room. 
   if (ROOM.canCamp()) {
      if ((sp = is_affected_by(spell_num, pc))) {
         pc.breakEarthMeld();
         return 0;
      }//if
      else {
	 pc.show("You entwine yourself in the powers of nature, binding
 yourself to the earth's embrace.\n");
      }//else
      
      /* it lasts untill mana runs out, or the player moves */
      pc.affected_by.pushFront(new stat_spell_cell(spell_num, -1));
      return 0;
   }//if
   else {
      pc.show("You cannot reach the earth's embrace here.\n");
      return -1;
   }//else
}//do_earthmeld














