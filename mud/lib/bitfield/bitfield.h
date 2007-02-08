// $Id$
// $Revision$  $Author$ $Date$

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

///*******************  bitfield.h  ****************************///
#ifndef BEN__BITFIELD_INCLUDE
#define BEN__BITFIELD_INCLUDE

#include <iostream>
#include <fstream>
#include <string2.h> //my string class

#define MAX_BIT_ALLOWED 65000

/** Actually, these can be used for other similar classes too. */
class BitfieldNames {
protected:
   int len;
   const char** bit_names;
   static int _cnt;
   String header;

public:
   /** Does not copy char** names, so make sure you don't delete it!
    */
   BitfieldNames(int length, const char** names, const char* col_name);
   
   ~BitfieldNames() {
      _cnt--;
   }

   int getLength() const { return len; }
   const char** getNames() const { return (const char**)bit_names; }
   const char* getName(int idx) const;
   const String& getHeader() const { return header; }

   /** put names into the buf for display */
   void listNames(String& buf) const;

   static int getInstanceCount() { return _cnt; } 
};


class bitfield {

private:
   unsigned short int *vector;
   int vect_len;
   int flags_on;
   int Assert(int boolean_val, const char* msg);

public:

   void set(int i_th, int posn);
   void print() const;
   void flip(int bit_posn);
   void turn_on(int bit_posn);
   void turn_off(int bit_posn);
   void flip_all();
   void on_all();
   void off_all();
   int  get(int bit_posn) const;
   int is_zero() const;
   int max_bit() const; //returns highest bit posn that field contains
   void ensureCapacity(int bit_posn);

   /** Returns -1 if no more. */
   int nextSet(int from_this) const ;
   
   /** Returns -1 if no more. */
   int firstSet() const ;

   /** Returns -1 if bitfield is full (and cannot grow).  This should
    * not happen!!
    */
   int firstClear() const ;

   /** Returns -1 if no more. */
   int lastSet() const ;

   /** Number of turned-on flags. */
   int flagsSet() const { return flags_on; };

   bitfield();  //default constructor
   bitfield(int bit_count);
   bitfield(const bitfield& b); //copy constructor
   ~bitfield();
   
   void operator= (const bitfield& b);
   int operator== (const bitfield& b) const;
   int operator!= (const bitfield& b) const;
   
   void init(const int num_of_flags); //callable initializer
   void Write(ofstream& ofile) const;
   int Read(ifstream& ifile);
   void Clear();
}; //class bitfield

#endif
