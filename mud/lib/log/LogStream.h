// $Id: LogStream.h,v 1.7 1999/07/16 06:12:53 greear Exp $
// $Revision: 1.7 $  $Author: greear $ $Date: 1999/07/16 06:12:53 $

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

#include <strstream.h>
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef LOG_STREAM_INCLUDE
#define LOG_STREAM_INCLUDE

#define LOG_STREAM_BUF_SIZE 200000  /* 200 kilobytes storage */


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
   DBG2 = 4096,  //More verbose than debug even..
   ALL = ~(0) // all
};//enum

class LogStream;

typedef LogStream& (*__ls_manip)(LogStream&);

class LogStream : public ostrstream {
protected:
   char buf[LOG_STREAM_BUF_SIZE];
   char* filename;
   int log_level;

public:
   LogStream(const char* fname, int lvl) : ostrstream(buf, LOG_STREAM_BUF_SIZE),
      log_level(lvl) { filename = strdup(fname); }

   ~LogStream() { 
      flushToFile(filename);
      free(filename);
   }

   const char* getFileName() const { return filename; }

   int flushToFile(const char* fname) {
      *((ostrstream*)this) << ends;
      ofstream ofile(fname);
      ofile << str() << flush();

      // Allow some outside filtering...
      char buf[200];
      sprintf(buf, "truncate_log %s", fname);
      system(buf);

      seekp(0); //TODO:  This reset's it, no?
      return 0;
   }

   /** The idea is that the << (const char*) method will be called
    * very often.  So, I'm going to over-ride it, and do a check on
    * our buffer usage.  If it's more than 90%, then dump I'm going
    * to flush it and reset myself to the beginning of the buffer.
    */
   LogStream& operator<< (char* msg) {
      return (*this) << (const char*)msg; 
   }

   LogStream& operator<< (const char* msg) {
      *((ostrstream*)this) << msg; 
      if (pcount() > ((float)(LOG_STREAM_BUF_SIZE) * 0.95)) {
         flushToFile(filename);
      }//if
      return *this;
   }//operator overload      

   LogStream& operator<< (const void* ptr) { 
      *((ostrstream*)this) << ptr;
      return *this;
   }

   LogStream& operator<< (int i) { 
      *((ostrstream*)this) << i;
      return *this;
   }

   int ofLevel(LogLevelEnum i) { return ((int)(i) & log_level); }

   int truncate() {
      return 1;
/*
      char buf[200];
      close();

      sprintf(buf, "truncate_log %s", filename);
      system(buf);

      open(filename, ios::app); //open and append
      return 1;
*/
   }//int

   int getLevel() { return log_level; }
   void setLevel(int i) { log_level = i; }

   void log(int lvl, const char* msg);

   void dbg(const char* msg) {
      log(DBG, msg);
   }

   void log(const char* msg) {
      log(DBG, msg);
   }

   LogStream& operator<<(__ls_manip func) { return (*func)(*this); }

};//LogStream


inline LogStream& endl(LogStream& lstr) {
   return lstr << (const char*)("\n");
}

inline LogStream& nl(LogStream& lstr) {
   return lstr << (const char*)("\n");
}

inline LogStream& flush(LogStream& lstr) {
   lstr.flush();
   return lstr;
}

inline void LogStream::log(int lvl, const char* msg) {
   if ((int)(lvl) & log_level) {
      (*this) << msg;
      (*this) << nl;
      flush();
   }
}//log

#endif
