// $Id: script.cc,v 1.24 2001/04/21 07:15:29 eroper Exp $
// $Revision: 1.24 $  $Author: eroper $ $Date: 2001/04/21 07:15:29 $

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

///**********************  script.cc  ******************************///
// This file contains methods pertaining to mob scripting.  This is where
// new conditional statements, and other modifications should go.
//

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
#include <fstream.h>
#include "batl_prc.h"
#include <PtrArray.h>
#include <unistd.h>
#include "load_wld.h"
#include <KVPair.h>


// initialize static variable.
int ScriptCmd::_cnt = 0;


/**  This will not kill, but can set HP to 1 in worst case. */
int affect_crit_stat(StatTypeE ste, String& up_down, int i_th,
                     String* victim, int dice_cnt, int dice_sides,
                     room* rm, critter* caller = NULL) {

   critter* crit_ptr;
   String buf(100);

   if (caller && 
       (caller->isCharmed() || 
        (caller->isPc() && (caller->getImmLevel() < 7)))) {
      caller->show("Huh??\n");
      return -1;
   }

   crit_ptr = rm->haveCritNamed(i_th, victim, ~0);

   if (!crit_ptr) {
      return -1;
   }//if

   if (crit_ptr->isMob()) {
      crit_ptr = mob_to_smob(*crit_ptr,  rm->getIdNum(), TRUE, i_th,
                             victim, ~0);
   }//if

   switch (ste) 
      {
      case STAT_HP: {
         if (strcasecmp(up_down, "up") == 0) {
            crit_ptr->setHP(crit_ptr->getHP() + d(dice_cnt, dice_sides));
         }
         else {
            crit_ptr->setHP(max(crit_ptr->getHP() - d(dice_cnt, dice_sides), 1));
         }
         return crit_ptr->getHP();
      }
      case STAT_MANA: {
         if (strcasecmp(up_down, "up") == 0) {
            crit_ptr->setMana(crit_ptr->getMana() + d(dice_cnt, dice_sides));
         }
         else {
            crit_ptr->setMana(max(crit_ptr->getMana() - d(dice_cnt, dice_sides), 1));
         }
         return crit_ptr->getMana();
      }
      case STAT_MOV: {
         if (strcasecmp(up_down, "up") == 0) {
            crit_ptr->setMov(crit_ptr->getMov() + d(dice_cnt, dice_sides));
         }
         else {
            crit_ptr->setMov(max(crit_ptr->getMov() - d(dice_cnt, dice_sides), 1));
         }
         return crit_ptr->getMov();
      }
      default:
        return -1;
      }//switch
}//affect_crit_stat


/** Exact some damage. */
int exact_damage(int dice_cnt, int dice_sides, String& msg, critter& pc, int was_ordered = FALSE) {
   
   if (pc.isMob()) {
      mudlog << "ERROR:  MOB sent to exact_damage, should be pc or SMOB.\n";
      return -1;
   }

   if (was_ordered) {
      return FALSE;
   }

   dice_cnt = bound(0, 20, dice_cnt);
   dice_sides = bound(0, 100, dice_sides);

   pc.emote(msg);

   exact_raw_damage(d(dice_cnt, dice_sides), NORMAL, pc);

   if (pc.HP < 0) { //was a fatality
      agg_kills_vict(&pc, pc);
   }

   return 0;
}//exact_damage


/** Used to provide branching in scripts..  The lengths of the
 * cooked* arrays passed in are directly related to the cooked*
 * arrays found in parse.cc. 
 */
int script_jump_true(String* cooked_strs, int* cooked_ints,
                     critter& script_targ, critter* c_script_owner,
                     room* r_script_owner, int sanity) {
   return script_jump(TRUE, cooked_strs, cooked_ints, script_targ,
                      c_script_owner, r_script_owner, sanity);
}//script_jump_true

int script_jump_false(String* cooked_strs, int* cooked_ints,
                      critter& script_targ, critter* c_script_owner,
                      room* r_script_owner, int sanity) {
   return script_jump(FALSE, cooked_strs, cooked_ints, script_targ,
                      c_script_owner, r_script_owner, sanity);
}


/**  Does a great many comparisons.   Supported fields are:
 *  HP, MANA, MOV, ALIGN
 */
int is_equal_to(int i_th, const String& rhs_critter,
                const String& field, critter& pc) {
   return do_mob_comparison(i_th, rhs_critter, field, pc, CT_EQUALS);
}//is_equal_to
         

/**  Does a great many comparisons.  Supported fields are:
 *  HP, MANA, MOV, ALIGN
 */
int is_greater_than(int i_th, const String& rhs_critter,
                    const String& field, critter& pc) {
   return do_mob_comparison(i_th, rhs_critter, field, pc, CT_GT);
}//isGreaterThan

/**  Does a great many comparisons.  Supported fields are:
 *  HP, MANA, MOV, ALIGN
 */
int is_less_than(int i_th, const String& rhs_critter,
                 const String& field, critter& pc) {
   return do_mob_comparison(i_th, rhs_critter, field, pc, CT_LT);
}//isLessThan

int do_mob_comparison(int i_th, const String& rhs_critter,
                      const String& field, critter& pc, ComparisonTypeE ct) {

   if (mudlog.ofLevel(SCRIPT)) {
      mudlog << "do_mob_comparison, i_th: " << i_th << " rhs_critter: "
             << rhs_critter << " field: " << field
             << " ComparisonTypeE: " << (int)ct << endl;
   }

   critter* crit_ptr = ROOM.haveCritNamed(i_th, &rhs_critter, ~0);

   int rhs_val;
   int pc_val;

   if (!crit_ptr) {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  Couldn't find crit_ptr, i_th: " << i_th
                << " rhs -:" << rhs_critter << ":-\n";
      }
      return 0;
   }
   
   if (pc.isImmort() || (pc.isSmob() && !pc.isCharmed())) {

      rhs_val = crit_ptr->getFieldValue(field);
      pc_val = pc.getFieldValue(field);

      switch (ct)
         {
         case CT_EQUALS:
            return (rhs_val == pc_val);
         case CT_GT:
            return (pc_val > rhs_val);
         case CT_LT:
            return (pc_val < rhs_val);
         }
   }
   return 0;
}

int is_in_posn(String& str, critter& pc) {
   if (isnum(str)) {
      return pc.POS == atoi(str);
   }
   else {
      int posn = bound(0, POS_HIGH_VAL, pc.POS);
      if (strncasecmp(str, PcPositionStrings[posn],
                      strlen(PcPositionStrings[posn])) == 0) {
         return TRUE;
      }//if
      else {
         return FALSE;
      }
   }//else
}//is_in_posn

