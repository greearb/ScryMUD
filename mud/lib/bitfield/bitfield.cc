// $Id: bitfield.cc,v 1.10 2001/03/29 03:02:38 eroper Exp $
// $Revision: 1.10 $  $Author: eroper $ $Date: 2001/03/29 03:02:38 $

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

///*******************  bitfield.cc  *************************///
/* nice general bitfield class, it expands as needed, so would
    not be good for something like Operating Systems w/out 
    modification */

#include "bitfield.h"
#include <iostream.h>


static const short int_len = 8 * sizeof(unsigned short int);
ofstream bitlog("./log/bitlog");
extern int core_dump(const char* msg); //misc2.cc

//static
int BitfieldNames::_cnt = 0;


/** Constructor. */
BitfieldNames::BitfieldNames(int length, const char** names,
                             const char* col_name)
      : len(length), bit_names(names), header(col_name) {
   bitlog << "BitfieldNames Constructor, length: " << length
          << " collection name -:" << col_name << ":-" << endl;
   _cnt++;
}

void BitfieldNames::listNames(String& buf) const {
   String tmp(100);

   buf = header;
   buf += " (DEFINED)\n\t";
   int sofar = 0;

   for (int i = 0; i<len; i++) {
      Sprintf(tmp, "[%i] %s, ", i, bit_names[i]);
      if ((sofar + tmp.Strlen()) > 80) {
         buf += "\n\t";
         sofar = tmp.Strlen();
      }
      else {
         sofar += tmp.Strlen();
      }
      buf += tmp;
   }
   buf += "\n";
}

const char* BitfieldNames::getName(int idx) const {
   if ((idx >= 0) && (idx < len)) {
      return bit_names[idx];
   }
   else {
      return "OUT_OF_RANGE";
   }
}



/** Returns -1 if no more. */
int bitfield::nextSet(int from_this) const {
   int mx = max_bit();
   for (int i = from_this + 1; i<mx; i++) {
      if (get(i)) {
         return i;
      }
   }//for
   return -1;
}//nextSet
 
/** Returns -1 if no more. */
int bitfield::lastSet() const {
   for (int i = max_bit(); i>= 0; i--) {
      if (get(i)) {
         return i;
      }//if
   }//for
   return -1;
}

/** Returns -1 if no more. */
int bitfield::firstSet() const {
   int mx = max_bit();
   for (int i = 0; i<mx; i++) {
      if (get(i)) {
         return i;
      }
   }//for
   return -1;
}


int bitfield::firstClear() const {
   int mx = MAX_BIT_ALLOWED;
   for (int i = 0; i<mx; i++) {
      if (!get(i)) {
         return i;
      }
   }//for
   return -1;
}



int bitfield::Assert(int boolean_val, const char* msg) {
   if (!boolean_val) {
      core_dump(msg);
   }
   return TRUE;
}

void bitfield::Clear() {
   off_all();
}//Clear()


int figure_len(int max_flag) {
   return (max_flag + 1)/int_len + 1;
}


int bitfield::max_bit() const {
   return ((vect_len * int_len) - 1);
}//max_bit


int bitfield::Read(ifstream& ifile) {
   int flag;
   char buf[81];

   Clear();
   ifile >> flag;

   if (!ifile) {
      return -1;
   }

   while (flag != -1) {
      turn_on(flag);
      ifile >> flag;
      
      if (!ifile) {
         return -1;
      }
   }//while
   ifile.getline(buf,80);  //read rest of line
   return 0;
}//Read


void bitfield::Write(ofstream& ofile) const {
   for (int i = max_bit(); i >= 0; i--) {
      if (get(i)) {
         ofile << i << " ";
      }//if
   }//for
   ofile << -1 << "  -:| bitfield |:-\n";
}//Write


void bitfield::print() const {
   String buf(max_bit() + 2);
   for (int i = 0; i < max_bit(); i++) {
      if (this->get(i))
         buf.Prepend("1");
      else
         buf.Prepend("0");
   }//for
   cout << buf << endl;
}//print()


void bitfield::set(int i_th, int val) {
   if (val)
      turn_on(i_th);
   else
      turn_off(i_th);
}

