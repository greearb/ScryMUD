// $Id: OLCMob.java,v 1.6 2001/03/31 07:06:00 greear Exp $
// $Revision: 1.6 $  $Author: greear $ $Date: 2001/03/31 07:06:00 $

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


class OLCMob extends Frame {
   ClientDisplay parent;
   HegemonManager hm;
   MobPanel1 descriptions;
   MobPanel2 mob_flags;
   MobPanel4 short_cur_stats;
   MobPanel5 eq_af_inv;
   MobPanel6 mob_data;
   MobCBListener cb_listener;
   
   CardLayout cl;

   public OLCMob(ClientDisplay par, HegemonManager h) {

      super("Create a new Critter");
      hm = h;
      parent = par;

      addWindowListener (new WindowAdapter () {
         public void windowClosing (WindowEvent e) {
            dispose();
         }});

      cb_listener = new MobCBListener(this);
          
      setLayout (cl = new CardLayout());
      
      descriptions = new MobPanel1(this);
      add(descriptions, "Descriptions");
      
      mob_flags = new MobPanel2(this, (ItemListener)(cb_listener));
      add(mob_flags, "Mob Flags");
      
      short_cur_stats = new MobPanel4(this);
      add(short_cur_stats, "Various Stats");
      
      eq_af_inv = new MobPanel5(this);
      add(eq_af_inv, "Equipment");
      
      mob_data = new MobPanel6(this);
      add(mob_data, "Mob Data");

      //set up defaults for enabled/disabled fields
      eq_af_inv.handleProcData(false);
      mob_data.handleProcData(false);

      
      pack();
   }//constructor


   // These objects will be NumCheckBox objects
   public void handleMobFlagsCB(int cb_num, boolean state) {
      switch (cb_num) {
       case 0: //HAS PROC DATA
         eq_af_inv.handleProcData(state);
         mob_data.handleProcData(state);
      }//switch
   }//handleCB

   
   void do_cancel() {
      dispose();
   }

   void do_help() {
      hm.getHelpFrame().show("olc_mob.bml");
   }
   
   void do_submit() {
      MessageDialog md =
        new MessageDialog("Critter Creation",
                          "The data is being sent to the server, " +
                          "please wait for a confirmation.",
                          "green", "black");
      parent.submitMob(this);
   }

   void goto_desc() {
      cl.show(this, "Descriptions");
   }

   void goto_mob_flags() {
      descriptions.getButtons().touchFlags();
      mob_flags.getButtons().touchFlags();
      short_cur_stats.getButtons().touchFlags();
      eq_af_inv.getButtons().touchFlags();
      mob_data.getButtons().touchFlags();
      cl.show(this, "Mob Flags");
   }

   void goto_short_cur_stats() {
      descriptions.getButtons().touchCurStats();
      mob_flags.getButtons().touchCurStats();
      short_cur_stats.getButtons().touchCurStats();
      eq_af_inv.getButtons().touchCurStats();
      mob_data.getButtons().touchCurStats();
      cl.show(this, "Various Stats");
   }

   void goto_eq_af_inv() {
      descriptions.getButtons().touchEq();
      mob_flags.getButtons().touchEq();
      short_cur_stats.getButtons().touchEq();
      eq_af_inv.getButtons().touchEq();
      mob_data.getButtons().touchEq();
      cl.show(this, "Equipment");
   }

   void goto_mob_data() {
      descriptions.getButtons().touchMobData();
      mob_flags.getButtons().touchMobData();
      short_cur_stats.getButtons().touchMobData();
      eq_af_inv.getButtons().touchMobData();
      mob_data.getButtons().touchMobData();
      cl.show(this, "Mob Data");
   }

   ///*****************  Access Functions ***********************///


  public final String getNames() {
     return descriptions.getNames() + " ~";
  }

  public final String getShortDesc() {
     return descriptions.getShortDesc();
  }

  public final String getInRoomDesc() {
     return descriptions.getInRoomDesc();
  }

