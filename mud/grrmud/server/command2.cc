// $Id: command2.cc,v 1.43 1999/08/20 06:20:04 greear Exp $
// $Revision: 1.43 $  $Author: greear $ $Date: 1999/08/20 06:20:04 $

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

/*	command2.cc, more commands for pcs and immortals */

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
#include "parse.h"
#include <PtrArray.h>
#include "load_wld.h"
#include "SkillSpell.h"

int score(const String* str2, critter& pc) {
   ToStringTypeE st = ST_NORMAL;
   if (strncasecmp(*str2, "long", max(1, str2->Strlen())) == 0) {
      st = (ToStringTypeE)(st | ST_LONG);
   }//if

   String buf(1000);
   pc.toStringStat(&pc, buf, st);
   pc.show(buf);
   return 0;
}//score

int critter::doUngroup(int i_th, const String* vict) {
   SCell<critter*> cll;
   critter* ptr1, *ptr2;
   String buf(81);

   if (isMob()) {
      mudlog.log(ERR, "ERROR: MOB trying to ungroup.\n");
      return -1;
   }//if

   if (vict->Strlen() == 0) { //ungroup all
      if (!GROUPEES.isEmpty() && (!FOLLOWER_OF)) { //is a leader
         GROUPEES.head(cll); /* run through all groupees. */
         while ((ptr1 = cll.next())) {
            FOLLOWERS.loseData(ptr1); /* Remove them from the followers. */
            if (ptr1 != this) {
               ptr1->FOLLOWER_OF = NULL; //follow self
               ptr1->show(CS_LEADER_GONE);
               ptr1->show(CS_FOLLOW_SELF);
            }//if
         }//while
         GROUPEES.clear();
         show(CS_HAVE_DISBANDED);
      }//if is a leader
      else if (FOLLOWER_OF) { //possibly in group, not leader
         critter* hack = this;
         FOLLOWER_OF->FOLLOWERS.loseData(hack);
         if (FOLLOWER_OF->GROUPEES.haveData(hack)) {
            FOLLOWER_OF->GROUPEES.loseData(hack);

            FOLLOWER_OF->GROUPEES.head(cll);
            while ((ptr1 = cll.next())) {   
               Sprintf(buf, cstr(CS_HAS_LEFT_GROUP, *ptr1),
                       getName(ptr1->SEE_BIT));
               buf.Cap();
               ptr1->show(buf);
            }//if
         }//while
         else {
            Sprintf(buf, cstr(CS_QUITS_FOL_YOU, *(FOLLOWER_OF)),
                    getName(FOLLOWER_OF->SEE_BIT));
            buf.Cap();
            FOLLOWER_OF->show(buf);
         }//if
         FOLLOWER_OF = NULL;
         show(CS_FOLLOW_SELF);
      }//if grouped but not a leader
      else { //not in a group
         show(CS_OK);
      }//else
   }//if no target
   else { //target mentioned
      ptr1 = have_crit_named(GROUPEES, i_th, vict, ~0, *(getCurRoom()));
      if (!ptr1) {
         show(CS_NOT_IN_GROUP);
         return -1;
      }//if

      GROUPEES.loseData(ptr1);  //no longer in group
      FOLLOWERS.loseData(ptr1); //no longer a follower

      GROUPEES.head(cll);
      while ((ptr2 = cll.next())) {   
         Sprintf(buf, cstr(CS_HAS_LEFT_GROUP, *ptr2),
                 ptr1->getName(ptr2->SEE_BIT));
         buf.Cap();
         ptr2->show(buf);
      }//while

      ptr1->FOLLOWER_OF = NULL;
      ptr1->show(CS_FOLLOW_SELF);
      show(CS_OK);

      String cmd = "ungroup";
      getCurRoom()->checkForProc(cmd, NULL_STRING, *this, ptr1->MOB_NUM);

   }//else
   return 0;
}//doUngroup()


int exit(critter& pc) {
   SCell<door*> cll(ROOM.DOORS);
   door* dr_ptr;
   String buf(81);
   int dest;


   if (pc.POS > POS_SIT) {
      pc.show(CS_TOO_RELAXED);
      return -1;
   }//if

   pc.show(CS_DETECT_THESE_EXITS);

   if (pc.isUsingClient())
      show("<DOOR_LIST>", pc);

   while ((dr_ptr = cll.next())) {
      if (detect(pc.SEE_BIT, dr_ptr->getVisBit())) {
         if (!((dr_ptr->isClosed() && dr_ptr->isSecret()) ||
               dr_ptr->isSecretWhenOpen())) {
            if (dr_ptr->getDestination() < 0) 
               dest = (-(dr_ptr->getDestination()));
            else
               dest = dr_ptr->getDestination();
            if (pc.isImmort()) {
               Sprintf(buf, "%P07%S[%i]:%P27", dr_ptr->getDirection(),
                       dr_ptr->getIdNum());
            }
            else {
               Sprintf(buf, "%P07%S:%P27", dr_ptr->getDirection());
            }

            buf.Cap();
            if (detect(pc.SEE_BIT, room_list[dest].getVisBit())) 
               buf += *(room_list[dest].getLongName());
            else
               buf += cstr(CS_CANT_DISCERN, pc);
            if (pc.isImmort()) {
               buf.Append(" [");
               buf.Append(dest);
               buf.Append("]");
            }//if
            buf += "\n";
            pc.show(buf);
         }//if its open, don't show closed exits
      }//if detect
   }//while
   if (pc.isUsingClient())
      show("</DOOR_LIST>\n\n", pc);
   else 
      show("\n\n", pc);
   return 0;
}//exits()


int auto_exit(critter& pc) { //more brief than the previous
   SCell<door*> cll(ROOM.DOORS);
   door* dr_ptr;
   String buf(81);
   String reg_disp(100);
   String client_disp(100);
   int dest;

   /* this function is called by 'look' btw. */
   /* assumes the person is standing, it just entered a room... */

   client_disp = "< EXITS ";
   reg_disp = cstr(CS_VIS_EXITS, pc);
   while ((dr_ptr = cll.next())) {
      if (detect(pc.SEE_BIT, dr_ptr->getVisBit())) {
         if (!((dr_ptr->isClosed() && dr_ptr->isSecret()) ||
               dr_ptr->isSecretWhenOpen())) {
            dest = abs(dr_ptr->getDestination());
            if (pc.isImmort()) { //if immortal, show extra info
               Sprintf(buf, "%s[%i] ", dr_ptr->getAbrevDir(), dest);
            }//if
            else {
               Sprintf(buf, "%s ", dr_ptr->getAbrevDir());
            }//else
            reg_disp.Append(buf);

            if (pc.USING_CLIENT) {
               client_disp.Append(dr_ptr->getAbrevDir());
               client_disp.Append(" ");
            }//if
         }//if its open, don't show closed exits
      }//if detect
   }//while
   client_disp.Append(">");
   reg_disp.Append("\n");
   
   pc.show(reg_disp);
   
   if (pc.USING_CLIENT) {
      pc.show(client_disp);
   }
   return 0;
}//auto_exits()


