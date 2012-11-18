//
//Copyright (C) 1998-2009  Ben Greear
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

///**********************  string2.cc  ***************************///
/*  This is a fairly poweful string class, complete with socket IO
    based on TCP/IP protocol (read() write())  */

#include "string2.h"
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <string>
#include <iomanip>

#ifdef __WIN32__
#include <winsock.h>
#else
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif


                      /* class functions */


LogStream* String::logfile = NULL;
int String::string_cnt = 0;
int String::total_bytes = 0;


/* TODO:  Someday put this in a better place... */
struct Counter64u toCounter64u(__uint64_t cnt) {
   struct Counter64u retval;
   retval.lo = (cnt & 0xFFFFFFFF);
   retval.hi = (cnt >> 32) & 0xFFFFFFFF;
   return retval;
}

__uint64_t toUint64(const Counter64u& cnt) {
   __uint64_t retval = cnt.hi;
   retval = ((retval << 32) & (0xFFFFFFFF00000000LL));
   retval |= (cnt.lo & 0xFFFFFFFF);
   return retval;
}


/* TODO:  Someday put this in a better place... */
struct Counter64s toCounter64s(__int64_t cnt) {
   struct Counter64s retval;
   retval.lo = (cnt & 0xFFFFFFFF);
   retval.hi = (cnt >> 32) & 0xFFFFFFFF;
   return retval;
}

__int64_t toInt64(const Counter64s& cnt) {
   __int64_t retval = cnt.hi;
   retval = ((retval << 32) & (0xFFFFFFFF00000000LL));
   retval |= (cnt.lo & 0xFFFFFFFF);
   return retval;
}


int makeNBO(struct Counter64u& pld) {
   pld.hi = htonl(pld.hi);
   pld.lo = htonl(pld.lo);
   return 0;
}


int makeHBO(struct Counter64u& pld) {
   pld.hi = ntohl(pld.hi);
   pld.lo = ntohl(pld.lo);
   return 0;
}

int makeNBO(struct Counter64s& pld) {
   pld.hi = htonl(pld.hi);
   pld.lo = htonl(pld.lo);
   return 0;
}


int makeHBO(struct Counter64s& pld) {
   pld.hi = ntohl(pld.hi);
   pld.lo = ntohl(pld.lo);
   return 0;
}



// return a value that should be pretty unique.
// This is basically lame, do some research if you want a real
// hash function!! --BLG
unsigned int String::hash() {
   unsigned int retval = 0;

   for (int i = 0; i<cur_len; i++) {
      retval += (int)(string[i]);
   }
   retval += cur_len;
   return retval;
}

int String::Assert(int boolean_val, const char* msg) const {
   if (!boolean_val) {
      LOGFILE << "String::Assert, msg: " << msg << endl;
      cout << "String::Assert, msg: " << msg << endl;
   }
   assert(boolean_val);

   //if (!boolean_val) {
   //   core_dump(msg);
   //}
   return TRUE;
}

void String::setLogFile(LogStream* dafile) { logfile = dafile; }

LogStream* String::getLogFile() { return logfile; }

String String::sterilizeForHtml() const {
   String retval(Strlen() + 20);
   
   char ch;
   for (int i = 0; i<cur_len; i++) {
      ch = string[i];
      if (ch == '<') {
         retval += "&lt;";
      }
      else if (ch == '>') {
         retval += "&gt;";
      }
      else if (ch == '&') {
         retval += "&amp;";
      }
      else {
         retval += ch;
      }
   }//for
   return retval;
}//sterilizeForHtml

void String::strip() {
   int i;

   if (cur_len == 0)
      return;

   for (i = 0; i<cur_len; i++) {
      if (!isspace(string[i]))
         break;
   }
   
   //cout << "Strip1: i: " << i << " cur_len: " << cur_len << endl;
   if (i == cur_len) {
      Clear();
      return;
   }
   
   //cout << "Strip2: i: " << i << " cur_len: " << cur_len << endl;
   int end;
   for (end = cur_len; end > i; end--) {
      if (!isspace(string[end - 1]))
         break;
   }//for

   //cout << "Strip3: i: " << i << " cur_len: " << cur_len << " end: " << end << endl;
   if (end <= i) {
      Clear();
      return;
   }

   //cout << "Strip4: i: " << i << " cur_len: " << cur_len << " end: " << end << endl;
   char buf[cur_len + 1];
   memcpy(buf, string + i, (end - i));
   buf[(end - i)] = 0;

   *this = buf;
}//Strip

// Remove all instances of 'c' from this string.
void String::cleanChar(char c) {
   // Can't strip null terminator.
   if (!c) {
      return;
   }
   int j = 0;
   for (int i = 0; i<size(); i++) {
      if (this->string[i] != c) {
         this->string[j] = this->string[i];
         j++;
      }
   }
   this->string[j] = 0;
   cur_len = j;
} 


//drop a few off the end of the string
void String::dropFromEnd(int num_chars) {
   if (cur_len > num_chars) {
      string[cur_len - num_chars] = 0;
      cur_len -= num_chars;
   }
   else {
      *this = "";
   }
}

