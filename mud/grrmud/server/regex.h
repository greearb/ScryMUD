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

#ifndef GRRMUD_REGEX_INCLUDE_H
#define GRRMUD_REGEX_INCLUDE_H

#include <string2.h>
#include <pcre.h>

class regex {
   protected:
      String err_msg;
      String pattern;
      pcre *re;
      //TODO: add a ptrarray of Strings for submatch contents
   public:
      regex();
      ~regex();
      regex* operator=(String &new_pat);
      bool operator==(String &match_str);
      bool operator==(const char* match_str);
      String &getErr(void) { return err_msg; }
};

#endif
