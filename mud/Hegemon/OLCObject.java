// $Id: OLCObject.java,v 1.4 1999/06/05 23:29:12 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date: 1999/06/05 23:29:12 $

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

class OLCObject extends Frame {
   ClientDisplay parent;
   HegemonManager hm;
   ObjPanel1 descriptions;
   ObjPanel2 obj_flags;
   ObjPanel3 extras_bag;
   ObjPanel4 spec_data;
   CardLayout cl;
   ObjCBListener cb_listener;
   
   public OLCObject(ClientDisplay par, HegemonManager h) {
      
      super("Create a new Object");
      hm = h;
      parent = par;
      
      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            dispose();
         }});


      cb_listener = new ObjCBListener(this);
      
      setLayout (cl = new CardLayout());
      
      descriptions = new ObjPanel1(this);
      add(descriptions, "Descriptions");
      
      obj_flags = new ObjPanel2(this, (ItemListener)(cb_listener));
      add(obj_flags, "Object Flags");
      
      extras_bag = new ObjPanel3(this);
      add(extras_bag, "Bag, Other Data");
      
      spec_data = new ObjPanel4(this);
      add(spec_data, "Special Procedures/Properties");

      // Lets set up the enabled/disabled fields
      obj_flags.handleNoRest(obj_flags.getState(0));
      obj_flags.handleNoWorn(obj_flags.getState(21));
      obj_flags.handleNoWorn(obj_flags.getState(40));
      obj_flags.handleContainer(false);
      extras_bag.handleContainer(false);
      spec_data.handleSpecProcs(false);
      
      pack();
   }//constructor
   
   // These objects will be NumCheckBox objects
   public void handleDescFlagsCB(int cb_num, boolean state) {
      switch (cb_num) {
       case 0: //!rest, grey out rest of restrictions
         obj_flags.handleNoRest(state);
         break;
       case 21: // !worn, grey out worn flags
         obj_flags.handleNoWorn(state);
         break;
       case 40: // !weapon
         obj_flags.handleNoWeapon(state);
         break;
      case 51: //wand
      case 52: //potion
      case 53: //scroll
         spec_data.handleSpells(state);
         break;
       case 54: //container
         obj_flags.handleContainer(state);
         extras_bag.handleContainer(state || obj_flags.getState(59));
         break;
       case 59: //canteen
         obj_flags.handleCanteen(state);
         extras_bag.handleContainer(state || obj_flags.getState(54));
         break;
       case 63: //SPEC PROCS
         spec_data.handleSpecProcs(state);
         break;
       case 73: //vend machine
         break;
      }//switch
   }//handleCB
   
   void do_cancel() {
      dispose();
   }
   
   void do_help() {
      hm.getHelpFrame().show("olc_object.bml");
   }
   
   void do_submit() {
      MessageDialog md =
         new MessageDialog("Object Creation",
                           "The object data is being sent to the mud server"
                           + " now.  Please check main console for"
                           + " confirmation.",
                           "green", "black");
      parent.submitObj(this);
   }

   void goto_desc() {
      cl.show(this, "Descriptions");
   }

   void goto_bag() {
      descriptions.getButtons().touchBag();
      obj_flags.getButtons().touchBag();
      extras_bag.getButtons().touchBag();
      spec_data.getButtons().touchBag();
      cl.show(this, "Bag, Other Data");
   }

   void goto_flags() {
      descriptions.getButtons().touchFlags();
      obj_flags.getButtons().touchFlags();
      extras_bag.getButtons().touchFlags();
      spec_data.getButtons().touchFlags();
      cl.show(this, "Object Flags");
   }
   
   void goto_spec() {
      descriptions.getButtons().touchSpec();
      obj_flags.getButtons().touchSpec();
      extras_bag.getButtons().touchSpec();
      spec_data.getButtons().touchSpec();
      cl.show(this, "Special Procedures/Properties");
   }
   
   /*---------------------------------------------------------------------*/
   /*  Here lie the functions which the OlcStore querries in order to
       satisfy requests for OLC input from the server. */
   
   /* from panel 1 */
   public final String getObjNum() {
      return (descriptions.getObjNum());
   }
   
   public final String getNames() {
      return (descriptions.getNames() + " ~");
   }
   
   public final String getShortDesc() {
      return descriptions.getShortDesc();
   }
   
   public final String getInRoomDesc() {
      return descriptions.getInRoomDesc();
   }
   
   public final String getDesc() {
      return (descriptions.getLongDesc() + "\n~");
   }
   
   /* from panel 2 */
   public final String getObjFlags() {
      return obj_flags.getObjFlags() + " -1";
   }
   
   /* from panel 3 */
   
   public final String getStatEffects() {
      return extras_bag.getStatEffects() + " -1";
   }
   
   public final String getInv() {
      return extras_bag.getInv() + " -1";
   }
   
   public final String getExtras() {
      return extras_bag.getExtras();
   }
   
   public final String getVbPrice() {
      return extras_bag.getVbPrice();
   }
   
   public final String getBagFlags() {
      return extras_bag.getBagFlags() + " -1";
   }
   
   public final String getBagData() {
      return extras_bag.getBagData();
   }
   
   /*  panel 4 */
   