int lock(int i_th, const String* name, critter& pc) {
   door* dr_ptr = NULL;
   object* ob_ptr = NULL;
   String buf(81);

   if (ok_to_do_action(NULL, "FSP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
   
      dr_ptr = ROOM.findDoor(i_th, name, pc);
      if (dr_ptr) {
         if (dr_ptr->isSecret()) {
            if (!name_is_secret(name, *dr_ptr)) {
               pc.show(CS_NO_SEE_EXIT);
               return -1;
            }//if
         }//if	    
         if (dr_ptr->isClosed()) {
            if (!dr_ptr->canOpen()) {
               Sprintf(buf, cstr(CS_CANT_BE_LOCKED, pc),
                       name_of_door(*(dr_ptr), pc.SEE_BIT));
               show(buf, pc);
            }//if
            else if (dr_ptr->isLocked()) {
               pc.show(CS_ALREADY_LOCKED);
            }//if
            else { //lock it
               if (pc.haveObjNumbered(1, dr_ptr->getKeyNum())) {
                  Sprintf(buf, cstr(CS_YOU_LOCK, pc), dr_ptr->getName(&pc));
                  show(buf, pc);
                  dr_ptr->lock();

                  //TODO:  Translation problem
                  Sprintf(buf, "locks the %S.\n", name_of_door(*dr_ptr, ~0));
                  emote(buf, pc, ROOM, TRUE);
                  
                  String cmd = "lock";
                  ROOM.checkForProc(cmd, NULL_STRING, pc,
                                    dr_ptr->getIdNum());
                  
                  return 0;
               }//if have key
               else {
                  pc.show(CS_NEED_KEY);
               }//else
            }//else
         }//if closed
         else {
            pc.show(CS_MUST_CLOSE);
         }//else
      }//
      else {   //check for items to be opened
         ob_ptr = pc.haveObjNamed(i_th, name);
         if (ob_ptr) {
            if (ob_ptr->bag && !ob_ptr->isModified()) {
               ob_ptr = obj_to_sobj(*ob_ptr, &(pc), TRUE, i_th, name, pc);
            }
         }
         else {
            ob_ptr = ROOM.haveObjNamed(i_th, name, pc);
            
            if (ob_ptr && ob_ptr->bag && !ob_ptr->isModified()) {
               ob_ptr = obj_to_sobj(*ob_ptr, pc.getCurRoom(), TRUE,
                                    i_th, name, pc);
            }
         }//else
         
         if (ob_ptr) {
            if (!ob_ptr->bag) {
               pc.show(CS_NOT_CONTAINER);
            }//if
            else if (ob_ptr->bag->getKey() == 0) {
               pc.show(CS_CANT_BE_LOCKED_CONT);
            }//if
            else if (ob_ptr->isClosed()) { //is closed
               if (ob_ptr->isLocked()) {
                  pc.show(CS_ALREADY_LOCKED);
               }//if
               else {
                  object* key = NULL;
                  int posn = -1;
                  key = have_obj_numbered(pc.inv, 1, ob_ptr->getKeyNum(), pc);
                  if (!key) {
                     for (int i = 1; i<MAX_EQ; i++) {
                        if (pc.EQ[i] && 
                            (pc.EQ[i]->OBJ_NUM == ob_ptr->getKeyNum())) {
                           posn = i;
                           key = pc.EQ[i];
                           break;
                        }//if
                     }//for
                  }//if
                  
                  if (key) {
                     Sprintf(buf, cstr(CS_YOU_LOCK, pc), name_of_obj(*ob_ptr,
                                                                     pc.SEE_BIT));
                     show(buf, pc);
                     ob_ptr->lock();

                     //TODO:  Translation problem.
                     Sprintf(buf, "locks the %S.\n", ob_ptr->getName());
                     emote(buf, pc, ROOM, TRUE);
                     
                     // send message to other side of the door...
                     dr_ptr->getDestRoom()->showAllCept(CS_SHARP_CLICK, dr_ptr);
                     
                     String cmd = "lock";
                     ROOM.checkForProc(cmd, NULL_STRING, pc, ob_ptr->OBJ_NUM);
                     
                     return 0;
                  }//if have key
                  else {
                     pc.show(CS_NEED_KEY);
                  }//else
               }//else
            }//if
            else {
               pc.show(CS_MUST_CLOSE);
            }//else
         }//if have valid object ptr
         else {
            pc.show(CS_NOTHING_TO_LOCK);
         }//else
      }//else
   }
   return -1;
}//lock


int unlock(int i_th, const String* name, critter& pc) {
   door* dr_ptr = NULL;
   object* ob_ptr = NULL;
   String buf(81);

   if (ok_to_do_action(NULL, "SmFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
   
      if ((dr_ptr = ROOM.findDoor(i_th, name, pc))) {
         if (dr_ptr->isSecret()) {
            if (!name_is_secret(name, *dr_ptr)) {
               pc.show(CS_NO_SEE_EXIT);
               return -1;
            }//if
         }//if	    
         if (dr_ptr->isClosed()) {
            if (!dr_ptr->canOpen()) {
               Sprintf(buf, cstr(CS_OPEN_AUTOMAGICALLY, pc),
                       name_of_door(*(dr_ptr), pc.SEE_BIT));
               show(buf, pc);
            }//if
            else if (dr_ptr->isLocked()) {
               object* key = NULL;
               int posn = -1;
               
               //mudlog << "About to try to find key in inv: key_num:  "
               //       << dr_ptr->getKeyNum() << endl;
               //mudlog << "Current room:  " << pc.getCurRoomNum() << endl;
               
               key = have_obj_numbered(pc.inv, 1, dr_ptr->getKeyNum(), pc);
               if (!key) {
                  for (int i = 1; i<MAX_EQ; i++) {
                     if (pc.EQ[i] && 
                         (pc.EQ[i]->OBJ_NUM == dr_ptr->getKeyNum())) {
                        posn = i;
                        key = pc.EQ[i];
                        break;
                     }//if
                  }//for
               }//if
               
               if (key) {
                  if (dr_ptr->consumesKey()) {
                     Sprintf(buf, cstr(CS_UNLOCK_CONSUME, pc),
                             name_of_door(*dr_ptr, pc.SEE_BIT),
                             key->getLongName());
                     if (posn > 0) {
                        pc.EQ[posn] = NULL;
                        remove_eq_effects(*key, pc, FALSE, FALSE, posn);
                     }
                     else {
                        pc.inv.loseData(key);
                        drop_eq_effects(*key, pc, FALSE);
                     }
                     if (key->isModified()) {
                        recursive_init_unload(*key, 0);
                        delete key;
                     }
                     key = NULL;
                  }//if
                  else {
                     Sprintf(buf, cstr(CS_YOU_UNLOCK, pc),
                             name_of_door(*dr_ptr, pc.SEE_BIT));
                  }
                  show(buf, pc);
                  dr_ptr->unlock();

                  //TODO:  Translation problem.
                  Sprintf(buf, "unlocks the %S.\n", dr_ptr->getName());
                  emote(buf, pc, ROOM, TRUE);
       
                  // send message to other side of the door...
                  dr_ptr->getDestRoom()->showAllCept(CS_FAINT_CLICK, dr_ptr);
                  
                  String cmd = "unlock";
                  ROOM.checkForProc(cmd, NULL_STRING, pc, dr_ptr->getIdNum());
                  
                  return 0;
               }//if
               else {
                  pc.show(CS_NO_KEY);
               }//else
            }//if
            else {
               pc.show(CS_NOT_LOCKED);
            }//else
         }//if closed
         else {
            pc.show(CS_NOT_EVEN_CLOSED);
         }//else
      }//
      else {   //check for items to be opened
         ob_ptr = pc.haveObjNamed(i_th, name);
         if (ob_ptr) {
            if (ob_ptr->bag && !ob_ptr->isModified()) {
               ob_ptr = obj_to_sobj(*ob_ptr, &(pc), TRUE,
                                    i_th, name, pc);
            }
         }
         else {
            ob_ptr = ROOM.haveObjNamed(i_th, name, pc);
            
            if (ob_ptr && ob_ptr->bag && !ob_ptr->isModified()) {
               ob_ptr = obj_to_sobj(*ob_ptr, pc.getCurRoom(), TRUE,
                                    i_th, name, pc);
            }
         }//else
         if (ob_ptr) {
            if (!ob_ptr->bag) {
               pc.show(CS_NOT_CONTAINER);
            }//if
            else if (ob_ptr->isClosed()) { //is closed
               if (ob_ptr->isLocked()) {
                  object* key = NULL;
                  int posn = -1;
                  
                  key = have_obj_numbered(pc.inv, 1, ob_ptr->getKeyNum(),
                                          pc);
                  if (!key) {
                     for (int i = 1; i<MAX_EQ; i++) {
                        if (pc.EQ[i] && 
                            (pc.EQ[i]->OBJ_NUM == ob_ptr->getKeyNum())) {
                           posn = i;
                           key = pc.EQ[i];
                           break;
                        }//if
                     }//for
                  }//if
                  
                  if (key) {
                     if (ob_ptr->consumesKey()) {
                        Sprintf(buf, cstr(CS_UNLOCK_CONSUME, pc),
                                ob_ptr->getLongName(&pc),
                                key->getLongName(&pc));
                        if (posn > 0) {
                           pc.EQ[posn] = NULL;
                           remove_eq_effects(*key, pc, FALSE, FALSE, posn);
                        }
                        else {
                           pc.inv.loseData(key);
                           drop_eq_effects(*key, pc, FALSE);
                        }
                        if (key->isModified()) {
                           recursive_init_unload(*key, 0);
                           delete key;
                        }
                        key = NULL;
                     }//if
                     else {
                        Sprintf(buf, cstr(CS_YOU_UNLOCK, pc),
                                ob_ptr->getLongName(&pc));
                     }
                     show(buf, pc);
                     ob_ptr->unlock();
                     
                     //TODO:  Translation problem
                     Sprintf(buf, "unlocks the %S.\n", ob_ptr->getLongName());
                     emote(buf, pc, ROOM, TRUE);
                     
                     String cmd = "unlock";
                     ROOM.checkForProc(cmd, NULL_STRING, pc,
                                       ob_ptr->getIdNum());
                     
                     return 0;
                  }//if
                  else {
                     pc.show(CS_NO_KEY);
                  }//else
               }//if locked
               else {
                  pc.show(CS_NOT_LOCKED);
               }//else
            }//if
            else {
               pc.show(CS_ALREADY_OPEN);
            }//else
         }//if have valid object ptr
         else {
            pc.show(CS_NOTHING_TO_UNLOCK);
         }//else
      }//else
   }
   return -1;
}//unlock


int open(int i_th, const String* name, critter& pc) {
   door* dr_ptr;
   object* ob_ptr;
   String buf(81);
   if (ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
   
      if ((dr_ptr = ROOM.findDoor(i_th, name, pc))) {
         if (dr_ptr->isSecret()) {
            if (!name_is_secret(name, *dr_ptr)) {
               pc.show(CS_NO_SEE_EXIT);
               return -1;
            }//if
         }//if	    
         if (dr_ptr->isClosed()) {
            if (!dr_ptr->canOpen()) {
               Sprintf(buf, cstr(CS_OPEN_AUTOMAGICALLY, pc),
                       dr_ptr->getName(&pc));
               show(buf, pc);
            }//if
            else if ((dr_ptr->isLocked()) || 
                     (dr_ptr->isMagLocked())) {
               //is locked, mag_locked
               Sprintf(buf, cstr(CS_IS_LOCKED, pc), name_of_door(*(dr_ptr),
                                                                 pc.SEE_BIT));
               show(buf, pc);
            }//if
            else {
               dr_ptr->open();
               Sprintf(buf, cstr(CS_YOU_OPEN, pc), name_of_door(*(dr_ptr),
                                                                pc.SEE_BIT));
               show(buf, pc);
               
               //TODO:  Translation problem.
               Sprintf(buf, "opens the %S.\n", name_of_door(*dr_ptr, ~0));
               emote(buf, pc, ROOM, TRUE);
               
               // send message to other side of the door...
               dr_ptr->getDestRoom()->showAllCept(CS_OPENS_QUIETLY, dr_ptr);
               
               String cmd = "open";
               ROOM.checkForProc(cmd, NULL_STRING, pc, dr_ptr->getIdNum());
               
               return 0;
            }//else
         }//if closed
         else {
            pc.show(CS_ALREADY_OPEN);
         }//else
      }//
      else {   //check for items to be opened
         ob_ptr = pc.haveObjNamed(i_th, name);
         if (ob_ptr) {
            if (ob_ptr->bag && !ob_ptr->isModified()) {
               ob_ptr = obj_to_sobj(*ob_ptr, &(pc), TRUE,
                                    i_th, name, pc);
            }
         }
         else {
            ob_ptr = ROOM.haveObjNamed(i_th, name, pc);
            
            if (ob_ptr && ob_ptr->bag && !ob_ptr->isModified()) {
               ob_ptr = obj_to_sobj(*ob_ptr, pc.getCurRoom(), TRUE,
                                    i_th, name, pc);
            }
         }//else
         
         if (ob_ptr) {
            if (!ob_ptr->bag) {
               pc.show(CS_NOT_CONTAINER);
               return -1;
            }//if
            else if (ob_ptr->isClosed()) { //is closed
               if ((ob_ptr->isLocked()) || (ob_ptr->isMagLocked())) {
                  //is locked, mag_locked
                  Sprintf(buf, cstr(CS_IS_LOCKED, pc), ob_ptr->getLongName(&pc));
                  buf.Cap();
                  show(buf, pc);
               }//if
               else {
                  ob_ptr->open();
                  Sprintf(buf, cstr(CS_YOU_OPEN, pc), ob_ptr->getName(&pc));
                  show(buf, pc);
                  
                  String cmd = "open";
                  ROOM.checkForProc(cmd, NULL_STRING, pc, ob_ptr->OBJ_NUM);
                  
                  return 0;
               }//else
            }//if
            else {
               pc.show(CS_ALREADY_OPEN);
            }//else
         }//if have valid object ptr
         else {
            pc.show(CS_NOTHING_TO_OPEN);
         }//else
      }//else, try to open an object, no door by that name
   }
   return -1;
}//open


int close(int i_th, const String* name, critter& pc) {
   door* dr_ptr;
   object* ob_ptr;
   String buf(81);
   if (ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      if ((dr_ptr = ROOM.findDoor(i_th, name, pc))) {
         if (dr_ptr->isSecret()) {
            if (!name_is_secret(name, *dr_ptr)) {
               pc.show(CS_NO_SEE_EXIT);
               return -1;
            }//if
         }//if
         if (dr_ptr->isOpen()) { //is open
            if (dr_ptr->canClose() &&
                !dr_ptr->isVehicleExit()) { //is closeable
               dr_ptr->close();
               Sprintf(buf, cstr(CS_YOU_CLOSE, pc), dr_ptr->getName(&pc));
               show(buf, pc);
               
               // send message to other side of the door...
               dr_ptr->getDestRoom()->showAllCept(CS_CLOSES_QUIETLY, dr_ptr);
               
               String cmd = "close";
               ROOM.checkForProc(cmd, NULL_STRING, pc, dr_ptr->getIdNum());
               
               return 0;
            }//if
            else { 
               Sprintf(buf, cstr(CS_CANNOT_CLOSE, pc),
                       name_of_door(*(dr_ptr), pc.SEE_BIT));
               show(buf, pc);
            }//else
         }//if open
         else {
            pc.show(CS_ALREADY_CLOSED);
         }//else
      }//
      else {
         ob_ptr = pc.haveObjNamed(i_th, name);
         if (ob_ptr) {
            if (ob_ptr->bag && !ob_ptr->isModified()) {
               ob_ptr = obj_to_sobj(*ob_ptr, &(pc), TRUE,
                                    i_th, name, pc);
            }
         }
         else {
            ob_ptr = ROOM.haveObjNamed(i_th, name, pc);
            
            if (ob_ptr && ob_ptr->bag && !ob_ptr->isModified()) {
               ob_ptr = obj_to_sobj(*ob_ptr, pc.getCurRoom(), TRUE,
                                    i_th, name, pc);
            }
         }//else
         if (ob_ptr) {
            if (!ob_ptr->bag) {
               pc.show(CS_NOT_CONTAINER);
            }//if
            else if (!ob_ptr->getBag()->isOpen()) {
               if (!ob_ptr->getBag()->canClose()) {
                  Sprintf(buf, cstr(CS_CANNOT_CLOSE, pc), ob_ptr->getName(&pc));
                  show(buf, pc);
               }//if
               else { 
                  ob_ptr->getBag()->close();
                  Sprintf(buf, cstr(CS_YOU_CLOSE, pc), ob_ptr->getName(&pc));
                  show(buf, pc);
                  
                  String cmd = "close";
                  ROOM.checkForProc(cmd, NULL_STRING, pc, ob_ptr->OBJ_NUM);
                  
                  return 0;
               }//else
            }//if
            else {
               pc.show(CS_ALREADY_CLOSED);
            }//else
         }//if have valid object ptr
         else {
            pc.show(CS_NOTHING_TO_CLOSE);
         }//else
      }//else, try to open an object, no door by that name
   }
   return -1;
}//close


int save(critter& pc) {

   if (!pc.isPc()) {
      mudlog.log(ERR, "ERROR:  npc tried to save, was blocked...\n");
      return -1;
   }//if

   pc.show(CS_SAVING);
   pc.save();
   return 0;
}//save


int nogossip(critter& pc) {

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR: MOB trying to go nogossip.\n");
      return -1;
   }//if

   if (pc.CRIT_FLAGS.get(6)) 
      pc.show(CS_NO_GOSSIP);
   else
      pc.show(CS_CAN_GOSSIP);
   pc.CRIT_FLAGS.flip(6); //should toggle gossip channel
   return 0;
}//nog


int eat(int i_th, const String* name, critter& pc) {
   object* obj_ptr;
   String buf(100);

   if (ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
   
      obj_ptr = have_obj_named(pc.inv, i_th, name, pc.SEE_BIT, ROOM);

      if (!obj_ptr) {
         Sprintf(buf, cstr(CS_DONT_SEEM_TO_HAVE, pc), name);
         show(buf, pc);
      }//if
      else if (!(obj_ptr->OBJ_FLAGS.get(61))) {
         Sprintf(buf, cstr(CS_CANT_EAT, pc), name);
         show(buf, pc);
      }//if
      else if (pc.pc && pc.HUNGER > HUNGER_MAX) {
         pc.show(CS_TOO_FULL);
      }//if
      else { //then i spose we'll eat!!
         consume_eq_effects(*obj_ptr, pc, TRUE); //do all effects of food
         pc.loseInv(obj_ptr); // rid inv of ptr
         
         recursive_init_unload(*obj_ptr, 0);
         if (obj_ptr->isModified())
            delete obj_ptr;
         return 0;
      }//else
   }
   return -1;
}//eat


int drink(int i_th, const String* name, critter& pc) {
   object* obj_ptr, *obj2;
   String buf(100);
   short in_inv = TRUE;

   if (ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      if (name->Strlen() == 0) {
         pc.show(CS_DRINK_FROM_WHAT);
         return -1;
      }//if

      obj_ptr = pc.haveObjNamed(i_th, name);
      
      if (!obj_ptr) {
         obj_ptr = ROOM.haveObjNamed(i_th, name, pc);
         in_inv = FALSE;
      }//if
      if (!obj_ptr) {
         Sprintf(buf, cstr(CS_AINT_GOT, pc), name);
         show(buf, pc);
      }//if
      else {  //ok, got some kind of object...lets test it
         if (obj_ptr->getInv().isEmpty()) {
            if (mudlog.ofLevel(INF)) {
               mudlog << "INFO:  container has no inventory, obj# "
                      << obj_ptr->getIdNum() << endl;
            }
         }
         
         if (!(obj_ptr->isCanteen())) {
            Sprintf(buf, cstr(CS_NO_LIQ_CONT, pc), name);
            show(buf, pc);
         }//if
         else if (pc.pc && pc.THIRST > THIRST_MAX) {
            pc.show(CS_TOO_FULL_TO_DRINK);
         }//if
         else if (obj_ptr->extras[0] == 0) { 
            Sprintf(buf, cstr(CS_CONT_EMPTY_SPRINTF, pc),
                    obj_ptr->getShortName(&pc));
            show(buf, pc);
         }//if 
         else if (obj_ptr->getInv().isEmpty()) {
            Sprintf(buf, cstr(CS_NO_LIQ_IN_CONT, pc),
                    &(obj_ptr->short_desc));
            buf.Cap();
            show(buf, pc);
         }//if 
         else if (!obj_ptr->isModified()) {
            if (in_inv) {
               obj2 = obj_to_sobj(*obj_ptr, &(pc), TRUE, i_th,
                                  name, pc);
            }//if
            else {
               obj2 = obj_to_sobj(*obj_ptr, pc.getCurRoom(), TRUE, i_th,
                                  name, pc);
            }//else
            object* obj3 = obj2->getInv().peekFront();
            obj2->CHARGES--;
            consume_eq_effects(*obj3, pc, TRUE);
            return 0;
         }//if
         else {
            obj2 = obj_ptr->getInv().peekFront();
            obj_ptr->CHARGES--;
            consume_eq_effects(*obj2, pc, TRUE);
            return 0;
         }//else
      }//else, got an object
   }
   return -1;
}//drink



int fill(int i_th, const String* targ, int j_th, const String* source, 
          critter& pc) {
   String buf(100);
   object* targ_obj, *source_obj;
   short in_inv_source = TRUE;
                    /* get pointers to each */
   if (ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      if (targ->Strlen() == 0) {
         pc.show(CS_FILL_WHAT);
         return -1;
      }//if

      if (source->Strlen() == 0) {
         pc.show(CS_FILL_FROM_WHERE);
         return -1;
      }//if

      targ_obj = pc.haveObjNamed(i_th, targ);
      source_obj = ROOM.haveObjNamed(j_th, source, pc);
      if (!source_obj) {
         source_obj = pc.haveObjNamed(i_th, source);
         in_inv_source = TRUE;
      }//if
      else {
         in_inv_source = FALSE;
      }
      
      if (!targ_obj) {
         Sprintf(buf, cstr(CS_DONT_SEEM_TO_HAVE, pc), targ);
         show(buf, pc);
      }//
      else if (!source_obj) {
         Sprintf(buf, cstr(CS_CANT_FIND, pc), source);
         show(buf, pc);
      }//
      else if (source_obj == targ_obj) {
         pc.show(CS_EMPTY_SELF);
      }//if
      else if (!(source_obj->OBJ_FLAGS.get(59)) || !(source_obj->bag)) {
         Sprintf(buf, cstr(CS_NOT_LIQ_CONT_FILL, pc),
                 &(source_obj->short_desc));
         show(buf, pc);
      }//if      
      else if (!(targ_obj->OBJ_FLAGS.get(59)) || !(targ_obj->bag)) {
         Sprintf(buf, cstr(CS_NOT_LIQ_CONT_FILL, pc),
                 &(targ_obj->short_desc));
         show(buf, pc);
      }//if      
                  /* so both are containers */

      else if ((targ_obj->getInv().peekFront() !=
                source_obj->getInv().peekFront()) && 
               (targ_obj->extras[0] != 0)) {
         Sprintf(buf, cstr(CS_MUST_EMPTY, pc), targ_obj->getName(&pc));
         show(buf, pc);
      }//if
                  /* ok, liquids are the same */

      else if (source_obj->extras[0] == 0) {
         Sprintf(buf, cstr(CS_EMPTY_ALREADY, pc), source_obj->getName(&pc));
         show(buf, pc);
      }//if
      else if (targ_obj->extras[0] >= targ_obj->bag->getMaxWeight()) {
         Sprintf(buf, cstr(CS_FULL_ALREADY, pc), targ_obj->getName(&pc));
         show(buf, pc);
      }//if
      else if (targ_obj->extras[0] == -1) {
         pc.show(CS_NEVER_NEED_FILLING);
      }
                   /* All clear to exchange liquids ;) */

      else {		/* take care of Obj to Sobj if needed. */
         mudlog.log(TRC, "About to take care of obj to sobj.\n");
         if (!targ_obj->isModified()) {
            targ_obj = obj_to_sobj(*targ_obj, &(pc), TRUE, i_th,
                                   targ, pc);
         }//if
         
         if (mudlog.ofLevel(DBG)) {
            mudlog << "in fill, targ: " << targ_obj->getName() << " num: " 
                   << targ_obj->getIdNum() << "  source: "
                   << source_obj->getName() << endl;
         }
         
         SCell<object*> cll(targ_obj->inv);
         object* ptr;
         while ((ptr = cll.next())) {
            recursive_init_unload(*ptr, 0);
         }//while
         clear_obj_list(targ_obj->inv);
         
         //TODO, check for SOBJ:  Answer, allways use OBJ, not SOBJ
         targ_obj->gainInv(&(obj_list[source_obj->getInv().peekFront()->getIdNum()]));
         
         /* test for infinite source */
         if (source_obj->extras[0] <= -1) {
            if (source_obj->extras[0] < -1) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "WARNING: Was an infinite source, < -1: "
                         << source_obj->extras[0] << " obj#: " 
                         << source_obj->getIdNum() << endl;
               }
            }
            else {
               mudlog.log(TRC, "Was an infinite source.\n");
            }
            targ_obj->extras[0] = targ_obj->bag->getMaxWeight();
         }//if
                     /* not an infinite source */
         else {
            mudlog.log(TRC, "Not an infinite source.\n");
            if (!source_obj->isModified()) {
               if (in_inv_source) {
                  source_obj = obj_to_sobj(*source_obj, &(pc), TRUE, i_th,
                                           source, pc);
               }//if in inv
               else {
                  source_obj = obj_to_sobj(*source_obj, pc.getCurRoom(), TRUE, i_th,
                                           source, pc);
               }//else
            }//if
            /* TODO: this next bit is badly innefficient. */
            while ((targ_obj->extras[0] != 
                    targ_obj->bag->getMaxWeight()) &&
                   (source_obj->extras[0] > 0)) {
               targ_obj->extras[0]++;
               source_obj->extras[0]--;
            }//while
         }//else
         Sprintf(buf, cstr(CS_YOU_FILL, pc), targ_obj->getShortName(&pc),
                 source_obj->getLongName(&pc));
         show(buf, pc);
         
         String cmd = "fill";
         ROOM.checkForProc(cmd, NULL_STRING, pc, targ_obj->OBJ_NUM);
         return 0;
      }//else
   }
   return -1;
}//fill()


