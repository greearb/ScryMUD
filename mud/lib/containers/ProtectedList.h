// $Id: ProtectedList.h,v 1.2 1999/06/05 23:29:16 greear Exp $
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

// PtrArray

#ifndef GRRMUD_PROTECTED_LIST_H
#define GRRMUD_PROTECTED_LIST_H

#include <list2.h>

extern int core_dump(const char* msg);

/**  Assumed to contain pointers to objects which implement
 * a method:  getIdNum(), which will return a (unique) integer
 * identifier for all objects of this type in the 'universe'.
 */
template <class T> class ProtectedList {
private:
   unsigned char locked;
   List<T> lst;

   void lock() {
      locked = 1;
   }

   int Assert(const int boolean_val) const {
      if (!boolean_val)
         ::core_dump("ProtectedList.h");
      return TRUE;
   }

public:
   ProtectedList() {
      locked = 0;
   }
   
   const List<T>& getLst() {
      return lst;
   }

   void append(const T& arg) {
      //Assert(!locked);
      lst.append(arg);
   }
   
   void prepend(const T& arg) {
      //Assert(!locked);
      lst.prepend(arg);
   }
   
   int size() const {
      return lst.size();
   }
   
   // does not delete objects in list
   void clear() {
      Assert(!locked);
      lst.clear();
   }

   int isEmpty() const {
      return lst.isEmpty();
   }
   
   T find(const T& val) const {
      //Assert(!locked);
      /*
       *list<T>::iterator it = lst.begin();
       *for(; it != lst.end(); ++it) {
       *  if (*it == val)
       *    return *it;
       *}
       */
      Cell<T> cell(lst);
      T ptr;
      while ((ptr = cell.next())) {
         if (ptr == val)
            return ptr;
      }
      return (T)(0);
   }

   int haveData(const T& val) {
      return (find(val) != (T)(0));
   }
      
   T lose(Cell<T>& cll) {
      return lst.lose(cll);
   }

   void head(Cell<T>& cll) {
      getBeginIterator(cll);
   }

   void getBeginIterator(Cell<T>& cll) {
      Assert(!locked);
      lock();
      lst.head(cll);
   }

   /*
    *iterator isEnd(iterator& i) {
    *  return (i == lst.end());
    *}
    */

   void unlock() {
      locked = 0;
   }

   int isLocked() const {
      return locked;
   }

   /*
    *void unique() {
    *  Assert(!locked);
    *  lst.unique();
    *}
    */

   void gainData(const T& val) {
      //Assert(!locked);
      lst.gainData(val);
   }

   void loseData(const T& val) {
      remove(val);
   }

   void remove(const T& val) {
      Assert(!locked);
      lst.loseData(val);
   }

   T peekFront() const {
      return lst.peekFront();
   }

   T peekRear() const {
      return lst.peekRear();
   }

   T popBack() {
      Assert(!locked);
      return lst.popBack();
   }

   T popFront() {
      Assert(!locked);
      return lst.popFront();
   }

   void clearPtrList() {
      Assert(!locked);
      clear_ptr_list(lst);
   }

};//ProtectedList<T>

#endif
