//
//Copyright (C) 2006  Ben Greear
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
// To contact the Author, Ben Greear:  greearb@candelatech.com
//
//

///**************************  tester.cc  **************************///

#include "string2.h"
#include "IOBuffer.h"
#include <signal.h>


void log(const char* msg) {
   cout << "LOG:  " << msg << endl;
}//log

int core_dump(const char* msg) {
   cerr << "In core_dump, msg:  " << msg << endl;

   //From the great Katrina McClelan!
   //TODO:  Add waidpid etc...
#ifndef __WIN32__
   kill(getpid(),SIGSEGV); /* this'll dump core */
   sleep(10); /* make sure it stops here */
#endif

   exit(200);
   return TRUE;
}//core_dump


void hexDump(const unsigned char* msg, int len, String& retval, int show_decode = TRUE,
             int show_newlines = TRUE, int bytes_per_line = 16) {
   retval = "";
   char buf[50];
   unsigned short tmp = 0;
   int i;
   for (i = 0; i<len; i++) {
      tmp = (unsigned short)(msg[i]);
      tmp &= 0xff; //mask out high bits (left over from signed-ness??)
      sprintf(buf, "%02hx ", tmp);
      retval.append(buf);
      if (show_newlines && ((i + 1) % bytes_per_line == 0)) {
         if (show_decode) {
            retval.append("   ");
            for (int j = i-(bytes_per_line-1); j<=i; j++) {
               if (isprint(msg[j])) {
                  retval.append(msg[j]);
               }
               else {
                  retval.append(".");
               }
            }//for
         }
         retval.append("\n");
      }//if
   }//for

   if (show_decode) {
      // do final char translations.
      String ending(30);
      int q = (i) % bytes_per_line;
      int offset = i-q;
      retval.append("   ");
      for (int l = 0; l< bytes_per_line-q; l++) {
         retval.append("   "); //space, where the hex would have gone.
      }
      //VLOG << "q: " << q << " offset: " << offset << " i: " << i << endl;
      for (int j = 0; j<q; j++) {
         if (isprint(msg[j + offset])) {
            ending.append(msg[j + offset]);
         }
         else {
            ending.append(".");
         }
      }//for
      retval.append(ending);
   }
}//hexDump



