// $Id: string2.cc,v 1.13 1999/08/03 05:55:34 greear Exp $
// $Revision: 1.13 $  $Author: greear $ $Date: 1999/08/03 05:55:34 $

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

///**********************  string2.cc  ***************************///
/*  This is a fairly poweful string class, complete with socket IO
    based on TCP/IP protocol (read() write())  */

#include "string2.h"
#include <stdarg.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

extern int core_dump(const char* msg); //misc2.cc

                      /* class functions */


LogStream* String::logfile = NULL;
int String::string_cnt = 0;
int String::total_bytes = 0;

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
      core_dump(msg);
   }
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

void String::Strip() {
   int i;

   if (cur_len == 0)
      return;

   for (i = 0; i<cur_len; i++) {
      if (!isspace(string[i]))
         break;
   }
   
   if (i == cur_len) {
      Clear();
      return;
   }
   
   int end;
   for (end = cur_len - 1; end >= i; end--) {
      if (!isspace(string[end]))
         break;
   }//for

   if (end <= i) {
      Clear();
      return;
   }

   char buf[cur_len + 1];
   memcpy(buf, string + i, (end - i) + 1);
   buf[(end - i) + 1] = 0;

   *this = buf;
}//Strip

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
   //LOGFILE << "In ensure Capacity...  max_length:  "
   //        << max_length << "  max_len:  " << max_len
   //        << endl << flush;
   if (max_len < max_length) {
      char* tmp = string;
      string = new char[max_length + max_len + 1];
      total_bytes += max_length; //keep our static statistics
      max_len += max_length; //cur_len remains the same here
      strcpy(string, tmp);
      delete[] tmp;
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
   string = new char[2];
   string[0] = '\0';
   cur_len = 0;
   max_len = 1;

   total_bytes += 2;
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
      string = new char[2];
      string[0] = '\0';
      cur_len = 0; max_len = 1;
   }//else
   total_bytes += (max_len + 1);
} // constructor


String::String(const String& S) {
   //log("In String const, here is source:");
   string_cnt++;
   string = new char[S.cur_len + 1];
   strcpy(string, S.string);
   cur_len = S.cur_len;
   max_len = S.cur_len;

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
      string = new char[2];
      cur_len = 0;
      max_len = 1;
      string[0] = '\0';
   }//else

   total_bytes += (max_len + 1);
} // constructor


String::~String() {
   string_cnt--;
   total_bytes -= (max_len + 1);
   delete[] string;
   string = NULL;
} //destructor

void String::Report() const {
   char buf[strlen(string) + 100];
   sprintf(buf, "cur_len: %i, max_len: %i, string: %s", cur_len, 
           max_len, string);
   LOGFILE << "Report of String -:" << buf << ":-" << endl;
}//Report
 

String String::Look_Command(short is_first = 0) const {
   String temp(100);
   int i = 0; 
   char a;

   //LOGFILE << "In look_command -:" << *this << ":-" << endl;
   
   if (max_len == 0) { //trivial case...
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
         temp.Append(a);
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
               temp.Append(a);
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
            temp.Append(a);
         }//else
      }//else

      i++;
      a = string[i];
   }//while

   return temp;

}//look_command