/** Starts inserting in the string[i+1] position. */
void String::insertAfter(int i, const char* str) {
   if (!str || (i < 0))
      return;

   if (i > cur_len) {
      this->Append(str);
      return;
   }

   ensureCapacity(cur_len + strlen(str));
   String tmp(string + i+1);
   strcpy(string + i+1, str);

   cur_len = strlen(string);

   this->Append(tmp);
}//insertAfter

void String::ensureCapacity(int max_length) {
   //LOGFILE << "In ensureCapacity...  max_length:  "
   //        << max_length << "  max_len:  " << max_len
   //        << endl << flush;
   if (!string) {
      if (max_length > 0) {
         string = new char[max_length + 1];
         string[0] = 0;
         cur_len = 0;
         max_len = max_length;
      }
   }
   else {
      if (max_len < max_length) {
         //cout << "Growing string: " << this << " max_length: " << max_length << endl;
         int ln = max_length + max_len + 1;
         total_bytes += max_length; //keep our static statistics
         max_len += max_length;
         char* tmp = string;
         string = new char[ln];
         strcpy(string, tmp);
         delete[] tmp;
      }
   }//if
   //LOGFILE << "In ensure Capacity...  done" << endl
   //        << flush;
}//ensureCapacity
   
//grabs everything untill that character is found.  It removes all that
// from the string, so successive calls iterate through it...
// Returned pointer is to a String allocated off of the Heap..ie delete it
// eventually if that makes sense.
String* String::getUntil(char dlm) {
   String* retval = new String(cur_len);
   
   int i;
   int found_dlm = FALSE;
   char ch;
   for (i = 0; i<cur_len; i++) {
      ch = *(string + i);
      if (ch == dlm) {
         found_dlm = TRUE;
         break;
      }
      else
         retval->Append(ch);
   }//for

   //now, move the string over
   if ((!found_dlm) || (i == cur_len)) {
      Clear();
   }
   else {
      String tmp((char*)(string + i + 1));
      *this = tmp;
   }

   if (retval->Strlen() > 0)
      return retval;
   else {
      delete retval;
      return NULL;
   }
}//getUntil


// After is supposed to be a boolean telling us to put it before
// or after... 
void String::parse_show(short after) { 
  String tmp(cur_len + 100); //doubt more that 100 newlines
  int i;
  char chr;

  for (i = 0; i<cur_len; i++) {
    if ((chr = string[i]) == '\n') {
       if (after)
          tmp.Append("\n\r");
       else
          tmp.Append("\r\n");
       continue;
    }//if
    tmp.Append(chr);
  }//for

  *this = tmp;
}//parse_show


/* these inserts don't work right */
void String::InsertBefore(char a, int posn) {

   ensureCapacity(cur_len + 1);
   
   int j = posn;
   char tmp = a;
   char tmp2;

   while (TRUE) {
      tmp2 = string[j];
      string[j] = tmp;
      if (!tmp) {
	 break;
      }//if
      tmp = tmp2;
      j++;
   }//while

   cur_len++;
}//InsertBefore


void String::InsertAfter(char a, int posn) {

   ensureCapacity(cur_len + 1);
   
   int j = posn + 1;
   char tmp = a;
   char tmp2;

   while (TRUE) {
      tmp2 = string[j];
      string[j] = tmp;
      if (!tmp) {
	 break;
      }//if
      tmp = tmp2;
      j++;
   }//while

   cur_len++;
}//InsertAfter

   

 
String::String() { //default constructor
   //log("In default const.\n");
   string_cnt++;
   string = NULL;
   cur_len = 0;
   max_len = -1;
} // constructor


String::String(const char* S) {
   //log("In char* const, here is S:  ");
   string_cnt++;
   if (S) {
      int i = strlen(S);
      string = new char[i + 1];
      cur_len = i;
      max_len = i;
      strcpy(string, S);
   }//if
   else {
      string = NULL;
      cur_len = 0; max_len = -1;
   }//else
   total_bytes += (max_len + 1);
} // constructor


String::String(const String& S) {
   //log("In String const, here is source:");
   string_cnt++;
   if (S.cur_len > 0) {
      string = new char[S.cur_len + 1];
      strcpy(string, S.string);
      cur_len = S.cur_len;
      max_len = S.cur_len;
   }
   else {
      string = NULL;
      cur_len = 0;
      max_len = -1;
   }

   total_bytes += (max_len + 1);
} // constructor


String::String(const int my_len) {
   int m_len = my_len;
   string_cnt++;

   if (m_len > 100000) {
      LOGFILE << "WARNING:  making string of size 100000" << endl;
      m_len = 100000;
   }

   if (m_len > 1) {
      string = new char[m_len + 1];
      cur_len = 0;
      max_len = m_len;
      string[0] = '\0';
   }//if
   else {
      string = NULL;
      cur_len = 0;
      max_len = -1;
   }//else

   total_bytes += (max_len + 1);
} // constructor


