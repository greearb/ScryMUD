// $Id$
// $Revision: 1.39 $  $Author$ $Date$

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

///**********************  object.cc  ******************************///


#include "object.h"
#include "misc.h"
#include "misc2.h"
#include "const.h"
#include "batl_prc.h"
#include "load_wld.h"
#include "battle.h"


//*************************************************************//
///****************** obj construct data  ********************///

int obj_construct_data::_cnt = 0;

obj_construct_data::obj_construct_data() {
   _cnt++;
   target_object = item1 = item2 = item3 = item4 = item5 = 0;
}//constructor

obj_construct_data::obj_construct_data(const obj_construct_data& source) {
   _cnt++;
   target_object = source.target_object;
   item1 = source.item1;
   item2 = source.item2;
   item3 = source.item3;
   item4 = source.item4;
   item5 = source.item5;
}//copy constructor

void obj_construct_data::Read(ifstream& da_file) {
   char buf[100];

   if (!da_file) {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  da_file FALSE in obj_construct_data read." << endl;
      }
      return;
   }

   da_file >> target_object >> item1 >> item2 >> item3 >> item4 >> item5;
   da_file.getline(buf, 99);
}//Read

void obj_construct_data::Write(ofstream& da_file) const {
   da_file << target_object << " " << item1 << " " << item2 << " "
           << item3 << " " << item4 << " " << item5;
   da_file << "\t target object, companions\n";
}//Write


//************************************************************//
///********************  obj spec data  *********************///

int obj_spec_data::_cnt = 0;

obj_spec_data::obj_spec_data() { //constructor
   _cnt++;
   construct_data = NULL;
   skin_ptr = NULL;
   head_ptr = NULL;
   w_eye_owner = NULL;
}//constructor

obj_spec_data::obj_spec_data(const obj_spec_data& source) { //copy constructr
   _cnt++;
   construct_data = NULL;
   skin_ptr = NULL;
   head_ptr = NULL;
   w_eye_owner = NULL;
   *this = source;
}//constructor

obj_spec_data::~obj_spec_data() { //destructor
   _cnt--;
   delete construct_data;
   if (skin_ptr && skin_ptr->IN_LIST) {
      delete skin_ptr;
      skin_ptr = NULL;
   }//if
   if (head_ptr && head_ptr->IN_LIST) {
      delete head_ptr;
      head_ptr = NULL;
   }//if
   construct_data = NULL;
   if (w_eye_owner) {
     w_eye_owner->pc->w_eye_obj = NULL;
     w_eye_owner = NULL;
   }//if
   casts_these_spells.clearAndDestroy();
}//destructor

obj_spec_data& obj_spec_data::operator=(const obj_spec_data& source) { 

   if (this == &source)
      return *this;

   Clear();  //clear current mess
   obj_spec_data_flags = source.obj_spec_data_flags;
   if (source.construct_data) {
      construct_data = new obj_construct_data(*(source.construct_data));
   }//if

   if (source.skin_ptr && !source.skin_ptr->IN_LIST) {
      skin_ptr = source.skin_ptr; //don't copy ptrs to SOBJ's
   }//if
   else {
      skin_ptr = NULL;
   }//else

   if (source.head_ptr && !source.head_ptr->IN_LIST) {
      head_ptr = source.head_ptr;
   }//if
   else {
      head_ptr = NULL;
   }//else

   w_eye_owner = source.w_eye_owner;

   casts_these_spells.clearAndDestroy();
   Cell<stat_spell_cell*> cll(source.casts_these_spells);
   stat_spell_cell* ptr;
   while ((ptr = cll.next())) {
      casts_these_spells.append(new stat_spell_cell(*ptr));
   }//while
   return *this;
}// operator= overload

void obj_spec_data::Clear() {
   obj_spec_data_flags.Clear();
   delete construct_data;
   construct_data = NULL;
   
   if (skin_ptr && skin_ptr->IN_LIST) {
      delete skin_ptr;
   }//if
   skin_ptr = NULL;

   if (head_ptr && head_ptr->IN_LIST) {
      delete head_ptr;
   }//if
   head_ptr = NULL;


   w_eye_owner = NULL;
   casts_these_spells.clearAndDestroy();
}//clear


void obj_spec_data::Read(ifstream& da_file) {
   char buf[82];
   int tmp;

   Clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  da_file FALSE in obj_spec_data read." << endl;
      }
      return;
   }

   obj_spec_data_flags.Read(da_file);

   obj_spec_data_flags.turn_off(4); //hack!!
   obj_spec_data_flags.turn_off(5);
   obj_spec_data_flags.turn_off(6);
   obj_spec_data_flags.turn_off(7);
   obj_spec_data_flags.turn_off(8);
   obj_spec_data_flags.turn_off(9);

   if (obj_spec_data_flags.get(1)) { //if have construct data
      if (!construct_data) {
         construct_data = new obj_construct_data;
      }//if
      construct_data->Read(da_file);
   }//if

   if (obj_spec_data_flags.get(2)) {
      da_file >> tmp;
      if (check_l_range(tmp, 1, NUMBER_OF_ITEMS, mob_list[0], FALSE)) {
         skin_ptr = &(obj_list[tmp]);
      }//if
      else {
         skin_ptr = NULL;
         obj_spec_data_flags.turn_off(2); //doesn't have one after all
      }//else
      da_file.getline(buf, 80);
   }//if
   
   if (obj_spec_data_flags.get(10)) {
      int first, second;
      da_file >> first;
      while (first != -1) {
         da_file >> second;
         casts_these_spells.append(new stat_spell_cell(first, second));
         da_file >> first;
      }//while
      da_file.getline(buf, 80);
   }//if
}//Read


void obj_spec_data::Write(ofstream& da_file) const {
   obj_spec_data_flags.Write(da_file);
   if (obj_spec_data_flags.get(1)) { //if have construct data
      if (!construct_data) {
         mudlog.log(ERROR, "ERROR:  trying to write NULL construct data.\n");
         return;
      }//if
      construct_data->Write(da_file);
   }//if
   if (obj_spec_data_flags.get(2)) {
     if (!skin_ptr) {
       da_file << 0 << "     skin number\n";
     }//if
     else {
       da_file << skin_ptr->OBJ_NUM << "        skin number\n";
     }//else
   }//if

   if (obj_spec_data_flags.get(10)) {
     Cell<stat_spell_cell*> cll(casts_these_spells);
     stat_spell_cell* ptr;
     int cnt = 0;
     while ((ptr = cll.next())) {
       if (++cnt > 15) {
         cnt = 0;
         da_file << "\n";
       }//if
       da_file << ptr->stat_spell << " " << ptr->bonus_duration << " ";
     }//while
     da_file << "-1 spell#, level (casts)\n";
   }//if
}//Write


//******************************************************************//
///***********************  bag struct  ***************************///

int bag_struct::_cnt = 0;

bag_struct::bag_struct() {
   _cnt++;
}//constructor

bag_struct::bag_struct(const bag_struct& source) {
   _cnt++;
   *this = source; //a shallow copy should work
}//copy constructor

void bag_struct::Write(ofstream& ofile) const {
   bag_flags.Write(ofile);
   ofile << key_num << " " << max_weight << " " << percentage_weight << " "
         << time_till_disolve << " " << "\t bag_stuff\n";
}//Write()


