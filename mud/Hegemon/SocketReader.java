// $Id: SocketReader.java,v 1.5 1999/06/05 23:29:12 greear Exp $
// $Revision: 1.5 $  $Author: greear $ $Date: 1999/06/05 23:29:12 $

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

import java.io.*;
import java.net.*;
import java.lang.*;
import java.awt.*;
import java.util.*;


class SocketReader extends Thread {
   byte[] buf = new byte[10000];
   SocketManager parent;
   HegemonManager hm;
   DataInputStream dis = null;
   HegemonInputFilter hif;
   
   public SocketReader (SocketManager par, HegemonManager h) {
      super ("Client Reader");
      parent = par;
      hm = h;
   }//constructor

   public void flush() {
      return;
   }
   
   public void run () {
      int c;
      int len;
      int real_len;
      String str;
      hif = new HegemonInputFilter(hm.getOlcStore(), parent.getWriter(),
                                   hm.getActions(), hm, hm.getScroll(),
                                   hm.getColorSelectionMgr());
      
      try {
         dis = new DataInputStream (parent.getSock().getInputStream ());
      }
      catch (Exception e) {
         Log.instance().err("DataInputStream():  " + e);
         parent.signalReaderDeath();
         return;
      }
      
      while (true) {
         try { //try to get INPUT
            len = dis.available();
            
            if (len == 0) { // if here, could be dead socket, block if not
               if ((c = dis.read()) == -1) {
                  break;
               }//if
               else {
                  buf[0] = (byte)c;
                  len = dis.available();

                  if (len > 0) {
                     if (len > 9998) //don't want to overflow buffer
                       len = 9998;

                     if ((real_len = dis.read(buf, 1, len)) == -1) {
                        Log.instance().err("SocketReader:  read failed.");
                        break;  //end of file, socket closed
                     }//if
                     str = new String(buf, 0, real_len + 1);
                  }
                  else {
                     str = new String(buf, 0, 1);
                  }
               }//else
            }//if
            else { //we can read alot..
               if (len > 9999) //don't want to overflow buffer
                 len = 9999;
               
               if ((real_len = dis.read(buf, 0, len)) == -1) {
                  Log.instance().err("SocketReader:  read failed.");
                  break;  //end of file, socket closed
               }//if
               
               str = new String(buf, 0, real_len);
            }//else
            
            Log.instance().io("From MUD -:" + str + ":-");
         }//try to get input
         catch (IOException e) {
            Log.instance().err("SocketReader, getting input:  " + e);
            break;
         }

         hif.filter(str);

      }//while
      parent.signalReaderDeath();
      Log.instance().inf("Done with while loop:  Server closed connection");
   }//run

   public void setOutput(HegemonScroll dc) {
      hif.setOutput(dc);
   }

   public DataInputStream getDis() {
      return dis;
   }

   public void destroy() {
      try {
         if (dis != null)
           dis.close ();
      }
      catch (IOException e) {
         Log.instance().err("Error in finalize:  " + e);
      }
   }
   
   protected void finalize () {
      destroy();
   }
   
}//Reader	  

