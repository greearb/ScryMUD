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
      Sprintf(buf, "You are possessing:  %S.\n",
              pc.possessing->getName());
      show(buf, pc);
   }

   if (pc.pc && pc.pc->imm_data) {
      pc.show("Poofin:\n");
      pc.show(pc.pc->poofin);
      pc.show("\nPoofout:\n");
      pc.show(pc.pc->poofout);
      pc.show("\n");
   }

   Sprintf(buf, "Class:  %s.	Race:  %s.\n", 
           get_class_name(pc.CLASS),
           get_race_name(pc.RACE));
   show(buf, pc);

   if (!IsEmpty(pc.affected_by)) {
      show("You are affected by:\n", pc);
      while ((ss_ptr = cll.next())) {
         Sprintf(buf, "\t%s.\n", 
                 (const char*)(SSCollection::instance().getNameForNum(ss_ptr->stat_spell)));
         show(buf, pc);
      }//while
   }//if
   else {
      show("You are currently unaffected by spells.\n", pc);
   }//else

   if (!IsEmpty(pc.pets)) {
      show("You have these for pets:\n", pc);
      out_crit(pc.pets, pc);
   }//if
   show("\n", pc);

   Sprintf(buf, "You have slain %i Players.  You have died %i times.\n",
	   pc.PK_COUNT, pc.DIED_COUNT);
   show(buf, pc);
   Sprintf(buf, "You have %i quest points.\n", pc.QUEST_POINTS);
   show(buf, pc);

   if (pc.temp_crit) {
     if (pc.GUARDED_BY) {
       Sprintf(buf, "You are guarded by %S.\n", 
	       name_of_crit(*(pc.GUARDED_BY), pc.SEE_BIT));
       show(buf, pc);
     }//if
     if (pc.GUARDING) {
       Sprintf(buf, "You are guarding %S.\n", 
	       name_of_crit(*(pc.GUARDING), pc.SEE_BIT));
       show(buf, pc);
     }//if
     if (pc.SHIELDED_BY) {
       Sprintf(buf, "You are shielded by %S.\n", 
	       name_of_crit(*(pc.SHIELDED_BY), pc.SEE_BIT));
       show(buf, pc);
     }//if
     if (pc.SHIELDING) {
       Sprintf(buf, "You are shielding %S.\n", 
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

   Sprintf(buf2, "\n\n%S %S\n", name_of_crit(pc, ~0),
	   &(pc.short_desc));
   show(buf2, pc);

   if (pc.SEX == 0) 
      buf2 = "Female ";
   else if (pc.SEX == 1) 
      buf2 += "Male ";
   else if (pc.SEX == 2) 
      buf2 += "Neuter ";

   buf2 += get_class_name(pc.CLASS);
   buf2 += " of level (";
   buf2.Append(pc.LEVEL);
   buf2.Append(").\n");
   show(buf2, pc);


   Sprintf(buf2, "GOLD %P15[%i]%P27 EXPERIENCE %P40[%i]%P50 AGE %P63[%i]\n",
           pc.GOLD, pc.EXP, pc.getAge());
   show(buf2, pc);

   Sprintf(buf2, "HP %P15[%i/%i]%P27 MANA %P40[%i/%i]%P50 MOVE %P63[%i/%i]\n",
           pc.HP, pc.HP_MAX, pc.MANA, pc.MA_MAX, pc.MOV, pc.MV_MAX);
   show(buf2, pc);

   Sprintf(buf2,
           "HP_REGEN %P15[%i]%P27 MANA_REGEN %P40[%i]%P50 MOVE_REGEN%P63[%i]\n",
           pc.HP_REGEN, pc.MA_REGEN, pc.MV_REGEN);
   show(buf2, pc);

   Sprintf(buf2,
           "ALIGNMENT %P15[%i]%P27 PRACTICES %P40[%i]%P50 WEIGHT%P63[%i/%i]\n",
           pc.ALIGN, pc.PRACS, pc.CRIT_WT_CARRIED, pc.CRIT_MAX_WT_CARRY);
   show(buf2, pc);

   Sprintf(buf2,
           "\nResistances:  AC [%i]  HEAT [%i]  COLD [%i]  ELECTRICAL [%i]  SPELL [%i]\n",
           pc.AC, pc.HEAT_RESIS, pc.COLD_RESIS, pc.ELEC_RESIS, pc.SPEL_RESIS);
   show(buf2, pc);

   Sprintf(buf2,
           "Experience to Next Level  [%i]\n", pc.getXpToNextLevel());
   pc.show(buf2);

   Sprintf(buf2,
           "Damage to you Modifier:  [%i/100]  Damage to foes: [%i/100].\n",
           pc.DAM_REC_MOD, pc.DAM_GIV_MOD);
   show(buf2, pc);

   if (pc.HUNGER == 0) 
      show("You are hungry.\n", pc);
   if (pc.THIRST == 0)
      show("You are thirsty.\n", pc);
   if (pc.DRUGGED > 0) 
      show("You are drugged.\n", pc);


   if (pc.POS == POS_STAND) 
      show("You are standing.\n", pc);
   else if (pc.POS == POS_SIT) 
      show("You are sitting.\n", pc);
   else if (pc.POS == POS_REST) 
      show("You are resting.\n", pc);
   else if (pc.POS == POS_SLEEP) 
      show("You are sleeping.\n", pc);
   else if (pc.POS == POS_MED) 
      show("You are meditating.\n", pc);
   else if (pc.POS == POS_STUN) 
      show("You are stunned.\n", pc);
   else if (pc.POS == POS_DEAD) 
      show("You are dead!\n", pc);
   else if (pc.POS == POS_PRONE) 
      show("You are lying prone.\n", pc);
   else
      show("You position is undefined!\n", pc);

   if (pc.FOLLOWER_OF) {
      Sprintf(buf2, "You are following %S.\n", 
          name_of_crit(*(pc.FOLLOWER_OF), pc.SEE_BIT));
      show(buf2, pc);
   }//if
   else
      show("You are following yourself.\n", pc);

   if (pc.MASTER) {
      Sprintf(buf2, "Your master is %S.\n", 
          name_of_crit(*(pc.MASTER), pc.SEE_BIT));
      show(buf2, pc);
   }//if
   else
      show("You are your own master.\n", pc);

   if (!IsEmpty(pc.IS_FIGHTING)) {
      Sprintf(buf2, "You are fighting %S!\n", 
              name_of_crit(*(Top(pc.IS_FIGHTING)), pc.SEE_BIT));
      show(buf2, pc);
   }//if
   return 0;
}//score



int critter::doUngroup(int i_th, const String* vict) {
   Cell<critter*> cll;
   critter* ptr1, *ptr2;
   String buf(81);

   if (!vict) {
      mudlog.log(ERR, "ERROR:  vict is NULL in ungroup.\n");
      return -1;
   }//if

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
               ptr1->show("The leader of the band is gone.\n");
               ptr1->show("You now follow yourself.\n");
            }//if
         }//while
         GROUPEES.clear();
         show("You have disbanded your group.\n");
      }//if is a leader
      else if (FOLLOWER_OF) { //possibly in group, not leader
         FOLLOWER_OF->FOLLOWERS.loseData(this);
         if (FOLLOWER_OF->GROUPEES.haveData(this)) {
            FOLLOWER_OF->GROUPEES.loseData(this);

            FOLLOWER_OF->GROUPEES.head(cll);
            while ((ptr1 = cll.next())) {   
               Sprintf(buf, "%S has left your group.\n",
                       getName(ptr1->SEE_BIT));
               buf.Cap();
               ptr1->show(buf);
            }//if
         }//while
         else {
            Sprintf(buf, "%S quits following you.\n",
                    getName(FOLLOWER_OF->SEE_BIT));
            buf.Cap();
            FOLLOWER_OF->show(buf);
         }//if
         FOLLOWER_OF = NULL;
         show("You now follow yourself.\n");
      }//if grouped but not a leader
      else { //not in a group
         show("Ok.\n");
      }//else
   }//if no target
   else { //target mentioned
      ptr1 = have_crit_named(GROUPEES, i_th, vict, ~0, *(getCurRoom()));
      if (!ptr1) {
         show("That person is not in your group.\n");
         return -1;
      }//if

      GROUPEES.loseData(ptr1);  //no longer in group
      FOLLOWERS.loseData(ptr1); //no longer a follower

      GROUPEES.head(cll);
      while ((ptr2 = cll.next())) {   
         Sprintf(buf, "%S has left your group.\n",
                 ptr1->getName(ptr2->SEE_BIT));
         buf.Cap();
         ptr2->show(buf);
      }//while

      ptr1->FOLLOWER_OF = NULL;
      ptr1->show("You now follow yourself.\n");
      show("ok.");

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
      show("You are too relaxed.\n", pc);
      return -1;
   }//if

   show("You can detect these exits:\n\n", pc);

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
               buf += "You cannot discern...";
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
   reg_disp = "Visible exits:  ";
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
               show("You don't see that exit.\n", pc);
               return -1;
            }//if
         }//if	    
         if (dr_ptr->isClosed()) {
            if (!dr_ptr->canOpen()) {
               Sprintf(buf, "The %S can't be locked.\n",
                       name_of_door(*(dr_ptr), pc.SEE_BIT));
               show(buf, pc);
            }//if
            else if (dr_ptr->isLocked()) {
               show("Its already locked.\n", pc);
            }//if
            else { //lock it
               if (have_obj_numbered(pc.inv, 1, dr_ptr->getKeyNum(),
                                     pc.SEE_BIT, ROOM)) {
                  Sprintf(buf, "You lock the %S.\n", name_of_door(*(dr_ptr),
                                                                  pc.SEE_BIT));
                  show(buf, pc);
                  dr_ptr->lock();
                  Sprintf(buf, "locks the %S.\n", name_of_door(*dr_ptr, ~0));
                  emote(buf, pc, ROOM, TRUE);
                  
                  String cmd = "lock";
                  ROOM.checkForProc(cmd, NULL_STRING, pc,
                                    dr_ptr->dr_data->door_num);
                  
                  return 0;
               }//if have key
               else {
                  show("You need the key to lock it.\n", pc);
               }//else
            }//else
         }//if closed
         else {
            show("It must be closed first!.\n", pc);
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
               show("That isn't a container.\n", pc);
            }//if
            else if (ob_ptr->bag->key_num == 0) {
               show("That container can't be locked.\n", pc);
            }//if
            else if (ob_ptr->isClosed()) { //is closed
               if (ob_ptr->isLocked()) {
                  show("Its already locked.\n", pc);
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
                     Sprintf(buf, "You lock the %S.\n", name_of_obj(*ob_ptr,
                                                                    pc.SEE_BIT));
                     show(buf, pc);
                     ob_ptr->lock();
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
                     show("You need the key to lock it.\n", pc);
                  }//else
               }//else
            }//if
            else {
               show("It must be closed before you can lock it.\n", pc);
            }//else
         }//if have valid object ptr
         else {
            show("You don't see anything like that you can lock.\n", pc);
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
               show("You don't see that exit.\n", pc);
               return -1;
            }//if
         }//if	    
         if (dr_ptr->isClosed()) {
            if (!dr_ptr->canOpen()) {
               Sprintf(buf, "The %S will open automatically when its ready.\n",
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
                     Sprintf(buf, "As you unlock the %S, %S is consumed!\n",
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
                     Sprintf(buf, "You unlock the %S.\n",
                             name_of_door(*dr_ptr, pc.SEE_BIT));
                  }
                  show(buf, pc);
                  dr_ptr->unlock();
                  Sprintf(buf, "unlocks the %S.\n", name_of_door(*dr_ptr, ~0));
                  emote(buf, pc, ROOM, TRUE);
                  
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
                  show("You don't seem to have the key.\n", pc);
               }//else
            }//if
            else {
               show("Its not locked!\n", pc);
            }//else
         }//if closed
         else {
            show("Its not even closed!\n", pc);
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
               show("That isn't a container.\n", pc);
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
                        Sprintf(buf, "As you unlock %S, %S is consumed!\n",
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
                        Sprintf(buf, "You unlock the %S.\n",
                                ob_ptr->getLongName(pc.SEE_BIT));
                     }
                     show(buf, pc);
                     ob_ptr->unlock();
                     
                     Sprintf(buf, "unlocks the %S.\n", ob_ptr->getLongName());
                     emote(buf, pc, ROOM, TRUE);
                     
                     String cmd = "unlock";
                     ROOM.checkForProc(cmd, NULL_STRING, pc,
                                       ob_ptr->getIdNum());
                     
                     return 0;
                  }//if
                  else {
                     show("You don't have the key.\n", pc);
                  }//else
               }//if locked
               else {
                  show("It isn't locked.\n", pc);
               }//else
            }//if
            else {
               show("Its already open!!.\n", pc);
            }//else
         }//if have valid object ptr
         else {
            show("You don't see anything like that you can lock.\n", pc);
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
               show("You don't see that exit.\n", pc);
               return -1;
            }//if
         }//if	    
         if (dr_ptr->dr_data->isClosed()) {
            if (!dr_ptr->canOpen()) {
               Sprintf(buf, "The %S will open automatically when its ready.\n",
                       name_of_door(*(dr_ptr), pc.SEE_BIT));
               show(buf, pc);
            }//if
            else if ((dr_ptr->isLocked()) || 
                     (dr_ptr->isMagLocked())) {
               //is locked, mag_locked
               Sprintf(buf, "The %S is locked.\n", name_of_door(*(dr_ptr),
                                                                pc.SEE_BIT));
               show(buf, pc);
            }//if
            else {
               dr_ptr->open();
               Sprintf(buf, "You open the %S.\n", name_of_door(*(dr_ptr),
                                                               pc.SEE_BIT));
               show(buf, pc);
               
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
            show("Its already open!!.\n", pc);
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
               show("That isn't a container.\n", pc);
               return -1;
            }//if
            else if (ob_ptr->isClosed()) { //is closed
               if ((ob_ptr->isLocked()) || (ob_ptr->isMagLocked())) {
                  //is locked, mag_locked
                  Sprintf(buf, "The %S is locked.\n", name_of_obj(*(ob_ptr),
                                                                  pc.SEE_BIT));
                  show(buf, pc);
               }//if
               else { 
                  ob_ptr->open();
                  Sprintf(buf, "You open the %S.\n", name_of_obj(*(ob_ptr),
                                                                 pc.SEE_BIT));
                  show(buf, pc);
                  
                  String cmd = "open";
                  ROOM.checkForProc(cmd, NULL_STRING, pc, ob_ptr->OBJ_NUM);
                  
                  return 0;
               }//else
            }//if
            else {
               show("Its already open!!.\n", pc);
            }//else
         }//if have valid object ptr
         else {
            show("You don't see anything like that you can open.\n", pc);
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
               show("You don't see that exit.\n", pc);
               return -1;
            }//if
         }//if
         if (dr_ptr->isOpen()) { //is open
            if (dr_ptr->canClose() &&
                !dr_ptr->isVehicleExit()) { //is closeable
               dr_ptr->close();
               Sprintf(buf, "You close the %S.\n", name_of_door(*(dr_ptr),
                                                                pc.SEE_BIT));
               show(buf, pc);
               
               // send message to other side of the door...
               Sprintf(buf, "%S closes quietly.\n", name_of_door(*dr_ptr, 0));
               show_all(buf, *(dr_ptr->getDestRoom()));
               
               String cmd = "close";
               ROOM.checkForProc(cmd, NULL_STRING, pc, 
                                 dr_ptr->dr_data->door_num);
               
               return 0;
            }//if
            else { 
               Sprintf(buf, "The %S cannot be closed.\n", 
                       name_of_door(*(dr_ptr), pc.SEE_BIT));
               show(buf, pc);
            }//else
         }//if open
         else {
            show("Its already closed!!\n", pc);
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
               show("That isn't a container.\n", pc);
            }//if
            else if (!ob_ptr->BAG_FLAGS.get(2)) { //is open
               if (ob_ptr->BAG_FLAGS.get(9)) { //if non-closeable
                  Sprintf(buf, "The %S can't be closed.\n", name_of_obj(*(ob_ptr),
                                                                        pc.SEE_BIT));
                  show(buf, pc);
               }//if
               else { 
                  ob_ptr->BAG_FLAGS.turn_on(2);
                  Sprintf(buf, "You close the %S.\n", name_of_obj(*(ob_ptr),
                                                                  pc.SEE_BIT));
                  show(buf, pc);
                  
                  String cmd = "close";
                  ROOM.checkForProc(cmd, NULL_STRING, pc, ob_ptr->OBJ_NUM);
                  
                  return 0;
               }//else
            }//if
            else {
               show("Its already closed!!\n", pc);
            }//else
         }//if have valid object ptr
         else {
            show("You don't see anything like that you can close.\n", pc);
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

   show("Saving.....\n", pc);
   pc.save();
   return 0;
}//save


