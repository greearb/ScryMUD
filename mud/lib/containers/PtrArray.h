// $Id: PtrArray.h,v 1.5 1999/06/26 06:14:17 greear Exp $
// $Revision: 1.5 $  $Author: greear $ $Date: 1999/06/26 06:14:17 $

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

#ifndef GRRMUD_PTR_ARRAY_H
#define GRRMUD_PTR_ARRAY_H

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#include <bitfield.h>
#include <assert.h>


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

#ifndef __INCLUDE_PTR_ARRAY_CC__
#define __INCLUDE_PTR_ARRAY_CC__
#include "PtrArray.cc"
#endif

#endif