  public final String getDesc() {
     return descriptions.getLongDesc() + "\n~";
  }
   
   /* from panel 2 */
  public final String getMobFlags() {
     return mob_flags.getMobFlags() + " -1";
  }

   public final String getLongData() {
      return mob_flags.getLongData();
   }

   public final String getMobDataFlags() {
      return mob_flags.getMobDataFlags() + " -1";
   }

   public final String getCurStats() {
      return mob_flags.getCurStats();
   }

   /*  from panel 3 */
   public final String getShortCurStats1() {
      return short_cur_stats.getShortCurStats1();
   }

   public final String getShortCurStats2() {
      return short_cur_stats.getShortCurStats2();
   }

   public final String getShortCurStats3() {
      return short_cur_stats.getShortCurStats3();
   }

   /* from panel 4 */

   public final String getEQ() {
      return eq_af_inv.getEQ() + " -1";
   }

   public final String getAffectedBy() {
      return eq_af_inv.getAffectedBy() + " -1";
   }

   public final String getInv() {
      return eq_af_inv.getInv() + " -1";
   }

   public final String getMxInGame() {
      return eq_af_inv.getMxInGame();
   }

   public final String getSkinNum() {
      return eq_af_inv.getSkinNum();
   }

   public final String getSpecData() {
      return eq_af_inv.getSpecData();
   }

   public final String getSpecDataFlags() {
      return eq_af_inv.getSpecDataFlags() + " -1";
   }

   /* from panel 5 */

   public final String getPermInv() {
      return mob_data.getPermInv() + " -1";
   }

   public final String getTeachDataFlags() {
      return mob_data.getTeachDataFlags() + " -1";
   }

   public final String getShopData() {
      return mob_data.getShopData();
   }
   
   public final String getShopDataFlags() {
      return mob_data.getShopDataFlags() + " -1";
   }

}//OLCMob


class MobPanel1 extends Panel {
   OLCMob parent;
   MobPaneButtons opb;

  LabeledTextField names;
  LabeledTextField short_desc;
  LabeledTextField in_room_desc;
  LabeledTextArea long_desc;

  public MobPanel1(OLCMob par) {
    super();
    parent = par;

    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    setLayout (gridbag);


    int REM = GridBagConstraints.REMAINDER;
    c.gridwidth = REM;
    c.fill = GridBagConstraints.BOTH;

    names = new LabeledTextField("Mob names:", "generic", 80);
    gridbag.setConstraints(names, c);
    add(names);

    short_desc = new LabeledTextField("Short Description:", 
				      "a generic critter", 80);
    gridbag.setConstraints(short_desc, c);
    add(short_desc);

    in_room_desc = new LabeledTextField("In Room Desc:", 
				      "A generic critter stands here.", 80);
    gridbag.setConstraints(in_room_desc, c);
    add(in_room_desc);

    long_desc = new LabeledTextArea("Long Description:", 
				 "A generic critter looks hopelessly vague.",
				    6, 80);
    gridbag.setConstraints(long_desc, c);
    add(long_desc);

    opb = new MobPaneButtons(parent);
    gridbag.setConstraints(opb, c);
    add(opb);

  }//constructor

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

  public MobPaneButtons getButtons() {
     return opb;
  }
   
}//MobPanel1


class MobPanel2 extends Panel {
   OLCMob parent;
   MobPaneButtons opb;

   LabeledCheckBoxGlob mob_flags;
   LabeledCheckBoxGlob mob_data_flags;
   LabeledTextBoxGlob cur_stats;
   LabeledTextBoxGlob long_data;

   /* constructor */
   public MobPanel2(OLCMob par, ItemListener cb_listener) {
      super();
      parent = par;
      
      int REM = GridBagConstraints.REMAINDER;
      
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);
      
      c.gridwidth = REM;
      c.fill = GridBagConstraints.BOTH;
      mob_flags = new LabeledCheckBoxGlob("Critter Flags", 4, 5);
      
