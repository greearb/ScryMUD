// $Id: LabeledCheckBoxGlob.java,v 1.3 2001/03/31 07:05:59 greear Exp $
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
import java.util.*;
import java.awt.event.*;

class LabeledCheckBoxGlob extends Panel {
  Label lbl;
  CheckBoxGlob cbg;
  ItemListener il;
   
  public LabeledCheckBoxGlob(String name, int rows, int cols) {
    super();

    il = null;
    lbl = new Label(name);
    cbg = new CheckBoxGlob(rows, cols);
    
    BorderLayout bl = new BorderLayout();
    setLayout(bl);

    add(lbl, "North");
    add(cbg, "South");
  }//constructor

  public void setEnabled(boolean state) {
     lbl.setEnabled(state);
     cbg.setEnabled(state);
     super.setEnabled(state);
  }
   
  public void setListener(ItemListener _il) {
     il = _il;
  }
   
  public void addBox(Checkbox cb) {
     if (il != null)
       cb.addItemListener(il);
     cbg.addBox(cb);
  }

  String getText() {
    return cbg.getText();
  }

   public boolean getState(int num) {
      return cbg.getState(num);
   }

   public void setState(int num, boolean state) {
      cbg.setState(num, state);
   }

   public void setStateRange(int beg, int end, boolean state) {
      cbg.setStateRange(beg, end, state);
   }

   public void setEnableRange(int beg, int end, boolean tf) {
      cbg.setEnableRange(beg, end, tf);
   }
   
}//LabeledCheckBoxGlob


class CheckBoxGlob extends Panel {
   Vector invisibles;
  
   public CheckBoxGlob(int rows, int cols) {
      super();
      GridLayout fl = new GridLayout(rows, cols, 0, 0);
      setLayout(fl);
      invisibles = new Vector();
   }
   
   public void addBox(Checkbox cb) {
      add(cb);
   }
   
   public void addBox(Checkbox cb, boolean invisible) {
      if (!invisible)
        add(cb);
      else
        invisibles.addElement(cb);
   }

   public void setEnabled(boolean state) {
      int cnt = getComponentCount();
      
      NumCheckbox cb;
      
      for (int i = 0; i < cnt; i++) {
         cb = (NumCheckbox)(getComponent(i));
         cb.setEnabled(state);
      }//for
      
      cnt = invisibles.size();
      for (int i = 0; i < cnt; i++) {
         cb = (NumCheckbox)(invisibles.elementAt(i));
         cb.setEnabled(state);
      }//for
      
   }//setEnabled
      
   
   public String getText() { /* get them all */
      int cnt = getComponentCount();
      
      String s;
      StringBuffer sb = new StringBuffer(100);
      NumCheckbox cb;
      
      for (int i = 0; i < cnt; i++) {
         cb = (NumCheckbox)(getComponent(i));
         if (cb.getState()) {
            sb.append(cb.getNum());
            sb.append(" ");
         }//if
      }//for
      
      cnt = invisibles.size();
      for (int i = 0; i < cnt; i++) {
         cb = (NumCheckbox)(invisibles.elementAt(i));
         if (cb.getState()) {
            sb.append(cb.getNum());
            sb.append(" ");
         }//if
      }//for
      
      return new String(sb);
   }//getText
   
   public NumCheckbox getBoxByNum(int num) {
      int cnt = getComponentCount();
      
      NumCheckbox cb;
      
      for (int i = 0; i < cnt; i++) {
         cb = (NumCheckbox)(getComponent(i));
         if (cb.getNum() == num) {
            return cb;
         }
      }//for
      
      cnt = invisibles.size();
      for (int i = 0; i < cnt; i++) {
         cb = (NumCheckbox)(invisibles.elementAt(i));
         if (cb.getNum() == num) {
            return cb;
         }
      }//for
      return null;
   }//getBoxByNum
   
   public boolean getState(int num) {
      NumCheckbox cb = getBoxByNum(num);
      if (cb != null)
        return cb.getState();
      else
        return false;
   }
   //HEERE
   
   public void setState(int num, boolean state) {
      NumCheckbox cb = getBoxByNum(num);
      if (cb != null)
        cb.setState(state);
   }

   public void setEnable(int num, boolean state) {
      NumCheckbox cb = getBoxByNum(num);
      if (cb != null)
        cb.setEnabled(state);
   }

   public void setStateRange(int beg, int end, boolean state) {
      for (int i = beg; i<= end; i++) {
         setState(i, state);
      }
   }

   public void setEnableRange(int beg, int end, boolean state) {
      for (int i = beg; i<= end; i++) {
         setEnable(i, state);
      }
   }

}//CheckBoxGlob
