// $Id$
// $Revision$  $Author$ $Date$

//
//Copyright (C) 2001  Ben Greear
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU Library General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU Library General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//To contact the maintainer, Edward Roper: edro+scrymud [at] wanfear.net
//

// PtrArrayBase, PtrArray, LazyPtrArray

#include "PtrArray.h"

///**********************************************************************///
///*********************  PtrArrayBase  *********************************///
///**********************************************************************///

template <class T>
PtrArrayBase<T>::PtrArrayBase(int mx) {
   if (mx < 0)
      mx = 0;

   max_len = mx;
   cur_len = 0;
   ptr_list = new T*[mx + 1];

   memset(ptr_list, 0, ((mx + 1) * sizeof(T*)));

   //for (int i = 0; i<mx; i++) {
   //   ptr_list[i] = NULL;
   //}
}//constructor


template <class T>
PtrArrayBase<T>::~PtrArrayBase() {
   delete[] ptr_list;
}

// template <class T>
// PtrArrayBase<T>& PtrArrayBase<T>::operator=(const PtrArrayBase<T>& src) {
//    if (this == &src)
//       return *this;

//    ensureCapacity(src.max_len);

//    bcopy(ptr_list, src.ptr_list, (src.max_len * sizeof(T*)));
//    memcpy(ptr_list + (max_len * sizeof(T*)), 0, 
//           ((max_len - src.max_len) * sizeof(T*)));

//    cur_len = src.cur_len;

//    return *this;
// }

// Virtually does a clearAndDestroy on Self!!!!!!
template <class T>
void PtrArrayBase<T>::becomeDeepCopyOf(const PtrArrayBase<T>& src) {
   
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In becomeDeepCopy, src.cur_len:  " << src.getCurLen()
             << " cur_len: " << cur_len << " max_len:  " << max_len
             << endl;
   }

   ensureCapacity(src.getCurLen());
   
   int i;
   for (i = 0; i<src.getCurLen(); i++) {
      if (ptr_list[i]) {
         mudlog << "Deleting one, idx: " << i << endl;
         delete ptr_list[i];
      }
      
      if (src.ptr_list[i]) {
         //mudlog << "Source existed for idx: " << i << endl << flush;
         ptr_list[i] = new T(*(src.ptr_list[i]));
         cur_len = i + 1;
      }
      else {
         ptr_list[i] = NULL;
      }
   }//for

   //mudlog << "Cleaning up remainder..." << endl;
   for (; i<getMaxLen(); i++) {
      if (ptr_list[i]) {
         //mudlog << "Deleting idx:  " << i << endl;
         delete ptr_list[i];
      }
      ptr_list[i] = NULL;
   }//for
}//becomeDeepCopyOf


// If it's not already in the array,
// find the first open slot and dump it in...
template <class T>
int PtrArrayBase<T>::gainData(T* val) {
   int open = -1;
   for (int i = 0; i<cur_len; i++) {
      if (ptr_list[i] == val) {
         return FALSE; //already in there
      }//if
      else if ((open < 0) && !ptr_list[i]) {
         open = i;
      }
   }//for

   if (open >= 0) {
      ptr_list[open] = val;
   }
   else {
      appendShallowCopy(val);
   }//else
   return TRUE;
}//gainData


// Return TRUE if val is in the array, false otherwise.
template <class T>
int PtrArrayBase<T>::haveData(const T* val) {
   for (int i = 0; i<cur_len; i++) {
      if (ptr_list[i] == val) {
         return TRUE;
      }//if
   }//for

   return FALSE;
}


// Find the first instance and remove it.
template <class T>
int PtrArrayBase<T>::loseData(const T* val) {
   for (int i = 0; i<cur_len; i++) {
      if (ptr_list[i] == val) {
         set((T*)NULL, i); //take care of cur_len if needed
         return TRUE;
      }//if
   }//for
   return FALSE;
}//loseData   


//  This copies the data from the incomming src, not just a shallow copy.
//  Does not copy NULL's over.
template <class T>
int PtrArrayBase<T>::appendDeepCopy(const PtrArrayBase<T>& src) {
   for (int i = 0; i<src.getCurLen(); i++) {
      if (src.ptr_list[i]) {
         appendDeepCopy(*(src.ptr_list[i]));
      }
   }//if
   return TRUE;
}//appendDeepCopy


//  This copies the data from the incomming src, not just a shallow copy.
//  Does not copy NULL's over.
template <class T>
int PtrArrayBase<T>::appendShallowCopy(const PtrArrayBase<T>& src) {
   for (int i = 0; i<src.getCurLen(); i++) {
      if (src.ptr_list[i]) {
         appendShallowCopy(src.ptr_list[i]);
      }
   }//if
   return TRUE;
}//appendDeepCopy


