// $Id: PsoEditor.java,v 1.4 2001/03/31 07:06:00 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date: 2001/03/31 07:06:00 $

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

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;


/**  Player Shop Owner editor.  This is used for player-run shop
 * manipulation.
 */
class PsoEditor extends Frame {
   LabeledTextField i_th;
   LabeledTextField keeper;
   TextField register;
   LabeledTextField new_obj;
   Button reload_b, new_b, update_reg_b, done_b;

   GridBagLayout gridbag;
   GridBagConstraints c;

   Vector elements; // holds all of the individual value collections

   HegemonManager hm;

   public String getIth() { return i_th.getText(); }
   public String getKeeperName() { return keeper.getText(); }
   
   public PsoEditor(HegemonManager h) {
      super("Player-run Shop Editor");
      hm = h;

      i_th = new LabeledTextField("i_th", "", 5);
      keeper = new LabeledTextField("Keeper Name", "", 10);
      register = new TextField("", 10);
      new_obj = new LabeledTextField("New Obj Name", "", 15);
      elements = new Vector();
      reload_b = new Button("Re-list");
      new_b = new Button("New Entry");
      done_b = new Button("Done");

      update_reg_b = new Button("Update Register");

      Panel reg_p = new Panel();
      reg_p.setLayout(new BorderLayout());
      reg_p.add("North", update_reg_b);
      reg_p.add("South", register);
      reg_p.setBackground(new Color(50,255,50));
      register.setBackground(new Color(255,255,255));
      update_reg_b.setBackground(new Color(130,130,255)); //blue

      Panel p = new Panel();
      p.setLayout(new BorderLayout());

      reload_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_relist();
         }});

      new_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_new();
         }});

      update_reg_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_update_reg();
         }});

      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            setVisible(false);
         }});

      int REM = GridBagConstraints.REMAINDER;
      gridbag = new GridBagLayout();
      c = new GridBagConstraints();
      setLayout (gridbag);

      c.fill = GridBagConstraints.HORIZONTAL;

      c.gridwidth = 1;
      
      gridbag.setConstraints(i_th, c);
      add(i_th);

      gridbag.setConstraints(keeper, c);
      add(keeper);

      gridbag.setConstraints(reg_p, c);
      add(reg_p);

      gridbag.setConstraints(new_obj, c);
      add(new_obj);

      c.gridwidth = REM;
      Panel p_top = new Panel();
      p_top.add(reload_b);
      p_top.add(done_b);
      
      p.add("North", new_b);
      p.add("South", p_top);
      gridbag.setConstraints(p, c);
      add(p);


      Panel label_p = new Panel();
      label_p.setLayout(new FlowLayout());

      Label l = new Label("Index");
      label_p.add(l);

      l = new Label("Object#  ");
      label_p.add(l);

      l = new Label("Buy Value");
      label_p.add(l);

      l = new Label("Sell Value        ");
      label_p.add(l);

      l = new Label("             Name                          Control Buttons     ");
      label_p.add(l);


      gridbag.setConstraints(label_p, c);
      add(label_p);

      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            setVisible(false);
         }});

      pack();
   }//constructor


   void do_new() {
      try {
         hm.getSocketManager().write("value_add " + hm.getPsoEditor().getIth()
                                     + " " + hm.getPsoEditor().getKeeperName()
                                     + " " + new_obj.getText() + "\n");
      }//try
      catch (Exception e) {
         MessageDialog md = new MessageDialog("Network Error",
                                              "Could not add entry: "
                                              + e, "red", "black");
         e.printStackTrace();
      }//catch
      do_relist();
   }//do_new


   void do_update_reg() {
      try {
         hm.getSocketManager().write("adjust_register " + hm.getPsoEditor().getIth()
                                     + " " + hm.getPsoEditor().getKeeperName()
                                     + " " + register.getText() + "\n");
      }//try
      catch (Exception e) {
         MessageDialog md = new MessageDialog("Network Error",
                                              "Could not update register: "
                                              + e, "red", "black");
         e.printStackTrace();
      }//catch
      do_relist();
   }//do_update_reg
      
   public void do_relist() {
      try {
         hm.getSocketManager().write("value_list " + hm.getPsoEditor().getIth()
                                     + " " + hm.getPsoEditor().getKeeperName()
                                     + "\n");
      }//try
      catch (Exception e) {
         MessageDialog md = new MessageDialog("Network Error",
                                              "Could not refresh: "
                                              + e, "red", "black");
         e.printStackTrace();
      }//catch
   }

