// $Id: LabeledTextArea.java,v 1.3 1999/06/05 23:29:12 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date: 1999/06/05 23:29:12 $

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


class LabeledTextArea extends Panel {
   String text;
   Label lb;
   TextArea txt;
   
   public LabeledTextArea(String lbl, String msg, int rows,
                          int cols) {
      super ();
      
      setLayout (new BorderLayout());
      lb = new Label(lbl);
      add (lb, "North");
      txt = new TextArea(msg, rows, cols, TextArea.SCROLLBARS_VERTICAL_ONLY);
      add (txt, "Center");
   }//constructor

   public TextArea getTextArea() {
      return txt;
   }

   public String getText() {
      if (txt == null)
        return " ";
      else
        return txt.getText();
   }

   public void append(String str) {
      txt.append(str);
   }

   public void clear() {
      txt.setText("");
   }

}//LabeledTextArea
      


