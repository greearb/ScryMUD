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
import java.util.*;
import java.awt.event.*;

class LabeledChoice extends Panel {
   public Label lbl;
   public Choice c;
   
   public LabeledChoice(String name) {
      super();

      lbl = new Label(name);
      c = new Choice();
    
      BorderLayout bl = new BorderLayout();
      setLayout(bl);

      add(lbl, "North");
      add(c, "South");
   }//constructor

   String getText() {
      return c.getSelectedItem();
   }

}//LabeledChoice
