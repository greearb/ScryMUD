// $Id: classes.cc,v 1.22 2001/03/29 03:02:28 eroper Exp $
// $Revision: 1.22 $  $Author: eroper $ $Date: 2001/03/29 03:02:28 $

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

///********************  classes.cc  *****************************///

#include "classes.h" 
#include "misc.h"
#include "misc2.h"
#include "room.h"
#include "const.h"
#include "commands.h"    //for wear_eq_effects...
#include "battle.h"
#include <PtrArray.h>
#include "command5.h"
#include "command4.h"
#include "BuildInfo.h"
#include <getopt.h>


void CmdLineInfo::init(int argc, char** argv) {
   //first, init values to defaults
   int lvl = (ERROR | DIS | WRN | LSEC);

   // This code ripped off from the man page for getopt. --BEN

   int c;
   //int digit_optind = 0;
   
   while (1) {
      //int this_option_optind = optind ? optind : 1;
      int option_index = 0;
      static struct option long_options[] =
      {
         {"help", 0, 0, 0},
         {"log_level", 1, 0, 0},
         {"port", 1, 0, 0},
         {"usage", 0, 0, 0},
         {"version", 0, 0, 0},
         {"summon", 0, 0, 0},
         {"teeport", 0, 0, 0},
         {0, 0, 0, 0}
      };

      // If it needs an argument, put a : after it.  The letter is the one
      // letter arg that will select the option.  For example, -h and --help
      // will do the same thing (and take no arguments)
      c = getopt_long (argc, argv, "hl:p:uvts",
                       long_options, &option_index);
      if (c == -1)
         break;
      
      if (c == 0) {
         // Then we were passed a long argument (ie --foo).  So, we'll just
         // grab the first character.  Have to make sure that is unique...
         c = long_options[option_index].name[0]; //grab first char

         //printf ("option %s", long_options[option_index].name);
         //if (optarg)
         //   printf (" with arg %s", optarg);
         //printf ("\n");
      }

      switch (c)
         {
         case 'p':
            config.port = atoi(optarg);
            cout << "Using " << config.port << " for the well known server port.\n";
            break;

         case 'l': {
            lvl = atoi(optarg);
            cout << "Setting logging level to: " << lvl << endl;
            break;
         }            
         case 'v': {
            cout << "ScryMUD version: " << BuildInfo::getVersion() << " built on: "
                 << BuildInfo::getBuildDate() << "\nby: " << BuildInfo::getBuilder()
                 << " on machine: " << BuildInfo::getBuildMachine() << endl;
            do_shutdown = TRUE;
            exit(0);
            break;
         }
         case 'h':
         case 'u': {
            cout << "ScryMUD Usage:
-l --log_level   bitfield (add up the integer values listed below)
                                                DIS = 1,   //disasters
                                                ERR = 2,   //errors
                                                WRN = 4,   //warnings
                                                INF = 8,   //info
                                                TRC = 16,  //function trace
                                                DBG = 32,  //debug
                                                SEC = 64,  // log security violations
                                                DB = 128,  // READ or WRITE to NVDS
                                                XMT = 256, // Server to client comm.
                                                INP = 512, // Input from clients.
                                                SCRIPT = 1024, //Scripting specific
                                                PARSE = 2048, //PARSE specific
                                                DBG2 = 4096,  //More verbose than debug.
                                                ALL = 65535 = ~(0)  // all
-p --port                    Specify server port
-v --version                 Print out build/version information.
-h --help                    Print this message.
-u --usage                   Print this message.\n\n";
            do_shutdown = TRUE;
            exit(0);
            break;
         }
         default:
            printf ("?? getopt returned character code 0%o ??\n", c);
         }//switch
   }//while TRUE,read all cmd-line arguments

   mudlog.setLevel(lvl);
   
   if (optind < argc) {
      printf ("non-option ARGV-elements: ");
      while (optind < argc) {
         printf ("%s ", argv[optind++]);
      }
      printf ("\n");
   }
}//CmdLineInfo constructor



/** Static method */
String HegemonMarkup::makeSafeForHegTag(const char* str) {
   int len = strlen(str);
   char c;
   String retval(len + 50);

   for (int i = 0; i<len; i++) {
      c = str[i];
      if (c == '>') {
         retval += ">>";
      }
      else {
         retval += c;
      }
   }//for
   return retval;
}//convertToHeg


int stat_spell_cell::_cnt = 0;

String stat_spell_cell::toString() const {
   String retval(100);
   Sprintf(retval, "stat_spell: %i  bonus_duration: %i", stat_spell,
           bonus_duration);
   return retval;
}


int PathCell::_cnt = 0;

PathCell::PathCell(String& description, String& direction_to_next,
                   int is_dest) {
   _cnt++;
   desc = description;
   dir_to_next = direction_to_next;
   is_destination = is_dest;
}


PathCell::PathCell() {
   _cnt++;
   dir_to_next = "NOWHERE";
   is_destination = 0;
   desc = "GENERIC_DESC";
   i_th_dir = 1;
}//constructor


void PathCell::Clear() {
   desc.Clear();
   dir_to_next.Clear();
   is_destination = i_th_dir = 0;
}

void PathCell::Read(ifstream& da_file) {
   char tmp[81];

   desc.Termed_Read(da_file); //description

//   mudlog << "PathCell:  got desc:  -:" << desc << ":-" << endl;

   da_file >> is_destination >> i_th_dir >> dir_to_next;

//   mudlog << "Is_dest:  " << is_destination << "  i_th:  " << i_th_dir
//          << "  dir:  " << dir_to_next << endl;

   da_file.getline(tmp, 80);
//   mudlog << "Junk:  -:" << tmp << ":-" << endl;
}//Read


void PathCell::Write(ofstream& da_file) {
   parse_for_max_80(desc);
   da_file << desc << "\n~\n";

   da_file << is_destination << " " << i_th_dir << " " << dir_to_next 
           << " is_dest, i_th, dir, End of PathCell..\n";
}//Write


void PathCell::setIsDest(int i) {
   if (i)
      is_destination = 1;
   else 
      is_destination = 0;
}

int PathCell::isDest() {
   return is_destination;
}


void PathCell::stat(int veh_num, int path_cell_num, critter& pc) {
   String buf(1000);

   if (pc.isPc()) {
      if (pc.isUsingClient()) {
         Sprintf(buf, "<PSTAT %i %i>", veh_num, path_cell_num);
         pc.show(buf);

         pc.show(" <PSTAT_LD>");
         pc.show(desc);
         pc.show("</STAT> ");

         Sprintf(buf, "<PATH_DIR %i %S> ", i_th_dir, &dir_to_next);
         pc.show(buf);
         

         Sprintf(buf, "<PATH_IS_DEST %i> ", is_destination);
         pc.show(buf);
      }

      Sprintf(buf, "Path Cell [%i] for vehicle [%i].  Desc:\n",
              path_cell_num, veh_num);
      pc.show(buf);
      pc.show(desc);
      Sprintf(buf, "Direction:  i_th: %i  dir: %S\n", i_th_dir,
              &dir_to_next);
      pc.show(buf);
      
      if (is_destination) {
         pc.show("It IS a DESTINATION.\n");
      }
      else {
         pc.show("It is NOT a destination.\n");
      }
   }//if it's a pc
}//stat
