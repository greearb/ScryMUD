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

/********************  parse.cc  ***************/
/* this holds the calls to every function, if you wish to add another 
   function, you better add it in the switch statement.  If you wish to add
   help for this function, add it in the second switch towards the bottom 
   of the file. */

#include "ar_skll.h"
#include "cr_skll.h"
#include "social2.h"
#include "ez_skll.h"
#include "dam_skll.h"
#include "misc.h"
#include "misc2.h"
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include "command4.h"
#include "command5.h"
#include "spells.h"
#include "skills.h"
#include "olc.h"
#include "olc2.h"
#include "socials.h"
#include <fstream.h>
#include <stdio.h>
#include "classes.h"
#include "wep_skll.h"
#include "parse.h"
#include <PtrArray.h>
#include "script.h"


CmdCollection cmds_collection;

int CmdSpecifier::_cnt = 0;

String CmdSpecifier::toString() {
   String retval(100);
   Sprintf(retval,
           "CmdSpecifier:  word: %S  help_key: %S  len: %i CmdId: %i",
           &word, &hlp_key, len, (int)(id));
   return retval;
}


CmdCollection::CmdCollection() {
   for (int i = 0; i<CMD_ARRAY_LEN; i++) {
      cmd_specifiers[i] = NULL;
   }
}//constructor


void CmdCollection::addCmdSpecifier(const CmdSpecifier* cs) {
   addCmdSpecifierNoCreate(new CmdSpecifier(*cs));
}

void CmdCollection::addCmdSpecifierNoCreate(CmdSpecifier* cs) {
   int idx1 = calculateIndex(cs->getCmd().charAt(0));

   // Append to the end of the list
   CmdSpecifier* walker = cmd_specifiers[idx1];
   if (walker) {
      while (walker->getNext()) {
         walker = walker->getNext();
      }

      // when here, walker->next is null
      walker->setNext(cs);
      cs->setNext(NULL);
   }
   else {
      cmd_specifiers[idx1] = cs;
      cs->setNext(NULL);
   }
}

// can return NULL
const CmdSpecifier* CmdCollection::findSpecifierFor(const CmdSpecifier* cs) {
   int idx = calculateIndex(cs->getCmd().charAt(0));
   CmdSpecifier* ptr;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "CmdCollection::findSpecifierFor:  cs: " << cs->toString()
             << "\n calculated index: " << idx << endl;
   }

   for (ptr = cmd_specifiers[idx]; ptr != NULL; ptr = ptr->getNext()) {
      if (*ptr == *cs) {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "Comparing ptr: " << ptr->toString()
                   << "to cs, was EQUAL." << endl;
         }
         return ptr;
      }//if
      else {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "Comparing ptr: " << ptr->toString() 
                   << "to cs, was NOT EQUAL." << endl;
         }
      }
   }//for
   
   return NULL;
}

	     /******************************************/
