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

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;

/** This allows for some quick editing of already-created 
 * Keyword entries.
 * Or you can create your own if you know what you're doing!
 */
class KeywordEditor extends Frame {
   LabeledTextArea desc;
   LabeledTextField names;
   LabeledTextField cell_num;
   Checkbox is_frozen;

   HegemonManager hm;
   
   public KeywordEditor(HegemonManager h) {
      super("Keyword Editor");
      hm = h;

      desc = new LabeledTextArea("Keyword Description", "", 10, 80);
      cell_num = new LabeledTextField("Keyword Number", "", 10);
      names = new LabeledTextField("Keyword Name(s)", "", 60);
      is_frozen = new Checkbox("Is Frozen", false);

      int REM = GridBagConstraints.REMAINDER;
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);

      c.fill = GridBagConstraints.BOTH;

      c.gridwidth = 1;
      
      gridbag.setConstraints(cell_num, c);
      add(cell_num);

      gridbag.setConstraints(is_frozen, c);
      add(is_frozen);

      c.gridwidth = REM;

      gridbag.setConstraints(names, c);
      add(names);

      gridbag.setConstraints(desc, c);
      add(desc);

      KEButtons oeb = new KEButtons(this);
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

   public void do_help() {
      if (!hm.IS_APPLET)
        hm.getHelpFrame().show("client" + System.getProperty("file.separator")
                               + "keditor");
      else {
         Log.instance().dbg("Trying to show: client/keditor");
         hm.getHelpFrame().show("client/keditor");
      }
   }//doHelp
   
   public void do_toggle_freeze() {
      is_frozen.setState(!is_frozen.getState());
   }
   
   public void setCellNum(String num) {
      if (is_frozen.getState())
         return;
      cell_num.setText(num);
   }

   public void setNames(String str) {
      if (is_frozen.getState())
         return;
      names.setText(str);
   }

   public void appendDesc(String str) {
      if (is_frozen.getState())
         return;
      desc.append(str);
   }

   public void do_clear() {
      desc.clear();
      names.clear();
      cell_num.clear();
      is_frozen.setState(false);
   }//

   public void do_update() {
      is_frozen.setState(false);

      try {
         //first, clear what was there already...
         hm.getSocketManager().write("clear_keyword " + cell_num.getText()
                                     + "\n");

         // now, add names...
         StringTokenizer tok = new StringTokenizer(names.getText());
         while (tok.hasMoreElements()) {
            hm.getSocketManager().write("add_kname " + cell_num.getText()
                                        + " " + tok.nextToken() + "\n");
         }//while

         // now change the description
         hm.getSocketManager().write("ch_kdesc " + cell_num.getText() 
                                     + "\n");
         hm.getSocketManager().write(desc.getText() + "\n~\n");
         
      }//try
      catch (Exception e) {
         e.printStackTrace();
      }

      //do_clear();
   }//do_update
}//OlEditor


class KEButtons extends Panel {
   KeywordEditor parent;
   
   public KEButtons(KeywordEditor par) {
      super();
      parent = par;

      Button cancel_b = new Button("Cancel");
      Button update_b = new Button("Update");
      Button done_b = new Button("Done");
      Button help_b = new Button("Help");

      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_close();
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
      add(done_b);
      add(help_b);
   }//OEButtons constructor
}//OEButtons
