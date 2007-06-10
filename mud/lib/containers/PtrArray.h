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

// PtrArray

#ifndef GRRMUD_PTR_ARRAY_H
#define GRRMUD_PTR_ARRAY_H

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#include <bitfield.h>
#include <assert.h>
#include <string2.h>

extern LogStream mudlog;
extern int min(int i, int j);


/**  An array of type class T pointers.  Does bounds checking
 * and will grow as needed. 
 */
template <class T> class PtrArrayBase {
private:
   //ensure it's never called, as dangling pointers suck!
   PtrArrayBase<T>& operator=(const PtrArrayBase<T>& src);
   PtrArrayBase(const PtrArrayBase<T>& src); //ensure it's never called

protected:
   int max_len;
   int cur_len;
   T** ptr_list;

public:
   PtrArrayBase(int mx);

   PtrArrayBase() {
      max_len = 10;
      cur_len = 0;
      ptr_list = new T*[max_len + 1];
   
      for (int i = 0; i<max_len; i++) {
         ptr_list[i] = NULL;
      }
   }

   virtual ~PtrArrayBase();

   /** Deep copy.
    * Virtually does a clearAndDestroy on Self!!!!!!
    */
   virtual void becomeDeepCopyOf(const PtrArrayBase<T>& src);

   /** NOTE:  Returns NULL if out of bounds or value is NULL. */
   virtual T* elementAt(int i);

   /** NOTE:  Returns NULL if out of bounds or value is NULL. */
   virtual const T* constElementAt(int i) const;

   /** does NOT delete pointers */
   virtual void clear();

   /** Deletes any data pointed to by internal pointers. */
   virtual void clearAndDestroy();

   /** we won't allow it to grow shorter! */
   virtual void ensureCapacity(int i);

   /** max len before we have to grow the array. */
   virtual int getMaxLen() const;

   /** maximum element asigned a value.  This may or may not be true,
    * depending on how you use the class.  (It is meant to be used
    * with append()
    */
   virtual int getCurLen() const;

   /** return TRUE if it worked..false otherwise */
   virtual int setAndDestroy(T* val, int posn);

   virtual int set(T* val, int posn);

   /** Make a copy of the incomming object. */
   virtual int appendDeepCopy(T& val);

   /** Just copy the pointer, not the real object. */
   virtual int appendShallowCopy(T* val);

   /**  This copies the data from the incomming src, NOT just a shallow copy.
    *  Does not copy NULL's over, ie it packs the src when appending it.
    */
   virtual int appendDeepCopy(const PtrArrayBase<T>& src);

   /** If it's not already in the array,
    * find the first open slot and dump it in...
    */
   virtual int gainData(T* val);

   /** Return TRUE if val is in the array, false otherwise. */
   virtual int haveData(const T* val);

   /** Find the first instance and delete it. */
   virtual int loseData(const T* val);

   /**  This just copies the pointers of the src array. */
   virtual int appendShallowCopy(const PtrArrayBase<T>& src);

};//PtrArrayBase<class T>


/**  This class will automatically instantiate new instances of type
 * class T if one tries to reference an index (within valid bounds)
 * that has a NULL value currently.  Of course, if there is one already
 * there, then it just returns it.
 */
template <class T> class LazyPtrArray : public PtrArrayBase<T> {
private:
   //ensure it's never called, as dangling pointers suck!
   LazyPtrArray<T>& operator=(const LazyPtrArray<T>& src);
   LazyPtrArray(const LazyPtrArray<T>& src); //ensure it's never called

protected:

public:
   LazyPtrArray(int mx) : PtrArrayBase<T>(mx) { };

   LazyPtrArray() : PtrArrayBase<T>() { };

   virtual ~LazyPtrArray() { }; //super-class destructor handles array.

   // NOTE:  if elementAt returns null, this will SEGV, be carefull!
   T& operator[] (const int i);

   /** NOTE:  If this value is NULL, we will create a default one
    * and return that.  Funky but useful.
    * WARNING: Over-ridden from base class...and meaning has changed a bit
    */
   virtual T* elementAt(int i);

   virtual T* elementAtNoCreate(int i);

};//LazyPtrArray<class T>


/**  Simply an array of pointers of type class T.  Will grow
 * as needed.
 */
template <class T> class PtrArray : public PtrArrayBase<T> {
private:
   //ensure it's never called, as dangling pointers suck!
   PtrArray& operator=(const PtrArray<T>& src); 
   PtrArray(const PtrArray<T>& src);//ensure it's never called

protected:

public:
   PtrArray(int mx) : PtrArrayBase<T>(mx) { };

   PtrArray() : PtrArrayBase<T>() { };

   virtual ~PtrArray() { }; //super class destructor handles the array.

   // Simply return the object stored at that offset.  Returns NULL
   // if it does not exist, or out of bounds.
   T* operator[] (const int i);

};//LazyPtrArray<class T>


