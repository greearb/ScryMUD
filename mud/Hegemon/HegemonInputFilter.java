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

import java.util.*;
import java.awt.*;


/* filter stuff coming FROM mud */
class HegemonInputFilter extends Object {
   StringBuffer current_line;
   OlcStore olc_store;
   int last_phase = -1;
   SocketWriter sink;
   ActionFrame actions;
   ColorSelectionManager color_mgr;
   HegemonManager hm;
   HegemonScroll heg_scroll;
   HegemonDisplay heg_disp;
   
   boolean last_tag_was_space;
   boolean last_was_lt;
   boolean in_tag;
   boolean last_was_gt;

   Integer context_mode;

   // PONDERING:  a case for every different thing we want to capture??
   public static final Integer MODE_NORMAL            = new Integer(0);
   public static final Integer MODE_HELP_PAGE         = new Integer(1);
   public static final Integer MODE_STAT_SD           = new Integer(2);
   public static final Integer MODE_STAT_ND           = new Integer(3);
   public static final Integer MODE_STAT_LD           = new Integer(4);
   public static final Integer MODE_MSCRIPT_DATA      = new Integer(5);
   public static final Integer MODE_PATH_CELL_STAT_LD = new Integer(6);
   public static final Integer MODE_KEYWORD_LD        = new Integer(7);
   public static final Integer MODE_BUG_TITLE         = new Integer(8);
   public static final Integer MODE_BUG_COMMENT_ENTRY = new Integer(9);
   

   StringBuffer tag_sofar;
   StringBuffer disp_sofar;

   Hashtable tag_hash; //hash table for tags w/no arguments
   
