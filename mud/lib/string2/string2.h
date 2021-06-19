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
#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <LogStream.h>
#include <stdarg.h>
#include <stdint.h>

using namespace std;

#define LOGFILE (*(String::getLogFile()))


// WARNING:  Don't add any methods to the Counter classes, it's
//  packed as a tight structure in memory and placed on the wire.

#if defined(__WIN32__) or defined(__sun__)
typedef uint32_t __uint32_t;
typedef int32_t __int32_t;
typedef uint64_t __uint64_t;
typedef int64_t __int64_t;
#endif

/* TODO:  Someday put this in a better place... */
struct Counter64u {
   __uint32_t hi;
   __uint32_t lo;
};

struct Counter64u toCounter64u(__uint64_t cnt);
__uint64_t toUint64(const Counter64u& cnt);


// WARNING:  NOTE:  TODO:  Not sure Counter64s works as planned...needs
// testing before being used.
/* TODO:  Someday put this in a better place... */
struct Counter64s {
   __int32_t hi;
   __int32_t lo;
};

struct Counter64s toCounter64s(__int64_t cnt);
__int64_t toInt64(const Counter64s& cnt);

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
   static long string_cnt;
   static long total_bytes;

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
   String& operator= (const float l);

   // NOTE:  can't assign with this operator, see setCharAt()
   char operator[] (const unsigned int i) const;
   char operator[] (const int i) const;

   String operator+ (const char* S) const;
   String operator+ (const String& S) const;
   String operator+ (const char S) const;
   String operator+ (const int i) const;
   String operator+ (const long i) const;
   String operator+ (const long long i) const;
   String operator+ (const unsigned int i) const;
   String operator+ (const unsigned long i) const;
   String operator+ (const unsigned long long i) const;

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
   friend void Sprintf(String& targ, const char* string, ...) __attribute__((format(printf, 2, 3)));
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

   void prepend(char source);
   //void prepend(char source) { prepend((const char)(source)); }

   void prepend(int source);
   void Prepend(int source) { prepend(source); }

   void prepend(long source);
   void Prepend(long source) { prepend(source); }

   void prepend(const char* source);
   void Prepend(const char* source) { prepend(source); }

   void prepend(const String& source);
   void Prepend(const String& source) { prepend(source); }

   void append(unsigned long source);
   void append(double source, int remainder = 2);
   void append(float source, int remainder = 2);

   void append(int source);
   void Append(int source) { append(source); }
   void append(unsigned int source);

   /** Makes 9 look like: 1001 */
   void appendAsBinary(unsigned long num);

   void append(long source);
   void Append(long source) { append(source); }

   void append(unsigned long long source);
   void append(long long source);

   void append(const char* source, int len);
   void append(const char* source);
   void Append(const char* source) { append(source); }

   void append(const String& source);
   void Append(const String& source) { append(source); }

   void append(unsigned char source);
   void append(char source);
   void Append(char source) { append(source); }

   void appendHex(unsigned long l);
   void append(void* v);
   void Getline(istream& stream, int max_length) { getLine(stream, max_length); }
   void getLine(istream& stream, int max_length);

   void cap();
   void Cap() { cap(); }

   void Tolower() { toLower(); }
   void toLower(); //makes total string lowercase

   void toUpper(); //makes total string lowercase
   int  Strlen() const;
   int size() const { return Strlen(); }
   int getCurLen() const { return Strlen(); }
   const char* c_str() const {
      if (string)
         return string;
      return "";
   }
   void Report() const; //logs vital stats, for debugging.
   void ensureCapacity(int max_length);
   unsigned int hash(); // return a value that should be pretty unique.

   bool endsWith(char c) const;

   //sends to a descriptor, deletes what it writes
   int send(const int desc, const int max_to_write) {
      return do_transmit(desc, max_to_write, true, false);
   }
   //writes to a descriptor, deletes what it writes
   int write(const int desc, const int max_to_write, bool use_rwfile) {
      return do_transmit(desc, max_to_write, false, use_rwfile);
   }

   //writes to a descriptor, deletes what it writes
   int Write(const int desc, const int max_to_write) {
      return do_transmit(desc, max_to_write, false, false);
   }

   // If is_send is false, will use 'write' instead.
   int do_transmit(const int desc, const int max_to_write, bool is_send, bool use_rwfile);

   /** This gets a copy of the bytes out of the buffer.  Repeated calls
    * to this method will return the same thing.  Use dropFromTail() to
    * remove the bytes from the buffer, ie to iterate through.
    */
   int peekBytes(unsigned char* buf, int len_to_get) const;

   /**  If ignore_semi_colons is false, then semi-colons will be turned into newlines.  If
    * do_recv is false, we will use the 'read' system call instead.  Use this for file
    * descriptors on Unix.
    */
   int Read(const int desc, const int max_to_read, bool ignore_semi_colons, bool do_recv = true) {
      bool eagain;
      return Read(eagain, desc, max_to_read, ignore_semi_colons, do_recv, false);
   }

   //reads from a desc
   int Read(bool& eagain, const int desc, const int max_to_read, bool ignore_semi_colons, bool do_recv = true, bool use_rwfile = false);

   /** Reads untill it finds the delim, and then reads untill
    * it finds another.  Escape the delim with a \ (backslash).
    * This is not too efficient, as it reads one character at
    * a time, btw.
    */
   int readToken(char delim, istream& dafile, int include_delim);

   void termedRead(istream& da_file);
   int contains(const char ch) const;
   int Contains(const char ch) const { return contains(ch); }

   void  Refigure_strlen();			//bletcherous hack!
   void parse_show(short posn);  //specific to ScryMUD program
   void InsertAfter(char a, int posn);
   void InsertBefore(char a, int posn);
   void insertAfter(int i, const char* str);

   void strip(); //take spaces off of front and back
   void Strip() { strip(); }

   // Remove all instances of 'c' from this string.
   void cleanChar(char c);

   void dropFromEnd(int num_chars); //drop a few off the end of the string
   void dropFromFront(int num_chars); //drop a few off the front, not very efficient
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

   String getCsvToken();

   /** Returns newly allocated String, or NULL if no newlines were found.
    * Removes anything returned, so can call this to iterate through each
    * line of a String (though not that efficiently.)
    */
   String* getLine();

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
