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

///**********************  command5.cc  ******************************///
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
#include "SkillSpell.h"
#include "script.h"
#include "olc.h"
#include "BuildInfo.h"

int update_cur_in_game(critter& pc) {
   if (pc.isImmort() && (pc.IMM_LEVEL >= 9)) {
      //first, clear em all out!
      int i = 0;
      for (i = 0; i<NUMBER_OF_MOBS; i++) {
         if (mob_list[i].isInUse()) {
            mob_list[i].setCurInGame(0);
         }//if
      }//for

      for (i = 0; i<NUMBER_OF_ITEMS; i++) {
         if (obj_list[i].isInUse()) {
            obj_list[i].setCurInGame(0);
         }//if
      }//for

      init_loads();

      pc.show("All done!\n");
   }//if
   else {
      pc.show("Eh??");
   }
   return 0;
}//update_cur_in_game


int page_break(int lines, critter& pc) {
   if (pc.isPc()) {
      pc.setLinesOnPage(bound(8, 10000, lines));
      
      pc.show("Page break set.\n");
      
      if (!pc.PC_FLAGS.get(22)) {
         pc.show("You are not using page breaks currently, use the toggle\n");
         pc.show("command (toggle 'page breaks') to turn them on.\n");
      }//if
   }//if
   else {
      pc.show("Eh?\n");
   }
   return 0;
}//page_break

const String* getColorName(const char* ansi_code) {
   if (strcmp(ansi_code, ANSI_BLACK) == 0)
      return &ANSI_BLACK_S;
   else if (strcmp(ansi_code, ANSI_RED) == 0)
      return &ANSI_RED_S;
   else if (strcmp(ansi_code, ANSI_BLUE) == 0)
      return &ANSI_BLUE_S;
   else if (strcmp(ansi_code, ANSI_GREEN) == 0)
      return &ANSI_GREEN_S;
   else if (strcmp(ansi_code, ANSI_YELLOW) == 0)
      return &ANSI_YELLOW_S;
   else if (strcmp(ansi_code, ANSI_BLUE) == 0)
      return &ANSI_BLUE_S;
   else if (strcmp(ansi_code, ANSI_MAGENTA) == 0)
      return &ANSI_MAGENTA_S;
   else if (strcmp(ansi_code, ANSI_CYAN) == 0)
      return &ANSI_CYAN_S;
   else if (strcmp(ansi_code, ANSI_WHITE) == 0)
      return &ANSI_WHITE_S;

   //background colors now
   else if (strcmp(ansi_code, ANSI_BBLACK) == 0)
      return &ANSI_BBLACK_S;
   else if (strcmp(ansi_code, ANSI_BRED) == 0)
      return &ANSI_BRED_S;
   else if (strcmp(ansi_code, ANSI_BBLUE) == 0)
      return &ANSI_BBLUE_S;
   else if (strcmp(ansi_code, ANSI_BGREEN) == 0)
      return &ANSI_BGREEN_S;
   else if (strcmp(ansi_code, ANSI_BYELLOW) == 0)
      return &ANSI_BYELLOW_S;
   else if (strcmp(ansi_code, ANSI_BBLUE) == 0)
      return &ANSI_BBLUE_S;
   else if (strcmp(ansi_code, ANSI_BMAGENTA) == 0)
      return &ANSI_BMAGENTA_S;
   else if (strcmp(ansi_code, ANSI_BCYAN) == 0)
      return &ANSI_BCYAN_S;
   else if (strcmp(ansi_code, ANSI_BWHITE) == 0)
      return &ANSI_BWHITE_S;
   
   // else, unknown
   else
      return &NULL_STRING;
}//getColorName


int isValidColor(const char* color) {
   return ((strcasecmp(color, ANSI_BLACK_S) == 0) ||
           (strcasecmp(color, ANSI_RED_S) == 0) ||
           (strcasecmp(color, ANSI_GREEN_S) == 0) ||
           (strcasecmp(color, ANSI_YELLOW_S) == 0) ||
           (strcasecmp(color, ANSI_BLUE_S) == 0) ||
           (strcasecmp(color, ANSI_MAGENTA_S) == 0) ||
           (strcasecmp(color, ANSI_CYAN_S) == 0) ||
           (strcasecmp(color, ANSI_WHITE_S) == 0) ||

           (strcasecmp(color, ANSI_BBLACK_S) == 0) ||
           (strcasecmp(color, ANSI_BRED_S) == 0) ||
           (strcasecmp(color, ANSI_BGREEN_S) == 0) ||
           (strcasecmp(color, ANSI_BYELLOW_S) == 0) ||
           (strcasecmp(color, ANSI_BBLUE_S) == 0) ||
           (strcasecmp(color, ANSI_BMAGENTA_S) == 0) ||
           (strcasecmp(color, ANSI_BCYAN_S) == 0) ||
           (strcasecmp(color, ANSI_BWHITE_S) == 0));
}//isValidColor


const char* colorToAnsiCode(const char* color_name) {
   if (strcasecmp(color_name, ANSI_BLACK_S) == 0)
      return ANSI_BLACK;
   if (strcasecmp(color_name, ANSI_RED_S) == 0)
      return ANSI_RED;
   if (strcasecmp(color_name, ANSI_GREEN_S) == 0)
      return ANSI_GREEN;
   if (strcasecmp(color_name, ANSI_YELLOW_S) == 0)
      return ANSI_YELLOW;
   if (strcasecmp(color_name, ANSI_BLUE_S) == 0)
      return ANSI_BLUE;
   if (strcasecmp(color_name, ANSI_MAGENTA_S) == 0)
      return ANSI_MAGENTA;
   if (strcasecmp(color_name, ANSI_CYAN_S) == 0)
      return ANSI_CYAN;
   if (strcasecmp(color_name, ANSI_WHITE_S) == 0)
      return ANSI_WHITE;
   
   if (strcasecmp(color_name, ANSI_BBLACK_S) == 0)
      return ANSI_BBLACK;
   if (strcasecmp(color_name, ANSI_BRED_S) == 0)
      return ANSI_BRED;
   if (strcasecmp(color_name, ANSI_BGREEN_S) == 0)
      return ANSI_BGREEN;
   if (strcasecmp(color_name, ANSI_BYELLOW_S) == 0)
      return ANSI_BYELLOW;
   if (strcasecmp(color_name, ANSI_BBLUE_S) == 0)
      return ANSI_BBLUE;
   if (strcasecmp(color_name, ANSI_BMAGENTA_S) == 0)
      return ANSI_BMAGENTA;
   if (strcasecmp(color_name, ANSI_BCYAN_S) == 0)
      return ANSI_BCYAN;
   if (strcasecmp(color_name, ANSI_BWHITE_S) == 0)
      return ANSI_BWHITE;

   return "";
}//colorToAnsiCode

int color(String& var, String& val_rough, critter& pc) {
   String val;
   String buf(100);

   int ln;
   const char* usage =
"USAGE:  color [text_type] [color]
See 'help color' for more information.\n";

   if (pc.isPc()) {
      ln = max(var.Strlen(), 3);

      if (var.Strlen() == 0) {
         Sprintf(buf, "Say: %P18 %S %P35 Yell: %P53 %S\n",
                 getColorName(pc.pc->say_str), getColorName(pc.pc->yell_str));
         pc.show(buf);
         Sprintf(buf, "Gossip: %P18 %S %P35 Tell: %P53 %S\n",
                 getColorName(pc.pc->gos_str), getColorName(pc.pc->tell_str));
         pc.show(buf);
         Sprintf(buf, "Desc: %P18 %S %P35 Objects: %P53 %S\n",
                 getColorName(pc.pc->desc_str),
                 getColorName(pc.pc->obj_list_str));
         pc.show(buf);
         Sprintf(buf, "Critters: %P18 %S %P35 Default: %P53 %S\n",
                 getColorName(pc.pc->mob_list_str), getColorName(pc.pc->dflt_str));
         pc.show(buf);
         Sprintf(buf, "Battle: %P18 %S %P35 Background: %P53 %S\n",
                 getColorName(pc.pc->battle_str), getColorName(pc.pc->bk_str));
         pc.show(buf);
      }//if
      else if (strncasecmp("OFF", var, 3) == 0) {
         pc.PC_FLAGS.turn_off(26); //stop using them
         pc.show(ANSI_NORMAL);
         //pc.show(ANSI_BWHITE);
         pc.show("Turning off color mode.  Use the toggle command, or\n");
         pc.show("color ON to re-enable it.\n");
      }
      else if (strncasecmp("NORMAL", var, 3) == 0) {
         // set to defaults
         pc.pc->gos_str = ANSI_NORMAL;
         pc.pc->say_str = ANSI_NORMAL;
         pc.pc->yell_str = ANSI_NORMAL;
         pc.pc->tell_str = ANSI_NORMAL;
         pc.pc->desc_str = ANSI_NORMAL;
         pc.pc->obj_list_str = ANSI_NORMAL;
         pc.pc->mob_list_str = ANSI_NORMAL;
         pc.pc->dflt_str = ANSI_NORMAL;
         pc.pc->bk_str = ANSI_NORMAL;
         pc.pc->battle_str = ANSI_NORMAL;
         pc.pc->user1_str = ANSI_NORMAL;
         pc.pc->user2_str = ANSI_NORMAL;
         pc.pc->user3_str = ANSI_NORMAL;
         pc.show(ANSI_NORMAL);
         pc.show(ANSI_NORMAL);
         pc.PC_FLAGS.turn_on(26); //start using them
         pc.show("Setting ansi colors to defaults.\n");
      }
      else if ((strncasecmp("SUGGESTED", var, ln) == 0) ||
               (strcasecmp("ON", var) == 0)) {
         pc.pc->gos_str = ANSI_CYAN;
         pc.pc->say_str = ANSI_MAGENTA;
         pc.pc->yell_str = ANSI_RED;
         pc.pc->tell_str = ANSI_GREEN;
         pc.pc->desc_str = ANSI_YELLOW;
         pc.pc->obj_list_str = ANSI_MAGENTA;
         pc.pc->mob_list_str = ANSI_RED;
         pc.pc->dflt_str = ANSI_WHITE;
         pc.pc->bk_str = ANSI_BBLACK;
         pc.pc->battle_str = ANSI_RED;
         pc.pc->user1_str = ANSI_WHITE;
         pc.pc->user2_str = ANSI_WHITE;
         pc.pc->user3_str = ANSI_WHITE;
         pc.show(ANSI_WHITE);
         pc.show(ANSI_BBLACK);
         pc.PC_FLAGS.turn_on(26); //start using them
         pc.PC_FLAGS.turn_on(25); //save colors
         pc.show("Changed to a personal favorite of mine.\n");
      }
      else if (isValidColor(val_rough)) {

         val = colorToAnsiCode(val_rough);
         pc.PC_FLAGS.turn_on(25); //save colors

         if (strncasecmp(var, "Say", ln) == 0) {
            pc.pc->say_str = val;
         }
         else if (strncasecmp(var, "gossip", ln) == 0) {
            pc.pc->gos_str = val;
         }
         else if (strncasecmp(var, "yell", ln) == 0) {
            pc.pc->yell_str = val;
         }
         else if (strncasecmp(var, "tell", ln) == 0) {
            pc.pc->tell_str = val;
         }
         else if (strncasecmp(var, "description", ln) == 0) {
            pc.pc->desc_str = val;
         }
         else if (strncasecmp(var, "object list", ln) == 0) {
            pc.pc->obj_list_str = val;
         }
         else if (strncasecmp(var, "critter list", ln) == 0) {
            pc.pc->mob_list_str = val;
         }
         else if (strncasecmp(var, "default", ln) == 0) {
            pc.pc->dflt_str = val;
            pc.show(val); //set the background color immediately
         }
         else if (strncasecmp(var, "background", ln) == 0) {
            pc.pc->bk_str = val;
            pc.show(val); //set the background color immediately
         }
         else if (strncasecmp(var, "battle", ln) == 0) {
            pc.pc->battle_str = val;
         }
         else {
            pc.show(usage);
            return -1;
         }

         pc.show("Color set.\n");

      }//if
      else {
         pc.show(usage);
         return -1;
      }
   }//if is a pc
   else {
      pc.show("You are not a PC!!\n");
      return -1;
   }
   return 0;
}//color


