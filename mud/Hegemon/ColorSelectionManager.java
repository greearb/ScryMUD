// $Id: ColorSelectionManager.java,v 1.5 1999/06/05 23:29:12 greear Exp $
// $Revision: 1.5 $  $Author: greear $ $Date: 1999/06/05 23:29:12 $

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
import java.util.*;
import java.net.*;

/**  This manages the fonts and colors for different types of
 * input.  Fully customizable by the end user.
 */
class ColorSelectionManager extends Frame {
   HegemonManager hm = null;
   
   ContextColorPair door_list;
   ContextColorPair mob_list;
   ContextColorPair item_list;
   ContextColorPair rm_desc;
   ContextColorPair rm_short_desc;
   ContextColorPair gossip;
   ContextColorPair tell;
   ContextColorPair yell;
   ContextColorPair auction;
   ContextColorPair battle;
   ContextColorPair other;
   ContextColorPair say;
   ContextColorPair back_ground;
   ContextColorPair input;

   ScrollPane sp;
   
   public ColorSelectionManager(HegemonManager h) {
      super("Color Selection Manager");

      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            do_close();
         }});

      hm = h;
      door_list = new ContextColorPair(Context.DOOR_LIST, hm);
      mob_list = new ContextColorPair(Context.MOB_LIST, hm);
      item_list = new ContextColorPair(Context.ITEM_LIST, hm);
      rm_desc = new ContextColorPair(Context.RM_DESC, hm);
      rm_short_desc = new ContextColorPair(Context.RM_SHORT_DESC, hm);
      gossip = new ContextColorPair(Context.GOSSIP, hm);
      tell = new ContextColorPair(Context.TELL, hm);
      yell = new ContextColorPair(Context.YELL, hm);
      auction = new ContextColorPair(Context.AUCTION, hm);
      battle = new ContextColorPair(Context.BATTLE, hm);
      other = new ContextColorPair(Context.OTHER, hm);
      say = new ContextColorPair(Context.SAY, hm);
      back_ground = new ContextColorPair(Context.BACKGROUND, hm);
      input = new ContextColorPair(Context.INPUT, hm);

      try {
         readObject();
      }
      catch (IOException e) {
         Log.instance().wrn("WARNING:  ColorSelectionManager():  " + e);
      }

      sp = new ScrollPane();
      Panel p = new Panel();
      
      int REM = GridBagConstraints.REMAINDER;
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      p.setLayout(gridbag);

      c.fill = GridBagConstraints.BOTH;
      c.weightx = c.weighty = 0.0;
            
      c.gridwidth = REM;
      gridbag.setConstraints(door_list, c);
      p.add(door_list);
      //c.gridwidth = REM;
      gridbag.setConstraints(mob_list, c);
      p.add(mob_list);

      //c.gridwidth = 1;
      gridbag.setConstraints(item_list, c);
      p.add(item_list);
      //c.gridwidth = REM;
      gridbag.setConstraints(rm_desc, c);
      p.add(rm_desc);

      //c.gridwidth = 1;
      gridbag.setConstraints(gossip, c);
      p.add(gossip);
      //c.gridwidth = REM;
      gridbag.setConstraints(tell, c);
      p.add(tell);

      //c.gridwidth = 1;
      gridbag.setConstraints(yell, c);
      p.add(yell);
      //c.gridwidth = REM;
      gridbag.setConstraints(auction, c);
      p.add(auction);

      //c.gridwidth = 1;
      gridbag.setConstraints(battle, c);
      p.add(battle);
      //c.gridwidth = REM;
      gridbag.setConstraints(rm_short_desc, c);
      p.add(rm_short_desc);

      //c.gridwidth = 1;
      gridbag.setConstraints(other, c);
      p.add(other);
      //c.gridwidth = REM;
      gridbag.setConstraints(say, c);
      p.add(say);

      //c.gridwidth = REM;
      gridbag.setConstraints(back_ground, c);
      p.add(back_ground);

      gridbag.setConstraints(input, c);
      p.add(input);

      CSButtons cs_buttons = new CSButtons(this);
      gridbag.setConstraints(cs_buttons, c);
      p.add(cs_buttons);

      sp.add(p);
      sp.setSize(650, 475);
      add(sp);
      sp.setScrollPosition(0, 0);

      pack();
   }//constructor

   public final Color getDoorListSelection() {
      return door_list.getColor();
   }

   public final Color getMobListSelection() {
      return mob_list.getColor();
   }

   public final Color getItemListSelection() {
      return item_list.getColor();
   }

   public final Color getRoomDescSelection() {
      return rm_desc.getColor();
   }

   public final Color getRoomShortDescSelection() {
      return rm_short_desc.getColor();
   }

   public final Color getGossipSelection() {
      return gossip.getColor();
   }
   
   public final Color getTellSelection() {
      return tell.getColor();
   }
   
   public final Color getYellSelection() {
      return yell.getColor();
   }
   
   public final Color getAuctionSelection() {
      return auction.getColor();
   }
   
   public final Color getBattleSelection() {
      return battle.getColor();
   }
   
   public final Color getOtherSelection() {
      return other.getColor();
   }

   public final Color getSaySelection() {
      return say.getColor();
   }

   public final Color getBackgroundSelection() {
      return back_ground.getColor();
   }

   public final Color getInputSelection() {
      return input.getColor();
   }


   public final Font getDoorListFont() {
      return door_list.getFont();
   }

   public final Font getMobListFont() {
      return mob_list.getFont();
   }

   public final Font getItemListFont() {
      return item_list.getFont();
   }

   public final Font getRoomDescFont() {
      return rm_desc.getFont();
   }

   public final Font getRoomShortDescFont() {
      return rm_short_desc.getFont();
   }

   public final Font getGossipFont() {
      return gossip.getFont();
   }
   
   public final Font getTellFont() {
      return tell.getFont();
   }
   
   public final Font getYellFont() {
      return yell.getFont();
   }
   
   public final Font getAuctionFont() {
      return auction.getFont();
   }
   
   public final Font getBattleFont() {
      return battle.getFont();
   }
   
   public final Font getOtherFont() {
      return other.getFont();
   }

   public final Font getSayFont() {
      return say.getFont();
   }

   public final Font getInputFont() {
      return input.getFont();
   }

   
   public void do_close() {
      setVisible(false);
      try {
         writeObject();
      }
      catch (IOException e) {
         Log.instance().err("ERROR: trying to write out ColorSelections:  " + e);
      }
   }//do_close

   public void do_help() {
      hm.getHelpFrame().show("colors.bml");
   }

   /** Writes it all to disk, for later retrieval by load() */
   public void writeObject() throws IOException {
      if (hm.IS_APPLET)
         return;
      
      BOStream ostream =
         new BOStream(new FileOutputStream(hm.getProperties().getProperty("ColorFile")));

      try {
         door_list.writeObject(ostream);
         mob_list.writeObject(ostream);
         item_list.writeObject(ostream);
         rm_desc.writeObject(ostream);
         rm_short_desc.writeObject(ostream);
         gossip.writeObject(ostream);
         tell.writeObject(ostream);
         yell.writeObject(ostream);
         auction.writeObject(ostream);
         battle.writeObject(ostream);
         other.writeObject(ostream);
         say.writeObject(ostream);
         back_ground.writeObject(ostream);
         input.writeObject(ostream);
      }//try
      catch (Exception e) {
         MessageDialog md =
           new MessageDialog("Write Failure",
                             "Failed to write Color Selections to disk: " +
                             e, "red", "black");
         e.printStackTrace();
      }//catch
   }//writeObject

   public void readObject() throws IOException {
      String af = hm.getProperties().getProperty("ColorFile");
      BIStream istream;
      
      if (hm.IS_APPLET) {
         URL url = new URL(af);
         istream = new BIStream(url.openStream());
      }
      else {
         istream = new BIStream(new FileInputStream(af));
      }

      String rslt;

      if (istream == null)
         return;
      
      try {
         door_list.readObject(istream);
         mob_list.readObject(istream);
         item_list.readObject(istream);
         rm_desc.readObject(istream);
         rm_short_desc.readObject(istream);
         gossip.readObject(istream);
         tell.readObject(istream);
         yell.readObject(istream);
         auction.readObject(istream);
         battle.readObject(istream);
         other.readObject(istream);
         say.readObject(istream);
         back_ground.readObject(istream);
         input.readObject(istream);
      }//try
      catch (Exception e) {
         new MessageDialog("Error reading color selections file.",
                           e.toString(), "red", "black");
         Log.instance().err("in ColorSelectionManager.readObject():  " + e);
         e.printStackTrace();
      }
   }//readObject
}//ColorSelectionManager


class CSButtons extends Panel {
   ColorSelectionManager parent;
   
   public CSButtons(ColorSelectionManager par) {
      super();
      parent = par;
      
      Button done_b = new Button("Done");

      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_close();
         }});

      Button help_b = new Button("Help");

      help_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_help();
         }});

      FlowLayout fl = new FlowLayout();
      setLayout(fl);

      add(done_b);
      add(help_b);
   }//CSButtons constructor
}//CSButtons
