// $Id: parse.h,v 1.7 1999/06/05 23:29:15 greear Exp $
// $Revision: 1.7 $  $Author: greear $ $Date: 1999/06/05 23:29:15 $

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

#ifndef PARSE_INCLUDE_H
#define PARSE_INCLUDE_H

#include <string2.h>
#include "classes.h"
#include "parse_gen.h"

#define RAW_MAX 20
#define COOKED_MAX 20

String parse_hlp_command(const String& str1);
//int process_input(critter& pc, String& input, short do_sub);


class CmdSpecifier {
private:
   static int _cnt;

protected:
   String word;
   String hlp_key;
   int len;
   CmdId id;
   CmdSpecifier* next;

public:
   CmdSpecifier(const char* w, int l, CmdId cmd_id, const char* help_key)
         : word(w), hlp_key(help_key), len(l), id(cmd_id), next(NULL)
      { _cnt++; word.Tolower(); }
   CmdSpecifier(const char* w, int l) 
         : word(w), len(l), id(ci_UNKNOWN), next(NULL)
      { _cnt++; word.Tolower(); }
   CmdSpecifier() 
         : len(0), id(ci_UNKNOWN), next(NULL)
      { _cnt++; word.Tolower(); }
   ~CmdSpecifier() { _cnt--; }

   // If *this is longer than src, then it will NEVER match.
   // But, if it is equal or lesser, it may match.
   int doesSatisfyMatch(const CmdSpecifier& src) const {
      if (len > src.len) {
         return 0;
      }
      return *this == src;
   }

   String toString() const;

   int operator==(const CmdSpecifier& src) const {
      return (strncasecmp(word, src.word, min(len, src.len)) == 0);
   }
   
   int operator<(const CmdSpecifier& src) {
      return (strncasecmp(word, src.word, max(len, src.len)) < 0);
   }

   int operator>(const CmdSpecifier& src) {
      return (strncasecmp(word, src.word, max(len, src.len)) > 0);
   }

   //unsigned int hash() { return word.hash(); }
   String getHelpKey() const { return hlp_key; }
   CmdId getId() const { return id; }
   CmdSpecifier* getNext() const { return next; }
   String getCmd() const { return word; }
   void setNext(CmdSpecifier* ptr) { next = ptr; }

   static int getInstanceCount() { return _cnt; }
};


   
#define CMD_ARRAY_LEN 26

class CmdCollection {
protected:
   CmdSpecifier* cmd_specifiers[CMD_ARRAY_LEN];

public:
   CmdCollection();

   void addCmdSpecifier(const CmdSpecifier* cs);
   void addCmdSpecifierNoCreate(CmdSpecifier* cs);

   // can return NULL
   const CmdSpecifier* findSpecifierFor(const CmdSpecifier* cs,
                                        int cmd_only);

   int calculateIndex(char val) {
      //convert to upper, subtract A, then hash
      return (abs(toupper(val) - 'A') % CMD_ARRAY_LEN);
   }

};

extern CmdCollection cmds_collection;

#endif
