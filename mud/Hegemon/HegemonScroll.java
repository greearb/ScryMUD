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
      
   }//constructor

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
         comps = components.getBottomPage(canvas.getSize().height - 15);
      }
      else {
         comps = components.getVector(top_y, top_y +
                                      canvas.getSize().height - 15);
      }
         

      //Log.it("In HegemonScroll.paint(), component's size: "
      //     + comps.size());
      canvas.setComponents(comps);
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

      canvas.setComponents(comps);
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
      else { //IN MODE PRE
         int len = txt.length();
         StringBuffer sb = new StringBuffer(100);
         char ch;
         for (int i = 0; i<len; i++) {
            ch = txt.charAt(i);
            if (ch == '\t') { //if a tab
               sb.append("        ");
            }
            else if (ch == '\r') { //carriage return
               continue;
            }
            else if (ch == '\n') { //newline
               //Log.it("Found a newline...\n");
               int my_len = sb.length();
               if (my_len > 0) {
                  //Log.it("\tLength > 0");
                  char[] buf = new char[my_len + 1];
                  sb.getChars(0, my_len, buf, 0);
                  //FontMetrics fm = props.getFontMetrics();
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
   
   public synchronized void pushFont(String fnt_name) {
      Log.instance().dbg("in pushFont, font_name:  " + fnt_name);
      Font f = Font.getFont(fnt_name, null);
      if (f != null) {
         Log.instance().dbg("Found a valid font.");
         props.pushFont(f);
      }//if
      else {
         Log.instance().dbg("Font not found.");
      }
   }//setFont

   public void drawNewline() {
      addComponent(new ScrollComponentNewline(vert_spacing * 4,
                   props.figurePointSize((props.getTextSize()).intValue())));
   }

}//HegemonScroll
   
