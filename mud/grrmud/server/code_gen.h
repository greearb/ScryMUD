// $Id$
// $Revision: 1.7 $  $Author$ $Date$

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

#ifndef __CODE_GEN_H__
#define __CODE_GEN_H__

#include <string2.h>
#include <fstream.h>
#include <list2.h>

#define ALIASES_ARRAY_LEN 50


/** Only change these two things to add a new language, and of course
 * the server code may need changes too.
 */
#define NUM_LANGUAGES 8

char* languages[NUM_LANGUAGES] = {
   "English",
   "Spanish",
   "Portugues",
   "Italian",
   "Serbian",
   "German",
   "Polish",
   "French"
};


class GenCmdInput {
public:
   String aliases[ALIASES_ARRAY_LEN];
   String help;
   String cmd;

   int read(ifstream& dafile);
   void clear();
   void print();
};

class LVPair {
public:
   LVPair(String l, String v) : lang(l), val(v) { }
   String lang;
   String val;
};

class LanguageEntry {
private:
   LanguageEntry& operator=(const LanguageEntry& src);

protected:
   String enum_name;
   PtrList<LVPair> vals;

public:
   LanguageEntry() {;}
   LanguageEntry(const LanguageEntry& src);
   void clear();
   int read(ifstream& dafile);
   
   String getEnumName() const { return enum_name; }
   
   String getConstArray() const;
};
   

int code_gen_language(char** argv, int argc);
int code_gen_commands(char** argv, int argc);

#endif