int empty(int i_th, const String* canteen, critter& pc) {
   String buf(100);
   object* obj2;
   if (ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      object* obj_ptr = pc.haveObjNamed(i_th, canteen);

      if (!obj_ptr) {
         Sprintf(buf, cstr(CS_DONT_SEEM_TO_HAVE, pc), canteen);
         show(buf, pc);
      }//if
      else if (!(obj_ptr->OBJ_FLAGS.get(59))) {
         Sprintf(buf, cstr(CS_NOT_LIQ_CONT_EMPTY, pc),
                 obj_ptr->getShortName(&pc));
         show(buf, pc);
      }//if
                  /* got a valid canteen */
   
      else if (obj_ptr->extras[0] == 0) {
         Sprintf(buf, cstr(CS_CONT_EMPTY_SPRINTF, pc),
                 obj_ptr->getShortName(&pc));
         show(buf, pc);
      }//if
      else if (obj_ptr->extras[0] == -1) {
         Sprintf(buf, cstr(CS_NEVER_EMPTY, pc),
                 obj_ptr->getShortName(&pc));
         show(buf, pc);
      }//if
                   /* ok, procede w/emptying */

      else {
         if (!obj_ptr->isModified()) {
            obj2 = obj_to_sobj(*obj_ptr, &(pc), TRUE, i_th,
                               canteen, pc);
            obj_ptr = obj2;
         }//if

         object* ptr;
         while (!(obj_ptr->inv.isEmpty())) {
            ptr = obj_ptr->inv.popFront();
            recursive_init_unload(*ptr, 0);
            if (ptr->isModified()) {
               delete ptr;
            }//if
         }//while

         obj_ptr->extras[0] = 0; //no charges
         Sprintf(buf, cstr(CS_YOU_EMPTY, pc), obj_ptr->getShortName(&pc));
         show(buf, pc);
         return 0;
      }//else
   }
   return -1;
}//empty()