void bag_struct::Read(ifstream& ofile) {
   char buf[81];
   
   bag_flags.Read(ofile);
   ofile >> key_num >> max_weight >> percentage_weight 
         >> time_till_disolve;
   ofile.getline(buf, 80);
}//Read()


//*************************************************************//
///**********************  object  ***************************///

int object::_cnt = 0;

object::object (object& source) { //copy constructor
   _cnt++;
   pause = source.pause;
   in_room = source.in_room;
   cur_script = NULL;

   in_list = NULL;
   bag = NULL;
   obj_proc = NULL;

   *this = source;

   obj_ptr_log << "OBJ_CC " << getIdNum() << " " << this << "\n";
}//obj copy constructor


object::object() {
   int i;

   _cnt++;
   pause = 0;
   in_room = 0;
   cur_script = NULL;
  
   for (i = 0; i<OBJ_MAX_EXTRAS; i++)
      extras[i] = 0;       
   for (i = 0; i<OBJ_CUR_STATS; i++)
      cur_stats[i] = 0;    
   
   in_list = NULL;
   bag = NULL;
   obj_proc = NULL;
   
   // Can't have an obj# at this point.
   obj_ptr_log << "OBJ_CON 0 " << this << "\n";
}//default constructor


object::~object() {
   _cnt--;

   obj_ptr_log << "OBJ_DES " << getIdNum() << " " << this << "\n";

   if ((!in_list) && (!do_shutdown)) {
      mudlog.log(ERROR, "ERROR:  trying to delete OBJ before shutdown.\n");
      //do_shutdown = TRUE;
      //exit(2);
   }//if

   if (!do_shutdown) {
      affected_objects.loseData(this);
      pulsed_proc_objects.loseData(this);
      proc_action_objs.loseData(this);
   }//if

   Clear();
}//destructor


object& object::operator= (object& source) {

   if (this == &source)
      return *this;
   
   Cell<stat_spell_cell*> cell;
   stat_spell_cell *tmp_stat, *tmp_stat2;
   Cell<object*> cll;
   object* obj_ptr;      
   int i;
   
   if (!in_list && obj_flags.get(10)) {
      mudlog.log(ERROR, "ERROR:  OBJ being assigned to... should be a SOBJ!\n");
      //do_shutdown = TRUE;
      //exit(2);
   }//if
   
   Clear();
   
   pause = source.pause;
   in_room = source.in_room;

   String* ptr;
   Cell<String*> scll(source.names);
   while ((ptr = scll.next())) {
      names.append(new String(*ptr));
   }//while
   
   short_desc = source.short_desc;
   in_room_desc = source.in_room_desc;
   long_desc = source.long_desc;
   
   in_list = source.in_list;
   
   obj_flags = source.obj_flags; //use bitfield's overloaded =

   if (!obj_flags.get(10)) { //in use
      mudlog << "ERROR:  assigning obj, flag 10 (in use) not set."
             << endl;
      obj_flags.turn_on(10);
   }
   
   for (i = 0; i<OBJ_MAX_EXTRAS; i++) 
      extras[i] = source.extras[i];
   
   for (i = 0; i<OBJ_CUR_STATS; i++) 
      cur_stats[i] = source.cur_stats[i];
   
   if (source.bag) {
      bag = new bag_struct(*(source.bag));
   }//if bag
   
   source.affected_by.head(cell);
   while ((tmp_stat = cell.next())) {
      tmp_stat2 = new stat_spell_cell;
      *tmp_stat2 = *tmp_stat; //shallow copy should work 
      affected_by.append(tmp_stat2);
   }//while
   
   source.inv.head(cll);
   while ((obj_ptr = cll.next())) {
      if (!obj_ptr->IN_LIST) { //multiple ptrs to SOBJ's will be a problem
         inv.append(obj_ptr);
      }//if
   }//while
   
   source.stat_affects.head(cell);
   while ((tmp_stat = cell.next())) {
      tmp_stat2 = new stat_spell_cell;
      *tmp_stat2 = *tmp_stat; //shallow copy should work 
      stat_affects.append(tmp_stat2);
   }//while
   
   if (source.obj_proc) {
      obj_proc = new obj_spec_data(*(source.obj_proc));
   }//if

   // Don't copy pending scripts, can't see any reason to.
   Cell<ObjectScript*> oscll(source.obj_proc_scripts);
   ObjectScript* osptr;
   
   while ((osptr = oscll.next())) {
      obj_proc_scripts.append(new ObjectScript(*osptr));
   }
   
   return *this;
}//operator= overload
            

void object::Clear() {
   pause = 0;
   in_room = 0;

   if (cur_script) {
      pending_scripts.loseData(cur_script);
      delete cur_script;
      cur_script = NULL;
   }

   names.clearAndDestroy();
   short_desc.Clear();
   in_room_desc.Clear();
   long_desc.Clear();
   
   obj_flags.Clear();
   
   delete bag;
   bag = NULL;
   in_list = NULL;
   
   affected_by.clearAndDestroy();
   stat_affects.clearAndDestroy();
   clear_obj_list(inv);
   
   delete obj_proc;
   obj_proc = NULL;

   obj_proc_scripts.clearAndDestroy();
   pending_scripts.clearAndDestroy();
   delete cur_script;
   cur_script = NULL;

}//Clear


int object::getCurWeight() const {
   if (inv.isEmpty()) {
      return extras[5];
   }
   else {
      Cell<object*> cll(inv);
      object* tmp_obj;
      int tmp_wt = 0;
      while ((tmp_obj = cll.next())) {
         tmp_wt += tmp_obj->getCurWeight();
      }//while
      return (tmp_wt * (this->bag->percentage_weight/100)) + extras[5];  // weight of inv, plus container's wt
   }//else
}//cur_weight

int object::getMaxWeight() const {
   if (bag) 
      return bag->max_weight;
   else
      return getMaxWeight();
}//max_weight


