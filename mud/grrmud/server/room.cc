// $Id: room.cc,v 1.42 1999/09/06 07:12:52 greear Exp $
// $Revision: 1.42 $  $Author: greear $ $Date: 1999/09/06 07:12:52 $

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

//**********************************************************************//
///************************  room  ********************************///

#include "room.h"
#include "misc.h"
#include "misc2.h"
#include "battle.h"
#include <PtrArray.h>
#include "const.h"
#include "zone.h"
#include "load_wld.h"
#include "Filters.h"
#include "Markup.h"


int KeywordPair::_cnt = 0;

void KeywordPair::toStringStat(critter* viewer, String& rslt, int idx,
                               ToStringTypeE st) {
   String buf(100);
   rslt.clear();
   
   if (viewer->isUsingClient()) {
      Sprintf(rslt, "<KEYWORD %i>", idx);
      Entity::toStringStat(viewer, rslt, st);
      rslt.append(buf);
      rslt.append("</KEYWORD>");
   }
   else {
      Sprintf(buf, "Keyword [%i]\n", idx);
      rslt.append(buf);
      Entity::toStringStat(viewer, buf, st);
      rslt.append(buf);
   }
}//toStringStat

SafeList<object*>& KeywordPair::getInv() {
   core_dump("KeywordPair::getInv");
   return dummy_inv;
}

KeywordPair::~KeywordPair() {
   _cnt--;
   clear();
}

KeywordPair::KeywordPair(KeywordPair& src) {
   _cnt++;
   *this = src;
}

int KeywordPair::getCurRoomNum() {
   if (getContainer()) {
      return getContainer()->getCurRoomNum();
   }
   if (mudlog.ofLevel(WRN)) {
      mudlog << "WARNING:  KeywordPair::getCurRoomNum failed\n";
   }
   return 0;
}//getCurRoomNum

KeywordPair& KeywordPair::operator=(KeywordPair& rhs) {
   if (&rhs != this) {
      Entity* hack = (Entity*)(&rhs);
      *((Entity*)(this)) = *hack;
   }//if

   return *this;
}//operator=


int room::_cnt = 0;

room::room() : critters(NULL), inv(NULL) {
   _cnt++;
   for (int i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = 0;
   }
   setVisBit(1024); //hack, forgot 'normal' vis bit earlier
   pause = 0;
   obj_ptr_log << "RM_CON " << getIdNum() << " " << this << "\n";
} // sub_room constructor

room::room(int rm_num) : critters(NULL), inv(NULL) {
   _cnt++;
   for (int i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = 0;
   }
   cur_stats[2] = rm_num;
   setVisBit(1024); //hack, forgot 'normal' vis bit earlier
   pause = 0;

   obj_ptr_log << "RM_CON_I " << getIdNum() << " " << this << "\n";
} // sub_room constructor


int room::makeReadyForAreaSpell() {
   SCell<critter*> cll(critters);
   critter* ptr;
   
   while ((ptr = cll.next())) {
      ptr->CRIT_FLAGS.turn_off(20);
   }
   return 0;
}

critter* room::findNextSpellCritter() {
   SCell<critter*> cll(critters);
   critter* ptr;

   while ((ptr = cll.next())) {
      if (!ptr->CRIT_FLAGS.get(20)) { //if spell_tested
         ptr->CRIT_FLAGS.turn_on(20);
         return ptr;
      }//if not affected yet
   }//while

   return NULL;
}//findNextSpellCritter


critter* room::findNextProcMob() {
   SCell<critter*> cll(critters);
   critter* ptr;
   
   while ((ptr = cll.next())) {
      if (!(ptr->pc)) {
         if (ptr->mob) {
            if (!ptr->master) {
               if (ptr->MOB_FLAGS.get(3)) {
                  ptr->MOB_FLAGS.turn_off(3);
                  return ptr;
               }//if haven't done procs yet
            }//if
         }//if a mob
      }//needed if obj has BOTH mob and pc data fields
   }//while

   return NULL;
}//findNextProcMob

KeywordPair* room::haveKeyword(int i_th, const String* name,
                               critter* viewer, int& sofar) {
   
   SCell<KeywordPair*> cell(keywords);
   KeywordPair* ptr;
   int count = 0, ptr_v_bit;
   int c_bit = ~0;
   LanguageE lang = English;
   if (viewer) {
      c_bit = viewer->getSeeBit();
      lang = viewer->getLanguage();
   }

   if (name->Strlen() == 0) 
      return NULL;

   while ((ptr = cell.next())) {
      ptr_v_bit = (ptr->getVisBit() | getVisBit());
      if (detect(c_bit, ptr_v_bit)) {
         if (ptr->isNamed(*name, lang)) {
            count++;
            sofar++;
            if (count == i_th) {
               return ptr;
            }//if
         }//if
      }//if
   }//while
   return NULL;
}


void room::recursivelyLoad() { //mobs and objects
   SCell<critter*> ccll(critters);
   critter* cptr;
   
   while ((cptr = ccll.next())) {
      recursive_init_loads(*cptr);
      cptr->incrementCurInGame(); //recursive_... no longer does it.
   }//while
   
   SCell<object*> ocll(inv);
   object* optr;
   while ((optr = ocll.next())) {
      recursive_init_loads(*optr, 0);
   }//while
}//recursivelyLoad()


room::room(room& source) : critters(NULL), inv(NULL) {
   _cnt++;
   *this = source; //overloaded = operator
   
   obj_ptr_log << "RM_CC " << getIdNum() << " " << this << "\n";
} // sub_room copy constructor


room::~room() {
   _cnt--;

   obj_ptr_log << "RM_DES " << getIdNum() << " " << this << "\n";

   clear();
}//sub_room deconstructor