int help(int i_th, String* command, String* mort, critter& pc) {
   String buf(100);
   String cmd(50);
   String page;

   if (!pc.isPc()) {
      return -1;
   }//if

   String parsed_cmd(parse_hlp_command(*command));

   if (mudlog.ofLevel(DBG)) {
      mudlog << "help:  parsed_cmd -:" << parsed_cmd << ":-  cmd -:"
             << *command << ":- mort: " << *mort << "\n";
   }

   if (parsed_cmd.Strlen() == 0) {
      // short cut this to look for skill/spell help first
      if (SSCollection::instance().doHelpFor(*command, pc) >= 0) {
         return 0;
      }
   }

   if (pc.pc->imm_data && (strcasecmp(*mort, "mortal") != 0)) {
      if (parsed_cmd.Strlen()) 
         Sprintf(cmd, "./Help/IMM_%S_%i", &parsed_cmd, i_th);
      else
         Sprintf(cmd, "./Help/IMM_help_%i", i_th);

      page = get_page(cmd);

      if (page.Strlen() > 0) {
         if (!pc.isUsingClient()) {
            strip_hegemon_tags(page);
            pc.show(page);
            return 0;
         }
         else {
            show(page, pc);
            return 0;
         }
      }//if
   }//if, first check IMM helps for immortal types

   if (parsed_cmd.Strlen()) 
      Sprintf(cmd, "./Help/%S_%i", &parsed_cmd, i_th);
   else                 
      Sprintf(cmd, "./Help/help_%i", i_th);

//   mudlog.log(TRC, "Here is the cmd we are looking for:");
//   mudlog.log(TRC, cmd);

   page = get_page(cmd);

   if (page.Strlen() > 0) {
      Sprintf(buf, cstr(CS_HELP_FOR_CMD, pc), &parsed_cmd);
      pc.show(buf);
      if (!pc.isUsingClient()) {
         strip_hegemon_tags(page);
         pc.show(page);
      }
      else {
         pc.show(page);
      }
      return 0;
   }//if
   else {
      Sprintf(buf, cstr(CS_NO_HELP, pc), i_th, command);
      show(buf, pc);
      pc.show(CS_NO_HELP_2);
      return -1;
   }//else
}//help