void object::Write(ofstream& ofile) {
   int i;
   Cell<stat_spell_cell*> ss_cell;
   stat_spell_cell* ss_ptr;
   Cell<object*> ob_cell;
   object* ob_ptr;
   Cell<String*> st_cell(names);
   String* st_ptr;
   
   int len_writ = 0;
   while ((st_ptr = st_cell.next())) {
      len_writ += st_ptr->Strlen() + 1;
      if (len_writ > 79) {
         ofile << endl;
         len_writ = 0;
      }//if
      ofile << *st_ptr << " ";
   }//while
   ofile << "~" << "\tnames\n";
   
   parse_for_max_80(short_desc);
   ofile << short_desc << endl << "~" << endl;
   parse_for_max_80(in_room_desc);
   ofile << in_room_desc << endl << "~" << endl;
   parse_for_max_80(long_desc);
   ofile << long_desc << endl << "~" << endl;
   
   obj_flags.Write(ofile);
   
   int n_game = extras[3];
   extras[3] = 0;
   for (i = 0; i<OBJ_MAX_EXTRAS; i++)
      ofile << extras[i] << " ";
   ofile << "\textras\n";
   extras[3] = n_game;
   
   for (i = 0; i<OBJ_CUR_STATS; i++)
      ofile << cur_stats[i] << " ";
   ofile << "\tcur_stats\n";
   
   /*  Do Bag      */
   if (bag) 
      bag->Write(ofile);
   
   
   /*  Affected By */
   i = 0;
   affected_by.head(ss_cell);
   while ((ss_ptr = ss_cell.next())) {
      ofile << ss_ptr->stat_spell << " " << ss_ptr->bonus_duration << " ";
      if ((++i % 20) == 0)
         ofile << endl;
   }//while
   ofile << -1 << "\taffected_by\n";
   
   /*  Inventory */
   
   if (!obj_flags.get(74)) { //don't write out inv of a board
      i = 0;
      inv.head(ob_cell);
      while ((ob_ptr = ob_cell.next())) {
         if (ob_ptr->IN_LIST) {
            ofile << -2 << "\t Start of SOBJ\n";
            ob_ptr->Write(ofile);
         }//if
         else {
            ofile << ob_ptr->OBJ_NUM << " ";
            if ((++i % 20) == 0)
               ofile << endl;
         }//else
      }//while
   }//if not a bulletin board
   ofile << -1 << "\tinv\n";
   
   
   /*  Stat_Affects */
   i = 0;
   stat_affects.head(ss_cell);
   while ((ss_ptr = ss_cell.next())) {
      ofile << ss_ptr->stat_spell << " " << ss_ptr->bonus_duration << " ";
      if ((++i % 20) == 0)
         ofile << endl;
   }//while
   ofile << -1 << "\tstat_affects\n";  //done
   
   if (obj_flags.get(63)) { //if has spec_data
      if (!obj_proc) {
         mudlog.log(ERROR, "ERROR:  trying to write a NULL obj_proc.\n");
         return;
      }//if
      obj_proc->Write(ofile);
   }//if has spec_data

   if (obj_flags.get(76)) {
      Cell<ObjectScript*> cll;
      obj_proc_scripts.head(cll);
      ObjectScript* ptr;

      int i = 1;
      while ((ptr = cll.next())) {
         ofile << i++ <<  "  Start of a object proc script\n";
         ptr->write(ofile);
      }
      
      ofile << "-1  End of object proc scripts" << endl;
   }

   ofile << "End of Object.\n\n";
}//Write...obj

