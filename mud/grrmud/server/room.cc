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


int KeywordPair::_cnt = 0;

void KeywordPair::show(int idx, critter& pc) {
   String buf(100);
   Sprintf(buf, "Keyword [%i] ", idx);
   
   Cell<String*> cll(names);
   String* ptr;

   while ((ptr = cll.next())) {
      buf.Append(*ptr);
      buf.Append(" ");
   }//while

   buf.Append("\n\n");
   pc.show(buf);
   pc.show("\n");
   
   pc.show(desc);
   pc.show("\n");

   if (pc.isUsingClient()) {
      Sprintf(buf, "<KEYWORD %i ", idx);

      names.head(cll);
      while ((ptr = cll.next())) {
         buf.Append(*ptr);
         buf.Append(" ");
      }//while

      buf.Append(">\n");

      pc.show(buf);
      
      pc.show("<KEYWORD_DESC>");
      pc.show(desc);
      pc.show("</KEYWORD_DESC>");
   }//if
}//show
      

int KeywordPair::isNamed(const String* name) {
   Cell<String*> cll(names);
   String* ptr;

   while ((ptr = cll.next())) {
      if (strncasecmp(*name, *ptr, name->Strlen()) == 0) {
         return TRUE;
      }//if
   }//while

   return FALSE;
}//isNamed

void KeywordPair::clear() {
   clear_ptr_list(names);
   desc.Clear();
}

KeywordPair::~KeywordPair() {
   _cnt--;
   clear();
}

KeywordPair::KeywordPair(const KeywordPair& src) {
   _cnt++;
   *this = src;
}

KeywordPair& KeywordPair::operator=(const KeywordPair& rhs) {
   if (&rhs != this) {
      clear();
      
      Cell<String*> cll(rhs.names);
      String* ptr;

      while ((ptr = cll.next())) {
         names.append(new String(*ptr));
      }

      desc = rhs.desc;
   }//if

   return *this;
}//operator=


void KeywordPair::write(ofstream& dafile) {
   Cell<String*> cll(names);
   String* ptr;

   while ((ptr = cll.next())) {
      dafile << *ptr << " ";
   }//while

   dafile << "~" << endl;

   dafile << desc << "\n~\n";
}//write


void KeywordPair::read(ifstream& dafile) {
   String buf(80);
   char buf2[100];

   while (TRUE) {
      dafile >> buf;
      if (strcmp(buf, "~") == 0) {
         dafile.getline(buf2, 79);
         break;
      }
      else {
         names.append(new String(buf));
      }
   }//while

   desc.Termed_Read(dafile);
}//read


KeywordPair* room::haveKeyword(int i_th, const String* name) {
   int cnt = 0;
   Cell<KeywordPair*> cll(keywords);
   KeywordPair* ptr;

   while ((ptr = cll.next())) {
      if (ptr->isNamed(name)) {
         cnt++;
         if (cnt == i_th) {
            return ptr;
         }
      }//if
   }//while

   return NULL;
}//haveKeyword


int room::_cnt = 0;

room::room() {
   _cnt++;
   for (int i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = 0;
   }
   setVisBit(1024); //hack, forgot 'normal' vis bit earlier
} // sub_room constructor

room::room(int rm_num) {
   _cnt++;
   for (int i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = 0;
   }
   cur_stats[2] = rm_num;
   setVisBit(1024); //hack, forgot 'normal' vis bit earlier
} // sub_room constructor


void room::recursivelyLoad() { //mobs and objects
   Cell<critter*> ccll(critters);
   critter* cptr;
   
   while ((cptr = ccll.next())) {
      recursive_init_loads(*cptr);
      cptr->incrementCurInGame(); //recursive_... no longer does it.
   }//while
   
   Cell<object*> ocll(inv);
   object* optr;
   while ((optr = ocll.next())) {
      recursive_init_loads(*optr, 0);
   }//while
}//recursivelyLoad()


room::room(room& source) {
   _cnt++;
   *this = source; //overloaded = operator
} // sub_room copy constructor