int pause(int rounds, critter& pc) {
  if (pc.isMob())
    return -1; //no MOB's allowed!!

  if (!pc.mob) { //SMOB only
    return -1;
  }//if 

  if (rounds < 0)
     return 0;

  if (rounds > 300)
     rounds = 300;

  pc.PAUSE += rounds;
  return 0;
}//pause

	  
int list_scripts(int mob_num, critter& pc) {

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (!check_l_range(mob_num, 1, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[mob_num].isInUse()) {
      show("That mob does not exist.", pc);
      return -1;
   }

   String buf(500);
   buf.Append("These scripts are defined for this mob,  the actual scripts
may be seen by using the stat_script [mob_num] [script_index] command.\n\n");

   pc.show(buf);

   String tmp(100);
   int found_one = FALSE;
   Cell<MobScript*> cll(mob_list[mob_num].mob->mob_proc_scripts);
   MobScript* ptr;
   int idx = 0;
   while ((ptr = cll.next())) {
      found_one = TRUE;
      tmp = ptr->toStringBrief(FALSE, 0, ENTITY_CRITTER, idx);
      Sprintf(buf, "[%i] %S\n", idx, &(tmp));
      pc.show(buf);
      idx++;
   }

   if (!found_one) {
      buf.Append("No scripts defined for this mob.\n");
      show(buf, pc);
      return -1;
   }
   return 0;
}//list_scripts


int list_room_scripts(int rm_num, critter& pc) {

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (rm_num == 1) {
      // Use current room, if player didn't enter anything
      rm_num = ROOM.getIdNum();
   }

   if (!check_l_range(rm_num, 1, NUMBER_OF_ROOMS, pc, TRUE))
      return -1;

   if (!room_list[rm_num].isInUse()) {
      show("That Room does not exist.", pc);
      return -1;
   }

   room_list[rm_num].listScripts(pc);
   return 0;
}//list_room_scripts


int list_obj_scripts(int obj_num, critter& pc) {

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (!check_l_range(obj_num, 1, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;

   if (!obj_list[obj_num].isInUse()) {
      show("That Object does not exist.", pc);
      return -1;
   }

   obj_list[obj_num].listScripts(pc);
   return 0;
}//list_obj_scripts



int stat_script(int mob_num, int script_idx, critter& pc) {

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In stat_script:  mob_num:  " << mob_num << " script_idx:  "
             << script_idx << endl;
   }

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (!check_l_range(mob_num, 1, NUMBER_OF_MOBS, pc, TRUE))
      return -1;

   if (!mob_list[mob_num].isInUse()) {
      show("That mob does not exist.", pc);
      return -1;
   }

   String buf(500);
 
   MobScript* ptr = mob_list[mob_num].mob->mob_proc_scripts.elementAt(script_idx);
   if (ptr) {
      buf.Append(ptr->toStringBrief(pc.USING_CLIENT, mob_num,
                                    ENTITY_CRITTER, script_idx));
      if (pc.USING_CLIENT)
         buf.Append("\n<SCRIPT_DATA>");
      else
         buf.Append("\n");
      buf.Append(ptr->getScript());
      
      if (pc.USING_CLIENT)
         buf.Append("</SCRIPT_DATA>\nCompiled Script:\n");
      else
         buf.Append("\nCompiled Script:\n");
      
      buf.Append(ptr->getCompiledScript());
      
      show(buf, pc);
      return 0;
   }//if

   Sprintf(buf, "Mob: %S does not have a script of index: %i",
           mob_list[mob_num].getName(), script_idx);
   show(buf, pc);
   return -1;
}//stat_script


int stat_room_script(int rm_num, int script_idx, critter& pc) {

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In stat_room_script:  rm_num:  " << rm_num << " script_idx:  "
             << script_idx << endl;
   }

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(rm_num, 1, NUMBER_OF_ROOMS, pc, TRUE))
      return -1;

   if (!room_list[rm_num].isInUse()) {
      show("That room does not exist.", pc);
      return -1;
   }

   String buf(500);
 
   RoomScript* ptr = room_list[rm_num].getScriptAt(script_idx);
   if (ptr) {
      buf.Append(ptr->toStringBrief(pc.USING_CLIENT, rm_num, ENTITY_ROOM,
                                    script_idx));
      if (pc.isUsingClient())
         buf.Append("\n<SCRIPT_DATA>");
      else
         buf.Append("\n");
      buf.Append(ptr->getScript());
      
      if (pc.USING_CLIENT)
         buf.Append("</SCRIPT_DATA>\nCompiled Script:\n");
      else
         buf.Append("\nCompiled Script:\n");
      
      buf.Append(ptr->getCompiledScript());
      
      show(buf, pc);
      return 0;
   }//if

   Sprintf(buf, "Room: %i does not have a script of index: %i",
           rm_num, script_idx);
   show(buf, pc);
   return -1;
}//stat_room_script


int stat_obj_script(int obj_num, int script_idx, critter& pc) {

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In stat_obj_script:  obj_num:  " << obj_num << " script_idx:  "
             << script_idx << endl;
   }

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(obj_num, 1, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;

   if (!obj_list[obj_num].isInUse()) {
      show("That object does not exist.", pc);
      return -1;
   }

   String buf(500);
 
   ObjectScript* ptr = obj_list[obj_num].getScriptAt(script_idx);
   if (ptr) {
      buf.Append(ptr->toStringBrief(pc.USING_CLIENT, obj_num, ENTITY_OBJECT,
                                    script_idx));
      if (pc.isUsingClient())
         buf.Append("\n<SCRIPT_DATA>");
      else
         buf.Append("\n");
      buf.Append(ptr->getScript());
      
      if (pc.USING_CLIENT)
         buf.Append("</SCRIPT_DATA>\nCompiled Script:\n");
      else
         buf.Append("\nCompiled Script:\n");
      
      buf.Append(ptr->getCompiledScript());
      
      show(buf, pc);
      return 0;
   }//if

   Sprintf(buf, "Object: %i does not have a script of index: %i",
           obj_num, script_idx);
   show(buf, pc);
   return -1;
}//stat_obj_script



int rem_script(int mob_num, String& trigger, int i_th, critter& pc) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In rem_script:  mob_num:  " << mob_num << "  trigger:  "
             << trigger << "  i_th:  " << i_th << endl;
   }

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (trigger.Strlen() == 0) {
      show("Which trigger do you want to stat??\n", pc);
      return -1;
   }

   if (!check_l_range(mob_num, 1, NUMBER_OF_MOBS, pc, TRUE))
       return -1;

   if (!mob_list[mob_num].isInUse()) {
      show("That mob does not exist.\n", pc);
      return -1;
   }

   if (!pc.doesOwnCritter(mob_list[mob_num])) {
      show("You don't own that mob...\n", pc);
      return -1;
   }

   int sofar = 1;
   String buf(500);
 
   Cell<MobScript*> cll(mob_list[mob_num].mob->mob_proc_scripts);
   MobScript* ptr;
   ptr = cll.next();
   while (ptr) {
      if (strcasecmp(*(ptr->getTrigger()), trigger) == 0) {
         if (sofar == i_th) {
            delete ptr;
            ptr = mob_list[mob_num].mob->mob_proc_scripts.lose(cll);
            show("Deleted it...\n", pc);
            return 0;
         }//if
         else {
            ptr = cll.next();
         }
         sofar++;
      }//if
      else {
         ptr = cll.next();
      }
   }//while

   show("Didn't find that script..\n", pc);
   return -1;
}//rem_script


int rem_room_script(int rm_num, String& trigger, int i_th, critter& pc) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In rem_room_script:  rm_num:  " << rm_num << "  trigger:  "
             << trigger << "  i_th:  " << i_th << endl;
   }

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (trigger.Strlen() == 0) {
      show("Which trigger do you want to remove??\n", pc);
      return -1;
   }

   if (rm_num == 1) {
      rm_num = ROOM.getIdNum();
   }
   
   if (!check_l_range(rm_num, 1, NUMBER_OF_ROOMS, pc, TRUE))
      return -1;

   if (!room_list[rm_num].isInUse()) {
      show("That room does not exist.\n", pc);
      return -1;
   }

   if (!pc.doesOwnRoom(room_list[rm_num])) {
      show("You don't own that room...\n", pc);
      return -1;
   }

   room_list[rm_num].removeScript(trigger, i_th, pc);
   return 0;
}//rem_room_script


int rem_obj_script(int obj_num, String& trigger, int i_th, critter& pc) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In rem_obj_script:  obj_num:  " << obj_num << "  trigger:  "
             << trigger << "  i_th:  " << i_th << endl;
   }

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (trigger.Strlen() == 0) {
      show("Which trigger do you want to remove??\n", pc);
      return -1;
   }

   if (!check_l_range(obj_num, 1, NUMBER_OF_ITEMS, pc, TRUE))
      return -1;

   if (!obj_list[obj_num].isInUse()) {
      show("That object does not exist.\n", pc);
      return -1;
   }

   if (!pc.doesOwnObject(obj_list[obj_num])) {
      show("You don't own that object...\n", pc);
      return -1;
   }

   obj_list[obj_num].removeScript(trigger, i_th, pc);
   return 0;
}//rem_obj_script


int teach(int i_th, const String* name, int prcnt, const String* skill, 
	   critter& pc) {
   int skill_num;

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.IMM_LEVEL < 5) {
      show("Eh?\n", pc);
      return -1;
   }//if

   prcnt = bound(1, 100, prcnt);

   critter* ptr = have_crit_named(pc_list, i_th, name,
                                  pc.SEE_BIT, ROOM);
   if (!ptr) {
      show("That person is not logged on.\n", pc);
      return -1;
   }//if

   skill_num = SSCollection::instance().getNumForName(*skill);

   if (skill_num < 0) {
      show("That skill is unknown.\n", pc);
      return -1;
   }//if

   if (get_percent_lrnd(skill_num, pc) < 0) { //then add it
      ptr->SKILLS_KNOWN.Insert(skill_num, prcnt);
      update_skill(skill_num, pc);
      show("Ok, your teachings were heard!.\n", pc);
      show("You learn of a new skill.\n", *ptr);
   }//if
   else {
      show("They evidently already know that skill.\n", pc);
      ptr->SKILLS_KNOWN.Insert(skill_num, prcnt); //make sure its at 100
      update_skill(skill_num, pc);
   }//else
   return 0;
}//teach