#ifdef USEMYSQL
void object::dbRead(long obj_num, long sobj_num, short read_all) {
   long l;
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select * from Objects where OBJ_NUM=";
   String name;
   query += obj_num;
   query += " and SOBJ_NUM=";
   query += sobj_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In object::dbRead(long, long, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      row=mysql_fetch_row(result);
      l=atol(row[OBJTBL_SOBJ_NUM]);
      if (l != -1 && !isModified()) {
         // Trying to read data for an SOBJ into an OBJ
         if (mudlog.ofLevel(DIS)) {
            mudlog << "In object::dbRead(long, long, short):\n";
            mudlog << "Attempting to read data for an SOBJ into an OBJ.\n";
         }
         core_dump("ERROR: Won't load data for an SOBJ into an OBJ.\n");
      }

      Clear();

      // Basic stuff
      OBJ_NUM = obj_num;

      short_desc = row[OBJTBL_SHORT_DESC];
      in_room_desc = row[OBJTBL_IN_ROOM_DESC];
      long_desc = row[OBJTBL_LONG_DESC];

      // Cripes, we definately need to make obj_flags private and write
      // functions to set each flag.
      obj_flags.set(OBJFLAG_NO_REST, atol(row[OBJTBL_NO_REST]));
      obj_flags.set(OBJFLAG_ANTI_EVIL, atol(row[OBJTBL_ANTI_EVIL]));
      obj_flags.set(OBJFLAG_ANTI_NEUTRAL, atol(row[OBJTBL_ANTI_NEUTRAL]));
      obj_flags.set(OBJFLAG_ANTI_GOOD, atol(row[OBJTBL_ANTI_GOOD]));
      obj_flags.set(OBJFLAG_ANTI_DONATE, atol(row[OBJTBL_ANTI_DONATE]));
      obj_flags.set(OBJFLAG_ANTI_DROP, atol(row[OBJTBL_ANTI_DROP]));
      obj_flags.set(OBJFLAG_ANTI_REMOVE, atol(row[OBJTBL_ANTI_REMOVE]));
      obj_flags.set(OBJFLAG_ANTI_MORTAL, atol(row[OBJTBL_ANTI_MORTAL]));
      obj_flags.set(OBJFLAG_ANTI_IMMORTAL, atol(row[OBJTBL_ANTI_IMMORTAL]));
      obj_flags.set(OBJFLAG_ANTI_DEMI, atol(row[OBJTBL_ANTI_DEMI]));
      obj_flags.set(OBJFLAG_IN_USE, atol(row[OBJTBL_IN_USE]));
      obj_flags.set(OBJFLAG_ANTI_WARRIOR, atol(row[OBJTBL_ANTI_WARRIOR]));
      obj_flags.set(OBJFLAG_ANTI_SAGE, atol(row[OBJTBL_ANTI_SAGE]));
      obj_flags.set(OBJFLAG_ANTI_WIZARD, atol(row[OBJTBL_ANTI_WIZARD]));
      obj_flags.set(OBJFLAG_ANTI_RANGER, atol(row[OBJTBL_ANTI_RANGER]));
      obj_flags.set(OBJFLAG_ANTI_THIEF, atol(row[OBJTBL_ANTI_THIEF]));
      obj_flags.set(OBJFLAG_ANTI_ALCHEMIST, atol(row[OBJTBL_ANTI_ALCHEMIST]));
      obj_flags.set(OBJFLAG_ANTI_CLERIC, atol(row[OBJTBL_ANTI_CLERIC]));
      obj_flags.set(OBJFLAG_ANTI_BARD, atol(row[OBJTBL_ANTI_BARD]));
      obj_flags.set(OBJFLAG_ANTI_MOB, atol(row[OBJTBL_ANTI_MOB]));
      obj_flags.set(OBJFLAG_ANTI_PC, atol(row[OBJTBL_ANTI_PC]));
      obj_flags.set(OBJFLAG_NOT_WORN, atol(row[OBJTBL_NOT_WORN]));
      obj_flags.set(OBJFLAG_HEAD, atol(row[OBJTBL_HEAD]));
      obj_flags.set(OBJFLAG_NECK1, atol(row[OBJTBL_NECK1]));
      obj_flags.set(OBJFLAG_NECK2, atol(row[OBJTBL_NECK2]));
      obj_flags.set(OBJFLAG_AROUND_BODY, atol(row[OBJTBL_AROUND_BODY]));
      obj_flags.set(OBJFLAG_ARMS, atol(row[OBJTBL_ARMS]));
      obj_flags.set(OBJFLAG_WRIST1, atol(row[OBJTBL_WRIST1]));
      obj_flags.set(OBJFLAG_WRIST2, atol(row[OBJTBL_WRIST2]));
      obj_flags.set(OBJFLAG_HANDS, atol(row[OBJTBL_HANDS]));
      obj_flags.set(OBJFLAG_WIELDED, atol(row[OBJTBL_WIELDED]));
      obj_flags.set(OBJFLAG_HELD, atol(row[OBJTBL_HELD]));
      obj_flags.set(OBJFLAG_LIGHT, atol(row[OBJTBL_LIGHT]));
      obj_flags.set(OBJFLAG_BODY, atol(row[OBJTBL_BODY]));
      obj_flags.set(OBJFLAG_BELT, atol(row[OBJTBL_BELT]));
      obj_flags.set(OBJFLAG_LEGS, atol(row[OBJTBL_LEGS]));
      obj_flags.set(OBJFLAG_FEET, atol(row[OBJTBL_FEET]));
      obj_flags.set(OBJFLAG_FINGER1, atol(row[OBJTBL_FINGER1]));
      obj_flags.set(OBJFLAG_FINGER2, atol(row[OBJTBL_FINGER2]));
      obj_flags.set(OBJFLAG_SHIELD, atol(row[OBJTBL_SHIELD]));
      obj_flags.set(OBJFLAG_NOT_A_WEAPON, atol(row[OBJTBL_NOT_A_WEAPON]));
      obj_flags.set(OBJFLAG_SLASH, atol(row[OBJTBL_SLASH]));
      obj_flags.set(OBJFLAG_SMASH, atol(row[OBJTBL_SMASH]));
      obj_flags.set(OBJFLAG_PIERCE, atol(row[OBJTBL_PIERCE]));
      obj_flags.set(OBJFLAG_WHIP, atol(row[OBJTBL_WHIP]));
      obj_flags.set(OBJFLAG_NEEDS_AMMO, atol(row[OBJTBL_NEEDS_AMMO]));
      obj_flags.set(OBJFLAG_DART_THROWER, atol(row[OBJTBL_DART_THROWER]));
      obj_flags.set(OBJFLAG_BOW, atol(row[OBJTBL_BOW]));
      obj_flags.set(OBJFLAG_DART, atol(row[OBJTBL_DART]));
      obj_flags.set(OBJFLAG_ARROW, atol(row[OBJTBL_ARROW]));
      obj_flags.set(OBJFLAG_JUNK, atol(row[OBJTBL_JUNK]));
      obj_flags.set(OBJFLAG_WAND, atol(row[OBJTBL_WAND]));
      obj_flags.set(OBJFLAG_POTION, atol(row[OBJTBL_POTION]));
      obj_flags.set(OBJFLAG_SCROLL, atol(row[OBJTBL_SCROLL]));
      obj_flags.set(OBJFLAG_CONTAINER, atol(row[OBJTBL_CONTAINER]));
      obj_flags.set(OBJFLAG_COINS, atol(row[OBJTBL_COINS]));
      obj_flags.set(OBJFLAG_ARMOR, atol(row[OBJTBL_ARMOR]));
      obj_flags.set(OBJFLAG_WEAPON, atol(row[OBJTBL_WEAPON]));
      obj_flags.set(OBJFLAG_LIGHT_SOURCE, atol(row[OBJTBL_LIGHT_SOURCE]));
      obj_flags.set(OBJFLAG_CANTEEN, atol(row[OBJTBL_CANTEEN]));
      obj_flags.set(OBJFLAG_LIQUID, atol(row[OBJTBL_LIQUID]));
      obj_flags.set(OBJFLAG_FOOD, atol(row[OBJTBL_FOOD]));
      obj_flags.set(OBJFLAG_BOAT, atol(row[OBJTBL_BOAT]));
      obj_flags.set(OBJFLAG_HAS_SPEC_PROC_DATA, atol(row[OBJTBL_HAS_SPEC_PROC_DATA]));
      obj_flags.set(OBJFLAG_TOOLBOX,atol(row[OBJTBL_TOOLBOX]));
      obj_flags.set(OBJFLAG_CAULDRON, atol(row[OBJTBL_CAULDRON]));
      obj_flags.set(OBJFLAG_PEN, atol(row[OBJTBL_PEN]));
      obj_flags.set(OBJFLAG_CONSTRUCT_COMPONENT, atol(row[OBJTBL_CONSTRUCT_COMPONENT]));
      obj_flags.set(OBJFLAG_CONCOCT_COMPONENT, atol(row[OBJTBL_CONCOCT_COMPONENT]));
      obj_flags.set(OBJFLAG_PARCHMENT, atol(row[OBJTBL_PARCHMENT]));
      obj_flags.set(OBJFLAG_NEEDS_RESETTING, atol(row[OBJTBL_NEEDS_RESETTING]));
      obj_flags.set(OBJFLAG_HERB, atol(row[OBJTBL_HERB]));
      obj_flags.set(OBJFLAG_VENDING_MACHINE, atol(row[OBJTBL_VENDING_MACHINE]));
      obj_flags.set(OBJFLAG_BULLETIN_BOARD, atol(row[OBJTBL_BULLETIN_BOARD]));
      obj_flags.set(OBJFLAG_IS_BUTCHERABLE, atol(row[OBJTBL_IS_BUTCHERABLE]));
      obj_flags.set(OBJFLAG_HAS_OBJ_SCRIPT, atol(row[OBJTBL_HAS_OBJ_SCRIPT]));

      obj_flags.turn_on(OBJFLAG_IN_USE);
      obj_flags.turn_off(OBJFLAG_NOT_COMPLETE);
      if (obj_flags.get(OBJFLAG_WIELDED))
         obj_flags.turn_on(OBJFLAG_WEAPON);

      extras[0] = atol(row[OBJTBL_CHARGES]);
      extras[1] = atol(row[OBJTBL_TIMES_RECHARGABLE]);
      extras[2] = atol(row[OBJTBL_PERCENT_LOAD]);
      extras[4] = atol(row[OBJTBL_MAX_IN_GAME]);
      extras[5] = atol(row[OBJTBL_WEIGHT]);
      extras[6] = atol(row[OBJTBL_DICE_SIDES]);
      extras[7] = atol(row[OBJTBL_DICE_COUNT]);
      extras[8] = atol(row[OBJTBL_LEVEL]);

      if (extras[0] < -1) 
         extras[0] = -1;

      cur_stats[0] = atol(row[OBJTBL_VIS_BIT]);
      cur_stats[1] = atol(row[OBJTBL_PRICE]);
      cur_stats[3] = atol(row[OBJTBL_FROM_ZONE]);

      // Bag stuff
      if (obj_flags.get(OBJFLAG_CONTAINER)) {
         bag = new bag_struct;
         bag->bag_flags.set(BAGFLAG_IS_CLOSED, atol(row[OBJTBL_IS_CLOSED]));
         bag->bag_flags.set(BAGFLAG_IS_LOCKED, atol(row[OBJTBL_IS_LOCKED]));
         bag->bag_flags.set(BAGFLAG_IS_PICKABLE, atol(row[OBJTBL_IS_PICKABLE]));
         bag->bag_flags.set(BAGFLAG_IS_MAG_LOCKABLE, atol(row[OBJTBL_IS_MAG_LOCKABLE]));
         bag->bag_flags.set(BAGFLAG_IS_MAG_LOCKED, atol(row[OBJTBL_IS_MAG_LOCKED]));
         bag->bag_flags.set(BAGFLAG_IS_DESTRUCTABLE, atol(row[OBJTBL_IS_DESCTRUCTABLE]));
         bag->bag_flags.set(BAGFLAG_IS_CORPSE, atol(row[OBJTBL_IS_CORPSE]));
         bag->bag_flags.set(BAGFLAG_NOT_CLOSABLE, atol(row[OBJTBL_NOT_CLOSABLE]));
         bag->bag_flags.set(BAGFLAG_PLAYER_OWNED, atol(row[OBJTBL_IS_PLAYER_OWNED]));
         bag->bag_flags.set(BAGFLAG_CONSUMES_KEY, atol(row[OBJTBL_CONSUMES_KEY]));

         bag->key_num           = atol(row[OBJTBL_KEY_NUM]);
         bag->max_weight        = atol(row[OBJTBL_MAX_WEIGHT]);
         bag->percentage_weight = atol(row[OBJTBL_PERCENT_WEIGHT]);
         bag->time_till_disolve = atol(row[OBJTBL_TIME_TILL_DISSOLVE]);
      }
      // Spec data
      if (obj_flags.get(OBJFLAG_HAS_SPEC_PROC_DATA)) {
         if (obj_proc->obj_spec_data_flags.get(2)) {
            if (check_l_range(atoi(row[OBJTBL_SKIN_NUMBER]), 1, NUMBER_OF_ITEMS,
               mob_list[0], FALSE)) {
               obj_proc->skin_ptr = &(obj_list[atoi(row[OBJTBL_SKIN_NUMBER])]);
            }
            else {
               obj_proc->skin_ptr = NULL;
               obj_proc->obj_spec_data_flags.turn_off(2);
            }
         }
      }
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In object::dbRead(long, long, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }
   
   // Names
   query="select NAME from ObjNames where OBJ_NUM=";
   query+=obj_num;
   query+=" and SOBJ_NUM=";
   query+=sobj_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In object::dbRead(long, long, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      while ((row=mysql_fetch_row(result))) {
         name = row[0];
         names.append(&name);
      }
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In object::dbRead(long, long, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }
   // Affected By
   query="select SPELL_NUM, DURATION from ObjSpells where OBJ_NUM=";
   query+=obj_num;
   query+=" and SOBJ_NUM=";
   query+=sobj_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In object::dbRead(long, long, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      stat_spell_cell* ss_ptr;
      while ((row=mysql_fetch_row(result))) {
         ss_ptr = new stat_spell_cell;
         ss_ptr->stat_spell = atoi(row[0]);
         ss_ptr->bonus_duration = atoi(row[1]);
         affected_by.append(ss_ptr);
      }
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In object::dbRead(long, long, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }
   // Inventory
   query="select CONTENT_OBJ_NUM, CONTENT_SOBJ_NUM, QUANTITY from ObjInv where OBJ_NUM=";
   query+=obj_num;
   query+=" and SOBJ_NUM=";
   query+=sobj_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In object::dbRead(long, long, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      int content_sobj, content_obj, quantity;
      while ((row=mysql_fetch_row(result))) {
         content_obj = atoi(row[0]);
         content_sobj = atoi(row[1]);
         quantity = atoi(row[2]);
         if (content_sobj != 0) {
            object* new_obj = new object;
            new_obj->dbRead(content_obj, content_sobj, read_all);
            inv.append(new_obj);
            affected_objects.append(new_obj);
         }
         else {
            if (obj_list[content_obj].isInUse()) {
               if (read_all || ((obj_list[content_obj].OBJ_PRCNT_LOAD *
                   config.currentLoadModifier) / 100) > d(1,100)) {
                  inv.append(&obj_list[content_obj]);
               }
            }
            else {
               inv.append(&obj_list[content_obj]);
            }
         }
      }
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In object::dbRead(long, long, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }
   // Stat Affects
   query="select STAT, ADJUSTMENT from ObjStatAff where OBJ_NUM=";
   query+=obj_num;
   query+=" and SOBJ_NUM=";
   query+=sobj_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In object::dbRead(long, long, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      stat_spell_cell* ss_ptr;
      while ((row=mysql_fetch_row(result))) {
         ss_ptr = new stat_spell_cell;
         ss_ptr->stat_spell = atoi(row[0]);
         ss_ptr->bonus_duration = atoi(row[1]);
         stat_affects.append(ss_ptr);
      }
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In object::dbRead(long, long, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }
   if (obj_proc) {
      // casts
      if (obj_proc->obj_spec_data_flags.get(10)) {
         String query="select SPELL_NUM, LEVEL from ObjCasts where OBJ_NUM=";
         query+=obj_num;
         query+=" and SOBJ_NUM=";
         query+=sobj_num;

         if (mysql_real_query(database, query, strlen(query))==0) {
            if ((result=mysql_store_result(database))==NULL) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "In object::dbRead(long, long, short):\n";
                  mudlog << "Error retrieving query results: "
                         << mysql_error(database) << endl;
               }
               return;
            }
            while ((row=mysql_fetch_row(result))) {
               obj_proc->casts_these_spells.append(new stat_spell_cell(
                        atoi(row[0]), atoi(row[1])));
            }
            mysql_free_result(result);
         }
         else {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In object::dbRead(long, long, short):\n";
               mudlog << "Error executing query: " << mysql_error(database)
                      << endl;
            }
            return;
         }
      }
      // constructs
      if (obj_proc->obj_spec_data_flags.get(1)) {
         if (!obj_proc->construct_data)
            obj_proc->construct_data = new obj_construct_data;

         query="select TARGET_OBJ_NUM from Componants where COMPONANT_OBJ_NUM=";
         query+=obj_num;

         if (mysql_real_query(database, query, strlen(query))==0) {
            if ((result=mysql_store_result(database))==NULL) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "In object::dbRead(long, long, short):\n";
                  mudlog << "Error retrieving query results: "
                         << mysql_error(database) << endl;
               }
               return;
            }
            row=mysql_fetch_row(result);
            obj_proc->construct_data->target_object = atoi(row[0]);
            mysql_free_result(result);
         }
         else {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In object::dbRead(long, long, short):\n";
               mudlog << "Error executing query: " << mysql_error(database)
                      << endl;
            }
            return;
         }
      }
      query="select COMPONANT_OBJ_NUM from Componants where TARGET_OBJ_NUM=";
      query+=obj_proc->construct_data->target_object;


      if (mysql_real_query(database, query, strlen(query))==0) {
         if ((result=mysql_store_result(database))==NULL) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In object::dbRead(long, long, short):\n";
               mudlog << "Error retrieving query results: "
                      << mysql_error(database) << endl;
            }
            return;
         }
         row=mysql_fetch_row(result);
         obj_proc->construct_data->item1 = atoi(row[0]);
         row=mysql_fetch_row(result);
         obj_proc->construct_data->item2 = atoi(row[0]);
         row=mysql_fetch_row(result);
         obj_proc->construct_data->item3 = atoi(row[0]);
         row=mysql_fetch_row(result);
         obj_proc->construct_data->item4 = atoi(row[0]);
         row=mysql_fetch_row(result);
         obj_proc->construct_data->item5 = atoi(row[0]);
         mysql_free_result(result);
      }
      else {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In object::dbRead(long, long, short):\n";
            mudlog << "Error executing query: " << mysql_error(database)
                   << endl;
         }
         return;
      }
   }
   // scripts
   if (obj_flags.get(76)) {
      ObjectScript* ptr;
      query="select * from ObjScripts where OBJ_NUM=";
      query+=obj_num;
      query+=" and SOBJ_NUM=";
      query+=sobj_num;

      if (mysql_real_query(database, query, strlen(query))==0) {
         if ((result=mysql_store_result(database))==NULL) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In object::dbRead(long, long, short):\n";
               mudlog << "Error retrieving query results: "
                      << mysql_error(database) << endl;
            }
            return;
         }
         while ((row=mysql_fetch_row(result))) {
            String* tmp_disc = new String(row[OSCRTBL_DISCRIMINATOR]);
            tmp_disc->Strip();
            tmp_disc->Prepend(" ");
            tmp_disc->Append(" ");

            ptr = new ObjectScript(*(new String(row[OSCRTBL_TRIGGER_CMD])),
                                   atoi(row[OSCRTBL_TARGET]),
                                   atoi(row[OSCRTBL_ACTOR]),
                                   *tmp_disc,
                                   atoi(row[OSCRTBL_PRECEDENCE]));
            String sbuf;
            sbuf = row[OSCRTBL_SCRIPT_COMMANDS];
            String* tmp_str;
            while ((tmp_str = sbuf.getUntil(';'))) {
               ptr->appendCmd(*tmp_str);
               delete tmp_str;
            }
            ptr->compile();
         }
         mysql_free_result(result);
      }
      else {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In object::dbRead(long, long, short):\n";
            mudlog << "Error executing query: " << mysql_error(database) << endl;
         }
         return;
      }
   }
}
#endif