/* used for random branching in scripts */
int chance(int cnt, int sides, int val) {

   return (val >= d(cnt,sides));

}//chance

/** Used to provide branching in scripts..  The lengths of the
 * cooked* arrays passed in are directly related to the cooked*
 * arrays found in parse.cc. 
 */
int script_jump(int on_test, String* cooked_strs, int* cooked_ints,
                critter& test_targ, critter* c_code_owner,
                room* r_code_owner, int sanity) {
   //Conditional command is second cooked_str
   // offset is the first cooked int.

   if (mudlog.ofLevel(SCRIPT)) {
      mudlog << "script_jump, on_test: " << on_test << " c_owner: "
             << c_code_owner << " r_owner: " << r_code_owner 
             << " sanity: " << sanity << endl;
   }

   int offset = cooked_ints[1];
   String cmd = cooked_strs[1];

   // First make sure we are a mob with a mob script.  May add pc
   // functionality later??
   int test = !on_test;

   if ((c_code_owner && c_code_owner->isInProcNow()) ||
       (r_code_owner && r_code_owner->isInProcNow())) {

      if (mudlog.ofLevel(SCRIPT)) {
         mudlog << "had an owner, and was inProcNow()" << endl;
      }

      if (strcasecmp(cmd, "TRUE") == 0) {
         test = TRUE;
      }
      else if (strcasecmp(cmd, "FALSE") == 0) {
         test = FALSE;
      }
      else {

         //  The + 1 steps past the first command, which was script_jump_***
         test = test_targ.executeCommand(cooked_strs + 1, cooked_ints + 1,
                                         sanity, c_code_owner, r_code_owner,
                                         FALSE,/* do_sub */
                                         FALSE/* was_ordered*/);
         if (mudlog.ofLevel(SCRIPT)) {
            mudlog << "script_jump, test is: " << test << " on test: "
                   << on_test << endl;
         }

         if (test < 0) {
            if (c_code_owner) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "WARNING:  Script is funky for test_targ:  "
                         << *(test_targ.getName()) << " code_owner: "
                         << *(c_code_owner->getName()) << " failed conditional: "
                         << cmd << " error: " << test << endl;
               }//if
            }//if
            else if (r_code_owner) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "WARNING:  Script is funky for test_targ:  "
                         << *(test_targ.getName()) << " code_owner: "
                         << r_code_owner->getIdNum() << " failed conditional: "
                         << cmd << " error: " << test << endl;
               }
            }//if

            test = FALSE;
         }//if
      }//else

      if ((test && on_test) || (!test && !on_test)) {
         if (c_code_owner)
            c_code_owner->doScriptJump(offset);
         else if (r_code_owner) 
            r_code_owner->doScriptJump(offset);
      }//if

   }//if in proc
   else {
      if (mudlog.ofLevel(WRN)) {
         if (c_code_owner)
            mudlog << "WARNING:  Mob is not scripting in script_jump, code_owner: "
                   << *(c_code_owner->getName()) << endl;
         else if (r_code_owner) 
            mudlog << "WARNING:  Mob is not scripting in script_jump, code_owner: "
                   << r_code_owner->getIdNum() << endl;

      }//if
   }//else
   return 0;
}//script_jump


//used in mob scripts primarily.
 int does_own(critter& pc, int obj1, int obj2, int obj3, int obj4,
             int obj5, int obj6) {

   //figure out if the player owns all the objects in question.
   if (mudlog.ofLevel(SCRIPT)) {
      mudlog << "In does_own, critter:  " << *(pc.getName())
             << " obj1: " << obj1 << " obj2: " << obj2
             << " obj3: " << obj3 << " obj4: " << obj4 << " obj5: "
             << obj5 << " obj6: " << obj6 << endl;
   }//if

   int count1 = 1;
   int count2 = 1;
   int count3 = 1;
   int count4 = 1;
   int count5 = 1;
   int count6 = 1;

   // Get the count for object 1
   if (obj2 == obj1) {
      count1++;
      obj2 = 0;
   }
   if (obj3 == obj1) {
      count1++;
      obj3 = 0;
   }
   if (obj4 == obj1) {
      count1++;
      obj4 = 0;
   }
   if (obj5 == obj1) {
      count1++;
      obj5 = 0;
   }
   if (obj6 == obj1) {
      count1++;
      obj6 = 0;
   }

   // Get the count for object 2
   if (obj3 == obj2) {
      count2++;
      obj3 = 0;
   }
   if (obj4 == obj2) {
      count2++;
      obj4 = 0;
   }
   if (obj5 == obj2) {
      count2++;
      obj5 = 0;
   }
   if (obj6 == obj2) {
      count2++;
      obj6 = 0;
   }
      
   // Get the count for object 3
   if (obj4 == obj3) {
      count3++;
      obj4 = 0;
   }
   if (obj5 == obj3) {
      count3++;
      obj5 = 0;
   }
   if (obj6 == obj3) {
      count3++;
      obj6 = 0;
   }
   // Get the count for object 4
   if (obj5 == obj4) {
      count4++;
      obj5 = 0;
   }
   if (obj6 == obj4) {
      count4++;
      obj6 = 0;
   }
   // Get the count for object 5
   if (obj6 == obj5) {
      count5++;
      obj6 = 0;
   }

   if ((obj1 <= 1) || (pc.haveObjNumbered(count1, obj1))) {
      if ((obj2 <= 1) || (pc.haveObjNumbered(count2, obj2))) {
         if ((obj3 <= 1) || (pc.haveObjNumbered(count3, obj3))) {
            if ((obj4 <= 1) || 
                (pc.haveObjNumbered(count4, obj4))) {
               if ((obj5 <= 1) ||
                   (pc.haveObjNumbered(count5, obj5))) {
                  if ((obj6 <= 1) ||
                      (pc.haveObjNumbered(count6, obj6))) {
                     if (mudlog.ofLevel(SCRIPT)) {
                        mudlog << "Returning TRUE" << endl;
                     }
                     return TRUE;
                  }//if
               }//if
            }//if
         }//if
      }//if
   }//if
   
   if (mudlog.ofLevel(SCRIPT)) {
      mudlog << "Returning FALSE" << endl;
   }

   return FALSE;
}//if_owns


///*********************************************************************///
////*********************** GenScript  ****************************///
///*********************************************************************///

int GenScript::_cnt = 0;

GenScript::GenScript() {
   _cnt++;
   stack_ptr = -1;
   target = -1;
   actor = -1;
   precedence = 0;
   needs_compiling = TRUE;
   next_lbl_num = 0;
}

