// $Id: ConnectionManager.java,v 1.7 2001/03/30 07:01:51 greear Exp $
// $Revision: 1.7 $  $Author: greear $ $Date: 2001/03/30 07:01:51 $

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

import java.awt.*;
import java.util.*;
import java.awt.event.*;
import java.io.*;
import java.net.*;

class ConnectionManager extends Frame {
   java.awt.List hosts;
   HegemonManager hm;
   
   public ConnectionManager(HegemonManager h) {
      super("Connection Manager");
      hosts = new java.awt.List(10, false);
      hm = h;

      hosts.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_done();
            hm.openConnection();
         }});

      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            do_done();
         }});


      try {
         readObject();
      }
      catch (Exception e) {
         Log.instance().wrn("ConnectionManager:  " + e);

         // Set some defaults.
         hosts.add("ScryMUD DEV | scry.wanfear.com | 4445");
         hosts.add("Scry@WANfear | scry.wanfear.com | 4444");
         hosts.add("ScryMUD | localhost | 4000");
      }
      
      setLayout(new BorderLayout());
      add(hosts, "North");
      add(new CMButtons(this), "South");
      pack();
   }

   public void addHost(HostAddr ha) {
      hosts.add(ha.toString());
   }

   public void addHost(String str) {
      hosts.add(str);
      try {
         writeObject();
      }
      catch (IOException e) {
         Log.instance().err("addHost:  " + e);
      }
   }
   
   public void createConnection() {
      HostDialog hd = new HostDialog(this, new HostAddr());
      hd.show();
   }

   private String getSelected() {
      return hosts.getSelectedItem(); //can be null
   }

   public String getHostname() {
      String str = getSelected();

      try {
         if (str == null) {
            str = hosts.getItem(0);
         }
         
         HostAddr ha = new HostAddr(str);
         return ha.getHostName();
      }
      catch (Exception e) {
         return "scry.wanfear.com"; //hard code a working value
      }
   }//getHostName


   public int getPortnum() {
      String str = getSelected();

      try {
         if (str == null) {
            str = hosts.getItem(0);
         }

         HostAddr ha = new HostAddr(str);
         return ha.getPortNum();
      }
      catch (Exception e) {
         return 4444;
      }
   }//getHostName

   public void do_delete() {
      String str;
      if ((str = hosts.getSelectedItem()) != null) {
         hosts.remove(str);
         try {
            writeObject();
         }
         catch (IOException e) {
            Log.instance().err("ERROR:  could not write out Servers file: " + e);
         }
      }
      else {
        MessageDialog md =
          new MessageDialog("Connection Manager",
                            "You need to select a connection to delete.",
                            "red", "black");
      }
   }

   public void do_new() {
      createConnection();
   }

   public void do_done() {
      setVisible(false);
   }

   public void do_edit() {
      String str;
      if ((str = hosts.getSelectedItem()) != null) {
         hosts.remove(str); //editing will re-add it
         HostDialog hd = new HostDialog(this, new HostAddr(str));
         hd.show();
      }
      else {
        MessageDialog md =
          new MessageDialog("Connection Manager",
                            "You need to select a connection to edit.",
                            "red", "black");
      }
   }//do_edit

   /** Writes it all to disk, for later retrieval by readObject() */
   public void writeObject() throws IOException {
      if (hm.IS_APPLET)
         return;
      
      BOStream ostream =
         new BOStream(new FileOutputStream(hm.getProperties().getProperty("HostFile")));
      String[] entries = hosts.getItems();
      int len = entries.length;
      try {
         for (int i = 0; i<len; i++) {
            ostream.write(entries[i]);
            ostream.write("\n~\n");
         }//for
      }//try
      catch (Exception e) {
         MessageDialog md =
           new MessageDialog("Write Failure",
                             "Failed to write hosts to disk: " +
                             e, "red", "black");
      }//catch
   }//writeObject

   /**  Read it in from a file specified in the hegemon.rc */
   public void readObject() throws IOException {
      BIStream istream;
      String conn_file = hm.getProperties().getProperty("HostFile");

      if (hm.IS_APPLET) {
         URL url = new URL(conn_file);
         istream = new BIStream(url.openStream());
      }
      else {
         istream = new BIStream(new FileInputStream(conn_file));
      }

      String rslt;
      StringBuffer action = new StringBuffer(200);
      
      try {
         while ((rslt = istream.readLine()) != null) {
            if ((rslt.trim()).equals("~")) {
               hosts.add((action.toString()).trim());
               action = new StringBuffer(200);
            }
            else {
               action.append(rslt + " ");
            }
         }//while
      }//try
      catch (Exception e) {
         Log.instance().err("in ConnectionManager.readObject():  " + e);
      }
   }//readObject

}//ConnectionManager


class CMButtons extends Panel {
   ConnectionManager parent;

   public CMButtons(ConnectionManager par) {
      super();
      parent = par;

      Button done_b = new Button("Done");
      Button edit_b = new Button("Edit");
      Button new_b = new Button("New");
      Button delete_b = new Button("Delete");

      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_done();
         }});

      edit_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_edit();
         }});

      new_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_new();
         }});

      delete_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_delete();
         }});
   
      setLayout(new FlowLayout());

      add(new_b);
      add(edit_b);
      add(delete_b);
      add(done_b);
   }//constructor for CMButtons
}//CMButtons