room& room::operator=(room& source) {

   if (this == &source)
      return *this;

   int i;
   SCell<object*> obj_cll;
   object* obj_ptr;
   SCell<critter*> crit_cll;
   critter* crit_ptr;
   
   //mudlog.log(DBG, "In rm operator= overload.\n");
   clear(); //clear this thing out!!
   
   Entity* hack = &source;
   *((Entity*)(this)) = *hack;
   *((Scriptable*)(this)) = (Scriptable)(source);

   short_desc = source.short_desc;
   
   room_flags = source.room_flags;

   keywords.becomeDeepCopyOf(source.keywords);
   
   for (i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = source.cur_stats[i];
   }

   source.inv.head(obj_cll);
   while ((obj_ptr = obj_cll.next())) {
      if (!obj_ptr->isModified()) { //no multiple ptrs to SOBJ's
         inv.append(obj_ptr);
      }//if
   }//while

   doors.becomeDeepCopyOf(source.doors);
   
   source.critters.head(crit_cll);
   while ((crit_ptr = crit_cll.next())) {
      if (!crit_ptr->isModified()) { //only copy MOB's
         gainCritter(crit_ptr); //will increment cur_in_game
      }//if
   }//while

   return *this;
}//room::operator= overload


void room::clear() {
   int i;
   
   if (mudlog.ofLevel(TRC)) {
      mudlog << "room::clear..." << endl;
   }

   short_desc.clear();
   
   room_flags.clear();

   keywords.clearAndDestroy();
   
   for (i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = 0;
   }

   clear_obj_list(inv);

   critter* tmp;
   while (!critters.isEmpty()) {
      tmp = critters.peekFront();
      removeCritter(tmp); //decrement cur_in_game
      if (tmp->isModified()) {
         delete tmp;
      }
   }//while

   doors.clearAndDestroy();

   if (mudlog.ofLevel(TRC)) {
      mudlog << "room::clear...  DONE." << endl;
   }

}// clear


int room::read(istream& ifile, int read_all = TRUE) {
   // Grab the first token, if it's <META then we're V3+, otherwise
   // we're V2.
   String str(50);
   ifile >> str; //grab the first token, if V2.x it not be <META

   if (str == "<META") {
      return read_v3(ifile, read_all);
   }
   else {
      return read_v2(ifile, read_all, str);
   }
}//read

int room::read_v2(istream& ofile, int read_all, String& firstName) {
   int i, test = TRUE;
   char tmp[81];
   String tmp_str(80);
   door* dr_ptr;

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room, read_all:  " << read_all << endl;
   }

   clear();  //stop up any memory leaks etc.

   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
      }
      return -1;
   }

   while (test) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in room read." << endl;
         }
         return -1;
      }
      ofile >> tmp_str;
      if (strcmp(tmp_str, "~") == 0) {
         test = FALSE;
      }//if
      else {
         addName(tmp_str);
      }//else
   }//while            
   ofile.getline(tmp, 80);         

   tmp_str.termedRead(ofile);
   addShortDesc(tmp_str);

   tmp_str.termedRead(ofile);
   addLongDesc(tmp_str);

   room_flags.read(ofile);
   room_flags.turn_on(23); //turn on in_use flag for CERTAIN
   if (room_flags.get(21)) { //if zlocked
      read_all = TRUE;
      room_flags.turn_on(22); //it will be totally loaded
   }//if
   setComplete(); //if we can read it, it's complete!

   KeywordPair* kwd_ptr;
   if (room_flags.get(32)) { //if has keywords
      while (TRUE) {
         ofile >> i;
         ofile.getline(tmp, 80);
         if (i != -1) {
            kwd_ptr = new KeywordPair();
            kwd_ptr->read(ofile, read_all);
            keywords.append(kwd_ptr);
         }
         else { //read line, then read desc
            break;
         }
      }//while
   }//if

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room, about to do cur_stats.  " << endl;
   }

   
   for (i = 0; i<ROOM_CUR_STATS; i++) {
      ofile >> cur_stats[i];
   }
   ofile.getline(tmp, 80);


                /*  Affected By */
   SpellDuration* sd_ptr = NULL;
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in room read." << endl;
         }
         return -1;
      }
      
      sd_ptr = new SpellDuration();
      sd_ptr->spell = i;
      ofile >> sd_ptr->duration;
      
      addAffectedBy(sd_ptr);
      
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);


   if (!affected_by.isEmpty()) {
      // Also place it on the list of rooms to be checked for loss of spell...
      room* _this = this;
      affected_rooms.appendUnique(_this);
   }//if

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room, about to do inventory." << endl;
   }
                /*  Inventory */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in room read, inv."
              << endl;
         }
         return -1;
      }
      if (check_l_range(i, 0, NUMBER_OF_ITEMS, mob_list[0], FALSE)) {
         if (obj_list[i].isInUse()) {
            if (read_all || 
                ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 100) > 
                d(1,100)) {
               gainInv(&(obj_list[i]));    //add it to inventory
//               obj_list[i].OBJ_CUR_IN_GAME++; //increment cur_in_game
            }//if
         }//if
         else {
            Sprintf(tmp_str, 
             "ERROR:  trying to load non_existant inv: %i in room:  %i.\n",
             i, cur_stats[2]);
            mudlog.log(ERR, tmp_str);
         }//else
      }//if
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);   

                       /*  Doors  */
   SCell<door*> cll;
   door* walker;
   short did_insert = FALSE;
   ofile >> i; //check for terminal value
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
         }
         return -1;
      }

      dr_ptr = new door;
      dr_ptr->read(ofile);
      dr_ptr->setContainer(this);
      doors.head(cll);
      String* dr_name = dr_ptr->getName();

      if (!dr_name) {
         mudlog.log(ERR, "ERROR:  dr_name is NULL.");
      }//if

      String* walk_name;
      while ((walker = cll.next())) {
         walk_name = walker->getName();
         if (*walk_name > *dr_name) {
            doors.insertBefore(cll, dr_ptr);
            did_insert = TRUE;
            break;
         }//if
      }//while

      if (!did_insert)
         doors.append(dr_ptr);
      ofile >> i; //check for next terminal value
   }//while 
   ofile.getline(tmp, 80); //clear rest of line

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room About to read critters.." << endl;
   }

                      /*  Critters, stored mobl#  */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
         }
         return -1;
      }

      if (mob_list[i].isInUse()) {
         if (mob_list[i].isPlayerShopKeeper()) {
            critter* shop_owner = load_player_shop_owner(i);
            if (shop_owner) {
               gainCritter(shop_owner);
            }//if
         }//if
         else { //regular case
            gainCritter(&(mob_list[i])); //this will increment cur_in_game
         }
      }//if
      else {
         Sprintf(tmp_str, 
                 "ERROR:  trying to load non_existant mob: %i, in room: %i\n",
                 i, cur_stats[2]);
         mudlog.log(ERR, tmp_str);
      }//else
      ofile >> i;
   }//while 
   ofile.getline(tmp, 80); //get comments after mobs...


   /* read procs, if we have them. */
   if (room_flags.get(35)) {
      //mudlog.log("Mob has proc scripts...");
      int sent_;
      GenScript* ptr;

      ofile >> sent_;
      ofile.getline(tmp, 80);

      if (mudlog.ofLevel(DB)) {
         mudlog << "Tmp, after script#: " << sent_ << " -:" << tmp
                << ":-\n";
      }

      while (sent_ != -1) {
         if (mudlog.ofLevel(DB))
            mudlog << "\nReading script# " << sent_ << endl;
         if (!ofile) {
            if (mudlog.ofLevel(ERR)) {
               mudlog << "ERROR:  mob_data reading script da_file FALSE." << endl;
            }
            return -1;
         }

         ptr = new GenScript();
         ptr->read(ofile);
         addProcScript(ptr);
         ofile >> sent_;
         ofile.getline(tmp, 80);
         if (mudlog.ofLevel(DB))
            mudlog << "Got rest of line -:" << tmp << ":-" << endl;
      }
   }//if it had proc scripts

   ofile.getline(tmp, 80); //get comments at end of room..
   // Post processing...

   int vb = getVisBit();
   setVisBit(vb | 1024); //hack, forgot a flag :P
   if (vb & 1) {
      room_flags.turn_on(14); //make normally_dark
   }

   if (mudlog.ofLevel(DB)) {
      mudlog << "Done reading room, number:  " << getRoomNum() << endl;
   }
   return 0;
}//read_v2