void bitfield::ensureCapacity(int bit_posn) {
   //first, check for bogus values
   Assert((bit_posn <= MAX_BIT_ALLOWED), "ensureCapacity, out of range");
   
   if (bit_posn > max_bit()) {
      bitfield tmp(*this);
      delete[] vector;
      vect_len = figure_len(bit_posn);
      vector = new unsigned short[vect_len];
      *this = tmp;
   }
}//ensureCapacity

void bitfield::flip(int bit_posn) {
   ensureCapacity(bit_posn);
   if (get(bit_posn)) {
      flags_on--;
   }
   else {
      flags_on++;
   }
   vector[(bit_posn / int_len)] ^=
         (1 << (bit_posn % int_len));
}//flip()


void bitfield::init(int num_bits) {
   ensureCapacity(num_bits);
   off_all();
}//init()


void bitfield::turn_on(int bit_posn) {
   ensureCapacity(bit_posn);
   if (!get(bit_posn)) {
      flags_on--;
   }
   vector[(bit_posn / int_len)] |=
         (1 << (bit_posn % int_len));
}//on()

void bitfield::turn_off(int bit_posn) {
   if (get(bit_posn)) {
      flags_on--;
   }
   if ((bit_posn <= max_bit()) && (bit_posn >= 0)) {
      vector[(bit_posn / int_len)] &=
         ~(1 << (bit_posn % int_len));
   }//if
}//off()


void bitfield::flip_all() {
   for (int i = 0; i<vect_len; i++) {
      vector[i] = ~vector[i];
   }//for

   //TODO:  Verify this some day!!
   flags_on = max_bit() - flags_on + 1; // +1 because flags_on is ones based.
}//flip_all()


void bitfield::on_all() {
   flags_on = max_bit() + 1;
   for (int i = 0; i<vect_len; i++) {
      vector[i] = ~(0);
   }//for
}//on_all;


void bitfield::off_all() {
   flags_on = 0;
   for (int i = 0; i<vect_len; i++) {
      vector[i] = 0;
   }//for
}//off_all


int bitfield::get(int bit_posn) const {
   if ((bit_posn > max_bit()) || (bit_posn < 0)) {
      return FALSE;
   }//if
   if (vector[(bit_posn / int_len)] &
          (1 << (bit_posn % int_len)))
      return TRUE;
   else
      return FALSE;
}// get()


bitfield::bitfield() : flags_on(0) {  //default constructor
   vector = new unsigned short int[1];
   vector[0] = 0;
   vect_len = 1;
}//default constructor


bitfield::bitfield(int bit_count) : flags_on(0) {
   vect_len = figure_len(bit_count);
   vector = new unsigned short int[vect_len];
   for (int i = 0; i<vect_len; i++) {
      vector[i] = 0;
   }//for
}//constructor


bitfield::bitfield(const bitfield& b) {
   vector = new unsigned short int[b.vect_len];
   vect_len = b.vect_len;
   flags_on = b.flags_on;
   for (int i = 0; i<vect_len; i++) {
      vector[i] = b.vector[i];
   }//for
}//copy constructor



bitfield::~bitfield() {
   delete[] vector;
   vector = NULL;
}//destructor


void bitfield::operator= (const bitfield& b) {
   if (b.vect_len > vect_len) {
      delete[] vector;
      vector = new unsigned short int[b.vect_len];
      vect_len = b.vect_len;
   }//if

   off_all();  //zero it out..
   for (int i = 0; i < b.vect_len; i++) {
      vector[i] = b.vector[i];
   }//for
   flags_on = b.flags_on;
}//operator=


int bitfield::operator== (const bitfield& b) const {
   int min = b.vect_len;
   if (vect_len < b.vect_len)
      min = vect_len; 

   for (int i = 0; i<min; i++) {
      if (vector[i] != b.vector[i]) 
         return FALSE;
   }//for
   return TRUE;
}//operator==


int bitfield::operator!= (const bitfield& b) const {
   return (!(*this == b));
}//operator!=


int bitfield::is_zero() const {
   for (int i = 0; i<vect_len; i++) {
      if (vector[i] != 0)
         return FALSE;
   }//for
   return TRUE;
}//is_zero
