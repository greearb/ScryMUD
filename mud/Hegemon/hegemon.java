// $Id: hegemon.java,v 1.3 2001/03/31 07:06:00 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date$

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
import java.applet.*;
import java.net.*;

public class hegemon extends Applet {
   static HegemonManager hm;
   boolean is_running = false;
   //Image image;
   int state;
   static final int STARTING = 0;
   static final int RUNNING = 1;
   static final int STOPPED = 2;

   public hegemon() {
      super();
      is_running = false;
   }

   public static void main(String args[]) {
      String rcfile = null;
      
      // for application mode
      hm = new HegemonManager("hegemon.properties", false, null);
   }//main

   public void init() {
      String rcfile = getParameter("RC_FILE_URL");
      setBackground(Color.black);

      //applet mode
      state = STARTING;
      //image = null;

//       try {
//          URL img_url = new URL(getParameter("SCRY_GIF"));
//          System.out.println("Getting image from: " + getParameter("SCRY_GIF"));
//          image = (Image)(img_url.getContent());
//       }
//       catch (Exception e) {
//          e.printStackTrace();
//       }

      repaint();

      is_running = true;
      hm = new HegemonManager(rcfile, true, this);
      state = RUNNING;
      repaint();
   }

   public void start() {
      state = STARTING;
      repaint();
      if ((hm == null) && (!is_running)) {
         String rcfile = getParameter("RC_FILE_URL");

         //applet mode
         hm = new HegemonManager(rcfile, true, this);
      }
      state = RUNNING;
      repaint();
   }//start

   public void setOperStatus(int s) {
      if ((s >= STARTING) && (s <= STOPPED))
         state = s;
   }//setOperStatus

   public void destroy() {
      state = STOPPED;
      repaint();
      hm = null;
   }

   public void paint(Graphics g) {
      g.setColor(Color.white);
      //g.drawImage(image, 0, 0, 200, 100, Color.black, null);

      if (state == STARTING) {
         g.drawString("Starting", 30, 30);
      }
      else if (state == RUNNING) {
         g.drawString("Starting", 30, 30);
      }
      else if (state == STOPPED) {
         g.drawString("Stopped", 30, 30);
      }
   }//paint
   
}//hegemon