      mob_flags.addBox(new NumCheckbox("is flying", false, 3));
      mob_flags.addBox(new NumCheckbox("can climb", false, 5));
      mob_flags.addBox(new NumCheckbox("gossip", true, 6));
      mob_flags.addBox(new NumCheckbox("yell", true, 7));
      mob_flags.addBox(new NumCheckbox("gratz", true, 8));
      mob_flags.addBox(new NumCheckbox("auction", true, 9));
      mob_flags.addBox(new NumCheckbox("shout", true, 10));
      mob_flags.addBox(new NumCheckbox("say", true, 11));
      mob_flags.addBox(new NumCheckbox("tell", true, 12));
      mob_flags.addBox(new NumCheckbox("wiznet", false, 13));
      mob_flags.addBox(new NumCheckbox("paralyzed", false, 14));
      mob_flags.addBox(new NumCheckbox("perm sleeped", false, 15));
      mob_flags.addBox(new NumCheckbox("dual wield", false, 16));
      mob_flags.addBox(new NumCheckbox("sneak", false, 17));
      mob_flags.addBox(new NumCheckbox("can dive", false, 19));
      
      
      gridbag.setConstraints(mob_flags, c);
      add(mob_flags);
      
      mob_data_flags = new LabeledCheckBoxGlob("Mob Data Flags", 2, 4);
      mob_data_flags.setListener(cb_listener);
      
      mob_data_flags.addBox(new NumCheckbox("HAS PROC DATA", false, 0));
      mob_data_flags.addBox(new NumCheckbox("scavenge", false, 1));
      mob_data_flags.addBox(new NumCheckbox("wander", false, 2));
      mob_data_flags.addBox(new NumCheckbox("edible corpse", false, 5));
      mob_data_flags.addBox(new NumCheckbox("is_banker", false, 6));
      mob_data_flags.addBox(new NumCheckbox("is_sessile", false, 7));
      mob_data_flags.addBox(new NumCheckbox("NO Homing", false, 8));
      mob_data_flags.addBox(new NumCheckbox("HAS SKIN", false, 16));
      
      gridbag.setConstraints(mob_data_flags, c);
      add(mob_data_flags);
      
      long_data = new LabeledTextBoxGlob("Miscellaneous Data.", 3, 3);
      long_data.addBox(new NumTextBox("Gold", "0", 0));
      long_data.addBox(new NumTextBox("Exp Worth", "0", 1));
      long_data.addBox(new NumTextBox("Bank Gold", "0", 2));
      
      gridbag.setConstraints(long_data, c);
      add(long_data);
      
      cur_stats = new LabeledTextBoxGlob("Stats", 1, 3);
      cur_stats.addBox(new NumTextBox("Vis Bits", "0", 0));
      cur_stats.addBox(new NumTextBox("See Bits", "0", 1));
      /* NOTE:  in_room must be entered..but should be zero */
      /* will not be seen in the GUI */
      cur_stats.addBox(new NumTextBox("In_Room", "0", 2), true);
      
      gridbag.setConstraints(cur_stats, c);
      add(cur_stats);
      
      opb = new MobPaneButtons(parent);
      gridbag.setConstraints(opb, c);
      add(opb);
      
   }//constructor
   
   public final String getLongData() {
      return long_data.getText();
   }

   public final String getMobFlags() {
      return mob_flags.getText();
   }

   public final String getMobDataFlags() {
      return mob_data_flags.getText();
   }

   public final String getCurStats() {
      return cur_stats.getText();
   }

   public MobPaneButtons getButtons() {
      return opb;
   }

}//MobPanel2


class MobPanel4 extends Panel {
  OLCMob parent;
  MobPaneButtons opb;

  LabeledTextBoxGlob short_cur_stats;

