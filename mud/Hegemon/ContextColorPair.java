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
import java.awt.event.*;
import java.io.*;


class ContextColorPair extends Panel {
   private Context context;
   private Color color;
   LabeledTextField red, green, blue, context_field;
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
      context_field = new LabeledTextField("Context", getContextName(), 15);
      Button modify = new Button("Modify");

      modify.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            modifyCallback();
         }});
                               
      context_field.setEditable(false);
      setForeground(color);
      
      add(red);
      add(green);
      add(blue);
      add(context_field);
      add(modify);
   }//constructor

   private String getContextName() {
      return context.getName();
   }
   
   public final Color getColor() {
      return color;
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

      repaint(1000);
   }//modifyCallback
   

   /** context will now change */
   public void readObject(BIStream istream) throws IOException {
      int r,g,b;
      color = null;

      r = istream.readInt();
      g = istream.readInt();
      b = istream.readInt();

      red.setText(String.valueOf(r));
      green.setText(String.valueOf(g));
      blue.setText(String.valueOf(b));
      
      color = new Color(r,g,b);
      setForeground(color);
      
      if ((r + g + b) > 350)
         setBackground(new Color(0, 0, 0));
      else
         setBackground(new Color(255, 255, 255));

      istream.readLine();
   }//read

   public void writeObject(BOStream ostream) throws IOException {
      StringBuffer sb = new StringBuffer(100);
      if (color != null) {
         sb.append(color.getRed());
         sb.append(" ");
         sb.append(color.getGreen());
         sb.append(" ");
         sb.append(color.getBlue());
         sb.append(" ");
      }
      else {
         sb.append("0 0 0 ");
      }

      sb.append(context.getNum());
      sb.append("\n");

      ostream.write(sb.toString());
   }//writeObject
}//ContextColorPair
