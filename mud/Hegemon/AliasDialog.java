// $Id$
// $Revision: 1.3 $  $Author$ $Date$

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


class AliasDialog extends Frame {
   LabeledTextArea action;
   LabeledTextField trigger;
   AliasFrame parent;
   AlDButtons buttons;
   
   public AliasDialog(AliasFrame par, Alias a) {
      super("Alias Dialog");
      parent = par;

      action = new LabeledTextArea("Translates To", a.getResponse(), 4, 80);
      trigger = new LabeledTextField("Alias", a.getAlias(), 80);
      
      buttons = new AlDButtons(this);
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
      
      c.fill = GridBagConstraints.BOTH;
      gridbag.setConstraints(buttons, c);
      add(buttons);
      
      pack();
      setLocation(100, 100);
      show();
   }

   void do_done() {
      parent.addAlias(trigger.getText() + " | " + action.getText());
      dispose();
   }//do_done
}//ActionDialog

class AlDButtons extends Panel {
   AliasDialog parent;
   
   public AlDButtons(AliasDialog par) {
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

      
