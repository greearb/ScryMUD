// $Id: string2.h,v 1.18 2003/02/25 04:14:44 greear Exp $
// $Revision: 1.18 $  $Author: greear $ $Date: 2003/02/25 04:14:44 $

//
//Copyright (C) 1998-2001  Ben Greear
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

// string2.h -- Declarations for the String library
// Coded by Ben Greear

// Everyone is free to use this code, just leave my name intact.
// This has not been tested very much, so use at your own risk!! :)

// Any questions, write me at:  greear@pollux.cs.uga.edu

 
#ifndef String2Include
#define String2Include


#define TRUE 	1
#define FALSE 	0

#include <stdio.h> 
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <LogStream.h>
#include <stdarg.h>

#define LOGFILE (*(String::getLogFile()))


// WARNING:  Don't add any methods to the Counter classes, it's
//  packed as a tight structure in memory and placed on the wire.

/* TODO:  Someday put this in a better place... */
struct Counter64u {
   unsigned long hi;
   unsigned long lo;
};

struct Counter64u toCounter64u(unsigned long long cnt);
unsigned long long toUint64(const Counter64u& cnt);


// WARNING:  NOTE:  TODO:  Not sure Counter64s works as planned...needs
// testing before being used.
/* TODO:  Someday put this in a better place... */
struct Counter64s {
   long hi;
   long lo;
};

struct Counter64s toCounter64s(long long cnt);
long long toInt64(const Counter64s& cnt);

int makeHBO(struct Counter64u& pld);
int makeNBO(struct Counter64u& pld);
int makeHBO(struct Counter64s& pld);
int makeNBO(struct Counter64s& pld);


class String {
protected:

   char *string;
   int max_len;
   int cur_len;
   int Assert(int boolean_val, const char* msg) const;

public:
   static int string_cnt;
   static int total_bytes;

   static LogStream* logfile;

   String  (); //default constructor
   String  (const char* S);
   String  (const String& S);
   String  (const int m_len);
   ~String ();
   
   static void setLogFile(LogStream* dafile);// { logfile = dafile; }
   static LogStream* getLogFile();// { return logfile; }

   ///*********************  OPERATORS  ******************************///
   
   String& operator+= (const String &S);
   String& operator+= (const char* S);
   String& operator+= (const char s);
   String& operator+= (const int i);
   String& operator+= (const unsigned int i);
   String& operator+= (const long l);
   String& operator+= (const unsigned long l);
   String& operator+= (const unsigned long long l);
   String& operator+= (const long long l);
   String& operator+= (const float l);
   String& operator+= (const double l);
   String& operator= (const String &S);
   String& operator= (const char* S);
   String& operator= (const int i);
   String& operator= (const unsigned int i);
   String& operator= (const long l);
   String& operator= (const unsigned long l);
   String& operator= (const long long l);
   String& operator= (const unsigned long long l);
   String& operator= (const char c);

   // NOTE:  can't assign with this operator, see setCharAt()
   char operator[] (const unsigned int i) const;
   char operator[] (const int i) const;

   String operator+ (const char* S) const;
   String operator+ (const String& S) const;
   String operator+ (const char S) const;

   int operator== (const char* S) const;
   int operator!= (const char* S) const;
   int operator> (const char* S) const;
   int operator< (const char* S) const;
   int operator> (const String& S) const;
   int operator< (const String& S) const;
   int operator>= (const char* S) const;
   int operator<= (const char* S) const;
   operator const char* () const;


///********************  FRIEND OVERLOADS  **************************///

   //friend ostream& operator<< (ostream& stream, const String& str);
   friend istream& operator>> (istream& stream, String& str);
   friend void Sprintf(String& targ, const char* string, ...);
   friend void vSprintf(String& targ, const char* string, va_list var_args);
   friend int Strncompare(const String& a, const String& b);

   friend String operator+(const char* lhs, const String& rhs);
 
///**********************  MEMBER FUNCTIONS  ************************///

   char charAt(int idx) const ;
   void setCharAt(int idx, const char val);

   /** Doesn't release memory, but virtually clears out the data. */
   void Clear() { clear(); }
   void clear(); //Should migrate to lower-case cmds some day.

