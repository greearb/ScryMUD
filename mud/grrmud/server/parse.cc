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



	     /******************************************/
int critter::processInput(String& input, short do_sub, critter* script_owner) {
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

   switch (toupper(str1[0])) {
      case 'A':
	 if (strncasecmp(str1, "abilities", len1) == 0) { 
	    abilities((*this));
	 }//if
	 else if (strncasecmp(str1, "ack", len1) == 0) { 
	    ack(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "add_kname", len1) == 0) { 
	    add_kname(i, &str2, *this);
	 }//if
	 else if (strncasecmp(str1, "add_keyword", len1) == 0) { 
	    add_keyword(*this);
	 }//if
	 else if (strncasecmp(str1, "add_mname", len1) == 0) { 
	    add_mname(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "add_mob_script", len1) == 0) { 
            add_mob_script((*this), i, str2, j, str3, k, l);
         }
	 else if (strncasecmp(str1, "add_oname", len1) == 0) { 
	    add_oname(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "add_stat_affect", len1) == 0) { 
	    add_stat_affect(i, j, k, (*this));
	 }//if
	 else if (strncasecmp(str1, "add_casts_spell", len1) == 0) { 
	    add_casts_spell(i, j, k, (*this));
	 }//if
	 else if (strncasecmp(str1, "add_zone", len1) == 0) { 
	    addZone(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "adlist", len1) == 0) { 
	    adlist(*this);
	 }//if
	 else if (strncasecmp(str1, "afk", len1) == 0) { 
	    afk(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "agree", len1) == 0) { 
	    agree(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "add_perm_inv", len1) == 0) { 
	    add_perm_inv(i, &str2, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "assist", len1) == 0) { 
	    assist(i, &str2, *this);
	 }//if
	 else if (strncasecmp(str1, "asave", max(5, len1)) == 0) { 
	    write_zone(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "alist", len1) == 0) { 
	    zlist(i, j, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "amlist", len1) == 0) { 
	    amlist(i, j, (*this));
	 }//if
	 else if (strcasecmp(str1, "add_proc") == 0) { 
	    add_proc(i, (*this));
	 }//if
	 else if (strcasecmp(str1, "add_path") == 0) { 
            // i:  vehicle ID number, j:  i_th-path_cell
            // 
	    add_path(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "amsave", max(6, len1)) == 0) { 
	    amsave(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "aolist", len1) == 0) { 
	    aolist(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "aosave", max(6, len1)) == 0) { 
	    aosave(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "adsave", max(6, len1)) == 0) { 
	    adsave(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "arlist", len1) == 0) { 
	    arlist(i, j, (*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;

      case 'B':
	 if ((strcasecmp(str1, "bs") == 0) ||
	          (strncasecmp(str1, "backstab", len1) == 0)) { 
	    backstab(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "bash", len1) == 0) { 
	    bash(i, &cooked_strs[1], (*this));
	 }//if
	 else if (strncasecmp(str1, "beep", len1) == 0) { 
	    beep(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "beckon", len1) == 0) { 
	    beckon(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "berserk", len1) == 0) { 
	    berserk((*this));
	 }//if
	 else if (strncasecmp(str1, "blush", len1) == 0) { 
	    blush(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "bonk", len1) == 0) { 
	    bonk(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "bodyslam", len1) == 0) { 
	    body_slam(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "bounce", len1) == 0) { 
	    bounce(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "bow", len1) == 0) { 
	    bow(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "brew", len1) == 0) { 
	    concoct((*this));
	 }//if
	 else if (strncasecmp(str1, "burp", len1) == 0) { 
	    burp(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "butcher", len1) == 0) { 
	    butcher(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "buy", len1) == 0) { 
	    buy(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "brief", len1) == 0) { 
	    brief((*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'C':
	 if (strncasecmp(str1, "cast", len1) == 0) { 
	    cast(&str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "cackle", len1) == 0) { 
	    cackle(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "circle", len1) == 0) { 
	    circle(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "clap", len1) == 0) { 
	    clap(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "claw", len1) == 0) { 
	    claw(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "clear_keyword", len1) == 0) { 
	    clear_keyword(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "clear_mnames", len1) == 0) { 
	    clear_mnames(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "clear_onames", len1) == 0) { 
	    clear_onames(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "close", len1) == 0) { 
	    close(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "color", len1) == 0) { 
	    color(str2, str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "consider", len1) == 0) { 
	    consider(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "cheer", len1) == 0) { 
	    cheer(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "chuckle", len1) == 0) { 
	    chuckle(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "ch_mdesc", len1) == 0) { 
	    ch_mdesc(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "ch_msdesc", len1) == 0) { 
	    ch_msdesc(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "ch_mndesc", len1) == 0) { 
	    ch_mndesc(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "ch_odesc", len1) == 0) { 
	    ch_odesc(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "ch_ondesc", len1) == 0) { 
	    ch_ondesc(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "ch_osdesc", len1) == 0) { 
	    ch_osdesc(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "ch_path_desc", len1) == 0) { 
	    ch_path_desc(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "ch_rdesc", len1) == 0) { 
	    ch_rdesc((*this));
	 }//if
	 else if (strncasecmp(str1, "ch_kdesc", len1) == 0) { 
	    ch_kdesc(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "ch_ddesc", len1) == 0) { 
	    ch_ddesc(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "ch_rname", len1) == 0) { 
	    ch_rname(&str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "ch_zname", len1) == 0) { 
	    ch_zname(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "construct", len1) == 0) { 
	    construct((*this));
	 }//if
	 else if (strncasecmp(str1, "create_concoction", len1) == 0) { 
	    create_concoction(i, j, k, l, m, n, (*this));
	 }//if
	 else if (strncasecmp(str1, "create_construction", len1) == 0) { 
	    create_construction(i, j, k, l, m, n, (*this));
	 }//if
	 else if (strncasecmp(str1, "curse", len1) == 0) { 
	    curse(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "curtsey", len1) == 0) { 
	    curtsey(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'D':
	 if (strncasecmp(str1, "down", len1) == 0) { 
	    down((*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "date", len1) == 0) { 
	    date((*this));
	 }//if
	 else if (strncasecmp(str1, "dance", len1) == 0) { 
	    dance(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "delete", len1) == 0) { 
            suicide(&str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "delete_door", len1) == 0) { 
	    delete_door(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "discuss", len1) == 0) { 
	    discuss(&str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "disco", len1) == 0) { 
	    disco(j, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "disdain", len1) == 0) { 
	    disdain(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "disarm", len1) == 0) { 
	    disarm(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "donate", len1) == 0) { 
	    donate(i, &str2, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "drop", len1) == 0) { 
	    drop(i, &str2, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "drink", len1) == 0) {
	    drink(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "dset", len1) == 0) { 
	    dset(i, &str2, &str3, k, (*this));
	 }//if
	 else if (strncasecmp(str1, "door_to", len1) == 0) { 
	    door_to(i, j, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "dstat", len1) == 0) { 
	    dstat(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "dsys", len1) == 0) { 
	    dsys(*this);
	 }//if
	 else if (strncasecmp(str1, "dclear", len1) == 0) { 
	    dclear(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "dlist", len1) == 0) { 
	    dlist(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "dvnum", len1) == 0) { 
	    dvnum(i, &str2, (*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'E': case 'F':
	 if (strncasecmp(str1, "east", len1) == 0) { 
	    east((*this), (*(getCurRoom())));
	 }//if
	 else if (strcasecmp(str1, "eat") == 0) {
	    eat(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "equipment", len1) == 0) {
	    show_eq((*this));
	 }//if
	 else if (strncasecmp(str1, "empty", len1) == 0) {
	    empty(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "enslave", len1) == 0) {
	    enslave(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "exits", len1) == 0) {
	    exit((*this));
	 }//if
	 else if (strncasecmp(str1, "examine", len1) == 0) { 
	    examine(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "fill", len1) == 0) {
	    fill(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "follow", len1) == 0) {
	    follow(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "flee", len1) == 0) {
            int is_dead = FALSE;
	    flee(*this, is_dead);
	 }//if
	 else if (strncasecmp(str1, "flex", len1) == 0) {
	    flex(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "flip_door", len1) == 0) {
	    flip_door(i, &str2, (*this));
	 }//if
	 else if (strcasecmp(str1, "freeze") == 0) {
	    freeze(i, &str2, (*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 
	 return 1;
      case 'G': 
	 if (strncasecmp(str1, "gag", len1) == 0) { 
	    gag(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "get", len1) == 0) { 
	    get(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "giggle", len1) == 0) { 
	    giggle(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "give", len1) == 0) { 
	    give(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "grin", len1) == 0) { 
	    grin(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "group", len1) == 0) { 
	    group(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "growl", len1) == 0) { 
	    growl(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "grumble", len1) == 0) { 
	    grumble(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "grunt", len1) == 0) { 
	    grunt(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "guard", len1) == 0) { 
	    guard(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "go", len1) == 0) { 
            int is_dead = FALSE;
	    go(i, &str2, (*this), is_dead);
	 }//if
	 else if (strncasecmp(str1, "goto", len1) == 0) { 
	    _goto(i, &str2, (*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 
	 return 1;
      case 'H':
	 if (strncasecmp(str1, "hide", len1) == 0) { 
	    hide((*this));
	 }//if
	 else if (strncasecmp(str1, "hit", len1) == 0) { 
	    hit(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "help", len1) == 0) { 
	    help(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "hehe", len1) == 0) { 
	    hehe(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "hold", len1) == 0) { 
	    hold(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "hop", len1) == 0) { 
	    hop(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "hmm", len1) == 0) { 
	    hmm(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "hug", len1) == 0) { 
	    hug(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "hum", len1) == 0) { 
	    hum(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "hurl", len1) == 0) { 
	    hurl(i, &str2, (*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'I': case 'J': case 'K':
	 if (strncasecmp(str1, "inventory", len1) == 0) { 
	    inventory((*this));
	 }//if
	 else if (strncasecmp(str1, "insane", len1) == 0) { 
	    insane(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "itch", len1) == 0) { 
	    itch(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "itrans", len1) == 0) { 
	    itrans(i, &str2, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "junk", len1) == 0) { 
	    junk(i, &str2, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "kiss", len1) == 0) { 
	    kiss(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "kick", len1) == 0) { 
	    kick(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "kill", len1) == 0) { 
	    hit(i, &str2, (*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'L': 
	 if (strncasecmp(str1, "look", len1) == 0) { 
	    look(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "lag", len1) == 0) { 
	    lag(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "laugh", len1) == 0) { 
	    laugh(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "label:", 6) == 0) {
            if (mudlog.ofLevel(DBG)) {
               mudlog << "parse.cc:  Got label: call...ignoring." << endl;
            }
	 }//if
	 else if (strncasecmp(str1, "lick", len1) == 0) { 
	    lick(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "list", len1) == 0) { 
	    list_merchandise(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "list_paths", len1) == 0) { 
	    list_paths(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "list_scripts", len1) == 0) { 
	    list_scripts(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "list_site_bans", len1) == 0) { 
	    list_site_bans(*this);
	 }//if
	 else if (strncasecmp(str1, "listen", len1) == 0) { 
	    listen(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "lock", len1) == 0) { 
	    lock(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "lore", len1) == 0) { 
	    lore(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "log_level", len1) == 0) { 
	    log_level(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "ldwho", len1) == 0) { 
	    ldwho((*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'M':
	 if (strncasecmp(str1, "make_builder", len1) == 0) { 
	    make_builder(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "meditate", len1) == 0) { 
	    meditate((*this));
	 }//if
	 else if (strncasecmp(str1, "mstat", len1) == 0) { 
	    mstat(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "mclone", len1) == 0) { 
	    mclone(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "mclear", max(6, len1)) == 0) { 
	    mclear(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "mreload", len1) == 0) { 
	    mreload((*this));
	 }//if
	 else if (strncasecmp(str1, "mlist", len1) == 0) { 
	    mlist(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "mload", len1) == 0) { 
	    mload(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "mset", len1) == 0) { 
	    mset(i, &str2, &str3, k, &str4, (*this));
	 }//if
	 else if (strncasecmp(str1, "muahaha", len1) == 0) { 
	    muahaha(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "murder", len1) == 0) { 
	    hit(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "mvnum", len1) == 0) { 
	    mvnum(i, &str2, (*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'N': case 'O':
	 if (strncasecmp(str1, "north", len1) == 0) { 
	    north((*this), (*(getCurRoom())));
	 }//if
	 else if ((strncasecmp(str1, "northeast", len1) == 0) || 
		  (strncasecmp(str1, "ne", len1) == 0)) {
	    northeast((*this), (*(getCurRoom())));
	 }//if
	 else if ((strncasecmp(str1, "northwest", len1) == 0) || 
		  (strncasecmp(str1, "nw", len1) == 0)) {
	    northwest((*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "nogossip", len1) == 0) { 
	    nogossip((*this));
	 }//if
	 else if (strncasecmp(str1, "nod", len1) == 0) { 
	    nod(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "offer", len1) == 0) { 
	    offer(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "open", len1) == 0) { 
	    open(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "opurge", len1) == 0) { 
	    opurge(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "ostat", len1) == 0) { 
	    ostat(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "oclone", len1) == 0) { 
	    oclone(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "oclear", max(6, len1)) == 0) { 
	    oclear(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "olist", len1) == 0) { 
	    olist(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "oload", len1) == 0) { 
	    oload(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "oreload", len1) == 0) { 
	    oreload((*this));
	 }//if
	 else if (strncasecmp(str1, "oset", len1) == 0) { 
	    oset(i, &str2, &str3, k, &str4, (*this));
	 }//if
	 else if (strncasecmp(str1, "ovnum", len1) == 0) { 
	    ovnum(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "olc", len1) == 0) {
	    if (!pc->imm_data) 
	       show("You are not cleared to build, talk to a god.\n");
	    else if (pc->imm_data->olc_counter > 0)
	       restart_olc((*this));
	    else {
	       start_olc((*this));
	       processInput(pc->input, do_sub);
	    }//else
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'P': 
	 if (strncasecmp(str1, "pat", len1) == 0) { 
	    pat(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "passwd", len1) == 0) { 
	    passwd(&str2, &str3, &str4, (*this));
	 }//if
	 else if (strncasecmp(str1, "page_break", len1) == 0) { 
	    page_break(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "practice", len1) == 0) { 
	    practice(&str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "pay", len1) == 0) { 
	    pay(i, j, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "pause", len1) == 0) { 
	    pause(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "pinch", len1) == 0) { 
	    pinch(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "possess", len1) == 0) { 
	    possess(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "prompt", len1) == 0) { 
	    prompt(&str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "pft", len1) == 0) { 
	    pft(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "picklock", len1) == 0) { 
	    picklock(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "poke", len1) == 0) { 
	    poke(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "ponder", len1) == 0) { 
	    ponder(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "post", len1) == 0) { 
	    post(&str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "prone", len1) == 0) { 
	    prone((*this));
	 }//if
	 else if (strncasecmp(str1, "puke", len1) == 0) { 
	    puke(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "put", len1) == 0) { 
	    put(i, &str2, j, &str3, (*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'Q': case 'R':
	 if (strncasecmp(str1, "quaf", len1) == 0) { 
	    quaf(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "quit", len1) == 0) { 
            mudlog << "Logging off player in parse.cc,"
                   << " name:  " << *(name_of_crit((*this), ~0)) 
                   << "  address:  " << &(*this) << endl;
	    log_out((*this));
	 }//if
	 else if (strncasecmp(str1, "reset_olc", len1) == 0) { 
	    reset_olc((*this));
	 }//if
	 else if (strncasecmp(str1, "remove", len1) == 0) { 
	    remove(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "rem_mname", len1) == 0) { 
	    rem_mname(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "rem_oname", len1) == 0) { 
	    rem_oname(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "rem_zone", len1) == 0) { 
	    remZone(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "rest", len1) == 0) { 
	    rest((*this));
	 }//if
	 else if (strncasecmp(str1, "recite", len1) == 0) { 
	    recite(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "read", len1) == 0) { 
	    read(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "rem_path", len1) == 0) { 
	    rem_path(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "rem_keyword", len1) == 0) { 
	    rm_keyword(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "rm_keyword", len1) == 0) { 
	    rm_keyword(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "rem_perm_inv", len1) == 0) { 
	    rem_perm_inv(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "rescue", len1) == 0) { 
	    rescue(i, &str2, (*this));
	 }//if
	 else if (strcasecmp(str1, "rezone") == 0) { 
	    rezone((*this));
	 }//if
	 else if (strcasecmp(str1, "replace_door") == 0) { 
	    replace_door(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "rofl", len1) == 0) { 
	    rofl(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "rinit", len1) == 0) { 
	    rinit(i, j, *this);
	 }//if
	 else if (strncasecmp(str1, "roll", len1) == 0) { 
            if ((i != 1) || (j != 1))
               //mimic dice roll
               roll(i, j, (*this));
            else
               //social
               roll(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "rstat", len1) == 0) { 
	    rstat(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "rset", len1) == 0) { 
	    rset(&str2, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "rm_give_proc", len1) == 0) { 
	    rm_give_proc(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "rm_discuss_proc", len1) == 0) { 
	    rm_discuss_proc(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "rm_bow_proc", len1) == 0) { 
	    rm_bow_proc(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "rm_curse_proc", len1) == 0) { 
	    rm_curse_proc(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "rm_script", len1) == 0) { 
	    rem_script(i, str2, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "rm_stat_affect", len1) == 0) { 
	    rm_stat_affect(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "rm_casts_spell", len1) == 0) { 
	    rm_casts_spell(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "rclone", len1) == 0) { 
	    rclone(i, &str2, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "rlist", len1) == 0) { 
	    rlist(i, j, (*this));
	 }//if
	 else if (strcasecmp(str1, "rclear") == 0) { 
	    rclear(i, (*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'S': 
	 if (strncasecmp(str1, "south", len1) == 0) { 
	    south((*this), (*(getCurRoom())));
	 }//if
	 else if ((strncasecmp(str1, "southeast", len1) == 0) || 
		  (strncasecmp(str1, "se", len1) == 0)) {
	    southeast((*this), (*(getCurRoom())));
	 }//if
	 else if ((strncasecmp(str1, "southwest", len1) == 0) || 
		  (strncasecmp(str1, "sw", len1) == 0)) {
	    southwest((*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "save", len1) == 0) { 
	    ::save(*this);
	 }//if
	 else if (strncasecmp(str1, "scan", len1) == 0) { 
	    scan(*this);
	 }//if
	 else if (strncasecmp(str1, "set_path_dir", len1) == 0) { 
	    set_path_dir(i, j, k, &str2, *this);
	 }//if
	 else if (strncasecmp(str1, "set_veh_stop", len1) == 0) { 
	    set_veh_stop(i, j, &str2, *this);
	 }//if
	 else if (strncasecmp(str1, "set_path_pointer", len1) == 0) { 
	    set_path_pointer(i, j, *this);
	 }//if
	 else if (strncasecmp(str1, "save_mob", len1) == 0) { 
	    save_mob(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "save_obj", len1) == 0) { 
	    save_obj(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "score", len1) == 0) { 
	    score(&str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "scold", len1) == 0) { 
	    scold(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "script_jump_true", 17) == 0) { 
            if (script_owner) {
               script_jump_true(cooked_strs, cooked_ints, *this, *script_owner);
            }//if
            else {
               mudlog << "ERROR:  script_owner NULL when calling script_jump_true"
                      << endl;
            }
	 }//if
	 else if (strncasecmp(str1, "script_jump_false", 17) == 0) { 
            if (script_owner) {
               script_jump_false(cooked_strs, cooked_ints, *this, *script_owner);
            }//if
            else {
               mudlog << "ERROR:  script_owner NULL when calling script_jump_false"
                      << endl;
            }
	 }//if
	 else if (strncasecmp(str1, "sac", len1) == 0) { 
    	    junk(i, &str2, &str3, (*this));
	 }//if 
	 else if (strncasecmp(str1, "scream", len1) == 0) { 
	    scream(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "scratch", len1) == 0) { 
	    scratch(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "scribe", len1) == 0) { 
	    scribe(&str2, (*this));
	 }//if 
	 else if (strncasecmp(str1, "see_all", len1) == 0) { 
	    see_all((*this));
	 }//if 
	 else if (strncasecmp(str1, "shield", len1) == 0) { 
	    shield(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "shrug", len1) == 0) { 
	    shrug(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "shake", len1) == 0) { 
	    shake(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "shoot", len1) == 0) { 
	    shoot(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "show_zones", len1) == 0) { 
	    showZones(*this);
	 }//if
	 else if (strncasecmp(str1, "sigh", len1) == 0) { 
	    sigh(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "silly", len1) == 0) { 
	    silly(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "silent_junk", len1) == 0) { 
	    silent_junk(i, &str2, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "sit", len1) == 0) { 
	    sit((*this));
	 }//if
	 else if (strncasecmp(str1, "siteban", len1) == 0) { 
	    siteban(&str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "sell", len1) == 0) { 
	    sell(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "skin", len1) == 0) { 
	    skin(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "sleep", len1) == 0) { 
	    sleep((*this));
	 }//if
	 else if (strncasecmp(str1, "slay", len1) == 0) { 
	    slay(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "slist", len1) == 0) { 
	    slist(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "smirk", len1) == 0) { 
	    smirk(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "snicker", len1) == 0) { 
	    snicker(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "sneak", len1) == 0) { 
	    sneak((*this));
	 }//if
	 else if (strncasecmp(str1, "snoop", len1) == 0) { 
	    snoop(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "sob", len1) == 0) { 
	    sob(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "stat_script", len1) == 0) { 
	    stat_script(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "splash", len1) == 0) { 
	    splash(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "stand", len1) == 0) { 
	    stand((*this));
	 }//if
	 else if (strncasecmp(str1, "stat_keyword", len1) == 0) { 
	    stat_keyword(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "stat_path", len1) == 0) { 
	    stat_path(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "steal", len1) == 0) { 
	    steal(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "sockets", len1) == 0) { 
	    sockets((*this));
	 }//if
	 else if (strcasecmp(str1, "suicide") == 0) { 
	    suicide(&str2, (*this));
	 }//if
	 else if (strcasecmp(str1, "shutdown") == 0) { 
	    shutdown(&str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "slap", len1) == 0) { 
	    slap(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strcasecmp(str1, "smile") == 0) { 
	    smile(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "strut", len1) == 0) { 
	    strut(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
     case 'T':
	 if (strncasecmp(str1, "take", len1) == 0) { 
	    get(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "tail", len1) == 0) { 
	    tail(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "teach", len1) == 0) { 
	    teach(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "throw", len1) == 0) { 
	    _throw(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "thank", len1) == 0) { 
	    thank(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "time", len1) == 0) { 
	    time((*this));
	 }//if
	 else if (strncasecmp(str1, "track", len1) == 0) { 
	    track(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "twiddle", len1) == 0) { 
	    twiddle(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "toggle", len1) == 0) { 
	    toggle_prompt(&str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "tog_veh_stop", len1) == 0) { 
	    tog_veh_stop(i, j, (*this));
	 }//if
	 else if (strncasecmp(str1, "total_rezone", len1) == 0) { 
	    total_rezone((*this));
	 }//if
	 else if (strncasecmp(str1, "tog_mflag", len1) == 0) { 
	    tog_mflag(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "tog_oflag", len1) == 0) { 
	    tog_oflag(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "tog_rflag", len1) == 0) { 
	    tog_rflag(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "tog_zflag", len1) == 0) { 
	    tog_zflag(i, (*this));
	 }//if
	 else if (strcasecmp(str1, "thaw") == 0) { 
	    thaw(i, &str2, (*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'U': case 'V':
	 if (strncasecmp(str1, "up", len1) == 0) { 
	    up((*this), (*(getCurRoom()))); 
	 }//if
	 else if (strncasecmp(str1, "unlock", len1) == 0) { 
	    unlock(i, &str2, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "unsiteban", len1) == 0) { 
	    unsiteban(&str2, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "unpost", len1) == 0) { 
	    unpost(i, &str2, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "ungag", len1) == 0) { 
	    ungag(i, &str2, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "ungroup", len1) == 0) { 
	    doUngroup(i, &str2); 
	 }//if
	 else if (strncasecmp(str1, "unsnoop", len1) == 0) { 
	    unsnoop(*this); 
	 }//if
	 else if (strncasecmp(str1, "uptime", len1) == 0) { 
	    uptime(*this); 
	 }//if
	 else if (strncasecmp(str1, "update_cur_in_game", len1) == 0) { 
	    update_cur_in_game(*this); 
	 }//if
	 else if (strncasecmp(str1, "update_cig", len1) == 0) { 
	    update_cur_in_game(*this); 
	 }//if
	 else if (strncasecmp(str1, "use", len1) == 0) { 
	    use(i, &str2, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "using_client", len1) == 0) { 
	    using_client((*this)); 
	 }//if
	 else if (strncasecmp(str1, "visible", len1) == 0) { 
	    visible((*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'W':
	 if (strncasecmp(str1, "west", len1) == 0) { 
	    west((*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "wear", len1) == 0) { 
	    wear(i, &str2, j, &str3, (*this));
	 }//if
	 else if (strncasecmp(str1, "weep", len1) == 0) { 
	    weep(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "wake", len1) == 0) { 
	    wake((*this));
	 }//if
	 else if (strncasecmp(str1, "wave", len1) == 0) { 
	    wave(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "wink", len1) == 0) { 
	    wink(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "wiggle", len1) == 0) { 
	    wiggle(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "whistle", len1) == 0) { 
	    whistle(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "where", len1) == 0) { 
	    where(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "who", len1) == 0) { 
	    who((*this));
	 }//if
	 else if (strncasecmp(str1, "wield", len1) == 0) { 
	    wield(i, &str2, (*this));
	 }//if
	 else if (strncasecmp(str1, "wibble", len1) == 0) { 
	    wibble(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "wimpy", len1) == 0) { 
	    wimpy(i, (*this));
	 }//if
	 else if (strncasecmp(str1, "wizinvis", len1) == 0) { 
	    wizinvis((*this));
	 }//if
	 else if (strcasecmp(str1, "write_world") == 0) { 
	    write_world((*this));
	 }//if
	 else {
	    show(PARSE_ERR_MSG);
	    return 0;
	 }//else
	 return 1;
      case 'X': case 'Y': case 'Z':
	 if (strncasecmp(str1, "yawn", len1) == 0) { 
	    yawn(i, &str2, (*this), (*(getCurRoom())));
	 }//if
	 else if (strncasecmp(str1, "zlock", len1) == 0) { 
	    zlock(i, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "zlist", len1) == 0) { 
	    zlist(i, j, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "zcreate", len1) == 0) { 
	    zcreate(i, &str2, j, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "zenable", len1) == 0) { 
	    zenable(i, &str2, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "zdisable", len1) == 0) { 
	    zdisable(i, &str2, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "zgoto", len1) == 0) { 
	    zgoto(i, (*this)); 
	 }//if
	 else if (strncasecmp(str1, "zunlock", len1) == 0) { 
	    zunlock(i, (*this)); 
	 }//if
         else {
   	    show(PARSE_ERR_MSG);
            return 0;
         }//else
	 return 1;
      default:   
	 show("Do what??\n");
	 return 0;
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

   switch (toupper(str1[0])) {
      case 'A':
	 if (strncasecmp(str1, "abilities", len1) == 0) { 
	    return "abilities";
	 }//if
	 else if (strncasecmp(str1, "add_oname", len1) == 0) { 
	    return "add_oname";
	 }//if
	 else if (strncasecmp(str1, "add_kname", len1) == 0) { 
	    return "add_kname";
	 }//if
	 else if (strncasecmp(str1, "add_keyword", len1) == 0) { 
	    return "add_keyword";
	 }//if
	 else if (strncasecmp(str1, "add_mname", len1) == 0) { 
	    return "add_mname";
	 }//if
	 else if (strncasecmp(str1, "add_mob_script", len1) == 0) { 
            return "add_mob_script";
         }
	 else if (strncasecmp(str1, "add_stat_affect", len1) == 0) { 
            return "add_stat_affect";
         }
	 else if (strncasecmp(str1, "add_casts_spell", len1) == 0) { 
            return "add_casts_spell";
         }
	 else if (strncasecmp(str1, "add_zone", len1) == 0) { 
            return "active_zones";
         }
	 else if (strncasecmp(str1, "adlist", len1) == 0) { 
            return "adlist";
         }
	 else if (strncasecmp(str1, "assist", len1) == 0) { 
	    return "assist";
	 }//if
	 else if (strncasecmp(str1, "asave", len1) == 0) { 
	    return "write_zone";
	 }//if
	 else if (strncasecmp(str1, "adsave", len1) == 0) { 
	    return "adsave";
	 }//if
	 else if (strncasecmp(str1, "add_perm_inv", len1) == 0) { 
	    return "add_perm_inv";
	 }//if
	 else if (strncasecmp(str1, "amlist", len1) == 0) { 
	    return "amlist";
	 }//if
	 else if (strncasecmp(str1, "add_proc", len1) == 0) { 
	    return "add_proc";
	 }//if
	 else if (strncasecmp(str1, "add_path", len1) == 0) { 
	    return "add_path";
	 }//if
	 else if (strncasecmp(str1, "amsave", len1) == 0) { 
	    return "amsave";
	 }//if
	 else if (strncasecmp(str1, "aolist", len1) == 0) { 
	    return "aolist";
	 }//if
	 else if (strncasecmp(str1, "aosave", len1) == 0) { 
	    return "aosave";
	 }//if
	 else if (strncasecmp(str1, "arlist", len1) == 0) { 
	    return "arlist";
	 }//if
	 else if (strncasecmp(str1, "anitre", len1) == 0) { 
	    return "anitre";
	 }//if
	 else if (strncasecmp(str1, "avintre", len1) == 0) { 
	    return "avintre";
	 }//if
	 return NULL;
      case 'B':
	 if ((strcasecmp(str1, "bs") == 0) ||
	          (strncasecmp(str1, "backstab", len1) == 0)) { 
	    return "backstab";
	 }//if
	 else if (strncasecmp(str1, "bash", len1) == 0) { 
	    return "bash";
	 }//if
	 else if (strncasecmp(str1, "beep", len1) == 0) { 
	    return "beep";
	 }//if
	 else if (strncasecmp(str1, "brew", len1) == 0) { 
	    return "concoct";
	 }//if
	 else if (strncasecmp(str1, "butcher", len1) == 0) { 
	    return "butcher";
	 }//if
	 else if (strncasecmp(str1, "buy", len1) == 0) { 
	    return "buy";
	 }//if
	 else if (strncasecmp(str1, "bodyslam", len1) == 0) { 
	    return "bodyslam";
	 }//if
	 else if (strncasecmp(str1, "bow", len1) == 0) { 
	    return "bow";
	 }//if
	 else if (strncasecmp(str1, "bug", len1) == 0) {
	    return "idea";
	 }//if
	 else if (strncasecmp(str1, "brief", len1) == 0) { 
	    return "brief";
	 }//if
         return NULL;
      case 'C': 
	 if (strncasecmp(str1, "cast", len1) == 0) { 
	    return "cast";
	 }//if
	 else if (strncasecmp(str1, "cheer", len1) == 0) { 
	    return "cheer";
	 }//if
	 else if (strncasecmp(str1, "channels", len1) == 0) { 
	    return "channels";
	 }//if
	 else if (strncasecmp(str1, "ch_rdesc", len1) == 0) { 
	    return "ch_rdesc";
	 }//if
	 else if (strncasecmp(str1, "ch_kdesc", len1) == 0) { 
	    return "ch_kdesc";
	 }//if
	 else if (strncasecmp(str1, "ch_odesc", len1) == 0) { 
	    return "ch_odesc";
	 }//if
	 else if (strncasecmp(str1, "ch_mdesc", len1) == 0) { 
	    return "ch_mdesc";
	 }//if
	 else if (strncasecmp(str1, "ch_mndesc", len1) == 0) { 
	    return "ch_mndesc";
	 }//if
	 else if (strncasecmp(str1, "ch_msdesc", len1) == 0) { 
	    return "ch_msdesc";
	 }//if
	 else if (strncasecmp(str1, "ch_ondesc", len1) == 0) { 
	    return "ch_ondesc";
	 }//if
	 else if (strncasecmp(str1, "ch_osdesc", len1) == 0) { 
	    return "ch_osdesc";
	 }//if
	 else if (strncasecmp(str1, "ch_path_desc", len1) == 0) { 
	    return "ch_path_desc";
	 }//if
	 else if (strncasecmp(str1, "ch_ddesc", len1) == 0) { 
	    return "ch_ddesc";
	 }//if
	 else if (strncasecmp(str1, "ch_rname", len1) == 0) { 
	    return "ch_rname";
	 }//if
	 else if (strncasecmp(str1, "ch_zname", len1) == 0) { 
	    return "ch_zname";
	 }//if
	 else if (strncasecmp(str1, "circle", len1) == 0) { 
	    return "circle";
	 }//if
	 else if (strncasecmp(str1, "classes", len1) == 0) { 
	    return "classes";
	 }//if
	 else if (strncasecmp(str1, "claw", len1) == 0) { 
	    return "claw";
	 }//if
	 else if (strncasecmp(str1, "clear_keyword", len1) == 0) { 
	    return "clear_keyword";
	 }//if
	 else if (strncasecmp(str1, "clear_mnames", len1) == 0) { 
	    return "clear_mnames";
	 }//if
	 else if (strncasecmp(str1, "clear_onames", len1) == 0) { 
	    return "clear_onames";
	 }//if
	 else if (strncasecmp(str1, "close", len1) == 0) { 
	    return "close";
	 }//if
	 else if (strncasecmp(str1, "color", len1) == 0) { 
	    return "color";
	 }//if
	 else if (strncasecmp(str1, "consider", len1) == 0) { 
	    return "consider";
	 }//if
	 else if (strncasecmp(str1, "commands", len1) == 0) { 
	    return "commands";
	 }//if
	 else if (strncasecmp(str1, "construct", len1) == 0) { 
	    return "construct";
	 }//if
	 else if (strncasecmp(str1, "concoct", len1) == 0) { 
	    return "concoct";
	 }//if
	 else if (strncasecmp(str1, "curse", len1) == 0) { 
	    return "curse";
	 }//if
         else if (strncasecmp(str1, "credits", len1) == 0) { 
	    return "credits";
	 }//if
         else if (strncasecmp(str1, "create_concoction", len1) == 0) { 
	    return "create_conct";
	 }//if
         else if (strncasecmp(str1, "create_construction", len1) == 0) { 
	    return "create_constrct";
	 }//if
	 return NULL;
      case 'D':
	 if (strncasecmp(str1, "down", len1) == 0) { 
	    return "down";
	 }//if
	 else if (strncasecmp(str1, "date", len1) == 0) { 
	    return "date";
	 }//if
	 else if (strncasecmp(str1, "darklings", len1) == 0) { 
	    return "darkling";
	 }//if
	 else if (strncasecmp(str1, "delete", len1) == 0) { 
	    return "suicide";
	 }//if
	 else if (strncasecmp(str1, "delete_door", len1) == 0) { 
	    return "delete_door";
	 }//if
	 else if (strncasecmp(str1, "discuss", len1) == 0) { 
	    return "discuss";
	 }//if
	 else if (strncasecmp(str1, "disarm", len1) == 0) { 
	    return "disarm";
	 }//if
	 else if (strncasecmp(str1, "disco", len1) == 0) { 
	    return "socials";
	 }//if
	 else if (strncasecmp(str1, "disdain", len1) == 0) { 
	    return "disdain";
	 }//if
	 else if (strncasecmp(str1, "donate", len1) == 0) { 
	    return "donate";
	 }//else
	 else if (strncasecmp(str1, "drop", len1) == 0) { 
	    return "drop";
	 }//if
	 else if (strncasecmp(str1, "dset", len1) == 0) { 
	    return "dset";
	 }//if
	 else if (strncasecmp(str1, "dclear", len1) == 0) { 
	    return "dclear";
	 }//if
	 else if (strncasecmp(str1, "drink", len1) == 0) {
	    return "drink";
	 }//if
	 else if (strncasecmp(str1, "dragons", len1) == 0) { 
	    return "dragon";
	 }//if
	 else if (strncasecmp(str1, "door_to", len1) == 0) { 
	    return "door_to";
	 }//if
	 else if (strncasecmp(str1, "dwarf", len1) == 0) { 
	    return "dwarf";
	 }//if
	 else if (strncasecmp(str1, "dwarves", len1) == 0) { 
	    return "dwarf";
	 }//if
	 else if (strncasecmp(str1, "drow", len1) == 0) { 
	    return "drow";
	 }//if
	 else if (strncasecmp(str1, "dstat", len1) == 0) { 
	    return "dstat";
	 }//if
	 else if (strncasecmp(str1, "dsys", len1) == 0) { 
	    return "dsys";
	 }//if
	 else if (strncasecmp(str1, "dlist", len1) == 0) { 
	    return "dlist";
	 }//if
	 else if (strncasecmp(str1, "dvnum", len1) == 0) { 
	    return "dvnum";
	 }//if
	 return NULL;
      case 'E': case 'F':
	 if (strncasecmp(str1, "east", len1) == 0) { 
	    return "east";
	 }//if
	 else if (strcasecmp(str1, "eat") == 0) {
	    return "eat";
	 }//if
	 else if (strncasecmp(str1, "equipment", len1) == 0) {
	    return "eq";
	 }//if
	 else if (strncasecmp(str1, "elf", len1) == 0) {
            return "elf";
	 }//if
	 else if (strncasecmp(str1, "emote", len1) == 0) {
            return "emote";
	 }//if
	 else if (strncasecmp(str1, "empty", len1) == 0) {
            return "empty";
	 }//if
	 else if (strncasecmp(str1, "entity", len1) == 0) {
            return "entity";
	 }//if
	 else if (strncasecmp(str1, "enslave", len1) == 0) {
	    return "enslave";
	 }//if
	 else if (strncasecmp(str1, "exits", len1) == 0) {
	    return "exit";
	 }//if
	 else if (strncasecmp(str1, "examine", len1) == 0) { 
	    return "examine";
	 }//if
	 else if (strncasecmp(str1, "fill", len1) == 0) {
	    return "fill";
	 }//if
	 else if (strncasecmp(str1, "faerie", len1) == 0) {
	    return "faerie";
	 }//if
	 else if (strncasecmp(str1, "follow", len1) == 0) {
	    return "follow";
	 }//if
	 else if (strncasecmp(str1, "force", len1) == 0) {
	    return "force";
	 }//if
	 else if (strncasecmp(str1, "force_all", len1) == 0) {
	    return "force_all";
	 }//if
	 else if (strncasecmp(str1, "flee", len1) == 0) {
	    return "flee";
	 }//if
	 else if (strncasecmp(str1, "flip_door", len1) == 0) {
	    return "flip_door";
	 }//if
	 else if (strncasecmp(str1, "freeze", len1) == 0) {
	    return "freeze";
	 }//if
	 return NULL;
      case 'G': 
	 if (strncasecmp(str1, "gag", len1) == 0) { 
	    return "gag";
	 }//if
	 else if (strncasecmp(str1, "get", len1) == 0) { 
	    return "get";
	 }//if
	 else if (strncasecmp(str1, "gecho", len1) == 0) { 
	    return "gecho";
	 }//if
	 else if (strncasecmp(str1, "give", len1) == 0) { 
	    return "give";
	 }//if
	 else if (strncasecmp(str1, "grin", len1) == 0) { 
	    return "grin";
	 }//if
	 else if (strncasecmp(str1, "group", len1) == 0) { 
	    return "group";
	 }//if
	 else if ((strncasecmp(str1, "gt", 2) == 0) ||
                  (strncasecmp(str1, "gs", len1) == 0)) { 
	    return "channels";
	 }//if
	 else if (strncasecmp(str1, "guard", len1) == 0) { 
	    return "guard";
	 }//if
	 else if (strncasecmp(str1, "go", len1) == 0) { 
	    return "go";
	 }//if
	 else if (strncasecmp(str1, "gossip", len1) == 0) { 
	    return "chanels";
	 }//if
	 else if (strncasecmp(str1, "goto", len1) == 0) { 
	    return "goto";
	 }//if
	 return NULL;
      case 'H':
	 if (strncasecmp(str1, "hide", len1) == 0) { 
	    return "hide";
	 }//if
	 else if (strncasecmp(str1, "hit", len1) == 0) { 
	    return "hit";
	 }//if
	 else if (strncasecmp(str1, "help", len1) == 0) { 
	    return "help";
	 }//if
	 else if (strncasecmp(str1, "hold", len1) == 0) { 
	    return "hold";
	 }//if
	 else if (strncasecmp(str1, "hurl", len1) == 0) { 
	    return "hurl";
	 }//if
	 else if (strncasecmp(str1, "humans", len1) == 0) { 
	    return "human";
	 }//if
	 return NULL;
      case 'I': case 'J': case 'K':
	 if (strncasecmp(str1, "inventory", len1) == 0) { 
	    return "inventory";
	 }//if
	 else if (strncasecmp(str1, "itrans", len1) == 0) { 
	    return "itrans";
	 }//if
	 else if (strncasecmp(str1, "idea", len1) == 0) { 
	    return "idea";
	 }//if
	 else if (strncasecmp(str1, "ironclads", len1) == 0) { 
	    return "ironclad";
	 }//if
	 else if (strncasecmp(str1, "junk", len1) == 0) { 
	    return "junk";
	 }//if
	 else if (strncasecmp(str1, "keywords", len1) == 0) { 
	    return "keyword";
	 }//if
	 else if (strncasecmp(str1, "kill", len1) == 0) { 
	    return "hit";
	 }//if

	 return NULL;
      case 'L': 
	 if (strncasecmp(str1, "look", len1) == 0) { 
	    return "look";
	 }//if
	 else if (strncasecmp(str1, "lore", len1) == 0) { 
	    return "lore";
	 }//if
	 else if (strncasecmp(str1, "list", len1) == 0) { 
	    return "list";
	 }//if
	 else if (strncasecmp(str1, "list_scripts", len1) == 0) { 
	    return "list_scripts";
	 }//if
	 else if (strncasecmp(str1, "list_site_bans", len1) == 0) { 
	    return "list_site_bans";
	 }//if
	 else if (strncasecmp(str1, "list_paths", len1) == 0) { 
	    return "list_paths";
	 }//if
	 else if (strncasecmp(str1, "lock", len1) == 0) { 
	    return "lock";
	 }//if
	 else if (strncasecmp(str1, "log_level", len1) == 0) { 
	    return "log_level";
	 }//if
	 else if (strncasecmp(str1, "ldwho", len1) == 0) { 
	    return "ldwho";
	 }//if
	 return NULL;
      case 'M':
	 if (strncasecmp(str1, "move", len1) == 0) { 
	    return "move";
	 }//if
	 else if (strncasecmp(str1, "make_builder", len1) == 0) { 
	    return "make_builder";
	 }//if
	 else if (strncasecmp(str1, "mclone", len1) == 0) { 
	    return "mclone";
	 }//if
	 else if (strncasecmp(str1, "mclear", len1) == 0) { 
	    return "mclear";
	 }//if
	 else if (strncasecmp(str1, "mlist", len1) == 0) { 
	    return "mlist";
	 }//if
	 else if (strncasecmp(str1, "mload", len1) == 0) { 
	    return "mload";
	 }//if
	 else if (strncasecmp(str1, "mreload", len1) == 0) { 
	    return "mreload";
	 }//if
	 else if (strncasecmp(str1, "mstat", len1) == 0) { 
	    return "mstat";
	 }//if
	 else if (strncasecmp(str1, "mset", len1) == 0) { 
	    return "mset";
	 }//if
	 else if (strncasecmp(str1, "mvnum", len1) == 0) { 
	    return "mvnum";
	 }//if
	 return NULL;
      case 'N': case 'O':
	 if (strncasecmp(str1, "north", len1) == 0) { 
	    return "north";
	 }//if
	 else if ((strncasecmp(str1, "northeast", len1) == 0) || 
		  (strncasecmp(str1, "ne", len1) == 0)) {
	    return "northeast";
	 }//if
	 else if ((strncasecmp(str1, "northwest", len1) == 0) || 
		  (strncasecmp(str1, "nw", len1) == 0)) {
	    return "northwest";
	 }//if
	 else if (strncasecmp(str1, "nogossip", len1) == 0) { 
	    return "nogossip";
	 }//if
	 else if (strncasecmp(str1, "nod", len1) == 0) { 
	    return "nod";
	 }//if
	 else if (strncasecmp(str1, "offer", len1) == 0) { 
	    return "offer";
	 }//if
	 else if (strncasecmp(str1, "open", len1) == 0) { 
	    return "open";
	 }//if
	 else if (strncasecmp(str1, "opurge", len1) == 0) { 
	    return "opurge";
	 }//if
	 else if (strncasecmp(str1, "ogrues", len1) == 0) { 
	    return "ogrue";
	 }//if
	 else if (strncasecmp(str1, "ostat", len1) == 0) { 
	    return "ostat";
	 }//if
	 else if (strncasecmp(str1, "oclone", len1) == 0) { 
	    return "oclone";
	 }//if
	 else if (strncasecmp(str1, "oclear", len1) == 0) { 
	    return "oclear";
	 }//if
	 else if (strncasecmp(str1, "oload", len1) == 0) { 
	    return "oload";
	 }//if
	 else if (strncasecmp(str1, "oreload", len1) == 0) { 
	    return "oreload";
	 }//if
	 else if (strncasecmp(str1, "oset", len1) == 0) { 
	    return "oset";
	 }//if
	 else if (strncasecmp(str1, "olist", len1) == 0) { 
	    return "olist";
	 }//if
	 else if (strncasecmp(str1, "oload", len1) == 0) { 
	    return "oload";
	 }//if
	 else if (strncasecmp(str1, "olc", len1) == 0) {
            return "olc";
	 }//if
	 else if (strncasecmp(str1, "ovnum", len1) == 0) {
            return "ovnum";
	 }//if
	 return NULL;
      case 'P': case 'Q':
	 if (strncasecmp(str1, "practice", len1) == 0) { 
	    return "practice";
	 }//if
	 else if ((strncasecmp(str1, "password", len1) == 0) ||
                  (strncasecmp(str1, "passwd", len1) == 0)) { 
	    return "passwd";
	 }//if
	 else if (strncasecmp(str1, "pay", len1) == 0) { 
	    return "pay";
	 }//if
	 else if (strncasecmp(str1, "page_break", len1) == 0) { 
	    return "page_break";
	 }//if
	 else if (strncasecmp(str1, "pecho", len1) == 0) { 
	    return "pecho";
	 }//if
	 else if (strncasecmp(str1, "possess", len1) == 0) { 
	    return "possess";
	 }//if
	 else if (strncasecmp(str1, "poofin", len1) == 0) { 
	    return "poofin";
	 }//if
	 else if (strncasecmp(str1, "poofout", len1) == 0) { 
	    return "poofin";
	 }//if
	 else if (strncasecmp(str1, "post", len1) == 0) { 
	    return "post";
	 }//if
	 else if (strncasecmp(str1, "prompt", len1) == 0) { 
	    return "prompt";
	 }//if
	 else if (strncasecmp(str1, "prone", len1) == 0) { 
	    return "prone";
	 }//if
	 else if (strncasecmp(str1, "picklock", len1) == 0) { 
	    return "picklock";
	 }//if
	 else if (strncasecmp(str1, "puke", len1) == 0) { 
	    return "puke";
	 }//if
	 else if (strncasecmp(str1, "put", len1) == 0) { 
	    return "put";
	 }//if
	 else if (strncasecmp(str1, "quaf", len1) == 0) { 
	    return "quaf";
	 }//if
	 else if (strncasecmp(str1, "quit", len1) == 0) { 
	    return "quit";
	 }//if
	 return NULL;
      case 'R':
	 if (strncasecmp(str1, "reply", len1) == 0) { 
	    return "reply";
	 }//if
	 else if (strncasecmp(str1, "recho", len1) == 0) { 
	    return "gecho";
	 }//if
	 else if (strncasecmp(str1, "races", len1) == 0) { 
	    return "races";
	 }//if
	 else if (strncasecmp(str1, "rem_oname", len1) == 0) { 
	    return "rem_oname";
	 }//if
	 else if (strncasecmp(str1, "rem_mname", len1) == 0) { 
	    return "rem_mname";
	 }//if
	 else if (strncasecmp(str1, "rem_zone", len1) == 0) { 
            return "active_zones";
         }
	 else if (strncasecmp(str1, "reset_olc", len1) == 0) { 
	    return "reset_olc";
	 }//if
	 else if (strncasecmp(str1, "regeneration", len1) == 0) { 
	    return "regeneration";
	 }//if
	 else if (strncasecmp(str1, "rest", len1) == 0) { 
	    return "rest";
	 }//if
	 else if (strncasecmp(str1, "recite", len1) == 0) { 
	    return "recite";
	 }//if
	 else if (strncasecmp(str1, "remove", len1) == 0) { 
	    return "remove";
	 }//if
	 else if (strncasecmp(str1, "rem_keyword", len1) == 0) { 
	    return "rm_keyword";
	 }//if
	 else if (strncasecmp(str1, "rm_keyword", len1) == 0) { 
	    return "rm_keyword";
	 }//if
	 else if (strncasecmp(str1, "rem_path", len1) == 0) { 
	    return "rem_path";
	 }//if
	 else if (strncasecmp(str1, "rem_perm_inv", len1) == 0) { 
	    return "rem_perm_inv";
	 }//if
	 else if (strncasecmp(str1, "read", len1) == 0) { 
	    return "read";
	 }//if
	 else if (strncasecmp(str1, "rezone", len1) == 0) { 
	    return "rezone";
	 }//if
	 else if (strncasecmp(str1, "replace_door", len1) == 0) { 
	    return "replace_door";
	 }//if
	 else if (strncasecmp(str1, "rinit", len1) == 0) { 
	    return "rinit";
	 }//if
	 else if (strncasecmp(str1, "rocktrolls", len1) == 0) { 
	    return "rocktroll";
	 }//if
	 else if (strncasecmp(str1, "roll", len1) == 0) { 
	    return "roll";
	 }//if
	 else if (strncasecmp(str1, "rstat", len1) == 0) { 
	    return "rstat";
	 }//if	 
	 else if (strncasecmp(str1, "rm_give_proc", len1) == 0) { 
	    return "rm_give_proc";
	 }//if	 
	 else if (strncasecmp(str1, "rm_discuss_proc", len1) == 0) { 
	    return "rm_discuss_proc";
	 }//if	 
	 else if (strncasecmp(str1, "rm_curse_proc", len1) == 0) { 
	    return "rm_curse_proc";
	 }//if	 
	 else if (strncasecmp(str1, "rm_bow_proc", len1) == 0) { 
	    return "rm_bow_proc";
	 }//if
	 else if (strncasecmp(str1, "rm_script", len1) == 0) { 
	    return "rm_script";
	 }//if	 
	 else if (strncasecmp(str1, "rm_stat_affect", len1) == 0) { 
	    return "rm_stat_affect";
	 }//if	 
	 else if (strncasecmp(str1, "rm_casts_spell", len1) == 0) { 
	    return "rm_casts_spell";
	 }//if	 
         else if (strncasecmp(str1, "rset", len1) == 0) { 
	    return "rset";
	 }//if
	 else if (strncasecmp(str1, "rclone", len1) == 0) { 
	    return "rclone";
	 }//if
	 else if (strncasecmp(str1, "rlist", len1) == 0) { 
	    return "rlist";
	 }//if
	 else if (strncasecmp(str1, "rclear", len1) == 0) { 
	    return "rclear";
	 }//if
	 return NULL;
      case 'S':
	 if (strncasecmp(str1, "say", 3) == 0) { 
	    return "say";
	 }//if
	 else if (strncasecmp(str1, "smob", len1) == 0) { 
	    return "smob";
	 }//if
	 else if (strncasecmp(str1, "south", len1) == 0) { 
	    return "south";
	 }//if
	 else if (strncasecmp(str1, "shout", len1) == 0) { 
	    return "shout";
	 }//if
	 else if ((strncasecmp(str1, "southeast", len1) == 0) || 
		  (strncasecmp(str1, "se", len1) == 0)) {
	    return "southeast";
	 }//if
	 else if ((strncasecmp(str1, "southwest", len1) == 0) || 
		  (strncasecmp(str1, "sw", len1) == 0)) {
	    return "southwest";
	 }//if
	 else if (strncasecmp(str1, "save", len1) == 0) { 
	    return "save";
	 }//if
	 else if (strncasecmp(str1, "scan", len1) == 0) { 
	    return "scan";
	 }//if
	 else if (strncasecmp(str1, "save_mob", len1) == 0) { 
	    return "save_mob";
	 }//if
	 else if (strncasecmp(str1, "save_obj", len1) == 0) { 
	    return "save_obj";
	 }//if
	 else if (strncasecmp(str1, "set_path_dir", len1) == 0) { 
	    return "set_path_dir";
	 }//if 
	 else if (strncasecmp(str1, "set_path_pointer", len1) == 0) { 
	    return "set_path_pointer";
	 }//if 
	 else if (strncasecmp(str1, "score", len1) == 0) { 
	    return "score";
	 }//if 
	 else if (strncasecmp(str1, "scribe", len1) == 0) { 
	    return "scribe";
	 }//if 
	 else if (strncasecmp(str1, "see_all", len1) == 0) { 
	    return "see_all";
	 }//if 
	 else if (strncasecmp(str1, "shield", len1) == 0) { 
	    return "shield";
	 }//if
	 else if (strncasecmp(str1, "shoot", len1) == 0) { 
	    return "shoot";
	 }//if
	 else if (strncasecmp(str1, "show_zones", len1) == 0) { 
            return "active_zones";
         }
	 else if (strncasecmp(str1, "sell", len1) == 0) { 
	    return "sell";
	 }//if
	 else if (strncasecmp(str1, "set_veh_stop", len1) == 0) { 
	    return "set_veh_stop";
	 }//if
	 else if (strncasecmp(str1, "sit", len1) == 0) { 
	    return "sit";
	 }//if
	 else if (strncasecmp(str1, "siteban", len1) == 0) { 
	    return "siteban";
	 }//if
	 else if (strncasecmp(str1, "sleep", len1) == 0) { 
	    return "sleep";
	 }//if
	 else if (strncasecmp(str1, "skin", len1) == 0) { 
	    return "skin";
	 }//if
	 else if (strncasecmp(str1, "slist", len1) == 0) { 
	    return "slist";
	 }//if
	 else if (strncasecmp(str1, "slay", len1) == 0) { 
	    return "slay";
	 }//if
	 else if (strncasecmp(str1, "sombri", len1) == 0) { 
	    return "sombri";
	 }//if 
	 else if (strncasecmp(str1, "socials", len1) == 0) { 
	    return "socials";
	 }//if
	 else if (strncasecmp(str1, "sneak", len1) == 0) { 
	    return "sneak";
	 }//if
	 else if (strncasecmp(str1, "snoop", len1) == 0) { 
	    return "snoop";
	 }//if 
	 else if (strncasecmp(str1, "stand", len1) == 0) { 
	    return "stand";
	 }//if
	 else if (strncasecmp(str1, "stat_keyword", len1) == 0) { 
	    return "stat_keyword";
	 }//if
	 else if (strncasecmp(str1, "stat_path", len1) == 0) { 
	    return "stat_path";
	 }//if
	 else if (strncasecmp(str1, "stat_script", len1) == 0) { 
	    return "stat_script";
	 }//if
	 else if (strncasecmp(str1, "sockets", len1) == 0) { 
	    return "sockets";
	 }//if
	 else if (strncasecmp(str1, "suicide", len1) == 0) { 
	    return "suicide";
	 }//if
	 else if (strncasecmp(str1, "shutdown", len1) == 0) { 
	    return "shutdown";
	 }//if
	 return NULL;
      case 'T':
	 if (strncasecmp(str1, "tail", len1) == 0) { 
	    return "tail";
	 }//if
	 else if (strncasecmp(str1, "title", len1) == 0) { 
	    return "title";
	 }//if
	 else if (strncasecmp(str1, "take", len1) == 0) { 
	    return "get";
	 }//if
	 else if (strncasecmp(str1, "teach", len1) == 0) { 
	    return "teach";
	 }//if
	 else if (strncasecmp(str1, "theme", len1) == 0) { 
	    return "theme";
	 }//if
	 else if (strncasecmp(str1, "throw", len1) == 0) { 
	    return "throw";
	 }//if
	 else if (strncasecmp(str1, "time", len1) == 0) { 
	    return "time";
	 }//if
	 else if (strncasecmp(str1, "track", len1) == 0) { 
	    return "track";
	 }//if
	 else if (strncasecmp(str1, "toggle", len1) == 0) { 
	    return "toggle";
	 }//if
	 else if (strncasecmp(str1, "tog_veh_stop", len1) == 0) { 
	    return "tog_veh_stop";
	 }//if
	 else if (strncasecmp(str1, "topics", len1) == 0) { 
	    return "topics";
	 }//if
	 else if (strncasecmp(str1, "total_rezone", len1) == 0) { 
	    return "total_rezone";
	 }//if
	 else if (strncasecmp(str1, "tog_mflag", len1) == 0) { 
            return "tog_mflag";
	 }//if
	 else if (strncasecmp(str1, "tog_oflag", len1) == 0) { 
	    return "tog_oflag";
	 }//if
	 else if (strncasecmp(str1, "tog_rflag", len1) == 0) { 
	    return "tog_rflag";
	 }//if
	 else if (strncasecmp(str1, "typo", len1) == 0) { 
	    return "bug";
	 }//if
	 else if (strncasecmp(str1, "thaw", len1) == 0) { 
	    return "thaw";
	 }//if
	 return NULL;
      case 'U': case 'V': case 'W':
	 if (strncasecmp(str1, "up", len1) == 0) { 
	    return "up"; 
	 }//if
	 else if (strncasecmp(str1, "ungroup", len1) == 0) { 
	    return "ungroup"; 
	 }//if
	 else if (strncasecmp(str1, "unpost", len1) == 0) { 
	    return "unpost"; 
	 }//if
	 else if (strncasecmp(str1, "ungag", len1) == 0) { 
	    return "ungag"; 
	 }//if
	 else if (strncasecmp(str1, "unlock", len1) == 0) { 
	    return "unlock"; 
	 }//if
	 else if (strncasecmp(str1, "unsiteban", len1) == 0) {
	    return "unsiteban"; 
         }//if
	 else if (strncasecmp(str1, "unpossess", len1) == 0) { 
	    return "unpossess"; 
	 }//if
	 else if (strncasecmp(str1, "unsnoop", len1) == 0) { 
	    return "unsnoop"; 
	 }//if
	 else if (strncasecmp(str1, "uptime", len1) == 0) { 
	    return "uptime"; 
	 }//if
	 else if (strncasecmp(str1, "update_cur_in_game", len1) == 0) { 
	    return "update_cig"; 
	 }//if
	 else if (strncasecmp(str1, "update_cig", len1) == 0) { 
	    return "update_cig"; 
	 }//if
	 else if (strncasecmp(str1, "use", len1) == 0) { 
	    return "use"; 
	 }//if
	 else if (strncasecmp(str1, "visible", len1) == 0) { 
	    return "visible";
	 }//if
	 else if (strncasecmp(str1, "west", len1) == 0) { 
	    return "west";
	 }//if
	 else if (strncasecmp(str1, "wear", len1) == 0) { 
	    return "wear";
	 }//if
	 else if (strncasecmp(str1, "weep", len1) == 0) { 
	    return "socials";
	 }//if
	 else if (strncasecmp(str1, "wake", len1) == 0) { 
	    return "wake";
	 }//if
	 else if (strncasecmp(str1, "who", len1) == 0) { 
	    return "who";
	 }//if
	 else if (strncasecmp(str1, "where", len1) == 0) { 
	    return "where";
	 }//if
	 else if (strncasecmp(str1, "wave", len1) == 0) { 
	    return "wave";
	 }//if
	 else if (strncasecmp(str1, "wield", len1) == 0) { 
	    return "wield";
	 }//if
	 else if (strncasecmp(str1, "wimpy", len1) == 0) { 
	    return "wimpy";
	 }//if
	 else if (strncasecmp(str1, "wizinvis", len1) == 0) { 
	    return "wizinvis";
	 }//if
	 else if (strncasecmp(str1, "wizlist", len1) == 0) { 
	    return "wizlist";
	 }//if
	 else if (strncasecmp(str1, "wizhelp", len1) == 0) { 
	    return "wizhelp";
	 }//if
	 else if (strncasecmp(str1, "write_world", len1) == 0) { 
	    return "write_world";
	 }//if
	 else if (strncasecmp(str1, "write_zone", len1) == 0) { 
	    return "write_zone";
	 }//if
	 return NULL;
      case 'X': case 'Y': case 'Z':
	 if (strncasecmp(str1, "zlock", len1) == 0) { 
	    return "zlock"; 
	 }//if
	 else if (strncasecmp(str1, "zlist", len1) == 0) { 
	    return "zlist"; 
	 }//if
	 else if (strncasecmp(str1, "zcreate", len1) == 0) { 
	    return "zcreate"; 
	 }//if
	 else if (strncasecmp(str1, "zenable", len1) == 0) { 
	    return "zenable"; 
	 }//if
	 else if (strncasecmp(str1, "zdisable", len1) == 0) { 
	    return "zdisable"; 
	 }//if
	 else if (strncasecmp(str1, "zunlock", len1) == 0) { 
	    return "zunlock"; 
	 }//if
	 else if (strncasecmp(str1, "zecho", len1) == 0) { 
	    return "gecho"; 
	 }//if
	 else if (strncasecmp(str1, "zgoto", len1) == 0) { 
	    return "zgoto"; 
	 }//if
	 return NULL;
      default:   
	 return NULL;
   }//switch
}//parse_hlp_command
