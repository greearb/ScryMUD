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


class OLCDoor extends Frame {
   ClientDisplay parent;
   
   LabeledCheckBoxGlob boxes;
   LabeledTextField names;
   LabeledTextArea desc;
   LabeledTextField token_num;
   LabeledTextField key_num;
   LabeledTextField vis_bit;
   HegemonManager hm;

   public OLCDoor(ClientDisplay par, HegemonManager h) {
      
      super("Create a Door.");
      hm = h;
      
      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            dispose();
         }});

      int REM = GridBagConstraints.REMAINDER;
      parent = par;
      boxes = new LabeledCheckBoxGlob("Door Flags", 3, 4);
      
      boxes.addBox(new NumCheckbox("Mag. Lockable", false, 1));
      boxes.addBox(new NumCheckbox("Closed", true, 2));
      boxes.addBox(new NumCheckbox("Locked", false, 3));
      boxes.addBox(new NumCheckbox("Pickable", false, 4));
      boxes.addBox(new NumCheckbox("Lockable", false, 5));
      boxes.addBox(new NumCheckbox("Mag. Locked", false, 6));
      boxes.addBox(new NumCheckbox("Destructable", false, 7));
      boxes.addBox(new NumCheckbox("Closeable", true, 8));
      boxes.addBox(new NumCheckbox("!open", false, 11));
      boxes.addBox(new NumCheckbox("Exit From Vehicle", false, 12));
      boxes.addBox(new NumCheckbox("Secret", false, 13));
      boxes.addBox(new NumCheckbox("Secret When Open", false, 16));
      boxes.addBox(new NumCheckbox("Consumes Key", false, 17));
      boxes.addBox(new NumCheckbox("!passdoor", false, 18));
      
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout(gridbag);
      
      c.fill = GridBagConstraints.BOTH;
      c.weightx = c.weighty = 0.0;
      
      c.gridwidth = REM;
      names =
        new LabeledTextField("Names: <dir name1 .. # .. name1 opp_dir>",
                             "", 50);
      gridbag.setConstraints(names, c);
      add(names);
      
      desc = new LabeledTextArea("Door Description", "", 5, 80);
      gridbag.setConstraints(desc, c);
      add(desc);
      
      gridbag.setConstraints(boxes, c);
      add(boxes);
      
      c.gridwidth = 50;
      vis_bit = new LabeledTextField("Visibility Bits:", "0", 15);
      gridbag.setConstraints(vis_bit, c);
      add(vis_bit);
      
      token_num = new LabeledTextField("Token Number:", "0", 15);
      gridbag.setConstraints(token_num, c);
      add(token_num);
      
      c.gridwidth = REM;
      key_num = new LabeledTextField("Key Number:", "0", 15);
      gridbag.setConstraints(key_num, c);
      add(key_num);
      
      DoorButtonPanel bp = new DoorButtonPanel(this);
      gridbag.setConstraints(bp, c);
      add(bp);
      
      this.pack();
   }//constructor
   
   void do_cancel() {
      dispose();
   }

   void do_help() {
      hm.getHelpFrame().show("olc_door.bml");
   }
   
   void do_submit() {

      MessageDialog md =
        new MessageDialog("Door Creation",
                          "The info is being sent to the server.  An error"
                          + " or confirmation notice should pop up soon..",
                          "green", "black");
      parent.submitDoor(this);
   }

   public final String getNames() {
      return names.getText() + " ~";
   }

   public final String getDesc() {
      return desc.getText() + "\n~";
   }

   public final String getFlags() {
      return boxes.getText() + " -1";
   }

  public final String getVisBit() {
     return vis_bit.getText();
  }

  public final String getKeyNum() {
     return key_num.getText();
  }

  public final String getTokenNum() {
     return token_num.getText();
  }
   
}//OLCDoor



class DoorButtonPanel extends Panel {
  OLCDoor parent;

  public DoorButtonPanel(OLCDoor par) {
    super();
    parent = par;

    Button cancel = new Button("Cancel");
    Button submit = new Button("Submit");
    Button help = new Button("Help");
    
    FlowLayout fl = new FlowLayout();
    setLayout(fl);

    cancel.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.do_cancel();
      }});

    help.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.do_help();
      }});

    submit.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.do_submit();
      }});

    add(submit);
    add(cancel);
    add(help);

  }//constructor
}//HDButtonPanel






