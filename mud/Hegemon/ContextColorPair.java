// $Id$
// $Revision: 1.6 $  $Author$ $Date$

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
import java.awt.event.*;
import java.io.*;


class ContextColorPair extends Panel {
   private Context context;
   private Color color;
   private Font font;
   LabeledTextField red, green, blue, context_field, font_sz;
   LabeledChoice font_choice;
   LabeledChoice font_style;
   HegemonManager h;

   public ContextColorPair(Context ctx, HegemonManager hm) {
      super();
      h = hm;
      color = new Color(0, 0, 0);
      context = ctx;

      setLayout(new FlowLayout());

      red = new LabeledTextField("Red", "0", 3);
      green = new LabeledTextField("Green", "0", 3);
      blue = new LabeledTextField("Blue", "0", 3);
      context_field = new LabeledTextField("Context", getContextName(), 10);
      font_choice = new LabeledChoice("Font");
      font_sz = new LabeledTextField("Size", "10", 3);
      font_style = new LabeledChoice("Type");
      font_style.c.addItem("PLAIN");
      font_style.c.addItem("BOLD");
      font_style.c.addItem("ITALIC");

      Button modify = new Button("Modify");
      modify.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            modifyCallback();
         }});
                               
      context_field.setEditable(false);
      setForeground(color);

      String[] fonts = getToolkit().getFontList();
      for (int i = 0; i<fonts.length; i++) {
         font_choice.c.addItem(fonts[i]);
      }
        
      add(red);
      add(green);
      add(blue);
      add(context_field);
      add(font_choice);
      add(font_sz);
      add(font_style);
      add(modify);
   }//constructor

   public Choice getFontChoice() {
      return font_choice.c;
   }

   private String getContextName() {
      return context.getName();
   }
   
   public final Color getColor() {
      return color;
   }
   
   public final Font getFont() {
      return font;
   }

   public final Context getContext() {
      return context;
   }

   /**  This will capture color data from LabeledTextFields and update the
     color object accordingly.  It always assumes changes have been made, but
     it won't matter if they haven't.... */
   public void modifyCallback() {
      int r,g,b;

      try {
         r = Integer.parseInt(red.getText());
         g = Integer.parseInt(green.getText());
         b = Integer.parseInt(blue.getText());
      }
      catch (Exception e) {
         Log.instance().err("In ModifyCallback:  " + e);
         return;
      }
      
      if (r < 0)
         r = 0;
      else if (r > 255)
         r = 255;

      if (g < 0)
         g = 0;
      else if (g > 255)
         g = 255;
      
      if (b < 0)
         b = 0;
      else if (b > 255)
         b = 255;

      color = new Color(r,g,b);
      setForeground(color);

      if ((r + g + b) > 350)
         setBackground(new Color(0, 0, 0));
      else
         setBackground(new Color(255, 255, 255));

      if (context == Context.BACKGROUND) {
         if (h == null) {
            Log.instance().err("ERROR:  h is null in ContextColorPair.");
         }
         else {
            if (h.getScroll() == null) {
               Log.instance().err("ERROR: h.getScroll() is null");
            }
            else {
               h.getScroll().setBackground(color);
            }
         }
      }//if

      modifyFontCallback();

      repaint(1000);
   }//modifyCallback

   public void modifyFontCallback() {
      // Now, deal with the font.
      try {
         String s = font_style.c.getSelectedItem();
         int style = Font.PLAIN;
         if (s.equals("BOLD")) {
            style = Font.BOLD;
         }
         else if (s.equals("ITALIC")) {
            style = Font.ITALIC;
         }
         Font f = new Font(font_choice.c.getSelectedItem(), style,
                           Integer.parseInt(font_sz.getText()));
         if (f != null) {
            font = f;
            red.setFont(f);
            green.setFont(f);
            blue.setFont(f);
            context_field.setFont(f);
            font_choice.setFont(f);
            font_style.setFont(f);
            font_sz.setFont(f);
         }
      } catch (Exception e) {
         new MessageDialog("ERROR Getting Font", e.toString(),
                           "black", "red");
      }
   }// modifyFontCallback

   /** context will now change */
   public void readObject(BIStream istream) throws IOException {
      int r,g,b;
      int version = 0;

      r = istream.readInt();

      if ((r & 1024) > 0) {
         version = 1024;
         r = istream.readInt();
      }

      g = istream.readInt();
      b = istream.readInt();

      istream.readToken(); //context, it's set at construction currently

      String fn = "Dialog";
      String fs = "12";
      String ft = "Plain";

      if ((version & 1024) > 0) {
         // read the font stuff too
         fn = istream.readToken();
         fs = istream.readToken();
         ft = istream.readToken();
      }
      initObject(r, g, b, fn, fs, ft);
      //istream.readLine();
   }//read


   protected void initObject(int r, int g, int b, String fn, String fs, String ft) {
      color = null;

      red.setText(String.valueOf(r));
      green.setText(String.valueOf(g));
      blue.setText(String.valueOf(b));
      
      color = new Color(r,g,b);
      setForeground(color);
      
      if ((r + g + b) > 350)
         setBackground(new Color(0, 0, 0));
      else
         setBackground(new Color(255, 255, 255));

      font_choice.c.select(fn);
      font_sz.setText(fs);
      font_style.c.select(ft);
      modifyFontCallback();
   }//initObject

   public void writeObject(BOStream ostream) throws IOException {
      StringBuffer sb = new StringBuffer(100);
      sb.append("1024 "); //version number

      if (color != null) {
         sb.append(color.getRed() + " ");
         sb.append(color.getGreen() + " ");
         sb.append(color.getBlue() + " ");
      }
      else {
         sb.append("0 0 0 ");
      }

      sb.append(context.getNum() + " ");

      sb.append(font_choice.c.getSelectedItem() + " ");
      sb.append(font_sz.getText() + " ");
      sb.append(font_style.getText() + "\n");

      ostream.write(sb.toString());
   }//writeObject
}//ContextColorPair