room::~room() {
   _cnt--;

   if (!do_shutdown) {
      affected_rooms.loseData(this);
      pulsed_proc_rooms.loseData(this);
      embattled_rooms.loseData(this);
   }//if

   Clear();
}//sub_room deconstructor


room& room::operator=(room& source) {

   if (this == &source)
      return *this;

   Cell<stat_spell_cell*> cell;
   stat_spell_cell *tmp_stat, *tmp_stat2;
   int i;
   String* string;
   Cell<String*> cll(source.names);
   Cell<object*> obj_cll;
   object* obj_ptr;
   Cell<critter*> crit_cll;
   critter* crit_ptr;
   
   //mudlog.log(DBG, "In rm operator= overload.\n");
   Clear(); //clear this thing out!!
   
   while ((string = cll.next())) {
      Put(new String(*(string)), names);
   }//while
   
   short_desc = source.short_desc;
   long_desc = source.long_desc;
   
   room_flags = source.room_flags;

   Cell<KeywordPair*> kcll(source.keywords);
   KeywordPair* kptr;
   while ((kptr = kcll.next())) {
      keywords.append(new KeywordPair(*kptr));
   }
   
   for (i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = source.cur_stats[i];
   }

   clear_ptr_list(affected_by);
   source.affected_by.head(cell);
   while ((tmp_stat = cell.next())) {
      tmp_stat2 = new stat_spell_cell;
      *tmp_stat2 = *tmp_stat; //shallow copy should work 
      affected_by.append(tmp_stat2);
   }//while
   
   source.inv.head(obj_cll);
   while ((obj_ptr = obj_cll.next())) {
      if (!obj_ptr->IN_LIST) { //no multiple ptrs to SOBJ's
         inv.append(obj_ptr);
      }//if
   }//while

   Cell<door*> dcell(source.doors);
   door* dptr;
   while ((dptr = dcell.next())) {
      doors.append(new door(*dptr));
   }//while
   
   source.critters.head(crit_cll);
   while ((crit_ptr = crit_cll.next())) {
      if (crit_ptr->isMob()) { //only copy MOB's
         gainCritter(crit_ptr); //will increment cur_in_game
      }//if
   }//while

   return *this;
}//rm::operator= overload


void room::Clear() {
   int i;
   
   if (mudlog.ofLevel(TRC)) {
      mudlog << "room::Clear..." << endl;
   }

   clear_ptr_list(names);
   short_desc.Clear();
   long_desc.Clear();
   
   room_flags.Clear();

   clear_ptr_list(keywords);
   
   for (i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = 0;
   }

   clear_ptr_list(affected_by);
   clear_obj_list(inv);

   // Assume here that the mob is only on the affected_mobs list.
   // Otherwise, have to do a better cleanup to get em out of the game
   // cleanly.
   critter* tmp;
   while (!critters.isEmpty()) {
      tmp = critters.peekFront();
      removeCritter(tmp); //decrement cur_in_game
      if (tmp->isSmob()) {
         affected_mobs.loseData(tmp);
         delete tmp;
      }
   }//while

   clear_ptr_list(doors); //doors.clear();

   if (mudlog.ofLevel(TRC)) {
      mudlog << "room::Clear...  DONE." << endl;
   }

}// Clear


