// $Id: command2.cc,v 1.26 1999/06/24 05:22:35 greear Exp $
// $Revision: 1.26 $  $Author: greear $ $Date: 1999/06/24 05:22:35 $

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


int score_long(critter& pc) {
   String buf(100);
   Cell<stat_spell_cell*> cll(pc.affected_by);
   stat_spell_cell* ss_ptr;

//   log("In score_long.\n");

   if (!pc.isPc()) {
      return -1;
   }

   if (pc.possessing) {
      Sprintf(buf, cstr(CS_YOU_POSSESSING, pc),
              pc.possessing->getName());
      show(buf, pc);
   }

   if (pc.pc && pc.pc->imm_data) {
      pc.show(CS_POOFIN);
      pc.show(pc.pc->poofin);
      pc.show(CS_POOFOUT);
      pc.show(pc.pc->poofout);
      pc.show("\n");
   }

   Sprintf(buf, cstr(CS_CLASS_RACE, pc), get_class_name(pc.CLASS),
           get_race_name(pc.RACE));
   show(buf, pc);

   if (!IsEmpty(pc.affected_by)) {
      pc.show(CS_AFFECTED_BY);
      while ((ss_ptr = cll.next())) {
         Sprintf(buf, "\t%s.\n", 
                 (const char*)(SSCollection::instance().getNameForNum(ss_ptr->stat_spell)));
         show(buf, pc);
      }//while
   }//if
   else {
      pc.show(CS_NOT_AFFECTED_SPELLS);
   }//else

   if (!IsEmpty(pc.pets)) {
      pc.show(CS_HAVE_PETS);
      out_crit(pc.pets, pc);
   }//if
   show("\n", pc);

   Sprintf(buf, cstr(CS_SLAIN_DIED, pc),
	   pc.PK_COUNT, pc.DIED_COUNT);
   show(buf, pc);
   Sprintf(buf, cstr(CS_QP, pc), pc.QUEST_POINTS);
   show(buf, pc);

   if (pc.temp_crit) {
     if (pc.GUARDED_BY) {
       Sprintf(buf, cstr(CS_GUARDED_BY, pc),
	       name_of_crit(*(pc.GUARDED_BY), pc.SEE_BIT));
       show(buf, pc);
     }//if
     if (pc.GUARDING) {
       Sprintf(buf, cstr(CS_GUARDING, pc),
	       name_of_crit(*(pc.GUARDING), pc.SEE_BIT));
       show(buf, pc);
     }//if
     if (pc.SHIELDED_BY) {
       Sprintf(buf, cstr(CS_SHIELDED_BY, pc),
	       name_of_crit(*(pc.SHIELDED_BY), pc.SEE_BIT));
       show(buf, pc);
     }//if
     if (pc.SHIELDING) {
       Sprintf(buf, cstr(CS_SHIELDING, pc),
	       name_of_crit(*(pc.SHIELDING), pc.SEE_BIT));
       show(buf, pc);
     }//if
   }//if has temp data
   return 0;
}//score_long


int score(const String* str2, critter& pc) {
   String buf2(100);

   if (!pc.isPc()) 
      return -1;

   if (str2->Strlen() != 0) { //score long perhaps
      score(&NULL_STRING, pc); //do regular score
      score_long(pc); //do extras
      return 0;
   }//if

   Sprintf(buf2, "\n%S %S\n", pc.getName(),
	   &(pc.short_desc));
   show(buf2, pc);

   if (pc.SEX == 0) 
      buf2 = cstr(CS_FEMALE, pc);
   else if (pc.SEX == 1) 
      buf2 = cstr(CS_MALE, pc);
   else if (pc.SEX == 2) 
      buf2 = cstr(CS_NEUTER, pc);

   buf2 += get_class_name(pc.CLASS);
   buf2 += cstr(CS_OF_LEVEL, pc);
   buf2.Append(pc.LEVEL);
   buf2.Append(").\n");
   show(buf2, pc);


   Sprintf(buf2, cstr(CS_SC_GOLD, pc),
           pc.GOLD, pc.EXP, pc.getAge());
   show(buf2, pc);

   Sprintf(buf2, cstr(CS_SC_HP, pc),
           pc.HP, pc.HP_MAX, pc.MANA, pc.MA_MAX, pc.MOV, pc.MV_MAX);
   show(buf2, pc);

   Sprintf(buf2, cstr(CS_SC_HPR, pc),
           pc.HP_REGEN, pc.MA_REGEN, pc.MV_REGEN);
   show(buf2, pc);

   Sprintf(buf2, cstr(CS_SC_ALIGN, pc),
           pc.ALIGN, pc.PRACS, pc.CRIT_WT_CARRIED, pc.CRIT_MAX_WT_CARRY);
   show(buf2, pc);

   Sprintf(buf2, cstr(CS_SC_RES, pc),
           pc.AC, pc.HEAT_RESIS, pc.COLD_RESIS, pc.ELEC_RESIS, pc.SPEL_RESIS);
   show(buf2, pc);

   Sprintf(buf2, cstr(CS_SC_EXP, pc), pc.getXpToNextLevel(), (pc.GOLD / 1000));
   pc.show(buf2);

   Sprintf(buf2, cstr(CS_SC_DAM, pc), pc.DAM_REC_MOD, pc.DAM_GIV_MOD);
   show(buf2, pc);

   if (pc.HUNGER == 0) 
      pc.show(CS_YOU_HUNGRY);
   if (pc.THIRST == 0)
      pc.show(CS_YOU_THIRSTY);
   if (pc.DRUGGED > 0) 
      pc.show(CS_YOU_DRUGGED);


   if (pc.POS == POS_STAND) 
      pc.show(CS_YOU_STANDING);
   else if (pc.POS == POS_SIT) 
      pc.show(CS_YOU_SITTING);
   else if (pc.POS == POS_REST) 
      pc.show(CS_YOU_RESTING);
   else if (pc.POS == POS_SLEEP) 
      pc.show(CS_YOU_SLEEPING);
   else if (pc.POS == POS_MED) 
      pc.show(CS_YOU_MED);
   else if (pc.POS == POS_STUN) 
      pc.show(CS_YOU_STUNNED);
   else if (pc.POS == POS_DEAD) 
      pc.show(CS_YOU_DEAD);
   else if (pc.POS == POS_PRONE) 
      pc.show(CS_YOU_PRONE);
   else
      pc.show(CS_POS_UNDEF);

   if (pc.FOLLOWER_OF) {
      Sprintf(buf2, cstr(CS_YOU_FOLLOWING, pc),
              name_of_crit(*(pc.FOLLOWER_OF), pc.SEE_BIT));
      show(buf2, pc);
   }//if
   else
      pc.show(CS_FOLLOWING_SELF);

   if (pc.MASTER) {
      Sprintf(buf2, cstr(CS_MASTER_IS, pc),
          name_of_crit(*(pc.MASTER), pc.SEE_BIT));
      show(buf2, pc);
   }//if
   else {
      pc.show(CS_OWN_MASTER);
   }

   if (!IsEmpty(pc.IS_FIGHTING)) {
      Sprintf(buf2, cstr(CS_YOU_FIGHTING, pc),
              name_of_crit(*(Top(pc.IS_FIGHTING)), pc.SEE_BIT));
      show(buf2, pc);
   }//if
   return 0;
}//score