//TODO
int room::read_v3(istream& ofile, int read_all = TRUE) {
   int i;
   char tmp[81];
   String tmp_str(80);
   door* dr_ptr;

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room, read_all:  " << read_all << endl;
   }

   room::clear();  //stop up any memory leaks etc.

   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
      }
      return -1;
   }

   Entity::read(ofile, read_all);
   Scriptable::read(ofile, read_all);

   short_desc.read(ofile);

   room_flags.read(ofile);
   room_flags.turn_on(23); //turn on in_use flag for CERTAIN
   if (room_flags.get(21)) { //if zlocked
      read_all = TRUE;
      room_flags.turn_on(22); //it will be totally loaded
   }//if
   setComplete(); //if we can read it, it's complete!

   KeywordPair* kwd_ptr;
   if (room_flags.get(32)) { //if has keywords
      while (TRUE) {
         ofile >> i;
         ofile.getline(tmp, 80);
         if (i != -1) {
            kwd_ptr = new KeywordPair();
            kwd_ptr->read(ofile);
            keywords.append(kwd_ptr);
            kwd_ptr->setContainer(this);
         }
         else { //read line, then read desc
            break;
         }
      }//while
   }//if

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room, about to do cur_stats.  " << endl;
   }
   
   for (i = 0; i<ROOM_CUR_STATS; i++) {
      ofile >> cur_stats[i];
   }
   ofile.getline(tmp, 80);


   if (!affected_by.isEmpty()) {
      // Also place it on the list of rooms to be checked for loss of spell...
      room* _this = this;
      affected_rooms.appendUnique(_this);
   }//if

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room, about to do inventory." << endl;
   }
		/*  Inventory */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in room read, inv."
              << endl;
         }
         return -1;
      }
      if (check_l_range(i, 0, NUMBER_OF_ITEMS, mob_list[0], FALSE)) {
         if (obj_list[i].isInUse()) {
            if (read_all || 
                ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 100) > 
                d(1,100)) {
               gainInv(&(obj_list[i]));    //add it to inventory
//               obj_list[i].OBJ_CUR_IN_GAME++; //increment cur_in_game
            }//if
         }//if
         else {
            Sprintf(tmp_str, 
             "ERROR:  trying to load non_existant inv: %i in room:  %i.\n",
             i, cur_stats[2]);
            mudlog.log(ERR, tmp_str);
         }//else
      }//if
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);   

                       /*  Doors  */
   SCell<door*> cll;
   door* walker;
   short did_insert = FALSE;
   ofile >> i; //check for terminal value
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
         }
         return -1;
      }

      dr_ptr = new door();
      dr_ptr->read(ofile, read_all);
      dr_ptr->setContainer(this);
      doors.head(cll);
      tmp_str = *(dr_ptr->getDirection());

      const String* walk_name;
      while ((walker = cll.next())) {
	 walk_name = walker->getName();
         if (*walk_name > tmp_str) {
            doors.insertBefore(cll, dr_ptr);
            did_insert = TRUE;
            break;
         }//if
      }//while

      if (!did_insert)
         doors.append(dr_ptr);
      ofile >> i; //check for next terminal value
   }//while 
   ofile.getline(tmp, 80); //clear rest of line

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room About to read critters.." << endl;
   }

                      /*  Critters, stored mobl#  */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
         }
         return -1;
      }

      if (mob_list[i].isInUse()) {
         if (mob_list[i].isPlayerShopKeeper()) {
            critter* shop_owner = load_player_shop_owner(i);
            if (shop_owner) {
               gainCritter(shop_owner);
            }//if
         }//if
         else { //regular case
            gainCritter(&(mob_list[i])); //this will increment cur_in_game
         }
      }//if
      else {
         Sprintf(tmp_str, 
                 "ERROR:  trying to load non_existant mob: %i, in room: %i\n",
                 i, cur_stats[2]);
         mudlog.log(ERR, tmp_str);
      }//else
      ofile >> i;
   }//while 
   ofile.getline(tmp, 80); //get comments after mobs...

   // Post processing...

   int vb = getVisBit();
   setVisBit(vb | 1024); //hack, forgot a flag :P
   if (vb & 1) {
      room_flags.turn_on(14); //make normally_dark
   }

   if (mudlog.ofLevel(DB)) {
      mudlog << "Done reading room, number:  " << getRoomNum() << endl;
   }
   return 0;
}//read_v3


