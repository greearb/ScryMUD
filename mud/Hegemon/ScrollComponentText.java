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

import java.util.*;
import java.awt.*;


class ScrollComponentText extends ScrollComponent {
   Vector segments;
   int width;
   int x_offset = 0;
   
   public int getVertSpacing() {
      return vert_spacing;
   }

   public String toString() {
      StringBuffer sb = new StringBuffer(100);
      int len = segments.size();
      sb.append("ScrollComponent: TEXT, seg len: " + len + "\n");
      for (int i = 0; i<len; i++) {
         sb.append("[" + i + "]  ");
         sb.append(((TextSegment)(segments.elementAt(i))).toString());
         sb.append("\n");
      }//for
      return sb.toString();
   }//toString
   
   public ScrollComponentText(int vertical_spacing,
                              HegemonDisplayProperties props) {
      super(vertical_spacing);
      x_offset = props.getTabs() * 20;
      width = 0;
      segments = new Vector(5); /* holds a list of text segments */
   }//constructor

   public void addString(String str, HegemonDisplayProperties props) {
      TextSegment ts;
      char[] buf = new char[str.length()];
      str.getChars(0, buf.length, buf, 0);
      segments.addElement(ts = new TextSegment(buf, props));
      height = Math.max(ts.getHeight(), height);
      width += ts.getWidth();
   }//addString

   public void addString(char[] buf, HegemonDisplayProperties props) {
      TextSegment ts;
      segments.addElement(ts = new TextSegment(buf, props));
      height = Math.max(ts.getHeight(), height);
      width += ts.getWidth();
   }
   
   public void addChars(char[] buf, int start, int end,
                        HegemonDisplayProperties props) {
      StringBuffer sb = new StringBuffer(end-start);
      sb.append(buf, start, end);
      this.addString(sb.toString(), props);
   }
      

   public void paint(Graphics g, int x, int y) {
      
      int len = segments.size();
      int x_val = x_offset;
      TextSegment ts;

      for (int i = 0; i<len; i++) {
         ts = (TextSegment)(segments.elementAt(i));
         ts.draw(g, x + x_val, y);
         x_val += ts.getWidth();
      }//for
   }//draw

   final public int getWidth() {
      return width;
   }
}//ScrollComponent


/** A text segment is a string with uniform properties.  For example, the
  color, and font are all the same.  Also, a text segment should never
  take more than one line to display. */
class TextSegment extends Object {
   Font font;
   Color color;
   char[] text;
   int width;
   int ascent;
   int descent;

   public TextSegment(char[] txt, HegemonDisplayProperties prop) {
      //Log.it("TextSegment constructor: txt -:" + txt + ":-");
      font = prop.getFont();
      color = prop.getColor();
      //Log.it("Color:  " + color);

      
      text = txt;

      FontMetrics fm = prop.getFontMetrics();
      if (fm == null) {
         Log.it("TextSegment(): fm was null");
         /* gotta guestimate it then */
         int sz = font.getSize();
         width = ((sz >> 1) * text.length + 1);
         ascent = (int)(sz * .6);
         descent = (int)(sz * .4);
      }//if
      else {
         //Log.it("TextSegment(): fm was NOT null");
         width = fm.charsWidth(txt, 0, txt.length);
         ascent = fm.getAscent();
         descent = fm.getDescent();
      }
   }//constructor

   public String toString() {
      return ("TextSegment:  -:" + text + ":-");
   }

   public void draw(Graphics g, int x, int y) {
//      Log.it("TextSegment: draw, text -:" + text + ":-" + " Font: " + font);
      g.setFont(font);
      g.setColor(color);
      g.drawChars(text, 0, text.length, x, y);
   }//draw

   public final int getHeight() {
      return (ascent + descent);
   }

   public final int getWidth() {
      return width;
   }
}//TextSegment
