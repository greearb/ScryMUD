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

///**********************  command3.cc ****************************///
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include "command4.h"
#include "misc.h"
#include "misc2.h"
#include <stdio.h>
#include "classes.h"
#include "battle.h"
#include "spec_prc.h"
#include "spells.h"
#include "skills.h"
#include "command5.h"
#include <PtrArray.h>
#include "load_wld.h"


void use(int i_th, const String* target, critter& pc) { //for wands
   critter* targ;
   object* wand = NULL;
   String buf(100);
   short is_holding;

   if (!ok_to_cast_spell(NULL, "SNFP", 0, pc)) {
     return;
   }//if

   if (ROOM.isNoWand()) {
      show("This room is not conducive to the use of your wand.\n", pc);
      return;
   }//if

                /* check if holding wand, has charges ect */

   if (pc.EQ[10]) { //if holding ANY object
      if (pc.EQ[10]->OBJ_FLAGS.get(51) &&
	  pc.EQ[10]->ob->obj_proc) { //if a wand
         wand = pc.EQ[10];
         is_holding = TRUE;
      }//if
   }//if

   if (!wand) { //check wielding position
      if (pc.EQ[9]) {
         if (pc.EQ[9]->OBJ_FLAGS.get(51) && 
	     pc.EQ[9]->ob->obj_proc) { //if a wand
            wand = pc.EQ[9];
            is_holding = FALSE;
         }//if
      }//if
   }//if
         
   if (!wand) {
      show("You must be holding a wand in your hands in order to use it.\n",
	   pc);
      return;
   }//if

   if (wand->CHARGES <= 0) {
      Sprintf(buf, "Your %S seems quite devoid of energies.\n",
              single_obj_name(*wand, pc.SEE_BIT));
      show(buf, pc);
      return;
   }//if
      
   if (!wand->IN_LIST) {
      if (is_holding) 
         pc.EQ[10] = wand = obj_to_sobj(*(pc.EQ[10]), &(pc.inv));
      else
         pc.EQ[9] = wand = obj_to_sobj(*(pc.EQ[9]), &(pc.inv));
   }//if

                /* wand is ready, check for target */

   Sprintf(buf, "You clench %S and try to remember how to trigger it.\n",
           long_name_of_obj(*wand, pc.SEE_BIT));
   show(buf, pc);

   short found_proc = TRUE;
   short do_dec = FALSE;
   Cell<stat_spell_cell*> cll(wand->ob->obj_proc->casts_these_spells);
   stat_spell_cell* ptr;

   while ((ptr = cll.next())) {
      switch (ptr->stat_spell)
        {
           //First, those requiring a MOB for target
        case 1: case 10: case 11: case 30:
        case 38: case 39: case 136: case 139: case 140:
        case 151: case 152: case 155: case 160:
        case 162: case 163: case 164: case 171:
        case 172: case 174: case 175: case 176:
        case 177: case 178: case 179: case 180: case 189:
        case 191: case 192: case 193: case 194: case 195: case 197:
        case 199: case 201: case 202: case 203: case 204:
        case 205: case 206: case 208: case 217: case 219:
        case 221: case 222: case 224: case 225: case 226:
        case 227: case 228:
        {
           found_proc = TRUE;
           if (target->Strlen() == 0) {
              targ = Top(pc.IS_FIGHTING);
           }//if
           else {
              targ = ROOM.haveCritNamed(i_th, target, pc.SEE_BIT);
           }//else
           
           if (targ) {
              if (targ->isMob()) { //if its a MOB
                 targ = mob_to_smob(*targ, pc.getCurRoomNum(), TRUE, i_th,
                                    target, pc.SEE_BIT);
              }//if
           }//if

           if (!targ) {
              targ = &pc;
           }
           do_wand_scroll_proc(targ, ptr->stat_spell, pc, ptr->bonus_duration);

          break;
        }//case for mob targets

        // now for doors
        case 21: case 181:
        {
           door* dr_ptr;
           if ((dr_ptr = door::findDoor(ROOM.DOORS, i_th, target,
                                        pc.SEE_BIT, ROOM))) {
              if (dr_ptr->isSecret()) {
                 if (!name_is_secret(target, *dr_ptr)) {
                    show("You don't see that exit.\n", pc);
                    return;
                 }//if
              }//if

              do_wand_scroll_proc(targ, ptr->stat_spell, pc, 
                                  ptr->bonus_duration);
           }//if
           else {
              pc.show("You need to specify a door on which to use your scroll.\n");
           }//else

          break;
        }//door case statement
        
        // spells requiring no target
        case 12:
        case 22: case 25: case 145: case 146:
        case 147: case 149: case 150: case 153: case 200:
        case 210: case 211: case 218:
           // spells requiring room targs, always use current room
        case 4: case 6: case 14: case 17: case 18: case 20:
        case 198: case 215: case 220:
        {
           do_wand_scroll_proc(ptr->stat_spell, pc,
                               ptr->bonus_duration);
           break;
        }//case

        case 159:        //special case, gate spell (portal too?)
        {
           targ = have_crit_named(pc_list, i_th, target, pc.SEE_BIT,
                                  ROOM);
           if (!targ) {
              show("That person isn't logged on.\n", pc);
           }//if
           else {
              do_wand_scroll_proc(targ, ptr->stat_spell, pc,
                                  ptr->bonus_duration);
           }
           break;
        }

        //object targets
        case 124: case 156:
        case 165: case 182: case 183: case 184: case 185:
        case 186: case 196: case 213:
        {
           object* ob_ptr;
           ob_ptr = have_obj_named(pc.inv, i_th, target,
                                   pc.SEE_BIT, ROOM);
           if (ob_ptr) {
              if (!ob_ptr->IN_LIST) {
                 ob_ptr = obj_to_sobj(*ob_ptr, &(pc.inv), TRUE,
                                      i_th, target, pc.SEE_BIT, ROOM);
              }
           }
           else {
              ob_ptr = have_obj_named(ROOM.inv, i_th, target,
                                      pc.SEE_BIT, ROOM);
              
              if (ob_ptr && !ob_ptr->IN_LIST) {
                 ob_ptr = obj_to_sobj(*ob_ptr, &(ROOM.inv), TRUE,
                                      i_th, target, pc.SEE_BIT, ROOM);
              }
           }//else
           
           if (ob_ptr) {
              pc.show("Which object shall the scroll be used on?\n");
           }
           else {
              do_wand_scroll_proc(ob_ptr, ptr->stat_spell, pc,
                                  ptr->bonus_duration);
           }

           break;
        }//case

        default:
        { 
           mudlog << "ERROR:  found default case in use (wand), spell_num: "
                  << ptr->stat_spell << "  on object#:  "
                  << wand->getIdNum() << endl;
           pc.show("You call upon forces unavailable at this time!\n");
           found_proc = FALSE;
        }
        }//switch
   }//while

   if (!found_proc) {
      show("Nothing seems to happen.\n", pc);
      Sprintf(buf, "ERROR:  wand# %i has no spells defined.\n", wand->OBJ_NUM);
      mudlog.log(ERR, buf);
   }//if
   if (do_dec) {
      wand->CHARGES--;
   }//if
}//use


