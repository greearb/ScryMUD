// $Id: KVPair.h,v 1.2 1999/06/05 23:29:16 greear Exp $
// $Revision: 1.2 $  $Author: greear $ $Date: 1999/06/05 23:29:16 $

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

#ifndef BEN_KVPAIR_H
#define BEN_KVPAIR_H


/**  Represents a key-value pair.  Key must define the comparison
 * operators (==, <, >).  Both should define at the = operator.
 */
template <class KEY, class VAL> class KVPair {
public:
   KEY key;
   VAL val;

   KVPair(const KEY& k, const VAL& v) : key(k), val(v) { }; //constructor
   KVPair() : key(0), val(0) { };

   ~KVPair() { };

   int operator==(const KVPair<KEY, VAL> src) {
      return key == src.key;
   }

   int operator>(const KVPair<KEY, VAL> src) {
      return key > src.key;
   }

   int operator<(const KVPair<KEY, VAL> src) {
      return key < src.key;
   }

   KVPair<KEY, VAL>& operator=(const KVPair<KEY, VAL> src) {
      key = src.key;
      val = src.val;
   }
      
};//KVPair.h

#endif
