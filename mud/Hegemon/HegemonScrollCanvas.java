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
import java.awt.event.*;

class HegemonScrollCanvas extends Canvas {
   Vector text_components = null;
   boolean do_paint;
   int x_offset = 0;
   HegemonDisplay display;

   public HegemonScrollCanvas(HegemonDisplay d) {
      super();
      do_paint = true;
      display = d;

      addKeyListener(new KeyAdapter() {
         public void keyPressed(KeyEvent e) {
            if (e.getKeyCode() == KeyEvent.VK_PAGE_UP) {
               display.do_page_up_adjustment();
            }
            else if (e.getKeyCode() == KeyEvent.VK_PAGE_DOWN) {
               display.do_page_down_adjustment();
            }
            else
              super.keyPressed(e);
         }});
   
   }

   public void clear() {
      text_components = null;
      do_paint = true;
   }

   public void setXOffset(int x) {
      x_offset = x;
   }
   
   public void setComponents(Vector t_comp) {
      //Log.it("Set components:  " + t_comp);
      text_components = t_comp;
   }

   public void paint() {
      //Log.it("In HegemonScrollCanvas.paint()");
      Graphics g = getGraphics();
      update(g);
      do_paint = false;
      paint(g);
   }
   
   public void paint(Graphics g) {
      //Log.it("In HegemonScrollCanvas.paint(Graphics g)");
      if (!do_paint) {
         do_paint = true;
         return;
      }

      //Log.it("Actually painting...");
      if (text_components == null) {
         Log.instance().dbg("text_components g is NULL.");
         return;
      }//if
      
      int sz = text_components.size();
      int y = 15;
      
      //Log.it("ScrollCanvas.paint(Graphics g), size:  " + sz);
      ScrollComponent sc = null;
      
      for (int i = 0; i<sz; i++) {
         sc = (ScrollComponent)text_components.elementAt(i);
         sc.paint(g, x_offset, y);
         y += sc.getHeight();
//         Log.it("Height:  " + y);
      }//for
   }//paint
}//HegemonScrollCanvas