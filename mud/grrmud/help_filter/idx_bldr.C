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


#include <stdio.h>
#include <string2.h>
#include <string.h>


int core_dump(const char* msg) {
   cerr << "ERROR:  " << msg << endl;
   exit(1);
}

char cbuf[200]; //global, but hey, it's a small prog!

const char* normalize(const char* str) {
   if (strncmp(str, "IMM_", 4) == 0) {
      strcpy(cbuf, str + 4);
   }
   else {
      strcpy(cbuf, str);
   }
   int len = strlen(cbuf);
   cbuf[len - 2] = 0;

   return cbuf;
}//normalize


int main(int argc, char** argv) {
   String buf(200);
   String tmp(30);

   for (int i = 1; i<argc; i++) {
      if (buf.Strlen() < 72) {
         Sprintf(tmp, "%s%P18 ", normalize(argv[i]));
         buf += tmp;
      }
      else {
         cout << buf << endl;
         buf = "";
         Sprintf(tmp, "%s%P18 ", normalize(argv[i]));
			buf += tmp;
      }//else
   }//for
   cout << buf << endl;
}//main
