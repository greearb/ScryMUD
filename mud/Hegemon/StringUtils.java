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



public class StringUtils extends Object
{

   public static String getString(String src, int begin_idx, String delimiters) 
      {
//         Log.it("In getString, src:  " + src + "idx:  " + begin_idx);
         
         StringBuffer retval = new StringBuffer(100);
         String my_src = new String(src.substring(begin_idx));

         my_src = my_src.trim();
         int len = my_src.length();
         char[] str = my_src.toCharArray();
         char ch;
         
         for (int i = 0; i<len; i++) {
            ch = str[i];
            if (delimiters.indexOf(ch) != -1) {
               return retval.toString();
            }
            else {
               retval.append(ch);
            }
         }
         return retval.toString();
      }
}

         
