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

// This allows for some quick editing of already-created objects,
// mobs, and rooms.  It auto-captures the info for easy editing...

class MobScriptEditor extends Frame {
   LabeledChoice trigger;
   LabeledTextField discrim;
   LabeledTextField target;
   LabeledTextField actor;
   LabeledChoice precedence;
   LabeledTextArea script;
   LabeledTextField mob_num;
   LabeledChoice entity;
   LabeledChoice discrim_choice;
   LabeledTextField script_idx;

   String[] cmds = {"close", "donate", "drop", "eat", "enter",
                    "examine", "exit", "fill", "follow",
                    "get", "give", "group", "hit", "list", "lock",
                    "look", "meditate", "nod", "open", "order",
                    "pay", "prone", "remove", "rest", "say", "shake",
                    "shoot", "sit", "slap", "sleep", "stand", "tell",
                    "throw", "ungroup", "unlock", "wake", "wear",
                    "yell" };
   
   String[] discrims = { "FEM", "MALE", "NEUTER", "BARD", "CLERIC",
                         "WARRIOR", "SAGE", "WIZARD", "THIEF", "ALCHEMIST",
                         "HUMAN", "ANITRE", "DARKLING", "DRAGON", "DWARF",
                         "OGRUE", "ELF", "UNDEAD", "ANIMAL", "MONSTER" };

   HegemonManager hm;
   MSButtons oeb;

   public MobScriptEditor(HegemonManager h) {
      super("Script Editor");
      hm = h;
      Log.instance().init("Constructing ScriptEditor..");

      trigger = new LabeledChoice("Trigger Command");
      entity = new LabeledChoice("Entity");
      discrim = new LabeledTextField("Optional Discriminator (Special tags have syntax: 'only')",
                                     "NA", 20);
      target = new LabeledTextField("Target #", "", 10);
      actor = new LabeledTextField("Actor #", "", 10);
      precedence = new LabeledChoice("Precedence (0 == none)");
      script = new LabeledTextArea("Script (double semicolons necessaary)",
                                   "", 10, 80);
      mob_num = new LabeledTextField("Entity # (Script Owner)", "0", 10);
      discrim_choice = new LabeledChoice("Special Descrim.");
      script_idx = new LabeledTextField("Script Index", "", 10);

      oeb = new MSButtons(this);

      entity.c.add("ROOM");
      entity.c.add("MOB");
      setEntity("MOB");

      /* add triggers, will need to update this often. */
      for (int i = 0; i < cmds.length; i++) {
         trigger.c.add(cmds[i]);
      }

      for (int i = 0; i<10; i++) {
         precedence.c.add(i + "");
      }

      for (int i = 0; i<10; i++) {
         discrim_choice.c.add(discrims[i]);
      }

      discrim_choice.c.addItemListener(new ItemListener() {
         public void itemStateChanged(ItemEvent e) {
            if (discrim.getText().equals("NA")) {
               discrim.clear();
            }
            discrim.append(discrim_choice.getText());
         }});

      int REM = GridBagConstraints.REMAINDER;
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);

      c.fill = GridBagConstraints.BOTH;

      c.gridwidth = 1;
      
      gridbag.setConstraints(entity, c);
      add(entity);

      gridbag.setConstraints(trigger, c);
      add(trigger);

      gridbag.setConstraints(precedence, c);
      add(precedence);

      gridbag.setConstraints(mob_num, c);
      add(mob_num);

      c.gridwidth = REM;
      gridbag.setConstraints(discrim_choice, c);
      add(discrim_choice);

      c.gridwidth = 1;
      gridbag.setConstraints(actor, c);
      add(actor);

      gridbag.setConstraints(target, c);
      add(target);
      
      gridbag.setConstraints(script_idx, c);
      add(script_idx);

      c.gridwidth = REM;
      gridbag.setConstraints(discrim, c);
      add(discrim);

      gridbag.setConstraints(script, c);
      add(script);

