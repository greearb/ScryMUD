// $Id: HegemonDisplay.java,v 1.5 2001/03/31 07:05:59 greear Exp $
// $Revision: 1.5 $  $Author: greear $ $Date$

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
import java.awt.image.*;
import java.util.*;
import java.text.*;
import java.awt.event.*;


/** Contains a canvas and scroll bars.  Lots of inteligence as to how to
  draw and redraw things too... */
class HegemonDisplay extends Panel {
   HegemonManager hm = null;
   HegemonScroll scroll;
   Scrollbar hbar;
   Scrollbar vbar;
   int view_x, view_y;
   Graphics cached_g = null;
   boolean auto_scroll;


   /* NOTE:  HegemonManager h can be null, but it will make the default color
      black, regardless of what the user chooses in the Color's option on
      their GUI (ClientDisplay.java). */
   public HegemonDisplay(HegemonManager h) {
      super();

      hm = h;
      //hbar = new Scrollbar(Scrollbar.HORIZONTAL, 0, 300, 0, 0);
      vbar = new Scrollbar(Scrollbar.VERTICAL, 0, 300, 0, 0);
      //hbar.addAdjustmentListener(new HbarListener(this));
      scroll = new HegemonScroll(hm, this);

      vbar.addAdjustmentListener(new VbarListener(this));
      auto_scroll = true;
      
      view_x = 5;
      view_y = 0;
      
      this.setLayout(new BorderLayout(0,0));
      this.add("Center", scroll);
      //this.add("South", hbar);
      this.add("East", vbar);
   }//constructor


   public Dimension getPreferredSize() {
      if (hm.getClientDisplay() != null) {
         int w = hm.getClientDisplay().getWidth() - 20;
         int h = hm.getClientDisplay().getHeight() - 180;
         return new Dimension(w, h);
      }
      return new Dimension(800, 700);
   }

   public Dimension getMinimumSize() {
      return getPreferredSize();
   }

   public Dimension getMaximumSize() {
      return getPreferredSize();
   }

//   public Dimension getPreferedSize() {
//      return new Dimension(800, 1000);
//   }
   
   public void clear() {
      //hbar.setValues(0, 300, 0, 0);
      vbar.setValues(0, 300, 0, 0);
      scroll.clear();
      auto_scroll = true;
      view_x = 5;
      view_y = 0;
   }//clear
   
   public final void paintScroll() {
      //Log.it("in HegemonDisplay.paintScroll()");
      scroll.setPaintAttributes(view_x, view_y);
      scroll.paint(auto_scroll);

      int height = getSize().height;
      int vbar_posn = vbar.getValue();
      int scroll_height = scroll.getScrollHeight();
      
      if (auto_scroll) {
         vbar_posn = scroll_height - height;
      }
      vbar.setValues(vbar_posn, height - 25, 0, scroll_height);

      vbar.paint(vbar.getGraphics());
      //paint(getGraphics());
   }//paintScroll

   public final HegemonScroll getScroll() {
      return scroll;
   }

   public void do_page_up_adjustment() {
      view_y = vbar.getValue() - vbar.getVisibleAmount() + 30;
      vbar.setValue(view_y);

      conformAdjustment();
   }

   public void do_page_down_adjustment() {
      view_y = vbar.getValue() + vbar.getVisibleAmount() - 30;
      vbar.setValue(view_y);

      conformAdjustment();
   }


   public void do_vbar_adjustment(AdjustmentEvent e) {
      if (e.getAdjustmentType() == AdjustmentEvent.BLOCK_INCREMENT) {
         view_y = vbar.getValue() + vbar.getVisibleAmount();
         vbar.setValue(view_y);
      }
      else if (e.getAdjustmentType() == AdjustmentEvent.BLOCK_DECREMENT) {
         view_y = vbar.getValue() - vbar.getVisibleAmount();
         vbar.setValue(view_y);
      }
      else if (e.getAdjustmentType() == AdjustmentEvent.UNIT_DECREMENT) {
         view_y = vbar.getValue() - (vbar.getVisibleAmount() / 5);
         vbar.setValue(view_y);
      }
      else if (e.getAdjustmentType() == AdjustmentEvent.UNIT_INCREMENT) {
         view_y = vbar.getValue() + (vbar.getVisibleAmount() / 5);
         vbar.setValue(view_y);
      }
      else {
         view_y = vbar.getValue();
      }
      
      conformAdjustment();
   }//do_vbar_adjustment

   /** After moving up or down the scroll, make everything synch up
    * and repaint it.
    */
   protected void conformAdjustment() {
      int height = getSize().height;
      
      //Log.it("In do_vbar_adjustment, view_y:  " + view_y + "  height:  "
      //       + height + " scrollHeight:  " + scroll.getScrollHeight());

      if ((view_y + height) >= scroll.getScrollHeight()) {
         auto_scroll = true;
      }
      else {
         auto_scroll = false;
      }
      
      //Log.it("view_y:  " + view_y + "  auto_scroll:  " + auto_scroll);
      this.update(this.getGraphics());
      this.paintScroll();
   }//conformAdjustment


   public void noAutoScroll() {
      auto_scroll = false;
   }
   
   public void paint(Graphics g) {
      //Log.it("HegemonDisplay.paint()");
      scroll.setPaintAttributes(view_x, view_y);
      // Now lets set the scroll bar attributes

      int height = getSize().height;
      int vbar_posn = vbar.getValue();
      int scroll_height = scroll.getScrollHeight();

      if (auto_scroll) {
         vbar_posn = scroll_height - height;
      }
      vbar.setValues(vbar_posn, height - 25, 0, scroll_height);

      super.paint(g);
   }//paint
   
}//HegemonDisplay


/* hacks to get around compiler error in Linux JDK v1.1.3
 (The inline classes cause the compiler to drop into an endless loop
 somehow).  */
class VbarListener extends Object implements AdjustmentListener {
   HegemonDisplay par;
   
   VbarListener(HegemonDisplay parent) {
      super();
      par = parent;
   }//constructor
   
   public void adjustmentValueChanged(AdjustmentEvent e) {
      par.do_vbar_adjustment(e);
   }
}//hbar adjustment listener