String get_page(const char* path_name) { 
   String page(1750);
   String buf(100);
   int count = 0;
   ifstream file(path_name);

   if (!file) {  //then help not available
      return page;
   }//if

                   /* find starting place */
   buf.Getline(file, 99);
   while (file) { //check for eof
      count++;
      page += buf;
      page += "\n";
      buf.Getline(file, 99);
   }//while
   return page;
}//get_page()
    


int buy(int i_th, const String* item, int j_th, const String* keeper,
        critter& pc) {
   String buf(100);
   critter* crit_ptr;

   mudlog.log(TRC, "In buy.\n");

   // Check for:  Standing, !battle, Paralyzed, Frozen
   if (!ok_to_do_action(NULL, "mSBPF", 1, pc)) {
      return -1;
   }//if

   if (keeper->Strlen() == 0) {
      crit_ptr = ROOM.findFirstShopKeeper();
      if (!crit_ptr) { //check for vending machine of some type
         SCell<object*> cll(ROOM.getInv());
         object* ptr;
         while ((ptr = cll.prev())) {
            if (ptr->OBJ_FLAGS.get(73)) { //if its a vend machine
               do_vend_buy(*ptr, i_th, item, pc);
               return 0;
            }//if
         }//while
      }//else
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(j_th, keeper, pc.SEE_BIT);
      if (!crit_ptr) {  //check for vending machine of some type
         object* ptr = ROOM.haveObjNamed(j_th, keeper, pc);
         if (ptr && ptr->OBJ_FLAGS.get(73)) { //if its a vend machine
            do_vend_buy(*ptr, i_th, item, pc);
            return 0;
         }//if
      }//else
   }//else

   if (!crit_ptr) {
      pc.show(CS_NO_CAN_SEE_PERSON);
   }//if
   else if (!crit_ptr->isNpc()) { //dealing w/a pc...
      Sprintf(buf, cstr(CS_TRADE_PC, pc),
              crit_ptr->getName(&pc));
      buf.Cap();
      show(buf, pc);
   }//if
   else if (!crit_ptr->mob->proc_data) { //not a shop keeper
      do_tell(*crit_ptr, cstr(CS_NOTHING_TO_SELL, pc), pc, 
	      FALSE, pc.getCurRoomNum());
   }//if
   else if (!(crit_ptr->FLAG1.get(1))) {
      do_tell(*crit_ptr, cstr(CS_NOT_TRADER, pc), pc, FALSE,
              pc.getCurRoomNum());
   }//if
   else {                  /* ok, is a shopkeeper */
      mudlog.log(TRC, "Is a shopkeeper.\n");
      return do_buy_proc(0, *crit_ptr, i_th, item, pc);
   }//else, is shopkeeper
   return -1;
}//buy()


int sell(int i_th, const String* item, int j_th, const String* keeper,
         critter& pc) {
   String buf(100);
   critter* crit_ptr;

   mudlog.log(TRC, "In sell.\n");

   // Check for:  Standing, !battle, Paralyzed, Frozen,
   if (!ok_to_do_action(NULL, "mSBPF", 1, pc)) {
      return -1;
   }//if

   if (keeper->Strlen() == 0) {
      crit_ptr = ROOM.findFirstShopKeeper();
   }//if
   else {
      mudlog.log(TRC, "Searching room for Shopkeeper, in SELL.\n");
      crit_ptr = ROOM.haveCritNamed(j_th, keeper, pc);
   }//else


   if (!crit_ptr) {
      pc.show(CS_NO_CAN_SEE_PERSON);
   }//if
   else if (!crit_ptr->mob) {
      Sprintf(buf, cstr(CS_TRADE_PC, pc),
              name_of_crit(*crit_ptr, pc.SEE_BIT));
      buf.Cap();
      show(buf, pc);
   }//if 
   else if (!crit_ptr->mob->proc_data) {
      do_tell(*crit_ptr, cstr(CS_NO_WANT_BUY, pc), pc, 
              FALSE, pc.getCurRoomNum());
   }//if
   else if (!(crit_ptr->FLAG1.get(1))) {
      do_tell(*crit_ptr, cstr(CS_NOT_TRADER, pc), pc, FALSE, pc.getCurRoomNum());
   }//if
   else {                  /* ok, is a shopkeeper */
      return do_sell_proc(1, *crit_ptr, i_th, item, pc);
   }//else, is shopkeeper
   return -1;
}//sell()