void quaf(int i_th, const String* item, critter& pc) { //for wands
   object* potion = NULL;
   String buf(100);
   int posn = 0;

   if (!ok_to_cast_spell(NULL, "F", 0, pc)) {
     return;
   }//if

   if (ROOM.isNoPotion()) {
      show("Open potions are not allowed here!!\n", pc);
      return;
   }//if

   if (item->Strlen() == 0) { //then must be holding
      for (int i = 9; i<11; i++) {
         if (pc.EQ[i]) { //if holding ANY object
            if (pc.EQ[i]->OBJ_FLAGS.get(52)) { //if a potion
               potion = pc.EQ[i];
               posn = i;
               break;
            }//if
         }//if
      }//for
   }//if
   else {
      potion = have_obj_named(pc.inv, i_th, item, pc.SEE_BIT, 
                              ROOM);
   }//if         

   if (!potion) {
      show("Quaf what?\n", pc);
      return;
   }//if

   if (!potion->isPotion()) {
      show("That isn't a potion.\n", pc);
      return;
   }//if

   Sprintf(buf, "You quaf %S.\n",
           long_name_of_obj(*potion, pc.SEE_BIT));
   show(buf, pc);

   short found_proc = TRUE;
   Cell<stat_spell_cell*> cll(potion->CASTS_THESE_SPELLS);
   stat_spell_cell* ptr;

   while ((ptr = cll.next())) {
      switch (ptr->stat_spell)
        {
           //First, those requiring a MOB for target
        case 1: case 10: case 11: case 30:
        case 38: case 39: case 136: case 139: case 140:
        case 151: case 152: case 155: case 160:
        case 162: case 163: case 164: case 171:
        case 172: case 174: case 175: case 176:
        case 177: case 178: case 179: case 180: case 188: case 189:
        case 191: case 192: case 193: case 195: case 197:
        case 199: case 201: case 202: case 203: case 204:
        case 205: case 206: case 208: case 217: case 219:
        case 221: case 222: case 224: case 225: case 226:
        case 227: case 228:
        {
           do_wand_scroll_proc(&pc, ptr->stat_spell, pc, ptr->bonus_duration);
           break;
        }//case for mob targets

        // spells requiring no target
        case 12:
        case 22: case 25: case 145: case 146:
        case 147: case 149: case 150: case 153: case 200:
        case 210: case 211: case 218:
           // spells requiring room targs, always use current room
        case 4: case 6: case 14: case 17: case 18: case 20:
        case 159: case 198: case 215: case 220:
        {
           do_wand_scroll_proc(ptr->stat_spell, pc, ptr->bonus_duration);

           break;
        }//case

        default:
        { 
           mudlog << "ERROR:  found default case in quaf, spell_num: "
                  << ptr->stat_spell << "  on object#:  "
                  << potion->getIdNum() << endl;
           pc.show("You call upon forces unavailable at this time!\n");
           found_proc = FALSE;
        }
        }//switch
   }//while

   if (!found_proc) {
      show("You feel slightly refreshed!\n", pc);
      Sprintf(buf, "ERROR:  item# %i casts no spells (quaf)\n", 
	      potion->OBJ_NUM);
      mudlog.log(ERR, buf);
   }//if

   if (posn) {
      if (pc.EQ[posn]->IN_LIST) { //if a SOBJ
         delete pc.EQ[posn];
      }//if
      pc.EQ[posn] = NULL;
   }//if
   else {
      pc.loseInv(potion);
      if (potion->IN_LIST) {
         delete potion;
         potion = NULL;
      }//if
   }//else
}//quaf


void recite(int i_th, const String* item, int j_th, const String* vict, 
            critter& pc) { //for scrolls
   critter* targ = NULL;
   object* scroll = NULL;
   String buf(100);
   short posn = 0;
   int junk_scroll = FALSE;

   if (!ok_to_cast_spell(NULL, "SNPF", -1, pc)) {
     return;
   }//if

                /* check if holding scroll */

   for (int i = 9; i<11; i++) {
      if (pc.EQ[i]) { //if holding ANY object
         if (pc.EQ[i]->OBJ_FLAGS.get(53) &&
             pc.EQ[i]->ob->obj_proc) { //if a scroll
            if (obj_is_named(*(pc.EQ[i]), *item)) {
               if (detect(pc.SEE_BIT, 
                          pc.EQ[i]->OBJ_VIS_BIT & ROOM.getVisBit())) {
                  scroll = pc.EQ[i];
                  posn = i;
                  break;
               }//if
            }//if
         }//if
      }//if
   }//while

   if (!scroll) {
      scroll = have_obj_named(pc.inv, i_th, item, pc.SEE_BIT, ROOM);
   }//else

   if (!scroll) {
      show("Recite what??\n", pc);
      return;
   }//if

   Sprintf(buf, "You unroll %S and begin to read...\n",
           long_name_of_obj(*scroll, pc.SEE_BIT));
   show(buf, pc);

   short found_proc = TRUE;
   Cell<stat_spell_cell*> cll(scroll->CASTS_THESE_SPELLS);
   stat_spell_cell* ptr;

   while ((ptr = cll.next())) {
      switch (ptr->stat_spell)
        {
           //First, those requiring a MOB for target
        case 1: case 10: case 11: case 30:
        case 38: case 39: case 136: case 139: case 140:
        case 151: case 152: case 155: case 160:
        case 162: case 163: case 164: case 171:
        case 172: case 174: case 175: case 176:
        case 177: case 178: case 179: case 180: case 189:
        case 191: case 192: case 193: case 194: case 195: case 197:
        case 199: case 201: case 202: case 203: case 204:
        case 205: case 206: case 208: case 217: case 219:
        case 221: case 222: case 224: case 225: case 226:
        case 227: case 228:
        {
           found_proc = TRUE;
           if (vict->Strlen() == 0) {
              targ = Top(pc.IS_FIGHTING);
           }//if
           else {
              targ = ROOM.haveCritNamed(j_th, vict, pc.SEE_BIT);
           }//else
           
           if (targ) {
              if (targ->isMob()) { //if its a MOB
                 targ = mob_to_smob(*targ, pc.getCurRoomNum(), TRUE, i_th,
                                    vict, pc.SEE_BIT);
              }//if
           }//if

           if (!targ) {
              targ = &pc;
           }
           if (do_wand_scroll_proc(targ, ptr->stat_spell, pc,
                                   ptr->bonus_duration))
             junk_scroll = TRUE;

          break;
        }//case for mob targets

        case 159:        //special case, gate spell (portal too?)
        {
           targ = have_crit_named(pc_list, i_th, vict, pc.SEE_BIT,
                                  ROOM);
           if (!targ) {
              show("That person isn't logged on.\n", pc);
           }//if
           else {
              if (do_wand_scroll_proc(targ, ptr->stat_spell, pc,
                                      ptr->bonus_duration))
                junk_scroll = TRUE;
           }
           break;
        }
        // now for doors
        case 21: case 181:
        {
           door* dr_ptr;
           if ((dr_ptr = door::findDoor(ROOM.DOORS, i_th, vict,
                                        pc.SEE_BIT, ROOM))) {
              if (dr_ptr->isSecret()) {
                 if (!name_is_secret(vict, *dr_ptr)) {
                    show("You don't see that exit.\n", pc);
                    return;
                 }//if
              }//if

              if (do_wand_scroll_proc(targ, ptr->stat_spell, pc,
                                      ptr->bonus_duration))
                junk_scroll = TRUE;
           }//if
           else {
              pc.show("You need to specify a door on which to use your scroll.\n");
           }//else

          break;
        }//door case statement
        
        // spells requiring no target
        case 12:
        case 22: case 25: case 145: case 146:
        case 147: case 149: case 150: case 153: case 200:
        case 210: case 211: case 218:
           // spells requiring room targs, always use current room
        case 4: case 6: case 14: case 17: case 18: case 20:
        case 198: case 215: case 220:
        {
           if (do_wand_scroll_proc(ptr->stat_spell, pc,
                                   ptr->bonus_duration))
             junk_scroll = TRUE;

          break;
        }//case

        //object targets
        case 124: case 156:
        case 165: case 182: case 183: case 184: case 185:
        case 186: case 196: case 213:
        {
           object* ob_ptr;
           ob_ptr = have_obj_named(pc.inv, i_th, vict,
                                   pc.SEE_BIT, ROOM);
           if (ob_ptr) {
              if (!ob_ptr->IN_LIST) {
                 ob_ptr = obj_to_sobj(*ob_ptr, &(pc.inv), TRUE,
                                      i_th, vict, pc.SEE_BIT, ROOM);
              }
           }
           else {
              ob_ptr = have_obj_named(ROOM.inv, i_th, vict,
                                      pc.SEE_BIT, ROOM);
              
              if (ob_ptr && !ob_ptr->IN_LIST) {
                 ob_ptr = obj_to_sobj(*ob_ptr, &(ROOM.inv), TRUE,
                                      i_th, vict, pc.SEE_BIT, ROOM);
              }
           }//else

           if (ob_ptr) {
              pc.show("Which object shall the scroll be used on?\n");
           }
           else {
              if (do_wand_scroll_proc(ob_ptr, ptr->stat_spell, pc,
                                      ptr->bonus_duration))
                junk_scroll = TRUE;
           }

           break;
        }//case

        default:
        { 
           mudlog << "ERROR:  found default case in recite, spell_num: "
                  << ptr->stat_spell << "  on object#:  "
                  << scroll->getIdNum() << endl;
           pc.show("You call upon forces unavailable at this time!\n");
           found_proc = FALSE;
        }
        }//switch
   }//while

   if (!found_proc) {
      show("Nothing seems to happen.\n", pc);
      Sprintf(buf, "ERROR:  object# %i has no spells in recite.\n",
	      scroll->OBJ_NUM);
      mudlog.log(ERR, buf);
   }//if

   if (junk_scroll) {
      if (posn) {
         recursive_init_unload(obj_list[pc.EQ[posn]->OBJ_NUM], 0);
         if (pc.EQ[posn]->IN_LIST) { //if a SOBJ
            delete pc.EQ[posn];
         }//if
         pc.EQ[posn] = NULL;
      }//if
      else {
         pc.loseInv(scroll);
         recursive_init_unload(*scroll, 0);
         if (scroll->IN_LIST) {
            delete scroll;
         }//if
      }//else
   }//if should get rid of it
}//recite



