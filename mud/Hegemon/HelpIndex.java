// $Id: HelpIndex.java,v 1.4 2001/03/30 07:01:51 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date: 2001/03/30 07:01:51 $

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
import java.awt.event.*;
import java.io.*;
import java.net.*;

// This class represents an index for a certain category of
// help topics...
class HelpIndex extends Frame {
   HelpFrame hf;
   HegemonManager hm;
   List topics; //easily displayable
   String category; //this will be a directory name!

   public HelpIndex() {
      super("Scry Help Topics:  UNKNOWN");
   }
   
   public HelpIndex(HegemonManager h, HelpFrame help_frame, String cat) {
      super("Scry Help Topics:  " + cat);
      topics = new List(25, false);
      hm = h;
      hf = help_frame;
      category = cat.toLowerCase();
      
      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            setVisible(false);
         }});

      try {
         readObject();
      }
      catch (Exception e) {
         Log.instance().err("HelpIndex():  " + e);
      }

      topics.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_display();
         }});

      add(topics, "North");
      add(new HIButtons(this), "South");
      pack();
   }//constructor

   // Adds this topic into the List of topics, in a sorted manner.
   // Has no other effect...
   private void insertSorted(String topic) {
      String tmp;
      int len = topics.getItemCount();
      int cmp_result;
      
      if (len == 0) {
         topics.add(topic);
         return;
      }
      else {
         for (int i = 0; i<len; i++) {
            tmp = topics.getItem(i);
            cmp_result = topic.compareTo(tmp);
            if (cmp_result < 0) {
               topics.add(topic, i);
               return;
            }//if
            else if (cmp_result == 0) {
               //don't want duplicate copies!!
               return;
            }//if
         }//for
         topics.add(topic);
         return;
      }//else
   }//insertSorted

   public String[] getTopics() {
      return topics.getItems();
   }
   
   public void do_close() {
      setVisible(false);
   }

   // This asks the server for 'help' on a certain topic.  Other code
   // deals with the output from the server....
   public void do_update() {
      //first, gotta get selected item
      String targ = topics.getSelectedItem();

      if (targ == null) {
         MessageDialog md =
           new MessageDialog("Selection Failure",
                             "You must select a topic to update.",
                             "red", "black");
      }
      else {
         // send out a help request on this item
         try {
            hm.getSocketManager().write("help " + targ + "\n");
         }
         catch (Exception e) {
            MessageDialog md =
              new MessageDialog("Communication Failure",
                                "Could not communicate with Scry Server:  "
                                + e, "red", "black");
         }
      }//else
   }//do_update

   
   public void do_display() {
      String targ = topics.getSelectedItem();

      if (targ == null) {
         MessageDialog md =
           new MessageDialog("Selection Failure",
                             "You must select a topic to display.",
                             "red", "black");
      }
      else {
         displayTopic(targ);
      }
   }//do_display

   
   // HelpFrame knows the base directory...
   public void displayTopic(String topic_name) {
      if (!hm.IS_APPLET)
        hf.show(category + System.getProperty("file.separator")
                + topic_name.toLowerCase());
      else {
         hf.show(category + "/" + topic_name.toLowerCase());
      }
   }


   // Assume values coming in are good...  Adds the topic to the list of
   // topics, if needed, and writes the body out to disk for later retrieval.
   // At this time the first arg is ignored.
   public  void saveHelpTopic(String cat, String tpc, String body) {
      insertSorted(tpc);

      if (hm.IS_APPLET)
         return;
      
      String base_dir = hm.getProperties().getProperty("HelpFileBaseDir");

      base_dir = base_dir.toLowerCase();
      tpc = tpc.toLowerCase();
      
      try {

         try {
            writeObject();
         }
         catch (IOException e) {
            Log.instance().err("ERROR: saveHelpTopic, writeObject:  " + e);
         }

         BOStream ostream =
           new BOStream(new FileOutputStream(base_dir +
                                             System.getProperty("file.separator")
                                             + category +
                                             System.getProperty("file.separator")
                                             + tpc));
         
         ostream.write("Category:  " + cat + "  Topic:  " + tpc + "\n\n");
         ostream.write(body);
      }
      catch (Exception e) {
         MessageDialog md =
           new MessageDialog("Write Failure",
                             "Failed to write help topic to disk, Category: "
                             + cat + "  Topic:  " + tpc + " Error:  " + e,
                             "red", "black");
      }//catch
   }//saveHelpTopic


   
   // First write out the the names of all the topics,
   //  then write out each of the topics in seperate files...
   
   /** Writes it all to disk, for later retrieval by load() */
   public void writeObject() throws IOException {
      if (hm.IS_APPLET)
         return;
      
      //get the file which contains a list of all the other files
      String fl = hm.getProperties().getProperty("HelpTopicsFile");
      String base_dir = hm.getProperties().getProperty("HelpFileBaseDir");
      
      //      Log.it("fl:  " + fl);
      String[] topic_array = topics.getItems();
      int len = topic_array.length;
      fl = fl.toLowerCase();
      
      try {
         File dr = new File(base_dir + System.getProperty("file.separator")+ category);
         dr.mkdirs();

         BOStream ostream =
           new BOStream(new FileOutputStream(base_dir +
                                             System.getProperty("file.separator")
                                             + category +
                                             System.getProperty("file.separator")
                                             + fl));

         for (int i = 0; i<len; i++) {
            //Log.it("Writing Help Topic:  " + topics.getItem(i));
            ostream.write(topics.getItem(i));
            ostream.write("\n");
         }//for

         ostream.flush();
      }//try
      catch (Exception e) {
         MessageDialog md =
           new MessageDialog("Write Failure",
                             "Failed to write help topics to disk: " +
                             e, "red", "black");
      }//catch
   }//writeObject

   public void readObject() throws IOException {
      String fl = hm.getProperties().getProperty("HelpTopicsFile");
      String base_dir = hm.getProperties().getProperty("HelpFileBaseDir");

      if (fl == null) {
         Log.instance().wrn("WARNING:  getProperty(\"HelpTopicsFile\") returned NULL.");
         fl = "scry_topics_list";
      }

      fl = fl.toLowerCase();
      String rslt = null;
      StringBuffer action = new StringBuffer(200);
      String abs_file_name;

      if (hm.IS_APPLET)
        abs_file_name = base_dir + "/" + category + "/" + fl;
      else
        abs_file_name = base_dir + System.getProperty("file.separator")
          + category + System.getProperty("file.separator") + fl;
        
      try {
         BIStream istream;
         if (hm.IS_APPLET) {
            URL url = new URL(abs_file_name);
            istream = new BIStream(url.openStream());
         }
         else {
            istream = new BIStream(new FileInputStream(abs_file_name));
         }
         
         while ((rslt = istream.readLine()) != null) {
            insertSorted(rslt.trim());
         }//while
      }//try
      catch (IOException e) {
         Log.instance().err("Trying to read HelpIndex from file:\n" + abs_file_name);
         Log.instance().err("ERROR:  in HelpIndex.readObject():  " + e);
      }
   }//readObject
}//HelpFrame


class HIButtons extends Panel {
   HelpIndex parent;
   
   public HIButtons(HelpIndex par) {
      super();
      parent = par;

      Button display_b = new Button("Display");
      Button update_b = new Button("Update");
      Button done_b = new Button("Done");

      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_close();
         }});

      display_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_display();
         }});

      update_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_update();
         }});

      FlowLayout fl = new FlowLayout();
      setLayout(fl);

      add(update_b);
      add(display_b);
      add(done_b);
   }//HIButtons constructor
}//HIButtons
