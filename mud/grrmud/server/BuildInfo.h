// $Id$
// $Revision: 1.2 $  $Author$ $Date$

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

#ifndef __BUILD_INFO_INCLUDE__
#define __BUILD_INFO_INCLUDE__


class BuildInfo {
public:
   static const char* getBuildMachine();
   static const char* getBuilder(); // $USER that built it
   static const char* getVersion(); // Code version, according to MakeInclude
   static const char* getBuildDate(); // as shown by `date`
   static const char* getRevision(); // Code revision, according to Subversion
};


#endif
