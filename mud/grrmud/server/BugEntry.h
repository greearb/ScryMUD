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

#ifndef __BUG_ENTRY_INCLUDE_GRRMUD__
#define __BUG_ENTRY_INCLUDE_GRRMUD__

#include "critter.h"


class BugEntry {
private:
   static int _cnt;

protected:
   int bug_num;
   String reporter;
   String date;
   String report;
   int room_num;

public:
   BugEntry()
         : bug_num(0), room_num(0) {
      _cnt++;
   }

   BugEntry(int bn, const char* _reporter, const char* _date, 
            const char* _report, int rn)
         : bug_num(bn), reporter(_reporter), date(_date),
           report(_report), room_num(rn) {
      _cnt++;
   }

   /** Copy constructor */
   BugEntry(const BugEntry& src)
         : bug_num(src.bug_num), reporter(src.reporter),
           date(src.date), report(src.report), room_num(src.room_num) {
      _cnt++;
   }
    
   //operator=(const BugEntry& src);  //not needed at this time.

   ~BugEntry() { _cnt--; }

   int read(ifstream& dafile);
   int write(ofstream& dafile);
   int writeHtml(ofstream& dafile);

   String toStringBrief();
   String toString();

   int getBugNum() const { return bug_num; }
   int getRoomNum() const { return room_num; }
   String& getReporter() { return reporter; }
   String& getReport() { return report; }
   String& getDate() { return date; }

   void setBugNum(int i) { bug_num = i; }
   void appendNote(const String& note);

   static int getInstanceCount() { return _cnt; }
};


class BugCollection {
private:
   static int _cnt;

protected:
   int next_bug_num;
   String file_name;
   String coll_name;
   List<BugEntry*> bugs;

public:
   BugCollection(const char* fname, const char* _coll_name) 
         : next_bug_num(0), file_name(fname), coll_name(_coll_name) {
      _cnt++;
   }

   ~BugCollection() { _cnt--; }

   String& getFileName() { return file_name; }

   static int getInstanceCount() { return _cnt; }

   int read();
   int write();
   int writeHtml();

   int removeBug(int num, const String& name, const String& notes,
                 BugCollection& completed_bc);
   int addBug(const char* cur_time, int rnum, const String& reporter,
              const String& report);

   int addBug(BugEntry* be);

   String toString(); //print em all out
};

#endif
