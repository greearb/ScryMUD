// $Id: AliasFrame.java,v 1.4 2001/03/30 07:01:51 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date: 2001/03/30 07:01:51 $

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

class AliasFrame extends Frame {
   HegemonManager hm;
   List aliases;
   Alias[] alias_list = null;
   boolean alias_enabled;
   
   public AliasFrame(HegemonManager h) {
      super("Alias Manager");
      aliases = new List(20, false);
      hm = h;
      try {
         readObject();
      }
      catch (Exception e) {
         Log.instance().err("AliasFrame():  " + e);
         
         // Poke in some defaults
         aliases.add("z10 | zgoto 10");
         aliases.add("cloneclif | rclone 2460");

      }

      aliases.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {
            do_edit();
         }});

      alias_enabled = true;
      alias_list = updateAliases();
      add(aliases, "North");
      add(new AlFButtons(this), "South");
      pack();
   }//constructor

   public void addAlias(Alias act) {
      aliases.add(act.toString());
      //Log.it("Writing out aliases.");
      try {
         writeObject();
      }
      catch (IOException e) {
         Log.instance().err("addAlias:  " + e);
      }
      alias_list = updateAliases();
   }
   
   public final void setAliasEnabled(boolean ae) {
      alias_enabled = ae;
   }

   public final boolean getAliasEnabled() {
      return alias_enabled;
   }
   
   public void addAlias(String glom) {
      aliases.add(glom);
      //Log.it("Writing out aliases.");
      try {
         writeObject();
      }
      catch (IOException e) {
         Log.instance().err("addAlias:  " + e);
      }
      alias_list = updateAliases();
   }

   public void createAlias() {
      AliasDialog hd = new AliasDialog(this, new Alias());
      hd.show();
   }

   public Alias[] getAliases() {
      return alias_list; //cached array of Alias's
   }

   private Alias[] updateAliases() {
      String[] items = aliases.getItems();
      int len = items.length;
      Alias retval[] = new Alias[len];
      
      for (int i = 0; i<len; i++) {
         retval[i] = new Alias(items[i]);
      }
      return retval;
   }//getActions

   
   public Alias getAlias(String trigger, String response) {
      String[] items = aliases.getItems();
      Alias a;
      int len = items.length;
      
      for (int i = 0; i<len; i++) {
         a = new Alias(items[i]);
         if (trigger != null) {
            if ((a.getAlias()).equals(trigger))
               return a;
         }
         else if (response != null) {
            if ((a.getResponse()).equals(response))
               return a;
         }
         else
            return null;
      }
      return null;
   }//getAlias
   

   public void do_delete() {
      String str;
      if ((str = aliases.getSelectedItem()) != null) {
         aliases.remove(str);
         try {
            writeObject();
            alias_list = updateAliases();
         }
         catch (Exception e) {
            Log.instance().err("do_delete:  " + e);
         }
      }
      else {
        MessageDialog md =
          new MessageDialog("Alias Manager",
                            "You need to select an alias to delete.",
                            "red", "black");
      }
   }//do_delete

   public void do_new() {
      createAlias();
   }

   public void do_close() {
      setVisible(false);
   }

   public void do_edit() {
      String str;
      if ((str = aliases.getSelectedItem()) != null) {
         aliases.remove(str); //editing will re-add it
         AliasDialog hd = new AliasDialog(this, new Alias(str));
         hd.show();
      }
      else {
        MessageDialog md =
          new MessageDialog("Alias Manager",
                            "You need to select an alias to edit.",
                            "red", "black");
      }
   }//do_edit

   /** Writes it all to disk, for later retrieval by load() */
   public void writeObject() throws IOException {
      if (hm.IS_APPLET)
         return;

      BOStream ostream =
         new BOStream(new FileOutputStream(hm.getProperties().getProperty("AliasFile")));
      String[] entries = aliases.getItems();
      int len = entries.length;
      try {
         for (int i = 0; i<len; i++) {
            ostream.write(entries[i]);
            ostream.write("\n~\n");
         }//for
      }//try
      catch (Exception e) {
         MessageDialog md =
           new MessageDialog("Write Failure",
                             "Failed to write aliases to disk: " +
                             e, "red", "black");
      }//catch
   }//writeObject

   public void readObject() throws IOException {
      if (hm.IS_APPLET)
         return;

      String af = hm.getProperties().getProperty("AliasFile");
      BIStream istream = new BIStream(new FileInputStream(af));
      String rslt;
      StringBuffer alias = new StringBuffer(200);
      
      try {
         while ((rslt = istream.readLine()) != null) {
            if ((rslt.trim()).equals("~")) {
               aliases.add((alias.toString()).trim());
               alias = new StringBuffer(200);
            }
            else {
               alias.append(rslt + " ");
            }
         }//while
      }//try
      catch (Exception e) {
         Log.instance().err("in AliasFrame.readObject():  " + e);
      }
   }//readObject
}//AliasFrame


class AlFButtons extends Panel {
   AliasFrame parent;
   
   public AlFButtons(AliasFrame par) {
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
   }//AlFButtons constructor
}//AlFButtons
