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
   
   mudlog << "In becomeDeepCopy, src.cur_len:  " << src.getCurLen()
          << " cur_len: " << cur_len << " max_len:  " << max_len
          << endl;

   ensureCapacity(src.getCurLen());
   
   int i;
   for (i = 0; i<src.getCurLen(); i++) {
      if (ptr_list[i]) {
         mudlog << "Deleting one, idx: " << i << endl;
         delete ptr_list[i];
      }
      
      if (src.ptr_list[i]) {
         mudlog << "Source existed for idx: " << i << endl << flush;
         ptr_list[i] = new T(*(src.ptr_list[i]));
         cur_len = i + 1;
      }
      else {
         ptr_list[i] = NULL;
      }
   }//for

   mudlog << "Cleaning up remainder..." << endl;
   for (; i<getMaxLen(); i++) {
      if (ptr_list[i]) {
         mudlog << "Deleting idx:  " << i << endl;
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

   if (val && (posn >= cur_len))
      cur_len = posn + 1;
   else if (!val) {
      if (posn == (cur_len - 1)) {
         for (int i = cur_len - 2; i>=0; i--) {
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
   
   if (val && (posn >= cur_len))
      cur_len = posn + 1;
   else if (!val) {
      if (posn == (cur_len - 1)) {
         for (int i = cur_len - 2; i>=0; i--) {
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
   if ((i < 0) || (i >= max_len))
      return NULL;
   
   return ptr_list[i];
}


// NOTE:  If this value is NULL, we will create a default one
// and return that.  Funky but useful.
template <class T>
T* LazyPtrArray<T>::elementAt(int i) {
//   cout << "PtrArrayBase::elementAt, i: " << i << "  max_len:  " 
//        << max_len << endl;

   if ((i < 0) || (i >= max_len)) {
//      cout << "Returning null" << endl;
      return NULL;
   }
   
   //cout << "Checking for null..." << endl;
   if (ptr_list[i] == NULL) {
//      cout << "Creating new one..." << endl;
      ptr_list[i] = new T(i);
   }
   else {
//      cout << " offset wasn't null..." << endl;
   }

   return ptr_list[i];
}


///**********************************************************************///
///*********************  PtrArray  *************************************///
///**********************************************************************///

template <class T>
T* PtrArray<T>::operator[] (const int i) {
   return elementAt(i);
}