//   public final String getConstructData() {
//      return spec_data.getConstructData();
//   }
   
   public final String getSpecDataFlags() {
      return spec_data.getSpecDataFlags() + " -1";
   }
   
   public final String getSkinNum() {
      return spec_data.getSkinNum();
   }
   
   public final String getCastsSpells() {
      return spec_data.getCastsSpells() + " -1";
   }
   
}//OLCObject


class ObjPanel1 extends Panel {
   OLCObject parent;
   ObjPaneButtons opb;

   LabeledTextField obj_num;
   LabeledTextField names;
   LabeledTextField short_desc;
   LabeledTextField in_room_desc;
   LabeledTextArea long_desc;

   public ObjPanel1(OLCObject par) {
      super();
      parent = par;
      
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);
      

      int REM = GridBagConstraints.REMAINDER;
      c.fill = GridBagConstraints.BOTH;

      c.gridwidth = 1;
      obj_num = new LabeledTextField("Object Number", "1", 20);
      gridbag.setConstraints(obj_num, c);
      add(obj_num);

      c.gridwidth = REM;
      names = new LabeledTextField("Object names:", "generic", 60);
      gridbag.setConstraints(names, c);
      add(names);

      short_desc = new LabeledTextField("Short Description:", 
                                        "a generic object", 80);
      gridbag.setConstraints(short_desc, c);
      add(short_desc);

      in_room_desc = new LabeledTextField("In Room Desc:", 
                                          "A generic object lies here.", 80);
      gridbag.setConstraints(in_room_desc, c);
      add(in_room_desc);
      
      long_desc = new LabeledTextArea("Long Description:", 
                                      "A generic object looks kinda sad.",
                                      6, 80);
      gridbag.setConstraints(long_desc, c);
      add(long_desc);
      
      opb = new ObjPaneButtons(parent);
      gridbag.setConstraints(opb, c);
      add(opb);
      
   }//constructor

   public final String getObjNum() {
      return obj_num.getText();
   }

   public final String getNames() {
      return names.getText();
   }

   public final String getShortDesc() {
      return short_desc.getText();
   }
   
   public final String getInRoomDesc() {
      return in_room_desc.getText();
   }
   
   public final String getLongDesc() {
      return long_desc.getText();
   }

   public ObjPaneButtons getButtons() {
      return opb;
   }
   
}//ObjPanel1


class ObjPanel2 extends Panel {
   OLCObject parent;
   ObjPaneButtons opb;

   LabeledCheckBoxGlob rest_flags;
   LabeledCheckBoxGlob worn_flags;
   LabeledCheckBoxGlob desc_flags;

   public final String getObjFlags() {
      return (rest_flags.getText() + " " + worn_flags.getText() +
              " " + desc_flags.getText());
   }
   
   public ObjPanel2(OLCObject par, ItemListener cb_listener) {
      super();
      parent = par;
      
      int REM = GridBagConstraints.REMAINDER;
      
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);

      c.gridwidth = REM;
      c.fill = GridBagConstraints.BOTH;
      rest_flags = new LabeledCheckBoxGlob("Restriction Flags", 4, 5);
      rest_flags.setListener(cb_listener);

