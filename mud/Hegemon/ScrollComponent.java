// $Id$
// $Revision$  $Author$ $Date$

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

/** A scrolling component. Probably outdated. */
class ScrollComponent {
   protected int height;
   protected int vert_spacing;
   protected boolean enabled; /* should we show it or not? */

   public ScrollComponent(int vertical_spacing) {
      super();
      vert_spacing = vertical_spacing;
      height = 0;
      enabled = true;
   }//constructor

   public boolean isEnabled() {
      return enabled;
   }

   public void setEnabled(boolean en) {
      enabled = en;
   }

   public void paint(Graphics g, int x, int y) {
      return;
   }//draw

   /** Used when the component knows where it should be */
   public void paint(Graphics g) {
      return;
   }

   public int getHeight() {
      return (height + vert_spacing);
   }

   public int getWidth() {
      return 0;
   }

   public String toString() {
      return "ScrollComponent base class";
   }
   
}//ScrollComponent


