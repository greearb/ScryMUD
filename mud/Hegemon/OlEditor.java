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

class OlEditor extends Frame {
   LabeledTextArea long_desc;
   LabeledTextField short_desc;
   LabeledTextField in_room_desc;
   LabeledTextField obj_name;
   LabeledTextField frozen_field;
   LabeledTextField obj_num_field;
   LabeledTextField type_field;
   
   int object_number;

   int obj_type;
   static public int MOB = 0;
   static public int OBJ = 1;
   static public int ROOM = 2;

   boolean is_frozen;
   HegemonManager hm;
   
   public OlEditor(HegemonManager h) {
      super("Online Editor");
      hm = h;

      object_number = -1;
      obj_type = -1;
      is_frozen = false;

      type_field = new LabeledTextField("Object Type", "", 20);
      frozen_field = new LabeledTextField("Frozen Status", "UN_FROZEN", 20);
      obj_num_field = new LabeledTextField("Number", "", 8);
      long_desc = new LabeledTextArea("Long Description", "", 10, 80);
      short_desc = new LabeledTextField("Short Description", "", 50);
      in_room_desc = new LabeledTextField("In Room Description", "", 80);

      int REM = GridBagConstraints.REMAINDER;
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);

      c.fill = GridBagConstraints.BOTH;

      c.gridwidth = 1;
      
      gridbag.setConstraints(obj_num_field, c);
      add(obj_num_field);

      gridbag.setConstraints(type_field, c);
      add(type_field);

      c.gridwidth = REM;

      gridbag.setConstraints(frozen_field, c);
      add(frozen_field);

      gridbag.setConstraints(short_desc, c);
      add(short_desc);
      
      gridbag.setConstraints(in_room_desc, c);
      add(in_room_desc);
      
      gridbag.setConstraints(long_desc, c);
      add(long_desc);

      OEButtons oeb = new OEButtons(this);
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
      return is_frozen;
   }
   
   public void do_close() {
      setVisible(false);
   }

   public void do_help() {
      if (!hm.IS_APPLET)
        hm.getHelpFrame().show("client" + System.getProperty("file.separator")
                               + "oleditor");
      else {
         Log.it("Trying to show: client/oleditor");
         hm.getHelpFrame().show("client/oleditor");
      }
   }//doHelp
   
   public void do_freeze() {
      if (is_frozen) {
         is_frozen = false;
         frozen_field.setText("UN_FROZEN");
      }
      else {
         is_frozen = true;
         frozen_field.setText("FROZEN");
      }
   }
   
   public void setObjNum(int obj_num) {
      if (is_frozen)
         return;
      obj_num_field.setText(Integer.toString(obj_num));
      object_number = obj_num;
   }

   public void setObjType(int object_type) {
      if (is_frozen)
         return;
      if (object_type == ROOM)
        type_field.setText("Room");
      else if (object_type == MOB)
        type_field.setText("Mobile");
      else if (object_type == OBJ)
        type_field.setText("Object");
      else
        type_field.setText("UNKNOWN");
      obj_type = object_type;
   }

   public void appendLongDesc(String str) {
      if (is_frozen)
         return;
      long_desc.append(str);
   }

   public void appendShortDesc(String str) {
      if (is_frozen)
         return;
      short_desc.append(str);
   }

   public void appendInRoomDesc(String str) {
      if (is_frozen)
         return;
      in_room_desc.append(str);
   }
   
   public void do_clear() {
      long_desc.clear();
      short_desc.clear();
      in_room_desc.clear();
      object_number = -1;
      obj_type = -1;
      is_frozen = false;
   }//

   public void do_update() {
      is_frozen = false;
      hm.getClientDisplay().giveFocus();
      // first, figure out which type of object we're dealing with
      if ((object_number >= 0) || (obj_type == ROOM)) {
         if (obj_type == MOB) {
            // then we're good to go
            try {
               // first, the in_room_description
               hm.getSocketManager().write("ch_mndesc " + object_number
                                           + " '" + in_room_desc.getText()
                                           + "'\n");
               // now the short desc
               hm.getSocketManager().write("ch_msdesc " + object_number
                                           + " '" + short_desc.getText()
                                           + "'\n");

               // now the long description
               hm.getSocketManager().write("ch_mdesc " + object_number
                                           + "\n");
               hm.getSocketManager().write(long_desc.getText() + "\n~\n");
            }//try
            catch (Exception e) {
               MessageDialog md = new MessageDialog("Update Error",
                                                    "Could not update mob: "
                                                    + e, "red", "black");
            }//catch
         }//if
         else if (obj_type == OBJ) {
            try {
               // first, the in_room_description
               hm.getSocketManager().write("ch_ondesc " + object_number
                                           + " '" + in_room_desc.getText()
                                           + "'\n");
               // now the short desc
               hm.getSocketManager().write("ch_osdesc " + object_number
                                           + " '" + short_desc.getText()
                                           + "'\n");

               // now the long description
               hm.getSocketManager().write("ch_odesc " + object_number
                                           + "\n");
               hm.getSocketManager().write(long_desc.getText() + "\n~\n");
            }//try
            catch (Exception e) {
               MessageDialog md = new MessageDialog("Update Error",
                                                    "Could not update object: "
                                                    + e, "red", "black");
            }//catch
         }//if
         else if (obj_type == ROOM) {
            try {
               // now the short desc
               hm.getSocketManager().write("ch_rname "
                                           + "'" + short_desc.getText()
                                           + "'\n");

               // now the long description
               hm.getSocketManager().write("ch_rdesc\n");
               
               hm.getSocketManager().write(long_desc.getText() + "\n~\n");
            }//try
            catch (Exception e) {
               MessageDialog md = new MessageDialog("Update Error",
                                                    "Could not update object: "
                                                    + e, "red", "black");
            }//catch
         }//if
            
      }//if obj_type is in range
      else {
         MessageDialog md = new MessageDialog("Update Error",
                                              "Object number is invalid.",
                                              "red", "black");
      }//else

      do_clear();
   }//do_update
}//OlEditor


class OEButtons extends Panel {
   OlEditor parent;
   
   public OEButtons(OlEditor par) {
      super();
      parent = par;

      Button freeze_b = new Button("Freeze/Unfreeze");
      Button cancel_b = new Button("Cancel");
      Button update_b = new Button("Update");
      Button done_b = new Button("Done");
      Button help_b = new Button("Help");

      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_close();
         }});

      freeze_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_freeze();
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

      add(freeze_b);
      add(update_b);
      add(cancel_b);
      add(done_b);
      add(help_b);
   }//OEButtons constructor
}//OEButtons
