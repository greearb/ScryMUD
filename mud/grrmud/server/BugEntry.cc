// $Id: BugEntry.cc,v 1.13 2004/07/05 23:06:35 eroper Exp $
// $Revision: 1.13 $  $Author: eroper $ $Date: 2004/07/05 23:06:35 $

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
// Meaningless test comment added by angelbob: 7/26/99
//

#include "BugEntry.h"
#include "critter.h"
#include "misc2.h"

int BugEntry::_cnt = 0;
int BugCollection::_cnt = 0;


char* BugEntry::state_str[] = {
   "open",
   "assigned",
   "retest",
   "closed",
   "defer",
   "all",
   "none",
   "high_state",
  };

char* BugEntry::html_color[] = {
   "<font color=\"#000000\"> ", //open (black)
   "<font color=\"#0000cc\"> ", //assigned (blue)
   "<font color=\"#cc0000\"> ", //retest (red)
   "<font color=\"#00cc00\"> ", //closed (green)
   "<font color=\"#000000\"> ", //all (black)  //these last 3 not used now.
   "<font color=\"#000000\"> ", //none (black)
   "<font color=\"#000000\"> " //high_state (black)
  };

char* BugCollection::ct_strs[] = {
   "BUGS",
   "IDEAS"
};

///*************************************************************///
///********************* CommentEntry  *****************************///
///*************************************************************///

CommentEntry::CommentEntry(const char* d, const char* rr, const char* r)
      : date(d), reporter(rr), report(r) {
   html_safe_report = report.sterilizeForHtml();
   heg_safe_report = report;
   parse_communication(heg_safe_report);
}//constructor


int CommentEntry::read(ifstream& dafile) {
   char buf[100];
   int sent;
   dafile >> sent;
   if (sent == -1)
     return -1;

   date.Termed_Read(dafile);

   dafile >> reporter;
   dafile.getline(buf, 99);
   
   report.Termed_Read(dafile);

   html_safe_report = report.sterilizeForHtml();
   heg_safe_report = report;
   parse_communication(heg_safe_report);

   if (dafile)
     return 0;
   return -1;
}

int CommentEntry::write(ofstream& dafile) {
   dafile << 1 << endl; //sentinal
   dafile << date << "\n~\n";
   dafile << reporter << endl;
   dafile << report << "\n~\n";
   if (dafile)
     return 0;
   return -1;
}//

int CommentEntry::writeSentinal(ofstream& dafile) {
   dafile << -1 << endl;
   if (dafile)
     return 0;
   return -1;
}//writeSentinal

int CommentEntry::writeHtml(ofstream& dafile, const char* fnt) {
   dafile << "<tr><td nowrap>" << fnt << date << "</font></td> <td>" 
          << fnt << reporter << "</font></td> <td>" << fnt
          << html_safe_report << "</font></td></tr>" << endl;
   return 0;
}

String CommentEntry::toStringHeg() {
   String buf(200);
   Sprintf(buf, "<BUG_COMMENT_ENTRY \"%S\" %S>%S</BUG_COMMENT_ENTRY>",
           &date, &reporter, &heg_safe_report);
   return buf;
}


String CommentEntry::toString() {
   String buf(200);
   Sprintf(buf, "   %S: %S\n\t%S\n", &date, &reporter, &report);
   return buf;
}//toString



///*************************************************************///
///********************* BugEntry  *****************************///
///*************************************************************///

BugEntry::state BugEntry::getState(const String& fer_state) {
   if (fer_state.Strlen() == 0) {
      return all;
   }

   for (int i = 0; i<high_state; i++) {
      if (strcmp(fer_state, state_str[i]) == 0) {
         return (state)(i);
      }
   }//for

   return none;
}

BugEntry::BugEntry(const BugEntry& src) //copy constructor
   : bug_num(src.bug_num), room_num(src.room_num), cur_state(src.cur_state), 
     create_date(src.create_date), reporter(src.reporter),
     assigned_to(src.assigned_to), title(src.title),
     html_safe_title(src.html_safe_title),
     heg_safe_title(src.heg_safe_title) {
        
   reports.clearAndDestroy();

   Cell<CommentEntry*> cll(src.reports);
   CommentEntry* ptr;
   while ((ptr = cll.next())) {
      reports.append(new CommentEntry(*ptr));
   }
   _cnt++;
}//copy constructor