GenScript::GenScript(String& trig, int targ, int act, String& discriminator,
                     int prec) {
   _cnt++;
   trigger_cmd = trig;
   target = targ;
   actor = act;
   stack_ptr = -1;
   needs_compiling = TRUE;
   next_lbl_num = 0;

   trig_discriminator = discriminator;
   if (trig_discriminator.Strlen() > 0) {
      trig_discriminator.Strip();
      trig_discriminator.Prepend(" ");
      trig_discriminator.Append(" ");
   }

   precedence = prec;
}//Full constructor

GenScript::GenScript(const GenScript& src) {
   _cnt++;
   *this = src;
}

void GenScript::doScriptJump(int abs_offset) {
   if ((abs_offset < compiled_cmds.getCurLen()) &&
       (abs_offset >= 0)) {
      stack_ptr = abs_offset;
   }//if
   else {
      stack_ptr = -1; //all done
      mudlog << "ERROR:  Script jump out of range." << endl;
   }
}//doScriptJump


const ScriptCmd* GenScript::getNextCommand() {
   if (mudlog.ofLevel(SCRIPT)) {
      mudlog << "GenScript::getNextCommand, stack_ptr: " << stack_ptr
             << endl << getRunningScript() << endl;
   }

   if (stack_ptr >= 0) {
      int tmp = stack_ptr;
      stack_ptr++;
      if (mudlog.ofLevel(SCRIPT)) {
         mudlog << "GenScript::getNextCommand, tmp: " << tmp << endl;
         if (running_cmds[tmp])
            mudlog << " returning: " << running_cmds[tmp]->getCommand() << endl;
      }
      return running_cmds[tmp];
   }//if
   else {
      if (mudlog.ofLevel(SCRIPT)) {
         mudlog << "GenScript::getNextCommand, stack_ptr: " << stack_ptr
                << " returning NULL" << endl;
      }
      return NULL;
   }
}//getNextCommand


/**  This txt_ is really a long glob of commands, seperated by
 * semi-colons.
 */
void GenScript::setScript(const String& txt_) {
   mudlog.log(DBG, "In setScript.");

   String txt(txt_);
   String* tmp_str;

   //mudlog.log(DBG, txt);
   //mudlog.log(DBG, "About to clear ptr list.");
   script_cmds.clearAndDestroy();

   //mudlog.log(DBG, "Going into while loop.");
   while ((tmp_str = txt.getUntil(';'))) {
      appendCmd(*tmp_str);
      delete tmp_str;
   }//while

   stack_ptr = -1;
   needs_compiling = TRUE;

   compile(); //Compile into script assembly code

   //mudlog.log(DBG, "Done w/addScript.");
}//addScript


GenScript& GenScript::operator=(const GenScript& src) {

   if (this == &src)
      return *this;

   //mudlog.log("In GenScript::operator=");
   trigger_cmd = src.trigger_cmd;
   trig_discriminator = src.trig_discriminator;
   stack_ptr = src.stack_ptr;
   needs_compiling = src.needs_compiling;

   script_cmds.becomeDeepCopyOf(src.script_cmds);
   compiled_cmds.becomeDeepCopyOf(src.compiled_cmds);
   running_cmds.becomeDeepCopyOf(src.running_cmds);

   target = src.target;
   actor = src.actor;
   precedence = src.precedence;

   return *this;
}//operator=


GenScript::~GenScript() {
   _cnt--;
   clear();
}

// Static Class Data
char* GenScript::triggers[] = {
   "break", "close", "discuss", "donate",
   "drop", "eat", "enter", "examine", "exit", "fill", "flip", "follow",
   "get", "give", "grab", "group",
   "hit", "insert", "junk", "list", "lock", "look", "meditate", "nod",
   "open", "order", "pay", "pick", "prone", "pull", "push", "put", 
   "remove", "rest", "say", "shake", "shoot", "sit", "slap", "sleep", "stand",
   "tail", "tell", "throw", "turn", "twist", "ungroup", "unlock", "wake",
   "wear", "wield", "yell", NULL };


// Static function, maybe should really check it in the future!
int GenScript::checkScript(const String& str) {
   return str.Strlen();
}


// Static function
int GenScript::validTrigger(const char* trig) {
   int cmp;
   for (int i = 0; triggers[i]; i++) {
      cmp = strcasecmp(triggers[i], trig);
      if (cmp == 0) {
         return TRUE;
      }
      else if (cmp > 0) { //already past it
         return FALSE;
      }
   }//for

   return FALSE;
}//validTrigger


String GenScript::toStringBrief(int client_format, int mob_num,
                                entity_type entity, int idx) {
   String buf(100);
   if (client_format) {
      String tmp_d;
      if (trig_discriminator.Strlen() == 0)
         tmp_d = "NA";
      else
         tmp_d = trig_discriminator;

      String tmp(100);

      if (entity == ENTITY_CRITTER) {
         Sprintf(buf, "<MOB_SCRIPT %S %i %i %i %i %i>", &trigger_cmd,
                 mob_num, actor, target, precedence, idx);
      }
      else if (entity == ENTITY_ROOM) {
         Sprintf(buf, "<ROOM_SCRIPT %S %i %i %i %i %i>", &trigger_cmd,
                 mob_num, actor, target, precedence, idx);
      }
      else if (entity == ENTITY_OBJECT) {
         Sprintf(buf, "<OBJ_SCRIPT %S %i %i %i %i %i>", &trigger_cmd,
                 mob_num, actor, target, precedence, idx);
      }

      Sprintf(tmp, "<DISCRIM %S>", &tmp_d);
      buf.Append(tmp);

      return buf;
   }
   else {
      Sprintf(buf, "Trigger:  %S  Actor Mob:  %i  Target #:  %i  Precedence: %i\n\tDiscriminator:  %S.\n", 
              &trigger_cmd, actor, target, precedence, &trig_discriminator);
   }
   return buf;
}//toStringBrief


/**  This gives the non-compiled, non-running version.  Think of it
 * as source code!
 */
String GenScript::getScript() {
   String buf(1024);

   for (int i = 0; i<script_cmds.getCurLen(); i++) {
      if (script_cmds.constElementAt(i)) {
         buf.Append(script_cmds.constElementAt(i)->getTarget());
         buf.Append(" ");
         buf.Append(script_cmds.constElementAt(i)->getCommand());
         buf.Append(";;\n");
      }//if
   }//while

   return buf;
}//getScript


/**  This gives the compiled, non-running version.  Think of it
 * as assembly code!
 */
String GenScript::getCompiledScript() {
   String buf(1024);
   String tmp(100);

   String targ(50);
   String cmd(100);
   for (int i = 0; i < compiled_cmds.getCurLen(); i++) {
      if (compiled_cmds.constElementAt(i)) {
         targ = compiled_cmds.constElementAt(i)->getTarget();
         cmd = compiled_cmds.constElementAt(i)->getCommand();
         Sprintf(tmp, "[%i] %S %S\n", i, &targ, &cmd);
         buf.Append(tmp);
      }//if
   }//while

   return buf;
}//getCompiledScript

