// $Id$
// $Revision: 1.4 $  $Author$ $Date$

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


class OLCRoom extends Frame {
   ClientDisplay parent;
   HegemonManager hm;
   RmPanel1 descriptions;
   RmPanel2 flags;
   RoomCBListener cb_listener;

   CardLayout cl;
   
   public OLCRoom(ClientDisplay par, HegemonManager h) {

      super("Create a new Room");
      hm = h;
      
      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            dispose();
         }});

      parent = par;

      cb_listener = new RoomCBListener(this);
      
      setLayout (cl = new CardLayout());
      
      descriptions = new RmPanel1(this);
      add(descriptions, "Descriptions");
      
      flags = new RmPanel2(this, (ItemListener)(cb_listener));
      add(flags, "Room Data and Flags");

      pack();
   }//constructor

   void do_cancel() {
      dispose();
   }

   void do_submit() {
      MessageDialog md =
         new MessageDialog("Room Creation", 
                           "The data is being sent to the " +
                           "MUD server.  Please wait for confirmation...",
                           "green", "black");
      parent.submitRoom(this);
   }

   void goto_desc() {
      cl.show(this, "Descriptions");
   }

   void goto_flags() {
      flags.getButtons().touchFlags();
      descriptions.getButtons().touchFlags();
      cl.show(this, "Room Data and Flags");
   }

   void do_help() {
      hm.getHelpFrame().show("olc_room.bml");
   }

   // These objects will be NumCheckBox objects
   public void handleRoomFlagsCB(int cb_num, boolean state) {
      switch (cb_num) {
       case 0: //!rest, grey out rest of restrictions
         flags.handleNoRest(state);
         break;
      }//switch
   }//handleCB

   public void handleVehicleChange(boolean state) {
      flags.handleVehicleChange(state);
   }
   
   /* panel 1:  descriptions */
   public boolean isVehicle() {
      return descriptions.isVehicle();
   }

   public final String getNames() {
      return descriptions.getNames() + " ~";
   }

   public final String getShortDesc() {
      return descriptions.getShortDesc();
   }
   
   public final String getDesc() {
      return descriptions.getLongDesc() + "\n~";
   }
   
   /* panel 2, flags */
   
   public final String getFlags() {
      return flags.getFlags() + " -1";
   }
   
   public final String getCurStats() {
      return flags.getCurStats();
   }
   
   public final String getInv() {
      return flags.getInv() + " -1";
   }
   
   public final String getDistance() {
      return flags.getDistance();
   }
   
   public final String getDirection() {
      return flags.getDirection();
   }

   public final String getVehicleFlags() {
      return (flags.getVehicleFlags() + " -1");
   }

   public final String getVehicleData() {
      return flags.getVehicleData();
   }

}//OLCRoom


/**  Descriptions, isVehicle */
class RmPanel1 extends Panel {
   OLCRoom parent;
   RmPaneButtons opb;

   Checkbox rm_veh;
   LabeledTextField names;
   LabeledTextField short_desc;
   LabeledTextArea long_desc;
   
   public final String getNames() {
      return names.getText();
   }
   
   public final String getShortDesc() {
      return short_desc.getText();
   }
   
   public final String getLongDesc() {
      return long_desc.getText();
   }
   
   public RmPaneButtons getButtons() {
      return opb;
   }
   
   public boolean isVehicle() {
      return rm_veh.getState();
   }

   public RmPanel1(OLCRoom par) {
      super();
      parent = par;
      
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);
      
      
      int REM = GridBagConstraints.REMAINDER;
      c.fill = GridBagConstraints.BOTH;

      c.gridwidth = 1;
      rm_veh = new Checkbox("Vehicle", false);
      rm_veh.addItemListener(new ItemListener() {
         public void itemStateChanged(ItemEvent e) {
            parent.handleVehicleChange(rm_veh.getState());
         }});

      gridbag.setConstraints(rm_veh, c);
      add(rm_veh);
      
      c.gridwidth = REM;
      names = new LabeledTextField("Room name:", "generic", 80);
      gridbag.setConstraints(names, c);
      add(names);
      
      short_desc = new LabeledTextField("Short Description:", 
                                        "The temple of Korg.", 80);
      gridbag.setConstraints(short_desc, c);
      add(short_desc);
      
      long_desc =
        new LabeledTextArea("Long Description:", 
                            "You see yourself in an incomplete room.",
                            6, 80);
      gridbag.setConstraints(long_desc, c);
      add(long_desc);
      
