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

#define LOGFILE (*(String::getLogFile()))


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
   String& operator+= (const long l);
   String& operator= (const String &S);
   String& operator= (const char* S);
   String& operator= (const int i);
   String& operator= (const long l);
   String& operator= (const char c);

   // NOTE:  can't assign with this operator, see setCharAt()
   char operator[] (const unsigned int i) const;

   String operator+ (const char* S) const;
   String operator+ (const String& S) const;
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

   friend ostream& operator<< (ostream& stream, const String& str);
   friend istream& operator>> (istream& stream, String& str);
   friend void Sprintf(String& targ, const char* string, ...);
   friend int Strncompare(const String& a, const String& b);
   
///**********************  MEMBER FUNCTIONS  ************************///

   char charAt(int idx) const ;
   void setCharAt(int idx, const char val);
   void Clear();
   void Prepend(const int source);
   void Prepend(const long source);
   void Prepend(const char* source);
   void Prepend(const String& source);
   void Append(const int source);
   void Append(const long source);
   void Append(const char* source);
   void Append(const String& source);
   void Append(const char source);
   void appendHex(const long l);
   void Getline(istream& stream, int max_len);
   void Cap();
   void Tolower(); //makes total string lowercase
   void Vclear(); //virtually clears it, (copies '\0' into first posn)
   int  Strlen() const;
   void Report() const; //logs vital stats, for debugging.
   void ensureCapacity(int max_length);

   int  Write(const int desc, const int max_to_write);
   //writes to a descriptor, deletes what it writes

   int  Read(const int desc, const int max_to_read); //reads from a desc
   void Termed_Read(ifstream& da_file);
   short Contains(const char ch) const;
   void  Refigure_strlen();			//bletcherous hack!
   void parse_show(short posn);  //specific to ScryMUD program
   void InsertAfter(char a, int posn);
   void InsertBefore(char a, int posn);
   void insertAfter(int i, const char* str);

   void Strip(); //take spaces off of front and back
   
   /* Both of these next two delete what they grab from the string. */
   
   String Get_Command(short& eos, short& term_by_period);	
				//grabs first word or number deliminated
				//by space, a '.' or null char
   
   String Get_Rest(short destruct = TRUE);	//grabs phrase, deliminated 
   //by a newline or null char
   
   String Look_Command() const; //grabs first word or number deliminated
				//by space, a '.' or null char, does not 
				//delete it from the string.

   //grabs everything untill that character is found.  It removes all that
   // from the string, so successive calls iterate through it...
   // Returned pointer is to a String allocated off of the Heap..ie delete it
   // eventually if that makes sense.
   String* getUntil(char dlm);
   
};
#endif