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

///********************  login.cc  ****************************///
#include "commands.h"
#include "classes.h"
#include "misc.h"
#include <stdio.h>
#include "spells.h"
#include "skills.h"
#include "command2.h"
#include "load_wld.h"
#include <time.h>
#include <PtrArray.h>
#include <unistd.h>
#include "const.h"

extern List<critter*> new_pc_list;

/*  This will prompt for name and passwd...will read in an old char from
    a file or will create a new one... */

int is_valid_name(const String& name) {
   int j = name.Strlen();
   for (int i = 0; i<j; i++) {
      if (!isalpha(name[i]))
         return FALSE;
   }//for
   return TRUE;
}//is_valid_name()


void critter::startLogin() {  
   mudlog.log(TRC, "In startLogin.\n");
   pc->mode = MODE_LOGGING_IN;
   pc->link_condition = CON_LOGGING_IN;
   pc->index = 0; //set to show the first prompt
   pc->input = NULL_STRING;  //null out input
   show(Opening_Screen);
   show(login_prompts[0]); 
   mudlog.log(TRC, "Done w/start_login.\n");
}//start_login


void critter::doLogin() {
   String string;
   short eos, term_by_period;
   int j;
   String* string2;
   String name(50);
   String tmp_host(50);

   mudlog.log(TRC, "In do_login.\n");

   if (!pc) {
      mudlog.log(ERR, "ERROR:  PC is NULL in do_login.\n");
      return;
   }//if

   if ((pc->index > 6) || (pc->index < 0)) {
      mudlog.log(ERR, "ERROR:  bad index sent to do_login.\n");
      setMode(MODE_LOGOFF_NEWBIE_PLEASE);
   }//if
   else {
      switch (pc->index)
         {
         case 0:  /* get the name.. */
            string = pc->input.Get_Command(eos, term_by_period);
            
            if (mudlog.ofLevel(DBG)) {
               mudlog << "TRACE:  in do_login, got name -:" << string << ":-"
                      << endl;
            }
            string.Cap();
            j = string.Strlen();
            if (j > 29) {
               show("\nYour name must be less than 30 characters.\n");
            }//if
            else if (j < 3) {
               show("\nYour name is just too short.\n");
            }//if
            else if (!is_valid_name(string)) {
               show("That is not a valid name.\n");
            }//if
            else {
               //log("In else of case 0\n");
               string2 = new String(string);
               Put(string2, (names));
               name = string;
               name.Tolower();
               name.Prepend("./Pfiles/");
               
               ifstream rfile(name);
               
               if (rfile) { //if player is not new
                  pc->index = 5;
               }//if
               else {
                  if (is_newbie_banned(*(getHostName()))) {
                     show("Your site has been newbie-banned.\n");
                     setMode(MODE_LOGOFF_NEWBIE_PLEASE);
                  }
                  else {
                     pc->index = 1;
                  }
               }//else
            }//else            
            //         log("All done w/case 0.\n");
            break;
         case 1:  //password for new player
            show(ANSI_ECHO_ON); //echo ON
            string = pc->input.Get_Command(eos, term_by_period);
            j = string.Strlen();
            if (j > 29) {
               show("\nYour password must be less than 30 characters.\n");
            }//if
            else if (j < 3) {
               show("\nYour password must be at least 3 characters.\n");
               mudlog.log(INF, "Logging off, password too short.\n");
               setMode(MODE_LOGOFF_NEWBIE_PLEASE);
            }//if
            else {
               pc->password = string;
               pc->index = 2; //go re-enter password
            }//else            
            break;
            
         case 2:  //recheck password for new player
            show(ANSI_ECHO_ON); //echo ON
            string = pc->input.Get_Command(eos, term_by_period);
            if (pc->password == string) {
               pc->password = crypt((const char*)(pc->password), "bg");
               pc->index = 3; //go choose male/female/neuter
            }
            else {
               show("Passwords do not match.\n\n");
               pc->index = 1; //go back, try again
            }//else            
            break;
            
         case 3: //get sex
            string = pc->input.Get_Command(eos, term_by_period);
            pc->index = 4;
            if (toupper(string[0]) == 'M')
               SEX = 1; //male
            else if (toupper(string[0]) == 'F')
               SEX = 0; //female
            else if (toupper(string[0]) == 'N')
               SEX = 2; //neuter
            else 
               pc->index = 3; // go try again :)
            break;
            
         case 6: //get race
            string = pc->input.Get_Command(eos, term_by_period);
            if (isnum(string)) {
               j = atoi(string);
               if (j == HUMAN || j == ANITRE || j == DARKLING ||
                   j == DWARF || j == OGRUE || j == ELF || j == SOMBRIAN) {
                  RACE = j;
                  if (mudlog.ofLevel(DBG)) {
                     mudlog << "New character, setting race to: " << j
                            << endl;
                  }//if
                  quit_do_login_new(*this); //completed it correctly
               }//if
               else { //get help?
                  switch (j) {
                  case 101:
                     string = "human";
                     break;
                  case 102:
                     string = "anitre";
                     break;
                  case 104:
                     string = "darkling";
                     break;
                  case 107:
                     string = "dwarf";
                     break;
                  case 109:
                     string = "ogrue";
                     break;
                  case 111:
                     string = "elf";
                     break;
                  case 114:
                     string = "sombri";
                     break;
                  default:
                     return;
                  }//switch
                  help(1, &string, *this);
               }//else
            }//if
            break;
            
         case 4: //get class
            string = pc->input.Get_Command(eos, term_by_period);
            if (isnum(string)) {
               j = atoi(string);
               if (check_l_range(j, 1, 8, *this, FALSE) && j != 6) {
                  CLASS = j;
                  pc->index = 6; //go get race
               }//if
               else {
                  show("That is not a valid race.\n");
               }//else
            }//if
            break;
            
         case 5: //password for non-new players.
            show(ANSI_ECHO_ON); //echo ON
            mudlog.log(DBG, "in case 5\n");
            if (TRUE) { //needs to be inside a block to shut g++ up!!
               string = pc->input.Get_Command(eos, term_by_period);
               
               string2 = names.peekFront();
               
               name = *(string2);
               name.Tolower();
               name.Prepend("./Pfiles/");
               
               ifstream rfile(name);
               if (!rfile) {
                  if (mudlog.ofLevel(ERR)) {
                     mudlog << "ERROR:  rfile not opened, case 5 of login.\n"
                            << "Here is the bad filename -:" << name << ":-"
                            << endl;
                  }
                  do_shutdown =  TRUE;
                  exit(2);
               }//if
               
               int tmp_int = pc->descriptor;
               int tmp_index = pc->index;
               tmp_host = pc->host;
               Read(rfile, TRUE);
               
               pc->descriptor = tmp_int;
               pc->host = tmp_host;
               pc->index = tmp_index;
               pc->mode = MODE_LOGGING_IN;
               pc->link_condition = CON_LOGGING_IN;
               
               int was_link_dead = TRUE;

               mudlog << "pc->password:  -:" << pc->password << ":-"
                      << endl;

               if (strcmp(pc->password, "X") == 0) {
                  mudlog << "Was an X.." << endl;
                  show("Your password (X) is the default, please change\n");
                  show("it as soon as you are fully logged in.\n");
                  string = "X";
                  pc->password = crypt("X", "bg");
               }//if

               if (strcmp(crypt(string, "bg"), pc->password) == 0) {
                  
                  critter* old_ptr;
                  old_ptr = have_crit_named(linkdead_list, 1, 
                                            Top((names)), ~0, *(getCurRoom()),
                                            TRUE);
                  if (!old_ptr) {
                     old_ptr = have_crit_named(pc_list, 2,
                                               Top(names), ~0, *(getCurRoom()),
                                               TRUE);
                     was_link_dead = FALSE;
                  }//if
                  
                  if (old_ptr) {
                     /*  Reconnecting after link Death. */
                     //if (mudlog.ofLevel(DBG)) {
                     //   mudlog << "WARNING:  Reconnecting after link death, or re-logging-in"
                     //       << " was_link_dead:  " << was_link_dead << endl;
                     //}
                     
                     /*  Make sure old descriptor is freed. */
                     int tmp_old_desc = old_ptr->pc->descriptor;
                     old_ptr->pc->descriptor = pc->descriptor;
                     pc->descriptor = tmp_old_desc;
                     
                     /*  set old player back in action */
                     old_ptr->cur_stats[0] &= ~32;      //make em visable again
                     old_ptr->pc->link_condition = CON_PLAYING;
                     old_ptr->pc->index = 0;
                     old_ptr->pc->mode = MODE_NORMAL;
                     old_ptr->pc->host = pc->host;
                     
                     linkdead_list.loseData(old_ptr);
                     if (was_link_dead) {
                        new_pc_list.gainData(old_ptr); //should be good to go
                        //pc.pc->descriptor = -1; //don't wanna close it!
                     }//if
                     
                     //should just clean up its mess...
                     setMode(MODE_LOGOFF_NEWBIE_PLEASE);
                     
                     old_ptr->show("<ENGAGE_HEGEMON>\n");
                     look(1, &NULL_STRING, *old_ptr); //autolook
                     
                     if (was_link_dead) {
                        if (mudlog.ofLevel(DBG)) {
                           mudlog << "Was link dead:  " 
                                  << *(old_ptr->getName()) << " address:  "
                                  << old_ptr << endl;
                        }
                        ::emote("has reconnected.", *old_ptr,
                                room_list[old_ptr->getCurRoomNum()], TRUE); 
                        old_ptr->show("Your body shivers as a new soul takes command.\n");
                     }//if
                     else {
                        if (mudlog.ofLevel(DBG)) {
                           mudlog << "Was already logged on:  "
                                  << *(old_ptr->getName()) << " address:  "
                                  << old_ptr << endl;
                        }
                        ::emote("retakes control of a body already in use.",
                                *old_ptr, room_list[old_ptr->getCurRoomNum()],
                                TRUE); 
                        old_ptr->show("You take over a body already in use.\n");
                     }//else

                     String buf(100);
                     Sprintf(buf, "  INFO:  %S has regained connection from: %S\n",
                             old_ptr->getName(), old_ptr->getHostName());
                     show_all_info(buf);
                  }//if was link dead
                  else {
                     /* regular logging in */
                     quit_do_login_old(*this); //join the game
                  }//else
               }//if
               else {
                  show("Password Incorrect...  Cya!\n");
                  if (mudlog.ofLevel(SEC)) {
                     mudlog << "WARNING:  old player:  Logging off character:  " 
                            << (*getName()) << " from host:  " << pc->host
                            << " for incorrect password:  -: " << string
                            << ":-" << endl;
                  }

                  setMode(MODE_LOGOFF_NEWBIE_PLEASE);
               }//else
            }//if TRUE
            break;
            
         default:
            mudlog.log(ERR, "ERROR:  default called in do_login.\n");
            show("ERROR found, tell imp:  'default called in do_login'.\n");
            setMode(MODE_LOGOFF_NEWBIE_PLEASE);
            break;
         }//switch        
   }//else
   mudlog.log(TRC, "Done w/do_login.\n");
}//do_login