//    public void do_help() {
//       if (!hm.IS_APPLET)
//         hm.getHelpFrame().show("client" + System.getProperty("file.separator")
//                                + "oleditor");
//       else {
//          Log.instance().dbg("Trying to show: client/oleditor");
//          hm.getHelpFrame().show("client/oleditor");
//       }
//    }//doHelp
   

   public void setList(String _i_th, String nm, String reg) {
      Log.instance().dbg("setList: " + _i_th + " " + nm);

      clear();
      i_th.setText(_i_th);
      keeper.setText(nm);
      register.setText(reg);
   }//setNames

   public void setItem(String idx, String obj_num, String sell_val,
                       String buy_val) {
      ValueEntry ve = new ValueEntry(idx, obj_num, buy_val, sell_val, hm);
      gridbag.setConstraints(ve, c);
      add(ve);
      elements.addElement(ve);
      pack();
   }

   public void setName(String idx, String val) {
      Log.instance().dbg("In PsoEditor.setName idx -:" + idx + ":- val -:"
                         + val + ":-\n");
      ValueEntry ve;
      for (int i = 0; i<elements.size(); i++) {
         ve = (ValueEntry)(elements.elementAt(i));
         if (idx.equals(ve.getIndex())) {
            ve.setObjName(val);
            return;
         }//if
      }//for
      Log.instance().err("Could not find index in setName.");
      return;
   }//setName
   
   public void clear() {
      i_th.clear();
      keeper.clear();

      for (int i = 0; i<elements.size(); i++) {
         remove((ValueEntry)(elements.elementAt(i)));
      }//for

      elements = new Vector();
   }//
}//PsoEditor

class ValueEntry extends Panel {
   TextField idx;
   TextField obj_num;
   TextField buy_val;
   TextField sell_val;
   TextField name;
   Button update_b;
   Button delete_b;
   HegemonManager hm;

   public String getIndex() { return idx.getText(); }

   public void setObjName(String str) {
      name.setText(str);
   }

   public ValueEntry(String i, String o, String b, String s,
                     HegemonManager h) {
      hm = h;

      setLayout(new FlowLayout());
      add(idx = new TextField(i, 3));
      add(obj_num = new TextField(o, 7));
      add(buy_val = new TextField(b, 7));
      add(sell_val = new TextField(s, 7));
      add(name = new TextField("", 24));

//       idx.setEnabled(false);
//       obj_num.setEnabled(false);
//       name.setEnabled(false);

      update_b = new Button("Update");
      update_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_update();
         }});
      add(update_b);

      delete_b = new Button("Delete");
      delete_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_delete();
         }});
      add(delete_b);
   }//constructor

   public void do_update() {
      try {
         hm.getSocketManager().write("value_set " + hm.getPsoEditor().getIth()
                                     + " " + hm.getPsoEditor().getKeeperName()
                                     + " " + idx.getText() 
                                     + " " + sell_val.getText()
                                     + " " + buy_val.getText() + "\n");
      }//try
      catch (Exception e) {
         MessageDialog md = new MessageDialog("Network Error",
                                              "Could not update entry: "
                                              + e, "red", "black");
         e.printStackTrace();
      }//catch
   }//do_update

   public void do_delete() {
      try {
         hm.getSocketManager().write("value_rem " + hm.getPsoEditor().getIth()
                                     + " " + hm.getPsoEditor().getKeeperName()
                                     + " " + idx.getText() + "\n");
         // Update ourselves....
         hm.getPsoEditor().do_relist();
      }//try
      catch (Exception e) {
         MessageDialog md = new MessageDialog("Network Error",
                                              "Could not delete entry: "
                                              + e, "red", "black");
         e.printStackTrace();
      }//catch
   }//do_delete

}//ValueEntry

