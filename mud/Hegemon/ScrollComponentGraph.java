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


import java.util.*;
import java.awt.*;

class ScrollComponentGraph extends ScrollComponent {
   Color color;
   float percent_full; /* range zero to 1 */
   int orientation;
   int h_outline;
   int w_outline;
   int dflt_x;
   int dflt_y;

   static final int O_NORTH = 0;
   static final int O_EAST = 1;
   static final int O_SOUTH = 2;
   static final int O_WEST = 3;

   public ScrollComponentGraph(int _x, int _y, int _height, int _width,
                               int vert_spacing, Color _color,
                               int _orientation) {
      super(vert_spacing);
      color = _color;
      h_outline = _height;
      w_outline = _width;
      orientation = _orientation;
      percent_full = (float)0.0;
      dflt_x = _x;
      dflt_y = _y;
      enabled = false;
   }//constructor

   public int getHeight() {
      return h_outline;
   }

   public int getWidth() {
      return w_outline;
   }

   public void setColor(Color c) {
      color = c;
   }

   public Color getColor() {
      return color;
   }

   public void setDefaultCoords(int _x, int _y) {
      dflt_x = _x;
      dflt_y = _y;
   }

   /** p should range from 0.0 to 1.0 */
   public void setPercent(float p) {
      percent_full = p;
   }

   public void paint(Graphics g) {
      if (enabled) {
         paint(g, dflt_x, dflt_y);
      }
   }

   public void paint(Graphics g, int x, int y) {
      if (!enabled) {
         return;
      }

      int fill;
      
      if ((orientation == O_NORTH) || (orientation == O_SOUTH)) {
         fill = (int)(percent_full * h_outline);
      }
      else {
         fill = (int)(percent_full * w_outline);
      }

      fill = Math.min(w_outline, fill); //never overflow the box

      g.setColor(color);
      g.drawRect(x, y, w_outline, h_outline);

      switch (orientation) {
      case O_NORTH: /* sits on the bottom. */
         g.fillRect(x, y + h_outline - fill, w_outline, fill);
         break;
      case O_EAST:
         g.fillRect(x, y, fill, h_outline);
         break;
      case O_SOUTH:
         g.fillRect(x, y, w_outline, fill);
         break;
      case O_WEST:
         g.fillRect(x + w_outline - fill, y, fill, h_outline);
         break;
      default:
         Log.instance().err("Default case in ScrollComponentGraph::paint(): " 
                            + orientation);
      }//switch
   }//paint

   public String toString() {
      return "SC: Graph";
   }
}//ScrollComponentNewline
