// $Id$
// $Revision: 1.4 $  $Author$ $Date$

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



class HostAddr {
   String mud_name;
   String host_name;
   int port;

   public HostAddr() {
      mud_name = new String();
      host_name = new String();
      port = 4000;
   }
   
   public HostAddr(String mn, String hn, int pt) {
      mud_name = mn;
      host_name = hn;
      port = pt;
   }

   public HostAddr(String glom) {
      int len = glom.length();
      int idx = 0;
      int newidx = 0;

      //Log.it("In HostAddr:  " + glom);
      
      try {
         idx = glom.indexOf("|");
         mud_name = (glom.substring(0, idx)).trim();
         idx++;

         newidx = glom.indexOf("|", idx);
         host_name = (glom.substring(idx, newidx)).trim();
         newidx++;
      
         port = Integer.parseInt((glom.substring(newidx, len)).trim());
      }
      catch (Exception e) {
         Log.instance().err("ERROR:  malformed glom:  HostAddr:  " + e);
         MessageDialog md = new MessageDialog("Host Address Error",
                                              e.toString(), "red", "black");
      }
   }//Constructor from one string

   public String getMudName() {
      return mud_name;
   }

   public final String getHostName() {
      return host_name;
   }

   public final int getPortNum() {
      return port;
   }

   public String toString() {
      return (mud_name + " | " + host_name + " | " + port);
   }
}
