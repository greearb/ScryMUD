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
import java.awt.image.*;
import java.text.*;
import java.awt.event.*;

/**  Stores a certain amount of data (text).  It also contains the logic
  needed to return vectors containing certain portions of the scroll.
  */
class HegemonScroll extends Panel {
   HegemonManager hm = null;
   ScrollComponentCollection components;
   HegemonDisplayProperties props;
   ScrollComponentText comp_in_progress;
   ScrollComponent last_added_component;
   HegemonScrollCanvas canvas;
   int vert_spacing;
   HegemonDisplay parent;
   int left_x;
   int top_y;
   boolean auto_scroll;
   Vector non_scrollables; /* vector of objects which are not
                              intended to scroll with the rest of
                              the text.
                           */

   ScrollComponentGraph hp;
   ScrollComponentGraph mana;
   ScrollComponentGraph mov;

   public HegemonScroll(HegemonManager h, HegemonDisplay par) {
      super();

      hm = h;
      parent = par;
      components = new ScrollComponentQueue(1000);
      vert_spacing = 1;

      setBackground(new Color(250, 250, 250));
      
      canvas = new HegemonScrollCanvas(parent);
      
      setLayout(new BorderLayout());
      this.add(canvas, "Center");
      
      props = new HegemonDisplayProperties(hm, canvas.getGraphics());
      comp_in_progress = new ScrollComponentText(vert_spacing, props);
      hp = new ScrollComponentGraph(5, canvas.getSize().height - 5,
                                    8, 55, 4, new Color(50, 0, 0),
                                    ScrollComponentGraph.O_EAST);
      mana = new ScrollComponentGraph(65, canvas.getSize().height - 5,
                                      8, 55, 4, new Color(0, 0, 50),
                                      ScrollComponentGraph.O_EAST);
      mov = new ScrollComponentGraph(125, canvas.getSize().height - 5,
                                     8, 55, 4, new Color(0, 50, 0),
                                     ScrollComponentGraph.O_EAST);

      non_scrollables = new Vector();
      non_scrollables.addElement(hp);
      non_scrollables.addElement(mana);
      non_scrollables.addElement(mov);
      
   }//constructor

   public void notifyScrollCanvasResized() {
      Dimension d = canvas.getSize();
      hp.setDefaultCoords(5, d.height - 10);
      mana.setDefaultCoords(65, d.height - 10);
      mov.setDefaultCoords(125, d.height - 10);
   }

   public void setPrompt(int h, int hm, int m, int mm,
                         int v, int vm) {
      int cc; /* color component */
      Color c;

      hp.setEnabled(true);
      mana.setEnabled(true);
      mov.setEnabled(true);

      hp.setPercent((float)h / (float)hm);
      cc = Math.max(200, 255 - Math.max(0, (hm / 8)));
      c = hp.getColor();
      if (c.getRed() != cc) {
         hp.setColor(new Color(cc, calcOtherColor(cc), calcOtherColor(cc)));
      }

      mana.setPercent((float)m / (float)mm);
      cc = Math.max(200, 255 - Math.max(0, (mm / 8)));
      c = mana.getColor();
      if (c.getBlue() != cc) {
         mana.setColor(new Color(calcOtherColor(cc), cc, calcOtherColor(cc)));
      }

      mov.setPercent((float)v / (float)vm);
      cc = Math.max(200, 255 - Math.max(0, (vm / 8)));
      c = mov.getColor();
      if (c.getGreen() != cc) {
         mov.setColor(new Color(calcOtherColor(cc), calcOtherColor(cc), cc));
      }
   }//setPrompt

   /** The other color component should always be less than the primary
    * color.  Also, as the primary reaches its strongest (200),
    * the other colors should drop to zero.  The goal is to go from
    * light shades for weaker players to deep, strong colors for more
    * powerful players.  However, the general hue:  Red==HP, Blue=MANA,
    * Green==MOVE, should remain the same.  p should range from 255
    * at the weakest, to 200 at the strongest.
    */
   protected int calcOtherColor(int p) {
      return (p - 200) * 2; //Yeesh, is it that simple!! 
   }


   public void clear() {
      props.reset();
      components.clear();
      canvas.clear();
      vert_spacing = 1;
      comp_in_progress = new ScrollComponentText(vert_spacing, props);
   }//clear
   
   public HegemonDisplay getPar() {
      return parent;
   }

   public void setScrollComponentCollection(ScrollComponentCollection scc) {
      components = scc;
   }

   public int getScrollHeight() {
      return components.getTotalHeight();
   }
   
   public final void setPaintAttributes(int x, int y) {
      left_x = x;
      top_y = y;
      canvas.setXOffset(left_x);
   }