int nogossip(critter& pc) {

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR: MOB trying to go nogossip.\n");
      return -1;
   }//if

   if (pc.CRIT_FLAGS.get(6)) 
      show("You are no longer subject to gossips.\n", pc);
   else
      show("You may now hear gossips.\n", pc);
   pc.CRIT_FLAGS.flip(6); //should toggle gossip channel
   return 0;
}//nog


int eat(int i_th, const String* name, critter& pc) {
   object* obj_ptr;
   String buf(100);

   if (ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
   
      obj_ptr = have_obj_named(pc.inv, i_th, name, pc.SEE_BIT, ROOM);

      if (!obj_ptr) {
         Sprintf(buf, "You don't seem to have the %S.\n", name);
         show(buf, pc);
      }//if
      else if (!(obj_ptr->OBJ_FLAGS.get(61))) {
         Sprintf(buf, "You can't eat the %S.\n", name);
         show(buf, pc);
      }//if
      else if (pc.pc && pc.HUNGER > HUNGER_MAX) {
         show("You are too full to eat any more.\n", pc);
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
         show("Drink from what??\n", pc);
         return -1;
      }//if

      obj_ptr = have_obj_named(pc.inv, i_th, name, pc.SEE_BIT, ROOM);
      
      if (!obj_ptr) {
         obj_ptr = ROOM.haveObjNamed(i_th, name, pc.SEE_BIT);
         in_inv = FALSE;
      }//if
      if (!obj_ptr) {
         Sprintf(buf, "You neither have nor see the %S.\n", name);
         show(buf, pc);
      }//if
      else {  //ok, got some kind of object...lets test it
         if (IsEmpty(obj_ptr->inv)) 
            mudlog.log(INF,
                       "INFO:  container has no inventory...may be a problem.\n");
         
         if (!(obj_ptr->OBJ_FLAGS.get(59))) { //canteen bit
            Sprintf(buf, "The %S is not a liquid container.\n", name);
            show(buf, pc);
         }//if
         else if (pc.pc && pc.THIRST > THIRST_MAX) {
            show("You are too full to drink any more.\n", pc);
         }//if
         else if (obj_ptr->extras[0] == 0) { 
            Sprintf(buf, "You can't shake another drop out of %S.\n", 
                    &(obj_ptr->short_desc));
            show(buf, pc);
         }//if 
         else if (IsEmpty(obj_ptr->inv)) {
            Sprintf(buf, "%S has no trace of liquid in it.\n", 
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
         show("Fill what??\n", pc);
         return -1;
      }//if

      if (source->Strlen() == 0) {
         show("Fill it from where?\n", pc);
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
         Sprintf(buf, "You don't seem to have the %S.\n", targ);
         show(buf, pc);
      }//
      else if (!source_obj) {
         Sprintf(buf, "You can't find the %S.\n", source);
         show(buf, pc);
      }//
      else if (source_obj == targ_obj) {
         show("You empty it into itself.... *duh*.\n", pc);
      }//if
      else if (!(source_obj->OBJ_FLAGS.get(59)) || !(source_obj->bag)) {
         Sprintf(buf, "Nice try, but %S is not a liquid container!!\n", 
                 &(source_obj->short_desc));
         show(buf, pc);
      }//if      
      else if (!(targ_obj->OBJ_FLAGS.get(59)) || !(targ_obj->bag)) {
         Sprintf(buf, "Nice try, but %S is not a liquid container!!\n", 
                 &(targ_obj->short_desc));
         show(buf, pc);
      }//if      
                  /* so both are containers */

      else if ((Top(targ_obj->inv) != Top(source_obj->inv)) && 
               (targ_obj->extras[0] != 0)) {
         Sprintf(buf, "You need to empty your %S first.\n", 
                 name_of_obj(*targ_obj, pc.SEE_BIT));
         show(buf, pc);
      }//if
                  /* ok, liquids are the same */

      else if (source_obj->extras[0] == 0) {
         Sprintf(buf, "The %S is empty already!!\n", name_of_obj(*source_obj,
                                                                 pc.SEE_BIT));
         show(buf, pc);
      }//if
      else if (targ_obj->extras[0] >= targ_obj->bag->max_weight) {
         Sprintf(buf, "The %S is already full!!\n", name_of_obj(*targ_obj,
                                                                pc.SEE_BIT));
         show(buf, pc);
      }//if
      else if (targ_obj->extras[0] == -1) {
         pc.show("That container will never need filling!\n");
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
         Sprintf(buf, "You fill your %S from %S.\n", 
                 targ_obj->getShortName(),
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
         Sprintf(buf, "You don't seem to have the %S.\n", canteen);
         show(buf, pc);
      }//if
      else if (!(obj_ptr->OBJ_FLAGS.get(59))) {
         Sprintf(buf, "The %S is not a liquid container.\n", 
                 Top(obj_ptr->names));
         show(buf, pc);
      }//if
                  /* got a valid canteen */
   
      else if (obj_ptr->extras[0] == 0) {
         Sprintf(buf, "Your %S is already empty.\n", 
                 Top(obj_ptr->names));
         show(buf, pc);
      }//if
      else if (obj_ptr->extras[0] == -1) {
         Sprintf(buf, "You could NEVER empty this %S!!\n", 
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
         Sprintf(buf, "You empty your %S onto the ground.\n", 
                 obj_ptr->getName());
         show(buf, pc);
         return 0;
      }//else
   }
   return -1;
}//empty()


int help(int i_th, String* command, critter& pc) {
   String buf(100);
   String cmd(50);
   String page;

   if (!pc.isPc()) {
      return -1;
   }//if

   String parsed_cmd(parse_hlp_command(*command));

   if (mudlog.ofLevel(DBG)) {
      mudlog << "help:  parsed_cmd -:" << parsed_cmd << ":-  cmd -:"
             << *command << ":-\n";
   }

   if (parsed_cmd.Strlen() == 0) {
      // short cut this to look for skill/spell help first
      if (SSCollection::instance().doHelpFor(*command, pc) >= 0) {
         return 0;
      }
   }

   if (pc.pc->imm_data) {
      if (parsed_cmd.Strlen()) 
         Sprintf(cmd, "./Help/IMM_%S_%i", &parsed_cmd, i_th);
      else
         Sprintf(cmd, "./Help/IMM_help_%i", i_th);

      page = get_page(cmd);

      if (page.Strlen() > 0) {
         show(page, pc);
         return 0;
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
      Sprintf(buf, "Help for Command:  %S\n", &parsed_cmd);
      pc.show(buf);
      pc.show(page);
      return 0;
   }//if
   else {
      Sprintf(buf, "Help for %i.%S is not available now.\n", i_th, command);
      show(buf, pc);
      show("If you think it should be added, use the 'idea' command.\n", pc);
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
      crit_ptr = ROOM.getLastCritter(); //most likely to be a shopkeeper
      if (crit_ptr) {
         if (crit_ptr->isMob()) {
            crit_ptr = mob_to_smob(*crit_ptr, pc.getCurRoomNum());
         }//if
      }//if
   }//if
   else {
      crit_ptr = 
            ROOM.haveCritNamed(j_th, keeper, pc.SEE_BIT);
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
      show("You do not see that person.\n", pc);
   }//if
   else if (!crit_ptr->isNpc()) { //dealing w/a pc...
      Sprintf(buf, "%S won't be traded with so easily!\n", 
              name_of_crit(*crit_ptr, pc.SEE_BIT));
      buf.Cap();
      show(buf, pc);
   }//if
   else if (!crit_ptr->mob->proc_data) { //not a shop keeper
      do_tell(*crit_ptr, "I have nothing I wish to sell.", pc, 
	      FALSE, pc.getCurRoomNum());
   }//if
   else if (!(crit_ptr->FLAG1.get(1))) {
      do_tell(*crit_ptr, "I don't trade for a living.", pc, FALSE,
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
      mudlog.log(TRC, "Setting crit_ptr == to Rear of ROOM.CRITTERS\n");
      crit_ptr = ROOM.getLastCritter(); //most likely to be a shopkeeper
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
      show("You do not see that person.\n", pc);
   }//if
   else if (!crit_ptr->mob) {
      Sprintf(buf, 
              "It might take some real persuading to get %S to trade...\n",
              name_of_crit(*crit_ptr, pc.SEE_BIT));
      show(buf, pc);
   }//if 
   else if (!crit_ptr->mob->proc_data) {
      do_tell(*crit_ptr, "I don't wan't to buy anything from you!", pc, 
              FALSE, pc.getCurRoomNum());
   }//if
   else if (!(crit_ptr->FLAG1.get(1))) {
      do_tell(*crit_ptr, "I don't trade for a living!", pc, FALSE, pc.getCurRoomNum());
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
         crit_ptr = ROOM.getLastCritter(); //get mob who has been there longest
      }//if
      else {
         crit_ptr = ROOM.haveCritNamed(j_th, master, pc.SEE_BIT);
      }//else
      
      if (spell->Strlen() == 0) {
         return abilities(pc);
      }//if
      else if (pc.EQ[9] || pc.EQ[10]) {
         show("You can't practice while holding or wielding something.\n",
              pc);
      }//else
      else if (pc.PRACS < 1) {
         show("You haven't enough practices!\n", pc);
      }//if
      else if (!crit_ptr) {
         show("You do not see that person.\n", pc);
      }//if
      else if (!crit_ptr->mob) {
         show("You cannot learn from another player yet.\n", pc);
      }//if
      else if (!crit_ptr->mob->proc_data) {
         do_tell(*crit_ptr, "I have nothing to teach you!", pc, FALSE, 
                 pc.getCurRoomNum());
      }//if
      else if (!(crit_ptr->FLAG1.get(2))) {
         do_tell(*crit_ptr, "I don't teach for a living!", pc, FALSE,
                 pc.getCurRoomNum());
      }//if
      else {                  /* ok, is a teacher, figure out spell */
         if (!(crit_ptr->TEACH_DATA_FLAGS.get(pc.CLASS))) {
            do_tell(*crit_ptr, "I can't teach those of your profession.", pc,
                    FALSE, pc.getCurRoomNum());
         }//if
         else if (crit_ptr->LEVEL < pc.LEVEL) {
            do_tell(*crit_ptr, 
                    "You have surpassed me, you must find another teacher.",
                    pc, FALSE, pc.getCurRoomNum());
         }//if
         else {  		/* need to figure NUMBER of skill or spell */
            s_num = SSCollection::instance().getNumForName(*spell); 
            if (s_num == -1) {
               Sprintf(buf, "-:%S:- has not been researched yet.\n",
                       spell);
               pc.show(buf);
            }//if
            else {             /* ok, got a valid skill or spell */
               if (SSCollection::instance().getSS(s_num).getMinLevel() > pc.LEVEL) {
                  do_tell(*crit_ptr, 
                          "If I taught you that the forces you could unleash would tear you apart!",
                          pc, FALSE, pc.getCurRoomNum());
               }//if
               else {
                  int p_learned = get_percent_lrnd(s_num, pc);
                  if (p_learned == -1) {
                     do_tell(*crit_ptr, "You don't know enough basics yet.", pc, 
                             FALSE, pc.getCurRoomNum());
                  }//if
                  else if (p_learned < 100) {
                     increment_percent_lrnd(s_num, pc);
                     show("You feel a bit wiser.\n", pc);
                     pc.PRACS--; //decrement the number of practices
                     if ((p_learned < 50) && (get_percent_lrnd(s_num, pc) >= 50)) {
                        update_skill(s_num, pc);
                     }//if
                     return 0;
                  }//if
                  else {
                     show("You know as much as can be taught of this topic.\n",
                          pc);
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
      show("A zero (0) means it is OFF, a one (1) means the flag is on.\n
You may toggle these switches:\n", pc);

      Sprintf(buf, "brief(%i)%P20autosplit(%i)%P45autoloot(%i)\n",
              (int)(pc.PC_FLAGS.get(8)), (int)(pc.PC_FLAGS.get(9)),
              (int)(pc.PC_FLAGS.get(12)));
      show(buf, pc);
      Sprintf(buf, "tank graph(%i)%P20cloak(%i)%P45autoexits(%i)\n",
              (int)(pc.PC_FLAGS.get(4)), (int)(pc.PC_FLAGS.get(3)),
              (int)(pc.PC_FLAGS.get(6)));
      show(buf, pc);

      Sprintf(buf, "client tags(%i)%P20page breaks(%i)%P45ansi color(%i)\n",
              (int)(pc.PC_FLAGS.get(5)), (int)(pc.PC_FLAGS.get(22)),
              (int)(pc.isUsingColor()));
      show(buf, pc);

      Sprintf(buf, 
              "carriage-return(%i)%P20cr behind (default is behind)(%i)\n",
              (int)(pc.PC_FLAGS.get(16)), (int)(pc.PC_FLAGS.get(15)));
      show(buf, pc);
      if (pc.isImmort()) {
         Sprintf(buf, "extra_info(%i)%P20no_hassle(%i)%P45no_vnums(%i)\n",
                 (int)(pc.PC_FLAGS.get(14)), (int)(pc.PC_FLAGS.get(7)),
		 (int)(pc.PC_FLAGS.get(20)));
	 show(buf, pc);
         Sprintf(buf, "detect_inventory(%i)%P20wizchat(%i)\n",
                 (int)(pc.PC_FLAGS.get(19)), (int)(pc.PC_FLAGS.get(24)));
	 show(buf, pc);
      }//if
      return 0;
   }//if

   if (pc.isImmort()) {
      if (strncasecmp(*field, "extra_info", len1) == 0) {
	 pc.PC_FLAGS.flip(14);
	 show("Ok.\n", pc);
	 return 0;
      }//if
      else if (strncasecmp(*field, "detect_inventory", len1) == 0) {
	 pc.PC_FLAGS.flip(19);
	 show("Ok.\n", pc);
	 return 0;
      }//if
      else if (strncasecmp(*field, "no_hassle", len1) == 0) {
	 pc.PC_FLAGS.flip(7);
	 show("Ok.\n", pc);
	 return 0;
      }//if
      else if (strncasecmp(*field, "no_vnums", len1) == 0) {
	 pc.PC_FLAGS.flip(20);
	 show("Ok.\n", pc);
	 return 0;
      }//if
      else if (strncasecmp(*field, "wizchat", len1) == 0) {
         pc.PC_FLAGS.flip(24);
	 show("Ok.\n", pc);
	 return 0;
      }//if         
   }//if

   if (strncasecmp(*field, "tank graph", len1) == 0) 
      pc.PC_FLAGS.flip(4);
   else if (strncasecmp(*field, "client tags", len1) == 0) 
      pc.PC_FLAGS.flip(5);
   else if (strncasecmp(*field, "cloaked", len1) == 0) 
      pc.PC_FLAGS.flip(3);
   else if (strncasecmp(*field, "brief", len1) == 0) 
      pc.PC_FLAGS.flip(8);
   else if (strncasecmp(*field, "cr behind", len1) == 0) 
      pc.PC_FLAGS.flip(15);
   else if (strncasecmp(*field, "carriage-return", len1) == 0) 
      pc.PC_FLAGS.flip(16);
   else if (strncasecmp(*field, "autosplit", len1) == 0) 
      pc.PC_FLAGS.flip(9);
   else if (strncasecmp(*field, "autoloot", len1) == 0) 
      pc.PC_FLAGS.flip(12);
   else if (strncasecmp(*field, "cloak", len1) == 0) 
      pc.PC_FLAGS.flip(3);
   else if (strncasecmp(*field, "autoexits", len1) == 0) 
      pc.PC_FLAGS.flip(6);
   else if (strncasecmp(*field, "page breaks", len1) == 0) 
      pc.PC_FLAGS.flip(22);
   else if (strncasecmp(*field, "ansi color", len1) == 0) 
      pc.PC_FLAGS.flip(26);
   else {
      show("Can't find the switch for that!\n", pc);
      return -1;
   }//else
   
   show("Okay.\n", pc);
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
         crit_ptr = ROOM.getLastCritter(); //get mob who has been there longest
      }//if
      else {
         crit_ptr = ROOM.haveCritNamed(i_th, keeper, pc.SEE_BIT);
      }//else
 

      if (!crit_ptr) {
         show("You do not see that person.\n", pc);
      }//if
      else if (!crit_ptr->mob) {
         show("It would take a more delicate approach....\n", pc);
      }//if
      else if (!crit_ptr->mob->proc_data) {
         do_tell(*crit_ptr, "Eh??", pc, FALSE, pc.getCurRoomNum());
      }//if
      else if (!(crit_ptr->FLAG1.get(1))) {
         do_tell(*crit_ptr, "I don't trade for a living.", pc, FALSE, pc.getCurRoomNum());
      }//if
      else {                  /* ok, is a shopkeeper */
         int cur_time = get_game_time();
         if (crit_ptr->OPEN_TIME < crit_ptr->CLOSE_TIME) {
            if ((cur_time < crit_ptr->OPEN_TIME) || 
                (cur_time > crit_ptr->CLOSE_TIME)) {
               Sprintf(buf, "Hours are from %s to %s.\n", 
                       military_to_am(crit_ptr->OPEN_TIME),
                       military_to_am(crit_ptr->CLOSE_TIME));
               show(buf, pc);
               return -1;
            }//if
         }//if
         else {
            if ((cur_time > crit_ptr->CLOSE_TIME) &&
                (cur_time < crit_ptr->OPEN_TIME)) {
               Sprintf(buf, "Hours are from %s to %s.\n", 
                       military_to_am(crit_ptr->OPEN_TIME),
                       military_to_am(crit_ptr->CLOSE_TIME));
               show(buf, pc);
               return -1;
            }//if
         }//else

         show("These items are for sale:\n", pc);
         
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
                  if (obj_ptr->in_list || (item_counts[id_num] == 1)) {
                     Sprintf(buf, "%P12 %S%P50%i", &(obj_ptr->short_desc),
                             price);
                  }
                  else {
                     Sprintf(buf, "  [*%i]%P12 %S%P50%i", item_counts[id_num],
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
                  if (obj_ptr->in_list || (item_counts[id_num] == 1)) {
                     Sprintf(buf, "%P12 %S%P50%i", &(obj_ptr->short_desc),
                             price);
                  }
                  else {
                     Sprintf(buf, "  [*%i]%P12 %S%P50%i", item_counts[id_num],
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
         crit_ptr = ROOM.getLastCritter(); //get mob who has been there longest
      }//if
      else {
         crit_ptr = ROOM.haveCritNamed(j_th, keeper, pc.SEE_BIT);
      }//else


      if (!crit_ptr) {
         show("You do not see that person.\n", pc);
      }//if
      else if (!crit_ptr->mob) {
         Sprintf(buf, 
                 "It might take some real persuading to get %S to trade...\n",
                 name_of_crit(*crit_ptr, pc.SEE_BIT));
         show(buf, pc);
      }//if 
      else if (!crit_ptr->mob->proc_data) {
         do_tell(*crit_ptr, "I don't want to buy anything from you!", pc, 
                 FALSE, pc.getCurRoomNum());
      }//if
      else if (!(crit_ptr->FLAG1.get(1))) {
         do_tell(*crit_ptr, "I don't trade for a living.",
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

   show("You know of these skills and spells:\n", pc);

   int key;
   int retval = 0;
   if (pc.SKILLS_KNOWN.Min(key)) {
      pc.SKILLS_KNOWN.Find(key, retval);
      Sprintf(buf, "\t%s %P50%i percent\n",
              (const char*)(SSCollection::instance().getNameForNum(key)),
              retval);
      buf.Cap();
      show(buf, pc);
      while (pc.SKILLS_KNOWN.Next(key)) {
         pc.SKILLS_KNOWN.Find(key, retval);
         Sprintf(buf, "\t%s %P50%i percent\n",
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
         Sprintf(buf, "Wimpy is set at:  %i.\n", pc.WIMPY);
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
      show("Ok.\n", pc);
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
         show("You don't see that person here.\n", pc);
         return -1;
      }//if
      else {
         return do_mstat(*crit_ptr, pc);
      }//else
   }
   return -1;
}//mstat


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

         Sprintf(buf2, "\tCur in game:  %i,  Max in game:  %i.\n",
                 crit_ptr->getCurInGame(), crit_ptr->getMaxInGame());
	 show(buf2, pc);

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
         show("You neither see nor have that object.\n", pc);
         return -1;
      }//if
      else {
         return do_lore(*obj_ptr, pc, show_extra);
      }//else
   }
   return -1;
}//lore


int do_lore(object& obj, critter& pc, int show_extra) {
   String buf(100);

   show((obj.short_desc), pc);
   show("\n", pc);
   show((obj.long_desc), pc);
   show("\n", pc);
  
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
      show("It is affected by:\n", pc);
      out_stat_spell_list(obj.affected_by, pc);      

      show("It's stat affects are:\n", pc);
      out_stat_spell_list(obj.stat_affects, pc);
   }//if

   return 0;
}//do_lore


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
      out_stat_spell_list(obj_ptr->affected_by, pc);      

      show("It's stat affects are:\n", pc);
      out_stat_spell_list(obj_ptr->stat_affects, pc);

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
            pc.show("\tCasts these spells:   (spell_level, spell_num)\n\t");
            Cell<stat_spell_cell*> cll(obj_ptr->CASTS_THESE_SPELLS);
            stat_spell_cell* ssptr;
            while ((ssptr = cll.next())) {
               Sprintf(buf2, "(%i, %i) ", ssptr->bonus_duration,
                       ssptr->stat_spell);
               pc.show(buf2);
            }//while
            pc.show("\n");
         }//if
         else {
            pc.show("Casts NO SPELLS.\n");
         }
      }//if obj proc

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
         show("You haven't the power!\n", pc);
         return -1;
      }//if

      pc.show("Saving all.\n");
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
         pc.show("Shutdown how??\n");
         return -2;
      }
      return 0;
   }
   return -1;
}//shutdown()


int log_level(int lvl, critter& pc) {
   if (ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      if (pc.getImmLevel() <= 8) {
         show("You haven't the power!\n", pc);
         return -1;
      }//if

      String buf(100);

      Sprintf(buf, "Setting logging level to:  %i.", lvl);
      pc.show(buf);
      mudlog << "INFO:  Setting logging level to:  " << lvl << endl;

      mudlog.setLevel(lvl);
      return 0;
   }
   return -1;
}//log_level


int rezone(critter& pc) { //forces reload of zone in which pc is
   if (ok_to_do_action(NULL, "IRF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      show("Reloading zone...\n", pc);
      update_zone(ROOM.getZoneNum(), FALSE);
      return 0;
   }
   return -1;
}//rezone();


int total_rezone(critter& pc) {
   if (ok_to_do_action(NULL, "IRF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      show("Totally reloading zone...\n", pc);
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









