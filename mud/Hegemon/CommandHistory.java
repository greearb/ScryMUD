// $Id: CommandHistory.java,v 1.3 2001/03/31 07:05:59 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date: 2001/03/31 07:05:59 $

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
import java.util.*;

public class CommandHistory extends Frame {
   HegemonManager hm;
   TextArea ta;
   CHButtons sb;
   Vector cmds;
   int cmd_offset;

   public CommandHistory(HegemonManager h) {
      super("Command History");
      hm = h;
      ta = new TextArea("", 10, 80, TextArea.SCROLLBARS_BOTH);
      cmds = new Vector(100);
      cmd_offset = 0;

      add("Center", ta);

      sb = new CHButtons(this);
      add("South", sb);

      pack();
   }//constructor

   public void do_clear() {
      ta.setText("");
      cmds = null;
      cmds = new Vector(100);
   }

   public void do_help() {
      new MessageDialog("Help for Command History",
                        "This area will store previous commands that you have"
                        + " entered.  You can cut and paste to easily modify"
                        + " and change previous commands.", "blue", "white");
   }

   public void do_close() {
      setVisible(false);
   }

   public void append(String txt) {
      ta.append(txt);
      cmds.addElement(txt);
      cmd_offset = cmds.size() - 1;
   }

   public int getCmdOffset() {
      return cmd_offset;
   }

   public String getPreviousCmd() {
      String retval = null;

      if (cmd_offset >= 0) {
         retval = (String)(cmds.elementAt(cmd_offset));
         cmd_offset--;
      }//if

      if (retval == null) {
         return "";
      }
      else {
         return retval;
      }
   }//getPreviousCmd

   public String getNextCmd() {
      String retval = null;
      if (cmd_offset + 1 < cmds.size()) {
         cmd_offset++;
         retval = (String)(cmds.elementAt(cmd_offset));
      }//if

      if (retval == null) {
         return "";
      }
      else {
         return retval;
      }
   }//getNextCmd

}//CommandHistory


class CHButtons extends Panel {
   CommandHistory parent;
   
   public CHButtons(CommandHistory par) {
      super();
      parent = par;

      Button close_b = new Button("Done");
      Button clear_b = new Button("Clear");
      Button help_b = new Button("Help");

      close_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_close();
         }});

      clear_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_clear();
         }});

      help_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_help();
         }});

      FlowLayout fl = new FlowLayout();
      setLayout(fl);

      add(close_b);
      add(clear_b);
      add(help_b);
   }//CHButtons constructor
}//CHButtons
