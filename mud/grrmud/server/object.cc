// $Id: object.cc,v 1.12 1999/06/16 06:43:27 greear Exp $
// $Revision: 1.12 $  $Author: greear $ $Date: 1999/06/16 06:43:27 $

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
      if (mudlog.ofLevel(ERR)) {
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
   w_eye_owner = NULL;
}//constructor

obj_spec_data::obj_spec_data(const obj_spec_data& source) { //copy constructr
   _cnt++;
   construct_data = NULL;
   skin_ptr = NULL;
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
   construct_data = NULL;
   if (w_eye_owner) {
     w_eye_owner->pc->w_eye_obj = NULL;
     w_eye_owner = NULL;
   }//if
   clear_ptr_list(casts_these_spells);
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
   w_eye_owner = source.w_eye_owner;

   clear_ptr_list(casts_these_spells);
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

   w_eye_owner = NULL;
   clear_ptr_list(casts_these_spells);
}//clear


void obj_spec_data::Read(ifstream& da_file) {
   char buf[82];
   int tmp;

   Clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
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
         mudlog.log(ERR, "ERROR:  trying to write NULL construct data.\n");
         return;
      }//if
      construct_data->Write(da_file);
   }//if
   if (obj_spec_data_flags.get(2)) {
     if (!skin_ptr) {
       da_file << 0 << "     skin number\n";
     }//if
     else {
       da_file << skin_ptr->OBJ_NUM << "	skin number\n";
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

}//default constructor


object::~object() {
   _cnt--;

   if ((!in_list) && (!do_shutdown)) {
      mudlog.log(ERR, "ERROR:  trying to delete OBJ before shutdown.\n");
      //do_shutdown = TRUE;
      //exit(2);
   }//if

   if (!do_shutdown) {
      affected_objects.loseData(this);
      pulsed_proc_objects.loseData(this);
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
      mudlog.log(ERR, "ERROR:  OBJ being assigned to... should be a SOBJ!\n");
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
      delete cur_script;
      cur_script = NULL;
   }

   clear_ptr_list(names);
   short_desc.Clear();
   in_room_desc.Clear();
   long_desc.Clear();
   
   obj_flags.Clear();
   
   delete bag;
   bag = NULL;
   in_list = NULL;
   
   clear_ptr_list(affected_by);
   clear_ptr_list(stat_affects);
   clear_obj_list(inv);
   
   delete obj_proc;
   obj_proc = NULL;

   clear_ptr_list(obj_proc_scripts);
   clear_ptr_list(pending_scripts);
   delete cur_script;
   cur_script = NULL;

}//Clear


int object::getCurWeight() {
   if (inv.isEmpty()) 
      return extras[5];
   else {
      Cell<object*> cll(inv);
      object* tmp_obj;
      int tmp_wt = 0;
      while ((tmp_obj = cll.next())) {
         tmp_wt += tmp_obj->getCurWeight();
      }//while
      return tmp_wt + extras[5];  // weight of inv, plus container's wt
   }//else
}//cur_weight

int object::getMaxWeight() {
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
         mudlog.log(ERR, "ERROR:  trying to write a NULL obj_proc.\n");
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


void object::Read(ifstream& ofile, short read_all) {
   int i, test = TRUE;
   stat_spell_cell* ss_ptr;
   char tmp[81];
   String tmp_str(80);
   String* string;
   
   Clear();  //stop up any memory leaks etc.

   mudlog.log(DB, "In obj::Read.\n");

   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in obj read." << endl;
      }
      return;
   }

   test = TRUE;
   while (test) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
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
         if (mudlog.ofLevel(ERR)) {
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
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in obj read." << endl;
         }
         return;
      }

      if (i == -2) { //assume its supposed to load, gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->Read(ofile, TRUE);

         inv.append(new_obj);    //add it to inventory
         affected_objects.append(new_obj);

         new_obj->IN_LIST = &(inv); //make sure its a SOBJ
      }//if
      else {
         if (obj_list[i].isInUse()) {
            if (read_all || ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 100) > 
                d(1,100)) {
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
         if (mudlog.ofLevel(ERR)) {
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
            if (mudlog.ofLevel(ERR)) {
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

   
void object::lock() {
   if (bag) {
      bag->lock();
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


int object::doGoToRoom(int dest_room, const char* from_dir, door* by_door,
                       int cur_room) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In object::doGoToRoom, dest_room:  " << dest_room
             << "  cur_room:  " << cur_room
             << "  by_door:  " << by_door << endl;
   }

   room_list[cur_room].removeObject(this);

   leave_room_effects(room_list[cur_room], *this);

   room_list[dest_room].gainObject(this);

   // This can cause us to delete ourselves btw...not very good coding
   // but..maybe it will work!
   do_entered_room_procs(*this, by_door, from_dir, room_list[dest_room]);

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
      mudlog.log(ERR, "script_data is NULL, object::addProcScript.");
      return;
   }

   obj_proc_scripts.append(script_data);
}//addProcScript


void object::listScripts(critter& pc) {
   String buf(500);
   buf.Append("These scripts are defined for this object, the actual scripts
may be seen by using the stat_room_script [rm_num] [script_index] command.\n\n");

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
      show(buf, pc);
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
            show("Deleted it...\n", pc);
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

   show("Didn't find that script..\n", pc);
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
                          int targ, room& rm) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In object::checkForProc, size of scripts: "
             << obj_proc_scripts.size() << endl;
   }

   if (!isModified()) {
      mudlog.log(ERR, "ERROR:  object::checkForProc, got an OBJ (not modified).");
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
}//checkForProcAction