String String::Get_Command(short& eos, short& term_by_period, short is_first = 0) {
   String temp(100);
   int i = 0, k = 0;
   eos = term_by_period = FALSE;
   char a;
   String buf(300);

   //LOGFILE << "In Get_command -:" << *this << ":-" << endl;

   a = string[i];
   while ((isspace(a)) || (a == '.')) {
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
               temp.Append(a);
               i += 2;
               a = string[i];
            }//if
            
            while (!(isspace(a) || a == '.' || !a)) {
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
	 

String String::Get_Rest(short destruct = TRUE) {
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
   cur_len = strlen(string);
   if (cur_len > max_len) {
      LOGFILE.log(ERR, "ERROR:  cur_len is > max_len, end of string was\n");
      //log("over-written.  Good luck, you'll need it!\n");
      cur_len = max_len;
      string[cur_len] = '\0';  //try to salvage the situation..
   }//if
}//Refigure_strlen

int String::Contains(const char ch) const {
   int retval = 0;
   for (int i = 0; i<cur_len; i++) {
      if (string[i] == ch) {
         retval++;
      }
   }//for

   return retval;
}


/** Reads untill it finds the delim, and then reads untill
 * it finds another.  Escape the delim with a \ (backslash).
 * This is not too efficient, as it reads one character at
 * a time, btw.
 */
int String::readToken(char delim, ifstream& dafile, int include_delim) {
   char ch;
   int is_escaped = FALSE;

   Vclear();

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

   if (!da_file) {
      LOGFILE.log(WRN, "WARNING:  String::Termed_Read, da_file is null\n");
      return;
   }

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


void String::Tolower() {
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
      this_round = write(desc, (string + sofar), 
                         max_to_write - sofar);
      if (this_round < 0) { //some error happened
         perror("String.Write() err");
         LOGFILE.log(ERR, "ERROR:  String.Write() err.\n");
         string[0] = '\0';
         cur_len = 0;
         return -1;
      }//if
      else if (this_round == 0) {
         string[0] = '\0';
         cur_len = 0;
         return sofar;
      }//if
      else if ((sofar += this_round) >= max_to_write) {
         if (max_to_write == cur_len) { //wrote it all
            string[0] = '\0';
            cur_len = 0;
         }
         else {
            // These can overlap, the +1 is for the null terminator.
            memmove(string, string + sofar, (cur_len - sofar + 1));
            cur_len = strlen(string);
         }//else
         return sofar;
      }//if
   } while (TRUE);
}//Write (to a descriptor)


int String::Read(const int desc, const int max_to_read) {
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
      this_round = read(desc, (string + sofar + begin), 
                        max_to_read - (begin + sofar) - 1);
      
      if (this_round > 0) {
         sofar += this_round;
      }//if
      else {
         if (this_round < 0) { //some error happened
            if (errno != EAGAIN) { //== EWOULDBLOCK
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
            return -1;
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
   for (i = begin; (a = string[i]); i++) {
      if (a == 8) {  //deal with backspace
         if (j > begin)
            j--;
      }//if
      else if (a == ';') {
         if (string[i+1] == ';') { //should escape it
            string[j] = ';';
            j++;
            i++; //move past second semicolon
         }//if
         else {
            string[j] = '\n';
            j++;
         }//else
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
   total_bytes -= (max_len + 1);
   delete[] string;
   string = new char[2];
   cur_len = 0;
   max_len = 1;
   total_bytes += (max_len + 1);
   string[0] = '\0';
}//Clear      


void String::Clear() {
   *this = "\0";
}//Clear


void String::Cap() {
   for (int i = 0; i<cur_len; i++) {
      if (isalpha(string[i])) {
         string[i] = toupper(string[i]);
         return;
      }//if
   }//for
}//Cap      

void String::Getline(istream& stream, int max_len) {
   char buf[max_len + 1];
   stream.getline(buf, max_len);
   *this = buf;
}//Getline      


void String::Append(const char* source) {
   if (!source)
      return;
   int k = strlen(source);

   ensureCapacity(cur_len + k);

   /*  string + cur_len suggested by Nevyn.  Good idea, should
    *   make it faster. --Ben
    */
   strcat(string + cur_len, source);
   cur_len += k;
}//Append      


void String::Append(const int i) {
   char source[25];
   sprintf(source, "%i", i);
   this->Append(source);
}//Append      

void String::appendHex(const long i) {
   char source[25];
   sprintf(source, "%lx", i);
   this->Append(source);
}//Append      


void String::Append(const char c) {
   char buf[2];
   sprintf(buf, "%c", c);
   this->Append(buf);
}//Append      


void String::Append(const String& S) {
   ensureCapacity(cur_len + S.cur_len);
   strcat(string, S.string);
   cur_len += S.cur_len;
}//Append      


void String::Prepend(const int i) {
   char source[25];
   sprintf(source, "%i", i);
   this->Prepend(source);
}//Prepend      


void String::Prepend(const long i) {
   char source[25];
   sprintf(source, "%d", (int)(i));
   this->Prepend(source);
}//Prepend      


void String::Prepend(const char* source) {
   String tmp(*this); //keep a copy handy
   *this = source;
   this->Append(tmp);
}//Prepend      


void String::Prepend(const String& source) {
   this->Prepend(source.string);
}//Prepend      


   
///*********************  OPERATORS  ******************************///

String::operator const char*() const {
   return string;
}//to char* operator overload


String& String::operator+= (const String &S) {
   this->Append(S);
   return *this;
}//+= operator

String& String::operator+= (const char s) {
   this->Append(s);
   return *this;
}//+= operator


String& String::operator+= (const char* S) {
   this->Append(S);
   return *this;
}//+= operator


String& String::operator+= (const int i) {
   this->Append(i);
   return *this;
}

String& String::operator+= (const long l) {
   this->Append((int)l); //bah, same thing in a real computer
   return *this;
}

String& String::operator= (const int i) {
   Clear();
   this->Append(i);
   return *this;
}

String& String::operator= (const long l) {
   Clear();
   Append((int)l); //same thing in a real computer, for now at least
   return *this;
}

String& String::operator= (const char c) {
   Clear();
   Append(c);
   return *this;
}


String& String::operator= (const String &S) {
   ensureCapacity(S.cur_len);
   strcpy(string, S.string);
   cur_len = S.cur_len;
   return *this;
}//assignment operator


String& String::operator= (const char* S) {
   if (S == NULL) {
      Clear();
      return *this;
   }//if
   int k = strlen(S);
   ensureCapacity(k);
   strcpy(string, S);
   cur_len = k;
   return *this;
}//assignment operator

char String::charAt(int i) const {
   Assert(((i >= 0) && (i <= max_len)), "ERROR:  Index out of range.\n");
   return string[i];
}

void String::setCharAt(int idx, const char val) {
   Assert(((idx >= 0) && (idx < cur_len)), "ERROR: Index out of range.\n");   
   Assert(val, "ERROR:  setCharAt:  trying to set a NULL value.\n");
   string[idx] = val;
}
      
char String::operator[] (const unsigned int i) const {
   Assert(((int)(i) < max_len), "ERROR:  index to high, String::operator[]\n");

   return string[i];

}//[] operator


String String::operator+ (const char* S) const {
   String tmp(*this);
   tmp.Append(S);
   return tmp;
}//assignment operator


String String::operator+ (const String& S) const {
   String tmp(*this);
   tmp.Append(S);
   return tmp;
}//assignment operator


/* equality operators, case insensitive */
int String::operator!= (const char* S) const {
   return strcasecmp(string, S);
}//assignment operator


int String::operator== (const char* S) const {
   return (!(strcasecmp(string, S)));
}//assignment operator

int String::operator< (const char* S) const {
   return (strcasecmp(string, S) < 0);
}//assignment operator

int String::operator> (const char* S) const {
   return (strcasecmp(string, S) > 0);
}//assignment operator

int String::operator> (const String& S) const {
   return (strcasecmp(string, S.string) > 0);
}//assignment operator

int String::operator< (const String& S) const {
   return (strcasecmp(string, S.string) < 0);
}//assignment operator

int String::operator<= (const char* S) const {
   return (strcasecmp(string, S) <= 0);
}//assignment operator

int String::operator>= (const char* S) const {
   return (strcasecmp(string, S) >= 0);
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

   targ = "\0"; //initialize targ

   for (i = 0; string[i]; ) {
      if (string[i] == '%') {
         i++;
         if (string[i] != '%') {
            switch (string[i]) {
               case 'i':    /* int */  
                  targ.Append(va_arg(ap, int));
                  break;
               case 'I':    /* ignore */
                  va_arg(ap, void*);
                  // Do nothing with it!!
                  break;
               case 'd':     /* long, turn it into an int */
                  targ.Append(va_arg(ap, int));
                  break;
               case 's':      /* char * */
                  if ((tmp_chr_str = va_arg(ap, char *))) {
                     targ.Append(tmp_chr_str);
                  }//if
                  break;
               case 'p':    /* pointer */
                  targ.appendHex(va_arg(ap, long));
                  break;
               case 'S':      /* String */
                  if ((tmp_str = va_arg(ap, String *))) {
                     targ.Append(*tmp_str);
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
                        LOGFILE.log(ERR, 
                                    "ERROR:  Sprintf, case 'P', first not an int\n");
                     }
                  }//if two digits
                  else {
                     LOGFILE.log(ERR,
                                 "ERROR:  Sprintf, case 'P', second not an int\n");
                  }
                  i += 2; //now pointing to next valid letter
                  
                  s_len = targ.Strlen();
                  while (s_len < num) {
                     targ.Append(" ");  //pad another space
                     s_len++;
                  }//while
                  i--;  //slight hack
                  break;
               default:
                  targ.Assert(FALSE, "ERROR: default called in Sprintf.\n");
            }//switch
            i++;
         }//if
         else { //
            i++;
            targ.Append("%");
         }//else
      }//if
      else {
         targ.Append(string[i]);
         i++;
      }//else
   }//for
}//vSprintf

		/* WARNING:  this will fail if input is > 99 chars. */

istream& operator>> (istream& stream, String& str) {
   char buf[100];
   stream >> buf;
   str = buf;
   return stream;
}//write_out operator


ostream& operator<< (ostream& stream, const String& str) {
   stream << str.string;
   return stream;
}//write_out operator
