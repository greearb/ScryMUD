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

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef LOG_STREAM_INCLUDE
#define LOG_STREAM_INCLUDE



enum LogLevelEnum {
   DIS = 1,   //disasters
   ERR = 2,   //errors
   WRN = 4,   //warnings
   INF = 8,   //info
   TRC = 16,  //function trace
   DBG = 32,  //debug
   SEC = 64,  // log security violations
   DB = 128,  // READ or WRITE from the world files
   XMT = 256, // Output from MUD to characters
   INP = 512, // Input from players
   SCRIPT = 1024, //Scripting specific stuff
   PARSE = 2048, //PARSE specific
   ALL = ~(0) // all
};//enum


class LogStream : public ofstream {
private:
   char* filename;
   int log_level;

public:
   LogStream(const char* fname, int lvl) : ofstream(fname),
      log_level(lvl) { filename = strdup(fname); }

   ~LogStream() { flush(); free(filename); }

   int ofLevel(LogLevelEnum i) { return ((int)(i) & log_level); }

   int truncate() {
      char buf[200];
      close();

      sprintf(buf, "truncate_log %s", filename);
      system(buf);

      open(filename, ios::app); //open and append
      return 1;
   }//int

   int getLevel() { return log_level; }
   void setLevel(int i) { log_level = i; }

   void log(int lvl, const char* msg) {
      if ((int)(lvl) & log_level) {
         (*this) << msg << endl;
         flush();
      }
   }

   void dbg(const char* msg) {
      log(DBG, msg);
   }

   void log(const char* msg) {
      log(DBG, msg);
   }

};//LogStream

#endif