void room::checkLight(int do_msg) {
   int vb = getVisBit();
   //mudlog << "In room::checkLight(), id_num: " << getIdNum() << endl;

   if (isPermDark()) {
      //mudlog << "Permdark, trying to make the room dark.\n";
      setVisBit(vb | 1); //perm dark it is
   }//if
   else if (isNormallyDark()) { //normally dark
      //mudlog << "Normally dark, trying to make the room dark.\n";
      setVisBit(vb | 1);
      if (isAffectedBy(ILLUMINATE_SKILL_NUM)) {
         //mudlog << "Room was ILLUMINATED, making it light." << endl;
         setVisBit(vb & ~1);
      }//if
      else {
         SCell<critter*> cll(critters);
         critter* crit_ptr;
         while ((crit_ptr = cll.next())) {
            if (crit_ptr->CRIT_FLAGS.get(1)) { //if using_light source 
               //if (mudlog.ofLevel(DBG)) {
               //   mudlog << "Room was ILLUMINATED, making it light." << endl;
               //}

               setVisBit(vb & ~1);
               break;
            }//if
         }//while
      }//else
   }//if normally_dark
   else {
      setVisBit(vb & ~1);
   }//else
   
   if (do_msg) {
      if (vb != getVisBit()) {
         if (vb & 1) { //it was changed to dark
            showAllCept(CS_ROOM_BECOMES_DARK);
         }//if
         else {
            showAllCept(CS_ROOM_BECOMES_LIGHT);
         }//else
      }//if
   }//if do_messages
}//checkLight


int room::write(ostream& ofile) {
   int i, j;
   SCell<door*> d_cell(doors);
   door* d_ptr;
   SCell<object*> ob_cell(inv);
   object* ob_ptr;
   SCell<critter*> c_cell(critters);
   critter* c_ptr;
   short was_totally_read = FALSE;

   normalize(); //make sure everything jives!
   
   Entity::write(ofile);
   Scriptable::write(ofile);

   if (room_flags.get(22)) { //totaly read flag
      was_totally_read = TRUE;
      room_flags.turn_off(22); // this flag is purely temp, don't write
   }//if


   short_desc.write(ofile);

   room_flags.write(ofile);

   //mudlog << "Keywords size:  " << keywords.size() << endl;
   
   if (room_flags.get(32)) { //if has keywords
      SCell<KeywordPair*> kcll(keywords);
      KeywordPair* kptr;

      while ((kptr = kcll.next())) {
         ofile << "1   Start of KeywordPair\n";
         kptr->write(ofile);
      }
      ofile << "-1  No more KeywordPairs\n";
   }//if

   //mudlog << "DOne writing out keywordds.." << endl;

   for (i = 0; i<ROOM_CUR_STATS; i++) {
      ofile << cur_stats[i] << " ";
      if ((i + 1) % 20 == 0)
         ofile << endl;
   }//for
   ofile << "\tcur_stats\n";

		/*  Inventory */
   
   j = 0;
   inv.head(ob_cell);
   while ((ob_ptr = ob_cell.next())) {
      ofile << ob_ptr->OBJ_NUM << " ";
      if ((++j % 20) == 0)
         ofile << endl;
   }//while
   ofile << -1 << "\tinv\n";

                       /*  Doors, stored door#, dest  */
   
   j = 0;
   while ((d_ptr = d_cell.next())) {
     if (d_ptr->isInUse()) {
       ofile << 1 << " "; //test for next, will read untill -1
       d_ptr->write(ofile);
       if (++j > 8) {
         ofile << endl;
         j = 0;
       }//if
     }//if
   }//while 
   ofile << -1 << "\tdoor#, dest\n";

                      /*  Critters, stored mob# */
   int cnt = 0;
   while ((c_ptr = c_cell.next())) {
      if (++cnt > 20) {
         ofile << "\n";
         cnt = 0;
      }

      if (c_ptr->mob && !c_ptr->pc)
         ofile << c_ptr->MOB_NUM << " ";
   }//while
   ofile << -1 << "\tmobs\n";

   ofile << "End of Room\n";
 
   if (was_totally_read) {
      room_flags.turn_on(22); //make it as it was
   }//if
   return 0;
}//Write....sub_room


void room::checkForBattle(critter& pc) {
   SCell<critter*> cll(critters);

   critter* ptr;
   while ((ptr = cll.next())) {
      if (ptr->doesRemember(pc) && ptr->canDetect(pc)) {
         say("There you are!!", *ptr, *(ptr->getCurRoom()));
         try_hit(pc, *ptr);
      }//if

      if (!haveCritter(&pc)) {
         return;
      }
      if (haveCritter(ptr)) { //make sure we're still there
         if (pc.doesRemember(*ptr) && pc.canDetect(*ptr)) {
            say("I've found you now!!", pc, *this);
            try_hit(*ptr, pc);
         }//if
      }//if
   }//while
}//checkForBattle


int room::getZoneNum() {
   return ZoneCollection::instance().getZoneFor(*this).getIdNum();
}

void room::toStringStat(critter* viewer, String& rslt, ToStringTypeE st) {
   String buf(500);

   if (viewer->isUsingClient()) {
      Sprintf(rslt, "<ROOM %i> ", getIdNum());
   }
   else {
      Sprintf(rslt, "Room: %i", getIdNum());
   }

   Entity::toStringStat(viewer, buf, st);
   rslt.append(buf);

   short_desc.toStringStat("<SHORT_DESC>", "</SHORT_DESC>\n", viewer, buf);
   rslt.append(buf);

   if (viewer->isImmort() || (st | ST_IDENTIFY)) {
      Markup::toString(NULL, room_flags, ROOM_FLAGS_NAMES,
                       viewer, NULL, buf);
      rslt.append(buf);
      
      Sprintf(buf, "Movement Cost: %i\n", getMovCost());
      rslt.append(buf);
   }

   if (viewer->isImmort()) {
      SCell<KeywordPair*> cll(keywords);
      KeywordPair* ptr;
      int cnt = 0;

      while ((ptr = cll.next())) {
         ptr->toStringStat(viewer, buf, cnt, st);
         rslt.append(buf);
         cnt++;
      }

      Scriptable::toStringStat(viewer, buf);
      rslt.append(buf);

      Sprintf(buf, "Number of critters:  %i  Number of Scripts Pending: %i\n\n",
              critters.size(), pending_scripts.size());
      rslt.append(buf);
   }//if immort

   if (viewer->isUsingClient()) {
      rslt.append("</ROOM>\n");
   }
}//toStringStat