int add_perm_inv(int i_th, const String* name, int obj_num, 
                 critter& pc) {
   String buf(100);
   critter* ptr;

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(obj_num, 2, NUMBER_OF_ITEMS, pc, TRUE)) 
      return -1;

   if (!obj_list[obj_num].OBJ_FLAGS.get(10)) {
      show("That object doesn't exist.\n", pc);
      return -1;
   }//if

   if (*name == "DB") {
      if (check_l_range(i_th, 2, NUMBER_OF_MOBS, pc, TRUE)) {
         ptr = &(mob_list[i_th]);
      }
      else {
         return -1;
      }
   }//if
   else {
      ptr = ROOM.haveCritNamed(i_th, name, pc.SEE_BIT);
      pc.show("You may want to use the [VNUM].DB method.  See help.\n");
   }

   if (ptr) {
      if (ptr->mob && ptr->mob->proc_data && 
          ptr->mob->proc_data->sh_data) {
         /* is a shopkeeper */
         ptr->PERM_INV.append(&(obj_list[obj_num]));
         Sprintf(buf, "%S added to %S's permanent (shop) inventory.\n",
                 long_name_of_obj(obj_list[obj_num], ~0),
                 name_of_crit(*ptr, ~0));
         show(buf, pc);
         return 0;
      }//if
      else {
         ptr->gainInv(&(obj_list[obj_num]));
         Sprintf(buf, "%S added to %S's regular inventory.\n",
                 long_name_of_obj(obj_list[obj_num], ~0),
                 name_of_crit(*ptr, ~0));
         show(buf, pc);
         return 0;
      }//else
   }//if a mob ptr found
   else {
      object* optr = ROOM.haveObjNamed(i_th, name, pc.SEE_BIT);
      if (!optr) {
         optr =  have_obj_named(pc.inv, i_th, name, 
                                pc.SEE_BIT, ROOM);
      }//if
      if (optr) {
         if (optr->bag) {
            optr->gainInv(&(obj_list[obj_num])); 
            Sprintf(buf, "%S added to %S's inventory.\n",
                    long_name_of_obj(obj_list[obj_num], ~0),
                    long_name_of_obj(*optr, ~0));
            show(buf, pc);
            return 0;
         }//if
         else {
            show("It isn't a bag!\n", pc);
            return -1;
         }//else
      }//if
      show("You don't see that object or critter here.\n", pc);
      return -1;
   }//else maybe its an object?
   return -1;
}//add_perm_inv