void room::Read(ifstream& ofile, short read_all) {
   int i, test = TRUE;
   stat_spell_cell* ss_ptr;
   char tmp[81];
   String tmp_str(80);
   String* string;
   door* dr_ptr;

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room, read_all:  " << read_all << endl;
   }

   Clear();  //stop up any memory leaks etc.

   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
      }
      return;
   }

   while (test) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in room read." << endl;
         }
         return;
      }

      ofile >> tmp_str;
      if (strcmp(tmp_str, "~") == 0) {
         test = FALSE;
      }//if
      else {
         string = new String(tmp_str);
         Put(string, names);
      }//else
   }//while            
   ofile.getline(tmp, 80);         

   short_desc.Termed_Read(ofile);
   //mudlog.log(DBG, short_desc);

   long_desc.Termed_Read(ofile);

   room_flags.Read(ofile);
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
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in room read." << endl;
         }
         return;
      }
      
      ss_ptr = new stat_spell_cell;
      ss_ptr->stat_spell = i;
      ofile >> ss_ptr->bonus_duration;
      Put(ss_ptr, affected_by);
      
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);


   if (!affected_by.isEmpty()) {
      // Also place it on the list of rooms to be checked for loss of spell...
      affected_rooms.append(this);
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
         return;
      }
      if (check_l_range(i, 0, NUMBER_OF_ITEMS, mob_list[0], FALSE)) {
         if (obj_list[i].OBJ_FLAGS.get(10)) {
            if (read_all || 
                ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 100) > 
                d(1,100)) {
               Put(&(obj_list[i]), inv);    //add it to inventory
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
   Cell<door*> cll;
   door* walker;
   short did_insert = FALSE;
   ofile >> i; //check for terminal value
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
         }
         return;
      }

      dr_ptr = new door;
      dr_ptr->Read(ofile);
      dr_ptr->in_room = cur_stats[2]; //RM_NUM
      doors.head(cll);
      String* dr_name = name_of_door(*dr_ptr, ~0);
      if (!dr_name) {
         mudlog.log(ERR, "ERROR:  dr_name is NULL.");
      }//if

      String* walk_name;
      while ((walker = cll.next())) {
	 walk_name = name_of_door(*walker, ~0);
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
         return;
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
}//Read


stat_spell_cell* room::isAffectedBy(int spell_num) {
   Cell<stat_spell_cell*> cll(affected_by);
   stat_spell_cell* ptr;
   
   while ((ptr = cll.next())) {
      if (ptr->stat_spell == spell_num)
         return ptr;
   }//while

   return NULL;
}//is_affected_by


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
         Cell<critter*> cll(critters);
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
            showAllCept("The room becomes dark.\n");
         }//if
         else {
            showAllCept("The room becomes light.\n");
         }//else
      }//if
   }//if do_messages
}//checkLight


