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

/* Manages actions, including persistant storage. */
import java.awt.*;
import java.awt.event.*;
import java.io.*;

class ActionFrame extends Frame {
   HegemonManager hm;
   List actions;
   Action[] action_list;
   boolean actions_enabled;
   
   public ActionFrame(HegemonManager h) {
      super("Actions Manager");
      actions = new List(20, false);
      hm = h;
      try {
         readObject();
      }
      catch (IOException e) {
         Log.instance().err("ActionFrame():  " + e);
      }

      actions.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_edit();
         }});

      actions_enabled = true;
      action_list = updateActions();
      add(actions, "North");
      add(new AFButtons(this), "South");
      pack();
   }//constructor

   public final void setActionsEnabled(boolean ae) {
      actions_enabled = ae;
   }

   public final boolean getActionsEnabled() {
      return actions_enabled;
   }
   
   public void addAction(Action act) {
      actions.addItem(act.toString());
      try {
         writeObject();
      }
      catch (IOException e) {
         Log.instance().err("addAction  " + e);
      }
      //Log.trc("updating action_list");
      action_list = updateActions();
   }

   public void addAction(String glom) {
      actions.addItem(glom);
      try {
         writeObject();
      }
      catch (IOException e) {
         Log.instance().err("addAction  " + e);
      }
      //Log.it("updating action_list");
      action_list = updateActions();
   }

   public void createAction() {
      ActionDialog hd = new ActionDialog(this, new Action());
      hd.show();
   }

   public Action[] getActions() {
      return action_list;
   }
   
   private Action[] updateActions() {
      String[] items = actions.getItems();
      int len = items.length;
      Action retval[] = new Action[len];
      
      for (int i = 0; i<len; i++) {
         //Log.dbg("Adding action:  " + items[i]);
         retval[i] = new Action(items[i]);
      }
      return retval;
   }//getActions
                              
   public Action getAction(String trigger, String response) {
      String[] items = actions.getItems();
      Action a;
      int len = items.length;
      
      for (int i = 0; i<len; i++) {
         a = new Action(items[i]);
         if (trigger != null) {
            if ((a.getTrigger()).equals(trigger))
               return a;
         }
         else if (response != null) {
            if ((a.getAction()).equals(response))
               return a;
         }
         else
            return null;
      }
      return null;
   }//getAction
   

   
   public void do_delete() {
      String str;
      if ((str = actions.getSelectedItem()) != null) {
         actions.remove(str);
         try {
            writeObject();
            action_list = updateActions();
         }
         catch (Exception e) {
            Log.instance().err("do_delete:  " + e);
         }
      }
      else {
        MessageDialog md =
          new MessageDialog("Connection Manager",
                            "You need to select a connection to delete.",
                            "red", "black");
      }
   }//do_delete

   public void do_new() {
      createAction();
   }

   public void do_close() {
      setVisible(false);
   }

   public void do_edit() {
      String str;
      if ((str = actions.getSelectedItem()) != null) {
         actions.remove(str); //editing will re-add it
         ActionDialog hd = new ActionDialog(this, new Action(str));
         hd.show();
      }
      else {
        MessageDialog md =
          new MessageDialog("Action Manager",
                            "You need to select an action to edit.",
                            "red", "black");
      }
   }//do_edit

   /** Writes it all to disk, for later retrieval by load() */
   public void writeObject() throws IOException {
      if (hm.IS_APPLET)
         return;

      String fl = hm.getProperties().getProperty("ActionFile");
      //      Log.it("fl:  " + fl);
      BOStream ostream = new BOStream(new FileOutputStream(fl));
      String[] entries = actions.getItems();
      int len = entries.length;
      try {
         for (int i = 0; i<len; i++) {
            //Log.dbg("Writing:  " + entries[i]);
            ostream.write(entries[i]);
            ostream.write("\n~\n");
         }//for
      }//try
      catch (Exception e) {
         MessageDialog md =
           new MessageDialog("Write Failure",
                             "Failed to write actions to disk: " +
                             e, "red", "black");
      }//catch

      ostream.flush();
   }//writeObject

   public void readObject() throws IOException {
      if (hm.IS_APPLET)
         return;

      BIStream istream =
        new BIStream(new FileInputStream(hm.getProperties().getProperty("ActionFile")));
      String rslt = null;
      StringBuffer action = new StringBuffer(200);
      
      try {
         while ((rslt = istream.readLine()) != null) {
            if ((rslt.trim()).equals("~")) {
               actions.addItem((action.toString()).trim());
               action = new StringBuffer(200);
            }
            else {
               action.append(rslt + " ");
            }
         }//while
      }//try
      catch (IOException e) {
         Log.instance().err("in ActionFrame.readObject():  " + e);
      }
   }//readObject
}//ActionFrame


class AFButtons extends Panel {
   ActionFrame parent;
   
   public AFButtons(ActionFrame par) {
      super();
      parent = par;
      
      Button done_b = new Button("Done");
      Button edit_b = new Button("Edit");
      Button new_b = new Button("New");
      Button delete_b = new Button("Delete");

      done_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_close();
         }});

      edit_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_edit();
         }});

      new_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_new();
         }});

      delete_b.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            parent.do_delete();
         }});
   
      
      FlowLayout fl = new FlowLayout();
      setLayout(fl);

      add(new_b);
      add(edit_b);
      add(delete_b);
      add(done_b);
   }//AFButtons constructor
}//AFButtons
