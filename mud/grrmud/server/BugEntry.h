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

#ifndef __BUG_ENTRY_INCLUDE_GRRMUD__
#define __BUG_ENTRY_INCLUDE_GRRMUD__

#include "critter.h"
#include <errno.h>


class CommentEntry {
public:
   String date;
   String reporter;
   String report;
   String html_safe_report; //not saved to disk
   String heg_safe_report; //not saved to disk

   CommentEntry() { }; //Strings initialize themselves...
   CommentEntry(const char* d, const char* rr, const char* r);

   CommentEntry(const CommentEntry& src)
     : date(src.date), reporter(src.reporter), report(src.report),
      html_safe_report(src.html_safe_report),
      heg_safe_report(src.heg_safe_report) { }

   int read(ifstream& dafile);
   int write(ofstream& dafile);
   int writeHtml(ofstream& dafile, const char* font);
   static int writeSentinal(ofstream& dafile);

   String toString();
   String toStringHeg(); //Hegemon markup
};

class BugEntry {
public:

   enum state {
      open,
      assigned,
      retest,
      closed,
      defer,
      all,
      none,
      high_state
   };

   BugEntry()
         : bug_num(0), room_num(0) {
      _cnt++;
   }

   BugEntry(int bn, const char* _reporter, const char* _date, 
            const char* _title, int rn);

   /** Copy constructor */
   BugEntry(const BugEntry& src);
   BugEntry& operator=(const BugEntry& src);
   ~BugEntry() { _cnt--; reports.clearAndDestroy(); }

   int clear();
   int read(ifstream& dafile);
   int write(ofstream& dafile);
   int writeHtml(ofstream& dafile);

   //does checking needed
   int changeState(state new_state, int imm_level, const String& name);

   String toStringBrief();
   String toString();
   String toStringHeg(const char* col_name);

   int getBugNum() const { return bug_num; }
   int getRoomNum() const { return room_num; }
   String& getReporter() { return reporter; }
   String& getCreateDate() { return create_date; }
   state getState() const { return cur_state; }

   void setBugNum(int i) { bug_num = i; }
   void appendComment(const CommentEntry& re);
   void appendComment(const char* date, const char* report,
                      const char* reporter);

   int canComment(int imm_level, const String& name);
   int reAssign(const String& new_name, int imm_level, const String& name);

   BugEntry* getBugEntry(int bug_num);

   static state getState(const String& fer_state);
   static int getInstanceCount() { return _cnt; }
   static const char* state_str[];
   static const char* html_color[];

protected:
   int bug_num;
   int room_num;
   state cur_state;
   bitfield flags; /* none defined yet */
   String create_date; //it was created.
   String reporter;
   String assigned_to; /* name of person asigned to fix it */
   PtrList<CommentEntry> reports;
   String title;

   String html_safe_title; //not saved to disk
   String heg_safe_title; //not saved to disk

private:
   static int _cnt;

};


class BugCollection {
public:
   enum CollectionType {
      BUGS,
      IDEAS
   };

private:
   static int _cnt;

protected:
   int next_bug_num;
   String file_name;
   String coll_name;
   CollectionType type;
   PtrList<BugEntry> bugs;

   static const char* ct_strs[];

   // Causes it to write it's self to disk for storage and in html.
   int notifyModified();

public:

   BugCollection(const char* fname, const char* _coll_name,
                 CollectionType ct) 
         : next_bug_num(0), file_name(fname), coll_name(_coll_name),
           type(ct) {
      _cnt++;
   }

   ~BugCollection() { _cnt--; bugs.clearAndDestroy(); }

   String& getFileName() { return file_name; }

   static int getInstanceCount() { return _cnt; }
   const char* getColTypeName();

   int read();
   int write();
   int writeHtml();

   BugEntry* getBugEntry(int bug_num);

   int canComment(int bug, int imm_level, const String& name);
   int changeState(int bug, const String& new_state, int imm_level,
                   const String& name);
   int reAssign(int bug, const String& new_name, int imm_level,
                const String& name);
   int purgeBug(int num);
   int addBug(const char* cur_time, int rnum, const String& reporter,
              const String& report);

   int addBug(BugEntry* be);
   int addComment(int bug, const CommentEntry& re, int imm_level,
                  const String& name);
   String toString(const String& in_state, int use_heg_markup);
   String toStringBrief(const String& in_state);

};

#endif
