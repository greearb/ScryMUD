// $Id$
// $Revision$  $Author$ $Date$

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
//To contact the maintainer, Edward Roper: edro+scrymud [at] wanfear.net
//

///**********************  command4.cc  ******************************///
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include "command4.h"
#include "command5.h"
#include "olc2.h"   // For normalize_obj
#include "misc.h"
#include "misc2.h"
#include <stdio.h>
#include "classes.h"
#include "battle.h"
#include "spec_prc.h"
#include "spells.h"
#include "skills.h"
#include <PtrArray.h>
#include "load_wld.h"
#include <time.h>
#include <unistd.h>
#include "vehicle.h"
#include "regex.h"


int do_gecho(const char* msg) {
   Cell<critter*> cll(pc_list);
   critter* ptr;

   while ((ptr = cll.next())) {
      show(msg, *ptr);
   }//while
   return 0;
}//do_gecho


int title(const String* tit, critter& pc) {
   if (tit->Strlen() > 79) {
      pc.show(CS_TITLE_80_ONLY);
      return -1;
   }//if
   (pc.short_desc) = *tit;

   switch(pc.short_desc[0]) {
      case '-':
      case ',':
      case ':':
      case '.':
      case '?':
      case '!':
      case '\'':
         break;
      default:
         pc.short_desc.Prepend(" ");
   }

   pc.show(CS_TITLE_CHANGED);
   return 0;
}//title


int poofin(String* pin, critter& pc) {
   if (!ok_to_do_action(NULL, "IF", 0, pc, NULL, NULL, TRUE)) {
      return -1;
   }

   if (pin->Strlen() > 79) {
      show("Your poofin must be less than 80 characters.\n", pc);
      return -1;
   }//if

   for (unsigned int i = 0; i<pin->Strlen(); i++) {
      if (pin->charAt(i) == '\n') {
         pin->setCharAt(i, ' ');
      }//if
   }//for

   pc.setPoofin(*pin);
   show("Poofin changed.\n", pc);
   return 0;
}//poofin


int poofout(String* pin, critter& pc) {
   if (!ok_to_do_action(NULL, "IF", 0, pc, NULL, NULL, TRUE)) {
      return -1;
   }

   if (pin->Strlen() > 79) {
      show("Your poofout must be less than 80 characters.\n", pc);
      return -1;
   }//if
   
   for (unsigned int i = 0; i<pin->Strlen(); i++) {
      if (pin->charAt(i) == '\n') {
         pin->setCharAt(i, ' ');
      }//if
   }//for

   pc.setPoofout(*pin);
   show("Poofout changed.\n", pc);
   return 0;
}//poofout


