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



int skin(int i_th, const String* vict, critter& pc) {
   int skill_num = SKIN_SKILL_NUM;
   object* obj;

   if (!ok_to_do_action(NULL, "mBSPFK", skill_num, pc)) {
      return -1;
   }//if     

   int in_rm = FALSE;
   obj = have_obj_named(pc.inv, i_th, vict, pc.SEE_BIT, ROOM);
   if (!obj) {
      in_rm = TRUE;
      obj = ROOM.haveObjNamed(i_th, vict, pc.SEE_BIT);
   }//if
   if (!obj) {
      show("You don't see that here.\n", pc); 
      return -1;
   }//if

   if (!obj->IN_LIST) {
      if (in_rm) 
         obj = obj_to_sobj(*obj, ROOM.getInv(), TRUE, i_th, vict,
                           pc.SEE_BIT, ROOM);
      else
         obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, vict,
                           pc.SEE_BIT, ROOM);
   }//if

   return do_skin(*obj, pc); 
}//skin


int do_skin(object& obj, critter& pc) {
   String buf(100);

   if ((obj.obj_proc && obj.obj_proc->skin_ptr &&
        obj.obj_proc->skin_ptr->OBJ_NUM == config.pcSkinObject) || 
        skill_did_hit(pc, SKIN_SKILL_NUM, pc)) {
      if (!obj.obj_proc ||
          !obj.obj_proc->obj_spec_data_flags.get(2) || 
          !obj.obj_proc->skin_ptr ||
          !obj.obj_proc->skin_ptr->OBJ_FLAGS.get(10)) { //not in use?
         show("This thing doesn't have a skin!.\n", pc);
         return -1;
      }//if

      pc.gainInv(obj.obj_proc->skin_ptr);
      obj.obj_proc->obj_spec_data_flags.turn_off(2);
      obj.obj_proc->skin_ptr = NULL;

      Sprintf(buf, "skins %S.", long_name_of_obj(obj, ~0));
      emote(buf, pc, ROOM, TRUE);
      Sprintf(buf, "You skin %S.\n", long_name_of_obj(obj, ~0));      
      show(buf, pc);
   }//if
   else {
      if (!obj.obj_proc ||
                !obj.obj_proc->obj_spec_data_flags.get(2)) {
         show("It doesn't have a skin.\n", pc);
         return -1;
      }//if

      obj.obj_proc->obj_spec_data_flags.turn_off(2);
      obj.obj_proc->skin_ptr->decrementCurInGame();

      if (obj.obj_proc->skin_ptr->IN_LIST) {
        delete obj.obj_proc->skin_ptr;
      }//if
      obj.obj_proc->skin_ptr = NULL;

      Sprintf(buf, "tries to skin %S but only makes a bloody mess!",
                long_name_of_obj(obj, ~0));
      emote(buf, pc, ROOM, TRUE);
      Sprintf(buf, "You try to skin %S but only make a bloody mess!\n",
                long_name_of_obj(obj, ~0));      
      show(buf, pc);
   }//else
   return 0;
}//do_skin



int butcher(int i_th, const String* vict, critter& pc) {
   int skill_num = BUTCHER_SKILL_NUM;
   object* obj;

   if (!ok_to_do_action(NULL, "mBSPF", skill_num, pc)) {
      return -1;
   }//if     

   int in_rm = FALSE;
   obj = have_obj_named(pc.inv, i_th, vict, pc.SEE_BIT, ROOM);
   if (!obj) {
      in_rm = TRUE;
      obj = ROOM.haveObjNamed(i_th, vict, pc.SEE_BIT);
   }//if
   if (!obj) {
      show("You don't see that here.\n", pc); 
      return -1;
   }//if

   if (!obj->OBJ_FLAGS.get(75)) {
      show("This doesn't look the least bit edible!!\n", pc);
      return -1;
   }//if

   if (!obj->IN_LIST) {
      if (in_rm) 
         obj = obj_to_sobj(*obj, ROOM.getInv(), TRUE, i_th, vict,
                           pc.SEE_BIT, ROOM);
      else
         obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, vict,
                           pc.SEE_BIT, ROOM);
   }//if

   return do_butcher(*obj, pc); 
}//butcher


int do_butcher(object& obj, critter& pc) {
   String buf(100);

   if (!obj.OBJ_FLAGS.get(75)) {
      show("This doesn't look the least bit edible!!\n", pc);
      return -1;
   }//if

   if (skill_did_hit(pc, BUTCHER_SKILL_NUM, pc)) {
      if (obj_list[config.meatObject].OBJ_FLAGS.get(10)) {
         pc.gainInv(&(obj_list[config.meatObject]));
         obj.OBJ_FLAGS.turn_off(75); //no longer butcherable

         recursive_init_loads(obj_list[config.meatObject], 0);

         Sprintf(buf, "butchers %S.", long_name_of_obj(obj, ~0));
         emote(buf, pc, ROOM, TRUE);
         Sprintf(buf, "You butcher %S.\n", long_name_of_obj(obj, ~0));      
         show(buf, pc);
         return 0;
      }//if
      else {
         show("BUG, my gawd, you've found a BUG!!\n", pc);
         return -1;
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
     return -1;
   }//else   
}//do_butcher

// this is intentionally using the same skill as skin...
int behead(int i_th, const String* vict, critter& pc) {
   int skill_num = SKIN_SKILL_NUM;
   object* obj;

   if (!ok_to_do_action(NULL, "mBSPFK", skill_num, pc)) {
      return -1;
   }//if     

   int in_rm = FALSE;
   obj = have_obj_named(pc.inv, i_th, vict, pc.SEE_BIT, ROOM);
   if (!obj) {
      in_rm = TRUE;
      obj = ROOM.haveObjNamed(i_th, vict, pc.SEE_BIT);
   }//if
   if (!obj) {
      show("You don't see that here.\n", pc); 
      return -1;
   }//if

   if (!obj->IN_LIST) {
      if (in_rm) 
         obj = obj_to_sobj(*obj, ROOM.getInv(), TRUE, i_th, vict,
                           pc.SEE_BIT, ROOM);
      else
         obj = obj_to_sobj(*obj, &(pc.inv), TRUE, i_th, vict,
                           pc.SEE_BIT, ROOM);
   }//if

   return do_behead(*obj, pc); 
}//behead


int do_behead(object& obj, critter& pc) {
   String buf(100);

   if ((obj.obj_proc && obj.obj_proc->head_ptr)) {

      if (!obj.obj_proc ||
          !obj.obj_proc->obj_spec_data_flags.get(11) || 
          !obj.obj_proc->head_ptr ||
          !obj.obj_proc->head_ptr->OBJ_FLAGS.get(10)) { //not in use?
         show("This thing doesn't have a head!.\n", pc);
         return -1;
      }//if

      pc.gainInv(obj.obj_proc->head_ptr);
      obj.obj_proc->obj_spec_data_flags.turn_off(11);
      obj.obj_proc->head_ptr = NULL;

      Sprintf(buf, "beheads %S.", long_name_of_obj(obj, ~0));
      emote(buf, pc, ROOM, TRUE);
      Sprintf(buf, "You behead %S.\n", long_name_of_obj(obj, ~0));      
      show(buf, pc);
   }//if
   else {
         pc.show("It doesn't have a head.\n");
         return -1;
   }

   return 0;
}//do_behead
