// $Id$
// $Revision: 1.5 $  $Author$ $Date$

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

import java.awt.*;
import java.io.*;
import java.awt.event.*;
import java.net.*;

/** Help GUI Class. */
class HelpFrame extends Frame {
   private String base_dir;
   private HegemonDisplay hd = null;
   //private HelpFrameButtons hfb = null;

   private HegemonInputFilter help_filter = null;
   private HelpIndex battle_index = null;
   private HelpIndex comm_index = null;
   private HelpIndex skills_index = null;
   private HelpIndex spells_index = null;
   private HelpIndex olc_index = null;
   private HelpIndex imm_index = null;
   private HelpIndex general_index = null;
   private HelpIndex client_index = null;
   
   private MenuBar menubar;
   private Menu file_m, indexes_m;
   private HegemonManager hm;

   // Used when reading in a help file from the MUD
   private String help_topic;
   private String help_category;
   private StringBuffer help_text;
   
   public HelpFrame(String title, String base_directory,
                    ColorSelectionManager csm, HegemonManager h) {
      super(title);

      hm = h;

      if (hm == null)
         Log.instance().err("ERROR:  hm is NULL in HelpFrame constructor.");

      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            setVisible(false);
         }});

      
      base_dir = base_directory; //of our help pages
      hd = new HegemonDisplay(hm);

      menubar = new MenuBar();
      setMenuBar(menubar);

      file_m = new Menu("File");

      MenuItem done_mi = new MenuItem("Done");
      done_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_close();
         }});

      indexes_m = new Menu("Indexes");

      battle_index = new HelpIndex(hm, this, "Battle"); //read it in from file
      MenuItem battle_index_mi = new MenuItem("Battle");
      battle_index_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            battle_index.setVisible(true);
         }});

      client_index = new HelpIndex(hm, this, "Client"); //read it in from file
      MenuItem client_index_mi = new MenuItem("Client");
      client_index_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            client_index.setVisible(true);
         }});

      comm_index = new HelpIndex(hm, this, "Communications"); //read it...
      MenuItem comm_index_mi = new MenuItem("Communications");
      comm_index_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            comm_index.setVisible(true);
         }});
      
      skills_index = new HelpIndex(hm, this, "Skills"); //read it in from file
      MenuItem skills_index_mi = new MenuItem("Skills");
      skills_index_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            skills_index.setVisible(true);
         }});
      
      spells_index = new HelpIndex(hm, this, "Spells"); //read it in from file
      MenuItem spells_index_mi = new MenuItem("Spells");
      spells_index_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            spells_index.setVisible(true);
         }});
      
      olc_index = new HelpIndex(hm, this, "OLC"); //read it in from file
      MenuItem olc_index_mi = new MenuItem("OLC");
      olc_index_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            olc_index.setVisible(true);
         }});
      
      general_index = new HelpIndex(hm, this, "General"); //read it in from file
      MenuItem general_index_mi = new MenuItem("General");
      general_index_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            general_index.setVisible(true);
         }});

      imm_index = new HelpIndex(hm, this, "Immortal"); //read it in from file
      MenuItem imm_index_mi = new MenuItem("Immortal");
      imm_index_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            imm_index.setVisible(true);
         }});

      client_index.setLocation(450, 100);
      general_index.setLocation(450, 100);
      comm_index.setLocation(450, 100);
      battle_index.setLocation(450, 100);
      skills_index.setLocation(450, 100);
      spells_index.setLocation(450, 100);
      imm_index.setLocation(450, 100);
      olc_index.setLocation(450, 100);
      

      indexes_m.add(general_index_mi);
      indexes_m.add(comm_index_mi);
      indexes_m.add(battle_index_mi);
      indexes_m.add(skills_index_mi);
      indexes_m.add(spells_index_mi);
      indexes_m.add(olc_index_mi);
      indexes_m.add(imm_index_mi);
      indexes_m.add(client_index_mi);

      
      file_m.add(done_mi);
      
      menubar.add(file_m);
      menubar.add(indexes_m);
      
      help_filter = new HegemonInputFilter(null, null, null, hm,
                                           hd.getScroll(), csm);
      
      hd.getScroll().setScrollComponentCollection(new ScrollComponentVector(100));

      add("Center", hd);

      this.pack();
      this.setSize(500, 550);
   }//constructor

   public void append(String str) {
      hd.getScroll().append(str);
   }

   public void showString(String str) {
      help_filter.filter(str);
   }

   public void appendAndCache(String str) {
      hd.getScroll().append(str);
      help_text.append(str);
   }

   public HegemonDisplay getHegemonDisplay() {
      return hd;
   }
   
   public void setHelpCategory(String cat) {
      help_category = cat;
      help_text = new StringBuffer(1024);
      hd.getScroll().clear();
   }

   public void setHelpTopic(String tpc) {
      help_topic = tpc;
   }

   public void paint(Graphics g) {
      //Log.it("TRACE:  Painting HelpFrame.");

      super.paint(g);
   }//paint
   
   public void saveHelpText() {
      if (hm.IS_APPLET)
         return;
      
      //first, find out which category it belongs too
      if (help_category != null) {
         if (help_category.equalsIgnoreCase("Battle")) 
           battle_index.saveHelpTopic("Battle", help_topic,
                                      help_text.toString());
         else if (help_category.equalsIgnoreCase("Communications"))
           comm_index.saveHelpTopic("Communications",
                                    help_topic, help_text.toString());
         else if (help_category.equalsIgnoreCase("Skills"))
           skills_index.saveHelpTopic("Skills",
                                      help_topic, help_text.toString());
         else if (help_category.equalsIgnoreCase("Spells"))
           spells_index.saveHelpTopic("Spells",
                                      help_topic, help_text.toString());
         else if (help_category.equalsIgnoreCase("OLC"))
           olc_index.saveHelpTopic("OLC",
                                   help_topic, help_text.toString());
         else if (help_category.equalsIgnoreCase("Immortal"))
           imm_index.saveHelpTopic("Immortal",
                                    help_topic, help_text.toString());
         else if (help_category.equalsIgnoreCase("General"))
           general_index.saveHelpTopic("General",
                                    help_topic, help_text.toString());
         else if (help_category.equalsIgnoreCase("Client"))
           client_index.saveHelpTopic("Client",
                                    help_topic, help_text.toString());
         else {
            Log.instance().err("ERROR:  bad help_category in saveHelpText:  "
                               + help_category);
         }
      }//if
      else {
         Log.instance().err("ERROR:  help_category is NULL in saveHelpText.");
      }//else
   }//saveHelpText
   
   public void clear() {
      hd.clear();
      hd.noAutoScroll();
   }
   
   public void show(String file_name) {
      Log.instance().trc("TRACE:  HelpFrame.show(" + file_name + ")");
      
      byte[] buffer = new byte[4097];
      int amt_read;
      String str;

      clear();

      FileInputStream istream = null;

      try {
         BufferedInputStream bis;
         
         if (!hm.IS_APPLET) {
            istream = new FileInputStream(base_dir + 
                                          System.getProperty("file.separator")
                                          + file_name);
            bis = new BufferedInputStream(istream);
         }
         else { //is applet
            URL url = new URL(base_dir + "/" + file_name);
            InputStream ips = url.openStream();
            bis = new BufferedInputStream(ips);
         }
         
         help_filter.filter("<color = blue>");
         while ((amt_read = bis.read(buffer, 0, 4096)) > 0) {
            str = new String(buffer, 0, amt_read);
            //Log.it("From HelpFile:  " + str);
            help_filter.filter(str);
         }//while
         help_filter.filter("</color>");
      }//try
      catch (IOException e) {
         new MessageDialog("HELP ERROR", "Error in HelpFrame.show():  " + e,
                           "red", "black");
      }//catch
      this.setVisible(true);
   }//show

   public void do_close() {
      this.setVisible(false);
   }

}//HelpFrame


/*
class HelpFrameButtons extends Panel {
   HelpFrame parent;
   
   public HelpFrameButtons(HelpFrame par) {
      super();
      parent = par;
      
      Button done_b = new Button("Done");
      Button open_b = new Button("Open File");
      
      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_close();
         }});

      open_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_open_file();
         }});

      FlowLayout fl = new FlowLayout();
      setLayout(fl);

      add(open_b);
      add(done_b);
   }//constructor
}//HelpFrameButtons
*/