int critter::doUngroup(int i_th, const String* vict) {
   Cell<critter*> cll;
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
         FOLLOWER_OF->FOLLOWERS.loseData(this);
         if (FOLLOWER_OF->GROUPEES.haveData(this)) {
            FOLLOWER_OF->GROUPEES.loseData(this);

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
   Cell<door*> cll(ROOM.DOORS);
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
      if (detect(pc.SEE_BIT, dr_ptr->dr_data->vis_bit)) {
         if (!((dr_ptr->isClosed() && dr_ptr->isSecret()) ||
               dr_ptr->isSecretWhenOpen())) {
            if (dr_ptr->destination < 0) 
               dest = (-(dr_ptr->destination));
            else
               dest = dr_ptr->destination;
            if (pc.pc->imm_data) {
               Sprintf(buf, "%P07%S[%i]:%P27", direction_of_door(*dr_ptr),
                       dr_ptr->dr_data->door_num);
            }
            else {
               Sprintf(buf, "%P07%S:%P27", direction_of_door(*dr_ptr));
            }

            buf.Cap();
            if (detect(pc.SEE_BIT, room_list[dest].getVisBit())) 
               buf += room_list[dest].short_desc;
            else
               buf += cstr(CS_CANT_DISCERN, pc);
            if (pc.isImmort()) {
               buf.Append(" [");
               buf.Append(dest);
               buf.Append("]");
            }//if
            buf += "\n";
            show(buf, pc);
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
   Cell<door*> cll(ROOM.DOORS);
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
      if (detect(pc.SEE_BIT, dr_ptr->dr_data->vis_bit)) {
         if (!((dr_ptr->isClosed() && dr_ptr->isSecret()) ||
               dr_ptr->isSecretWhenOpen())) {
            dest = abs(dr_ptr->destination);
            if (pc.isImmort()) { //if immortal, show extra info
               Sprintf(buf, "%s[%i] ", abbrev_dir_of_door(*dr_ptr),
                       dest);
            }//if
            else {
               Sprintf(buf, "%s ", abbrev_dir_of_door(*dr_ptr));
            }//else
            reg_disp.Append(buf);

            if (pc.USING_CLIENT) {
               client_disp.Append(abbrev_dir_of_door(*dr_ptr));
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
   
      dr_ptr = door::findDoor(ROOM.DOORS, i_th, name, pc.SEE_BIT, ROOM);
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
               if (have_obj_numbered(pc.inv, 1, dr_ptr->getKeyNum(),
                                     pc.SEE_BIT, ROOM)) {
                  Sprintf(buf, cstr(CS_YOU_LOCK, pc), name_of_door(*(dr_ptr),
                                                                   pc.SEE_BIT));
                  show(buf, pc);
                  dr_ptr->lock();

                  //TODO:  Translation problem
                  Sprintf(buf, "locks the %S.\n", name_of_door(*dr_ptr, ~0));
                  emote(buf, pc, ROOM, TRUE);
                  
                  String cmd = "lock";
                  ROOM.checkForProc(cmd, NULL_STRING, pc,
                                    dr_ptr->dr_data->door_num);
                  
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
         ob_ptr = have_obj_named(pc.inv, i_th, name,
                                 pc.SEE_BIT, ROOM);
         if (ob_ptr) {
            if (ob_ptr->bag && !ob_ptr->IN_LIST) {
               ob_ptr = obj_to_sobj(*ob_ptr, &(pc.inv), TRUE,
                                    i_th, name, pc.SEE_BIT, ROOM);
            }
         }
         else {
            ob_ptr = ROOM.haveObjNamed(i_th, name, pc.SEE_BIT);
            
            if (ob_ptr && ob_ptr->bag && !ob_ptr->IN_LIST) {
               ob_ptr = obj_to_sobj(*ob_ptr, ROOM.getInv(), TRUE,
                                    i_th, name, pc.SEE_BIT, ROOM);
            }
         }//else
         
         if (ob_ptr) {
            if (!ob_ptr->bag) {
               pc.show(CS_NOT_CONTAINER);
            }//if
            else if (ob_ptr->bag->key_num == 0) {
               pc.show(CS_CANT_BE_LOCKED_CONT);
            }//if
            else if (ob_ptr->isClosed()) { //is closed
               if (ob_ptr->isLocked()) {
                  pc.show(CS_ALREADY_LOCKED);
               }//if
               else {
                  object* key = NULL;
                  int posn = -1;
                  key = have_obj_numbered(pc.inv, 1, ob_ptr->getKeyNum(),
                                          pc.SEE_BIT, ROOM);
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
                     Sprintf(buf, "locks the %S.\n", name_of_obj(*ob_ptr, ~0));
                     emote(buf, pc, ROOM, TRUE);
                     
                     // send message to other side of the door...
                     Sprintf(buf, "You hear a sharp click from %S.\n",
                             name_of_door(*dr_ptr, ~0));
                     show_all(buf, *(dr_ptr->getDestRoom()));
                     
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
   
      if ((dr_ptr = door::findDoor(ROOM.DOORS, i_th, name, pc.SEE_BIT, ROOM))) {
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
               
               key = have_obj_numbered(pc.inv, 1, dr_ptr->getKeyNum(),
                                       pc.SEE_BIT, ROOM);
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
                     if (key->IN_LIST) {
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
                  Sprintf(buf, "unlocks the %S.\n", name_of_door(*dr_ptr, ~0));
                  emote(buf, pc, ROOM, TRUE);
       
                  // TODO:  Translation problem.
                  // send message to other side of the door...
                  Sprintf(buf, "You hear a faint click from %S.\n",
                          name_of_door(*dr_ptr, 0));
                  show_all(buf, *(dr_ptr->getDestRoom()));
                  
                  String cmd = "unlock";
                  ROOM.checkForProc(cmd, NULL_STRING, pc,
                                    dr_ptr->dr_data->door_num);
                  
                  
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
         ob_ptr = have_obj_named(pc.inv, i_th, name,
                                 pc.SEE_BIT, ROOM);
         if (ob_ptr) {
            if (ob_ptr->bag && !ob_ptr->IN_LIST) {
               ob_ptr = obj_to_sobj(*ob_ptr, &(pc.inv), TRUE,
                                    i_th, name, pc.SEE_BIT, ROOM);
            }
         }
         else {
            ob_ptr = ROOM.haveObjNamed(i_th, name, pc.SEE_BIT);
            
            if (ob_ptr && ob_ptr->bag && !ob_ptr->IN_LIST) {
               ob_ptr = obj_to_sobj(*ob_ptr, ROOM.getInv(), TRUE,
                                    i_th, name, pc.SEE_BIT, ROOM);
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
                                          pc.SEE_BIT, ROOM);
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
                                ob_ptr->getLongName(pc.SEE_BIT),
                                key->getLongName());
                        if (posn > 0) {
                           pc.EQ[posn] = NULL;
                           remove_eq_effects(*key, pc, FALSE, FALSE, posn);
                        }
                        else {
                           pc.inv.loseData(key);
                           drop_eq_effects(*key, pc, FALSE);
                        }
                        if (key->IN_LIST) {
                           recursive_init_unload(*key, 0);
                           delete key;
                        }
                        key = NULL;
                     }//if
                     else {
                        Sprintf(buf, cstr(CS_YOU_UNLOCK, pc),
                                ob_ptr->getLongName(pc.SEE_BIT));
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
   
      if ((dr_ptr = door::findDoor(ROOM.DOORS, i_th, name, pc.SEE_BIT, ROOM))) {
         if (dr_ptr->isSecret()) {
            if (!name_is_secret(name, *dr_ptr)) {
               pc.show(CS_NO_SEE_EXIT);
               return -1;
            }//if
         }//if	    
         if (dr_ptr->dr_data->isClosed()) {
            if (!dr_ptr->canOpen()) {
               Sprintf(buf, cstr(CS_OPEN_AUTOMAGICALLY, pc),
                       name_of_door(*(dr_ptr), pc.SEE_BIT));
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
               Sprintf(buf, "opens the %S.\n", name_of_door(*dr_ptr, 0));
               emote(buf, pc, ROOM, TRUE);
               
               // send message to other side of the door...
               Sprintf(buf, "%S opens quietly.\n", name_of_door(*dr_ptr, 0));
               show_all(buf, *(dr_ptr->getDestRoom()));
               
               String cmd = "open";
               ROOM.checkForProc(cmd, NULL_STRING, pc,
                                 dr_ptr->dr_data->door_num);
               
               return 0;
            }//else
         }//if closed
         else {
            pc.show(CS_ALREADY_OPEN);
         }//else
      }//
      else {   //check for items to be opened
         ob_ptr = have_obj_named(pc.inv, i_th, name,
                                 pc.SEE_BIT, ROOM);
         if (ob_ptr) {
            if (ob_ptr->bag && !ob_ptr->IN_LIST) {
               ob_ptr = obj_to_sobj(*ob_ptr, &(pc.inv), TRUE,
                                    i_th, name, pc.SEE_BIT, ROOM);
            }
         }
         else {
            ob_ptr = ROOM.haveObjNamed(i_th, name, pc.SEE_BIT);
            
            if (ob_ptr && ob_ptr->bag && !ob_ptr->IN_LIST) {
               ob_ptr = obj_to_sobj(*ob_ptr, ROOM.getInv(), TRUE,
                                    i_th, name, pc.SEE_BIT, ROOM);
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
                  Sprintf(buf, cstr(CS_IS_LOCKED, pc), name_of_obj(*(ob_ptr),
                                                                   pc.SEE_BIT));
                  show(buf, pc);
               }//if
               else { 
                  ob_ptr->open();
                  Sprintf(buf, cstr(CS_YOU_OPEN, pc), name_of_obj(*(ob_ptr),
                                                                  pc.SEE_BIT));
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
      if ((dr_ptr = door::findDoor(ROOM.DOORS, i_th, name, pc.SEE_BIT, ROOM))) {
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
               Sprintf(buf, cstr(CS_YOU_CLOSE, pc), name_of_door(*(dr_ptr),
                                                                 pc.SEE_BIT));
               show(buf, pc);
               
               //TODO:  Translation problem
               // send message to other side of the door...
               Sprintf(buf, "%S closes quietly.\n", name_of_door(*dr_ptr, 0));
               show_all(buf, *(dr_ptr->getDestRoom()));
               
               String cmd = "close";
               ROOM.checkForProc(cmd, NULL_STRING, pc, 
                                 dr_ptr->dr_data->door_num);
               
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
         ob_ptr = have_obj_named(pc.inv, i_th, name,
                                 pc.SEE_BIT, ROOM);
         if (ob_ptr) {
            if (ob_ptr->bag && !ob_ptr->IN_LIST) {
               ob_ptr = obj_to_sobj(*ob_ptr, &(pc.inv), TRUE,
                                    i_th, name, pc.SEE_BIT, ROOM);
            }
         }
         else {
            ob_ptr = ROOM.haveObjNamed(i_th, name, pc.SEE_BIT);
            
            if (ob_ptr && ob_ptr->bag && !ob_ptr->IN_LIST) {
               ob_ptr = obj_to_sobj(*ob_ptr, ROOM.getInv(), TRUE,
                                    i_th, name, pc.SEE_BIT, ROOM);
            }
         }//else
         if (ob_ptr) {
            if (!ob_ptr->bag) {
               pc.show(CS_NOT_CONTAINER);
            }//if
            else if (!ob_ptr->BAG_FLAGS.get(2)) { //is open
               if (ob_ptr->BAG_FLAGS.get(9)) { //if non-closeable
                  Sprintf(buf, cstr(CS_CANNOT_CLOSE, pc), name_of_obj(*(ob_ptr),
                                                                      pc.SEE_BIT));
                  show(buf, pc);
               }//if
               else { 
                  ob_ptr->BAG_FLAGS.turn_on(2);
                  Sprintf(buf, cstr(CS_YOU_CLOSE, pc), name_of_obj(*(ob_ptr),
                                                                   pc.SEE_BIT));
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
         if (obj_ptr->IN_LIST)
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

      obj_ptr = have_obj_named(pc.inv, i_th, name, pc.SEE_BIT, ROOM);
      
      if (!obj_ptr) {
         obj_ptr = ROOM.haveObjNamed(i_th, name, pc.SEE_BIT);
         in_inv = FALSE;
      }//if
      if (!obj_ptr) {
         Sprintf(buf, cstr(CS_AINT_GOT, pc), name);
         show(buf, pc);
      }//if
      else {  //ok, got some kind of object...lets test it
         if (IsEmpty(obj_ptr->inv)) {
            if (mudlog.ofLevel(INF)) {
               mudlog << "INFO:  container has no inventory, obj# "
                      << obj_ptr->getIdNum() << endl;
            }
         }
         
         if (!(obj_ptr->OBJ_FLAGS.get(59))) { //canteen bit
            Sprintf(buf, cstr(CS_NO_LIQ_CONT, pc), name);
            show(buf, pc);
         }//if
         else if (pc.pc && pc.THIRST > THIRST_MAX) {
            pc.show(CS_TOO_FULL_TO_DRINK);
         }//if
         else if (obj_ptr->extras[0] == 0) { 
            Sprintf(buf, cstr(CS_CONT_EMPTY_SPRINTF, pc),
                    obj_ptr->getShortName());
            show(buf, pc);
         }//if 
         else if (IsEmpty(obj_ptr->inv)) {
            Sprintf(buf, cstr(CS_NO_LIQ_IN_CONT, pc),
                    &(obj_ptr->short_desc));
            buf.Cap();
            show(buf, pc);
         }//if 
         else if (!obj_ptr->IN_LIST) {
            if (in_inv) {
               obj2 = obj_to_sobj(*obj_ptr, &(pc.inv), TRUE, i_th,
                                  name, pc.SEE_BIT, ROOM);
            }//if
            else {
               obj2 = obj_to_sobj(*obj_ptr, ROOM.getInv(), TRUE, i_th,
                                  name, pc.SEE_BIT, ROOM);
            }//else
            object* obj3 = Top(obj2->inv);
            obj2->CHARGES--;
            consume_eq_effects(*obj3, pc, TRUE);
            return 0;
         }//if
         else {
            obj2 = Top(obj_ptr->inv);
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

      targ_obj = have_obj_named(pc.inv, i_th, targ, pc.SEE_BIT, ROOM);
   
      source_obj = ROOM.haveObjNamed(j_th, source, pc.SEE_BIT);
      if (!source_obj) {
         source_obj = 
            have_obj_named(pc.inv, i_th, source, pc.SEE_BIT, ROOM);
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

      else if ((Top(targ_obj->inv) != Top(source_obj->inv)) && 
               (targ_obj->extras[0] != 0)) {
         Sprintf(buf, cstr(CS_MUST_EMPTY, pc),
                 name_of_obj(*targ_obj, pc.SEE_BIT));
         show(buf, pc);
      }//if
                  /* ok, liquids are the same */

      else if (source_obj->extras[0] == 0) {
         Sprintf(buf, cstr(CS_EMPTY_ALREADY, pc), name_of_obj(*source_obj,
                                                              pc.SEE_BIT));
         show(buf, pc);
      }//if
      else if (targ_obj->extras[0] >= targ_obj->bag->max_weight) {
         Sprintf(buf, cstr(CS_FULL_ALREADY, pc), name_of_obj(*targ_obj,
                                                             pc.SEE_BIT));
         show(buf, pc);
      }//if
      else if (targ_obj->extras[0] == -1) {
         pc.show(CS_NEVER_NEED_FILLING);
      }
                   /* All clear to exchange liquids ;) */

      else {		/* take care of Obj to Sobj if needed. */
         mudlog.log(TRC, "About to take care of obj to sobj.\n");
         if (!targ_obj->in_list) {
            targ_obj = obj_to_sobj(*targ_obj, &(pc.inv), TRUE, i_th,
                                   targ, pc.SEE_BIT, ROOM);
         }//if
         
         if (mudlog.ofLevel(DBG)) {
            mudlog << "in fill, targ: " << targ_obj->getName() << " num: " 
                   << targ_obj->getIdNum() << "  source: "
                   << source_obj->getName() << endl;
         }
         
         Cell<object*> cll(targ_obj->inv);
         object* ptr;
         while ((ptr = cll.next())) {
            recursive_init_unload(*ptr, 0);
         }//while
         clear_obj_list(targ_obj->inv);
         
         //TODO, check for SOBJ:  Answer, allways use OBJ, not SOBJ
         targ_obj->gainInv(&(obj_list[Top(source_obj->inv)->getIdNum()]));
         
         /* test for infinite source */
         if (source_obj->extras[0] <= -1) {
            if (source_obj->extras[0] < -1) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "WARNING: Was an infinite source, < -1: "
                         << source_obj->extras[0] << endl;
               }
            }
            else {
               mudlog.log(TRC, "Was an infinite source.\n");
            }
            targ_obj->extras[0] = targ_obj->bag->max_weight;
         }//if
                     /* not an infinite source */
         else {
            mudlog.log(TRC, "Not an infinite source.\n");
            if (!source_obj->in_list) {
               if (in_inv_source) {
                  source_obj = obj_to_sobj(*source_obj, &(pc.inv), TRUE, i_th,
                                           source, pc.SEE_BIT, ROOM);
               }//if in inv
               else {
                  source_obj = obj_to_sobj(*source_obj, ROOM.getInv(), TRUE, i_th,
                                           source, pc.SEE_BIT, ROOM);
               }//else
            }//if
            /* this next bit is badly innefficient. */
            while ((targ_obj->extras[0] != 
                    targ_obj->bag->max_weight) &&
                   (source_obj->extras[0] > 0)) {
               targ_obj->extras[0]++;
               source_obj->extras[0]--;
            }//while
         }//else
         Sprintf(buf, cstr(CS_YOU_FILL, pc), targ_obj->getShortName(),
                 source_obj->getLongName());
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

      object* obj_ptr = have_obj_named(pc.inv, i_th, canteen, pc.SEE_BIT, 
                                       ROOM);

      if (!obj_ptr) {
         Sprintf(buf, cstr(CS_DONT_SEEM_TO_HAVE, pc), canteen);
         show(buf, pc);
      }//if
      else if (!(obj_ptr->OBJ_FLAGS.get(59))) {
         Sprintf(buf, cstr(CS_NOT_LIQ_CONT_EMPTY, pc),
                 Top(obj_ptr->names));
         show(buf, pc);
      }//if
                  /* got a valid canteen */
   
      else if (obj_ptr->extras[0] == 0) {
         Sprintf(buf, cstr(CS_CONT_EMPTY_SPRINTF, pc),
                 Top(obj_ptr->names));
         show(buf, pc);
      }//if
      else if (obj_ptr->extras[0] == -1) {
         Sprintf(buf, cstr(CS_NEVER_EMPTY, pc),
                 Top(obj_ptr->names));
         show(buf, pc);
      }//if
                   /* ok, procede w/emptying */

      else {
         if (!obj_ptr->IN_LIST) {
            obj2 = obj_to_sobj(*obj_ptr, &(pc.inv), TRUE, i_th,
                               canteen, pc.SEE_BIT, ROOM);
            obj_ptr = obj2;
         }//if
         
         Cell<object*> cll(obj_ptr->inv);
         object* ptr;      
         while ((ptr = obj_ptr->inv.lose(cll))) {
            recursive_init_unload(*ptr, 0);
            if (ptr->IN_LIST) {
               delete ptr;
            }
         }//while

         obj_ptr->extras[0] = 0; //no charges
         Sprintf(buf, cstr(CS_YOU_EMPTY, pc), obj_ptr->getShortName());
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
      if (crit_ptr) {
         if (crit_ptr->isMob()) {
            crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum());
         }//if
      }//if
   }//if
   else {
      crit_ptr = ROOM.haveCritNamed(j_th, keeper, pc.SEE_BIT);
      if (crit_ptr) {
         if (crit_ptr->isMob()) {
            crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum(), TRUE, j_th, 
                   keeper, pc.SEE_BIT);
         }//if
      }//if
      else {  //check for vending machine of some type
         Cell<object*> cll(*(ROOM.getInv()));
         object* ptr;
         while ((ptr = cll.prev())) {
            if (ptr->OBJ_FLAGS.get(73)) { //if its a vend machine
               do_vend_buy(*ptr, i_th, item, pc);
               return 0;
            }//if
         }//while
      }//else
   }//else

   if (!crit_ptr) {
      pc.show(CS_NO_CAN_SEE_PERSON);
   }//if
   else if (!crit_ptr->isNpc()) { //dealing w/a pc...
      Sprintf(buf, cstr(CS_TRADE_PC, pc),
              name_of_crit(*crit_ptr, pc.SEE_BIT));
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
      if (crit_ptr) {
         if (crit_ptr->isMob()) {
            crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum());
         }//if
      }//if
   }//if
   else {
      mudlog.log(TRC, "Searching room for Shopkeeper, in SELL.\n");
      crit_ptr = ROOM.haveCritNamed(j_th, keeper, pc.SEE_BIT);
      if (crit_ptr) {
         if (crit_ptr->isMob()) {
            crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum(),
                                   TRUE, j_th, keeper, pc.SEE_BIT);
         }//if
      }//if
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
   if (ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      if (master->Strlen() == 0) {
         crit_ptr = ROOM.findFirstTeacher();
      }//if
      else {
         crit_ptr = ROOM.haveCritNamed(j_th, master, pc.SEE_BIT);
      }//else
      
      if (spell->Strlen() == 0) {
         return abilities(pc);
      }//if
      else if (pc.EQ[9] || pc.EQ[10]) {
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

      Sprintf(buf, cstr(CS_TOG4_1, pc), (int)(pc.PC_FLAGS.get(28)));
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
   object* obj_ptr;
   int price;
   critter* crit_ptr;

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR: MOB trying to list_merchandise.\n");
      return -1;
   }//if

   if (ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {

      if (keeper->Strlen() == 0) {
         crit_ptr = ROOM.findFirstShopKeeper();
      }//if
      else {
         crit_ptr = ROOM.haveCritNamed(i_th, keeper, pc.SEE_BIT);
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
         int cur_time = get_game_time();
         if (crit_ptr->OPEN_TIME < crit_ptr->CLOSE_TIME) {
            if ((cur_time < crit_ptr->OPEN_TIME) || 
                (cur_time > crit_ptr->CLOSE_TIME)) {
               Sprintf(buf, cstr(CS_HOURS_FROM, pc),
                       military_to_am(crit_ptr->OPEN_TIME),
                       military_to_am(crit_ptr->CLOSE_TIME));
               show(buf, pc);
               return -1;
            }//if
         }//if
         else {
            if ((cur_time > crit_ptr->CLOSE_TIME) &&
                (cur_time < crit_ptr->OPEN_TIME)) {
               Sprintf(buf, cstr(CS_HOURS_FROM, pc),
                       military_to_am(crit_ptr->OPEN_TIME),
                       military_to_am(crit_ptr->CLOSE_TIME));
               show(buf, pc);
               return -1;
            }//if
         }//else

         pc.show(CS_ITEMS_FOR_SALE);
         
         Cell<object*> cell(crit_ptr->inv);
         static int item_counts[NUMBER_OF_ITEMS + 1];
         
         memset(item_counts, 0, sizeof(int) * (NUMBER_OF_ITEMS + 1));
         
         // Now, find the instance count.
         while ((obj_ptr = cell.next())) {
            item_counts[obj_ptr->getIdNum()]++;
         }//while
         
         crit_ptr->PERM_INV.head(cell);
         while ((obj_ptr = cell.next())) {
            item_counts[obj_ptr->getIdNum()]++;
         }//while
         
         crit_ptr->inv.head(cell);
         int id_num;
         int cnt = 0;
         while ((obj_ptr = cell.next())) {
            
            id_num = obj_ptr->getIdNum();
            
            if (!obj_ptr->in_list &&
                (item_counts[id_num] == -1)) { //already done it
               continue;
            }
            
            if (detect(pc.SEE_BIT, (obj_ptr->OBJ_VIS_BIT | ROOM.getVisBit()))) {
               price = crit_ptr->findItemSalePrice(*obj_ptr, pc);
               
               if (price < 0) {
                  continue; //buf = "  NOT FOR SALE NOW.";
               }//if
               
               if (pc.shouldShowVnums()) {
                  if (obj_ptr->in_list || (item_counts[id_num] == 1)) {
                     Sprintf(buf, " [%i]%P06 %S%P50%i", id_num,
                             &(obj_ptr->short_desc), price);
                  }
                  else {
                     Sprintf(buf, " [%i]%P06 [*%i]%P12 %S%P50%i", id_num,
                             item_counts[id_num], &(obj_ptr->short_desc), price);
                  }
               }
               else {
                  cnt++;
                  if (obj_ptr->in_list || (item_counts[id_num] == 1)) {
                     Sprintf(buf, "  [%i]%P12 %S%P50%i", cnt, &(obj_ptr->short_desc),
                             price);
                  }
                  else {
                     Sprintf(buf, "  [%i] [*%i]%P12 %S%P50%i", cnt, item_counts[id_num],
                             &(obj_ptr->short_desc), price);
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

         crit_ptr->PERM_INV.head(cell);
         while ((obj_ptr = cell.next())) {
            
            id_num = obj_ptr->getIdNum();
            
            if (!obj_ptr->in_list &&
                (item_counts[id_num] == -1)) { //already done it
               continue;
            }
            
            if (detect(pc.SEE_BIT, (obj_ptr->OBJ_VIS_BIT | ROOM.getVisBit()))) {
               price = crit_ptr->findItemSalePrice(*obj_ptr, pc);
               
               if (price < 0) {
                  continue; //buf = "  NOT FOR SALE NOW.";
               }//if
               
               if (pc.shouldShowVnums()) {
                  if (obj_ptr->in_list || (item_counts[id_num] == 1)) {
                     Sprintf(buf, " [%i]%P06 %S%P50%i", id_num,
                             &(obj_ptr->short_desc), price);
                  }
                  else {
                     Sprintf(buf, " [%i]%P06 [*%i]%P12 %S%P50%i", id_num,
                             item_counts[id_num], &(obj_ptr->short_desc), price);
                  }
               }
               else {
                  cnt++;
                  if (obj_ptr->in_list || (item_counts[id_num] == 1)) {
                     Sprintf(buf, "  [%i]%P12 %S%P50%i", cnt, &(obj_ptr->short_desc),
                             price);
                  }
                  else {
                     Sprintf(buf, "  [%i] [*%i]%P12 %S%P50%i", cnt, item_counts[id_num],
                             &(obj_ptr->short_desc), price);
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

         String cmd = "list";
         ROOM.checkForProc(cmd, NULL_STRING, pc, crit_ptr->MOB_NUM);
         return 0;
      }//else, is shopkeeper
   }
   return -1;
}//list()



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
      else if (i > pc.HP_MAX / 2) {
         pc.WIMPY = pc.HP_MAX / 2;
      }//if
      else {
         pc.WIMPY = i;
      }//else
      pc.show(CS_OK);
      return 0;
   }
   return -1;
}//wimpy


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
         return do_mstat(*crit_ptr, pc);
      }//else
   }
   return -1;
}//mstat



//TODO:  Translate all these little strings!!
int do_mstat(critter& targ, critter& pc) {
   String buf2(200);
   String buf(100);
   critter* crit_ptr = &targ; //to reduce my typing :P
   mudlog.log(TRC, "In do_mstat.\n");

   if (!crit_ptr->isInUse()) {
      show("This critter UNDEFINED.\n", pc);
      return -1;
   }//if
   else {

      if (!pc.isUsingClient() || crit_ptr->pc) {
         show("\n", pc);
         show((crit_ptr->short_desc), pc);
         show("\n", pc);
         show((crit_ptr->in_room_desc), pc);
         show("\n", pc);
         show((crit_ptr->long_desc), pc);
         show("\n", pc);
      }
      else {
         Sprintf(buf2, "<MSTAT %i>", crit_ptr->MOB_NUM);
         show(buf2, pc);
         show("\n<STAT_SD>", pc);
         show((crit_ptr->short_desc), pc);
         show("</STAT>\n<STAT_ND>", pc);
         show((crit_ptr->in_room_desc), pc);
         show("</STAT>\n<STAT_LD>", pc);
         show((crit_ptr->long_desc), pc);
         show("</STAT>\n", pc);
      }//else

      Cell<String*> cll(crit_ptr->names);
      String* ptr;
      buf2 = "";
      while ((ptr = cll.next())) {
         buf2 += *ptr;
         buf2 += " ";
      }//while

      if (!pc.isUsingClient() || crit_ptr->pc) {
         Sprintf(buf, "\nNames (Keywords): %S\n\n", &buf2);
         pc.show(buf);
      }//if
      else { //then show tags...
         Sprintf(buf, "<NAMES %S>\n", &buf2);
         pc.show(buf); //output the client deals with
         pc.show(buf2); //output that they can read
         pc.show("\n\n");
      }//else

      out_field(crit_ptr->CRIT_FLAGS, pc, CRIT_FLAGS_NAMES);

      Sprintf(buf2, "\nCLASS:  %s  race:  %s    Belongs to ZONE:  %i\n", 
	      get_class_name(crit_ptr->CLASS), get_race_name(crit_ptr->RACE),
              crit_ptr->getNativeZoneNum());
      show(buf2, pc);

      Sprintf(buf2, "VIS_BIT:  %i, see_bit:  %i, GOLD:  %i, exp: %i XP_WORTH: %i\n",
	      crit_ptr->VIS_BIT, crit_ptr->SEE_BIT, crit_ptr->GOLD,
	      crit_ptr->EXP, crit_ptr->EXP / EXP_DIVISOR);
      show(buf2, pc);

      Sprintf(buf2, "POS: %i str: %i  int: %i  con: %i  cha: %i wis: %i  dex: %i\n",
              crit_ptr->POS, crit_ptr->STR, crit_ptr->INT, crit_ptr->CON,
              crit_ptr->CHA, crit_ptr->WIS, crit_ptr->DEX);
      show(buf2, pc);
  
      Sprintf(buf2, "hit: %i  dam: %i  ac: %i  atks: %i pauz: %i  sex: %i\n",
              crit_ptr->HIT, crit_ptr->DAM, crit_ptr->AC, crit_ptr->ATTACKS, 
              crit_ptr->PAUSE, crit_ptr->SEX);
      show(buf2, pc);

      Sprintf(buf2, "class: %i  race: %i  H: %i  M: %i V: %i  align: %i\n",
              crit_ptr->CLASS, crit_ptr->RACE, crit_ptr->HP, crit_ptr->MANA, 
              crit_ptr->MOV, crit_ptr->ALIGN);
      show(buf2, pc);
                     /* done through 18 */
      Sprintf(buf2,
              "lvl: %i  Home_Town: %i  wimpy: %i  Prac: %i Hmx: %i  Mmx: %i\n",
              crit_ptr->LEVEL, crit_ptr->getHomeTown(), crit_ptr->WIMPY,
              crit_ptr->PRACS, crit_ptr->HP_MAX, crit_ptr->MA_MAX);
      show(buf2, pc);

      Sprintf(buf2, "Vmx: %i  CRITTER_TYPE: %i  dam_rec_mod: %i  DAM_GIV_MOD: %i\n",
              crit_ptr->MV_MAX, crit_ptr->CRITTER_TYPE,
              crit_ptr->DAM_GIV_MOD, crit_ptr->DAM_REC_MOD);
      show(buf2, pc);

      Sprintf(buf2,
              "heat_resis: %i  COLD_RESIS: %i  elect_resis: %i  SPELL_RESIS: %i\n",
              crit_ptr->HEAT_RESIS, crit_ptr->COLD_RESIS,
              crit_ptr->ELEC_RESIS, crit_ptr->SPEL_RESIS);
      show(buf2, pc);

      Sprintf(buf2,
              "RELIGION: %i  BH_dice_count: %i  BH_DICE_SIDES: %i  HP_regen: %i\n",
              crit_ptr->RELIGION, crit_ptr->BH_DICE_COUNT,
              crit_ptr->BH_DICE_SIDES, crit_ptr->HP_REGEN);
      show(buf2, pc);

      Sprintf(buf2,
              "CUR_WEIGHT: %i  max_weight: %i PAUSE: %i H_rgn: %i  M_rgn: %i\n",
              crit_ptr->CRIT_WT_CARRIED, crit_ptr->CRIT_MAX_WT_CARRY, 
              crit_ptr->PAUSE, crit_ptr->HP_REGEN,   crit_ptr->MA_REGEN);
      show(buf2, pc);

      Sprintf(buf2, "V_rgn: %i  naked_weight: %i\n\n", crit_ptr->MV_REGEN,
              crit_ptr->getNakedWeight());
      show(buf2, pc);


      if (crit_ptr->pc) {
         out_field(crit_ptr->pc->pc_data_flags, pc, PC_DATA_FLAGS_NAMES);

         Sprintf(buf2, "\n\tDescriptor:  %i,  Host:  %S.\n", 
             crit_ptr->pc->descriptor, &crit_ptr->pc->host);
         show(buf2, pc);
         Sprintf(buf2, "\tLink cond:  %i,  Mode:  %i,  Index:  %i.\n",
             crit_ptr->pc->link_condition, crit_ptr->pc->mode,
             crit_ptr->pc->index);
         show(buf2, pc);
         Sprintf(buf2, "\tHunger:  %i,  Thirst:  %i,  Drugged:  %i.\n",
             crit_ptr->pc->hunger, crit_ptr->pc->thirst,
             crit_ptr->pc->drugged);
         show(buf2, pc);
	 Sprintf(buf2, "pk_count:  %i, died %i times, quest_points %i\n",
	     crit_ptr->PK_COUNT, crit_ptr->DIED_COUNT, 
	     crit_ptr->QUEST_POINTS);
	 show(buf2, pc);
	 if (crit_ptr->pc->imm_data) {
	   Sprintf(buf2, "IMM_LEVEL:  %i\n", crit_ptr->IMM_LEVEL);
	   show(buf2, pc);
	 }//if
      }//if

      if (crit_ptr->mob) {
         Sprintf(buf2, "\tThis is MOB#: %i.\n", crit_ptr->MOB_NUM);
         show(buf2, pc);
	 if (crit_ptr->isSmob()) {
	    show("It is a SMOB, not a MOB.\n", pc);
	 }
	 else {
	    show("It is a MOB, not a SMOB.\n", pc);
	 }//else

         Sprintf(buf2,
                 "\tCur in game:  %i,  Max in game:  %i,  Home Room:  %i,  Is Sessile: %i.\n",
                 crit_ptr->getCurInGame(), crit_ptr->getMaxInGame(),
                 crit_ptr->mob->home_room, crit_ptr->isSessile());
	 show(buf2, pc);

         if (crit_ptr->isTracking()) {
            Sprintf(buf2, "\tTracking target:  %S",
                    crit_ptr->getTrackingTarget());
            show(buf2, pc);
         }
         else {
            pc.show("\tNot Tracking");
         }

         if (crit_ptr->shouldBeHoming()) {
            Sprintf(buf2, "\t\tHoming to room:  %i\n",
                    crit_ptr->getHomeRoom());
            show(buf2, pc);
         }
         else {
            pc.show("\t\tNot Homing\n");
         }

         out_field(crit_ptr->MOB_FLAGS, pc, MOB_DATA_FLAGS_NAMES);

         if (crit_ptr->mob->proc_data) {
            //show("\tIt has PROC_DATA.\n", pc);
            show("\tProc Flags (flag1):  ", pc);
            out_field(crit_ptr->FLAG1, pc, MOB_PROC_DATA_FLAGS_NAMES);
            Sprintf(buf2, "\tDirection Guarding:  %i\n", crit_ptr->INT1);
            show(buf2, pc);
	    Sprintf(buf2, 
		    "skill_violnc:  %i, benev:  %i, def:  %i, bad_ass %i, Soc_Aware: %i\n",
		    crit_ptr->mob->getSkillViolence(),
                    crit_ptr->mob->getBenevolence(),
		    crit_ptr->mob->getDefensiveness(),
                    crit_ptr->mob->getBadAssedness(),
                    crit_ptr->mob->getSocialAwareness());
	    show(buf2, pc);
            if (crit_ptr->mob->proc_data->sh_data) {
               show("Its a shopkeeper.\n", pc);
               Sprintf(buf2, "markup  %i, buy_percentage  %i, open_time %i",
                       crit_ptr->MARKUP, crit_ptr->BUY_PERCENTAGE, 
                       crit_ptr->OPEN_TIME);
               show(buf2, pc);
               Sprintf(buf2, " close_time  %i\n", crit_ptr->CLOSE_TIME);
               show(buf2, pc);
               out_field(crit_ptr->SHOP_DATA_FLAGS, pc, SHOP_DATA_FLAGS_NAMES);
               show("Permanent Inventory:\n", pc);
               out_inv(crit_ptr->PERM_INV, pc, CRIT_INV);
            }//if shopkeeper

            if (crit_ptr->mob->proc_data->teach_data) {
               show("Its a TEACHER.\n", pc);
               out_field(crit_ptr->TEACH_DATA_FLAGS, pc, TEACH_DATA_FLAGS_NAMES);
            }//if
	    if (crit_ptr->mob->proc_data->give_proc) {
	      show("It HAS a give proc.\n", pc);
	    }//if
	    if (crit_ptr->mob->proc_data->bow_proc) {
	      show("It HAS a bow proc.\n", pc);
	    }//if
	    if (crit_ptr->mob->proc_data->curse_proc) {
	      show("It HAS a curse proc.\n", pc);
	    }//if
	    Cell<say_proc_cell*> sp_cll(crit_ptr->mob->proc_data->topics);
	    say_proc_cell* sp_ptr;
	    while ((sp_ptr = sp_cll.next())) {
	      Sprintf(buf2, "It has this discuss proc:  %S.\n",
		      &(sp_ptr->topic));
	      show(buf2, pc);
	    }//while
         }//if proc data
	 Sprintf(buf2, "Skin num: %i\n\n", crit_ptr->mob->skin_num);
	 show(buf2, pc);

         if (crit_ptr->mob->hasMobScript()) {
            pc.show("Mob scripts:\n");
            do_list_scripts(*crit_ptr, pc);
         }

         if (crit_ptr->mob->isRunningScript()) {
            pc.show("Running Scripts:\n");
            Cell<MobScript*> ms_cll(crit_ptr->mob->pending_scripts);
            MobScript* ms_ptr;
            while ((ms_ptr = ms_cll.next())) {
               pc.show(ms_ptr->getRunningScript());
               pc.show("\n\n");
            }
         }

      }//if a mob

                 /* pc data */
      if (!IsEmpty(crit_ptr->IS_FIGHTING)) {
         Sprintf(buf2, "%S is fighting:\n", name_of_crit(*crit_ptr, ~0));
         show(buf2, pc);
         out_crit(crit_ptr->IS_FIGHTING, pc);
         show("\n\n", pc);
      }//if

      if (crit_ptr->master) {
         Sprintf(buf2, "Has a MASTER:  %S.\n", 
            name_of_crit(*(crit_ptr->master), ~0));
         show(buf2, pc);
      }//if
   }//else
   return 0;
}//do_mstat


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
   String buf(100);

   buf = obj.short_desc;
   buf.Cap();
   pc.show(buf);
   pc.show("\n");
   pc.show(obj.long_desc);
   pc.show("\n");
  
   buf = NULL_STRING;

   if (obj.OBJ_FLAGS.get(1))
      buf += "Anti-Evil ";
   if (obj.OBJ_FLAGS.get(2))
      buf += "Anti-Neutral ";
   if (obj.OBJ_FLAGS.get(3))
      buf += "Anti-Good ";
   if (obj.OBJ_FLAGS.get(4))
      buf += "Anti-Donate ";
   if (obj.OBJ_FLAGS.get(5))
      buf += "Anti-Drop ";
   if (obj.OBJ_FLAGS.get(6))
      buf += "Anti-Remove ";
   if (obj.OBJ_FLAGS.get(7))
      buf += "Anti-Mortal ";
   if (obj.OBJ_FLAGS.get(11))
      buf += "Anti-Warrior ";
   if (obj.OBJ_FLAGS.get(12))
      buf += "Anti-Sage ";
   if (obj.OBJ_FLAGS.get(13))
      buf += "Anti-Wizard ";
   if (obj.OBJ_FLAGS.get(14))
      buf += "Anti-Ranger ";
   if (obj.OBJ_FLAGS.get(15))
      buf += "Anti-Thief ";
   if (obj.OBJ_FLAGS.get(16))
      buf += "Anti-Alchemist ";
   if (obj.OBJ_FLAGS.get(17))
      buf += "Anti-Cleric ";
   if (obj.OBJ_FLAGS.get(18))
      buf += "Anti-Bard ";
   if (obj.OBJ_FLAGS.get(20))
      buf += "Anti-PC ";
   
   if (buf.Strlen()) {
      buf += "\n";
      show("You believe this object is:  ", pc);
      show(buf, pc);
   }//if
   
   if (obj.OBJ_FLAGS.get(21))
      show("It cannot be worn.\n", pc);
   else {
      show("\nIt can be worn in these positions: ", pc);
      if (obj.OBJ_FLAGS.get(22))
         show("Head ", pc);
      if (obj.OBJ_FLAGS.get(23) || obj.OBJ_FLAGS.get(24))
         show("Neck ", pc);
      if (obj.OBJ_FLAGS.get(25))
         show("Around Body ", pc);
      if (obj.OBJ_FLAGS.get(26))
         show("Arms ", pc);
      if (obj.OBJ_FLAGS.get(27) || obj.OBJ_FLAGS.get(28))
         show("Wrist ", pc);
      if (obj.OBJ_FLAGS.get(29))
         show("Hands ", pc);
      if (obj.OBJ_FLAGS.get(30))
         show("Wielded ", pc);
      if (obj.OBJ_FLAGS.get(31))
         show("Held ", pc);
      if (obj.OBJ_FLAGS.get(32))
         show("Light ", pc);
      if (obj.OBJ_FLAGS.get(33))
         show("Body ", pc);
      if (obj.OBJ_FLAGS.get(34))
         show("Belt ", pc);
      if (obj.OBJ_FLAGS.get(35))
         show("Legs ", pc);
      if (obj.OBJ_FLAGS.get(36))
         show("Feet ", pc);
      if (obj.OBJ_FLAGS.get(37) || obj.OBJ_FLAGS.get(38))
         show("Finger ", pc);
      if (obj.OBJ_FLAGS.get(39))
         show("Shield ", pc);
      
      if (obj.OBJ_FLAGS.get(40)) {
         show("\nIt is not a weapon.\n", pc);
      }
      else {
         if (show_extra) {
            Sprintf(buf, "Weapon Damage: %id%i.\n",
                    obj.getDiceCnt(), obj.getDiceSides());
            pc.show(buf);
         }
         else {
            show("\nIt is a weapon.\n", pc);
         }
      }
   }//else

   // Take care of stat affects.
   if (show_extra) {
      if (!obj.affected_by.isEmpty()) {
         show("It is affected by:\n", pc);
         out_spell_list(obj.affected_by, pc);      
      }

      if (!obj.stat_affects.isEmpty()) {
         show("Its stat affects are:\n", pc);
         show_stat_affects(obj, pc);
      }

      if (!obj.CASTS_THESE_SPELLS.isEmpty()) {
         pc.show("Casts these spells:\n");
         out_spell_list(obj.CASTS_THESE_SPELLS, pc);
         pc.show("\n");
      }//if
   }//if

   return 0;
}//do_lore

//TODO:  Translate
int do_ostat(object& obj, critter& pc) {
   String buf2(100);
   String buf(100);
   object* obj_ptr = &obj;  //to save me typing more!!

   if (!obj.isInUse()) {
      show("This object is NULL, not init'd by the game.\n", pc);
      return -1;
   }//if
   else {
      if (!pc.isUsingClient()) {
         show("\n", pc);
         show((obj_ptr->short_desc), pc);
         show("\n", pc);
         show((obj_ptr->in_room_desc), pc);
         show("\n", pc);
         show((obj_ptr->long_desc), pc);
         show("\n", pc);
      }
      else {
         Sprintf(buf2, "<OSTAT %i>", obj_ptr->OBJ_NUM);
         show(buf2, pc);
         show("\n<STAT_SD>", pc);
         show((obj_ptr->short_desc), pc);
         show("</STAT>\n<STAT_ND>", pc);
         show((obj_ptr->in_room_desc), pc);
         show("</STAT>\n<STAT_LD>", pc);
         show((obj_ptr->long_desc), pc);
         show("</STAT>\n", pc);
      }

      Cell<String*> cll(obj_ptr->names);
      String* ptr;
      buf2 = "";
      while ((ptr = cll.next())) {
         buf2 += *ptr;
         buf2 += " ";
      }//while

      if (!pc.isUsingClient()) {
         Sprintf(buf, "\nNames (Keywords): %S", &buf2);
         pc.show(buf);
      }//if
      else { //then show tags...
         Sprintf(buf, "<NAMES %S>\n", &buf2);
         pc.show(buf); //output the client deals with
         pc.show(buf2); //output that they can read
      }//else

      Sprintf(buf2, "\nBelongs to zone:  %i.\n", obj_ptr->OBJ_IN_ZONE);
      show(buf2, pc);

      out_field(obj_ptr->obj_flags, pc, OBJ_FLAGS_NAMES);

      show("\n", pc);
      Sprintf(buf2, "chrgs: %i  rechrg: %i  p_load: %i  cur_in_game: %i.\n", 
              (int)(obj_ptr->extras[0]), (int)(obj_ptr->extras[1]), 
              (int)(obj_ptr->extras[2]), (int)(obj_ptr->getCurInGame()));
      show(buf2, pc);
      Sprintf(buf2, "max_in_game: %i  wt: %i  dice_sides: %i  #dice: %i.\n", 
              (int)(obj_ptr->extras[4]), (int)(obj_ptr->getCurWeight()), 
              (int)(obj_ptr->extras[6]), (int)(obj_ptr->extras[7])); 
      show(buf2, pc);
      Sprintf(buf2, "lvl: %i  v_bit: %i  price: %i  item#: %i zone# %i.\n", 
              (int)(obj_ptr->extras[8]), obj_ptr->cur_stats[0], 
              obj_ptr->cur_stats[1], obj_ptr->cur_stats[2], 
              obj_ptr->cur_stats[3]);
      show(buf2, pc);

      Sprintf(buf2, "cur_room_num: %i  pause: %i\n",
              obj_ptr->getCurRoomNum(), obj_ptr->getPause());
      pc.show(buf2);

      if (obj_ptr->bag) {
         show("\tIt's a bag, here is inventory:\n", pc);
         Cell<object*> ob_cll(obj_ptr->inv);
         object* ob_ptr;
         while ((ob_ptr = ob_cll.next())) {
            Sprintf(buf2, "\t\t%S\n", &(ob_ptr->short_desc));
            show(buf2, pc);
         }//while
         Sprintf(buf2, "max weight:  %i,  percentate weight:  %i  ", 
                 obj_ptr->bag->max_weight, 
                 obj_ptr->bag->percentage_weight);
         show(buf2, pc);
         Sprintf(buf2, "time till disolve:  %i.\n",
                 obj_ptr->bag->time_till_disolve);
         show(buf2, pc);
         show("Bag flags set:\n", pc);
         out_field(obj_ptr->bag->bag_flags, pc, BAG_FLAGS_NAMES);
         Sprintf(buf2, "Key number (zero is NO KEY): %i.\n", 
                 obj_ptr->bag->key_num);
         show(buf2, pc);
      }//if

      show("It is affected by:\n", pc);
      out_spell_list(obj_ptr->affected_by, pc);      

      if (obj_ptr->obj_proc) {
         show("\tIt has SPEC PROCS.\n", pc);
         out_field(obj_ptr->obj_proc->obj_spec_data_flags, pc,
                   OBJ_SPEC_DATA_FLAGS_NAMES);
         if (obj_ptr->obj_proc->construct_data) {
            show("\tIt has construct data.\n", pc);
            Sprintf(buf2, 
       "\tTarget: %i, item1: %i, item2: %i, item3: %i, item4: %i, item5: %i\n",
                    obj_ptr->obj_proc->construct_data->target_object,
                    obj_ptr->obj_proc->construct_data->item1,
                    obj_ptr->obj_proc->construct_data->item2,
                    obj_ptr->obj_proc->construct_data->item3,
                    obj_ptr->obj_proc->construct_data->item4,
                    obj_ptr->obj_proc->construct_data->item5);
            show(buf2, pc);
         }//if construct data
         
         if (obj_ptr->CASTS_THESE_SPELLS.size() > 0) {
            pc.show("\tCasts these spells:\n\t ");
            out_spell_list(obj_ptr->CASTS_THESE_SPELLS, pc);
            pc.show("\n");
         }//if
         else {
            pc.show("Casts NO SPELLS.\n");
         }
      }//if obj proc

      pc.show("Stat affects are:\n");
      show_stat_affects(*obj_ptr, pc);

      if (obj_ptr->in_list) 
         show("It IS a SOBJ.\n", pc);
      else 
         show("Its NOT a SOBJ.\n", pc); 
   }//else
   return 0;
}//do_ostat


int rstat(int i_th, critter& pc) {
   String buf(100);

   if (ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      if (i_th == 1) {
         i_th = pc.getCurRoomNum();
      }//if
      if (check_l_range(i_th, 0, NUMBER_OF_ROOMS, pc, TRUE)) {
         Sprintf(buf, "Here is room #%i.\n", i_th);
         room_list[i_th].stat(pc);
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
            Sprintf(buf, "Here is door #%i.\n", i_th);
            do_dstat(door_list[i_th], pc);
            return 0;
         }//if
      }//if
      else {
         dr_ptr = door::findDoor(ROOM.doors, i_th, name, pc.SEE_BIT, 
                                 ROOM);
         if (!dr_ptr) {
            show("You don't see that door.\n", pc);
            return -2;
         }//if
         else {
            Sprintf(buf, "Dest:  %i Distance:  %i In_room:  %i  Ticks to disolve:  %i.\n",
                    dr_ptr->destination, dr_ptr->distance, dr_ptr->in_room,
                    dr_ptr->ticks_till_disolve);
            show(buf, pc);
            do_dstat(*(dr_ptr->dr_data), pc);
            return 0;
         }//else
      }//else
   }
   return -1;
}//dstat


int do_dstat(door_data& dr, critter& pc) {
   String buf2(100);

   if (Top(dr.names)) {
      show(*(Top(dr.names)), pc);
      show("\n", pc);
   }//if
   show(dr.long_desc, pc);

   show("\nFlags set:", pc);
   out_field(dr.door_data_flags, pc, DOOR_DATA_FLAGS_NAMES);

   show("\n", pc);
   Sprintf(buf2, "v_bit: %i  DOOR#: %i  key_num: %i  token_num: %i.\n", 
           dr.vis_bit, dr.door_num, dr.key_num, dr.token_num);
   show(buf2, pc);
   return 0;
}//do_dstat


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
