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


/**
 * Class that represents an Action.
 */
class Action extends Object {
   String trigger;
   String action;
   boolean is_anchored; //at the first of the line
   boolean case_sensitive;

   public Action() {
      trigger = new String();
      action = new String();
      is_anchored = false;
      case_sensitive = false;
   }
   
   public Action(String glom) {
      int idx = glom.indexOf("|");
      int len = glom.length();
      
      if (idx >= 0) {
         trigger = (glom.substring(0, idx)).trim();
         idx++;
      }
      else {
         Log.instance().err("ERROR:  glom is bad:  Action constructor");
         return;
      }

      action = (glom.substring(idx, len)).trim();

      /* if ANY of the characters are upper case, then the whole
         thing will be case sensitive */
      for (int i = 0; i<len; i++) {
         if (Character.isUpperCase(glom.charAt(i))) {
            case_sensitive = true;
            break;
         }
      }

      if (trigger.length() < 1)
         return;
      else if (trigger.charAt(0) == '^') {
         is_anchored = true;
         trigger = (trigger.substring(1, trigger.length())).trim();
      }
   }//Constructor from one string
   
   public Action(String trig, String act) {
      super();
      setAction(trig, act, false, false);
   }

   public Action(String trig, String act, boolean ia, boolean cs) {
      super();
      setAction(trig, act, ia, cs);
   }

   protected final void setAction(String trig, String act,
                                  boolean ia, boolean cs) {
      trigger = trig.trim();
      action = act.trim();
      is_anchored = ia;
      case_sensitive = cs;
   }

   final public boolean getAnchored() {
      return is_anchored;
   }

   final public boolean getCaseSensitive() {
      return case_sensitive;
   }

   final public String getTrigger() {
      return trigger;
   }

   final public String getAction() {
      return action;
   }

   public String toString() {
      return (trigger + " | " + action);
   }
}
