// $Id: obj_cmds.cc,v 1.4 2001/03/29 03:02:32 eroper Exp $
// $Revision: 1.4 $  $Author: eroper $ $Date: 2001/03/29 03:02:32 $

//
//ScryMUD Server Code
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


// obj_cmds.cc, commands that objects 'do' to others.  Used in room scripts
//            primarily.

#include "misc.h"
#include "misc2.h"
#include "rm_cmds.h"



/* Only works for those in the current room!! */
int object::tell(int i, const String* name, String& msg, room& rm) {
   critter* crit_ptr = rm.haveCritNamed(i, name, ~0);

   if (crit_ptr) {
      return do_tell(msg, *crit_ptr);
   }//if
   return -1;
}//tell


int object::do_tell(const char* msg, critter& targ) {

   String buf(200);

   String tag;
   String untag;

   if (targ.isUsingClient()) {
      tag = "<TELL>";
      untag = "</TELL>";
   }
   else if (targ.isUsingColor()) {
      tag = *(targ.getTellColor());
      untag = *(targ.getDefaultColor());
   }

   Sprintf(buf, "%S%s\n%S",
           &tag, msg, &untag);
   show(buf, targ);

   return 0;
}//do_tell


int object::com_recho(const String* msg, room& rm) {
   return rm.com_recho(msg);
}
   

int object::com_zecho(const String* msg, room& rm) {
   return rm.com_zecho(msg);
}


int object::wizchat(const char* message, room& rm) {
   Cell<critter*> cell(pc_list);
   critter* crit_ptr;
   String buf(200);
   String msg(message);

   String tag;
   String untag;

   while ((crit_ptr = cell.next())) {
      if (crit_ptr->getMode() == MODE_LOGGING_IN){
         continue;
      }
      if (crit_ptr->isWizchat()) { //if channel wizchat
         if (crit_ptr->isUsingClient()) {
            tag = "<GOSSIP>";
            untag = "</GOSSIP>";
         }
         else if (crit_ptr->isUsingColor()) {
            tag = *(crit_ptr->getGossipColor());
            untag = *(crit_ptr->getDefaultColor());
         }
         else {
            tag = "";
            untag = "";
         }

         Sprintf(buf, "%S\n[OBJ in RM %i] wizchats, '%S'\n%S", 
                       &tag, rm.getIdNum(), &msg, &untag);
         crit_ptr->show(buf); 
      }//if
   }//while
   return 0;
}//wizchat


/**  Move all in room out some door.  Does no checks, just puts em
 * through the door, even if it's closed??!!
 */
int object::move_all(int i_th, const String* dir, room& rm) {
   return rm.move_all(i_th, dir);
}


/**  Move all objects in room out some door.  Does no checks, just puts em
 * through the door, even if it's closed??!!
 */
int object::omove_all(int i_th, const String* dir, room& rm) {
   return rm.omove_all(i_th, dir);
}


int object::move(int i_th, const String* pc, int j_th, const String* dir, room& rm) {
   return rm.move(i_th, pc, j_th, dir);
}


int object::omove(int i_th, const String* obj, int j_th, const String* dir, room& rm) {
   return rm.omove(i_th, obj, j_th, dir);
}


/** Echo message into the room in this direction */
int object::neighbor_echo(int i_th, const String* dir, const String& buf, room& rm) {
   return rm.neighbor_echo(i_th, dir, buf);
}


int object::obj_pause(int ticks) {
   pause += ticks;

   if (pause < 0)
      pause = 0;

   return 0;
}
