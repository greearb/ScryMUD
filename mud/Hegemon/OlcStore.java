// $Id: OlcStore.java,v 1.5 2001/03/31 07:06:00 greear Exp $
// $Revision: 1.5 $  $Author: greear $ $Date: 2001/03/31 07:06:00 $

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

import java.util.*;
import java.awt.*;
import java.awt.event.*;

class OlcStore extends Object {
   HegemonManager hm = null;
   public OLCDoor door = null;
   public OLCRoom rm = null;
   public OLCMob mob = null;
   public OLCObject obj = null;
   boolean did_dialog = false;
   
   String tmp_buf; //used with ODialogs
   ODialog od = null;

   String case0;

   public void dispose() {
      door = null;
      rm = null;
      mob = null;
      obj = null;
   }

   
   public OlcStore(HegemonManager h) {
      hm = h;
   }

   public void setDoor(OLCDoor targ) {
      door = targ;
   }
   
   public void setRoom(OLCRoom targ) {
      rm = targ;
   }
   
   public void setObj(OLCObject targ) {
      obj = targ;
   }
   
   public void setMob(OLCMob targ) {
      mob = targ;
   }
   
   // This lets us know that tmp_buf has been filled
   // with the ODialog's information, so we can now ship that off..
   // Message comes from an ODialog, or a processCase call..

   protected void signalBuf() throws Exception {
      int len;
      if (tmp_buf == null) { 
         throw new Exception("tmp_buf is NULL");
      }
      else if ((len = tmp_buf.length()) == 0) {
         throw new Exception("tmp_buf is empty");
      }

      if (tmp_buf.charAt(len - 1) != '\n') {
         hm.getSocketManager().write(tmp_buf + "\n");
         hm.getScroll().append("WRITING: (for phase below) " + tmp_buf + "\n");
      }//if
      else {
         hm.getSocketManager().write(tmp_buf);
         hm.getScroll().append("WRITING: (for phase below)  " + tmp_buf);
      }
   }//signalBuf


  /** Lets the submit for various olc components set the correct
    choice for the first prompt in Scry's OLC */
  public void setCase0(String s) {
    case0 = s;
  }

  protected void setTmpBuf(String s) {
     tmp_buf = s;
  }

