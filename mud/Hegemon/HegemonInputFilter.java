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

   Integer context_mode;

   // PONDERING:  a case for every different thing we want to capture??
   public static final Integer MODE_NORMAL            = new Integer(0);
   public static final Integer MODE_HELP_PAGE         = new Integer(1);
   public static final Integer MODE_STAT_SD           = new Integer(2);
   public static final Integer MODE_STAT_ND           = new Integer(3);
   public static final Integer MODE_STAT_LD           = new Integer(4);
   public static final Integer MODE_MSCRIPT_DATA      = new Integer(5);
   public static final Integer MODE_PATH_CELL_STAT_LD = new Integer(6);
   public static final Integer MODE_KEYWORD_LD = new Integer(7);
   

   StringBuffer tag_sofar;
   StringBuffer disp_sofar;
   
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
      in_tag = false;
      tag_sofar = new StringBuffer(50);
      disp_sofar = new StringBuffer(1000);
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
   }//filter()


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
         if (acts_vect != null)
           acts_len = acts_vect.length;
      
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
      //Log.it("in displayInput, txt:  -:" + txt + ":-");
      int len = txt.length();
      char ch;

      for (int i = 0; i<len; i++) {
         ch = txt.charAt(i);

         if (last_was_lt) {
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

            if ((ch == '=') || (Character.isWhitespace(ch))) {
               if (!last_tag_was_space)
                 tag_sofar.append(' ');

               last_tag_was_space = true;
               continue;
            }
            else {
               last_tag_was_space = false;
            }
         
            if (ch == '>') {
               if (processTag((tag_sofar.toString()).trim())) {
                  //Log.it("Could process tag:  " + tag_sofar);
                  tag_sofar = new StringBuffer(50);
                  last_was_lt = false;
                  in_tag = false;
               }
               else {
                  //Log.it("Could not process tag:  " + tag_sofar);
                  dispenseText("<" + tag_sofar.toString() + ">");
                  tag_sofar = new StringBuffer(50);
                  last_was_lt = false;
                  in_tag = false;
               }
            }//if going out of a tag
            else {
               tag_sofar.append(ch);
            }
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
      }//if
      
      if (disp_sofar.length() > 0) {
         dispenseText(disp_sofar.toString());
         disp_sofar = new StringBuffer(1000);
      }//if
      
      heg_disp.paintScroll();
   }//displayInput


   // Depending on which case we're in, send text to certain places..
   void dispenseText(String txt) {

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
      else {
         Log.instance().err("ERROR:  default case in dispenseText.");
      }
   }//dispenseText

   
   /**  Ok then, have a tag, minus the < and the >.  We ASSUME that
    exactly one SPACE seperates each token.  And it DOES NOT START OR
    END WITH A SPACE. Returns true if was able to comprehend the tag,
    else returns false. */
   
   private final boolean processTag(String tag) {
      Log.instance().dbg("in processTag, tag:  -:" + tag + ":-");
      
      int idx;
      int last_idx = 0;
      
      Vector args = new Vector();
      
      idx = tag.indexOf(' ', last_idx);
      while (idx != -1) {
         args.addElement(tag.substring(last_idx, idx));
         last_idx = idx + 1; //walk past terminating space
         idx = tag.indexOf(' ', last_idx);
      }//while
      
      args.addElement(tag.substring(last_idx,
                                    tag.length())); /* last one not terminated
                                                       by a space... */

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
      Log.instance().dbg("\nin processArgs(), COMMAND:  -:" + cmd 
                         + ":- len: " + len + "\n");

      //First of all, lets deal with variable length ones (puke!)
      if (cmd.equalsIgnoreCase("DISCRIM")) {
         StringBuffer buf = new StringBuffer(100);
         for (int i = 1; i<len; i++) {
            buf.append((String)(args.elementAt(i)) + " ");
         }
         setMobScriptDiscrim(buf.toString());
         return true;
      }//if
      else if (cmd.equalsIgnoreCase("KEYWORD")) {
         //first one is keyword number
         hm.getKeywordEditor().do_clear();
         hm.getKeywordEditor().setCellNum((String)(args.elementAt(1)));

         StringBuffer buf = new StringBuffer(100);
         // the rest are names
         for (int i = 2; i<len; i++) {
            buf.append((String)(args.elementAt(i)) + " ");
         }
         hm.getKeywordEditor().setNames(buf.toString());
         return true;
      }
      else if (cmd.equalsIgnoreCase("NAMES")) {
         StringBuffer buf = new StringBuffer(100);
         // the rest are names
         for (int i = 1; i<len; i++) {
            buf.append((String)(args.elementAt(i)) + " ");
         }
         hm.getOlEditor().setNames(buf.toString());
         return true;
      }//else
      else if (cmd.equalsIgnoreCase("EXITS")) {
         hm.getClientDisplay().getInputField().getExitNavigator().setDirs(args);
         return true;
      }//else

      /* now our large switch of different commands... */
      if (len == 1) {
         if (cmd.equalsIgnoreCase("BATTLE")) {
            Color c = color_mgr.getBattleSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/BATTLE")) {
            if (color_mgr.getBattleSelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/FONT")) {
            heg_scroll.getProperties().popFont();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("TELL")) {
            Color c = color_mgr.getTellSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/TELL")) {
            if (color_mgr.getGossipSelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("DOOR_LIST")) {
            Color c = color_mgr.getDoorListSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/DOOR_LIST")) {
            if (color_mgr.getDoorListSelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("MOB_LIST")) {
            Color c = color_mgr.getMobListSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/MOB_LIST")) {
            if (color_mgr.getMobListSelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("ITEM_LIST")) {
            Color c = color_mgr.getItemListSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/ITEM_LIST")) {
            if (color_mgr.getItemListSelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("RM_DESC")) {
            Color c = color_mgr.getRoomDescSelection();
            heg_scroll.getProperties().pushContextMode(MODE_STAT_LD);
            context_mode = MODE_STAT_LD;
            if (c != null)
              heg_scroll.pushColor(c);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/RM_DESC")) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            if (color_mgr.getRoomDescSelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/RM_SHORT_DESC")) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            if (color_mgr.getRoomShortDescSelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("GOSSIP")) {
            Color c = color_mgr.getGossipSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/GOSSIP")) {
            if (color_mgr.getGossipSelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("SAY")) {
            Color c = color_mgr.getSaySelection();
            if (c != null)
              heg_scroll.pushColor(c);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/SAY")) {
            if (color_mgr.getSaySelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("YELL")) {
            Color c = color_mgr.getYellSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/YELL")) {
            if (color_mgr.getYellSelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("AUCTION")) {
            Color c = color_mgr.getAuctionSelection();
            if (c != null)
              heg_scroll.pushColor(c);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/AUCTION")) {
            if (color_mgr.getAuctionSelection() != null) 
              heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("TAB")) {
            heg_scroll.getProperties().pushTab();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/TAB")) {
            heg_scroll.getProperties().popTab();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/HELP")) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            hm.getHelpFrame().saveHelpText(); //write it to disk
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("P")) {
            heg_scroll.drawNewline();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("PRE")) {
            heg_scroll.getProperties().pushMode(heg_scroll.
                                                getProperties().MODE_PRE);
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/PRE")) {
            heg_scroll.getProperties().popMode();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/COLOR")) {
            heg_scroll.getProperties().popColor();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("STAT_SD")) {
            heg_scroll.getProperties().pushContextMode(MODE_STAT_SD);
            context_mode = MODE_STAT_SD;
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("STAT_ND")) {
            heg_scroll.getProperties().pushContextMode(MODE_STAT_ND);
            context_mode = MODE_STAT_ND;
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("STAT_LD")) {
            heg_scroll.getProperties().pushContextMode(MODE_STAT_LD);
            context_mode = MODE_STAT_LD;
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("KEYWORD_DESC")) {
            heg_scroll.getProperties().pushContextMode(MODE_KEYWORD_LD);
            context_mode = MODE_KEYWORD_LD;
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("PSTAT_LD")) {
            heg_scroll.getProperties().pushContextMode(MODE_PATH_CELL_STAT_LD);
            context_mode = MODE_PATH_CELL_STAT_LD;
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("MSCRIPT_DATA")) {
            heg_scroll.getProperties().pushContextMode(MODE_MSCRIPT_DATA);
            context_mode = MODE_MSCRIPT_DATA;
            valid = true;

         }
         else if (cmd.equalsIgnoreCase("/KEYWORD_DESC")) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/MSCRIPT_DATA")) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("/STAT")) {
            heg_scroll.getProperties().popContextMode();
            context_mode = heg_scroll.getProperties().getContextMode();
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("ENGAGE_HEGEMON")) {
            try {
               hm.getSocketManager().write("using_client\n");
            } catch (Exception e) { e.printStackTrace(); }
            valid = true;
         }
      }//if len == 1
      else if (len == 2) { //all these require exactly two
         if (cmd.equalsIgnoreCase("RM_SHORT_DESC")) {
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
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("FONT")) {
            heg_scroll.pushFont((String)(args.elementAt(1)));
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("COLOR")) {
            heg_scroll.pushColor((String)(args.elementAt(1)));
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("MSTAT")) {
            OlEditor tmp = hm.getOlEditor();
            if (!tmp.isFrozen()) {
               int i = Integer.parseInt((String)(args.elementAt(1)));
               tmp.do_clear();
               tmp.setObjNum(i);
               tmp.setObjType(OlEditor.MOB);
            }
            valid = true;
         }//if
         else if (cmd.equalsIgnoreCase("OSTAT")) {
            OlEditor tmp = hm.getOlEditor();
            if (!tmp.isFrozen()) {
               int i = Integer.parseInt((String)(args.elementAt(1)));
               tmp.do_clear();
               tmp.setObjNum(i);
               tmp.setObjType(OlEditor.OBJ);
            }
            valid = true;
         }//if
         else if (cmd.equalsIgnoreCase("PHASE")) {
            /* check for olc triggers */

            int tmp = Integer.parseInt((String)(args.elementAt(1)));
         
            if (tmp == last_phase) {
               MessageDialog md =
                 new MessageDialog("ALERT",
                                   "You must enter OLC data at your prompt.",
                                   "yellow", "black");
               valid = true;
            }//if
            else {
               //           System.out.println("NUM: " + tmp);
               last_phase = tmp;
               signalCase(last_phase);
            }
         }//if olc case
         else if (cmd.equalsIgnoreCase("PATH_IS_DEST")) {
            hm.getPathCellEditor().setIsDest((String)(args.elementAt(1)));
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("H")) {
            try {
               heg_scroll.getProperties().pushTextSize(new Integer((String)(args.elementAt(1))));
               valid = true;
            }
            catch (Exception e) {
               Log.instance().err(e.toString());
               valid = true;
            }
         }
      }//else, those who require two arguments
      else if (len == 3) {
         if (cmd.equalsIgnoreCase("HELP")) {
            heg_scroll.getProperties().pushContextMode(MODE_HELP_PAGE);
            context_mode = MODE_HELP_PAGE;
            HelpFrame tmp = hm.getHelpFrame();
            tmp.setHelpCategory((String)args.elementAt(1));
            tmp.setHelpTopic((String)args.elementAt(2));
            valid = true;
         }//if
         else if (cmd.equalsIgnoreCase("PSTAT")) {
            hm.getPathCellEditor().do_clear();         // PathCell Stat
            hm.getPathCellEditor().setVehNum((String)(args.elementAt(1)));
            hm.getPathCellEditor().setCellNum((String)(args.elementAt(2)));
            valid = true;
         }
         else if (cmd.equalsIgnoreCase("PATH_DIR")) {
            hm.getPathCellEditor().setDir((String)(args.elementAt(1)),
                                          (String)(args.elementAt(2)));
            valid = true;
         }
      }//if
      else if (len == 6) {
         if (cmd.equalsIgnoreCase("MOB_SCRIPT")) {
            hm.getMobScriptEditor().do_clear();
            setMobScriptData((String)(args.elementAt(1)),
                             (String)(args.elementAt(2)),
                             (String)(args.elementAt(3)),
                             (String)(args.elementAt(4)),
                             (String)(args.elementAt(5)));
            valid = true;
         }//if
      }//if
         
      return valid;
   }//processArgs

   void setMobScriptData(String trig, String mnum, String actor_num,
                         String target_num, String precedence) {
      hm.getMobScriptEditor().setMobScriptData(trig, mnum, actor_num,
                                               target_num, precedence);
   }

   void setMobScriptDiscrim(String discrim) {
      hm.getMobScriptEditor().setMobScriptDiscrim(discrim);
   }
   
}//HegemonInputFilter class
