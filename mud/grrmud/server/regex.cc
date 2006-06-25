// $Id$
// $Revision$  $Author$ $Date$

//
//ScryMUD Server Code
//Copyright (C) 2005  Edward Roper
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
// To contact the Author, Edward Roper: eroper@wanfear.com
//

#include "regex.h"

regex::regex() {
   re = NULL;
   err_msg.clear();
   pattern.clear();
}//regex::regex

regex::~regex() {
   if ( re ) {
      pcre_free(re);
   }
}//regex::~regex

regex *regex::operator=(String &new_pat) {
   const char *err;
   int err_offset;

   re = pcre_compile(
         (const char*)new_pat,
         0,
         &err,
         &err_offset,
         NULL);
   if (re == NULL) {
      Sprintf(err_msg, "REGEX compilation failed at %d: %s\n",
            err_offset, err);
      return NULL;
   }
   pattern = new_pat;
   return this;
}//regex::operator=

bool regex::operator==(const char* match_str) {

   int rc;

   //no pattern initialized
   if ( !re ) {
      return false;
   }

   rc = pcre_exec(
         re,
         NULL,
         match_str,
         strlen(match_str),
         0,
         0,
         NULL,
         0);

   if ( rc < 0 ) {
      return false;
   }
   return true;
}//regex::operator==

bool regex::operator==(String &match_str) {
   return (*this == (const char*)match_str);
}//regex::operator==
