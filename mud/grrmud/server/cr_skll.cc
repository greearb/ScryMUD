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

//*********************** cr_skll.cc ********************//

#include "misc.h"
#include "misc2.h"
#include "classes.h"
#include "spells.h"
#include "battle.h"
#include "skills.h"
#include "commands.h"
#include "batl_prc.h"
#include "cr_skll.h"
#include <PtrArray.h>
#include "load_wld.h"



void skin(int i_th, const String* vict, critter& pc) {
   int skill_num = SKIN_SKILL_NUM;
   object* obj;

   if (!ok_to_cast_spell(NULL, "SPF", skill_num, pc)) {
     return;
   }//if     

   int in_rm = FALSE;
   obj = have_obj_named(pc.inv, i_th, vict, pc.SEE_BIT, ROOM);
   if (!obj) {
      in_rm = TRUE;
      obj = have_obj_named(ROOM.inv, i_th, vict, pc.SEE_BIT, ROOM);
   }//if
   if (!obj) {
      show("You don't see that here.\n", pc); 
      return;
   }//if

   if (!obj->IN_LIST) {
      if (in_rm) 
	 obj = obj_to_sobj(*obj, &(ROOM.inv), TRUE, i_th, vict,
			   pc.SEE_BIT, ROOM);
      else
	 obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, vict,
			   pc.SEE_BIT, ROOM);
   }//if

   do_skin(*obj, pc); 
}//skin


void do_skin(object& obj, critter& pc) {
   String buf(100);

   if ((obj.ob->obj_proc && obj.ob->obj_proc->skin_ptr &&
	obj.ob->obj_proc->skin_ptr->OBJ_NUM == PC_SKIN_OBJECT) || 
	skill_did_hit(pc, SKIN_SKILL_NUM, pc)) {
      if (!obj.ob->obj_proc ||
	  !obj.ob->obj_proc->obj_spec_data_flags.get(2) || 
	  !obj.ob->obj_proc->skin_ptr ||
	  !obj.ob->obj_proc->skin_ptr->OBJ_FLAGS.get(10)) { //not in use?
	 show("This thing doesn't have a skin!.\n", pc);
	 return;
      }//if

      pc.gainInv(obj.ob->obj_proc->skin_ptr);
      obj.ob->obj_proc->obj_spec_data_flags.turn_off(2);
      obj.ob->obj_proc->skin_ptr = NULL;

      Sprintf(buf, "skins %S.", long_name_of_obj(obj, ~0));
      emote(buf, pc, ROOM, TRUE);
      Sprintf(buf, "You skin %S.\n", long_name_of_obj(obj, ~0));      
      show(buf, pc);
   }//if
   else {
      if (!obj.ob->obj_proc ||
		!obj.ob->obj_proc->obj_spec_data_flags.get(2)) {
	 show("It doesn't have a skin.\n", pc);
	 return;
      }//if

      obj.ob->obj_proc->obj_spec_data_flags.turn_off(2);
      obj.ob->obj_proc->skin_ptr->decrementCurInGame();

      if (obj.ob->obj_proc->skin_ptr->IN_LIST) {
	delete obj.ob->obj_proc->skin_ptr;
      }//if
      obj.ob->obj_proc->skin_ptr = NULL;

      Sprintf(buf, "tries to skin %S but only makes a bloody mess!",
		long_name_of_obj(obj, ~0));
      emote(buf, pc, ROOM, TRUE);
      Sprintf(buf, "You try to skin %S but only make a bloody mess!\n",
		long_name_of_obj(obj, ~0));      
      show(buf, pc);
   }//else

}//do_skin



void butcher(int i_th, const String* vict, critter& pc) {
   int skill_num = BUTCHER_SKILL_NUM;
   object* obj;

   if (pc.isMob()) {
      return; 
   }//if

   if (get_percent_lrnd(skill_num, pc) == -1) {
      show("You wouldn't even know where to start!\n", pc); 
      return; 
   }//if

   if (pc.POS > POS_REST) {
      show("You are too relaxed.\n", pc); 
      return; 
   }//if

   if (pc.pc && pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc); 
      return; 
   }//if

   if (pc.CRIT_FLAGS.get(14)) { //if paralyzed return; 
      show("You can't move a muscle.\n", pc);
      return;
   }//if

   int in_rm = FALSE;
   obj = have_obj_named(pc.inv, i_th, vict, pc.SEE_BIT, ROOM);
   if (!obj) {
      in_rm = TRUE;
      obj = have_obj_named(ROOM.inv, i_th, vict, pc.SEE_BIT, ROOM);
   }//if
   if (!obj) {
      show("You don't see that here.\n", pc); 
      return;
   }//if

   if (!obj->OBJ_FLAGS.get(75)) {
     show("This doesn't look the least bit edible!!\n", pc);
     return;
   }//if

   if (!obj->IN_LIST) {
      if (in_rm) 
	 obj = obj_to_sobj(*obj, &(ROOM.inv), TRUE, i_th, vict,
			   pc.SEE_BIT, ROOM);
      else
	 obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, vict,
			   pc.SEE_BIT, ROOM);
   }//if

   do_butcher(*obj, pc); 
}//butcher


void do_butcher(object& obj, critter& pc) {
   String buf(100);

   if (!obj.OBJ_FLAGS.get(75)) {
     show("This doesn't look the least bit edible!!\n", pc);
     return;
   }//if

   if (skill_did_hit(pc, BUTCHER_SKILL_NUM, pc)) {
      if (obj_list[MEAT_OBJ_NUMBER].OBJ_FLAGS.get(10)) {
         pc.gainInv(&(obj_list[MEAT_OBJ_NUMBER]));
         obj.OBJ_FLAGS.turn_off(75); //no longer butcherable

         recursive_init_loads(obj_list[MEAT_OBJ_NUMBER], 0);

         Sprintf(buf, "butchers %S.", long_name_of_obj(obj, ~0));
         emote(buf, pc, ROOM, TRUE);
         Sprintf(buf, "You butcher %S.\n", long_name_of_obj(obj, ~0));      
         show(buf, pc);
      }//if
      else {
	 show("BUG, my gawd, you've found a BUG!!\n", pc);
	 return;
      }//if
   }//if skill worked
   else {
     Sprintf(buf, "You try to butcher %S, but ruin it all!!\n",
	     long_name_of_obj(obj, pc.SEE_BIT));
     show(buf, pc);

     Sprintf(buf, "tries to butcher %S, but ruins it all!!",
	     long_name_of_obj(obj, pc.SEE_BIT));
     emote(buf, pc, ROOM, TRUE);

     obj.OBJ_FLAGS.turn_off(75);
   }//else
}//do_skin