ofstream idea_log("./log/idea_log");

void idea(const String& str, critter& pc) {
   String buf(300);

   if (pc.mob)
      return;

   if (str.Strlen() == 0) {
      show("Usage:  idea <text of idea>.\n", pc);
      return;
   }//if

   Sprintf(buf, "%S, room %i:  %S\n\n", name_of_crit(pc, ~0),
           pc.getCurRoomNum(), &str);
   idea_log << buf << flush;
   show("Idea logged, thanks a heap.\n", pc);
}//idea   

ofstream bug_log("./log/bug_log");

void bug(const String& str, critter& pc) {
   String buf(300);

   if (pc.mob)
      return;

   if (str.Strlen() == 0) {
      show("Usage:  bug <text of bug description>.\n", pc);
      return;
   }//if

   Sprintf(buf, "%S, room %i:  %S\n\n", name_of_crit(pc, ~0), pc.getCurRoomNum(), 
           &str);
   bug_log << buf << flush;
   show("Bug logged, thanks.\n", pc);
}//bug


void oclone(int i_th, const String* item, critter& pc) {
   String buf(100);
   object* obj_ptr;

   if (!pc.pc || !pc.pc->imm_data) {
     show("Eh??\n", pc);
     return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (!pc.doesOwnRoom(ROOM)) {
      show("You don't own this zone.\n", pc);
      return;
   }//if

   if (item->Strlen() == 0) {
      if (check_l_range(i_th, 0, NUMBER_OF_ITEMS, pc, TRUE)) {
         obj_ptr = &(obj_list[i_th]);
         if (!obj_ptr->OBJ_FLAGS.get(10)) {
            show("That object has not been defined yet.\n", pc);
            return;
         }//if
      }//if
      else {
         return;
      }//else
   }//if
   else {
      obj_ptr = have_obj_named(pc.inv, i_th, item, pc.SEE_BIT,
                               ROOM);
      if (!obj_ptr) {
         obj_ptr = ROOM.haveObjNamed(i_th, item, pc.SEE_BIT);
      }//if
   }//else

   if (!obj_ptr) {
      show("You don't see that object.\n", pc);
   }//if
   else {  //got one!
      int new_obj = get_next_obj();
      if (!check_l_range(new_obj, 2, NUMBER_OF_ITEMS, pc, FALSE)) {
         show("OOPS, seems the object database is FULL, talk at your IMP.\n",
              pc);
         return;
      }//if
      obj_list[new_obj] = obj_list[obj_ptr->OBJ_NUM];
      Sprintf(buf, "CLONE OF:  %S.", Top(obj_ptr->ob->names));
      obj_list[new_obj].ob->in_room_desc = buf;
      obj_list[new_obj].ob->short_desc = buf;
      obj_list[new_obj].OBJ_NUM = new_obj;
      obj_list[new_obj].ob->cur_stats[3] = ROOM.getZoneNum();
      obj_list[new_obj].IN_LIST = NULL;
      pc.gainInv(&(obj_list[new_obj]));
   }//else
   show("Okay, CLONE of object now in your inventory.\n", pc);
}//oclone


void oload(int i_th, const String* item, critter& pc) {
   object* obj_ptr;
   //   mudlog.log(TRC, "In oclone.\n");

   if (!item) {
      mudlog.log(ERR, "ERROR:  NULL sent to clone.\n");
      return;
   }//if

   if (pc.isImmort() || (pc.isSmob() && !pc.isCharmed())) {

      if (pc.isFrozen()) {
         show("You are too frozen to do anything.\n", pc);
         return;
      }//if
      
      if (item->Strlen() == 0) {
         if (check_l_range(i_th, 0, NUMBER_OF_ITEMS, pc, TRUE)) {
            obj_ptr = &(obj_list[i_th]);
            if (!obj_ptr->OBJ_FLAGS.get(10)) {
               show("That object has not been defined yet.\n", pc);
               return;
            }//if
         }//if
         else {
            return;
         }//else
      }//if
      else {
         obj_ptr = have_obj_named(pc.inv, i_th, item, pc.SEE_BIT,
                                  ROOM);
         if (!obj_ptr) {
            obj_ptr = ROOM.haveObjNamed(i_th, item, pc.SEE_BIT);
         }//if
      }//else
      if (!obj_ptr) {
         show("You don't see that object.\n", pc);
      }//if
      else {  //got one!
         
         if (!obj_ptr->isBulletinBoard()) {
            recursive_init_loads(*obj_ptr, 0);
         }
         else {
            obj_ptr->incrementCurInGame();
         }
         pc.gainInv(&(obj_list[obj_ptr->OBJ_NUM]));
      }//else
      show("Okay, loaded object now in your inventory.\n", pc);
   }//if
   else {
      show("Eh??\n", pc);
      return;
   }//if

}//oload


void rclone(int src_room, const String* direction, int dist, critter& pc) {
  //mudlog.log(TRC, "In rclone.\n");

   dist--;

   if (!check_l_range(dist, -3, 10, pc, FALSE)) {
      show("The distance must be between -3 and 10.\n", pc);
      return;
   }//if

   if (!pc.isImmort()) {
     show("Eh??\n", pc);
     return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (direction->Strlen() == 0) {
      show("Usage:  rclone <room_number> <direction>\n", pc);
      return;
   }//if

   int zone_num = ROOM.getZoneNum();
   room* new_rm = get_next_room(zone_num);

   if (!pc.doesOwnRoom(ROOM)) {
     show("You don't own this zone.\n", pc);
     return;
   }//if

   if (!new_rm) {
      show("Ack, there are no more available room slots in this zone.\n", 
           pc);
      return;
   }//if

   int new_room_num = new_rm->getIdNum();
   
   if (mudlog.ofLevel(DBG))
      mudlog << "Rclone:  New room num:  " << new_room_num << endl;

   if (src_room == 1) {
      src_room = ROOM.getIdNum();
   }

   if (!check_l_range(src_room, 1, NUMBER_OF_ROOMS, pc, TRUE))
      return;

   if (room_list[src_room].isVehicle()) {
      pc.show("Vehicle cloning not supported at this time.");
      return;
   }

   if (!room_list[src_room].isInUse()) {
      pc.show("Source room is not in use!\n");
      return;
   }

   *new_rm = room_list[src_room];

   // We coppied over it when assigning it...
   new_rm->setRoomNum(new_room_num);

   clear_ptr_list(new_rm->doors);  //don't want to start w/any doors
   clear_ptr_list(new_rm->keywords);  //or keywords

   if (mudlog.ofLevel(DBG))
      mudlog << "Rclone:  New room num, before door_to:  "
             << new_room_num << endl;

   door_to(new_room_num, dist, direction, pc);
}//rclone



void rinit(int src_rm, int dest_rm, critter& pc) {
  //mudlog.log(TRC, "In rclone.\n");

   if (dest_rm == 1) {
      dest_rm = src_rm;
      src_rm = ROOM.getIdNum();
   }

   if (!check_l_range(dest_rm, 2, NUMBER_OF_ROOMS, pc, TRUE)) {
      return;
   }//if

   if (!check_l_range(src_rm, 2, NUMBER_OF_ROOMS, pc, TRUE)) {
      return;
   }//if

   if (!pc.pc || !pc.pc->imm_data) {
     show("Eh??\n", pc);
     return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   int dest_z = room_list[dest_rm].getZoneNum();
   int z = ROOM.getZoneNum();

   if (!pc.doesOwnRoom(ROOM)) {
     show("You don't own this zone.\n", pc);
     return;
   }//if

   if (z != dest_z) {
      pc.show("Your destinatio room must be in this zone.\n");
      return;
   }

   if (room_list[dest_rm].isInUse()) {
      pc.show("That room is already in use, maybe you want rclear?\n");
      return;
   }

   if (!room_list[src_rm].isInUse()) {
      pc.show("Source room is not in use!\n");
      return;
   }

   if (room_list[src_rm].isVehicle()) {
      pc.show("Can't init with a vehicle as a source.\n");
      return;
   }

   room_list[dest_rm] = room_list[src_rm];

   room_list[dest_rm].setRoomNum(dest_rm); //just in case

   clear_ptr_list(room_list[dest_rm].DOORS);//don't want to start w/any doors

}//rinit


void mclone(int i_th, const String* item, critter& pc) {
   critter* crit_ptr; 
   String buf(100);

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (!pc.isImmort()) {
     show("Eh??\n", pc);
     return;
   }//if

   if (!pc.doesOwnRoom(ROOM)) {
     show("You don't own this zone.\n", pc);
     return;
   }//if

   if (item->Strlen() == 0) {
      if (check_l_range(i_th, 0, NUMBER_OF_MOBS, pc, TRUE)) {
         crit_ptr = &(mob_list[i_th]);
         if (!crit_ptr->isInUse()) {
            show("That mobile has not been defined yet.\n", pc);
            return;
         }//if
      }//if
      else {
         return;
      }//else
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, item, pc.SEE_BIT);
   }//else

   if (!crit_ptr) {
      show("You don't see that person.\n", pc);
      return;
   }//if
   
   if (crit_ptr->pc || !crit_ptr->mob) {
      show("You can't clone a pc.\n", pc); 
      return;
   }//if
                 /* do it then */

   int new_num = get_next_mob();
   if (!check_l_range(new_num, 2, NUMBER_OF_MOBS, pc, FALSE)) {
      show("OOPS, seems we are out of room in the database, tell an IMP!\n",
           pc);
      return;
   }//if
   
   mob_list[new_num] = mob_list[crit_ptr->MOB_NUM]; //make a copy
   Sprintf(buf, "CLONE OF:  %S.", Top(crit_ptr->names));
   mob_list[new_num].in_room_desc = buf;
   mob_list[new_num].short_desc = buf;
   mob_list[new_num].setIdNum(new_num);
   mob_list[new_num].setNativeZoneNum(ROOM.getZoneNum());
   mob_list[new_num].CRITTER_TYPE = 2;
   ROOM.gainCritter(&(mob_list[new_num]));
   show("Okay, critter CLONED, its now in your room.\n", pc);
}//mclone


void mload(int i_th, const String* item, critter& pc) {
   critter* crit_ptr; 
   
   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (!pc.isImmort()) {
      show("Eh??\n", pc);
      return;
   }//if

   if (!pc.doesOwnRoom(ROOM)) {
     show("You don't own this zone.\n", pc);
     return;
   }//if

   if (item->Strlen() == 0) {
      if (check_l_range(i_th, 0, NUMBER_OF_MOBS, pc, TRUE)) {
         crit_ptr = &(mob_list[i_th]);
         if (!crit_ptr->CRIT_FLAGS.get(18)) {
            show("That mobile has not been defined yet.\n", pc);
            return;
         }//if
      }//if
      else {
         return;
      }//else
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(i_th, item, pc.SEE_BIT);
   }//else

   if (!crit_ptr) {
      show("You don't see that person.\n", pc);
      return;
   }//if
   
   if (crit_ptr->pc || !crit_ptr->mob) {
      show("You can't load a pc.\n", pc); 
      return;
   }//if
                 /* do it then */

   if (crit_ptr->isPlayerShopKeeper()) {
      critter* shop_keeper = load_player_shop_owner(crit_ptr->getIdNum());
      if (shop_keeper) {
         ROOM.gainCritter(shop_keeper);
      }//if
   }//if
   else {
      ROOM.gainCritter(&(mob_list[crit_ptr->getIdNum()]));
   }

   recursive_init_loads(*crit_ptr);
   show("Okay, critter LOADED in this room.\n", pc);
}//mload


void wield(int i_th, const String* item, critter& pc) {
   String buf("wield");
  
   wear(i_th, item, 1, &buf, pc);
}//wield


void hold(int i_th, const String* item, critter& pc) {
   String buf("hold");
  
   wear(i_th, item, 1, &buf, pc);
}//hold


void flee_to_safety(critter& pc, int& is_dead) {
  /* this is a semi-inteligent flee used by mobs to escape aggressors */
  /* it attempts to escape to havens, !magic, or rough terrain        */
   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB sent to flee_to_safety.\n");
      return;
   }//if
   
   if (pc.CRIT_FLAGS.get(14)) { //if paralyzed
      show("You can't move a muscle.\n", pc);
      return;
   }//if
   
   Cell<door*> cll(ROOM.DOORS);
   door* ptr;

   while ((ptr = cll.next())) {
      if (room_list[abs(ptr->destination)].isHaven() ||  //haven
	  room_list[abs(ptr->destination)].isNoMagic() ||  //!magic
	  room_list[abs(ptr->destination)].isSwamp()) { //swamp
	 if (move(pc, 1, *(direction_of_door(*ptr)), TRUE, ROOM, is_dead)) {
	    return;
	 }//if
	 else {
	    flee(pc, is_dead);
	    return;
	 }//else
      }//if
   }//while

   flee(pc, is_dead);  //any is as good as the rest
}//flee_to_safety


void flee(critter& pc, int& is_dead) {
   int flee_chance;
   critter* crit_ptr;
   String buf(100);
   short have_exit = FALSE;
   door* dr_ptr;

   //   mudlog.log(TRC, "In flee.\n");

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (pc.isParalyzed()) {
      show("You can't move a muscle.\n", pc);
      return;
   }//if
   
   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  mob trying to flee.\n");
   }//if
   else if (pc.POS != POS_STAND) { //if not standing
      show("You can't flee sitting on your butt!\n", pc);
   }//if
   else { //lets try it
      if ((crit_ptr = Top(pc.IS_FIGHTING))) { //ie in battle
         if (crit_ptr->mob) { //if its a mob
            if (crit_ptr->MOB_FLAGS.get(1)) { //if !flee
               Sprintf(buf, "%S prevents you from fleeing.\n",
                       name_of_crit(*crit_ptr, pc.SEE_BIT));
               buf.Cap();
               show(buf, pc);
	       Sprintf(buf, "prevents %S from fleeing!!\n",
		       name_of_crit(pc, ~0));
	       emote(buf, *crit_ptr, ROOM, TRUE, &pc);
               pc.PAUSE += 1;
               return;
            }//if
         }//if
         flee_chance = pc.DEX * 5 + ROOM.doors.size() * 3;

         if (flee_chance > d(1,100)) { //will try at least

            Cell<door*> dr_cll(ROOM.doors);
            door* dr_ptr;
            while ((dr_ptr = dr_cll.next())) {
               if (!(dr_ptr->isClosed())) {
                  have_exit = TRUE;
                  break;
               }//if
            }//while
            if (!have_exit) {
               show("Death may be your only exit!\n", pc);
               pc.PAUSE += 1;
               return;
            }//if

			/* now move pc outa room */
				/* quit battle */
            Cell<critter*> cll(pc.IS_FIGHTING);
            critter* tmp_ptr;
            while ((tmp_ptr = cll.next())) {
               tmp_ptr->IS_FIGHTING.loseData(&pc);
            }//while
            pc.IS_FIGHTING.clear();

            	/* we know we have an exit, lets find it */
            int sanity = 0;
            while (TRUE) {
               if (sanity++ > 20) {
                  show("You can't find an exit!\n", pc);
                  pc.PAUSE += 1;
                  return;
               }
                  
               if ((dr_ptr = door::findDoor(ROOM.doors, 1,
                      Top(door_list[d(1,10)].names), ~0, ROOM))) {
                  if (!(dr_ptr->dr_data->door_data_flags.get(2))) {
                     break;  //weee hoooo, found a good one!
                  }//if
               }//if
            }//while
            
            show("You try to flee the scene!\n", pc);
            Sprintf(buf, "tries to flee to the %S!", 
                    direction_of_door(*dr_ptr));
            emote(buf, pc, ROOM, FALSE);

            move(pc, 1, *(direction_of_door(*dr_ptr)), FALSE, ROOM, is_dead);
            
            if (is_dead)
               return;

            pc.PAUSE += 1;
            pc.MOV -= 25;
         }//if
         else { //didn't even beat the odds...
            show("You can't elude your foe.\n", pc);
            pc.PAUSE += 1;
         }//else
      }//if in battle
      else {
         Cell<door*> dr_cll(ROOM.doors);
         while ((dr_ptr = dr_cll.next())) {
            if (!(dr_ptr->dr_data->door_data_flags.get(2))) {
               have_exit = TRUE;
               break;
            }//if
         }//while
         if (!have_exit) {
            show("You can't find an exit.\n", pc);
         }//if
         else {
            while (TRUE) {
               if ((dr_ptr = door::findDoor(ROOM.doors, 1,
                      Top(door_list[d(1,10)].names), ~0, ROOM))) {
                  if (!(dr_ptr->dr_data->door_data_flags.get(2))) {
                     break;  //weee hoooo, found a good one!
                  }//if
               }//if
            }//while
            show("You try to flee the scene!\n", pc);
            Sprintf(buf, "tries to flee to the %S!", 
                    direction_of_door(*dr_ptr));
            emote(buf, pc, ROOM, FALSE);
            move(pc, 1, *(Top(dr_ptr->dr_data->names)), FALSE, ROOM, is_dead);
            
            if (is_dead)
               return;

            pc.PAUSE += 1;
            pc.MOV -= 25;
         }//else
      }//else, not in battle
   }//else, past little checks
}//flee


