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

// This allows for some quick editing of already-created path cells,
// Or you can create your own if you know what you're doing!
class PathCellEditor extends Frame {
   LabeledTextArea long_desc;
   LabeledTextField dir_name;
   LabeledTextField dir_num;
   LabeledTextField veh_num;
   LabeledTextField cell_num;
   Checkbox is_frozen;
   Checkbox is_dest;

   HegemonManager hm;
   
   public PathCellEditor(HegemonManager h) {
      super("Online Vehicle Path Editor");
      hm = h;

      dir_name = new LabeledTextField("Direction Name", "", 20);
      dir_num = new LabeledTextField("Direction Num", "", 20);
      long_desc = new LabeledTextArea("Path Cell Description", "", 10, 80);
      veh_num = new LabeledTextField("Vehicle Number", "", 10);
      cell_num = new LabeledTextField("Path Cell Number", "", 10);
      is_frozen = new Checkbox("Is Frozen", false);
      is_dest = new Checkbox("Is Destination", false);

      int REM = GridBagConstraints.REMAINDER;
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);

      c.fill = GridBagConstraints.BOTH;

      c.gridwidth = 1;
      
      gridbag.setConstraints(veh_num, c);
      add(veh_num);

      gridbag.setConstraints(cell_num, c);
      add(cell_num);

      gridbag.setConstraints(is_frozen, c);
      add(is_frozen);

      c.gridwidth = REM;

      gridbag.setConstraints(is_dest, c);
      add(is_dest);
      
      c.gridwidth = 1;

      gridbag.setConstraints(dir_num, c);
      add(dir_num);

      c.gridwidth = REM;
      gridbag.setConstraints(dir_name, c);
      add(dir_name);
      
      gridbag.setConstraints(long_desc, c);
      add(long_desc);

      PCEButtons oeb = new PCEButtons(this);
      gridbag.setConstraints(oeb, c);
      add(oeb);
      pack();
   }//constructor

   public boolean isFrozen() {
      return is_frozen.getState();
   }
   
   public void do_close() {
      setVisible(false);
   }

   public void setIsDest(String str) {
      if (str.equals("0")) {
         is_dest.setState(false);
      }
      else {
         is_dest.setState(true);
      }
   }//setIsDest


   public void do_help() {
      if (!hm.IS_APPLET)
        hm.getHelpFrame().show("client" + System.getProperty("file.separator")
                               + "pceditor");
      else {
         Log.instance().dbg("Trying to show: client/pceditor");
         hm.getHelpFrame().show("client/pceditor");
      }
   }//doHelp
   
   public void do_toggle_freeze() {
      is_frozen.setState(!is_frozen.getState());
   }
   
   public void setDir(String i_th, String dir) {
      if (is_frozen.getState())
         return;
      dir_num.setText(i_th);
      dir_name.setText(dir);
   }

   public void setVehNum(String num) {
      if (is_frozen.getState())
         return;
      veh_num.setText(num);
   }

   public void setCellNum(String num) {
      if (is_frozen.getState())
         return;
      cell_num.setText(num);
   }

   public void appendLongDesc(String str) {
      if (is_frozen.getState())
         return;
      long_desc.append(str);
   }

   public void do_clear() {
      long_desc.clear();
      dir_name.clear();
      dir_num.setText("1");
      veh_num.clear();
      cell_num.clear();
      is_frozen.setState(false);
      is_dest.setState(false);
   }//

   public void do_update() {
      is_frozen.setState(false);

      //first, set the state
      try {
         if (is_dest.getState()) {
            hm.getSocketManager().write("set_veh_stop " + veh_num.getText()
                                        + " " + cell_num.getText() + " yes\n");
         }
         else {
            hm.getSocketManager().write("set_veh_stop " + veh_num.getText()
                                        + " " + cell_num.getText() + " no\n");
         }
         
         //now, long desc
         
         //dir name and num
         hm.getSocketManager().write("set_path_dir " + veh_num.getText()
                                     + " " + cell_num.getText() + " "
                                     + dir_num.getText() + " '"
                                     + dir_name.getText() + "'\n"); 

         hm.getSocketManager().write("ch_path_desc " + veh_num.getText()
                                     + " " + cell_num.getText() + "\n");
         hm.getSocketManager().write(long_desc.getText() + "\n~\n");
         
         
      }//try
      catch (Exception e) {
         e.printStackTrace();
      }

      //do_clear();
   }//do_update
}//OlEditor


class PCEButtons extends Panel {
   PathCellEditor parent;
   
   public PCEButtons(PathCellEditor par) {
      super();
      parent = par;

      //Button freeze_b = new Button("Freeze/Unfreeze");
      Button cancel_b = new Button("Cancel");
      Button update_b = new Button("Update");
      Button done_b = new Button("Done");
      Button help_b = new Button("Help");

      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_close();
         }});

      //freeze_b.addActionListener(new ActionListener() {
      //   public void actionPerformed(ActionEvent e) {
      //      parent.do_toggle_freeze();
      //   }});

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

      //add(freeze_b);
      add(update_b);
      add(cancel_b);
      add(done_b);
      add(help_b);
   }//OEButtons constructor
}//OEButtons