BugEntry::BugEntry(int bn, const char* _reporter, const char* _date, 
                   const char* _title, int rn)
      : bug_num(bn), room_num(rn), cur_state(open), 
        create_date(_date), reporter(_reporter),
        assigned_to("ADMIN"), title(_title) {

   html_safe_title = title.sterilizeForHtml();
   heg_safe_title = title;
   parse_communication(heg_safe_title);
   _cnt++;
}//Constructor

    
int BugEntry::clear() {
   bug_num = room_num = 0;
   cur_state = open;
   flags.Clear();
   create_date.Clear();
   reporter.Clear();
   assigned_to.Clear();
   reports.clearAndDestroy();
   title.Clear();
   html_safe_title.Clear();
   heg_safe_title.Clear();
   return 0;
}//clear

BugEntry& BugEntry::operator=(const BugEntry& src) {
   bug_num = src.bug_num;
   room_num = src.bug_num;
   cur_state = src.cur_state;
   flags = src.flags;
   create_date = src.create_date;
   reporter = src.reporter;
   assigned_to = src.assigned_to;
   title = src.title;
   html_safe_title = src.html_safe_title;
   heg_safe_title = src.heg_safe_title;
   reports.clearAndDestroy();
   
   Cell<CommentEntry*> cll(src.reports);
   CommentEntry* ptr;
   while ((ptr = cll.next())) {
      reports.append(new CommentEntry(*ptr));
   }
   return *this;
}//operatoe=


void BugEntry::appendComment(const CommentEntry& re) {
   reports.append(new CommentEntry(re));
}

void BugEntry::appendComment(const char* date, const char* report,
                             const char* reporter) {
   reports.append(new CommentEntry(date, reporter, report));
}

//does checking needed
int BugEntry::changeState(state new_state, int imm_level, const String& name) {
   
   if ((imm_level < 9) && (strcasecmp(name, assigned_to) != 0)) {
      return -EPERM;
   }

   // This is the only valid state transition for closed, so it's not in
   // the switch statement below...
   if (new_state == open) {
      cur_state = open;
      return 0;
   }

   // Don't restrict me cause I'm lazy!!
   if (imm_level > 5) {
      cur_state = new_state;
      return 0;
   }

   switch (cur_state)
     {
      case open:
         if ((new_state == assigned) || (new_state == defer)) {
            cur_state = new_state;
            return 0;
         }
         else {
            return -EINVAL;
         }
     case assigned:
        if (new_state == retest) {
           cur_state = retest;
           return 0;
        }
        else {
           return -EINVAL;
        }
     case retest:
         if (new_state == closed) {
           cur_state = closed;
           return 0;
        }
        else {
           return -EINVAL;
        }
     default:
         return -EINVAL;
     }//switch
}//stateTransition


int BugEntry::read(ifstream& dafile) {
   char buf[100];
   clear();

   dafile >> bug_num;

   // Terminator reached.
   if (bug_num == -1) {
      return -1;
   }

   int cs;
   dafile >> room_num >> cs;
   cur_state = (state)(cs);
   dafile.getline(buf, 99);

   flags.Read(dafile);

   dafile.getline(buf, 99);
   create_date = buf;
   create_date.Strip();

   dafile >> reporter;
   dafile.getline(buf, 99);

   dafile >> assigned_to;
   dafile.getline(buf, 99);

   title.Termed_Read(dafile);

   html_safe_title = title.sterilizeForHtml();
   heg_safe_title = title;
   parse_communication(heg_safe_title);

   CommentEntry re;
   while (re.read(dafile) >= 0) {
      reports.append(new CommentEntry(re));
   }
   return bug_num;
}

int BugEntry::write(ofstream& dafile) {
   dafile << bug_num << " " << room_num << " " << (int)cur_state
          << " bug_num, room_num, state\n";

   flags.Write(dafile);

   dafile << create_date << endl;
   dafile << reporter << endl;
   dafile << assigned_to << endl;

   dafile << title << "\n~\n";
   
   Cell<CommentEntry*> cll(reports);
   CommentEntry* ptr;
   while ((ptr = cll.next())) {
      ptr->write(dafile);
   }
   CommentEntry::writeSentinal(dafile);

   if (dafile)
     return 0;
   return -1;
};