void slay(int i_th, const String* vict, critter& pc) {
   String buf(100);
   critter* ptr;

   if (!pc.pc || !pc.pc->imm_data) {
      show("You wish!!\n", pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

   if (!ptr) {
      show("You detect not that being.\n", pc);
      return;
   }//if

   if (ptr->isMob()) {
      ptr = mob_to_smob(*ptr, pc.getCurRoomNum(), TRUE, i_th, vict, pc.SEE_BIT);
   }//if

   if (ptr->pc && ptr->pc->imm_data && 
       (ptr->IMM_LEVEL > pc.IMM_LEVEL)) {
     show("RESPECT THY ELDERS!!\n", pc);
     pc.setHP(1); //that'll teach em!!
     return;
   }//if

   Sprintf(buf, "You slay %S.\n", name_of_crit(*ptr, pc.SEE_BIT));
   show(buf, pc);
   Sprintf(buf, "%S slays you!!\n", name_of_crit(pc, ptr->SEE_BIT));
   show(buf, *ptr);
   Sprintf(buf, "%S slays %S!\n", name_of_crit(pc, ~0), 
           name_of_crit(*ptr, ~0));
   show_all_but_2(pc, *ptr, buf, ROOM);
   
   agg_kills_vict(pc, *ptr);
}//slay


void time(critter& pc) {
   String buf(50);

   Sprintf(buf, "It is %s.\n", military_to_am(Hour));
   show(buf, pc);
}//time


void date(critter& pc) {
   String buf(100);

   Sprintf(buf, "The date is %s %i, %i.\n", get_month(Day),
           get_day_of_month(Day), Year);
   show(buf, pc);
}//date


void tail(int i_th, const String* vict, critter& pc, int do_smob = FALSE) {
  String buf(100);
  critter* ptr;

  if (pc.isMob())
    return;

  if (!pc.pc && !do_smob)
    return;
  
  if (pc.isFrozen()) {
     show("You are too frozen to do anything.\n", pc);
     return;
  }//if
  
  if (pc.isParalyzed()) {
     show("You can't move a muscle.\n", pc);
     return;
  }//if
  
  if (get_percent_lrnd(TAIL_SKILL_NUM, pc) < 1) {
     show("You wiggle your bottom!?!!!\n", pc);
     Sprintf(buf, "wiggles %s bottom!?!!!\n", get_his_her(pc));
     emote(buf, pc, ROOM, FALSE);
     return;
  }//if

  if (vict->Strlen() == 0) {
     ptr = &pc;
  }//if
  else {
     ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);
  }//if

  if (!ptr) {
     show("You can't detect that being.\n", pc);
     return;
  }//if
  
  pc.CRIT_FLAGS.turn_on(23);
  pc.doFollow(*ptr, TRUE);
}//tail

  
void follow(int i_th, const String* vict, critter& pc, int do_msg = TRUE) {
   critter* ptr;
   String buf(100);

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  mob trying to follow..needs to be smob or pc.\n");
      return;
   }//if

   if (pc.pc && pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (pc.CRIT_FLAGS.get(14)) { //if paralyzed
      show("You can't move a muscle.\n", pc);
      return;
   }//if
   
   if (!vict) {
      mudlog.log(ERR, "ERROR: vict is NULL, in follow().\n");
      return;
   }//if

   if (vict->Strlen() == 0) {
      ptr = &pc;
   }//if
   else {
      ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);
   }//if

   if (!ptr) {
      show("You can't detect that being.\n", pc);
      return;
   }//if

   pc.doFollow(*ptr, do_msg);
}//follow


