// $Id: audit.cc,v 1.2 2004/07/05 23:06:35 eroper Exp $
// $Revision: 1.2 $  $Author: eroper $ $Date: 2004/07/05 23:06:35 $

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


/** This file should grow to hold various types of audits that we can run
 * to diagnose problems.
 */


/* obj_ptr_log can handle these types of object keys:
 * OBJ_CC      From object copy constructor.
 * OBJ_LST     On object list (from right after we read the world.)
 * OBJ_CON     From dflt constructor.
 * OBJ_DES     From destructor.
 * OBJ_NOTE        Something someone wanted to add.
 */

#include "audit.h"
#include <iostream.h>
#include <string.h>
#include <stdlib.h>


char* usage = "audit [file] [type] [arg1 arg2 ...]\n"
"Supported types:\n"
"    OBJ [address in hex: eg. 0xAABBccDd]\n"
"\n";

int core_dump(const char* msg) {
   cerr << "Dumping core on purpose: " << msg << endl;
   abort();
}

int main(int argc, char** argv) {
   String fname(100);
   String type(50);
   String arg1(50);

   if (argc > 1)
      fname = argv[1];

   if (argc > 2)
      type = argv[2];

   if (argc > 3)
      arg1 = argv[3];


   if (strcasecmp(type, "OBJ") == 0) {
      return auditObject(fname, arg1);
   }
   else {
      cout << usage << endl;
      return 2;
   }
   return 0;
}//main

int auditObject(String& fname, String& addr) {
   ifstream ifile(fname);
   if (!ifile) {
      cout << "File -:" << fname << ":- could not be opened.\n";
      return 1;
   }

   String buf(100);
   String tmp(50);
   String num(50);

   addr.toLower();

   while (ifile) {
      buf.getLine(ifile, 100);

      if (strstr(buf, addr)) {
         tmp = buf.getToken();
         num = buf.getToken();
         if (tmp == "OBJ_CC") {
            cout << "Object# " << num << " copy constructed." << endl;
         }
         else if (tmp == "OBJ_CR") {
            cout << "Object# " << num << " created in obj_to_sobj." << endl;
         }
         else if (tmp == "OBJ_LST") {
            cout << "Object# " << num 
                 << " is in Object List after world has been read." << endl;
         }
         else if (tmp == "OBJ_CON") {
            cout << "Object# " << num << " constructed by default constructor." << endl;
         }
         else if (tmp == "OBJ_DES") {
            cout << "Object# " << num << " destructed." << endl;
         }
         else if (tmp == "OBJ_NOTE") {
            cout << "Object# " << num << " NOTE:" << endl;
            tmp = buf.getToken(); //get rid of the hex
            cout << "\t" << buf << endl;
         }
         else if (tmp == "CRI_CC") {
            cout << "Critter# " << num << " copy constructed." << endl;
         }
         else if (tmp == "CRI_CR") {
            cout << "Critter# " << num << " created in mob_to_smob." << endl;
         }
         else if (tmp == "CRI_LST") {
            cout << "Critter# " << num 
                 << " is in Critter List after world has been read." << endl;
         }
         else if (tmp == "CRI_CON") {
            cout << "Critter# " << num << " constructed by default constructor." << endl;
         }
         else if (tmp == "CRI_DES") {
            cout << "Critter# " << num << " destructed." << endl;
         }
         else if (tmp == "CRI_NOTE") {
            cout << "Critter# " << num << " NOTE:" << endl;
            tmp = buf.getToken(); //get rid of the hex
            cout << "\t" << buf << endl;
         }
         else {
            cout << "UKNOWN FORMAT:  " << tmp << " " << num << " " << buf << endl;
         }
      }//if
   }//while
   return 0;
}//auditObject