   public void paint(boolean a_scroll) {
      Vector comps;
      auto_scroll = a_scroll;

      //Log.it("HegemonScroll.paint(" + auto_scroll + "), top_y:  "
      //       + top_y + " height:  " + canvas.getSize().height);
      
      if (auto_scroll) {
         comps = components.getBottomPage(canvas.getSize().height - 25);
      }
      else {
         comps = components.getVector(top_y, top_y +
                                      canvas.getSize().height - 25);
      }
         

      //Log.it("In HegemonScroll.paint(), component's size: "
      //     + comps.size());
      canvas.setComponents(comps, non_scrollables);
      canvas.paint();
   }//paint    
   

   public void paint(Graphics g) {
      Log.instance().trc("In HegemonScroll.paint(Graphics g)");
      
      if ((g != null) && (props.getGraphics() == null)) {
         Log.instance().dbg("Setting graphics in the Properties.");
         props.setGraphics(g);
      }
      
      Vector comps;

      // We can't send auto_scroll in here, but lets hope our cached value is
      // good enough....
      if (auto_scroll) {
         comps = components.getBottomPage(canvas.getSize().height - 15);
      }
      else {
         comps = components.getVector(top_y, top_y +
                                      canvas.getSize().height - 15);
      }

      canvas.setComponents(comps, non_scrollables);
      super.paint(g);
   }//paint

   final public HegemonDisplayProperties getProperties() {
      return props;
   }

   private final void addComponent(ScrollComponent sc) {
      if (sc == last_added_component) {
         //Log.it("\n\n       RETURNING FROM addComponent().\n\n");
         return; //no need to add it again
      }
      else {
         last_added_component = sc;
      }

      //Log.it("\naddComponent:  " + sc + "\n");
      try {
         components.push(sc);
      }
      catch (Exception e) {
         Log.instance().err("addComponent:  Trying to push: " + e);
      }
   }//addComponent
   
   /** appends text, using the current context, to the end of the scroll. */
   public synchronized void append(String str) {
      //Log.it("In HegemonScroll.append(), str -:  " + str + ":-");
      if (str == null)
         return;
      else {
         drawString(str);
         //parent.paintScroll();
      }
   }//append(String)