/* pc follows vict */
void critter::doFollow(critter& vict, int do_msg = TRUE) {
  String buf(100);

  if (vict.isMob() || isMob()) {
    mudlog.log(ERR, "ERROR:  MOB's in do_follow.\n");
    return;
  }//if

  if (&vict == this) { //if follow self
    if (!FOLLOWER_OF) {
      show("You already follow yourself.\n");
      return;
    }//if
    else {
      doUngroup(1, &NULL_STRING);  //no longer part of a group
      doBecomeNonPet();  //no longer a pet
    }//else
  }//if
  else {                       //follow some other
    if (FOLLOWER_OF) {  // if possibly part of a group
      doUngroup(1, &NULL_STRING);  //pets will still be pets....for now
    }//if
    FOLLOWER_OF = &vict;
    vict.FOLLOWERS.append(this);

    if (do_msg) {
      Sprintf(buf, "%S now follows you.\n", getName(vict.SEE_BIT));
      buf.Cap();
      vict.show(buf);
    }//if do_msg

    Sprintf(buf, "You now follow %S.\n", vict.getName(SEE_BIT));
    show(buf);

    String cmd = "follow";
    getCurRoom()->checkForProc(cmd, NULL_STRING, *this, vict.MOB_NUM);

  }//else, follow some other
}//doFollow