int  quit_do_login_new(critter& pc) {
   int i;
   //log("in quit_do_login_new\n");

   pc.short_desc = " the newbie";
   pc.long_desc = "You see someone who is quite normal.";

   pc.pc->link_condition = CON_PLAYING;
   pc.pc->index = 0;
   pc.pc->mode = MODE_NORMAL;
   pc.AGE    = 18;
   pc.PROMPT_STRING = "%N<%hH %mM %vV >  ";
   
   pc.PC_FLAGS.turn_on(6); //auto-exits

   pc.setPosn(POS_STAND);
   // con, cha, int, wis.......
   for (i = 1; i<7; i++) {
      pc.short_cur_stats[i] = (d(1,8) + 10);
   }//for
   pc.HIT = 1; //7
   pc.DAM = 1;  // 8
   pc.AC = d(8,20) - 2;  //average of 80 I think
   pc.ATTACKS = 1;
   pc.PAUSE = 0;
   //pc.RACE = 1; //race, only human for now
   pc.HP = (15 + d(3,9) + pc.CON/2); // hp
   pc.MANA = (100 + pc.WIS/2);    // mana
   pc.MOV = (100 + pc.DEX/2);    // mov
   pc.ALIGN = 0; //align
   pc.LEVEL = 1; //level
   pc.setCurRoomNum(NEWBIE_ROOM); //starting room
   pc.PRACS = (pc.WIS / 3); //wis dependent, practices
   pc.setHP_MAX(pc.HP); //hp_max
   pc.MA_MAX = pc.MANA; // mana_max
   pc.MV_MAX = pc.MOV; // mov_max
   pc.CRITTER_TYPE = 0; // 0 is pc, 1 is smob, 2 is mob
   pc.DAM_GIV_MOD = 100; // > 100 = hit harder
   pc.DAM_REC_MOD = 100; // < 100 = get hit less hard
   pc.HUNGER = 25; //hunger
   pc.THIRST = 25; //thirst
   pc.DRUGGED = -2; //start out NOT drugged
   pc.VIS_BIT = 1024; // vis_bit ie not blind
   pc.SEE_BIT = 1024; // see_bit, ie not blind

   pc.crit_flags.turn_on(MOB_GOSSIP);
   pc.crit_flags.turn_on(MOB_YELL);
   pc.crit_flags.turn_on(MOB_GRATZ);
   pc.crit_flags.turn_on(MOB_AUC);
   pc.crit_flags.turn_on(MOB_SHOUT);
   pc.crit_flags.turn_on(MOB_SAY);
   pc.crit_flags.turn_on(MOB_TELL);

   pc.HEAT_RESIS = d(4,40);
   pc.COLD_RESIS = d(4,40); //cold resistance
   pc.ELEC_RESIS = d(4,40); //elect resistance
   pc.SPEL_RESIS = d(4,40); //general spell resistance
   pc.BH_DICE_COUNT = 2; // bare hand dice count
   pc.BH_DICE_SIDES = 3; // bare hand dice sides
   pc.WIMPY = 0; // wimpy
   pc.HP_REGEN = 100; // hp_regen%
   pc.MA_REGEN = 100; // mana_regen%
   pc.MV_REGEN = 100; // mov_regen%

   /***************  End of perm_stats construction. *****************/
   pc.GOLD = 5 * (pc.CHA * 250); //starting gold is charisma dependent :)
   pc.EXP = 1; 
   pc.BANK_GOLD = 0;
   pc.BIRTH_DAY = Day;
   pc.BIRTH_YEAR = Year;
   pc.RENT_DAY = Day;
   pc.RENT_YEAR = Year;

   switch (pc.RACE) {
      case HUMAN:
         break;
      case ANITRE:
	 pc.COLD_RESIS -= 50;
	 break;
      case DARKLING:
	 pc.SEE_BIT |= 1;  //see dark
	 pc.STR--;
	 pc.DEX += 2;
	 pc.CON--;
	 pc.CHA--;  //retribution for see dark
         break;
      case DWARF:
	 pc.STR++;
	 pc.DEX--;
	 pc.CON++;
	 pc.CHA--;
	 break;
      case ELF:
	 pc.STR -= 2;
	 pc.INT++;
	 pc.WIS++;
	 pc.CON--;
	 pc.DEX++;
	 break;
      case OGRUE:
	 pc.STR += 2;
	 pc.DEX -= 2;
	 pc.CON += 2;
	 pc.INT--;
	 pc.WIS--;
	 break;
      case SOMBRIAN:
	 break;
      default:
	 mudlog.log(ERR, "ERROR:  default called in race switch, login.cc\n");
	 break;
   }//switch	   	 


   //GAIN FIRST LEVEL SKILLS HERE, IS FOUNDATION FOR ALL ELSE//
   switch (pc.CLASS) {
      case WARRIOR:
	 pc.SKILLS_KNOWN.Insert(WEAPON_MASTERY_SKILL_NUM, 25);
	 pc.SKILLS_KNOWN.Insert(STRENGTH_CONDITIONING_SKILL_NUM, 25);
	 pc.STR++;
	 break;
      case SAGE:
	 pc.SKILLS_KNOWN.Insert(HERBALISM_SKILL_NUM, 25);
	 pc.SKILLS_KNOWN.Insert(PHILOSOPHY_SKILL_NUM, 25);
	 pc.WIS++;
	 break;
      case WIZARD:
	 pc.SKILLS_KNOWN.Insert(LITERACY_SKILL_NUM, 10);
	 pc.SKILLS_KNOWN.Insert(CHANNELLING_SKILL_NUM, 10);
	 pc.INT++;
	 break;
      case RANGER:
	 pc.SKILLS_KNOWN.Insert(FORESTRY_SKILL_NUM, 25);
	 pc.SKILLS_KNOWN.Insert(STRENGTH_CONDITIONING_SKILL_NUM, 10);
	 pc.CON++;
	 break;
      case THIEF:
	 pc.SKILLS_KNOWN.Insert(ACROBATICS_SKILL_NUM, 25);
	 pc.SKILLS_KNOWN.Insert(STRENGTH_CONDITIONING_SKILL_NUM, 10);
	 pc.DEX++;
	 break;
      case BARD:
	 pc.SKILLS_KNOWN.Insert(HONOR_CODE_SKILL_NUM, 25);
	 pc.SKILLS_KNOWN.Insert(STRENGTH_CONDITIONING_SKILL_NUM, 10);
	 pc.CHA++;
	 break;
      case CLERIC:
	 pc.SKILLS_KNOWN.Insert(PHILOSOPHY_SKILL_NUM, 25);
	 pc.SKILLS_KNOWN.Insert(PHYSIK_SKILL_NUM, 25);
	 pc.WIS++;
	 break;
      default:
	 mudlog.log(ERR, 
                    "ERROR:  In default of class modifiers in login.cc.\n");
         break;
   }//switch for class
	
   pc.CRIT_FLAGS.turn_on(18); //in_use

   String buf(100);

   Sprintf(buf, "   INFO: [NEW PLAYER]  %S connecting from: %S\n",
	   name_of_crit(pc, ~0), pc.getHostName());
   buf.setCharAt(0, 7); //beep
   buf.setCharAt(1, 7); //beep
   buf.setCharAt(2, 7); //beep
   show_all_info(buf);

   pc.pc->last_login_time = time(NULL);
   
   if (mudlog.ofLevel(DBG)) {
      mudlog << "New character:  "
             << *(name_of_crit(pc, ~0)) << " address:  "
             << &pc << endl;
   }

   // Give them some EQ to start with...
   
   // object 358 is the newbie bag
   object* obj_ptr = &(obj_list[NEWBIE_BAG_NUM]);
   if (obj_ptr->isInUse()) {
      if (!obj_ptr->isBulletinBoard()) {
         recursive_init_loads(*obj_ptr, 0);
      }
      else {
         obj_ptr->incrementCurInGame();
      }
      pc.gainInv(&(obj_list[NEWBIE_BAG_NUM]));
   }//if
   else {
      mudlog << "ERROR:  newbie bag is not IN USE, ie not built.\n";
   }


   String temp_str;
   emote("has entered the game.", pc, ROOM, TRUE);
   room_list[pc.getCurRoomNum()].gainCritter(&pc);
   pc.show("<ENGAGE_HEGEMON>\n");
   show("Welcome to the Game.\n\n", pc);
   look(1, &NULL_STRING, pc); //autolook
   show("\r\nIf your screen is 'stair-stepped', type:  toggle carriage\n",
	pc);
   return 0;
}//quit_do_login_new()
 