void object::fileRead(ifstream& ofile, short read_all) {
   int i, test = TRUE;
   stat_spell_cell* ss_ptr;
   char tmp[81];
   String tmp_str(80);
   String* string;
   
   Clear();  //stop up any memory leaks etc.

   mudlog.log(DB, "In obj::Read.\n");

   if (!ofile) {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  da_file FALSE in obj read." << endl;
      }
      return;
   }

   test = TRUE;
   while (test) {
      if (!ofile) {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  da_file FALSE in obj read." << endl;
         }
         return;
      }

      ofile >> tmp_str;
      if (strcmp(tmp_str, "~") == 0) {
         test = FALSE;
      }//if
      else {
         string = new String(tmp_str);
         names.append(string);
      }//else
   }//while            
   ofile.getline(tmp, 80);         

   mudlog.log(DB, "About to do termed read..");
   
   short_desc.Termed_Read(ofile);
   in_room_desc.Termed_Read(ofile);
   long_desc.Termed_Read(ofile);
   
   mudlog.log(DB, "Done with termed read..");

   obj_flags.Read(ofile);
   obj_flags.turn_on(10); //in_use
   obj_flags.turn_off(71); //if we can read it, it's complete!
   if (obj_flags.get(30)) {
      obj_flags.turn_on(57); //all wieldables are weapons now
   }//if
   
   for (i = 0; i<OBJ_MAX_EXTRAS; i++)
      ofile >> extras[i];
   ofile.getline(tmp, 80);
   
   for (i = 0; i<OBJ_CUR_STATS; i++)
      ofile >> cur_stats[i];
   ofile.getline(tmp, 80);
   
   /*  bag  */
   if (obj_flags.get(54)) { //is a container
      if (!bag)
         bag = new bag_struct;
      bag->Read(ofile);
   }//if
   
   /*  Affected By */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  da_file FALSE in obj read." << endl;
         }
         return;
      }
      ss_ptr = new stat_spell_cell;
      ss_ptr->stat_spell = i;
      ofile >> ss_ptr->bonus_duration;
      affected_by.append(ss_ptr);
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);
   
   /*  Inventory */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  da_file FALSE in obj read." << endl;
         }
         return;
      }

      if (i == -2) { //assume its supposed to load, gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->fileRead(ofile, TRUE);

         inv.append(new_obj);    //add it to inventory
         affected_objects.append(new_obj);

         new_obj->IN_LIST = &(inv); //make sure its a SOBJ
      }//if
      else {
         if (obj_list[i].isInUse()) {
            if (read_all || ((obj_list[i].OBJ_PRCNT_LOAD *
                config.currentLoadModifier) / 100) > d(1,100)) {
               mudlog << "INFO:  Loading inventory object: " << i << " on object: "
                      << cur_stats[2] << endl;
               inv.append(&(obj_list[i]));    //add it to inventory
            }//if
            else {
               if (mudlog.ofLevel(DB)) {
                  mudlog << "INFO:  Not loading object: " << i << " on object: "
                         << cur_stats[2] << " because %load check failed."
                         << endl;
               }
            }//else
         }//if
         else { //TODO:  load percentage ignored here....need a fix!
            inv.append(&(obj_list[i]));    //add it to inventory
         }//if
      }//else
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);   

   // stat affects

   int bonus;
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  da_file FALSE in obj read." << endl;
         }
         return;
      }

      ofile >> bonus;
      ss_ptr = new stat_spell_cell;
      ss_ptr->stat_spell = i;
      ss_ptr->bonus_duration = bonus;
      stat_affects.append(ss_ptr);
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);
   
   if (obj_flags.get(63)) {   // if it has spec data
      if (!obj_proc)
         obj_proc = new obj_spec_data;
      obj_proc->Read(ofile);
   }//if

   /* read procs, if we have them. */
   if (obj_flags.get(76)) {
      //mudlog.log("Object has proc scripts...");
      int sent_;
      ObjectScript* ptr;

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
            if (mudlog.ofLevel(ERROR)) {
               mudlog << "ERROR:  object reading script da_file FALSE." << endl;
            }
            return;
         }

         ptr = new ObjectScript();
         ptr->read(ofile);
         obj_proc_scripts.append(ptr);
         ofile >> sent_;
         ofile.getline(tmp, 80);
         if (mudlog.ofLevel(DB))
            mudlog << "Got rest of line -:" << tmp << ":-" << endl;
      }//while
   }//if it had proc scripts


   /* if charges < -1, make them -1 (infinity) */
   if (extras[0] < -1) {
      extras[0] = -1;
   }

   //mudlog.log(DBG, "Done reading in obj.\n");
   ofile.getline(tmp, 80); //junk end of obj msg
   ofile.getline(tmp, 80); //junk end of obj space

   OBJ_VIS_BIT |= 1024; //hack, turn on 'normal' bit
   pause = 0;
   in_room = 0;

}//Read....object