void group(int i_th, const String* vict, critter& pc) {
   Cell<critter*> cll;
   critter* ptr;
   String buf(100);

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  mob sent to group, needs to be a smob or pc.\n");
      return;
   }//if

   if (vict->Strlen() == 0) { //display group

      if (IsEmpty(pc.GROUPEES) && !(pc.FOLLOWER_OF)) {
         show("You are a party of one!\n", pc);
         return;
      }//if

      Sprintf(buf, "Name %P30 Class%P48 Hp         Mana        Mov\n\n");
      show(buf, pc);

      if (pc.FOLLOWER_OF) {
         pc.FOLLOWER_OF->GROUPEES.head(cll);
      }//if
      else {
         pc.GROUPEES.head(cll); //then IS the leader
      }//if

      while ((ptr = cll.next())) {
         Sprintf(buf, "%S%P30 %s(%i)%P45 %i/%i%P55%i/%i%P68%i/%i\n",
             name_of_crit(*ptr, pc.SEE_BIT), class_of_crit(*ptr), 
             ptr->LEVEL, ptr->HP, ptr->HP_MAX,
             ptr->MANA, ptr->MA_MAX, ptr->MOV, ptr->MV_MAX);
         buf.Cap();
         show(buf, pc);
      }//while
   }//if displaying group

 	//*******************  group all  *****************************//

   else if (strcasecmp("all", *vict) == 0) {
      if (pc.FOLLOWER_OF) {
         show("You have to be the leader in order to enroll groupees.\n", 
              pc);
         return;
      }//if
      if (IsEmpty(pc.FOLLOWERS)) {
         show("You need followers in order to form a group!\n", pc);
         return;
      }//if

      pc.GROUPEES.gainData(&pc); //enroll self fer sure

      pc.FOLLOWERS.head(cll);
      while ((ptr = cll.next())) {
 	 if (ptr->isTailing() || 
	     (pc.pc && pc.isNoHassle())) {
	   continue; //fail silently
	 }//if !hassle or tailing, can't be grouped

         pc.GROUPEES.gainData(ptr);
         show("You are now part of the group.\n", *ptr);
         Sprintf(buf, "%S is now part of the group.\n", name_of_crit(*ptr,
                 pc.SEE_BIT));
         buf.Cap();
         show(buf, pc);
      }//while
   }// if group all

	//*****************  group an individual  ************************//

   else { //group some individual
      ptr = have_crit_named(pc.FOLLOWERS, i_th, vict, pc.SEE_BIT, ROOM);
      if (!ptr) {
         show("That person is not following you.\n", pc);
         return;
      }//

      pc.GROUPEES.gainData(&pc);  //make sure self is in

      pc.GROUPEES.gainData(ptr);
      show("You are now part of the group.\n", *ptr);
      show("Ok.\n", pc);

      String cmd = "group";
      ROOM.checkForProc(cmd, NULL_STRING, pc, ptr->MOB_NUM);

   }//else
}//group


