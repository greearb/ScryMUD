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



class Alias extends Object {
   String trigger;
   String action;

   public Alias() {
      trigger = new String();
      action = new String();
   }
   
   public Alias(String glom) {
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
   }//Constructor from one string
   
   public Alias(String trig, String act) {
      super();
      setAlias(trig, act);
   }

   protected final void setAlias(String trig, String act) {
      trigger = trig.trim();
      action = act.trim();
   }

   final public String getAlias() {
      return trigger;
   }

   final public String getResponse() {
      return action;
   }

   public String toString() {
      return (trigger + " | " + action);
   }
}