      rest_flags.addBox(new NumCheckbox("!rest", true, 0));
      rest_flags.addBox(new NumCheckbox("!evil", false, 1));
      rest_flags.addBox(new NumCheckbox("!neutral", false, 2));
      rest_flags.addBox(new NumCheckbox("!good", false, 3));
      rest_flags.addBox(new NumCheckbox("!donate", false, 4));
      rest_flags.addBox(new NumCheckbox("!drop", false, 5));
      rest_flags.addBox(new NumCheckbox("!remove", false, 6));
      rest_flags.addBox(new NumCheckbox("!mortal", false, 7));
      rest_flags.addBox(new NumCheckbox("!immort", false, 8));
      rest_flags.addBox(new NumCheckbox("!demi", false, 9));
      rest_flags.addBox(new NumCheckbox("!warrior", false, 11));
      rest_flags.addBox(new NumCheckbox("!sage", false, 12));
      rest_flags.addBox(new NumCheckbox("!wizard", false, 13));
      rest_flags.addBox(new NumCheckbox("!ranger", false, 14));
      rest_flags.addBox(new NumCheckbox("!thief", false, 15));
      rest_flags.addBox(new NumCheckbox("!alchemist", false, 16));
      rest_flags.addBox(new NumCheckbox("!cleric", false, 17));
      rest_flags.addBox(new NumCheckbox("!bard", false, 18));
      rest_flags.addBox(new NumCheckbox("!mob", false, 19));
      rest_flags.addBox(new NumCheckbox("!pc", false, 20));
      
      gridbag.setConstraints(rest_flags, c);
      add(rest_flags);
      
      worn_flags = new LabeledCheckBoxGlob("Where Worn Flags", 4, 5);
      worn_flags.setListener(cb_listener);
      
      worn_flags.addBox(new NumCheckbox("!worn", true, 21));
      worn_flags.addBox(new NumCheckbox("head", false, 22));
      worn_flags.addBox(new NumCheckbox("neck", false, 23));
      worn_flags.addBox(new NumCheckbox("neck", false, 24));
      worn_flags.addBox(new NumCheckbox("arnd bdy", false, 25));
      worn_flags.addBox(new NumCheckbox("arms", false, 26));
      worn_flags.addBox(new NumCheckbox("wrist1", false, 27));
      worn_flags.addBox(new NumCheckbox("wrist2", false, 28));
      worn_flags.addBox(new NumCheckbox("hands", false, 29));
      worn_flags.addBox(new NumCheckbox("wielded", false, 30));
      worn_flags.addBox(new NumCheckbox("held", false, 31));
      worn_flags.addBox(new NumCheckbox("light", false, 32));
      worn_flags.addBox(new NumCheckbox("body", false, 33));
      worn_flags.addBox(new NumCheckbox("belt", false, 34));
      worn_flags.addBox(new NumCheckbox("legs", false, 35));
      worn_flags.addBox(new NumCheckbox("feet", false, 36));
      worn_flags.addBox(new NumCheckbox("finger1", false, 37));
      worn_flags.addBox(new NumCheckbox("finger2", false, 38));
      worn_flags.addBox(new NumCheckbox("shield", false, 39));

      gridbag.setConstraints(worn_flags, c);
      add(worn_flags);


      desc_flags = new LabeledCheckBoxGlob("Description Flags", 8, 5);
      desc_flags.setListener(cb_listener);
      