int practice(const String* spell, int j_th, const String* master,
              critter& pc) {
   int s_num;
   String buf(100);
   critter* crit_ptr;

   if (spell->Strlen() == 0) {
      return abilities(pc);
   }//if

   if (ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      if (master->Strlen() == 0) {
         crit_ptr = ROOM.findFirstTeacher();
      }//if
      else {
         crit_ptr = ROOM.haveCritNamed(j_th, master, pc.SEE_BIT);
      }//else
      
      if (pc.EQ[9] || pc.EQ[10]) {
         pc.show(CS_NO_PRAC_WIELDING);
      }//else
      else if (pc.PRACS < 1) {
         pc.show(CS_NO_PRACS);
      }//if
      else if (!crit_ptr) {
         pc.show(CS_NO_CAN_SEE_PERSON);
      }//if
      else if (!crit_ptr->mob) {
         pc.show(CS_NO_LEARN_PLAYER);
      }//if
      else if (!crit_ptr->mob->proc_data) {
         do_tell(*crit_ptr, cstr(CS_NOTHING_TO_TEACH, pc), pc, FALSE, 
                 pc.getCurRoomNum());
      }//if
      else if (!(crit_ptr->FLAG1.get(2))) {
         do_tell(*crit_ptr, cstr(CS_NO_TEACH_FOR_LIVING, pc), pc, FALSE,
                 pc.getCurRoomNum());
      }//if
      else {                  /* ok, is a teacher, figure out spell */
         if (!(crit_ptr->TEACH_DATA_FLAGS.get(pc.CLASS))) {
            do_tell(*crit_ptr, cstr(CS_CANT_TEACH_PROFF, pc), pc,
                    FALSE, pc.getCurRoomNum());
         }//if
         else if (crit_ptr->LEVEL < pc.LEVEL) {
            do_tell(*crit_ptr, cstr(CS_TEACH_SURPASSED, pc),
                    pc, FALSE, pc.getCurRoomNum());
         }//if
         else {  		/* need to figure NUMBER of skill or spell */
            s_num = SSCollection::instance().getNumForName(*spell); 
            if (s_num == -1) {
               Sprintf(buf, cstr(CS_NOT_RESEARCHED, pc), spell);
               pc.show(buf);
            }//if
            else {             /* ok, got a valid skill or spell */
               if (SSCollection::instance().getSS(s_num).getMinLevel() > pc.LEVEL) {
                  do_tell(*crit_ptr, cstr(CS_SPELL_TOO_POWERFUL, pc),
                          pc, FALSE, pc.getCurRoomNum());
               }//if
               else {
                  int p_learned = get_percent_lrnd(s_num, pc);
                  if (p_learned == -1) {
                     do_tell(*crit_ptr, cstr(CS_NO_PREREQ, pc), pc,
                             FALSE, pc.getCurRoomNum());
                  }//if
                  else if (p_learned < 100) {
                     increment_percent_lrnd(s_num, pc);
                     pc.show(CS_FEEL_WISER);
                     pc.PRACS--; //decrement the number of practices
                     if ((p_learned < 50) && (get_percent_lrnd(s_num, pc) >= 50)) {
                        update_skill(s_num, pc);
                     }//if
                     return 0;
                  }//if
                  else {
                     pc.show(CS_KNOW_TOO_MUCH);
                  }//else
               }//else
            }//else, got a valid spell/skill
         }// else, correct class of teacher
      }// else, is a teacher
   }
   return -1;
}//practice()


int toggle_prompt(const String* field, critter& pc) {
   String buf(100);
   int len1 = field->Strlen();

   if (!pc.isPc())
      return -1;

   if (len1 == 0) {
      pc.show(CS_TOG_PROMPT);

      Sprintf(buf, cstr(CS_TOG1, pc),
              (int)(pc.PC_FLAGS.get(8)), (int)(pc.PC_FLAGS.get(9)),
              (int)(pc.PC_FLAGS.get(12)));
      show(buf, pc);
      Sprintf(buf, cstr(CS_TOG2, pc),
              (int)(pc.PC_FLAGS.get(4)), (int)(pc.PC_FLAGS.get(3)),
              (int)(pc.PC_FLAGS.get(6)));
      show(buf, pc);

      Sprintf(buf, cstr(CS_TOG3, pc),
              (int)(pc.PC_FLAGS.get(5)), (int)(pc.PC_FLAGS.get(22)),
              (int)(pc.isUsingColor()));
      show(buf, pc);

      Sprintf(buf, cstr(CS_TOG4, pc),
              (int)(pc.PC_FLAGS.get(16)), (int)(pc.PC_FLAGS.get(15)));
      show(buf, pc);

      Sprintf(buf, cstr(CS_TOG4_1, pc), (int)(pc.PC_FLAGS.get(28)),
              (int)(!pc.canBeBeeped()));
      show(buf, pc);

      if (pc.isImmort()) {
         Sprintf(buf, cstr(CS_TOG5, pc),
                 (int)(pc.PC_FLAGS.get(14)), (int)(pc.PC_FLAGS.get(7)),
		 (int)(pc.PC_FLAGS.get(20)));
	 show(buf, pc);
         Sprintf(buf, cstr(CS_TOG6, pc),
                 (int)(pc.PC_FLAGS.get(19)), (int)(pc.PC_FLAGS.get(24)));
	 show(buf, pc);
      }//if
      return 0;
   }//if

   if (pc.isImmort()) {
      if (strncasecmp(*field, cstr(CS_TOG_EXTRA, pc), len1) == 0) {
	 pc.PC_FLAGS.flip(14);
	 pc.show(CS_OK);
	 return 0;
      }//if
      else if (strncasecmp(*field, cstr(CS_TOG_DET, pc), len1) == 0) {
	 pc.PC_FLAGS.flip(19);
	 pc.show(CS_OK);
	 return 0;
      }//if
      else if (strncasecmp(*field, cstr(CS_TOG_NOH, pc), len1) == 0) {
	 pc.PC_FLAGS.flip(7);
	 pc.show(CS_OK);
	 return 0;
      }//if
      else if (strncasecmp(*field, cstr(CS_TOG_NOV, pc), len1) == 0) {
	 pc.PC_FLAGS.flip(20);
	 pc.show(CS_OK);
	 return 0;
      }//if
      else if (strncasecmp(*field, cstr(CS_TOG_WC, pc), len1) == 0) {
         pc.PC_FLAGS.flip(24);
	 pc.show(CS_OK);
	 return 0;
      }//if         
   }//if

   if (strncasecmp(*field, cstr(CS_TOG_TG, pc), len1) == 0) 
      pc.PC_FLAGS.flip(4);
   else if (strncasecmp(*field, cstr(CS_TOG_NO_BEEP, pc), len1) == 0) 
      pc.PC_FLAGS.flip(29); //no_beep
   else if (strncasecmp(*field, cstr(CS_TOG_CT, pc), len1) == 0) 
      pc.PC_FLAGS.flip(5);
   else if (strncasecmp(*field, cstr(CS_TOG_CLOAKED, pc), len1) == 0) 
      pc.PC_FLAGS.flip(3);
   else if (strncasecmp(*field, cstr(CS_TOG_BR, pc), len1) == 0) 
      pc.PC_FLAGS.flip(8);
   else if (strncasecmp(*field, cstr(CS_TOG_CRB, pc), len1) == 0) 
      pc.PC_FLAGS.flip(15);
   else if (strncasecmp(*field, cstr(CS_TOG_CR, pc), len1) == 0) 
      pc.PC_FLAGS.flip(16);
   else if (strncasecmp(*field, cstr(CS_TOG_AUTOSPLIT, pc), len1) == 0) 
      pc.PC_FLAGS.flip(9);
   else if (strncasecmp(*field, cstr(CS_TOG_AUTOLOOT, pc), len1) == 0) 
      pc.PC_FLAGS.flip(12);
   else if (strncasecmp(*field, cstr(CS_TOG_AUTOEXITS, pc), len1) == 0) 
      pc.PC_FLAGS.flip(6);
   else if (strncasecmp(*field, cstr(CS_TOG_PB, pc), len1) == 0) 
      pc.PC_FLAGS.flip(22);
   else if (strncasecmp(*field, cstr(CS_TOG_ANSI, pc), len1) == 0) 
      pc.PC_FLAGS.flip(26);
   else if (strncasecmp(*field, cstr(CS_TOG_MOB_ENTRY, pc), len1) == 0) 
      pc.PC_FLAGS.flip(28);
   else {
      pc.show(CS_NO_FIND_TOGGLE);
      return -1;
   }//else
   
   pc.show(CS_OK);
   return 0;
}//toggle_prompt()



int list_merchandise(int i_th, const String* keeper, critter& pc) {
   String buf(100);
   critter* crit_ptr = NULL;

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR: MOB trying to list_merchandise.\n");
      return -1;
   }//if

   if (ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      if (keeper->Strlen() == 0) {
         crit_ptr = ROOM.findFirstShopKeeper();
         if (!crit_ptr) {
            //check for vending machine of some type
            SCell<object*> cll(ROOM.getInv());
            object* ptr;
            while ((ptr = cll.prev())) {
               if (ptr->OBJ_FLAGS.get(73)) { //if its a vend machine
                  SafeList<object*> dummy(NULL);
                  return do_list_merchandise(ptr->inv, dummy, NULL, ptr, pc);
               }//if
            }//while
         }
      }//else
      else {
         crit_ptr = ROOM.haveCritNamed(i_th, keeper, pc);
         if (!crit_ptr) {
            object* ptr = ROOM.haveObjNamed(i_th, keeper, pc);
            if (ptr && ptr->OBJ_FLAGS.get(73)) { //if its a vend machine
               SafeList<object*> dummy(NULL);
               return do_list_merchandise(ptr->inv, dummy, NULL,
                                          ptr, pc);
            }//if
         }
      }//else

      if (!crit_ptr) {
         pc.show(CS_NO_CAN_SEE_PERSON);
      }//if
      else if (!crit_ptr->mob) {
         pc.show(CS_MORE_DELICATE);
      }//if
      else if (!crit_ptr->mob->proc_data) {
         do_tell(*crit_ptr, cstr(CS_EH, pc), pc, FALSE, pc.getCurRoomNum());
      }//if
      else if (!(crit_ptr->FLAG1.get(1))) {
         do_tell(*crit_ptr, cstr(CS_NOT_TRADER, pc), pc, FALSE, pc.getCurRoomNum());
      }//if
      else {                  /* ok, is a shopkeeper */
               //log("Doing sell proc_1.\n");
         if (!crit_ptr->isOpen(get_game_time(), TRUE, pc)) {
            return -1;
         }

         do_list_merchandise(crit_ptr->inv, crit_ptr->PERM_INV,
                             crit_ptr, NULL, pc);

         String cmd = "list";
         ROOM.checkForProc(cmd, NULL_STRING, pc, crit_ptr->MOB_NUM);
         return 0;
      }//else, is shopkeeper
   }
   return -1;
}//list()