  /** Submit the appropriate data for a given case.  If the OLC system
    does not contain this data, prompt the user with a pop-up dialog
    asking for the data.  This will allow much more independence between
    the MUD and the Client. */
  public void processCase(int which_case) {
     tmp_buf = getText(which_case);
     if (tmp_buf == null) {
        if (!did_dialog)
          od = new ODialog(this, "Unknown OLC Prompt, check Display");
        else
          return; //the ODialog will take care of it...
     }
     else {
        try {
           signalBuf(); //let it know we can ship tmp_buf to the writer...
        }
        catch (Exception e) {
           Log.instance().err(e + ":  which_case:  " + which_case);
           //e.printStackTrace();
        }
     }
  }//processCase

   
   /**  Get the appropriate text, based on the case number.  Can cause
    a popup dialog box to ask for any info it doesn't know how to get. */
  private String getText(int which_case) {
     did_dialog = false;

     try {
        switch (which_case) {
         case 999:
           return "\n";
         case 0: 
           return case0;
         case 1:
           return obj.getObjNum();//"1"; //server will figure it out
         case 2:
           return obj.getNames();
         case 3:
           return obj.getShortDesc();
         case 4:
           return obj.getInRoomDesc();
         case 5:
           return obj.getDesc();
         case 6:
           return obj.getObjFlags();
         case 7:
           return obj.getExtras();
         case 8:
           return obj.getVbPrice();
         case 9:
           return obj.getBagFlags();
         case 10:
           return obj.getBagData();
         case 11:
           return "-1"; //obj.getAffectedBy(), not implemented at this time
         case 12:
           return obj.getInv();
         case 13:
           return obj.getStatEffects();
         case 14:
           return obj.getSpecDataFlags();
         case 15:
           return "1"; //server will decide CRITTER NUMBER
         case 16:
           return mob.getNames();
         case 17:
           return mob.getShortDesc();
         case 18:
           return mob.getInRoomDesc();
         case 19:
           return mob.getDesc();
         case 20:
           return mob.getMobFlags();
         case 21:
           return mob.getLongData();
         case 22:
           return mob.getShortCurStats1();
         case 23:
           return mob.getShortCurStats2();
         case 24:
           return mob.getShortCurStats3();
         case 25:
           return mob.getCurStats();
         case 26:
           return mob.getEQ();
         case 27:
           return mob.getAffectedBy();
         case 28:
           return mob.getInv();
         case 29:
           return (mob.getMxInGame() + " " +
                   mob.getSkinNum());
         case 30:
           return mob.getMobDataFlags();
         case 31:
           return mob.getSpecData();
         case 32:
           return mob.getSpecDataFlags();
         case 33:
           return mob.getShopData();
         case 34:
           return mob.getShopDataFlags();
         case 35:
           return mob.getPermInv();
         case 36:
           return mob.getTeachDataFlags();
         case 37:
            //THIS CASE IS DEPRECATED IN THE ScryMUD SERVER.
            od = new ODialog(this, "Deprecated case, CONSTRUCT DATA");
            did_dialog = true;
            return null;
         case 38:
           return "1"; //server will figure ROOM NUMBER
         case 39:
           return rm.getNames();
         case 40:
           return rm.getShortDesc();
         case 41:
           return rm.getDesc();
         case 42:
           return rm.getFlags();
         case 43:
           return rm.getCurStats(); //vis_bit and movement
         case 44:
           return "-1"; //rm.getAffectedBy(), unsupported at this time
         case 45:
           return rm.getInv();
         case 46:
           return (rm.getDistance() + " " + rm.getDirection());
         case 47:
           return "1"; //server will figure DOOR NUMBER
         case 48:
           return door.getNames();
         case 49:
           return door.getDesc();
         case 50:
           return door.getFlags();
         case 51:
           return (door.getVisBit() + " " + door.getTokenNum() + " " +
                   door.getKeyNum());
         case 52:
            return rm.getVehicleFlags();
         case 53:
            //time_between_stops, cur_fuel, max_fuel
            return rm.getVehicleData();
         case 54: 
            return "1"; //server will figure out vehicle number
         case 55:
           od = new ODialog(this, "Special Procedure:  Give");
           did_dialog = true;
           return null;
         case 56:
           od = new ODialog(this, "Special Procedure:  Discuss");
           did_dialog = true;
           return null;
         case 57:
           od = new ODialog(this, "Special Procedure:  Bow");
           did_dialog = true;
           return null;
         case 58:
           od = new ODialog(this, "Special Procedure:  Curse");
           did_dialog = true;
           return null;
         case 59:
           od = new ODialog(this, "Special Procedure:  Discuss:  Response");
           did_dialog = true;
           return null;
         case 60:
           od = 
             new ODialog(this, "Special Procedure:  Give:  Correct Object Message");
           did_dialog = true;
           return null;
         case 61:
           od = new ODialog(this, "Special Procedure:  Bow:  Skill Name");
           did_dialog = true;
           return null;
         case 62:
           od = new ODialog(this, "Special Procedure:  Curse:  Skill Name");
           did_dialog = true;
           return null;
         case 63:
           od = new ODialog(this, "Special Procedure:  Discuss:  Skill Name");
           did_dialog = true;
           return null;
         case 64:
           od = new ODialog(this, "Special Procedure:  Give:  Skill Name");
           did_dialog = true;
           return null;
         case 65:
           od = new ODialog(this, "Special Procedure:  Bow:  Object Num");
           did_dialog = true;
           return null;
         case 66:
           od = new ODialog(this, "Special Procedure:  Curse:  Object Num");
           did_dialog = true;
           return null;
         case 67:
           od = new ODialog(this, "Special Procedure:  Discuss:  Object Num");
           did_dialog = true;
           return null;
         case 68:
           od = new ODialog(this, "Special Procedure:  Give:  Object Num");
           did_dialog = true;
           return null;
         case 69:
           od = new ODialog(this, "Special Procedure:  Bow:  Trans to Room");
           did_dialog = true;
           return null;
         case 70:
           od = new ODialog(this, "Special Procedure:  Curse:  Trans to Room");
           did_dialog = true;
           return null;
         case 71:
           od = new ODialog(this, "Special Procedure:  Discuss:  Trans to Room");
           did_dialog = true;
           return null;
         case 72:
           od = new ODialog(this, "Special Procedure:  Give:  Trans to Room");
           did_dialog = true;
           return null;
         case 73:
           od = new ODialog(this, "Special Procedure:  Another Discuss Proc??");
           did_dialog = true;
           return null;
         case 74:
           od = 
             new ODialog(this, "Special Procedure:  Give:  Wrong Gift Message");
           did_dialog = true;
           return null;
         case 75:
           od = new ODialog(this, "Special Procedure:  Wrong Alignment Message");
           did_dialog = true;
           return null;
         case 76:
           od = new ODialog(this, "Special Procedure:  Wrong Class Message");
           did_dialog = true;
           return null;
         case 77:
           od = new ODialog(this, "Special Procedure:  Wrong Race Message");
           did_dialog = true;
           return null;
         case 78:
           return obj.getSkinNum();
         case 79:
           return obj.getCastsSpells();
         default:
           od = new ODialog(this, "Unknown OLC Prompt, check Display");
           did_dialog = true;
           return null;
        }//switch on the case number
     }//try
     catch (NullPointerException e) {
        Log.instance().wrn("OlcStore.getText():  " + e +
                           "/nProbably typed OLC from command line.");
        new MessageDialog("SUGGESTION", "It looks like you tried to enter olc"
                          + " from the command line, instead of the GUI.\n"
                          + "This is NOT suggested, so please exit olc:\n"
                          + "'quit_olc'\n"
                          + "If you don't quit, you will get a lot of pop-up"
                          + " boxes, and will most likely be confused!!",
                          "red", "black");
        return null;
     }

  }//getText(int which_case)
  
}//OlcStore class


