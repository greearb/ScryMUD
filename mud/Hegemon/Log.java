// $Id$
// $Revision: 1.5 $  $Author$ $Date$

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

/** Logging class. Not sure what the capabilities are. */
class Log extends Object {

   public Log() {
      flags = ALL;
   }

   public static Log instance() {
      if (self == null) {
         self = new Log();
      }//if
      return self;
   }

   protected void output(String msg) {
      System.out.println(msg);
      System.out.flush();
   }

   public void err(String msg) {
      if ((flags & ERROR) > 0) {
         output(msg);
      }//if
   }//err

   public void wrn(String msg) {
      if ((flags & WARNING) > 0) {
         output(msg);
      }//if
   }//wrn

   public void inf(String msg) {
      if ((flags & INFO) > 0) {
         output(msg);
      }//if
   }//inf

   public void init(String msg) {
      if ((flags & INIT) > 0) {
         output(msg);
      }//if
   }//init

   public void trc(String msg) {
      if ((flags & TRACE) > 0) {
         output(msg);
      }//if
   }//trc

   public void dbg(String msg) {
      if ((flags & DEBUG) > 0) {
         output(msg);
      }//if
   }//dbg

   public void io(String msg) {
      if ((flags & IO) > 0) {
         output(msg);
      }//if
   }//io

   public void setLevel(int lvl) {
      output("Changing Logging level to: " + lvl + "\n");
      flags = lvl;
   }

   public int getLevel() {
      return flags;
   }

   public void setFlag(int flag) {
      flags |= flag;
   }

   public void clearFlag(int flag) {
      flags &= ~flag;
   }

   public void flush() {
      System.out.flush();
   }

   private static Log self;
   protected int flags;
   
   public static final int ERROR = 1;
   public static final int WARNING = 2;
   public static final int INFO = 4;
   public static final int INIT = 8;
   public static final int TRACE = 16;
   public static final int DEBUG = 32;
   public static final int IO = 64;
   public static final int ALL = ~0;
}
