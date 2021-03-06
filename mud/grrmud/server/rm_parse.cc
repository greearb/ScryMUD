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
//To contact the maintainer, Edward Roper: edro+scrymud [at] wanfear.net
//

/********************  rm_parse.cc  ***************/
/* this holds the calls to every function that a 'room' can call.
 * If you wish to add another 
 * function, you better add it in the switch statement.
 */

#include "misc.h"
#include "misc2.h"
#include <stdio.h>
#include "classes.h"
#include "rm_parse.h"
#include <PtrArray.h>
#include "script.h"
#include "rm_cmds.h"
#include "parse.h"

             /******************************************/
int room::processInput(String& input) {
   String raw_strings[RAW_MAX];
   String cooked_strs[COOKED_MAX];
   int cooked_ints[COOKED_MAX];
   String buf(100);
   int i, j, len1, k;
   short eos, term_by_period;

   for (i = 0; i<COOKED_MAX; i++) {
      cooked_ints[i] = 1;
   }

   if (mudlog.ofLevel(INF)) {
      mudlog << "ROOM INPUT:  room:  " << getIdNum() << "  input -:"
             << input << ":-\n" << flush;
   }

   if (getPause() > 0) {
      return 0;
   }//if

   if (input.Strlen() == 0) {
      return 0; //do nothing
   }//if

                        /* make sure it ends in newline */
   j = input.Strlen();
   if (input[j - 1] != '\n') {
      //log("WARNING:  input doesn't end in a newline, process_input.\n");
      input += "\n";
   }//if

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Getting raw_strings[0]" << endl;
   }
   
   raw_strings[0] = input.Get_Command(eos, term_by_period); 

   if (raw_strings[0].Strlen() == 0) {
      return 0;
   }//if

