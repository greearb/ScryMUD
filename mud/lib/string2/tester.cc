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

#include "string2.h"






void log(const char* msg) {
   cout << "LOG:  " << msg << endl;
}//log


main() {
   String s1("First String.");
   String s2(50);

   cout << "First string:  " << s1 << endl;

   s2 = "  Second String.";
   cout << "Should be Second String:  " << s2 << endl;

   String s3 = s1 + s2;
   cout << "Should be First:  " << s1 << endl;
   cout << "Should be Second:  " << s2 << endl;
   cout << "First + second:  " << s1 + s2 << endl;

   cout << "Should be First + second, s3:  " << s3 << endl;

   cout << "Should be first letter:  \n";
   char ch;
   ch = s3[0];
   cout << ch << endl;
   cout << "Ok, here is string before ch is changed: \n" << s3 << endl;
   ch = 'D';
   cout << "Ok, here is string after ch is changed to D: \n" << s3 << endl;



   String s4(s3);
   cout << "Should be same as s3:  ";
   cout << s4 << endl;

   s4 = s4 + " end of s4";
   cout << "Should be s4 + 'end of s4':  " << s4 << endl;

   cout << "Here is s4:  " << s4 << endl;
   cout << "Here is s3:  " << s3 << endl;

   if (s4 == s3)
      cout << "s4 equals s3.\n"; 
   else
      cout << "s4 does not equal s3.\n"; 

 

   cout << s1 << endl;
   cout << s2 << endl;
   cout << s3 << endl;
   cout << s4 << endl;
  

   String eos(" prepend before 423:  ");
   String tst(500);

   cout << "\n\ntst(500) has been created..\n\n";
   
   tst.Append(2);
   tst.Append(3);
   tst.Prepend(4);
   cout << "\ntst before prepending eos:  " << tst << endl;
   tst.Prepend(eos);
   cout << "\ntst after prepending eos:  " << tst << endl;
   tst.Prepend("\nTHIS is the string testing ");
   cout << tst << endl;
   cout << eos << endl;
   tst = " Road.";
   Sprintf(eos, "Here is one: %i, 'for the': %s%S\n", 1, "for the", &tst);
   cout << eos;

   s1.StringClassStatus();
}//main