      desc_flags.addBox(new NumCheckbox("!weapon", true, 40));
      desc_flags.addBox(new NumCheckbox("slash", false, 41));
      desc_flags.addBox(new NumCheckbox("smash", false, 42));
      desc_flags.addBox(new NumCheckbox("pierce", false, 43));
      desc_flags.addBox(new NumCheckbox("whip", false, 44));
      desc_flags.addBox(new NumCheckbox("needs ammo", false, 45));
      desc_flags.addBox(new NumCheckbox("dart thr.", false, 46));
      desc_flags.addBox(new NumCheckbox("bow", false, 47));
      desc_flags.addBox(new NumCheckbox("dart", false, 48));
      desc_flags.addBox(new NumCheckbox("arrow", false, 49));
      desc_flags.addBox(new NumCheckbox("junk", false, 50));
      desc_flags.addBox(new NumCheckbox("wand", false, 51));
      desc_flags.addBox(new NumCheckbox("potion", false, 52));
      desc_flags.addBox(new NumCheckbox("scroll", false, 53));
      desc_flags.addBox(new NumCheckbox("container", false, 54));
      desc_flags.addBox(new NumCheckbox("coins", false, 55));
      desc_flags.addBox(new NumCheckbox("armor", false, 56));
      desc_flags.addBox(new NumCheckbox("weapon", false, 57));
      desc_flags.addBox(new NumCheckbox("light src", false, 58));
      desc_flags.addBox(new NumCheckbox("canteen", false, 59));
      desc_flags.addBox(new NumCheckbox("liquid", false, 60));
      desc_flags.addBox(new NumCheckbox("food", false, 61));
      desc_flags.addBox(new NumCheckbox("boat", false, 62));
      desc_flags.addBox(new NumCheckbox("SPEC PROCs", false, 63));
      desc_flags.addBox(new NumCheckbox("toolbox", false, 64));
      desc_flags.addBox(new NumCheckbox("cauldron", false, 65));
      desc_flags.addBox(new NumCheckbox("pen", false, 66));
      //desc_flags.addBox(new NumCheckbox("constr cmp", false, 67));
      //desc_flags.addBox(new NumCheckbox("conct cmp", false, 68));
      desc_flags.addBox(new NumCheckbox("parchment", false, 69));
      //desc_flags.addBox(new NumCheckbox("vid scrn", false, 71));
      desc_flags.addBox(new NumCheckbox("herb", false, 72));
      desc_flags.addBox(new NumCheckbox("vend mach.", false, 73));
      desc_flags.addBox(new NumCheckbox("bul. board", false, 74));
      desc_flags.addBox(new NumCheckbox("butcherable", false, 75));

      gridbag.setConstraints(desc_flags, c);
      add(desc_flags);

      opb = new ObjPaneButtons(parent);
      gridbag.setConstraints(opb, c);
      add(opb);

      // set up starting state info, disable some things...
      handleNoRest(true);
      handleNoWorn(true);
      handleNoWeapon(true);
      
   }//constructor

   public boolean getState(int box_num) {
      if ((box_num < 0) || (box_num > 75))
         return false;

      if (box_num <= 20)
        return rest_flags.getState(box_num);
      else if (box_num <= 39)
         return worn_flags.getState(box_num);
      else
        return desc_flags.getState(box_num);
   }//getState

   public void handleContainer(boolean state) {
      if (!state) {
         desc_flags.setState(59, false);
      }
   }//handleContainer

   public void handleCanteen(boolean state) {
      if (state)
         desc_flags.setState(54, true); //if canteen, then container
   }//handleCanteen

   public void handleNoRest(boolean state) {
      if (state) { //!rest chosen
         rest_flags.setStateRange(1, 20, false);
         rest_flags.setEnableRange(1, 20, false);
      }
      else {
         rest_flags.setEnableRange(1, 20, true);
      }         
   }//handleNoRest

   public void handleNoWorn(boolean state) {
      if (state) { //!worn chosen
         worn_flags.setStateRange(22, 39, false);
         worn_flags.setEnableRange(22, 39, false);
      }
      else {
         worn_flags.setEnableRange(22, 39, true);
      }         
   }//handleNoWorn

   public void handleNoWeapon(boolean state) {
      if (state) { //!weapon chosen
         desc_flags.setStateRange(41, 49, false);
         desc_flags.setEnableRange(41, 49, false);
      }
      else {
         desc_flags.setEnableRange(41, 49, true);
      }         
   }//handleNoWorn

   public ObjPaneButtons getButtons() {
      return opb;
   }
   
}//ObjPanel2


class ObjPanel3 extends Panel {
   OLCObject parent;
   ObjPaneButtons opb;
   
   LabeledTextBoxGlob extras;
   LabeledCheckBoxGlob bag_flags;
   LabeledTextBoxGlob bag_data;
   LabeledTextField inv;
   LabeledTextField stat_effects;
   LabeledTextField vis_bit;
   LabeledTextField price;
   
   ///**********************  Constructor *****************************///
   public ObjPanel3(OLCObject par) {
      super();
      parent = par;
      
      int REM = GridBagConstraints.REMAINDER;
      int REL = GridBagConstraints.RELATIVE;
      
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);
      
      c.gridwidth = REM;
      c.fill = GridBagConstraints.BOTH;

      extras = new LabeledTextBoxGlob("Miscellaneous Data.", 3, 3);

