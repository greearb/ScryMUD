// $Id: rm_cmds.cc,v 1.11 2001/03/29 03:02:33 eroper Exp $
// $Revision: 1.11 $  $Author: eroper $ $Date: 2001/03/29 03:02:33 $

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
int room::tell(int i, const String* name, String& msg) {
   critter* crit_ptr = haveCritNamed(i, name, ~0);

   if (crit_ptr) {
      return do_tell(msg, *crit_ptr);
   }//if
   return -1;
}//tell


int room::do_tell(const char* msg, critter& targ) {

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


int room::com_recho(const String* msg) {
   String buf2(100);

   Cell<critter*> cll(critters);
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
   return 0;
}//com_recho
   

int room::com_zecho(const String* msg) {
   String buf2(100);

   Cell<critter*> cll(pc_list);
   critter* ptr;
   int znum = getZoneNum();

   while ((ptr = cll.next())) {
      if (ptr->getCurZoneNum() == znum) {
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
   return 0;
}//com_zecho


int room::wizchat(const char* message) {
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
                       &tag, getIdNum(), &msg, &untag);
         crit_ptr->show(buf); 
      }//if
   }//while
   return 0;
}//wizchat


/**  Move all in room out some door.  Does no checks, just puts em
 * through the door, even if it's closed??!!
 */
int room::move_all(int i_th, const String* dir) {
   String buf(100);
   critter* ptr;
   door* door_ptr = door::findDoor(DOORS, i_th, dir, ~0, *this);
   if (!door_ptr || !door_ptr->getDestRoom()) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << __FUNCTION__ << " failed, no door or dest room\n";
      }
      return -1;
   }

   int dest_rm = door_ptr->getDestRoom()->getIdNum();
   int is_dead;

   if ((dest_rm >= 0) && (dest_rm < NUMBER_OF_ROOMS) && room_list[dest_rm].isInUse()) {
      while (!critters.isEmpty()) {
         ptr = critters.peekFront();
         is_dead = FALSE;

         ptr->doGoToRoom(dest_rm, NULL, NULL, is_dead, getIdNum(), 1);
         
      }//while
   }//if
   return 0;
}//move_all

/**  Move all in room out some door.  Does no checks, just puts em
 * through the door, even if it's closed??!!
 */
int room::transport_all(int dest_rm) {
   String buf(100);
   critter* ptr;

   int is_dead;

   if ((dest_rm >= 0) && (dest_rm < NUMBER_OF_ROOMS) && room_list[dest_rm].isInUse()) {
      while (!critters.isEmpty()) {
         ptr = critters.peekFront();
         is_dead = FALSE;

         ptr->doGoToRoom(dest_rm, NULL, NULL, is_dead, getIdNum(), 1);
         
      }//while
   }//if
   return 0;
}//transport_all

/**  Move all objects in room to some other room.  Does no checks, just puts em
 * in the new room.
 */
int room::otransport_all(int dest_rm) {
   String buf(100);
   object* ptr;

   if ((dest_rm >= 0) && (dest_rm < NUMBER_OF_ROOMS) && room_list[dest_rm].isInUse()) {
      while (!inv.isEmpty()) {
         ptr = inv.peekFront();
         ptr->doGoToRoom(dest_rm, NULL, NULL, getIdNum(), 1);
      }//while
   }//if
   return 0;
}//omove_all


/**  Move all objects in room out some door.  Does no checks, just puts em
 * through the door, even if it's closed??!!
 */
int room::omove_all(int i_th, const String* dir) {
   String buf(100);
   object* ptr;
   door* door_ptr = door::findDoor(DOORS, i_th, dir, ~0, *this);
   if (!door_ptr || !door_ptr->getDestRoom()) {
      return -1;
   }

   int dest_rm = door_ptr->getDestRoom()->getIdNum();

   if ((dest_rm >= 0) && (dest_rm < NUMBER_OF_ROOMS)) {
      while (!inv.isEmpty()) {
         ptr = inv.peekFront();
         ptr->doGoToRoom(dest_rm, NULL, NULL, getIdNum(), 1);
      }//while
   }//if
   return 0;
}//omove_all


int room::move(int i_th, const String* pc, int j_th, const String* dir) {
   String buf(100);
   critter* ptr;
   
   if (mudlog.ofLevel(DBG)) {
      mudlog << "room::move:  i_th: " << i_th
             << " pc -:" << *pc << ":- j_th: " << j_th
             << "  dir -:" << dir << ":- in room:  " << getIdNum()
             << endl;
   }


   door* door_ptr = door::findDoor(DOORS, j_th, dir, ~0, *this);
   if (!door_ptr || !door_ptr->getDestRoom()) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "room::move:  could not find door, j_th: " << j_th
                << "  dir -:" << dir << ":- in room:  " << getIdNum()
                << endl;
      }
      return -1;
   }

   int dest_rm = door_ptr->getDestRoom()->getIdNum();

   /* deal with some special cases */

   if (*pc == "__RANDOM__") {
      ptr = critters.elementAt(d(1, critters.size()) - 1);
   }//if pick a random one
   else if (*pc == "__TOP__") {
      ptr = critters.peekFront();
   }
   else if (*pc == "__BOTTOM__") {
      ptr = critters.peekBack();
   }
   else {
      ptr = haveCritNamed(i_th, pc, ~0);
   }

   if (!ptr) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "room::move:  could not find critter, i_th: "
                << i_th << " name -:" << *pc << ":-" << endl;
      }
      return -1;
   }

   if ((dest_rm >= 0) && (dest_rm < NUMBER_OF_ROOMS) && room_list[dest_rm].isInUse()) {
      int is_dead;
      ptr->doGoToRoom(dest_rm, NULL, NULL, is_dead, getIdNum(), 1);
   }//if
   else {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "room::move:  dest_rm: " << dest_rm << " is out of range."
                << endl;
      }
      return -1;
   }
   return 0;
}//move