/* called after OLC to enforce as much state as possible. */
void room::normalize() {
   setVisBit(getVisBit() | 1024);

   if (keywords.size() > 0) {
      room_flags.turn_on(32);
   }
   else {
      room_flags.turn_off(32);
   }
}//normalize_room

 
const String* room::getRandomExitDir(critter& pc) {
   int sz = doors.size();
   int idx = d(1, sz);

   SCell<door*> cll(doors);
   door* ptr;

   int cnt = 0;
   while ((ptr = cll.next())) {
      cnt++;
      if (cnt == idx) {
         if (ptr->getDestRoom()) {
            if (!(ptr->getDestRoom()->isNoWanderForeignMob() &&
                  (pc.getNativeZoneNum() != ptr->getDestRoom()->getZoneNum()))) { 
               if (!(ptr->getDestRoom()->isNoWanderMob() &&
                     pc.isNpc()) &&
                   mob_can_enter(pc, *(ptr->getDestRoom()), FALSE)) {
                  return ptr->getDirection();
               }//if
            }//if
         }//if
      }//if
      else if (cnt > idx) {
         break;
      }//else
   }//while

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Didn't find the one we wanted..getting first one." << endl;
   }

   // Didn't find the one we wanted...get the first one.
   doors.head(cll);

   while ((ptr = cll.next())) {
      if (ptr->getDestRoom()) {
         if (!(ptr->getDestRoom()->isNoWanderForeignMob() &&
               (pc.getNativeZoneNum() != ptr->getDestRoom()->getZoneNum()))) { 
            if (!(ptr->getDestRoom()->isNoWanderMob() && 
                  !pc.isPc()) &&
                mob_can_enter(pc, *(ptr->getDestRoom()), FALSE)) {
               return ptr->getDirection();
            }//if
         }//if
      }//if
   }//while

   return &UNKNOWN;
}//getRandomExitDir

int room::isInUse() {
   return (room_flags.get(23) || critters.size());
}

int room::getCritCount(critter& pc) {
   return crit_count(critters, pc);
}


/* this returns the critter if exists.  rm_num is the number of
 * the room we found it in.  Returns NULL if not found.
 * searches entire world. */
critter* room::haveCritNamedInWorld(const int i_th, const String* name,
                                    const int see_bit, int& rm_num) {
   Cell<String*> char_cell;
   SCell<critter*> cell;
   critter* crit_ptr;
   int count = 0;

   mudlog.log(DBG, "In have_crit_named, for whole room_list");

   if (name->Strlen() == 0) 
      return NULL;

   if (i_th <= 0)
      return NULL;

   //mudlog.log(DBG, "Looking in room number:  ");
   for (int i = 0; i<NUMBER_OF_ROOMS; i++) {

//       if (mudlog.ofLevel(DBG)) {
//          mudlog << i << " ";
//          if (((i + 1) % 100) == 0)
//             mudlog << flush;
//       }
      if (room_list.elementAtNoCreate(i)) {
         room_list[i].critters.head(cell);
         while ((crit_ptr = cell.next())) {
            if (detect(see_bit, crit_ptr->VIS_BIT)) {
               if (crit_ptr->isNamed(*name, NULL)) {
                  count++;
                  if (count == i_th) {
                     rm_num = i;
                     return crit_ptr;
                  }//if
               }//while
            }//if
         }//while
      }//for
   }//if
   rm_num = 0;
   return NULL;
}//have_crit_named for whole room_list, returns room#


/**  Find a critter in a given zone.  Returns the critter, or
 * NULL if not found.  in_room holds the room number we found it
 * in. */
critter* room::haveCritNamedInZone(zone& zn, const int i_th,
                                   const String* name, const int see_bit,
                                   int& in_room) {
   SCell<critter*> cell;
   critter* crit_ptr;
   int count = 0, len;

   in_room = 0;

   //log("In have_crit_named, for whole zone");

   if (!name) {
      mudlog.log(ERR, "ERROR:  NULL name sent to have_crit_named.\n");
      return NULL;
   }//if

   if ((len = name->Strlen()) == 0) 
      return NULL;

   if (i_th <= 0)
      return NULL;

   int end = zn.getEndRoomNum();

   for (int i = zn.getBeginRoomNum(); i <= end; i++) {
      room_list[i].critters.head(cell);
      while ((crit_ptr = cell.next())) {
         if (detect(see_bit, crit_ptr->VIS_BIT)) {
            if (crit_ptr->isNamed(*name, NULL)) {
               count++;
               if (count == i_th) {
                  in_room = i;
                  return crit_ptr;
               }//if
            }//if
         }//if
      }//while
   }//for
   in_room = 0;
   return NULL;
}//have_crit_named for whole zone


void room::doPoofOut(critter& pc) {
   String buf(100);
   SCell<critter*> cll(critters);
   critter* ptr;
   while ((ptr = cll.next())) {
      if (&pc != ptr) {
         if (ptr->isImmort() && (ptr->IMM_LEVEL >= pc.IMM_LEVEL)) {
            Sprintf(buf, "[OUT: %S] %S\n", pc.getName(), &(pc.getPoofout()));
         }
         else {
            Sprintf(buf, "%S\n", &(pc.getPoofout()));
         }
         ptr->show(buf);
      }
   }//while
}//doPoofOut


int room::doRclear(int new_rm_num) {
   critter* crit_ptr;

   while (!critters.isEmpty()) {
      crit_ptr = critters.peekFront();

      if ((crit_ptr->isPc()) || crit_ptr->isSmob()) {
         //mudlog << "Was a SMOB or PC." << endl;
         int is_dead;
         crit_ptr->doGoToRoom(new_rm_num, NULL, NULL, is_dead, getIdNum(), 1);
         if (!is_dead) {
            crit_ptr->show("The void has swallowed your previous location!!\n");
         }
      }
      else {
         //just remove the pointer...
         removeCritter(crit_ptr);
      }//else
   }//while

   clear();  //clear out the room pc WAS in!!
   return 0;
}//doRclear


