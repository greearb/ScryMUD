// $Id: HegemonManager.java,v 1.10 1999/06/23 04:16:05 greear Exp $
// $Revision: 1.10 $  $Author: greear $ $Date: 1999/06/23 04:16:05 $

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
import java.util.*;
import java.io.*;
import java.net.*;

class HegemonManager extends Object {
   OlcStore olc_mgr;
   ActionFrame action_mgr;
   ClientDisplay client_display;
   ConnectionManager connect_mgr;
   SocketManager socket_mgr;
   Properties props;
   AliasFrame alias_mgr;
   HegemonScroll scroll = null;
   ColorSelectionManager color_mgr;
   HelpFrame help_mgr;
   OlEditor olc_editor;
   PathCellEditor path_editor;
   MobScriptEditor ms_editor;
   KeywordEditor k_editor;
   CommandHistory cmd_history;
   hegemon mother_class;
   PsoEditor pso_editor; //player-run shop keeper editor
   BugListEditor bl_editor;

   public static boolean IS_APPLET;
   
   public HegemonManager(String prop_file, boolean is_applet, hegemon heg) {
      super();

      mother_class = heg;
      Log.instance().setLevel(Log.ERROR | Log.WARNING | Log.INFO);

      // This isn't an error, just top priority :)
      Log.instance().err(BuildInfo.getBuildInfo());

      props = new Properties();
      /* props are kept in:  $HOME/hegemon/hegemon.properties */
      try {
         if (!is_applet) {
            props.load(new FileInputStream(prop_file));
         }
         else {
            URL url = new URL(prop_file);
            InputStream ips = url.openStream();
            if (ips == null) {
               Log.instance().err("ERROR:  could not get InputStream from url.");
            }
            else {
               Log.instance().trc("Found input stream, going to load properties... ");
               props.load(ips);
               Log.instance().trc("Loaded properties..");
            }
         }
      }
      catch (Exception e) {
         new MessageDialog("Error reading properties file.", e.toString(),
                           "red", "black");
         Log.instance().trc("HegemonManager, loading hegemon.properties file: " 
                            + e);
         Log.instance().trc("Properties file:  " + prop_file);
      }

      IS_APPLET = is_applet;
      
      cmd_history = new CommandHistory(this);
      path_editor = new PathCellEditor(this);
      pso_editor = new PsoEditor(this);
      bl_editor = new BugListEditor(this);
      olc_editor = new OlEditor(this);
      ms_editor = new MobScriptEditor(this);
      k_editor = new KeywordEditor(this);
      color_mgr = new ColorSelectionManager(this);
      alias_mgr = new AliasFrame(this);
      connect_mgr = new ConnectionManager(this);
      action_mgr = new ActionFrame(this);
      olc_mgr = new OlcStore(this); //null untill we have a writer
      help_mgr = new HelpFrame("ScryMUD Help",
                               props.getProperty("HelpFileBaseDir"),
                               color_mgr, this);
  
      client_display = new ClientDisplay(this);
      client_display.show();
      scroll = client_display.getHegemonScroll();

      scroll.setBackground(color_mgr.getBackgroundSelection());

      if (is_applet) {
         heg.setOperStatus(hegemon.RUNNING);
         heg.repaint();
      }
   }//constructor

   public final CommandHistory getCommandHistory() {
      return cmd_history;
   }

   public final PsoEditor getPsoEditor() {
      return pso_editor;
   }

   public final BugListEditor getBugListEditor() {
      return bl_editor;
   }
   
   public final OlEditor getOlEditor() {
      return olc_editor;
   }

   public final PathCellEditor getPathCellEditor() {
      return path_editor;
   }

   public final MobScriptEditor getMobScriptEditor() {
      return ms_editor;
   }

   public final KeywordEditor getKeywordEditor() {
      return k_editor;
   }
   
   public final HegemonScroll getScroll() {
      return scroll;
   }

   public final HelpFrame getHelpFrame() {
      return help_mgr;
   }

   public final AliasFrame getAliasManager() {
      return alias_mgr;
   }
   
   public final ActionFrame getActionManager() {
      return action_mgr;
   }

   public final ColorSelectionManager getColorSelectionMgr() {
      return color_mgr;
   }
   
   public final OlcStore getOlcManager() {
      return olc_mgr;
   }

   public final Properties getProperties() {
      return props;
   }

   public void openConnection() {
      String hostname;
      int port;
      
      hostname = connect_mgr.getHostname();
      port = connect_mgr.getPortnum();
      if (socket_mgr != null) {
         socket_mgr.destroy();
         socket_mgr = null;
      }
      socket_mgr = new SocketManager(hostname, port, this);

   }

   public void closeConnection() {
      if (socket_mgr != null) {
         socket_mgr.signalWriterDeath(); //modify Title & stuff
         socket_mgr.destroy();
      }
      socket_mgr = null;
   }//closeConnection

   public SocketManager getSocketManager() throws Exception {
      if (socket_mgr == null) {
         throw new Exception("No connection exists.");
      }
      else
        return socket_mgr;
   }

   public void showConnection() {
      connect_mgr.show();
   }

   public void showColors() {
      color_mgr.show();
   }

   public void showActions() {
      action_mgr.show();
   }

   public void showAliases() {
      alias_mgr.show();
   }

   public void quit() {
      if (socket_mgr != null) {
         socket_mgr.flush();
      }

      if (mother_class != null) {
         // Not that I don't trust Java's garbage collection or anything... :P

         pso_editor.dispose();
         pso_editor = null;
         bl_editor.dispose();
         bl_editor = null;
         olc_mgr.dispose();
         olc_mgr = null;
         action_mgr.dispose();
         action_mgr = null;
         client_display.dispose();
         client_display = null;
         connect_mgr.dispose();
         connect_mgr = null;
         socket_mgr.close();
         socket_mgr = null;
         props= null;
         alias_mgr.dispose();
         alias_mgr = null;
         //scroll.dispose();
         scroll = null;
         color_mgr.dispose();
         color_mgr = null;
         help_mgr.dispose();
         help_mgr = null;
         olc_editor.dispose();
         olc_editor = null;
         path_editor.dispose();
         path_editor = null;
         ms_editor.dispose();
         ms_editor = null;
         k_editor.dispose();
         k_editor = null;
         cmd_history.dispose();
         cmd_history = null;
         mother_class.destroy();
         mother_class = null;
      }
      else {
         System.exit(0); //should flush first...
      }
   }

   public hegemon getMotherClass() {
      return mother_class;
   }

   public ClientDisplay getClientDisplay() {
      return client_display;
   }

   public OlcStore getOlcStore() {
      return olc_mgr;
   }

   public ActionFrame getActions() {
      return action_mgr;
   }
   
}//HegemonManager
