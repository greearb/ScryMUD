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


class MessageDialog extends Frame {
  TextArea msg;
  
  public MessageDialog(String title, String message, String fg_color,
                       String bg_color) {

    super(title);

    msg = new TextArea(message, (int)(message.length() / 50) + 3,
                       50, TextArea.SCROLLBARS_NONE);

    setLayout(new BorderLayout());

    this.add(msg, "North");
    Button ok = new Button("ok");
    this.add(ok, "South");

    ok.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	do_ok();
      }});
    
    this.pack();
    this.setLocation(100, 100);

    this.show();
    try {
       msg.setBackground(ColorUtils.getColor(bg_color));
       msg.setForeground(ColorUtils.getColor(fg_color));
    }
    catch (ColorNameException e) {
       Log.instance().err("In MessageDialog constructor:  " + e);
    }
  }//constructor


  void do_ok() {
    dispose();
  }

}//MessageDialog

      