/**  This gives the compiled, running version.  Think of it
 * as machine code!
 */
String GenScript::getRunningScript() {
   String buf(1024);
   String tmp(100);

   //mudlog << "getRunningScript, cur_len:  " << running_cmds.getCurLen()
   //       << endl;

   String targ(50);
   String cmd(100);

   for (int i = 0; i<running_cmds.getCurLen(); i++) {
      if (running_cmds.constElementAt(i)) {
         targ = running_cmds.constElementAt(i)->getTarget();
         cmd = running_cmds.constElementAt(i)->getCommand();
         Sprintf(tmp, "[%i] %S %S\n", i, &targ, &cmd);
         buf.Append(tmp);
      }//if
   }//while

   return buf;
}//getRunningScript


/**  Should these arguments trigger this command? */
int GenScript::matches(const String& cmd, String& arg1, critter& act,
                       int targ, int obj_actor_num = -1) {

   if (mudlog.ofLevel(DBG)) {
      mudlog << "GenScript::Matches(args....)" << endl;
      mudlog << "Cmd:  " << cmd << "  arg1 -:" << arg1 << ":- act:  "
             <<  *(name_of_crit(act, ~0))
             << "  targ:  " << targ << " obj_actor_num: "
             << obj_actor_num << endl;
      mudlog << "this:  " << toStringBrief(0, 0, ENTITY_ROOM, 0) << endl;
   }

   if (strcasecmp(cmd, trigger_cmd) == 0) {
      mudlog.log("commands are equal...");
      // now check to see if arg1 is found ANYWHERE in the
      // discriminator (if it exists)
      if (trig_discriminator.Strlen() > 0) {
         arg1.Tolower();

         // Do special checks (but not for say, discuss, yell, or tell)
         if (strcasecmp(cmd, "say") && strcasecmp(cmd, "yell")
             && strcasecmp(cmd, "tell") && strcasecmp(cmd, "discuss")) {
            //if we're not communications..
            if (strcasecmp(cmd, "give") == 0) { //if we are give cmd
               int obj_num = atoi(arg1);
               if ((obj_num > 0) && (obj_num < NUMBER_OF_ITEMS)) {
                  if (!strstr(trig_discriminator, arg1)) {
                     if (mudlog.ofLevel(DBG)) {
                        mudlog << "Returning false, give, target not right."
                               << endl;
                     }
                     return FALSE;
                  }//if
               }//if
            }//if
            else if (strcasecmp(cmd, "enter") == 0) {
               if (!strstr(trig_discriminator, arg1)) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, enter, descrim wrong."
                            << endl;
                  }
                  return FALSE;
               }
            }//else
            else if (strcasecmp(cmd, "put") == 0) {
               int obj_num = atoi(arg1); //bag
               if ((obj_num > 1) && (obj_num < NUMBER_OF_ITEMS)) {
                  if (!strstr(trig_discriminator, arg1)) {
                     if (mudlog.ofLevel(DBG)) {
                        mudlog << "Returning false, put, descrim wrong."
                               << endl;
                     }
                     return FALSE;
                  }//if
               }//if
            }
            else if ((strcasecmp(cmd, "pull") == 0) || 
                     (strcasecmp(cmd, "push") == 0)) {
               if (!strstr(trig_discriminator, arg1)) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, put, descrim wrong."
                            << endl;
                  }
                  return FALSE;
               }//if
            }

            // Checks for normal discriminators.
            if (strstr(trig_discriminator, "FEM")) {
               if (!act.isFemale()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not FEMALE" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "MALE")) {
               if (!act.isMale()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not MALE" << endl;
                  }
                  return FALSE;
               }//if
            }//if

            if (strstr(trig_discriminator, "NEUTER")) {
               if (!act.isNeuter()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not MALE" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "BARD")) {
               if (!act.isBard()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not BARD" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "WARRIOR")) {
               if (!act.isWarrior()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not Warrior" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "SAGE")) {
               if (!act.isSage()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not SAGE" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "WIZARD")) {
               if (!act.isWizard()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not wizard" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "THIEF")) {
               if (!act.isThief()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not THIEF" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "ALCHEMIST")) {
               if (!act.isAlchemist()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not ALCHEMIST" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "CLERIC")) {
               if (!act.isCleric()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not CLERIC" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "HUMAN")) {
               if (!act.isHuman()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not Human" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "ANITRE")) {
               if (!act.isAnitre()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not ANITRE" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "DARKLING")) {
               if (!act.isDarkling()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not DARKLING" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "DRAGON")) {
               if (!act.isDragon()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not DRAGON" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "DWARF")) {
               if (!act.isDwarf()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not DWARF" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "OGRUE")) {
               if (!act.isOgrue()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not OGRUE" << endl;
                  }
                  return FALSE;
               }//if
            }//if

            if (strstr(trig_discriminator, "ELF")) {
               if (!act.isElf()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not ELF" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "UNDEAD")) {
               if (!act.isUndead()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not UNDEAD" << endl;
                  }
                  return FALSE;
               }//if
            }//if
            
            if (strstr(trig_discriminator, "ANIMAL")) {
               if (!act.isAnimal()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not ANIMAL" << endl;
                  }
                  return FALSE;
               }//if
            }//if

            if (strstr(trig_discriminator, "MONSTER")) {
               if (!act.isMonster()) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, not MONSTER" << endl;
                  }
                  return FALSE;
               }//if
            }//if
         }//if we're not say, discuss, yell, or tell
         else {
            // Don't just let them guess letters...
            if (trig_discriminator.Strlen() >= 5) {
               if (arg1.Strlen() < 3) {
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "Returning false, arg1.strlen < 3" << endl;
                  }
                  return FALSE;
               }
            }
            
            // Look for substring (trig_discriminator) in arg1
            //          haystack, needle
            //mudlog << "Discrim -:" << trig_discriminator << ":-  arg1 -:"
            //       << arg1 << ":-" << endl;
            if (!strstr(arg1, trig_discriminator)) {
               String tmp_arg1(arg1.Strlen() + 5);
               tmp_arg1.Append(" ");
               tmp_arg1.Append(arg1);
               tmp_arg1.Append(" ");
               //mudlog << "Discrim -:" << trig_discriminator << ":-  arg1 -:"
               //       << tmp_arg1 << ":-" << endl;
               if (!strstr(tmp_arg1, trig_discriminator)) {
                  //now try turning all punctuation to spaces...
                  for (int i = 0; i<tmp_arg1.Strlen(); i++) {
                     if (ispunct(tmp_arg1.charAt(i))) {
                        tmp_arg1.setCharAt(i, ' ');
                     }//if
                  }//for

                  if (!strstr(tmp_arg1, trig_discriminator)) {
                     if (mudlog.ofLevel(DBG)) {
                        mudlog << "Returning false, discrim, strstr failed."
                               << endl;
                     }
                     return FALSE;
                  }//if
               }//if
            }//if
         }//else
      }//if, check the discriminator

      // Now, other special checks

      //  Should we check targets??
      if (target > 1) {
         if (targ != target) {
            if (mudlog.ofLevel(DBG)) {
               mudlog << "Returning false, target != targ" << endl;
            }
            return FALSE;
         }//if
      }

      if (actor > 1) {
         // Objects can be the 'actor' for object scripts (only)
         if (obj_actor_num > 0) {
            if (obj_actor_num != actor) {
               if (mudlog.ofLevel(DBG)) {
                  mudlog << "Returning false, OBJ_NUM != actor" << endl;
               }
               return FALSE;
            }
         }
         else {
            if (act.MOB_NUM != actor) {
               if (mudlog.ofLevel(DBG)) {
                  mudlog << "Returning false, MOB_NUM != actor" << endl;
               }
               return FALSE;
            }
         }//else
      }//if

      if (mudlog.ofLevel(DBG)) {
         mudlog << "Returning TRUE" << endl;
      }
      return TRUE;
   }//if

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Returning FALSE, commands not equal." << endl;
   }
   return FALSE;
}//matches


// Consider this a weaker == operator (don't compare actuall script)
int GenScript::matches(const GenScript& src) {
   if (strcasecmp(trig_discriminator, src.trig_discriminator) != 0)
      return FALSE;
   if (strcasecmp(trigger_cmd, src.trigger_cmd) != 0)
      return FALSE;
   if (actor != src.actor)
      return FALSE;
   if (target != src.target)
      return FALSE;

   return TRUE;
}//Matches (GenScript)


void GenScript::clean() {
   running_cmds.clearAndDestroy();
   resetStackPtr();
}


void GenScript::clear() {
   trigger_cmd.Clear();
   trig_discriminator.Clear();

   script_cmds.clearAndDestroy();
   compiled_cmds.clearAndDestroy();
   running_cmds.clearAndDestroy();

   target = actor = -1;
   precedence = 0;
   stack_ptr = -1;
   needs_compiling = TRUE;
   next_lbl_num = 0;
}//clear

int MobScript::_cnt = 0;
int MobScript::parseScriptCommand(ScriptCmd& cmd, critter& owner, int& obj_was_deleted) {
   // Look at first command and see if it has non-standard actors.
   obj_was_deleted = FALSE;
   critter* script_actor = NULL;
   object* obj_script_actor = NULL;

   //Sprintf(tmp_buf, "**M@%ul_%i ", (unsigned int)(&act), rm.getIdNum());
   String targ(100);
   targ = cmd.getTarget();
   if (targ.Strlen() && strncmp("**M@", targ, 4) == 0) {
      //we will assume it's right, make the compiler fix it otherwise.
      char* foo = (char*)((const char*)targ); //do type conversion.
      foo[12] = 0;
      unsigned int mob_ptr = strtoul(foo + 4, NULL, 16);
      foo[12] = '_'; //put it back like it was, for now
      int rm_num = strtol(foo + 13, NULL, 10);

      if (mob_ptr != 0) {
         rm_num = bound(0, NUMBER_OF_ROOMS, rm_num);
         script_actor = room_list[rm_num].haveCritter((critter*)(mob_ptr));
      }
   }//if
   else if (targ.Strlen() && strncmp("**O@", targ, 4) == 0) {
      //we will assume it's right, make the compiler fix it otherwise.
      char* foo = (char*)((const char*)targ); //do type conversion.
      foo[12] = 0;
      unsigned int obj_ptr = strtoul(foo + 4, NULL, 16);
      foo[12] = '_'; //put it back like it was, for now
      int rm_num = strtol(foo + 13, NULL, 10);

      if (obj_ptr != 0) {
         rm_num = bound(0, NUMBER_OF_ROOMS, rm_num);
         obj_script_actor = room_list[rm_num].haveObject((object*)(obj_ptr));
      }
   }//if

   if (obj_script_actor) {
      mudlog.log("Had an obj_script_actor.\n");
      targ = cmd.getCommand(); //reuse targ, it should contain the command now.
      return obj_script_actor->processInput(targ, room_list[obj_script_actor->getCurRoomNum()],
                                            obj_was_deleted);
   }
   else if (!script_actor) {
      script_actor = &owner;
   }//if

   targ = cmd.getCommand(); //reuse targ, it should contain the command now.
   return script_actor->processInput(targ, FALSE, TRUE, &owner, NULL);
}//parseScriptCommand



/** compile() must be called after all appends have been
 * completed.  It is most definately DOES MODIFY it's argument str.
 */
void GenScript::appendCmd(String& str) {
   str.Strip();

   if (str.Strlen() > 0) {
      String targ(30);
      short a,b;

      if (strncmp("**%", str, 3) == 0) {
         targ = str.Get_Command(a,b); //pop it off the front of the string
      }//if
      else {
         targ = "";
      }
      script_cmds.appendShallowCopy(new ScriptCmd(targ, str));
   }//if

   needs_compiling = TRUE;
}//AppendCmd


/**  Do all the substitution needed...  Be sure to reset the
 * stack_ptr after doing this...  TODO:  Do it here??
 * Also
 */
void GenScript::generateScript(String& cmd, String& arg1, critter& act,
                               int targ, room& rm, critter* script_owner,
                               object* object_owner = NULL) {

   //mudlog << "GenScript::generateScript, compiled_cmds.cur_len: "
   //       << compiled_cmds.getCurLen() << endl;

   if (needs_compiling)
      compile();

   critter* targ_crit = NULL;
   object* targ_obj = NULL;

   if (targ > 0) {
      // first, generate a critter if possible 
      if (check_l_range(targ, 0, NUMBER_OF_MOBS, act, FALSE)) {
         if (mob_list[targ].CRIT_FLAGS.get(18))
            targ_crit = &(mob_list[targ]);
      }

      if (check_l_range(targ, 0, NUMBER_OF_ITEMS, act, FALSE)) {
         if (obj_list[targ].OBJ_FLAGS.get(10))
            targ_obj = &(obj_list[targ]);
      }
   }

   int len;
   ScriptCmd tmp_cmd;
   String cmd_str(50);
   String targ_str(50);
   String buf(500);
   char tmp_buf[100];
   char ch;

   running_cmds.clearAndDestroy(); //null out entire array.

   for (int idx = 0; idx < compiled_cmds.getCurLen(); idx++) {
      
      if (compiled_cmds[idx]) {
         tmp_cmd = *(compiled_cmds[idx]);
      }
      else {
         mudlog << "ERROR:  idx[" << idx 
                << "] Got a null cmd when generating script." << endl;
         continue;
      }

      // First, take care of special case where we are specifying the
      // actor with the first command of:  **%M

      targ_str = tmp_cmd.getTarget();
      

      if (strncmp("**%M", targ_str, 4) == 0) { //actor
         sprintf(tmp_buf, "**M@%8X_%i ", (unsigned int)(&act), rm.getIdNum());
         targ_str = tmp_buf;
      }//if
      else if (strncmp("**%m", targ_str, 4) == 0) { //target
         sprintf(tmp_buf, "**M@%8X_%i ", (unsigned int)(targ_crit),
                 rm.getIdNum());
         targ_str = tmp_buf;
      }//if
      else if (strncmp("**%S", targ_str, 4) == 0) { //script owner, default
         if (object_owner) {
            // For Object Scripts
            sprintf(tmp_buf, "**O@%8X_%i ", (unsigned int)(object_owner),
                    rm.getIdNum());
         }
         else {
            sprintf(tmp_buf, "**M@%8X_%i ", (unsigned int)(script_owner),
                    rm.getIdNum());
         }
         targ_str = tmp_buf;
      }//if
      else if (strncmp("**%o", targ_str, 4) == 0) { //target
         sprintf(tmp_buf, "**O@%8X_%i ", (unsigned int)(targ_obj),
                 rm.getIdNum());
         targ_str = tmp_buf;
      }//if
      else {
         targ_str = "";
      }

      cmd_str = tmp_cmd.getCommand();

      // ok, got a command, now lets do substitutions.
      // buf accumulates the new command string.
      buf.Clear();
      len = cmd_str.Strlen();
      for (int i = 0; i<len; i++) {
         ch = cmd_str[i];
         //mudlog << "i: " << i << " ch: " << ch << endl;

         if (ch == '%') {
            i++;

            if (i >= len)
               break;

            // If here, then check the next char to determine action.
            ch = cmd_str[i];
            //mudlog << "Before switch: i: " << i << " ch: " << ch << endl;
            switch (ch) {
            case '%':
               buf.Append("%");
               break;
            case 'M': //ACTOR
               buf.Append(*(act.getShortName(~0)));
               break;
            case 'm': //target
               if (targ_crit) {
                  buf.Append(*(targ_crit->getShortName(~0)));
               }
               break;
            case 'o': //target
               if (targ_obj) { //this is the short name, btw
                  buf.Append(*(targ_obj->getName(~0)));
               }
               break;
            case 'S': // one running script, if mob
               if (script_owner) {
                  buf.Append(*(script_owner->getName(~0)));
               }//if
               else if (object_owner) {
                  buf.Append(*(object_owner->getName(~0)));
               }
               break;
            default:
               buf.Append("__PARSE ERROR__");
            }//switch
         }//if we found a % sign
         else if (ch == '\n') {
            buf.Append(' ');
         }
         else {
            buf.Append(ch);
         }
      }//for


      // When here, we have a command, ready to be put onto the queue.
      //mudlog << "Setting running_cmds[" << idx << "] " << buf << endl;

      // No need to set and destroy because we cleared earlier.
      running_cmds.set(new ScriptCmd(targ_str, buf), idx);
      //mudlog << "Running_cmds.curlen: " << running_cmds.getCurLen()
      //       << endl;
      //mudlog << getRunningScript() << endl;
   }//for

}//GenerateScript


void GenScript::compile() { //compile into script assembly...
   // The trick will be to turn the normal script into something
   // more easily understood and dealt with by the computer.  For
   // now, we will parse if-then-else statements, and compute labels.

   // First, walk the entire set of commands, translating high level
   // constructs into label jumps.  Later, will translate label jumps
   // into index jumps.

   //mudlog << "In GenScript::compile" << endl;

   compiled_cmds.clearAndDestroy();

   PtrArray<ScriptCmd> first_pass(100); //will hold first pass compile results

   int i = 0;

   //mudlog << "About to parse Block" << endl;

   parseBlockFP(i, script_cmds, first_pass);

   //mudlog << "About to optimize labels." << endl;

   optimizeLabels(first_pass, compiled_cmds);

   if (mudlog.ofLevel(DBG)) {
      //mudlog << "All done compiling, compiled_cmds:" << endl;
      //for (int z = 0; z<compiled_cmds.getCurLen(); z++) {
      //   mudlog << *(compiled_cmds.constElementAt(z)) << endl;
      //}//for
   }//if

   first_pass.clearAndDestroy();
   
   needs_compiling = FALSE;
}//compile


int GenScript::findOffset(List<KVPair<String, int>*>& lst,
                          const String& str) {
   Cell<KVPair<String, int>*> cll(lst);
   KVPair<String, int>* ptr;

   // Key is label, val is the index.

   while ((ptr = cll.next())) {
      if (strcmp(ptr->key, str) == 0) {
         return ptr->val;
      }//if
   }//while
   return -1;
}

void GenScript::optimizeLabels(const PtrArray<ScriptCmd>& incomming,
                               PtrArray<ScriptCmd>& rslts) {
   short eos, tbp; //not really using these...

   //mudlog << "In optimize Labels" << endl;

   PtrList<KVPair<String, int> > labels;

   rslts.clearAndDestroy();

   //find all labels and compute their offsets.
   String first_cmd;
   String cmd_str(100);
   String new_cmd(200);
   String buf(200);
   
   int offset;

   for (int i = 0; i<incomming.getCurLen(); i++) {
      cmd_str = incomming.constElementAt(i)->getCommand();
      first_cmd = cmd_str.Look_Command();

      if (strcasecmp(first_cmd, "label:") == 0) {
         //mudlog << "Found a label on line: " << i << " -:"
         //       << *(incomming.constElementAt(i)) << endl;

         //we don't care what this responds..it's a label cmd, just want to
         // strip it off of the string.
         cmd_str.Get_Command(eos, tbp);

         cmd_str.Strip();

         // Makes a copy of incomming data, btw.
         labels.append(new KVPair<String, int>(cmd_str, i));
      }//if
   }//for

   // So now, we have all the labels... Now lets translate the script_goto's
   // into script_jumps.

   //mudlog << "Optimize Labels:  translating script_goto's into script_jumps"
   //       << endl;

   String tmp_cmd(50);

   for (int i = 0; i<incomming.getCurLen(); i++) {
      cmd_str = incomming.constElementAt(i)->getCommand();
      first_cmd = cmd_str.Look_Command();
      //mudlog << "Line[" << i << "] " << *(incomming.constElementAt(i))
      //       << endl;
      if (strncasecmp(first_cmd, "script_goto", strlen("script_goto")) == 0) {
         //mudlog << "Found script_goto on line: " << i << endl;
         tmp_cmd = cmd_str.Get_Command(eos, tbp); //grab first one again
         tmp_cmd = cmd_str.Get_Command(eos, tbp); //grab second one (label)
         tmp_cmd.Strip();

         offset = findOffset(labels, tmp_cmd);

         if (offset < 0) {
            mudlog << "ERROR:  got a -1 for findOffset for label -:"
                   << tmp_cmd << ":-" << endl;
         }

         cmd_str.Strip();

         if (strncasecmp(first_cmd, "script_goto_false",
                        strlen("script_goto_false")) == 0) {
            Sprintf(buf, "script_jump_false %i %S", offset, &cmd_str);
         }
         else {
            Sprintf(buf, "script_jump_true %i %S", offset, &cmd_str);
         }

         rslts.appendShallowCopy(new ScriptCmd(incomming.constElementAt(i)->getTarget(), buf));
      }//if
      else {
         rslts.appendShallowCopy(new ScriptCmd(*(incomming.constElementAt(i))));
      }//for
   }//for

   labels.clearAndDestroy();
}//optimizeLabels


String GenScript::getNextLabel() {
   String retval(30);
   Sprintf(retval, "LaBeL__%i", next_lbl_num);
   next_lbl_num++;
   return retval;
}


void GenScript::parseBlockFP(int& start_idx,
                             const PtrArray<ScriptCmd>& incomming,
                             PtrArray<ScriptCmd>& rslts) {
   short eos, tbp; //not really using these...

   String first_cmd;
   String cmd_str(100);
   String new_cmd(200);
   String tmp2(100);
   int i;

   //mudlog << "In parseBlockFP:  idx:  " << start_idx << endl;

   for (i = start_idx; i<incomming.getCurLen(); i++) {
      cmd_str = incomming.constElementAt(i)->getCommand();
      // lets look at the command, and deal with it.
      first_cmd = cmd_str.Look_Command();

      //mudlog << "Incomming[" << i << "] " << *(incomming.constElementAt(i))
      //       << "  FirstCommand:  " << first_cmd << endl;

      if (strcasecmp(first_cmd, "if") == 0) {
         //mudlog << "Was an 'if'" << endl;

         //insert a label jump in rslts.
         cmd_str.Get_Command(eos, tbp); //strip first "if" cmd off of it

         // Now get next two labels.
         String lbl_false = getNextLabel();
         Sprintf(new_cmd, "script_goto_false %S %S", &lbl_false, &cmd_str);

         rslts.appendShallowCopy(new ScriptCmd(incomming.constElementAt(i)->getTarget(), new_cmd));

         //if it's true, then just fall through...

         // Now, get all stuff up to the else (or end)

         PtrArray<ScriptCmd> internal_block;

         // recurse
         i++;
         parseBlockFP(i, incomming, internal_block);

         // we grab pointers, shallow copy, so there is no need to
         // delete the new data allocated for internal_block
         rslts.appendShallowCopy(internal_block);

         // Now do the second block, the else.
         internal_block.clear(); //don't delete ptrs

         // We want to jump over the 'else' part if we ran the first
         // part, so make a jump to the end of the entire "if".
         String lbl_eo_if = getNextLabel();
         Sprintf(new_cmd, "script_goto_true %S TRUE", &lbl_eo_if);
         rslts.appendShallowCopy(new ScriptCmd(NULL_STRING, new_cmd));

         // Now append the false label from above.
         Sprintf(tmp2, "label: %S", &lbl_false);
         rslts.appendShallowCopy(new ScriptCmd(NULL_STRING, tmp2));

         parseBlockFP(i, incomming, internal_block);
         i--; //parseBlock sets to next one, and for loop increments as well.
         // So, decrement it by one to negate for-loop increment in this case.

         rslts.appendShallowCopy(internal_block);
         internal_block.clear(); //just null out ptrs, don't delete data

         Sprintf(tmp2, "label: %S", &lbl_eo_if);
         rslts.appendShallowCopy(new ScriptCmd(NULL_STRING, tmp2));

         // Now, increment i (automatically, and continue!
      }//if it was an if statement
      else if (strcasecmp(first_cmd, "else") == 0) {
         //then done with a block.
         start_idx = i + 1;
         return;
      }
      else if (strcasecmp(first_cmd, "end") == 0) {
         //then done with a block.
         start_idx = i + 1;
         return;
      }
      else {
         rslts.appendShallowCopy(new ScriptCmd(*(incomming.constElementAt(i))));
      }//else
   }//for
   start_idx = i + 1;
}//parseBlockFP


void GenScript::read(ifstream& da_file) {
   char buf[100];
   clear();
   mudlog.log(DB, "in GenScript::Read()");

   if (!da_file) {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  da_file FALSE in GenScript read." << endl;
      }
      return;
   }

   da_file >> buf;

   trigger_cmd = buf;

   if (mudlog.ofLevel(DB))
      mudlog << "Trigger: -:" << trigger_cmd << ":-" << endl << flush;

   da_file >> target >> actor >> precedence;


   if (mudlog.ofLevel(DB))
      mudlog << "Target: " << target << " actor: " << actor << " takes_prec: "
             << precedence << endl;

   da_file.getline(buf, 80);

   if (mudlog.ofLevel(DB))
      mudlog << "Rest of line -:" << buf << ":-" << endl;

   trig_discriminator.Getline(da_file, 80);

   if (mudlog.ofLevel(DB))
      mudlog << "Trig_discriminator -:" << trig_discriminator << ":-" << endl;

   if (trig_discriminator.Strlen() > 0) {
      trig_discriminator.Strip();
      trig_discriminator.Prepend(" ");
      trig_discriminator.Append(" ");
   }

   // now, read in the command script
   // It will be one large chuck of text, which will be parsed
   // and put into the queue.
   String sbuf(1024);
   sbuf.Termed_Read(da_file);

   if (mudlog.ofLevel(DB)) {
      mudlog << "Script read:  Termed Read:  -:" << sbuf << ":-\n" << endl;
   }

   String* tmp_str;
   while ((tmp_str = sbuf.getUntil(';'))) {
      if (mudlog.ofLevel(DB)) {
         mudlog << "Script read:  Get Until:  -:" << *tmp_str << ":-" << endl;
      }
      appendCmd(*tmp_str);
      delete tmp_str;
   }

   compile(); //Compile into script assembly code
   
}//Read


void GenScript::write(ofstream& da_file) const {
   da_file << trigger_cmd << " " << target 
           << " " << actor << " " << precedence 
           << "\t Trigger Command, targ, actor \n";
   da_file << trig_discriminator << endl;

   String output(1024);
   for (int i = 0; i<script_cmds.getCurLen(); i++) {
      if (script_cmds.constElementAt(i)) {
         output.Append(script_cmds.constElementAt(i)->getTarget());
         output.Append(" ");
         output.Append(script_cmds.constElementAt(i)->getCommand());
         output.Append(";\n");
      }//if
   }//for

   da_file << output << "\n~\n";
}//Write


int RoomScript::_cnt = 0;
int RoomScript::parseScriptCommand(ScriptCmd& cmd, room& owner, int& obj_was_deleted) {
   obj_was_deleted = FALSE;
   if (mudlog.ofLevel(DBG)) {
      mudlog << "RoomScript::parseScriptCommand, target -:"
             << cmd.getTarget() << ":-  cmd -:"
             << cmd.getCommand() << ":-  owner id: "
             << owner.getIdNum() << endl;
   }

   String command(cmd.getCommand());
   // Look at first command and see if it has non-standard actors.
   critter* script_actor = NULL;
   object* obj_script_actor = NULL;

   // This is how the target is created, btw.
   //Sprintf(tmp_buf, "**M@%ul_%i ", (unsigned int)(&act), rm.getIdNum());

   String targ(50);
   targ = cmd.getTarget();
   if (targ.Strlen() && strncmp("**M@", targ, 4) == 0) {
      //we will assume it's right, make the compiler fix it otherwise.
      char* foo = (char*)((const char*)targ); //do type conversion.
      foo[12] = 0;
      unsigned int mob_ptr = strtoul(foo + 4, NULL, 16);
      foo[12] = '_'; //put it back like it was, for now
      int rm_num = strtol(foo + 13, NULL, 10);

      if (mob_ptr != 0) {
         rm_num = bound(0, NUMBER_OF_ROOMS, rm_num);
         script_actor = room_list[rm_num].haveCritter((critter*)(mob_ptr));
      }
   }//if
   else if (targ.Strlen() && strncmp("**O@", targ, 4) == 0) {
      //we will assume it's right, make the compiler fix it otherwise.
      char* foo = (char*)((const char*)targ); //do type conversion.
      foo[12] = 0;
      unsigned int obj_ptr = strtoul(foo + 4, NULL, 16);
      foo[12] = '_'; //put it back like it was, for now
      int rm_num = strtol(foo + 13, NULL, 10);

      if (obj_ptr != 0) {
         rm_num = bound(0, NUMBER_OF_ROOMS, rm_num);
         obj_script_actor = room_list[rm_num].haveObject((object*)(obj_ptr));
      }
   }//if

   if (obj_script_actor) {
      mudlog.log("Had an obj_script_actor.\n");
      targ = cmd.getCommand(); //reuse targ, it should contain the command now.
      return obj_script_actor->processInput(targ, room_list[obj_script_actor->getCurRoomNum()],
                                            obj_was_deleted);
   }
   else if (script_actor) {
      mudlog.log("Had a script_actor.\n");
      targ = cmd.getCommand(); //reuse targ, it should contain the command now.
      return script_actor->processInput(targ, FALSE, TRUE, NULL, &owner);
   }
   else {
      mudlog.log("Room was the owner.\n");
      targ = cmd.getCommand(); //reuse targ, it should contain the command now.
      return owner.processInput(targ);
   }
}//parseScriptCommand

int ObjectScript::_cnt = 0;
int ObjectScript::parseScriptCommand(ScriptCmd& cmd, object& owner, int& obj_was_deleted) {
   obj_was_deleted = FALSE;
   if (mudlog.ofLevel(DBG)) {
      mudlog << "ObjectScript::parseScriptCommand, target -:"
             << cmd.getTarget() << ":-  cmd -:"
             << cmd.getCommand() << ":-  owner id: "
             << owner.getIdNum() << " obj_cur_room# " << owner.getCurRoomNum()
             << endl;
   }

   String command(cmd.getCommand());
   // Look at first command and see if it has non-standard actors.
   critter* script_actor = NULL;
   object* obj_script_actor = NULL;

   // This is how the target is created, btw.
   //Sprintf(tmp_buf, "**M@%ul_%i ", (unsigned int)(&act), rm.getIdNum());

   String targ(50);
   targ = cmd.getTarget();
   if (targ.Strlen() && strncmp("**M@", targ, 4) == 0) {
      //we will assume it's right, make the compiler fix it otherwise.
      char* foo = (char*)((const char*)targ); //do type conversion.
      foo[12] = 0;
      unsigned int mob_ptr = strtoul(foo + 4, NULL, 16);
      foo[12] = '_'; //put it back like it was, for now
      int rm_num = strtol(foo + 13, NULL, 10);

      if (mob_ptr != 0) {
         rm_num = bound(0, NUMBER_OF_ROOMS, rm_num);
         script_actor = room_list[rm_num].haveCritter((critter*)(mob_ptr));
      }
   }//if
   else if (targ.Strlen() && strncmp("**O@", targ, 4) == 0) {
      //we will assume it's right, make the compiler fix it otherwise.
      char* foo = (char*)((const char*)targ); //do type conversion.
      foo[12] = 0;
      unsigned int obj_ptr = strtoul(foo + 4, NULL, 16);
      foo[12] = '_'; //put it back like it was, for now
      int rm_num = strtol(foo + 13, NULL, 10);

      if (obj_ptr != 0) {
         rm_num = bound(0, NUMBER_OF_ROOMS, rm_num);
         obj_script_actor = room_list[rm_num].haveObject((object*)(obj_ptr));
      }
   }//if

   if (obj_script_actor) {
      mudlog.log("Had an obj_script_actor.\n");
      targ = cmd.getCommand(); //reuse targ, it should contain the command now.
      return obj_script_actor->processInput(targ, room_list[obj_script_actor->getCurRoomNum()],
                                            obj_was_deleted);
   }
   else if (script_actor) {
      mudlog.log("Had a script_actor.\n");
      targ = cmd.getCommand(); //reuse targ, it should contain the command now.
      return script_actor->processInput(targ, FALSE, TRUE, NULL,
                                        &(room_list[owner.getCurRoomNum()]));
   }
   else {
      mudlog.log("Room was the owner.\n");
      targ = cmd.getCommand(); //reuse targ, it should contain the command now.
      return owner.processInput(targ, room_list[owner.getCurRoomNum()],
                                obj_was_deleted);
   }
}//parseScriptCommand

