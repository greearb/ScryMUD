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

///**********************  command4.cc  ******************************///
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include "command4.h"
#include "command5.h"
#include "misc.h"
#include "misc2.h"
#include <stdio.h>
#include "classes.h"
#include "battle.h"
#include "spec_prc.h"
#include "spells.h"
#include "skills.h"
#include <fstream.h>
#include <PtrArray.h>
#include "load_wld.h"
#include <time.h>
#include <unistd.h>


void do_gecho(const char* msg) {
   Cell<critter*> cll(pc_list);
   critter* ptr;

   while ((ptr = cll.next())) {
      show(msg, *ptr);
   }//while
}//do_gecho


void title(const String* tit, critter& pc) {
   if (tit->Strlen() > 79) {
      show("Your title must be less than 80 characters.\n", pc);
      return;
   }//if
   (pc.short_desc) = *tit;
   show("Title changed.\n", pc);
}//title


void poofin(String* pin, critter& pc) {
   if (!pc.isImmort()) {
      pc.show("Eh?");
      return;
   }

   if (pin->Strlen() > 79) {
      show("Your poofin must be less than 80 characters.\n", pc);
      return;
   }//if

   for (int i = 0; i<pin->Strlen(); i++) {
      if (pin->charAt(i) == '\n') {
         pin->setCharAt(i, ' ');
      }//if
   }//for

   pc.setPoofin(*pin);
   show("Poofin changed.\n", pc);
}//poofin


void poofout(String* pin, critter& pc) {
   if (!pc.isImmort()) {
      pc.show("Eh?");
      return;
   }

   if (pin->Strlen() > 79) {
      show("Your poofout must be less than 80 characters.\n", pc);
      return;
   }//if
   
   for (int i = 0; i<pin->Strlen(); i++) {
      if (pin->charAt(i) == '\n') {
         pin->setCharAt(i, ' ');
      }//if
   }//for

   pc.setPoofout(*pin);
   show("Poofout changed.\n", pc);
}//poofout


void oreload(critter& pc) {

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (pc.isImmort()) {
      if (pc.doesOwnRoom(ROOM)) {
         update_objects(ROOM.getZoneNum(), TRUE);
         show("Objects in your current zone have been reloaded.\n", pc);
      }//if
      else {
         show ("You don't own this zone.\n", pc);
      }//else
   }//if
   else {
      show("Eh??\n", pc);
   }//else
}//oreload


void mreload(critter& pc) {
   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (pc.isImmort()) {
      if (pc.doesOwnRoom(ROOM)) {
         update_critters(ROOM.getZoneNum(), TRUE);
         show("Critters in your current zone have been reloaded.\n", pc);
      }//if
      else {
         show ("You don't own this zone.\n", pc);
      }//else
   }//if
   else {
      show("Eh??\n", pc);
   }//else
}//mreload


void read(int i_th, const String* post, critter& pc) {
   String buf(100);
   if (!pc.pc)
      return;

   Cell<object*> cll(ROOM.inv);
   object* ptr;

   while ((ptr = cll.next())) {
      if (ptr->OBJ_FLAGS.get(74))
         break;
   }//while
   if (!ptr) {
      show("You don't see a bulletin board here.\n", pc);
      return;
   }//if

   object* msg;
   if (post->Strlen() > 0) {
      msg = have_obj_named(ptr->ob->inv, i_th, post, ~0, ROOM);
   }//if
   else { //by number only
      Sprintf(buf, "%i", i_th);
      msg = have_obj_named(ptr->ob->inv, 1, &buf, ~0, ROOM);
   }//else

   if (!msg) {
      show("The board doesn't contain that message.\n", pc);
      return;
   }//if

   show(msg->ob->short_desc, pc);
   show("\n\n", pc);
   show(msg->ob->long_desc, pc);
}//read