int room::transport(int i_th, const String* pc, int dest_rm) {
   String buf(100);
   critter* ptr;
   
   if (mudlog.ofLevel(DBG)) {
      mudlog << "room::move:  i_th: " << i_th
             << " pc -:" << *pc << ":- in room:  " << getIdNum()
             << endl;
   }

   /* deal with some special cases */

   if (*pc == "__RANDOM__") {
      ptr = critters.elementAt(d(1, critters.size()) - 1);
   }//if pick a random one
   else if (*pc == "__TOP__") {
      ptr = critters.peekFront();
   }
   else if (*pc == "__BOTTOM__") {
      ptr = critters.peekBack();
   }
   else {
      ptr = haveCritNamed(i_th, pc, ~0);
   }

   if (!ptr) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "room::transport:  could not find critter, i_th: "
                << i_th << " name -:" << *pc << ":-" << endl;
      }
      return -1;
   }

   if ((dest_rm >= 0) && (dest_rm < NUMBER_OF_ROOMS) && room_list[dest_rm].isInUse()) {
      int is_dead;
      ptr->doGoToRoom(dest_rm, NULL, NULL, is_dead, getIdNum(), 1);
   }//if
   else {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "room::transport:  dest_rm: " << dest_rm << " is out of range."
                << endl;
      }
      return -1;
   }
   return 0;
}//transport


int room::omove(int i_th, const String* obj, int j_th, const String* dir) {
   String buf(100);
   object* ptr;
   door* door_ptr = door::findDoor(DOORS, i_th, dir, ~0, *this);
   if (!door_ptr || !door_ptr->getDestRoom()) {
      return -1;
   }

   int dest_rm = door_ptr->getDestRoom()->getIdNum();

   /* deal with some special cases */

   if (*obj == "__RANDOM__") {
      ptr = inv.elementAt(d(1, inv.size()) - 1);
   }//if pick a random one
   else if (*obj == "__TOP__") {
      ptr = inv.peekFront();
   }
   else if (*obj == "__BOTTOM__") {
      ptr = inv.peekBack();
   }
   else {
      ptr = haveObjNamed(i_th, obj, ~0);
   }

   if (!ptr) {
      return -1;
   }

   if ((dest_rm >= 0) && (dest_rm < NUMBER_OF_ROOMS) && room_list[dest_rm].isInUse()) {
      return ptr->doGoToRoom(dest_rm, NULL, NULL, getIdNum(), 1);
   }//if
   return -1;
}//omove


int room::otransport(int i_th, const String* obj, int dest_rm) {
   String buf(100);
   object* ptr;

   /* deal with some special cases */

   if (*obj == "__RANDOM__") {
      ptr = inv.elementAt(d(1, inv.size()) - 1);
   }//if pick a random one
   else if (*obj == "__TOP__") {
      ptr = inv.peekFront();
   }
   else if (*obj == "__BOTTOM__") {
      ptr = inv.peekBack();
   }
   else {
      ptr = haveObjNamed(i_th, obj, ~0);
   }

   if (!ptr) {
      return -1;
   }

   if ((dest_rm >= 0) && (dest_rm < NUMBER_OF_ROOMS) && room_list[dest_rm].isInUse()) {
      return ptr->doGoToRoom(dest_rm, NULL, NULL, getIdNum(), 1);
   }//if
   return -1;
}//otransport


/** Echo message into the room in this direction */
int room::neighbor_echo(int i_th, const String* dir, const String& buf) {
   door* door_ptr = door::findDoor(DOORS, i_th, dir, ~0, *this);
   if (!door_ptr) {
      return -1;
   }

   room* dest_rm = door_ptr->getDestRoom();

   if (dest_rm) {
      return dest_rm->com_recho(&buf);
   }

   return -1;
}//neighbor_echo


/** Echo message into the room in this direction */
int room::other_room_echo(int dest_rm, const String& buf) {

   if ((dest_rm >= 0) && (dest_rm < NUMBER_OF_ROOMS) && room_list[dest_rm].isInUse()) {
      return room_list[dest_rm].com_recho(&buf);
   }
   return -1;
}//neighbor_echo


int room::rm_pause(int ticks) {
   pause += ticks;

   if (pause < 0)
      pause = 0;

   return 0;
}
