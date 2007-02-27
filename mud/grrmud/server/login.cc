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
#include "command5.h"
#include "load_wld.h"
#include <time.h>
#include <PtrArray.h>
#include <unistd.h>
#include "const.h"
#include "Filters.h"
#include "clients.h"
#include "telnet_handler.h"
#include "command3.h"


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
   show(get_page("./opening")); //opening page
   show(login_prompts[0]); 
   show(pc->p_handler->end_of_record());
   mudlog.log(TRC, "Done w/start_login.\n");
}//start_login


void critter::doLogin() {
#ifdef USEMYSQL
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query;
#endif

   String string;
   short eos, term_by_period;
   int j;
   String* string2;
   String name(50);
   String tmp_host(50);

   mudlog.log(TRC, "In do_login.\n");

   if (!pc) {
      mudlog.log(ERROR, "ERROR:  PC is NULL in do_login.\n");
      return;
   }//if

   if ((pc->index > 7) || (pc->index < 0)) {
      mudlog.log(ERROR, "ERROR:  bad index sent to do_login.\n");
      setMode(MODE_LOGOFF_NEWBIE_PLEASE);
   }//if
   else {
      // Login states
      // 0: name
      // 1: new-player password
      // 2: new-player password re-entry
      // 3: new-player gender
      // 4: new-player class
      // 5: existing-player password
      // 6: new-player race
      // 7: new-player language
      // default: this is an error condition
      switch (pc->index)
         {
         case 0:  /* get the name.. */
            string = pc->input.Get_Command(eos, term_by_period);

            //if someone logs in as "who", show them who's on and hang up on
            //them
            if (string == "who") {
               SEE_BIT = 1024;
               who(*this);
               setMode(MODE_LOGOFF_NEWBIE_PLEASE);
               break;
            }

            if (string == "__HEGEMON__") {
               using_client(*this);
               setClient(HEGEMON);
               show("Detected Hegemon Client\n");
               break;
            }
            else if ((strcasecmp(string, "quit") == 0) ||
                     (strcasecmp(string, "exit") == 0) ||
                     (string.Strlen() == 0)) {
               show("Bye!!\n\n");
               setMode(MODE_LOGOFF_NEWBIE_PLEASE);
               break;
            }

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
               switch (config.useMySQL) {
#ifdef USEMYSQL
                  case true:
                     query = "select * from Critters where MOB_NUM=0 and SHORT_DESC=";
                     query+=string;
                     if (mysql_real_query(database, query, strlen(query))!=0) {
                        if (mudlog.ofLevel(WRN)) {
                           mudlog << "In critter::doLogin():\n";
                           mudlog << "Error executing query: "
                                  << mysql_error(database) << endl;
                        }
                        return;
                     }
                     if ((result=mysql_store_result(database))==NULL) {
                        if (mudlog.ofLevel(WRN)) {
                           mudlog << "In critter::doLogin():\n";
                           mudlog << "Error retrieving query results: "
                                  << mysql_error(database) << endl;
                        }
                        return;
                     }
                     row=mysql_fetch_row(result);
                     if (row) pc->index = 5;
                     else {
                        if (is_newbie_banned(*(getHostName()))) {
                           show("Your site has been newbie-banned.\n");
                           setMode(MODE_LOGOFF_NEWBIE_PLEASE);
                        }
                        else pc->index = 7;
                     }
                  break;
#endif
                  case false:
                     string2 = new String(string);
                     names.append(string2);
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
                           pc->index = 7; //go choose language
                        }
                     }//else
                  break;
               }
            }//else            
            //         log("All done w/case 0.\n");
            break;
         case 1:  //password for new player

            string = pc->input.Get_Command(eos, term_by_period);

            if (string == "__HEGEMON__") {
               using_client(*this);
               setClient(HEGEMON);

               show("Detected Hegemon Client\n");
               break;
            }
            else if ((strcasecmp(string, "quit") == 0) ||
                     (strcasecmp(string, "exit") == 0) ||
                     (string.Strlen() == 0)) {
               show("Bye!!\n\n");
               setMode(MODE_LOGOFF_NEWBIE_PLEASE);
               break;
            }

            if (!isUsingClient())
               show(ANSI_ECHO_ON); //echo ON

            j = string.Strlen();
            if (j > 29) {
               show("\nYour password must be less than 30 characters.\n");
            }//if
            else if (j < 3) {
               show("\nYour password must be at least 3 characters.\n");
               mudlog.log(INF, "Logging off, password too short.\n");
               setMode(MODE_LOGOFF_NEWBIE_PLEASE);
            }//if
            else if (isnum(string)) {
               show("\nYour password must have at least one letter.\n");
            }
            else {
               pc->password = string;
               pc->index = 2; //go re-enter password
            }//else            
            break;
            
         case 2:  //recheck password for new player

            if (!isUsingClient()) {
               show(ANSI_ECHO_ON); //echo ON
            }

            string = pc->input.Get_Command(eos, term_by_period);

            pc->p_handler->set_echo(false);//stop echoing for the client

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
                   j == DWARF || j == OGRUE || j == ELF) {
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
//                  case 114:
//                     string = "sombri";
//                     break;
                  default:
                     return;
                  }//switch
                  help(1, &string, &NULL_STRING, *this);
               }//else
            }//if
            else {
               int slen = string.Strlen();
               if ((strncasecmp(string, "human", slen) == 0) ||
                   (strncasecmp(string, "anitre", slen) == 0) ||
                   (strncasecmp(string, "darkling", slen) == 0) ||
                   (strncasecmp(string, "dwarf", slen) == 0) ||
                   (strncasecmp(string, "ogrue", slen) == 0) ||
                   (strncasecmp(string, "elf", slen) == 0)) {
                  help(1, &string, &NULL_STRING, *this);
               }
               else {
                  show("Please enter a correct number.\n");
               }
            }
            break;
            
         case 4: { //get class
            string = pc->input.Get_Command(eos, term_by_period);
            j = 0;
            int slen = string.Strlen();
            if (slen == 0) {
               show("Please enter the number or name of the class you want.\n\n");
               break;
            }
            else if (isnum(string)) {
               j = atoi(string);
            }//if
            else if (strncasecmp(string, "warrior", slen) == 0) {
               j = WARRIOR;
            }
            else if (strncasecmp(string, "sage", slen) == 0) {
               j = SAGE;
            }
            else if (strncasecmp(string, "wizard", slen) == 0) {
               j = WIZARD;
            }
            else if (strncasecmp(string, "ranger", slen) == 0) {
               j = RANGER;
            }
            else if (strncasecmp(string, "thief", slen) == 0) {
               j = CLERIC;
            }
            else if (strncasecmp(string, "bard", slen) == 0) {
               j = BARD;
            }
            else if (strncasecmp(string, "necromancer", slen) == 0) {
               j = NECROMANCER;
            }
            else if (strncasecmp(string, "help", slen) == 0) {
               j = 0;
            }
            else {
               show("Enter a class name or the number representing it,\n or help if you need it.\n");
               break;
            }

            if (j == 0) {
               String classes("classes");
               help(1, &classes, &NULL_STRING, *this);
            }
            else {
               if (check_l_range(j, 1, 9, *this, FALSE) && j != 6) {
                  CLASS = j;
                  pc->index = 6; //go get race
               }//if
               else {
                  show("That is not a valid race.\n");
               }//else
            }//else
            break;
         }

         case 5: //password for non-new players.
            mudlog.log(DBG, "in case 5\n");

            if (TRUE) { //needs to be inside a block to shut g++ up!!

               string = pc->input.Get_Command(eos, term_by_period);

               if (string == "__HEGEMON__") {
                  using_client(*this);
                  setClient(HEGEMON);

                  show("Detected Hegemon Client\n");
                  break;
               }

               if (!isUsingClient())
                  show(ANSI_ECHO_ON); //echo ON

               ProtocolHandler* tmp_p_handler = pc->p_handler;
               int tmp_int = pc->descriptor;
               int tmp_index = pc->index;
               tmp_host = pc->host;
               int using_hegemon = isUsingClient();

               switch (config.useMySQL) {
#ifdef USEMYSQL
                  case true:
                     dbRead(0, atoi(row[CRITTBL_PC_NUMBER]), true);
                  break;
#endif
                  case false:
                     string2 = names.peekFront();
                     name = *(string2);
                     name.Tolower();
                     name.Prepend("./Pfiles/");
                     
                     ifstream rfile(name);
                     if (!rfile) {
                        if (mudlog.ofLevel(ERROR)) {
                           mudlog << "ERROR: rfile not opened, case 5 of login."
                                  << "\nHere is the bad filename -:"
                                  << name << ":-" << endl;
                        }
                        do_shutdown =  TRUE;
                        exit(2);
                     }//if
                     
                     fileRead(rfile, TRUE);

                  break;
               }

               // if it's the old version remove everything so we can force
               // correct nasties that ran away with us.
               if ( pc->file_format_version == 0 ) {
                  int i;
                  for(i=0;i<MAX_EQ;i++) {
                     if (EQ[i]) {
                        remove_eq_effects(*(EQ[i]), *this, FALSE, FALSE, i);
                     }
                  }
               }
               /* the following code is to correct for an issue we had with
                * runaway BHDC/BHDS. Eventually this code can go away.
                * Discovered some people got negatively impacted, hence
                * setting it regardless.
                */
               BH_DICE_COUNT = 2;
               BH_DICE_SIDES = 3;

               // re-gain effects from worn gear
                  {
                     int i;
                     for(i=0;i<MAX_EQ;i++) {
                        if (EQ[i]) {
                           wear_eq_effects(*(EQ[i]), *this, i, FALSE);
                        }
                     }
                  }

               setNoClient(); //turn off by default

               //This has to be restored before calling using_client() so we
               //don't allow protocol handlers to go runaway.

               pc->p_handler = tmp_p_handler;

               if (using_hegemon) {
                  using_client(*this);
                  setClient(HEGEMON);
               }
               
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
                                            names.peekFront(),
                                            ~0, *(getCurRoom()),
                                            TRUE);
                  if (!old_ptr) {
                     old_ptr = have_crit_named(pc_list, 2,
                                               names.peekFront(),
                                               ~0, *(getCurRoom()),
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
                     
                     //  set old player back in action 
                     // stick our current protocol handler into the old critter
                     // and let our protocol handler know that it has been moved.
                     ProtocolHandler* tmp_p_handler = old_ptr->pc->p_handler;
                     old_ptr->pc->p_handler = pc->p_handler;
                     old_ptr->pc->p_handler->newCritter(old_ptr);
                     pc->p_handler = tmp_p_handler;

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

                     if (isUsingClient()) {
                        using_client(*old_ptr);
                        old_ptr->setClient(whichClient());
                     }

                     old_ptr->pc->p_handler->set_echo(false);//stop echoing for the client


                     // I don't think this is needed. --Ben
                     //if (!old_ptr->isUsingClient()) {
                     //   old_ptr->show(CTAG_ENGAGE_CLIENT(HEGEMON)); // can we use crit.whichClient() here?
                     //}
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

                     doShowList(this, Selectors::instance().CC_gets_info_allow,
                                Selectors::instance().CC_none, pc_list,
                                CS_PLAYER_REGAINED_INFO,
                                getName(), getHostName());
                  }//if was link dead
                  else {
                     /* regular logging in */

                     pc->p_handler->set_echo(false);//stop echoing for the client

                     quit_do_login_old(*this); //join the game

                     // alert imms of possible multiplaying
                     Cell<critter*> cll(pc_list);
                     critter* ptr;
                     int show=false;
                     String mplayers(100);

                     mplayers.Append("INFO:  ");
                     mplayers.Append(*getName(~0));
                     mplayers.Append("[");
                     mplayers.Append(pc->host);
                     mplayers.Append("] is connected from the same IP as ");
                     while ((ptr = cll.next())) {
                        if (ptr!=this && ptr->getLinkCond()==1 &&
                            strcasecmp(ptr->pc->host, pc->host)==0) {
                           show=true;
                           mplayers.Append(*ptr->getName(~0));
                           mplayers.Append("[");
                           mplayers.Append(ptr->pc->host);
                           mplayers.Append("], ");
                        }//if
                     }//while
                     if (show) {
                        mplayers.dropFromEnd(2);
                        mplayers.Append(".\n");
                        pc_list.head(cll);
                        while((ptr = cll.next())) {
                           if (ptr->isImmort()) {
                              if (ptr->pc->imm_data->imm_level > 8) {
                                 ptr->show(mplayers);
                              }
                           }
                        }
                     }
                  }//else
               }//if
               else {
                  show("Password Incorrect...  Cya!\n");
                  if (mudlog.ofLevel(LSEC)) {
                     mudlog << "WARNING:  old player:  Logging off character:  " 
                            << (*getName()) << " from host:  " << pc->host
                            << " for incorrect password:  -: " << string
                            << ":-" << endl;
                  }

                  setMode(MODE_LOGOFF_NEWBIE_PLEASE);
               }//else
            }//if TRUE
            break;

         case 7: //Language choice for newbies.
            string = pc->input.Get_Command(eos, term_by_period);
            if (isnum(string)) {
               j = atoi(string);
               
               if ((j >= English) && (j < LastLanguage)) {
                  pc->preferred_language = (LanguageE)(j);
                  pc->index = 1; //go get the password.
                  PC_FLAGS.turn_on(27);
                  break;
               }
            }
            else if ((strcasecmp(string, "quit") == 0) ||
                     (strcasecmp(string, "exit") == 0)) {
               show("Bye!!\n\n");
               setMode(MODE_LOGOFF_NEWBIE_PLEASE);
               break;
            }
            show("Please pick a valid number.\n");
            break;
   
         default:
            mudlog.log(ERROR, "ERROR:  default called in do_login.\n");
            show("ERROR found, tell imp:  'default called in do_login'.\n");
            setMode(MODE_LOGOFF_NEWBIE_PLEASE);
            break;
         }//switch        
   }//else

   mudlog.log(TRC, "Done w/do_login.\n");

}//do_login


