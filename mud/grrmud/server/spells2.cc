// $Id: spells2.cc,v 1.13 1999/09/01 06:00:04 greear Exp $
// $Revision: 1.13 $  $Author: greear $ $Date: 1999/09/01 06:00:04 $

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

///**********************  spells2.cc  *******************************///

/* These are mostly clerical spells.  They have no fatalities and
   no battle will be started.  */

#include "commands.h"
#include "command2.h"
#include <stdio.h>
#include "misc.h"
#include "misc2.h"
#include "battle.h"
#include "classes.h"
#include "spells.h"
#include "skills.h"
#include <math.h>
#include "spells2.h"
#include <PtrArray.h>
#include "zone.h"



void do_cast_locate(const String* targ, critter& agg, int is_canned,
			      int lvl) {
   String buf(100);
   int spell_num = LOCATE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (targ->Strlen() < 1) {
      show("Locate what?\n", agg);
      return;
   }//if
   else if (targ->Strlen() == 1) {
      show("You need to be more specific.\n", agg);
      return;
   }//if

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

      if (!is_canned)
         agg.MANA -= spell_mana;

      int zon;
      int end;
      int begin;
      
      if (agg.getImmLevel() > 5) {
         // Search the entire game!!
         end = Cur_Max_Room_Num;
         begin = 1;
      }
      else {
         zon = room_list[agg.getCurRoomNum()].getZoneNum();
         end = ZoneCollection::instance().elementAt(zon).getEndRoomNum();
         begin = ZoneCollection::instance().elementAt(zon).getBeginRoomNum();
      }

      SCell<critter*> ccll;
      critter* ptr;
      SCell<object*> ocll;
      for (int i = begin; i<= end; i++) {
         // TODO:  Optimize this..move it into room class so we don't have
         // to make coppies of the list! --BEN
         SafeList<critter*> tmp_crits(room_list[i].getCrits());
         tmp_crits.head(ccll);
         while ((ptr = ccll.next())) {
            do_locate_object(*ptr, targ, agg, i, 0, lvl);
         }//while
         room_list[i].getInv().head(ocll);
         while ((ptr = ccll.next())) {
            do_locate_object(*ptr, targ, agg, i, 0, lvl);
         }//while
      }//for
   }//if it worked
   else { //not canned AND lost concentration
      show("You have a hard time locating your thoughts!\n", agg);
      emote("obviously forgot part of the spell!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
         agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 2; 
}//do_cast_locate
 

void cast_locate(const String* victim, critter& pc) {
   int spell_num = LOCATE_SKILL_NUM;


                       /* check out pc */
   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
      return;
   }//if

                 /* all checks have been passed, lets do it */

   do_cast_locate(victim, pc, FALSE, 0);
}//do_cast_locate



void do_cast_wizard_eye(object& vict, critter& pc, int is_canned,
			      int lvl) {
   String buf(100);
   int spell_num = WIZARD_EYE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!pc.pc)
     return;

   if (!is_canned)
     lvl = pc.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(pc, spell_num))) {
     if (!is_canned)
       pc.MANA -= spell_mana;

     if (!vict.obj_proc) {
       vict.obj_proc = new obj_spec_data;
       vict.OBJ_FLAGS.turn_on(63);
     }//if
     if (vict.obj_proc->w_eye_owner) {
       show("That object is already being used for a wizard's eye!\n", pc);
       return;
     }//if
     else if (pc.pc->w_eye_obj) {
       show("You are already watching something!\n", pc);
       return;
     }//if
     else {
       vict.obj_proc->w_eye_owner = &pc;
       pc.pc->w_eye_obj = &vict;
     }//
   }//if it worked
   else { //not canned AND lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, pc);
     emote("obviously forgot part of the spell!", pc, ROOM, TRUE);
     if (!is_canned)
       pc.MANA -= spell_mana / 2;
   }//else lost concentration
   pc.PAUSE += 1; 
}//do_cast_wizard_eye
 