int object::isMagic() {
   if (!stat_affects.isEmpty() || !affected_by.isEmpty())
      return TRUE;
   else
      return FALSE;
}//is_magic

object* object::loseInv(object* obj) {
   return inv.loseData(obj);
}


void object::gainInv(object* obj) {
   inv.append(obj);
   if (obj->IN_LIST)
      obj->IN_LIST = &(inv);
}//gainInv

int object::getIdNum() {
   return OBJ_NUM;
}

void object::setIdNum(int i) {
   OBJ_NUM = i;
}


int object::getZoneNum() const {
   return OBJ_IN_ZONE;
}

int object::isFood() const {
   return (OBJ_FLAGS.get(61));
}

int object::isLocked() const {
   return (bag && BAG_FLAGS.get(3));
}


int object::isMagLocked() const {
   return (bag && BAG_FLAGS.get(6));
}

int object::isClosed() const {
   return (bag && BAG_FLAGS.get(2));
}

int object::isPlayerOwned() const {
   return (bag && BAG_FLAGS.get(10));
}

int object::isPotion() const {
   return (OBJ_FLAGS.get(52) && obj_proc);
}

int object::isScroll() const {
   return (OBJ_FLAGS.get(53) && obj_proc);
}

int object::isNamed(const String& name) const {
   return obj_is_named(*this, name);
}