/** This is a growable array of objects.
 */
template <class T> class ObjArray {
private:
   int _cnt;

protected:
   int len;
   bitfield bf;
   T* array;

   int init(int sz);

public:
   ObjArray() { init(5); }
   ObjArray(int sz) { init(sz); }
   ObjArray(const ObjArray<T>& src);
   ObjArray<T>& operator=(const ObjArray<T>& src);
   ~ObjArray();

   T& elementAt(int idx) const {
      assert((idx >= 0) && (idx < len));
      return array[idx];
   }

   void setElementAt(int idx, T& val) {
      assert((idx >= 0) && (idx < len));
      array[idx] = val;
      bf.turn_on(idx);
   }

   void ensureCapacity(int sz);

   int isEmpty() const { return bf.is_zero(); }

   /** Get the next valid index.  Uses internal bitfield to walk
    * over un-used spaces.  Returns -1 when there is no next.
    */
   int getNextIdx(int from_idx) const { return bf.nextSet(from_idx); }
   
   /** Insert in first free position. */
   void addElement(T& val);
   
   /** Add to the end of the array. */
   void appendElement(T& val);

   /** Mark spot in array is un-used. */
   void removeElement(int idx) {
      assert((idx >= 0) && (idx < len));
      bf.turn_off(idx);
   }

   /** Does not clean up memory, just marks all as un-used. */
   void clear() { bf.off_all(); }

   /** Cleans up all the memory it can. */
   void purge();

   /** How many elements are valid in our array. */
   int getElementCount() const { return bf.flagsSet(); }

   /** Get the maximum capacity available w/out grabbing more memory. */
   int getCapacity() const { return len; }

   /** Returns TRUE if the array contains the element 'val', as
    * determined by operator=
    */
   int contains(T& val) const ;

};//ObjArray


extern int __fixed_history_cnt;

/** This is a fixed length queue.  Meant to append and search very
 * fast.
 */
template <class T> class FixedHistory {
private:

protected:
   unsigned int len;
   unsigned int head;
   T* array;

   unsigned int init(unsigned int sz);

public:
   FixedHistory() { init(10); __fixed_history_cnt++; }
   FixedHistory(int sz) { init(sz); __fixed_history_cnt++; }
   FixedHistory(const FixedHistory<T>& src);
   FixedHistory<T>& operator=(const FixedHistory<T>& src);
   virtual ~FixedHistory() { delete[] array; __fixed_history_cnt--; }

   T& elementAt(int idx) const {
      assert((idx >= 0) && ((unsigned int)(idx) < len));
      return array[idx];
   }

   void setElementAt(int idx, T& val) {
      assert((idx >= 0) && ((unsigned int)(idx) < len));
      array[idx] = val;
   }

   void ensureCapacity(unsigned int sz);

   int getNextIdx(int from_idx) const { return (head + 1) % len; }
   int getCurIdx() { return (head % len); }
   int getPrevIdx(int from_idx) const { return (head - 1) % len; }
   
   /** Add to the end of the array, will wrap of course. */
   void appendElement(T& val) {
      head++;
      array[head % len] = val;
   }

   /** Does not clean up memory, just marks all as un-used. */
   void clear();

   /** Cleans up all the memory it can. */
   void purge();

   /** Get the maximum capacity available w/out grabbing more memory. */
   unsigned int getCapacity() const { return len; }

   /** Returns TRUE if the array contains the element 'val', as
    * determined by operator==
    */
   int contains(T& val) const {
      return (indexOf(val) >= 0);
   }

   /** Returns < 0 if not found, first idx otherwise.  As determined by
    * operator=
    */
   int indexOf(T& val) const {
      int mx = min(len, head);
      for (int i = 0; i<mx; i++) {
         if (val == array[i]) {
            return i;
         }
      }
      return -1;
   }

   /** Returns cout of objects in array, as determined by operator=
    */
   int objCount(T& val) const {
      int retval = 0;
      int mx = min(len, head);
      for (int i = 0; i<mx; i++) {
         if (val == array[i]) {
            retval++;
         }
      }//for
      return retval;
   }//objCount


   String toString() {
      String retval(200);
      Sprintf(retval, "len: %i  head: %i  head_idx: %i\n", len, (int)(head), (int)(head % len));
      char tmp[50];
      for (unsigned int i = 0; i<len; i++) {
         sprintf(tmp, "  [%i]  %lu\n", i, (long unsigned int)(array[i]));
         retval.append(tmp);
      }//for
      return retval;
   }      
   
};//FixedHistory



#ifndef __INCLUDE_PTR_ARRAY_CC__
#define __INCLUDE_PTR_ARRAY_CC__
#include "PtrArray.cc"
#endif

#endif