int rem_perm_inv(int j_th, const String* obj_name, int i_th,
                 const String* targ, critter& pc) {
   String buf(100);
   critter* ptr;

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (*targ == "DB") {
      if (check_l_range(i_th, 2, NUMBER_OF_MOBS, pc, TRUE)) {
         ptr = &(mob_list[i_th]);
      }
      else {
         return -1;
      }
   }//if
   else {
      ptr = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);
      pc.show("You may want to use the [VNUM].DB method.  See help.\n");
   }

   object* inv_obj = NULL;

   if (ptr) {
      if (ptr->mob && ptr->mob->proc_data && 
          ptr->mob->proc_data->sh_data) {
         /* is a shopkeeper */
         if ((inv_obj = have_obj_named(ptr->PERM_INV, j_th, obj_name, 
                                       pc.SEE_BIT, ROOM))) {
            ptr->PERM_INV.loseData(inv_obj);
            Sprintf(buf, "%S deleted from %S's permanent (shop) inventory.\n",
                    long_name_of_obj(*inv_obj, ~0),
                    name_of_crit(*ptr, ~0));
            show(buf, pc);
            if (inv_obj->IN_LIST)
               delete inv_obj;
            return 0;
         }//if
         else {
            show("That object is not in its permanent inventory.\n", pc);
            return -1;
         }//else
      }//if
      else {
         if ((inv_obj = have_obj_named(ptr->inv, j_th, obj_name, 
                                       pc.SEE_BIT, ROOM))) {
            ptr->loseInv(inv_obj);
            Sprintf(buf, "%S deleted from %S's inventory.\n",
                    long_name_of_obj(*inv_obj, ~0),
                    name_of_crit(*ptr, ~0));
            show(buf, pc);
            if (inv_obj->IN_LIST)
               delete inv_obj;
            return 0;
         }//if
         else {
            show("That object is not in its inventory.\n", pc);
            return -1;
         }//else
      }//else
   }//if a mob ptr found
   else {
      object* optr = ROOM.haveObjNamed(i_th, targ, pc.SEE_BIT);
      if (!optr) {
         optr =  have_obj_named(pc.inv, i_th, targ, 
                                pc.SEE_BIT, ROOM);
      }//if
      if (optr) {
         if ((inv_obj = have_obj_named(optr->inv, j_th, obj_name, 
                                       pc.SEE_BIT, ROOM))) {
            optr->gainInv(inv_obj);
            Sprintf(buf, "%S deleted from %S's inventory.\n",
                    long_name_of_obj(*inv_obj, ~0),
                    name_of_obj(*optr, ~0));
            show(buf, pc);
            if (inv_obj->IN_LIST)
               delete inv_obj;
            return 0;
         }//if
         else {
            show("That object is not in its permanent inventory.\n", pc);
            return -1;
         }//else
      }//if
   }//else maybe its an object?
   show("You don't see that object or critter here.\n", pc);
   return -1;
}//rem_perm_inv

   
int beep(int i_th, const String* name, critter& pc) {
   char buf[20];
   String buf2(100);

   if (!ok_to_do_action(NULL, "mFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* ptr = have_crit_named(pc_list, i_th, name,
                                  pc.SEE_BIT, ROOM);
   if (!ptr) {
      show("That person is not logged on.\n", pc);
      return -1;
   }//if

   buf[0] = 7; //beep
   buf[1] = 7;
   buf[2] = 7;
   buf[3] = 0; //terminate it

   show(buf, *ptr);
   Sprintf(buf2, "%S beeps you.\n", name_of_crit(pc, ptr->SEE_BIT));
   show(buf2, *ptr);

   show("Done...\n", pc);
   return 0;
}//beep


int opurge(int i_th, const String* name, critter& pc) {
   object* obj;

   if (!ok_to_do_action(NULL, "IFPR", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   obj = ROOM.haveObjNamed(i_th, name, pc.SEE_BIT);
   if (!obj) {
      show("You don't see that here.\n", pc);
   }//if
   else {
      ROOM.loseInv(obj);

      if (!obj->isBulletinBoard()) {
         recursive_init_unload(*obj, 0);
      }
      else {
         obj->decrementCurInGame();
      }

      show("Ok, object purged from room.\n", pc);
      if (obj->IN_LIST) {
	delete obj;
      }//if
      return 0;
   }//else
   return -1;
}//opurge


int do_transport(critter& targ, critter& pc, room& dest) {
  /* assume targ AND pc are SMOB's or PC's */

   String buf(100);
   
   show("The world tilts and angles bend!\n", targ);
   emote("shrinks impossibly and disappears.", targ, 
         room_list[targ.getCurRoomNum()], TRUE); 
   Sprintf(buf, "You transport %S.\n", name_of_crit(targ, pc.SEE_BIT));
   show(buf, pc);
   
   int is_dead;
   targ.doGoToRoom(dest.getRoomNum(), NULL, NULL, is_dead, targ.getCurRoomNum());
   
   if (!is_dead) {
      look(1, &NULL_STRING, targ);
      emote("blinks into existence.", targ, dest, TRUE);
   }
   return 0;
}//do_transport
 

/* Toggle client usage to TRUE */
int using_client(critter& pc) {
   if (pc.pc) {
      pc.PC_FLAGS.turn_on(5);
      return 0;
   }
   return 0;
}


int itrans(int i_th, const String* targ, int rm_num, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   critter* ptr = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);
   
   if (!ptr) {
      ptr = have_crit_named(pc_list, i_th, targ, pc.SEE_BIT, ROOM);
   }//if
   
   if (!ptr) {
      show("Who would you like to transport?\n", pc);
      return -1;
   }//if
   
   if (ptr->isMob()) {
      ptr = mob_to_smob(*ptr, pc.getCurRoomNum(), TRUE, i_th, targ, pc.SEE_BIT);
   }//if
   else if (ptr->pc && ptr->pc->imm_data && 
            (ptr->IMM_LEVEL >= pc.IMM_LEVEL)) {
      show("Whoa, you can't itrans one so powerful.\n", pc);
      return -1;
   }//if
   
   if (rm_num == 1) {
      rm_num = pc.getCurRoomNum(); //default is to trans him TO the player's own room
   }//if
   
   if (check_l_range(rm_num, 0, NUMBER_OF_ROOMS, pc, TRUE)) {
      return do_transport(*ptr, pc, room_list[rm_num]);
   }//
   return -1;
}//itrans



int discuss(const String* topic, int i_th, const String* targ, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* ptr = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);

   if (!ptr) {
      show("With whom would you like to discuss?\n", pc);
   }//if
   else if (topic->Strlen() == 0) {
      show("What would you like to discuss?\n", pc);
   }//if
   else if (pc.POS > POS_REST) {
      show("You are too relaxed.\n", pc);
   }//if
   else if (!IsEmpty(pc.IS_FIGHTING)) {
      show("You find it hard to have a decent conversation while fighting!\n",
           pc);
   }//if
   else { //looks like we're good to go
      if (ptr->mob && !ptr->pc) {
         if (ptr->mob->proc_data) {
            if (!IsEmpty(ptr->mob->proc_data->topics)) { //then do it
               if (ptr->isMob()) {
                  ptr = mob_to_smob(*ptr, pc.getCurRoomNum(), TRUE, i_th, targ, pc.SEE_BIT);
               }//if
               return do_domob_say_proc(*ptr, pc, *topic);
            }//if
         }//if
      }//if
      if (ptr->pc) {
         Sprintf(buf, "%S wishes to discuss %S with you.\n", 
                 name_of_crit(pc, ptr->SEE_BIT), topic);
         show(buf, *ptr);
      }//if
   }//else
   return -1;
}//discuss


int suicide(const String* passwd,  critter& pc) {
  String buf(100);

   if (!ok_to_do_action(NULL, "mrBFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!pc.isPc()) {
      return -1;
   }

   if (pc.pc->password != crypt(*passwd, "bg")) {
      show("In order to FOREVER kill your character, you must enter your \n",
           pc);
      show("password too.\n", pc);
      return -1;
   }//if

   pc.doSuicide();
   return 0;
}//suicide


/* this is a 'give' for money basically */
int pay(int amount, int i_th, const String* targ,  critter& pc) {
   String buf(100);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In Pay, amount: " << amount << " i_th: " << i_th
             << " targ:  " << targ << endl;
   }

   if (!ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   critter* ptr = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);
   
   if (!ptr) {
      show("Pay who??\n", pc);
      return -1;
   }//if

   if (amount <= 0) {
      pc.show("The amount needs to be greater than zero!\n");
      return -1;
   }
   
   if (amount > pc.GOLD) {
      show("You don't have that much!\n", pc);
      return -1;
   }//if
   
   if (ptr->isMob()) {
      ptr = mob_to_smob(*ptr, pc.getCurRoomNum(), TRUE, i_th,
                        targ, pc.SEE_BIT);
   }//if

   ptr->GOLD += amount;
   pc.GOLD -= amount;
   
   Sprintf(buf, "You are pleasantly suprised as %S pays you %i.\n",
           name_of_crit(pc, ptr->SEE_BIT), amount);
   show(buf, *ptr);
   
   Sprintf(buf, "You give %S %i coins.\n", name_of_crit(*ptr, pc.SEE_BIT),
           amount);
   show(buf, pc);

   String cmd = "pay";
   String amt;
   amt.Append(amount);

   ROOM.checkForProc(cmd, amt, pc, ptr->MOB_NUM);
   return 0;
}//pay


int mvnum(int i_th, const String* name, critter& pc) {
   int i;
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (i_th < 0)
      i_th = 0;

   for (i = i_th; i<NUMBER_OF_MOBS; i++) {
      if (mob_is_named(mob_list[i], *name)) {
	 Sprintf(buf, "[%i] %S\n", i, &(mob_list[i].short_desc));
	 show(buf, pc);
      }//if
   }//for
   show("ok\n", pc);
   return 0;
}//mvnum


int ovnum(int i_th, const String* name, critter& pc) {
   int i;
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (i_th < 0)
      i_th = 0;

   for (i = i_th; i<NUMBER_OF_ITEMS; i++) {
      if (obj_is_named(obj_list[i], *name)) {
	 Sprintf(buf, "[%i] %S\n", i, &(obj_list[i].short_desc));
	 show(buf, pc);
      }//if
   }//for
   show("ok\n", pc);
   return 0;
}//ovnum


int dvnum(int i_th, const String* name, critter& pc) {
   int i;
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (i_th < 0)
      i_th = 0;

   for (i = i_th; i<NUMBER_OF_DOORS; i++) {
      if (door_is_named(door_list[i], *name)) {
	 Sprintf(buf, "[%i] %S\n", i, &(door_list[i].long_desc));
	 show(buf, pc);
      }//if
   }//for
   show("ok\n", pc);
   return 0;
}//dvnum


int arlist(int zone, int how_many, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if ((zone == 1) && (how_many == 1)) {
      zone = ROOM.getZoneNum();
      how_many = 100; //all
   }//if
   else if ((zone > 1) && (how_many == 1)) {
      how_many = 100;
   }


   if (!check_l_range(zone, 0, NUMBER_OF_ZONES, pc, FALSE)) {
      show("Zone is out of range.\n", pc);
      return -1;
   }//if
   
   Sprintf(buf, "Here are all rooms defined for zone# %i.\n", zone);
   show(buf, pc);


   int count = 0;
   for (int i = ZoneCollection::instance().elementAt(zone).getBeginRoomNum();
        ((i <= ZoneCollection::instance().elementAt(zone).getEndRoomNum())
         && (count < how_many));
        i++) {
      if (room_list[i].isInUse()) {
	 Sprintf(buf, "[%i]\t%S\n", i, &(room_list[i].short_desc));
	 count++;
	 show(buf, pc);
      }//if
   }//for
   return 0;
}//arlist


int slist(int begin, int how_many, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (how_many == 1)
      how_many = 20;

   if ((begin == 1) && (how_many == 1)) {
      begin = 0;
   }//if

   if ((begin >= NUMBER_OF_SKILL_SPELLS) || (begin < 0))
      begin = 0;

   int end = begin + how_many;

   if ((end >= NUMBER_OF_SKILL_SPELLS) || (end < 0))
      end = NUMBER_OF_SKILL_SPELLS -1;
   show("Here is the spell list you requested:\n", pc);

   for (int i = begin; i < end; i++) {
      Sprintf(buf, "\t%i:  %S\n", i, 
              &(SSCollection::instance().getSS(i).getName()));
      show(buf, pc);
   }//for
   return 0;
}//slist


int aolist(int zone, int how_many, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (zone == 1) { // default to current zone
     zone = ROOM.getZoneNum();
   }//if

   if (how_many == 1) { //default to 100 (probably all)
      how_many = 100; //all
   }//if

   if (!check_l_range(zone, 0, NUMBER_OF_ZONES, pc, FALSE)) {
      show("Zone is out of range.\n", pc);
      return -1;
   }//if
   
   Sprintf(buf, "Here are all objects defined for zone# %i.\n", zone);
   show(buf, pc);


   int count = 0;
   for (int i = 0; (i < NUMBER_OF_ITEMS && count < how_many); i++) {
      if (obj_list[i].OBJ_FLAGS.get(10)) {
	 if (obj_list[i].OBJ_IN_ZONE == zone) {
	    Sprintf(buf, "[%i]\t%S\n", i, &(obj_list[i].short_desc));
	    count++;
	    show(buf, pc);
	 }//if
      }//if
   }//for
   return 0;
}//aolist


int aosave(int zone, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (zone == 1) {
      zone = ROOM.getZoneNum();
   }//if

   if (!check_l_range(zone, 0, NUMBER_OF_ZONES, pc, FALSE)) {
      show("Zone is out of range.\n", pc);
      return -1;
   }//if
 
   if (!ZoneCollection::instance().elementAt(zone).isOwnedBy(pc)) {
      show("You do not own that zone.  Only owners can aosave a zone.\n", pc);
      return -1;
   }//if

   if (!ZoneCollection::instance().elementAt(zone).canWriteObjects()) {
     show("All builders of your zone must finish their objects before\n", pc);
     show("you can write them to disk.\n", pc);
     return -1;
   }//if

   Sprintf(buf, "Re-writing the objects for zone# %i.\n", zone);
   show(buf, pc);

   return do_aosave(zone);
}//aosave


int do_aosave(int zone) {
   String buf(100);

   Sprintf(buf, "./World/objects_%i", zone);
   ofstream ofile(buf);
   if (!ofile) {
      mudlog.log(ERR, "ERROR:  ofile is NULL, aosave.\n");
      return -1;
   }//if

   for (int i = 0; i < NUMBER_OF_ITEMS; i++) {
      if (obj_list[i].OBJ_FLAGS.get(10)) { //in use
         if (obj_list[i].OBJ_IN_ZONE == zone) {
            ofile << i << "\t Start of Object\n";
            obj_list[i].Write(ofile);
            ofile << endl;
         }//if
      }//if
   }//for
   ofile << endl << "-1 END OF FILE" << endl;
   return 0;
}//do_aosave


//Writes every zone, if players are not currently editing the zones.
int write_world(critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (pc.getImmLevel() != 10) {
      pc.show("You must be a level 10 immort to write the entire world!\n");
      return -1;
   }

   show("Writing the entire WORLD...\n", pc);
   
   ZoneCollection::instance().writeWorld(pc);
   return 0;
}//write_world


int amsave(int zone, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (zone == 1) {
      zone = ROOM.getZoneNum();
   }//if

   if (!check_l_range(zone, 0, NUMBER_OF_ZONES, pc, FALSE)) {
      show("Zone is out of range.\n", pc);
      return -1;
   }//if
   
   if (!ZoneCollection::instance().elementAt(zone).isOwnedBy(pc)) {
      show("You do not own that zone.  Only owners can amsave a zone.\n", pc);
      return -1;
   }//if

   if (!ZoneCollection::instance().elementAt(zone).canWriteMobs()) { //if locked (no write)
     show("All builders of your zone must finish their mobiles before\n", pc);
     show("you can write it to disk.\n", pc);
     return -1;
   }//if

   Sprintf(buf, "Re-writing the mobs for zone# %i.\n", zone);
   show(buf, pc);
   
   return do_amsave(zone);
}//amsave


int do_amsave(int zone) {
   String buf(100);
   
   Sprintf(buf, "./World/critters_%i", zone);
   ofstream ofile(buf);
   if (!ofile) {
      mudlog.log(ERR, "ERROR:  ofile is NULL, amsave.\n");
      return -1;
   }//if
   
   for (int i = 0; i < NUMBER_OF_MOBS; i++) {
      if (mob_list[i].mob) {
         if (mob_list[i].isInUse()) {
            if (mob_list[i].getNativeZoneNum() == zone) {
               ofile << i << "\tStart of Mob\n";
               mob_list[i].Write(ofile);
               ofile << endl;
            }//if
         }//if
      }//if
   }//for
   ofile << endl << "-1 END OF MOB FILE" << endl;
   return 0;
}//do_amsave


int adsave(int zone, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (zone == 1) {
      zone = ROOM.getZoneNum();
   }//if

   if (!check_l_range(zone, 0, NUMBER_OF_ZONES, pc, FALSE)) {
      show("Zone is out of range.\n", pc);
      return -1;
   }//if
   
   if (!ZoneCollection::instance().elementAt(zone).isOwnedBy(pc)) {
      show("You do not own that zone.  Only owners can adsave a zone.\n", pc);
      return -1;
   }//if

   if (!ZoneCollection::instance().elementAt(zone).canWriteDoors()) { //if locked (no write)
      show("All builders of your zone must finish their doors before\n", pc);
      show("you can write them to disk.\n", pc);
      return -1;
   }//if

   Sprintf(buf, "Re-writing the doors for zone# %i.\n", zone);
   show(buf, pc);

   return do_adsave(zone);
}//adsave


int do_adsave(int zone) {
   String buf(100);

   Sprintf(buf, "./World/doors_%i", zone);
   ofstream ofile(buf);
   if (!ofile) {
      mudlog.log(ERR, "ERROR:  ofile is NULL, adsave.\n");
      return -1;
   }//if

   for (int i = 0; i < NUMBER_OF_DOORS; i++) {
      if (door_list[i].door_data_flags.get(10)) { //if in use
         if (door_list[i].in_zone == zone) {
            ofile << i << "\tStart of Door Data\n";
            door_list[i].Write(ofile);
            ofile << endl;
         }//if
      }//if
   }//for
   ofile << endl << "-1 END OF DOOR FILE" << endl;
   return 0;
}//do_adsave


int amlist(int zone, int how_many, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (zone == 1) {
      zone = ROOM.getZoneNum();
   }//if

   if (how_many == 1) {   
      how_many = 100; //all
   }//if
   
   if (!check_l_range(zone, 0, NUMBER_OF_ZONES, pc, FALSE)) {
      show("Zone is out of range.\n", pc);
      return -1;
   }//if
   
   Sprintf(buf, "Here are all mobs defined for zone# %i.\n", zone);
   show(buf, pc);


   int count = 0;
   for (int i = 0; (i<NUMBER_OF_MOBS && count < how_many); i++) {
      if (mob_list[i].getNativeZoneNum() == zone) {
         if (mob_list[i].isInUse()) {
            Sprintf(buf, "[%i]\t%S\n", i, &(mob_list[i].short_desc));
            count++;
            show(buf, pc);
         }//if
      }//if
   }//for
   return 0;
}//amlist


/* only imms of level 9 and above can do this */
int make_builder(int i_th, const String* name, critter& pc) {

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.getImmLevel() < 9) {
      return -1;
   }//if

   critter* ptr = ROOM.haveCritNamed(i_th, name, pc.SEE_BIT);
   if (!ptr) {
      show("You don't see that person here.\n", pc);
      return -1;
   }//if

   if (!ptr->pc) {
      show("Only pc's can become builders!!\n", pc);
      return -1;
   }//if

   if (ptr->pc->imm_data) {
      ptr->PC_FLAGS.turn_on(11);  // is builder
   }//if
   else {
      ptr->PC_FLAGS.turn_on(11);  // is builder
      ptr->PC_FLAGS.turn_on(2);   // has imm data
      ptr->pc->imm_data = new immort_data;
      /* IMM_LEVEL defaults to zero */
   }//else
   show("You are now a builder.  Remember:  HELP is you friend, and\n",
        *ptr);
   show("be sure to see help on the 'theme' of the MUD.\n", *ptr);
   show("Done.\n", pc);
   return 0;
}//make_builder



