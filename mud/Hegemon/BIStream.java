// $Id: BIStream.java,v 1.4 2001/03/31 07:05:58 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date$

//
//Hegemon Client Code:  Java Client for ScryMUD Server Code
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
// To contact the Author, Ben Greear:  greearb@candelatech.com, (preferred)
//                                     bgreear@mayannetworks.com
//

import java.io.*;

/**  Implements a few more utility functions to read different types
  and amounts of data. */
class BIStream extends BufferedInputStream {
   int last_char;
   
   public BIStream(InputStream is) {
      super(is);
      last_char = -1;
   }//constructor
   
   /**  Grabs all till newline ('\\n') or EOF.  If terminated by a newline,
     it junks it (NOT appended to the returned value).  If it reads no
     keepable characters, then it returns NULL.*/
   public final String readLine() throws IOException {
      return readUntil(new NewlineFinder());
   }//readLine()

   public void gobbleWhitespace() throws IOException {
      int rslt;
      
      while (true) {
         rslt = super.read();
         if (rslt == -1)
            break;
         else if (!Character.isWhitespace((char)(rslt))) {
            last_char = rslt;
            return;
         }
      }//while

      /* if here, then we reached the EOF */
      return;
   }//gobbleWhitespace
   
   /** returns first string of non-whitespace chars. */
   public final String readToken() throws IOException {
      gobbleWhitespace();
      return readUntil(new WhiteSpaceFinder());
   }//readToken

   public final int readInt() throws IOException {
      try {
         return Integer.parseInt(readToken());
      }
      catch (NumberFormatException e) {
         Log.instance().wrn("ERROR:  in BIStream readInt:  " + e);
         throw (new IOException("ERROR:  BIStream.readInt:  number format"
                                + " was wrong."));
      }//catch
   }//readInt
   
   public final String readUntil(CharFinder finder) throws IOException {
      StringBuffer sb = new StringBuffer(200);
      char ch;
      boolean got_one = false;
      int rslt;

      if (last_char != -1) {
         rslt = last_char;
         last_char = -1;
      }
      else {
         rslt = super.read();
      }
      
      while (rslt != -1) {
         ch = (char)(rslt);
         if (finder.targetIs(ch)) {
            break;
         }
         else {
            got_one = true;
            sb.append(ch);
         }
         rslt = super.read();
      }//while
      
      if (!got_one) {
         return null;
      }
      else
         return sb.toString();
   }//readUntil()
   
}//BIStream

class WhiteSpaceFinder extends Object implements CharFinder {
   public WhiteSpaceFinder() {
      super();
   }

   public boolean targetIs(char ch) {
      return (Character.isWhitespace(ch));
   }
}

class NewlineFinder extends Object implements CharFinder {
   public NewlineFinder() {
      super();
   }

   public boolean targetIs(char ch) {
      return (ch == '\n');
   }
}
