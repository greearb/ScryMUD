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


// rm_cmds.cc, commands that rooms 'do' to others.  Used in room scripts
//            primarily.

#include "misc.h"
#include "misc2.h"
#include "rm_cmds.h"



/* Only works for those in the current room!! */
int tell(int i, const String* name, String& msg, room& rm) {
   critter* crit_ptr;
   int count = 0, z;

   crit_ptr = rm.haveCritNamed(i_th, targ, ~0);

   if (crit_ptr) {
      do_tell(rm, msg, *crit_ptr, TRUE);
      return;
   }//if
}//tell


void do_tell(room& rm, const char* msg, critter& targ) {

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

   Sprintf(buf, "%SThe surroundings whisper to you, '%S'\n%S",
           &tag, &msg, &untag);
   show(buf, targ);

}//do_tell


void com_recho(const String* msg, room& rm) {
   String buf2(100);

   Cell<critter*> cll(rm.getCrits());
   critter* ptr;
   
   while ((ptr = cll.next())) {
      if (ptr->isImmort()) {
         Sprintf(buf2, "[RECHO]  %S", msg);
         show(buf2, *ptr);
      }
      else {
         show(*msg, *ptr);
         show("\n", *ptr);
      }
   }//while
}//com_gecho
   

void com_zecho(const String* msg, room& rm) {
   String buf2(100);

   Cell<critter*> cll(pc_list);
   critter* ptr;

   while ((ptr = cll.next())) {
      if (ptr->getCurZoneNum() == rm.getZoneNum()) {
         if (ptr->isImmort()) {
            Sprintf(buf2, "[ZECHO]  %S", msg);
            show(buf2, *ptr);
         }
         else {
            show(*msg, *ptr);
            show("\n", *ptr);
         }
      }//while
   }//if
}//com_zecho


void wizchat(const char* message, room& rm) {
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

         Sprintf(buf, "%S\n[ROOM %i] wizchats, '%S'\n%S", 
                       &tag, rm.getIdNum(), &msg, &untag);
         show(buf, *crit_ptr); 
      }//if
   }//while
}//wizchat