template <class T>
int PtrArrayBase<T>::appendShallowCopy(T* val) {
   ensureCapacity(cur_len + 1);

   ptr_list[cur_len] = val;
   cur_len++;

   return TRUE;
}

template <class T>
int PtrArrayBase<T>::appendDeepCopy(T& val) {
   ensureCapacity(cur_len + 1);

   ptr_list[cur_len] = new T(val);
   cur_len++;

   return TRUE;
}

// does NOT delete pointers
template <class T>
void PtrArrayBase<T>::clear() {
   for (int i = 0; i<cur_len; i++) {
      ptr_list[i] = NULL;
   }
   cur_len = 0;
}//clear


template <class T>
void PtrArrayBase<T>::clearAndDestroy() {

   for (int i = 0; i<cur_len; i++) {
      delete ptr_list[i];
   }

   delete[] ptr_list;
   max_len = 10;
   cur_len = 0;
   ptr_list = new T*[max_len + 1];
   
   for (int i = 0; i <= max_len; i++) {
      ptr_list[i] = NULL;
   }
}      

// we won't allow it to grow shorter!
template <class T>
void PtrArrayBase<T>::ensureCapacity(int i) {

   //mudlog << "ensureCapacity, max_len: " << max_len
   //       << " i: " << i << endl;

   if (i < 0)
      i = 0;
   
   if (i < max_len)
      return;

   T** new_vals = new T*[i + max_len];
   
   int j;
   for (j = 0; j<cur_len; j++) {
      new_vals[j] = ptr_list[j];
   }
   max_len += i;

   for (;j<max_len; j++) {
      new_vals[j] = NULL;
   }
   
   delete[] ptr_list;
   
   ptr_list = new_vals;
}

template <class T>
int PtrArrayBase<T>::getMaxLen() const {
   return max_len;
}

template <class T>
int PtrArrayBase<T>::getCurLen() const {
   return cur_len;
}

template <class T>
T* PtrArrayBase<T>::elementAt(int i) {
   if ((i < 0) || (i >= max_len))
      return NULL;

   return ptr_list[i];
}//elementAt


template <class T>
const T* PtrArrayBase<T>::constElementAt(int i) const {
   if ((i < 0) || (i >= max_len))
      return NULL;

   return ptr_list[i];
}//constElementAt


// return TRUE if it worked..false otherwise
template <class T>
int PtrArrayBase<T>::setAndDestroy(T* val, int posn) {
   if (posn < 0)
      return FALSE;

   ensureCapacity(posn + 1);
   
   if (ptr_list[posn])
      delete ptr_list[posn];

   if (val && (posn >= cur_len)) {
      cur_len = posn + 1;
   }
   else if (!val) {
      if (posn == (cur_len - 1)) {
         int old_cur_len = cur_len;
         cur_len = 0;
         for (int i = old_cur_len - 2; i>=0; i--) {
            cur_len = i + 1;
            if (ptr_list[i]) {
               break;
            }//if
         }//for
      }//if
   }//else

   ptr_list[posn] = val;
   return TRUE;
}

template <class T>
int PtrArrayBase<T>::set(T* val, int posn) {
   if (posn < 0)
      return FALSE;

   ensureCapacity(posn + 1);
   
   if (val && (posn >= cur_len)) {
      cur_len = posn + 1;
   }
   else if (!val) {
      if (posn == (cur_len - 1)) {
         int old_cur_len = cur_len;
         cur_len = 0;
         for (int i = old_cur_len - 2; i>=0; i--) {
            cur_len = i + 1;
            if (ptr_list[i]) {
               break;
            }//if
         }//for
      }//if
   }//else

   ptr_list[posn] = val;
   return TRUE;
}


///**********************************************************************///
///*********************  LazyPtrArray  *********************************///
///**********************************************************************///


// NOTE:  if elementAt returns null, this will SEGV, be carefull!
template <class T>
T& LazyPtrArray<T>::operator[] (const int i) {
//   cout << "PtrArrayBase::[" << i << "]  max_len:  " << max_len << endl;
   return *(elementAt(i));
}
      

template <class T>
T* LazyPtrArray<T>::elementAtNoCreate(int i) {
   if ((i < 0) || (i >= PtrArrayBase<T>::max_len))
      return NULL;
   
   return PtrArrayBase<T>::ptr_list[i];
}


