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

/**
 * GUI for connecting to certain host.
 */
class HostDialog extends Frame {
  ConnectionManager parent;
  LabeledTextField mud_name;
  LabeledTextField host_name;
  LabeledTextField port;
  
  public HostDialog(ConnectionManager par, HostAddr ha) {

    super("Chose a Server.");

    parent = par;
    mud_name = new LabeledTextField("Mud Name", ha.getMudName(), 50);
    host_name = new LabeledTextField("Host Name", ha.getHostName(), 50);
    port = new LabeledTextField("Port Number",
                                Integer.toString(ha.getPortNum()), 5);

    HDButtonPanel bp = new HDButtonPanel(this);

    BorderLayout bl = new BorderLayout();
    setLayout (bl);

    this.add(mud_name, "North");
    this.add(host_name, "Center");
    this.add(port, "East");
    this.add(bp, "South");
    this.pack();
  }//constructor


  void do_cancel() {
    dispose();
  }

  void do_select() {
    Integer myInt = new Integer(port.getText());
    int nt = myInt.intValue();
    parent.addHost(mud_name.getText() + " | " + host_name.getText() +
                   " | " + nt);
    dispose();
  }

}//HostDialog

      

class HDButtonPanel extends Panel {
  HostDialog parent;

  public HDButtonPanel(HostDialog par) {
    super();
    parent = par;

    Button cancel = new Button("Cancel");
    Button select = new Button("Add");
    
    FlowLayout fl = new FlowLayout();
    setLayout(fl);

    cancel.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.do_cancel();
      }});

    select.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.do_select();
      }});

    add(select);
    add(cancel);

  }//constructor
}//HDButtonPanel