int do_list_merchandise(SafeList<object*>& inv, SafeList<object*>& perm_inv,
                        critter* crit_owner, object* obj_owner, 
                        critter& pc) {
   object* obj_ptr = NULL;
   SCell<object*> cell(inv);
   int price;
   String buf(100);

   static int item_counts[NUMBER_OF_ITEMS + 1];
         
   memset(item_counts, 0, sizeof(int) * (NUMBER_OF_ITEMS + 1));
         
   pc.show(CS_ITEMS_FOR_SALE);

   // Now, find the instance count.
   while ((obj_ptr = cell.next())) {
      item_counts[obj_ptr->getIdNum()]++;
   }//while
         
   perm_inv.head(cell);
   while ((obj_ptr = cell.next())) {
      item_counts[obj_ptr->getIdNum()]++;
   }//while
         
   inv.head(cell);
   int id_num;
   int cnt = 0;
   while ((obj_ptr = cell.next())) {
            
      id_num = obj_ptr->getIdNum();
            
      if (!obj_ptr->isModified() &&
          (item_counts[id_num] == -1)) { //already done it
         continue;
      }
      
      if (detect(pc.SEE_BIT, (obj_ptr->OBJ_VIS_BIT | ROOM.getVisBit()))) {
         if (crit_owner) {
            price = crit_owner->findItemSalePrice(*obj_ptr, pc);
         }
         else if (obj_owner) {
            price = obj_owner->getDefaultPrice();
         }
         else {
            return -1;
         }
      
         if (price < 0) {
            continue; //buf = "  NOT FOR SALE NOW.";
         }//if
         
         if (pc.shouldShowVnums()) {
            if (obj_ptr->isModified() || (item_counts[id_num] == 1)) {
               Sprintf(buf, " [%i]%P06 %S%P50%i", id_num,
                       obj_ptr->getLongName(&pc), price);
            }
            else {
               Sprintf(buf, " [%i]%P06 [*%i]%P12 %S%P50%i", id_num,
                       item_counts[id_num], obj_ptr->getLongName(&pc), price);
            }
         }
         else {
            cnt++;
            if (obj_ptr->isModified() || (item_counts[id_num] == 1)) {
               Sprintf(buf, "  [%i]%P12 %S%P50%i", cnt, obj_ptr->getLongName(&pc),
                       price);
            }
            else {
               Sprintf(buf, "  [%i] [*%i]%P12 %S%P50%i", cnt, item_counts[id_num],
                       obj_ptr->getLongName(&pc), price);
            }
         }
         
         item_counts[id_num] = -1;
         
         //if ((!obj_wear_by(*obj_ptr, pc, -1, FALSE)) &&
         //    (!obj_ptr->isFood())) 
         //   buf.Prepend("**");
         
         if (obj_ptr->OBJ_VIS_BIT & 2)  //if invisible
            buf.Append(" *\n");
         else 
            buf.Append("\n");
         show(buf, pc);
      }//if detectable
   }//while

   perm_inv.head(cell);
   while ((obj_ptr = cell.next())) {
            
      id_num = obj_ptr->getIdNum();
      
      if (!obj_ptr->isModified() &&
          (item_counts[id_num] == -1)) { //already done it
         continue;
      }
      
      if (detect(pc.SEE_BIT, (obj_ptr->OBJ_VIS_BIT | ROOM.getVisBit()))) {
         if (crit_owner) {
            price = crit_owner->findItemSalePrice(*obj_ptr, pc);
         }
         else if (obj_owner) {
            price = obj_owner->getDefaultPrice();
         }
         else {
            return -1;
         }
         
         if (price < 0) {
            continue; //buf = "  NOT FOR SALE NOW.";
         }//if
         
         if (pc.shouldShowVnums()) {
            if (obj_ptr->isModified() || (item_counts[id_num] == 1)) {
               Sprintf(buf, " [%i]%P06 %S%P50%i", id_num,
                       obj_ptr->getLongName(&pc), price);
            }
            else {
               Sprintf(buf, " [%i]%P06 [*%i]%P12 %S%P50%i", id_num,
                       item_counts[id_num], obj_ptr->getLongName(&pc), price);
            }
         }
         else {
            cnt++;
            if (obj_ptr->isModified() || (item_counts[id_num] == 1)) {
               Sprintf(buf, "  [%i]%P12 %S%P50%i", cnt, obj_ptr->getLongName(&pc),
                       price);
            }
            else {
               Sprintf(buf, "  [%i] [*%i]%P12 %S%P50%i", cnt, item_counts[id_num],
                       obj_ptr->getLongName(&pc), price);
            }
         }
         
         item_counts[id_num] = -1;
         
         //if ((!obj_wear_by(*obj_ptr, pc, -1, FALSE)) &&
         //    (!obj_ptr->isFood())) 
         //   buf.Prepend("**");
         
         if (obj_ptr->OBJ_VIS_BIT & 2)  //if invisible
            buf.Append(" *\n");
         else 
            buf.Append("\n");
         show(buf, pc);
      }//if detectable
      
   }//while
   return 0;
}//do_list_merchandise