  public MobPanel4(OLCMob par) {
    super();
    parent = par;

    int REM = GridBagConstraints.REMAINDER;
    int REL = GridBagConstraints.RELATIVE;

    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    setLayout (gridbag);

    c.gridwidth = REM;
    c.fill = GridBagConstraints.BOTH;

    short_cur_stats = new LabeledTextBoxGlob("Critter Stats.", 8, 5);

    /* 11 should be zero */
    /* 22 zero */
    /* 26 two */
    /* 34 zero */
    short_cur_stats.addBox(new NumTextBox("NULL", "0", 11), true);
    short_cur_stats.addBox(new NumTextBox("NULL", "0", 22), true);
    short_cur_stats.addBox(new NumTextBox("NULL", "2", 26), true);
    short_cur_stats.addBox(new NumTextBox("NULL", "0", 34), true);

    short_cur_stats.addBox(new NumTextBox("POS", "0", 0));
    short_cur_stats.addBox(new NumTextBox("STR", "15", 1));
    short_cur_stats.addBox(new NumTextBox("INT", "14", 2));
    short_cur_stats.addBox(new NumTextBox("CON", "15", 3));
    short_cur_stats.addBox(new NumTextBox("CHA", "14", 4));
    short_cur_stats.addBox(new NumTextBox("WIS", "14", 5));
    short_cur_stats.addBox(new NumTextBox("DEX", "14", 6));
    short_cur_stats.addBox(new NumTextBox("HIT", "1", 7));
    short_cur_stats.addBox(new NumTextBox("DAM", "1", 8));
    short_cur_stats.addBox(new NumTextBox("AC", "60", 9));
    short_cur_stats.addBox(new NumTextBox("Attacks", "1", 10));
    short_cur_stats.addBox(new NumTextBox("SEX", "1", 12));
    short_cur_stats.addBox(new NumTextBox("Class", "1", 13));
    short_cur_stats.addBox(new NumTextBox("Race", "1", 14));
    short_cur_stats.addBox(new NumTextBox("HP", "100", 15));
    short_cur_stats.addBox(new NumTextBox("MANA", "100", 16));
    short_cur_stats.addBox(new NumTextBox("MOV", "100", 17));
    short_cur_stats.addBox(new NumTextBox("ALIGN", "50", 18));
    short_cur_stats.addBox(new NumTextBox("Level", "5", 19));
    short_cur_stats.addBox(new NumTextBox("HomeTown", "1", 20));
    short_cur_stats.addBox(new NumTextBox("Wimpy", "0", 21));
    short_cur_stats.addBox(new NumTextBox("HP_MAX", "100", 23));
    short_cur_stats.addBox(new NumTextBox("MA_MAX", "100", 24));
    short_cur_stats.addBox(new NumTextBox("MV_MAX", "100", 25));
    short_cur_stats.addBox(new NumTextBox("DAM_Rec_divisor", "100", 27));
    short_cur_stats.addBox(new NumTextBox("DAM_given_mult", "100", 28));
    short_cur_stats.addBox(new NumTextBox("Heat Resis.", "80", 29));
    short_cur_stats.addBox(new NumTextBox("Cold Resis", "80", 30));
    short_cur_stats.addBox(new NumTextBox("Elec Resis", "100", 31));
    short_cur_stats.addBox(new NumTextBox("Spel Resis", "90", 32));
    short_cur_stats.addBox(new NumTextBox("Religion", "0", 33));
    short_cur_stats.addBox(new NumTextBox("BH_Dice_Cnt", "2", 35));
    short_cur_stats.addBox(new NumTextBox("BH_Dice_Side", "2", 36));
    short_cur_stats.addBox(new NumTextBox("HP_regen", "100", 37));
    short_cur_stats.addBox(new NumTextBox("MA_regen", "100", 38));
    short_cur_stats.addBox(new NumTextBox("MV_regen.", "100", 39));
    short_cur_stats.addBox(new NumTextBox("Guild", "0", 40));
    short_cur_stats.addBox(new NumTextBox("Naked Weight", "150", 41));

    /* these won't show up on the GUI */
    short_cur_stats.addBox(new NumTextBox("NULL", "0", 42), true);
    short_cur_stats.addBox(new NumTextBox("NULL", "0", 43), true);
    short_cur_stats.addBox(new NumTextBox("NULL", "0", 44), true);

    gridbag.setConstraints(short_cur_stats, c);
    add(short_cur_stats);

    opb = new MobPaneButtons(parent);
    gridbag.setConstraints(opb, c);
    add(opb);

  }//Constructor