int object::isLiquid() const {
   return OBJ_FLAGS.get(60);
}

int object::isCanteen() const {
   return OBJ_FLAGS.get(59);
}

int object::isHerb() const {
   return (OBJ_FLAGS.get(72));
}

int object::isInUse() const {
   return (OBJ_FLAGS.get(10));
}

int object::isNotComplete() const {
   return (OBJ_FLAGS.get(71));
}

void object::setComplete() {
   OBJ_FLAGS.turn_off(71);
}

void object::setIncomplete() {
   OBJ_FLAGS.turn_on(71);
}

// These return the current value after the operation
int object::setCurInGame(int i) {
   obj_list[getIdNum()].extras[3] = i;
   return i;
}

int object::getCurInGame() {
   return obj_list[getIdNum()].extras[3];
}

int object::getMaxInGame() {
   return obj_list[getIdNum()].extras[4];
}

int object::getKeyNum() {
   if (bag) {
      return bag->key_num;
   }
   return 0;
}

void object::setButcherable(int val) {
   if (val)
      OBJ_FLAGS.turn_on(75);
   else
      OBJ_FLAGS.turn_off(75);
}

int object::isLightSource() const {
   return (obj_flags.get(58));
}

int object::isBulletinBoard() const {
   return (obj_flags.get(74));
}


int object::isLit() const {
   return (obj_flags.get(58) &&
           ((extras[0] == -1) || (extras[0] > 0)));
}

int object::isTwoHanded() const {
   return (obj_flags.get(77));
}
   
void object::lock() {
   if (bag) {
      bag->lock();
      if (isPlayerOwned()) {
         save_player_box(*this);
      }
   }
}//lock

int object::consumesKey() {
   if (bag) {
      return bag->consumesKey();
   }
   return FALSE;
}//lock

void object::unlock() {
   if (bag) {
      bag->unlock();
      if (isPlayerOwned()) {
         save_player_box(*this);
      }
   }
}//lock

void object::open() {
   if (bag) {
      bag->open();
   }
}//lock


int object::decrementCurInGame() {
   return --(obj_list[getIdNum()].extras[3]);
}

int object::incrementCurInGame() {
   return ++(obj_list[getIdNum()].extras[3]);
}

void object::makeComponent(int targ, int comp1, int comp2, int comp3,
                           int comp4, int comp5, ComponentEnum con_type) {

   if (!obj_proc) {
      obj_proc = new obj_spec_data();
   }
   OBJ_FLAGS.turn_on(63); //has spec proc data

   if (!obj_proc->construct_data) {
      obj_proc->construct_data = new obj_construct_data();
   }
   OBJ_SPEC_FLAGS.turn_on(1); //set the flag to make it a construct object

   COMPONENT_TARG = targ;
   COMPONENT_ITEM1 = comp1;
   COMPONENT_ITEM2 = comp2;
   COMPONENT_ITEM3 = comp3;
   COMPONENT_ITEM4 = comp4;
   COMPONENT_ITEM5 = comp5;


   if (con_type == CONSTRUCTION) {
      OBJ_FLAGS.turn_on(67);
   }
   else if (con_type == CONCOCTION) {
      OBJ_FLAGS.turn_on(68);
   }
   else {
      mudlog << "ERROR:  illegal ComponentType, object::makeComponent."
             << endl;
   }
}//makeComponent

void object::setCurRoomNum(int i, int sanity) {
   in_room = i;

   if (inv.isEmpty()) {
      return;
   }
   else {
      Cell<object*> cll(inv);
      object* ptr;

      if (sanity > 20) {
         mudlog << "WARNING:  object::setCurInRoom, sanity over-run, obj#: "
                << getIdNum() << endl;
         return;
      }

      while ((ptr = cll.next())) {
         if (ptr->isModified()) {
            ptr->setCurRoomNum(i, sanity + 1);
         }
      }//while
   }//if
}//setCurInRoom


int object::getObjCountByNumber(int onum, int sanity) {

   if (mudlog.ofLevel(DBG)) {
      mudlog << "getObjCountByNumber onum: " << onum << " sanity: " 
             << sanity << " Obj_NUM: " << getIdNum() << endl;
   }

   if (inv.isEmpty()) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "Inventory is empty, returning zero." << endl;
      }
      return 0;
   }
   else {
      Cell<object*> cll(inv);
      object* ptr;
      int count = 0;

      if (sanity > 20) {
         return 0;
      }

      if (mudlog.ofLevel(DBG)) {
         mudlog << "Searching object inventory." << endl;
      }

      while ((ptr = cll.next())) {
         if (ptr->getIdNum() == onum) {
            count++;
         }//if detect
         count += ptr->getObjCountByNumber(onum, sanity + 1);
      }//while

      if (mudlog.ofLevel(DBG)) {
         mudlog << "Returning count: " << count << endl;
      }

      return count;
   }//if
}//getObjectCountByNumber

int object::doJunk() {
   if (isModified()) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "DEBUG:  in doJunk, wasModified, obj: " << this << endl;
      }      
      return find_and_delete_obj(this, getCurRoomNum());
   }
   else {
      mudlog.log(ERROR, "ERROR:  in doJunk, was NOT Modified.\n");
      return FALSE;
   }
}

int object::doMload(int i_th) {
   critter* crit_ptr;

   if (0 < i_th < NUMBER_OF_MOBS) {
      crit_ptr = &(mob_list[i_th]);
      // Make sure this mob is in use
      if (!crit_ptr->CRIT_FLAGS.get(18)) return -1;
   }
   else return -1;
   if (crit_ptr->isPlayerShopKeeper()) {
      critter* shop_keeper = load_player_shop_owner(crit_ptr->getIdNum());
      if(shop_keeper) {
         room_list[getCurRoomNum()].gainCritter(shop_keeper);
      }
   }
   else {
      room_list[getCurRoomNum()].gainCritter(&(mob_list[crit_ptr->getIdNum()]));
   }
   recursive_init_loads(*crit_ptr);
   return 0;
}

int object::doGoToRoom(int dest_room, const char* from_dir, door* by_door,
                       int cur_room, int sanity) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In object::doGoToRoom, dest_room:  " << dest_room
             << "  cur_room:  " << cur_room << "  by_door:  " 
             << by_door  << " sanity: " << sanity << endl;
   }

   if (sanity > 20) {
      mudlog << "ERROR: busted sanity check, object::doGoToRoom" << endl;
      return -1;
   }

   room_list[cur_room].removeObject(this);

   leave_room_effects(room_list[cur_room], *this);

   room_list[dest_room].gainObject(this);

   // This can cause us to delete ourselves btw...not very good coding
   // but..maybe it will work!
   do_entered_room_procs(*this, by_door, from_dir, room_list[dest_room],
                         sanity);

   return 0;
}//doGoToRoom


void object::finishedObjProc() {
   if (cur_script) {
      pending_scripts.loseData(cur_script);
      delete cur_script;
   }

   // This could easily end up being NULL, that's OK!
   cur_script = pending_scripts.peekFront();
}//finishedRoomProc

