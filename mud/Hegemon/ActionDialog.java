// $Id$
// $Revision$  $Author$ $Date$

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
import javax.swing.*;

/**
 * Class to create and edit Actions. Extends JFrame and pops up from ActionManager.
 */
class ActionDialog extends JFrame {
   LabeledTextArea action;
   LabeledTextField trigger;
   Checkbox anchored;
   ActionManager parent;
   ADButtons buttons;
   
   /**
    * Creates a new instance of ActionDialog for use in ActionManager.
    *
    * @param par ActionManger who holds this class
    * @param a Action to be edited (or new one to be created)
    */
   public ActionDialog(ActionManager par, Action a) {
      super("Action Dialog");
      parent = par;

      action = new LabeledTextArea("Result", a.getAction(), 4, 80);
      trigger = new LabeledTextField("Trigger", a.getTrigger(), 80);
      anchored = new Checkbox("Anchored", a.getAnchored());
      
      
      /* GridBagLayout formatting and adding: */
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
   }//ActionDialog constructor

   /**
    * Commit the changes and exit the ActionDialog JFrame.
    */
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

/**
 * Class to hold and handle ActionDialog's JButtons.
 */
class ADButtons extends JPanel implements ActionListener {
   ActionDialog parent;
   JButton done;
   
   /**
    * Creates a new instance of ADButtons for holding and handling ActionDialog's JButtons.
    */
   public ADButtons(ActionDialog par) {
      super();
      parent = par;

      done = new JButton("Done");
      done.addActionListener(this);
      setLayout(new FlowLayout());
      
      add(done);
   }//constructor
   
   /**
    * Method to handle ActionEvents from done button.
    *
    * @param e ActionEvent that should come from done button being pressed
    */
   public void actionPerformed(ActionEvent e) {
       if (e.getSource() == done) {
           parent.do_done();
       }
   }
}//ADButtons

      