  public final String getShortCurStats1() {
      return short_cur_stats.getText(0, 14);
   }

   public final String getShortCurStats2() {
      return short_cur_stats.getText(15, 29);
   }

   public final String getShortCurStats3() {
      return short_cur_stats.getText(30, 44);
   }
   
   public MobPaneButtons getButtons() {
      return opb;
   }
   
}//ObjPanel4


class MobPanel5 extends Panel {
   OLCMob parent;
   MobPaneButtons opb;

   LabeledTextField eq;
   LabeledTextField affected_by;
   LabeledTextField inv;
   LabeledTextField mx_in_game;
   LabeledTextField skin_num;
   LabeledTextBoxGlob spec_data;
   LabeledCheckBoxGlob spec_flags;
   
   public MobPanel5(OLCMob par) {
      super();
      parent = par;
      
      int REM = GridBagConstraints.REMAINDER;
      
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      setLayout (gridbag);
      
      
      c.fill = GridBagConstraints.BOTH;
      
      c.gridwidth = 1;
      eq = new LabeledTextField("EQ", "", 40);
      gridbag.setConstraints(eq, c);
      add(eq);
      
      c.gridwidth = REM;
      affected_by = new LabeledTextField("Affected By", "", 40);
      gridbag.setConstraints(affected_by, c);
      add(affected_by);
      
      c.gridwidth = 1;
      mx_in_game = new LabeledTextField("Max In Game", "1", 10);
      gridbag.setConstraints(mx_in_game, c);
      add(mx_in_game);
      
      skin_num = new LabeledTextField("Skin Number", "0", 10);
      gridbag.setConstraints(skin_num, c);
      add(skin_num);
      
      c.gridwidth = REM;
      inv = new LabeledTextField("Inventory", "", 40);
      gridbag.setConstraints(inv, c);
      add(inv);
      
      spec_data = new LabeledTextBoxGlob("Spec Data (AI)", 2, 5);
      
      spec_data.addBox(new NumTextBox("Posn Guarding", "0", 0));
      spec_data.addBox(new NumTextBox("Skill Activity", "0", 1));
      spec_data.addBox(new NumTextBox("Benevolence", "0", 2));
      spec_data.addBox(new NumTextBox("Defensiveness", "0", 3));
      spec_data.addBox(new NumTextBox("Bad-Assedness", "0", 4));
      spec_data.addBox(new NumTextBox("Social Awareness", "0", 5));
      
      gridbag.setConstraints(spec_data, c);
      add(spec_data);
      
      
      spec_flags = new LabeledCheckBoxGlob("Special Procedure Flags", 4, 3);
      
      spec_flags.addBox(new NumCheckbox("SHOPKEEPER", false, 1));
      spec_flags.addBox(new NumCheckbox("TEACHER", false, 2));
      spec_flags.addBox(new NumCheckbox("same_class_pass", true, 3));
      spec_flags.addBox(new NumCheckbox("same_race_pass", true, 4));
      spec_flags.addBox(new NumCheckbox("has_mob_give_proc", false, 5));
      spec_flags.addBox(new NumCheckbox("has_mob_discuss_proc", false, 6));
      spec_flags.addBox(new NumCheckbox("has_mob_bow_proc", false, 7));
      spec_flags.addBox(new NumCheckbox("has_mob_curse_proc", false, 8));
      spec_flags.addBox(new NumCheckbox("is a racist", false, 9));
      spec_flags.addBox(new NumCheckbox("same align only", false, 10));
      spec_flags.addBox(new NumCheckbox("same class only", false, 11));
      
      gridbag.setConstraints(spec_flags, c);
      add(spec_flags);
      
      
      opb = new MobPaneButtons(parent);
      gridbag.setConstraints(opb, c);
      add(opb);
      
   }//Constructor
   