void object::addProcScript(const String& txt, ObjectScript* script_data) {
   //similar to reading it in...
   //first, see if we are over-writing one...
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In object::addProcScript, txt:  \n" << txt
             << "\nscript data:  "
             << script_data->toStringBrief(0, 0, ENTITY_ROOM, 0) << endl;
   }

   obj_flags.turn_on(76);

   Cell<ObjectScript*> cll;
   ObjectScript* ptr;
   obj_proc_scripts.head(cll);

   while ((ptr = cll.next())) {
      if (ptr->matches(*script_data)) {
         //got a match.
         mudlog.log("object::addProcScript, they match.");
         *ptr = *script_data;
         ptr->setScript(txt);
         delete script_data;
         return;
      }//if
   }//while
   
   mudlog.log(DBG, "About to setScript.");
   
   script_data->setScript(txt);
   mudlog.log(DBG, "done with setScript.");

   if (!script_data) {
      mudlog.log(ERROR, "script_data is NULL, object::addProcScript.");
      return;
   }

   obj_proc_scripts.append(script_data);
}//addProcScript


void object::listScripts(critter& pc) {
   String buf(500);
   buf.Append("These scripts are defined for this object, the actual scripts\n"
         "may be seen by using the stat_room_script [rm_num] [script_index] command.\n\n");

   pc.show(buf);

   String tmp(100);
   int found_one = FALSE;
   Cell<ObjectScript*> cll(obj_proc_scripts);
   ObjectScript* ptr;
   int idx = 0;
   while ((ptr = cll.next())) {
      found_one = TRUE;
      tmp = ptr->toStringBrief(FALSE, 0, ENTITY_OBJECT, idx);
      Sprintf(buf, "[%i] %S\n", idx, &(tmp));
      pc.show(buf);
      idx++;
   }

   if (!found_one) {
      buf.Append("No scripts defined for this object.\n");
      pc.show(buf);
   }
}//listScripts

void object::removeScript(String& trigger, int i_th, critter& pc) {
   int sofar = 1;
   String buf(500);
 
   Cell<ObjectScript*> cll(obj_proc_scripts);
   ObjectScript* ptr;
   ptr = cll.next();
   while (ptr) {
      if (strcasecmp(*(ptr->getTrigger()), trigger) == 0) {
         if (sofar == i_th) {
            delete ptr;
            ptr = obj_proc_scripts.lose(cll);
            pc.show("Deleted it...\n");
            return;
         }//if
         else {
            ptr = cll.next();
         }
         sofar++;
      }//if
      else {
         ptr = cll.next();
      }
   }//while

   pc.show("Didn't find that script..\n");
}//removeScript


int object::insertNewScript(ObjectScript* script) {
   ObjectScript* ptr;
   Cell<ObjectScript*> cll(pending_scripts);

   // Don't append scripts that have a zero precedence, if there
   // are other scripts in the queue.
   if ((script->getPrecedence() == 0) && (!pending_scripts.isEmpty())) {
      delete script;
      return 0;
   }

   while ((ptr = cll.next())) {
      if (ptr->getPrecedence() < script->getPrecedence()) {
         // Then insert it
         pending_scripts.insertBefore(cll, script);
         return 0;
      }//if
   }//while

   // If here, then we need to place it at the end.
   pending_scripts.append(script);
   return 0;
}


void object::doScriptJump(int abs_offset) {
   if (cur_script)
      cur_script->doScriptJump(abs_offset);
}

// NOTE:  The script owner is *this.  It is likely, but not necessary
// that targ is also *this
void object::checkForProc(String& cmd, String& arg1, critter& actor,
                          int targ, room& rm, int sanity) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In object::checkForProc, size of scripts: "
             << obj_proc_scripts.size() << endl;
   }

   if (!isModified()) {
      mudlog.log(ERROR, "ERROR:  object::checkForProc, got an OBJ (not modified).");
      return;
   }

   if (sanity > 100) {
      mudlog.log(ERROR, "ERROR: object::checkForProc busted recursion sanity check.");
      return;
   }

   Cell<ObjectScript*> cll;
   ObjectScript* ptr;
         
   obj_proc_scripts.head(cll);

   while ((ptr = cll.next())) {
      mudlog.log("In while.");
      mudlog.log(ptr->toStringBrief(0, 0, ENTITY_OBJECT, 0));
      if (ptr->matches(cmd, arg1, actor, targ, getIdNum())) {
         mudlog.log("Matches..");
         if (pending_scripts.size() >= 10) { //only queue 10 scripts
            return; //do nothing, don't want to get too much backlog.
         }
         else {
            // add it to the pending scripts.
            ptr->generateScript(cmd, arg1, actor, targ, rm, NULL, this);
            insertNewScript(new ObjectScript(*ptr));

            if (cur_script) {
               if (cur_script->getPrecedence() <
                   pending_scripts.peekFront()->getPrecedence()) {
                  
                  pending_scripts.loseData(cur_script); //take it out of queue
                  delete cur_script; //junk it!
                  cur_script = pending_scripts.peekFront();
                  cur_script->resetStackPtr(); //get ready to start
               }//if
               // else, it just goes into the queue
            }//if we currently have a script.
            else { //there was none, so grab the first one.
               cur_script = pending_scripts.peekFront();
               proc_action_objs.gainData(this);
               cur_script->resetStackPtr(); //get ready to start
            }

            return;
         }//else
      }//if matches
   }//while

   // Look through all inventory (recursively)

   Cell<object*> ocll(inv);
   object* optr;
   while ((optr = ocll.next())) {
      if (optr->hasScript()) {
         if (!optr->isModified()) {
            object* tmp = optr;
            optr = obj_to_sobj(*optr, &inv, getCurRoomNum());
            if (!inv.substituteData(tmp, optr, 1)) {
               mudlog.log(ERROR, "ERROR: critter::checkForProc: substituteData  failed after obj_to_sobj.\n");
            }//if
         }
         optr->checkForProc(cmd, arg1, actor, targ, rm, sanity + 1);
      }//if
   }//while

}//checkForProcAction

const String* object::getLongName(critter& pc) {
   return getLongName(pc.SEE_BIT);
}


int object::getSacPoints(List<object*>& tmp_list, int sanity) {
   int retval = 0;
   int tmp = 0;

   if (sanity > 100) {
      mudlog << __PRETTY_FUNCTION__ << ": ERROR: busted sanity check.\n";
      return 0;
   }

   if (obj_count(tmp_list, *this) == 0) {
      tmp_list.append(this);
      tmp = calcSacPoints();
      if (tmp > 0)
         retval += tmp;
   }
   
   Cell<object*> cll(inv);
   object* ptr;
   while ((ptr = cll.next())) {
      tmp = ptr->getSacPoints(tmp_list, sanity + 1);
      if (tmp > 0)
         retval += tmp;
   }//while
   return retval;
}

int object::calcSacPoints() {
   int retval = 0;
   Cell<stat_spell_cell*> cll(stat_affects);
   stat_spell_cell* ptr;
   while ((ptr = cll.next())) {
      switch (ptr->stat_spell) {
         // Only do select stat_affects, see critter.h comments for the
         // definitions. (short_cur_stats) --Ben
      case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 10:
      case 23: case 24: case 25: case 35: case 36: case 37: case 38: case 39: {
         retval += ptr->bonus_duration;
      }
      default:
         ;//Do nothing
      }//switch
   }//while
   return retval;
}
