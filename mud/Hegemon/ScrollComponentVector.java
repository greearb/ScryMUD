// $Id$
// $Revision: 1.3 $  $Author$ $Date$

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


import java.util.*;

class ScrollComponentVector implements ScrollComponentCollection {

   Vector vect;
   int total_height;
   int orig_len; //original len
   
   public ScrollComponentVector(int length) {
      super();

      orig_len = length;
      vect = new Vector(length);
      total_height = 0;
   }//constructor

   public void clear() {
      vect = new Vector(orig_len);
      total_height = 0;
   }
   
   public String toString() {
      return ("ScrollComponentVector:  height:  " + total_height);
   }

   public int getTotalHeight() {
      return total_height;
   }
   
   //TODO:  make this more efficient
   /** This gives us the index of the first element we want to add to our
     Vector when getting a 'page'.  It could be speeded up with some sort
     of caching scheme I think.. */
   private final int findFirst(int y_top) {
      int sofar = 0;
      ScrollComponent sc;

      int len = vect.size();
      for (int i = 0; i<len; i++) {

         sc = (ScrollComponent)(vect.elementAt(i));

         sofar += sc.getHeight();

         if (sofar >= y_top)
           return i;
      }//while

      if ((len - 1) < 0)
         return 0;
      else
        return (len - 1);
   }//findFirst

   /**  This is the common case for screen draws. */
   public final synchronized Vector getBottomPage(int total_height) {
      //Log.it("In ScrollComponentVector.getBottomPage(" + total_height
      //       + "), vect.size():  " + vect.size());
      
      Vector retval = new Vector(100);
      
      int height_sofar = 0;
      ScrollComponent sc;

      int len = vect.size();
      int i;
      
      for (i = len - 1; i >= 0; i--) {
         sc = (ScrollComponent)(vect.elementAt(i));
         //Log.it("\n\nScrollComponent:  " + sc + "\n\n");
         height_sofar += sc.getHeight();

         if (height_sofar >= total_height) {
            //Log.it("Found a full page w/out using all..");
            break;
         }
      }//for

      if (i < 0)
         i = 0;
      
      for (; i < len; i++) {
         retval.addElement(vect.elementAt(i));
      }

      return retval;
   }//getBottomPage

   
   public final synchronized Vector getVector(int y_top, int y_bottom) {
      Vector retval = new Vector(100);

      int my_idx = this.findFirst(y_top);

      int total_height = y_bottom - y_top;
      int height_sofar = 0;
      ScrollComponent sc;

      int len = vect.size();
      
      for (int i = my_idx; i < len; i++) {
         sc = (ScrollComponent)(vect.elementAt(i));

         height_sofar += sc.getHeight();
         retval.addElement(sc);

         if (height_sofar >= total_height) {
            return retval;
         }
      }//while

      return retval;
   }//getArray

   
   public final synchronized void push(ScrollComponent obj) {
      total_height += obj.getHeight();
      vect.addElement(obj);
   }//push


   /**  Return the number of elements found in the queue.  Could
    allow for a cheaper method of cleaning out the array (ie don't have
    to test for isEmpty all the time..) */

   public final int length() {
      return vect.size();
   }//length

}//ScrollComponentVector