class ODialog extends Frame {
  LabeledTextArea lta;
  ODButtons odb;
  OlcStore parent;

  public ODialog(OlcStore par, String desc) {
    super("OLC Dialog Box");
    parent = par;

    setLayout(new BorderLayout());

    lta = new LabeledTextArea(desc, "", 5, 80);
    
    add(lta, "North");
    
    odb = new ODButtons(this);
    add(odb, "South");
    pack();
    show();
  }

  void submit() {
    parent.setTmpBuf(lta.getText());
    try {
       parent.signalBuf(); //let it know we're good to go
    }
    catch (Exception e) {
       MessageDialog md =
          new MessageDialog("SUBMIT ERROR", "You must enter some OLC data!",
                            "red", "black");
       return; //ie DO NOT DISPOSE
    }
                                            
    dispose();
  }

  void exit_olc() {
    parent.setTmpBuf("quit_olc");
    try {
       parent.signalBuf();
    }
    catch (Exception e) {
       Log.instance().err(e + ":  In exit_olc.");
    }
    /* don't dispose it, maybe they just want help */
  }

  void cancel() {
    dispose(); //maybe they just want to do it on the command line
  }

}//ODialog


class ODButtons extends Panel {
  Button submit;
  Button exit_olc;
  Button cancel;
  ODialog parent;

  public ODButtons(ODialog par) {
    parent = par;
    setLayout(new BorderLayout());

    submit = new Button("Submit");
    submit.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	submit_action();
      }});
    add(submit, "West");

    exit_olc = new Button("Quit Olc");
    exit_olc.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	exit_olc_action();
      }});
    add(exit_olc, "Center");

    cancel = new Button("Cancel");
    cancel.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	cancel_action();
      }});
    add(cancel, "East");
  }

  void cancel_action() {
    parent.cancel();
  }

  void exit_olc_action() {
    parent.exit_olc();
  }

  void submit_action() {
    parent.submit();
  }

}//ODButtons













