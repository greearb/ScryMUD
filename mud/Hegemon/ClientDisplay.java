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

import java.awt.event.*;
import java.awt.*;
import java.util.*;
import java.net.*;

/**  This is our main GUI, it holds all the rest, including the
  HegemonDisplay, which draws the stuff coming from the server. */
class ClientDisplay extends Frame {

   HegemonManager hm; //keep track of all our different objects
   HelpFrame hf;
   ColorChoicesFrame ccf;
   LogFrame log_frame;

   OlEditor oe; //for quick description changes
   PathCellEditor pe;
   MobScriptEditor mse;
   KeywordEditor ke;

   CommandHistory cmd_history;

   MenuBar menubar;
   Menu file_m, connections_m, olc_m, controls_m, help_m;
   
   HegemonDisplay output_field;
   InputArea input_field;

   /* MENU items:  need a handle to enable/disable them. */
   MenuItem action_turn_off_mi;
   MenuItem action_turn_on_mi;
   MenuItem alias_turn_off_mi;
   MenuItem alias_turn_on_mi;
   /* END of menu Items */

   public HegemonScroll getHegemonScroll() {
      return output_field.getScroll();
   }
   
   public ClientDisplay(HegemonManager h) {
      super("Hegemon Client");
      hm = h;

      //our help system :)
      hf = hm.getHelpFrame();
      hf.setLocation(325, 110);

      ccf = new ColorChoicesFrame("Color Choices");

      cmd_history = hm.getCommandHistory();
      cmd_history.setLocation(200, 50);

      oe = hm.getOlEditor();
      oe.setLocation(345, 50);


      pe = hm.getPathCellEditor();
      pe.setLocation(355, 60);


      mse = hm.getMobScriptEditor();
      mse.setLocation(350, 55);
      
      ke = hm.getKeywordEditor();
      ke.setLocation(360, 65);

      log_frame = new LogFrame(this, hm);

      ///*******************  File Menu  *****************************///
      
      MenuItem quit_action;
      MenuItem logging_action;
      MenuItem color_view_mi, color_choices_mi;
      MenuItem action_edit_mi;
      MenuItem alias_edit_mi;
      MenuItem cmd_history_mi;

      file_m = new Menu("File");
      file_m.add((quit_action = new MenuItem("Quit")));
      file_m.add((logging_action = new MenuItem("Logging")));
      
      file_m.addSeparator();

      quit_action.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            quit();
         }});

      logging_action.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            showLoggingFrame();
         }});
      
      file_m.add((color_view_mi = new MenuItem("Modify Colors")));

      color_view_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doViewColors();
         }});

      file_m.add((color_choices_mi = new MenuItem("Color Choices")));

      color_choices_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doViewChoices();
         }});

      file_m.addSeparator();
      
      file_m.add((action_edit_mi = new MenuItem("Edit Actions")));
      file_m.add((action_turn_on_mi = new MenuItem("Enable Actions")));
      file_m.add((action_turn_off_mi = new MenuItem("Disable Actions")));
      
      action_turn_on_mi.setEnabled(false); //they start out ON already
      
      action_edit_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_actions();
         }});
      action_turn_on_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_actions_enable();
         }});
      action_turn_off_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_actions_disable();
         }});
      
      
      file_m.addSeparator();
      file_m.add((alias_edit_mi = new MenuItem("Edit Aliases")));
      file_m.add((alias_turn_on_mi = new MenuItem("Enable Aliases")));
      file_m.add((alias_turn_off_mi = new MenuItem("Disable Aliases")));
      
      alias_turn_on_mi.setEnabled(false); //they start out ON already
      

      alias_edit_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_aliases();
         }});
      alias_turn_on_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_alias_enable();
         }});
      alias_turn_off_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_alias_disable();
         }});
      
      file_m.addSeparator();
      file_m.add((cmd_history_mi = new MenuItem("Command History")));

      cmd_history_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_show_cmd_history();
         }});
      
      
      ///********************  connection menu ****************///
      
      MenuItem open_connect_mi, select_connect_mi;
      connections_m = new Menu("Connections");
      connections_m.add((open_connect_mi = new MenuItem("Open Connection")));
      connections_m.add((select_connect_mi = new MenuItem("Select Server")));
      
      open_connect_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_open_connection(); //will take selected, or if none selected,
            //the first one
         }});
      
      select_connect_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_select_connection(); //pops up Connection Manager
         }});
      
      ///******************  olc_menu  ********************************///
      
      MenuItem create_door;
      olc_m = new Menu("OLC");
      olc_m.add((create_door = new MenuItem("Create Door")));
      
      create_door.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            create_door();
         }});
      
      MenuItem create_room;
      olc_m.add((create_room = new MenuItem("Create Room")));
      
      create_room.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            create_room();
         }});
      
      MenuItem create_object;
      olc_m.add((create_object = new MenuItem("Create Object")));
      
      create_object.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            create_object();
         }});
      
      MenuItem create_mob;
      olc_m.add((create_mob = new MenuItem("Create Mobile")));
      
      create_mob.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            create_mob();
         }});

      MenuItem olc_editor;
      olc_m.add((olc_editor = new MenuItem("Description Editor")));
      
      olc_editor.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doOlcEditor();
         }});


      MenuItem p_editor;
      olc_m.add((p_editor = new MenuItem("Vehicle Path Editor")));
      
      p_editor.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doPathEditor();
         }});


      MenuItem ms_editor;
      olc_m.add((ms_editor = new MenuItem("Mob/Room Script Editor")));
      
      ms_editor.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doMSEditor();
         }});


      MenuItem k_editor;
      olc_m.add((k_editor = new MenuItem("Keyword Editor")));
      
      k_editor.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doKEditor();
         }});


      olc_m.addSeparator();
      
      MenuItem asave;
      olc_m.add((asave = new MenuItem("Save Current Area")));
      
      asave.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("asave\n");
         }});

      MenuItem aosave;
      olc_m.add((aosave = new MenuItem("Save Cur. Zone's Objects")));
      
      aosave.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("aosave\n");
         }});

      MenuItem amsave;
      olc_m.add((amsave = new MenuItem("Save Cur. Zone's Mobs")));
      
      amsave.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            send("amsave\n");
         }});

      
      
      ///*************************  Help Menu  ********************///
      
      help_m = new Menu("Help");

      MenuItem help_intro_mi = new MenuItem("View Topics");
      MenuItem help_view_mi, help_about_mi;

      if (!hm.IS_APPLET) {
         help_m.add(help_intro_mi);
      }

      help_m.add((help_about_mi = new MenuItem("About")));
      
      help_intro_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doIntroHelp();
         }});

      help_about_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            doHegemonHelp();
         }});



      ///*************************  Controls Menu  ********************///
      
      MenuItem pso_mi;
      controls_m = new Menu("Controls");

      controls_m.add((pso_mi = new MenuItem("Player-Run Stores")));

      pso_mi.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_show_pso_editor();
         }});
      
      
      

      ///*************************  MenuBar *******************///
      
      menubar = new MenuBar();
      setMenuBar(menubar);
      
      menubar.add(file_m);
      menubar.add(connections_m);
      menubar.add(olc_m);
      menubar.add(controls_m);
      menubar.add(help_m);
      
      ///*******************  IO fields ********************///
      output_field = new HegemonDisplay(hm);
      
      input_field = new InputArea(this, hm);

      ///*********************  Layout  ****************************///
      
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      // Scroll panel
      setLayout(gridbag);
      int REM = c.gridwidth = GridBagConstraints.REMAINDER;
      c.fill = GridBagConstraints.BOTH;
      c.weightx = 1.0;
      c.weighty = 2.0;

      gridbag.setConstraints(output_field, c);
      add(output_field);

      //Input Field
      //c.gridwidth = REM;
      c.weighty = 1.0;
      //c.weightx = 1.0;
      c.fill = GridBagConstraints.BOTH;
      gridbag.setConstraints(input_field, c);
      add(input_field);
      
      this.pack();
      setSize(650, 600);
   }//constructor

   void do_open_connection() {
      hm.openConnection();
   }//open_connection

   void do_select_connection() {
      hm.showConnection();
   }

   void showLoggingFrame() {
      log_frame.show();
   }
   
   void do_actions() {
      hm.showActions();
   }

   void do_actions_enable() {
      hm.getActionManager().setActionsEnabled(true);
      action_turn_on_mi.setEnabled(false); //grey it out
      action_turn_off_mi.setEnabled(true); //un-grey it
   }

   void do_actions_disable() {
      hm.getActionManager().setActionsEnabled(false);
      action_turn_off_mi.setEnabled(false); //grey it out
      action_turn_on_mi.setEnabled(true); //un-grey it
   }

   void do_aliases() {
      hm.showAliases();
   }

   void do_alias_enable() {
      hm.getAliasManager().setAliasEnabled(true);
      alias_turn_on_mi.setEnabled(false); //grey it out
      alias_turn_off_mi.setEnabled(true); //un-grey it
   }

   void do_alias_disable() {
      hm.getAliasManager().setAliasEnabled(false);
      alias_turn_off_mi.setEnabled(false); //grey it out
      alias_turn_on_mi.setEnabled(true); //un-grey it
   }

   void quit() {
      hm.quit();
   }

   void create_door() {
      OLCDoor od = new OLCDoor(this, hm);
      od.show();
   }

   void create_room() {
      OLCRoom od = new OLCRoom(this, hm);
      od.show();
   }

   void create_mob() {
      OLCMob od = new OLCMob(this, hm);
      od.show();
   }

   void create_object() {
      OLCObject od = new OLCObject(this, hm);
      od.show();
   }

   void enterOLC() {// on the mud server, prompts should start coming...
      try {
         hm.getSocketManager().write("olc\n");
      }
      catch (Exception e) {
         Log.instance().err("enterOLC:  " + e);
      }
   }

   void submitRoom(OLCRoom targ) {
      if (targ.isVehicle())
         hm.getOlcManager().setCase0("6");
      else
         hm.getOlcManager().setCase0("3");

      hm.getOlcManager().setRoom(targ);
      enterOLC();
   }

   void submitObj(OLCObject targ) {
      hm.getOlcManager().setCase0("1");
      hm.getOlcManager().setObj(targ);
      enterOLC();
   }

   void submitDoor(OLCDoor targ) {
      hm.getOlcManager().setCase0("4");
      hm.getOlcManager().setDoor(targ);
      enterOLC();
   }

   void submitMob(OLCMob targ) {
      hm.getOlcManager().setCase0("2");
      hm.getOlcManager().setMob(targ);
      enterOLC();
   }

   public final HegemonDisplay getOutput() {
      return output_field;
   }

   void doViewHelp() {
      if (!hm.IS_APPLET) {
         hf.setVisible(true);
      }
      else {
         try {
            URL u = new URL("http://www.primenet.com/~greear/ScryMUD/mort_help.html");
            hm.getMotherClass().getAppletContext().showDocument(u);
         }//try
         catch (Exception e) {
            e.printStackTrace();
         }
      }//else
   }//doViewHelp

   void doOlcEditor() {
      oe.setVisible(true);
   }

   void doPathEditor() {
      pe.setVisible(true);
   }

   void doMSEditor() {
      //Log.instance().dbg("Making MobScript editor visible..");
      mse.setVisible(true);
   }

   void doKEditor() {
      //Log.it("Making Keyword editor visible..");
      ke.setVisible(true);
   }

   void doIntroHelp() {
      hf.setVisible(true);
      hf.show("opening_page.bml");
   }

   void doHegemonHelp() {
      hf.setVisible(true);
      hf.show("hegemon.bml");
   }
   
   void doViewColors() {
      hm.showColors();
   }

   void doViewChoices() {
      ccf.setVisible(true);
   }

   public void giveFocus() {
      input_field.getTA().requestFocus();
   }

   public final InputArea getInputField() {
      return input_field;
   }

   public void do_show_cmd_history() {
      cmd_history.setVisible(true);
   }

   public void do_show_pso_editor() {
      hm.getPsoEditor().setVisible(true);
   }

   public void send(String str) {
      try {
         (hm.getSocketManager()).write(str);
      }
      catch (Exception ex) {
         MessageDialog md =
           new MessageDialog("INPUT ERROR", ex.toString(), "red",
                             "black");
      }
   }
}//ClientDisplay