   public void handleProcData(boolean state) {
      spec_flags.setEnabled(state);
   }
   
   public final String getSpecData() {
      return spec_data.getText();
   }

   public final String getSpecDataFlags() {
      return spec_flags.getText();
   }
  
   public final String getMxInGame() {
      return mx_in_game.getText();
   }
   
   public final String getSkinNum() {
      return skin_num.getText();
   }

   public final String getEQ() {
      return eq.getText();
   }

   public final String getAffectedBy() {
      return affected_by.getText();
   }

   public final String getInv() {
      return inv.getText();
   }

   public MobPaneButtons getButtons() {
      return opb;
   }

}//MobPanel5


class MobPanel6 extends Panel {
   OLCMob parent;
   MobPaneButtons opb;

   LabeledTextField inv; /* perm inv, shopkeepers */
   LabeledTextBoxGlob shop_data;
   LabeledCheckBoxGlob spec_flags;
   LabeledCheckBoxGlob shop_data_flags;
   LabeledCheckBoxGlob teach_data_flags;

   public void handleProcData(boolean state) {
      inv.setEnabled(state);
      shop_data.setEnabled(state);
      shop_data_flags.setEnabled(state);
      teach_data_flags.setEnabled(state);
   }
   
   public final String getPermInv() {
      return inv.getText();
   }

   public final String getTeachDataFlags() {
      return teach_data_flags.getText();
   }

   public final String getShopDataFlags() {
      return shop_data_flags.getText();
   }

   public final String getShopData() {
      return shop_data.getText();
   }

   public MobPaneButtons getButtons() {
      return opb;
   }
   
  public MobPanel6(OLCMob par) {
    super();
    parent = par;

    int REM = GridBagConstraints.REMAINDER;

    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    setLayout (gridbag);

    c.gridwidth = REM;
    c.fill = GridBagConstraints.BOTH;

    shop_data = new LabeledTextBoxGlob("Shop Data", 1, 5);

    shop_data.addBox(new NumTextBox("Markup %", "200", 0));
    shop_data.addBox(new NumTextBox("Buy %", "75", 1));
    shop_data.addBox(new NumTextBox("Open Time", "8", 2));
    shop_data.addBox(new NumTextBox("Close Time", "21", 3));

    gridbag.setConstraints(shop_data, c);
    add(shop_data);

    shop_data_flags = new LabeledCheckBoxGlob("Shop Data Flags", 4, 5);

    shop_data_flags.addBox(new NumCheckbox("buy_proc0", true, 0));
    shop_data_flags.addBox(new NumCheckbox("sell_proc0", true, 1));
    shop_data_flags.addBox(new NumCheckbox("offer_proc0", true, 2));
    shop_data_flags.addBox(new NumCheckbox("dart_throwers", false, 46));
    shop_data_flags.addBox(new NumCheckbox("bows", false, 47));
    shop_data_flags.addBox(new NumCheckbox("darts", false, 48));
    shop_data_flags.addBox(new NumCheckbox("arrows", false, 49));
    shop_data_flags.addBox(new NumCheckbox("junk", false, 50));
    shop_data_flags.addBox(new NumCheckbox("wands", false, 51));
    shop_data_flags.addBox(new NumCheckbox("potions", false, 52));
    shop_data_flags.addBox(new NumCheckbox("scrolls", false, 53));
    shop_data_flags.addBox(new NumCheckbox("containers", false, 54));
    shop_data_flags.addBox(new NumCheckbox("armor", false, 56));
    shop_data_flags.addBox(new NumCheckbox("weapons", false, 57));
    shop_data_flags.addBox(new NumCheckbox("lights", false, 58));
    shop_data_flags.addBox(new NumCheckbox("canteens", false, 59));
    shop_data_flags.addBox(new NumCheckbox("foods", false, 61));
    shop_data_flags.addBox(new NumCheckbox("boats", false, 62));
    shop_data_flags.addBox(new NumCheckbox("toolboxes", false, 64));
    shop_data_flags.addBox(new NumCheckbox("cauldrons", false, 65));
    shop_data_flags.addBox(new NumCheckbox("pens", false, 66));
    shop_data_flags.addBox(new NumCheckbox("constr. comp", false, 67));
    shop_data_flags.addBox(new NumCheckbox("conct. comp", false, 68));
    shop_data_flags.addBox(new NumCheckbox("parchments", false, 69));

    gridbag.setConstraints(shop_data_flags, c);
    add(shop_data_flags);


    inv = new LabeledTextField("Permanent Inventory", "", 40);
    gridbag.setConstraints(inv, c);
    add(inv);

    teach_data_flags = new LabeledCheckBoxGlob(
		"Teacher Data Flags:  Will teach...", 2, 4);

    teach_data_flags.addBox(new NumCheckbox("Warrior", false, 1));
    teach_data_flags.addBox(new NumCheckbox("Sage", false, 2));
    teach_data_flags.addBox(new NumCheckbox("Wizard", false, 3));
    teach_data_flags.addBox(new NumCheckbox("Ranger", false, 4));
    teach_data_flags.addBox(new NumCheckbox("Thief", false, 5));
    teach_data_flags.addBox(new NumCheckbox("Alchemist", false, 6));
    teach_data_flags.addBox(new NumCheckbox("Cleric", false, 7));
    teach_data_flags.addBox(new NumCheckbox("Bard", false, 8));

    gridbag.setConstraints(teach_data_flags, c);
    add(teach_data_flags);

    opb = new MobPaneButtons(parent);
    gridbag.setConstraints(opb, c);
    add(opb);

  }//Constructor
}//MobPanel6