int offer(int i_th, const String* item, int j_th, const String* keeper,
         critter& pc) {
   String buf(100);
   critter* crit_ptr;

   if (ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      if (keeper->Strlen() == 0) {
         crit_ptr = ROOM.findFirstShopKeeper();
      }//if
      else {
         crit_ptr = ROOM.haveCritNamed(j_th, keeper, pc.SEE_BIT);
      }//else


      if (!crit_ptr) {
         pc.show(CS_NO_CAN_SEE_PERSON);
      }//if
      else if (!crit_ptr->mob) {
         Sprintf(buf, cstr(CS_TAKE_PURSUADING, pc),
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
      }//if 
      else if (!crit_ptr->mob->proc_data) {
         do_tell(*crit_ptr, cstr(CS_NO_WANT_BUY, pc), pc,
                 FALSE, pc.getCurRoomNum());
      }//if
      else if (!(crit_ptr->FLAG1.get(1))) {
         do_tell(*crit_ptr, cstr(CS_NOT_TRADER, pc),
                 pc, FALSE, pc.getCurRoomNum());
      }//if
      else {                  /* ok, is a shopkeeper */
         return do_offer_proc(2, *crit_ptr, i_th, item, pc);
      }//else, is shopkeeper
   }
   return -1;
}//offer()


int abilities(critter& pc) {
   String buf(100);

   if (!pc.isPc()) {
      return -1;
   }//if

   pc.show(CS_KNOW_SKILL_SPELLS);

   int key;
   int retval = 0;
   if (pc.SKILLS_KNOWN.Min(key)) {
      pc.SKILLS_KNOWN.Find(key, retval);
      Sprintf(buf, cstr(CS_SS_PERCENT, pc),
              (const char*)(SSCollection::instance().getNameForNum(key)),
              retval);
      buf.Cap();
      show(buf, pc);
      while (pc.SKILLS_KNOWN.Next(key)) {
         pc.SKILLS_KNOWN.Find(key, retval);
         Sprintf(buf, cstr(CS_SS_PERCENT, pc),
                 (const char*)(SSCollection::instance().getNameForNum(key)),
                 retval);
         buf.Cap();
         show(buf, pc);
      }//while
      return 0;
   }//if
   show("NONE\n", pc);
   return 0;
}//abilities


int wimpy(int i, critter& pc) {
   String buf(100);

   if (ok_to_do_action(NULL, "mFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      if (i == 1) {
         Sprintf(buf, cstr(CS_WIMPY_SET, pc), pc.WIMPY);
         show(buf, pc);
      }//if
      else if (i <= 0) {
         pc.WIMPY = 0;
      }//if
      else if (i > pc.getHpMax() / 2) {
         pc.WIMPY = pc.getHpMax() / 2;
      }//if
      else {
         pc.WIMPY = i;
      }//else
      pc.show(CS_OK);
      return 0;
   }
   return -1;
}//wimpy


int language(const String& lang,  critter& pc) {
   String buf(100);

   if (ok_to_do_action(NULL, "mFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      int len = max(lang.Strlen(), 1);
      if (strncasecmp(lang, "English", len) == 0) {
         pc.pc->preferred_language = English;
      }
      else if ((strncasecmp(lang, "Spanish", len) == 0) ||
               (strncasecmp(lang, "Espanol", len) == 0)) {
         pc.pc->preferred_language = Spanish;
      }
      else if (strncasecmp(lang, "Portugues", len) == 0) {
         pc.pc->preferred_language = Portugues;
      }
      else if (strncasecmp(lang, "Italian", len) == 0) {
         pc.pc->preferred_language = Italian;
      }
      else {
         //TODO:  Translate
         pc.show("Only:  English, Spanish, Portugues and Italian are supported now.\n");
         pc.show("Usage:  language <<language_choice>\n\n");
         return -1;
      }

      pc.PC_FLAGS.turn_on(27);
      pc.show(CS_OK);
      return 0;
   }
   return -1;
}//language


int mstat(int i_th, const String* name, critter& pc) {
   critter* crit_ptr;

   if (ok_to_do_action(NULL, "IP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      if (name->Strlen() == 0) {
         if (check_l_range(i_th, 0, NUMBER_OF_MOBS, pc, TRUE)) {
            crit_ptr = &(mob_list[i_th]);
         }//if
         else {
            return -1;
         }//else
      }//if
      else {
         crit_ptr = ROOM.haveCritNamed(i_th, name, pc.SEE_BIT);
      }//else
      
      if (!crit_ptr) {
         pc.show(CS_NO_CAN_SEE_PERSON);
         return -1;
      }//if
      else {
         String buf(1000);
         crit_ptr->toStringStat(&pc, buf, (ToStringTypeE)(ST_LONG | ST_NORMAL));
         pc.show(buf);
         return 0;
      }//else
   }
   return -1;
}//mstat


int ostat(int i_th, const String* name, critter& pc) {
   object* obj_ptr;
   String buf(100);

   if (ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      if (name->Strlen() == 0) {
         if (check_l_range(i_th, 0, NUMBER_OF_ITEMS, pc, TRUE)) {
            Sprintf(buf, "Here is object #%i.\n", i_th);
            pc.show(buf);
            return do_ostat(obj_list[i_th], pc);
         }//if
      }//if
      else {
         obj_ptr = have_obj_named(pc.inv, i_th, name, pc.SEE_BIT, ROOM);
         if (!obj_ptr)
            obj_ptr = ROOM.haveObjNamed(i_th, name, pc.SEE_BIT);
         if (!obj_ptr) {
            show("You neither see nor have that object.\n", pc);
            return -1;
         }//if
         else {
            return do_ostat(*obj_ptr, pc);
         }//else
      }//else
   }
   return -1;
}//ostat


int lore(int i_th, const String* name, critter& pc, int show_extra = FALSE) {
   object* obj_ptr;
   String buf(100);

   if (!pc.isPc())
     return -1;

   if (ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      obj_ptr = have_obj_named(pc.inv, i_th, name, pc.SEE_BIT, ROOM);
      if (!obj_ptr)
         obj_ptr = ROOM.haveObjNamed(i_th, name, pc.SEE_BIT);
      if (!obj_ptr) {
         pc.show(CS_NEITHER_SEE_NOR_HAVE_OBJ);
         return -1;
      }//if
      else {
         return do_lore(*obj_ptr, pc, show_extra);
      }//else
   }
   return -1;
}//lore

//TODO:  Translate
int do_lore(object& obj, critter& pc, int show_extra) {
   String buf(1000);
   ToStringTypeE st = ST_LORE;
   if (show_extra) {
      st = (ToStringTypeE)(st | ST_IDENTIFY);
   }

   obj.toStringStat(&pc, buf, st);
   pc.show(buf);
   return 0;
}

//TODO:  Translate
int do_ostat(object& obj, critter& pc) {
   String buf(1000);
   obj.toStringStat(&pc, buf, ST_ALL);
   pc.show(buf);
   return 0;
}//do_ostat


int rstat(int i_th, critter& pc) {
   String buf(500);

   if (ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      if (i_th == 1) {
         i_th = pc.getCurRoomNum();
      }//if
      if (check_l_range(i_th, 0, NUMBER_OF_ROOMS, pc, TRUE)) {
         room_list[i_th].toStringStat(&pc, buf, ST_ALL);
         return 0;
      }//if
   }
   return -1;
}//rstat


int dstat(int i_th, const String* name, critter& pc) {
   door* dr_ptr;
   String buf(100);

   if (ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      if (name->Strlen() == 0) {
         if (check_l_range(i_th, 0, NUMBER_OF_DOORS, pc, TRUE)) {
            door_list[i_th].toStringStat(&pc, buf, ST_ALL);
            pc.show(buf);
            return 0;
         }//if
      }//if
      else {
         dr_ptr = ROOM.findDoor(i_th, name, pc);
         if (!dr_ptr) {
            show("You don't see that door.\n", pc);
            return -2;
         }//if
         else {
            String buf(1000);
            dr_ptr->toStringStat(&pc, buf, ST_ALL);
            pc.show(buf);
         }
      }//else
   }
   return -1;
}//dstat


int shutdown(const String* cond, critter& pc) {
   if (ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      if (!pc.getImmLevel() > 8) {
         pc.show(CS_HAVENT_POWER);
         return -1;
      }//if

      pc.show(CS_SAVING_ALL);
      save_all();

      if (cond->Strlen() == 0) {
         do_shutdown = TRUE;
         grr_reboot = 1;
      }//if
      else if (strcasecmp(*cond, "complete") == 0) {
         do_shutdown = TRUE;
         grr_reboot = 0;
      }//else
      else if (strcasecmp(*cond, "NEW_DB") == 0) {
         CONVERT_WORLD_FROM_DEV = 1; //true
         write_setup();
         do_shutdown = TRUE;
         grr_reboot = 2;
      }//
      else {
         pc.show(CS_SHUTDOWN_HOW);
         return -2;
      }
      return 0;
   }
   return -1;
}//shutdown()


int log_level(int lvl, critter& pc) {
   if (ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      if (pc.getImmLevel() <= 8) {
         pc.show(CS_HAVENT_POWER);
         return -1;
      }//if

      String buf(100);
      if (lvl == 1) {
         pc.show(CS_LOG_FLAGS);
         Sprintf(buf, cstr(CS_LOG_LVL_IS, pc), mudlog.getLevel());
         pc.show(buf);
         if (mudlog.ofLevel(DIS)) {
            pc.show(CS_DISASTER);
         }
         if (mudlog.ofLevel(ERR)) {
            pc.show(CS_ERROR);
         }
         if (mudlog.ofLevel(WRN)) {
            pc.show(CS_WARNING);
         }
         if (mudlog.ofLevel(INF)) {
            pc.show(CS_INFO);
         }
         if (mudlog.ofLevel(TRC)) {
            pc.show(CS_FUNCT_TRACE);
         }
         if (mudlog.ofLevel(DBG)) {
            pc.show(CS_DEBUG);
         }
         if (mudlog.ofLevel(SEC)) {
            pc.show(CS_SECURITY);
         }
         if (mudlog.ofLevel(DB)) {
            pc.show(CS_DB_RW);
         }
         if (mudlog.ofLevel(XMT)) {
            pc.show(CS_XMIT);
         }
         if (mudlog.ofLevel(INP)) {
            pc.show(CS_INPUT);
         }
         if (mudlog.ofLevel(SCRIPT)) {
            pc.show(CS_SCRIPT);
         }
         if (mudlog.ofLevel(PARSE)) {
            pc.show(CS_PARSE);
         }
         if (mudlog.ofLevel(DBG2)) {
            pc.show(CS_DBG2);
         }
         return 0;
      }//if it was 1

      if (lvl == -1) {
         lvl = 1;
      }

      Sprintf(buf, cstr(CS_SETTING_LOG, pc), lvl);
      pc.show(buf);
      mudlog << "INFO:  Setting logging level to:  " << lvl << endl;

      mudlog.setLevel(lvl);
      return 0;
   }
   return -1;
}//log_level


int rezone(critter& pc) { //forces reload of zone in which pc is
   if (ok_to_do_action(NULL, "IRF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      pc.show(CS_RELOADING_ZONE);
      update_zone(ROOM.getZoneNum(), FALSE);
      return 0;
   }
   return -1;
}//rezone();


int total_rezone(critter& pc) {
   if (ok_to_do_action(NULL, "IRF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      pc.show(CS_TOTAL_RELOAD_ZONE);
      update_zone(ROOM.getZoneNum(), TRUE);
      int z = ROOM.getZoneNum();
      if (z != -1) {
         for (int i = ZoneCollection::instance().elementAt(z).getBeginRoomNum();
              i <= ZoneCollection::instance().elementAt(z).getEndRoomNum(); i++) {
            room_list[i].setTotalLoaded(TRUE); //fix empty rooms
         }//for
      }//if
      return 0;
   }
   return -1;
}//total_rezone();