int withdraw(int i_th, const String* coins, int j_th,
             const String* banker, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "SBFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (coins->Strlen() && (strncasecmp(*coins, "coins", coins->Strlen()) != 0) &&
       (strncasecmp(*coins, "gold", coins->Strlen()) != 0)) {
      pc.show("You must withdraw 'gold' or 'coins'.\n");
      return -1;
   }

   if (i_th < 0) {
      pc.show("Nice try, maybe you want to deposit??\n");
      return -1;
   }

   critter* crit = ROOM.haveCritNamed(j_th, banker, pc.SEE_BIT);

   if (!crit) {
      crit = ROOM.getLastCritter(); //most likely to be a banker, if one exists
   }//if

   if (crit == &pc) {
      pc.show("Looks like the banker is not around right now!\n");
      return -1;
   }

   if (!crit->isBanker()) {
      Sprintf(buf, "%S is not a banker.\n", crit->getName());
      buf.Cap();
      pc.show(buf);
      return -1;
   }//if

   return pc.withdrawCoins(i_th, *crit);
}//withdraw

int balance(int i_th, const String* banker, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "SBFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* crit = ROOM.haveCritNamed(i_th, banker, pc.SEE_BIT);

   if (!crit) {
      crit = ROOM.getLastCritter(); //most likely to be a banker, if one exists
   }//if

   if (crit == &pc) {
      pc.show("Looks like the banker is not around right now!\n");
      return -1;
   }

   if (!crit->isBanker()) {
      Sprintf(buf, "%S is not a banker.\n", crit->getName());
      buf.Cap();
      pc.show(buf);
      return -1;
   }//if

   return pc.balanceCoins(*crit);
}//balance

int deposit(int i_th, const String* coins, int j_th,
            const String* banker, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "SBFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (coins->Strlen() && (strncasecmp(*coins, "coins", coins->Strlen()) != 0) &&
       (strncasecmp(*coins, "gold", coins->Strlen()) != 0)) {
      pc.show("You must deposit 'gold' or 'coins'.\n");
      return -1;
   }

   if (i_th < 0) {
      pc.show("Nice try, maybe you want to withdraw??\n");
      return -1;
   }

   critter* crit = ROOM.haveCritNamed(j_th, banker, pc.SEE_BIT);

   if (!crit) {
      crit = ROOM.getLastCritter(); //most likely to be a banker, if one exists
   }//if

   if (crit == &pc) {
      pc.show("Looks like the banker is not around right now!\n");
      return -1;
   }

   if (!crit->isBanker()) {
      Sprintf(buf, "%S is not a banker.\n", crit->getName());
      buf.Cap();
      pc.show(buf);
      return -1;
   }//if

   return pc.depositCoins(i_th, *crit);
}//deposit


int gag(int i_th, const String* targ, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* crit = have_crit_named(pc_list, i_th, targ,
                                   pc.SEE_BIT, ROOM);
   if (!crit) {
      crit = have_crit_named(linkdead_list, i_th, targ,
                             pc.SEE_BIT, ROOM);
   }//if

   if (!crit) {
      show("That person isn't logged on!\n", pc);
      return -1;
   }//if

   if (crit->pc->imm_data && (crit->IMM_LEVEL >= pc.IMM_LEVEL)) {
      show("Nice try, but you could NEVER gag one so powerful!!\n", pc);
      return -1;
   }//if

   Sprintf(buf, "%S has gagged you!!\n", name_of_crit(pc, ~0));
   show(buf, *crit);
   emote("goes a-gagging!!", pc, ROOM, TRUE);
   emote("will speak no more!", *crit, room_list[crit->getCurRoomNum()], TRUE);
   crit->PC_FLAGS.turn_on(1);
   return 0;
}//gag


int freeze(int i_th, const String* targ, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* crit = have_crit_named(pc_list, i_th, targ,
                                   pc.SEE_BIT, ROOM);
   if (!crit) {
      crit = have_crit_named(linkdead_list, i_th,
                             targ, pc.SEE_BIT, ROOM);
   }//if

   if (!crit) {
      show("That person isn't logged on!\n", pc);
      return -1;
   }//if

   if (crit->pc->imm_data && (crit->IMM_LEVEL >= pc.IMM_LEVEL)) {
      show("Nice try, but you could NEVER freeze one so powerful!!\n", pc);
      return -1;
   }//if

   Sprintf(buf, "%S has frozen you!!\n", name_of_crit(pc, ~0));
   show(buf, *crit);
   emote("gets out the freezer!!", pc, ROOM, TRUE);
   emote("will do no more!", *crit, room_list[crit->getCurRoomNum()], TRUE);
   crit->PC_FLAGS.turn_on(0);
   return 0;
}//freeze


int ungag(int i_th, const String* targ, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* crit = have_crit_named(pc_list, i_th, targ,
                                   pc.SEE_BIT, ROOM);
   if (!crit) {
      crit = have_crit_named(linkdead_list, i_th, targ,
                             pc.SEE_BIT, ROOM);
   }//if

   if (!crit) {
      show("That person isn't logged on!\n", pc);
      return -1;
   }//if

   if (crit->pc->imm_data && (crit->IMM_LEVEL >= pc.IMM_LEVEL)) {
      show("Nice try, but that gag was put on by one too powerful!!\n", pc);
      return -1;
   }//if

   Sprintf(buf, "%S has ungagged you!!\n", name_of_crit(pc, ~0));
   show(buf, *crit);
   emote("removes a gag!!", pc, ROOM, TRUE);
   emote("can speak once more!", *crit, room_list[crit->getCurRoomNum()], TRUE);
   crit->PC_FLAGS.turn_off(1);
   return 0;
}//ungag


int thaw(int i_th, const String* targ, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* crit = have_crit_named(pc_list, i_th, targ,
                                   pc.SEE_BIT, ROOM);
   if (!crit) {
      crit = have_crit_named(linkdead_list, i_th, targ,
                             pc.SEE_BIT, ROOM);
   }//if

   if (!crit) {
      show("That person isn't logged on!\n", pc);
      return -1;
   }//if

   if (crit->pc->imm_data && (crit->IMM_LEVEL >= pc.IMM_LEVEL)) {
      show("Nice try, but that freeze was put on by one too powerful!!\n", pc);
      return -1;
   }//if

   Sprintf(buf, "%S has thawed you out!!\n", name_of_crit(pc, ~0));
   show(buf, *crit);
   emote("thaws 'em out!!", pc, ROOM, TRUE);
   emote("is warm once more!", *crit, room_list[crit->getCurRoomNum()], TRUE);
   crit->PC_FLAGS.turn_off(0);
   return 0;
}//thaw


int value_set(int i_th, const String* targ, int val_idx,
              int sell_val, int buy_val, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mFPS", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* crit = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);

   if (!crit) {
      pc.show("Set value for which shopkeeper??\n");
      return -1;
   }//if

   if (crit->isPlayerShopKeeper()) {
      if (crit->isManagedBy(pc)) {
         crit->valueSet(val_idx, sell_val, buy_val, pc); //will do msgs
         save_player_shop_owner(*crit);
         return 0;
      }//if
      else {
         pc.show("You are not the manager of this shopkeeper.\n");
      }
   }//if
   else {
      pc.show("That is not a player-run shopkeeper.\n");
   }//else
   return -1;
}//value_set


int value_list(int i_th, const String* targ, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mSFBP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* crit = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);

   if (!crit) {
      pc.show("List values for which shopkeeper??\n");
      return -1;
   }//if

   if (crit->isPlayerShopKeeper()) {
      crit->valueList(i_th, targ, pc);
      return 0;
   }//if
   else {
      pc.show("That is not a player-run shopkeeper.\n");
   }//else
   return -1;
}//value_list


int value_add(int i_th, const String* targ, int j_th,
               const String* obj, critter& pc) {
   String buf(100);
   object* obj_ptr;

   if (!ok_to_do_action(NULL, "mSBFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* crit = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);

   if (!crit) {
      pc.show("Add value for which shopkeeper??\n");
      return -1;
   }//if

   obj_ptr = have_obj_named(pc.inv, j_th, obj, pc.SEE_BIT, ROOM);

   if (!obj_ptr) {
      pc.show("You don't have that object.\n");
      return -1;
   }//if

   if (crit->isPlayerShopKeeper()) {
      if (crit->isManagedBy(pc)) {
         crit->valueAdd(*obj_ptr, pc); //will do msgs
         save_player_shop_owner(*crit);
         return 0;
      }//if
      else {
         pc.show("You are not the manager of this shopkeeper.\n");
      }
   }//if
   else {
      pc.show("That is not a player-run shopkeeper.\n");
   }//else
   return -1;
}//value_add


int adjust_register(int i_th, const String* targ, int new_balance,
                     critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mBSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (new_balance < 0) {
      pc.show("That balance is a little too low!\n");
      return -1;
   }//if

   critter* crit = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);

   if (!crit) {
      pc.show("Balance the register for which shopkeeper??\n");
      return -1;
   }//if

   if (crit->isPlayerShopKeeper()) {
      if (crit->isManagedBy(pc)) {
         
         if (new_balance == 1) {
            // just checking balance
            Sprintf(buf, "The shopkeeper holds %i coins.\n",
                    crit->GOLD);
            pc.show(buf);
            return 0;
         }//if
         else {
            if (crit->GOLD > new_balance) {
               //gonna give some to the manager (pc)
               pc.GOLD += (crit->GOLD - new_balance);
               
               Sprintf(buf, "You take %i coins from the shopkeeper.",
                       (crit->GOLD - new_balance));
               // and take some from the shopkeeper
               crit->GOLD = new_balance;
               pc.show(buf);
               save_player_shop_owner(*crit);
               return 0;
            }//if
            else {
               // gonna give some to the shopkeeper
               if ((new_balance - crit->GOLD) > pc.GOLD) {
                  pc.show("Nice try, but you don't have that much gold to give!\n");
                  return -1;
               }
               else {
                  pc.GOLD -= (new_balance - crit->GOLD);
                  Sprintf(buf, "Transferred %i coins to the shopkeeper.",
                          (new_balance - crit->GOLD));
                  crit->GOLD = new_balance;
                  pc.show(buf);
                  save_player_shop_owner(*crit);
                  return 0;
               }//else
            }//else
         }//else
      }//if
      else {
         pc.show("You are not the manager of this shopkeeper.\n");
      }
   }//if
   else {
      pc.show("That is not a player-run shopkeeper.\n");
   }//else
   return -1;
}//adjust_register


int value_rem(int i_th, const String* targ, int val_idx, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mSBFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* crit = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);

   if (!crit) {
      pc.show("Add value for which shopkeeper??\n");
      return -1;
   }//if

   if (crit->isPlayerShopKeeper()) {
      if (crit->isManagedBy(pc)) {
         crit->valueRem(val_idx, pc); //will do msgs
         save_player_shop_owner(*crit);
         return 0;
      }//if
      else {
         pc.show("You are not the manager of this shopkeeper.\n");
      }
   }//if
   else {
      pc.show("That is not a player-run shopkeeper.\n");
   }//else
   return -1;
}//value_rem