// Write a table row entry...
int BugEntry::writeHtml(ofstream& dafile) {
   String buf(200);

   if (!reports.isEmpty()) {
      dafile << "\n<table width=\"95%\" border=3>\n"
         "<caption align=top><strong>";
   }
   else {
      dafile << "<center><b>\n";
   }

   dafile << html_color[cur_state];

   Sprintf(buf, "[%i] %P08[%s]%P17 <u>%S</u>  %P35By %S %P60Room: %i<br>\n<font size += 1>%S</font>",
           bug_num, state_str[cur_state], &create_date, &reporter,
           room_num, &html_safe_title);
   
   dafile << buf << "</font>\n";
   
   if (!reports.isEmpty()) {
      dafile << "</strong></caption><h4>\n";

      Cell<CommentEntry*> cll(reports);
      CommentEntry* ptr;
      int did_one = FALSE;
      while ((ptr = cll.next())) {
         did_one = TRUE;
         ptr->writeHtml(dafile, html_color[cur_state]);
      }
   
   
      dafile << "</h4></table>\n";
   }//if
   else {
      dafile << "</b></center>\n";
   }

   if (dafile)
     return 0;
   return -1;
};   


/** This is w/out hegemon markup */
String BugEntry::toString() {
   String retval(1000);

   if (reports.isEmpty()) {
      Sprintf(retval,
              "[%i] %P08[%s] %S\n\t  Submitted by %S on %S: in room _%i_.  Assigned to: %S\n",
              bug_num, state_str[cur_state], &title, &reporter, &create_date,
              room_num, &assigned_to);
   }
   else {
      Sprintf(retval,
              "* [%i] %P08[%s] %S\n\t  Submitted by %S on %S: in room _%i_.  Assigned to: %S\n",
              bug_num, state_str[cur_state], &title, &reporter, &create_date,
              room_num, &assigned_to);
   }

   Cell<CommentEntry*> cll(reports);
   CommentEntry* ptr;
   while ((ptr = cll.next())) {
      retval += ptr->toString();
   }

   return retval;
}//toString


/** This is w/out hegemon markup */
String BugEntry::toStringBrief() {
   String retval(1000);

   if (reports.isEmpty()) {
      Sprintf(retval, "[%i] %P08[%s] %S\n\t  Submitted by %S on %S: in room %i.  Assigned to: %S\n",
              bug_num, state_str[cur_state], &heg_safe_title, &reporter,
              &create_date, room_num, &assigned_to);
   }
   else {
      Sprintf(retval, "** [%i] %P08[%s] %S\n\t  Submitted by %S on %S: in room %i.  Assigned to: %S\n",
              bug_num, state_str[cur_state], &heg_safe_title, &reporter,
              &create_date, room_num, &assigned_to);
   }

   return retval;
}//toStringBrief


/** This is with hegemon markup */
String BugEntry::toStringHeg(const char* col_type) {
   String retval(1000);
   String buf(100);

   Sprintf(retval, "<BUG_TITLE>%S</BUG_TITLE>", &heg_safe_title);
   Sprintf(buf, "<BUG_ENTRY %i %s \"%S\" %S %i %S %s>",
           bug_num, state_str[cur_state], &create_date, &reporter, room_num,
           &assigned_to, col_type);

   retval += buf;

   Cell<CommentEntry*> cll(reports);
   CommentEntry* ptr;
   while ((ptr = cll.next())) {
      retval += ptr->toStringHeg();
   }

   retval += "</BUG_ENTRY>";

   return retval;
}//toStringHeg


int BugEntry::canComment(int imm_level, const String& name) {
   // Only way to comment on a closed one btw.
   if (imm_level >= 9) {
      return TRUE;
   }

   if (cur_state == assigned) {
      return (strcasecmp(name, assigned_to) == 0);
   }

   // retest and open can be commented on by anyone.
   return TRUE;
}//canComment


int BugEntry::reAssign(const String& new_name, int imm_level,
                       const String& name) {
   if ((imm_level >= 9) || (strcasecmp(assigned_to, name) == 0)) {
      if (new_name.Strlen() >= 3) {
         assigned_to = new_name;
         return 0;
      }
      else {
         return -EINVAL;
      }
   }
   else {
      return -EPERM;
   }
}//reAssign


///******************************************************************///
///********************* BugCollection  *****************************///
///******************************************************************///

const char* BugCollection::getColTypeName() {
   return ct_strs[type];
}

int BugCollection::write() {
   ofstream dafile(file_name);

   if (dafile) {
      dafile << "_VERSION_  1\n";
      dafile << next_bug_num << endl;

      Cell<BugEntry*> cll(bugs);
      BugEntry* ptr;
      while ((ptr = cll.next())) {
         ptr->write(dafile);
         dafile << endl;
      }//while

      dafile << -1 << " End of Bug Collection" << endl;
      return 0;
   }
   else {
      mudlog << "ERROR:  could not open BugCollection for writing, file -:"
             << file_name << endl;
      return -1;
   }
}//write