String::~String() {
   string_cnt--;
   total_bytes -= (max_len + 1);
   if (string) {
      delete[] string;
      string = NULL;
   }
} //destructor

void String::Report() const {
   char buf[strlen(string) + 100];
   sprintf(buf, "cur_len: %i, max_len: %i, string: %s", cur_len, 
           max_len, string);
   LOGFILE << "Report of String -:" << buf << ":-" << endl;
}//Report
 

String String::lookCommand(short is_first) const {
   String temp(100);
   int i = 0; 
   char a;

   //LOGFILE << "In look_command -:" << *this << ":-" << endl;
   
   if (max_len <= 0) { //trivial case...
      return temp;
   }//if

   a = string[i];
   while ((isspace(a)) || (a == '.')) {
      i++;
      a = string[i];
   }//while

   int in_quotes = FALSE;

   if (a == '\'') {
      if (is_first) {
         temp = '\'';
         return temp;
      }
      if (string[i+1] == '\'') {
         temp.append(a);
         i += 2;
         a = string[i];
      }//if
      else {
         in_quotes = TRUE;
         i++;
         a = string[i];
      }//else
   }//if

   while (TRUE) {
      if (!a || a == '\n')
         break; //done

      if (in_quotes) {
         if (a == '\'') {
            if (string[i+1] == '\'') { //escaped the quote
               temp.append(a);
            }//if
            else {
               break; //all done
            }//else
         }//if
      }//if
      else {
         if (isspace(a) || (a == '.')) {
            //then all done
            break;
         }//if
         else {
            temp.append(a);
         }//else
      }//else

      i++;
      a = string[i];
   }//while

   return temp;

}//look_command


String* String::getLine() {
   String* rv = new String(100);
   int q;
   bool found_newline = false;
   for (q = 0; q<size(); q++) {
      char c = string[q];
      if (c == '\n') {
         found_newline = true;
         q++; // get past newline
         break;
      }
      if (c != '\r') {
         rv->append(c);
      }
   }

   if (found_newline) {
      if (q == size()) {
         cur_len = 0;
         string[0] = 0;
      }
      else {
         memmove(&string[0], &string[q], cur_len - q);
         cur_len -= q;
         string[cur_len] = 0; // just in case
      }
      return rv;
   }
   else {
      delete rv;
      return NULL;
   }
}
   

String String::getCommand(short& eos, short& term_by_period,
                          short is_first, short ignore_period) {
   String temp(100);
   int i = 0, k = 0;
   eos = term_by_period = FALSE;
   char a;
   String buf(300);

   //LOGFILE << "In Get_command -:" << *this << ":-" << endl;

   if (!string) {
      // Empty string
      return temp;
   }

   a = string[i];
   while ((isspace(a)) || (!ignore_period && (a == '.'))) {
      i++;
      a = string[i];
   }//while

   if (a) {			//wasn't all spaces

      //LOGFILE << "a -:" << a << ":-  i:" << i << endl;

      if (is_first && (a == '\'')) {
         temp = '\'';
         i++; //move to the next one
      }
      else {
         if ((a == '\'') && (string[i+1] != '\''))  { // in quotes

            //LOGFILE << "In quotes." << endl;
            
            i++;
            a = string[i];
            while (a && (a != '\n')) {
               if (a == '\'') {
                  if (string[i+1] == '\'') { //then escape it.
                     temp += a;
                     i += 2;
                  }//if
                  else {
                     break;
                  }
               }//if
               else {
                  temp += a;
                  i++;
               }//else
               a = string[i];
            }//while
            if (a == '\'') { //closing quote
               i++; //move past it
            }//if
         }//if
         else {
            if ((a == '\'') && (string[i+1] == '\'')) {
               temp.append(a);
               i += 2;
               a = string[i];
            }//if
            
            while (!(isspace(a) || (!ignore_period && (a == '.')) || !a)) {
               temp += a;
               i++;
               a = string[i];
            }//while
            if (a == '.')
               term_by_period = TRUE;
         }//else
      }//else
   }//if
   else {
      string[0] = '\0';  //null it out, was empty.. I think :)
      cur_len = 0;
      eos = TRUE;
      return temp;		//it was only spaces i spose
   }//else

/*    This gives a very detailed look at the string in question..for debugin
   for (int ss = 0; string[ss]; ss++) {
      buf.Append(ss); buf += "\t-=|"; 
      buf += string[ss]; buf += "|=-\n";
   }//for
   log(buf);

   buf = "Here is i:  ";
   buf.Append(i);
   log(buf);

   buf = "Here is cur_len:  ";
   buf.Append(cur_len);
   log(buf);
*/

   if (string[i] == '\n') {
      //log("String[i] was a newline.\n");
      eos = TRUE;
      i++;				//to get past newline char
   }//if

   k = 0;
   while (i < cur_len) {
      string[k] = string[i];
      i++; 
      k++;
   }//while
   string[k] = '\0';
   cur_len = k;

/*        more debugging stuff
   log("After adjustment...\n");
   buf = '\0';
   for (int ss = 0; string[ss]; ss++) {
      buf.Append(ss); buf += "\t-=|"; 
      buf += string[ss]; buf += "|=-\n";
   }//for
   log(buf);
*/
   
   //log("Here is string returned by get_command:");
   //log(temp);
   return temp;
}//get_command
	 
