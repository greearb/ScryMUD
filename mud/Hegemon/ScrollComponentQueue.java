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
// To contact the Author, Ben Greear:  greear@cyberhighway.net, (preferred)
//                                     greearb@agcs.com
//


import java.util.*;

/**  Head points to the next slot to add an element to.
  Tail points to the slot just before oldest element.
  If (previous index of head) == tail, then the queue is empty.
  NOTE:  this queue wastes one space, but this gives us a lot
  more simplicity in keeping track of is-full and is_empty status.
  */

class ScrollComponentQueue implements ScrollComponentCollection {
   int head_idx;
   int tail_idx;
   int len;
   ScrollComponent[] vect;
   int total_height;
   int item_count;
   
   public ScrollComponentQueue(int length) {
      super();
      len = length + 1; /* we waste a space for simplicity */
      head_idx = 0;
      tail_idx = len - 1;
      vect = new ScrollComponent[len];
      total_height = 0;
      item_count = 0;
   }//constructor

   public void clear() {
      head_idx = 0;
      tail_idx = len - 1;
      //vect = new ScrollComponent[len];
      item_count = total_height = 0;
   }//clear

   public String toString() {
      return ("ScrollComponentQueue:  height:  " + total_height);
   }

   public int getTotalHeight() {
      return total_height;
   }
   
   //TODO:  make this more efficient
   /** This gives us the index of the first element we want to add to our
     Vector when getting a 'page'.  It could be speeded up with some sort
     of caching scheme I think.. */
   private final int findFirst(int y_top) {
      int my_idx = getNextIdx(tail_idx);
      int sofar = 0;
      ScrollComponent sc;
            
      while (true) {
         //if we're all the way to the end
         if (my_idx == head_idx)
            return my_idx;

         sc = (vect[my_idx]);

         if (sc != null) {
            sofar += sc.getHeight();

         }
         //if we've encountered it alreaady
         if (sofar >= y_top)
           return my_idx;
         
         my_idx = getNextIdx(my_idx);
      }//while
   }//findFirst

   
   /**  This is the common case for screen draws. */
   public final synchronized Vector getBottomPage(int total_height) {
      Vector tmp_retval = new Vector(100);

      if (isEmpty())
         return tmp_retval;
      
      int my_idx = getPrevIdx(head_idx);
      int default_last = getNextIdx(tail_idx);
      int height_sofar = 0;
      ScrollComponent sc;

//      Log.it("getBottomPage: my_idx:  " + my_idx + " default_last: "
//             + default_last + " total_height:  " + total_height);
      while (true) {
         if (my_idx == default_last) {
            //Log.it("Found default_last, NULL count: " + cnt);
            break;
         }

         sc = vect[my_idx];
         if (sc != null) {
            height_sofar += sc.getHeight();
            tmp_retval.addElement(sc);
         }
         else {
            Log.it("ERROR:  sc is NULL, idx:  " + my_idx);
         }

         if (height_sofar >= total_height) {
            //Log.it("Height satisfied, NULL count: " + cnt);
            break;
         }
         my_idx = getPrevIdx(my_idx);
      }//while

      /* when here, we have the return value, but it is reversed. */
      /* we need to reverse the reversed one to return a correct Vector. */
      int sz = tmp_retval.size();
      Vector retval = new Vector(sz);
      for (int i = sz - 1; i >= 0; i--) {
         retval.addElement(tmp_retval.elementAt(i));
      }

      return retval;
   }//getBottomPage

   
   /** Return an array of Objects contained in the queue.  The
     oldest elements are at the lower indexes of the returned
     array.. */
   public final synchronized Vector getVector(int y_top, int y_bottom) {
      //Log.it("ScrollComponentQueue.getVector:  y_top:  " + y_top
      //       + "  y_bottom: " + y_bottom + " tail: " + tail_idx
      //       + " head:  " + head_idx);
      Vector retval = new Vector(100);

      if (isEmpty())
         return retval;
      
      int my_idx = this.findFirst(y_top);
      int default_last = getPrevIdx(head_idx);
      int total_height = y_bottom - y_top;
      int height_sofar = 0;
      ScrollComponent sc;

      //Log.it("ScrollComponentQueue.getVector: my_idx:  " + my_idx 
      //       + " default_last: " + default_last
      //       + " total_height:  " + total_height);
      while (true) {
         if (my_idx == default_last) {
            //Log.it("Fount default_last, NULL count: " + cnt);
            return retval;
         }

         sc = vect[my_idx];
         if (sc != null) {
            height_sofar += sc.getHeight();
            retval.addElement(sc);
         }
         else {
            Log.it("ERROR: getVector(),  sc is NULL, idx:  " + my_idx);
         }

         if (height_sofar >= total_height) {
            //Log.it("Height satisfied, NULL count: " + cnt);
            return retval;
         }
         my_idx = getNextIdx(my_idx);
      }//while
   }//getArray

   
   /**  Add an object to the head of the queue, will throw an
     Exception if its already full. */
   public final synchronized void push(ScrollComponent obj) {
      //Log.it("ScrollComponentQueue: in push");//, obj: " + obj);
      if (this.isFull()) {
         try {
            this.pull();
         }
         catch (Exception e) {
            Log.it(toString() + e);
         }
      }

      vect[head_idx] = obj;
      head_idx = getNextIdx(head_idx);
      total_height += obj.getHeight();
      item_count++;

   }//push

   
   /**  Remove and return the oldest object from the queue.  Throws
    Exception if it's empty. */
   public final synchronized Object pull() throws Exception {
      if (!isEmpty()) {
         tail_idx = getNextIdx(tail_idx);
         ScrollComponent sc = vect[tail_idx];
         total_height -= sc.getHeight();
         vect[tail_idx] = null;
         item_count--;
         return sc;
      }
      else {
         throw new Exception("ERROR:  ScrollComponentQueue.pull:  Queue"
                             + " is empty.");
      }
   }//pull

   
   /** returns the oldest object, but DOES NOT REMOVE IT
     from the queue. Throws exception if its empty. */
   public final synchronized Object peek() throws Exception {
      if (!isEmpty()) {
         return vect[getNextIdx(tail_idx)];
      }
      else {
         throw new Exception("ERROR: on peek(), Queue is empty.");
      }
   }//pull


   private final int getNextIdx(int idx) {
      if (idx == (len - 1)) {
         return 0;
      }
      else {
         return idx + 1;
      }
   }//getNextIdx

   
   private final int getPrevIdx(int idx) {
      if (idx == 0)
         return len - 1;
      else
         return idx - 1;
   }//getPrevIdx

   
   /**  Test the empty-ness of the queue. */
   public final boolean isEmpty() {
      if (getPrevIdx(head_idx) == tail_idx)
        return true;
      else
         return false;
   }//isEmpty

   
   /**  Test the fullness of the queue. */
   public final boolean isFull() {
      if (tail_idx == head_idx)
        return true;
      else
        return false;
   }//is Full

   
   /**  Return the number of elements found in the queue.  Could
    allow for a cheaper method of cleaning out the array (ie don't have
    to test for isEmpty all the time..) */

   public final int length() {
      return item_count;
   }//length

   
   public final int capacity() {
      return len - 1; 
   }
}//ScrollComponentQueue