      gridbag.setConstraints(oeb, c);
      add(oeb);
/*
      long_desc.addKeyListener(new KeyAdapter() {
         public void keyPressed(KeyEvent e) {
            if (e.getKeyCode() == KeyEvent.VK_CONTROL) {
               do_update(); //send stuff off to server
            }//if
            else
              super.keyPressed(e);
         }});
         */
      pack();
   }//constructor


   public boolean isFrozen() {
      return oeb.isFrozen();
   }
   
   public void do_close() {
      setVisible(false);
   }

   public void do_help() {
      if (!hm.IS_APPLET)
        hm.getHelpFrame().show("client" + System.getProperty("file.separator")
                               + "script_editor");
      else {
         Log.instance().dbg("Trying to show: client/script_editor");
         hm.getHelpFrame().show("client/script_editor");
      }
   }//doHelp
   
   public void setMobScriptDiscrim(String d) {
      if (isFrozen())
        return;

      discrim.append(d);
   }

   public void setEntity(String d) {
      if (isFrozen())
        return;
      entity.c.select(d);
   }

   public void setMobScriptData(String trig, String mnum, String actor_num,
                                String target_num, String pd, String script_index) {
      if (isFrozen())
         return;

      mob_num.append(mnum);
      trigger.c.select(trig);
      actor.append(actor_num);
      target.append(target_num);
      precedence.c.select(pd);
      script_idx.append(script_index);
   }

   public void appendScript(String str) {
      if (isFrozen())
         return;
      script.append(str);
   }

   public void do_clear() {
      trigger.c.select(cmds[0]);
      discrim.clear();
      oeb.setFrozen(false);
      target.clear();
      actor.clear();
      precedence.c.select("0");
      script.clear();
      mob_num.clear();
      script_idx.clear();
   }//

   /**  Read the script from the MUD.  Same as typing stat_script with
    *  the appropriate arguments.
    */
   public void do_refresh() {
      oeb.setFrozen(false);
      String cmd = null;

      if (entity.getText().equalsIgnoreCase("ROOM")) {
         cmd = "stat_room_script ";
      }
      else if (entity.getText().equalsIgnoreCase("MOB")) {
         cmd = "stat_script ";
      }

      else {
         MessageDialog md = new MessageDialog("Refresh Error",
                                              "Was neither ROOM nor MOB entity",
                                              "red", "black");
         return;
      }

      try {
         // first, the in_room_description
         hm.getSocketManager().write(cmd + mob_num.getText()
                                     + " " + script_idx.getText() + "\n");
         
      }//try
      catch (Exception e) {
         MessageDialog md = new MessageDialog("Refresh Error",
                                              "Could not refresh script: "
                                              + e, "red", "black");
      }//catch
      //do_clear();
   }//doRefresh


   public void do_update() {
      oeb.setFrozen(false);
      String cmd = null;

      if (entity.getText().equalsIgnoreCase("ROOM")) {
         cmd = "add_room_script ";
      }
      else if (entity.getText().equalsIgnoreCase("MOB")) {
         cmd = "add_mob_script ";
      }

      else {
         MessageDialog md = new MessageDialog("Update Error",
                                              "Was neither ROOM nor MOB entity",
                                              "red", "black");
         return;
      }

      try {
         // first, the in_room_description
         hm.getSocketManager().write(cmd + mob_num.getText()
                                     + " " + trigger.getText() + " "
                                     + actor.getText() + " '"
                                     + "DISCRIM_" + discrim.getText().trim() + "' " 
                                     + target.getText() + " " + 
                                     precedence.getText() + "\n");
         
         // now the script itself
         hm.getSocketManager().write(script.getText() + "\n~\n");
      }//try
      catch (Exception e) {
         MessageDialog md = new MessageDialog("Update Error",
                                              "Could not update script: "
                                              + e, "red", "black");
      }//catch
      //do_clear();
   }//do_update

}//MobScriptEditor


class MSButtons extends Panel {
   MobScriptEditor parent;
   Button freeze_b;
   
   public MSButtons(MobScriptEditor par) {
      super();
      parent = par;

      freeze_b = new Button("  FREEZE ");
      freeze_b.setBackground(new Color(0, 255,0));

      Button clear_b = new Button("Clear");
      Button cancel_b = new Button("Cancel");
      Button commit_b = new Button("Commit");
      Button done_b = new Button("Done");
      Button help_b = new Button("Help");
      Button refresh_b = new Button("Refresh");

      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_close();
         }});

      freeze_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            if (freeze_b.getLabel().equals("  FREEZE ")) {
               freeze_b.setLabel("UN-FREEZE");
               freeze_b.setBackground(new Color(255,0,0));
            }
            else {
               freeze_b.setLabel("  FREEZE ");
               freeze_b.setBackground(new Color(0,255,0));
            }
         }});

      clear_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_clear();
         }});

      cancel_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_clear();
            parent.do_close();
         }});

      help_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_help();
         }});

      commit_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_update();
         }});

      refresh_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_refresh();
         }});

      FlowLayout fl = new FlowLayout();
      setLayout(fl);

      add(freeze_b);
      add(refresh_b);
      add(commit_b);
      add(cancel_b);
      add(clear_b);
      add(done_b);
      add(help_b);
   }//MSButtons constructor

   public boolean isFrozen() {
      return freeze_b.getLabel().equals("UN-FREEZE");
   }

   public void setFrozen(boolean val) {
      if (val) {
         freeze_b.setLabel("UN-FREEZE");
         freeze_b.setBackground(new Color(255,0,0));
      }
      else {
         freeze_b.setLabel("  FREEZE ");
         freeze_b.setBackground(new Color(0,255,0));
      }
   }//setFrozen

}//MSButtons
