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
   LabeledTextField trigger;
   LabeledTextField discrim;
   LabeledTextField target;
   LabeledTextField actor;
   LabeledTextField precedence;
   LabeledTextArea script;
   LabeledTextField mob_num;
   Checkbox is_frozen;
   HegemonManager hm;

   public MobScriptEditor(HegemonManager h) {
      super("Mob Script Editor");
      hm = h;
      Log.it("Constructing MobScriptEditor..");

      is_frozen = new Checkbox("Is Frozen", false);
      trigger = new LabeledTextField("Trigger Command", "", 20);
      discrim = new LabeledTextField("Optional Discriminator", "NA", 20);
      target = new LabeledTextField("Target #", "", 10);
      actor = new LabeledTextField("Actor #", "", 10);
      precedence = new LabeledTextField("Precedence (0 no, other yes)",
                                        "", 10);
      script = new LabeledTextArea("Mob Script (double semicolons necessaary)",
                                   "", 10, 80);
      mob_num = new LabeledTextField("Mob # (Script Owner)", "0", 10);

      int REM = GridBagConstraints.REMAINDER;
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);

      c.fill = GridBagConstraints.BOTH;

      c.gridwidth = 1;
      
      gridbag.setConstraints(trigger, c);
      add(trigger);

      gridbag.setConstraints(mob_num, c);
      add(mob_num);

      c.gridwidth = REM;
      gridbag.setConstraints(discrim, c);
      add(discrim);

      c.gridwidth = 1;
      gridbag.setConstraints(actor, c);
      add(actor);

      gridbag.setConstraints(is_frozen, c);
      add(is_frozen);

      gridbag.setConstraints(target, c);
      add(target);
      
      c.gridwidth = REM;
      gridbag.setConstraints(precedence, c);
      add(precedence);
      
      gridbag.setConstraints(script, c);
      add(script);

      MSButtons oeb = new MSButtons(this);
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
      return is_frozen.getState();
   }
   
   public void do_close() {
      setVisible(false);
   }

   public void do_help() {
      if (!hm.IS_APPLET)
        hm.getHelpFrame().show("client" + System.getProperty("file.separator")
                               + "script_editor");
      else {
         Log.it("Trying to show: client/script_editor");
         hm.getHelpFrame().show("client/script_editor");
      }
   }//doHelp
   
   public void setMobScriptDiscrim(String d) {
      if (isFrozen())
        return;

      discrim.append(d);
   }

   public void setMobScriptData(String trig, String mnum, String actor_num,
                                String target_num, String pd) {
      if (isFrozen())
        return;

      mob_num.append(mnum);
      trigger.append(trig);
      actor.append(actor_num);
      target.append(target_num);
      precedence.append(pd);
   }

   public void appendScript(String str) {
      if (isFrozen())
         return;
      script.append(str);
   }

   public void do_clear() {
      trigger.clear();
      discrim.clear();
      is_frozen.setState(false);
      target.clear();
      actor.clear();
      precedence.clear();
      script.clear();
      mob_num.clear();
   }//

   public void do_update() {
      is_frozen.setState(false);

      try {
         // first, the in_room_description
         hm.getSocketManager().write("add_mob_script " + mob_num.getText()
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
                                              "Could not update mob script: "
                                              + e, "red", "black");
      }//catch
      //do_clear();
   }//do_update

}//MobScriptEditor


class MSButtons extends Panel {
   MobScriptEditor parent;
   
   public MSButtons(MobScriptEditor par) {
      super();
      parent = par;

      Button clear_b = new Button("Clear");
      Button cancel_b = new Button("Cancel");
      Button update_b = new Button("Update");
      Button done_b = new Button("Done");
      Button help_b = new Button("Help");

      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_close();
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

      update_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_update();
         }});

      FlowLayout fl = new FlowLayout();
      setLayout(fl);

      add(update_b);
      add(cancel_b);
      add(clear_b);
      add(done_b);
      add(help_b);
   }//MSButtons constructor
}//MSButtons