int consider(int i_th, const String* targ, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   critter* crit = ROOM.haveCritNamed(i_th, targ, pc.SEE_BIT);

   if (!crit) {
      pc.show("Consider who??\n");
      return -1;
   }//if

   int comp_val = pc.compareTo(*crit);

   if (pc.isImmort()) {
      Sprintf(buf, "Raw compare value: %i (high means target is wimp)\n",
              comp_val);
      pc.show(buf);
   }//if
   
   if (comp_val > 100) {
      Sprintf(buf, "This %s looks like a joke!\n", get_dude_chic(*crit));
      pc.show(buf);
   }
   else if (comp_val > 75) {
      Sprintf(buf, "This %s is well below you in skills of battle.\n",
              get_fellow_lady(*crit));
      pc.show(buf);
   }
   else if (comp_val > 50) {
      Sprintf(buf, "This %s should be easy to beat in battle.\n",
              get_fellow_lady(*crit));
      pc.show(buf);
   }
   else if (comp_val > 25) {
      Sprintf(buf, "This %s looks like a decent fight, but one you should win.\n",
              get_fellow_lady(*crit));
      pc.show(buf);
   }
   else if (comp_val > 0) {
      Sprintf(buf, "This %s may be slightly weaker than you, but not by much.\n",
              get_fellow_lady(*crit));
      pc.show(buf);
   }
   else if (comp_val > -25) {
      Sprintf(buf, "This %s will be a tough match, it could go either way!\n",
              get_dude_chic(*crit));
      pc.show(buf);
   }
   else if (comp_val > -50) {
      Sprintf(buf, "This %s will be a tough match, you will probably need help.\n",
              get_dude_chic(*crit));
      pc.show(buf);
   }
   else if (comp_val > -75) {
      Sprintf(buf, "This %s looks like %s will kick your butt!\n",
              get_dude_chic(*crit), get_he_she(*crit));
      pc.show(buf);
   }
   else if (comp_val > -100) {
      Sprintf(buf, "This %s will probably stomp you like a bug!\n",
              get_dude_chic(*crit));
      pc.show(buf);
   }
   else {
      Sprintf(buf, "Grab a shovel, maybe %S will dispose of your body in a kind manner!\n",
              crit->getName());
      pc.show(buf);
   }

   Sprintf(buf, "%S sizes you up, almost as if %s was considering battle!\n",
           pc.getName(crit->SEE_BIT), get_he_she(pc));
   crit->show(buf);

   String cmd = "consider";
   ROOM.checkForProc(cmd, NULL_STRING, pc, crit->MOB_NUM);
   return 0;
}//consider


int dsys(critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   Sprintf(buf, "ScryMUD version: %s
Built on:        %s
On Machine:      %s
By:              %s\n\n",
           BuildInfo::getVersion(), BuildInfo::getBuildDate(),
           BuildInfo::getBuildMachine(), BuildInfo::getBuilder());
   pc.show(buf);

   Sprintf(buf, "Listening on port: %i\n", DFLT_PORT);
   pc.show(buf);

   pc.show("Number C++ object instances allocated and not deleted:\n");

   Sprintf(buf, "[%i]%P10 object %P30[%i] %P42 door\n",
           object::getInstanceCount(), door::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 room %P30[%i] %P42 critter\n",
           room::getInstanceCount(), critter::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 String %P30[%i] %P42 String bytes total \n",
           String::string_cnt, String::total_bytes);
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 List<> %P30[%i] %P42 List<> Node \n",
           pc.names.getListCnt(), pc.names.getNodeCnt());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 ZoneList %P30[%i] %P42 List<> Cell\n",
           ZoneList::getInstanceCount(), pc.names.getCellCnt());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 zone %P30[%i] %P42 stat_spell_cell\n",
           zone::getInstanceCount(), stat_spell_cell::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 PathCell %P30[%i] %P42 teacher_data.\n",
           PathCell::getInstanceCount(), teacher_data::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 temp_crit_data %P30[%i] %P42 immort_data\n",
           temp_crit_data::getInstanceCount(),
           immort_data::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 shop_data %P30[%i] %P42 temp_proc_data\n",
           shop_data::getInstanceCount(), temp_proc_data::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 say_proc_cell %P30[%i] %P42 action_proc_data\n",
           say_proc_cell::getInstanceCount(),
           action_proc_data::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 spec_data %P30[%i] %P42 mob_data\n",
           spec_data::getInstanceCount(), mob_data::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 pc_data %P30[%i] %P42 KeywordPair\n",
           pc_data::getInstanceCount(), KeywordPair::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 obj_construct_data %P30[%i] %P42 obj_spec_data\n",
           obj_construct_data::getInstanceCount(),
           obj_spec_data::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 bag_struct %P30[%i] %P42 object\n", //NOTE:  Re-use this one!
           bag_struct::getInstanceCount(), object::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 door_data %P30[%i] %P42 ScriptCmd\n",
           door_data::getInstanceCount(), ScriptCmd::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 GenScript  %P30[%i] %P42 MobScript\n",
           GenScript::getInstanceCount(), MobScript::getInstanceCount());
   pc.show(buf);
   Sprintf(buf, "[%i]%P10 RoomScript\n",
           RoomScript::getInstanceCount());
   pc.show(buf);

   Sprintf(buf, "\nSize (length) of:\n\tembattled_rooms: %i \t affected_doors: %i\n",
           embattled_rooms.size(), affected_doors.size());
   pc.show(buf);
   
   Sprintf(buf, "\taffected_mobs: %i \t affected_rooms: %i\n",
           affected_mobs.size(), affected_rooms.size());
   pc.show(buf);

   Sprintf(buf, "\taffected_rooms: %i, \t affected_objs: %i\n",
           affected_rooms.size(), affected_objects.size());
   pc.show(buf);

   Sprintf(buf, "\tproc_action_mobs: %i \t pulsed_proc_mobs: %i\n",
           proc_action_mobs.getCurLen(), pulsed_proc_mobs.size());
   pc.show(buf);

   Sprintf(buf, "\tpulsed_proc_rooms: %i \t pulsed_proc_objects: %i\n",
           pulsed_proc_rooms.size(), pulsed_proc_objects.size());
   pc.show(buf);

   Sprintf(buf, "\tproc_action_rooms: %i \t proc_action_objs: %i\n",
           proc_action_rooms.getCurLen(), proc_action_objs.getCurLen());
   pc.show(buf);

   return 0;
}//dsys


int handle_olc(critter& pc, int do_sub) {
   if (!pc.isImmort()) {
      pc.show("You are not cleared to build, talk to a god.\n");
      return -1;
   }
   else if (pc.pc->imm_data->olc_counter > 0) {
      restart_olc(pc);
      return 0;
   }
   else {
      start_olc(pc);
      // This mostly just shows the prompt... --BLG
      return pc.processInput(pc.pc->input, do_sub);
   }//else
}


/* this is the command called by the player, it parses and calls emote() */
int com_emote(const String* msg, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mrGFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (msg->Strlen() == 0) {
      show("What do you wish to emote?\n", pc);
      return -1;
   }//if

   emote(*msg, pc, ROOM, TRUE);  //do it then 
   Sprintf(buf, "%S %S\n", name_of_crit(pc, ~0), msg);
   buf.Cap();
   show(buf, pc);
   return -1;
}//com_emote


/* Possessive Emote
 * this is the command called by the player, it parses and calls pemote()
 */
int com_pemote(const String* msg, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mrGFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (msg->Strlen() == 0) {
      show("What do you wish to emote?\n", pc);
      return -1;
   }//if

   pemote(*msg, pc, ROOM, TRUE);  //do it then 
   Sprintf(buf, "%S's %S\n", name_of_crit(pc, ~0), msg);
   buf.Cap();
   show(buf, pc);
   return 0;
}//com_pemote


