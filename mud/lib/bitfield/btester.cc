// $Id$
// $Revision: 1.2 $  $Author$ $Date$

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

///**************************  tester.cc  **************************///

#include "bitfield.h"
#include <iostream.h>





void log(const char* msg) {
   cout << "LOG:  " << msg << endl;
}//log


main() {
   cout << "Sizeof int:  " << sizeof(unsigned int) << endl;
   cout << "Sizeof short:  " << sizeof(unsigned short) << endl;
   cout << "Sizeof long:  "  << sizeof(unsigned long) << endl;
   cout << "Sizeof long long:  " << sizeof(unsigned long long) << endl;

   bitfield bit1;
   bitfield bit2(13);
   bitfield bit3(75);
   bitfield bit4(bit2);

   cout << "Bitstreams 1 - 4, should be all zero's.\n";
   bit1.print();
   bit2.print();
   bit3.print();
   bit4.print();

   bit2.turn_on(1);
   bit2.turn_on(3);
   bit2.turn_on(6);
   cout << "\nShould have 1,3,6 bit turned on.\n";
   bit2.print();

   bit2.turn_off(3);
   bit2.turn_on(0);
   cout << "\nSHould have 0, 1, 6 bit turned on.\n";
   bit2.print();
   bit2.flip(0);
   cout << "\n0 should have been flipped, back to 0\n";
   bit2.print();

   bit2.flip_all();
   cout << "Only 1, 6 should be turned off.\n";
   bit2.print();
   bit2.on_all();
   cout << "\nShould be all 1's\n";
   bit2.print();
   bit2.off_all();
   cout << "\n should be all 0's\n";
   bit2.print();
   cout << "flag 1:  " << bit2.get(1) << endl;
   cout << "flag 31:  " << bit2.get(31) << endl;
   cout << "\n Should be all zero's.\n";
   bit2.print();
   cout << "\nShould be 1:  " << bit2.is_zero() << endl;
   bit2.turn_on(11);
   cout << "\nShould be 0: " << bit2.is_zero() << endl;

   cout << "Bit2:  ";
   bit2.print();
   bit4.turn_on(21);
   cout << "\nBit4:  ";
   bit4.print();
   bit2 = bit4;
   cout << "\nBoth should have 1 in 21 position..\n";
   bit2.print();
   bit4.print();
   cout << "\nShould be 1:  " << (bit2 == bit4) << endl;
   cout << "\nShould be 0:  " << (bit2 != bit4) << endl;
   
   bit2.off_all();
   bit4.on_all();
   bit2.flip(20);
   bit2 &= bit4;
   cout << "\nShould be only 1 in 20 position.\n";
   bit2.print();

   bit2.off_all();
   bit4.on_all();
   bit4.flip(20);
   bit2 |= bit4;
   cout << "\nShould be only 0 in 20 position.\n";
   bit2.print();

   bit2.off_all();
   bit4.on_all();
   bit2.flip(20);
   cout << "\nShould be 1 in 20 position only.\n";
   bitfield tmp = bit2 & bit4;
   tmp.print();
   bit2.print();

   bit2.off_all();
   bit4.on_all();
   bit4.flip(20);
   cout << "\nShould be 0 in 20 position only.\n";
   (bit2 | bit4).print();
   bit2.print();
    
   
   bit3.off_all();
   bit3.print();
   bit3.on_all();
   bit3.print();
   bit3.flip(72);
   bit3.turn_off(8);
   bit3.flip(95);
   bit3.flip(94);
   bit3.print();
}//main