   /** Releases all the memory it can. */
   void purge();

   void prepend(const int source);
   void Prepend(const int source) { prepend(source); }

   void prepend(const long source);
   void Prepend(const long source) { prepend(source); }

   void prepend(const char* source);
   void Prepend(const char* source) { prepend(source); }

   void prepend(const String& source);
   void Prepend(const String& source) { prepend(source); }

   void append(const unsigned long source);
   void append(const double source, int remainder = 2);
   void append(const float source, int remainder = 2);

   void append(const int source);
   void Append(const int source) { append(source); }
   void append(const unsigned int source);

   /** Makes 9 look like: 1001 */
   void appendAsBinary(const unsigned long num);

   void append(const long source);
   void Append(const long source) { append(source); }

   void append(const unsigned long long source);
   void append(const long long source);

   void append(const char* source);
   void Append(const char* source) { append(source); }

   void append(const String& source);
   void Append(const String& source) { append(source); }

   void append(const unsigned char source);
   void append(const char source);
   void Append(const char source) { append(source); }

   void appendHex(const unsigned long l);
   void Getline(istream& stream, int max_len) { getLine(stream, max_len); }
   void getLine(istream& stream, int max_len);

   void cap();
   void Cap() { cap(); }

   void Tolower() { toLower(); }
   void toLower(); //makes total string lowercase

   void toUpper(); //makes total string lowercase
   unsigned int  Strlen() const;
   void Report() const; //logs vital stats, for debugging.
   void ensureCapacity(int max_length);
   unsigned int hash(); // return a value that should be pretty unique.

   int  Write(const int desc, const int max_to_write);
   //writes to a descriptor, deletes what it writes

   int  Read(const int desc, const int max_to_read); //reads from a desc

   /** Reads untill it finds the delim, and then reads untill
    * it finds another.  Escape the delim with a \ (backslash).
    * This is not too efficient, as it reads one character at
    * a time, btw.
    */
   int readToken(char delim, istream& dafile, int include_delim);

   void termedRead(istream& da_file);
   void Termed_Read(istream& da_file) { return termedRead(da_file); }

   int contains(const char ch) const;
   int Contains(const char ch) const { return contains(ch); }

   void  Refigure_strlen();			//bletcherous hack!
   void parse_show(short posn);  //specific to ScryMUD program
   void InsertAfter(char a, int posn);
   void InsertBefore(char a, int posn);
   void insertAfter(int i, const char* str);

   void strip(); //take spaces off of front and back
   void Strip() { strip(); }

   void dropFromEnd(int num_chards); //drop a few off the end of the string
   String sterilizeForHtml() const;

   /* Both of these next two delete what they grab from the string.
    * grabs first word or number deliminated
    * by space, a '.' or null char
    */
   String Get_Command(short& eos, short& term_by_period, short is_first = 0) {
      return getCommand(eos, term_by_period, is_first);
   }
   String getCommand(short& eos, short& term_by_period, short is_first = 0,
                     short ignore_period = FALSE);

   /** Grabs first token deliminated by a space, period, or null character.*/
   String getToken() {
      short eos, tbp;
      return getCommand(eos, tbp);
   }

   /** Grabs first token deliminated by a space, (optional)period,
    * or null character.*/
   String getToken(short ignore_period) {
      short eos, tbp;
      return getCommand(eos, tbp, 0, ignore_period);
   }

   /** grabs phrase, deliminated by a newline or null char
    * If destruct is TRUE, then the returned String will be removed
    * from the beginning of this string.
    */
   String getRest(short destruct = TRUE);
   String Get_Rest(short destruct = TRUE) { return getRest(destruct); }
   
   /** grabs first word or number deliminated by space, a '.'
    * or null char, does not delete it from the string.
    */
   String lookCommand(short is_first = 0) const;
   String Look_Command(short is_first = 0) const {
      return lookCommand(is_first);
   }

   //grabs everything untill that character is found.  It removes all that
   // from the string, so successive calls iterate through it...
   // Returned pointer is to a String allocated off of the Heap..ie delete it
   // eventually if that makes sense.
   String* getUntil(char dlm);

   void readUntil(char dlm, istream& dafile);
   
};
#endif
