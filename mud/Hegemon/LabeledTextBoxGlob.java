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
import java.util.*;

class LabeledTextBoxGlob extends Panel {
  Label lbl;
  TextBoxGlob cbg;
  
  public LabeledTextBoxGlob(String name, int rows, int cols) {
    super();

    lbl = new Label(name);
    cbg = new TextBoxGlob(rows, cols);
    
    BorderLayout bl = new BorderLayout();
    setLayout(bl);

    add(lbl, "North");
    add(cbg, "South");
  }//constructor

  public void setEnabled(boolean state) {
     cbg.setEnabled(state);
     lbl.setEnabled(state);
     super.setEnabled(state);
  }

  public void addBox(NumTextBox cb) {
    cbg.addBox(cb);
  }

  public void addBox(NumTextBox cb, boolean invisible) {
     cbg.addBox(cb, invisible);
  }
   
  public String getText() {
    return cbg.getText();
  }

  public String getText(int start, int end) {
    return cbg.getText(start, end);
  }
  
}//LabeledTextBoxGlob


class TextBoxGlob extends Panel {
  Vector invisibles;
   
  public TextBoxGlob(int rows, int cols) {
    super();
    invisibles = new Vector();
    GridLayout fl = new GridLayout(rows, cols, 0, 0);
    setLayout(fl);
  }

  public void addBox(NumTextBox cb) {
     add(cb);
  }

  public void addBox(NumTextBox cb, boolean invisible) {
     if (!invisible)
       add(cb);
     else
       invisibles.addElement(cb); //add it to the invisible list
  }

  public void setEnabled(boolean state) {
     int cnt = getComponentCount();
     int low = 0;
     int high = 0;
     int tmp;
    
     if (cnt > 0) {
        high = low = ((NumTextBox)(getComponent(0))).getNum();
     }

     int i;
     
     for (i = 0; i<cnt; i++) {
        if ((tmp = ((NumTextBox)(getComponent(i))).getNum()) < low)
          low = tmp;
        if (tmp > high)
          high = tmp;
     }//for

     for (i = 0; i<invisibles.size(); i++) {
        if ((tmp = ((NumTextBox)(invisibles.elementAt(i))).getNum()) < low)
          low = tmp;
        if (tmp > high)
          high = tmp;
     }//for
     
     setEnabledRange(low, high, state);
  }//setEnabled     

   
  public void setEnabledRange(int low, int high, boolean state) {
    for (int i = low; i <= high; i++) {
       setBoxEnabled(i, state);
    }//for
  }//getText

   
  public String getText() {
     int cnt = getComponentCount();
     int low = 0;
     int high = 0;
     int tmp;
    
     if (cnt > 0) {
        high = low = ((NumTextBox)(getComponent(0))).getNum();
     }

     int i;
     
     for (i = 0; i<cnt; i++) {
        if ((tmp = ((NumTextBox)(getComponent(i))).getNum()) < low)
          low = tmp;
        if (tmp > high)
          high = tmp;
     }//for

     for (i = 0; i<invisibles.size(); i++) {
        if ((tmp = ((NumTextBox)(invisibles.elementAt(i))).getNum()) < low)
          low = tmp;
        if (tmp > high)
          high = tmp;
     }//for
     
     return getText(low, high);
  }//getText

  public String getText(int start, int end) { /* start and end of the NUMBERS
						 of the text boxes, not their
						 array offset */
    int low = start; // low keeps track of the next box to get the text for

    String s;
    StringBuffer sb = new StringBuffer(100);
    for (int i = start; i <= end; i++) {
      s = getBoxText(i); //get the text from the box numbered 'i'
      if (s != null) {
	sb.append(s);
	sb.append(" ");
      }//if
    }//for

    return sb.toString();
  }//getText


  public String getBoxText(int box_num) { // get the text for box numbered box_num
    int cnt = getComponentCount();
    NumTextBox tb;
    
    for (int i = 0; i<cnt; i++) {
      tb = (NumTextBox)(getComponent(i));
      if (tb.getNum() == box_num) {
	return tb.getText();
      }//if
    }//for

    /* lets check in the invisible boxes now */
    cnt = invisibles.size();
    for (int i = 0; i < cnt; i++) {
      tb = (NumTextBox)(invisibles.elementAt(i));
      if (tb.getNum() == box_num) {
	return tb.getText();
      }//if
    }//for
    
    return null; // just couldn't find it, exit somewhat gracefully
  }//getBoxText


  public void setBoxEnabled(int box_num, boolean state) {
    int cnt = getComponentCount();
    NumTextBox tb;
    
    for (int i = 0; i<cnt; i++) {
      tb = (NumTextBox)(getComponent(i));
      if (tb.getNum() == box_num) {
	tb.setEnabled(state);
      }//if
    }//for

    /* lets check in the invisible boxes now */
    cnt = invisibles.size();
    for (int i = 0; i < cnt; i++) {
      tb = (NumTextBox)(invisibles.elementAt(i));
      if (tb.getNum() == box_num) {
	tb.setEnabled(state);
      }//if
    }//for
  }//getBoxText
   
}//TextBoxGlob
