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

class Queue extends Object {
   int head_idx;
   int tail_idx;
   int len;
   Object[] vect;

   public Queue(int length) {
      super();
      len = length + 1; /* we waste a space for simplicity */
      head_idx = 0;
      tail_idx = len;
      vect = new Object[len]; /* 0 --> double capacity if needed */
   }//constructor

   /** Return an array of Objects contained in the queue.  The
     oldest elements are at the lower indexes of the returned array.. */
   public final synchronized Object[] getArray() {
      int my_len = this.length();
      int my_idx = tail_idx;
      Object[] retval = new Object[my_len];

      Log.it("Queue: In getArray(), my_len:  " + my_len);
      for (int i = 0; i < my_len; i++) {
         retval[i] = vect[my_idx = getNextIdx(my_idx)];
      }//for
      return retval;
   }//getArray
   
   /**  Add an object to the head of the queue, will throw an
     Exception if its already full. */
   public final synchronized void push(Object obj) throws Exception {
      //Log.it("Queue:  in push");
      if (!this.isFull()) {
         vect[head_idx] = obj;
         head_idx = getNextIdx(head_idx);
      }
      else {
         throw new Exception("ERROR:  Queue is full.");
      }
   }//push

   /**  Remove and return the oldest object from the queue.  Throws
    Exception if it's empty. */
   public final synchronized Object pull() throws Exception {
      if (!isEmpty()) {
         tail_idx = getNextIdx(tail_idx);
         return vect[tail_idx];
      }
      else {
         throw new Exception("ERROR:  Queue is empty.");
      }
   }//pull

   public final void keepNewest(int cnt) {
      int to_junk_cnt = (length() - cnt);
      for (int i = 0; i<to_junk_cnt; i++) {
         try {
            pull();
         }
         catch (Exception e) {
            Log.it("Queue::keepNewest, cnt:  " + cnt + "  " + e);
         }//catch
      }//for
   }//keepNewest

   /** returns the oldest object, but DOES NOT REMOVE IT
     from the queue. Throws exception if its empty. */
   public final synchronized Object peek() throws Exception {
      if (!isEmpty()) {
         return vect[getNextIdx(tail_idx)];
      }
      else {
         throw new Exception("ERROR:  Queue is empty.");
      }
   }//pull

   private final int getNextIdx(int idx) {
      if (idx == len) {
         return 0;
      }
      else {
         return idx + 1;
      }
   }//getNextIdx

   private final int getPrevIdx(int idx) {
      if (idx == 0)
         return len;
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
   }//isFull

   /**  Return the number of elements found in the queue.  Could
    allow for a cheaper method of cleaning out the array (ie don't have
    to test for isEmpty all the time..) */

   public final int length() {
      if (tail_idx >= head_idx) {
         return head_idx + (len - tail_idx);
      }//if
      else {
         return head_idx - tail_idx;
      }
   }//length

   public final int capacity() {
      return len - 1; //we waste a space, but is much more convenient!
   }
}//Queue