   public HegemonInputFilter(OlcStore os, SocketWriter snk,
                             ActionFrame acts, HegemonManager h,
                             HegemonScroll hs, ColorSelectionManager cm) {
      super();
      hm = h;

      context_mode = MODE_NORMAL;
      
      heg_scroll = hs;
      heg_disp = heg_scroll.getPar();
      
      current_line = new StringBuffer(100);
      olc_store = os;
      sink = snk; /* for automatic output (ie actions) */
      actions = acts;
      color_mgr = cm;
      
      last_tag_was_space = false;
      last_was_lt = false;
      last_was_gt = false;
      in_tag = false;
      tag_sofar = new StringBuffer(50);
      disp_sofar = new StringBuffer(1000);


      tag_hash = new Hashtable();
      tag_hash.put("BATTLE", new TagCommand() {
         public boolean run(Vector args) {
            Color c = color_mgr.getBattleSelection();
            if (c != null)
               heg_scroll.pushColor(c);
            Font f = color_mgr.getBattleFont();
            if (f != null)
               heg_scroll.pushFont(f);
            return true;
         }});
           
      tag_hash.put("/BATTLE", new TagCommand() {
         public boolean run(Vector args) {
            if (color_mgr.getBattleSelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getBattleFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});

      tag_hash.put("/BUG_ENTRY", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            hm.getBugListEditor().signalEntryDone();
            return true;
         }});

      tag_hash.put("BUG_TITLE", new TagCommand() {
         public boolean run(Vector args) {
            hm.getBugListEditor().clear();
            heg_scroll.getProperties().pushContextMode(MODE_BUG_TITLE);
            context_mode = MODE_BUG_TITLE;
            return true;
         }});

      tag_hash.put("/BUG_TITLE", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            return true;
         }});

      tag_hash.put("TELL", new TagCommand() {
         public boolean run(Vector args) {
            Color c = color_mgr.getTellSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            Font f = color_mgr.getTellFont();
            if (f != null)
              heg_scroll.pushFont(f);
            return true;
         }});

      tag_hash.put("/TELL", new TagCommand() {
         public boolean run(Vector args) {
            if (color_mgr.getGossipSelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getTellFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});

      tag_hash.put("/BUG_COMMENT_ENTRY", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            hm.getBugListEditor().signalCommentDone();
            return true;
         }});

      tag_hash.put("DOOR_LIST", new TagCommand() {
         public boolean run(Vector args) {
            Color c = color_mgr.getDoorListSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            Font f = color_mgr.getDoorListFont();
            if (f != null)
              heg_scroll.pushFont(f);
            return true;
         }});

      tag_hash.put("/DOOR_LIST", new TagCommand() {
         public boolean run(Vector args) {
            if (color_mgr.getDoorListSelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getDoorListFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});
      
      tag_hash.put("MOB_LIST", new TagCommand() {
         public boolean run(Vector args) {
            Color c = color_mgr.getMobListSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            Font f = color_mgr.getMobListFont();
            if (f != null)
              heg_scroll.pushFont(f);
            return true;
         }});

      tag_hash.put("/MOB_LIST", new TagCommand() {
         public boolean run(Vector args) {
          if (color_mgr.getMobListSelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getMobListFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});

      tag_hash.put("ITEM_LIST", new TagCommand() {
         public boolean run(Vector args) {
            Color c = color_mgr.getItemListSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            Font f = color_mgr.getItemListFont();
            if (f != null)
              heg_scroll.pushFont(f);
            return true;
         }});

      tag_hash.put("/ITEM_LIST", new TagCommand() {
         public boolean run(Vector args) {
            if (color_mgr.getItemListSelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getItemListFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});
      
      tag_hash.put("RM_DESC", new TagCommand() {
         public boolean run(Vector args) {
            Color c = color_mgr.getRoomDescSelection();
            heg_scroll.getProperties().pushContextMode(MODE_STAT_LD);
            context_mode = MODE_STAT_LD;
            if (c != null)
              heg_scroll.pushColor(c);
            Font f = color_mgr.getRoomDescFont();
            if (f != null)
              heg_scroll.pushFont(f);
            return true;
         }});

      tag_hash.put("/RM_DESC", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            if (color_mgr.getRoomDescSelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getRoomDescFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});

      tag_hash.put("/RM_SHORT_DESC", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            if (color_mgr.getRoomShortDescSelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getRoomShortDescFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});

      tag_hash.put("GOSSIP", new TagCommand() {
         public boolean run(Vector args) {
            Color c = color_mgr.getGossipSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            Font f = color_mgr.getGossipFont();
            if (f != null)
              heg_scroll.pushFont(f);
            return true;
         }});

      tag_hash.put("/GOSSIP", new TagCommand() {
         public boolean run(Vector args) {
            if (color_mgr.getGossipSelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getGossipFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});

      tag_hash.put("SAY", new TagCommand() {
         public boolean run(Vector args) {
            Color c = color_mgr.getSaySelection();
            if (c != null)
              heg_scroll.pushColor(c);
            Font f = color_mgr.getSayFont();
            if (f != null)
              heg_scroll.pushFont(f);
            return true;
         }});

      tag_hash.put("/SAY", new TagCommand() {
         public boolean run(Vector args) {
            if (color_mgr.getSaySelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getSayFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});

      tag_hash.put("YELL", new TagCommand() {
         public boolean run(Vector args) {
            Color c = color_mgr.getYellSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            Font f = color_mgr.getYellFont();
            if (f != null)
              heg_scroll.pushFont(f);
            return true;
         }});

      tag_hash.put("/YELL", new TagCommand() {
         public boolean run(Vector args) {
            if (color_mgr.getYellSelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getYellFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});

      tag_hash.put("AUCTION", new TagCommand() {
         public boolean run(Vector args) {
            Color c = color_mgr.getAuctionSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            Font f = color_mgr.getAuctionFont();
            if (f != null)
              heg_scroll.pushFont(f);
            return true;
         }});

      tag_hash.put("/AUCTION", new TagCommand() {
         public boolean run(Vector args) {
            if (color_mgr.getAuctionSelection() != null) 
              heg_scroll.getProperties().popColor();
            if (color_mgr.getAuctionFont() != null)
              heg_scroll.getProperties().popFont();
            return true;
         }});

      tag_hash.put("TAB", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().pushTab();
            return true;
         }});

      tag_hash.put("/TAB", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popTab();
            return true;
         }});

      tag_hash.put("/HELP", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            hm.getHelpFrame().saveHelpText(); //write it to disk
            return true;
         }});

      tag_hash.put("P", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.drawNewline();
            return true;
         }});

      tag_hash.put("PRE", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().pushMode(heg_scroll.
                                                getProperties().MODE_PRE);
            return true;
         }});


      tag_hash.put("/PRE", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popMode();
            return true;
         }});

      tag_hash.put("/COLOR", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popColor();
            return true;
         }});

      tag_hash.put("STAT_SD", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().pushContextMode(MODE_STAT_SD);
            context_mode = MODE_STAT_SD;
            return true;
         }});

      tag_hash.put("STAT_ND", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().pushContextMode(MODE_STAT_ND);
            context_mode = MODE_STAT_ND;
            return true;
         }});

      tag_hash.put("STAT_LD", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().pushContextMode(MODE_STAT_LD);
            context_mode = MODE_STAT_LD;
            return true;
         }});

      tag_hash.put("KEYWORD_DESC", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().pushContextMode(MODE_KEYWORD_LD);
            context_mode = MODE_KEYWORD_LD;
            return true;
         }});

      tag_hash.put("PSTAT_LD", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().pushContextMode(MODE_PATH_CELL_STAT_LD);
            context_mode = MODE_PATH_CELL_STAT_LD;
            return true;
         }});

      tag_hash.put("SCRIPT_DATA", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().pushContextMode(MODE_MSCRIPT_DATA);
            context_mode = MODE_MSCRIPT_DATA;
            return true;
         }});

      tag_hash.put("/KEYWORD_DESC", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            return true;
         }});

      tag_hash.put("/SCRIPT_DATA", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            return true;
         }});

      tag_hash.put("/STAT", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            return true;
         }});

      tag_hash.put("ENGAGE_HEGEMON", new TagCommand() {
         public boolean run(Vector args) {
            try {
               hm.getSocketManager().write("using_client\n");
            } catch (Exception e) { e.printStackTrace(); }
            return true;
         }});

      tag_hash.put("__SCRY__", new TagCommand() {
         public boolean run(Vector args) {
            try {
               /* let the server know what we are.
                */
               hm.getSocketManager().write("__HEGEMON__\n");
            } catch (Exception e) { e.printStackTrace(); }
            return true;
         }});

      tag_hash.put("DISCRIM", new TagCommand() {
         public boolean run(Vector args) {
            StringBuffer buf = new StringBuffer(100);
            for (int i = 1; i<args.size(); i++) {
               buf.append((String)(args.elementAt(i)) + " ");
            }
            setMobScriptDiscrim(buf.toString());
            return true;
         }});

      tag_hash.put("KEYWORD", new TagCommand() {
         public boolean run(Vector args) {
            //first one is keyword number
            hm.getKeywordEditor().do_clear();
            hm.getKeywordEditor().setCellNum((String)(args.elementAt(1)));

            StringBuffer buf = new StringBuffer(100);
            // the rest are names
            for (int i = 2; i<args.size(); i++) {
               buf.append((String)(args.elementAt(i)) + " ");
            }
            hm.getKeywordEditor().setNames(buf.toString());
            return true;
         }});

      tag_hash.put("NAMES", new TagCommand() {
         public boolean run(Vector args) {
            StringBuffer buf = new StringBuffer(100);
            // the rest are names
            for (int i = 1; i<args.size(); i++) {
               buf.append((String)(args.elementAt(i)) + " ");
            }
            hm.getOlEditor().setNames(buf.toString());
            return true;
         }});

      tag_hash.put("EXITS", new TagCommand() {
         public boolean run(Vector args) {
            hm.getClientDisplay().getInputField().getExitNavigator().setDirs(args);
            return true;
         }});

      tag_hash.put("VALUE_INAME", new TagCommand() {
         public boolean run(Vector args) {
            Log.instance().dbg("Was VALUE_INAME..\n");
            StringBuffer buf = new StringBuffer(100);
            // First is the index...

            // the rest are it's name
            for (int i = 2; i<args.size(); i++) {
               buf.append((String)(args.elementAt(i)) + " ");
            }
            hm.getPsoEditor().setName((String)(args.elementAt(1)), 
                                      buf.toString());
            return true;
         }});

      tag_hash.put("RM_SHORT_DESC", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().pushContextMode(MODE_STAT_SD);
            context_mode = MODE_STAT_SD;
            OlEditor tmp = hm.getOlEditor();
            if (!tmp.isFrozen()) {
               int i = Integer.parseInt((String)(args.elementAt(1)));
               tmp.do_clear();
               tmp.setObjNum(i);
               tmp.appendInRoomDesc("N/A");
               tmp.setObjType(OlEditor.ROOM);
            }
            Color c = color_mgr.getRoomShortDescSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            return true;
         }});

      tag_hash.put("COLOR", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.pushColor((String)(args.elementAt(1)));
            return true;
         }});

      tag_hash.put("MSTAT", new TagCommand() {
         public boolean run(Vector args) {
            OlEditor tmp = hm.getOlEditor();
            if (!tmp.isFrozen()) {
               int i = Integer.parseInt((String)(args.elementAt(1)));
               tmp.do_clear();
               tmp.setObjNum(i);
               tmp.setObjType(OlEditor.MOB);
            }
            return true;
         }});

      tag_hash.put("OSTAT", new TagCommand() {
         public boolean run(Vector args) {
            OlEditor tmp = hm.getOlEditor();
            if (!tmp.isFrozen()) {
               int i = Integer.parseInt((String)(args.elementAt(1)));
               tmp.do_clear();
               tmp.setObjNum(i);
               tmp.setObjType(OlEditor.OBJ);
            }
            return true;
         }});

      tag_hash.put("PHASE", new TagCommand() {
         public boolean run(Vector args) {
            int tmp = Integer.parseInt((String)(args.elementAt(1)));
         
            if (tmp == last_phase) {
               MessageDialog md =
                 new MessageDialog("ALERT",
                                   "You must enter OLC data at your prompt.",
                                   "yellow", "black");
               return true;
            }//if
            else {
               Log.instance().dbg("(OLC) Phase: " + tmp);
               last_phase = tmp;
               signalCase(last_phase);
               return false;
            }
         }});

      tag_hash.put("PATH_IS_DEST", new TagCommand() {
         public boolean run(Vector args) {
            hm.getPathCellEditor().setIsDest((String)(args.elementAt(1)));
            return true;
         }});

      tag_hash.put("H", new TagCommand() {
         public boolean run(Vector args) {
            try {
               heg_scroll.getProperties().pushTextSize(new Integer((String)(args.elementAt(1))));
               return true;
            }
            catch (Exception e) {
               Log.instance().err(e.toString());
               return true;
            }
         }});

      tag_hash.put("HELP", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.getProperties().pushContextMode(MODE_HELP_PAGE);
            context_mode = MODE_HELP_PAGE;
            HelpFrame tmp = hm.getHelpFrame();
            tmp.setHelpCategory((String)args.elementAt(1));
            tmp.setHelpTopic((String)args.elementAt(2));
            return true;
         }});

      tag_hash.put("PSTAT", new TagCommand() {
         public boolean run(Vector args) {
            hm.getPathCellEditor().do_clear();         // PathCell Stat
            hm.getPathCellEditor().setVehNum((String)(args.elementAt(1)));
            hm.getPathCellEditor().setCellNum((String)(args.elementAt(2)));
            return true;
         }});

      tag_hash.put("PATH_DIR", new TagCommand() {
         public boolean run(Vector args) {
            hm.getPathCellEditor().setDir((String)(args.elementAt(1)),
                                          (String)(args.elementAt(2)));
            return true;
         }});

      tag_hash.put("BUG_COMMENT_ENTRY", new TagCommand() {
         public boolean run(Vector args) {
            hm.getBugListEditor().newEntry((String)(args.elementAt(1)),
                                           (String)(args.elementAt(2)));
            heg_scroll.getProperties().pushContextMode(MODE_BUG_COMMENT_ENTRY);
            context_mode = MODE_BUG_COMMENT_ENTRY;
            
            return true;
         }});

      tag_hash.put("VALUE_LIST", new TagCommand() {
         public boolean run(Vector args) {
            hm.getPsoEditor().setList((String)(args.elementAt(1)), // i_th
                                      (String)(args.elementAt(2)), // name
                                      (String)(args.elementAt(3))); // gold
            return true;
         }});

      tag_hash.put("VALUE_ITEM", new TagCommand() {
         public boolean run(Vector args) {
            hm.getPsoEditor().setItem((String)(args.elementAt(1)), // idx
                                      (String)(args.elementAt(2)), // obj#
                                      (String)(args.elementAt(3)), // $sell
                                      (String)(args.elementAt(4))); // $buy
            return true;
         }});

      tag_hash.put("PROMPT", new TagCommand() {
         public boolean run(Vector args) {
            heg_scroll.setPrompt(Integer.parseInt((String)(args.elementAt(1))),
                                 Integer.parseInt((String)(args.elementAt(2))),
                                 Integer.parseInt((String)(args.elementAt(3))),
                                 Integer.parseInt((String)(args.elementAt(4))),
                                 Integer.parseInt((String)(args.elementAt(5))),
                                 Integer.parseInt((String)(args.elementAt(6))));
            return true;
         }});

      tag_hash.put("MOB_SCRIPT", new TagCommand() {
         public boolean run(Vector args) {
            hm.getMobScriptEditor().do_clear();
            setMobScriptData((String)(args.elementAt(1)),
                             (String)(args.elementAt(2)),
                             (String)(args.elementAt(3)),
                             (String)(args.elementAt(4)),
                             (String)(args.elementAt(5)),
                             "MOB",
                             (String)(args.elementAt(6)));
            return true;
         }});

      tag_hash.put("ROOM_SCRIPT", new TagCommand() {
         public boolean run(Vector args) {
            hm.getMobScriptEditor().do_clear();
            setMobScriptData((String)(args.elementAt(1)),
                             (String)(args.elementAt(2)),
                             (String)(args.elementAt(3)),
                             (String)(args.elementAt(4)),
                             (String)(args.elementAt(5)),
                             "ROOM",
                             (String)(args.elementAt(6)));
            return true;
         }});

      tag_hash.put("OBJ_SCRIPT", new TagCommand() {
         public boolean run(Vector args) {
            hm.getMobScriptEditor().do_clear();
            setMobScriptData((String)(args.elementAt(1)),
                             (String)(args.elementAt(2)),
                             (String)(args.elementAt(3)),
                             (String)(args.elementAt(4)),
                             (String)(args.elementAt(5)),
                             "OBJECT",
                             (String)(args.elementAt(6)));
            return true;
         }});

      tag_hash.put("BUG_ENTRY", new TagCommand() {
         public boolean run(Vector args) {
            hm.getBugListEditor().setBugEntry((String)(args.elementAt(1)), //num
                                              (String)(args.elementAt(2)), //state
                                              (String)(args.elementAt(3)), //cr_date
                                              (String)(args.elementAt(4)), //reporter
                                              (String)(args.elementAt(5)), //room_num
                                              (String)(args.elementAt(6)), //assigned_to
                                              (String)(args.elementAt(7))); //col_type
            return true;
         }});

      // Would be nice to force the hash-table to optimize itself at this point.
   }//constructor

   public final void setOutput(HegemonScroll disp) {
      heg_scroll = disp;
   }
   
   public final synchronized void filter(String input) {
      /* add GAGs and SUBSTITUTIONS here... */
      // input = doGags(input);

      //Log.it("in filter, input:  -:" + input + ":-");
      if (actions != null) {
         filterActions(input);
      }
      
      displayInput(input);
   }//filter

   public final void filterActions(String input) {
      if ((hm == null) || !hm.getActionManager().getActionsEnabled()) {
         return;
      }

      int len = input.length();
      char ch;
      
      for (int i = 0; i<len; i++) {
         if ((ch = input.charAt(i)) == '\n') {
            filterLine(current_line.toString());
            current_line = new StringBuffer(100);
         }
         else {
            current_line.append(ch);
         }
      }//for
      /* when here, all but the last line (and only if it doesn't have
         a newline at the end of it) has been filtered.  Lets filter this
         remainder in case someone want's to action off of a prompt.
         For example:  Enter thy name:  is a favorite.
         */
      if (filterLine(current_line.toString()) == true) {
         current_line = new StringBuffer(100); /*make sure we don't do an
                                                 action on it again... */
      }
      return; //just wanted to look :)
   }//filterActions()


   /** returns true if it did an action, false otherwise */
   private boolean filterLine(String ln) {
        
      int idx = 0;
      int len = ln.length();
      String num;
      Enumeration e;
      boolean retval = false;
      
      /* check for actions */
      if ((sink != null) && (actions != null)) {
         Action acts_vect[] = actions.getActions();
         Action a;
      
         int acts_len = 0;
         if (acts_vect != null) {
            acts_len = acts_vect.length;
         }

         for (int i = 0; i<acts_len; i++) {
            a = acts_vect[i];
            if ((idx = ln.indexOf(a.getTrigger())) != -1) {
               retval = true;
               sink.send(a.getAction() + "\n");
            }//if
         }//for
      }//if sink, ie if we are doing actions
      return retval;
   }//filterLine()

   protected void signalCase(int which_un) {
      if (olc_store != null) {
         olc_store.processCase(which_un);
      }
   }

   void displayInput(String txt) {
      Log.instance().io("\nin displayInput, last_was_lt: " + last_was_lt
                        + " last_was_gt: " + last_was_gt + " disp_sofar -:"
                        + disp_sofar + " tag_sofar: " + tag_sofar + " in_tag: "
                        + in_tag + " txt:  -:" + txt + ":-");
      int len = txt.length();
      char ch;

      for (int i = 0; i<len; i++) {
         ch = txt.charAt(i);

         if (last_was_lt && !in_tag) {
            if (ch == '<') {
               disp_sofar.append(ch);
               last_was_lt = false;
               continue;
            }
            else {
               dispenseText(disp_sofar.toString());
               disp_sofar = new StringBuffer(1000);
               in_tag = true;
            }
         }//if last was a less-than
         
         if (in_tag) {
            if (last_was_gt) {
               if (ch == '>') {
                  tag_sofar.append(ch);
                  last_was_gt = false;
               }
               else {
                  // When here, we got a tag...
                  if (!processTag((tag_sofar.toString()).trim())) {
                     //Log.it("Could not process tag:  " + tag_sofar);
                     dispenseText("<" + tag_sofar.toString() + ">");
                  }

                  // Clear the slate, ready to start over...
                  tag_sofar = new StringBuffer(50);
                  last_was_lt = false;
                  in_tag = false;
                  last_was_gt = false;
                  i--; //need to re-look at this character...
               }//if last was greater-than.
            }//if last was greater-than
            else {
               if ((ch == '=') || (Character.isWhitespace(ch))) {
                  if (!last_tag_was_space)
                     tag_sofar.append(' ');
                  last_tag_was_space = true;
               }
               else {
                  last_tag_was_space = false;
                  if (ch == '>') {
                     last_was_gt = true;
                  }
                  else {
                     last_was_gt = false;
                     tag_sofar.append(ch);
                  }
               }//else
            }//else
         }//if in_tag
         else {
            if (ch == '<') {
               last_was_lt = true;
            }// if not a less than
            else {
               disp_sofar.append(ch);
               last_was_lt = false;
            }
         }//else, not in tag
      }//for
      
      if (disp_sofar.length() > 0) {
         dispenseText(disp_sofar.toString());
         disp_sofar = new StringBuffer(1000);
      }//if
      
      heg_disp.paintScroll();
   }//displayInput


   // Depending on which case we're in, send text to certain places..
   void dispenseText(String txt) {
      Log.instance().dbg("DispenseText, context_mode: " + context_mode 
                         + " text -:" + txt + ":-\n");
      if (context_mode.equals(MODE_NORMAL)) {
         heg_scroll.append(txt);
         return;
      }//if
      else if (context_mode.equals(MODE_HELP_PAGE)) {
         hm.getHelpFrame().appendAndCache(txt);
         hm.getHelpFrame().getHegemonDisplay().paintScroll();

         if (heg_scroll != null) {
            heg_scroll.append(txt);
         }
         return;
      }//if
      else if (context_mode.equals(MODE_STAT_SD)) {
         hm.getOlEditor().appendShortDesc(txt);
         heg_scroll.append(txt);
      }
      else if (context_mode.equals(MODE_STAT_ND)) {
         hm.getOlEditor().appendInRoomDesc(txt);
         heg_scroll.append(txt);
      }
      else if (context_mode.equals(MODE_STAT_LD)) {
         hm.getOlEditor().appendLongDesc(txt);
         heg_scroll.append(txt);
      }
      else if (context_mode.equals(MODE_KEYWORD_LD)) {
         hm.getKeywordEditor().appendDesc(txt);
         //heg_scroll.append(txt);
      }
      else if (context_mode.equals(MODE_PATH_CELL_STAT_LD)) {
         hm.getPathCellEditor().appendLongDesc(txt);
         //heg_scroll.append(txt);
      }
      else if (context_mode.equals(MODE_MSCRIPT_DATA)) {
         hm.getMobScriptEditor().appendScript(txt);
         heg_scroll.append(txt);
      }
      else if (context_mode.equals(MODE_BUG_TITLE)) {
         hm.getBugListEditor().appendTitle(txt);
      }
      else if (context_mode.equals(MODE_BUG_COMMENT_ENTRY)) {
         hm.getBugListEditor().appendComment(txt);
      }
      else {
         Log.instance().err("ERROR:  default case in dispenseText.");
      }
   }//dispenseText

   
   /**  Ok then, have a tag, minus the < and the >.  We ASSUME that
    exactly one SPACE seperates each token.  And it DOES NOT START OR
    END WITH A SPACE. Returns true if was able to comprehend the tag,
    else returns false. */
   
   private final boolean processTag(String tag) {
      Log.instance().io("in processTag, tag:  -:" + tag + ":-");

      Vector args = new Vector();

      int len = tag.length();
      boolean in_quotes = false;
      StringBuffer sb = new StringBuffer(50);
      char ch;
      boolean is_escaped = false;
      for (int i = 0; i<len; i++) {
         ch = tag.charAt(i);
         if (ch == '\\') {
            if (is_escaped) {
               sb.append(ch);
            }
            else {
               is_escaped = true;
            }
         }
         else if (ch == '\"') {
            if (is_escaped) {
               sb.append(ch);
            }
            else {
               //end or beginning of quoted tag
               if (in_quotes) { //at the end then
                  args.addElement(sb.toString());
                  sb = new StringBuffer(50);
                  in_quotes = false;
               }
               else {
                  in_quotes = true;
               }
            }//else
         }//if got a quote
         else if (ch == ' ') {
            if (in_quotes) {
               sb.append(ch);
            }
            else {
               if (sb.length() > 0) {
                  args.addElement(sb.toString());
                  sb = new StringBuffer(50);
               }
            }
         }//if
         else {
            sb.append(ch);
         }
      }//for

      // Get the last one...
      if (sb.length() > 0) {
         args.addElement(sb.toString());
      }

      /* when here, got all our args in a row. */
      return processArgs(args);
   }//processTag

   /** return value is true if cmd was recognized, false otherwise. */
   private final boolean processArgs(Vector args) {
      int len = args.size();
      if (len == 0)
         return false; //blah, all fer nothing!
      boolean valid = false;
      
      String cmd = (String)(args.elementAt(0));
      if ((Log.instance().getLevel() & Log.IO) > 0) {
         Log.instance().io("\nin processArgs(), COMMAND:  -:" + cmd 
                           + ":- len: " + len + "\n");
         for (int i = 0; i<len; i++) {
            Log.instance().io("\targ[" + i + "] -:" + (String)(args.elementAt(i))
                              + ":-");
         }//for
      }//if
      
      /* now our large switch of different commands... */
      TagCommand tg = (TagCommand)(tag_hash.get(cmd.toUpperCase()));
      if (tg != null) {
         try {
            return tg.run(args);
         }
         catch (Exception e) {
            e.printStackTrace();
            return false;
         }
      }
      return false;
   }//processArgs

   void setMobScriptData(String trig, String mnum, String actor_num,
                         String target_num, String precedence, String entity,
                         String script_idx) {
      hm.getMobScriptEditor().setMobScriptData(trig, mnum, actor_num,
                                               target_num, precedence, script_idx);
      hm.getMobScriptEditor().setEntity(entity);
   }

   void setMobScriptDiscrim(String discrim) {
      hm.getMobScriptEditor().setMobScriptDiscrim(discrim);
   }
   
}//HegemonInputFilter class