void siteban(const String* site, critter& pc) {
   String buf(100);
   if (!pc.pc || !pc.pc->imm_data || !(pc.IMM_LEVEL >= 8)) {
      pc.show("Eh??");
      return;
   }

   if (pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (site->Strlen() < 2) {
      pc.show("You need to be more specific as to which site to ban.\n");
      return;
   }

   banned_hosts.append(new String(*site));

   pc.show("Done.");
   writeSiteBanned();
}//siteban


void unsiteban(const String* site, critter& pc) {
   String buf(100);
   if (!pc.pc || !pc.pc->imm_data || !(pc.IMM_LEVEL >= 8)) {
      pc.show("Eh??");
      return;
   }

   if (pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   Cell<String*> cll(banned_hosts);
   String* ptr;
   ptr = cll.next();
   while (ptr) {
      if (*ptr == *site) {
         delete ptr;
         ptr = banned_hosts.lose(cll);
         pc.show("Removed it..");
         writeSiteBanned();
         return;
      }
      else {
         ptr = cll.next();
      }
   }

   pc.show("Didn't find that site to delete it...");
}//unsiteban


void list_site_bans(critter& pc) {
   String buf(100);
   if (!pc.pc)
      return;

   pc.show("These sites are banned:\n\n");

   Cell<String*> cll(banned_hosts);
   String* ptr;

   while ((ptr = cll.next())) {
      pc.show("      ");
      pc.show(*ptr);
      pc.show("\n");
   }
}//list_site_bans


void passwd(const String* old, const String* new1, const String* new2,
            critter& pc) {
   if (!pc.pc)
      return;

   if (pc.pc->password != crypt((const char*)(*old), "bg")) {
      pc.show("Please enter a correct old password, seek help.\n");
      return;
   }

   if (*new1 != *new2) {
      pc.show("Your new passwords do not match, please retry.\n");
      return;
   }

   pc.pc->password = crypt(*new1, "bg");

   pc.show("Password successfully changed.\n");

   pc.save();

}//passwd


void unpost(int i_th, const String* post, critter& pc) {
   String buf(100);
   if (!pc.pc || !pc.pc->imm_data || !(pc.IMM_LEVEL >= 8)) {
      pc.show("Eh??");
      return;
   }

   if (pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   Cell<object*> cll(ROOM.inv);
   object* ptr;

   while ((ptr = cll.next())) {
      if (ptr->OBJ_FLAGS.get(74))
         break;
   }//while

   if (!ptr) {
      show("You don't see a bulletin board here.\n", pc);
      return;
   }//if

   object* msg;
   if (post->Strlen() > 0) {
      msg = have_obj_named(ptr->ob->inv, i_th, post, ~0, ROOM);
   }//if
   else { //by number only
      Sprintf(buf, "%i", i_th);
      msg = have_obj_named(ptr->ob->inv, 1, &buf, ~0, ROOM);
   }//else

   if (!msg) {
      show("The board doesn't contain that message.\n", pc);
      return;
   }//if


   // NOTE:  Don't ever decrementCurInGame or anything, post objects
   //        are screwy in that they never have a counterpart in the
   //        obj_list.
   ptr->ob->inv.loseData(msg);
   delete msg;

	/* update the numbers and short_descs for asthetic reasons */
   short eos, term_by_period;
   int i = 1;
   ptr->ob->inv.head(cll);
   while ((msg = cll.next())) {
      clear_ptr_list(msg->ob->names);
      Sprintf(buf, "%i", i);
      Put(new String(buf), msg->ob->names);
      msg->ob->names.append(new String("message"));
      buf = msg->ob->short_desc.Get_Command(eos, term_by_period);
      Sprintf(buf, "Message_%i", i);
      msg->ob->short_desc.Prepend(buf);
      i++;
   }//while

   show("The message has been removed.\n", pc);

   write_board(*ptr); //save new state
}//unpost


void write_board(object& obj) {
   String buf(100);
   
   Sprintf(buf, "./Boards/board_%i", obj.OBJ_NUM);
   ofstream ifile(buf);
   if (!ifile) {
      mudlog.log(ERR, "ERROR:  couldn't open board file.\n");
      return;
   }//if

   Cell<object*> cll(obj.ob->inv);
   object* ptr;

   while ((ptr = cll.prev())) {
      ifile << ptr->OBJ_NUM << "\t\tMessage number\n";
      ptr->Write(ifile);
      ifile << "\n\n";
   }//while

   ifile << "-1		EOF.\n";
}//write_board


void post(const String* title, critter& pc) {
   if (!pc.pc)
      return;

   if (pc.POS > POS_SIT) {
     show("You are too relaxed.\n", pc);
     return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (pc.isParalyzed()) {
      show("You can't move a muscle.\n", pc);
      return;
   }//if

   if (pc.isGagged()) {
      pc.show("You have been gagged!\n");
      return;
   }//if
   
   object* ptr;
   Cell<object*> cll(ROOM.inv);
   while ((ptr = cll.next())) {
      if (ptr->OBJ_FLAGS.get(74)) {
         break;

      }//if
   }//while

   if (!ptr) {
      show("There isn't a board here to post upon.\n", pc);
      return;
   }//if


   show("Enter your message to be posted.\n", pc);
   show("Use a solitary '~' on a line by itself to end.\n", pc);
   pc.pc->post_msg = new object;
   pc.pc->post_msg->ob->short_desc = *title;
   pc.setMode(MODE_WRITING_POST); 
}//post


void do_post(critter& pc) {
   String buf = pc.pc->input.Get_Rest();

   if (buf.Strlen() == 0) {
      show("Input a line of your message or a ~ to end it.\n", pc);
      return;
   }//if

   if (buf == "~") {
      show("Your message has been posted.\n", pc);

      Cell<object*> cll(ROOM.inv);
      object* ptr;
      while ((ptr = cll.next())) {
         if (ptr->OBJ_FLAGS.get(74)) {
            break;
         }//if
      }//while
      
      if (!ptr) {
         show("Ack, someone moved the bulitin board.\n", pc);

			/* clean up mess */
	 delete pc.pc->post_msg;
         pc.pc->post_msg = NULL;
      }//if
      else {		/* good to go, save to disk */
         pc.pc->post_msg->ob->in_list = &(ptr->ob->inv);  //make it SOBJ
	 pc.pc->post_msg->OBJ_FLAGS.turn_on(7); //!mort
	 int i = get_next_msg_num(*ptr);
	 pc.pc->post_msg->OBJ_NUM = i;

         String* name = new String(20);
	 Sprintf(*name, "%i", i);
         Put(name, (pc.pc->post_msg->ob->names));
         Sprintf(buf, "message_%i", i);
         name = new String(buf);
         pc.pc->post_msg->ob->names.append(name);

	 String tmp_desc((pc.pc->post_msg->ob->short_desc));
   	 Sprintf((pc.pc->post_msg->ob->short_desc),
		 "message_%i from %S:  ", i, name_of_crit(pc, ~0));
         pc.pc->post_msg->ob->short_desc.Append(tmp_desc);

   	 Sprintf((pc.pc->post_msg->ob->in_room_desc),
		 "A message from %S lies here.", name_of_crit(pc, ~0));


	 Sprintf(buf, "./Boards/board_%i", ptr->OBJ_NUM);
	 ifstream ifile(buf, ios::nocreate);
     	 if (!ifile) {
	    mudlog.log(INF, "CHECK:  creating new board file.\n");
	    String buf2(100);
	    Sprintf(buf2, "cp ./World/DEFAULT_OBJECTS ./Boards/board_%i",
		     ptr->OBJ_NUM);
	    system(buf2); //now it exists
	 }//if
	 else
	    mudlog.log(INF, "CHECK:  didn't create new board file.\n");
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

         Put(pc.pc->post_msg, ptr->ob->inv);
	 pc.pc->post_msg = NULL;
      }//else

      pc.setMode(MODE_NORMAL);
      return;
   }//if

   (pc.pc->post_msg->ob->long_desc) += buf;
   (pc.pc->post_msg->ob->long_desc) += "\n";
}//do_post


/* called as 'quit' by the player */
void log_out(critter& pc) {
   if (mudlog.ofLevel(DBG))
      mudlog << "in log_out:  "  << *(name_of_crit(pc, ~0))
             << " getCurRoomNum():  " << pc.getCurRoomNum()
             << "PC's addr:  " << &pc << endl;

   if (!pc.pc)
      return;

   if (!IsEmpty(pc.IS_FIGHTING)) {
      show("You must stop fighting first.\n", pc);
      return;
   }//if
   pc.save();
   pc.setMode(MODE_QUIT_ME_PLEASE);

   show("Cya later.\n", pc);
   emote("has left the game.\n", pc, ROOM, TRUE);
}//log_out


void go(int i_th, const String* direction, critter& pc, int& is_dead) {
   if (pc.isMob()) { //if MOB
      return;
   }//if
   move(pc, i_th, *direction, TRUE, ROOM, is_dead);
}//go


void snoop(int i_th, const String* vict, critter& pc) {

   if (!pc.pc || !pc.pc->imm_data || pc.IMM_LEVEL < 8) {
      show("Eh??", pc);
      return;
   }//if

   if (pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   critter* ptr = have_crit_named(pc_list, i_th, vict, pc.SEE_BIT, ROOM);

   if (!ptr) {
      pc.show("That person cannot be found among the active players.\n");
      return;
   }
   else if (ptr->SNOOPED_BY) {
      pc.show("You can't snoop that person now.\n");
      return;
   }
   else if (!ptr->pc) {
      pc.show("You can only snoop PC's.\n");
      return;
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
}//snoop

void unsnoop(critter& pc) {
   if (pc.pc && pc.SNOOPING) {

      if (mudlog.ofLevel(WRN)) {
         mudlog << "SNOOP:  " << *(pc.getName()) << " stopped snooping:  "
                << *(pc.SNOOPING->getName()) << endl;
      }

      pc.SNOOPING->SNOOPED_BY = NULL;
      pc.SNOOPING = NULL;
      pc.show("You have stopped snooping.\n");

   }//if
}//unsnoop


void possess(int i_th, const String* vict, critter& pc) {

   if (!pc.pc || !pc.pc->imm_data || pc.IMM_LEVEL < 5) {
      show("Eh??", pc);
      return;
   }//if

   if (pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   critter* ptr = ROOM.haveCritNamed(i_th, vict, pc.SEE_BIT);

   if (!ptr) {
      pc.show("That person is not here.\n");
      return;
   }
   else if (ptr->possessed_by) {
      pc.show("You can't possess that person now.\n");
      return;
   }
   else if (ptr->isPc()) {
      pc.show("You can't possess PC's at this time.\n");
      return;
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
}//possess


void _goto(int i_th, const String* name, critter& pc) {
   int rm_num = -1;

   if (!pc.isImmort()) {
      show(PARSE_ERR_MSG, pc);
      return;
   }//if

   if (pc.isFrozen()) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (!IsEmpty(pc.IS_FIGHTING)) {
      show("You can't goto while fighting!\n", pc);
      return;
   }//if

   if (name->Strlen() == 0) { //called like goto <rm_number>
      //mudlog << "_goto:  name has zero length." << endl;
      if (!check_l_range(i_th, 0, NUMBER_OF_ROOMS, pc, TRUE))
         return;
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
         return;
      }//if
      i_th = rm_num;
   }//else      

   if (!check_l_range(i_th, 0, NUMBER_OF_ROOMS - 1, pc, FALSE)) {
      show("Got an out of range room_num.  Check to see if the\n", pc);
      show("mob you tried to go to is constructed correctly.\n", pc);
      return;
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
            return;
         }//if
      }//if
   }//if

   String buf(100);

   if (pc.shouldDoPoofout()) {
      ROOM.doPoofOut(pc);
   }
   else {
      emote("dissapears with a popping sound!\n", pc, ROOM, FALSE);
   }

   //mudlog << "WARNING: about to doGoToRoom: " << i_th << endl;

   int is_dead;
   pc.doGoToRoom(i_th, NULL, NULL, is_dead, pc.getCurRoomNum());

   if (pc.shouldDoPoofin()) {
      ROOM.doPoofIn(pc);
   }
   else {
      emote("appears suddenly.\n", pc, ROOM, FALSE);
   }
}//_goto room_number


void where(int i_th, const String* name, critter& pc) {
   Cell<critter*> cll;
   critter* ptr = NULL;
   String buf(100);

   int zn = ROOM.getZoneNum();

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (name->Strlen() == 0) { //list all in zone
      if (!pc.isImmort()) {
         pc_list.head(cll);
         while ((ptr = cll.next())) {
            if (zn == ptr->getCurZoneNum()) {
               if ((pc.LEVEL + 3 > ptr->LEVEL) &&
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
                  }
                  else {
                     Sprintf(buf, "[Room %i] %P11[PC] ***%S***.\n",
                             i, ptr->getName());
                  }
                  pc.show(buf);
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
               Sprintf(buf, "[Room %i] %P11[PC]%P20%S.\n",
                       ptr->getCurRoomNum(), ptr->getName());
               pc.show(buf);
            }//while
            return;
         }//if
         else if (strcasecmp(*name, "objs") == 0) {
         int start = ZoneCollection::instance().elementAt(pc.getCurZoneNum()).getBeginRoomNum();
         int end = ZoneCollection::instance().elementAt(pc.getCurZoneNum()).getEndRoomNum();
            for (int i = start; i<= end; i++) {
               if (room_list.elementAtNoCreate(i)) {
                  Cell<object*> ocll(room_list[i].inv);
                  object* optr;
                  while ((optr = ocll.next())) {
                     Sprintf(buf, "[Room %i] %P11[%i]%P20%S.\n",
                             i, optr->getIdNum(), optr->getLongName());
                     pc.show(buf);
                  }//while
               }//if
            }//for
            return;
         }//if
      }//if immort

      ptr = have_crit_named(pc_list, i_th, name, pc.SEE_BIT, ROOM);
      if (ptr) {
         if ((pc.LEVEL + 3 > ptr->LEVEL) &&
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
}//where


void roll(int i, int j, critter& pc) {
   String buf(100);
   Sprintf(buf, "You roll of %i dice of %i side(s) is:  %i\n",
           i, j, d(i, j));
   pc.show(buf);
}


void uptime(critter& pc) {
   if (!pc.pc || !pc.pc->imm_data) {
      pc.show("Eh??");
      return;
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
}//uptime


void zgoto(int i_th, critter& pc) {
   int rm_num;

   if (!pc.pc || !pc.pc->imm_data) {
      show(PARSE_ERR_MSG, pc);
      return;
   }//if

   if (pc.pc && pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (!IsEmpty(pc.IS_FIGHTING)) {
      show("You can't zgoto while fighting!\n", pc);
      return;
   }//if

   if (!check_l_range(i_th, 0, NUMBER_OF_ZONES, pc, TRUE)) {
      return;
   }//if

   if (!ZoneCollection::instance().elementAt(i_th).isInUse()) {
      show("That zone has not been defined yet.\n", pc);
      return;
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
      emote("dissapears with a popping sound!\n", pc, ROOM, FALSE);
   }

   int is_dead;
   pc.doGoToRoom(rm_num, NULL, NULL, is_dead, pc.getCurRoomNum());

   if (pc.shouldDoPoofin()) {
      ROOM.doPoofIn(pc);
   }
   else {
      emote("appears suddenly.\n", pc, ROOM, FALSE);
   }
   //look(1, &NULL_STRING, pc);
}//goto zone



void rclear(int i_th, critter& pc) {
   int rm_num;
   mudlog.log(TRC, "In rclear.\n");

   if (!pc.isImmort()) {
      show(PARSE_ERR_MSG, pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (i_th == 1) { //do current room
      rm_num = pc.getCurRoomNum();
   }//if
   else {
      if (!check_l_range(i_th, 0, NUMBER_OF_ROOMS, pc, TRUE)) {
         return;
      }//if
      rm_num = i_th;
   }//else
      
   if (!pc.doesOwnRoom(room_list[rm_num])) {
      show("You don't have permission to edit this room.\n", pc);
      return;
   }//if

   int new_rm_num;
   if (pc.getCurRoomNum() != LOGIN_ROOM) {
      new_rm_num = LOGIN_ROOM;
   }//if
   else {
      new_rm_num = LOGIN_ROOM + 1;
   }//else

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Rclear, new_rm_num:  " << new_rm_num 
             << " rm_num:  " << rm_num << "  pc:  " << *(pc.getName())
             << " i_th:  " << i_th << endl;
   }

   room_list[rm_num].doRclear(new_rm_num);
   room_list.setAndDestroy(NULL, rm_num);
}// rclear


void oclear(int i_th, critter& pc) {

   mudlog.log(TRC, "In oclear.\n");

   if (!pc.isImmort()) {
      show(PARSE_ERR_MSG, pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (!check_l_range(i_th, 2, NUMBER_OF_ITEMS, pc, TRUE)) {
      return;
   }//if
      
   if (!pc.doesOwnObject(obj_list[i_th])) {
      show("You don't have permission to edit this object.\n", pc);
      return;
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

}// oclear


void mclear(int i_th, critter& pc) {

   mudlog.log(TRC, "In mclear.\n");

   if (!pc.pc || !pc.pc->imm_data) {
      show(PARSE_ERR_MSG, pc);
      return;
   }//if

   if (pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (!check_l_range(i_th, 2, NUMBER_OF_MOBS, pc, TRUE)) {
      return;
   }//if
      
   if (!pc.doesOwnCritter(mob_list[i_th])) {
      show("You don't have permission to edit this mob.\n", pc);
      return;
   }//if

   show("Ok, the mob was deleted from the online DB.  In order to make\n",pc);
   show("these changes permanent, you will have to do an 'amsave'.\n", pc);

   for (int i = 0; i < NUMBER_OF_ROOMS; i++) {
      room_list[i].purgeCritter(i_th, pc);
   }//for

   mob_list[i_th].Clear();  //clear out the mob


}// mclear


void dclear(int i_th, critter& pc) {

   mudlog.log(TRC, "In dclear.\n");

   if (!pc.pc || !pc.pc->imm_data) {
      show(PARSE_ERR_MSG, pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (!check_l_range(i_th, 2, NUMBER_OF_DOORS, pc, TRUE)) {
      return;
   }//if
      
   if (!pc.doesOwnDoor(door_list[i_th])) {
      show("You don't have permission to edit this door.\n", pc);
      return;
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
}// dclear


void brief(critter& pc) {
   String buf("brief");
   toggle_prompt(&buf, pc);
}//brief


/* helper function, not a command */
int num_of_generic_door(const String* direction) {
   String* tmp_name;
   for (int i = 1; i <= 10; i++) {
      tmp_name = Top(door_list[i].names);
      if (strcasecmp(*direction, *tmp_name) == 0)
         return i;
   }//for
   return -1;
}//num_of_generic_door


void flip_door(int i_th, const String* name, critter& pc) {
   if (!pc.pc)
      return;
   if (!pc.PC_FLAGS.get(11)) {
      show("Huh??\n", pc);
      return;
   }//if

   if (pc.pc && pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (!ROOM.isZlocked()) {
      show("You cannot edit a room that is not locked.\n", pc);
      return;
   }//if

   if (!pc.doesOwnRoom(ROOM)) {
      show("You don't have permission to edit this room.\n", pc);
      return;
   }//if

   door* dr_ptr = door::findDoor(ROOM.DOORS, i_th, name, pc.SEE_BIT, ROOM);
   if (!dr_ptr) {
      show("That door doesn't seem to exist.\n", pc);
      return;
   }//if

   if (!dr_ptr->dr_data->door_data_flags.get(9)) {
      show("That door is flagged !FLIP.\n", pc);
      return;
   }//if

   dr_ptr->destination = (-(dr_ptr->destination));
   show("The door has been flipped.\n", pc);

}//flip_door


int door_to(int room_num, int dist, const String* direction, critter& pc) {
   String buf(100);

   if (!pc.pc)
      return FALSE;

   if (!pc.isImmort()) {
      show("Huh??\n", pc);
      return FALSE;
   }//if

   if (mudlog.ofLevel(DBG)) {
      String buf(100);
      Sprintf(buf, "DEBUG:  door_to:  Room_num: %i dist: %i dir -:%S:-\n",
              room_num, dist, direction);
      pc.show(buf);
   }

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return FALSE;
   }//if
   
   if (dist == 1) {
      show("NOTE:  door_to has been changed to include distance.\n", pc);
      show("READ THE HELP IF YOU HAVEN'T ALREADY.\n", pc);
   }//if one, odds are its a default value, ie not on purpose

   if (!check_l_range(dist, 0, 25, pc, FALSE)) {
      show("Distance is out of range, must be between 0 and 25.\n", pc);
      return FALSE;
   }//if

   if (room_num <= 1) {
      Sprintf(buf, "You cannot build a door to that room ( room# <= 1): %i\n",
              room_num);
      pc.show(buf);
      return FALSE;
   }//if

   if (room_num > NUMBER_OF_ROOMS) {
      show("Your stated room number is too high.\n", pc);
      return FALSE;
   }//if

   String op_dir(get_opposite_dir(*direction));
   do_door_to(ROOM, room_list[room_num], dist, pc, direction);
   do_door_to(room_list[room_num], ROOM, dist, pc, &op_dir);
   return TRUE;
}//door_to


void do_door_to(room& cur_room, room& targ_room, int distance, critter&
		pc, const String* direction, int is_gate = FALSE) {
   String buf(100);
   int i;

   if (!is_gate) {
      if (!cur_room.isZlocked()) {
         show("You cannot edit a room that is not locked.\n", pc);
         return;
      }//if
      if (!pc.doesOwnRoom(cur_room)) {
         show("You don't have permission to edit this room.\n", pc);
         return;
     }//if

     i = num_of_generic_door(direction);
   
     if (i < 0) {
       show("In what direction should your door to exit?\n", pc);
       return;
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
      affected_rooms.gainData(&cur_room);
   }//if
   Put(new_door, cur_room.doors);
   if (!is_gate)
     show("OK, doors added (its reflexive now)\n", pc);
}//do_door_to


void replace_door(int nd_num, const String* direction, critter& pc) {
   if (!pc.pc)
      return;
   if (!pc.PC_FLAGS.get(11)) {
      show("Huh??\n", pc);
      return;
   }//if

   if (pc.pc && pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (!ROOM.isZlocked()) {
      show("You cannot edit a room that is not locked.\n", pc);
      return;
   }//if
   if (!check_l_range(nd_num, 0, NUMBER_OF_DOORS, pc, TRUE))
      return;
   if (IsEmpty(door_list[nd_num].names)) {
      show("That door doesn't exist yet.\n", pc);
      return;
   }//if
   if (!pc.doesOwnRoom(ROOM)) {
      show("You don't have permission to edit this room.\n", pc);
      return;
   }//if
   
   door* dr_ptr = door::findDoor(ROOM.DOORS, 1, direction, ~0, ROOM);
   if (!dr_ptr) {
      show("That door doesn't seem to exist.\n", pc);
      return;
   }//if

   dr_ptr->dr_data = &(door_list[nd_num]); //essentially add new door
}//replace_door


void delete_door(int i_th, const String* direction, critter& pc) {
   if (!pc.pc)
      return;
   if (!pc.PC_FLAGS.get(11)) {
      show("Huh??\n", pc);
      return;
   }//if

   if (pc.pc && pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (!ROOM.isZlocked()) {
      show("You cannot edit a room that is not locked.\n", pc);
      return;
   }//if
   if (!pc.doesOwnRoom(ROOM)) {
      show("You don't have permission to edit this room.\n", pc);
      return;
   }//if

   door* dr_ptr = 
         door::findDoor(ROOM.DOORS, i_th, direction, pc.SEE_BIT, ROOM);
   if (!dr_ptr) {
      show("That door doesn't seem to exist.\n", pc);
      return;
   }//if

   ROOM.DOORS.loseData(dr_ptr);
   delete dr_ptr; //only a shallow delete

   show("Ok, door has been removed.\n", pc);
}//delete_door


void write_zone(int znum, critter& pc) {

   String buf(100);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In write zone, znum:  " << znum << endl;
   }

   if (!pc.isImmort()) {
      show("Huh??\n", pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (znum == 1) {
     znum = ROOM.getZoneNum();
   }//if

   if (!check_l_range(znum, 0, NUMBER_OF_ZONES, pc, TRUE)) {
      return;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).isLocked()) { //locked from mortals, mob_procs
      show("WARNING:  You cannot write a zone that is not locked.\n", pc);
      return;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).isTotallyLoaded()) {
      show("WARNING:  You cannot write a zone that isn't totally_loaded.\n", 
            pc);
      return;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).isOwnedBy(pc)) {
      show("You don't have permission to edit this zone.\n", pc);
      return;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).canWriteRooms()) { //if locked (no write, checked out)
      show("WARNING:  All builders of your zone must finish their projects\n",
           pc);
      show("before you can write it to disk.\n", pc);
      return;
   }//if

   do_write_zone(znum);

   show("Your zone has been written.\n", pc);
}//write_zone


void do_write_zone(int znum) {
  String buf(100);
  int j;

  Sprintf(buf, "./World/zone_%i", znum);
  ofstream rfile(buf);
  if (!rfile) {
     mudlog.log(ERR, "ERROR:  rfile not opened in 'write_zone'.\n");
     return;
  }//if

  for (j = ZoneCollection::instance().elementAt(znum).getBeginRoomNum();
       j <= ZoneCollection::instance().elementAt(znum).getEndRoomNum();
       j++) {
    if (!IsEmpty(room_list[j].names)) {
       if (room_list[j].isVehicle()) {
          rfile << (j | 0x01000000) << "\t\tVehicle number\n";
       }
       else {
          rfile << j << "\t\tRoom Number\n";
       }
       room_list[j].Write(rfile);
    }//if
  }//for
  rfile << "\n\n" << -1 << "\t\tEND OF FILE MARKER\n" << flush;
}//do_write_zone


void zlock(int znum, critter& pc) {

   if (!pc.pc)
      return;

   if (!pc.PC_FLAGS.get(11)) {
      show("You can't do that!\n", pc);
      return;
   }//if

   if (pc.pc && pc.PC_FLAGS.get(0)) { //if frozen
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (znum == 1) {
      znum = ROOM.getZoneNum();
   }//if

   if (!check_l_range(znum, 0, NUMBER_OF_ZONES, pc, TRUE)) {
      return;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).isOwnedBy(pc)) {
      show("You don't have permission to edit this zone.\n", pc);
      return;
   }//if

   ZoneCollection::instance().zlock(znum);
   show("Your present zone is now locked.\n", pc);
}//zlock



void zunlock(int znum, critter& pc) {

   if (!pc.pc)
      return;

   if (!pc.isImmort()) {
      show("You can't do that!\n", pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if
   
   if (znum == 1) { //default is zone pc is in
     znum = ROOM.getZoneNum(); 
   }//if

   if (!check_l_range(znum, 0, NUMBER_OF_ZONES, pc, TRUE)) {
      return;
   }//if

   if (!ZoneCollection::instance().elementAt(znum).isOwnedBy(pc)) {
      show("You don't have permission to edit this zone.\n", pc);
      return;
   }//if

   ZoneCollection::instance().zunlock(znum);
   show("Your zone is now unlocked.\n", pc);
}//zunlock


/* helper function */
void do_lock_room(room& rm) {
   rm.lock();
}//do_lock_room


/* helper function */
void do_unlock_room(room& rm) {
   rm.unlock();
}//do_lock_room


/* set the players prompt */
void prompt(const String* pr, critter& pc) {
   if (!pc.pc)
      return;
   
   if (*pr == "NORMAL") //reset to default
      pc.pc->prompt = "%N< %hH %mM %vV >  ";
   else
      pc.pc->prompt = *pr;
   show("Ok.\n", pc);
}//prompt


void give(int i_th, const String* obj, int j_th, const String* target,
               critter& pc) {
   String buf(100);
   critter* targ;
   object* item;

   if (pc.isMob()) {
      mudlog.log(ERR, "ERROR:  pc is MOB in give.\n");
      return;
   }//if

   if (pc.POS >= POS_SLEEP) {
      show("You are too relaxed.\n", pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (pc.isParalyzed()) {
      show("You can't move a muscle.\n", pc);
      return;
   }//if
   
   item = have_obj_named(pc.inv, i_th, obj, pc.SEE_BIT, ROOM);
   if (!item) {
      show("You don't have that object.\n", pc);
      return;
   }//if

   targ = ROOM.haveCritNamed(j_th, target, pc.SEE_BIT);
   if (!targ) {
      show("You don't see that person here.\n", pc);
      return;
   }//if

   if (targ == &pc) {
      show("Nothing much seems to change!!\n", pc);
      return;
   }//if

   if (targ->isMob())
      targ = mob_to_smob(*targ, pc.getCurRoomNum(), TRUE, j_th, target, pc.SEE_BIT);

   		/* assume have valid item and targ */

   do_give(*targ, pc, *item);

}//give()


void do_give(critter& targ, critter& pc, object& obj) {
   String buf(100);

   if (!source_give_to(pc, obj, targ)) {
      return;
   }//if

   if (!obj_drop_by(obj, pc)) {
      return;
   }//if

   if (obj.IN_LIST)
      obj.IN_LIST = &(targ.inv);

   targ.gainInv(&obj);

   /* this may fail silently with some mob procs, however, it should be
      harmless. */
   pc.loseInv(&obj); 

   drop_eq_effects(obj, pc, FALSE); //don't do msgs
   gain_eq_effects(obj, obj_list[0], targ, -1, FALSE); //don't do msgs


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

   Sprintf(buf, "You give %S to %S.\n", &(obj.ob->short_desc),
        name_of_crit(targ, pc.SEE_BIT));
   show(buf, pc);

   Sprintf(buf, "%S gives you %S.\n", name_of_crit(pc, targ.SEE_BIT),
           long_name_of_obj(obj, targ.SEE_BIT));
   buf.Cap();
   show(buf, targ);

   do_domob_give_proc(targ, pc, obj); //this can recurse but twice I believe

   String cmd = "give";
   String obj_num;
   obj_num = obj.OBJ_NUM;
   ROOM.checkForProc(cmd, obj_num, pc, targ.MOB_NUM);

}//do_give()


void olist(int start, int end, critter& pc) {
   String buf(100);

   if (!pc.pc || !pc.pc->imm_data) {
     show("Eh??\n", pc);
     return;
   }//if

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
      return;
   }//if

   for (int i = start; i<= end; i++) {
      if (obj_list[i].OBJ_FLAGS.get(10)) {
         Sprintf(buf, "\t%i\t%S\n", i, &(obj_list[i].ob->short_desc));
         show(buf, pc);
      }//if
      else {
         Sprintf(buf, "\t%i\tTHIS OBJECT UNDEFINED.\n", i);
         show(buf, pc);
      }//else
   }//for 
}//olist


void zlist(int start, int end, critter& pc) {
   if (!pc.isImmort()) {
     show("Eh??\n", pc);
     return;
   }//if

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
      return;
   }//if

   ZoneCollection::instance().zlist(pc, start, end);
}//zlist


void mlist(int start, int end, critter& pc) {
   String buf(100);

   if (!pc.isImmort()) {
     show("Eh??\n", pc);
     return;
   }//if

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
      return;
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
}//mlist


void rlist(int start, int end, critter& pc) {
   String buf(100);

   if (!pc.pc || !pc.pc->imm_data) {
     show("Eh??\n", pc);
     return;
   }//if

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
      return;
   }//if

   for (int i = start; i<= end; i++) {
      if (!IsEmpty(room_list[i].names)) {
         Sprintf(buf, "\t%i\t%S\n", i, &(room_list[i].short_desc));
         show(buf, pc);
      }//if
      else {
         Sprintf(buf, "\t%i\tTHIS ROOM UNDEFINED.\n", i);
         show(buf, pc);
      }//else
   }//for
}//rlist


void dlist(int start, int end, critter& pc) {
   String buf(100);

   if (!pc.pc || !pc.pc->imm_data) {
     show("Eh??\n", pc);
     return;
   }//if

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
      return;
   }//if

   for (int i = start; i<= end; i++) {
      if (!IsEmpty(door_list[i].names)) {
         Sprintf(buf, "\t%i\t%S\n", i, &(door_list[i].long_desc));
         show(buf, pc);
      }//if
      else {
         Sprintf(buf, "\t%i\tTHIS DOOR UNDEFINED.\n", i);
         show(buf, pc);
      }//else
   }//for
}//dlist


void adlist(critter& pc) {
   String buf(100);

   if (!pc.pc || !pc.pc->imm_data) {
     show("Eh??\n", pc);
     return;
   }//if

   int zone_num = ROOM.getZoneNum();

   for (int i = 0; i<NUMBER_OF_DOORS; i++) {
      if (door_list[i].isInUse() && door_list[i].isInZone(zone_num)) {
         Sprintf(buf, "       %i%P14%S\n", i, &(door_list[i].long_desc));
         show(buf, pc);
      }//if
   }//for
}//adlist


void tog_rflag(int flagnum, critter& pc) {
   String buf(50);

   if (!pc.isImmort()) {
     show("Eh??\n", pc);
     return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (!pc.doesOwnRoom(ROOM)) {
      show("You don't have permission to edit this zone.\n", pc);
      return;
   }//if

   if (!check_l_range(flagnum, 0, 200, pc, TRUE)) {
      return;
   }//if

   if (flagnum == 23) {
      pc.show("You can't toggle that flag, try rclone or rclear.\n");
   }
   else {
      Sprintf(buf, "Toggling flag#:  %i.\n", flagnum);
      show(buf, pc);
      ROOM.flipFlag(flagnum);
   }
}//tog_rflag


void tog_zflag(int flagnum, critter& pc) {
   String buf(50);

   if (!pc.isImmort()) {
      show("Eh??\n", pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (!pc.doesOwnRoom(ROOM)) {
      show("You don't have permission to edit this zone.\n", pc);
      return;
   }//if

   if (!check_l_range(flagnum, 0, 200, pc, TRUE)) {
      return;
   }//if

   if (flagnum == 23) {
      pc.show("You can't toggle that flag, try rclone or rclear.\n");
   }
   else {
      int start = ZoneCollection::instance().elementAt(pc.getCurRoom()->getZoneNum()).getBeginRoomNum();
      int end = ZoneCollection::instance().elementAt(pc.getCurRoom()->getZoneNum()).getEndRoomNum();
      Sprintf(buf, "Toggling flag#:  %i in rooms %i through %i.\n",
              flagnum, start, end);
      show(buf, pc);
      for (int i = start; i <= end; i++) {
         if (room_list.elementAtNoCreate(i)) {
            room_list[i].flipFlag(flagnum);
         }//if
      }//for
   }//else
}//tog_zflag


void rset(const String* targ, int new_val, critter& pc) {
   String buf(50);

   if (!pc.isImmort()) {
     show("Eh??\n", pc);
     return;
   }//if

   if (!pc.doesOwnRoom(ROOM)) {
      show("You don't have permission to edit this zone.\n", pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   int len1;

   if ((len1 = targ->Strlen()) == 0) {
      show("You may set any of these, range is in parentheses:\n", pc);
      show("movement (-5, 50)\n", pc);
      show("vis_bit (0, 2 billion), NOTE:  better know what yer doing!\n", pc);
      return;
   }//if

   if (strncasecmp(*targ, "movement", len1) == 0) {
     if (check_l_range(new_val, -5, 50, pc, TRUE))
        ROOM.setMovCost(new_val);
   }//if
   else if (strncasecmp(*targ, "vis_bit", len1) == 0) {
     if (check_l_range(new_val, 0, 2000000000, pc, TRUE))
        ROOM.setVisBit(new_val);
   }//if
   else {
      show("You can't set that, see help.\n", pc);
   }//else 
}//rset


/* vict is the name of the mob you wish to mset, targ is thing to set */
void mset(int i_th, const String* vict, const String* targ, int new_val,
          const String* new_val_string, critter& pc) {
   String buf(50);

   if (!pc.isImmort()) {
      show("Eh??\n", pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (vict->Strlen() == 0) {
      show("NOTE:  some of these may be restricted from you.\n", pc);
      show("These are your choices, range is in parentheses:\n\n", pc);
      show("Usage:  mset <<i_th> [mob] [ting_to_set] [new_value]\n", pc);
      show("If the new value is a string with spaces, be sure to\n", pc);
      show("put it in single quotes.\n", pc);

      Sprintf(buf, "skin_num, for MOBS (0-%i)\n", NUMBER_OF_ITEMS);
      show(buf, pc);
      show("open_time [shop keepers] (0, 23)\n", pc);
      show("close_time [shop keepers] (0, 23)\n", pc);
      show("mob_name (keyword)          short_desc\n", pc);
      show("in_room_desc                naked_weight (20, 20000)\n", pc);
      show("sex (0-female, 1-male, 2-neuter)\n", pc);
      show("position (0, 6)             align (-1000 1000)\n", pc);
      show("hunger (-1, 9999)           thirst (-1, 9999)\n", pc);
      show("quest_points (0, 1000)      gold (0, 50 million)\n", pc);
      show("experience (0, 100 million) bank_gold (0, 50 million)\n", pc);
      show("strength (0, 100)           inteligence (0, 100)\n", pc);
      show("constitution (0, 100)       charisma (0, 100)\n", pc);
      show("wisdom (0, 100)             dexterity (0, 100)\n", pc);
      show("hit (-100, 100)             dam (0, 100)\n", pc);
      show("ac (-200, 200)              attacks (1, 4)\n", pc);
      show("class (100, 108)            race (0, 17)\n", pc);
      show("level (0, 40)               imm_level (0, 10)\n", pc);
      show("practices (0, 100)          mana (0, 10000)\n", pc);
      show("mov (0, 10000)              hp (0, 10000)\n", pc);
      show("hp_max (0, 10000)           mana_max (0, 10000)\n", pc);
      show("mov_max (0, 10000)          religion (0, 6)\n", pc);
      show("guild (0, 10)               vis_bit (0, 2 billion)\n", pc);
      show("see_bit (0, 2 billion)      max_to_load, for MOBS (0-200)\n", pc);
      show("heat_resis (-100, 200)      cold_resis (-100, 200)\n", pc);
      show("elect_resis (-100, 200)     spell_resis (-100, 200)\n", pc);
      show("bhd_count (0, 20)           bhd_sides (0, 100)\n", pc);
      show("hp_regen (-50, 500)         mana_regen(-50, 500)\n", pc);
      show("dam_giv_mob (1, 1000)       dam_rcv_mod(1, 1000)\n", pc);
      show("mov_regen (-50, 500)        exp_worth(0, millions)\n", pc);
      show("bad_assedness (-10, 10)     skill_violence (-10, 10)\n", pc);
      show("defensiveness (-10, 10)     social_awareness (-10, 10)\n", pc);
      show("benevolence (-10, 10)       pause (0, 50)\n", pc);
      show("manager\n", pc);
      return;
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
      return;
   }//if

   if (!ptr->isInUse()) {
      pc.show("That mob has not been created yet.\n");
      return;
   }

   if (!ptr->pc) {
      if (!pc.doesOwnCritter(*ptr)) {
	show("You don't have permission to edit this mob.\n", pc);
	return;
      }//if
   }//if
   else {
      if (ptr->isImmort() && (ptr->IMM_LEVEL >= pc.IMM_LEVEL)
	  && (ptr != &pc)) {
         show("You can't mess with one so powerful.\n", pc);
	 return;
      }//if
   }//else

   short flag = FALSE; //did it work

   int len1 = targ->Strlen();
   if (len1 == 0) {
      show("Which attribute do you wish to set?\n", pc);
      return;
   }//if

   if (ptr->pc && pc.IMM_LEVEL == 10) { //most restricted ones
      if (strncasecmp(*targ, "imm_level", len1) == 0) {
         if (check_l_range(new_val, 0, 10, pc, TRUE)) {
            if (ptr->pc->imm_data) {
               ptr->setImmLevel(new_val);
               show("Imm level changed.\n", pc);
               return;
            }//
         }//if
      }//if
   }//if
   
   if ((ptr->pc) && (pc.IMM_LEVEL < 9) && (ptr != &pc)) {
	 //only 9+ imms can 'set' pcs
      show("You must be an imm of level 9 or more to 'mset' players.\n", pc);
      return;
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
      else if (strncasecmp(*targ, "manager", len1) == 0) {
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
   }//if pc only


   /* go check the rest, can be for pc OR mob, no more restrictions */

   if (strncasecmp(*targ, "max_to_load", len1) == 0) {
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
     if (check_l_range(new_val, 20, 20000, pc, TRUE)) {
        ptr->setNakedWeight(new_val);
        flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mob_name", len1) == 0) {
      if (!ptr->isPc()) {
         clear_ptr_list(ptr->names);
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
     if (check_l_range(new_val, 0, 6, pc, TRUE)) {
       ptr->POS = new_val;
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
     if (check_l_range(new_val, 0, 500000, pc, TRUE)) {
       ptr->EXP = new_val * EXP_DIVISOR;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "pause", len1) == 0) {
     if (check_l_range(new_val, 0, 50, pc, TRUE)) {
       ptr->setPause(new_val);
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "align", len1) == 0) {
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
   else if (strncasecmp(*targ, "dam_rcv_mod", max(len1, 5)) == 0) {
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
   else if (strncasecmp(*targ, "inteligence", len1) == 0) {
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
     if (check_l_range(new_val, 1, 8, pc, TRUE)) {
       ptr->CLASS = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "race", len1) == 0) {
     if (check_l_range(new_val, 1, 17, pc, TRUE)) {
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
     if (check_l_range(new_val, 0, 10000, pc, TRUE)) {
       ptr->HP = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mana", len1) == 0) {
     if (check_l_range(new_val, 0, 10000, pc, TRUE)) {
       ptr->MANA = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mov", len1) == 0) {
     if (check_l_range(new_val, 0, 10000, pc, TRUE)) {
       ptr->MOV = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "hp_max", max(len1, 4)) == 0) {
     if (check_l_range(new_val, 0, 10000, pc, TRUE)) {
       ptr->setHP_MAX(new_val);
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mana_max", max(len1, 5)) == 0) {
     if (check_l_range(new_val, 0, 10000, pc, TRUE)) {
       ptr->MA_MAX = new_val;
       flag = TRUE;
     }//if
   }//if
   else if (strncasecmp(*targ, "mov_max", max(len1, 5)) == 0) {
     if (check_l_range(new_val, 0, 10000, pc, TRUE)) {
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
   }//if
   else {
     show("Oops, for some reason it failed, remember that not everyone\n", pc);
     show("can do an mset on everyone else, some things are restricted.\n",
	  pc);
   }//else
}//mset


void oset(int i_th, const String* vict, const String* targ, int new_val,
	  const String* new_val_string, critter& pc) {
   String buf(50);

   if (!pc.isImmort()) {
      show("Eh??\n", pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

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
      show("obj_name (ie keyword)       short_desc", pc);
      show("in_room_desc", pc);
      return;
   }//if

   object* ptr = have_obj_named(pc.inv, i_th, vict, pc.SEE_BIT,
				ROOM);
   if (!ptr) {
      ptr = have_obj_named(ROOM.inv, i_th, vict, pc.SEE_BIT,
			   ROOM);      
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
      return;
   }//if

   if (!pc.doesOwnObject(*ptr)) {
     show("You don't have permission to edit this object.\n", pc);
     return;
   }//if

   int len1 = targ->Strlen();

   if (len1 == 0) {
     show("Which attribute do you wish to change??\n", pc);
     return;
   }//if

   short flag = FALSE; //did it work

   if (ptr->ob->bag) { //thingies for bags
      if (strncasecmp(*targ, "key_num", len1) == 0) {
	if (check_l_range(new_val, 0, NUMBER_OF_ITEMS, pc, TRUE)) {
	  ptr->ob->bag->key_num = new_val;
	  flag = TRUE;
	}//if
      }//if
      else if (strncasecmp(*targ, "max_weight", len1) == 0) {
	if (check_l_range(new_val, 0, 9999, pc, TRUE)) {
	  ptr->ob->bag->max_weight = new_val;
          flag = TRUE;
	}//if
      }//if
      else if (strncasecmp(*targ, "percentage_weight", len1) == 0) {
	if (check_l_range(new_val, 1, 1000, pc, TRUE)) {
	  ptr->ob->bag->percentage_weight = new_val;
	  flag = TRUE;
	}//if
      }//if
      else if (strncasecmp(*targ, "time_till_disolve", len1) == 0) {
	if (check_l_range(new_val, 0, 500, pc, TRUE)) {
	  ptr->ob->bag->time_till_disolve = new_val;
	  flag = TRUE;
	}//if
      }//if
   }//bag only

   if (strncasecmp(*targ, "charges", len1) == 0) {
      if (check_l_range(new_val, 0, 100, pc, TRUE)) {
         ptr->ob->extras[0] = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "obj_name", len1) == 0) {
      clear_ptr_list(ptr->ob->names);
      ptr->ob->names.append(new String(*new_val_string));
      flag = TRUE;
   }//if
   else if (strncasecmp(*targ, "short_desc", len1) == 0) {
      ptr->ob->short_desc = *new_val_string;
      flag = TRUE;
   }//if
   else if (strncasecmp(*targ, "in_room_desc", len1) == 0) {
      ptr->ob->in_room_desc = *new_val_string;
      flag = TRUE;
   }//if
   else if (strncasecmp(*targ, "percent_load", len1) == 0) {
      if (check_l_range(new_val, 0, 100, pc, TRUE)) {
         ptr->ob->extras[2] = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "weight", len1) == 0) {
      if (check_l_range(new_val, 0, 10000, pc, TRUE)) {
         ptr->ob->extras[5] = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "max_in_game", len1) == 0) {
      if (check_l_range(new_val, 0, 1000, pc, TRUE)) {
         ptr->ob->extras[4] = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "dice_sides", len1) == 0) {
      if (check_l_range(new_val, 0, 50, pc, TRUE)) {
         ptr->ob->extras[6] = new_val;
         flag = TRUE;
      }//if
   }//if
   else if (strncasecmp(*targ, "dice_count", len1) == 0) {
      if (check_l_range(new_val, 0, 50, pc, TRUE)) {
         ptr->ob->extras[7] = new_val;
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
   }//if
   else {
     show("Value not set.\n", pc);
   }//else
}//oset



void dset(int i_th, const String* vict, const String* targ, int new_val,
	  critter& pc) {
   String buf(50);

   if (!pc.pc || !pc.pc->imm_data) {
     show("Eh??\n", pc);
     return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (vict->Strlen() == 0) {
      show("NOTE:  some of these may be restricted from you.\n", pc);
      show("These are your choices, range is in parentheses:\n", pc);
      show("toggle_flag:

0 open exit, basically no door, 1 is_mag_lockable, 2 is_closed, 3 is_locked,
4 is_pickable, 5 is_lockable, 6 mag_locked (spell only can open it),
7 is_destructable, 8 is_closeable, 9 is_flippable, 
11 is_unopenable (other than normal reasons)
12 is_vehicle_exit, 13 is_secret, 14 is_blocked, 16 hidden_when_open.\n\n", pc);

      show("vis_bit (web page has definitions)\n", pc);
      show("key_num (0, MAX_ITEM_NUM)\n", pc);
      show("token_num (0, MAX_ITEM_NUM)\n", pc);
      show("distance(0, 10)\n", pc);
      return;
   }//if

   door* ptr = door::findDoor(ROOM.DOORS, i_th, vict, pc.SEE_BIT, ROOM);
   if (!ptr) {
      show("You don't see that door.\n", pc);
      return;
   }//if


   int len1 = targ->Strlen();

   if (len1 == 0) {
     show("Which attribute do you wish to change??\n", pc);
     return;
   }//if

   short flag = FALSE; //did it work

   if (!pc.doesOwnRoom(ROOM)) {
     show("You have to own this zone to change a door.\n", pc);
     return;
   }//if

   if (strncasecmp(*targ, "distance", len1) == 0) {
     if (check_l_range(new_val, 0, 15, pc, TRUE)) {
       ptr->distance = new_val;
       flag = TRUE;
     }//if
   }//if

   if (flag) {
     show("Ok.\n", pc);
     return;
   }//if


   if (!pc.doesOwnDoor(*ptr)) {
     show("You don't have permission to edit this door.\n", pc);
     return;
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
   }//if
   else {
     show("Value not set for some reason.\n", pc);
   }//else
}//dset


void tog_oflag(int flagnum, const String* flag_type,
               int i_th, const String* obj, critter& pc) {
   String buf(50);

   if (!pc.isImmort()) {
      show("Eh??\n", pc);
      return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (!check_l_range(flagnum, 0, 200, pc, TRUE)) {
      return;
   }//if

   object* obj_ptr = NULL;

   if (obj->Strlen() > 0) {
      obj_ptr = have_obj_named(pc.inv, i_th, obj, pc.SEE_BIT,
                               ROOM);
      if (!obj_ptr) {
         obj_ptr = have_obj_named(ROOM.inv, i_th, obj, pc.SEE_BIT,
                                  ROOM);
      }
   }//if
   else { 
      if (check_l_range(i_th, 2, NUMBER_OF_ITEMS, pc, TRUE))
        return;
      
      if (!obj_list[i_th].isInUse()) {
         pc.show("That object does not exist.\n");
         return;
      }

      obj_ptr = &(obj_list[i_th]);
   }
        
      
   if (!obj_ptr) {
      pc.show("You don't see that object.\n");
      return;
   }//if

   if (!pc.doesOwnObject(*obj_ptr)) {
      show("You don't have permission to edit this object.\n", pc);
      return;
   }//if

   if ((strncasecmp(*flag_type, "obj_flag", 1) == 0) ||
       (strncasecmp(*flag_type, "bag_flag", 1) == 0)) {
      if (obj_ptr->IN_LIST) {
         Sprintf(buf, "Toggling obj_flag#:  %i on SOBJ:  %S.\n", flagnum, 
                 &(obj_ptr->ob->short_desc));
         show(buf, pc);
      }//if SOBJ
      else {
         Sprintf(buf, "Toggling obj_flag#:  %i on OBJ:  %S.\n", flagnum, 
                 &(obj_ptr->ob->short_desc));
         show(buf, pc);
      }//else
   }//if
   else {
      show("You must specify flagtype as:  'obj_flag' or 'bag_flag'.\n",
           pc);
      show("See the help page.\n", pc);
      return;
   }//else

   if (strncasecmp(*flag_type, "obj_flag", 1) == 0) {
      if (flagnum != 8 && flagnum != 9 && flagnum != 10 && flagnum != 63
          && flagnum != 70 && flagnum != 73 && flagnum != 74) {
         obj_ptr->OBJ_FLAGS.flip(flagnum);
      }//if
      else {
         show("OOPS, you can't toggle that obj_flag.\n", pc);
      }//else
   }//if
   else { //bag flags
      if (!obj_ptr->ob->bag) {
         show("Doh, thats not a bag!!\n", pc);
         return;
      }//if
      if (flagnum != 8) {
         obj_ptr->ob->bag->bag_flags.flip(flagnum);
      }//if
      else {
         show("OOPS, you can't toggle that bag_flag.\n", pc);
      }//else
   }//else
}// tog_oflag
   

void tog_mflag(int flagnum, const String* flag_type, 
               int i_th, const String* mob, critter& pc) {
   String buf(50);

   if (!pc.isImmort()) {
     show("Eh??\n", pc);
     return;
   }//if

   if (pc.isFrozen()) {
      show("You are too frozen to do anything.\n", pc);
      return;
   }//if

   if (!check_l_range(flagnum, 0, 200, pc, TRUE)) {
      return;
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
      return;
   }//if

   if (!mob_ptr->isInUse()) {
      pc.show("That mob has not been created yet.\n");
      return;
   }


   if (!mob_ptr->pc && !pc.doesOwnCritter(*mob_ptr)) {
      show("You don't have permission to edit this mobile.\n", pc);
      return;
   }//if

   if (mob_ptr->pc && mob_ptr->pc->imm_data) {
     if ((mob_ptr->IMM_LEVEL >= pc.IMM_LEVEL) && (&pc != mob_ptr)) {
       show("You don't have permission to edit this player.\n", pc);
       return;
     }//if
   }//if

   if (mob_ptr->pc && (pc.IMM_LEVEL < 9)) {
      show("You can't toggle PC flags.\n", pc);
      return;
   }//if

   if ((strncasecmp(*flag_type, "mob_flag", 1) == 0) ||
       (strncasecmp(*flag_type, "crit_flag", 1) == 0)) {
      if (mob_ptr->isMob()) {
         Sprintf(buf, "Toggling flag#:  %i on MOB:  %S.\n", flagnum,
                 &(mob_ptr->short_desc));
         show(buf, pc);
      }//if SOBJ
      else {
         Sprintf(buf, "Toggling flag#:  %i on SMOB:  %S.\n", flagnum,
                 &(mob_ptr->short_desc));
         show(buf, pc);
      }//else
   }//if
   else {
      show("You must specify flagtype as:  'mob_flag' or 'crit_flag'.\n",
           pc);
      return;
   }//else
   
   if (strncasecmp(*flag_type, "mob_flag", 1) == 0) {
      if ((flagnum == 1) || (flagnum == 2) || (flagnum == 5)) {
         mob_ptr->MOB_FLAGS.flip(flagnum);
      }//if
      else {
         show("OOPS, you can't toggle that flag.\n", pc);
      }//else
   }//if
   else {
      if ((flagnum != 18) && (flagnum != 24)) { //put restrictions here
         mob_ptr->CRIT_FLAGS.flip(flagnum);
      }//if
      else {
         show("OOPS, you can't toggle that flag.\n", pc);
      }//else
   }//else 
}// tog_mflag