int room::doScan(critter& pc) {
   String buf(100);
   SCell<door*> dcll(doors);
   SCell<critter*> cll;
   critter* ptr;
   int a, b;

   int pl = get_percent_lrnd(SCAN_SKILL_NUM, pc);

   door* dptr;
   while ((dptr = dcll.next())) {
      if (detect(pc.SEE_BIT, dptr->getVisBit() | getVisBit())) {
         if (dptr->isOpen()) {
            dptr->getDestRoom()->critters.head(cll);
            while ((ptr = cll.next())) {
               if (detect(pc.SEE_BIT,
                          ptr->VIS_BIT | dptr->getDestRoom()->getVisBit())) {
                  a = max((pc.LEVEL - pc.POS), 10) + pl;
                  b = max((ptr->LEVEL - ptr->POS), 10) + 100;

                  if (ptr->isHiding()) {
                     b *= max(1, max(get_percent_lrnd(HIDE_SKILL_NUM, *ptr),
                                     get_percent_lrnd(BLEND_SKILL_NUM, *ptr)) / 10);
                  }

                  if (d(1, a) > d(1, b)) {                     
                     Sprintf(buf, "     %S%P25 %S.\n", dptr->getDirection(),
                             ptr->getName());
                     pc.show(buf);
                  }//if
               }//if
            }//while
         }//if
      }//if
   }//while
   return 0;
}//doScan

void room::doPoofIn(critter& pc) {
   String buf(100);
   SCell<critter*> cll(critters);
   critter* ptr;
   while ((ptr = cll.next())) {
      if (&pc != ptr) {
         if (ptr->isImmort() && (ptr->IMM_LEVEL >= pc.IMM_LEVEL)) {
            Sprintf(buf, "[IN: %S] %S\n", pc.getName(), &(pc.getPoofin()));
         }
         else {
            Sprintf(buf, "%S\n", &(pc.getPoofin()));
         }
         
         ptr->show(buf);
      }
   }//while
}//doPoofIn


void room::resetProcMobs() {
   SCell<critter*> cll(critters);
   critter* ptr;
   while ((ptr = cll.next())) {
      //mudlog << "Found a critter:  " << ptr << endl << flush;
      if (ptr->mob) {
         //mudlog << "Was a mob..." << endl;
         ptr->MOB_FLAGS.turn_on(3); //should do spec procs
      }//if
   }//while
}//resetProcMobs


void room::purgeCritter(int mob_num, critter& pc) {
   int done = FALSE;
   while (!done) {
      SCell<critter*> cll(critters);
      critter* ptr;

      done = TRUE;
      while ((ptr = cll.next())) {
         if (ptr->isNpc() && (ptr->getIdNum() == mob_num)) {
            agg_kills_vict(pc, *ptr);
            done = FALSE;
            break;
         }//if
      }//while
   }//for
}//purgeCritter


int room::sub_a_4_b(critter* crit_ptr, int i_th, const String& name,
                    critter* viewer) {
   return crit_sub_a_4_b(crit_ptr, critters, i_th, 
                         &name, viewer);
}

int room::sub_a_4_b(critter* a, critter* b, int i_th) {

   // Don't want to add it if we can't substitute it, but the list<>
   // method does indeed append if it's not already there.
   if (critters.haveData(b)) {
      return critters.substituteData(b, a, i_th);
   }
   return FALSE;
}

void room::gainCritter(critter* crit) {
   if (crit->isNpc()) {
      crit->incrementCurInGame();
   }
   critters.prepend(crit);

   // This nips a bunch of bugs in the bud.  No more will we have
   // MOB's running around in the game.  If it's in the game, it will
   // be a SMOB, which for reasons lost in antiquity, means it is a
   // unique Instance and may be modified independently of the database.
   if (crit->isMob()) {
      crit = mob_to_smob(*crit, *this);
   }//if
   crit->setContainer(this);
}

void room::loseObjectFromGame(object& which_un) {
   SCell<object*> ocll(inv);
   object* optr;
   SCell<critter*> ccll;
   critter* cptr;

   int targ_num = which_un.getIdNum();

   optr = ocll.next();
   while (optr) {
      if (optr->isModified() && (optr->getIdNum() == targ_num)) { //if SOBJ
         delete optr;
         optr = inv.lose(ocll);
      }
      else if (optr->getIdNum() == targ_num) {
         optr = inv.lose(ocll);
      }
      else
         optr = ocll.next();
   }//while
   
   critters.head(ccll);
   while ((cptr = ccll.next())) {
      cptr->loseObjectFromGame(&which_un);
   }//while
}//loseObjectFromGame

critter* room::removeCritter(critter* crit) {
   critter* retval;

   retval = critters.loseData(crit);

   if (retval) {
      if (crit->isNpc()) {
         crit->decrementCurInGame();
      }
   }//if
   else {
      // We use room 0 for newbies logging in and such...will get boundary
      // case errors here in some instances...
      if (getIdNum() != 0) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "WARNING:  could not remove the critter in"
                   << " room::removeCrit(), crit_name: " << *(crit->getName()) 
                   << " num: " << crit->getIdNum() << " room: "
                   << getIdNum() << endl;
            core_dump("removeCritter");
         }//if
      }//if
   }//else

   return retval;
}//removeCritter


void room::getPetsFor(critter& owner, SafeList<critter*>& rslts) {
   critter* ptr;
   SCell<critter*> cll(critters);
   while ((ptr = cll.next())) {
      if (owner.PETS.haveData(ptr)) {
         rslts.append(ptr);
      }//if
   }//while
}//getPetsFor

void room::showCritters(critter& pc) {
   out_crit(critters, pc);
}

//show inv to pc (use out_inv method)
void room::outInv(critter& pc) {
   out_inv(inv, pc, ROOM_INV);
}

void room::gainInv(object* obj) {
   if (obj->obj_flags.get(55)) { //if it's coins
      if (obj->cur_stats[1] == 0) {
         if (obj->isModified()) {
            mudlog << "ERROR:  Possible memory leak, gainInv:  obj is SOBJ"
                   << " but has zero coins, obj#" << obj->getIdNum()
                   << " room# " << getIdNum() << endl;
         }
         return; //don't add it, it had zero coins
      }//if
   }//if
         
   inv.prepend(obj);
   if (obj->isModified()) {
      obj->setContainer(this);
   }
}