/* command called by pc as gecho() */
int com_gecho(const String* msg, critter& pc) {
   String buf2(100);

   if (!ok_to_do_action(NULL, "mrGFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.getImmLevel() < 5) {
      show("You haven't the power!\n", pc);
      return -1;
   }//if

   if (msg->Strlen() == 0) {
      show("What do you wish to gecho?\n", pc);
      return -1;
   }//if

   Cell<critter*> cll(pc_list);
   critter* ptr;

   while ((ptr = cll.next())) {
      if (ptr->isImmort() && (ptr->getImmLevel() >= pc.getImmLevel())) {
         Sprintf(buf2, "[%S]  %S", pc.getName(), msg);
         show(buf2, *ptr);
      }
      else {
         show(*msg, *ptr);
         show("\n", *ptr);
      }
   }//while
   return 0;
}//com_gecho



/* command called by pc as recho(), room echo */
int com_recho(const String* msg, critter& pc) {
   String buf2(100);

   if ((pc.isImmort() && pc.IMM_LEVEL > 1) ||
       ((pc.isSmob() && !pc.isCharmed()))) {

      if (!ok_to_do_action(NULL, "rGFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
         return -1;
      }

      if (msg->Strlen() == 0) {
         show("What do you wish to recho?\n", pc);
         return -1;
      }//if
      
      Cell<critter*> cll(ROOM.getCrits());
      critter* ptr;
      
      while ((ptr = cll.next())) {
         if (ptr->isImmort() && (ptr->getImmLevel() >= pc.getImmLevel())) {
            Sprintf(buf2, "[%S]  %S", pc.getName(), msg);
            show(buf2, *ptr);
         }
         else {
            show(*msg, *ptr);
            show("\n", *ptr);
         }
      }//while
      return 0;
   }//if
   else {
      show("You haven't the power!\n", pc);
      return -1;
   }//if
}//com_gecho
   


/* command called by pc as recho(), room echo */
int com_pecho(int i_th, const String* targ, const String* msg, critter& pc) {
   String buf2(100);

   if ((pc.isImmort() && pc.IMM_LEVEL > 1) ||
       ((pc.isSmob() && !pc.isCharmed()))) {

      if (!ok_to_do_action(NULL, "rGFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
         return -1;
      }

      if (msg->Strlen() == 0) {
         show("What do you wish to pecho?\n", pc);
         return -1;
      }//if
      
      critter* ptr; //targ
      ptr = have_crit_named(pc_list, i_th, targ,
                            pc.SEE_BIT, ROOM);

      if (ptr) {
         if (ptr->isImmort() && (ptr->getImmLevel() >= pc.getImmLevel())) {
            Sprintf(buf2, "[%S]  %S", pc.getName(), msg);
            show(buf2, *ptr);
         }
         else {
            show(*msg, *ptr);
            show("\n", *ptr);
         }
         return 0;
      }//if
   }//if
   else {
      show("You haven't the power!\n", pc);
   }//if
   return -1;
}//com_pecho
   

/* command called by pc as zecho(), zone echo */
int com_zecho(const String* msg, critter& pc) {
   String buf2(100);

   if (!ok_to_do_action(NULL, "IGFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }


   if (pc.IMM_LEVEL < 5) {
      show("You haven't the power!\n", pc);
      return -1;
   }//if

   if (msg->Strlen() == 0) {
      show("What do you wish to zecho?\n", pc);
      return -1;
   }//if

   Cell<critter*> cll(pc_list);
   critter* ptr;

   while ((ptr = cll.next())) {
      if (ptr->getCurZoneNum() == pc.getCurZoneNum()) {
         if (ptr->isImmort() && (ptr->getImmLevel() >= pc.getImmLevel())) {
            Sprintf(buf2, "[%S]  %S", pc.getName(), msg);
            show(buf2, *ptr);
         }
         else {
            show(*msg, *ptr);
            show("\n", *ptr);
         }
      }//while
   }//if
   return 0;
}//com_zecho


int prone(critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "mBFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.POS == POS_PRONE) {
      show("You are already prone.\n", pc);
      return -1;
   }//if

   if (pc.POS == POS_STUN) {
      show("You can't seem to order your body into motion!\n", pc);
      return -1;
   }//if

   if (pc.POS == POS_SLEEP) {
      show("You are asleep!\n", pc);
      return -1;
   }//if


   emote("drops to the ground in a prone position.", pc, ROOM, TRUE);
   show("You drop to the prone position.\n", pc);
   pc.setPosn(POS_PRONE);

   String cmd = "prone";
   ROOM.checkForProc(cmd, NULL_STRING, pc, -1);
   return 0;
}//prone()


/* ranged attack, pretty neat eh?? */
int shoot(int i_th, const String* dir, int j_th, const String* mob, 
	   critter& pc) {
   String buf(100);
   door* dptr;
   int dest;
   critter* targ;

   if (!ok_to_do_action(NULL, "SFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

			/* check for exit */

   dptr = door::findDoor(ROOM.DOORS, i_th, dir, pc.SEE_BIT, ROOM);
   if (!dptr) {
      show("You can't find an exit in that direction.\n", pc);
      return -1;
   }//if

   if (dptr->isClosed()) {
      show("That direction is closed.\n", pc);
      return -1;
   }//if

		/* have door, its open */
		   /* check for targ */

   dest = abs(dptr->destination);
   targ = room_list[dest].haveCritNamed(j_th, mob, pc.SEE_BIT);   

   if (!targ) {
      show("You can't find your target.\n", pc);
      return -1;
   }//if

   if (targ->isMob()) { //its a SMOB
      targ = mob_to_smob(*targ, dest, TRUE, j_th, mob, 
	     pc.SEE_BIT | room_list[dest].getVisBit());
   }//if

   if (!(targ = check_for_diversions(*targ, "GSM", pc)))
      return -1;

   /* can't guard w/out being a NON-MOB (ie PC or SMOB */
   return do_shoot(*targ, pc);
}//shoot


int do_shoot(critter& targ, critter& pc) {
   String buf(100);
   Cell<object*> cll;
   object* ammo = NULL;
   int targ_is_dead = FALSE;

   if ((targ.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  SMOB sent to do_shoot.\n");
      return -1;
   }//if

   if (can_start_battle(targ, pc, TRUE)) {
      if (!pc.EQ[9] || !((pc.EQ[9]->OBJ_FLAGS.get(46)) ||
		         pc.EQ[9]->OBJ_FLAGS.get(47))) {
  	 show("You must be wielding a projectile weapon in order to shoot.\n",
	      pc);
	 return -1;
      }//if
      if (pc.EQ[9]->OBJ_FLAGS.get(45)) { //needs ammo to work
	 if (pc.EQ[9]->OBJ_FLAGS.get(46)) { //needs darts
            pc.inv.head(cll);
            while ((ammo = cll.next())) {
      	       if (ammo->OBJ_FLAGS.get(48)) { //its a dart
		  pc.inv.lose(cll);
                  break;
	       }//if
	    }//while
	 }//if dart thrower
         else { //bow
            pc.inv.head(cll);
            while ((ammo = cll.next())) {
      	       if (ammo->OBJ_FLAGS.get(49)) { //its an arrow
		  pc.inv.lose(cll);
                  break;
	       }//if
	    }//while
         }//else

         if (!ammo) {
	    show("You are out of ammo.\n", pc);
	    return -1;
 	 }//if
      }//if needs ammo

		/* we have ammo if we need it... */

      String lproj_name("a projectile");
      String sproj_name("projectile");

      if (ammo) {
	 lproj_name = *(ammo->getLongName());
         sproj_name = *(ammo->getName());
      }//if

      if (mudlog.ofLevel(DBG)) {
         mudlog << "Long Projectile name:  " << lproj_name << endl
                << "Short Projectile name:  " << sproj_name << endl;
      }

      Sprintf(buf, "readies %s %S.\n",
	      get_his_her(pc), name_of_obj(*(pc.EQ[9]), ~0));
      emote(buf, pc, ROOM, TRUE);

      if (!did_shot_hit(targ, pc)) {
	 show("Your shot goes wide of your mark.\n", pc);
	 emote("shoots and misses.", pc, ROOM, TRUE);
	 Sprintf(buf, "%S comes whistling by your head!\n", &lproj_name);
         buf.Cap();
	 show_all(buf, room_list[targ.getCurRoomNum()]);

         String cmd = "shoot";
         ROOM.checkForProc(cmd, NULL_STRING, pc, targ.MOB_NUM);

	 do_shot_proc(targ, pc, FALSE, targ_is_dead);
      }//if
      else {
         int damage = d(pc.EQ[9]->extras[7], pc.EQ[9]->extras[6]);
         if (ammo) {
   	    damage += d(ammo->extras[7], ammo->extras[6]);
	 }//if
         exact_raw_damage(damage, NORMAL, targ);

	 if (targ.HP > 0) { //still alive
	    Sprintf(buf, "You hit %S with %S.",
		    name_of_crit(targ, pc.SEE_BIT), &lproj_name);
	    show(buf, pc);
	    Sprintf(buf, "%S wounds you with %S!\n",
		    name_of_crit(pc, targ.SEE_BIT), &lproj_name);
	    show(buf, targ);
	    Sprintf(buf, "staggers as %S's %S hits %s!\n",
		    name_of_crit(pc, ~0), &sproj_name,
		    get_him_her(targ));
	    emote(buf, targ, room_list[targ.getCurRoomNum()], TRUE);

            String cmd = "shoot";
            ROOM.checkForProc(cmd, NULL_STRING, pc, targ.MOB_NUM);

	    do_shot_proc(targ, pc, TRUE, targ_is_dead);
	 }//if hit but not fatal
	 else { //fatality
	    Sprintf(buf, "Your %S sinks deep into %S's chest!\n",
		    &sproj_name, name_of_crit(targ, pc.SEE_BIT));
	    show(buf, pc);
	    Sprintf(buf, "%S's %S sinks deep into %S's chest!\n",
	            name_of_crit(pc, ~0), &sproj_name,
		    name_of_crit(targ, ~0));
	    room_list[targ.getCurRoomNum()].showAllCept(buf, &targ);
	    Sprintf(buf, "%S has slain you with %s %S!\n",
		    name_of_crit(pc, targ.SEE_BIT), get_his_her(pc),
		    &sproj_name);
	    show(buf, targ);
	    emote("twitches once and moves no more!", targ, 
		  room_list[targ.getCurRoomNum()], TRUE);
	    alert_room_proc(targ.getCurRoomNum(), PROJECTILE_DEATH, targ, pc);
	    agg_kills_vict(pc, targ);
	 }//else fatality
      }// else hit
   }// if can start battle

   if (ammo) {
      if (ammo->IN_LIST) {
         delete ammo;
      }//if
   }//if

   pc.setPause(pc.getPause() + 1);

   return 0;
}//do_shoot


int _throw(int i_th, const String* dir, int j_th, const String* mob, 
            critter& pc) {
   String buf(100);
   door* dptr;
   int dest;
   critter* targ;

   if (!ok_to_do_action(NULL, "mSFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

			/* check for exit */

   dptr = door::findDoor(ROOM.DOORS, i_th, dir, pc.SEE_BIT, ROOM);
   if (!dptr) {
      show("You can't find an exit in that direction.\n", pc);
      return -1;
   }//if

   if (dptr->isClosed()) {
      show("That direction is closed.\n", pc);
      return -1;
   }//if

		/* have door, its open */
		   /* check for targ */

   dest = abs(dptr->destination);
   targ = room_list[dest].haveCritNamed(j_th, mob, pc.SEE_BIT);   

   if (!targ) {
      show("You can't find your target.\n", pc);
      return -1;
   }//if

   if (targ->isMob()) { //its a SMOB
      targ = mob_to_smob(*targ, dest, TRUE, j_th, mob, 
	     pc.SEE_BIT | room_list[dest].getVisBit());
   }//if

   if (!(targ = check_for_diversions(*targ, "GSM", pc)))
      return -1;

   return do_throw(*targ, pc);
}//throw


int do_throw(critter& targ, critter& pc) {
   String buf(100);
   int posn;

   if ((targ.isMob()) || (pc.isMob())) {
      mudlog.log(ERR, "ERROR:  MOB sent to do_throw.\n");
      return -1;
   }//if

   if (pc.EQ[10])
      posn = 10;
   else if (pc.EQ[9])
      posn = 9;
   else {
      show("You must be holding or wielding something to throw it!\n", pc);
      return -1;
   }//else
   
   if (!obj_drop_by(*(pc.EQ[posn]), pc)) {//should messages too
      return -1;
   }//if can't get rid of it

   if (pc.EQ[posn]->OBJ_FLAGS.get(57)) { //if its a weapon
      if (can_start_battle(targ, pc, TRUE)) {
         Sprintf(buf, "readies %s %S for throwing...",
                 get_his_her(pc), name_of_obj(*(pc.EQ[posn]), ~0));
         emote(buf, pc, ROOM, TRUE);
         
         if (!did_shot_hit(targ, pc, TRUE)) {
            Sprintf(buf, "Your %S just misses %S!\n", 
                    pc.EQ[posn]->getName(pc.SEE_BIT), 
                    name_of_crit(targ, pc.SEE_BIT));
            show(buf, pc);
            emote("throws but misses.", pc, ROOM, TRUE);
            Sprintf(buf, "%S comes whistling by your head!\n", 
                    long_name_of_obj(*(pc.EQ[posn]), ~0));
            buf.Cap();
            show_all(buf, room_list[targ.getCurRoomNum()]);
            room_list[targ.getCurRoomNum()].gainInv(pc.EQ[posn]);
            
            String cmd = "throw";
            ROOM.checkForProc(cmd, NULL_STRING, pc, targ.MOB_NUM);

            int is_dead = FALSE;
            do_shot_proc(targ, pc, FALSE, is_dead); //same response as being shot
         }//if
         else { //did hit
            int damage = d(pc.EQ[posn]->extras[7], 
                           pc.EQ[posn]->extras[6]);
            
            exact_raw_damage(damage, NORMAL, targ, pc);
            
            targ.gainInv(pc.EQ[posn]); //item is now (in) the target
            gain_eq_effects(*(pc.EQ[posn]), obj_list[0], targ, FALSE, FALSE);
            
            if (targ.HP > 0) { //still alive
               Sprintf(buf, "You hit %S with your %S.",
                       name_of_crit(targ, pc.SEE_BIT), 
                       name_of_obj(*(pc.EQ[posn]), pc.SEE_BIT));
               show(buf, pc);
               Sprintf(buf, "%S wounds you with %s %S!\n",
                       name_of_crit(pc, targ.SEE_BIT),
                       get_his_her(pc), 
                       name_of_obj(*(pc.EQ[posn]), targ.SEE_BIT));
               show(buf, targ);
               Sprintf(buf, "staggers as %S's %S hits %s!",
                       name_of_crit(pc, ~0), 
                       name_of_obj(*(pc.EQ[posn]), targ.SEE_BIT),
                       get_his_her(targ));
               emote(buf, targ, room_list[targ.getCurRoomNum()], TRUE);

               int is_dead = FALSE;
               do_shot_proc(targ, pc, TRUE, is_dead); //same as being shot
               
               if (!is_dead) {
                  String cmd = "throw";
                  ROOM.checkForProc(cmd, NULL_STRING, pc, targ.MOB_NUM);
               }
            }//if hit but not fatal
            else { //fatality
               Sprintf(buf, "Your %S sinks deep into %S's chest!\n",
                       name_of_obj(*(pc.EQ[posn]), ~0), 
                       name_of_crit(targ, pc.SEE_BIT));
               show(buf, pc);
               Sprintf(buf, "\b's %S sinks deep into %S's chest!\n",
                       name_of_obj(*(pc.EQ[posn]), ~0), 
                       name_of_crit(targ, ~0));
               emote(buf, pc, room_list[targ.getCurRoomNum()], TRUE);
               Sprintf(buf, "%S has slain you with %s %S!\n",
                       name_of_crit(pc, targ.SEE_BIT), get_his_her(pc),
                       name_of_obj(*(pc.EQ[posn]), ~0));
               buf.Cap();
               show(buf, targ);
               emote("twitches once and moves no more!", targ, 
                     room_list[targ.getCurRoomNum()], TRUE);
               alert_room_proc(targ.getCurRoomNum(), PROJECTILE_DEATH,
                               targ, pc);
               agg_kills_vict(pc, targ);
            }//else fatality
         }// else hit
         remove_eq_effects(*(pc.EQ[posn]), pc, FALSE, FALSE, posn);
         pc.EQ[posn] = NULL; //don't have it any more
      }// if can start battle
   }//if a weapon
   else {
      Sprintf(buf, "You throw %S at %S...it falls to the ground.\n",
              long_name_of_obj(*(pc.EQ[posn]), pc.SEE_BIT),
              name_of_crit(targ, pc.SEE_BIT));
      show(buf, pc);
      Sprintf(buf, "%S drops at %S's feet.\n",
              name_of_obj(*(pc.EQ[posn]), ~0), 
              name_of_crit(targ, ~0));
      pemote(buf, pc, room_list[targ.getCurRoomNum()], TRUE, &targ);
      Sprintf(buf, "%S throws %s %S at your feet.\n",
              name_of_crit(pc, targ.SEE_BIT), get_his_her(pc),
              name_of_obj(*(pc.EQ[posn]), targ.SEE_BIT));
      buf.Cap();
      show(buf, targ);
      
      remove_eq_effects(*(pc.EQ[posn]), pc, FALSE, FALSE, posn);
      room_list[targ.getCurRoomNum()].gainInv(pc.EQ[posn]);
      pc.EQ[posn] = NULL; //don't have it any more
   }//else, not a weapon
   return 0;
}//do_throw


int did_shot_hit(critter& targ, critter& pc, int throwing = FALSE) {
   float pc_roll;
   float targ_roll;

   pc_roll = (3 * pc.DEX + 5 * pc.HIT + pc.LEVEL);
   if (targ.POS == POS_PRONE) 
      pc_roll *= 0.25;
   if (room_list[targ.getCurRoomNum()].isSwamp()) {//TERRAIN TYPE SWAMP
      pc_roll *= 0.5;
   }//if

   if (throwing) {
     pc_roll = d(1, (int)(pc_roll) + 
		 get_percent_lrnd(THROWING_SKILL_NUM, pc));
   }//if
   else {
     pc_roll = d(1, (int)(pc_roll) + 
                 get_percent_lrnd(ARCHERY_SKILL_NUM, pc));
   }//else

   targ_roll = (4 * targ.DEX + 100); //the 100 offsets pc's percent lrnd
   if (targ.EQ[18] || is_affected_by(MAGIC_SHIELD_SKILL_NUM, targ))
      targ_roll *= 2;
   targ_roll = d(1, (int)(targ_roll));
   
   return (targ_roll < pc_roll);

}//did_shot_hit


int zcreate(int num_rooms, const String* name, int num_ticks,
	     critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.getImmLevel() < 9) {
      show("Eh?", pc);
      return -1;
   }//if

   if ((num_rooms == 1) || (name->Strlen() == 0)) {
      show("Usage:  zcreate [num_rooms] [num_ticks] ['zone name'].\n", pc);
      return -1;
   }//if

   ZoneCollection::instance().createNewZone(pc, num_ticks, num_rooms, *name);
   return 0;
}//zcreate


int zenable(int znum, const String* char_name, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.getImmLevel() < 9) {
      show("Eh?\n", pc);
      return -1;
   }//if

   if (char_name->Strlen() == 0) {
      show("You need to specify a pc-name.\n", pc);
      return -1;
   }//if

   if (!check_l_range(znum, 0, NUMBER_OF_ZONES -1, pc, TRUE))
      return -1;

   ZoneCollection::instance().elementAt(znum).addOwner(*char_name);

   Sprintf(buf, "You add '%S' to the owners of zone# %i.\n",
	   char_name, znum);
   show(buf, pc);

   ZoneCollection::instance().writeSelf();
   return 0;
}//zenable


int ch_zname(int znum, const String* new_name, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.getImmLevel() < 9) {
      show("Eh?\n", pc);
      return -1;
   }//if

   if (new_name->Strlen() == 0) {
      show("You need to specify a new name.\n", pc);
      return -1;
   }//if

   if (!check_l_range(znum, 0, NUMBER_OF_ZONES -1, pc, TRUE))
      return -1;

   ZoneCollection::instance().elementAt(znum).setName(*new_name);

   Sprintf(buf, "You change zone %i's name to -:%S:-\n",
           znum, new_name);
   show(buf, pc);

   ZoneCollection::instance().writeSelf();
   return 0;
}//zenable


int zdisable(int znum, const String* char_name, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.getImmLevel() < 9) {
      show("Eh?\n", pc);
      return -1;
   }//if

   if (char_name->Strlen() == 0) {
      show("You need to specify a pc-name.\n", pc);
      return -1;
   }//if

   if (!check_l_range(znum, 0, NUMBER_OF_ZONES -1, pc, TRUE))
      return -1;

   if (ZoneCollection::instance().elementAt(znum).removeOwner(*char_name)) {
      Sprintf(buf, "You delete '%S' from the owners of zone# %i.\n",
              char_name, znum);
      show(buf, pc);
      ZoneCollection::instance().writeSelf();
      return 0;
   }//if
   else {
      pc.show("Couldn't find that name in the list of owners.\n");
      return -1;
   }//else
}//zdisable


int see_all(critter& pc) {
   if (pc.pc && pc.pc->imm_data) {
      pc.SEE_BIT |= 47;   //all normal, and imm_invis1
      if (pc.IMM_LEVEL >= 3)
         pc.SEE_BIT |= 16;
      if (pc.IMM_LEVEL >= 5)
         pc.SEE_BIT |= 64;
      if (pc.IMM_LEVEL >= 7)
         pc.SEE_BIT |= 128;
      if (pc.IMM_LEVEL >= 9)
         pc.SEE_BIT |= 256;
      if (pc.IMM_LEVEL >= 10)
         pc.SEE_BIT |= 512;
      
      show("You may now detect all but the most powerful beings!\n", pc);
      return 0;
   }//if
   else {
      show("Eh??\n", pc);
      return -1;
   }//else
}//see_all


int wizinvis(critter& pc) {
   if (pc.pc && pc.pc->imm_data) {
      pc.VIS_BIT |= 8;   //imm_invis1
      if (pc.IMM_LEVEL >= 3)
         pc.VIS_BIT |= 16;
      if (pc.IMM_LEVEL >= 5)
         pc.VIS_BIT |= 64;
      if (pc.IMM_LEVEL >= 7)
         pc.VIS_BIT |= 128;
      if (pc.IMM_LEVEL >= 9)
         pc.VIS_BIT |= 256;
      if (pc.IMM_LEVEL >= 10)
         pc.VIS_BIT |= 512;
      
      show("You are invisible to all but the most powerful beings!\n", pc);
      return 0;
   }//if
   else {
      show("Eh??\n", pc);
      return -1;
   }//else
}//wizinvis


/* make yourself visible, for gods only, it could be some sort of punishment
   for mortals to be invisible, they will have to find some other way of
   making themselves visible */
int visible(critter& pc) {
   if (pc.pc && pc.pc->imm_data) {
      pc.VIS_BIT = 0;
      emote("fades into sight.", pc, ROOM, TRUE);
      show("Your corporal form rejoins the living world.\n", pc);
      return 0;
   }//if
   else {
     show("Not that easy here...", pc);
     return -1;
   }//else
}//visible


int describe(critter& pc) {

   if (pc.isNpc()) {
      show("Eh??\n", pc);
      return -1;
   }//if
  
   show("Enter a new description for yourself.\n", pc);
   show("Use a solitary '~' on a line by itself to end.\n", pc);
   show("You may now paste descs up to 2k, ';' acts as a newline btw.\n",
	pc);

   pc.long_desc.Clear();
   pc.setMode(MODE_CH_DESC);
   return 0;
}//describe




