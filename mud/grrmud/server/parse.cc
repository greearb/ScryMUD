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


int WordLenPair::_cnt = 0;

CmdCollection::CmdCollection() {
   for (int i = 0; i<CMD_ARRAY_LEN; i++) {
      for (int j = 0; j<CMD_ARRAY_LEN; j++) {
         cmd_arrays[i][j] = NULL;
      }
   }
}//constructor


void CmdCollection::addCmdSpecifier(const CmdSpecifier* cs) {
   addCmdSpecifierNoCreate(new CmdSpecifier(cs));
}

void CmdCollection::addCmdSpecifierNoCreate(const CmdSpecifier* cs) {
   int idx1 = cs.getCmd().charAt(0) % CMD_ARRAY_LEN;

   cs.setNext(cmd_specifiers[idx1]);
   cmd_specifiers[idx1] = cs;
}

// can return NULL
const CmdSpecifier* CmdCollection::findSpecifierFor(const CmdSpecifier* cs) {
   int idx = cs.getCmd().charAt(0) % CMD_ARRAY_LEN;
   CmdSpecifier* ptr;

   for (ptr = cmd_specifiers[idx]; ptr != NULL; ptr = ptr->getNext()) {
      if (*ptr == cs) {
         return ptr;
      }//if
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
   int i, j, len1, k, l, m, n;
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
      return 0;
   }//if

   if (PAUSE > 0) {
      buf = input.Look_Command();
      if (!((buf == "say") || (buf == "gossip") || (buf == "yell") ||
            (buf == "tell") || (buf == ":") || (buf == "/"))) {
         //show("DEBUG:  You are in pause mode.\n", pc); 
         if (!(pc && (getLinkCond() == CON_LINKDEAD)))
            return 0;
      }//if
   }//if

   if (input.Strlen() == 0) {
      return 0; //do nothing
   }//if
   else {
      if (pc) {
         if (isInPageBreak()) {
            releasePageBreak();
            input = "";
            return 1;
         }//if
         else {
            setDoPrompt(TRUE);
         }
      }//if
      else if (possessed_by) {
         if (possessed_by->isInPageBreak()) {
            possessed_by->releasePageBreak();
            input = "";
            return 1;
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
         return 1;
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
               return 1;
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
            return 1;
         }//else
      }//if olc
   
      if (MODE == MODE_LOGGING_IN) {
         doLogin();
         //log("Do login finished...\n");
         if (MODE != MODE_NORMAL) {
            show(login_prompts[pc->index]);
         }//if
         return 1;
      }//if logging in

      if (MODE == MODE_CH_DESC) {
         do_ch_desc(*this);
         return 1;
      }//if

      if (MODE == MODE_ADD_MOB_SCRIPT) {
         do_add_mob_script(*this);
         return 1;
      }

      if (MODE == MODE_ADD_ROOM_SCRIPT) {
         do_add_room_script(*this);
         return 1;
      }

      if (MODE == MODE_WRITING_POST) {
         do_post(*this);
         return 1;
      }//if

		     /* copy entry to last_input */
      if ((input.Look_Command() != "!") && do_sub) {
	pc->last_input = input.Get_Rest(FALSE); //get till newline
	pc->last_input += "\n";
      }//if
   }//if a pc

   raw_strings[0] = input.Get_Command(eos, term_by_period); 

   if (raw_strings[0].Strlen() == 0) {
      return 0;
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
               tell(i, &str2, buf, *this);
            }//if
            else {
               show(PARSE_ERR_MSG);
               return FALSE;
            }//else
         }//if is number
         else {
            str2 = buf;
            buf = input.Get_Rest();
            parse_communication(buf);
            tell(1, &str2, buf, *this);
         }//else
      }//if
      else {
         show("Tell who what?\n");
         return 0;
      }//else
      return 1;
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
               com_pecho(i, &str2, &buf, *this);
            }//if
            else {
               show(PARSE_ERR_MSG);
               return FALSE;
            }//else
         }//if is number
         else {
            str2 = buf;
            buf = input.Get_Rest();
            parse_communication(buf);
            buf.Append("\n");
            com_pecho(1, &str2, &buf, *this);
         }//else
      }//if
      else {
         show("Tell who what?\n");
         return 0;
      }//else
      return 1;
   }//if pecho
   
   if (strncasecmp(raw_strings[0], "reply", max(1, len1)) == 0) {
      if (pc) {
         buf = input.Get_Rest();
         parse_communication(buf);
         tell(1, &(pc->rep_to), buf, *this);
         return 1;
      }//
   }//if


   if ((strcasecmp(raw_strings[0], "emote") == 0) ||
       (strcasecmp(raw_strings[0], ":") == 0)) {
      buf = input.Get_Rest();
      parse_communication(buf);
      com_emote(&buf, *this);
      return 1;
   }//if

   if (strcasecmp(raw_strings[0], "pemote") == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      com_pemote(&buf, *this);
      return 1;
   }//if
   
   if (strcasecmp(raw_strings[0], "gecho") == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      com_gecho(&buf, *this);
      return 1;
   }//if

   if (strcasecmp(raw_strings[0], "recho") == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      com_recho(&buf, *this);
      return 1;
   }//if

   if (strcasecmp(raw_strings[0], "zecho") == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      com_zecho(&buf, *this);
      return 1;
   }//if
   
   /* yell */
   if (strncasecmp(raw_strings[0], "yell", len1) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      yell(buf, *this);
      return 1;
   }//if yell

   if (strncasecmp(raw_strings[0], "shout", max(2, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      shout(buf, *this);
      return 1;
   }//if yell

   if ((strncasecmp(raw_strings[0], "gossip", max(3, len1)) == 0)
       || (raw_strings[0] == "/")) {
      buf = input.Get_Rest();
      parse_communication(buf);
      gossip(buf, *this);
      return 1;
   }//if gossip

   if ((strncasecmp(raw_strings[0], "gs", max(2, len1)) == 0)
       || (strncasecmp(raw_strings[0], "gt", max(2, len1)) == 0)) {
      buf = input.Get_Rest();
      parse_communication(buf);
      group_say(buf, *this);
      return 1;
   }//if gossip

   if ((strncasecmp(raw_strings[0], "wizchat", max(3, len1)) == 0)
       || (raw_strings[0] == "wc")) {
      buf = input.Get_Rest();
      parse_communication(buf);
      wizchat(buf, *this);
      return 1;
   }//if wizchat

   if (strncasecmp(raw_strings[0], "auction", max(3, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      auction(buf, *this);
      return 1;
   }//if auction

   if ((strncasecmp(raw_strings[0], "force", max(5, len1)) == 0) && len1<6) {
      buf = input.Get_Rest();
      buf += "\n";  //better fer processing!
      parse_communication(buf);
      force(&buf, *this);
      return 1;
   }//if force

   if (strncasecmp(raw_strings[0], "force_all", max(9, len1)) == 0) {
      buf = input.Get_Rest();
      buf += "\n";  //better fer processing!
      parse_communication(buf);
      force_all(&buf, *this);
      return 1;
   }//if force_all

   if (strncasecmp(raw_strings[0], "order", len1) == 0) {
      buf = input.Get_Rest();
      buf += "\n";  //better fer processing!
      parse_communication(buf);
      order(&buf, *this);
      return 1;
   }//if order


   if (strcasecmp(raw_strings[0], "self") == 0) {
      buf = input.Get_Rest();
      buf += "\n";  //better fer processing!
      if (possessed_by) {
         possessed_by->processInput(buf, do_sub);
      }
      else {
         show("Eh??");
      }
      return 1;
   }//if 'self'

   if ((strncasecmp(raw_strings[0], "say", max(3, len1)) == 0)
       || (raw_strings[0] == "\"")) {
      buf = input.Get_Rest();
      parse_communication(buf);
      say(buf, *this, *(getCurRoom()));
      return 1;
   }//if say

   if (strncasecmp(raw_strings[0], "title", max(3, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      title(&buf, *this); 
      return 1;
   }//if title

   if (strncasecmp(raw_strings[0], "poofout", max(5, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      poofout(&buf, *this); 
      return 1;
   }//if poofout

   if (strncasecmp(raw_strings[0], "poofin", max(5, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      poofin(&buf, *this); 
      return 1;
   }//if title

   if (strncasecmp(raw_strings[0], "idea", max(4, len1)) == 0) {
      buf = input.Get_Rest();
      idea(buf, *this); 
      return 1;
   }//if idea

   if ((strncasecmp(raw_strings[0], "bug", max(3, len1)) == 0) ||
       (strncasecmp(raw_strings[0], "typo", max(4, len1)) == 0)) {
      buf = input.Get_Rest();
      bug(buf, *this); 
      return 1;
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
	 return 0;
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
      return 0;
   }//if

   i = cooked_ints[1];
   j = cooked_ints[2];
   k = cooked_ints[3];
   l = cooked_ints[4];
   m = cooked_ints[5];
   n = cooked_ints[6];

   CmdSpecifier cur_cmd(str1, len1);
   CmdSpecifier* real_cmd = exe_cmds.findSpecifierFor(cur_cmd);

   if (real_cmd) {
      CmdId id = real_cmd.getId();

      TODO;
   }//switch
#undef str1
#undef str2
#undef str3
#undef str4

return 0;
}// process input


/*  Return the help command that the input string represents.  This
 * allows us to abbreviate help commands, and make multiple commands
 * bring up the same help page.
 */
const char* parse_hlp_command(const String& str1) {
   int len1 = str1.Strlen();
   if (!len1)
      return NULL;

   TODO;

}//parse_hlp_command