int BugCollection::writeHtml() {
   String tmp_name(file_name);
   tmp_name += ".html";

   ofstream dafile(tmp_name);

   if (dafile) {
      dafile << endl
         << "<HTML>" << endl
         << "<HEAD> " << endl
         << "<TITLE>  " << coll_name << " </TITLE>" << endl
         << "</HEAD>" << endl
         << "<BODY BGCOLOR=\"#FFFFFF\">" << endl
         << endl
         << "<CENTER>" << endl
         << "<H2>" << endl
         << "<a href=http://scry.wanfear.com>ScryMUD "
         << coll_name << "</a></H2></CENTER><P>" << endl
         << "This is an automagically generated list of <b>" << coll_name << "</b> entries"
         << "from ScryMUD.<P>" << endl
         << " <center>\n"
         "<a href=\"#open\">Open Issues</a> \n"
         "<a href=\"#assigned\">Assigned Issues</a> \n"
         "<a href=\"#retest\">Issues to Retest</a> \n"
         "<a href=\"#closed\">Closed Issues</a>\n"
         "<a href=\"#defer\">Deferred Issues</a>\n"
         "</center><P>\n";

      Cell<BugEntry*> cll(bugs);
      BugEntry* ptr;

      dafile << "<center>\n"
         "<a name=\"open\"></a>\n"
         "<h2><u>Open Issues</u></h2><br>\n"
         "<table width=\"100%\"border=3>\n";

      while ((ptr = cll.next())) {
         if (ptr->getState() == BugEntry::open) {
            dafile << "<tr><td>\n";
            ptr->writeHtml(dafile);
            dafile << "</td></tr><P>" << endl;
         }
      }//while

      dafile << "</table><P>\n"
         "<a name=\"assigned\"></a>\n"
         "<h2><u>Assigned Issues</u></h2>\n"
         "<table width=\"100%\"border=3>\n";

      bugs.head(cll);
      while ((ptr = cll.next())) {
         if (ptr->getState() == BugEntry::assigned) {
            dafile << "<tr><td>\n";
            ptr->writeHtml(dafile);
            dafile << "</td></tr><P>" << endl;
         }
      }//while

      dafile << "</table><P>\n"
         "<a name=\"retest\"></a>\n"
         "<h2><u>Issues In Retest</u></h2>\n"
         "<table width=\"100%\"border=3>\n";

      bugs.head(cll);
      while ((ptr = cll.next())) {
         if (ptr->getState() == BugEntry::retest) {
            dafile << "<tr><td>\n";
            ptr->writeHtml(dafile);
            dafile << "</td></tr><P>" << endl;
         }
      }//while


      dafile << "</table><P>\n"
         "<a name=\"defer\"></a>\n"
         "<h2><u>Issues that have been Deferred</u></h2>\n"
         "<table width=\"100%\"border=3>\n";

      bugs.head(cll);
      while ((ptr = cll.next())) {
         if (ptr->getState() == BugEntry::defer) {
            dafile << "<tr><td>\n";
            ptr->writeHtml(dafile);
            dafile << "</td></tr><P>" << endl;
         }
      }//while

      dafile << "</table><P>\n"
         "<a name=\"closed\"></a>\n"
         "<h2><u>Closed Issues</u></h2>\n"
         "<table width=\"100%\"border=3>\n";

      bugs.head(cll);
      while ((ptr = cll.next())) {
         if (ptr->getState() == BugEntry::closed) {
            dafile << "<tr><td>\n";
            ptr->writeHtml(dafile);
            dafile << "</td></tr><P>" << endl;
         }
      }//while

      dafile << "</table></center>\n\n<hr>\n"
         "<ADDRESS>\n"
         "Generated by:<A HREF=\"mailto:greear@cyberhighway.net\">ScryMUD Admin</A>\n"
         "</ADDRESS></body></html>\n";

      dafile << flush;
      dafile.close();

      String cmd(50);
      Sprintf(cmd, "./mv_logs.bash %S", &tmp_name);
      system(cmd);

      return 0;
   }
   else {
      mudlog << "ERROR:  could not open BugCollection html file -:"
             << tmp_name << endl;
      return -1;
   }
}//writeHtml


