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

#include <iostream.h>
#include <fstream.h>

#define MAX_BIT_ALLOWED 256

class bitfield {

private:
   unsigned short int *vector;
   short vect_len;
   int Assert(int boolean_val, const char* msg);

public:

   void set(int i_th, int posn);
   void print() const;
   void flip(short bit_posn);
   void turn_on(short bit_posn);
   void turn_off(short bit_posn);
   void flip_all();
   void on_all();
   void off_all();
   short  get(short bit_posn) const;
   short is_zero() const;
   int max_bit() const; //returns highest bit posn that field contains
   void ensureCapacity(short bit_posn);
   
   bitfield();  //default constructor
   bitfield(short bit_count);
   bitfield(const bitfield& b); //copy constructor
   ~bitfield();
   
   void operator= (const bitfield& b);
   short operator== (const bitfield& b) const;
   short operator!= (const bitfield& b) const;
   
   void init(const short num_of_flags); //callable initializer
   void Write(ofstream& ofile) const;
   void Read(ifstream& ifile);
   void Clear();
}; //class bitfield

#endif
