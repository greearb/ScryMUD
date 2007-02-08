// $Id$
// $Revision$  $Author$ $Date$

//
//Copyright (C) 2001  Ben Greear
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU Library General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU Library General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// To contact the Author, Ben Greear:  greearb@candelatech.com
//

//  This class has the ability to buffer logs in RAM for efficiency,
//  roll logs after a certain amount of usage, and it's verbosity
//  level can be adjusted on the fly.

/*
#ifdef HAVE_NEW_IOSTREAMS
#include <sstream>
typedef std::stringstream my_sstream;
#else
#include <strstream>
typedef strstream my_sstream;
#endif
*/
#include <sstream>
typedef std::stringstream my_sstream;

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#ifndef LOG_STREAM_INCLUDE
#define LOG_STREAM_INCLUDE

#define LOG_STREAM_BUF_SIZE 20000  /* 20 kilobytes storage */


enum LogLevelEnum {
   DIS = 1,   //disasters
   ERROR = 2,   //errors
   WRN = 4,   //warnings
   INF = 8,   //info
   TRC = 16,  //function trace
   DBG = 32,  //debug
   SEC = 64,  // log security violations
   LSEC = 64,  // log security violations
   DB = 128,  // READ or WRITE from the world files
   XMT = 256, // Output from MUD to characters
   INP = 512, // Input from players
   SCRIPT = 1024, //Scripting specific stuff
   PARSE = 2048, //PARSE specific
   DBG2 = 4096,  //More verbose than debug even..
   LIO = 8192,   // other IO (From cards)
   OUT1 = (1 << 14), // Should print stuff to stdout, dbg level 1
   LL_PROF = (1 << 15), // Profiling information.
   CUST1 = (1 << 16), // Customer-viewable stuff (latency, for example)
   ALL = ~(0) // all
};//enum

class LogStream;

typedef LogStream& (*__ls_manip)(LogStream&);

LogStream& endl(LogStream& lstr);

class LogStream : public std::ofstream {
protected:
   char* filename;
   int log_level;
   int file_action;
   int max_logfile_sz; // allowed on disk
   int sofar; // amt bytes written since last file-roll

public:
   enum actionE {
      APPEND = 1,
      OVERWRITE = 2
   };

   LogStream(const char* fname, int lvl, int fileAction, int max_logfile_size)
         : log_level(lvl), file_action(fileAction), max_logfile_sz(max_logfile_size), sofar(0) {
      filename = NULL;
      setFileName(fname);
   }//constructor

   LogStream(const char* fname, int lvl)
         : log_level(lvl), file_action(OVERWRITE), max_logfile_sz(100000), sofar(0) {
      filename = NULL;
      setFileName(fname);
   }//constructor

   ~LogStream() { 
      flush();
      if (filename)
         free(filename);
   }//destructor

   const char* getFileName() const { return filename; }

   void setFileName(const char* fname) {
      if (fname && filename && (strcmp(fname, filename) == 0)) {
         return;
      }

      if (filename) {
         free(filename);
         filename = NULL;
      }

      close();
      clear();

      if (fname) {
         filename = strdup(fname);
         if (file_action & OVERWRITE) {
            open(filename);
         }
         else {
            open(filename, std::ios::app);
         }
         //cout << "Opening log-str -:" << filename << ":-\n";

         *(this) << "Log stream opened, name: " << filename << endl;
      }
   }//setFileName

   int rollFile() {
      sofar = 0;
      if (file_action & OVERWRITE) {
         close();
         clear();
         char buf[strlen(filename) * 2 + 100];
         sprintf(buf, "mv -f %s %s.old", filename, filename);
         system(buf);
         open(filename);
      }
      return 0;
   }

   int cpFileTo(char* new_fname) {
      flush();
      char buf[strlen(filename) + strlen(new_fname) + 100];
      sprintf(buf, "cp %s %s", filename, new_fname);
      system(buf);
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
      *((std::ofstream*)this) << msg;
      sofar += strlen(msg);
      if (sofar > max_logfile_sz) {
         rollFile();
      }//if
      return *this;
   }//operator overload      

   LogStream& operator<< (const void* ptr) { 
      *((std::ofstream*)this) << ptr;
      sofar += 4; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (short int i) { 
      *((std::ofstream*)this) << i;
      sofar += 4; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (unsigned char i) { 
      *((std::ofstream*)this) << i;
      sofar += 1; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (char i) { 
      *((std::ofstream*)this) << i;
      sofar += 1; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (unsigned short int i) { 
      *((std::ofstream*)this) << i;
      sofar += 4; //guess how long it might be...
      return *this;
   }
      
   LogStream& operator<< (int i) { 
      *((std::ofstream*)this) << i;
      sofar += 4; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (unsigned int i) { 
      *((std::ofstream*)this) << i;
      sofar += 4; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (unsigned long int i) { 
      *((std::ofstream*)this) << i;
      sofar += 4; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (long int i) { 
      *((std::ofstream*)this) << i;
      sofar += 4; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (unsigned long long int i) { 
      *((std::ofstream*)this) << i;
      sofar += 4; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (long long int i) { 
      *((std::ofstream*)this) << i;
      sofar += 4; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (double d) { 
      *((std::ofstream*)this) << d;
      sofar += 8; //guess how long it might be...
      return *this;
   }

   LogStream& operator<< (float d) { 
      *((std::ofstream*)this) << d;
      sofar += 8; //guess how long it might be...
      return *this;
   }

   int ofLevel(LogLevelEnum i) { return ((int)(i) & log_level); }

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
   lstr << (const char*)("\n");
   lstr.flush();
   return lstr;
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
      (*this) << "\n";
      flush();
   }
}//log

#endif