/* If destruct is TRUE, then the returned String will be removed
 * from the beginning of this string.
 */
String String::getRest(short destruct) {
   String temp(300);
   int i = 0, k = 0;
   char a;

   //log("In get_rest\n");

   if (string[0] == ' ')
      i++;	//junk leading space

   while (((a = string[i]) != '\n') && (a)) {
      temp += a;
      i++;
   }//while
   if (a == '\n')
      i++;	//advance past newline
   
   if (destruct) {
      while (i < cur_len) {
         string[k] = string[i];
         i++; 
         k++;
      }//while
      string[k] = '\0';
      cur_len = k;
   }//if should destruct

   return temp;
}//get_rest         



/*  This function is book keeping for when you change the string with the 
    [] operator.  This is a bit messy, but I need the power, and
    hopefully, this here will allow some security as well, if
    used correctly.  */

void String::Refigure_strlen() {
   if (string) {
      cur_len = strlen(string);

      if (cur_len > max_len) {
         LOGFILE.log(LS_ERROR, "ERROR:  cur_len is > max_len, end of string was\n");
         //log("over-written.  Good luck, you'll need it!\n");
         cur_len = max_len;
         string[cur_len] = '\0';  //try to salvage the situation..
      }//if
   }
   else {
      cur_len = 0;
      max_len = -1;
   }
}//Refigure_strlen

int String::contains(const char ch) const {
   int retval = 0;
   for (int i = 0; i<cur_len; i++) {
      if (string[i] == ch) {
         retval++;
      }
   }//for

   return retval;
}


/**  Reads in bytes untill it gets to the dlm.  It does NOT include the
 * delimiter.  A backslash escapes the delimiter.
 */
void String::readUntil(char delim, istream& dafile) {
   int is_escaped = FALSE;
   char ch;
   clear();
   // Get to the beginning
   while (dafile) {
      dafile.get(ch);
      if (ch == '\\') {
         if (is_escaped) {
            is_escaped = FALSE;
            this->append(ch);
         }
         else {
            is_escaped = TRUE;
         }
      }
      else if (ch == delim) {
         if (is_escaped) {
            is_escaped = FALSE;
            this->append(ch);
         }
         else {
            return;
         }
      }
      else {
         is_escaped = FALSE;
         this->append(ch);
      }
   }//while
}//readUntil


/** Reads untill it finds the delim, and then reads untill
 * it finds another.  Escape the delim with a \ (backslash).
 * This is not too efficient, as it reads one character at
 * a time, btw.
 */
int String::readToken(char delim, istream& dafile, int include_delim) {
   char ch;
   int is_escaped = FALSE;

   clear();

   // Get to the beginning
   while (dafile) {
      dafile.get(ch);
      if (ch == '\\') {
         if (is_escaped) {
            is_escaped = FALSE;
         }
         else {
            is_escaped = TRUE;
         }
      }
      else if (ch == delim) {
         if (is_escaped) {
            is_escaped = FALSE;
         }
         else {
            break;
         }
      }
      else {
         is_escaped = FALSE;
      }
   }//while

   if (!dafile)
      return -1;

   if (include_delim)
      *this += delim;

   while (dafile) {
      dafile.get(ch);
      if (ch == '\\') {
         if (is_escaped) {
            is_escaped = FALSE;
            *this += ch;
         }
         else {
            is_escaped = TRUE;
         }
      }
      else if (ch == delim) {
         if (is_escaped) {
            is_escaped = FALSE;
            *this += ch;
         }
         else {
            break;
         }
      }
      else {
         // Only escape the delimiter at this time.
         if (is_escaped) {
            *this += '\\';
         }
         *this += ch;
         is_escaped = FALSE;
      }
   }//while

   if (!dafile)
      return -1;

   if (include_delim)
      *this += delim;

   return 0;
}//readToken


void String::termedRead(istream& da_file) { //reads lines untill it finds
					      // a line containing only "~"
   char buf[182];
   short test;

   *this = "";  

   Assert(da_file && 1, "WARNING:  String::Termed_Read, da_file is null\n"); 

   da_file.getline(buf, 180);

   if (LOGFILE.ofLevel(DB)) {
      LOGFILE << "String::Termed_Read, buf -:" << buf << ":-" << endl;
   }

   test = strcmp(buf, "~");
   while (test) {
      if (!da_file) {
         LOGFILE.log(WRN,
                     "WARNING:  String::Termed_Read, in while da_file null\n");
         break;
      }//if

      (*this) += buf;
      da_file.getline(buf, 180);

      if (LOGFILE.ofLevel(DB)) {
         LOGFILE << "String::Termed_Read, in while buf -:" 
                 << buf << ":-" << endl;
      }

      if (strcmp(buf, "~") != 0) {
         (*this) += "\n";
      }//while
      else {
         test = FALSE;
      }//else
   }//while
}//Termed_Read