int  quit_do_login_new(critter& pc) {
   int i, j, pos;
   int positions[] = { 1, 2, 3, 4, 5, 6 };
   int points, points_left=30;
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
   for (i = 0; i<6; i++) {
      pos = d(1, 6) - 1;
      j = positions[i];
      positions[i] = positions[pos];
      positions[pos] = j;
   }
   // con, cha, int, wis.......
   for (i = 0; i<6; i++) {
      // You can still get high stats, but your stats will never be lower than
      // 10, and they will never add up to more than 90 -- Justin
      points = d(1,8);
      if (points > points_left) { points = points_left; points_left=0; }
      else points_left -= points;
      pc.short_cur_stats[positions[i]] = points+10;
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
   pc.setCurRoomNum(config.newbieRoom); //starting room
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
   pc.BIRTH_DAY = config.day;
   pc.BIRTH_YEAR = config.year;
   pc.RENT_DAY = config.day;
   pc.RENT_YEAR = config.year;

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
         mudlog.log(ERROR, "ERROR:  default called in race switch, login.cc\n");
         break;
   }//switch                    


   //GAIN FIRST LEVEL SKILLS HERE, IS FOUNDATION FOR ALL ELSE//
   switch (pc.CLASS) {
      case WARRIOR:
         pc.SKILLS_KNOWN.Insert(WEAPON_MASTERY_SKILL_NUM, 10);
         pc.SKILLS_KNOWN.Insert(PHYSICAL_ARTS_SKILL_NUM, 25);
         pc.STR++;
         break;
      case SAGE:
         pc.SKILLS_KNOWN.Insert(HERBALISM_SKILL_NUM, 25);
         pc.SKILLS_KNOWN.Insert(PHILOSOPHY_SKILL_NUM, 25);
         pc.SKILLS_KNOWN.Insert(BLACKSMITHING_SKILL_NUM, 25);
         pc.WIS++;
         break;
      case WIZARD:
         pc.SKILLS_KNOWN.Insert(LITERACY_SKILL_NUM, 10);
         pc.SKILLS_KNOWN.Insert(CHANNELLING_SKILL_NUM, 10);
         pc.INT++;
         break;
      case RANGER:
         pc.SKILLS_KNOWN.Insert(FORESTRY_SKILL_NUM, 25);
         pc.SKILLS_KNOWN.Insert(PHYSICAL_ARTS_SKILL_NUM, 10);
         pc.CON++;
         break;
      case THIEF:
         pc.SKILLS_KNOWN.Insert(ACROBATICS_SKILL_NUM, 25);
         pc.SKILLS_KNOWN.Insert(PHYSICAL_ARTS_SKILL_NUM, 10);
         pc.DEX++;
         break;
      case BARD:
         pc.SKILLS_KNOWN.Insert(HONOR_CODE_SKILL_NUM, 25);
         pc.SKILLS_KNOWN.Insert(PHYSICAL_ARTS_SKILL_NUM, 10);
         pc.CHA++;
         break;
      case CLERIC:
         pc.SKILLS_KNOWN.Insert(PHILOSOPHY_SKILL_NUM, 25);
         pc.SKILLS_KNOWN.Insert(PHYSIK_SKILL_NUM, 25);
         pc.WIS++;
         break;
      case NECROMANCER:
         pc.SKILLS_KNOWN.Insert(CURSING_SKILL_NUM, 25);
         pc.SKILLS_KNOWN.Insert(CONJURING_SKILL_NUM, 10);
         pc.SKILLS_KNOWN.Insert(NECROPHILIA_SKILL_NUM, 25);
         pc.STR-=2;
         pc.DEX-=2;
         pc.INT+=4;
         pc.WIS+=3;
         pc.CON-=3;
      default:
         mudlog.log(ERROR, 
                    "ERROR:  In default of class modifiers in login.cc.\n");
         break;
   }//switch for class
        
   pc.CRIT_FLAGS.turn_on(18); //in_use

   String buf(100);

   doShowList(&pc, Selectors::instance().CC_gets_info_allow,
              Selectors::instance().CC_using_client, pc_list,
              CS_NEW_PLAYER_INFO_NC, pc.getName(), pc.getHostName());
   
   doShowList(&pc, Selectors::instance().CC_gets_info_allow,
              Selectors::instance().CC_not_using_client, pc_list,
              CS_NEW_PLAYER_INFO_C, pc.getName(), pc.getHostName());

   // alert imms of possible multiplaying
   Cell<critter*> cll(pc_list);
   critter* ptr;
   int do_show=false;
   String mplayers(100);

   mplayers.Append("INFO:  ");
   mplayers.Append(*pc.getName(~0));
   mplayers.Append("[");
   mplayers.Append(pc.pc->host);
   mplayers.Append("] is connected from the same IP as ");
   while ((ptr = cll.next())) {
      if (ptr!=&pc && ptr->getLinkCond()==1 &&
          strcasecmp(ptr->pc->host, pc.pc->host)==0) {
         do_show=true;
         mplayers.Append(*ptr->getName(~0));
         mplayers.Append(", ");
      }
   }
   if (do_show) {
      mplayers.dropFromEnd(2);
      mplayers.Append(".");
      pc_list.head(cll);
      while((ptr = cll.next())) {
         if (ptr->isImmort()) {
            if (ptr->pc->imm_data->imm_level > 8) {
               ptr->show(mplayers);
            }
         }
      }
   }

   pc.pc->last_login_time = time(NULL);
   
   if (mudlog.ofLevel(DBG)) {
      mudlog << "New character:  "
             << *(name_of_crit(pc, ~0)) << " address:  "
             << &pc << endl;
   }

   // Give them some EQ to start with...
   
   // object 358 is the newbie bag
   object* obj_ptr = &(obj_list[config.newbieBagObject]);
   if (obj_ptr->isInUse()) {
      if (!obj_ptr->isBulletinBoard()) {
         recursive_init_loads(*obj_ptr, 0);
      }
      else {
         obj_ptr->incrementCurInGame();
      }
      pc.gainInv(&(obj_list[config.newbieBagObject]));
   }//if
   else {
      mudlog << "ERROR:  newbie bag is not IN USE, ie not built.\n";
   }


   String temp_str;
   emote("has entered the game.", pc, ROOM, TRUE);
   room_list[pc.getCurRoomNum()].gainCritter(&pc);
   if (!pc.isUsingClient()) {
      pc.show(CTAG_ENGAGE_CLIENT(HEGEMON));
   }
   else {
      pc.show("You're prompt is on by default, but as Hegemon displays\n"
            "bar graphs anyway, you may wish to change your prompt.  I suggest the\n"
            "command:  prompt %N\n");
   }
   show("Welcome to the Game.\n\n", pc);
   look(1, &NULL_STRING, pc); //autolook
   show("\r\nIf your screen is 'stair-stepped', type:  toggle carriage\n",
         pc);

   pc.save(); //make sure they have a Pfile

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
      pc.setCurRoomNum(config.loginRoom);
   }
   else if (!ROOM.isInUse()) {
      pc.setCurRoomNum(config.loginRoom);
   }
   else if (BOOT_TIME > pc.pc->last_login_time) { 
      pc.setCurRoomNum(config.loginRoom);
   }//if not logged in since last crash

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Logging in to room...  " 
             << pc.getCurRoomNum() << endl;
   }

   pc.pc->last_login_time = time(NULL);

   //if (pc.USING_CLIENT) {
   //   show("\n</PRE>\n", pc);
   //}
                 
   doShowList(&pc, Selectors::instance().CC_gets_info_allow,
              Selectors::instance().CC_none, pc_list,
              CS_PLAYER_HAS_CONNECTED_INFO,
              pc.getName(), pc.getHostName());

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Logging on existing character:  "
             << *(name_of_crit(pc, ~0)) << " address:  "
             << &pc << endl;
   }

   emote("has entered the game.", pc, ROOM, TRUE);
   ROOM.gainCritter(&pc);
   
   if (!pc.isUsingClient()) {
      pc.show(CTAG_ENGAGE_CLIENT(HEGEMON));
   }

   if (pc.isUsingColor()) {
      pc.show(*(pc.getBackGroundColor()));
      pc.show(*(pc.getDefaultColor()));
   }

   update_skills(pc); //sync them up, does not necessarly add new ones
   pc.PC_FLAGS.turn_off(33); //make sure they aren't marked as afk

   pc.show(get_page("./motd")); //message of the day

   pc.show("Welcome back!!\n\n");
   look(1, &NULL_STRING, pc); //autolook

   recursive_init_loads(pc); //update stuff in game, like IN_GAME count
   return 0;
}//quit_do_login_old()