object* room::loseInv(object* obj) {
   return inv.loseData(obj);
}

int room::canSeeSky() {
   return (room_flags.get(4) || room_flags.get(15) || room_flags.get(16)
           || room_flags.get(17));
}


critter* room::haveCritNamed(int i_th, const String* name, int see_bit) {
   return ::have_crit_named(critters, i_th, name, see_bit, *this);
}

critter* room::haveCritNamed(int i_th, const String* name, critter& pc) {
   int foo;
   return haveCritNamed(i_th, name, pc, foo);
}

critter* room::haveCritNamed(int i_th, const String* name, critter& pc,
                             int& count_sofar) {
   if ((strcasecmp(*name, "me") == 0) ||
       (strcasecmp(*name, "self") == 0)) {
      return &pc;
   }
   return ::have_crit_named(critters, i_th, name, pc.SEE_BIT,
                            count_sofar, *this);
}

critter* room::haveCritter(critter* ptr) {
   if (critters.haveData(ptr)) {
      return ptr;
   }
   return NULL;
}

object* room::haveObject(object* ptr) {
   if (inv.haveData(ptr)) {
      return ptr;
   }
   return NULL;
}

object* room::haveObjNamed(int i_th, const String* name, critter& viewer) {
   int foo;
   return ::have_obj_named(inv, i_th, name, viewer.getSeeBit(),
                           *this, foo, viewer.getLanguage());
}

void room::checkForProc(String& cmd, String& arg1, critter& actor,
                        int targ_num) {
   Scriptable::checkForProc(cmd, arg1, &actor, targ_num, this, NULL);
}


object* room::haveObjNamed(int i_th, const String* name, int c_bit) {
   int foo;
   return ::have_obj_named(inv, i_th, name, c_bit, *this, foo);
}

object* room::haveAccessibleObjNamed(int i_th, const String* name,
                                     critter& pc, int& count_sofar, int& posn) {
   object* ptr;
   ptr = have_obj_named(inv, i_th, name, pc.getSeeBit(), *this, count_sofar);
   if (!ptr) {
      ptr = have_obj_named(pc.inv, i_th - count_sofar, name, pc.getSeeBit(), *this,
                           count_sofar);
   }
   if (!ptr) {
      ptr = pc.findObjInEq(i_th - count_sofar, *name, pc.getSeeBit(), *this,
                           posn, count_sofar);
   }
   return ptr;
}//haveAccesibleObjNamed

critter* room::findFirstBanker() {
   SCell<critter*> cell(critters);
   critter* crit_ptr;

   while ((crit_ptr = cell.next())) { 
      if (crit_ptr->isBanker()) {
         return crit_ptr;
      }//if
   }//while
   
   return NULL;
}//findFirstBanker


critter* room::findFirstShopKeeper() {
   SCell<critter*> cell(critters);
   critter* crit_ptr;

   while ((crit_ptr = cell.next())) { 
      if (crit_ptr->isShopKeeper()) {
         return crit_ptr;
      }//if
   }//while
   
   return NULL;
}//findFirstShopOwner


critter* room::findFirstTeacher() {
   SCell<critter*> cell(critters);
   critter* crit_ptr;

   while ((crit_ptr = cell.next())) { 
      if (crit_ptr->isTeacher()) {
         return crit_ptr;
      }//if
   }//while
   
   return NULL;
}//findFirstTeacher


door* room::findDoor(int i_th, const String* direction, critter& viewer, int& sofar) {
   return door::findDoor(doors, i_th, direction, sofar, &viewer);
}

door* room::findDoor(int i_th, const String* direction, critter& viewer) {
   int foo = 0;
   return door::findDoor(doors, i_th, direction, foo, &viewer);
}

door* room::findDoor(int i_th, const String* direction) {
   return door::findDoor(doors, i_th, direction, NULL);
}


void room::showAllCept(CSentryE msg, critter* pc, critter* pc2) {
   SCell<critter*> cell(critters);
   critter* crit_ptr;

   while ((crit_ptr = cell.next())) { 
      if ((crit_ptr != pc) && (crit_ptr != pc2)) {  
	 if (crit_ptr->POS < POS_SLEEP)  
	    crit_ptr->show(msg);
      }
   }//while

   SCell<object*> cll(inv);
   object* obj;

   while ((obj = cll.next())) {
     if (obj->obj_proc && (crit_ptr = obj->obj_proc->w_eye_owner)) {
       if ((crit_ptr != pc) && (crit_ptr != pc2)) {
	 if (crit_ptr->POS < POS_SLEEP) {
	   crit_ptr->show("#####");
	   crit_ptr->show(msg);
	 }//if
       }//if
     }//if
   }//while
}// show_all_cept


void room::showAllCept(String& msg, critter* pc, critter* pc2) {
   SCell<critter*> cell(critters);
   critter* crit_ptr;

   while ((crit_ptr = cell.next())) { 
      if ((crit_ptr != pc) && (crit_ptr != pc2)) {  
	 if (crit_ptr->POS < POS_SLEEP)  
	    crit_ptr->show(msg);
      }
   }//while

   SCell<object*> cll(inv);
   object* obj;

   while ((obj = cll.next())) {
     if (obj->obj_proc && (crit_ptr = obj->obj_proc->w_eye_owner)) {
       if ((crit_ptr != pc) && (crit_ptr != pc2)) {
	 if (crit_ptr->POS < POS_SLEEP) {
	   crit_ptr->show("#####");
	   crit_ptr->show(msg);
	 }//if
       }//if
     }//if
   }//while
}// show_all_cept