///****************************************************************///
///*************** non-standard-input-commands  *******************///
///****************************************************************///


   len1 = raw_strings[0].Strlen();

   if (strncasecmp(raw_strings[0], "tell", max(4, len1)) == 0) {
      buf = input.Get_Command(eos, term_by_period);
      if (buf.Strlen() != 0) {
         if (isnum(buf)) {
            i = atoi(buf);
            buf = input.Get_Command(eos, term_by_period);
            if (buf.Strlen() != 0) {
               (cooked_strs[1]) = buf;
               buf = input.Get_Rest();
               parse_communication(buf);
               return this->tell(i, &(cooked_strs[1]), buf);
            }//if
         }//if is number
         else {
            (cooked_strs[1]) = buf;
            buf = input.Get_Rest();
            parse_communication(buf);
            return this->tell(1, &(cooked_strs[1]), buf);
         }//else
      }//if
      return -1;
   }//if tell


   if (strncasecmp(raw_strings[0], "neighbor_echo", 13) == 0) {
      buf = input.Get_Command(eos, term_by_period);
      if (buf.Strlen() != 0) {
         if (isnum(buf)) {
            i = atoi(buf);
            buf = input.Get_Command(eos, term_by_period);
            if (buf.Strlen() != 0) {
               (cooked_strs[1]) = buf;
               buf = input.Get_Rest();
               parse_communication(buf);
               return this->neighbor_echo(i, &(cooked_strs[1]), buf);
            }//if
         }//if is number
         else {
            (cooked_strs[1]) = buf;
            buf = input.Get_Rest();
            parse_communication(buf);
            return this->neighbor_echo(1, &(cooked_strs[1]), buf);
         }//else
      }//if

      return -1;
   }//if neighbor_echo


   if (strncasecmp(raw_strings[0], "other_room_echo", 15) == 0) {
      buf = input.Get_Command(eos, term_by_period);
      if (buf.Strlen() != 0) {
         if (isnum(buf)) {
            i = atoi(buf);
            buf = input.Get_Rest();
            parse_communication(buf);
            return this->other_room_echo(i, buf);
         }//if is number
         else {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "WARNING:  No room number in other_room_echo, rm#"
                      << this->getIdNum() << endl;
            }
         }//else
      }//if
      return -1;
   }//if other_room_echo

   
   if (strcasecmp(raw_strings[0], "recho") == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return this->com_recho(&buf);
   }//if

   if (strcasecmp(raw_strings[0], "zecho") == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return this->com_zecho(&buf);
   }//if
   
   if (strncasecmp(raw_strings[0], "wizchat", max(3, len1)) == 0) {
      buf = input.Get_Rest();
      parse_communication(buf);
      return this->wizchat(buf);
   }//if auction

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

   if ((len1 = cooked_strs[0].Strlen()) == 0) {
      return -1;
   }//if

   i = cooked_ints[1];
   j = cooked_ints[2];
   k = cooked_ints[3];

   switch (toupper(cooked_strs[0][0]))
      {
      case 'A':
         if (strncasecmp(cooked_strs[0], "affect_hp", len1) == 0) { 
            return affect_crit_stat(STAT_HP, cooked_strs[2], i, &(cooked_strs[1]),
                                    j, k, this);
         }
         else if (strncasecmp(cooked_strs[0], "affect_mana", len1) == 0) {
            return affect_crit_stat(STAT_MANA, cooked_strs[2], i, &(cooked_strs[1]),
                                    j, k, this);
         }
         else if (strncasecmp(cooked_strs[0], "affect_mov", len1) == 0) {
            return affect_crit_stat(STAT_MOV, cooked_strs[2], i, &(cooked_strs[1]),
                                    j, k, this);
         }
         else {
            return -1;
         }
      case 'B':
      case 'C':
         if (strncasecmp(cooked_strs[0], "close", len1) == 0) {
            return this->doClose(i, &(cooked_strs[1]));
         }
      case 'D':
      case 'E':
      case 'F':
      case 'G': 
      case 'H':
      case 'I': 
      case 'J': 
      case 'K':
      case 'L':
      case 'M':
         if (strncasecmp(cooked_strs[0], "mload", len1) == 0) { 
            return this->doMload(i);
         }//if
         else {
            return -1;
         }
      case 'N':
      case 'O':
         if (strncasecmp(cooked_strs[0], "oload", len1) == 0) { 
            return this->doOload(i);
         }//if
         else {
            return -1;
         }

      case 'P': 
         if (strncasecmp(cooked_strs[0], "pause", len1) == 0) { 
            return this->rm_pause(i);
         }//if
         else {
            return -1;
         }
      case 'Q':
      case 'R':
         if (strncasecmp(cooked_strs[0], "rm_move", len1) == 0) { 
            return this->move(i, &(cooked_strs[1]), j, &(cooked_strs[2]));
         }//if
         else if (strncasecmp(cooked_strs[0], "rm_move_all", len1) == 0) { 
            return this->move_all(i, &(cooked_strs[1]));
         }//if
         else if (strncasecmp(cooked_strs[0], "rm_transport", len1) == 0) { 
            return this->transport(i, &(cooked_strs[1]), j);
         }//if
         else if (strncasecmp(cooked_strs[0], "rm_transport_all", len1) == 0) { 
            return this->transport_all(i);
         }//if
         else if (strncasecmp(cooked_strs[0], "rm_otransport", len1) == 0) { 
            return this->otransport(i, &(cooked_strs[1]), j);
         }//if
         else if (strncasecmp(cooked_strs[0], "rm_otransport_all", len1) == 0) { 
            return this->otransport_all(i);
         }//if
         else if (strncasecmp(cooked_strs[0], "rm_omove", len1) == 0) { 
            return this->omove(i, &(cooked_strs[1]), j, &(cooked_strs[2]));
         }//if
         else if (strncasecmp(cooked_strs[0], "rm_omove_all", len1) == 0) { 
            return this->omove_all(i, &(cooked_strs[1]));
         }//if
         else {
            return -1;
         }

      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
      default:   
         return -1;
   }//switch

   return -1;
}// process input