int main() {
   int rv = 0;
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

   cout << "*****************************************************************\n";
   cout << "********************** end of string test ***********************\n";
   cout << "*****************************************************************\n\n\n\n";

   // Now, do the IO buffer test...
   IOBuffer iob(50); //should round up to 200 in the code
   int LEN = 325;
   unsigned char buf[50];

   for (int j = 1; j<5; j++) {
      iob.append((unsigned char*)("*****************************************************************"),
                 j*7);
      iob.dropFromTail(j*7);

      if (iob.getCurLen() != 0) {
         cout << "  j: " << j << endl;
         cout << "ERROR: IOBuffer.getCurLen() != 0 first append/drop.\n";
         cout << iob.toString() << endl;
         exit(2);
      }
      
      for (int i = 0; i<LEN; i++) {
         //cout << "\n\n------ BEFORE APPEND --------\n";
         //cout << iob.toString() << endl;
         memset(buf, (j << 4 | (i & 0xF)), 20);

         if (iob.append(buf, j) < 0) {
            cout << "ERROR: trying to append.\n";
            cout << iob.toString() << endl;
            exit(2);
         }

         //cout << "------ AFTER APPEND --------\n";
         //cout << "i: " << i << "  j: " << j << "  calc: " << ((i + 1) * j)
         //     << " LEN: " << LEN << endl;
         //cout << iob.toString() << endl;
         if (iob.getCurLen() != ((i + 1) * j)) {
            cout << "ERROR: IOBuffer failed append & getCurLen() test.\n";
            cout << iob.toString() << endl;
            exit(2);
         }
      }//for
      
      for (int i = 0; i<LEN; i++) {
         if (iob.dropFromTail(j) < 0) {
            cout << "i: " << i << "  j: " << j << " LEN: " << LEN << endl;
            cout << "ERROR: trying to drop from tail.\n";
            cout << iob.toString() << endl;
            exit(2);
         }
         if (iob.getCurLen() != (LEN - (i + 1)) * j) {
            cout << "i: " << i << "j: " << j << " LEN: " << LEN << endl;
            cout << "ERROR: IOBuffer failed dropFromTail & getCurLen() test.\n";
            cout << iob.toString() << endl;
            exit(2);
         }
      }//for

      if (iob.getCurLen() != 0) {
         cout << "  j: " << j << " LEN: " << LEN << endl;
         cout << "ERROR: IOBuffer.getCurLen() != 0 after all drops.\n";
         cout << iob.toString() << endl;
         exit(2);
      }
   }//for

   cout << "IOBuffer passed dropFromTail, append, and getCurLen() tests.\n\n\n";

   // Lets test some appends and getBytes
   unsigned char app_buf[137]; 
   unsigned char app_buf2[137];
   for (int i = 0; i<137; i++) {
      app_buf[i] = i;
   }
   memcpy(app_buf2, app_buf, 137);

   int mx = iob.getMaxLen();
   unsigned char junk_buf[mx];
   memset(junk_buf, 0, mx);

   // Test append of various sizes, but always clear, so it should
   // reset the head, tail to zero.
   for (int i = 0; i<mx; i++) {
      iob.clear(); //set head and tail to zero
      iob.append(junk_buf, i); //move the head along
      iob.dropFromTail(i); // remove copy of junk_buf, resets head & tail
      assert(iob.isEmpty());
      iob.append(app_buf, 137); //put app_buf in...

      iob.peekBytes(app_buf2, 137); //look at it.

      assert(iob.getCurLen() == 137);
      if (iob.getMaxContigFree() != (iob.getMaxLen() - 137)) {
         cerr << "ERROR:  maxContigFree: " << iob.getMaxContigFree()
              << " != maxlen: " << iob.getMaxLen() - 137 << endl;
         cerr << iob.toString() << endl;
         exit(5);
      }

      if (iob.getMaxContigUsed() != 137) {
         cerr << "ERROR:  maxContigUsed: " << iob.getMaxContigUsed()
              << " != 137\n";
         cerr << iob.toString() << endl;
         exit(4);
      }

      if (memcmp(app_buf, app_buf2, 137) != 0) {
         cout << "ERROR: append/getBytes2 screwed up, i: " << i << " mx: " << mx << endl;
         cout << iob.toString() << endl;
         String tmp_str(500);
         cout << "     ";
         char foo_buf[50];
         for (int j = 0; j<137; j++) {
            sprintf(foo_buf, "%2x  %2x\n     ", (unsigned int)(app_buf[j]),
                    (unsigned int)(app_buf2[j]));
            tmp_str.append(foo_buf);
         }//for
         cout << tmp_str;
         exit(2);
      }//if
   }//for

   iob.clear(); //set head and tail to zero
   // Test appends that should move head & tail around.
   for (int i = 0; i<mx; i++) {
      iob.append(junk_buf, i); //move the head along
      iob.dropFromTail(i); // remove first copy of junk_buf
      iob.append(app_buf, 137); //put app_buf in
      iob.dropFromTail(i); // remove other copy of junk_buf

      iob.peekBytes(app_buf2, 137); //look at it.

      assert(iob.getCurLen() == 137);

      iob.append(junk_buf, i+1); //move the head along
      iob.dropFromTail(137);

      if (memcmp(app_buf, app_buf2, 137) != 0) {
         cout << "ERROR: append/getBytes screwed up, i: " << i << " mx: " << mx << endl;
         cout << iob.toString() << endl;
         String tmp_str(500);
         cout << "     ";
         char foo_buf[50];
         for (int j = 0; j<137; j++) {
            sprintf(foo_buf, "%2x  %2x\n     ", (unsigned int)(app_buf[j]),
                    (unsigned int)(app_buf2[j]));
            tmp_str.append(foo_buf);
         }//for
         cout << tmp_str;
         exit(2);
      }//if
   }//for

   cout << "Passed append/getBytes tests.\n";

   if (1) {
      unsigned char* pktp = (unsigned char*)("123");
      int pkt_len = strlen((char*)(pktp));
      
      IOBuffer iob2(200);
      for (int i = 0; i<201; i++) {
         for (int j = 0; j< (i + 5); j++) {
            iob2.append(pktp, pkt_len);
            memset(buf, 0, pkt_len + 1);
            iob2.peekBytes(buf, pkt_len);
            if (memcmp(buf, pktp, pkt_len) != 0) {
               cerr << "Append FAILED: pktp -:" << pktp << ":- buf -:" << buf << endl;
            }
         }
         for (int j = 0; j<(i + 5); j++) {
            memset(buf, 0, pkt_len + 1);
            iob2.peekBytes(buf, pkt_len);
            if (memcmp(buf, pktp, pkt_len) != 0) {
               cerr << "Drop FAILED: pktp -:" << pktp << ":- buf -:" << buf << endl;
            }
            iob2.dropFromTail(pkt_len);
         }
      }
   }

   if (1) {
      unsigned char* pktp = (unsigned char*)("12345");
      int pkt_len = strlen((char*)(pktp));
      
      IOBuffer iob2(200);
      for (int i = 0; i<80; i++) {
         iob2.append(pktp, pkt_len);
         memset(buf, 0, pkt_len + 1);
         iob2.peekBytes(buf, pkt_len);
         if (memcmp(buf, pktp, pkt_len) != 0) {
            cerr << "FOO FAILED: pktp -:" << pktp << ":- buf -:" << buf << endl;
         }
         iob2.dropFromTail(pkt_len);
      }
   }

   if (1) {
      unsigned char* p1 = (unsigned char*)("1");
      unsigned char* p2 = (unsigned char*)("2");
      int pkt_len = 1;
      unsigned char* pktp;

      IOBuffer iob2(201);
      for (int i = 0; i<1000; i++) {
         if ((i % 2) == 0) {
            pktp = p1;
         }
         else {
            pktp = p2;
         }
         iob2.append(pktp, pkt_len);
         memset(buf, 0, pkt_len + 1);
         iob2.peekBytes(buf, pkt_len);
         if (memcmp(buf, pktp, pkt_len) != 0) {
            cerr << "FOO-1 FAILED: pktp -:" << pktp << ":- buf -:" << buf << ":-" << endl;
         }
         iob2.dropFromTail(pkt_len);
      }
   }

   cout << "Passed append many, drop many test." << endl;

   // Test full/empty boundary cases
   if (1) {
      IOBuffer iob2;
#define FE_MX 77777
      srand(FE_MX);
      unsigned char buf[FE_MX];
      for(int i = 0; i<FE_MX; i++) {
         buf[i] = rand();
      }

      for (int i = 0; i<77777; i++) {
         iob2.clear();

         assert(iob2.getCurLen() == 0);
         if (iob2.getMaxContigFree() != iob2.getMaxLen()) {
            cerr << "ERROR:  maxContig: " << iob2.getMaxContigFree()
                 << " != maxLen: " << iob2.getMaxLen() << endl;
            cerr << iob2.toString() << endl;
            exit(3);
         }

         iob2.append(buf, i);

         assert(iob2.getCurLen() == i);
         assert(iob2.getMaxContigFree() == (iob2.getMaxLen() - i));
         if (iob2.getMaxContigUsed() != i) {
            cerr << "ERROR:  maxContigUsed: " << iob2.getMaxContigUsed()
                 << " != i: " << i << endl;
            cerr << iob2.toString() << endl;
            exit(4);
         }

         iob2.assertSanity();

         if (iob2.getCurLen() != i) {
            cerr << "ERROR:  curlen: " << iob2.getCurLen() << " != i: " << i << endl;
            rv = 7;
         }

         unsigned char buf2[i];
         iob2.peekBytes(buf2, i);
         if (memcmp(buf2, buf, i) != 0) {
            cerr << "ERROR:  peeked bad bytes, i: " << i << endl;
            for (int q = 0; q<i; q++) {
               if (buf[q] != buf2[q]) {
                  cerr << " buf[" << q << "]: " << buf[q] << " != buf2: "
                       << buf2[q] << endl;
               }
            }
            rv = 7;
         }
      }
   }

   return rv;
}//main