      opb = new RmPaneButtons(parent);
      gridbag.setConstraints(opb, c);
      add(opb);

  }//constructor

}//RmPanel1


/**  Room stats and flags, vehicle data too. */
class RmPanel2 extends Panel {
   OLCRoom parent;
   RmPaneButtons opb;
   
   LabeledCheckBoxGlob flags;
   LabeledTextField vis_bit;
   LabeledTextField mov_cost;
   LabeledTextField inv;
   LabeledTextField dist;
   LabeledTextField dir;

   LabeledCheckBoxGlob veh_flags;
   LabeledTextField time_bet_stops;
   LabeledTextField cur_fuel;
   LabeledTextField max_fuel;
   
   public final String getFlags() {
      return flags.getText();
   }

   public final String getVehicleFlags() {
      return veh_flags.getText();
   }

   public final String getVehicleData() {
      return (time_bet_stops.getText() + " " + cur_fuel.getText() + " "
              + max_fuel.getText());
   }
   
   public final String getCurStats() {
      return (vis_bit.getText() + " " + mov_cost.getText());
   }
   
   public final String getInv() {
      return inv.getText();
   }
   
   public final String getDistance() {
      return dist.getText();
   }
   
   public final String getDirection() {
      return dir.getText();
   }

   public RmPaneButtons getButtons() {
      return opb;
   }
   
   public RmPanel2(OLCRoom par, ItemListener flags_listener) {
      super();
      parent = par;
      
      int REM = GridBagConstraints.REMAINDER;
      
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);
      
      c.gridwidth = REM;
      c.fill = GridBagConstraints.BOTH;
      flags = new LabeledCheckBoxGlob("Room Flags", 6, 4);
      flags.setListener(flags_listener);

      flags.addBox(new NumCheckbox("!restrictions", true, 0));
      flags.addBox(new NumCheckbox("!immort", false, 1));
      flags.addBox(new NumCheckbox("!god", false, 2));
      flags.addBox(new NumCheckbox("perm. dark", false, 3));
      flags.addBox(new NumCheckbox("has weather", false, 4));
      flags.addBox(new NumCheckbox("!shout", false, 5));
      flags.addBox(new NumCheckbox("!mag_exit", false, 6));
      flags.addBox(new NumCheckbox("is haven", false, 7));
      flags.addBox(new NumCheckbox("!PK", false, 8));
      flags.addBox(new NumCheckbox("!magic", false, 9));
      flags.addBox(new NumCheckbox("!mob", false, 10));
      flags.addBox(new NumCheckbox("!potions", false, 11));
      flags.addBox(new NumCheckbox("!staffs", false, 12));
      flags.addBox(new NumCheckbox("!mort", false, 13));
      flags.addBox(new NumCheckbox("normally dark", false, 14));
      flags.addBox(new NumCheckbox("shallow water", false, 15));
      flags.addBox(new NumCheckbox("deep water", false, 16));
      flags.addBox(new NumCheckbox("swamp", false, 17));
      flags.addBox(new NumCheckbox("need fly", false, 18));
      flags.addBox(new NumCheckbox("need boat", false, 19));
      flags.addBox(new NumCheckbox("need climb", false, 20));
      flags.addBox(new NumCheckbox("!mag_entry", false, 24));
      flags.addBox(new NumCheckbox("!vehicles", false, 25));
      flags.addBox(new NumCheckbox("is cramped", false, 26));
      flags.addBox(new NumCheckbox("!ranged combat", false, 27));
      flags.addBox(new NumCheckbox("need dive", false, 28));
      flags.addBox(new NumCheckbox("!Mob Wander", false, 33));
      flags.addBox(new NumCheckbox("!Foreign Mob Wander", false, 34));
      
      gridbag.setConstraints(flags, c);
      add(flags);


      c.gridwidth = 1;
      vis_bit = new LabeledTextField("Vis Bit", "0", 12);
      gridbag.setConstraints(vis_bit, c);
      add(vis_bit);
      
      mov_cost = new LabeledTextField("Movement Cost", "1", 12);
      gridbag.setConstraints(mov_cost, c);
      add(mov_cost);
      
      c.gridwidth = REM;
      inv = new LabeledTextField("Inventory", "", 40);
      gridbag.setConstraints(inv, c);
      add(inv);
      
      c.gridwidth = 1;
      dist = new LabeledTextField("Distance to Room", "0", 12);
      gridbag.setConstraints(dist, c);
      add(dist);
      
      c.gridwidth = REM;
      dir = new LabeledTextField("Direction to Room", "East", 25);
      gridbag.setConstraints(dir, c);
      add(dir);
      
