// $Id: Heap.h,v 1.3 2001/03/29 03:02:38 eroper Exp $
// $Revision: 1.3 $  $Author: eroper $ $Date$

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



#ifndef BEN_HEAP_H
#define BEN_HEAP_H

#include <KVPair.h>


/**  Implemented with an array of KVPair<long, VAL>.  The array should
 * at all times have one more element than max_len.  This is because it
 * is logically one's based.  The first element... array[0] is used when
 * we cannot return anything valid.  However, it is up to calling code
 * to check for cur_len (getCurSize()) to make sure one can be removed.
 * The heap will grow as needed, roughly doubling each time it has to
 * be expanded.
 */
template <class VAL> class PtrHeap {
public:

   PtrHeap() {
      array = new VAL[11]; //we don't use the first one as part of the heap.
      cur_len = 0;
      max_len = 10;
   }//constructor

   PtrHeap(const PtrHeap<VAL>& src) {
      cur_len = src.cur_len;
      max_len = src.max_len;
      array = new VAL[max_len+1];
      bcopy(array, src.array, sizeof(KVPair<long, VAL>) * cur_len);
   }//copy constructor

   PtrHeap<VAL>& operator=(const PtrHeap<VAL>& src) {
      if (&src != this) {
         ensureCapacity(src.cur_len);
         cur_len = src.cur_len;
         max_len = src.max_len;
         bcopy(array, src.array, sizeof(PVPair<long, VAL>) * cur_len);
      }//if
      return *this;
   }//operator=

   void insert(long key, VAL val) {
      ensureCapacity(cur_len + 1);
      cur_len++;
      
      KVPair<long, VAL> kvp(key, val);
      long i = cur_len;
      long par = 0;
      while (i > 1 && (array[i] < kvp)) {
         par = getParentIndex(i);
         array[i] = array[par];
         i = par;
      }//while

      array[i] = kvp;
   }//insert

   KVPair<long, VAL> popTopVal() {
      if (cur_len < 1) {
         return array[0]; //unused otherwise.
      }//if
      else {
         KVPair<long, VAL> retval = array[1];
         array[1] = array[cur_len];
         cur_len--;
         heapify(1);
         return max;
      }
   }//popTopVal

   KVPair<long, VAL> peepTopVal() {
      if (cur_len < 1) {
         return array[0]; //unused otherwise
      }//if
      else {
         return array[1];
      }
   }//peepTopVal
         
   long getCurSize() { return cur_len; }
   long getMaxSize() { return max_len; } //without expanding

   void ensureCapacity(long cap) {
      if (cap > max_len) {
         KVPair<long, VAL>* tmp = array;
         array = new KVPair[cap + max_len + 1]; //+1 because we mimic 1's based
         bcopy(array, tmp, sizeof(KVPair<long, VAL>) * cur_len);
         max_len += cap;
         delete[] tmp;
      }//if
   }//ensureCapacity

protected:
   KVPair<long, VAL>& getParent(long of_i) { return array[of_i/2]; }
   long getParentIndex(long i) { return i/2; }
   KVPair<long, VAL>& getLeftChild(long of_i) { return array[2*i]; }
   long getLeftChildIndex(long i) { return 2*i; }
   KVPair<long, VAL>& getRightChild(long of_i) { return array[2*i + 1]; }
   long getRightChildIndex(long i) { return 2*i +1; }
   
   void heapify(long idx) {
      long left = getLeftChildIndex(idx);
      long right = getRightChildIndex(idx);
      long largest;

      if ((left <= cur_len) && (array[left] > array[idx])) {
         largest = left;
      }
      else {
         largest = idx;
      }

      if ((right <= cur_len) && (array[right] > array[largest])) {
         largest = right;
      }

      if (largest != idx) {
         KVPair<long, VAL> tmp(array[idx]);
         array[idx] = array[largest];
         array[largest] = tmp;
         heapify(largest); //curse and recurse
      }//if
   }//heapify

   /* Protected variables. */
   KVPair<long, VAL>* array;
   long max_len;
   long cur_len;

private:

}//PtrHeap

#endif