int BugCollection::read() {
   char buf[100];
   ifstream dafile(file_name);
   int cnt = 0;
   int version_num;
   String version;

   if (dafile) {
      dafile >> version >> version_num;

      if (strcmp(version, "_VERSION_") != 0) {
         mudlog << "ERROR:  BugCollection file -:" << file_name
                << ":- is an incompatible version.\n  It will be ignored "
                << "and destroyed the first time a change is made to the "
                << "bug list.  I'll be able to handle Version Conversion\n"
                << "better next time...\n";
         cerr  << "ERROR:  BugCollection file -:" << file_name
               << ":- is an incompatible version.\n  It will be ignored "
               << "and destroyed the first time a change is made to the "
               << "bug list.  I'll be able to handle Version Conversion\n"
               << "better next time...\n";
      }

      dafile >> next_bug_num;
      dafile.getline(buf, 99);

      BugEntry be;

      while (dafile && (be.read(dafile) != -1)) {
         cnt++;
         bugs.append(new BugEntry(be));
      }
      return cnt;
   }//if
   else {
      return -1;
   }
}//read
   

BugEntry* BugCollection::getBugEntry(int bug_num) {
   Cell<BugEntry*> cll(bugs);
   BugEntry* ptr;
   while ((ptr = cll.next())) {
      if (bug_num == ptr->getBugNum()) {
         return ptr;
      }
   }//while
   return NULL;
}

int BugCollection::changeState(int bug, const String& new_state, int imm_level,
                               const String& name) {
   BugEntry* be = getBugEntry(bug);
   BugEntry::state s = BugEntry::getState(new_state);
   if (be) {
      int retval = be->changeState(s, imm_level, name);
      if (retval >= 0)
        notifyModified();
      return retval;
   }
   return -EINVAL;
}

int BugCollection::reAssign(int bug, const String& new_name, int imm_level,
                            const String& name) {
   BugEntry* be = getBugEntry(bug);
   if (be) {
      int retval = be->reAssign(new_name, imm_level, name);
      if (retval >= 0)
        notifyModified();
      return retval;
   }
   return -EINVAL;
}

int BugCollection::canComment(int bug, int imm_level, const String& name) {
   BugEntry* be = getBugEntry(bug);
   if (be) {
      return be->canComment(imm_level, name);
   }
   return FALSE;
}

int BugCollection::notifyModified() {
   this->write();
   this->writeHtml();
   return 0;
}

int BugCollection::addComment(int bug, const CommentEntry& re, int imm_level,
                              const String& name) {
   BugEntry* be = getBugEntry(bug);
   if (be) {
      if (be->canComment(imm_level, name)) {
         be->appendComment(re);
         notifyModified();
         return 0;
      }
      else {
         return -EINVAL;
      }
   }//if
   else {
      return -EINVAL;
   }
}//addComment


int BugCollection::purgeBug(int num) {
   BugEntry* be = getBugEntry(num);
   if (be) {
      bugs.loseData(be);
      delete be;
      notifyModified();
      return 0;
   }//if we found it
   return -EINVAL;
}

int BugCollection::addBug(BugEntry* be) {
   if (be) {
      bugs.append(be);
      notifyModified();
      return 0;
   }
   return -EINVAL;
}

int BugCollection::addBug(const char* cur_date, int rnum,
                          const String& reporter, const String& report) {
   bugs.append(new BugEntry(next_bug_num, reporter, cur_date, report, rnum));
   next_bug_num++;

   notifyModified();
   return 0;
}


String BugCollection::toString(const String& in_state, int use_heg_markup) {
   String buf(10000);
   
   BugEntry::state s = BugEntry::getState(in_state);

   Cell<BugEntry*> cll(bugs);
   BugEntry* ptr;
   while ((ptr = cll.next())) {
      if ((s == BugEntry::all) || (s == ptr->getState())) {
         if (use_heg_markup) {
            buf.Append(ptr->toStringHeg(ct_strs[type]));
         }
         else {
            buf.Append(ptr->toString());
         }
         buf.Append("\n");
      }
   }
   return buf;
}


String BugCollection::toStringBrief(const String& in_state) {
   String buf(10000);
   
   BugEntry::state s = BugEntry::getState(in_state);

   Cell<BugEntry*> cll(bugs);
   BugEntry* ptr;
   while ((ptr = cll.next())) {
      if ((s == BugEntry::all) || (s == ptr->getState())) {
         buf.Append(ptr->toStringBrief());
      }
   }
   return buf;
}