/* some parsing is done by this function */
void order(String* str, critter& pc) {
   Cell<critter*> cll;
   critter* ptr;
   short eos = FALSE, term_by_period = FALSE;
   String str1, str2;
   String name, buf(100);
   int i = 1;

   if (pc.pc && pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (pc.POS > POS_SIT) {
     show("You are too relaxed.\n", pc);
     return;
   }//if

   str1 = str->Get_Command(eos, term_by_period);
   if (str1.Strlen() != 0) {
      if (isnum(str1)) {
         i = atoi(str1);
         name = str->Get_Command(eos, term_by_period);
      }//if
      else {
         if (strncasecmp(str1, "followers", str1.Strlen()) == 0) {
            i = -1;
         }//if
         else {
            name = str1;
         }//else
      }//else

      if (str->Strlen() == 0) {
         show("Order them to do what??\n", pc);
         return;
      }//if

      if (i == -1) { //if order fol <cmd_string>
         if (IsEmpty(pc.PETS)) {
            show("But, you have no pets!\n", pc);
            return;
         }//if

	 List<critter*> tmp_lst;
         ROOM.getPetsFor(pc, tmp_lst);

	 /* this list will be static, the other can change according to what
	    the mob was ordered to do */
	 tmp_lst.head(cll);
	 while ((ptr = cll.next())) {
            if (ROOM.haveCritter(ptr)) {
               ptr->processInput(*str, TRUE);
            }//if
	 }//while

         show("Ok.\n", pc);
      }//if
      else {
         ptr = ROOM.haveCritNamed(i, &name, pc.SEE_BIT);
         if (ptr) {
            if (HaveData(ptr, pc.PETS)) {

               String cmd = "order";
               ROOM.checkForProc(cmd, *str, pc, ptr->MOB_NUM);

               ptr->processInput(*str, TRUE);
               show("Ok.\n", pc);
            }//if
            else {
             Sprintf(buf, "%S looks at you and smirks at your suggestion.\n",
                     name_of_crit(*ptr, pc.SEE_BIT));
             buf.Cap();
             show(buf, pc);
            }//else
         }//if ptr
         else {
            show("You don't see that person here.\n", pc);
         }//else
      }//else, order <targ> <cmd_string>
   }//if valid string gotten
   else {
      show("Order who to do what???\n", pc);
   }//else
}//order


/* some parsing is done by this function */
void force(String* str, critter& pc) {
   critter* ptr;
   short eos = FALSE, term_by_period = FALSE;
   String str1, str2;
   String name, buf(100);
   int i = 1;

   if (!pc.pc || !pc.pc->imm_data) {
      show("He knew there was another, but you are not the one!\n", pc);
      return;
   }//if

   if (pc.IMM_LEVEL < 5) {
      show("You are not yet worthy of the force.\n", pc);
      return;
   }//if
  
   if (pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   str1 = str->Get_Command(eos, term_by_period);
   if (str1.Strlen() != 0) {
      if (isnum(str1)) {
         i = atoi(str1);
         name = str->Get_Command(eos, term_by_period);
      }//if
      else {
	 name = str1;
      }//else

      if (str->Strlen() == 0) {
         show("Force them to do what??\n", pc);
         return;
      }//if

      ptr = have_crit_named(pc_list, i, &name, pc.SEE_BIT, ROOM);
      if (!ptr) {
	ptr = have_crit_named(linkdead_list, i, &name,
                              pc.SEE_BIT, ROOM);
      }//if
      if (!ptr) {
         ptr = ROOM.haveCritNamed(i, &name, pc.SEE_BIT);
      }//if

      if (ptr) {
        if (ptr->isMob()) {
	  ptr = mob_to_smob(*ptr, pc.getCurRoomNum(), TRUE, i, &name, pc.SEE_BIT);
	}//if
        
        if (ptr->pc && ptr->pc->imm_data &&
	    ptr->IMM_LEVEL >= pc.IMM_LEVEL) {
	  show("Heh, don't you WISH you could force them!!\n", pc);
          return;
	}//if

        Sprintf(buf, "%S has forced you to: %S\n", 
		name_of_crit(pc, ptr->SEE_BIT), str);
        show(buf, *ptr);
	ptr->processInput(*str, TRUE);
	show("Ok\n", pc);
      }//if
      else {
	show("That person is neither playing nor linkdead, or not\n", pc);
        show("visible to you.\n", pc);
      }//else
   }//if valid string gotten
   else {
      show("Force who to do what???\n", pc);
   }//else
}//force
   


/* some parsing is done by this function */
void force_all(String* str, critter& pc) {
   critter* ptr;
   String buf(100);
   
   mudlog.log(TRC, "In force_all.\n");
   
   if (!pc.pc || !pc.pc->imm_data) {
      show("He knew there was another, but you are not the one!\n", pc);
      return;
   }//if
   
   if (pc.IMM_LEVEL < 5) {
      show("You are not yet worthy of the force.\n", pc);
      return;
   }//if
   
   if (pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (str->Strlen() == 0) {
      show("Force all to do what??\n", pc);
      return;
   }//if
   
   if (mudlog.ofLevel(DBG)) {
      mudlog << "Passed tests in force_all, str:  " << *str << endl;
   }

   Cell<critter*> cll(pc_list);
   while ((ptr = cll.next())) {
      if (ptr->MODE == MODE_NORMAL) {
         if (!(ptr->pc->imm_data && (ptr->IMM_LEVEL >= pc.IMM_LEVEL))) {
            Sprintf(buf, "%S has forced you to: %S\n", 
                    name_of_crit(pc, ptr->SEE_BIT), str);
            show(buf, *ptr);
            ptr->processInput(*str, FALSE);
         }//if
      }//if in normal mode
   }//while
   show("Ok\n", pc);
}//force_all
   
   
void enslave(int i_th, const String* vict, critter& pc) {
   critter* ptr;
   String buf(100);

   if (!pc.pc || !pc.pc->imm_data) {
      show("Do what??\n", pc);
      return;
   }//if

   if (pc.pc && pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (!vict) {
      mudlog.log(ERR, "ERROR: vict is NULL, in enslave().\n");
      return;
   }//if

   ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

   if (!ptr) {
      show("You can't detect that being.\n", pc);
      return;
   }//if

   ///******************* have a valid ptr now ***********************///

   if (ptr->isMob()) {
      ptr = mob_to_smob(*ptr, pc.getCurRoomNum(), TRUE, i_th, vict,
                      pc.SEE_BIT);
   }//if

   if (ptr->pc && ptr->pc->imm_data) { //if vict is an IMM
     if (ptr->IMM_LEVEL >= pc.IMM_LEVEL) {
        show("You cannot enslave one so powerful, fool!!\n", pc);
        pc.PAUSE += 10;
        return;
     }//if
   }//if

   Put(ptr, pc.PETS);
   ptr->MASTER = &pc;
   ptr->doFollow(pc);
}//enslave


void do_tell(critter& pc, const char* message, critter& targ, 
             short show_teller, int targs_room_num) {

   targs_room_num = targs_room_num; // get rid of un-used variable warning 

   String buf(200);
   String msg;

   if (!message)
      return;
   else {
      msg = message;
      pc.drunkifyMsg(msg);
   }


   String tag;
   String untag;

   if (targ.isUsingClient()) {
      tag = "<TELL>";
      untag = "</TELL>";
   }
   else if (targ.isUsingColor()) {
      tag = *(targ.getTellColor());
      untag = *(targ.getDefaultColor());
   }

   Sprintf(buf, "%S%S tells you, '%S'\n%S",
           &tag, name_of_crit(pc, targ.SEE_BIT), &msg, &untag);
   buf.setCharAt(tag.Strlen(), toupper(buf[tag.Strlen()]));
   show(buf, targ);

   if (targ.pc) {
      targ.pc->rep_to = *(name_of_crit(pc, targ.SEE_BIT));
   }

   if (show_teller) {

      if (pc.isUsingClient()) {
         tag = "<TELL>";
         untag = "</TELL>";
      }
      else if (pc.isUsingColor()) {
         tag = *(pc.getTellColor());
         untag = *(pc.getDefaultColor());
      }
      else {
         tag = "";
         untag = "";
      }

      Sprintf(buf, "%SYou tell %S, '%S'\n%S",
              &tag, name_of_crit(targ, pc.SEE_BIT), &msg, &untag);
      show(buf, pc);
   }//if

   String cmd = "tell";
   String mmsg = msg;
   ROOM.checkForProc(cmd, mmsg, pc, targ.MOB_NUM);

}//do_tell

void showZones(critter& pc) {
   if (pc.isImmort()) {
      pc.show(ZoneList::instance().toString());
   }
   else {
      pc.show("Eh?\n");
   }
}//showZones

// to active zones.
void addZone(int i, critter& pc) {
   if (check_l_range(i, 0, NUMBER_OF_ZONES, pc, TRUE)) {
      if (pc.isImmort() && (pc.getImmLevel() > 8)) {
         ZoneList::instance().add(i);
         ZoneList::instance().writeSelf();
         ZoneCollection::instance().zunlock(i);
         pc.show("Ok.\n");
      }
      else {
         pc.show("Eh??\n");
      }
   }
}//addZone


// from active zones
void remZone(int i, critter& pc) {
   if (pc.isImmort() && (pc.getImmLevel() > 8)) {
      ZoneList::instance().remove(i);
      ZoneList::instance().writeSelf();
      ZoneCollection::instance().zlock(i);
      pc.show("Ok.\n");
   }
   else {
      pc.show("Eh??\n");
   }
}

void junk(int i_th, const String* str1, const String* str2, critter& pc) {
   do_junk(TRUE, i_th, str1, str2, pc);
}

void silent_junk(int i_th, const String* str1, const String* str2,
                 critter& pc) {
   if (!pc.isMob() && !pc.isImmort()) {
      pc.show("Eh??");
   }
   else {
      do_junk(FALSE, i_th, str1, str2, pc);
   }
}//silent_junk


void do_junk(int do_msg, int i_th, const String* str1,
             const String* str2, critter& pc) {
   String buf(100);
   String targ;
   Cell<object*> cll(pc.inv);
   object* obj_ptr;
   short did_msg = FALSE;

   if (!str1) {
      mudlog.log(ERR, "ERROR: str1 is NULL in junk().\n");
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (pc.isParalyzed()) {
      show("You can't move a muscle.\n", pc);
      return;
   }//if
   
   if ((*str1 == "all") && (str2->Strlen() == 0)) {
      show("If you want to junk EVERYTHING, go to a dump.\n", pc);
      return;
   }//if
                  /* check for junk all.roadkill */
   if ((*str1 == "all") && (str2->Strlen() != 0)) {
      targ = *str2;
      i_th = -1;
   }//if
   else if (i_th == -1) {
      targ = *str1;
   }//if

   if (i_th == -1) {
      //mudlog.log(TRC, "i_th is/was -1.\n");
      obj_ptr = cll.next();
      while(obj_ptr) {
         if (detect(pc.SEE_BIT, obj_ptr->OBJ_VIS_BIT)) {
            if (obj_is_named(*obj_ptr, targ)) {
               did_msg = TRUE;
               if (obj_ptr->OBJ_FLAGS.get(5) && !(pc.pc && 
			pc.pc->imm_data && (pc.IMM_LEVEL > 1))) {
                  if (do_msg) {
                     Sprintf(buf, "You can't junk %S.\n", 
                             long_name_of_obj(*obj_ptr,
                                              pc.SEE_BIT));
                     show(buf, pc);
                  }//if
                  obj_ptr = cll.next();
	       }//if
	       else { //can junk
                  drop_eq_effects(*obj_ptr, pc, FALSE);

                  if (do_msg) {
                     Sprintf(buf, "You junk %S.\n", 
                             &(obj_ptr->ob->short_desc));
                     show(buf, pc);
                  }//if

                  pc.GOLD += obj_ptr->PRICE / 50;

                  recursive_init_unload(*obj_ptr, 0);

		  if (obj_list[obj_ptr->OBJ_NUM].getCurInGame() < 0) {
                     if (mudlog.ofLevel(DBG)) {
                        mudlog << "ERROR:  junk: obj_cur_in_game:  "
                               << obj_list[obj_ptr->OBJ_NUM].getCurInGame()
                               << "  object_number:  " << obj_ptr->OBJ_NUM
                               << endl;
                     }
		     obj_list[obj_ptr->OBJ_NUM].setCurInGame(0);
		  }//if
                  if (obj_ptr->ob->in_list) { //if a SOBJ
		    delete obj_ptr;
                  }//if is a SOBJ
                  obj_ptr = pc.inv.lose(cll);
               }//else
            }// if obj is named...
            else {
               obj_ptr = cll.next();
            }//else
         }//if can detect
         else {
            obj_ptr = cll.next();
         }//else
      }//while
      if (!did_msg) {
         show("You don't have anything like that.\n", pc);
      }//if
      else {
         show("The gods reward you.\n", pc);
      }//else
   }//if
   else {
      obj_ptr = have_obj_named(pc.inv, i_th, str1, pc.SEE_BIT, ROOM);
      if (!obj_ptr) {
         show("You don't have that.\n", pc);
      }//if
      else {
         if (obj_ptr->OBJ_FLAGS.get(5) && !(pc.pc && 
			pc.pc->imm_data && pc.IMM_LEVEL > 1)) {
            if (do_msg) {
               Sprintf(buf, "You can't junk %S.\n", name_of_obj(*obj_ptr,
                                                                pc.SEE_BIT));
               show(buf, pc);
            }//if
         }//if
         else {
            drop_eq_effects(*obj_ptr, pc, FALSE);
            pc.loseInv(obj_ptr);
            if (do_msg) {
               Sprintf(buf, "You junk %S.\n", 
                       &(obj_ptr->ob->short_desc));
               show(buf, pc);
               show("The gods reward your for your sacrifice.\n", pc);
            }//if
            pc.GOLD += ((obj_ptr->PRICE / 50) + 1);

            if (!obj_ptr->isBulletinBoard()) {
               recursive_init_unload(*obj_ptr, 0);
            }
            else {
               obj_ptr->decrementCurInGame();
            }

	    if (obj_list[obj_ptr->OBJ_NUM].getCurInGame() < 0) {
               if (mudlog.ofLevel(DBG)) {
                  mudlog << "ERROR:  jnk(), obj_cur_in_game:  "
                         << obj_list[obj_ptr->OBJ_NUM].getCurInGame()
                         << " object_num:  " << obj_ptr->OBJ_NUM << endl;
               }
	       obj_list[obj_ptr->OBJ_NUM].setCurInGame(0);
	    }//if
            if (obj_ptr->ob->in_list) {
               delete obj_ptr;
            }//if is a SOBJ
         }//else
      }//else
   }//else, not junk all.sumpin
}//do_junk


void tell(int i_th, const String* targ, const char* msg, critter& pc) {
   critter* crit_ptr;
   int count = 0, z;

   if (i_th > 100) {
      show("I won't search above 100, too many cycles.\n", pc);
   }//if

   if (pc.POS > POS_REST) {
     show("You are too relaxed.\n", pc);
     return;
   }//if

   if (pc.isGagged()) {
     show("You have been gagged.\n", pc);
     return;
   }//if

   if (i_th == 1) {
      crit_ptr = have_crit_named(pc_list, i_th, targ,
                                 pc.SEE_BIT, ROOM);

      if (crit_ptr) {
         do_tell(pc, msg, *crit_ptr, TRUE, crit_ptr->getCurRoomNum());
         return;
      }//if
   }//if

   // Only can tell mobs in your own zone
   int begin = ZoneCollection::instance().elementAt(pc.getCurZoneNum()).getBeginRoomNum();
   int end = ZoneCollection::instance().elementAt(pc.getCurZoneNum()).getEndRoomNum();
   
   for (int i = begin; i<=end; i++) {
      z = 1;
      if (room_list.elementAtNoCreate(i)) {
         while ((crit_ptr = room_list[i].haveCritNamed(z, targ, pc.SEE_BIT))) {
            count++;
            if (count == i_th) {
               do_tell(pc, msg, *crit_ptr, TRUE, i);
               return;
            }
            z++;
         }//while   
      }//if
   }//for

   pc.show("That person is not around!\n");
}//tell


void who(critter& pc) {
   Cell<critter*> cll(pc_list);
   critter* ptr;
   String buf(100);

   mudlog.log(TRC, "In who..\n");
   if (!pc.pc)
      return;

   show("These people are actively playing:\n", pc);

   while ((ptr = cll.next())) {
      if ((!detect(pc.SEE_BIT, ptr->VIS_BIT)) &&
          (ptr->isImmort())) {
         continue;
      }//if

      if ((ptr->getMode() ==  MODE_LOGGING_IN) && (!pc.isImmort())) {
         continue;
      }

      String class_str;
      String lvl_str;

      int can_detect = FALSE;
      if (ptr->isImmort()) {
         if (pc.isImmort() && (pc.getImmLevel() >= ptr->getImmLevel())) {
            can_detect = TRUE;
         }
         else {
            can_detect = FALSE;
         }
      }//
      else if (pc.isImmort()) {
         can_detect = TRUE;
      }
      else {
         //neither are immorts
         can_detect = FALSE;
      }

      if (ptr->isCloaked() && !can_detect) {
         class_str = "Cloaked";
         lvl_str = "??";
      }
      else {
         class_str = get_class_name(ptr->CLASS);
         if (ptr->isImmort()) {
            Sprintf(lvl_str, "IMM-%i", ptr->getImmLevel());
         }
         else {
            lvl_str = ptr->LEVEL;
         }
      }//else

      Sprintf(buf, "%S%P11 %S(%S)%P25 %S\n",
              name_of_crit(*ptr, pc.SEE_BIT),
              &class_str, &lvl_str, &(ptr->short_desc));
      show(buf, pc);
   }//while
   Sprintf(buf, "\nThere are currently %i people actively playing.\n",
           pc_list.size());
   pc.show(buf);
   mudlog.log(TRC, "Done with who.\n");
}//who


void sockets(critter& pc) {
   Cell<critter*> cll(pc_list);
   critter* ptr;
   String buf(100);

   mudlog.log(TRC, "In sockets..\n");
   if (!pc.pc || !pc.pc->imm_data) {
      show("Eh??\n", pc);
      return;
   }//if

   show("Verbose list of players:\n", pc);

   while ((ptr = cll.next())) {
      Sprintf(buf, "%S(%i)%P25%S\n\t%S\n", name_of_crit(*ptr, pc.SEE_BIT),
              ptr->LEVEL, &(ptr->short_desc),
              &(ptr->pc->host));
      show(buf, pc);
   }//while
}//sockets


void ldwho(critter& pc) {
   Cell<critter*> cll(linkdead_list);
   critter* ptr;
   String buf(100);

   if (!pc.pc || !pc.pc->imm_data) {
      show("Eh??\n", pc);
      return;
   }//if

   show("These people are link dead.\n", pc);

   while ((ptr = cll.next())) {
      Sprintf(buf, "%S%P20%s(%i)%P35%S\n", name_of_crit(*ptr, ~0),
              get_class_name(ptr->CLASS), ptr->LEVEL, 
              &(ptr->short_desc));
      show(buf, pc);
   }//while
}//ldwho