// NOTE:  If this value is NULL, we will create a default one
// and return that.  Funky but useful.
template <class T>
T* LazyPtrArray<T>::elementAt(int i) {
//   cout << "PtrArrayBase::elementAt, i: " << i << "  max_len:  " 
//        << max_len << endl;

   if ((i < 0) || (i >= PtrArrayBase<T>::max_len)) {
//      cout << "Returning null" << endl;
      return NULL;
   }
   
   //cout << "Checking for null..." << endl;
   if (PtrArrayBase<T>::ptr_list[i] == NULL) {
//      cout << "Creating new one..." << endl;
      PtrArrayBase<T>::ptr_list[i] = new T(i);
   }
   else {
//      cout << " offset wasn't null..." << endl;
   }

   return PtrArrayBase<T>::ptr_list[i];
}


///**********************************************************************///
///*********************  PtrArray  *************************************///
///**********************************************************************///

template <class T>
T* PtrArray<T>::operator[] (const int i) {
   return PtrArrayBase<T>::elementAt(i);
}


///**********************************************************************///
///*********************  ObjArray  *************************************///
///**********************************************************************///

/** This should never be called more than once!! */
template <class T>
int ObjArray<T>::init(int sz) {
   if (sz < 5) {
      sz = 5;
   }
   _cnt++;

   len = sz;
   array = new T[sz];
   return 0;
}//constructor

template <class T>
ObjArray<T>::ObjArray(const ObjArray<T>& src)
      : len(src.len), bf(src.bf) {
   array = new T[len];
   for (int i = 0; i<len; i++) {
      array[i] = src.array[i];
   }//for
}//copy constructor

template <class T>
ObjArray<T>& ObjArray<T>::operator=(const ObjArray<T>& src) {
   if (this != &src) {
      ensureCapacity(src.len);
      bf = src.bf;
      
      // TODO:  Skip un-used ones??
      for (int i = 0; i<len; i++) {
         array[i] = src.array[i];
      }//for
   }
   return *this;
}//operator=


template <class T>
ObjArray<T>::~ObjArray() {
   delete[] array;
   _cnt--;
}//destructor


template <class T>
void ObjArray<T>::ensureCapacity(int cap) {
   if (cap > len) {
      T* tmp = array;
      array = new T[cap + len];

      // TODO:  Skip un-used ones??
      for (int i = 0; i<len; i++) {
         array[i] = tmp[i];
      }//for
      
      delete[] tmp;
      len += cap;
   }//if
}//ensureCapacity


template <class T>
void ObjArray<T>::addElement(T& val) {
   int idx = bf.firstClear();
   ensureCapacity(idx);
   array[idx] = val;
   bf.turn_on(idx);
}//addElement

template <class T>
void ObjArray<T>::appendElement(T& val) {
   int idx = bf.lastSet();
   idx++;
   ensureCapacity(idx);
   array[idx] = val;
   bf.turn_on(idx);
}//appendElement


template <class T>
void ObjArray<T>::purge() {
   delete[] array;
   array = NULL;
   bf.off_all();
   len = 0;
}//purge

/** Returns TRUE if the array contains the element 'val', as
 * determined by operator=
 */
template <class T>
int ObjArray<T>::contains(T& val) const {
   for (int i = getNextIdx(-1); i != -1;
        i = getNextIdx(i)) {
      if (val == elementAt(i)) {
         return TRUE;
      }
   }//for
   return FALSE;
}//contains


///**********************************************************************///
///*********************  FixedHistory  *********************************///
///**********************************************************************///


template <class T>
unsigned int FixedHistory<T>::init(unsigned int sz) {
   array = new T[sz];
   head = 0;
   len = sz;
   return len;
}//init

template <class T>
FixedHistory<T>::FixedHistory(const FixedHistory<T>& src) :
      len(src.len), head(src.head) {
   __fixed_history_cnt++;
   array = new T[len];
   for (unsigned int i = 0; i<len; i++) {
      array[i] = src.array[i];
   }
}

template <class T>
FixedHistory<T>& FixedHistory<T>::operator=(const FixedHistory<T>& src) {
   ensureCapacity(src.len);
   len = src.len;
   for (unsigned int i = 0; i<len; i++) {
      array[i] = src.array[i];
   }
   head = src.head;
}

template <class T>
void FixedHistory<T>::ensureCapacity(unsigned int cap) {
   if (cap > len) {
      T* tmp = array;
      array = new T[cap];

      unsigned int i;
      for (i = 0; i<len; i++) {
         array[i] = tmp[i];
      }//for

      delete[] tmp;
      len = cap;
   }//if
}//ensureCapacity

   /** Does not clean up memory, just marks all as un-used. */
template <class T>
void FixedHistory<T>::clear() {
   head = 0;
}


/** Cleans up all the memory it can. */
template <class T>
void FixedHistory<T>::purge() {
   delete[] array;
   array = NULL;
   len = 0;
   head = 0;
}

