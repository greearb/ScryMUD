// $Id: SocketWriter.java,v 1.4 2001/03/31 07:06:00 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date: 2001/03/31 07:06:00 $

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
import java.net.*;
import java.util.*;

class SocketWriter extends Object {
   HegemonManager hm;
   SocketManager parent;
   DataOutputStream dos = null;

   public SocketWriter (SocketManager c, HegemonManager h) {
      parent = c;
      hm = h;
      try {
         dos = new DataOutputStream(parent.sock.getOutputStream());
      }
      catch (IOException e) {
         System.err.println ("SocketWriter: " + e);
      }
   }

   private String substituteAliases(Alias[] aliases, String str, int sanity) {
      if ((sanity > 10) || (aliases == null) ||
          (!hm.getAliasManager().getAliasEnabled())) {
         //         Log.it("Doing noting in substituteAliases");
         return str;
      }
      
      int len = aliases.length;
      String sub;
      StringBuffer sb = new StringBuffer(200);
      int strlen = str.length();
      int idx;
//      Log.it("aliases.length:  " + len);
//      Log.it("str:  -:" + str + ":-");
      
      for (int i = 0; i<len; i++) {
         sub = aliases[i].getAlias();
//         Log.it("sub:  -:" + sub + ":-");
         
         if ((idx = str.indexOf(sub)) != -1) {//got one!
            /* copy first part into sb */
            sb.append(str.substring(0, idx));
            /* append the substitution */
            sb.append(aliases[i].getResponse());
            /* append the last part */
            sb.append(str.substring(idx + sub.length(), strlen));
            /* now RECURSE */
            return substituteAliases(aliases, sb.toString(), ++sanity);
         }//if found one
      }//for

      /* if here, then we didn't find one. */
      return str;
   }//substituteAliases
         
   public void send (String str) {
      try {
         /* TODO:  Add alias parsing here */
         Log.instance().io("====> Before alias substitution:  -:" + str + ":-");
         str = substituteAliases(hm.getAliasManager().getAliases(), str, 0);
         Log.instance().io("====> After alias substitution:  -:" + str + ":-");
         
         dos.writeBytes(str);
      }
      catch (IOException e) {
         Log.instance().err("Writer: " + e);
         parent.signalWriterDeath();
      }//catch
   }//send

   public void flush() {
      try {
         dos.flush();
      }
      catch (Exception e) {
         Log.instance().err("Flush:  " + e);
      }
   }

   public void destroy() {
      try {
         if (dos != null)
           dos.close ();
      }
      catch (IOException e) {
         Log.instance().err("Error in finalize:  " + e);
      }
   }
   
   protected void finalize () {
      destroy();
   }
} //end Writer 
      