void room::showAllCeptN(CSentryE msg, Entity* getNameOf) {
   SCell<critter*> cell(critters);
   critter* crit_ptr;
   String buf(100);

   while ((crit_ptr = cell.next())) { 
      if (crit_ptr->POS < POS_SLEEP) {
         Sprintf(buf, cstr(msg, *crit_ptr), getNameOf->getName(crit_ptr));
         buf.cap();
         crit_ptr->show(buf);
      }
   }//while

   SCell<object*> cll(inv);
   object* obj;

   while ((obj = cll.next())) {
      if (obj->obj_proc && (crit_ptr = obj->obj_proc->w_eye_owner)) {
	 if (crit_ptr->POS < POS_SLEEP) {
            crit_ptr->show("#####");
            Sprintf(buf, cstr(msg, *crit_ptr), getNameOf->getName(crit_ptr));
            buf.cap();
            crit_ptr->show(msg);
	 }//if
      }//if
   }//while
}// show_all_cept


int room::doEmote(critter& pc, CSelectorColl& includes,
                  CSelectorColl& denies, CSentryE cs_entry, ...) {
   va_list argp;
   va_start(argp, cs_entry);
   int retval = 0;
   retval = vDoEmote(pc, includes, denies, cs_entry, argp);
   va_end(argp);
   return retval;
}

int room::vDoEmote(critter& pc, CSelectorColl& includes, CSelectorColl& denies,
                   CSentryE cs_entry, va_list argp) {
   SCell<critter*> cll(critters);
   critter* ptr;
   String buf(100);
   String buf2(100);


   while ((ptr = cll.next())) {
      if (!(denies.matches(ptr, &pc))) {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "room::doEmote, not denied." << endl;
         }
         if (includes.matches(ptr, &pc)) {
            if (mudlog.ofLevel(DBG)) {
               mudlog << "room::doEmote, includes matched." << endl;
               mudlog << "cstr of " << (int)(cs_entry) << "-:"
                      << cstr(cs_entry, *ptr) << ":-" <<  endl;
            }

            vSprintf(buf, cstr(cs_entry, *ptr), argp);
            if (mudlog.ofLevel(DBG)) {
               mudlog << endl << "buf -:" << buf << ":-" << endl << endl;
            }
            Sprintf(buf2, "%S %S", pc.getName(ptr), &buf);
            buf2.Cap();
            ptr->show(buf2);
         }//if
      }//if
   }//while
   return 0;
}//vDoEmote

int room::doEmoteN(critter& pc, CSentryE cs_entry, Entity& vict) {
   return doEmoteN(pc, Selectors::instance().CC_all,
                   Selectors::instance().CC_sleeping,
                   cs_entry, vict);
}

int room::doEmoteN(critter& pc, CSelectorColl& includes, CSelectorColl& denies,
                   CSentryE cs_entry, Entity& vict) {
   SCell<critter*> cll(critters);
   critter* ptr;
   String buf(100);
   String buf2(100);

   while ((ptr = cll.next())) {
      if (!(denies.matches(ptr, &pc))) {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "room::doEmoteN, not denied." << endl;
         }
         if (includes.matches(ptr, &pc)) {
            if (mudlog.ofLevel(DBG)) {
               mudlog << "room::doEmoteN, includes matched." << endl;
               mudlog << "cstr of " << (int)(cs_entry) << "-:"
                      << cstr(cs_entry, *ptr) << ":-" <<  endl;
            }

            Sprintf(buf, cstr(cs_entry, *ptr), vict.getLongName());
            if (mudlog.ofLevel(DBG)) {
               mudlog << endl << "buf -:" << buf << ":-" << endl << endl;
            }
            Sprintf(buf2, "%S %S", pc.getName(ptr), &buf);
            buf2.cap();
            ptr->show(buf2);
         }//if
      }//if
   }//while
   return 0;
}//doEmoteN


/** These default to english, makes a copy of incoming data. */
void room::addShortDesc(String& new_val) {
   short_desc.addString(English, new_val);
}

void room::addShortDesc(LanguageE l, String& buf) {
   short_desc.addString(l, buf);
}

void room::setShortDesc(CSentryE msg) {
   for (int i = 0; i<LS_PER_ENTRY; i++) {
      LString nm((LanguageE)(i), CSHandler::getString(msg, (LanguageE)(i)));
      short_desc.addLstring(nm);
   }
}

void room::appendShortDesc(CSentryE msg) {
   for (int i = 0; i<LS_PER_ENTRY; i++) {
      LString nm((LanguageE)(i), CSHandler::getString(msg, (LanguageE)(i)));
      short_desc.appendString(nm);
   }
}

void room::appendShortDesc(String& msg) {
   short_desc.appendString(English, msg);
}

void room::prependShortDesc(String& str) {
   short_desc.prependString(English, str);
}

void room::prependShortDesc(CSentryE msg) {
   for (int i = 0; i<LS_PER_ENTRY; i++) {
      LString nm((LanguageE)(i), CSHandler::getString(msg, (LanguageE)(i)));
      short_desc.prependString(nm);
   }
}

   /** Makes a copy of incoming data. */
void room::addShortDesc(LString& new_val) {
   short_desc.addLstring(new_val);
}

String* room::getShortDesc(critter* observer) {
   if (detect(observer->getSeeBit(), vis_bit)) {
      return short_desc.getString(observer->getLanguage());
   }
   else {
      return &UNKNOWN;
   }
}

String* room::getShortDesc(int see_bit) {
   if (detect(see_bit, vis_bit)) {
      return short_desc.getString(English);
   }
   else {
      return &UNKNOWN;
   }
}


int room::haveMinObj(int cnt, int obj_num) {
   SCell<object*> cll(inv);
   object* ptr;
   int count = 0;

   if (cnt == 0)
      return FALSE;

   while ((ptr = cll.next())) {
      count += ptr->getObjCountByNumber(obj_num, 0);

      if (ptr->getIdNum() == obj_num) {
         count++;
      }//if obj nums agree

      if (count >= cnt) {
         return TRUE;
      }//if
   }//while

   return FALSE;
}//haveObjNumbered


object* room::getObjNumbered(int cnt, int obj_num, critter& viewer) {
   SCell<object*> cll(inv);
   object* ptr;
   int count = 0;

   if (cnt == 0)
      return NULL;

   while ((ptr = cll.next())) {
      if (viewer.canDetect(*ptr)) {
         if (ptr->getIdNum() == obj_num) {
            count++;
            if (count == cnt) {
               return ptr;
            }//if
         }//if obj nums agree
      }
   }//while

   return NULL;
}//getObjNumbered