void cast_wizard_eye(int i_th, const String* victim, critter& pc) {
   object* vict = NULL;
   int spell_num = WIZARD_EYE_SKILL_NUM;


                       /* check out pc */

   if (!(vict = have_obj_named(pc.inv, i_th, victim, pc.SEE_BIT, 
			       ROOM))) {
     vict = ROOM.haveObjNamed(i_th, victim, pc.SEE_BIT);
   }//if

   if (!vict) {
      show("On which item??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
      return;
   }//if

                 /* all checks have been passed, lets do it */

   do_cast_wizard_eye(*vict, pc, FALSE, 0);
}//do_cast_wizard_eye



void do_cast_recharge(object& vict, critter& agg, int is_canned,
			      int lvl) {
   String buf(100);
   int spell_num = RECHARGE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

     if (!is_canned)
       agg.MANA -= spell_mana;

     vict.CHARGES++;

     Sprintf(buf, "You pour all your energy into %S.\n", 
	     long_name_of_obj(vict, agg.SEE_BIT));
     show(buf, agg);

     Sprintf(buf, "pours all %s energy into %S.\n", get_his_her(agg),
	     long_name_of_obj(vict, agg.SEE_BIT));
   }//if it worked
   else { //not canned AND lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 2; 
}//do_cast_recharge
 

void cast_recharge(int i_th, const String* victim, critter& pc) {
   object* vict = NULL;
   int spell_num = RECHARGE_SKILL_NUM;

   if (!(vict = pc.haveObjNamed(i_th, victim))) {
      show("Recharge what??\n", pc);
      return;
   }//if

   if (!vict->isModified()) {
     vict = obj_to_sobj(*vict, &(pc), TRUE, i_th, victim, 
                        pc);
   }//if


   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   if (!vict->OBJ_FLAGS.get(51)) {
     show("That is not a type of wand!\n", pc);
     return;
   }//if

   if (vict->RECHARGE <= 0) {
     show("That can't be recharged any more times.\n", pc);
     return;
   }//if

   if (vict->CHARGES != 0) {
     show("It already contains as much power as you could place in it!\n",
	  pc);
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_recharge(*vict, pc, FALSE, 0);
}//do_cast_recharge



void do_cast_identify(object& vict, critter& agg, int is_canned,
			      int lvl) {
   String buf(100);
   int spell_num = IDENTIFY_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

      if (!is_canned)
         agg.MANA -= spell_mana;

      // Do lore with extra info.
      do_lore(vict, agg, TRUE);
      
   }//if it worked
   else { //not canned AND lost concentration
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot part of the spell!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
         agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_identify
 

void cast_identify(int i_th, const String* victim, critter& pc) {
   object* vict = NULL;
   int spell_num = IDENTIFY_SKILL_NUM;

   if (!(vict = have_obj_named(pc.inv, i_th, victim, pc.SEE_BIT, 
			       ROOM))) {
      vict = ROOM.haveObjNamed(i_th, victim, pc.SEE_BIT);
   }//if

   if (!vict) {
      show("Identify what??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_identify(*vict, pc, FALSE, 0);
}//do_cast_identify


void do_cast_bind_wound(critter& vict, critter& agg, int is_canned,
			      int lvl) {
   String buf(100);
   int spell_num = BIND_WOUND_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   SpellDuration* ptr;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) {
     ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;

     if (ptr) {
       show("Ok.\n", agg);
       ptr->duration += lvl/2;
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl + 5));
        vict.HP_REGEN += BIND_WOUND_EFFECT;

        if (&vict == &agg) {
           show("You wrap your wounds in magical bandages.\n", vict);
           Sprintf(buf, "wraps %s wounds in magical bandages.", 
                   get_his_her(vict));
           emote(buf, vict, room_list[vict.getCurRoomNum()], TRUE);
        }//if
        else {
           Sprintf(buf, "You wrap %S's wounds in magical bandages.\n", 
                   name_of_crit(vict, agg.SEE_BIT));
           show(buf, agg);
           Sprintf(buf, "%S wraps your wounds in magical bandages.\n", 
                   name_of_crit(agg, vict.SEE_BIT));
           buf.Cap();
           show(buf, vict);
           Sprintf(buf, "wraps %S's wounds with magical bandages.",
                   name_of_crit(vict, ~0));
           emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
        }//else
     }//else
   }//if it worked
   else { //not canned AND lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_bind_wound
 

void cast_bind_wound(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = BIND_WOUND_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Who do you wish to bandage??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSNB", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_bind_wound(*vict, pc, FALSE, 0);
}//cast_bind_wound



void do_cast_shadows_blessing(critter& vict, critter& agg, int is_canned,
			      int lvl) {
   String buf(100);
   int spell_num = SHADOWS_BLESSING_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   SpellDuration* ptr;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) {
     ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;

     if (ptr) {
       show("Ok.\n", agg);
       ptr->duration += lvl/2;
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl + 5));
       vict.AC += SHADOWS_BLESSING_EFFECT;

       if (&vict == &agg) {
	 show("You cloak yourself in a vail of shadows!\n", vict);
	 Sprintf(buf, "cloaks %s in a vail of shadows!", 
		 get_himself_herself(vict));
	 emote(buf, vict, room_list[vict.getCurRoomNum()], TRUE);
       }//if
       else {
	 Sprintf(buf, "You cloak %S in a vail of shadows.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S cloaks you in a vail of shadows!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "cloaks %S in a vail of shadows!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if it worked
   else { //not canned AND lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_shadows_blessing
 

void cast_shadows_blessing(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = SHADOWS_BLESSING_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Who do you wish to cover in shadows??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_shadows_blessing(*vict, pc, FALSE, 0);
}//do_cast_shadows_blessing


void do_cast_cure_blind(critter& vict, critter& agg, 
			   int is_canned, int lvl) {
   String buf(100);
   int spell_num = CURE_BLINDNESS_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

     int pnum;
     SpellDuration* ptr = vict.isAffectedBy((pnum = BLINDNESS_SKILL_NUM));
     
     if (!is_canned)
       agg.MANA -= spell_mana;

     if (!ptr) {
       show("Ok.\n", agg);
     }//if
     else {
       rem_effects_crit(pnum, vict, FALSE); //no longer affected by blindness
       vict.getAffectedBy().loseData(ptr);

       if (&vict == &agg) {
	 show("You can see again!\n", agg);
	 Sprintf(buf, "cures %s blindness.", 
		 get_himself_herself(agg));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       }//if
       else {
	 Sprintf(buf, "You cure %S's blindness.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S cures your blindness.\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "cures %S's blindness!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     Sprintf(buf, "seems to have lost %s concentration!", get_his_her(agg));
     emote(buf, agg, 
	   room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_cure_blind
 

void cast_cure_blind(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = CURE_BLINDNESS_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to make see again??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_cure_blind(*vict, pc, FALSE, 0);
}//do_cast_cure_blind


void do_cast_remove_poison(critter& vict, critter& agg, 
			   int is_canned, int lvl) {
   String buf(100);
   int spell_num = REMOVE_POISON_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) {

     int pnum;
     SpellDuration* ptr = vict.isAffectedBy((pnum = POISON_SKILL_NUM));
     
     if (!is_canned)
       agg.MANA -= spell_mana;

     if (!ptr) {
       show("Ok.\n", agg);
     }//if
     else {
       rem_effects_crit(pnum, vict, FALSE); //no longer affected by poison
       vict.getAffectedBy().loseData(ptr);

       if (&vict == &agg) {
	 show("You cure yourself!\n", agg);
	 Sprintf(buf, "cures %s of a poisoning.", 
		 get_himself_herself(agg));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       }//if
       else {
	 Sprintf(buf, "You remove poison from %S's body.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S cures you of a poisoning.\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "cures %S of a poisoning!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     Sprintf(buf, "seems to have lost %s concentration!", get_his_her(agg));
     emote(buf, agg, 
	   room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_remove_poison
 

void cast_remove_poison(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = REMOVE_POISON_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to rid of poison??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_remove_poison(*vict, pc, FALSE, 0);
}//do_cast_remove_poison



void do_cast_sleep(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short did_hit = TRUE;
   int spell_num = SLEEP_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if ((is_canned && (did_hit = 
		      did_spell_hit(vict, CRONIC, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, CRONIC, vict)))) {

     SpellDuration* ptr = vict.isAffectedBy(spell_num);
     
     if (!is_canned)
       agg.MANA -= spell_mana;

     if (ptr) {
        show("They are already asleep!\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl/6 + 1));
       vict.CRIT_FLAGS.turn_on(15); //is perm_sleeped
       vict.setPosn(POS_SLEEP);

       if (&vict == &agg) {
	 show("You put yourself to sleep!\n", agg);
	 Sprintf(buf, "puts %s to sleep for a while!", 
		 get_himself_herself(agg));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
       }//if
       else {
	 Sprintf(buf, "You put %S to sleep!\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S forces sleep to fall over your eyes...\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "forces %S to sleep!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else if (!did_hit) {
     Sprintf(buf, "You fail to put %S to sleep!\n", 
	     name_of_crit(vict, agg.SEE_BIT));
     show(buf, agg);
     Sprintf(buf, "%S fails to force you to sleep.\n", 
	     name_of_crit(agg, vict.SEE_BIT));
     buf.Cap();
     show(buf, vict);
     Sprintf(buf, "fails to force %S to go to sleep!",
	     name_of_crit(vict, ~0));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
   }//if missed
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     Sprintf(buf, "seems to have lost %s concentration!", get_his_her(agg));
     emote(buf, agg, 
	   room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_sleep
 

void cast_sleep(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = SLEEP_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to put to sleep??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "BKMSVN", spell_num, pc)) {
      return;
   }//if

   do_cast_sleep(*vict, pc, FALSE, 0);
}//cast_sleep


void do_cast_strength(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   int spell_num = STRENGTH_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 

     SpellDuration* ptr = vict.isAffectedBy(spell_num);
     
     if (!is_canned)
       agg.MANA -= spell_mana;

     if (ptr) {
       ptr->duration += lvl / 2;
       show("Ok.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl));
       vict.STR += STRENGTH_EFFECT;

       if (&vict == &agg) {
	 show("You feel stronger!\n", agg);
       }//if
       else {
	 Sprintf(buf, "You make %S's stronger!\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S makes you stronger!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "gives %S strength!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("feels a little weak in the head!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_strength
 

void cast_strength(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = STRENGTH_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to strengthen??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_strength(*vict, pc, FALSE, 0);
}//do_cast_strength


void do_cast_remove_curse(critter& vict, critter& agg, int is_canned,
			  int lvl) {
   String buf(100);
   int spell_num = REMOVE_CURSE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   SpellDuration* ptr = vict.isAffectedBy(CURSE_SKILL_NUM);
   if (!ptr) {
     Sprintf(buf, "%S isn't cursed.\n", name_of_crit(vict, agg.SEE_BIT));
     show(buf, agg);
     return;
   }//if

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 

     if (!is_canned)
       agg.MANA -= spell_mana;


     rem_effects_crit(CURSE_SKILL_NUM, vict, TRUE);
     vict.getAffectedBy().loseData(ptr); //might be redundant

     if (&vict == &agg) {
       show("You feel a curse lifted from your body!\n", agg);
     }//if
     else {
       Sprintf(buf, "You lift a curse from %S's soul!\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, "%S removes a curse on your soul!\n", 
	       name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "removes a curse from %S!",
	       name_of_crit(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       if (d(1,10) == 1) {
	 show("OOPS, the curse has transferred to YOU!!\n", agg);
	 do_cast_curse(agg, agg, FALSE, 0);
       }//if
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     Sprintf(buf, "lost %s concentration!", get_his_her(agg));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_remove_curse (mob)


void do_cast_remove_curse(object& vict, critter& agg, int is_canned,
			  int lvl) {
   String buf(100);
   int spell_num = REMOVE_CURSE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   SpellDuration* ptr = vict.isAffectedBy(CURSE_SKILL_NUM);
   if (!ptr) {
     Sprintf(buf, "%S isn't cursed.\n", name_of_obj(vict, agg.SEE_BIT));
     show(buf, agg);
     return;
   }//if

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 

     if (!is_canned)
       agg.MANA -= spell_mana;

     rem_effects_obj(CURSE_SKILL_NUM, vict);
     vict.getAffectedBy().loseData(ptr); //might be redundant

     Sprintf(buf, "You lift a curse from %S!\n", 
	     long_name_of_obj(vict, agg.SEE_BIT));
     show(buf, agg);
     Sprintf(buf, "removes a curse from %S!",
	     long_name_of_obj(vict, ~0));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     Sprintf(buf, "lost %s concentration!", get_his_her(agg));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_remove_curse (obj)
 

void cast_remove_curse(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = REMOVE_CURSE_SKILL_NUM;
   object* obj = NULL;

   if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
     obj = pc.haveObjNamed(i_th, victim);
     if (obj) {
       if (!obj->isModified()) {
	 obj = obj_to_sobj(*obj, &(pc), TRUE, i_th, victim, pc);
       }//if
     }//if
     else {
       obj = ROOM.haveObjNamed(i_th, victim, pc);
       if (obj) {
	 if (!obj->isModified()) {
            obj = obj_to_sobj(*obj, &(ROOM), TRUE, i_th, victim, pc);
	 }//if
       }//if
       else {
          show("Remove curse from what??\n", pc);
          return;
       }//else
     }//else
     /* if here then have a valid object */
     do_cast_remove_curse(*obj, pc, FALSE, 0);
   }//if
   else {
     do_cast_remove_curse(*vict, pc, FALSE, 0);
   }//else
}//cast_remove_curse


void cast_curse(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = CURSE_SKILL_NUM;
   object* obj = NULL;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
     obj = pc.haveObjNamed(i_th, victim);
     if (obj) {
       if (!obj->isModified()) {
	 obj = obj_to_sobj(*obj, &(pc), TRUE, i_th, victim, pc);
       }//if
     }//if
     else {
       obj = ROOM.haveObjNamed(i_th, victim, pc);
       if (obj) {
	 if (!obj->isModified()) {
	   obj = obj_to_sobj(*obj, &(ROOM), TRUE, i_th, victim, pc);
	 }//if
       }//if
       else {
	 show("Curse what??\n", pc);
	 return;
       }//else
     }//else
     /* if here then have a valid object */
     if (!ok_to_do_action(NULL, "KMSN", spell_num, pc)) {
       return;
     }//if
     do_cast_curse(*obj, pc, FALSE, 0);
   }//if
   else {
     if (!ok_to_do_action(vict, "KMSNV", spell_num, pc)) {
       return;
     }//if
     do_cast_curse(*vict, pc, FALSE, 0);
   }//else
}//cast_curse


void do_cast_curse(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short did_hit = TRUE;
   int spell_num = CURSE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if ((is_canned && (did_hit = 
		      did_spell_hit(vict, CRONIC, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, CRONIC, vict)))) {

     SpellDuration* ptr = vict.isAffectedBy(spell_num);
     
     if (!is_canned)
       agg.MANA -= spell_mana;

     if (ptr) {
       ptr->duration += lvl / 3;
       show("Ok.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl/2 + 5));
       vict.STR += CURSE_STR_EFFECT;
       vict.MA_REGEN += CURSE_MA_REGEN_EFFECT;

       if (&vict == &agg) {
	 show("You feel cursed!\n", agg);
       }//if
       else {
	 Sprintf(buf, "You lay a curse on %S!\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S lays a curse on you!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "lays a curse on %S!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else if (!did_hit) {
     Sprintf(buf, "Your curse does not stick to %S!\n", 
	     name_of_crit(vict, agg.SEE_BIT));
     show(buf, agg);
     Sprintf(buf, "%S tries and fails to lay a curse on you!\n", 
	     name_of_crit(agg, vict.SEE_BIT));
     buf.Cap();
     show(buf, vict);
     Sprintf(buf, "fails to lay a curse on %S!",
	     name_of_crit(vict, ~0));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
   }//if didn't hit
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     Sprintf(buf, "loses %s concentration.", get_his_her(agg));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_curse (mob)
 

void do_cast_curse(object& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   int spell_num = CURSE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 

     SpellDuration* ptr = vict.isAffectedBy(spell_num);
     
     if (!is_canned)
       agg.MANA -= spell_mana;

     if (ptr) {
       ptr->duration += lvl / 3;
       show("Ok.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl/2 + 5));
       vict.OBJ_FLAGS.turn_on(5); //no_drop

       Sprintf(buf, "You lay a curse on %S!\n", 
	       long_name_of_obj(vict, agg.SEE_BIT));
       show(buf, agg);

       Sprintf(buf, "lays a curse on %S!",
	       long_name_of_obj(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     Sprintf(buf, "loses %s concentration.", get_his_her(agg));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_curse (obj)


void do_cast_blindness(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short did_hit = TRUE;
   int spell_num = BLINDNESS_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if ((is_canned && (did_hit = 
		      did_spell_hit(vict, CRONIC, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, CRONIC, vict)))) {

     SpellDuration* ptr = vict.isAffectedBy(spell_num);
     
     if (!is_canned)
       agg.MANA -= spell_mana;

     if (ptr) {
       ptr->duration += lvl / 3;
       show("Ok.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl/2));
       vict.SEE_BIT &= ~(1024); //turn off the flag

       if (&vict == &agg) {
	 show("You have been blinded!\n", agg);
       }//if
       else {
	 Sprintf(buf, "You blind %S!\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S blinds you!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "blinds %S!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else if (!did_hit) {
     Sprintf(buf, "You fail to blind %S!\n", 
	     name_of_crit(vict, agg.SEE_BIT));
     show(buf, agg);
     Sprintf(buf, "%S fails to blind you!\n", 
	     name_of_crit(agg, vict.SEE_BIT));
     buf.Cap();
     show(buf, vict);
     Sprintf(buf, "fails to blind %S!",
	     name_of_crit(vict, ~0));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
   }//if missed
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("feels a little weak in the head!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_blindness
 

void cast_blindness(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = BLINDNESS_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to blind??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSVN", spell_num, pc)) {
     return;
   }//if

   do_cast_blindness(*vict, pc, FALSE, 0);
}//cast_blindness


void do_cast_weaken(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short did_hit = TRUE;
   int spell_num = WEAKEN_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if ((is_canned && (did_hit = 
		      did_spell_hit(vict, CRONIC, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, CRONIC, vict)))) {

     SpellDuration* ptr = vict.isAffectedBy(spell_num);
     
     if (!is_canned)
       agg.MANA -= spell_mana;

     if (ptr) {
       ptr->duration += lvl / 3;
       show("Ok.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl/2));
       vict.STR += WEAKEN_EFFECT;

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
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1; 
}//do_cast_weaken
 

void cast_weaken(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = WEAKEN_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to weaken??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSVN", spell_num, pc)) {
     return;
   }//if

   do_cast_weaken(*vict, pc, FALSE, 0);
}//do_cast_weaken



void do_cast_fly(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   int spell_num = FLY_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     SpellDuration* ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (ptr) {
       ptr->duration += lvl/3;
       show("Ok.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl/2 + 5));
       vict.CRIT_FLAGS.turn_on(3);

       if (&vict == &agg) {
	 show("Your feet leave the ground and only the wind slows you down!", 
	      agg);
       }//if
       else {
	 Sprintf(buf, "You cause %S to start flying.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S causes you to start flying!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "causes %S to start flying!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1;   // increment pause_count
}//do_cast_fly
 

void cast_fly(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = FLY_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to make fly??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_fly(*vict, pc, FALSE, 0);
}//cast_fly


void do_cast_haste(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   int spell_num = HASTE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     SpellDuration* ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (d(1,15) == 15) { //ouch, lost some CON!!
       show(
     "As you cast the spell you feel it tear something from your very soul!\n",
     agg);
       agg.CON--;
     }//if

     if (ptr) {
       ptr->duration += lvl/4 + 2;
       show("Ok.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl/4 + 4));
       vict.ATTACKS++;

       if (&vict == &agg) {
	 show("Your reflexes are taken to a whole new level!\n", 
	      agg);
	 emote("moves with a quickness quite unlike before!\n", agg,
	       room_list[agg.getCurRoomNum()], TRUE);
       }//if
       else {
	 Sprintf(buf, "You hasten %S's movement.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S hastens your movements!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "hastens %S's movements!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1;   // increment pause_count
}//do_cast_haste
 

void cast_haste(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = HASTE_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to hasten??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_haste(*vict, pc, FALSE, 0);
}//do_cast_haste



void do_cast_dispel_magic(critter& vict, critter& agg, int is_canned,
			   int lvl) {
   String buf(100);
   int spell_num = DISPEL_MAGIC_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
   int do_join_in_battle = FALSE;

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     do_join_in_battle = TRUE;

     int mana_count = 0;
     Cell<SpellDuration*> cll(vict.getAffectedBy());
     SpellDuration* ptr;

     while ((ptr = cll.next())) {
       mana_count += get_mana_cost(ptr->spell);
       rem_effects_crit(ptr->spell, vict, TRUE);
     }//while
     vict.getAffectedBy().clearAndDestroy();
     
     if ((agg.pc) && (agg.MANA < mana_count * 2) && d(1,4) == 4) {
       show(
     "You try to absorb too much power and part of your memory is erased!\n",
     agg);
       int tmp;
       if (agg.SKILLS_KNOWN.Min(tmp))
	 agg.SKILLS_KNOWN.Delete(tmp);
     }//if

     if (!is_canned) {
       agg.MANA -= mana_count;
       if (agg.MANA < 0)
	 agg.MANA = 0;
     }//if

     if (&vict == &agg) {
       show("You are purged of all your magic!\n", 
	    agg);
       Sprintf(buf, "has been stripped of all %s magic!", get_his_her(vict));
       emote(buf, vict, room_list[vict.getCurRoomNum()], TRUE);
     }//if
     else {
       Sprintf(buf, "You remove %S's magical protection.\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, "%S removes your magical protection!\n", 
	       name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "removes %S's magical protection!",
	       name_of_crit(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);       
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration

   if (do_join_in_battle) {
     if (!vict.isFighting(agg)) {
       join_in_battle(vict, agg);
     }//if
   }//if
   agg.incrementPause(1);
}//do_cast_dispel_magic
 

void cast_dispel_magic(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = DISPEL_MAGIC_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to free of their magic??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KSNV", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_dispel_magic(*vict, pc, FALSE, 0);
}//do_cast_dispel_magic


void do_cast_fireproof(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   int spell_num = FIREPROOF_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     SpellDuration* ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (ptr) {
       ptr->duration += lvl/2 +5;
       show("Ok.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl + 5));
       vict.HEAT_RESIS += FIRE_PROOF_EFFECT;

       if (&vict == &agg) {
	 show("You feel almost immune from heat!\n", 
	      agg);
       }//if
       else {
	 Sprintf(buf, "You lend protection from fire to %S.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S protects you from excessive heat!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "lends %S some protection from fire!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1;   // increment pause_count
}//do_cast_fireproof
 

void cast_fireproof(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = FIREPROOF_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to make fireproof??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_fireproof(*vict, pc, FALSE, 0);
}//do_cast_fireproof



void do_cast_magic_shield(critter& vict, critter& agg, int is_canned,
			  int lvl) {
   String buf(100);    
   int spell_num = MAGIC_SHIELD_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     SpellDuration* ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (ptr) {
       ptr->duration += lvl/2;
       show("Ok.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl));

       if (&vict == &agg) {
	 show("A magical shield forms around you!\n", 
	      agg);
       }//if
       else {
	 Sprintf(buf, "You envelop %S in a magical shield.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S envelops you in a magical shield!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "envelops %S in a magical shield!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1;   // increment pause_count
}//do_cast_magic_shield
 

void cast_magic_shield(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = MAGIC_SHIELD_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to shield??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_magic_shield(*vict, pc, FALSE, 0);
}//cast_magic_shield


void do_cast_sanctuary(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
    
   int spell_num = SANCTUARY_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     SpellDuration* ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (ptr) {
       ptr->duration += lvl/2 +5;
       show("Ok.\n", agg);
     }//if
     else if (vict.DAM_REC_MOD <= 70) {
       show("You can protect them no better!\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl + 5));
       vict.DAM_REC_MOD += SANCTUARY_EFFECT_DRM;

       if (&vict == &agg) {
	 show("You are protected by a glowing white light!\n", 
	      agg);
       }//if
       else {
	 Sprintf(buf, "You surround %S with a protective white light.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S surrounds you with a protective white light!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "surrounds %S with a protective white light!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1;   // increment pause_count
}//do_cast_sanctuary
 

void cast_sanctuary(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = SANCTUARY_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Who??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_sanctuary(*vict, pc, FALSE, 0);
}//cast_sanctuary


void do_cast_prismatic_globe(critter& vict, critter& agg, int is_canned,
			     int lvl) {
   String buf(100);    
   int spell_num = PRISMATIC_GLOBE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     SpellDuration* ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (ptr) {
       ptr->duration += lvl/2;
       show("You strengthen the globe.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl + 5));
       vict.AC += PRISMATIC_GLOBE_EFFECT_AC;
       vict.MOV += PRISMATIC_GLOBE_EFFECT_MOV;

       if (&vict == &agg) {
	 show("An almost impermeable globe surrounds you!\n", 
	      agg);
       }//if
       else {
	 Sprintf(buf, "You surround %S with a prismatic globe.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S surrounds you with a prismatic globe!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "surrounds %S with a prismatic globe!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      emote("obviously forgot part of the spell!", agg, 
            room_list[agg.getCurRoomNum()], TRUE);
      if (!is_canned)
         agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1;   // increment pause_count
}//do_cast_prismatic_globe
 

void cast_prismatic_globe(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = PRISMATIC_GLOBE_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Who??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_prismatic_globe(*vict, pc, FALSE, 0);
}//do_cast_prismatic_globe



void do_cast_stone_skin(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);    
   int spell_num = STONE_SKIN_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     SpellDuration* ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (ptr) {
       ptr->duration += lvl/2 +5;
       show("Ok.\n", agg);
     }//if
     else if (vict.DAM_REC_MOD <= 40) {
       show("You can protect them no better!\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl + 5));
       vict.DAM_REC_MOD += STONE_SKIN_EFFECT_DRM;
       vict.DEX += STONE_SKIN_EFFECT_DEX;

       if (&vict == &agg) {
	 show("Your skin turns as hard as stone!\n", 
	      agg);
       }//if
       else {
	 Sprintf(buf, "You turn %S's skin as hard as stone.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S turns you skin as hard as stone!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "turns %S's skin as hard as stone!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1;   // increment pause_count
}//do_cast_stone_skin
 

void cast_stone_skin(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = STONE_SKIN_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Who??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_stone_skin(*vict, pc, FALSE, 0);
}//cast_stone_skin


void do_cast_armor(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   int spell_num = ARMOR_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     SpellDuration* ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (ptr) {
       ptr->duration += lvl/2 +5;
       show("Ok.\n", agg);
     }//if
     else if ((ptr = vict.isAffectedBy(DIVINE_PROTECTION_SKILL_NUM))) {
       show("You are already protected by divine protection.\n",
            agg); //can't be affected by both at once
     }//if divine_protection already
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl + 5));

       vict.AC += ARMOR_EFFECT;

       if (&vict == &agg) {
          show("You feel better protected!\n", 
               agg);
          Sprintf(buf, "protects %s.", get_himself_herself(agg));
          emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, NULL);
       }//if
       else {
	 Sprintf(buf, "You lend protection to %S.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S protects you!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "lends %S some divine protection!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1;   // increment pause_count
}//do_cast_armor
 

void cast_armor(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = ARMOR_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to armor??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_armor(*vict, pc, FALSE, 0);
}//do_cast_armor



void do_cast_absorb_blows(critter& vict, critter& agg, int is_canned,
			  int lvl) {
   String buf(100);    
   int spell_num = ABSORB_BLOWS_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     SpellDuration* ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (ptr) {
       ptr->duration += lvl/2;
       show("Ok.\n", agg);
     }//if
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl));

       if (&vict == &agg) {
	 show(
"You weave an interdimensional lattice of magical streams around yourself!\n", 
	      agg);
       }//if
       else {
	 Sprintf(buf, 
		 "You wrap %S in an interdimensional lattice of power.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, 
		 "%S wraps you in an interdimensional lattice of power!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "wraps %S in an interdimensional lattice of power!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1;   // increment pause_count
}//do_cast_absorb_blows
 

void cast_absorb_blows(critter& pc) {
   critter* vict = NULL;
   int spell_num = ABSORB_BLOWS_SKILL_NUM;

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_absorb_blows(pc, pc, FALSE, 0);
}//do_cast_absorb_blows



void do_cast_divine_protection(critter& vict, critter& agg, int is_canned,
			       int lvl) {
   String buf(100);
   int spell_num = DIVINE_PROTECTION_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);

   int lost_con = FALSE;

   if (!is_canned)
     lvl = agg.LEVEL;

   if (is_canned || !(lost_con = lost_concentration(agg, spell_num))) { 
     SpellDuration* ptr = vict.isAffectedBy(spell_num);

     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (ptr) {
       ptr->duration += lvl/2 +5;
       show("Ok.\n", agg);
     }//if
     else if ((ptr = vict.isAffectedBy(ARMOR_SKILL_NUM))) {
       show("Ok.\n", agg); //can't be affected by both at once
     }//if armored already
     else {
        vict.addAffectedBy(new SpellDuration(spell_num, lvl + 5));
       vict.AC += DIVINE_PROTECTION_EFFECT;

       if (&vict == &agg) {
	 show("You feel much better protected!\n", 
	      agg);
       }//if
       else {
	 Sprintf(buf, "You lend divine protection to %S.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, "%S protects you with divine forces!\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "lends %S some divine protection!",
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else
   }//if worked
   else { //not canned && LOST concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote("obviously forgot part of the spell!", agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else lost concentration
   agg.PAUSE += 1;   // increment pause_count
}//do_cast_divine_protection
 

void cast_divine_protection(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = DIVINE_PROTECTION_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc);

   if (!vict) {
      show("Whom do you wish to protect??\n", pc);
      return;
   }//if

   if (!ok_to_do_action(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_divine_protection(*vict, pc, FALSE, 0);
}//do_cast_divine_protection



void do_locate_object(object &obj, const String* targ, critter& pc,
		      int rm_num, int sanity, int lvl) {
   if ((sanity > 10) || 
       !detect(pc.SEE_BIT, 
               room_list[rm_num].getVisBit() | obj.OBJ_VIS_BIT)) {
      return;
  }//if

   if (!obj.getInv().isEmpty()) {
      SCell<object*> cll(obj.inv);
      object* ptr;
      while ((ptr = cll.next())) {
         do_locate_object(*ptr, targ, pc, rm_num, sanity + 1, lvl);
      }//while
  }//if

   /* Must be magical (affect stats) before you can locate it. */

   if (obj.isMagic() || pc.isImmort()) {
      if ((obj.getLevel() <= lvl) || (pc.isImmort())) {
         String buf(100);
         if (obj.isNamed(*targ)) {
            if (pc.isImmort()) {
               Sprintf(buf, "ROOM: %i  [%i]%S:  %P40%S.\n", rm_num,
                       obj.getIdNum(), long_name_of_obj(obj, ~0),
                       &(room_list[rm_num].short_desc));
            }
            else {
               Sprintf(buf, "%S:  %P40%S.\n", long_name_of_obj(obj, ~0),
                       &(room_list[rm_num].short_desc));
            }

            buf.Cap();
            show(buf, pc);
         }//if it's named thusly.
      }//if
   }//if
}//do locate object (object)


void do_locate_object(critter &crit, const String* targ, critter& pc,
		      int rm_num, int sanity, int lvl) {
   if ((sanity > 10) || 
       !detect(pc.SEE_BIT, 
               room_list[rm_num].getVisBit() | crit.VIS_BIT)) {
      return;
   }//if
   
   if (!crit.getInv().isEmpty()) {
      SCell<object*> cll(crit.inv);
      object* ptr;
      while ((ptr = cll.next())) {
         do_locate_object(*ptr, targ, pc, rm_num, sanity + 1, lvl);
      }//while
   }//if
   
   for (int i = 1; i<MAX_EQ; i++) {
      if (crit.EQ[i]) {
         do_locate_object(*(crit.EQ[i]), targ, pc, rm_num, sanity + 1, lvl);
      }//if
   }//for
}//do locate object (critter)