      extras.addBox(new NumTextBox("Charges/Light count", "0", 0));
      extras.addBox(new NumTextBox("Recharges (wands)", "0", 1));
      extras.addBox(new NumTextBox("Load Percentage", "50", 2));
      //extras.addBox(new NumTextBox("Cur in game", "0", 3));
      extras.addBox(new NumTextBox("Max in game", "50", 3));
      extras.addBox(new NumTextBox("Weight", "5", 4));
      extras.addBox(new NumTextBox("Dice sides", "0", 5));
      extras.addBox(new NumTextBox("Dice count", "0", 6));
      extras.addBox(new NumTextBox("Level", "0", 7));

      gridbag.setConstraints(extras, c);
      add(extras);
      
      bag_flags = new LabeledCheckBoxGlob("Bag Flags", 2, 4);
      
      bag_flags.addBox(new NumCheckbox("closed", true, 2));
      bag_flags.addBox(new NumCheckbox("locked", false, 3));
      bag_flags.addBox(new NumCheckbox("pickable", false, 4));
      bag_flags.addBox(new NumCheckbox("mag lockable", false, 5));
      bag_flags.addBox(new NumCheckbox("mag locked", false, 6));
      bag_flags.addBox(new NumCheckbox("destructable", false, 7));
      bag_flags.addBox(new NumCheckbox("corpse", false, 8));
      bag_flags.addBox(new NumCheckbox("!close", false, 9));
      bag_flags.addBox(new NumCheckbox("consumes key", false, 17));
      
      gridbag.setConstraints(bag_flags, c);
      add(bag_flags);
      
      bag_data = new LabeledTextBoxGlob("Bag Data.", 1, 4);
      
      bag_data.addBox(new NumTextBox("Key number", "0", 0));
      bag_data.addBox(new NumTextBox("Max weight", "30", 1));
      bag_data.addBox(new NumTextBox("Percent Weight", "100", 2));
      bag_data.addBox(new NumTextBox("Time Till Disolve", "-1", 3));
      
      gridbag.setConstraints(bag_data, c);
      add(bag_data);
      
      c.gridwidth = 1;
      inv = new LabeledTextField("Inventory", "", 40);
      gridbag.setConstraints(inv, c);
      add(inv);
      
      c.gridwidth = REM;
      stat_effects = new LabeledTextField("Stat Effects", "", 40);
      gridbag.setConstraints(stat_effects, c);
      add(stat_effects);
      
      c.gridwidth = 1;
      vis_bit = new LabeledTextField("Vis Bit", "0", 15);
      gridbag.setConstraints(vis_bit, c);
      add(vis_bit);
      
      c.gridwidth = REM;
      price = new LabeledTextField("Price", "500", 15);
      gridbag.setConstraints(price, c);
      add(price);
      
      opb = new ObjPaneButtons(parent);
      gridbag.setConstraints(opb, c);
      add(opb);
      
   }//Constructor

   public void handleContainer(boolean is_container) {
      if (is_container) {
         bag_flags.setEnabled(true);
         bag_data.setEnabled(true);
         inv.setEnabled(true);
      }
      else {
         bag_flags.setEnabled(false);
         bag_data.setEnabled(false);
         inv.setEnabled(false);
      }
   }//handleContainer
   
   public final String getInv() {
      return inv.getText();
   }
   
   public final String getStatEffects() {
      return stat_effects.getText();
   }
   
   public final String getExtras() {
      return extras.getText();
   }
   
   public final String getVbPrice() {
      return (vis_bit.getText() + " " + price.getText());
   }
   
   public final String getBagFlags() {
      return bag_flags.getText();
   }
   
   public final String getBagData() {
      return bag_data.getText();
   }
   
   public ObjPaneButtons getButtons() {
      return opb;
   }   

}//ObjPanel3


class ObjPanel4 extends Panel {
   OLCObject parent;
   ObjPaneButtons opb;

   LabeledCheckBoxGlob spec_data_flags;
   //LabeledTextBoxGlob construct_data;
   LabeledTextField skin_num;
   LabeledTextField casts_spells;
   
   public ObjPanel4(OLCObject par) {
      super();
      parent = par;
      
      int REM = GridBagConstraints.REMAINDER;
      int REL = GridBagConstraints.RELATIVE;
      
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);
      
      c.gridwidth = REM;
      c.fill = GridBagConstraints.BOTH;
      
      spec_data_flags = new LabeledCheckBoxGlob("Special Proc Flags", 2, 3);
      