int  quit_do_login_old(critter& pc) {
   String temp_str(100);
      
   mudlog.log(TRC, "In quit_do_login_old\n");

   pc.pc->link_condition = CON_PLAYING;
   pc.pc->index = 0;
   pc.pc->mode = MODE_NORMAL;
   pc.VIS_BIT &= ~32;  //make sure not link_dead_invisible
   pc.pc->input = NULL_STRING;   

   
   if (pc.LEVEL < 5) {
      pc.setCurRoomNum(LOGIN_ROOM);
   }
   else if (!ROOM.isInUse()) {
      pc.setCurRoomNum(LOGIN_ROOM);
   }
   else if (BOOT_TIME > pc.pc->last_login_time) { 
      pc.setCurRoomNum(LOGIN_ROOM);
   }//if not logged in since last crash

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Logging in to room...  " 
             << pc.getCurRoomNum() << endl;
   }

   pc.pc->last_login_time = time(NULL);

   // Turn off, if they are using Hegemon, it will be automatically
   // turned back on.
   pc.setNoClient();

   //if (pc.USING_CLIENT) {
   //   show("\n</PRE>\n", pc);
   //}
   
   Sprintf(temp_str, "   INFO:  %S has connected from: %S\n",
	   name_of_crit(pc, ~0), pc.getHostName());
   show_all_info(temp_str);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Logging on existing character:  "
             << *(name_of_crit(pc, ~0)) << " address:  "
             << &pc << endl;
   }

   emote("has entered the game.", pc, ROOM, TRUE);
   ROOM.gainCritter(&pc);
   pc.show("<ENGAGE_HEGEMON>\n");

   if (pc.isUsingColor()) {
      pc.show(*(pc.getBackGroundColor()));
      pc.show(*(pc.getDefaultColor()));
   }

   update_skills(pc); //sync them up, does not necessarly add new ones

   show("Welcome back!!\n\n", pc);
   look(1, &NULL_STRING, pc); //autolook

   recursive_init_loads(pc); //update stuff in game, like IN_GAME count
   return 0;
}//quit_do_login_old()