   private synchronized void drawString(String txt) {
      //Log.it("HegemonScroll.drawString:  comp_in_progress: "
      //       + comp_in_progress + "\nTEXT  -:" + txt + ":-");
      /* Assume text can and will be auto-wrapped.  All whitespace will be
       translated to spaces which will be consolidated as per HTML. */

      //this just screws things up in ScryMUD.  Perhaps in some other
      //application it would be more useful...
      if (false) { //!props.isModePRE()) {
         int len = txt.length();
         char sb[] = new char[len + 2];
         int sb_len;
      
         char ch;
         boolean last_was_ws = false;
         int j = 0;
         for (int i = 0; i<len; i++) {
            ch = txt.charAt(i);
            if (Character.isWhitespace(ch)) {
               if (last_was_ws) {
                  continue; //don't append it
               }
               else {
                  last_was_ws = true;
               }
            }//if was ws
            else {
               last_was_ws = false;
            }
            sb[j] = ch;
            j++;
         }//for (consolidate white space)      
         sb_len = j;

         
         /* when here:  sb contains the string we want to deal with. */
         /* now lets paint the string, wrapping as needed. */

         int width = this.getSize().width;
         FontMetrics fm = props.getFontMetrics();
         int max_adv = fm.getMaxAdvance();

         int str_width;
         int space_avail;
         int offset = 0;
         int num_chars;
         BreakIterator line_breaker = BreakIterator.getLineInstance();
         line_breaker.setText(new String(sb));

         int brk1 = 0;
         int brk2 = 0;
         int our_break = 0;
         
         while (true) { //lets do the entire array...
            //Log.it("In while:  sb: -:" + sb + ":- offset: -:" + offset +
            //       ":- sb.length: -:" + sb_len); 
            str_width = fm.charsWidth(sb, offset, sb_len - (offset + 1));
            space_avail = width - comp_in_progress.getWidth();
            
            if (str_width > space_avail) { /* gotta wrap the sucker */
               
               /* num chars we can paint... */
               num_chars = (int)(space_avail/max_adv); 
               
               brk1 = line_breaker.following(offset + num_chars);
               brk2 = line_breaker.previous(); /* this is the one we want */

               if (brk2 > offset) {
                  our_break = brk2;
               }
               else {
                  our_break = brk1;
               }

               comp_in_progress.addChars(sb, offset, (our_break - offset),
                                         props);

               offset = our_break + 1;
               addComponent(comp_in_progress);
               comp_in_progress = new ScrollComponentText(vert_spacing,
                                                          props);
            }//if had to wrap
            else {

               comp_in_progress.addChars(sb, offset, (our_break - offset),
                                         props);
               break;
            }
            
            if (offset >= (sb.length - 1)) { /* subtract one b/c starts at 0 */
               break; /* done */
            }
         }//while true
      }//if not in MODE_PRE
      else { //IN MODE PRE (we'll do line wrapping at least now)
         int len = txt.length();
         StringBuffer sb = new StringBuffer(100);
         char ch;
         int comp_len_sofar = 0;
         if (comp_in_progress != null) {
            comp_len_sofar = comp_in_progress.getWidth();
         }
         FontMetrics fm = props.getFontMetrics();
         Font font = props.getFont();
         int sz = font.getSize();
         int mx_size = parent.getSize().width - 40; //-20 is for scroll bar
         int last_ws_idx = 0;

         for (int i = 0; i<len; i++) {
            ch = txt.charAt(i);

            if (Character.isWhitespace(ch)) {
               last_ws_idx = i;
            }

            if (fm == null) {
               /* gotta guestimate it then */
               comp_len_sofar += (sz >> 1);
            }//if
            else {
               comp_len_sofar += fm.charWidth(ch);
            }

            if (ch == '\t') { //if a tab
               sb.append("        ");
            }
            else if (ch == '\r') { //carriage return
               continue;
            }
            else if (comp_len_sofar > mx_size) {
               int my_len = sb.length();
               Log.instance().dbg("Comp_len_sofar: " + comp_len_sofar
                                  + " > mx_size: " + mx_size + " sb -:"
                                  + sb + "last_ws_idx: " + last_ws_idx
                                  + " sb.length(): " + sb.length());
               if (last_ws_idx > 0) {
                  my_len = sb.length() - (i - last_ws_idx);
                  i = last_ws_idx;
               }

               Log.instance().dbg("i: " + i + " my_len: " + my_len);
               char[] buf = new char[my_len + 1];
               sb.getChars(0, my_len, buf, 0);
               comp_in_progress.addChars(buf, 0, my_len, props);
               addComponent(comp_in_progress);
               comp_in_progress = new ScrollComponentText(vert_spacing, props);
               last_ws_idx = 0;
               sb = new StringBuffer(100);
               comp_len_sofar = 0;
            }
            else if (ch == '\n') { //newline
               //Log.it("Found a newline...\n");
               int my_len = sb.length();
               if (my_len > 0) {
                  //Log.it("\tLength > 0");
                  char[] buf = new char[my_len + 1];
                  sb.getChars(0, my_len, buf, 0);
                  comp_in_progress.addChars(buf, 0, my_len, props);
                  addComponent(comp_in_progress);
                  comp_in_progress = new ScrollComponentText(vert_spacing,
                                                             props);
                  sb = new StringBuffer(100);
               }//if the buffer contains something...
               else {
                  //Log.it("\tLength == 0.");
                  if (comp_in_progress.getWidth() > 0) {
                     addComponent(comp_in_progress);
                     comp_in_progress = new ScrollComponentText(vert_spacing,
                                                                props);
                  }//if
                  addComponent(new ScrollComponentNewline(12, vert_spacing));
               }
               last_ws_idx = 0;
               comp_len_sofar = 0;
            }
            else {
               sb.append(ch);
            }
         }//for

         if ((len = sb.length()) > 0) {
            //Log.it("\tWas some left over...");
            comp_in_progress.addString(sb.toString(), props);
            if (comp_in_progress != last_added_component) {
               addComponent(comp_in_progress);
            }//if
         }//if we have some left over...
      }//else, IN MODE_PRE
   }//drawString

   public synchronized void pushColor(String color_name) {
      try {
         Color c = ColorUtils.getColor(color_name);
         props.pushColor(c);
      }
      catch (ColorNameException e) {
         Log.instance().err(e.toString());
      }
   }//pushColor(String)
   

   public synchronized void pushColor(Color c) {
      props.pushColor(c);
   }//pushColor(Color)
   
   public synchronized void pushFont(Font f) {
      if (f != null) {
         props.pushFont(f);
      }//if
   }//setFont

   public synchronized void popColor() {
      props.popColor();
   }

   public synchronized void popFont() {
      props.popFont();
   }

   public void drawNewline() {
      addComponent(new ScrollComponentNewline(vert_spacing * 4,
                   props.figurePointSize((props.getTextSize()).intValue())));
   }

}//HegemonScroll
   