class MobPaneButtons extends Panel {
   OLCMob parent;
   Button descriptions;
   Button mob_flags;
   Button short_cur_stats;
   Button eq;
   Button mob_data;

  public MobPaneButtons(OLCMob par) {
    super();
    parent = par;

    Button cancel = new Button("Cancel");
    Button submit = new Button("Submit");
    descriptions = new Button("Descriptions");
    mob_flags = new Button("Mob Flags");
    short_cur_stats = new Button("Mob Stats");
    eq = new Button("Equipment");
    mob_data = new Button("Mob Data");
    Button help = new Button("Help");

    descriptions.setBackground(new Color(0, 255, 0));
    mob_flags.setBackground(new Color(255, 0, 0));
    short_cur_stats.setBackground(new Color(255, 0, 0));
    eq.setBackground(new Color(255, 0, 0));
    mob_data.setBackground(new Color(255, 0, 0));
    
    FlowLayout fl = new FlowLayout();
    setLayout(fl);

    descriptions.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.goto_desc();
      }});

    mob_flags.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.goto_mob_flags();
      }});

    help.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.do_help();
      }});

    short_cur_stats.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.goto_short_cur_stats();
      }});

    eq.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.goto_eq_af_inv();
      }});

    mob_data.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	parent.goto_mob_data();
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
    add(descriptions);
    add(mob_flags);
    add(short_cur_stats);
    add(eq);
    add(mob_data);
    add(help);
    
  }//constructor

  public void touchFlags() {
     mob_flags.setBackground(new Color(0, 255, 0));
  }

  public void touchCurStats() {
     short_cur_stats.setBackground(new Color(0, 255, 0));
  }
  
  public void touchEq() {
     eq.setBackground(new Color(0, 255, 0));
  }
  
  public void touchMobData() {
     mob_data.setBackground(new Color(0, 255, 0));
  }
  
}//MButtonPanel


/** This class will listen to all check boxes and 
  tell the OLCMob class about them. */
class MobCBListener implements ItemListener {
   OLCMob par;
   
   public MobCBListener(OLCMob mob) {
      super();
      par = mob;
   }

   public void itemStateChanged(ItemEvent e) {
      NumCheckbox ncb = (NumCheckbox)(e.getSource());
      par.handleMobFlagsCB(ncb.getNum(), ncb.getState());
   }
}//MobCBListener
