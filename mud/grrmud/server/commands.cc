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

//*********************** commands.cc ********************//
//       Contains functions called by process_input.  //

#include "rm_spll.h"
#include "dam_spll.h"
#include "ez_spll.h"
#include "trv_spll.h"
#include <iostream.h>
#include <fstream.h>
#include "commands.h"
#include "command2.h"
#include "command5.h"
#include "misc.h"
#include "misc2.h"
#include "battle.h"
#include <stdio.h>
#include "spec_prc.h"
#include "batl_prc.h"
#include "classes.h"
#include "spells.h"
#include "spells2.h"
#include "skills.h"
#include "pet_spll.h"
#include <PtrArray.h>
#include "load_wld.h"


int inventory(critter& pc) {
   String buf(100);
   
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In inventory, size of inv:  " << pc.inv.size()
             << endl;
   }//if

   show("Your inventory:  \n", pc);
   out_inv(pc.inv, pc, CRIT_INV);
   return 0;
}//inventory


int drop(int i_th, const String* pre_obj, const String* obj_all,
          critter& pc) {
   object *obj_ptr;
   Cell<object*> cell(pc.inv);
   String obj(25);
   short flag1;

   mudlog.log(TRC, "In drop.\n");

   if (!ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if ((pre_obj->Strlen() == 0) && (obj_all->Strlen() == 0)) {
      pc.show("Drop what??\n");
      return -1;
   }

                   /* adjust for: drop all.sword
         which will be sent in as: drop all sword by the parser. */
                    /* this is a hack of sorts btw */

   if (strcasecmp(*pre_obj, "all") == 0) {
      i_th = -1; //signifies "all"
      obj = *obj_all;
   }//if    
   else {
      obj = *pre_obj;
   }//else

   flag1 = FALSE; //no msg..
   if (strncasecmp(obj, "all", obj.Strlen()) == 0) { 
      mudlog.log(DBG, "drop called as: drop all");
      obj_ptr = cell.next();
      while (obj_ptr) {
         if (detect(pc.SEE_BIT, (obj_ptr->OBJ_VIS_BIT | 
                                 ROOM.getVisBit()))) {
            flag1 = TRUE;
            if (obj_drop_by(*obj_ptr, pc)) { 
               ROOM.gainInv(obj_ptr);
               drop_eq_effects(*obj_ptr, pc, TRUE);
               obj_ptr = pc.inv.lose(cell);  //deletes and increments
            }//if
            else 
               obj_ptr = cell.next();
         }// if detect
         else 
            obj_ptr = cell.next();
      }//while obj_ptr
      if (!flag1) {
         show("Ok.\n", pc);
      }//if
   }//if "all"
   else if (i_th == -1) { //like drop all.sword
      mudlog.log(DBG, "drop called as: drop all.sword\n");
      pc.inv.head(cell);
      obj_ptr = cell.next();

      while (obj_ptr) {
         if ((detect(pc.SEE_BIT, (obj_ptr->OBJ_VIS_BIT | 
                                  ROOM.getVisBit()))) && 
             (obj_is_named(*obj_ptr, obj))) {
            if (obj_drop_by(*obj_ptr, pc)) { 
               ROOM.gainInv(obj_ptr);
               drop_eq_effects(*obj_ptr, pc, TRUE);
               obj_ptr = pc.inv.lose(cell);
            }//else
            else 
               obj_ptr = cell.next();
         }// if detect
         else
            obj_ptr = cell.next();
      }//while obj_ptr
   }// if drop all.waybread
   else if ((obj_ptr = have_obj_named(pc.inv, i_th, &obj, pc.SEE_BIT, 
            ROOM))) {
      mudlog.log(DBG, "drop called as: drop sword or drop 2.sword\n");
      if (obj_drop_by(*obj_ptr, pc)) {
         pc.loseInv(obj_ptr);
         ROOM.gainInv(obj_ptr);
         drop_eq_effects(*obj_ptr, pc, TRUE);
      }//if
   }//if   
   else {
         show("You don't seem to have that.\n", pc);
         return -1;
   }//else
   return 0;
}//drop


int donate(int i_th, const String* pre_obj, const String* obj_all,
          critter& pc) {
   object *obj_ptr;
   Cell<object*> cell(pc.inv);
   String obj(25);
   short flag1;

   mudlog.log(DBG, "In donate.\n");

   if (!ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if ((pre_obj->Strlen() == 0) && (obj_all->Strlen() == 0)) {
      pc.show("Donate what??\n");
      return -1;
   }

                   /* adjust for: donate all.sword
         which will be sent in as: donate all sword by the parser. */
                    /* this is a hack of sorts btw */

   if (strcasecmp(*pre_obj, "all") == 0) {
      i_th = -1; //signifies "all"
      obj = *obj_all;
   }//if    
   else {
      obj = *pre_obj;
   }//else

   flag1 = FALSE; //no msg..
   if (strncasecmp(obj, "all", obj.Strlen()) == 0) { 
      mudlog.log(DBG, "donate called as: donate all");
      obj_ptr = cell.next();
      while (obj_ptr) {
         if (detect(pc.SEE_BIT, (obj_ptr->OBJ_VIS_BIT | 
                                 ROOM.getVisBit()))) {
            flag1 = TRUE;
            if (obj_drop_by(*obj_ptr, pc)) { 
               room_list[DONATE_ROOM].gainInv(obj_ptr);
               donate_eq_effects(*obj_ptr, pc, TRUE);
               obj_ptr = pc.inv.lose(cell);  //deletes and increments
            }//if
            else 
               obj_ptr = cell.next();
         }// if detect
         else 
            obj_ptr = cell.next();
      }//while obj_ptr
      if (!flag1) {
         show("Ok.\n", pc);
      }//if
   }//if "all"
   else if (i_th == -1) { //like donate all.sword
      mudlog.log(DBG, "donate called as: donate all.sword\n");
      pc.inv.head(cell);
      obj_ptr = cell.next();

      while (obj_ptr) {
         if ((detect(pc.SEE_BIT, (obj_ptr->OBJ_VIS_BIT | 
                                  ROOM.getVisBit()))) && 
             (obj_is_named(*obj_ptr, obj))) {
            if (obj_drop_by(*obj_ptr, pc)) { 
               room_list[DONATE_ROOM].gainInv(obj_ptr);
               donate_eq_effects(*obj_ptr, pc, TRUE);
               obj_ptr = pc.inv.lose(cell);
            }//else
            else 
               obj_ptr = cell.next();
         }// if detect
         else
            obj_ptr = cell.next();
      }//while obj_ptr
   }// if drop all.waybread
   else if ((obj_ptr = have_obj_named(pc.inv, i_th, &obj, pc.SEE_BIT, 
                                      ROOM))) {
      mudlog.log(DBG, "donate called as: donate sword or drop 2.sword\n");
      if (obj_drop_by(*obj_ptr, pc)) {
         pc.loseInv(obj_ptr);
         room_list[DONATE_ROOM].gainInv(obj_ptr);
         donate_eq_effects(*obj_ptr, pc, TRUE);
      }//if
   }//if   
   else {
      show("You don't seem to have that.\n", pc);
      return -1;
   }//else
   return 0;
}//donate


int show_eq(critter& pc) {
   String posn(100);

   mudlog.log(DBG, "In eq.\n");

   show("You are wearing:  \n", pc);

   out_eq(pc, pc);
   pc.show("\n");
   return 0;
}//eq


int examine(int i_th, const String* obj, critter& pc) {
   String buf(100);

   if (ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
   
      object* obj_ptr = have_obj_named(pc.inv, i_th, obj, pc.SEE_BIT, 
                                       ROOM);

      if (!obj_ptr) {
         obj_ptr = ROOM.haveObjNamed(i_th, obj, pc.SEE_BIT);
      }//if
      if (!obj_ptr) {
         Sprintf(buf, "You can't find the %S.\n", obj);
         show(buf, pc);
      }//if
                   //is a container?
      else if (!obj_ptr->OBJ_FLAGS.get(54) || !obj_ptr->ob->bag) {
         show("That is not a container.\n", pc);
      }//if
      else if (obj_ptr->isClosed()) {
         show("Its closed!\n", pc);
      }//if
      else {
         Sprintf(buf, "%S contains: \n\n", &(obj_ptr->ob->short_desc));
         buf.Cap();
         show(buf, pc);
         
         out_inv(obj_ptr->ob->inv, pc, OBJ_INV);
         
         String cmd = "examine";
         ROOM.checkForProc(cmd, NULL_STRING, pc, obj_ptr->OBJ_NUM);
      }//else
      return 0;
   }
   return -1;
}//examine



int wear(int i_th, const String* obj, int j, const String* posn,
          critter &pc) {
   Cell<object*> cell;
   object *obj_ptr;
   int i = 0, t = 0;

   mudlog.log(DBG, "In wear.\n");

   if (!ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!obj->Strlen()) {
      show("Wear what??\n", pc);
      mudlog.log(DBG, "Wear failed, object not specified.\n");
   }//if
   else if ((strncasecmp(*obj, "all", obj->Strlen())) == 0) {
      mudlog.log(DBG, "Wear called like 'wear all'\n");
      pc.inv.head(cell);
      obj_ptr = cell.next();
    
      if (IsEmpty(pc.inv)) {
         show("You have nothing to wear!\n", pc);
      }//if
      else {
         int retval = -1;
         while (obj_ptr) {
            if (don_obj(*obj_ptr, pc) >= 0) { //takes care of all changes...
               obj_ptr = pc.inv.lose(cell);
               retval = 0;
            }//if
            else
               obj_ptr = cell.next();
         }//while
         return retval;
      }//else
   }//if
   else if (!posn->Strlen()) { //no posn noted
      mudlog.log(DBG, "Wear, no posn noted.\n");
      obj_ptr = have_obj_named(pc.inv, i_th, obj, pc.SEE_BIT, ROOM);
      if (!obj_ptr) {
         show("You don't have that item.\n", pc);
         mudlog.log(DBG, "Wear failed, don't have item.\n");
      }//if
      else {
         if (don_obj(*obj_ptr, pc) >= 0) { 
            pc.loseInv(obj_ptr);
            return 0;
         }
      }//else
   }//if
   else { //posn was noted
      mudlog.log(DBG, "Wear, posn noted.\n");
      obj_ptr = have_obj_named(pc.inv, i_th, obj, pc.SEE_BIT, ROOM);
      if (!obj_ptr) {
         show("You don't have that item.\n", pc);
         mudlog.log(DBG, "Wear failed, don't have object\n");
         return -1;
      }//if
      else {
         i = t = 0;
         short len = posn->Strlen();
         if (strncasecmp(*posn, "head", len) == 0) {
            i = 1;
         }//if
         else if (strncasecmp(*posn, "neck", len) == 0) {
            i = 2;
            t = 3;
         }//if
         else if (strncasecmp(*posn, "about body", len) == 0) {
            i = 4;
         }//if
         else if (strncasecmp(*posn, "arms", len) == 0) {
            i = 5;
         }//if
         else if (strncasecmp(*posn, "wrist", len) == 0) {
            i = 6;
            t = 7;
         }//if
         else if (strncasecmp(*posn, "hands", len) == 0) {
            i = 8;
         }//if
         else if (strncasecmp(*posn, "wielded", len) == 0) {
            i = 9;
         }//if
         else if (strncasecmp(*posn, "hold", len) == 0) {
            i = 10;
         }//if
         else if (strncasecmp(*posn, "light", len) == 0) {
            i = 11;
         }//if
         else if (strncasecmp(*posn, "body", len) == 0) {
            i = 12;
         }//if
         else if (strncasecmp(*posn, "waist", len) == 0) {
            i = 13;
         }//if
         else if (strncasecmp(*posn, "legs", len) == 0) {
            i = 14;
         }//if
         else if (strncasecmp(*posn, "feet", len) == 0) {
            i = 15;
         }//if
         else if (strncasecmp(*posn, "finger", len) == 0) {
            i = 16;
            t = 17;
         }//if
         else if (strncasecmp(*posn, "shield", len) == 0) {
            i = 18;
         }//if
         //else if (strncasecmp(*posn, "ears", len) == 0) {
         //   i = 19;
         //}//if
      }//else
      if (!i) { //not a valid key_word for posn
         show("Wear it where??\n", pc);
         mudlog.log(DBG, "Wear failed, posn invalid.\n");
         return -1;
      }//if
      else { //i and maybe t are good
         if (pc.EQ[i] || (j == 2)) { //gotta go in posn t if anywhere
            if (!pc.EQ[t]) { //then can go there
               if (obj_wear_by(*obj_ptr, pc, t, TRUE)) {
                  pc.loseInv(obj_ptr);
                  pc.EQ[t] = obj_ptr;
                  wear_eq_effects(*obj_ptr, pc, t, TRUE); 
                  return 0;
               }//if
            }//if
            else { //can go in neither posn
               show("You are already using something there.\n", pc);
               mudlog.log(DBG, 
                          "Wear failed, already wearing something there.\n");
            }//else
         }//if 
         else { //put it in spot i
            mudlog.log(DBG, "wear called as:  wear ring finger.\n");
            if (obj_wear_by(*obj_ptr, pc, i, TRUE)) {
               pc.loseInv(obj_ptr);
               pc.EQ[i] = obj_ptr;
               wear_eq_effects(*obj_ptr, pc, i, TRUE); 
               return 0;
            }//if
         }//else, put it in posn i
      }//else, i and MAYBE t are good
   }//else, posn was noted
   return -1;
}//wear


int remove(int i_th, const String* obj, critter &pc) {
   int i = 0;
   int found_one = FALSE;
   int retval = -1;

   if (!ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!obj->Strlen()) {
      show("Remove what??\n", pc);
   }//if
   else if (strcasecmp(*obj, "all") == 0) {
      mudlog.log(DBG, "remove called like: remove all\n");
      for (i = 1; i<MAX_EQ; i++) {
         if (pc.EQ[i]) {
            if (obj_remove_by(*(pc.EQ[i]), pc)) { //checks for everything
	       found_one = TRUE;
               pc.gainInv(pc.EQ[i]);
               remove_eq_effects(*(pc.EQ[i]), pc, FALSE, TRUE, i);
               pc.EQ[i] = NULL;// remove it from eq list
               retval = 0;
            }//if ok to remove
         }//if wearing an item there
      }//for i loop
   }//if obj == "all"
   else { // must be:   remove 2.sword or remove sword
      int count = 0;
      mudlog.log(DBG, "remove called like: remove sword or remove 2.ring\n");
      if (i_th > 0) { //remove 2.ring
         for (i = 1; i<MAX_EQ; i++) {
            if (pc.EQ[i]) { //wearing there
               if (obj_is_named(*(pc.EQ[i]), *obj)) { //got one :)
                  count++;
                  if (count == i_th) { // found it!!
                     if (obj_remove_by(*(pc.EQ[i]), pc)) { 
			found_one = TRUE;
                        pc.gainInv(pc.EQ[i]);
                        remove_eq_effects(*(pc.EQ[i]), pc, FALSE, TRUE,
					  i);
                        pc.EQ[i] = NULL;
                        return 0;
                     }//if ok to remove
                  }//if count is right
               }//obj is named right
            }//if wearing something there
         }//for loop through eq
      }//if i_th is good
   }//else
   if (!found_one) {
      show("You don't seem to be wearing that.\n", pc);
   }//if
   return retval;
}//remove


      
int look(int i_th, const String* obj, critter &pc) {

   mudlog.log(DBG, "In look.\n");
   
   return do_look(i_th, obj, pc, ROOM);
}


int do_look(int i_th, const String* obj, critter& pc, room& rm) {
   critter* crit_ptr;
   object* obj_ptr;
   door* door_ptr;
   KeywordPair* kwd_ptr = NULL;
   String buf(100);
   short flag = FALSE;



//    int cnt;
//    if ( !pc.isMob() && ((cnt = rm.CRITTERS.dataCount(&pc)) > 1)) {
//       if (mudlog.ofLevel(ERR)) {
//          mudlog << "ERROR:  more than one pointer to the same critter:  "
//                 << cnt << " room:  " << pc.getCurRoomNum() << endl;
//         mudlog << "ERROR:  Name of crit:  " << name_of_crit(pc, ~0) << endl;

//          mudlog << "Recovering as best we can...." << endl;
//       }
//       rm.CRITTERS.loseData(&pc);
//    }//if

   if (pc.POS > POS_REST) {
      show("\nYou aren't in a lively enough position to look something.\n", 
            pc);
   }//if
            /* dark and cannot see in dark? */
   else if (!detect(pc.SEE_BIT, rm.getVisBit())) { 
      show("All you see is darkness...\n", pc);
   }//if
   else if (pc.POS >= POS_DEAD) { //position
      show("\nYou should not be able to see at all!\n", pc);      
   }//if

       /* just plain look? */
   else if (obj->Strlen() == 0) {
      show("\n", pc);
      if (pc.shouldShowVnums()) {
         Sprintf(buf, "[%i]   ", pc.getCurRoomNum());
         show(buf, pc);
      }//if

      if (pc.isUsingClient()) {
         if (pc.shouldShowVnums()) {
            Sprintf(buf, "<RM_SHORT_DESC %i>", pc.getCurRoomNum());
            show(buf, pc);
         }
         else {
            pc.show("<RM_SHORT_DESC 0>");
         }
      }

      show(rm.short_desc, pc);
      if (pc.USING_CLIENT) 
         show("</RM_SHORT_DESC>\n", pc);
      else
         show("\n", pc);
      
      if (!pc.IS_BRIEF) { //isn't brief
         if (pc.USING_CLIENT) 
            show("<RM_DESC>", pc);
         show(rm.long_desc, pc);
         if (pc.USING_CLIENT) 
            show("</RM_DESC>\n", pc);
         else
            show("\n", pc);
      }//else
      if (pc.shouldDoAutoExits()) { //if autoexit set
         auto_exit(pc);
      }//if

      rm.showCritters(pc);

      show("\n", pc);
      rm.outInv(pc);
      show("\n", pc);
   }//if
   else {                        /* look <thing> */
      obj_ptr = rm.haveObjNamed(i_th, obj, pc.SEE_BIT);
      if (!obj_ptr) {  //check in pc's inventory then
         obj_ptr = have_obj_named(pc.inv, i_th, obj, pc.SEE_BIT, rm);
      }//if
      if (!obj_ptr) {
         int jnk_posn = 0; //not used, but have to have it.
         obj_ptr = pc.findObjInEq(i_th, *obj, pc.SEE_BIT, rm, jnk_posn);
      }
      if (obj_ptr) { //so it was in rooms inv, or pc's inv
         show("\n\n", pc);
         show((obj_ptr->ob->long_desc), pc);
         show("\n", pc);

         String cmd = "look";
         rm.checkForProc(cmd, NULL_STRING, pc, obj_ptr->OBJ_NUM);
      }//if
      else if ((crit_ptr = rm.haveCritNamed(i_th, obj, pc.SEE_BIT))) {

         show("\n\n", pc);
         show((crit_ptr->long_desc), pc);

         int cond = (int) (((float)crit_ptr->HP / (float)crit_ptr->HP_MAX) * 
                          9.0);
         if (!check_l_range(cond, 0, 9, pc, FALSE)) {
            if (mudlog.ofLevel(ERR)) {
               mudlog << "ERROR:  cond out of range in look, here is mob: "
                      << *(crit_ptr->getName()) << " posn: " << cond << endl;
            }
	 }//if
	 else {
            Sprintf(buf, "\n%s %s\n", get_he_she(*crit_ptr), 
                    critter_conditions[cond]);
            buf.Cap();
            show(buf, pc);
         }//else

	 int sanct_num = SANCTUARY_SKILL_NUM;

         Cell<stat_spell_cell*> cll(crit_ptr->affected_by);
         stat_spell_cell* sp;
         while ((sp = cll.next())) {
            if (sp->stat_spell == sanct_num) {
               Sprintf(buf, "\t%s glows brightly.\n\n", 
                       get_he_she(*crit_ptr));
	       buf.Cap();
               show(buf, pc);
            }//if
         }//while

         for (int q = 1; q < MAX_EQ; q++) {
            if (crit_ptr->EQ[q]) {
               flag = TRUE;
               break;
            }//if
         }//for

         if (flag) {
            out_eq(*crit_ptr, pc);
            show("\n\n", pc);
         }//if

         if (pc.pc && pc.shouldSeeInventory()) {
            out_inv(crit_ptr->inv, pc, CRIT_INV);
         }//if

         String cmd = "look";
         rm.checkForProc(cmd, NULL_STRING, pc, crit_ptr->MOB_NUM);
      }//if
      else if ((door_ptr = 
           door::findDoor(rm.doors, i_th, obj, pc.SEE_BIT, rm))) {
         show("\n\n", pc);
         show(door_ptr->dr_data->long_desc, pc);
         show("\n", pc);

         String cmd = "look";
         rm.checkForProc(cmd, NULL_STRING, pc, door_ptr->dr_data->door_num);
      }//if
      else if ((kwd_ptr = rm.haveKeyword(i_th, obj))) {
         pc.show("\n\n");
         pc.show(kwd_ptr->desc);
         pc.show("\n");
      }
      else if (strncasecmp(*obj, "sky", obj->Strlen()) == 0) {
         if (ROOM.canSeeSky()) {
            pc.show("You look off into the great blue yonder!\n");
         }
         else {
            pc.show("You can't see the sky from here!\n");
         }
      }//if
      else {
         show(NO_CAN_SEE_MSG, pc);
      }//else
   }//else, look <thingie>
   return 0; //no real thought put into this at this time..
}//do_look



/* hit is generic battle starter..same as kill */
int hit(int i_th, const String* victim, critter &pc) {
   critter* crit_ptr;
   String buf(100);

   crit_ptr = ROOM.haveCritNamed(i_th, victim, pc.SEE_BIT);

   if (!crit_ptr) {
      show("Fight who??\n", pc);
      return -1;
   }//if

   if (crit_ptr->isMob()) {
      crit_ptr = mob_to_smob(*crit_ptr,  pc.getCurRoomNum(), TRUE, i_th,
                             victim, pc.SEE_BIT);
   }//if
      
   if (!ok_to_do_action(crit_ptr, "mSVPF", -1, pc)) {
      return -1;
   }//if

   if (!(crit_ptr = check_for_diversions(*crit_ptr, "GSM", pc)))
      return -1;

   return do_hit(*crit_ptr, pc);
}//hit


/* hit is generic battle starter..same as kill */
int assist(int i_th, const String* targ, critter &pc) {
   critter* crit_ptr;
   critter* friendly;

   String buf(100);

   friendly = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);

   if (!ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!friendly) {
     show("Assist who??\n", pc);
     return -1;
   }//if

   if (friendly == &pc) {
      pc.show("You're already giving it your all!\n");
      return -1;
   }

   if (friendly->isFighting()) {
      crit_ptr = friendly->getFirstFighting();

      if (!(crit_ptr = check_for_diversions(*crit_ptr, "GSM", pc)))
         return -1;

      if (crit_ptr == &pc) {
         pc.show("You start giving yourself a hard time!\n");
         return -1;
      }

      Sprintf(buf, "You assist %S in fighting against %S!\n",
              friendly->getName(), crit_ptr->getName(pc.SEE_BIT));
      pc.show(buf);

      return do_hit(*crit_ptr, pc);
   }//if
   else {
      pc.show("You cannot assist someone who is not fighting.\n");
   }
   return -1;
}//hit


int do_hit(critter& vict, critter& pc) {
  if (&vict == &pc) {
     show("You rare back and smack the s**t out of yourself!\n", pc);
     return -1;
  }//if

  join_in_battle(pc, vict);   
  do_battle_round(pc, vict, 9);   

  String cmd = "hit";
  ROOM.checkForProc(cmd, NULL_STRING, pc, vict.MOB_NUM);
  show("Battle envelops you!\n", pc);
  return 0;
}//do_hit


int cast(const String* spell, int j_th, const String* victim, critter &pc) {
   int len = spell->Strlen();

   if (mudlog.ofLevel(TRC)) {
      mudlog << "In cast, spell:  " << *spell << "  j:  " << j_th
             << "  vict:  " << *victim << endl;
   }

   if (!ok_to_do_action(NULL, "mFPN", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (strncasecmp(*spell, "absorb blows", len) == 0) 
      cast_absorb_blows(pc);
   else if (strncasecmp(*spell, "armor", len) == 0) 
      cast_armor(j_th, victim, pc);
   else if (strncasecmp(*spell, "bind wound", len) == 0) 
      cast_bind_wound(j_th, victim,pc);
   else if (strncasecmp(*spell, "blindness", len) == 0) 
      cast_blindness(j_th, victim, pc);
   else if (strncasecmp(*spell, "bless", len) == 0) 
      cast_bless(j_th, victim, pc);
   else if (strncasecmp(*spell, "burning hands", len) == 0) 
      cast_burning_hands(j_th, victim, pc);
   else if (strncasecmp(*spell, "calm", len) == 0) 
      cast_calm(pc);
   else if (strncasecmp(*spell, "cause critical", len) == 0) 
      cast_cause_critical(j_th, victim, pc);
   else if (strncasecmp(*spell, "cause sickness", len) == 0) 
      cast_cause_sickness(j_th, victim, pc);
   else if (strncasecmp(*spell, "charm", len) == 0) 
      cast_charm(j_th, victim, pc);
   else if (strncasecmp(*spell, "conjure horde", len) == 0) 
      cast_conjure_horde(pc);
   else if (strncasecmp(*spell, "conjure minion", len) == 0) 
      cast_conjure_minion(pc);
   else if (strncasecmp(*spell, "create food", len) == 0) 
      cast_create_food(pc);
   else if (strncasecmp(*spell, "create golem", len) == 0) 
      cast_create_golem(pc);
   else if (strncasecmp(*spell, "create light", len) == 0) 
      cast_create_light(pc);
   else if (strncasecmp(*spell, "create water", len) == 0) 
      cast_create_water(j_th, victim, pc);
   else if (strncasecmp(*spell, "cure blindness", len) == 0) 
      cast_cure_blind(j_th, victim, pc);
   else if (strncasecmp(*spell, "cure critical", len) == 0) 
      cast_cure_critical(j_th, victim, pc);
   else if (strncasecmp(*spell, "cure serious", len) == 0) 
      cast_cure_serious(j_th, victim, pc);
   else if (strncasecmp(*spell, "dart of darkness", len) == 0) 
      cast_dark_dart(j_th, victim, pc);
   else if (strncasecmp(*spell, "detect alignment", len) == 0) 
      cast_detect_alignment(j_th, victim, pc);
   else if (strncasecmp(*spell, "detect hidden", len) == 0) 
      cast_detect_hidden(j_th, victim, pc);
   else if (strncasecmp(*spell, "detect invisibility", len) == 0) 
      cast_detect_invisibility(j_th, victim, pc);
   else if (strncasecmp(*spell, "detect magic", len) == 0) 
      cast_detect_magic(j_th, victim, pc);
   else if (strncasecmp(*spell, "detect poison", len) == 0) 
      cast_detect_poison(j_th, victim, pc);
   else if (strncasecmp(*spell, "dispell evil", len) == 0) 
      cast_dispell_evil(j_th, victim, pc);
   else if (strncasecmp(*spell, "dispell good", len) == 0) 
      cast_dispell_good(j_th, victim, pc);
   else if (strncasecmp(*spell, "dispell magic", len) == 0) 
      cast_dispell_magic(j_th, victim, pc);
   else if (strncasecmp(*spell, "distortion wall", len) == 0) 
      cast_distortion_wall(j_th, victim, pc);
   else if (strncasecmp(*spell, "divine protection", len) == 0) 
      cast_divine_protection(j_th, victim, pc);
   else if (strncasecmp(*spell, "enchant armor", len) == 0) 
      cast_enchant_armor(j_th, victim, pc);
   else if (strncasecmp(*spell, "enchant weapon", len) == 0) 
      cast_enchant_weapon(j_th, victim, pc);
   else if (strncasecmp(*spell, "entangle", len) == 0) 
      cast_entangle(j_th, victim, pc);
   else if (strncasecmp(*spell, "faerie fire", len) == 0) 
      cast_faerie_fire(j_th, victim, pc);
   else if (strncasecmp(*spell, "fireball", len) == 0) 
      cast_fireball(j_th, victim, pc);
   else if (strncasecmp(*spell, "fire blade", len) == 0) 
      cast_fire_blade(j_th, victim, pc);
   else if (strncasecmp(*spell, "fireproof", len) == 0) 
      cast_fireproof(j_th, victim, pc);
   else if (strncasecmp(*spell, "firestorm", len) == 0) 
      cast_firestorm(pc);
   else if (strncasecmp(*spell, "firewall", len) == 0) 
      cast_firewall(pc);
   else if (strncasecmp(*spell, "flame strike", len) == 0) 
      cast_flame_strike(pc);
   else if (strncasecmp(*spell, "fly", len) == 0) 
      cast_fly(j_th, victim, pc);
   else if (strncasecmp(*spell, "frost blade", len) == 0) 
      cast_frost_blade(j_th, victim, pc);
   else if (strncasecmp(*spell, "gate", len) == 0) 
      cast_gate(j_th, victim, pc);
   else if (strncasecmp(*spell, "group heal", len) == 0) 
      cast_group_heal(pc);
   else if (strncasecmp(*spell, "haste", len) == 0) 
      cast_haste(j_th, victim, pc);
   else if (strncasecmp(*spell, "heal", len) == 0) 
      cast_heal(j_th, victim, pc);
   else if (strncasecmp(*spell, "heros feast", len) == 0) 
      cast_heros_feast(pc);
   else if (strncasecmp(*spell, "holy word", len) == 0) 
      cast_holy_word(j_th, victim, pc);
   else if (strncasecmp(*spell, "icestorm", len) == 0) 
      cast_icestorm(pc);
   else if (strncasecmp(*spell, "identify", len) == 0) 
      cast_identify(j_th, victim,pc);
   else if (strncasecmp(*spell, "illuminate", len) == 0) 
      cast_illuminate(pc);
   else if (strncasecmp(*spell, "illusion", len) == 0) 
      cast_illusion(pc);
   else if (strncasecmp(*spell, "invisibility", len) == 0) 
      cast_invisibility(j_th, victim, pc);
   else if (strncasecmp(*spell, "lightning bolt", len) == 0) 
      cast_lightning(j_th, victim, pc);
   else if (strncasecmp(*spell, "lightning storm", len) == 0) 
      cast_lightning_storm(pc);
   else if (strncasecmp(*spell, "mass charm", len) == 0) 
      cast_mass_charm(pc);
   else if (strncasecmp(*spell, "magic shield", len) == 0) 
      cast_magic_shield(j_th, victim, pc);
   else if (strncasecmp(*spell, "meteorstorm", len) == 0) 
      cast_meteorstorm(pc);
   else if (strncasecmp(*spell, "mirror image", len) == 0) 
      cast_mirror_image(pc);
   else if (strncasecmp(*spell, "passdoor", len) == 0) 
      cast_passdoor(j_th, victim, pc);
   else if (strncasecmp(*spell, "poison", len) == 0) 
      cast_poison(j_th, victim, pc);
   else if (strncasecmp(*spell, "portal", len) == 0) 
      cast_portal(j_th, victim, pc);
   else if (strncasecmp(*spell, "prismatic globe", len) == 0) 
      cast_prismatic_globe(j_th, victim, pc);
   else if (strncasecmp(*spell, "protetion from evil", len) == 0) 
      cast_pfe(pc);
   else if (strncasecmp(*spell, "protection from good", len) == 0) 
      cast_pfg(pc);
   else if (strncasecmp(*spell, "quake", len) == 0) 
      cast_quake(pc);
   else if (strncasecmp(*spell, "rainbow", len) == 0) 
      cast_rainbow(j_th, victim, pc);
   else if (strncasecmp(*spell, "raise undead", len) == 0) 
      cast_raise_undead(pc);
   else if (strncasecmp(*spell, "recall", len) == 0) 
      cast_recall(j_th, victim, pc);
   else if (strncasecmp(*spell, "recharge", len) == 0) 
      cast_recharge(j_th, victim, pc);
   else if (strncasecmp(*spell, "remove curse", len) == 0) 
      cast_remove_curse(j_th, victim, pc);
   else if (strncasecmp(*spell, "remove poison", len) == 0) 
      cast_remove_poison(j_th, victim, pc);
   else if (strncasecmp(*spell, "restore", len) == 0) 
      cast_restore(j_th, victim, pc);
   else if (strncasecmp(*spell, "rune edge", len) == 0) 
      cast_rune_edge(j_th, victim, pc);
   else if (strncasecmp(*spell, "sanctuary", len) == 0) 
      cast_sanctuary(j_th, victim, pc);
   else if (strncasecmp(*spell, "shadows blessing", len) == 0) 
      cast_shadows_blessing(j_th, victim, pc);
   else if (strncasecmp(*spell, "shocking grasp", len) == 0) 
      cast_shocking_grasp(j_th, victim, pc);
   else if (strncasecmp(*spell, "sleep", len) == 0) 
      cast_sleep(j_th, victim, pc);
   else if (strncasecmp(*spell, "stone skin", len) == 0) 
      cast_stone_skin(j_th, victim, pc);
   else if (strncasecmp(*spell, "summon", len) == 0) 
      cast_summon(j_th, victim, pc);
   else if (strncasecmp(*spell, "strength", len) == 0) 
      cast_strength(j_th, victim, pc);
   else if (strncasecmp(*spell, "teleport", len) == 0) 
      cast_teleport(pc);
   else if (strncasecmp(*spell, "tornado", len) == 0) 
      cast_tornado(pc);
   else if (strncasecmp(*spell, "typhoon", len) == 0) 
      cast_typhoon(pc);
   else if (strncasecmp(*spell, "weaken", len) == 0) 
      cast_weaken(j_th, victim, pc);
   else if (strncasecmp(*spell, "web", len) == 0) 
      cast_web(j_th, victim, pc);
   else {
      show("That spell is in need of research.\n", pc);
      return -1;
   }//else
   return 0;
}//pc_cast()


int put(int i, const String* item, int j, const String* bag, 
         critter& pc) {      
   String buf(100);
   Cell<object*> cell;
   object* bag_ptr, *tmp;
   object* vict_ptr;
   short item_in_inv = TRUE, bag_in_inv = TRUE, found_it = FALSE;

   mudlog.log(DBG, "In put\n");

   if (!ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (i == -1) { //as in: "put all.waybread bag"
      mudlog.log(DBG, 
                 "called put with something like: put all.waybread bag\n");

      bag_ptr = have_obj_named(pc.inv, j, bag, pc.SEE_BIT, ROOM);
      bag_in_inv = TRUE;

      if (!bag_ptr) {
         bag_in_inv = FALSE;
         bag_ptr = ROOM.haveObjNamed(j, bag, pc.SEE_BIT);
      }//if
      if (!bag_ptr) {
         show("You don't seem to have that container.\n", pc);
         return -1;
      }//if
      else if (!bag_ptr->ob->bag) {
         show("How are you gonna put something in THAT?\n", pc);
         return -1;
      }//if
      else if (bag_ptr->isClosed()) {
         show("You need to open it first.\n\0", pc);
         return -1;
      }//if 
      if (!bag_ptr->IN_LIST) { //its not a SOBJ
         if (bag_in_inv) { 
            tmp = obj_to_sobj(*bag_ptr, &(pc.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//if
         else {
            tmp = obj_to_sobj(*bag_ptr, &(ROOM.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//else
         bag_ptr = tmp;
      }//if
      
      pc.inv.head(cell);
      vict_ptr = cell.next();
      while (vict_ptr) {
         if (detect(pc.SEE_BIT, (vict_ptr->OBJ_VIS_BIT | 
                                 ROOM.getVisBit()))) {
            if (obj_is_named(*vict_ptr, *item)) {
               if (vict_ptr != bag_ptr) {
                  found_it = TRUE;
                  if (eq_put_by(*vict_ptr, *bag_ptr, pc, bag_in_inv)) 
                     vict_ptr = pc.inv.lose(cell);
                  else 
                     vict_ptr = cell.next();
               }//if
               else {
                  show("Attempt to self contain denied!!.\n", pc);
                  vict_ptr = cell.next();
               }//else
            }// if obj is named..
            else 
               vict_ptr = cell.next();
         }//if
         else
            vict_ptr = cell.next();
         }//while vict_ptr
      if (!found_it) {
         Sprintf(buf, "You neither see nor have the %S.\n", item);
         show(buf, pc);
      }//if
   }//if "put all.waybread bag"
   else if (strncasecmp(*item, "all", item->Strlen()) == 0) {// put all bag
      mudlog.log(DBG, "put called w/something like: put all bag\n");

      bag_ptr = have_obj_named(pc.inv, j, bag, pc.SEE_BIT, ROOM);
      bag_in_inv = TRUE;

      if (!bag_ptr) {
         bag_in_inv = FALSE;
         bag_ptr = have_obj_named(ROOM.inv, j, bag, pc.SEE_BIT, ROOM);
      }//if
      if (!bag_ptr) {
         show("You don't see that container.\n", pc);
         return -1;
      }//if
      else if (!bag_ptr->ob->bag) {
         show("How can put something in THAT?\n", pc);
         return -1;
      }//if
      else if (bag_ptr->isClosed()) {
         show("You need to open it first.\n", pc);
         return -1;
      }//if 
      if (!bag_ptr->IN_LIST) { //its not a SOBJ
         if (bag_in_inv) { 
            tmp = obj_to_sobj(*bag_ptr, &(pc.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//if
         else {
            tmp = obj_to_sobj(*bag_ptr, &(ROOM.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//else
         bag_ptr = tmp;
      }//if

      pc.inv.head(cell);
      vict_ptr = cell.next();
      while (vict_ptr) {
         mudlog.log(DBG, "in while loop:  put all bag\n");
         if (detect(pc.SEE_BIT, (vict_ptr->OBJ_VIS_BIT | 
                                 ROOM.getVisBit()))) {
            if (vict_ptr != bag_ptr) {
               if (eq_put_by(*vict_ptr, *bag_ptr, pc, bag_in_inv)) {
                  found_it = TRUE;
                  vict_ptr = pc.inv.lose(cell);
               }//if
               else 
                  vict_ptr = cell.next();
            }//if
            else {
               show("Attempt to self contain denied!!\n", pc);
               vict_ptr = cell.next();
            }//else
         }//if
         else 
            vict_ptr = cell.next();
      }//while
   }// if "put all bag"  
   else {
      mudlog.log(DBG, "past all in put\n");

      vict_ptr = have_obj_named(pc.inv, i, item, pc.SEE_BIT, ROOM);
      bag_ptr = have_obj_named(pc.inv, j, bag, pc.SEE_BIT, ROOM);
   
      if (!bag_ptr) {     
         bag_ptr = have_obj_named(ROOM.inv, j, bag, pc.SEE_BIT, ROOM);
         bag_in_inv = FALSE;
      }//if

      if (!bag_ptr) {
         show("You do not see that container.\n", pc);
         return -1;
      }//if
      else if (!vict_ptr) {
         show("You do not see that item.\n", pc);
         return -1;
      }//if
      else if (!bag_ptr->ob->bag) {
         show("How the hell are you gonna fit something in that???\n", pc);
         return -1;
      }//if
      else if (bag_ptr->isClosed()) {
         show("You need to open it first.\n\0", pc);
         return -1;
      }//if 
      if (!bag_ptr->IN_LIST) { //its not a SOBJ
         if (bag_in_inv) { 
            tmp = obj_to_sobj(*bag_ptr, &(pc.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//if
         else {
            tmp = obj_to_sobj(*bag_ptr, &(ROOM.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//else
         bag_ptr = tmp;
      }//if
      if (!vict_ptr) {
         vict_ptr = have_obj_named(ROOM.inv, i, item, pc.SEE_BIT, ROOM);
         item_in_inv = FALSE;
      }//if
      if (!vict_ptr) {
         Sprintf(buf, "You do not see the %S.\n", item);
         show(buf, pc);
         return -1;
      }//if
      
               //If here, we assume have a bag, have an item.//

      if (vict_ptr != bag_ptr) {
         if (eq_put_by(*vict_ptr, *bag_ptr, pc, bag_in_inv)) {
            pc.loseInv(vict_ptr);
         }//if
      }//if
      else {
         show("Attempt to self contain aborted!!\n", pc);
         return -1;
      }//else
   }//else
   return 0;
}//put


int get(int i, const String* item, int j, const String* bag, critter& pc) {
   String buf(100);
   object* bag_ptr;
   object* vict_ptr, *tmp;
   short bag_in_inv = TRUE, found_it = FALSE;
   Cell<object*>  cell;
   short tst = FALSE;

   //mudlog.log(DBG, "In get\n");
   if (!ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if ((item->Strlen() == 0) && (bag->Strlen() == 0)) {
      pc.show("Get what??\n");
      return -1;
   }


   if ((strncasecmp(*item, "all", item->Strlen()) == 0) && 
                   (!bag->Strlen())) {
              //corresponds to "get all"
      mudlog.log(DBG, "get called like: get all\n");
      ROOM.inv.head(cell);
      vict_ptr = cell.next();
      while (vict_ptr) {
         if (detect(pc.SEE_BIT, (vict_ptr->OBJ_VIS_BIT | 
                                 ROOM.getVisBit()))) {
            tst = TRUE; //Something is there....
            if (mudlog.ofLevel(DBG)) {
               mudlog << "detected object:  " << vict_ptr->ob->short_desc
                      << endl;
            }

            if (obj_get_by(*vict_ptr, pc, TRUE)) {
               pc.gainInv(vict_ptr);
               gain_eq_effects(*vict_ptr, obj_list[0], pc, -1, TRUE); 
                               //gold ect
               vict_ptr = ROOM.inv.lose(cell);
            }//if obj_get_by
            else {
               vict_ptr = cell.next();         
            }//else
         }//if detect
         else {
            vict_ptr = cell.next();         
         }//else
      }//while
      if (!tst) {
         show("You can't detect anything...\n", pc);
         return -1;
      }
      return 0;
   }// if "get all"
   else if ((i == -1) && (bag->Strlen() != 0)) { 
               //as in: "get all.waybread bag"
      mudlog.log(DBG, "get as in: get all.waybread bag\n");
      bag_ptr = have_obj_named(pc.inv, j, bag, pc.SEE_BIT, ROOM);
      bag_in_inv = FALSE;
      if (!bag_ptr) {
         bag_in_inv = TRUE;
         bag_ptr = have_obj_named(ROOM.inv, j, bag, pc.SEE_BIT, ROOM);
      }//if
      if (!bag_ptr) {
         show("You don't seem to have that container.\n", pc);
         return -1;
      }//if
      if (!bag_ptr->ob->bag) {
         show("How are you gonna get something from THAT?\n", pc);
         return -1;
      }//if
      if (bag_ptr->OBJ_FLAGS.get(59)) {
         show("Try empty instead!\n", pc);
         return -1;
      }//if
      if (bag_ptr->isClosed()) {
         show("You need to open it first.\n\0", pc);
         return -1;
      }//if 
      if (!bag_ptr->IN_LIST) { //its not a SOBJ
         if (bag_in_inv) { 
            tmp = obj_to_sobj(*bag_ptr, &(pc.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//if
         else {
            tmp = obj_to_sobj(*bag_ptr, &(ROOM.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//else
         bag_ptr = tmp;
      }//if

      bag_ptr->ob->inv.head(cell);
      vict_ptr = cell.next();
      while (vict_ptr) {
         if (obj_get_by(*vict_ptr, pc, TRUE)) {
            found_it = TRUE;
            pc.gainInv(vict_ptr);
            gain_eq_effects(*vict_ptr, *bag_ptr, pc, bag_in_inv, TRUE); 
                           //gold ect
            vict_ptr = bag_ptr->ob->inv.lose(cell);
         }//if obj_get_by
         else
            vict_ptr = cell.next();
      }//while obj_ptr
      if (!found_it) {
         Sprintf(buf, "The %S doesn't seem to contain the %S.\n", bag, item);
         show(buf, pc);
      }//if
   }//if "get all.waybread bag"
   else if (i == -1) {
     // called as:  get all.gold  (from the room)
      ROOM.inv.head(cell);
      vict_ptr = cell.next();
      while (vict_ptr) {
	if (obj_is_named(*vict_ptr, *item)) {
	  if (obj_get_by(*vict_ptr, pc, TRUE)) {
            found_it = TRUE;
            pc.gainInv(vict_ptr);
            gain_eq_effects(*vict_ptr, *bag_ptr, pc, bag_in_inv, TRUE); 
	              //gold ect
            vict_ptr = ROOM.inv.lose(cell);
	  }//if obj_get_by
	  else
            vict_ptr = cell.next();
	}//if named right
      }//while obj_ptr
      if (!found_it) {
         show("You don't see that here.\n", pc);
      }//if
   }//if     
   else if (strncasecmp(*item, "all", item->Strlen()) == 0){ // "get all bag"
      mudlog.log(DBG, "get called like: get all bag\n");
      bag_ptr = have_obj_named(pc.inv, j, bag, pc.SEE_BIT, ROOM);
      bag_in_inv = TRUE;
     
      if (!bag_ptr) {
         bag_in_inv = FALSE;
         bag_ptr = have_obj_named(ROOM.inv, j, bag, pc.SEE_BIT, ROOM);
      }//if
      if (!bag_ptr) {
         show("You don't see that container.\n", pc);
         return -1;
      }//if
      else if (!bag_ptr->ob->bag) {
         show("How are you gonna get something from THAT?\n", pc);
         return -1;
      }//if
      else if (bag_ptr->isClosed()) {
         show("You need to open it first.\n", pc);
         return -1;
      }//if 
      if (bag_ptr->OBJ_FLAGS.get(59)) {
         show("Try empty instead!\n", pc);
         return -1;
      }//if
      if (bag_ptr->isBulletinBoard()) {
         pc.show("You cannot get things from that!\n");
         return -1;
      }
      if (!bag_ptr->IN_LIST) { //its not a SOBJ
         if (bag_in_inv) { 
            tmp = obj_to_sobj(*bag_ptr, &(pc.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//if
         else {
            tmp = obj_to_sobj(*bag_ptr, &(ROOM.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//else
         bag_ptr = tmp;
      }//if

      bag_ptr->ob->inv.head(cell);
      vict_ptr = cell.next();
      tst = FALSE;
      while (vict_ptr) {
         mudlog.log(DBG, "Found an inv ptr to test..\n");
         if (detect(pc.SEE_BIT, (vict_ptr->OBJ_VIS_BIT | 
                                 ROOM.getVisBit()))) {
            tst = TRUE;
            if (vict_ptr != bag_ptr) {
               if (obj_get_by(*vict_ptr, pc, TRUE)) {
                  pc.gainInv(vict_ptr);
                  
                  gain_eq_effects(*vict_ptr, *bag_ptr, pc, bag_in_inv, TRUE);
                  vict_ptr = bag_ptr->ob->inv.lose(cell);
               }//if
               else
                  vict_ptr = cell.next();
            }//
            else {
               show("Attempt to self extract denied.\n", pc);
               vict_ptr = cell.next();
            }//else
         }//if detect
         else 
            vict_ptr = cell.next();         
      }//while
      if (!tst) {
         Sprintf(buf, "The %S appears to be empty.\n", bag);
         show(buf, pc);
      }//if
      tst = FALSE;
   }// if "get all bag"  
          /* Should be done with all instances of "all" */
   else if (!bag->Strlen()) {  // get 2.sword or get sword
      mudlog.log(DBG, "get called like:  get 2.sword or get sword\n");
      vict_ptr = have_obj_named(ROOM.inv, i, item, pc.SEE_BIT, ROOM);
      if (vict_ptr) {
         if (obj_get_by(*vict_ptr, pc, TRUE)) {
            pc.gainInv(vict_ptr);
            ROOM.loseInv(vict_ptr);
            gain_eq_effects(*vict_ptr, obj_list[0], pc, -1, TRUE); //gold ect
         }//if obj_get_by
      }//if
      else {
         show("You don't see that here.\n", pc);
      }//else
   }//if

   /*  Then it must be something like: get 2.bag sack.... */
   //check for both in inventory
   else {

      mudlog.log(DBG, "get called like: get 2.bag sack\n");

      bag_ptr = have_obj_named(pc.inv, j, bag, pc.SEE_BIT, ROOM);

      if (!bag_ptr) {     
         bag_ptr = have_obj_named(ROOM.inv, j, bag, pc.SEE_BIT, ROOM);
         bag_in_inv = FALSE;
      }//if
      if (!bag_ptr) {
         show("You do not see that container.\n", pc);
         return -1;
      }//if
      else if (!bag_ptr->ob->bag) {
         show("How are you gonna get something from THAT?\n", pc);
         return -1;
      }//if
      else if (bag_ptr->isClosed()) {
         show("You need to open it first.\n", pc);
         return -1;
      }//if
      if (bag_ptr->OBJ_FLAGS.get(59)) {
         show("Try empty instead!\n", pc);
         return -1;
      }//if

			/* have a valid bag_ptr here */
      if (!bag_ptr->IN_LIST) { //its not a SOBJ
         if (bag_in_inv) { 
            tmp = obj_to_sobj(*bag_ptr, &(pc.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//if
         else {
            tmp = obj_to_sobj(*bag_ptr, &(ROOM.inv), TRUE, j,
                   bag, pc.SEE_BIT, ROOM);
         }//else
         bag_ptr = tmp;
      }//if
      if (!(vict_ptr = have_obj_named(bag_ptr->ob->inv, i, item, 
                            pc.SEE_BIT, ROOM))) {
         Sprintf(buf, "The %S doesn't contain the %S.\n", bag, item);
         show(buf, pc);
         return -1;
      }//if
      else {
         //If here, we assume have a bag, have an item.//
         if (bag_ptr != vict_ptr) { 
            if (obj_get_by(*vict_ptr, pc, TRUE)) {
               pc.gainInv(vict_ptr);
               bag_ptr->loseInv(vict_ptr);
               gain_eq_effects(*vict_ptr, *bag_ptr, pc, bag_in_inv, TRUE); 
            }//if
         }//if
         else {
            show("Attempt to self extract denied.\n", pc);
         }//else
      }//else
   }//else
   return 0;
}//get



int say(const char* message, critter& pc, room& rm) {
   List<critter*> tmp_lst(rm.getCrits());
   Cell<critter*> cell(tmp_lst);
   critter* crit_ptr;
   String buf(200);
   String msg;

   msg = message;
   pc.drunkifyMsg(msg);
   
   if (pc.isGagged()) {
      show("You have been gagged.\n", pc);
      return -1;
   }//if
   else if (pc.POS > POS_REST) {
      show("You mutter something in your sleep....\n", pc);
      return -1;
   }//if
   else { //good to go
      Cell<object*> ocll(rm.inv);
      object* optr;
      while ((optr = ocll.next())) {
         if (optr->ob->obj_proc && optr->ob->obj_proc->w_eye_owner) {
            Sprintf(buf, "\n#####%S says, '%S'\n", 
                    name_of_crit(pc, optr->ob->obj_proc->w_eye_owner->SEE_BIT),
                    &msg);
            buf.setCharAt(7, toupper(buf[7]));
            show(buf, *(optr->ob->obj_proc->w_eye_owner));	  
         }//if
      }//while

      String tag;
      String untag;

      while ((crit_ptr = cell.next())) {
         if (crit_ptr != &pc) { 
            if (crit_ptr->IS_SAY) { //if channel say is on
               if (crit_ptr->pc) {

                  if (crit_ptr->pc && crit_ptr->USING_CLIENT) {
                     tag = "<SAY>";
                     untag = "</SAY>";
                  }
                  else if (crit_ptr->isUsingColor()) {
                     tag = *(crit_ptr->getSayColor());
                     untag = *(crit_ptr->getDefaultColor());
                  }
                  else {
                     tag = "";
                     untag = "";
                  }

                  Sprintf(buf, "%S\n%S says, '%S'\n%S", 
                          &tag, name_of_crit(pc, crit_ptr->SEE_BIT),
                          &msg, &untag);
                  buf.setCharAt(tag.Strlen() + 1, toupper(buf[tag.Strlen() + 1]));
                     
                  show(buf, *crit_ptr);
               }//if is a pc              
            }//if channel say is on
         }//if not self
         else {
            if (pc.pc && pc.USING_CLIENT) {
               tag = "<SAY>";
               untag = "</SAY>";
            }
            else if (pc.isUsingColor()) {
               tag = *(pc.getSayColor());
               untag = *(pc.getDefaultColor());
            }
            else {
               tag = "";
               untag = "";
            }

            Sprintf(buf, "%SYou say, '%S'\n%S", &tag, &msg, &untag);
            pc.show(buf);
         }//else  
      }//while
      
      String cmd = "say";
      String arg1 = msg;
      ROOM.checkForProc(cmd, arg1, pc, -1);
      
   }//else
   return 0;
}//say


int pemote(const char* message, critter& pc, room& rm, short show_non_detects,
	   critter* crit = NULL) {
   return do_emote(message, pc, rm, show_non_detects, EMOTE_POSSESSIVE, crit);
}

int emote(const char* message, critter& pc, room& rm, short show_non_detects,
          critter* crit = NULL) {
   return do_emote(message, pc, rm, show_non_detects, EMOTE_NON_POSSESSIVE, crit);
}


int do_emote(const char* message, critter& pc, room& rm, short show_non_detects,
              int possessive, critter* crit = NULL) {
   List<critter*> tmp_lst(rm.getCrits());
   Cell<critter*> cell(tmp_lst);
   critter* crit_ptr;
   String buf(200);
   String msg;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In do_emote msg -:" << message << ":- pc: "
             << *(pc.getName()) << " pc_num: " << pc.getIdNum()
             << " pc_addr: " << &pc 
             << " room#: " << rm.getIdNum() << "  crit: " << crit
             << endl;
   }//if


   if (pc.pc && (pc.MODE == MODE_LOGGING_IN)) { 
      return -1; //beautification hack
   }// if

   msg = message;
   //   pc.drunkifyMsg(msg);

   Cell<object*> ocll(rm.inv);
   object* optr;
   while ((optr = ocll.next())) {
      if (optr->ob->obj_proc && (crit_ptr = optr->ob->obj_proc->w_eye_owner)) {
         if (detect(crit_ptr->SEE_BIT, (pc.VIS_BIT | rm.getVisBit()))) {
            if (possessive == EMOTE_POSSESSIVE) {
               Sprintf(buf, "#####%S's %S\n", 
                       name_of_crit(pc, crit_ptr->SEE_BIT), &msg);
               buf.setCharAt(4, toupper(buf[4]));
            }//if possessive
            else {
               Sprintf(buf, "#####%S %S\n", 
                       name_of_crit(pc, crit_ptr->SEE_BIT), &msg);
               buf.setCharAt(4, toupper(buf[4]));
            }
            show(buf, *crit_ptr);
         }//if
         else {
            if (show_non_detects) {
               if (possessive == EMOTE_POSSESSIVE) {
                  Sprintf(buf, "#####Someone's %S\n", &msg);
               }
               else {
                  Sprintf(buf, "#####Someone %S\n", &msg);
               }
               show(buf, *crit_ptr);
            }//if
         }//else
      }//if
   }//while

   while ((crit_ptr = cell.next())) {

      if (crit_ptr->isSleeping()) {
         continue;
      }

      if ((crit_ptr != &pc) && (crit_ptr != crit)) { 
         if (detect(crit_ptr->SEE_BIT, (pc.VIS_BIT | rm.getVisBit()))) {
            if (possessive == EMOTE_POSSESSIVE) {
               Sprintf(buf, "%S's %S\n", 
                       pc.getName(crit_ptr->SEE_BIT), &msg);
            }
            else {
               Sprintf(buf, "%S %S\n", 
                       pc.getName(crit_ptr->SEE_BIT), &msg);
            }
            buf.Cap();
            crit_ptr->show(buf);
         }//if
         else {
            if (show_non_detects) {
               if (possessive == EMOTE_POSSESSIVE) {
                  Sprintf(buf, "Someone's %S\n", &msg);
               }
               else {
                  Sprintf(buf, "Someone %S\n", &msg);
               }
               show(buf, *crit_ptr);
            }//if
         }//else
      }//if
   }//while
   return 0;
}//do_emote


int yell(const char* message, critter& pc) {
   Cell<critter*> cell(pc_list);
   critter* crit_ptr;
   String buf(200);
   String msg;

   mudlog.log(DBG, "In yell\n");

   msg = message;
   pc.drunkifyMsg(msg);
   
   if (!pc.IS_YELL) {
      show("You are deaf to yells.\n", pc);
      return -1;
   }//if
   else if (pc.pc && pc.PC_FLAGS.get(1)) {
      show("You have been gagged.\n", pc);
      return -1;
   }//if
   else if (pc.POS > POS_REST) {
      show("\nYou roar in your sleep!\n", pc);
      return -1;
   }//if
   else {

      String tag;
      String untag;

      while ((crit_ptr = cell.next())) {
         if (crit_ptr != &pc) { 
            if (crit_ptr->IS_YELL) { //if channel yell is on
               if (crit_ptr->pc && crit_ptr->USING_CLIENT) {
                  tag = "<YELL>";
                  untag = "</YELL>";
               }
               else if (crit_ptr->isUsingColor()) {
                  tag = *(crit_ptr->getYellColor());
                  untag = *(crit_ptr->getDefaultColor());
               }
               else {
                  tag = "";
                  untag = "";
               }

               Sprintf(buf, "%S\n%S yells:  '%S'\n%S", 
                       &tag, name_of_crit(pc, crit_ptr->SEE_BIT), &msg, &untag);
               buf.setCharAt(tag.Strlen() + 1, toupper(buf[tag.Strlen() + 1]));
               show(buf, *crit_ptr);
            }//if channel yell is open
         }//if not equal to self
         else {
            if (pc.pc && pc.USING_CLIENT) {
               tag = "<YELL>";
               untag = "</YELL>";
            }
            else if (pc.isUsingColor()) {
               tag = *(pc.getYellColor());
               untag = *(pc.getDefaultColor());
            }
            else {
               tag = "";
               untag = "";
            }

            Sprintf(buf, "%SYou yell, '%S'\n%S", &tag, &msg, &untag);
            pc.show(buf);
         }//else
      }//while

      String cmd = "yell";
      String arg1 = msg;
      ROOM.checkForProc(cmd, arg1, pc, -1);

   }//else
   return 0;
}//yell


int gossip(const char* message, critter& pc) {
   Cell<critter*> cell(pc_list);
   critter* crit_ptr;
   String buf(200);
   String msg;

   mudlog.log(DBG, "In gossip:");
   mudlog.log(DBG, message);

   msg = message;
   pc.drunkifyMsg(msg);
   
   if (pc.isGagged()) {
      show("You have been gagged.\n", pc);
      return -1;
   }//if
   else if (!(pc.IS_GOSSIP)) {
      show("You are not on the gossip channel.\n", pc);
      return -1;
   }//if
   else if (pc.POS > POS_REST) {
      show("\nYou mutter in your sleep!\n", pc);
      return -1;
   }//if
   else {
      String tag;
      String untag;

      while ((crit_ptr = cell.next())) {
         if (crit_ptr->getMode() == MODE_LOGGING_IN){
            continue;
         }
         if (crit_ptr != &pc) {
            if (crit_ptr->IS_GOSSIP) { //if channel gossip is on

               if (crit_ptr->isSleeping()) {
                  continue;
               }

               if (crit_ptr->pc && crit_ptr->USING_CLIENT) {
                  tag = "<GOSSIP>";
                  untag = "</GOSSIP>";
               }
               else if (crit_ptr->isUsingColor()) {
                  tag = *(crit_ptr->getGossipColor());
                  untag = *(crit_ptr->getDefaultColor());
               }
               else {
                  tag = "";
                  untag = "";
               }

               Sprintf(buf, "%S\n%S gossips, '%S'\n%S", 
                       &tag, name_of_crit(pc, crit_ptr->SEE_BIT), &msg, &untag);
               buf.setCharAt(tag.Strlen() + 1, toupper(buf[tag.Strlen() + 1]));
               show(buf, *crit_ptr); 
            }//if
         }//if
         else {

            if (pc.pc && pc.USING_CLIENT) {
               tag = "<GOSSIP>";
               untag = "</GOSSIP>";
            }
            else if (pc.isUsingColor()) {
               tag = *(pc.getGossipColor());
               untag = *(pc.getDefaultColor());
            }
            else {
               tag = "";
               untag = "";
            }
            
            Sprintf(buf, "%SYou gossip, '%S'\n%S", &tag, &msg, &untag);
            pc.show(buf);
         }//else
      }//while
   }//else
   return 0;
}//gossip




int group_say(const char* message, critter& pc) {
   critter* crit_ptr;
   String buf(200);
   String msg;

   mudlog.log(DBG, "In group_say:");
   mudlog.log(DBG, message);

   msg = message;
   pc.drunkifyMsg(msg);
   
   if (pc.isGagged()) {
      show("You have been gagged.\n", pc);
      return -1;
   }//if
   else if (pc.POS > POS_REST) {
      show("\nYou mutter in your sleep!\n", pc);
      return -1;
   }//if
   else {
      String tag;
      String untag;

      if (pc.groupees.isEmpty()) {
         pc.show("You are not in any group!\n");
      }
      else {
         Cell<critter*> cell(pc.groupees);
         while ((crit_ptr = cell.next())) {
            if (crit_ptr != &pc) {
               if (crit_ptr->isSleeping()) {
                  continue;
               }
               if (crit_ptr->pc && crit_ptr->USING_CLIENT) {
                  tag = "<TELL>";
                  untag = "</TELL>";
               }
               else if (crit_ptr->isUsingColor()) {
                  tag = *(crit_ptr->getTellColor());
                  untag = *(crit_ptr->getDefaultColor());
               }
               else {
                  tag = "";
                  untag = "";
               }

               Sprintf(buf, "%S\n%S tells the group, '%S'\n%S", 
                       &tag, name_of_crit(pc, crit_ptr->SEE_BIT),
                       &msg, &untag);
               buf.setCharAt(tag.Strlen() + 1, toupper(buf[tag.Strlen() + 1]));
               show(buf, *crit_ptr); 
            }//if
            else {

               if (pc.pc && pc.USING_CLIENT) {
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
            
               Sprintf(buf, "%SYou tell the group, '%S'\n%S", &tag, &msg,
                       &untag);
               pc.show(buf);
            }//else
         }//while
      }//else
   }//else
   return 0;
}//group_say


int wizchat(const char* message, critter& pc) {
   Cell<critter*> cell(pc_list);
   critter* crit_ptr;
   String buf(200);
   String msg;

   msg = message;
   //pc.drunkifyMsg(msg);
   
   if (pc.isGagged()) {
      show("You have been gagged.\n", pc);
      return -1;
   }//if
   else if (!(pc.isWizchat())) {
      show("You are not on the wizchat channel.\n", pc);
      return -1;
   }//if
   else if (pc.POS > POS_REST) {
      show("\nYou mutter in your sleep!\n", pc);
      return -1;
   }//if
   else {
      String tag;
      String untag;

      while ((crit_ptr = cell.next())) {
         if (crit_ptr->getMode() == MODE_LOGGING_IN){
            continue;
         }
         if (crit_ptr != &pc) {
            if (crit_ptr->isWizchat()) { //if channel wizchat
               if (crit_ptr->pc && crit_ptr->USING_CLIENT) {
                  tag = "<GOSSIP>";
                  untag = "</GOSSIP>";
               }
               else if (crit_ptr->isUsingColor()) {
                  tag = *(crit_ptr->getGossipColor());
                  untag = *(crit_ptr->getDefaultColor());
               }
               else {
                  tag = "";
                  untag = "";
               }

               Sprintf(buf, "%S\n%S wizchats, '%S'\n%S", 
                       &tag, name_of_crit(pc, crit_ptr->SEE_BIT), &msg,
                       &untag);
               buf.setCharAt(tag.Strlen() + 1, toupper(buf[tag.Strlen() + 1]));
               show(buf, *crit_ptr); 
            }//if
         }//if
         else {
            if (pc.pc && pc.USING_CLIENT) {
               tag = "<GOSSIP>";
               untag = "</GOSSIP>";
            }
            else if (pc.isUsingColor()) {
               tag = *(pc.getGossipColor());
               untag = *(pc.getDefaultColor());
            }
            else {
               tag = "";
               untag = "";
            }
            Sprintf(buf, "%SYou wizchat, '%S'\n%S", &tag, &msg, &untag);
            pc.show(buf);
         }//else
      }//while
   }//else
   return 0;
}//wizchat


int auction(const char* message, critter& pc) {
   Cell<critter*> cell(pc_list);
   critter* crit_ptr;
   String buf(200);
   String msg;

   mudlog.log(DBG, "In auction:");
   mudlog.log(DBG, message);

   msg = message;
   pc.drunkifyMsg(msg);
   
   if (pc.isGagged()) {
      show("You have been gagged.\n", pc);
      return -1;
   }//if
   else if (!(pc.IS_AUCTION)) {
      show("You are not on the auction channel.\n", pc);
      return 1;
   }//if
   else if (pc.POS > POS_REST) {
      show("\nYou mutter in your sleep!\n", pc);
      return 1;
   }//if
   else {
      String tag;
      String untag;
      while ((crit_ptr = cell.next())) {
         if (crit_ptr->getMode() == MODE_LOGGING_IN){
            continue;
         }
         if (crit_ptr != &pc) { 
            if (crit_ptr->IS_AUCTION) { //if channel auction is on
               if (crit_ptr->pc && crit_ptr->USING_CLIENT) {
                  tag = "<AUCTION>";
                  untag = "</AUCTION>";
               }
               else if (crit_ptr->isUsingColor()) {
                  tag = *(crit_ptr->getGossipColor());
                  untag = *(crit_ptr->getDefaultColor());
               }
               else {
                  tag = "";
                  untag = "";
               }

               Sprintf(buf, "%S\n%S auctions, '%S'\n%S",
                       &tag, name_of_crit(pc, crit_ptr->SEE_BIT), &msg, &untag);
               buf.setCharAt(tag.Strlen() + 1, toupper(buf[tag.Strlen() + 1]));
               show(buf, *crit_ptr); 
            }//if
         }//if
         else {
            if (pc.pc && pc.USING_CLIENT) {
               tag = "<GOSSIP>";
               untag = "</GOSSIP>";
            }
            else if (pc.isUsingColor()) {
               tag = *(pc.getGossipColor());
               untag = *(pc.getDefaultColor());
            }
            else {
               tag = "";
               untag = "";
            }

            Sprintf(buf, "%SYou auction, '%S'\n%S", &tag, &msg, &untag);
            pc.show(buf);
         }//else
      }//while
   }//else
   return 0;
}//auction



int shout(const char* message, critter& pc) {
   Cell<critter*> cell(pc_list);
   critter* crit_ptr;
   String buf(200);
   String msg;

   mudlog.log(DBG, "In shout:");
   mudlog.log(DBG, message);

   msg = message;
   pc.drunkifyMsg(msg);

   int which_z = 0;
   if (pc.isMob()) { //then we need to guess a zone
      which_z = pc.getNativeZoneNum();
   }
   else { //then it's SMOB or PC
      which_z = ROOM.getZoneNum();
   }

   if (pc.isGagged()) {
      show("You have been gagged.\n", pc);
      return -1;
   }//if
   else if (!(pc.IS_SHOUT)) {
      show("You are not on the shout channel.\n", pc);
      return -1;
   }//if
   else if (pc.POS > POS_REST) {
      show("\nYou mutter in your sleep!\n", pc);
      return -1;
   }//if
   else {
      String tag;
      String untag;

      while ((crit_ptr = cell.next())) {
         if (crit_ptr != &pc) { 
            if (crit_ptr->IS_SHOUT) { //if channel shout is on
               if (which_z == (room_list[crit_ptr->getCurRoomNum()].getZoneNum())) {
                  if (crit_ptr->pc && crit_ptr->USING_CLIENT) {
                     tag = "<YELL>";
                     untag = "</YELL>";
                  }
                  else if (crit_ptr->isUsingColor()) {
                     tag = *(crit_ptr->getYellColor());
                     untag = *(crit_ptr->getDefaultColor());
                  }
                  else {
                     tag = "";
                     untag = "";
                  }

                  Sprintf(buf, "%S\n%S shouts, '%S'\n%S",
                          &tag, name_of_crit(pc, crit_ptr->SEE_BIT), &msg, &untag);
                  buf.setCharAt(tag.Strlen() + 1, toupper(buf[tag.Strlen() + 1]));
                  show(buf, *crit_ptr); 
               }//if
            }//if
         }//if
         else {
            if (pc.pc && pc.USING_CLIENT) {
               tag = "<YELL>";
               untag = "</YELL>";
            }
            else if (pc.isUsingColor()) {
               tag = *(pc.getYellColor());
               untag = *(pc.getDefaultColor());
            }
            else {
               tag = "";
               untag = "";
            }

            Sprintf(buf, "%SYou shout, '%S'\n%S", &tag, &msg, &untag);
            pc.show(buf);
         }//else
      }//while
   }//else
   return 0;
}//shout


int east(critter& pc, room& rm, int& is_dead) {
   return move(pc, 1, "east", TRUE, rm, is_dead);
}//east

int west(critter& pc, room& rm, int& is_dead) {
   return move(pc, 1, "west", TRUE, rm, is_dead);
}//west

int south(critter& pc, room& rm, int& is_dead) {
   return move(pc, 1, "south", TRUE, rm, is_dead);
}

int north(critter& pc, room& rm, int& is_dead) {
   return move(pc, 1, "north", TRUE, rm, is_dead);
}

int northwest(critter& pc, room& rm, int& is_dead) {
   return move(pc, 1, "northwest", TRUE, rm, is_dead);
}

int southwest(critter& pc, room& rm, int& is_dead) {
   return move(pc, 1, "southwest", TRUE, rm, is_dead);
}

int southeast(critter& pc, room& rm, int& is_dead) {
   return move(pc, 1, "southeast", TRUE, rm, is_dead);
}

int northeast(critter& pc, room& rm, int& is_dead) {
   return move(pc, 1, "northeast", TRUE, rm, is_dead);
}

int up(critter& pc, room& rm, int& is_dead) {
   return move(pc, 1, "up", TRUE, rm, is_dead);
}

int down(critter& pc, room& rm, int& is_dead) {
   return move(pc, 1, "down", TRUE, rm, is_dead);
}


int rest(critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.POS == POS_REST) {
      show("You're already resting.\n", pc);
      return 0;
   }//if
   if (pc.POS < POS_REST) {
      if (IsEmpty(pc.IS_FIGHTING)) {
         show("You take off your boots and make yourself comfortable.\n", 
              pc);
         emote("sits down to take a breather.\n", pc, ROOM, FALSE);
         pc.POS = POS_REST; //rest

         String cmd = "rest";
         ROOM.checkForProc(cmd, NULL_STRING, pc, -1);

      }//if
      else {
         show("You can't rest now, you're fighting!!\n", pc);
         return -1;
      }//else
   }//if
   else {
      show("You're already way beyond resting!\n", pc);
      return -1;
   }//else
   return 0;
}//rest


int sit(critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.POS == POS_SIT) {
      show("You're already sitting.\n", pc);
      return 0;
   }//if
   if ((pc.POS < POS_REST) || (pc.POS == POS_PRONE)) {
      if (IsEmpty(pc.IS_FIGHTING)) {
         show("You sit on your haunches, resting but wary.\n", pc);
         emote("sits down.\n", pc, ROOM, FALSE);
         pc.POS = POS_SIT; //sit

         String cmd = "sit";
         ROOM.checkForProc(cmd, NULL_STRING, pc, -1);
      }//if
      else {
         show("You can't sit now, you're fighting!!\n", pc);
         return -1;
      }//else
   }//if
   else {
      show("You're already way beyond sitting!\n", pc);
      return -1;
   }//else
   return 0;
}//sit


int stand(critter& pc) {
   String buf(100);

   mudlog.log(DBG, "In stand.\n");

   if (!ok_to_do_action(NULL, "mFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.POS == POS_STAND) {
      show("You're already standing.\n", pc);
      return 0;
   }//if

   if ((pc.POS < POS_SLEEP) || (pc.POS == POS_PRONE)) {
      if (!(pc.CRIT_FLAGS.get(14))) {  //not paralyzed
         show("You stand up ready to face the world once again.\n", pc);
         emote("stands up.\n", pc, ROOM, FALSE);
         pc.POS = POS_STAND; //stand

         String cmd = "stand";
         ROOM.checkForProc(cmd, NULL_STRING, pc, -1);
      }//if
      else {
         show("You can't even FEEL your legs, how can you stand?\n", pc);
         return -1;
      }//else
   }//if
   else {
      show("Maybe you should try to wake first.\n", pc);
      return -1;
   }//else
   return 0;
}//stand


int sleep(critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.POS == POS_SLEEP) {
      show("You're already sleeping.\n", pc);
      return 0;
   }//if
   else if ((pc.POS <= POS_MED)) {
      if (IsEmpty(pc.IS_FIGHTING)) {
         show("You lay yourself down to sleep.\n", pc);
         emote("goes to sleep.\n", pc, ROOM, FALSE);
         pc.POS = POS_SLEEP; //sleep

         String cmd = "sleep";
         ROOM.checkForProc(cmd, NULL_STRING, pc, -1);
      }//if
      else {
         show("You can't sleep now, you're fighting!!\n", pc);
         return -1;
      }//else
   }//if
   else {
      show("You're already way beyond sleeping!\n", pc);
      return -1;
   }//else
   return 0;
}//sleep


int meditate(critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.POS == POS_MED) {
      show("You're already meditating.\n", pc);
      return 0;
   }//if
   else if ((pc.POS != POS_SLEEP)) {
      if (IsEmpty(pc.IS_FIGHTING)) {
         if (d(1, 100) < d(1, get_percent_lrnd(MEDITATION_SKILL_NUM, pc))) {
            show("You relax into a deep trance.\n", pc);
            emote("goes into a trance.\n", pc, ROOM, FALSE);
            pc.POS = POS_MED; //sleep
            
            String cmd = "meditate";
            ROOM.checkForProc(cmd, NULL_STRING, pc, -1);
            return 0;
         }//if
         else {
            show("You just can't seem to concentrate...\n", pc);
            pc.PAUSE += 2;
         }//else
      }//if
      else {
         show("You can't meditate now, you're fighting!!\n", pc);
      }//else
   }//if
   else {
      show("You must wake first.\n", pc);
   }//else
   return -1;
}//meditate


int wake(critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.POS < POS_SLEEP) {
      show("You're already awake.\n", pc);
   }//if
   else if (pc.POS < POS_DEAD) {
      if (!(pc.CRIT_FLAGS.get(15))) { //not perm_sleeped by spell
         show("You wake, sit up and stretch.\n", pc);
         emote("wakes and sits.\n", pc, ROOM, FALSE);
         pc.POS = POS_SIT; //sit

         String cmd = "wake";
         ROOM.checkForProc(cmd, NULL_STRING, pc, -1);
         return 0;
      }//if
      else {
         show("You can't wake up, don't you just hate that??!!\n", pc);
      }//else
   }//if
   else {
      show("You see a flickering...REDISH??? light coming towards you!\n", 
           pc);
   }//else
   return -1;
}//wake


//*******************************************************************//
///*********************  auxillary functions  *********************///
//*******************************************************************//


int move(critter& pc, int i_th, const char* direction, short do_followers, 
         room& rm, int& is_dead) {
   Cell<critter*> cell;
   critter* ptr2;
   door* door_ptr;   
   String buf(100);
   int dest;

   //   mudlog.log(DBG, "In move, direction is:\n");
   //   mudlog.log(DBG, direction);

   if (!direction) {
      mudlog.log(ERR, "ERROR: direction NULL in move.\n");
      return FALSE;
   }//if

   if (!ok_to_do_action(NULL, "FP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   String str_dir(direction);

   if (pc.MOV < 1) {
      show("You can't take one more step!!\n", pc);
   }//if
   else if (pc.POS != POS_STAND) {
      show("You have to be standing in order to travel.\n", pc);
   }//if
   else if (!IsEmpty(pc.IS_FIGHTING)) { //if fighting
      show("You are busy fighting.\n\0", pc);
   }//if
   else if ((door_ptr = door::findDoor(rm.DOORS, i_th, &str_dir, ~0, rm))) {
      dest = abs(door_ptr->destination);

      if (door_ptr->isSecret() && //secret and closed
	  door_ptr->isClosed()) {
         show("You can't discern an exit in that direction.\n\0", pc);
      }//if
      else if (door_ptr->isClosed()) {
         Sprintf(buf, "The %S is closed.\n", name_of_door(*door_ptr,
                 pc.SEE_BIT));
         show(buf, pc);
      }//if
      else if (door_ptr->dr_data->door_data_flags.get(14) &&
	       door_ptr->crit_blocking &&
	       (door_ptr->crit_blocking != &pc)) {
	Sprintf(buf, "The %S is blocked by %S.\n", 
		name_of_door(*door_ptr, pc.SEE_BIT),
		name_of_crit(*(door_ptr->crit_blocking), pc.SEE_BIT));
	show(buf, pc);
      }//else
      else if (mob_can_enter(pc, room_list[dest], TRUE)) {
         //mudlog << "In move(), about to make a tmp_lst.\n" << flush;
         List<critter*> tmp_lst(rm.getCrits());
         //mudlog << "In move(), made tmp_lst.\n" << flush;
         tmp_lst.head(cell);
         while ((ptr2 = cell.next())) {
	   if (!(ptr2->pc)) {
	     if (ptr2->mob) {
	       if (ptr2->mob->proc_data) {
		 if (ptr2->mob->proc_data->int1 != 0) {
		   if (strcasecmp(
				  *(Top(door_list[ptr2->INT1].names)), 
				  *(direction_of_door(*door_ptr))) == 0) {
		     if ((ptr2->FLAG1.get(3) && (ptr2->CLASS == pc.CLASS)) ||
			 (ptr2->FLAG1.get(4) && (ptr2->RACE == pc.RACE))) {
		       break;  //its ok for them to pass
		     }//if
		     Sprintf(buf, "%S blocks your way.\n", 
			     name_of_crit(*ptr2, pc.SEE_BIT));
		     buf.Cap();
		     show(buf, pc);
		     Sprintf(buf, "blocks %S's way.\n", name_of_crit(pc, ~0));
		     emote(buf, *ptr2, rm, TRUE, &pc);
		     return -1;
		   }//if
		 }//if a sentinal
	       }//if
	     }//if
	   }//if
         }//while

         if (door_ptr->getTokenNum() != 0) {
            object* token = NULL;
            int posn = -1;
            
	    token = have_obj_numbered(pc.inv, 1, door_ptr->getTokenNum(),
                                      pc.SEE_BIT, rm);
            if (!token) {
               for (int i = 1; i<MAX_EQ; i++) {
                  if (pc.EQ[i] && 
                      (pc.EQ[i]->OBJ_NUM == door_ptr->getTokenNum())) {
                     posn = i;
                     token = pc.EQ[i];
                     break;
                  }//if
               }//for
            }//if

            if (token) {
               if (door_ptr->consumesKey()) {
                  Sprintf(buf, "As give up %S as you pass through.\n",
                          token->getLongName());
                  if (posn > 0) {
                     pc.EQ[posn] = NULL;
                     remove_eq_effects(*token, pc, FALSE, FALSE, posn);
                  }
                  else {
                     pc.inv.loseData(token);
                     drop_eq_effects(*token, pc, FALSE);
                  }
                  if (token->IN_LIST) {
                     recursive_init_unload(*token, 0);
                     delete token;
                  }
                  token = NULL;
               }//if
            }//if
	    else {
               Sprintf(buf, "You must have %S in order to pass.\n",
                       obj_list[bound(1, NUMBER_OF_ITEMS,
                                      door_ptr->getTokenNum())].getLongName());
               pc.show(buf);
               return -1;
	    }//else
         }//if


	 //         mudlog.log(DBG, "passed tests in move\n
	 if (!pc.isNoHassle())
	    pc.PAUSE += door_ptr->distance;

	 int sneak_worked = FALSE;
	 if (pc.isSneaking() &&
	     d(1, 100) < (d(1, get_percent_lrnd(SNEAK_SKILL_NUM, pc) +
			    pc.DEX * 10 + pc.LEVEL * 5))) {
	   sneak_worked = TRUE;
	 }//if
         
	 if (!sneak_worked) {
            Sprintf(buf, "leaves %s.\n", direction);
            emote(buf, pc, rm, FALSE);
	 }//if

         if (!pc.isMob()) {
            pc.MOV -= rm.getMovCost();
         }//if
         
         int old_room_num = rm.getIdNum();

         door* opposite_door = door::findDoorByDest(room_list[dest].doors,
                                                    old_room_num);
         String from_dir(30);
         int is_custom = FALSE;
         if (opposite_door) {
            from_dir = *(opposite_door->getDirection());
            if (strcasecmp(from_dir, "up") == 0) {
               is_custom = TRUE;
               from_dir = "above";
            }
            else if (strcasecmp(from_dir, "down") == 0) {
               is_custom = TRUE;
               from_dir = "below";
            }
         }//if

         pc.doGoToRoom(dest, from_dir, door_ptr, is_dead, rm.getIdNum());

         if (is_dead) {
            return 0;
         }

         if (from_dir.Strlen()) {
            if (!sneak_worked) {
               if (is_custom) {
                  Sprintf(buf, "has arrived from %S.", &from_dir);
               }//if
               else {
                  Sprintf(buf, "has arrived from the %S.", &from_dir);
               }//else
               emote(buf, pc, room_list[dest], FALSE);
            }//if
         }//if
         else {
            if (!sneak_worked) {
               emote("has arrived.", pc, room_list[dest], FALSE);
            }//if
         }//else

         if (do_followers) {
            pc.FOLLOWERS.head(cell);
            while ((ptr2 = cell.next())) {
               if (room_list[old_room_num].haveCritter(ptr2)) {
                  Sprintf(buf, "You follow %S %s.",
                          pc.getName(ptr2->SEE_BIT), direction);
                  ptr2->show(buf);
                  int is_follower_dead;
                  move(*ptr2, i_th, direction, TRUE, rm, is_follower_dead);
               }//if
            }//while
         }//if


         return 0;
      }//else
   }//if
   else {
      show("You can't discern an exit in that direction.\n\0", pc);
   }//else
   return -1;
}//move


int don_obj(object& obj, critter& pc) {
   String buf(100);

   mudlog.log(DBG, "In don_obj\n");
   if (obj.OBJ_FLAGS.get(21)) {
      Sprintf(buf, "You can't wear %S.\n", &(obj.ob->short_desc));
      mudlog.log(DBG, "don_obj failed, obj failed flag 21.\n");
      show(buf, pc);
   }//if
   else {
      for (int i = 22; i< (MAX_EQ + 21); i++) {
         if (obj.OBJ_FLAGS.get(i)) { //if obj is worn there
            if (!pc.EQ[i - 21]) {
               if (obj_wear_by(obj, pc, i - 21, TRUE)) {
                  pc.EQ[i - 21] = &obj; 
                  wear_eq_effects(obj, pc, i - 21, TRUE); 
                  return 0;
               }//if
            }//if
            else {
               Sprintf(buf, 
                    "   %S:%P25You are already using something there.\n",
                    name_of_obj(obj, pc.SEE_BIT));
               buf.Cap();
               show(buf, pc);
               mudlog.log(DBG, 
                          "don_obj failed, already using something there.\n");
            }//else
         }//if
      }//for
   }//else
   return -1;
}//don_object


int out_eq(critter& targ, critter& looker) {
   String wmsg(81);
   String buf(100);

   mudlog.log(DBG, "In out_eq\n");

   for (int i = 1; i<MAX_EQ; i++) {
      if (targ.EQ[i]) {
         switch (i) { //which position
            //case 19:  
            //   wmsg = "     Worn on ears:";
            //   break;
            case 1:
               wmsg = "     Worn on head:";
               break;
            case 2: case 3:
               wmsg = "     Worn around neck:";
               break;
            case 4:
               wmsg = "     Worn around body:";
               break;
            case 5:
               wmsg = "     Worn on arms:";
               break;
            case 6: case 7:
               wmsg = "     Worn on wrist:";
               break;
            case 8:
               wmsg = "     Worn on hands:";
               break;
            case 9:
               wmsg = "     Wielded:";
               break;
            case 10:
               wmsg = "     Held:";
               break;
            case 11:
               wmsg = "     Used as light:";
               break;
            case 12:
               wmsg = "     Worn on body:";
               break;
            case 13:
               wmsg = "     Worn around waist:";
               break;
            case 14:
               wmsg = "     Worn on legs:";
               break;
            case 15:
               wmsg = "     Worn on feet:";
               break;
            case 16: case 17:
               wmsg = "     Worn on finger:";
               break;
            case 18:
               wmsg = "     Used as shield:";
               break;
            default:
               if (mudlog.ofLevel(ERR)) {
                  mudlog << "ERROR:  Default called in out_eq, i:  "
                         << i << "  MaxEQ:  " << MAX_EQ << endl;
               }
               wmsg = "Used somewhere undefined:\t";
               break;
         }//switch

         if (looker.shouldShowVnums()) {
            Sprintf(buf, "%S%P30 [%i]%P37 %S.\n", &wmsg, targ.EQ[i]->getIdNum(),
                    long_name_of_obj(*(targ.EQ[i]), looker.SEE_BIT));
         }
         else {
            Sprintf(buf, "%S%P30 %S.\n", &wmsg,
                    long_name_of_obj(*(targ.EQ[i]), looker.SEE_BIT));
         }

         show(buf, looker);
      }//if eq there
   }//for
   return 0;
}//out_eq


// Returns TRUE if can be worn, FALSE otherwise.
int obj_wear_by(object& obj, critter& pc, int in_posn, short do_msg) {
   String buf(100);
   int i, posn = in_posn, k;

   if (obj.OBJ_LEVEL > pc.LEVEL) {
      if (do_msg) {
         Sprintf(buf, "You are not experienced enough to use %S.\n",
                 long_name_of_obj(obj, pc.SEE_BIT));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, level restriction.\n");
      return FALSE;
   }//if


   if (in_posn == -1) { //wear it anywhere
      k = Obj_Wear_Flags_Mask.max_bit();
      for (i = 22; i <= k; i++) {
         if (Obj_Wear_Flags_Mask.get(i) && obj.OBJ_FLAGS.get(i)) {
            posn = i - 21; //displacement of wear flags in obj_flags
         }//if
      }//for
   }//if

   if ((obj.OBJ_CUR_WEIGHT > pc.STR) && ((posn == 9) || (posn == 10))) {
      if (do_msg) {
	 Sprintf(buf, "%S is just too heavy for you!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, object too heavy.\n");
      return FALSE;
   }//if

   if (!obj.OBJ_FLAGS.get(21 + posn)) {
      if (do_msg) {
	 show("You can't wear it there.\n", pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, wrong posn.\n");
      return FALSE;
   }//if

   if (obj.OBJ_FLAGS.get(0)) { //if !restrictions
      return TRUE;
   }//if

   if ((obj.OBJ_FLAGS.get(1)) && (pc.ALIGN < -350)) {
      if (do_msg) {
  	 Sprintf(buf, "You're too evil for your %S!\n", single_obj_name(obj, 
              pc.SEE_BIT));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, pc too evil.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(2)) && ((pc.ALIGN >= -350) && (pc.ALIGN <= 
                                   350))) {
      if (do_msg) {
         Sprintf(buf, "You're too neutral for your %S!\n", 
	 single_obj_name(obj, pc.SEE_BIT));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, pc to neutral.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(3)) && (pc.ALIGN > 350)) {
      if (do_msg) {
         Sprintf(buf, "You're too pleasant for your %S!\n",
   	    single_obj_name(obj, pc.SEE_BIT));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, pc to good.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(7)) && (!pc.pc || !pc.pc->imm_data)) { 
      if (do_msg) {
         Sprintf(buf, "%S belongs to an immortal!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, immort only.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(8)) && (!pc.pc || !pc.pc->imm_data ||
		!(pc.IMM_LEVEL > 5))) {
      if (do_msg) {
         Sprintf(buf, "%S belongs to a demi-god!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, demi only.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(9)) && (!pc.pc || !pc.pc->imm_data ||
		pc.IMM_LEVEL < 9)) {
      if (do_msg)  {
         Sprintf(buf, "%S belongs to a god!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, god only.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(11)) && (pc.CLASS == 1)) { //warrior
      if (do_msg) {
         Sprintf(buf, "Silly warrior, %S is not for you!\n", 
              &(obj.ob->short_desc));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, obj is !warrior.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(12)) && (pc.CLASS == 2)) {  // sage
      if (do_msg) {
         Sprintf(buf, "Silly sage, %S is not for you!\n", 
              &(obj.ob->short_desc));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, obj is !sage.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(13)) && (pc.CLASS == 3)) { // wizard
      if (do_msg) {
         Sprintf(buf, "Silly wizard, %S is not for you!\n", 
              &(obj.ob->short_desc));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, obj is !wizard.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(14)) && (pc.CLASS == 4)){ //ranger
      if (do_msg) {
         Sprintf(buf, "Silly ranger, %S is not for you!\n", 
              &(obj.ob->short_desc));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, obj is !ranger.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(15)) && (pc.CLASS == 5)) {//thief
      if (do_msg) {
         Sprintf(buf, "Silly thief, %S is not for you!\n", 
              &(obj.ob->short_desc));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, obj is !thief.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(16)) && (pc.CLASS == 6)){ //alchemist
      if (do_msg) {
         Sprintf(buf, "Alchemists can't use %S!\n", 
		&(obj.ob->short_desc));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, obj is !alchemist.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(17)) && (pc.CLASS == 7)){ //cleric
      if (do_msg) {
         Sprintf(buf, "Silly cleric, %S is not for you!\n", 
              &(obj.ob->short_desc));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, obj is !cleric.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(18)) && (pc.CLASS == 8)){ //bard
      if (do_msg) {
         Sprintf(buf, "Silly bard, %S is not for you!\n", 
              &(obj.ob->short_desc));
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, obj is !bard.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(19)) && (pc.isNPC())) {
      if (do_msg) {
         Sprintf(buf, "%S is !MOB equipment!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc); //no-one should ever see this i believe
      }//if
      mudlog.log(DBG, "obj_wear_by failed, obj is !NPC.\n");
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(20)) && (pc.isPc())) { //!pc
      if (do_msg) {
         Sprintf(buf, "%S is for mobs only!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc);
      }//if
      mudlog.log(DBG, "obj_wear_by failed, obj is !PC.\n");
      return FALSE;
   }//if

   mudlog.log(DBG, "obj_wear_by succeeded.\n");
   return TRUE;
}//obj_wear_by 


// Returns TRUE if can be gotten, FALSE otherwise.
int obj_get_by(object& obj, critter& pc, short do_msg) {
   String buf(100);

   //mudlog.log(DBG, "In obj_get_by.\n");

   if (obj.OBJ_CUR_WEIGHT > (pc.CRIT_MAX_WT_CARRY - pc.CRIT_WT_CARRIED)) {
      if (do_msg) {
         Sprintf(buf, "%S:\tYou can't carry that much weight.\n", 
              &(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc);
      }//if
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(7)) && (!pc.pc || !pc.pc->imm_data)) { 
      if (do_msg) {
         Sprintf(buf, "%S belongs to an immortal!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc);
      }//if
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(8)) && (!pc.pc || !pc.pc->imm_data ||
		!(pc.IMM_LEVEL > 5))) {
      if (do_msg) {
         Sprintf(buf, "%S belongs to a demi-god!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc);
      }//if
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(9)) && (!pc.pc || !pc.pc->imm_data ||
		pc.IMM_LEVEL < 9)) {
      if (do_msg)  {
         Sprintf(buf, "%S belongs to a god!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc);
      }//if
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(19)) && (pc.isNPC())) {
      if (do_msg) {
         Sprintf(buf, "%S is !MOB equipment!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc); //no-one should ever see this i believe
      }//if
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(20)) && (pc.isPc())) { //!pc
      if (do_msg) {
         Sprintf(buf, "%S is for mobs only!\n", 
		&(obj.ob->short_desc));
         buf.Cap();
         show(buf, pc);
      }//if
      return FALSE;
   }//if
   
   if (obj.isBulletinBoard() && (!(pc.isImmort() && (pc.IMM_LEVEL > 8)))) {
      if (do_msg) {
         pc.show("You shouldn't be getting buletin boards, try opurge.\n");
      }
      return FALSE;
   }

   return TRUE;
}//obj_get_by


// Returns TRUE if can be given, FALSE otherwise.
int source_give_to(critter& pc, object& obj, critter& targ) {
   String buf(100);

   if (obj.OBJ_CUR_WEIGHT > (targ.CRIT_MAX_WT_CARRY - 
                             targ.CRIT_WT_CARRIED)) {
      Sprintf(buf, "%S:\t%S can't carry that much weight.\n", 
              &(obj.ob->short_desc), name_of_crit(targ, pc.SEE_BIT)); 
      buf.Cap();
      show(buf, pc);
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(7)) && (targ.LEVEL < 31)) { //!mort
      Sprintf(buf, "%S is not for mortals!\n", &(obj.ob->short_desc));
      buf.Cap();
      show(buf, pc);
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(8)) && ((targ.LEVEL <= 35) && (targ.LEVEL >= 31))){
      Sprintf(buf, "%S is not for demi-gods!\n", 
		&(obj.ob->short_desc));
      buf.Cap();
      show(buf, pc);
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(9)) && ((targ.LEVEL > 35) && (targ.LEVEL < 40))){
      Sprintf(buf, "%S is not for gods!\n", &(obj.ob->short_desc));
      buf.Cap();
      show(buf, pc);
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(19)) && (targ.isNPC())) {
      Sprintf(buf, "%S is for players only!\n", 
		&(obj.ob->short_desc));
      buf.Cap();
      show(buf, pc); 
      return FALSE;
   }//if
   if ((obj.OBJ_FLAGS.get(20)) && (targ.isPc())) { //!pc
      Sprintf(buf, "%S is not for players!\n", &(obj.ob->short_desc));
      buf.Cap(); 
      show(buf, pc); //few should ever see this!!
      return FALSE;
   }//if
   return TRUE;
}//source_give_to


// Returns TRUE if can be removed, FALSE otherwise.
int obj_remove_by(object& obj, critter& pc) {
   String buf(100);

   //mudlog.log(DBG, "In obj_remove_by.\n");

   if (obj.OBJ_FLAGS.get(6)) {
      Sprintf(buf, "The %S seems to have grown very attached to you!!\n", 
              &(obj.ob->short_desc));
      buf.Cap();
      show(buf, pc);
      return FALSE;
   }//if
   return TRUE;
}//obj_remove_by


// Returns TRUE if can be dropped, FALSE otherwise.
int obj_drop_by(object& obj, critter& pc) {
   String buf(100);

   //mudlog.log(DBG, "In obj_drop_by.\n");

   if (obj.OBJ_FLAGS.get(5)) {
      Sprintf(buf, "Yeech, you can't let go of %S!!\n", 
              &(obj.ob->short_desc));
      buf.Cap();
      show(buf, pc);
      return FALSE;
   }//if
   return TRUE;
}//obj_drop_by


int gain_eq_effects(object& obj, object& bag, critter& pc,
                    short bag_in_inv, short do_msg) { 
   String buf(100);
   List<critter*> tmp_lst(ROOM.getCrits());
   Cell<critter*>  cell(tmp_lst);
   critter* crit_ptr;

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB in gain_eq_effects.\n");
      return -1;
   }//if

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In gain_eq_effects:  obj#:  " << obj.OBJ_NUM << endl;
   }

   if (obj.OBJ_FLAGS.get(62)) { //boats
     pc.CRIT_FLAGS.turn_on(4);
   }//if
     
                /* effects for getting gold coins */

   if (obj.OBJ_FLAGS.get(55)) { //coins
      pc.GOLD += obj.ob->cur_stats[1];  //cost == # of coins
      if (do_msg) {
         Sprintf(buf, "There were %i coins.\n", obj.ob->cur_stats[1]);
         show(buf, pc);
      }//if
      pc.loseInv(&obj); 
      if (obj.IN_LIST) {
         delete &obj; 
         return 0;
      }
   }//if

                   /* messages for !gold*/

   else if (bag_in_inv == -1) {
      if (do_msg) {
         Sprintf(buf, "You get %S.\n", long_name_of_obj(obj, pc.SEE_BIT));
         show(buf, pc);
   
         while ((crit_ptr = cell.next())) {
            if (crit_ptr != &pc) {
               Sprintf(buf, "%S gets %S.\n", 
                    name_of_crit(pc, crit_ptr->SEE_BIT), 
                    long_name_of_obj(obj, crit_ptr->SEE_BIT));
               buf.Cap();
               show(buf, *crit_ptr);
            }//if
         }//while
      }//if
   }//if

   else if (bag_in_inv == 0) {
      if (do_msg) {
         Sprintf(buf, "You get %S from %S.\n", 
              long_name_of_obj(obj, pc.SEE_BIT),
              long_name_of_obj(bag, pc.SEE_BIT));
         show(buf, pc);

         tmp_lst.head(cell);
         while ((crit_ptr = cell.next())) {
            if (crit_ptr != &pc) {
               Sprintf(buf, "%S gets %S from %s %S.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT), 
                 long_name_of_obj(obj, crit_ptr->SEE_BIT), 
                 get_his_her(pc), single_obj_name(bag, crit_ptr->SEE_BIT));
               buf.Cap();
               show(buf, *crit_ptr);
            }//if
         }//while
      }//if
   }//if
   else if (bag_in_inv == TRUE) {
      if (do_msg) {
         Sprintf(buf, "You get %S from your %S.\n",
              long_name_of_obj(obj, pc.SEE_BIT), 
              single_obj_name(bag, pc.SEE_BIT));
         show(buf, pc);

         tmp_lst.head(cell);
         while ((crit_ptr = cell.next())) {
            if (crit_ptr != &pc) {
               Sprintf(buf, "%S gets %S from %S.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT), 
                 long_name_of_obj(obj, crit_ptr->SEE_BIT), 
                 long_name_of_obj(bag, crit_ptr->SEE_BIT));
               buf.Cap();
               show(buf, *crit_ptr);
            }//if
         }//while
      }//if
   }//if
   else {
      Sprintf(buf, "ERROR:  bag_in_inv is something wierd: %i.\n", 
              bag_in_inv);
      mudlog.log(ERR, buf);
   }//else
   
   if (mudlog.ofLevel(DBG)) {
      mudlog << "About to checkForProcs, gain_eq_effects:  pc:  "
             << flush
             << name_of_crit(pc, ~0) << flush << "  object:  "  
             << obj.OBJ_NUM << endl << flush;
   }

   mudlog.log(DBG, "All done w/gain_eq_effects.\n");

   String cmd = "get";
   ROOM.checkForProc(cmd, NULL_STRING, pc, obj.OBJ_NUM);
   return 0;
}//gain_eq_effects



int wear_eq_effects(object& obj, critter& pc, int posn, short do_msg) { 
            //lights, stat adjusts
   String buf(100);
   short flag = FALSE;

   //  mudlog.log(DBG, "In wear_eq_effects.\n");

                     /*  Lights  */
   if (obj.isLit()) {
      pc.CRIT_FLAGS.turn_on(1); //turn on using_light flag
      ROOM.checkLight(FALSE);
   }//if
                      /* do stat_affects now */
   Cell<stat_spell_cell*> cell(obj.ob->stat_affects);
   stat_spell_cell* st_ptr;
   while ((st_ptr = cell.next())) {
      if (st_ptr->stat_spell <= MOB_SHORT_CUR_STATS) //ignore food/drink/drug
         pc.short_cur_stats[st_ptr->stat_spell] += 
              st_ptr->bonus_duration;
   }//while

   if (posn == 9 || posn == 10) {
      if (posn == 9) {
	pc.CRIT_FLAGS.turn_on(16);
      }//if
      int pl;
      if (obj.OBJ_FLAGS.get(43)) {
         if ((pl = get_percent_lrnd(DAGGER_SKILL_NUM, pc)) > -1)
	    pc.DAM += (pl / 20);
      }//if
      else if (obj.OBJ_FLAGS.get(41)) { 
	if ((pl = get_percent_lrnd(SWORD_SKILL_NUM, pc)) > -1) {
	  pc.DAM += (pl / 20);
	  int pl2;
	  if ((pl2 = get_percent_lrnd(FENCING_SKILL_NUM, pc)) > -1) {
             pc.DAM += (pl / 20);
	  }//if
	}//if
      }//if
      else if (obj.OBJ_FLAGS.get(42)) { 
         if ((pl = get_percent_lrnd(MACE_SKILL_NUM, pc)) > -1)
	    pc.DAM += (pl / 20);
      }//if
      else if (obj.OBJ_FLAGS.get(44)) { 
         if ((pl = get_percent_lrnd(WHIP_SKILL_NUM, pc)) > -1)
	    pc.DAM += (pl / 20);
      }//if
      else if (obj.OBJ_FLAGS.get(47)) { 
         if ((pl = get_percent_lrnd(BOW_SKILL_NUM, pc)) > -1)
	    pc.DAM += (pl / 20);
      }//if
   }//if wield or holding

            /* do spec_procs, takes care of ww_descs etc */
   if (obj.OBJ_FLAGS.get(63)) { //if has spec_data
      int k = Obj_Wear_Procs_Mask.max_bit();
      for (int i = 0; i < k ; i++) {
         if (Obj_Wear_Procs_Mask.get(i) && obj.OBJ_SPEC_FLAGS.get(i)) {
            flag = TRUE;
            do_this_obj_proc(OBJ_WEAR_PROC, i, pc, obj, posn);
         }//if
      }//for
   }//if
   if (do_msg) {
      if (!flag) {   //default wear proc
         do_this_obj_proc(OBJ_WEAR_PROC, -1, pc, obj, posn); 
      }//if
   }//if

   String cmd = "wear";
   ROOM.checkForProc(cmd, NULL_STRING, pc, obj.OBJ_NUM);
   return 0;
}//wear_eq_effects


int consume_eq_effects(object& obj, critter& pc, short do_msg) { 
   String buf(100);
   //mudlog.log(DBG, "In consume_eq_effects.\n");

   if (!((obj.OBJ_FLAGS.get(60)) || (obj.OBJ_FLAGS.get(61)))) {
      mudlog.log(ERR, "ERROR: consuming something that ain't edible.\n");
      return -1;
   }//if 

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  pc is MOB in consume_eq_effects.\n");
      return -1;
   }//if

                    /* do spec_procs */
   if (obj.OBJ_FLAGS.get(63)) { //if has spec_data
      int k = Obj_Consume_Procs_Mask.max_bit();
      for (int i = 0; i < k ; i++) {
         if (Obj_Consume_Procs_Mask.get(i) && obj.OBJ_SPEC_FLAGS.get(i)) {
            do_this_obj_proc(OBJ_CONSUME_PROC, i, pc, obj, FALSE);
         }//if
      }//for
   }//if

   if (obj.OBJ_FLAGS.get(60)) { //liquid
      Sprintf(buf, "drinks %S.\n", &(obj.ob->short_desc));
   }//if
   else {
      Sprintf(buf, "eats %S.\n", &(obj.ob->short_desc));
   }//else
   emote(buf, pc, ROOM, FALSE);

   String cmd = "eat";
   ROOM.checkForProc(cmd, NULL_STRING, pc, obj.OBJ_NUM);

   if (!pc.isPc()) //ie not a pc
      return -1;

   //   mudlog.log(DBG, "About to do stat_affects.\n");

                      /* do stat_affects now */
   Cell<stat_spell_cell*> cell(obj.ob->stat_affects);
   stat_spell_cell* st_ptr;
   while ((st_ptr = cell.next())) {
      switch (st_ptr->stat_spell) {
         case THIRST_ID:
            pc.THIRST += (int)((float)(st_ptr->bonus_duration) * 2.5);
            if (pc.THIRST < 0)
               pc.THIRST = 0;
            if ((st_ptr->bonus_duration > 0) && (do_msg)) 
               show("You feel less thirsty.\n", pc);
            else if (do_msg)
               show("You feel even more thirsty.\n", pc);
            break;
         case HUNGER_ID: //hunger
            pc.HUNGER += (int)((float)(st_ptr->bonus_duration) * 2.5);
            if (pc.HUNGER < 0)
               pc.HUNGER = 0;
            if ((st_ptr->bonus_duration > 0) && (do_msg))
               show("You feel less hungry.\n", pc);
            else if (do_msg)
               show("You feel even hungrier.\n", pc);
            break;
         case DRUGGED_ID: //drugged
            pc.DRUGGED += (int)((float)(st_ptr->bonus_duration) * 2.5);
            if (pc.DRUGGED < 0)
               pc.DRUGGED = 0;
            if ((st_ptr->bonus_duration > 0) && (do_msg)) 
               show("You feel a little higher.\n", pc);
            else if (do_msg)
               show("Some feeling returns to your nose!.\n", pc);
            break;
      }//switch
   }//while
   mudlog.log(DBG, "Done w/consume_eq_effects.\n");
   return 0;
}//consume_eq_effects


int remove_eq_effects(object& obj, critter& pc, short from_corpse,
                       short do_msg, int posn) { 
                      //lights, stat adjusts
   String buf(100);
   short flag = FALSE;

   //mudlog.log(DBG, "In remove_eq_effects...");
   
   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB in remove_eq_effects.\n");
      return -1;
   }//if

                     /*  Lights  */
   if (obj.isLightSource()) {
      pc.checkLight(&obj, posn);
      ROOM.checkLight(FALSE);
   }//if light_source

                      /* do stat_affects now */
   Cell<stat_spell_cell*> cell(obj.ob->stat_affects);
   stat_spell_cell* st_ptr;
   while ((st_ptr = cell.next())) {
      if (st_ptr->stat_spell <= MOB_SHORT_CUR_STATS) { //ignore food/drink/drug
         pc.short_cur_stats[st_ptr->stat_spell] -= 
                 st_ptr->bonus_duration;
      }//if
   }//while

   //mudlog.log(DBG, "Checking for weapons..");
   if (posn == 9 || posn == 10) {
      //mudlog.log(DBG, "Was in a weapon position..");
      if (posn == 9) {
	pc.CRIT_FLAGS.turn_off(16); //no more dual wielding
      }//if
      int pl;
      if (obj.OBJ_FLAGS.get(43)) {
         if ((pl = get_percent_lrnd(DAGGER_SKILL_NUM, pc)) > -1)
	    pc.DAM -= (pl / 20);
      }//if
      else if (obj.OBJ_FLAGS.get(41)) { 
	if ((pl = get_percent_lrnd(SWORD_SKILL_NUM, pc)) > -1) {
	  pc.DAM -= (pl / 20);
	  int pl2;
	  if ((pl2 = get_percent_lrnd(FENCING_SKILL_NUM, pc)) > -1) {
	    pc.DAM -= (pl / 20);
	  }//if
	}//if
      }//if
      else if (obj.OBJ_FLAGS.get(42)) { 
         if ((pl = get_percent_lrnd(MACE_SKILL_NUM, pc)) > -1)
	    pc.DAM -= (pl / 20);
      }//if
      else if (obj.OBJ_FLAGS.get(44)) { 
         if ((pl = get_percent_lrnd(WHIP_SKILL_NUM, pc)) > -1)
	    pc.DAM -= (pl / 20);
      }//if
      else if (obj.OBJ_FLAGS.get(47)) { 
         if ((pl = get_percent_lrnd(BOW_SKILL_NUM, pc)) > -1)
	    pc.DAM -= (pl / 20);
      }//if
   }//if wield or holding

   //mudlog.log(DBG, "Doing spec procs.. ");
                    /* do spec_procs */
   if (obj.OBJ_FLAGS.get(63)) { //if has spec_data
      int k = Obj_Remove_Procs_Mask.max_bit();
      for (int i = 0; i < k ; i++) {
         if (Obj_Remove_Procs_Mask.get(i) && obj.OBJ_SPEC_FLAGS.get(i)) {
            flag = TRUE;
            do_this_obj_proc(OBJ_REMOVE_PROC, i, pc, obj, FALSE);
         }//if
      }//for
   }//if

   if (do_msg) {
      if (!flag && !from_corpse) {   /* do generic rem proc */
         do_this_obj_proc(OBJ_REMOVE_PROC, -2, pc, obj, FALSE);
      }//if
   }//if

   String cmd = "remove";
   ROOM.checkForProc(cmd, NULL_STRING, pc, obj.OBJ_NUM);
   return 0;
}//remove_eq_effects


int drop_eq_effects(object& obj, critter& pc, short do_msg) {
   String buf(100);
   List<critter*> tmp_lst(ROOM.getCrits());
   Cell<critter*> cell(tmp_lst);
   critter* crit_ptr;

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB in drop_eq_effects.\n");
      return -1;
   }//if

   if (do_msg) {
      Sprintf(buf, "You drop %S.\n", long_name_of_obj(obj, pc.SEE_BIT));
      show(buf, pc);

      while ((crit_ptr = cell.next())) {
         if (crit_ptr != &pc) {
            Sprintf(buf, "%S drops %S.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT), 
                 long_name_of_obj(obj, crit_ptr->SEE_BIT));
            buf.Cap();
            show(buf, *crit_ptr);
         }//if
      }//while
   }//if

   if (obj.OBJ_FLAGS.get(62)) {
      pc.CRIT_FLAGS.turn_off(4); //no longer has a boat
      Cell<object*> cll(pc.inv);
      object* obj_ptr;
      while ((obj_ptr = cll.next())) {
         if (obj_ptr->OBJ_FLAGS.get(62)) {
            pc.CRIT_FLAGS.turn_on(4); //has boat now
            break;
         }//if
      }//while
      if (!pc.CRIT_FLAGS.get(4)) {
         for (int i = 1; i<MAX_EQ; i++) {
            if (pc.EQ[i]) {
               if (pc.EQ[i]->OBJ_FLAGS.get(62)) {
                  pc.CRIT_FLAGS.turn_on(4); //has boat now
                  break;
               }//if
            }//if
         }//for
      }//if
   }//if obj was a boat

   String cmd = "drop";
   ROOM.checkForProc(cmd, NULL_STRING, pc, obj.OBJ_NUM);
   return 0;
}//drop_eq_effects



int donate_eq_effects(object& obj, critter& pc, short do_msg) {
   String buf(100);
   List<critter*> tmp_lst(ROOM.getCrits());
   Cell<critter*> cell(tmp_lst);
   critter* crit_ptr;

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB in drop_eq_effects.\n");
      return -1;
   }//if

   if (do_msg) {
      Sprintf(buf, "You donate %S.\n", long_name_of_obj(obj, pc.SEE_BIT));
      show(buf, pc);

      while ((crit_ptr = cell.next())) {
         if (crit_ptr != &pc) {
            Sprintf(buf, "%S donates %S.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT), 
                 long_name_of_obj(obj, crit_ptr->SEE_BIT));
            buf.Cap();
            show(buf, *crit_ptr);
         }//if
      }//while
      
      List<critter*> lst2(room_list[DONATE_ROOM].getCrits());
      lst2.head(cell);
      while ((crit_ptr = cell.next())) {
            Sprintf(buf, "%S appears in the room with a popping sound.\n", 
                 long_name_of_obj(obj, crit_ptr->SEE_BIT));
            buf.Cap();
            show(buf, *crit_ptr);
      }//while
   }//if

   if (obj.OBJ_FLAGS.get(62)) {
      pc.CRIT_FLAGS.turn_off(4); //no longer has a boat
      Cell<object*> cll(pc.inv);
      object* obj_ptr;
      while ((obj_ptr = cll.next())) {
         if (obj_ptr->OBJ_FLAGS.get(62)) {
            pc.CRIT_FLAGS.turn_on(4); //has boat now
            break;
         }//if
      }//while
      if (!pc.CRIT_FLAGS.get(4)) {
         for (int i = 1; i<MAX_EQ; i++) {
            if (pc.EQ[i]) {
               if (pc.EQ[i]->OBJ_FLAGS.get(62)) {
                  pc.CRIT_FLAGS.turn_on(4); //has boat now
                  break;
               }//if
            }//if
         }//for
      }//if
   }//if obj was a boat

   String cmd = "donate";
   ROOM.checkForProc(cmd, NULL_STRING, pc, obj.OBJ_NUM);
   return 0;
}//donate_eq_effects


// Returns TRUE if can be put, FALSE otherwise.
int eq_put_by(object& vict, object& bag, critter& pc, short bag_in_inv) {
   String buf(100);
   List<critter*> tmp_lst(ROOM.getCrits());
   Cell<critter*> cell(tmp_lst);
   critter* crit_ptr;

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  MOB in eq_put_by.\n");
      return FALSE;
   }//if

   if (bag.OBJ_FLAGS.get(59)) {
      show("Try fill instead!\n", pc);
      return FALSE;
   }//if

   if (bag.isBulletinBoard()) {
      show("Try post instead.\n", pc);
      return FALSE;
   }

   if ((bag.OBJ_CUR_WEIGHT + vict.OBJ_CUR_WEIGHT) > bag.OBJ_MAX_WEIGHT) {
      Sprintf(buf, "The %S can't hold the %S.\n", 
              name_of_obj(bag, pc.SEE_BIT), name_of_obj(vict, pc.SEE_BIT));
      show(buf, pc);
      return FALSE;
   }//if
   else { //do it
      bag.gainInv(&vict);  //add it to the bag...

      if (!bag_in_inv) {
         Sprintf(buf, "You put %S in %S.\n", 
                 long_name_of_obj(vict, pc.SEE_BIT), 
                 long_name_of_obj(bag, pc.SEE_BIT));
         show(buf, pc);

         while ((crit_ptr = cell.next())) {
            if (crit_ptr != &pc) {
               Sprintf(buf, "%S puts %S in %S.\n", 
                 name_of_crit(pc, crit_ptr->SEE_BIT), 
                 long_name_of_obj(vict, crit_ptr->SEE_BIT),
                 long_name_of_obj(bag, crit_ptr->SEE_BIT));
               buf.Cap();
               show(buf, *crit_ptr);
            }//if
         }//while
      }//if
      else {
         Sprintf(buf, "You put %S in %S.\n", 
                 long_name_of_obj(vict, pc.SEE_BIT),
                 long_name_of_obj(bag, pc.SEE_BIT));
         show(buf, pc);

         tmp_lst.head(cell);
         while ((crit_ptr = cell.next())) {
            if (crit_ptr != &pc) {
               Sprintf(buf, "%S puts %S in %s %S.\n", 
                       name_of_crit(pc, crit_ptr->SEE_BIT), 
                       long_name_of_obj(vict, crit_ptr->SEE_BIT),
                       get_his_her(pc),
                       name_of_obj(bag, crit_ptr->SEE_BIT));
               buf.Cap();
               show(buf, *crit_ptr);
            }//if
         }//while
      }//else 
   }//else
   return TRUE;
}//eq_put_by
