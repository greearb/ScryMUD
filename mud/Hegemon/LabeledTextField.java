// $Id: LabeledTextField.java,v 1.3 2001/03/31 07:05:59 greear Exp $
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


class LabeledTextField extends Panel {
   String text;
   Label lb;
   TextField txt;
   int len;
   
   public LabeledTextField (String lbl, String msg, int length) {
      super ();
      len = length;
      setLayout (new BorderLayout());
      lb = new Label(lbl);
      add (lb, "North");
      txt = new TextField(msg, len);
      add (txt, "South");
   }//constructor

   String getText() {
      if (txt == null)
        return "";
      else
        return txt.getText();
   }

   public void setText(String str) {
      txt.setText(str);
      //      remove(txt);
      //      txt = new TextField(str, len);
      //     add(txt, "South");
   }
   
   public void setEditable(boolean tf) {
      txt.setEditable(tf);
   }

   public void append(String str) {
      txt.setText(txt.getText() + str);
   }

   public void clear() {
      txt.setText("");
   }

   
}//LabeledTextField
      



