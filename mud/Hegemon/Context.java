// $Id: Context.java,v 1.5 2001/03/31 07:05:59 greear Exp $
// $Revision: 1.5 $  $Author: greear $ $Date: 2001/03/31 07:05:59 $

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




class Context extends Object {
   int ctx_num;
   String name;

   public final static Context NONE = new Context(0, "None");
   public final static Context GOSSIP = new Context(1, "Gossip");
   public final static Context RM_DESC = new Context(2, "Room Desc");
   public final static Context DOOR_LIST = new Context(3, "Door List");
   public final static Context MOB_LIST = new Context(4, "Mob List");
   public final static Context ITEM_LIST = new Context(5, "Item List");
   public final static Context RM_SHORT_DESC = new Context(6,
                                                           "Room Short Desc");
   public final static Context TELL = new Context(7, "Tell");
   public final static Context YELL = new Context(8, "Yell");
   public final static Context AUCTION = new Context(9, "Auction");
   public final static Context BATTLE = new Context(10, "Battle");
   public final static Context OTHER = new Context(11, "Other");
   public final static Context SAY = new Context(12, "Say");
   public final static Context BACKGROUND = new Context(13, "Background");
   public final static Context INPUT = new Context(14, "Input");

   /** this is just a big ole switch statement */
   public static Context getContext(int num) {
      switch (num) {
       case 0:
         return NONE;
       case 1:
         return GOSSIP;
       case 2:
         return RM_DESC;
       case 3:
         return DOOR_LIST;
       case 4:
         return MOB_LIST;
       case 5:
         return ITEM_LIST;
       case 6:
         return RM_SHORT_DESC;
       case 7:
         return TELL;
       case 8:
         return YELL;
       case 9:
         return AUCTION;
       case 10:
         return BATTLE;
       case 11:
         return OTHER;
       case 12:
         return SAY;
       case 13:
         return BACKGROUND;
       case 14:
         return INPUT;
       default:
         Log.instance().err("ERROR:  in getContext:  unknown context number:  "
                            + num);
         return NONE;
      }
   }
   
   public Context() {
      ctx_num = 0;
      name = null;
   }

   public Context(int num, String desc) {
      ctx_num = num;
      name = desc;
   }

   public final String getName() {
      return name;
   }

   public final int getNum() {
      return ctx_num;
   }
}//Context