      // vehicle data
      veh_flags = new LabeledCheckBoxGlob("Vehicle Flags", 4, 3);

      veh_flags.addBox(new NumCheckbox("is_self_guided", true, 0));
      veh_flags.addBox(new NumCheckbox("is_stealthy", false, 1));
      veh_flags.addBox(new NumCheckbox("has_unlimited_fuel", true, 2));
      veh_flags.addBox(new NumCheckbox("can_fly", false, 3));
      veh_flags.addBox(new NumCheckbox("can_climb", false, 4));
      veh_flags.addBox(new NumCheckbox("can_float", false, 5));
      veh_flags.addBox(new NumCheckbox("can_dive", false, 6));
      veh_flags.addBox(new NumCheckbox("can_see_out", true, 7));

      gridbag.setConstraints(veh_flags, c);
      add(veh_flags);
      
      c.gridwidth = 1;
      time_bet_stops = new LabeledTextField("Time Between Stops", "8", 10);
      gridbag.setConstraints(time_bet_stops, c);
      add(time_bet_stops);

      cur_fuel = new LabeledTextField("Current Fuel", "300", 10);
      gridbag.setConstraints(cur_fuel, c);
      add(cur_fuel);

      c.gridwidth = REM;
      max_fuel = new LabeledTextField("Maximum Fuel", "300", 10);
      gridbag.setConstraints(max_fuel, c);
      add(max_fuel);


      opb = new RmPaneButtons(parent);
      gridbag.setConstraints(opb, c);
      add(opb);

      handleNoRest(true);
      handleVehicleChange(false);
   }//constructor
   
   public void handleNoRest(boolean state) {
      if (state) {
         flags.setStateRange(1, 2, false);
         flags.setEnableRange(1, 2, false);
         flags.setStateRange(5, 6, false);
         flags.setEnableRange(5, 6, false);
         flags.setStateRange(8, 13, false);
         flags.setEnableRange(8, 13, false);
         flags.setStateRange(25, 25, false);
         flags.setEnableRange(25, 25, false);
         flags.setStateRange(27, 27, false);
         flags.setEnableRange(27, 27, false);
      }
      else {
         flags.setEnableRange(1, 2, true);
         flags.setEnableRange(5, 6, true);
         flags.setEnableRange(8, 13, true);
         flags.setEnableRange(25, 25, true);
         flags.setEnableRange(27, 27, true);
      }
   }//handleNoRest

   public void handleVehicleChange(boolean state) {
      if (state) {
         veh_flags.setEnabled(true);
         time_bet_stops.setEnabled(true);
         cur_fuel.setEnabled(true);
         max_fuel.setEnabled(true);
      }
      else {
         veh_flags.setEnabled(false);
         time_bet_stops.setEnabled(false);
         cur_fuel.setEnabled(false);
         max_fuel.setEnabled(false);
      }
   }//handleVehicleChange
}//RmPanel2


class RmPaneButtons extends Panel {
   OLCRoom parent;
   Button flags;
   Button desc;
   
   public RmPaneButtons(OLCRoom par) {
      super();
      parent = par;
      
      Button cancel = new Button("Cancel");
      Button submit = new Button("Submit");
      desc = new Button("Descriptions");
      flags = new Button("Room Stats and Flags");
      Button help = new Button("Help");

      desc.setBackground(new Color(0, 255, 0));
      flags.setBackground(new Color(255, 0, 0));
      
      FlowLayout fl = new FlowLayout();
      setLayout(fl);
      
      desc.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.goto_desc();
         }});
      
      help.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_help();
         }});
      
      flags.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.goto_flags();
         }});
      
      cancel.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_cancel();
         }});
      
      submit.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_submit();
         }});
      
      add(submit);
      add(cancel);
      add(new Button("#######"));
      add(desc);
      add(flags);
      add(help);
      
   }//constructor

   public void touchDesc() {
      desc.setBackground(new Color(0, 255, 0));
   }

   public void touchFlags() {
      flags.setBackground(new Color(0, 255, 0));
   }
   
}//RButtonPanel


/** This class will listen to all check boxes and 
  tell the OLCObject class about them. */
class RoomCBListener implements ItemListener {
   OLCRoom par;
   
   public RoomCBListener(OLCRoom ob) {
      super();
      par = ob;
   }

   public void itemStateChanged(ItemEvent e) {
      NumCheckbox ncb = (NumCheckbox)(e.getSource());
      par.handleRoomFlagsCB(ncb.getNum(), ncb.getState());
   }
}//RoomCBListener
