// $Id: ActionDialog.java,v 1.3 2001/03/31 07:05:58 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date$

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


class ActionDialog extends Frame {
   LabeledTextArea action;
   LabeledTextField trigger;
   Checkbox anchored;
   ActionFrame parent;
   ADButtons buttons;
   
   public ActionDialog(ActionFrame par, Action a) {
      super("Action Dialog");
      parent = par;

      action = new LabeledTextArea("Result", a.getAction(), 4, 80);
      trigger = new LabeledTextField("Trigger", a.getTrigger(), 80);
      anchored = new Checkbox("Anchored", a.getAnchored());
      
      buttons = new ADButtons(this);
      int REM = GridBagConstraints.REMAINDER;

      GridBagConstraints c = new GridBagConstraints();
      GridBagLayout gridbag = new GridBagLayout();
      setLayout(gridbag);
      
      c.fill = GridBagConstraints.BOTH;
      c.weightx = c.weighty = 0.0;
      c.gridwidth = REM;

      gridbag.setConstraints(trigger, c);
      add(trigger);

      gridbag.setConstraints(action, c);
      add(action);
      
      c.fill = GridBagConstraints.NONE;
      gridbag.setConstraints(anchored, c);
      add(anchored);

      c.fill = GridBagConstraints.BOTH;
      gridbag.setConstraints(buttons, c);
      add(buttons);
      
      pack();
      setLocation(100, 100);
      show();
   }

   void do_done() {
      if (anchored.getState()) {
         parent.addAction("^" + trigger.getText() + " | " + action.getText());
      }
      else {
         parent.addAction(trigger.getText() + " | " + action.getText());
      }
      dispose();
   }//do_done
}//ActionDialog

class ADButtons extends Panel {
   ActionDialog parent;
   
   public ADButtons(ActionDialog par) {
      super();
      parent = par;

      Button done = new Button("Done");
      setLayout(new FlowLayout());
      
      done.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_done();
         }});
      
      add(done);
   }//constructor
}//ADButtons

      