int oreload(critter& pc) {

   if (!ok_to_do_action(NULL, "IFR", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (update_objects(ROOM.getZoneNum(), TRUE) >= 0) {
      show("Objects in your current zone have been reloaded.\n", pc);
   }
   else {
      pc.show("ERROR:  There was an error, probably out of disk space (VERY BAD).\n");
   }
   return 0;
}//oreload


int mreload(critter& pc) {
   if (!ok_to_do_action(NULL, "IFR", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   if (update_critters(ROOM.getZoneNum(), TRUE) >= 0) {
      show("Critters in your current zone have been reloaded.\n", pc);
   }
   else {
      pc.show("ERROR:  There was an error, probably out of disk space (VERY BAD).\n");
   }
   return 0;
}//mreload


int read(int i_th, const String* post, int j_th, const String* board, critter& pc) {
   String buf(100);

   Cell<object*> cll(*(ROOM.getInv()));
   object* ptr;
   
   if (board->Strlen() == 0) {
      while ((ptr = cll.next())) {
         if (ptr->OBJ_FLAGS.get(74))
            break;
      }//while
   }
   else {
      ptr = ROOM.haveObjNamed(j_th, board, pc.SEE_BIT);
   }

   if (!ptr) {
      return look(i_th, post, pc, TRUE);
      //show("You don't see a bulletin board here.\n", pc);
      //return -1;
   }//if

   object* msg;
   if (post->Strlen() > 0) {
      msg = have_obj_named(ptr->inv, i_th, post, ~0, ROOM);
   }//if
   else { //by number only
      Sprintf(buf, "%i", i_th);
      msg = have_obj_named(ptr->inv, 1, &buf, ~0, ROOM);
   }//else

   if (!msg) {
      show("The board doesn't contain that message.\n", pc);
      return -1;
   }//if

   show(msg->short_desc, pc);
   show("\n\n", pc);
   show(msg->long_desc, pc);
   return 0;
}//read


int siteban(const String* site, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.getImmLevel() < 8) {
      pc.show("Eh??");
      return -1;
   }

   if (site->Strlen() < 2) {
      pc.show("You need to be more specific as to which site to ban.\n");
      return -1;
   }

   banned_hosts.append(new String(*site));

   pc.show("Done.");
   writeSiteBanned();
   return 0;
}//siteban


int unsiteban(const String* site, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   if (pc.getImmLevel() < 8) {
      pc.show("Eh??");
      return -1;
   }

   Cell<String*> cll(banned_hosts);
   String* ptr;
   ptr = cll.next();
   while (ptr) {
      if (*ptr == *site) {
         delete ptr;
         ptr = banned_hosts.lose(cll);
         pc.show("Removed it..");
         writeSiteBanned();
         return 0;
      }
      else {
         ptr = cll.next();
      }
   }

   pc.show("Didn't find that site to delete it...");
   return -1;
}//unsiteban


int list_site_bans(critter& pc) {
   String buf(100);

   pc.show("These sites are banned:\n\n");

   Cell<String*> cll(banned_hosts);
   String* ptr;

   while ((ptr = cll.next())) {
      pc.show("      ");
      pc.show(*ptr);
      pc.show("\n");
   }
   return 0;
}//list_site_bans


int passwd(const String* old, const String* new1, const String* new2,
            critter& pc) {
   if (!pc.pc)
      return -1;

   if (pc.pc->password != crypt((const char*)(*old), "bg")) {
      pc.show("Please enter a correct old password, see help.\n");
      return -1;
   }

   if (*new1 != *new2) {
      pc.show("Your new passwords do not match, please retry.\n");
      return -1;
   }

   if (new1->Strlen() < 3) {
      pc.show("You must have at least 3 characters in your password.\n");
      return -1;
   }

   pc.pc->password = crypt(*new1, "bg");

   pc.show("Password successfully changed.\n");

   pc.save();
   return 0;
}//passwd


int unpost(int i_th, const String* post, critter& pc) {
   String buf(100);
   String tempname;

   if (!ok_to_do_action(NULL, "FPSB", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   Cell<object*> cll(*(ROOM.getInv()));
   object* ptr;

   while ((ptr = cll.next())) {
      if (ptr->OBJ_FLAGS.get(74))
         break;
   }//while

   if (!ptr) {
      show("You don't see a bulletin board here.\n", pc);
      return -1;
   }//if

   object* msg;
   if (post->Strlen() > 0) {
      msg = have_obj_named(ptr->inv, i_th, post, ~0, ROOM);
   }//if
   else { //by number only
      Sprintf(buf, "%i", i_th);
      msg = have_obj_named(ptr->inv, 1, &buf, ~0, ROOM);
   }//else

   if (!msg) {
      show("The board doesn't contain that message.\n", pc);
      return -1;
   }//if

   Cell<object*> mcll(ptr->inv);
   object *iptr = NULL;
   int i;
   for (i=0, iptr = mcll.next(); iptr != NULL; i++, iptr = mcll.next()) {
      if (iptr == msg)
         break;
   }//for
   tempname = String(*(pc.getName()));
   tempname.Strip();
   Sprintf(buf, "message_%i from %S:", i-1, &tempname);
   if (pc.getImmLevel() < 8 && strncmp(msg->short_desc, buf, buf.Strlen())!=0) {
      pc.show("It wouldn't be right to take someone else's message down.\n");
      return -1;
   }

   // NOTE:  Don't ever decrementCurInGame or anything, post objects
   //        are screwy in that they never have a counterpart in the
   //        obj_list.
   ptr->inv.loseData(msg);
   delete msg;

        /* update the numbers and short_descs for asthetic reasons */
   short eos, term_by_period;
   i = 0;
   ptr->inv.head(cll);
   while ((msg = cll.next())) {
      msg->names.clearAndDestroy();
      Sprintf(buf, "%i", i);
      msg->names.append(new String(buf));
      msg->names.append(new String(cstr(CS_MESSAGE, pc)));
      buf = msg->short_desc.Get_Command(eos, term_by_period);
      Sprintf(buf, "Message_%i", i);
      msg->short_desc.Prepend(buf);
      i++;
   }//while

   show("The message has been removed.\n", pc);

   return write_board(*ptr); //save new state
}//unpost


int write_board(object& obj) {
   String buf(100);
   
   Sprintf(buf, "./Boards/board_%i", obj.OBJ_NUM);
   ofstream ifile(buf);
   if (!ifile) {
      mudlog.log(ERROR, "ERROR:  couldn't open board file.\n");
      return -1;
   }//if

   Cell<object*> cll(obj.inv);
   object* ptr;

   while ((ptr = cll.prev())) {
      ifile << ptr->OBJ_NUM << "\t\tMessage number\n";
      ptr->Write(ifile);
      ifile << "\n\n";
   }//while

   ifile << "-1                EOF.\n";
   return 0;
}//write_board


int post(const String* title, critter& pc) {
   if (!ok_to_do_action(NULL, "mFPSG", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!pc.isPc())
      return -1;
   
   object* ptr;
   Cell<object*> cll(*(ROOM.getInv()));
   while ((ptr = cll.next())) {
      if (ptr->OBJ_FLAGS.get(74)) {
         break;

      }//if
   }//while

   if (!ptr) {
      show("There isn't a board here to post upon.\n", pc);
      return -1;
   }//if


   pc.show(CS_ENTER_MSG_POST);
   pc.pc->post_msg = new object;
   pc.pc->post_msg->short_desc = *title;
   pc.setMode(MODE_WRITING_POST); 
   return 0;
}//post


int do_post(critter& pc) {
   String buf = pc.pc->input.Get_Rest();

   if (buf.Strlen() == 0) {
      show("Input a line of your message or a ~ to end it.\n", pc);
      return -1;
   }//if

   if (buf == "~") {
      show("Your message has been posted.\n", pc);

      Cell<object*> cll(*(ROOM.getInv()));
      object* ptr;
      while ((ptr = cll.next())) {
         if (ptr->OBJ_FLAGS.get(74)) {
            break;
         }//if
      }//while
      
      if (!ptr) {
         show("Ack, someone moved the bulletin board.\n", pc);

                        /* clean up mess */
         delete pc.pc->post_msg;
         pc.pc->post_msg = NULL;
      }//if
      else {                /* good to go, save to disk */
         pc.pc->post_msg->in_list = &(ptr->inv);  //make it SOBJ
         pc.pc->post_msg->OBJ_FLAGS.turn_on(7); //!mort
         int i = get_next_msg_num(*ptr);
         pc.pc->post_msg->OBJ_NUM = i;

         String* name = new String(20);
         Sprintf(*name, "%i", i);
         pc.pc->post_msg->names.append(name);
         Sprintf(buf, "message_%i", i);
         name = new String(buf);
         pc.pc->post_msg->names.append(name);

         String tmp_desc((pc.pc->post_msg->short_desc));
            Sprintf((pc.pc->post_msg->short_desc),
                 "message_%i from %S:  ", i, name_of_crit(pc, ~0));
         pc.pc->post_msg->short_desc.Append(tmp_desc);

            Sprintf((pc.pc->post_msg->in_room_desc),
                 "A message from %S lies here.", name_of_crit(pc, ~0));


         Sprintf(buf, "./Boards/board_%i", ptr->OBJ_NUM);
         ifstream ifile(buf);
         if (!ifile) {
            mudlog.log(INF, "CHECK:  creating new board file.\n");
            String buf2(100);
            Sprintf(buf2, "cp ./World/DEFAULT_OBJECTS ./Boards/board_%i",
                     ptr->OBJ_NUM);
            system(buf2); //now it exists
         }//if
         else {
            mudlog.log(INF, "CHECK:  didn't create new board file.\n");
         }

         ofstream da_msg("./Boards/TMP_MSG");
         da_msg << i << "\tmessage number\n";
         pc.pc->post_msg->Write(da_msg);
         da_msg << endl << endl << flush;
            
         Sprintf(buf, "cat ./Boards/board_%i >> ./Boards/TMP_MSG",
                 ptr->OBJ_NUM); //append it
         system(buf);
         Sprintf(buf, "mv -f ./Boards/TMP_MSG ./Boards/board_%i", 
                    ptr->OBJ_NUM); //move it back to right name
         system(buf);

         ptr->inv.append(pc.pc->post_msg);
         pc.pc->post_msg = NULL;
      }//else

      pc.setMode(MODE_NORMAL);
      return 0;
   }//if

   (pc.pc->post_msg->long_desc) += buf;
   (pc.pc->post_msg->long_desc) += "\n";
   return 0;
}//do_post


/* called as 'quit' by the player */
int log_out(critter& pc) {
   if (mudlog.ofLevel(DBG))
      mudlog << "in log_out:  "  << *(name_of_crit(pc, ~0))
             << " getCurRoomNum():  " << pc.getCurRoomNum()
             << "PC's addr:  " << &pc << endl;

   if (!pc.pc)
      return -1;

   if (!pc.IS_FIGHTING.isEmpty()) {
      pc.show("You must stop fighting first.\n", HL_DEF);
      return -1;
   }//if

   // if there's a violence timer...
   if (config.useViolenceTimer) {
      if (pc.isViolent()) {
         pc.show("You must wait for your violence timer to expire.\n", HL_DEF);
         return -1;
      }
   }

   pc.save();

   //If they're link-dead we logout a bit differently. I'm not sure if this is
   //just for "accounting" reasons, or if maybe there is some other reason.
   if ( pc.getMode() == MODE_GO_LINKDEAD_PLEASE ) {
      pc.setMode(MODE_LOG_OFF_LINKDEAD_PLEASE);
   } else {//normal quit or idle timeout
      pc.setMode(MODE_QUIT_ME_PLEASE);
   }

   show("Cya later.\n", pc);
   emote("has left the game.\n", pc, ROOM, TRUE);
   return 0;
}//log_out


int go(int i_th, const String* direction, critter& pc, int& is_dead) {
   if (pc.isMob()) { //if MOB
      return -1;
   }//if
   return move(pc, i_th, *direction, TRUE, ROOM, is_dead);
}//go


int snoop(int i_th, const String* vict, critter& pc) {

   if (!ok_to_do_action(NULL, "IFPG", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.getImmLevel() < 8) {
      show("Eh??", pc);
      return -1;
   }//if

   critter* ptr = have_crit_named(pc_list, i_th, vict, pc.SEE_BIT, ROOM);

   if (!ptr) {
      pc.show("That person cannot be found among the active players.\n");
      return -1;
   }
   else if (ptr->SNOOPED_BY) {
      pc.show("You can't snoop that person now.\n");
      return -1;
   }
   else if (!ptr->pc) {
      pc.show("You can only snoop PC's.\n");
      return -1;
   }
   else if (&pc == ptr) {
      pc.show("You can't snoop yourself!\n");
      return -1;
   }
   else {
      pc.SNOOPING = ptr;
      ptr->SNOOPED_BY = &pc;
      String buf(100);
      Sprintf(buf, "You are now snooping %S.\n", ptr->getName());
      pc.show(buf);

      if (mudlog.ofLevel(WRN)) {
         mudlog << "SNOOP:  " << *(pc.getName()) << " started snooping:  "
                << *(ptr->getName()) << endl;
      }
   }
   return 0;
}//snoop

int unsnoop(critter& pc) {
   if (pc.pc && pc.SNOOPING) {

      if (mudlog.ofLevel(WRN)) {
         mudlog << "SNOOP:  " << *(pc.getName()) << " stopped snooping:  "
                << *(pc.SNOOPING->getName()) << endl;
      }

      pc.SNOOPING->SNOOPED_BY = NULL;
      pc.SNOOPING = NULL;
      pc.show("You have stopped snooping.\n");

   }//if
   return 0;
}//unsnoop


int possess(int i_th, const String* vict, critter& pc) {

   if (!ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (pc.getImmLevel() < 5) {
      show("Eh??", pc);
      return -1;
   }//if

   critter* ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

   if (!ptr) {
      pc.show("That person is not here.\n");
      return -1;
   }
   else if (ptr->possessed_by) {
      pc.show("You can't possess that person now.\n");
      return -1;
   }
   else if (ptr->isPc()) {
      pc.show("You can't possess PC's at this time.\n");
      return -1;
   }
   else {
      if (ptr->isMob()) {
         ptr = mob_to_smob(*ptr,  pc.getCurRoomNum(), TRUE, i_th,
                           vict, pc.SEE_BIT);
      }//if

      pc.possessing = ptr;
      ptr->possessed_by = &pc;
      String buf(100);
      Sprintf(buf, "You are now possessing %S.\n", ptr->getName());
      pc.show(buf);
   }
   return 0;
}//possess


int _goto(int i_th, const String* name, critter& pc) {
   int rm_num = -1;

   if (pc.isMob() && !ok_to_do_action(NULL, "BF", 0, pc, pc.getCurRoom(),
            NULL, TRUE) ) {
      return -1;
   } else if (!ok_to_do_action(NULL, "IBF", 0, pc, pc.getCurRoom(),
            NULL, TRUE)) {
      return -1;
   }

   if (name->Strlen() == 0) { //called like goto <rm_number>
      //mudlog << "_goto:  name has zero length." << endl;
      if (!check_l_range(i_th, 0, NUMBER_OF_ROOMS, pc, TRUE))
         return -1;
   }//if
   else { 
      critter* crit = have_crit_named(pc_list, i_th, name,
                                      pc.SEE_BIT, ROOM);
      if (!crit) {
         crit = have_crit_named(linkdead_list, i_th, name,
                                pc.SEE_BIT, ROOM);
      }//if

      if (crit) {
         rm_num = crit->getCurRoomNum();
      }//if
      else {
         crit = room::haveCritNamedInWorld(i_th, name, pc.SEE_BIT, rm_num);
      }

      if (!crit) {
         show("That person is not to be found.\n", pc);
         return -1;
      }//if
      i_th = rm_num;
   }//else      

   if (!check_l_range(i_th, 0, NUMBER_OF_ROOMS - 1, pc, FALSE)) {
      show("Got an out of range room_num.  Check to see if the\n", pc);
      show("mob you tried to go to is constructed correctly.\n", pc);
      return -1;
   }//if

   int znum = room_list[i_th].getZoneNum();
   
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In _goto, znum: " << znum << endl;
   }

   if (ZoneCollection::instance().elementAt(znum).isLocked()) {
      if (!ZoneCollection::instance().elementAt(znum).isOwnedBy(pc)
          && !(znum == 0)) {
         if (pc.getImmLevel() <= 5) {
            pc.show("Sorry, that zone is locked and you do not own it.\n");
            pc.show("If you have a legit reason to visit it, talk to the\n");
            pc.show("head builder or coder for special permission.\n");
            return -1;
         }//if
      }//if
   }//if

   String buf(100);

   if (pc.shouldDoPoofout()) {
      ROOM.doPoofOut(pc);
   }
   else {
      emote("disappears with a popping sound!\n", pc, ROOM, FALSE);
   }

   //mudlog << "WARNING: about to doGoToRoom: " << i_th << endl;

   int is_dead;
   pc.doGoToRoom(i_th, NULL, NULL, is_dead, pc.getCurRoomNum(), 1);

   if (pc.shouldDoPoofin()) {
      ROOM.doPoofIn(pc);
   }
   else {
      emote("appears suddenly.\n", pc, ROOM, FALSE);
   }
   return 0;
}//_goto room_number


int where(int i_th, const String* name, critter& pc) {
   Cell<critter*> cll;
   critter* ptr = NULL;
   String buf(100);

   int zn = ROOM.getZoneNum();

   if (!ok_to_do_action(NULL, "Fr", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (name->Strlen() == 0) { //list all in zone
      if (!pc.isImmort()) {
         pc_list.head(cll);
         while ((ptr = cll.next())) {
            if (zn == ptr->getCurZoneNum()) {
               int rmtModPC = pc.isRemort() ? 30 : 0;
               int rmtModPtr = ptr->isRemort() ? 30 : 0;
               if (((pc.LEVEL + rmtModPC + 3 > ptr->LEVEL + rmtModPtr)) &&
                  detect(pc.SEE_BIT, ptr->VIS_BIT |
                     room_list[ptr->getCurRoomNum()].getVisBit())) {
                  Sprintf(buf, "%S%P20%S\n", name_of_crit(*ptr, pc.SEE_BIT),
                          &(room_list[ptr->getCurRoomNum()].short_desc));
                  show(buf, pc);
               }//if
            }//if
         }//while
      }//if
      else {
         int start = ZoneCollection::instance().elementAt(pc.getCurZoneNum()).getBeginRoomNum();
         int end = ZoneCollection::instance().elementAt(pc.getCurZoneNum()).getEndRoomNum();
         for (int i = start; i<= end; i++) {
            if (room_list.elementAtNoCreate(i)) {
               Cell<critter*> ccll(room_list[i].getCrits());
               while ((ptr = ccll.next())) {
                  if (!ptr->isPc()) {
                     Sprintf(buf, "[Room %i] %P11[%i]%P20%S.\n",
                             i, ptr->getIdNum(), ptr->getName());
                     pc.show(buf);
                  }
                  else {
                     if (!ptr->isImmort() ||  (ptr->isImmort() && (ptr->getImmLevel() <= pc.getImmLevel()))) {
                        Sprintf(buf, "[Room %i] %P11[PC] ***%S***.\n", i, ptr->getName());
                        pc.show(buf);
                     }
                  }
               }//while
            }//if
         }//for
      }//else
   }//if
   else {
      if (pc.isImmort()) {
         if (strcasecmp(*name, "pcs") == 0) {
            Cell<critter*> ccll(pc_list);
            critter* ptr;
            while ((ptr = ccll.next())) {
               if (!ptr->isImmort() ||  (ptr->isImmort() && (ptr->getImmLevel() <= pc.getImmLevel()))) {
                  Sprintf(buf, "[Room %i] %P11[PC]%P20%S.\n",
                          ptr->getCurRoomNum(), ptr->getName());
                  pc.show(buf);
               }
            }//while
            return 0;
         }//if
         else if (strcasecmp(*name, "objs") == 0) {
            int start = ZoneCollection::instance().elementAt(pc.getCurZoneNum()).getBeginRoomNum();
            int end = ZoneCollection::instance().elementAt(pc.getCurZoneNum()).getEndRoomNum();
            for (int i = start; i<= end; i++) {
               if (room_list.elementAtNoCreate(i)) {
                  Cell<object*> ocll(*(room_list[i].getInv()));
                  object* optr;
                  while ((optr = ocll.next())) {
                     Sprintf(buf, "[Room %i] %P11[%i]%P20%S.\n",
                             i, optr->getIdNum(), optr->getLongName());
                     pc.show(buf);
                  }//while
               }//if
            }//for
            return 0;
         }//if
      }//if immort

      ptr = have_crit_named(pc_list, i_th, name, pc.SEE_BIT, ROOM);
      if (ptr) {
         if (pc.isImmort()) {
            if (!ptr->isImmort() || (ptr->getImmLevel() <= pc.getImmLevel())) {
               Sprintf(buf, "%S%P20[%i]%S\n", ptr->getName(pc),
                       ptr->getCurRoomNum(), &(room_list[ptr->getCurRoomNum()].short_desc));
               show(buf, pc);
            }
         }
         else if ((pc.LEVEL + 3 > ptr->LEVEL) &&
                  (ROOM.getZoneNum() == 
                   room_list[ptr->getCurRoomNum()].getZoneNum()) &&
                  detect(pc.SEE_BIT, ptr->VIS_BIT | 
                         room_list[ptr->getCurRoomNum()].getVisBit())) {
            Sprintf(buf, "%S%P20%S\n", ptr->getName(pc.SEE_BIT),
                    &(room_list[ptr->getCurRoomNum()].short_desc));
            show(buf, pc);
         }//if
      }//if
      else {
         show("That person isn't logged on as far as you can tell.\n", pc);
      }//else
   }//else
   return 0;
}//where


int roll(int i, int j, critter& pc) {
   String buf(100);
   if (i > 100) {
      pc.show("Can only roll your dice a max of 100 times!!\n");
      i = 100;
   }

   Sprintf(buf, "Your roll of %i dice of %i side(s) is:  %i\n",
           i, j, d(i, j));
   pc.show(buf);
   return 0;
}


int uptime(critter& pc) {
   if (!ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   int ut = (time(NULL) - BOOT_TIME);
   int seconds = ut % 60;
   int mins = (ut % (60 * 60) / 60);
   int hours = (ut % (60 * 60 * 24) / (60 * 60));
   int days = (ut / (60 * 60 * 24));

   String buf(100);
   Sprintf(buf, "ScryMUD has been up for %i seconds, or:\n", ut);
   pc.show(buf);
   Sprintf(buf, "%i days, %i hrs, %i mins and %i secs\n",
           days, hours, mins, seconds);
   pc.show(buf);
   return 0;
}//uptime


int zgoto(int i_th, critter& pc) {
   int rm_num;

   if (!ok_to_do_action(NULL, "BIF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(i_th, 0, NUMBER_OF_ZONES, pc, TRUE)) {
      return -1;
   }//if

   if (!ZoneCollection::instance().elementAt(i_th).isInUse()) {
      show("That zone has not been defined yet.\n", pc);
      return -1;
   }//if

   int i = ZoneCollection::instance().elementAt(i_th).getBeginRoomNum();
   int ndroom = ZoneCollection::instance().elementAt(i_th).getEndRoomNum();
   rm_num = i;
   for (; i< ndroom; i++) {
      if (room_list[i].isInUse()) {
         rm_num = i;
         break;
      }//if
   }//for

   String buf(100);

   if (pc.shouldDoPoofout()) {
      ROOM.doPoofOut(pc);
   }
   else {
      emote("disappears with a popping sound!\n", pc, ROOM, FALSE);
   }

   int is_dead;
   pc.doGoToRoom(rm_num, NULL, NULL, is_dead, pc.getCurRoomNum(), 1);

   if (pc.shouldDoPoofin()) {
      ROOM.doPoofIn(pc);
   }
   else {
      emote("appears suddenly.\n", pc, ROOM, FALSE);
   }
   return 0;
}//goto zone



int rclear(int i_th, critter& pc) {
   int rm_num;
   String buf(100);

   mudlog.log(TRC, "In rclear.\n");

   if (!ok_to_do_action(NULL, "BIF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (i_th == 1) { //do current room
      rm_num = pc.getCurRoomNum();
   }//if
   else {
      if (!check_l_range(i_th, 0, NUMBER_OF_ROOMS, pc, TRUE)) {
         return -1;
      }//if
      rm_num = i_th;
   }//else
      
   if (!pc.doesOwnRoom(room_list[rm_num])) {
      show("You don't have permission to edit this room.\n", pc);
      return -1;
   }//if

   int new_rm_num;
   if (pc.getCurRoomNum() != config.loginRoom) {
      new_rm_num = config.loginRoom;
   }//if
   else {
      new_rm_num = config.loginRoom + 1;
   }//else

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Rclear, new_rm_num:  " << new_rm_num 
             << " rm_num:  " << rm_num << "  pc:  " << *(pc.getName())
             << " i_th:  " << i_th << endl;
   }

   room_list[rm_num].doRclear(new_rm_num);
   room_list.setAndDestroy(NULL, rm_num);

   Sprintf(buf, "Cleared room# %i\n", rm_num);
   pc.show(buf);

   return 0;
}// rclear


int oclear(int i_th, critter& pc) {

   mudlog.log(TRC, "In oclear.\n");

   if (!ok_to_do_action(NULL, "BIF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (!check_l_range(i_th, 2, NUMBER_OF_ITEMS, pc, TRUE)) {
      return -1;
   }//if
      
   if (!pc.doesOwnObject(obj_list[i_th])) {
      show("You don't have permission to edit this object.\n", pc);
      return -1;
   }//if

   show("Ok, the object was deleted from the online DB.  In order to make\n",
        pc);
   show("these changes permanent, you will have to do an 'aosave'.\n", pc);

   Cell<critter*> ccll;
   critter* cptr;
   for (int i = 0; i<NUMBER_OF_ROOMS; i++) {
      if (room_list.elementAtNoCreate(i)) {
         room_list[i].loseObjectFromGame(obj_list[i_th]);
      }//if
   }//for

   pc_list.head(ccll);
   while ((cptr = ccll.next())) {
      cptr->loseObjectFromGame(&(obj_list[i_th]));
   }//while

   obj_list[i_th].Clear();  //clear out the object
   return 0;
}// oclear


int mclear(int i_th, critter& pc) {

   mudlog.log(TRC, "In mclear.\n");

   if (!ok_to_do_action(NULL, "BIF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(i_th, 2, NUMBER_OF_MOBS, pc, TRUE)) {
      return -1;
   }//if
      
   if (!pc.doesOwnCritter(mob_list[i_th])) {
      show("You don't have permission to edit this mob.\n", pc);
      return -1;
   }//if

   show("Ok, the mob was deleted from the online DB.  In order to make\n",pc);
   show("these changes permanent, you will have to do an 'amsave'.\n", pc);

   for (int i = 0; i < NUMBER_OF_ROOMS; i++) {
      room_list[i].purgeCritter(i_th, pc);
   }//for

   mob_list[i_th].Clear();  //clear out the mob

   return 0;
}// mclear


int dclear(int i_th, critter& pc) {

   mudlog.log(TRC, "In dclear.\n");

   if (!ok_to_do_action(NULL, "BIF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(i_th, 2, NUMBER_OF_DOORS, pc, TRUE)) {
      return -1;
   }//if
      
   if (!pc.doesOwnDoor(door_list[i_th])) {
      show("You don't have permission to edit this door.\n", pc);
      return -1;
   }//if

   show("Ok, the door was deleted from the online DB.  In order to make\n",
        pc);
   show("these changes permanent, you will have to do an 'adsave'.\n", pc);

   door_list[i_th].Clear();  //clear out the mob

   Cell<door*> cll;
   door* ptr;
   for (int i = 0; i<NUMBER_OF_ROOMS; i++) {
      if (room_list.elementAtNoCreate(i)) {
         room_list[i].DOORS.head(cll);
         ptr = cll.next();
         while (ptr) {
            if (ptr->dr_data == &door_list[i_th])  {
               delete ptr;
               ptr = room_list[i].DOORS.lose(cll);
            }//if
            else
               ptr = cll.next();
         }//while
      }//if
   }//for
   return 0;
}// dclear


int brief(critter& pc) {
   String buf("brief");
   return toggle_prompt(&buf, pc);
}//brief


/* helper function, not a command */
int num_of_generic_door(const String* direction) {
   String* tmp_name;
   for (int i = 1; i <= 10; i++) {
      tmp_name = door_list[i].names.peekFront();
      if (strcasecmp(*direction, *tmp_name) == 0)
         return i;
   }//for
   return -1;
}//num_of_generic_door


int flip_door(int i_th, const String* name, critter& pc) {

   if (!ok_to_do_action(NULL, "BIFRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   door* dr_ptr = door::findDoor(ROOM.DOORS, i_th, name, pc.SEE_BIT, ROOM);
   if (!dr_ptr) {
      show("That door doesn't seem to exist.\n", pc);
      return -1;
   }//if

   if (!dr_ptr->dr_data->door_data_flags.get(9)) {
      show("That door is flagged !FLIP.\n", pc);
      return -1;
   }//if

   dr_ptr->destination = (-(dr_ptr->destination));
   show("The door has been flipped.\n", pc);
   return 0;
}//flip_door


int door_to(int room_num, int dist, const String* direction, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "BIFZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (mudlog.ofLevel(DBG)) {
      String buf(100);
      Sprintf(buf, "DEBUG:  door_to:  Room_num: %i dist: %i dir -:%S:-\n",
              room_num, dist, direction);
      pc.show(buf);
   }

   if (dist == 1) {
      show("NOTE:  door_to has been changed to include distance.\n", pc);
      show("READ THE HELP IF YOU HAVEN'T ALREADY.\n", pc);
   }//if one, odds are its a default value, ie not on purpose

   if (!check_l_range(dist, 0, 25, pc, FALSE)) {
      show("Distance is out of range, must be between 0 and 25.\n", pc);
      return -1;
   }//if

   if (room_num <= 1) {
      Sprintf(buf, "You cannot build a door to that room ( room# <= 1): %i\n",
              room_num);
      pc.show(buf);
      return -1;
   }//if

   if (room_num > NUMBER_OF_ROOMS) {
      show("Your stated room number is too high.\n", pc);
      return -1;
   }//if

   String op_dir(get_opposite_dir(*direction));
   do_door_to(ROOM, room_list[room_num], dist, pc, direction);
   do_door_to(room_list[room_num], ROOM, dist, pc, &op_dir);
   return 0;
}//door_to


int do_door_to(room& cur_room, room& targ_room, int distance, critter&
                pc, const String* direction, int is_gate) {
   String buf(100);
   int i;

   if (!is_gate) {
      if (!ok_to_do_action(NULL, "BZR", 0, pc, &cur_room, NULL, TRUE)) {
         return -1;
      }

      i = num_of_generic_door(direction);
   
      if (i < 0) {
         show("In what direction should your door to exit?\n", pc);
         return -1;
      }//if
   }//if not a gate/portal
   else {
      i = is_gate;
   }//else

   door* new_door = new door;
   new_door->dr_data = &(door_list[i]);
   new_door->destination = targ_room.getRoomNum();
   new_door->distance = distance;
   new_door->in_room = cur_room.getRoomNum();
   if (is_gate) {
      new_door->ticks_till_disolve = pc.LEVEL/8 + 1;
      obj_ptr_log << "NOTE " << cur_room.getIdNum() << " " << &cur_room
                  << "Adding to affected_rooms bcause of gate." << endl;
      affected_rooms.gainData(&cur_room);
   }//if
   cur_room.doors.append(new_door);
   if (!is_gate)
     show("OK, doors added (its reflexive now)\n", pc);
   return 0;
}//do_door_to


int replace_door(int nd_num, const String* direction, critter& pc) {

   if (!ok_to_do_action(NULL, "BIFRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (!check_l_range(nd_num, 0, NUMBER_OF_DOORS, pc, TRUE))
      return -1;

   if (door_list[nd_num].names.isEmpty()) {
      show("That door doesn't exist yet.\n", pc);
      return -1;
   }//if
   
   door* dr_ptr = door::findDoor(ROOM.DOORS, 1, direction, ~0, ROOM);
   if (!dr_ptr) {
      show("That door doesn't seem to exist.\n", pc);
      return -1;
   }//if

   dr_ptr->dr_data = &(door_list[nd_num]); //essentially add new door
   return 0;
}//replace_door


int delete_door(int i_th, const String* direction, critter& pc) {
   if (!ok_to_do_action(NULL, "BIFRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   door* dr_ptr = 
         door::findDoor(ROOM.DOORS, i_th, direction, pc.SEE_BIT, ROOM);
   if (!dr_ptr) {
      show("That door doesn't seem to exist.\n", pc);
      return -1;
   }//if

   ROOM.DOORS.loseData(dr_ptr);
   delete dr_ptr; //only a shallow delete

   show("Ok, door has been removed.\n", pc);
   return 0;
}//delete_door


int write_zone(int znum, critter& pc) {

   String buf(100);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In write zone, znum:  " << znum << endl;
   }

   if (!ok_to_do_action(NULL, "BIF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (znum == 1) {
     znum = ROOM.getZoneNum();
   }//if

   if (!check_l_range(znum, 0, NUMBER_OF_ZONES, pc, TRUE)) {
      return -1;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).isLocked()) { //locked from mortals, mob_procs
      show("WARNING:  You cannot write a zone that is not locked.\n", pc);
      return -1;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).isTotallyLoaded()) {
      show("WARNING:  You cannot write a zone that isn't totally_loaded.\n", 
            pc);
      return -1;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).isOwnedBy(pc)) {
      show("You don't have permission to edit this zone.\n", pc);
      return -1;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).canWriteRooms()) { //if locked (no write, checked out)
      show("WARNING:  All builders of your zone must finish their projects\n",
           pc);
      show("before you can write it to disk.\n", pc);
      return -1;
   }//if

   Sprintf(buf, "Your zone# %i is being written.\n", znum);
   pc.show(buf);

   return do_write_zone(znum);
}//write_zone

int do_write_zone(int znum) {
   switch (config.useMySQL) {
#ifdef USEMYSQL
      case true:
         return do_db_write_zone(znum);
      break;
#endif
      case false:
         return do_file_write_zone(znum);
      break;
   }
   return -1;
}

#ifdef USEMYSQL
int do_db_write_zone(int znum) {
   int j;
   for (j = ZoneCollection::instance().elementAt(znum).getBeginRoomNum();
        j <= ZoneCollection::instance().elementAt(znum).getEndRoomNum();
        j++) {
      if (!room_list[j].names.isEmpty()) {
         room_list[j].dbWrite();
      }
   }
   return 0;
}
#endif

int do_file_write_zone(int znum) {
   String buf(100);
   int j;
   
   Sprintf(buf, "./World/zone_%i", znum);
   ofstream rfile(buf);
   if (!rfile) {
      mudlog.log(ERROR, "ERROR:  rfile not opened in 'write_zone'.\n");
      return -1;
   }//if
   
   for (j = ZoneCollection::instance().elementAt(znum).getBeginRoomNum();
        j <= ZoneCollection::instance().elementAt(znum).getEndRoomNum();
        j++) {
      if (!room_list[j].names.isEmpty()) {
         if (room_list[j].isVehicle()) {
            rfile << (j | 0x01000000) << "\t\tVehicle number\n";
         }
         else {
            rfile << j << "\t\tRoom Number\n";
         }
         room_list[j].fileWrite(rfile);
      }//if
   }//for
   rfile << "\n\n" << -1 << "\t\tEND OF FILE MARKER\n" << flush;
   return 0;
}//do_write_zone


int zlock(int znum, critter& pc) {

   if (!ok_to_do_action(NULL, "BIF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (znum == 1) {
      znum = ROOM.getZoneNum();
   }//if

   if (!check_l_range(znum, 0, NUMBER_OF_ZONES, pc, TRUE)) {
      return -1;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).isOwnedBy(pc)) {
      show("You don't have permission to edit this zone.\n", pc);
      return -1;
   }//if

   ZoneCollection::instance().zlock(znum);
   show("Your present zone is now locked.\n", pc);
   return 0;
}//zlock



int zunlock(int znum, critter& pc) {
   if (!ok_to_do_action(NULL, "BIF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   if (znum == 1) { //default is zone pc is in
     znum = ROOM.getZoneNum(); 
   }//if

   if (!check_l_range(znum, 0, NUMBER_OF_ZONES, pc, TRUE)) {
      return -1;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).isOwnedBy(pc)) {
      show("You don't have permission to edit this zone.\n", pc);
      return -1;
   }//if

   ZoneCollection::instance().zunlock(znum);
   show("Your zone is now unlocked.\n", pc);
   return 0;
}//zunlock


/* helper function */
int do_lock_room(room& rm) {
   rm.lock();
   return 0;
}//do_lock_room


/* helper function */
int do_unlock_room(room& rm) {
   rm.unlock();
   return 0;
}//do_lock_room


/* set the players prompt */
int prompt(const String* pr, critter& pc) {
   if (!ok_to_do_action(NULL, "mFG", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!pc.isPc()) {
      pc.show("Only PCs can set prompts...\n");
      return -1;
   }
   
   if (*pr == "NORMAL") //reset to default
      pc.pc->prompt = "%N< %hH %mM %vV >  ";
   else
      pc.pc->prompt = *pr;
   show("Ok.\n", pc);
   return -1;
}//prompt


int give(int i_th, const String* obj, int j_th, const String* target,
         critter& pc) {
   String buf(100);
   critter* targ;
   object* item;

   if (!ok_to_do_action(NULL, "mrFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }
   
   item = have_obj_named(pc.inv, i_th, obj, pc.SEE_BIT, ROOM);
   if (!item) {
      show("You don't have that object.\n", pc);
      return -1;
   }//if

   targ = ROOM.haveCritNamed(j_th, target, pc.SEE_BIT);
   if (!targ) {
      pc.show("To whom shall you give it?\n");
      return -1;
   }//if

   if (targ == &pc) {
      show("Nothing much seems to change!\n", pc);
      return -1;
   }//if

   if (targ->isMob())
      targ = mob_to_smob(*targ, pc.getCurRoomNum(), TRUE, j_th, target, pc.SEE_BIT);

                   /* assume have valid item and targ */

   return do_give(*targ, pc, *item);

}//give()


int do_give(critter& targ, critter& pc, object& obj) {
   String buf(100);

   if (!source_give_to(pc, obj, targ)) {
      return -1;
   }//if

   if (!obj_drop_by(obj, pc)) {
      return -1;
   }//if

   if (obj.IN_LIST)
      obj.IN_LIST = &(targ.inv);

   targ.gainInv(&obj);

   /* this may fail silently with some mob procs, however, it should be
      harmless. */
   pc.loseInv(&obj);

   Cell<critter*> cll(ROOM.getCrits());
   critter* crit_ptr;
   while ((crit_ptr = cll.next())) {
      if ((crit_ptr != &targ) && (crit_ptr != &pc)) {
         Sprintf(buf, "%S gives %S to %S.\n",
                 name_of_crit(pc, crit_ptr->SEE_BIT),
                 long_name_of_obj(obj, crit_ptr->SEE_BIT),
                 name_of_crit(targ, crit_ptr->SEE_BIT));
         buf.Cap();
         show(buf, *crit_ptr);
      }//if
   }//while

   Sprintf(buf, "You give %S to %S.\n", &(obj.short_desc),
        name_of_crit(targ, pc.SEE_BIT));
   show(buf, pc);

   Sprintf(buf, "%S gives you %S.\n", name_of_crit(pc, targ.SEE_BIT),
           long_name_of_obj(obj, targ.SEE_BIT));
   buf.Cap();
   show(buf, targ);

   if (targ.isPlayerShopKeeper()) {
      if (targ.mob->proc_data->sh_data->getPsdFor(obj)==NULL) {
         say("I have no use for that.", targ, ROOM);
         targ.loseInv(&obj);
         ROOM.gainInv(&obj);
         drop_eq_effects(obj, targ, TRUE);
      }
   }

   int deleted_obj = FALSE;

   drop_eq_effects(obj, pc, FALSE); //don't do msgs
   gain_eq_effects(obj, NULL, targ, -1, FALSE, deleted_obj); //don't do msgs

   if (!deleted_obj) {
      do_domob_give_proc(targ, pc, obj); //this can recurse but twice I believe
      
      String cmd = "give";
      String obj_num;
      obj_num = obj.OBJ_NUM;
      ROOM.checkForProc(cmd, obj_num, pc, targ.MOB_NUM);
   }

   return 0;
}//do_give()


int olist(int start, int end, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (end == 1) {
      if (start + 20 >= NUMBER_OF_ITEMS) {
         end = (NUMBER_OF_ITEMS - 1);
      }//if
      else {
         end = start + 20;
      }//else
   }//if

   if (!check_l_range(start, 0, NUMBER_OF_ITEMS, pc, TRUE) ||
       !check_l_range(end, 0, NUMBER_OF_ITEMS, pc, TRUE)) {
      return -1;
   }//if

   for (int i = start; i<= end; i++) {
      if (obj_list[i].OBJ_FLAGS.get(10)) {
         Sprintf(buf, "\t%i\t%S\n", i, &(obj_list[i].short_desc));
         show(buf, pc);
      }//if
      else {
         Sprintf(buf, "\t%i\tTHIS OBJECT UNDEFINED.\n", i);
         show(buf, pc);
      }//else
   }//for 
   return 0;
}//olist

int osdsearch(String *match_str, critter& pc) {
   String buf(100);
   regex my_regex;

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (! (my_regex = *match_str) ) {
      pc.show(my_regex.getErr());
      return -1;
   }

   for (int i=0;i<NUMBER_OF_ITEMS;i++) {
      if ( obj_list[i].isInUse() ) {
         if ( my_regex == obj_list[i].short_desc ) {
            Sprintf(buf, "\t%i\t%S\n", i, &(obj_list[i].short_desc));
            pc.show(buf);
         }//if matches
      }//if in use
   }//for

   return 0;
}//osdsearch

int oldsearch(String *match_str, critter& pc) {
   String buf(100);
   regex my_regex;

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (! (my_regex = *match_str) ) {
      pc.show(my_regex.getErr());
      return -1;
   }

   for (int i=0;i<NUMBER_OF_ITEMS;i++) {
      if ( obj_list[i].isInUse() ) {
         if ( my_regex == obj_list[i].long_desc ) {
            Sprintf(buf, "\t%i\t%S\n", i, &(obj_list[i].short_desc));
            pc.show(buf);
         }//if matches
      }//if in use
   }//for

   return 0;
}//oldsearch

int zlist(int start, int end, critter& pc) {
   if (!ok_to_do_action(NULL, "IF", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   // Change default to zero.
   if (start == 1) {
      start = 0;
   }

   if (end == 1) {
      if (start + 20 >= NUMBER_OF_ZONES) {
         end = (NUMBER_OF_ZONES - 1);
      }//if
      else {
         end = start + 20;
      }//else
   }//if

   if (!check_l_range(start, 0, NUMBER_OF_ZONES, pc, TRUE) ||
       !check_l_range(end, 0, NUMBER_OF_ZONES, pc, TRUE)) {
      return -1;
   }//if

   ZoneCollection::instance().zlist(pc, start, end);
   return 0;
}//zlist

int msdsearch(String *match_str, critter& pc) {
   String buf(100);
   regex my_regex;

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (! (my_regex = *match_str) ) {
      pc.show(my_regex.getErr());
      return -1;
   }

   for (int i=0;i<=NUMBER_OF_MOBS;i++) {
      if (mob_list[i].isInUse()) {
         if ( my_regex == mob_list[i].short_desc ) {
         Sprintf(buf, "\t%i\t%S\n", i, &(mob_list[i].short_desc));
         show(buf, pc);
         }//if matches
      }//if in use
   }//for
   return 0;
}//msdsearch

int mldsearch(String *match_str, critter& pc) {
   String buf(100);
   regex my_regex;

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (! (my_regex = *match_str) ) {
      pc.show(my_regex.getErr());
      return -1;
   }

   for (int i=0;i<=NUMBER_OF_MOBS;i++) {
      if (mob_list[i].isInUse()) {
         if ( my_regex == mob_list[i].long_desc ) {
         Sprintf(buf, "\t%i\t%S\n", i, &(mob_list[i].short_desc));
         show(buf, pc);
         }//if matches
      }//if in use
   }//for
   return 0;
}//mldsearch

int mlist(int start, int end, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (end == 1) {
      if (start + 20 > NUMBER_OF_MOBS) {
         end = NUMBER_OF_MOBS;
      }//if
      else {
         end = start + 20;
      }//else
   }//if

   if (!check_l_range(start, 0, NUMBER_OF_MOBS, pc, TRUE) ||
       !check_l_range(end, 0, NUMBER_OF_MOBS, pc, TRUE)) {
      return -1;
   }//if

   for (int i = start; i<= end; i++) {
      if (mob_list[i].CRIT_FLAGS.get(18)) {
         Sprintf(buf, "\t%i\t%S\n", i, &(mob_list[i].short_desc));
         show(buf, pc);
      }//if
      else {
         Sprintf(buf, "\t%i\tTHIS MOB UNDEFINED.\n", i);
         show(buf, pc);
      }//else
   }//for
   return 0;
}//mlist

int rlist(int start, int end, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (end == 1) {
      if (start + 20 > NUMBER_OF_ROOMS) {
         end = NUMBER_OF_ROOMS;
      }//if
      else {
         end = start + 20;
      }//else
   }//if

   if (!check_l_range(start, 0, NUMBER_OF_ROOMS, pc, TRUE) ||
       !check_l_range(end, 0, NUMBER_OF_ROOMS, pc, TRUE)) {
      return -1;
   }//if

   for (int i = start; i<= end; i++) {
      if (!room_list[i].names.isEmpty()) {
         Sprintf(buf, "\t%i\t%S\n", i, &(room_list[i].short_desc));
         show(buf, pc);
      }//if
      else {
         Sprintf(buf, "\t%i\tTHIS ROOM UNDEFINED.\n", i);
         show(buf, pc);
      }//else
   }//for
   return 0;
}//rlist


int dlist(int start, int end, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (end == 1) {
      if (start + 20 > NUMBER_OF_DOORS) {
         end = NUMBER_OF_DOORS;
      }//if
      else {
         end = start + 20;
      }//else
   }//if

   if (!check_l_range(start, 0, NUMBER_OF_DOORS, pc, TRUE) ||
       !check_l_range(end, 0, NUMBER_OF_DOORS, pc, TRUE)) {
      return -1;
   }//if

   for (int i = start; i<= end; i++) {
      if (!door_list[i].names.isEmpty()) {
         Sprintf(buf, "\t%i\t%S\n", i, &(door_list[i].long_desc));
         show(buf, pc);
      }//if
      else {
         Sprintf(buf, "\t%i\tTHIS DOOR UNDEFINED.\n", i);
         show(buf, pc);
      }//else
   }//for
   return 0;
}//dlist


int adlist(int znum, critter& pc) {
   String buf(100);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   int zone_num;

   if (znum == 1) {
      zone_num = ROOM.getZoneNum();
   }
   else {
      zone_num = znum;
   }

   if (zone_num < 0)
      zone_num = 0;
   if (zone_num >= NUMBER_OF_ZONES) 
      zone_num = NUMBER_OF_ZONES - 1;

   Sprintf(buf, "Here are all the doors for zone: %i\n\n", zone_num);
   pc.show(buf);

   for (int i = 0; i<NUMBER_OF_DOORS; i++) {
      if (door_list[i].isInUse() && door_list[i].isInZone(zone_num)) {
         Sprintf(buf, "       %i%P14%S\n", i, &(door_list[i].long_desc));
         show(buf, pc);
      }//if
   }//for
   return 0;
}//adlist


int tog_rflag(int flagnum, critter& pc) {
   String buf(50);

   if (!ok_to_do_action(NULL, "IFPRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(flagnum, 0, 200, pc, TRUE)) {
      return -1;
   }//if

   if (flagnum == 23) {
      pc.show("You can't toggle that flag, try rclone or rclear.\n");
      return -1;
   }else if( flagnum == 37) {
      pc.show("Vehicle flags can't be toggled, use olc to make a vehicle\n");
      return -1;
   }
   
   Sprintf(buf, "Toggling flag#:  %i.\n", flagnum);
   show(buf, pc);
   ROOM.flipFlag(flagnum);
   return 0;
   
   
}//tog_rflag


int tog_vflag(int flagnum, critter& pc) {
   String buf(50);

   if (!ok_to_do_action(NULL, "IFPRZ", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(flagnum, 0, 200, pc, TRUE)) {
      return -1;
   }//if

   if (!ROOM.isVehicle()) {
      pc.show("This room is not a vehicle.\n");
      return -1;
   }
   else {
      vehicle* v = (vehicle*)(&ROOM);

      Sprintf(buf, "Toggling vehicle flag#:  %i.\n", flagnum);
      show(buf, pc);
   
      v->toggleVehicleFlag(flagnum);
      return 0;
   }
   return -1;
}//tog_vflag


int set_zflag(int flagnum, const String* on_off, critter& pc) {
   String buf(50);
   int flag;

   if (!ok_to_do_action(NULL, "IFPR", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(flagnum, 0, 200, pc, TRUE)) {
      return -1;
   }//if

   if (strcasecmp(*on_off, "on") == 0) {
      flag = 1;
   }
   else if (strcasecmp(*on_off, "off") == 0) {
      flag = 0;
   }
   else {
      pc.show("Syntax:  set_zflag [flag_num] [on/off]\n");
      return -1;
   }
                                          
   if (flagnum == 23) {
      pc.show("You can't toggle that flag, try rclone or rclear.\n");
   }
   else {
      int start = ZoneCollection::instance().elementAt(pc.getCurRoom()->getZoneNum()).getBeginRoomNum();
      int end = ZoneCollection::instance().elementAt(pc.getCurRoom()->getZoneNum()).getEndRoomNum();
      if (flag) {
         Sprintf(buf, "Turning ON flag#:  %i in rooms %i through %i.\n",
                 flagnum, start, end);
      }
      else {
         Sprintf(buf, "Turning OFF flag#:  %i in rooms %i through %i.\n",
                 flagnum, start, end);
      }         
      show(buf, pc);
      for (int i = start; i <= end; i++) {
         if (room_list.elementAtNoCreate(i)) {
            room_list[i].setFlag(flagnum, flag);
         }//if
      }//for
      return 0;
   }//else
   return -1;
}//tog_zflag


int rset(const String* targ, int new_val, critter& pc) {
   String buf(50);

   if (!ok_to_do_action(NULL, "IFPR", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   int len1;
   int flag = FALSE;

   if ((len1 = targ->Strlen()) == 0) {
      show("You may set any of these, range is in parentheses:\n", pc);
      show("movement (-5, 50)\n", pc);
      show("vis_bit (0, 2 billion), NOTE:  better know what yer doing!\n", pc);
      show("in_room (1, MAX_RM_NUM), Vehicles only.\n", pc);
      show("fall_to (0, MAX_RM_NUM), Where a flying critter will land if it falls.\n",pc);
      return 0;
   }//if

   if (strncasecmp(*targ, "movement", len1) == 0) {
      if (check_l_range(new_val, -5, 50, pc, TRUE)) {
         ROOM.setMovCost(new_val);
         flag = TRUE;
      }
   }//if
   else if (strncasecmp(*targ, "vis_bit", len1) == 0) {
      if (check_l_range(new_val, 0, 2000000000, pc, TRUE)) {
         ROOM.setVisBit(new_val);
         flag = TRUE;
      }
   }//if
   else if (strncasecmp(*targ, "in_room", len1) == 0) {
      if (ROOM.isVehicle()) {
         vehicle* v = (vehicle*)(&(ROOM));
         if (strncasecmp(*targ, "in_room", len1) == 0) {
            if (check_l_range(new_val, 1, NUMBER_OF_ROOMS, pc, TRUE)) {
               v->setInRoom(new_val);
               flag = TRUE;
            }
         }
      }
   }
   else if (strncasecmp(*targ, "fall_to", len1) == 0) {
      if (check_l_range(new_val, 0, NUMBER_OF_ROOMS, pc, TRUE)) {
         ROOM.setFallTo(new_val);
         if (new_val == 0) pc.show("Falling critters will stay in this room.");
         flag = TRUE;
      }
   }

   if (!flag) {
      show("Error trying to rset.\n", pc);
      return -1;
   }//else 
   return 0;
}//rset


// IMM-10 command. Allows (limitted) capability to globally mset a zone 
// this command leaves teachers and player shopkeepers alone.
int area_mset(critter &pc, const String* targ, const String* new_val_string,
        const int new_val) {
    int zone = pc.getCurRoom()->getZoneNum();

    // not bothering to check ok_to_do_action, only IMM-10 can use this
    // command, and without restriction.
    if (!ok_to_do_action(NULL, "I", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
        return -1;
    }//IMMs only
    if ( pc.getImmLevel() < 10) {
        return -1;
    }//And currently only IMM-10

    if ( targ->Strlen() == 0 ) {
        pc.show("This command _ALWAYS_ modifies the \"DB\" directly, though\n");
        pc.show("changes will not be made permanent until an amsave is issued.\n\n");
        pc.show("Not everything can be set through this interface.\n");
        pc.show("ac          (-200, 200)   heat_resis   (-100,200)\n");
        pc.show("cold_resis  (-100, 200)   elect_resis (-100, 200)\n");
        pc.show("spell_resis (-100, 200)   dam_rcv_mod    (1, 200)\n");
        return 0;
    }//if no target_setting.

    // make sure the zone is locked
    if ( !pc.getCurRoom()->isZlocked() ) {
        pc.show("The zone must be zlocked to use this command.\n");
        return -1;
    }

    int len = targ->Strlen();
    int which_stat;//we use this as a shortcut (index for short_cur_stats)

    if ( strncasecmp(*targ, "ac", len) == 0 ) {
        if ( !check_l_range(new_val, -200, 200, pc, TRUE ) ) {
            pc.show("Invalid range.");
            return(-1);
        } else { which_stat = 9; }
    } else if ( strncasecmp(*targ, "heat_resis", len) == 0 ) {
        if ( !check_l_range(new_val, -100, 200, pc, TRUE ) ) {
            pc.show("Invalid range.");
            return(-1);
        } else { which_stat = 29; }
    } else if ( strncasecmp(*targ, "cold_resis", len) == 0 ) {
        if ( !check_l_range(new_val, -100, 200, pc, TRUE ) ) {
            pc.show("Invalid range.");
            return(-1);
        } else { which_stat = 30; }
    } else if ( strncasecmp(*targ, "elect_resis", len) == 0 ) {
        if ( !check_l_range(new_val, -100, 200, pc, TRUE ) ) {
            pc.show("Invalid range.");
            return(-1);
        } else { which_stat = 31; }
    } else if ( strncasecmp(*targ, "spell_resis", len) == 0 ) {
        if ( !check_l_range(new_val, -100, 200, pc, TRUE ) ) {
            pc.show("Invalid range.");
            return(-1);
        } else { which_stat = 32; }
    } else if ( strncasecmp(*targ, "dam_rcv_mod", len) == 0 ) {
        if ( !check_l_range(new_val, 1, 200, pc, TRUE ) ) {
            pc.show("Invalid range.");
            return(-1);
        } else { which_stat = 27; }
    } else {
        pc.show("Not a valid stat for modification with this command.\n");
        return(-1);
    }

    // walk all in-use, in-this-zone mobs and modify the stat.
    for (int i = 0; i < NUMBER_OF_MOBS; i++ ) {
        if ( mob_list[i].getNativeZoneNum() == zone ) {
            if ( mob_list[i].isInUse() ) {
                if ( mob_list[i].isTeacher() || mob_list[i].isPlayerShopKeeper() ) {
                    continue;
                }//we leave teachers and player owned shops alone.
                mob_list[i].short_cur_stats[which_stat] = new_val;
            }//mob exists.
        }//mob belongs to the right zone.
    }//mob_list[] loop.
    pc.show("Completed. Changes will not show up until a reboot, though you can\n");
    pc.show("see them in the database with a mstat # (vs. name). Any new mobs\n");
    pc.show("that you load will reflect the changes. Be sure to amsave to make\n");
    pc.show("the change permanent.\n");
    pc.show("\nPlease note that player shopkeepers and teachers are unaffected\n");
    pc.show("by this command.\n\n");
    return 0 ;
}//area_mset

/* vict is the name of the mob you wish to mset, targ is thing to set */
int mset(int i_th, const String* vict, const String* targ, int new_val,
         const String* new_val_string, critter& pc) {
   String buf(50);

   if ((pc.getImmLevel() >= 7) && !(pc.getCurRoom()->isZlocked())) {
      if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
         return -1;
      }
   }
   else {
      if (!ok_to_do_action(NULL, "IFPR", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
         return -1;
      }
   }

   if (vict->Strlen() == 0) {
      show("NOTE:  some of these may be restricted from you.\n", pc);
      show("These are your choices, range is in parentheses:\n\n", pc);
      show("Usage:  mset <<i_th> [mob] [thing_to_set] [new_value]\n", pc);
      show("If the new value is a string with spaces, be sure to\n", pc);
      show("put it in single quotes.\n", pc);

      Sprintf(buf, "skin_num, for MOBS (0-%i)\n", NUMBER_OF_ITEMS);
      show(buf, pc);
      show("open_time [shop keepers] (0, 23)\n", pc);
      show("close_time [shop keepers] (0, 23)\n", pc);
      show("buy% (0, 1000)              markup% (sell) (0, 10000)\n", pc);
      show("mob_name (keyword)          short_desc\n", pc);
      show("in_room_desc                naked_weight (1, 20000)\n", pc);
      show("sex (0-female, 1-male, 2-neuter)\n", pc);
      show("position (0, 6)             alignment (-1000 1000)\n", pc);
      show("hunger (-1, 9999)           thirst (-1, 9999)\n", pc);
      show("drugged (-1, 9999)          wimpy(0-99999)\n", pc);
      show("quest_points (0, 1000)      gold (0, 50 million)\n", pc);
      show("experience (0, 100 million) bank_gold (0, 50 million)\n", pc);
      show("strength (0, 100)           intelligence (0, 100)\n", pc);
      show("constitution (0, 100)       charisma (0, 100)\n", pc);
      show("wisdom (0, 100)             dexterity (0, 100)\n", pc);
      show("hit (-100, 100)             dam (0, 100)\n", pc);
      show("ac (-200, 200)              attacks (1, 4)\n", pc);
      show("class (100, 108)            race (0, 17)\n", pc);
      show("level (0, 40)               imm_level (0, 10)\n", pc);
      show("practices (0, 100)          mana (0, 32000)\n", pc);
      show("mov (0, 32000)              hp (0, 32000)\n", pc);
      show("hp_max (0, 32000)           mana_max (0, 32000)\n", pc);
      show("mov_max (0, 32000)          religion (0, 6)\n", pc);
      show("guild (0, 10)               vis_bit (0, 2 billion)\n", pc);
      show("see_bit (0, 2 billion)      max_in_game, for MOBS (0-200)\n", pc);
      show("heat_resis (-100, 200)      cold_resis (-100, 200)\n", pc);
      show("elect_resis (-100, 200)     spell_resis (-100, 200)\n", pc);
      show("bhd_count (0, 20)           bhd_sides (0, 100)\n", pc);
      show("hp_regen (-50, 500)         mana_regen(-50, 500)\n", pc);
      show("dam_giv_mob (1, 1000)       dam_rcv_mod(1, 1000)\n", pc);
      show("mov_regen (-50, 500)        exp_worth(0, millions)\n", pc);
      show("bad_assedness (-10, 10)     skill_violence (-10, 10)\n", pc);
      show("defensiveness (-10, 10)     social_awareness (-10, 10)\n", pc);
      show("benevolence (-10, 10)       pause (0, 50)\n", pc);
      show("Language (0 English, 1 Spanish, 2 Portugues)\n", pc);
      show("home_room (0 - MAX_ROOMS)   password(3 char mininum)\n", pc);
      show("manager (of store)          ticks_old(0, 500)\n", pc);
      show("ticks_till_freedom (0 - 500000)  home_town(0, max_zones)\n", pc);
      return 0;
   }//if

   critter* ptr = NULL;
   if (strcasecmp(*vict, "DB") == 0) {
      if (check_l_range(i_th, 2, NUMBER_OF_MOBS, pc, TRUE, "Mob Number: ")) {
         pc.show("Modifying Database Directly.\n");
         ptr = &(mob_list[i_th]);
      }//if
   }
   else {
      ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);
   }//else

   if (!ptr) {
      show("You don't see that person.\n", pc);
      return -1;
   }//if

   if (!ptr->isInUse()) {
      pc.show("That mob has not been created yet.\n");
      return -1;
   }

   if (!ptr->pc) {
      if ((pc.doesOwnCritter(*ptr)) ||
          (!(ROOM.isZlocked()) && pc.getImmLevel() > 6)) {
         //ok
      }
      else {
         show("You don't have permission to edit this mob.\n", pc);
         return -1;
      }//if
   }//if
   else {
      if (ptr->isImmort() && (ptr->IMM_LEVEL >= pc.IMM_LEVEL)
          && (ptr != &pc)) {
         show("You can't mess with one so powerful.\n", pc);
         return -1;
      }//if
   }//else

   short flag = FALSE; //did it work

   int len1 = targ->Strlen();
   if (len1 == 0) {
      show("Which attribute do you wish to set?\n", pc);
      return -1;
   }//if

   if (ptr->pc && pc.IMM_LEVEL == 10) { //most restricted ones
      if (strncasecmp(*targ, "imm_level", len1) == 0) {
         if (check_l_range(new_val, 0, 10, pc, TRUE)) {
            if (ptr->pc->imm_data) {
               ptr->setImmLevel(new_val);
               show("Imm level changed.\n", pc);
               return 0;
            }//
         }//if
      }//if
   }//if
   
   if ((ptr->pc) && (pc.IMM_LEVEL < 9) && (ptr != &pc)) {
         //only 9+ imms can 'set' pcs
      show("You must be an imm of level 9 or more to 'mset' players.\n", pc);
      return -1;
   }//if

   // NPC Only attributes.
   if (ptr->isNpc()) {
      if (strncasecmp(*targ, "bad_assedness", len1) == 0) {
         if (check_l_range(new_val, -10, 10, pc, TRUE)) {
            ptr->mob->setBadAssedness(new_val);
            flag = TRUE;
         }//if
      }//if
      else if (strncasecmp(*targ, "skill_violence", len1) == 0) {
        if (check_l_range(new_val, -10, 10, pc, TRUE)) {
          ptr->mob->setSkillViolence(new_val);
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "ticks_old", len1) == 0) {
        if (check_l_range(new_val, 0, 500, pc, TRUE)) {
          ptr->mob->setTicksOld(new_val);
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "ticks_till_freedom", len1) == 0) {
        if (check_l_range(new_val, 0, 500000, pc, TRUE)) {
          ptr->mob->setTicksTillFreedom(new_val);
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "home_room", len1) == 0) {
        if (check_l_range(new_val, 0, NUMBER_OF_ROOMS - 1, pc, TRUE)) {
           ptr->mob->home_room = new_val;
           flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "defensiveness", len1) == 0) {
        if (check_l_range(new_val, -10, 10, pc, TRUE)) {
          ptr->mob->setDefensiveness(new_val);
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "social_awareness", len1) == 0) {
        if (check_l_range(new_val, -10, 10, pc, TRUE)) {
          ptr->mob->setSocialAwareness(new_val);
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "benevolence", len1) == 0) {
        if (check_l_range(new_val, -10, 10, pc, TRUE)) {
          ptr->mob->setBenevolence(new_val);
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "manager", 7) == 0) {
         flag = TRUE;
         if (pc.getImmLevel() >= 9) {
            if (ptr->isPlayerShopKeeper()) {
               ptr->mob->proc_data->sh_data->manager = *new_val_string;
               
               if (ptr->isSmob()) {
                  pc.show("Saving shop owner now.\n");
                  save_player_shop_owner(*ptr);
               }//if

            }//if
            else {
               pc.show("Not a shopkeeper.\n");
            }//else
         }//if
         else {
            pc.show("You must be level 9 immort or higher.\n");
         }
      }//if
   }//if


   if (ptr->pc) { //only can set these on pc's
      if (strncasecmp(*targ, "hunger", len1) == 0) {
        if (check_l_range(new_val, -1, 9999, pc, TRUE)) {
          ptr->HUNGER = new_val;
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "thirst", len1) == 0) {
        if (check_l_range(new_val, -1, 9999, pc, TRUE)) {
          ptr->THIRST = new_val;
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "drugged", len1) == 0) {
        if (check_l_range(new_val, -1, 9999, pc, TRUE)) {
          ptr->DRUGGED = new_val;
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "quest_points", len1) == 0) {
        if (check_l_range(new_val, 0, 1000, pc, TRUE)) {
          ptr->pc->quest_points = new_val;
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "practices", len1) == 0) {
        if (check_l_range(new_val, 0, 100, pc, TRUE)) {
          ptr->PRACS = new_val;
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "password", len1) == 0) {
         if (new_val_string->Strlen() > 3) {
             ptr->pc->password = crypt(*new_val_string, "bg");
             pc.show("Password changed.");
             return 0;
         }
         pc.show("Password must be more than 3 characters long.");
         return -1;
      }//if
      else if (strncasecmp(*targ, "language", len1) == 0) {
        if (check_l_range(new_val, (int)(English), (int)(LastLanguage - 1),
                          pc, TRUE)) {
          ptr->pc->preferred_language = (LanguageE)(new_val);
          ptr->PC_FLAGS.turn_on(27);
          flag = TRUE;
        }//if
      }//if
   }//if pc only


   /* go check the rest, can be for pc OR mob, no more restrictions */

   if (strncasecmp(*targ, "max_in_game", len1) == 0) {
     if (check_l_range(new_val, 0, 200, pc, TRUE)) {
       if (ptr->mob) {
          ptr->setMaxInGame(new_val);
          flag = TRUE;
        }//if
        else {
           show("This can only work on MOBILES.\n", pc);
        }//else
     }//if
   }//if
   else if (strncasecmp(*targ, "naked_weight", len1) == 0) {
     if (check_l_range(new_val, 1, 20000, pc, TRUE)) {
        ptr->setNakedWeight(new_val);
        flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "wimpy", len1) == 0) {
     if (check_l_range(new_val, 0, 99999, pc, TRUE)) {
        ptr->setWimpy(new_val);
        flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "home_town", len1) == 0) {
     if (check_l_range(new_val, 0, NUMBER_OF_ZONES, pc, TRUE)) {
        ptr->setHomeTown(new_val);
        flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mob_name", len1) == 0) {
      if (!ptr->isPc()) {
         ptr->names.clearAndDestroy();
         ptr->names.append(new String(*new_val_string));
         flag = TRUE;
      }//if
      else {
         show("This can only work on MOBILES.\n", pc);
      }//else
   }//if
   else if (strncasecmp(*targ, "short_desc", len1) == 0) {
      if (!ptr->isPc()) {
         ptr->short_desc = *new_val_string;
         flag = TRUE;
      }//if
      else {
         show("This can only work on MOBILES.\n", pc);
      }//else
   }//if
   else if (strncasecmp(*targ, "in_room_desc", len1) == 0) {
      if (!ptr->isPc()) {
         ptr->in_room_desc = *new_val_string;
         flag = TRUE;
      }//if
      else {
         show("This can only work on MOBILES.\n", pc);
      }//else
   }//if
   else if (strncasecmp(*targ, "close_time", len1) == 0) {
     if (check_l_range(new_val, 0, 23, pc, TRUE)) {
       if (ptr->mob && ptr->mob->proc_data &&
           ptr->mob->proc_data->sh_data) {
          ptr->mob->proc_data->sh_data->close_time = new_val;
          flag = TRUE;
        }//if
        else {
          show("This can only work on MOBILES.\n", pc);
        }//else
     }//if
   }//if
   else if (strncasecmp(*targ, "buy%", len1) == 0) {
     if (check_l_range(new_val, 0, 10000, pc, TRUE)) {
       if (ptr->mob && ptr->mob->proc_data &&
           ptr->mob->proc_data->sh_data) {
          ptr->mob->proc_data->sh_data->buy_percentage = new_val;
          flag = TRUE;
        }//if
        else {
          show("This can only work on MOBILES.\n", pc);
        }//else
     }//if
   }//if
   else if (strncasecmp(*targ, "markup%", len1) == 0) {
     if (check_l_range(new_val, 0, 10000, pc, TRUE)) {
       if (ptr->mob && ptr->mob->proc_data &&
           ptr->mob->proc_data->sh_data) {
          ptr->mob->proc_data->sh_data->markup = new_val;
          flag = TRUE;
        }//if
        else {
          show("This can only work on MOBILES.\n", pc);
        }//else
     }//if
   }//if
   else if (strncasecmp(*targ, "close_time", len1) == 0) {
     if (check_l_range(new_val, 0, 23, pc, TRUE)) {
       if (ptr->mob && ptr->mob->proc_data &&
           ptr->mob->proc_data->sh_data) {
          ptr->mob->proc_data->sh_data->close_time = new_val;
          flag = TRUE;
        }//if
        else {
          show("This can only work on MOBILES.\n", pc);
        }//else
     }//if
   }//if
   else if (strncasecmp(*targ, "open_time", len1) == 0) {
     if (check_l_range(new_val, 0, 23, pc, TRUE)) {
       if (ptr->mob && ptr->mob->proc_data &&
           ptr->mob->proc_data->sh_data) {
          ptr->mob->proc_data->sh_data->open_time = new_val;
          flag = TRUE;
        }//if
        else {
          show("This can only work on MOBILES.\n", pc);
        }//else
     }//if
   }//if
   else if (strncasecmp(*targ, "skin_num", len1) == 0) {
     if (check_l_range(new_val, 0, NUMBER_OF_ITEMS, pc, TRUE)) {
       if (ptr->mob) {
         if (new_val == 0)  {
           ptr->MOB_FLAGS.turn_off(16);
             ptr->mob->skin_num = 0;
         }//if
         else {
           ptr->MOB_FLAGS.turn_on(16);
           ptr->mob->skin_num = new_val;
         }//else
         flag = TRUE;
        }//if
        else {
          show("This can only work on MOBILES.\n", pc);
        }//else
     }//if
   }//if
   else if (strncasecmp(*targ, "gold", len1) == 0) {
     if (check_l_range(new_val, 0, 50000000, pc, TRUE)) {
       ptr->GOLD = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "position", len1) == 0) {
     if (check_l_range(new_val, POS_STAND, POS_PRONE, pc, TRUE)) {
       ptr->setPosn(new_val);
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "experience", len1) == 0) {
     if (check_l_range(new_val, 0, 100000000, pc, TRUE)) {
       ptr->EXP = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "exp_worth", len1) == 0) {
     if (check_l_range(new_val, 0, 100000000/config.experienceDivisor, pc, TRUE)) {
       ptr->EXP = new_val * config.experienceDivisor;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "pause", len1) == 0) {
     if (check_l_range(new_val, 0, 50, pc, TRUE)) {
       ptr->setPause(new_val);
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "alignment", len1) == 0) {
     if (check_l_range(new_val, -1000, 1000, pc, TRUE)) {
       ptr->ALIGN = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "heat_resis", len1) == 0) {
     if (check_l_range(new_val, -100, 200, pc, TRUE)) {
       ptr->HEAT_RESIS = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "cold_resis", len1) == 0) {
     if (check_l_range(new_val, -100, 200, pc, TRUE)) {
       ptr->COLD_RESIS = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "elect_resis", len1) == 0) {
     if (check_l_range(new_val, -100, 200, pc, TRUE)) {
       ptr->ELEC_RESIS = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "spell_resis", len1) == 0) {
     if (check_l_range(new_val, -100, 200, pc, TRUE)) {
       ptr->SPEL_RESIS = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "bhd_count", len1) == 0) {
     if (check_l_range(new_val, 0, 20, pc, TRUE)) {
       ptr->BH_DICE_COUNT = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "bhd_sides", len1) == 0) {
     if (check_l_range(new_val, 0, 100, pc, TRUE)) {
       ptr->BH_DICE_SIDES = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "hp_regen", max(len1, 3)) == 0) {
     if (check_l_range(new_val, -50, 500, pc, TRUE)) {
       ptr->HP_REGEN = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "dam_giv_mod", max(len1, 5)) == 0) {
     if (check_l_range(new_val, 1, 1000, pc, TRUE)) {
       ptr->DAM_GIV_MOD = new_val;
       flag = TRUE;
     }//if
   }//if
   else if ((strncasecmp(*targ, "dam_rcv_mod", max(len1, 5)) == 0) ||
            (strncasecmp(*targ, "dam_rec_mod", max(len1, 5)) == 0)) {
     if (check_l_range(new_val, 1, 1000, pc, TRUE)) {
       ptr->DAM_REC_MOD = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mana_regen", max(len1, 5)) == 0) {
     if (check_l_range(new_val, 0, 500, pc, TRUE)) {
       ptr->MA_REGEN = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mov_regen", max(len1, 4)) == 0) {
     if (check_l_range(new_val, -50, 500, pc, TRUE)) {
       ptr->MV_REGEN = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "bank_gold", len1) == 0) {
     if (check_l_range(new_val, 0, 50000000, pc, TRUE)) {
       ptr->BANK_GOLD = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "strength", len1) == 0) {
     if (check_l_range(new_val, 0, 100, pc, TRUE)) {
       ptr->STR = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "intelligence", len1) == 0) {
     if (check_l_range(new_val, 0, 100, pc, TRUE)) {
       ptr->INT = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "constitution", len1) == 0) {
     if (check_l_range(new_val, 0, 100, pc, TRUE)) {
       ptr->CON = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "charisma", len1) == 0) {
     if (check_l_range(new_val, 0, 100, pc, TRUE)) {
       ptr->CHA = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "wisdom", len1) == 0) {
     if (check_l_range(new_val, 0, 100, pc, TRUE)) {
       ptr->WIS = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "dexterity", len1) == 0) {
     if (check_l_range(new_val, 0, 100, pc, TRUE)) {
       ptr->DEX = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "hit", len1) == 0) {
     if (check_l_range(new_val, -100, 100, pc, TRUE)) {
       ptr->HIT = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "dam", len1) == 0) {
     if (check_l_range(new_val, 0, 100, pc, TRUE)) {
       ptr->DAM = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "ac", len1) == 0) {
     if (check_l_range(new_val, -200, 200, pc, TRUE)) {
       ptr->AC = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "attacks", len1) == 0) {
     if (check_l_range(new_val, 1, 4, pc, TRUE)) {
       ptr->ATTACKS = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "sex", len1) == 0) {
     if (check_l_range(new_val, 0, 2, pc, TRUE)) {
       ptr->SEX = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "class", len1) == 0) {
     if (check_l_range(new_val, 1, 9, pc, TRUE)) {
       ptr->CLASS = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "race", len1) == 0) {
     if (check_l_range(new_val, 1, 18, pc, TRUE)) {
        ptr->RACE = new_val;
        flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "level", len1) == 0) {
     if (check_l_range(new_val, 0, 40, pc, TRUE)) {
       ptr->LEVEL = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "hp", len1) == 0) {
     if (check_l_range(new_val, 0, 32000, pc, TRUE)) {
       ptr->HP = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mana", len1) == 0) {
     if (check_l_range(new_val, 0, 32000, pc, TRUE)) {
       ptr->MANA = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mov", len1) == 0) {
     if (check_l_range(new_val, 0, 32000, pc, TRUE)) {
       ptr->MOV = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "hp_max", max(len1, 4)) == 0) {
     if (check_l_range(new_val, 0, 32000, pc, TRUE)) {
       ptr->setHP_MAX(new_val);
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mana_max", max(len1, 5)) == 0) {
     if (check_l_range(new_val, 0, 32000, pc, TRUE)) {
       ptr->MA_MAX = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mov_max", max(len1, 5)) == 0) {
     if (check_l_range(new_val, 0, 32000, pc, TRUE)) {
       ptr->MV_MAX = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "religion", len1) == 0) {
     if (check_l_range(new_val, 0, 6, pc, TRUE)) {
       ptr->RELIGION = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "guild", len1) == 0) {
     if (check_l_range(new_val, 0, 10, pc, TRUE)) {
       ptr->GUILD = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "vis_bit", len1) == 0) {
      if (check_l_range(new_val, 0, 2000000000, pc, TRUE)) {
         if (ptr->isPc() && !(pc.isImmort() && (pc.IMM_LEVEL >= 9))) {
            pc.show("That is restricted to you.\n");
         }
         else {
            ptr->VIS_BIT = new_val;
            flag = TRUE;
         }
      }//if
   }//if
   else if (strncasecmp(*targ, "see_bit", len1) == 0) {
      if (check_l_range(new_val, 0, 2000000000, pc, TRUE)) {
         if (ptr->isPc() && !(pc.isImmort() && (pc.IMM_LEVEL >= 9))) {
            pc.show("That is restricted to you.\n");
         }
         else {
            ptr->SEE_BIT = new_val;
            flag = TRUE;
         }
      }//if
   }//if


   if (flag) {
     show("Ok, new value has been set.\n", pc);
     return 0;
   }//if
   else {
     show("Oops, for some reason it failed, remember that not everyone\n", pc);
     show("can do an mset on everyone else, some things are restricted.\n",
          pc);
     return -1;
   }//else
}//mset


int oset(int i_th, const String* vict, const String* targ, int new_val,
         const String* new_val_string, critter& pc) {
   String buf(50);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (vict->Strlen() == 0) {
      show("NOTE:  some of these may be restricted from you.\n", pc);
      show("These are your choices, range is in parentheses:\n", pc);
      show("Usage:  oset <<i_th> [object_name] [field] [new_value]\n", pc);
      show("If the new value has spaces in it, surround it with single\n", pc);
      show("quotes.\n\n", pc);

      Sprintf(buf, "key_num (0, %i)\n", NUMBER_OF_ITEMS);
      show(buf, pc);
      show("max_weight (0, 9999)        percentage_weight (1, 1000)\n",
           pc);
      show("time_till_disolve (0, 500)  charges/light_count (-1, 100)\n",
           pc);
      show("percent_load (0, 100)       weight (0, 10000)\n", pc);
      show("dice_sides (0, 50)          dice_count (0, 50)\n", pc);
      show("level (0, 30)               vis_bit (0, 2 billion)\n", pc);
      show("price (0, 50 million)       max_in_game(0, 1000)\n", pc);
      show("obj_name (ie keyword)       short_desc\n", pc);
      show("in_room_desc                liquid(1, MAX_OBJS)\n", pc);
      show("max_recharge_cnt(100)       \n", pc);
      return 0;
   }//if

   object* ptr = have_obj_named(pc.inv, i_th, vict, pc.SEE_BIT,
                                ROOM);
   if (!ptr) {
      ptr = ROOM.haveObjNamed(i_th, vict, pc.SEE_BIT);      
   }//if

   if (!ptr) {
      if (strcasecmp(*vict, "DB") == 0) {
         if ((i_th > 1) && (i_th < NUMBER_OF_ITEMS)) {
            if (obj_list[i_th].isInUse()) {
               ptr = &(obj_list[i_th]);
            }//if
         }//if
      }//if
   }//if

   if (!ptr) {
      show("You don't see that object.\n", pc);
      return -1;
   }//if

   if (!pc.doesOwnObject(*ptr)) {
     show("You don't have permission to edit this object.\n", pc);
     return -1;
   }//if

   int len1 = targ->Strlen();

   if (len1 == 0) {
     show("Which attribute do you wish to change??\n", pc);
     return -1;
   }//if

   short flag = FALSE; //did it work

   if (ptr->bag) { //thingies for bags
      if (strncasecmp(*targ, "key_num", len1) == 0) {
        if (check_l_range(new_val, 0, NUMBER_OF_ITEMS, pc, TRUE)) {
          ptr->bag->key_num = new_val;
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "max_weight", len1) == 0) {
        if (check_l_range(new_val, 0, 9999, pc, TRUE)) {
          ptr->bag->max_weight = new_val;
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "percentage_weight", len1) == 0) {
        if (check_l_range(new_val, 1, 1000, pc, TRUE)) {
          ptr->bag->percentage_weight = new_val;
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "time_till_disolve", len1) == 0) {
        if (check_l_range(new_val, 0, 500, pc, TRUE)) {
          ptr->bag->time_till_disolve = new_val;
          flag = TRUE;
        }//if
      }//if
      else if (strncasecmp(*targ, "liquid", len1) == 0) {
        if (check_l_range(new_val, 1, Cur_Max_Obj_Num, pc, TRUE)) {
           if (obj_list[new_val].isInUse() && obj_list[new_val].isLiquid()) {
              if (ptr->isCanteen()) {
                 Cell<object*> cll(ptr->inv);
                 object* optr;
                 while ((optr = cll.next())) {
                    recursive_init_unload(*optr, 0);
                 }//while
                 clear_obj_list(ptr->inv);

                 ptr->gainInv(&(obj_list[new_val]));
                 obj_list[new_val].incrementCurInGame();
                 flag = TRUE;
              }//if
              else {
                 pc.show("That isn't a canteen (liquid holder.)\n");
              }
           }//if
           else {
              pc.show("That 'liquid' object# isn't in the game, or isn't a liquid.\n");
           }
        }//if
      }//if
   }//bag only

   if (strncasecmp(*targ, "charges", len1) == 0) {
      if (check_l_range(new_val, -1, 100, pc, TRUE)) {
         ptr->extras[0] = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "max_recharge_cnt", len1) == 0) {
      if (check_l_range(new_val, 0, 100, pc, TRUE)) {
         ptr->extras[1] = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "obj_name", len1) == 0) {
      ptr->names.clearAndDestroy();
      ptr->names.append(new String(*new_val_string));
      flag = TRUE;
   }//if
   else if (strncasecmp(*targ, "short_desc", len1) == 0) {
      ptr->short_desc = *new_val_string;
      flag = TRUE;
   }//if
   else if (strncasecmp(*targ, "in_room_desc", len1) == 0) {
      ptr->in_room_desc = *new_val_string;
      flag = TRUE;
   }//if
   else if (strncasecmp(*targ, "percent_load", len1) == 0) {
      if (check_l_range(new_val, 0, 100, pc, TRUE)) {
         ptr->extras[2] = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "weight", len1) == 0) {
      if (check_l_range(new_val, 0, 10000, pc, TRUE)) {
         ptr->extras[5] = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "max_in_game", len1) == 0) {
      if (check_l_range(new_val, 0, 1000, pc, TRUE)) {
         ptr->extras[4] = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "dice_sides", len1) == 0) {
      if (check_l_range(new_val, 0, 50, pc, TRUE)) {
         ptr->extras[6] = new_val;
         if(!(pc.pc && pc.pc->pc_data_flags.get(2)
              && pc.pc->imm_data->imm_level >= 9)) {
           normalize_obj(*ptr);
         }
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "dice_count", len1) == 0) {
      if (check_l_range(new_val, 0, 50, pc, TRUE)) {
         ptr->extras[7] = new_val;
         if(!(pc.pc && pc.pc->pc_data_flags.get(2)
              && pc.pc->imm_data->imm_level >= 9)) {
           normalize_obj(*ptr);
         }
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "price", len1) == 0) {
      if (check_l_range(new_val, 0, 50000000, pc, TRUE)) {
         ptr->PRICE = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "level", len1) == 0) {
      if (check_l_range(new_val, 0, 30, pc, TRUE)) {
         ptr->OBJ_LEVEL = new_val;
         if(!(pc.pc && pc.pc->pc_data_flags.get(2)
              && pc.pc->imm_data->imm_level >= 9)) {
           normalize_obj(*ptr);
         }
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "vis_bit", len1) == 0) {
      if (check_l_range(new_val, 0, 2000000000, pc, TRUE)) {
         ptr->OBJ_VIS_BIT = new_val;
         flag = TRUE;
      }//if
   }//if

   if (flag) {
     show("Ok, new value has been set.\n", pc);
     return 0;
   }//if
   else {
     show("Value not set.\n", pc);
     return -1;
   }//else
}//oset



int dset(int i_th, const String* vict, const String* targ, int new_val,
         critter& pc) {
   String buf(50);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (vict->Strlen() == 0) {
      show("NOTE:  some of these may be restricted from you.\n", pc);
      show("These are your choices, range is in parentheses:\n", pc);
      show("toggle_flag:\n"
            "\n"
            "0 open exit, basically no door, 1 is_mag_lockable, 2 is_closed, 3 is_locked,\n"
            "4 is_pickable, 5 is_lockable, 6 mag_locked (spell only can open it),\n"
            "7 is_destructable, 8 is_closeable, 9 is_flippable, \n"
            "11 is_unopenable (other than normal reasons)\n"
            "12 is_vehicle_exit, 13 is_secret, 14 is_blocked, 16 hidden_when_open.\n\n", pc);

      show("vis_bit (web page has definitions)\n", pc);
      show("key_num (0, MAX_ITEM_NUM)\n", pc);
      show("token_num (0, MAX_ITEM_NUM)\n", pc);
      show("distance(0, 10)\n", pc);
      return 0;
   }//if

   door* ptr = door::findDoor(ROOM.DOORS, i_th, vict, pc.SEE_BIT, ROOM);
   if (!ptr) {
      show("You don't see that door.\n", pc);
      return -1;
   }//if


   int len1 = targ->Strlen();

   if (len1 == 0) {
     show("Which attribute do you wish to change??\n", pc);
     return -1;
   }//if

   short flag = FALSE; //did it work

   if (!pc.doesOwnRoom(ROOM)) {
     show("You have to own this zone to change a door.\n", pc);
     return -1;
   }//if

   if (strncasecmp(*targ, "distance", len1) == 0) {
     if (check_l_range(new_val, 0, 15, pc, TRUE)) {
       ptr->distance = new_val;
       flag = TRUE;
     }//if
   }//if

   if (flag) {
     show("Ok.\n", pc);
     return 0;
   }//if


   if (!pc.doesOwnDoor(*ptr)) {
     show("You don't have permission to edit this door.\n", pc);
     return -1;
   }//if

   if (strncasecmp(*targ, "toggle_flag", len1) == 0) {
     if (check_l_range(new_val, 0, 20, pc, TRUE)) {
       if ((new_val != 10) && (new_val != 14)) {
         ptr->dr_data->door_data_flags.flip(new_val);
         flag = TRUE;
       }//if
     }//if
   }//if
   else if (strncasecmp(*targ, "vis_bit", len1) == 0) {
     if (check_l_range(new_val, 0, (1 << 30), pc, TRUE)) {
       ptr->dr_data->vis_bit = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "key_num", len1) == 0) {
     if (check_l_range(new_val, 0, NUMBER_OF_ITEMS, pc, TRUE)) {
       ptr->dr_data->key_num = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "token_num", len1) == 0) {
     if (check_l_range(new_val, 0, NUMBER_OF_ITEMS, pc, TRUE)) {
       ptr->dr_data->token_num = new_val;
       flag = TRUE;
     }//if
   }//if
   

   if (flag) {
     show("Ok, new value has been set.\n", pc);
     return 0;
   }//if
   else {
     show("Value not set for some reason.\n", pc);
     return -1;
   }//else
}//dset


int tog_oflag(int flagnum, const String* flag_type,
              int i_th, const String* obj, critter& pc) {
   String buf(50);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(flagnum, 0, 200, pc, TRUE)) {
      return -1;
   }//if

   object* obj_ptr = NULL;

   if ((obj->Strlen() > 0) && (strcasecmp(*obj, "DB") != 0)) {
      obj_ptr = have_obj_named(pc.inv, i_th, obj, pc.SEE_BIT,
                               ROOM);
      if (!obj_ptr) {
         obj_ptr = ROOM.haveObjNamed(i_th, obj, pc.SEE_BIT);
      }
   }//if
   else { 
      if (!check_l_range(i_th, 2, NUMBER_OF_ITEMS, pc, TRUE))
         return -1;
      
      if (!obj_list[i_th].isInUse()) {
         pc.show("That object does not exist.\n");
         return -1;
      }

      obj_ptr = &(obj_list[i_th]);
   }
        
      
   if (!obj_ptr) {
      pc.show("You don't see that object.\n");
      return -1;
   }//if

   if (!pc.doesOwnObject(*obj_ptr)) {
      show("You don't have permission to edit this object.\n", pc);
      return -1;
   }//if

   if ((strncasecmp(*flag_type, "obj_flag", 1) == 0) ||
       (strncasecmp(*flag_type, "bag_flag", 1) == 0) ||
      (strncasecmp(*flag_type, "spec_flag", 1) == 0)) {
      if (obj_ptr->IN_LIST) {
         Sprintf(buf, "Toggling obj_flag#:  %i on SOBJ:  %S.\n", flagnum, 
                 &(obj_ptr->short_desc));
         show(buf, pc);
      }//if SOBJ
      else {
         Sprintf(buf, "Toggling obj_flag#:  %i on OBJ:  %S.\n", flagnum, 
                 &(obj_ptr->short_desc));
         show(buf, pc);
      }//else
   }//if
   else {
      show("You must specify flagtype as: obj_flag, bag_flag, or spec_flag.\n",
           pc);
      show("See the help page.\n", pc);
      return -1;
   }//else

   if (strncasecmp(*flag_type, "obj_flag", 1) == 0) {
      if (flagnum != 8 && flagnum != 9 && flagnum != 10 && flagnum != 63
          && flagnum != 70 && flagnum != 73 && flagnum != 74) {
         obj_ptr->OBJ_FLAGS.flip(flagnum);

         //Special handling for certain flags.
         switch (flagnum) {
            case 54: //container
               if ( ! obj_ptr->isContainer() ) {
                  // was a container, now it's not.
                  Cell<object*> inv_cll(obj_ptr->inv);
                  object* o_ptr;

                  //reduce cur_in_game counts.
                  while ( (o_ptr = inv_cll.next()) ) {
                     recursive_init_unload(*o_ptr, 0);

                     if (obj_ptr->isModified()) {
                        delete obj_ptr;
                     }//if is a SOBJ
                     obj_ptr->inv.lose(inv_cll);
                  }

                  // clear_obj_list(inv); <-- Think this is missing
                  // recursive_init_unloads. Maybe it shouldn't have it
                  // though. If it should in fact call recursive_init_unloads,
                  // I should fix it and use it instead of the above.

                  delete obj_ptr->bag;
                  obj_ptr->bag = NULL;
               } else {
                  // make it a container
                  obj_ptr->bag = new bag_struct;
               }//else
               break; //case 54
         }//switch(flagnum)

         return 0;
      }//if
      else {
         show("OOPS, you can't toggle that obj_flag.\n", pc);
         return -1;
      }//else
   }//if
   else if (strncasecmp(*flag_type, "bag_flag", 1) == 0) { //bag flags
      if (!obj_ptr->bag) {
         show("Doh, thats not a bag!!\n", pc);
         return -1;
      }//if
      if (flagnum != 8) {
         obj_ptr->bag->bag_flags.flip(flagnum);
         return 0;
      }//if
      else {
         show("OOPS, you can't toggle that bag_flag.\n", pc);
         return -1;
      }//else
   }//else
   else { //spec flags
      if (obj_ptr->obj_proc) {
         obj_ptr->obj_proc = new obj_spec_data();
         obj_ptr->obj_flags.turn_on(63);  //note we have spec data
      }
       
      if ((flagnum == 3) || (flagnum == 0)) {
         obj_ptr->obj_proc->obj_spec_data_flags.flip(flagnum);
         return 0;
      }//if
      else {
         show("OOPS, you can't toggle that bag_flag.\n", pc);
         return -1;
      }//else
   }//else
}// tog_oflag
   

int tog_mflag(int flagnum, const String* flag_type, 
              int i_th, const String* mob, critter& pc) {
   String buf(50);

   if (!ok_to_do_action(NULL, "IFP", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   if (!check_l_range(flagnum, 0, 200, pc, TRUE)) {
      return -1;
   }//if

   critter* mob_ptr = NULL;
   if (strcasecmp(*mob, "DB") != 0) {
      mob_ptr = ROOM.haveCritNamed(i_th, mob, pc.SEE_BIT);
   }
   else {
      if (check_l_range(i_th, 2, NUMBER_OF_MOBS, pc, TRUE, "Mob Number: ")) {
         pc.show("Modifying Database Directly.\n");
         mob_ptr = &(mob_list[i_th]);
      }//if
   }//else

   if (!mob_ptr) {
      show("You don't see that critter.\n", pc);
      return -1;
   }//if

   if (!mob_ptr->isInUse()) {
      pc.show("That mob has not been created yet.\n");
      return -1;
   }


   if (!mob_ptr->pc && !pc.doesOwnCritter(*mob_ptr)) {
      show("You don't have permission to edit this mobile.\n", pc);
      return -1;
   }//if

   if (mob_ptr->pc && mob_ptr->isImmort()) {
      if ((mob_ptr->IMM_LEVEL >= pc.IMM_LEVEL) && (&pc != mob_ptr)) {
         show("You don't have permission to edit this player.\n", pc);
         return -1;
      }//if
   }//if

   if (mob_ptr->pc && (pc.IMM_LEVEL < 9)) {
      show("You aren't allowed to toggle PC flags.\n", pc);
      return -1;
   }//if

   if ((strncasecmp(*flag_type, "mob_flag", 1) == 0) ||
       (strncasecmp(*flag_type, "pc_flag", 1) == 0) ||
       (strncasecmp(*flag_type, "crit_flag", 1) == 0) ||
       (strncasecmp(*flag_type, "teach_flag", 1) == 0) ||
       (strncasecmp(*flag_type, "shop_flag", 1) == 0)) {
      if (mob_ptr->isMob()) {
         Sprintf(buf, "Toggling flag#:  %i on MOB:  %S.\n", flagnum,
                 &(mob_ptr->short_desc));
         show(buf, pc);
      }//if SOBJ
      else if (mob_ptr->isSmob()) {
         Sprintf(buf, "Toggling flag#:  %i on SMOB:  %S.\n", flagnum,
                 &(mob_ptr->short_desc));
         show(buf, pc);
      }//else
      else if (mob_ptr->isPc()) {
         Sprintf(buf, "Toggling flag#:  %i on PC:  %S.\n", flagnum,
                 mob_ptr->getName());
         show(buf, pc);
      }
   }//if
   else {
      show("You must specify flagtype as:  'mob_flag', 'pc_flag', 'crit_flag', 'teach_flag' or 'shop_flag'.\n",
           pc);
      return -1;
   }//else

   if (strncasecmp(*flag_type, "mob_flag", 1) == 0) {
      if ((flagnum == 1) || (flagnum == 2) || (flagnum == 5) ||
         (flagnum == 6) || (flagnum == 7) || (flagnum == 8) ||
         (flagnum == 9) ||  (flagnum == 10)) {
         if (mob_ptr->isNpc()) {
            mob_ptr->MOB_FLAGS.flip(flagnum);
            return 0;
         }
         else {
            pc.show("That is not an NPC.\n");
            return -1;
         }
      }//if
      else {
         show("OOPS, you can't toggle that flag.\n", pc);
         return -1;
      }//else
   }//if
   else if (strncasecmp(*flag_type, "shop_flag", 1) == 0) {
      if (mob_ptr->mob && mob_ptr->mob->hasProcData()) {
         return mob_ptr->mob->togShopFlag(flagnum);
      }
      else {
         pc.show("OOPS:  That critter is not a shop owner.\n");
         return -1;
      }
   }
   else if (strncasecmp(*flag_type, "teach_flag", 1) == 0) {
      if (mob_ptr->isTeacher()) {
         return mob_ptr->mob->togTeachFlag(flagnum);
      }
      else {
         pc.show("OOPS:  That critter is not a teacher.\n");
         return -1;
      }
   }
   else if (strncasecmp(*flag_type, "pc_flag", 1) == 0) {
      if (mob_ptr->pc) {
         switch(flagnum) {
            case 24: case 30: case 31:
               mob_ptr->PC_FLAGS.flip(flagnum);
               return 0;
            default:
               show("OOPS, you can't toggle that flag.\n", pc);
               show("Valid flags: 24 !wizchat, 30 is_remort, 31 has_sacrificed.\n", pc);
               return -1;
         }
      }
      else {
         pc.show("OOPS:  That critter is not a PC.\n");
         return -1;
      }
   }
   else if (strncasecmp(*flag_type, "crit_flag", 1) == 0) {
      if ((flagnum != 18) && (flagnum != 24)) { //put restrictions here
         mob_ptr->CRIT_FLAGS.flip(flagnum);
         return 0;
      }//if
      else {
         show("OOPS, you can't toggle that flag.\n", pc);
         return -1;
      }//else
   }//else 
   else {
      pc.show("You must specify flagtype as:  'mob_flag', 'pc_flag', 'crit_flag', 'teach_flag' or 'shop_flag'.\n");
      return -1;
   }
   return 0 ;
}// tog_m
