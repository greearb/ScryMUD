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
#include <string2.h>
#include <iostream.h>


static const short int_len = 8 * sizeof(unsigned short int);
ofstream bitlog("./log/bitlog");
extern int core_dump(const char* msg); //misc2.cc

 
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
   return max_flag/int_len + 1;
}

int bitfield::max_bit() const {
   return ((vect_len * int_len) - 1);
}//max_bit

void bitfield::Read(ifstream& ifile) {
   short flag;
   char buf[81];
 
   Clear();
   ifile >> flag;
   while (flag != -1) {
      turn_on(flag);
      ifile >> flag;
   }//while
   ifile.getline(buf,80);  //read rest of line
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

void bitfield::ensureCapacity(short bit_posn) {
   //first, check for bogus values
   Assert((bit_posn < MAX_BIT_ALLOWED), "ensureCapacity, out of range");
   
   if (bit_posn > max_bit()) {
      bitfield tmp(*this);
      delete[] vector;
      vect_len = figure_len(bit_posn);
      vector = new unsigned short[vect_len];
      *this = tmp;
   }
}//ensureCapacity

void bitfield::flip(short bit_posn) {
   ensureCapacity(bit_posn);
   vector[(bit_posn / int_len)] ^=
         (1 << (bit_posn % int_len));
}//flip()


void bitfield::init(short num_bits) {
   ensureCapacity(num_bits);
   off_all();
}//init()


void bitfield::turn_on(short bit_posn) {
   ensureCapacity(bit_posn);
   vector[(bit_posn / int_len)] |=
         (1 << (bit_posn % int_len));
}//on()

void bitfield::turn_off(short bit_posn) {
   if ((bit_posn <= max_bit()) && (bit_posn >= 0)) {
      vector[(bit_posn / int_len)] &=
         ~(1 << (bit_posn % int_len));
   }//if
}//off()


void bitfield::flip_all() {
   for (int i = 0; i<vect_len; i++) {
      vector[i] = ~vector[i];
   }//for
}//flip_all()


void bitfield::on_all() {
   for (int i = 0; i<vect_len; i++) {
      vector[i] = ~(0);
   }//for
}//on_all;


void bitfield::off_all() {
   for (int i = 0; i<vect_len; i++) {
      vector[i] = 0;
   }//for
}//off_all


short bitfield::get(short bit_posn) const {
   if ((bit_posn > max_bit()) || (bit_posn < 0)) {
      return FALSE;
   }//if
   if (vector[(bit_posn / int_len)] &
          (1 << (bit_posn % int_len)))
      return TRUE;
   else
      return FALSE;
}// get()



bitfield::bitfield() {  //default constructor
   vector = new unsigned short int[1];
   vector[0] = 0;
   vect_len = 1;
}//default constructor



bitfield::bitfield(short bit_count) {
   vect_len = figure_len(bit_count);
   vector = new unsigned short int[vect_len];
   for (int i = 0; i<vect_len; i++) {
      vector[i] = 0;
   }//for
}//constructor


bitfield::bitfield(const bitfield& b) {
   vector = new unsigned short int[b.vect_len];
   vect_len = b.vect_len;
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
}//operator=



short bitfield::operator== (const bitfield& b) const {
   int min = b.vect_len;
   if (vect_len < b.vect_len)
      min = vect_len; 

   for (int i = 0; i<min; i++) {
      if (vector[i] != b.vector[i]) 
         return FALSE;
   }//for
   return TRUE;
}//operator==


short bitfield::operator!= (const bitfield& b) const {
   return (!(*this == b));
}//operator!=



short bitfield::is_zero() const {
   for (int i = 0; i<vect_len; i++) {
      if (vector[i] != 0)
         return FALSE;
   }//for
   return TRUE;
}//is_zero