int critter::processInput(String& input, short do_sub,
                          critter* c_script_owner, room* r_script_owner) {
   String raw_strings[RAW_MAX];
   String cooked_strs[COOKED_MAX];
   int cooked_ints[COOKED_MAX];
   String buf(100);
   int i, j, k, len1;
   short eos, term_by_period;

   for (i = 0; i<COOKED_MAX; i++) {
      cooked_ints[i] = 1;
   }

   if (mudlog.ofLevel(INF)) {
      mudlog << "INPUT:  player:  " << *(getName()) << "  input:  "
             << input << endl;
      if (pc) {
         mudlog << "PARSE:  Was PC, host:  " << pc->host << endl;
      }
   }

   /* any input causes you to un-meditate */
   if (POS == POS_MED) 
     POS = POS_REST;

   if (isMob()) { //not gonna parse for MOB's, but will for SMOBs btw
      mudlog.log(ERR, "ERROR:  MOB tried to process_input, parse.cc.\n");
      return -1;
   }//if

   if (PAUSE > 0) {
      buf = input.Look_Command();
      if (!((buf == "say") || (buf == "gossip") || (buf == "yell") ||
            (buf == "tell") || (buf == ":") || (buf == "/"))) {
         //show("DEBUG:  You are in pause mode.\n", pc); 
         if (!(pc && (getLinkCond() == CON_LINKDEAD)))
            return -1;
      }//if
   }//if

   if (input.Strlen() == 0) {
      return -1; //do nothing
   }//if
   else {
      if (pc) {
         if (isInPageBreak()) {
            releasePageBreak();
            input = "";
            return 0;
         }//if
         else {
            setDoPrompt(TRUE);
         }
      }//if
      else if (possessed_by) {
         if (possessed_by->isInPageBreak()) {
            possessed_by->releasePageBreak();
            input = "";
            return 0;
         }//if
         else {
            possessed_by->setDoPrompt(TRUE);
         }
      }
   }//else

			/* make sure it ends in newline */
   j = input.Strlen();
   if (input[j - 1] != '\n') {
      //log("WARNING:  input doesn't end in a newline, process_input.\n");
      input += "\n";
   }//if

	       /*  Test for various Modes. */

   if (pc) { //if its a pc
      pc->idle_ticks = 0;

      if ((MODE == MODE_QUIT_ME_PLEASE) ||
          (MODE == MODE_LOGOFF_NEWBIE_PLEASE)) {
         return 0;
      }//if quitting

      if (MODE == MODE_OLC) {
         String first_cmd = input.Look_Command(); 
         if (first_cmd == ":") {
            first_cmd = input.Get_Command(eos, term_by_period);
            first_cmd = input.Look_Command();
            if ((strcasecmp(first_cmd, "say") == 0) ||
                (strncasecmp(first_cmd, "gossip", 3) == 0) ||
                (strncasecmp(first_cmd, "tell", 3) == 0)) {
               show("Escaping OLC for this one command.\n");
            }//if
            else {
               show("You can only escape olc to:  say, gossip, or tell.\n");
               input = "";
               return -1;
            }
         }//if
         else {
            do_olc(*this); //read it in and process it
            pc->input = NULL_STRING;  //get rid of any junk still there
            if (MODE == MODE_OLC) { //still
               if (USING_CLIENT) {
                  Sprintf(buf, "<Phase = %i>", pc->imm_data->olc_counter);
                  show(buf);
               }
               show(olc_prompts[pc->imm_data->olc_counter]); //show prompt
            }//if
            return 0;
         }//else
      }//if olc
   
      if (MODE == MODE_LOGGING_IN) {
         doLogin();
         //log("Do login finished...\n");
         if (MODE != MODE_NORMAL) {
            show(login_prompts[pc->index]);
         }//if
         return 0;
      }//if logging in

      if (MODE == MODE_CH_DESC) {
         return do_ch_desc(*this);
      }//if

      if (MODE == MODE_ADD_MOB_SCRIPT) {
         return do_add_mob_script(*this);
      }

      if (MODE == MODE_ADD_ROOM_SCRIPT) {
         return do_add_room_script(*this);
      }

      if (MODE == MODE_WRITING_POST) {
         return do_post(*this);
      }//if

		     /* copy entry to last_input */
      if ((input.Look_Command() != "!") && do_sub) {
	pc->last_input = input.Get_Rest(FALSE); //get till newline
	pc->last_input += "\n";
      }//if
   }//if a pc

   raw_strings[0] = input.Get_Command(eos, term_by_period); 

   if (raw_strings[0].Strlen() == 0) {
      return -1;
   }//if

   if (pc) {
      if (raw_strings[0] == "!") {
         pc->input.Prepend(pc->last_input);
         return processInput(pc->input, do_sub);
      }//if
   }//if a pc

///****************************************************************///
///*************** non-standard-input-commands  *******************///
///****************************************************************///
#define str1 (cooked_strs[0])
#define str2 (cooked_strs[1])
#define str3 (cooked_strs[2])
#define str4 (cooked_strs[3])


   len1 = raw_strings[0].Strlen();

   if (strncasecmp(raw_strings[0], "tell", max(3, len1)) == 0) {
      buf = input.Get_Command(eos, term_by_period);
      if (buf.Strlen() != 0) {
         if (isnum(buf)) {
            i = atoi(buf);
            buf = input.Get_Command(eos, term_by_period);
            if (buf.Strlen() != 0) {
               str2 = buf;
               buf = input.Get_Rest();
               parse_communication(buf);
               return tell(i, &str2, buf, *this);
            }//if
            else {
               show(PARSE_ERR_MSG);
               return -1;
            }//else
         }//if is number
         else {
            str2 = buf;
            buf = input.Get_Rest();
            parse_communication(buf);
            return tell(1, &str2, buf, *this);
         }//else
      }//if
      else {
         show("Tell who what?\n");
         return -1;
      }//else
      return -1;
   }//if tell

   
   if (strncasecmp(raw_strings[0], "pecho", max(3, len1)) == 0) {
      buf = input.Get_Command(eos, term_by_period);
      if (buf.Strlen() != 0) {
         if (isnum(buf)) {
            i = atoi(buf);
            buf = input.Get_Command(eos, term_by_period);
            if (buf.Strlen() != 0) {
               str2 = buf;
               buf = input.Get_Rest();
               buf.Append("\n");
               parse_communication(buf);
               return com_pecho(i, &str2, &buf, *this);
            }//if
            else {
               show(PARSE_ERR_MSG);
               return -1;
            }//else
         }//if is number
         else {
            str2 = buf;
            buf = input.Get_Rest();
            parse_communication(buf);
            buf.Append("\n");
            return com_pecho(1, &str2, &buf, *this);
         }//else
      }//if
      else {
         show("Tell who what?\n");
         return -1;
      }//else
      return -1;
   }//if pecho
   
   if (strncasecmp(raw_strings[0], "reply", max(1, len1)) == 0) {
      if (pc) {
         buf = input.Get_Rest();
         parse_communication(buf);
         return tell(1, &(pc->rep_to), buf, *this);
      }//
   }//if


   if ((strcasecmp(raw_strings[0], "emote") == 0) ||
       (strcasecmp(raw_strings[0], ":") == 0)) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return com_emote(&buf, *this);
   }//if

   if (strcasecmp(raw_strings[0], "pemote") == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return com_pemote(&buf, *this);
   }//if
   
   if (strcasecmp(raw_strings[0], "gecho") == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return com_gecho(&buf, *this);
   }//if

   if (strcasecmp(raw_strings[0], "recho") == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return com_recho(&buf, *this);
   }//if

   if (strcasecmp(raw_strings[0], "zecho") == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return com_zecho(&buf, *this);
   }//if
   
   /* yell */
   if (strncasecmp(raw_strings[0], "yell", len1) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return yell(buf, *this);
   }//if yell

   if (strncasecmp(raw_strings[0], "shout", max(2, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return shout(buf, *this);
   }//if yell

   if ((strncasecmp(raw_strings[0], "gossip", max(3, len1)) == 0)
       || (raw_strings[0] == "/")) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return gossip(buf, *this);
   }//if gossip

   if ((strncasecmp(raw_strings[0], "gs", max(2, len1)) == 0)
       || (strncasecmp(raw_strings[0], "gt", max(2, len1)) == 0)) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return group_say(buf, *this);
   }//if gossip

   if ((strncasecmp(raw_strings[0], "wizchat", max(3, len1)) == 0)
       || (raw_strings[0] == "wc")) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return wizchat(buf, *this);
   }//if wizchat

   if (strncasecmp(raw_strings[0], "auction", max(3, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return auction(buf, *this);
   }//if auction

   if ((strncasecmp(raw_strings[0], "force", max(5, len1)) == 0) && len1<6) {
      buf = input.Get_Rest();
      buf += "\n";  //better fer processing!
      parse_communication(buf);
      return force(&buf, *this);
   }//if force

   if (strncasecmp(raw_strings[0], "force_all", max(9, len1)) == 0) {
      buf = input.Get_Rest();
      buf += "\n";  //better fer processing!
      parse_communication(buf);
      return force_all(&buf, *this);
   }//if force_all

   if (strncasecmp(raw_strings[0], "order", len1) == 0) {
      buf = input.Get_Rest();
      buf += "\n";  //better fer processing!
      parse_communication(buf);
      return order(&buf, *this);
   }//if order


   if (strcasecmp(raw_strings[0], "self") == 0) {
      buf = input.Get_Rest();
      buf += "\n";  //better fer processing!
      if (possessed_by) {
         return possessed_by->processInput(buf, do_sub);
      }
      else {
         show("Eh??");
         return -1;
      }
   }//if 'self'

   if ((strncasecmp(raw_strings[0], "say", max(3, len1)) == 0)
       || (raw_strings[0] == "\"")) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return say(buf, *this, *(getCurRoom()));
   }//if say

   if (strncasecmp(raw_strings[0], "title", max(3, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return title(&buf, *this); 
   }//if title

   if (strncasecmp(raw_strings[0], "poofout", max(5, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return poofout(&buf, *this); 
   }//if poofout

   if (strncasecmp(raw_strings[0], "poofin", max(5, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return poofin(&buf, *this); 
   }//if title

   if (strncasecmp(raw_strings[0], "idea", max(4, len1)) == 0) {
      buf = input.Get_Rest();
      return idea(buf, *this); 
   }//if idea

   if ((strncasecmp(raw_strings[0], "bug", max(3, len1)) == 0) ||
       (strncasecmp(raw_strings[0], "typo", max(4, len1)) == 0)) {
      buf = input.Get_Rest();
      return bug(buf, *this); 
   }//if bug


   mudlog.log(DBG, "Entering the while loop.\n");   
   int count = 1;
   while (!eos) { //if more than one command to get...
      raw_strings[count] = input.Get_Command(eos, term_by_period);
      if (term_by_period && (strcasecmp(raw_strings[count], "all") == 0)) {
         raw_strings[count] == "-1";
      }//if
      count++;
      
      //max input is RAW_MAX words/numbers
      if ((count >= (RAW_MAX - 1)) && (!eos)) {
	 show(PARSE_ERR_MSG);
	 return -1;
      }//if
   }//while
   //   log("Done w/while loop.\n");

   mudlog.log(DBG, "\n");

/* now separate the numbers from the words... */
/* example of command:  cast fireball 2.guard */
/*                      put 2.wand 3.bag      */

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Here are all commands\n";
      for (j = 0; j < RAW_MAX; j++) {
         mudlog << j << ":  " << raw_strings[j] << "  ";
         if (raw_strings[i].Strlen() == 0)
            break;
      }//for
      mudlog << endl << flush;
   }

   i = j = k = 0;
   for (buf = raw_strings[j]; buf.Strlen(); ) {
      if (j >= RAW_MAX) {
         mudlog << "ERROR:  over-ran RAW_MAX while parsing." << endl;
         break;
      }
      if (isnum(buf)) {
         cooked_ints[k] = atoi(buf);
         k++;
      }//if
      else { //it was a string of some type
         cooked_strs[i] = buf;
	 if (k == i) {
            cooked_ints[k] = 1;  //implicit 1
            k++;
	 }//if
         i++;
      }//else
      j++;
      buf = raw_strings[j];
   }//for

   if (mudlog.ofLevel(DBG)) {
      mudlog << "OK, made it to beginning of definitions.\n"
             << "Here are cooked_strs\n";
      for (i = 0; i< COOKED_MAX; i++) {
         mudlog << i << ":  " << cooked_strs[i] << " ";
      }//for
      mudlog << endl;
   
      mudlog << "Here are cooked_ints\n";
      for (i = 0; i< COOKED_MAX; i++) {
         mudlog << "  " << i << ":  " << cooked_ints[i] << " ";
      }//for
      mudlog << endl << flush;
   }//if

   /********************************************************/
   /***********  HERE START DEFINITIONS OF COMMANDS ********/
   /********************************************************/
   int is_dead = FALSE;

   if ((len1 = cooked_strs[0].Strlen()) == 0) {
      show(PARSE_ERR_MSG);
      return -1;
   }//if

   CmdSpecifier cur_cmd(str1, len1);
   const CmdSpecifier* real_cmd = cmds_collection.findSpecifierFor(&cur_cmd);

   if (real_cmd) {
      return exe_cmd_array[(int)(real_cmd->getId())]->
         execute(cooked_strs[0], cooked_strs[1], cooked_strs[2],
                 cooked_strs[3], cooked_strs[4],
                 cooked_ints[1], cooked_ints[2], cooked_ints[3],
                 cooked_ints[4], cooked_ints[5], cooked_ints[6],
                 is_dead, *this, c_script_owner, r_script_owner,
                 cooked_strs, cooked_ints, do_sub);
   }//if
   else {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "ERROR:  Could not find real_cmd for cmd:  "
                << cur_cmd.toString() << endl;
      }
      show(PARSE_ERR_MSG);
      return -1;
   }//switch

   return -1;
}// process input

#undef str1
#undef str2
#undef str3
#undef str4

const char* parse_hlp_command(const String& str1) {
   CmdSpecifier cur_cmd(str1, str1.Strlen());
   const CmdSpecifier* real_cmd = cmds_collection.findSpecifierFor(&cur_cmd);

   if (real_cmd) {
      return (const char*)(real_cmd->getHelpKey());
   }
   else if (mudlog.ofLevel(DBG)) {
      mudlog << "ERROR: (Help) Could not find real_cmd for cmd:  "
             << cur_cmd.toString() << endl;
   } 
   return NULL;
}//parse_hlp_command