void room::Write(ofstream& ofile) {
   int i, j;
   Cell<stat_spell_cell*> ss_cell(affected_by);
   stat_spell_cell* ss_ptr;
   Cell<String*> n_cell(names);
   String* n_ptr;
   Cell<door*> d_cell(doors);
   door* d_ptr;
   Cell<object*> ob_cell(inv);
   object* ob_ptr;
   Cell<critter*> c_cell(critters);
   critter* c_ptr;
   short was_totally_read = FALSE;

   normalize(); //make sure everything jives!
   
   if (room_flags.get(22)) { //totaly read flag
      was_totally_read = TRUE;
      room_flags.turn_off(22); // this flag is purely temp, don't write
   }//if

   while ((n_ptr = n_cell.next())) {
      ofile << *n_ptr << " ";
   }//while            
   ofile << "~" << "\tnames\n";

   parse_for_max_80(short_desc);
   ofile << short_desc << endl << "~" << endl;
   parse_for_max_80(long_desc);
   ofile << long_desc << endl << "~" << endl;

   room_flags.Write(ofile);

   //mudlog << "Keywords size:  " << keywords.size() << endl;
   
   if (room_flags.get(32)) { //if has keywords
      Cell<KeywordPair*> kcll(keywords);
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

		/*  Affected By */
   while ((ss_ptr = ss_cell.next())) {
      ofile << ss_ptr->stat_spell << " " << ss_ptr->bonus_duration << " ";
      if ((i + 1) % 20 == 0)
         ofile << endl;
   }//while
   ofile << -1 << "\taffected_by\n";



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
     if (d_ptr->dr_data->door_data_flags.get(10)) {
       ofile << 1 << " "; //test for next, will read untill -1
       d_ptr->Write(ofile);
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
}//Write....sub_room


void room::checkForProc(String& cmd, String& arg1, critter& actor,
                        int targ) {
   Cell<critter*> cll(critters);
   critter* ptr;
   //mudlog.log("room::checkForProc.");
   while ((ptr = cll.next())) {
      if (!ptr->pc) { //SMOB (not a PC)
         //mudlog.log("Wasn't a pc..");
         if (ptr->mob->mob_data_flags.get(17)) { //ok then, it has data
            if (ptr->isMob()) { //if it's a MOB
               //mudlog.log("Doing mob_to_smob..");
               ptr = mob_to_smob(*ptr, getRoomNum());
            }
            ptr->checkForProc(cmd, arg1, actor, targ, *this);
         }//if
      }//if
   }//while
}//checkForMobAction



int room::getZoneNum() {
   return ZoneCollection::instance().getZoneFor(*this).getIdNum();
}

void room::stat(critter& pc) {
   String buf2(100);
   int i, k;

   pc.show("\nRoom Flag definitions:  \n");
   pc.show("     0 no_rest, 1 !imm, 2 !god, 3 is_perm_dark, 4 weather,
     5 !shout, 6 !mag_exit, 7 is_haven(!kill_mob and !pk), 8 !pk, 9 !magic, 10 !mob,
     11 !potions, 12 !staffs, 13 !mort 14 normally_dark,
     15 river/small lake, 16 ocean/big lake, 17 swamp, 18 need_fly,
     19 need_boat, 20 need_climb 21 is_zlocked (for olc),
     22 is_total_loaded, 23 is_used, 24 !magic_entry
     25 !vehicle (vehicles can't drive here), 26 cramped (!huge)
     27 !ranged, 28 need_dive_ability, 29 used_in_track, 30 can_camp
     31 !complete, 32 has_keywords, 33 !mob_wander 34 !foreign_mob_wander\n\n");

   if (names.peekFront()) {
      show(*(names.peekFront()), pc);
   }//if

   Sprintf(buf2, "  Belongs to zone:  %i.\n", getZoneNum());
   show(buf2, pc);

   show(short_desc, pc);
   show("\n", pc);
   show(long_desc, pc);
   pc.show("\n");

   Cell<KeywordPair*> cll(keywords);
   KeywordPair* ptr;
   int cnt = 0;

   while ((ptr = cll.next())) {
      ptr->show(cnt, pc);
      cnt++;
   }

   show("\nRoom Flags set:", pc);
   k = room_flags.max_bit();
   for (i = 0; i <= k; i++) {
      if (room_flags.get(i)) {
         Sprintf(buf2, "%i ", i);
         show(buf2, pc);
      }//if
   }//for
   show("\n", pc);
   Sprintf(buf2, "v_bit: %i  mv$: %i  r_num: %i.\n", 
           getVisBit(), getMovCost(), getRoomNum());
   show(buf2, pc);

   Sprintf(buf2, "Number of critters:  %i\n", critters.size());
   show(buf2, pc);
}//stat


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

   Cell<door*> cll(doors);
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

int room::isInUse() const {
   return (room_flags.get(23) || critters.size());
}

int room::getCritCount(critter& pc) {
   return crit_count(critters, pc);
}


critter* room::findNextProcMob() {
   return find_next_proc_mob(critters);
}

/* this returns the critter if exists.  rm_num is the number of
 * the room we found it in.  Returns NULL if not found.
 * searches entire world. */
critter* room::haveCritNamedInWorld(const int i_th, const String* name,
                                    const int see_bit, int& rm_num) {
   Cell<String*> char_cell;
   Cell<critter*> cell;
   critter* crit_ptr;
   int count = 0, len;
   String *string;

   mudlog.log(DBG, "In have_crit_named, for whole room_list");

   if ((len = name->Strlen()) == 0) 
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
               crit_ptr->names.head(char_cell);
               while ((string = char_cell.next())) {
                  if (strncasecmp(*string, *name, len) == 0){ 
                     count++;
                     if (count == i_th) {
                        rm_num = i;
                        return crit_ptr;
                     }//if
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
   Cell<String*> char_cell;
   Cell<critter*> cell;
   critter* crit_ptr;
   int count = 0, len;
   String *string;

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
            crit_ptr->names.head(char_cell);
            while ((string = char_cell.next())) {
               if (strncasecmp(*string, *name, len) == 0){ 
                  count++;
                  if (count == i_th) {
                     in_room = i;
                     return crit_ptr;
                  }//if
               }//if
            }//while
         }//if
      }//while
   }//for
   in_room = 0;
   return NULL;
}//have_crit_named for whole zone


void room::doPoofOut(critter& pc) {
   String buf(100);
   Cell<critter*> cll(critters);
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


void room::doRclear(int new_rm_num) {
   critter* crit_ptr;

   while (!critters.isEmpty()) {
      crit_ptr = critters.peekFront();

      if ((crit_ptr->isPc()) || crit_ptr->isSmob()) {
         //mudlog << "Was a SMOB or PC." << endl;
         int is_dead;
         crit_ptr->doGoToRoom(new_rm_num, NULL, NULL, is_dead, getIdNum());
         if (!is_dead) {
            show("The void has swallowed your previous location!!\n", 
                 *crit_ptr);
         }
      }
      else {
         //just remove the pointer...
         removeCritter(crit_ptr);
      }//else
   }//while

   Clear();  //clear out the room pc WAS in!!
}//doRclear


void room::doScan(critter& pc) {
   String buf(100);
   Cell<door*> dcll(doors);
   Cell<critter*> cll;
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
}//doScan

void room::doPoofIn(critter& pc) {
   String buf(100);
   Cell<critter*> cll(critters);
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
   Cell<critter*> cll(critters);
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
      Cell<critter*> cll(critters);
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
                    int see_bit) {
   return crit_sub_a_4_b(crit_ptr, critters, i_th, 
                         &name, see_bit, *this);
}

int room::sub_a_4_b(critter* a, critter* b, int i_th) {
   return critters.substituteData(b, a, i_th);
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

   crit->setCurRoomNum(getRoomNum());
}

void room::loseObjectFromGame(object& which_un) {
   Cell<object*> ocll(inv);
   object* optr;
   Cell<critter*> ccll;
   critter* cptr;

   int targ_num = which_un.getIdNum();

   optr = ocll.next();
   while (optr) {
      if (optr->IN_LIST && (optr->getIdNum() == targ_num)) { //if SOBJ
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
      if (mudlog.ofLevel(WRN)) {
         mudlog << "WARNING:  could not remove the critter in"
                << " room::removeCrit(), crit_name: " << *(crit->getName()) 
                << " num: " << crit->getIdNum() << " room: "
                << getIdNum() << endl;
         core_dump("removeCritter");
      }//if
   }//else

   return retval;
}//removeCritter


void room::getPetsFor(critter& owner, List<critter*>& rslts) {
   critter* ptr;
   Cell<critter*> cll(critters);
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
   inv.prepend(obj);
   if (obj->IN_LIST)
      obj->IN_LIST = &inv;
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

critter* room::haveCritter(critter* ptr) {
   if (critters.haveData(ptr)) {
      return ptr;
   }
   return NULL;
}


object* room::haveObjNamed(int i_th, const String* name, int see_bit) {
   return ::have_obj_named(inv, i_th, name, see_bit, *this);
}

void room::showAllCept(const char* msg, critter* pc) const {
   Cell<critter*> cell(critters);
   critter* crit_ptr;

   while ((crit_ptr = cell.next())) { 
      if (crit_ptr != pc)  
	 if (crit_ptr->POS < POS_SLEEP)  
	    show(msg, *crit_ptr);
   }//while

   Cell<object*> cll(inv);
   object* obj;

   while ((obj = cll.next())) {
     if (obj->ob->obj_proc && (crit_ptr = obj->ob->obj_proc->w_eye_owner)) {
       if (crit_ptr != pc) {
	 if (crit_ptr->POS < POS_SLEEP) {
	   show("#####", *crit_ptr);
	   show(msg, *crit_ptr);
	 }//if
       }//if
     }//if
   }//while
}// show_all_cept
