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

class HegStack extends Stack {
   int stack_cnt;

   public HegStack() {
      super();
      stack_cnt = 0;
   }//constructor

   public Object push(Object obj) {
      stack_cnt++;
      return super.push(obj);
   }//push

   public synchronized Object pop() {
      if (stack_cnt > 0)
         stack_cnt--;
      return super.pop();
   }//pop

   public int length() {
      return stack_cnt;
   }//length
}//HegStack