      spec_data_flags.addBox(new NumCheckbox("Teleport on Consumption", false,
                                             0));
      //spec_data_flags.addBox(new NumCheckbox("CONSTRUCT DATA", false, 1));
      spec_data_flags.addBox(new NumCheckbox("Has a skin", false, 2));
      spec_data_flags.addBox(new NumCheckbox("Poison on Consumpt.", false, 3));
      spec_data_flags.addBox(new NumCheckbox("Casts Spells", false, 10));
      
      gridbag.setConstraints(spec_data_flags, c);
      add(spec_data_flags);
      
//       construct_data =
//         new LabeledTextBoxGlob("Construct/Concoct Data.  Enter the NUMBERS"
//                                + "of the objects involved.", 2, 3);

//       construct_data.addBox(new NumTextBox("Resulting Object", "0", 0));
//       construct_data.addBox(new NumTextBox("Component 1", "0", 1));
//       construct_data.addBox(new NumTextBox("Comp 2", "0", 2));
//       construct_data.addBox(new NumTextBox("Comp 3", "0", 3));
//       construct_data.addBox(new NumTextBox("Comp 4", "0", 4));
//       construct_data.addBox(new NumTextBox("Comp 5", "0", 5));
      
      
//       gridbag.setConstraints(construct_data, c);
//       add(construct_data);
      
      c.gridwidth = 1;
      skin_num = new LabeledTextField("Skin number", "0", 8);
      gridbag.setConstraints(skin_num, c);
      add(skin_num);
      
      c.gridwidth = REM;
      casts_spells =
        new LabeledTextField("Casts these Spells <level spell_num> pairs",
                             "", 50);
      gridbag.setConstraints(casts_spells, c);
      add(casts_spells);
      
      opb = new ObjPaneButtons(parent);
      gridbag.setConstraints(opb, c);
      add(opb);
      
   }//Constructor

   public void handleSpecProcs(boolean state) {
      spec_data_flags.setEnabled(state);
      //construct_data.setEnabled(state);
      casts_spells.setEnabled(state);
   }

   public void handleSpells(boolean state) {
      //this only goes one way..easy, not to misleading I think
      // Other things can cause this to be turned on is the reason.
      if (state)
        casts_spells.setEnabled(true);
   }

   public void handleSkin(boolean state) {
      skin_num.setEnabled(state);
   }
   

   public final String getSpecDataFlags() {
      return spec_data_flags.getText();
   }
   
//   public final String getConstructData() {
//      return construct_data.getText();
//   }
   
   public final String getSkinNum() {
      return skin_num.getText();
   }
   
   public final String getCastsSpells() {
      return casts_spells.getText();
   }
   
   
   public ObjPaneButtons getButtons() {
      return opb;
   }
   
}//ObjPanel4



class ObjPaneButtons extends Panel {
   OLCObject parent;
   Button desc;
   Button bag;
   Button flags;
   Button spec;

   public ObjPaneButtons(OLCObject par) {
      super();
      parent = par;
      
      Button cancel = new Button("Cancel");
      Button submit = new Button("Submit");
      desc = new Button("Descriptions");
      bag = new Button("Bag/Misc Data");
      flags = new Button("Object Flags");
      spec = new Button("Spec Procs");
      Button help = new Button("Help");

      desc.setBackground(new Color(0, 255, 0));
      bag.setBackground(new Color(255, 0, 0));
      flags.setBackground(new Color(255, 0, 0));
      spec.setBackground(new Color(255, 0, 0));
      
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
      
      bag.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.goto_bag();
         }});
      
      flags.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.goto_flags();
         }});
      
      spec.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.goto_spec();
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
      add(bag);
      add(spec);
      add(help);
      
   }//constructor

   public void touchFlags() {
      flags.setBackground(new Color(0, 255, 0));
   }
   
   public void touchBag() {
      bag.setBackground(new Color(0, 255, 0));
   }
   
   public void touchSpec() {
      spec.setBackground(new Color(0, 255, 0));
   }
   
}//OButtonPanel


/** This class will listen to all check boxes and 
  tell the OLCObject class about them. */
class ObjCBListener implements ItemListener {
   OLCObject par;
   
   public ObjCBListener(OLCObject ob) {
      super();
      par = ob;
   }

   public void itemStateChanged(ItemEvent e) {
      NumCheckbox ncb = (NumCheckbox)(e.getSource());
      par.handleDescFlagsCB(ncb.getNum(), ncb.getState());
   }
}//ObjCBListener