void String::toLower() {
   for (int i = 0; i < cur_len; i++) {
      string[i] = tolower(string[i]);
   }//for
}//Tolower

void String::toUpper() {
   for (int i = 0; i < cur_len; i++) {
      string[i] = toupper(string[i]);
   }//for
}//Tolower


int String::Strlen() const {
   return cur_len;
}//Strlen


int String::Write(const int desc, const int max_to_write) {
   int sofar = 0, this_round;
   String buf;

   if (desc == -1) {  //cleans up an error message or two.
      return -1;
   }//if

   Assert((max_to_write <= cur_len),
          "ERROR:  trying to write more than exists...cya!\n");

   do {
      //Sprintf(buf, "In Write, desc:  %i\n", desc);
      //log(buf);
      this_round = send(desc, (string + sofar), 
                         max_to_write - sofar, 0);
      if (this_round < 0) { //some error happened
#ifdef __WIN32__
         if (WSAGetLastError() == WSAEWOULDBLOCK) {
#else
         if ((errno == EAGAIN) || (errno == EINTR)) {
#endif
            break; //done
         }
         else {
            perror("String.Write() err");
            LOGFILE << "ERROR:  String.Write() err: " << errno << ": "
                    << strerror(errno) << " desc: " << desc
                    << " rv: " << this_round << endl;
#ifdef __WIN32__
            LOGFILE << "  WSAError: " << WSAGetLastError() << endl;
#endif
            string[0] = '\0';
            cur_len = 0;
            return -1;
         }
      }//if
      else if (this_round == 0) {
         string[0] = '\0';
         cur_len = 0;
         return sofar;
      }//if
      else if ((sofar += this_round) >= max_to_write) {
         break; //done
      }
   } while (TRUE);


   if (sofar == cur_len) { //wrote it all
      string[0] = '\0';
      cur_len = 0;
   }
   else {
      // These can overlap, the +1 is for the null terminator.
      memmove(string, string + sofar, (cur_len - sofar + 1));
      cur_len = strlen(string);
   }//else

   return sofar;

}//Write (to a descriptor)


int String::Read(const int desc, const int max_to_read, bool ignore_semi_colons, bool do_recv) {
   int begin = 0;
   int sofar = 0;
   int this_round = 0;
   int i, j;
   String tmp(100);
   //Sprintf(tmp, "begin: %i, sofar: %i, input now: %S",
   //       begin, sofar, this);
   //log(tmp);

   ensureCapacity(max_to_read);
   begin = Strlen();

   //log("Beginning of Read do, here is input so far:");
   //log(*this);

   do {
      int to_rd = (max_to_read - (begin + sofar) - 1);
#ifdef __WIN32__
      unsigned long avail_rd = 0xFFFFFFFF;
      ioctlsocket(desc, FIONREAD, &avail_rd);
      if (avail_rd == 0) {
         LOGFILE << "About to read desc: " << desc << " avail_rd: " << avail_rd
                 << " to_rd: " << to_rd << endl;
      }
      if ((avail_rd > 0) && (to_rd > (int)(avail_rd))) {
         to_rd = avail_rd;
      }
      // NOTE:  Need to let it read zero to detect end-of-stream.
      //if (to_rd == 0) {
      //   break;
      //}
#endif
      if (do_recv) {
         this_round = recv(desc, (string + sofar + begin), to_rd, 0);
      }
      else {
         this_round = read(desc, (string + sofar + begin), to_rd);
      }
      //LOGFILE << "Completed read read, rv: " << this_round << endl;
      
      if (this_round > 0) {
         sofar += this_round;
         if (this_round == to_rd) {
            // We have all we need
            break;
         }
      }//if
      else {
         if (this_round < 0) { //some error happened
#ifdef __WIN32__
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
#else
            if ((errno != EAGAIN) && (errno != EINTR)) { //== EWOULDBLOCK
#endif
               //perror("String.Read() err, not EAGAIn");
               string[sofar] = '\0';
               cur_len = sofar;
               //log("ERROR:  String::Read exitted on error.\n");
               //log(*this);
               return -1;
            }//if
            else  //this means it read all in buffer
               break;
         }//if
         else {
            //log("ERROR:  EOF encountered on socket read.\n");
            string[sofar] = '\0';
            cur_len = sofar;
            if (sofar == 0) {
               return -1;
            }
            else {
               break;
            }
         }//else
      }//else
   } while (TRUE); //*(string + begin + sofar -1) != '\n'); //newline


   *(string + begin + sofar) = 0;
   cur_len = begin + sofar;

/*		This is for debugging
      tmp = '\0';
      for (int ss = 0; string[ss]; ss++) {
         tmp.Append(ss); tmp += "\t-=|"; 
         tmp += string[ss]; tmp += "|=-\n";
      }//for
      log(tmp);
*/

		/* get rid of DOS's carriage return */

   j = begin;
   char a;
   bool in_quotes = false;
   for (i = begin; (a = string[i]); i++) {
      if (a == 8) {  //deal with backspace
         if (j > begin)
            j--;
      }//if
      else if (a == '\'') {
         in_quotes = !in_quotes;
         string[j] = a;
         j++;
      }
      else if (a == ';') {
         if (ignore_semi_colons || in_quotes) {
            string[j] = a;
            j++;
         }
         else {
            if (string[i+1] == ';') { //should escape it
               string[j] = ';';
               j++;
               i++; //move past second semicolon
            }//if
            else {
               string[j] = '\n';
               j++;
            }//else
         }
      }//if should deal with semicolon
      else if (a != 13) { //delete DOS's CR
         string[j] = a;
         j++;
      }//if
   }//for
   string[j] = 0;
   cur_len = j;
   
   char chr;
   for (i = begin; ((chr = string[i]) != '\n'); i++) {
      if (!chr) {
         return 0;	//do nothing..has no newline in it
      }//if
   }//for
   
   return 1;
}//Read (from a descriptor)


void String::purge() {
   if (string) {
      total_bytes -= (max_len + 1);
      delete[] string;
      string = NULL;
      cur_len = 0;
      max_len = -1;
      total_bytes += (max_len + 1);
   }
}//Clear      


void String::clear() {
   if (string) {
      string[0] = 0;
      cur_len = 0;
   }
}//Clear


void String::cap() {
   for (int i = 0; i<cur_len; i++) {
      if (isalpha(string[i])) {
         string[i] = toupper(string[i]);
         return;
      }//if
   }//for
}//Cap      

// TODO:  Optimize this to read right into our internal buffer.
void String::getLine(istream& stream, int max_length) {
   char buf[max_length + 1];
   buf[0] = 0;
   stream.getline(buf, max_length);
   *this = buf;
}//Getline      


/** Makes 9 look like: 1001 */
void String::appendAsBinary(unsigned long num) {
   int i;
   for (i = 31; i>=0; i--) {
      if (num & (1<<i)) {
         break;
      }
   }

   for (; i>=0; i--) {
      if (num & (1<<i)) {
         append("1");
      }
      else {
         append("0");
      }
   }
}

void String::append(const char* source, int len) {
   if (!source)
      return;

   ensureCapacity(cur_len + len + 1);

   strncat(string + cur_len, source, len);
   cur_len += len;
}//append      


void String::append(const char* source) {
   if (!source)
      return;
   int k = strlen(source);

   ensureCapacity(cur_len + k);

   /*  string + cur_len suggested by Nevyn.  Good idea, should
    *   make it faster. --Ben
    */
   strcat(string + cur_len, source);
   cur_len += k;
}//append      


void String::append(int i) {
   char source[25];
   sprintf(source, "%i", i);
   this->append(source);
}//append

void String::append(unsigned int i) {
   char source[25];
   sprintf(source, "%u", i);
   this->append(source);
}//append

void String::append(long i) {
   char source[25];
   sprintf(source, "%li", i);
   this->append(source);
}//append      


void String::append(unsigned long i) {
   char source[25];
   sprintf(source, "%lu", i);
   this->append(source);
}//append      


void String::append(unsigned long long i) {
   char source[45];
   sprintf(source, "%llu", i);
   this->append(source);
}//append      


void String::append(long long i) {
   char source[45];
   sprintf(source, "%lli", i);
   this->append(source);
}//append      

void String::append(void* v) {
   char source[50];
   sprintf(source, "%p", v);
   this->append(source);
}//append


void String::appendHex(unsigned long i) {
   char source[25];
   sprintf(source, "%x", (unsigned int)(i));
   this->append(source);
}//append      


void String::append(char c) {
   char buf[2];
   sprintf(buf, "%c", c);
   this->append(buf);
}//append      

void String::append(unsigned char c) {
   char buf[2];
   sprintf(buf, "%c", c);
   this->append(buf);
}//append      


void String::append(float f, int rem) {
   append((double)(f), rem);
}//append      


void String::append(double d, int rem) {
   char buf[40];
   char args[20];
   sprintf(args, "%%.%if", rem);
   sprintf(buf, args, d);
   this->append(buf);
}//append


void String::append(const String& S) {
   if (&S != this) {
      if (S.cur_len > 0) {
         ensureCapacity(cur_len + S.cur_len);
         strcat(string, S.string);
         cur_len += S.cur_len;
      }
   }
   else {
      String tmp(S);
      append(tmp);
   }
}//append      


void String::prepend(int i) {
   char source[25];
   sprintf(source, "%i", i);
   this->prepend(source);
}//prepend      

void String::prepend(char i) {
   char source[25];
   sprintf(source, "%c", i);
   this->prepend(source);
}//prepend      


void String::prepend(long i) {
   char source[25];
   sprintf(source, "%d", (int)(i));
   this->prepend(source);
}//prepend      


void String::prepend(const char* source) {
   String tmp(*this); //keep a copy handy
   *this = source;
   this->append(tmp);
}//prepend      


void String::prepend(const String& source) {
   this->prepend(source.string);
}//prepend      


   
///*********************  OPERATORS  ******************************///

String::operator const char*() const {
   if (string) {
      return string;
   }
   return "";
}//to char* operator overload


String& String::operator+= (const String &S) {
   this->append(S);
   return *this;
}//+= operator

String& String::operator+= (const float f) {
   this->append(f);
   return *this;
}//+= operator

String& String::operator+= (const double d) {
   this->append(d);
   return *this;
}//+= operator

String& String::operator+= (const char s) {
   this->append(s);
   return *this;
}//+= operator


String& String::operator+= (const char* S) {
   this->append(S);
   return *this;
}//+= operator


String& String::operator+= (const int i) {
   this->append(i);
   return *this;
}

String& String::operator+= (const unsigned int i) {
   this->append((unsigned long)(i));
   return *this;
}

String& String::operator+= (const long l) {
   this->append((int)l); //bah, same thing in a real computer
   return *this;
}

String& String::operator+= (const unsigned long l) {
   this->append(l);
   return *this;
}

String& String::operator+= (const unsigned long long l) {
   this->append(l);
   return *this;
}

String& String::operator+= (const long long l) {
   this->append(l);
   return *this;
}

String& String::operator= (const int i) {
   Clear();
   this->append(i);
   return *this;
}

String& String::operator= (const unsigned int i) {
   Clear();
   this->append((unsigned long)(i));
   return *this;
}

String& String::operator= (const long l) {
   Clear();
   append(l);
   return *this;
}

String& String::operator= (const unsigned long l) {
   Clear();
   append(l); //same thing in a real computer, for now at least
   return *this;
}

String& String::operator= (const long long l) {
   Clear();
   append(l);
   return *this;
}

String& String::operator= (const unsigned long long l) {
   Clear();
   append(l);
   return *this;
}

String& String::operator= (const char c) {
   Clear();
   append(c);
   return *this;
}


String& String::operator= (const String &S) {
   if (S.string && S.cur_len) {
      ensureCapacity(S.cur_len);
      strcpy(string, S.string);
      cur_len = S.cur_len;
   }
   else {
      Clear();
   }
   return *this;
}//assignment operator


String& String::operator= (const char* S) {
   int k;
   if ((S == NULL) || ((k = strlen(S)) == 0)) {
      Clear();
      return *this;
   }//if
   ensureCapacity(k);
   strcpy(string, S);
   cur_len = k;
   return *this;
}//assignment operator

char String::charAt(int i) const {
   Assert(((i >= 0) && (i <= max_len)), "ERROR:  Index out of range, charAt.\n");
   return string[i];
}

void String::setCharAt(int idx, const char val) {
   Assert(((idx >= 0) && (idx < cur_len)), "ERROR: Index out of range, setCharAt\n");   
   Assert(val, "ERROR:  setCharAt:  trying to set a NULL value.\n");
   string[idx] = val;
}
      
char String::operator[] (const unsigned int i) const {
   Assert(((int)(i) < max_len), "ERROR:  index to high, String::operator[]\n");

   return string[i];

}//[] operator

char String::operator[] (const int i) const {
   Assert((i < max_len) && (i >= 0), "ERROR:  index to high, String::operator[]\n");
   return string[i];
}//[] operator

//Friend
String operator+(const char* lhs, const String& rhs) {
   String tmp(lhs);
   tmp.append(rhs);
   return tmp;
}

String String::operator+ (const char* S) const {
   String tmp(*this);
   tmp.append(S);
   return tmp;
}//assignment operator

String String::operator+ (const char S) const {
   String tmp(*this);
   tmp.append(S);
   return tmp;
}//assignment operator

String String::operator+ (const String& S) const {
   String tmp(*this);
   tmp.append(S);
   return tmp;
}//assignment operator

String String::operator+ (const int i) const {
   String tmp(*this);
   tmp.append(i);
   return tmp;
}//append operator

String String::operator+ (const long i) const {
   String tmp(*this);
   tmp.append(i);
   return tmp;
}//append operator

String String::operator+ (const long long i) const {
   String tmp(*this);
   tmp.append(i);
   return tmp;
}//append operator

String String::operator+ (const unsigned int i) const {
   String tmp(*this);
   tmp.append(i);
   return tmp;
}//append operator

String String::operator+ (const unsigned long i) const {
   String tmp(*this);
   tmp.append(i);
   return tmp;
}//append operator

String String::operator+ (const unsigned long long i) const {
   String tmp(*this);
   tmp.append(i);
   return tmp;
}//append operator


/* equality operators, case insensitive */
int String::operator!= (const char* S) const {
   return strcasecmp(c_str(), S);
}//assignment operator


int String::operator== (const char* S) const {
   return (!(strcasecmp(c_str(), S)));
}//assignment operator

int String::operator< (const char* S) const {
   return (strcasecmp(c_str(), S) < 0);
}//assignment operator

int String::operator> (const char* S) const {
   return (strcasecmp(c_str(), S) > 0);
}//assignment operator

int String::operator> (const String& S) const {
   return (strcasecmp(c_str(), S.c_str()) > 0);
}//assignment operator

int String::operator< (const String& S) const {
   return (strcasecmp(c_str(), S.c_str()) < 0);
}//assignment operator

int String::operator<= (const char* S) const {
   return (strcasecmp(c_str(), S) <= 0);
}//assignment operator

int String::operator>= (const char* S) const {
   return (strcasecmp(c_str(), S) >= 0);
}//assignment operator




              /* friend functions */

void Sprintf(String& targ, const char* string, ... ) {
   va_list ap;
   va_start(ap, string);

   vSprintf(targ, string, ap);
   va_end(ap);
   return;
}

void vSprintf(String& targ, const char* string, va_list ap) {
   int i = 0;
   int num = 0;
   String buf(100);
   int s_len;
   char* tmp_chr_str;
   String* tmp_str;
   void* junk;
   char ch_buf[50];
   Counter64u* c64u;
   unsigned long long* i64u;
   long long* i64;
   double f;

   //Counter64s* c64s;
   int tmpi;

   targ = ""; //initialize targ

   for (i = 0; string[i]; ) {
      if (string[i] == '%') {
         i++;
         if (string[i] != '%') {
            //cerr << __PRETTY_FUNCTION__ << ": string[" << i << "]: " << (char)(string[i]) << endl;
            switch (string[i]) {
               // Only handle signed counter-64 at this time. --Ben
               case 'U':
                  c64u = (Counter64u*)(va_arg(ap, void*));
                  if (c64u) {
                     sprintf(ch_buf, "%llu", (unsigned long long)(toUint64(*c64u)));
                     targ.append(ch_buf);
                  }
                  break;
               case 'c':    /* char */  
                  targ.append((char)(va_arg(ap, int)));
                  break;
               case 'f':    /* float, 4 points of precision */
                  f = va_arg(ap, double);
                  targ.append(f, 4);
                  break;
               case 'i':    /* int */  
                  targ.append(va_arg(ap, int));
                  break;
               case 'u':    /* unsigned int32 */  
                  targ.append(va_arg(ap, unsigned int));
                  break;
               case 'I':    /* ignore */
                  junk = va_arg(ap, void*);
                  // Do nothing with it!!
                  break;
               case 'L': /* uint64 */
                  i64u = (unsigned long long*)(va_arg(ap, void*));
                  targ.append(*i64u);
                  break;
               case 'D': /* int64 */
                  i64 = (long long*)(va_arg(ap, void*));
                  targ.append(*i64);
                  break;
               case 'd':     /* long, turn it into an int */
                  targ.append(va_arg(ap, int));
                  break;
               case 's':      /* char * */
                  if ((tmp_chr_str = va_arg(ap, char *))) {
                     targ.append(tmp_chr_str);
                  }//if
                  break;
               case 'p':    /* pointer */
                  targ.append(va_arg(ap, void*));
                  break;
               case 'S':      /* String */
                  if ((tmp_str = va_arg(ap, String *))) {
                     targ.append(*tmp_str);
                  }//if
                  break;
               case 'P':      /* pad w/spaces */
                  i++; //looking at first number now
                  if (isdigit(string[i+1])) { //two digits?
                     if (isdigit(string[i])) { //it should be
                        num = 10 * ((int)(string[i]) - 48);
                        num += (int)(string[i+1]) - 48;
                     }//if
                     else {
                        LOGFILE.log(LS_ERROR, "ERROR:  Sprintf, case 'P', first not an int\n");
                     }
                  }//if two digits
                  else {
                     LOGFILE.log(LS_ERROR, "ERROR:  Sprintf, case 'P', second not an int\n");
                  }
                  i += 2; //now pointing to next valid letter
                  
                  s_len = targ.Strlen();
                  while (s_len < num) {
                     targ.append(" ");  //pad another space
                     s_len++;
                  }//while
                  i--;  //slight hack
                  break;
               case 'x':
                  tmpi = va_arg(ap, int);
                  sprintf(ch_buf, "%X", (unsigned int)(tmpi));
                  targ.append(ch_buf);
                  break;
               default:
                  sprintf(ch_buf, "ERROR: default (%c) called in Sprintf.\n",
                          string[i]);
                  cout << ch_buf << endl;
                  targ.Assert(FALSE, ch_buf);
            }//switch
            i++;
         }//if
         else { //
            i++;
            targ.append("%");
         }//else
      }//if
      else {
         targ.append(string[i]);
         i++;
      }//else
   }//for
}//vSprintf

/* WARNING:  this will truncate if input is > 299 chars. */
istream& operator>> (istream& stream, String& str) {
   char buf[300];
   memset(buf, 0, sizeof(buf));
   stream >> setw(sizeof(buf) - 1) >> buf;
   str = buf;
   return stream;
}//write_out operator

/** Let the char* operator just turn it into a char* 
ostream& operator<< (ostream& stream, const String& str) {
   stream << str.string;
   return stream;
}//write_out operator
*/
