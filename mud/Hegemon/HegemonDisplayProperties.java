// $Id: HegemonDisplayProperties.java,v 1.4 2001/03/31 07:05:59 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date: 2001/03/31 07:05:59 $

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


class HegemonDisplayProperties extends Object {
   HegemonManager hm = null;
   
   Stack fonts;
   Stack colors;
   Stack modes;  // PRE, NORMAL....un-used right now
   Stack text_sizes;
   Stack context_modes; // Help Page, Mstat, normal...used in InputFilter
   
   Graphics g;

   Integer mode;
   Font font;
   Color color;
   Integer text_size;
   Integer context_mode;
   
   Font dflt_font;
   Color dflt_color;
   int tabs = 0;
   
   public static final Integer MODE_PRE = new Integer(0);
   public static final Integer MODE_NORMAL = new Integer(1);
   
   public HegemonDisplayProperties(HegemonManager h, Graphics gr) {
      super();

      hm = h;
      fonts = new Stack();
      colors = new Stack();
      text_sizes = new Stack();
      modes = new Stack();
      context_modes = new Stack();

      dflt_font = font = new Font("Dialog", Font.PLAIN, 12);
      if (font == null) {
         dflt_font = font = new Font("curier", Font.PLAIN, 12);
         Log.instance().err("ERROR:  couldn't get a default font, trying Dialog font.\n");
      }//if

      if (gr != null) {
         g = gr;
      }//else

      if (hm != null) {
         color = dflt_color = hm.getColorSelectionMgr().getOtherSelection();
      }
      else {
         color = dflt_color = new Color(0, 0, 0);
      }
      
      //Log.instance().dbg("Other Color:  " + color);
      text_size = new Integer(4);
      mode = MODE_PRE;

      context_mode = HegemonInputFilter.MODE_NORMAL;
      context_modes.push(context_mode);
      
      fonts.push(font);
      colors.push(color);
      text_sizes.push(text_size);
      modes.push(MODE_NORMAL);
      modes.push(mode);
      
   }//constructor


   // Reset things to an initial state...
   public void reset() {

      //Log.it("Resetting the Properties..");
      
      fonts = new Stack();
      colors = new Stack();
      text_sizes = new Stack();
      modes = new Stack();
      context_modes = new Stack();
      
      dflt_font = font = new Font("Dialog", Font.PLAIN, 12);
      if (font == null) {
         dflt_font = font = new Font("courier", Font.PLAIN, 12);
         Log.instance().err("ERROR:  couldn't get Dialog font, trying courier.\n");
      }//if

      if (hm != null) {
         color = dflt_color = hm.getColorSelectionMgr().getOtherSelection();
      }
      else {
         color = dflt_color = new Color(0, 0, 0);
      }
      
      //Log.it("OtherColor:  " + color);
      text_size = new Integer(4);
      mode = MODE_PRE;
      
      context_mode = HegemonInputFilter.MODE_NORMAL;
      context_modes.push(context_mode);

      fonts.push(font);
      colors.push(color);
      text_sizes.push(text_size);
      modes.push(MODE_NORMAL);
      modes.push(mode);
   }//reset
   
   final public int getTabs() {
      return tabs;
   }

   final public Integer getTextSize() {
      return text_size;
   }

   final public Integer getContextMode() {
      return context_mode;
   }
   
   public int figurePointSize(int i) {
      return Math.min(Math.max(5, (20 - i*2)), 20);
   }
   
   public FontMetrics getFontMetrics() {
      if (g != null) {
         g.setFont(getFont());
         return g.getFontMetrics();
      }
      return null;
   }

   public void setGraphics(Graphics gr) {
      //Log.it("in setGraphics:  font: " + gr.getFont());
      g = gr;
   }//setGraphics

   public Graphics getGraphics() {
      return g;
   }//getGraphics

   public void pushColor(Color c) {
      //Log.it("In pushColor: olc:" + color + " new: " + c);
      if (colors.size() < 50) {
         //Log.it("Pushed it.");
         colors.push(c);
      }
      color = c;
   }//pushColor

   public void popColor() {
      //Log.it("In popColor: " + color);
      if (colors.size() > 1) {
         colors.pop();
         color = (Color)colors.peek();
         //Log.it("Popped it: " + color);
      }
   }//popColor


   public void pushTextSize(Integer i) {
      if (text_sizes.size() < 50) {
         text_sizes.push(i);
         if (i != text_size) {
            String name = font.getName();
            int style = font.getStyle();
            int pt_size = figurePointSize(i.intValue());
            Font new_fnt = new Font(name, style, pt_size);
            if (new_fnt != null) {
               pushFont(new_fnt);
            }
            else {
               pushFont(font);
            }
         }//if
         else {
            pushFont(font);
         }
      }//if
      text_size = i;
   }//pushTextSize

   public void popTextSize() {
      if (text_sizes.size() > 1) {
         text_sizes.pop();
         text_size = (Integer)(text_sizes.peek());
         popFont(); // because we pushed one earlier
      }
   }//popColor

   public void pushContextMode(Integer i) {
      if (context_modes.size() < 50) {
         context_modes.push(i);
      }
      context_mode = i;
   }//pushTextSize

   public void popContextMode() {
      if (context_modes.size() > 1) {
         context_modes.pop();
         context_mode = (Integer)(context_modes.peek());
      }
   }//popColor

   public void pushMode(Integer i) {
      if ((i == MODE_PRE) || (i == MODE_NORMAL)) {
         if (modes.size() < 50) {
            modes.push(i);
         }
         mode = i;
      }
   }//pushMode

   public void popMode() {
      if (modes.size() > 1) {
         modes.pop();
         mode = (Integer)(modes.peek());
      }
   }//popMode

   public void pushTab() {
      if (tabs < 5)
         tabs++;
   }//pushTab

   public void popTab() {
      if (tabs > 0)
         tabs--;
   }//popTab

   public void pushFont(Font f) {
      Log.instance().dbg("DisplayProperties.pushFont:  " + font.getName());
      if (fonts.size() < 50) {
         fonts.push(f);
      }
      font = f;
      g.setFont(f);
   }//pushFont

   public void popFont() {
      if (fonts.size() > 1) {
         fonts.pop();
         font = (Font)fonts.peek();
         g.setFont(font);
      }
   }//popFont

   public Color getColor() {
      return color;
   }

   public Font getFont() {
      return font;
   }

   public boolean isModePRE() {
      if (mode == MODE_PRE)
        return true;
      return false;
   }

   public boolean isModeNormal() {
      if (mode == MODE_NORMAL)
        return true;
      return false;
   }
   
}//HegemonDisplayProperties
