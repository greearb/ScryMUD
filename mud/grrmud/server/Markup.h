// $Id: Markup.h,v 1.2 1999/08/16 00:37:06 greear Exp $
// $Revision: 1.2 $  $Author: greear $ $Date: 1999/08/16 00:37:06 $

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


#ifndef __CLIENT_MARKUP_H__
#define __CLIENT_MARKUP_H__

#include <bitfield.h>


template <class T> class SafeList;
template <class T> class PtrList;
class critter;
class object;
class StatBonus;
class SpellDuration;

class Markup {
public:
   static void toString(const char* pre, const bitfield& flags,
                        const BitfieldNames& flag_names, critter* pc,
                        const char* post, String& rslt);

   static void toString(SafeList<critter*>* lst, critter* viewer, String& rslt);

   static void toString(SafeList<object*>* lst, critter* viewer, String& rslt);

   static void toString(PtrList<StatBonus>* lst, critter* viewer, String& rslt);

   static void toString(PtrList<SpellDuration>* lst, critter* viewer, String& rslt);

};


#endif