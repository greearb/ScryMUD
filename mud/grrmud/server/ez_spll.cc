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

///********************** ez_spll.cc **********************///

#include "ez_spll.h"
#include "commands.h"
#include "command2.h"
#include <stdio.h>
#include "misc.h"
#include "misc2.h"
#include "battle.h"
#include "classes.h"
#include "spells.h"
#include "skills.h"
#include "spec_prc.h"
#include <PtrArray.h>


void cast_detect_alignment(int i_th, const String* victim,
                           critter& pc) {
   critter* vict = NULL;
   int spell_num = DETECT_ALIGNMENT_SKILL_NUM;

   if (!(vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT))) {
     show("You don't see that person.\n", pc);
     return;
   }//if

   if (!ok_to_cast_spell(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_detect_alignment(*vict, pc, FALSE, 0);  //does no error checking

}//cast_detect_alignment


void do_cast_detect_alignment(critter& vict, critter& agg, int is_canned, 
			  int lvl) {
   String buf(100);
   int spell_num = DETECT_ALIGNMENT_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   /*  Check for: lost concentration, did_hit    */
   /*  Do: damage to vict, mana from agg, messages to all involved, */
   /*      increment pause_count, */


   if (!is_canned)
      lvl = agg.LEVEL;

   short did_hit = TRUE;
   short lost_con = TRUE;

   if ((is_canned && (did_hit = 
		      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

     if (!is_canned)
       agg.MANA -= spell_mana;

     if (vict.ALIGN < -350) {
       Sprintf(buf, "This looks like one bad %s!\n", get_dude_chic(vict));
     }//if
     else if (vict.ALIGN < 350) {
       Sprintf(buf, "This %s seems like an ok person.\n", get_dude_chic(vict));
     }//if
     else {
       Sprintf(buf, "This %s is a real holy roller!\n", get_dude_chic(vict));
     }//if

     show(buf, agg);
   }//if !lost_con
   else if (!did_hit) {
     Sprintf(buf, "You fail to ascertain %S's personality.\n",
	     name_of_crit(vict, agg.SEE_BIT));
     show(buf, agg);
   }//if missed
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

}//do_cast_detect_alignment


void cast_detect_poison(int i_th, const String* victim, critter& pc) {
   object* vict = NULL;
   int spell_num = DETECT_POISON_SKILL_NUM;

   if (!(vict = have_obj_named(ROOM.inv, i_th, victim, 
				pc.SEE_BIT, ROOM))) {
      vict = have_obj_named(pc.inv, i_th, victim, 
				pc.SEE_BIT, ROOM);
      if (!vict) {
         show("You don't see that person.\n", pc);
         return;
      }//if
   }//if

   if (!ok_to_cast_spell(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_detect_poison(*vict, pc, FALSE, 0);  //does no error checking

}//cast_detect_poison


void do_cast_detect_poison(object& vict, critter& agg, int is_canned, 
			  int lvl) {
   String buf(100);
   int spell_num = DETECT_POISON_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {

     if (!is_canned)
       agg.MANA -= spell_mana;

     if (vict.ob->obj_proc &&
	 vict.ob->obj_proc->obj_spec_data_flags.get(3)) {
       Sprintf(buf, "You get a STRONG reaction from %S!\n",
	       long_name_of_obj(vict, agg.SEE_BIT));
       show(buf, agg);
     }//if
     else {
       Sprintf(buf, "%S seems to be safe from poison.\n",
	       long_name_of_obj(vict, agg.SEE_BIT));
       buf.Cap();
       show(buf, agg);
     }//else
   }//if !lost_con
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

}//do_cast_detect_poison


void cast_detect_invisibility(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = DETECT_INVISIBILITY_SKILL_NUM;

   if (!(vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT))) {
      show("You don't see that person.\n", pc);
      return;
   }//if

   if (vict->isMob()) {
     vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
			victim, pc.SEE_BIT);
   }//if

   if (!ok_to_cast_spell(vict, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_detect_invisibility(*vict, pc, FALSE, 0);  //does no error checking

}//cast_detect_invisibility


void do_cast_detect_invisibility(critter& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = DETECT_INVISIBILITY_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 

   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
       show("Ok.\n", agg);
     }//if
     else {
       if (&vict == &agg) {
	 show("You become aware of formerly invisible things.\n",
	      vict);
	 emote("seems to see things formerly invisible.", agg, 
	       room_list[agg.getCurRoomNum()], TRUE); 
       }//if
       else {
	 Sprintf(buf, "You allow %S to see invisible things.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, 
  	     "%S allows you to see invisible things.\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "allows %S to see invisible things.", 
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
     if (sp)
       sp->bonus_duration += lvl/2;
     else
       Put(new stat_spell_cell(spell_num, lvl/2),
	   vict.affected_by);
     vict.SEE_BIT |= 2;  //can now see invisible
   }//if
}//do_cast_detect_invisibility


void cast_detect_hidden(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = DETECT_HIDDEN_SKILL_NUM;

   if (!(vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT))) {
      show("You don't see that person.\n", pc);
      return;
   }//if

   if (vict->isMob()) {
       vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
                          victim, pc.SEE_BIT);
   }//if

   if (!ok_to_cast_spell(vict, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_detect_hidden(*vict, pc, FALSE, 0);  //does no error checking

}//cast_detect_hidden


void do_cast_detect_hidden(critter& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = DETECT_HIDDEN_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
       show("Ok.\n", agg);
     }//if
     else {
       if (&vict == &agg) {
	 show("You become aware of things not seen before.\n",
	      vict);
	 emote("seems to see things a little more clearly.", agg, 
	       room_list[agg.getCurRoomNum()], TRUE); 
       }//if
       else {
	 Sprintf(buf, "You allow %S to detect hidden things.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, 
  	     "%S allows you to see things not seen by others.\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "allows %S to see things not easily seen.", 
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
     if (sp)
       sp->bonus_duration += lvl/2;
     else
       Put(new stat_spell_cell(spell_num, lvl/2),
	   vict.affected_by);
     vict.SEE_BIT |= 4;  //can now see hidden
   }//if
}//do_cast_detect_hidden



void cast_bless(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = BLESS_SKILL_NUM;

   if (!(vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT))) {
      show("You don't see that person.\n", pc);
      return;
   }//if

   if (vict->isMob()) {
     vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th, victim,
			pc.SEE_BIT);
   }//if

   if (!ok_to_cast_spell(vict, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_bless(*vict, pc, FALSE, 0);  //does no error checking
}//cast_bless


void do_cast_bless(critter& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = BLESS_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
       show("Ok.\n", agg);
     }//if
     else {
       if (&vict == &agg) {
	 show("You feel blessed by a god.\n",
	      vict);
	 emote("seems a little luckier.", agg, 
	       room_list[agg.getCurRoomNum()], TRUE); 
       }//if
       else {
	 Sprintf(buf, "You give your god's blessing to %S.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, 
  	     "%S gives you %s god's blessing.\n",
		 get_his_her(agg),
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "gives %s god's blessing to %S.\n",
		 get_his_her(agg),
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
     if (sp)
       sp->bonus_duration += lvl/3;
     else {
       Put(new stat_spell_cell(spell_num, 5 + lvl/2),
	   vict.affected_by);
       vict.HIT += BLESS_EFFECTS;
     }//else
   }//if
}//do_cast_bless



void cast_pfg(critter& pc) {
   critter* vict = NULL;
   int spell_num = PFG_SKILL_NUM;

   if (!ok_to_cast_spell(vict, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_pfg(pc, pc, FALSE, 0);  //does no error checking
}//cast_pfg


void do_cast_pfg(critter& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = PFG_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (vict.ALIGN > 350) {
     if (&vict == &agg) {
       show("You are too good to be protected by an evil god!\n", agg);
     }//if
     else {
       Sprintf(buf, "%S is too good to be protected by an evil god!\n",
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
     }//else
     return;
   }//if

   if ((sp = is_affected_by(PFE_SKILL_NUM, vict))) {
     if (&vict == &agg) {
       show("You are already protected by a benelovent deity!\n", agg);
     }//if
     else {
       Sprintf(buf, "%S is already protected by a benelovent deity.\n",
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
     }//else
     return;
   }//if

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
        show("Ok.\n", agg);
     }//if
     else {
       if (&vict == &agg) {
	 show("You feel protected from good.\n",
	      vict);
	 emote("is protected from good.", agg, 
	       room_list[agg.getCurRoomNum()], TRUE); 
       }//if
       else {
	 Sprintf(buf, "You protect %S from good.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, 
  	     "%S protects you from good.\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "protects %S from good.", 
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
     if (sp)
       sp->bonus_duration += lvl/2;
     else {
       Put(new stat_spell_cell(spell_num, 10 + lvl/2),
	   vict.affected_by);
       vict.CRIT_FLAGS.turn_on(25); //pfg now
     }//else
   }//if
}//do_cast_pfg



void cast_pfe(critter& pc) {
   critter* vict = NULL;
   int spell_num = PFE_SKILL_NUM;

   if (!ok_to_cast_spell(vict, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_pfe(pc, pc, FALSE, 0);  //does no error checking

}//cast_pfe


void do_cast_pfe(critter& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = PFE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (vict.ALIGN < -350) {
     if (&vict == &agg) {
       show("You are too evil to be protected by a benign god!\n", agg);
     }//if
     else {
       Sprintf(buf, "%S is too evil to be protected by a benign god!\n",
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
     }//else
     return;
   }//if

   if ((sp = is_affected_by(PFG_SKILL_NUM, vict))) {
     if (&vict == &agg) {
       show("You are already protected by a malevolent deity!\n", agg);
     }//if
     else {
       Sprintf(buf, "%S is already protected by a malevolent deity.\n",
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
     }//else
     return;
   }//if

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
       show("Ok.\n", agg);
     }//if
     else {
       if (&vict == &agg) {
	 show("You feel protected from evil.\n",
	      vict);
	 emote("is protected from evil.", agg, 
	       room_list[agg.getCurRoomNum()], TRUE); 
       }//if
       else {
	 Sprintf(buf, "You protect %S from evil.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, 
  	     "%S protects you from evil.\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "protects %S from evil.", 
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
     if (sp)
       sp->bonus_duration += lvl/2;
     else {
       Put(new stat_spell_cell(spell_num, 10 + lvl/2),
	   vict.affected_by);
       vict.CRIT_FLAGS.turn_on(24); //pfe now
     }//else
   }//if
}//do_cast_pfe



void cast_detect_magic(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = DETECT_MAGIC_SKILL_NUM;

   if (!(vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT))) {
      show("You don't see that person.\n", pc);
      return;
   }//if

   if (!vict->pc) {
      show("Ok.\n", pc); //mobs don't need det_magic
   }//if   

   if (!ok_to_cast_spell(vict, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_detect_magic(*vict, pc, FALSE, 0);  //does no error checking

}//cast_detect_magic


void do_cast_detect_magic(critter& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = DETECT_MAGIC_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
       show("Ok.\n", agg);
     }//if
     else {
       if (&vict == &agg) {
	 show("You become aware of subtle flows of magical power.\n",
	      vict);
	 emote("seems to see things not seen before.", agg, 
	       room_list[agg.getCurRoomNum()], TRUE); 
       }//if
       else {
	 Sprintf(buf, "You allow %S to detect magical energies.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, 
  	     "%S allows you to see the subtle flows of magical powers.\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "does something strange to %S's eyes.\n", 
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
      if (vict.pc) {
         if (sp)
   	   sp->bonus_duration += lvl/3;
         else
	   Put(new stat_spell_cell(spell_num, lvl/3),
	       vict.affected_by);
         vict.PC_FLAGS.turn_on(18); //can now det magic
      }//if a pc
   }//if
}//do_cast_detect_magic


void cast_create_water(int i_th, const String* victim, critter& pc) {
   int spell_num = CREATE_WATER_SKILL_NUM;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In cast_create_water, i:  " << i_th << "  vict:  "
             << *victim << endl;
   }

   if (!ok_to_cast_spell(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   object* obj = have_obj_named(pc.inv, i_th, victim, pc.SEE_BIT,
                                ROOM);

   if (!obj) {
      pc.show("You have to place the water in something!\n");
      pc.show("As in:  cast 'create water' jug.\n");
      return;
   }

   if (!obj->IN_LIST) {
     obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, victim, 
		       pc.SEE_BIT, ROOM);
   }//if

   if (!obj->OBJ_FLAGS.get(59) && obj->ob->bag) {
     show("That cannot hold water!\n", pc);
     return;
   }//if

   do_cast_create_water(*obj, pc, FALSE, 0);  //does no error checking

}//cast_create_water


void do_cast_create_water(object& vict, critter& agg, int is_canned, 
			  int lvl) {
   String buf(100);
   short do_effects = FALSE;
   int spell_num = CREATE_WATER_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In do cast_create_water, i:  " << endl;
   }

   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (!vict.ob->bag) {
     mudlog.log(ERR, "ERROR:  non-bag sent to do_cast_create_water.");
     return;
   }//if

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
      object* obj_ptr = Top(vict.ob->inv);
      if (obj_ptr && obj_ptr->OBJ_NUM != 7) {
         show("You need to empty it first!\n", agg);
         return;
      }//if
      else if (vict.BAG_FLAGS.get(2)) {
         show("You must open it first.\n", agg);
         return;
      }//if
      
      do_effects = TRUE;
      
      if (!is_canned)
         agg.MANA -= spell_mana;
      
      Sprintf(buf, "You fill %S to the top!\n", 
              long_name_of_obj(vict, agg.SEE_BIT));
      show(buf, agg);
      Sprintf(buf, "fills %S to the top!",
              long_name_of_obj(vict, ~0));
      emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
   }//if did_hit
   else if (lost_con) {//lost concentration 
      show(LOST_CONCENTRATION_MSG_SELF, agg);
      Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
      buf.Cap();
      room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
      if (!is_canned)
         agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;
   
   if (do_effects) {
      clear_obj_list(vict.ob->inv);
      vict.CHARGES = 0;
      vict.CHARGES = vict.OBJ_MAX_WEIGHT - vict.OBJ_CUR_WEIGHT;
      vict.gainInv(&(obj_list[7]));
   }//if
}//do_cast_create_water



void cast_enchant_weapon(int i_th, const String* victim, critter& pc) {
   int spell_num = ENCHANT_WEAPON_SKILL_NUM;

   if (!ok_to_cast_spell(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   object* obj = have_obj_named(pc.inv, i_th, victim, pc.SEE_BIT,
			    ROOM);
   if (!obj) {
      pc.show("You don't see that weapon.\n");
      return;
   }

   if (!obj->IN_LIST) {
     obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, victim, 
		       pc.SEE_BIT, ROOM);
   }//if

   if (obj->is_magic()) {
     show("That cannot hold any more magic!\n", pc);
     return;
   }//if
   
   if (!obj->OBJ_FLAGS.get(30)) {
     show("That isn't a weapon!\n", pc);
     return;
   }//if

   do_cast_enchant_weapon(*obj, pc, FALSE, 0);  //does no error checking

}//cast_enchant_weapon


void do_cast_enchant_weapon(object& vict, critter& agg, int is_canned, 
			  int lvl) {
   mudlog.log(TRC, "In do_cast_enchant_weapon..\n");

   String buf(100);
   short do_effects = FALSE;
   int spell_num = ENCHANT_WEAPON_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     Sprintf(buf, "You cause %S to glow with a faint blue light.\n", 
	     long_name_of_obj(vict, agg.SEE_BIT));
     show(buf, agg);
     Sprintf(buf, "causes %S to glow with a faint blue light.",
	     long_name_of_obj(vict, ~0));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
     /* first do dice incrementation */
     int i;
     for (i = 0; i<4; i++) {
       if (d(1,2) == 2)
	 vict.OBJ_DAM_DICE_SIDES++;
       else
	 vict.OBJ_DAM_DICE_COUNT++;
     }//for

     /* now increment HIT and DAM affects */
     stat_spell_cell* dm_ptr = has_stat_affect(8, vict);
     stat_spell_cell* ht_ptr = has_stat_affect(7, vict);

     for (i = 0; i<4; i++) {
       if (d(1,2) == 2) {
	 if (!dm_ptr)
	   Put((dm_ptr = new stat_spell_cell(8, 1)), vict.ob->stat_affects);
	 else
	   dm_ptr->bonus_duration++;
       }//if
       else {
	 if (!ht_ptr)
	   Put((ht_ptr = new stat_spell_cell(7, 1)), vict.ob->stat_affects);
	 else
	   ht_ptr->bonus_duration++;
       }//else
     }//for
   }//if
}//do_cast_enchant_weapon



void cast_enchant_armor(int i_th, const String* victim, critter& pc) {
   int spell_num = ENCHANT_ARMOR_SKILL_NUM;

   if (!ok_to_cast_spell(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   object* obj = have_obj_named(pc.inv, i_th, victim, pc.SEE_BIT,
			    ROOM);
   if (!obj) {
      pc.show("You don't see that object around..\n");
      return;
   }

   if (!obj->IN_LIST) {
     obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, victim, 
		       pc.SEE_BIT, ROOM);
   }//if

   if (obj->is_magic()) {
     show("That cannot hold any more magic!\n", pc);
     return;
   }//if
   
   if (obj->OBJ_FLAGS.get(30)) {
     show("That's a weapon!\n", pc);
     return;
   }//if

   do_cast_enchant_armor(*obj, pc, FALSE, 0);  //does no error checking

}//cast_enchant_armor


void do_cast_enchant_armor(object& vict, critter& agg, int is_canned, 
			  int lvl) {
   String buf(100);
   short do_effects = FALSE;
   int spell_num = ENCHANT_ARMOR_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     Sprintf(buf, "You cause %S to glow with a faint white light.\n", 
	     long_name_of_obj(vict, agg.SEE_BIT));
     show(buf, agg);
     Sprintf(buf, "causes %S to glow with a faint white light.",
	     long_name_of_obj(vict, ~0));
     emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
     stat_spell_cell* ac_ptr = has_stat_affect(9, vict);
     stat_spell_cell* spll_ptr = has_stat_affect(32, vict);

     /* first do AC decrementation */
     if (!ac_ptr)
       Put(new stat_spell_cell(9, -(lvl/3)), vict.ob->stat_affects);
     else
       ac_ptr->bonus_duration -= (lvl/3);

     /* spell resistance decrementation (less is good) */ 
     if (!spll_ptr)
       Put(new stat_spell_cell(32, -(lvl/3)), vict.ob->stat_affects);
     else
       spll_ptr->bonus_duration -= (lvl/3);

   }//if
}//do_cast_enchant_armor


void cast_fire_blade(int i_th, const String* victim, critter& pc) {
   int spell_num = FIRE_BLADE_SKILL_NUM;

   if (!ok_to_cast_spell(NULL, "KMSNB", spell_num, pc)) {
     return;
   }//if

   object* obj = have_obj_named(pc.inv, i_th, victim, pc.SEE_BIT,
			    ROOM);
   if (!obj) {
     show("You don't see that here.\n", pc);
     return;
   }//if
   if (!obj->IN_LIST) {
     obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, victim, 
		       pc.SEE_BIT, ROOM);
   }//if

   if (!(obj->OBJ_FLAGS.get(43) || obj->OBJ_FLAGS.get(41))) {
     show("This can't be enchanted thus.\n", pc);
     return;
   }//if

   do_cast_fire_blade(*obj, pc, FALSE, 0);  //does no error checking
}//cast_fire_blade


void do_cast_fire_blade(object& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = FIRE_BLADE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 

   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
       show("Ok\n", agg);
     }//if
     else {
       Sprintf(buf, "You make %S glow with a magical heat!\n", 
	       long_name_of_obj(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, "makes %S glow with a magical heat!\n", 
	       long_name_of_obj(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
     if ((sp = is_affected_by(FROST_BLADE_SKILL_NUM, vict))) {
       vict.ob->affected_by.loseData(sp);
       delete sp;
     }//if

     sp = is_affected_by(spell_num, vict);
     if (sp)
       sp->bonus_duration += lvl/3;
     else {
       Put(new stat_spell_cell(spell_num, lvl/2), vict.ob->affected_by);
     }//else
   }//if
}//do_cast_fire_blade


void cast_frost_blade(int i_th, const String* victim, critter& pc) {
   int spell_num = FROST_BLADE_SKILL_NUM;

   if (!ok_to_cast_spell(NULL, "KMSNB", spell_num, pc)) {
     return;
   }//if

   object* obj = have_obj_named(pc.inv, i_th, victim, pc.SEE_BIT,
			    ROOM);
   if (!obj) {
     show("You don't see that here.\n", pc);
     return;
   }//if
   if (!obj->IN_LIST) {
     obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, victim, 
		       pc.SEE_BIT, ROOM);
   }//if

   if (!(obj->OBJ_FLAGS.get(43) || obj->OBJ_FLAGS.get(41))) {
     show("This can't be enchanted thus.\n", pc);
     return;
   }//if

   do_cast_frost_blade(*obj, pc, FALSE, 0);  //does no error checking
}//cast_frost_blade


void do_cast_frost_blade(object& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = FROST_BLADE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 

   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
       show("Ok\n", agg);
     }//if
     else {
       Sprintf(buf, "You make %S very cold!\n", 
	       long_name_of_obj(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, "makes %S very cold!\n", 
	       long_name_of_obj(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
     if ((sp = is_affected_by(FIRE_BLADE_SKILL_NUM, vict))) {
       vict.ob->affected_by.loseData(sp);
       delete sp;
       sp = is_affected_by(FROST_BLADE_SKILL_NUM, vict);
     }//if

     if (sp)
       sp->bonus_duration += lvl/3;
     else {
       Put(new stat_spell_cell(spell_num, lvl/2), vict.ob->affected_by);
     }//else
   }//if
}//do_cast_frost_blade


void cast_rune_edge(int i_th, const String* victim, critter& pc) {
   int spell_num = RUNE_EDGE_SKILL_NUM;

   if (!ok_to_cast_spell(NULL, "KMSNB", spell_num, pc)) {
     return;
   }//if

   object* obj = have_obj_named(pc.inv, i_th, victim, pc.SEE_BIT,
			    ROOM);
   if (!obj) {
     show("You don't see that here.\n", pc);
     return;
   }//if
   if (!obj->IN_LIST) {
     obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, victim, 
		       pc.SEE_BIT, ROOM);
   }//if

   if (!(obj->OBJ_FLAGS.get(43) || obj->OBJ_FLAGS.get(41))) {
     show("This can't be sharpened.\n", pc);
     return;
   }//if

   do_cast_rune_edge(*obj, pc, FALSE, 0);  //does no error checking

}//cast_rune_edge


void do_cast_rune_edge(object& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = RUNE_EDGE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   /*  Check for: lost concentration, did_hit    */
   /*  Do: damage to vict, mana from agg, messages to all involved, */
   /*      increment pause_count, */


   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
       show("Ok\n", agg);
     }//if
     else {
       Sprintf(buf, "You magically sharpen %S to a keen edge!\n", 
	       long_name_of_obj(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, "magically sharpens %S to a keen edge!\n", 
	       long_name_of_obj(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
      if (sp)
	sp->bonus_duration += lvl/3;
      else {
	Put(new stat_spell_cell(spell_num, lvl/2), vict.ob->affected_by);
	vict.OBJ_DAM_DICE_SIDES += RUNE_EDGE_EFFECTS;
      }//else
   }//if
}//do_cast_rune_edge



void cast_invisibility(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = INVISIBILITY_SKILL_NUM;

   if (!ok_to_cast_spell(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   if (!(vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT))) {
     object* obj = have_obj_named(ROOM.inv, i_th, victim, pc.SEE_BIT,
				  ROOM);
     if (!obj) {
       obj = have_obj_named(pc.inv, i_th, victim, pc.SEE_BIT,
			    ROOM);
       if (!obj) {
	 show("You don't see that here.\n", pc);
	 return;
       }//if
       if (!obj->IN_LIST) {
	 obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, victim, 
			   pc.SEE_BIT, ROOM);
       }//if
     }//if
     if (!obj->IN_LIST) {
       obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, victim, 
			 pc.SEE_BIT, ROOM);
     }//if
     /* if here, then have a valid obj */
     
     do_cast_invisibility(*obj, pc, FALSE, 0);
   }//if no vict
   else { //got a valid vict
     
     if (vict->isMob()) {
       vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
                          victim, pc.SEE_BIT);
     }//if
   
     do_cast_invisibility(*vict, pc, FALSE, 0);  //does no error checking
   }//else

}//cast_invisibility


void do_cast_invisibility(critter& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = INVISIBILITY_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
       show("Ok\n", agg);
     }//if
     else {
       if (&vict == &agg) {
	 show("You fade from sight!\n", vict);
	 emote("fades from sight.", agg, 
	       room_list[agg.getCurRoomNum()], TRUE); 
       }//if
       else {
	 Sprintf(buf, "You cause %S to fade from sight.\n", 
		 name_of_crit(vict, agg.SEE_BIT));
	 show(buf, agg);
	 Sprintf(buf, 
		 "%S causes you to fade from sight.\n", 
		 name_of_crit(agg, vict.SEE_BIT));
	 buf.Cap();
	 show(buf, vict);
	 Sprintf(buf, "causes %S to dissappear from sight.\n", 
		 name_of_crit(vict, ~0));
	 emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE, &vict);
       }//else
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
      if (sp)
	sp->bonus_duration += lvl/3;
      else
	Put(new stat_spell_cell(spell_num, lvl/3), vict.affected_by);

      vict.VIS_BIT |= 2;
   }//if

}//do_cast_invisibility (mob)



void do_cast_invisibility(object& vict, critter& agg, int is_canned, 
			  int lvl) {
   stat_spell_cell* sp = NULL;
   String buf(100);
   short do_effects = FALSE;
   int spell_num = INVISIBILITY_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;

   if (is_canned || (!(lost_con = lost_concentration(agg, spell_num)))) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if ((sp = is_affected_by(spell_num, vict))) {
       show("Ok\n", agg);
     }//if
     else {
       Sprintf(buf, "You cause %S to fade from sight.\n", 
	       long_name_of_obj(vict, agg.SEE_BIT));
       show(buf, agg);
     }//else, not already affected
   }//if did_hit
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

   if (do_effects) {
      if (sp)
	sp->bonus_duration += lvl/3;
      else
	Put(new stat_spell_cell(spell_num, lvl/3), vict.ob->affected_by);

      vict.OBJ_VIS_BIT |= 2;
   }//if

}//do_cast_invisibility (object)


void cast_mirror_image(critter& pc) {
   int spell_num = MIRROR_IMAGE_SKILL_NUM;

   if (!ok_to_cast_spell(NULL, "KMSNB", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_mirror_image(pc, FALSE, 0);  //does no error checking
}//cast_mirror_image


void do_cast_mirror_image(critter& pc, int is_canned, int lvl) {
   String buf(100);
   short do_effects = FALSE;
   int spell_num = MIRROR_IMAGE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 

   if (!is_canned)
      lvl = pc.LEVEL;

   if (is_canned || !lost_concentration(pc, spell_num)) { 
     do_effects = TRUE;

     if (!is_canned)
       pc.MANA -= spell_mana;

     int num_mirs = 2 + d(1, lvl/6);

     Sprintf(buf, "You create %i mirror images of yourself.\n", num_mirs);
     show(buf, pc);
     Sprintf(buf, "creates %i mirrors of %s.", num_mirs,
	     get_himself_herself(pc));
     emote(buf, pc, ROOM, TRUE); 
     pc.mirrors = max(num_mirs, pc.mirrors);
   }//if did_hit
   else { //lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, pc);
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(pc, ~0));
     buf.Cap();
     ROOM.showAllCept(buf, &pc); 
     pc.MANA -= spell_mana / 2;
   }//else

   pc.PAUSE++;

}//do_cast_mirror_image


void cast_cure_serious(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = CURE_SERIOUS_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);

   if (!vict) {
      show("Who do you wish to heal??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
                          victim, pc.SEE_BIT);
   }//if
   
   if (!ok_to_cast_spell(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_cure_serious(*vict, pc, FALSE, 0);  //does no error checking
}//cast_cure_serious


void do_cast_cure_serious(critter& vict, critter& agg, int is_canned,
			  int lvl) {
   String buf(100);
   short do_effects = FALSE;
   int spell_num = CURE_SERIOUS_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 

   if (!is_canned)
      lvl = agg.LEVEL;

   if (is_canned || !lost_concentration(agg, spell_num)) { 
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if (&vict == &agg) {
       show("You cure yourself.\n", vict);
       Sprintf(buf, "cures %s.", get_himself_herself(agg));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE); 
     }//if
     else {
       Sprintf(buf, "You cure %S.\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
	       "%S cures you.\n", name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "cures %S.\n", 
	       name_of_crit(vict, ~0));
       emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE);
     }//else
   }//if did_hit
   else { //lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
     agg.MANA -= spell_mana / 2;
   }//else

   agg.PAUSE++;

   if (do_effects) {
      vict.HP += (10 + d(1,15));
      if (vict.HP > vict.HP_MAX)
	 vict.HP = vict.HP_MAX;
   }//if
}//do_cast_cure_serious


void cast_cure_critical(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = CURE_CRITICAL_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);

   if (!vict) {
      show("Who do you wish to cure??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
                          victim, pc.SEE_BIT);
   }//if
   

   if (!ok_to_cast_spell(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_cure_critical(*vict, pc, FALSE, 0);  //does no error checking
}//cast_cure_critical


void do_cast_cure_critical(critter& vict, critter& agg, int is_canned,
			   int lvl) {
   String buf(100);
   short do_effects = FALSE;
   int spell_num = CURE_CRITICAL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 

   if (!is_canned)
      lvl = agg.LEVEL;

   if (is_canned || !lost_concentration(agg, spell_num)) { 
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if (&vict == &agg) {
       show("You cure yourself!\n", vict);
       Sprintf(buf, "cures %s.", get_himself_herself(agg));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE); 
     }//if
     else {
       Sprintf(buf, "You cure %S.\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
	       "%S cures you.\n", name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "cures %S.\n", 
	       name_of_crit(vict, ~0));
       emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE);
     }//else
   }//if did_hit
   else { //lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);

     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     agg.MANA -= spell_mana / 2;
   }//else

   agg.PAUSE++;

   if (do_effects) {
      vict.HP += (20 + d(1, lvl));
      if (vict.HP > vict.HP_MAX)
	 vict.HP = vict.HP_MAX;
   }//if
}//do_cast_cure_critical


void cast_heal(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = HEAL_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);

   if (!vict) {
      show("Who do you wish to heal??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
                          victim, pc.SEE_BIT);
   }//if
   

   if (!ok_to_cast_spell(vict, "KMSN", spell_num, pc)) {
     return;
   }//if
                 /* all checks have been passed, lets do it */

   do_cast_heal(*vict, pc, FALSE, 0);  //does no error checking
}//cast_heal


void do_cast_heal(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_effects = FALSE;
   int spell_num = HEAL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 

   if (!is_canned)
      lvl = agg.LEVEL;

   if (is_canned || !lost_concentration(agg, spell_num)) { 
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if (&vict == &agg) {
       show("You heal yourself!\n", vict);
       Sprintf(buf, "heals %s.", get_himself_herself(agg));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE); 
     }//if
     else {
       Sprintf(buf, "You heal %S.\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
	       "%S heals you.\n", name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "heals %S.\n", 
	       name_of_crit(vict, ~0));
       emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE);
     }//else
   }//if did_hit
   else { //lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);

     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     agg.MANA -= spell_mana / 2;
   }//else

   agg.PAUSE++;

   if (do_effects) {
      vict.HP += (d(4, lvl) + 150);
      if (vict.HP > vict.HP_MAX)
	 vict.HP = vict.HP_MAX;
   }//if
}//do_cast_heal



void cast_restore(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = RESTORE_SKILL_NUM;

   vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);

   if (!vict) {
      show("Who do you wish to restore??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
                          victim, pc.SEE_BIT);
   }//if

   if (!ok_to_cast_spell(vict, "KMSN", spell_num, pc)) {
     return;
   }//if   
                 /* all checks have been passed, lets do it */

   do_cast_restore(*vict, pc, FALSE, 0);  //does no error checking
}//cast_restore


void do_cast_restore(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_effects = FALSE;
   int spell_num = RESTORE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   /*  Check for: lost concentration, did_hit    */
   /*  Do: damage to vict, mana from agg, messages to all involved, */
   /*      increment pause_count, */


   if (!is_canned)
      lvl = agg.LEVEL;

   if (is_canned || !lost_concentration(agg, spell_num)) {
     do_effects = TRUE;

     if (!is_canned)
       agg.MANA -= spell_mana;

     if (&vict == &agg) {
       show("You restore yourself!\n", vict);
       Sprintf(buf, "restores %s.", get_himself_herself(agg));
	    emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE); 
     }//if
     else {
       Sprintf(buf, "You restore %S.\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
	       "%S restores you.\n", name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "restores %S.\n", 
	       name_of_crit(vict, ~0));
       emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE);
     }//else
   }//if did_hit
   else { //lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);

     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     agg.MANA -= spell_mana / 2;
   }//else

   agg.PAUSE++;

   if (do_effects) {
      vict.HP += (d(7, 100) + 2 * lvl);
      if (vict.HP > vict.HP_MAX)
	 vict.HP = vict.HP_MAX;
   }//if
}//do_cast_restore



void cast_entangle(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = ENTANGLE_SKILL_NUM;

   if (victim->Strlen() == 0) 
      vict = Top(pc.IS_FIGHTING);
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);

   if (!vict) {
      show("Who do you wish to entangle??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
                          victim, pc.SEE_BIT);
   }//if

   if (!ok_to_cast_spell(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if
   
   if (!(vict = check_for_diversions(*vict, "GSM", pc)))
     return;
                 /* all checks have been passed, lets do it */

   do_cast_entangle(*vict, pc, FALSE, 0);  //does no error checking
}//cast_entangle



void cast_web(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = WEB_SKILL_NUM;

   if (victim->Strlen() == 0) 
      vict = Top(pc.IS_FIGHTING);
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);

   if (!vict) {
      show("Who do you wish to web??\n", pc);
      return;
   }//if

   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
                          victim, pc.SEE_BIT);
   }//if

   if (!ok_to_cast_spell(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if

   if (!(vict = check_for_diversions(*vict, "GSM", pc)))
     return;
                 /* all checks have been passed, lets do it */

   do_cast_web(*vict, pc, FALSE, 0);  //does no error checking
}//cast_web


void do_cast_web(critter& vict, critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_effects = FALSE;
   short do_join_in_battle = TRUE;
   int spell_num = WEB_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short did_hit = TRUE;
   short lost_con = TRUE;

   if ((is_canned && (did_hit = 
		      did_spell_hit(vict, AGILITY, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, AGILITY, vict)))) {

     do_effects = TRUE;
     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (&vict == &agg) {
       show("You cover yourself with a fine web!\n", vict);
       emote("is suddenly covered in a fine, sticky web.", agg, 
	     room_list[agg.getCurRoomNum()], TRUE); 
       do_join_in_battle = FALSE;
     }//if
     else {
       Sprintf(buf, "You cover %S in a sticky web!\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
	       "%S has covered you with a fine web of something!!\n", 
	       name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "covers %S with a fine, sticky web.\n", 
	       name_of_crit(vict, ~0));
       emote(buf, agg, room_list[vict.getCurRoomNum()], TRUE);
     }//else
   }//if did_hit
   else if (!did_hit) { //missed
     if (!is_canned)
       agg.MANA -= spell_mana;

     if (&agg == &vict) {
       show("Your web falls around your feet.\n", agg);
       do_join_in_battle = FALSE;
     }//if
     else {
       Sprintf(buf, "Your web misses %S.\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);

       Sprintf(buf, "%S fails to catch you in %s web!\n", 
                       name_of_crit(agg, vict.SEE_BIT), get_his_her(agg));
       buf.Cap();
       show(buf, vict);
     }//else
   }//else, missed
   else if (lost_con) {//lost concentration 
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     
     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else
   
   agg.PAUSE++;

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
      vict.DEX += WEB_DEX_AUGMENTATION;
      vict.MV_REGEN += WEB_MV_REGEN_AUGMENTATION;
   }//if

   if (do_join_in_battle && !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if
}//do_cast_web


void do_cast_entangle(critter& vict, critter& agg, int is_canned,
		      int lvl) {
   String buf(100);
   short do_effects = FALSE;
   short do_join_in_battle = TRUE;
   int spell_num = ENTANGLE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short did_hit = TRUE;
   short lost_con = FALSE;

   if ((is_canned && (did_hit = 
		      did_spell_hit(vict, AGILITY, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, AGILITY, vict)))) {

     do_effects = TRUE;

     if (&vict == &agg) {
       show("You entangle yourself, great!!\n", vict);
       emote("is suddenly covered in a thick, sticky web.", agg, 
	     room_list[agg.getCurRoomNum()], TRUE); 
       do_join_in_battle = FALSE;
     }//if
     else {
       Sprintf(buf, "You tangle %S in a thick sticky web!\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
	       "%S has covered you with a strong web of something!!\n", 
	       name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "covers %S with a strong, sticky web.\n", 
	       name_of_crit(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
     }//else
   }//if did_hit
   else if (!did_hit) { //missed
     if (&agg == &vict) {
       show("Your web falls around your feet.\n", agg);
       do_join_in_battle = FALSE;
     }//if
     else {
       Sprintf(buf, "Your web misses %S.\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       
       Sprintf(buf, "%S fails to catch you in %s web!\n", 
	       name_of_crit(agg, vict.SEE_BIT), get_his_her(agg));
       buf.Cap();
       show(buf, vict);
     }//else
     agg.PAUSE += 1;   // increment pause_count
   }//else, missed with canned magic   
   else { //lost concentration
     show(LOST_CONCENTRATION_MSG_SELF, agg);

     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     agg.MANA -= spell_mana / 2;
     agg.PAUSE += 1; 
   }//else

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
      vict.DEX += ENTANGLE_DEX_AUGMENTATION;
      vict.MV_REGEN += ENTANGLE_MV_REGEN_AUGMENTATION;
   }//if

   if (do_join_in_battle && !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if
}//do_cast_entangle


void cast_faerie_fire(int i_th, const String* victim, critter& pc) {
   critter* vict = NULL;
   int spell_num = FAERIE_FIRE_SKILL_NUM;

   if (victim->Strlen() == 0) 
      vict = Top(pc.IS_FIGHTING);
   else 
      vict = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);

   if (!vict) {
      show("Who do you wish to cast faerie fire on??\n", pc);
      return;
   }//if

               /* got a victim, now check for mob/smob */
   if (vict->isMob()) {
      vict = mob_to_smob(*vict, pc.getCurRoomNum(), TRUE, i_th,
                          victim, pc.SEE_BIT);
   }//if
                 /* all checks have been passed, lets do it */

   if (!ok_to_cast_spell(vict, "KMSNV", spell_num, pc)) {
     return;
   }//if

   do_cast_faerie_fire(*vict, pc, FALSE, 0);  //does no error checking
}//cast_faerie_fire


void do_cast_faerie_fire(critter& vict, critter& agg, int is_canned,
		      int lvl) {
   String buf(100);
   short do_effects = FALSE;
   short do_join_in_battle = TRUE;
   int spell_num = FAERIE_FIRE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   short lost_con = TRUE;
   short did_hit = TRUE;

   if ((is_canned && (did_hit = 
		      did_spell_hit(vict, NORMAL, agg, lvl, TRUE))) ||
       (!is_canned && !(lost_con = lost_concentration(agg, spell_num)) && 
         (did_hit = did_spell_hit(agg, NORMAL, vict)))) {

     do_effects = TRUE;
     do_join_in_battle = TRUE;
     if (!is_canned)
       agg.MANA -= spell_mana;
     
     if (&vict == &agg) {
       show("You glow with an eerie purplish light!!\n", vict);
       emote("suddenly glows with an eerie purplish light.", agg, 
	     room_list[agg.getCurRoomNum()], TRUE); 
       do_join_in_battle = FALSE;
     }//if
     else {
       Sprintf(buf, "You make %S glow with an eerie purplish light!\n", 
	       name_of_crit(vict, agg.SEE_BIT));
       show(buf, agg);
       Sprintf(buf, 
	       "%S makes you glow with a purplish light!!\n", 
	       name_of_crit(agg, vict.SEE_BIT));
       buf.Cap();
       show(buf, vict);
       Sprintf(buf, "makes %S glow with an eerie purplish light.", 
	       name_of_crit(vict, ~0));
       emote(buf, agg, room_list[agg.getCurRoomNum()], TRUE);
     }//else
   }//if did_hit
   else if (!did_hit) { //missed
     do_join_in_battle = TRUE;
     show("Your spell goes awry and the ground glows for an instant.",
	  agg);
     
     Sprintf(buf, "%S fails to catch you on faerie fire!\n", 
	     name_of_crit(agg, vict.SEE_BIT));
     buf.Cap();
     show(buf, vict);
     if (!is_canned)
       agg.MANA -= spell_mana;
   }//if
   else if (lost_con) {
     show(LOST_CONCENTRATION_MSG_SELF, agg);

     Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
     buf.Cap();
     room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
     agg.MANA -= spell_mana / 2;
   }//else
   
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
      vict.AC += FAERIE_FIRE_AC_AUGMENTATION;
   }//if

   if (do_join_in_battle && !HaveData(&vict, agg.IS_FIGHTING)) {
      join_in_battle(agg, vict);
   }//if
}//do_cast_faerie_fire



void cast_calm(critter& pc) {
   int spell_num = CALM_SKILL_NUM;
                 /* all checks have been passed, lets do it */

   if (!ok_to_cast_spell(NULL, "KMSN", spell_num, pc)) {
     return;
   }//if

   do_cast_calm(pc, FALSE, 0);  //does no error checking
}//cast_calm


void do_cast_calm(critter& agg, int is_canned, int lvl) {
   String buf(100);
   short do_effects = FALSE;
   int spell_num = CALM_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
 
   if (!is_canned)
      lvl = agg.LEVEL;

   if (is_canned) {
     do_effects = TRUE;
     show("You bring peace to the room.\n", agg);
     emote("brings peace to all!", agg, room_list[agg.getCurRoomNum()], TRUE);
   }//if canned
   else {//not canned 
      if (!lost_concentration(agg, spell_num)) { 
	do_effects = TRUE;

	show("You bring peace to the room!\n", agg);
	emote("brings peace to all.\n", agg, room_list[agg.getCurRoomNum()],
	      TRUE); 
	agg.MANA -= spell_mana;
	agg.PAUSE += 1;   // increment pause_count
      }//if ! lost concentration
      else { //lost concentration
	show(LOST_CONCENTRATION_MSG_SELF, agg);

	Sprintf(buf, LOST_CONCENTRATION_MSG_OTHER, name_of_crit(agg, ~0));
	buf.Cap();
	room_list[agg.getCurRoomNum()].showAllCept(buf, &agg);
 
	agg.MANA -= spell_mana / 2;
	agg.PAUSE += 1; 
      }//else
   }//else !canned

   if (do_effects) {
      List<critter*> tmp_lst(room_list[agg.getCurRoomNum()].getCrits());
      Cell<critter*> cll(tmp_lst);
      critter* ptr;

      while ((ptr = cll.next())) {
         if (room_list[agg.getCurRoomNum()].haveCritter(ptr)) {
            ptr->IS_FIGHTING.clear(); //no more battle
            do_was_calmed_procs(*ptr, agg); //maybe they really don't like it!!
         }//if
      }//while
   }//if
}//do_cast_calm









