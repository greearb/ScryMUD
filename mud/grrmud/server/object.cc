// $Id: object.cc,v 1.31 1999/09/06 07:12:52 greear Exp $
// $Revision: 1.31 $  $Author: greear $ $Date: 1999/09/06 07:12:52 $

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
#include "Markup.h"

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

void obj_construct_data::read(istream& da_file, int read_all = TRUE) {
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

void obj_construct_data::write(ostream& da_file) {
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
   if (skin_ptr && skin_ptr->isModified()) {
      delete skin_ptr;
      skin_ptr = NULL;
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

   clear();  //clear current mess
   obj_spec_data_flags = source.obj_spec_data_flags;
   if (source.construct_data) {
      construct_data = new obj_construct_data(*(source.construct_data));
   }//if

   if (source.skin_ptr && !source.skin_ptr->isModified()) {
      skin_ptr = source.skin_ptr; //don't copy ptrs to SOBJ's
   }//if
   else {
      skin_ptr = NULL;
   }//else
   w_eye_owner = source.w_eye_owner;

   casts_these_spells.becomeDeepCopyOf(source.casts_these_spells);

   return *this;
}// operator= overload

void obj_spec_data::clear() {
   obj_spec_data_flags.clear();
   delete construct_data;
   construct_data = NULL;
   
   if (skin_ptr && skin_ptr->isModified()) {
      delete skin_ptr;
   }//if
   skin_ptr = NULL;

   w_eye_owner = NULL;
   casts_these_spells.clearAndDestroy();
}//clear


int obj_spec_data::read(istream& da_file, int read_all = TRUE) {
   char buf[82];
   int tmp;

   clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in obj_spec_data read." << endl;
      }
      return -1;
   }

   obj_spec_data_flags.read(da_file);

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
      construct_data->read(da_file);
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
         casts_these_spells.append(new SpellDuration(first, second));
         da_file >> first;
      }//while
      da_file.getline(buf, 80);
   }//if
   return 0;
}//read

void obj_spec_data::toStringStat(critter* viewer, String& rslt) {
   String buf(100);
   rslt.clear();

   if (viewer->isUsingClient()) {
      rslt.append("<OBJ_SPEC>\n");
   }
   else {
      rslt.append("Object Spec Proc Data:\n");
   }

   Markup::toString(NULL, obj_spec_data_flags, OBJ_SPEC_DATA_FLAGS_NAMES, viewer,
                    NULL, buf);
   rslt.append(buf);

   if (construct_data) {
      rslt.append("Construct Data:\n");

      Sprintf(buf, 
              "\tTarget: %i, item1: %i, item2: %i, item3: %i, item4: %i, item5: %i\n",
              construct_data->target_object, construct_data->item1,
              construct_data->item2, construct_data->item3,
              construct_data->item4, construct_data->item5);
      rslt.append(buf);
   }//if
         
   // Casts spells taken care of elsewhere.
}//toStringStat


int obj_spec_data::write(ostream& da_file) {
   obj_spec_data_flags.write(da_file);
   if (obj_spec_data_flags.get(1)) { //if have construct data
      if (!construct_data) {
         mudlog.log(ERR, "ERROR:  trying to write NULL construct data.\n");
         return -1;
      }//if
      construct_data->write(da_file);
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
     Cell<SpellDuration*> cll(casts_these_spells);
     SpellDuration* ptr;
     int cnt = 0;
     while ((ptr = cll.next())) {
       if (++cnt > 15) {
	 cnt = 0;
	 da_file << "\n";
       }//if
       da_file << ptr->spell << " " << ptr->duration << " ";
     }//while
     da_file << "-1 spell#, level (casts)\n";
   }//if
   return 0;
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

int bag_struct::write(ostream& ofile) {
   MetaTags mt(*this);
   mt.write(ofile);

   Closable::write(ofile);

   ofile << " " << max_weight << " " << percentage_weight << " "
         << time_till_disolve << " " << "\t bag_stuff\n";
   return 0;
}//Write()


void bag_struct::toStringStat(critter* viewer, String& rslt, ToStringTypeE st) {
   String buf(300);
   rslt.clear();

   if (viewer->isUsingClient()) {
      rslt.append("<BAG>\n");
   }
   else {
      rslt.append("Bag Information: \n");
   }

   Closable::toStringStat(viewer, buf, st);
   rslt.append(buf);

   Sprintf(buf, "max weight:  %i,  percentage weight:  %i  ", 
           max_weight, percentage_weight);
   rslt.append(buf);

   if (viewer->isImmort()) {
      Sprintf(buf, "time till disolve:  %i.\n", time_till_disolve);
      rslt.append(buf);

   }//if
   if (viewer->isUsingClient()) {
      rslt.append("</BAG>\n");
   }
}//toStrintStat


int bag_struct::read(istream& ofile, int read_all = TRUE) {
   char buf[81];
   String str(80);
   ofile >> str; //grab the first token, if V2.x it will be a number (bitfield)

   if (isnum(str)) {
      if (strcmp(str, "-1") != 0) {
         bitfield bf;
         bf.read(ofile);
         bf.turn_on(atoi(buf));

         // Now, set all these flags.  Hateful, but we gotta translate them to
         // how the Closable's flags like it.
         setClosed(bf.get(2));
         setLocked(bf.get(3));
         setPickable(bf.get(4));
         setMagLockable(bf.get(5));
         setMagLocked(bf.get(6));
         setDestructable(bf.get(7));
         setCorpse(bf.get(8));
         setNoClose(bf.get(9));
         setConsumesKey(bf.get(17));
      }//if

      int key;
      ofile >> key;
      setKey(key);

      ofile >> max_weight >> percentage_weight >> time_till_disolve;
   }
   else { //V3.0 format
      MetaTags mt(buf, ofile);

      Closable::read(ofile);
      ofile >> max_weight >> percentage_weight >> time_till_disolve;
   }
   ofile.getline(buf, 80);
   return (0);
}//read()


//*************************************************************//
///**********************  object  ***************************///

int object::_cnt = 0;

object::object (object& source) :
      Entity(source), Scriptable(source), inv(NULL) { //copy constructor
   _cnt++;

   is_modified = FALSE;
   bag = NULL;
   obj_proc = NULL;

   *this = source;

   obj_ptr_log << "OBJ_CC " << getIdNum() << " " << this << "\n";
}//obj copy constructor


object::object() : inv(NULL) {
   int i;

   _cnt++;
  
   for (i = 0; i<OBJ_MAX_EXTRAS; i++)
      extras[i] = 0;       
   for (i = 0; i<OBJ_CUR_STATS; i++)
      cur_stats[i] = 0;    
   
   is_modified = FALSE;
   bag = NULL;
   obj_proc = NULL;
   
   // Can't have an obj# at this point.
   obj_ptr_log << "OBJ_CON 0 " << this << "\n";
}//default constructor


object::~object() {
   _cnt--;

   obj_ptr_log << "OBJ_DES " << getIdNum() << " " << this << "\n";

   if ((!isModified()) && (!do_shutdown)) {
      mudlog.log(ERR, "ERROR:  trying to delete OBJ before shutdown.\n");
      ::core_dump("Deleting un-modified object.");
   }//if

   clear();
}//destructor


object& object::operator= (object& source) {

   if (this == &source)
      return *this;
   
   SCell<object*> cll;
   object* obj_ptr;      
   int i;
   
   if (!isModified() && isInUse()) {
      mudlog.log(ERR, "ERROR:  OBJ being assigned to... should be a SOBJ!\n");
      ::core_dump("object::operator=, !modified, and is in use.");
   }//if
   
   clear();
   
   short_desc = source.short_desc;
   in_room_desc = source.in_room_desc;
   
   obj_flags = source.obj_flags; //use bitfield's overloaded =

   if (!isInUse()) { //in use
      mudlog << "ERROR:  assigning obj, flag 10 (in use) not set."
             << endl;
      obj_flags.turn_on(10); //make it in-use
   }
   
   for (i = 0; i<OBJ_MAX_EXTRAS; i++) 
      extras[i] = source.extras[i];
   
   for (i = 0; i<OBJ_CUR_STATS; i++) 
      cur_stats[i] = source.cur_stats[i];
   
   if (source.bag) {
      bag = new bag_struct(*(source.bag));
   }//if bag
   
   affected_by.becomeDeepCopyOf(source.affected_by);
   
   source.inv.head(cll);
   while ((obj_ptr = cll.next())) {
      if (!obj_ptr->isModified()) { //multiple ptrs to SOBJ's will be a problem
         inv.append(obj_ptr);
      }//if
   }//while

   stat_affects.becomeDeepCopyOf(source.stat_affects);
   
   if (source.obj_proc) {
      obj_proc = new obj_spec_data(*(source.obj_proc));
   }//if

   return *this;
}//operator= overload
	    

void object::clear() {
   Entity::clear();
   Scriptable::clear();

   short_desc.clear();
   in_room_desc.clear();
   
   obj_flags.clear();
   
   delete bag;
   bag = NULL;

   stat_affects.clearAndDestroy();
   clear_obj_list(inv);
   
   delete obj_proc;
   obj_proc = NULL;
}//Clear


/** ptr will be consumed by this call, ie takes charge of memory. */
void object::addStatAffect(StatBonus* ptr) {
   stat_affects.prepend(ptr);
}

room* object::getCurRoom() {
   return &(room_list[getCurRoomNum()]);
}

int object::getCurRoomNum() {
   if (getContainer()) {
      return getContainer()->getCurRoomNum();
   }
   if (mudlog.ofLevel(WRN)) {
      mudlog << "WARNING:  object::getCurRoomNum failed, obj# " << getIdNum()
             << endl;
   }
   return 0;
}//getCurRoomNum


int object::getCurWeight() {
   if (inv.isEmpty()) {
      return extras[5];
   }
   else {
      SCell<object*> cll(inv);
      object* tmp_obj;
      int tmp_wt = 0;
      while ((tmp_obj = cll.next())) {
         tmp_wt += tmp_obj->getCurWeight();
      }//while
      return tmp_wt + extras[5];  // weight of inv, plus container's wt
   }//else
}//cur_weight

int object::getMaxWeight() const {
   if (bag) 
      return bag->getMaxWeight();
   else
      return getMaxWeight();
}//max_weight


int object::write(ostream& ofile) {
   int i;
   Cell<SpellDuration*> sd_cell;
   Cell<StatBonus*> sb_cell;
   StatBonus* sb_ptr;
   SCell<object*> ob_cell;
   object* ob_ptr;

   //TODO
   MetaTags mt(*this);
   mt.write(ofile);

   Entity::write(ofile);
   Scriptable::write(ofile);

   short_desc.write(ofile);
   in_room_desc.write(ofile);
   
   obj_flags.write(ofile);
   
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
      bag->write(ofile);
   
   
   /*  Inventory */   
   if (!isBulletinBoard()) { //don't write out inv of a board
      i = 0;
      inv.head(ob_cell);
      while ((ob_ptr = ob_cell.next())) {
         if (ob_ptr->isModified()) {
            ofile << -2 << "\t Start of SOBJ\n";
            ob_ptr->write(ofile);
         }//if
         else {
            ofile << ob_ptr->getIdNum() << " ";
            if ((++i % 20) == 0)
               ofile << endl;
         }//else
      }//while
   }//if not a bulletin board
   ofile << -1 << "\tinv\n";
   
   
   /*  Stat_Affects */
   i = 0;
   stat_affects.head(sb_cell);
   while ((sb_ptr = sb_cell.next())) {
      ofile << sb_ptr->stat << " " << sb_ptr->bonus << " ";
      if ((++i % 20) == 0)
         ofile << endl;
   }//while
   ofile << -1 << "\tstat_affects\n";  //done
   
   if (obj_flags.get(63)) { //if has spec_data
      if (!obj_proc) {
         mudlog.log(ERR, "ERROR:  trying to write a NULL obj_proc.\n");
         return -1;
      }//if
      obj_proc->write(ofile);
   }//if has spec_data

   ofile << "End of Object.\n\n";
   return 0;
}//Write...obj


int object::read(istream& da_file, int read_all) {
   String buf(100);
   da_file >> buf;
   if (strcasecmp(buf, "<META") == 0) {
      MetaTags mt(buf, da_file);
      return read_v3(da_file, read_all);
   }
   else {
      return read_v2(da_file, buf, read_all);
   }
}//read

int object::read_v2(istream& ofile, String& first_name, short read_all) {
   int i, test = TRUE;
   StatBonus* sb_ptr;
   SpellDuration* sd_ptr;
   char tmp[81];
   String tmp_str(280);
   
   clear();  //stop up any memory leaks etc.

   mudlog.log(DB, "In obj::read.\n");

   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in obj read." << endl;
      }
      return -1;
   }

   addName(first_name);

   test = TRUE;
   while (test) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in obj read." << endl;
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

   mudlog.log(DB, "About to do termed read..");

   tmp_str.termedRead(ofile);
   addShortDesc(tmp_str);
   
   tmp_str.termedRead(ofile);
   addInRoomDesc(tmp_str);

   tmp_str.termedRead(ofile);
   addLongDesc(tmp_str);
   
   mudlog.log(DB, "Done with termed read..");

   obj_flags.read(ofile);
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
      bag->read(ofile);
   }//if
   
   /*  Affected By */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in obj read." << endl;
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
   
   /*  Inventory */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in obj read." << endl;
         }
         return -1;
      }

      if (i == -2) { //assume its supposed to load, gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->read(ofile, TRUE);

         inv.append(new_obj);    //add it to inventory
         affected_objects.append(new_obj);

         new_obj->setContainer(this);
         new_obj->setModified(TRUE);
      }//if
      else {
         if (obj_list[i].isInUse()) {
            if (read_all || ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 100) > 
                d(1,100)) {
               mudlog << "INFO:  Loading inventory object: " << i << " on object: "
                      << cur_stats[2] << endl;
               object* ptr = &(obj_list[i]); //compiler issue
               inv.append(ptr); //add it to inventory
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
            object* ptr = &(obj_list[i]); //compiler issue
            inv.append(ptr);     //add it to inventory
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
         return -1;
      }

      ofile >> bonus;
      sb_ptr = new StatBonus();
      sb_ptr->stat = i;
      sb_ptr->bonus = bonus;

      addStatAffect(sb_ptr);
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);
   
   if (obj_flags.get(63)) {   // if it has spec data
      if (!obj_proc)
         obj_proc = new obj_spec_data;
      obj_proc->read(ofile);
   }//if

   /* read procs, if we have them. */
   if (obj_flags.get(76)) {
      //mudlog.log("Object has proc scripts...");
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
               mudlog << "ERROR:  object reading script da_file FALSE." << endl;
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
   return 0;
}//read_v2


int object::read_v3(istream& ofile, int read_all) {
   int i;
   StatBonus* ss_ptr;
   char tmp[81];
   String tmp_str(80);
   
   clear();  //stop up any memory leaks etc.

   mudlog.log(DB, "In obj::read_v3.\n");

   Entity::read(ofile);
   Scriptable::read(ofile);

   short_desc.read(ofile);
   in_room_desc.read(ofile);
   
   obj_flags.read(ofile);
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
      bag->read(ofile);
   }//if
   
   /*  Inventory */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in obj read." << endl;
         }
         return -1;
      }

      if (i == -2) { //assume its supposed to load, gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->read(ofile, TRUE);

         inv.append(new_obj);    //add it to inventory
         affected_objects.append(new_obj);

         new_obj->setContainer(this);
         new_obj->setModified(TRUE);
      }//if
      else {
         if (obj_list[i].isInUse()) {
            if (read_all || 
                ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 100) > 
                d(1,100)) {
               mudlog << "INFO:  Loading inventory object: " << i
                      << " on object: " << cur_stats[2] << endl;
               object* ptr = &(obj_list[i]); //compiler issue
               inv.append(ptr); //add it to inventory
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
            object* ptr = &(obj_list[i]); //compiler issue
            inv.append(ptr); //add it to inventory
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
         return -1;
      }

      ofile >> bonus;
      ss_ptr = new StatBonus();
      ss_ptr->stat = i;
      ss_ptr->bonus = bonus;
      stat_affects.append(ss_ptr);
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);
   
   if (obj_flags.get(63)) {   // if it has spec data
      if (!obj_proc)
         obj_proc = new obj_spec_data;
      obj_proc->read(ofile);
   }//if

   /* if charges < -1, make them -1 (infinity) */
   if (extras[0] < -1) {
      extras[0] = -1;
   }

   //mudlog.log(DBG, "Done reading in obj.\n");
   ofile.getline(tmp, 80); //junk end of obj msg
   ofile.getline(tmp, 80); //junk end of obj space

   OBJ_VIS_BIT |= 1024; //hack, turn on 'normal' bit
   return 0;
}//read_v3

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
   if (obj->isModified())
      obj->setContainer(this);
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
   return (bag && bag->isLocked());
}


int object::isMagLocked() const {
   return (bag && bag->isMagLocked());
}

int object::isClosed() const {
   return (bag && bag->isClosed());
}


int object::isPotion() const {
   return (OBJ_FLAGS.get(52) && obj_proc);
}

int object::isScroll() const {
   return (OBJ_FLAGS.get(53) && obj_proc);
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
      return bag->getKey();
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


void object::toStringStat(critter* viewer, String& rslt, ToStringTypeE st) {
   String buf(500);
   rslt.clear();

   if (!isInUse()) {
      rslt.append("This object is NULL, not init'd by the game.\n");
      return;
   }//if

   if (viewer->isUsingClient()) {
      Sprintf(rslt, "<OBJECT %i>\n", getIdNum());
   }
   else {
      Sprintf(rslt, "Object: %i\n", getIdNum());
   }

   Entity::toStringStat(viewer, buf, st);
   rslt.append(buf);

   Scriptable::toStringStat(viewer, buf);
   rslt.append(buf);

   short_desc.toStringStat("<SHORT_DESC>", "</SHORT_DESC>", viewer, buf);
   rslt.append(buf);

   in_room_desc.toStringStat("<NROOM_DESC>", "</NROM_DESC>", viewer, buf);
   rslt.append(buf);

   if (isWeapon()) {
      if ((st | ST_IDENTIFY) || viewer->isImmort()) {
         Sprintf(buf, "Weapon Damage: %id%i.\n",
                 getDiceCnt(), getDiceSides());
         rslt.append(buf);
      }
   }//else

   if ((st | ST_IDENTIFY) || viewer->isImmort()) {

      if (!stat_affects.isEmpty()) {
         rslt.append("Stat affects:\n");
         Markup::toString(&stat_affects, viewer, STAT_AFFECT_NAMES, buf);
         rslt.append(buf);
      }

      if (obj_proc && !CASTS_THESE_SPELLS.isEmpty()) {
         rslt.append("Casts these spells:\n");
         Markup::toString(&(CASTS_THESE_SPELLS), viewer, buf);
         rslt.append(buf);
      }//if

      Markup::toString(NULL, obj_flags, OBJ_FLAGS_NAMES, viewer, NULL, buf);
      rslt.append(buf);

      if (!inv.isEmpty()) {
         rslt.append("Inventory:\n");
         Markup::toString(&inv, viewer, OBJ_INV, buf);
         rslt.append(buf);
      }

      Sprintf(buf, "\nBelongs to zone:  %i.\n", getIdNum());
      rslt.append(buf);

      if (bag) {
         bag->toStringStat(viewer, buf, st);
         rslt.append(buf);
      }
   }//if immort or identify

   if (viewer->isImmort()) {
      Sprintf(buf, "\nchrgs: %i  rechrg: %i  p_load: %i  cur_in_game: %i.\n", 
              (int)(extras[0]), (int)(extras[1]), (int)(extras[2]),
              (int)(getCurInGame()));
      rslt.append(buf);

      Sprintf(buf, "max_in_game: %i  wt: %i  dice_sides: %i  #dice: %i.\n", 
              (int)(extras[4]), (int)(getCurWeight()), (int)(extras[6]),
              (int)(extras[7])); 
      rslt.append(buf);

      Sprintf(buf, "lvl: %i  price: %i  item#: %i zone# %i.\n", 
              (int)(extras[8]), cur_stats[1], cur_stats[2], cur_stats[3]);
      rslt.append(buf);

      if (obj_proc) {
         obj_proc->toStringStat(viewer, buf);
      }//if obj proc

      if (isModified()) 
         rslt.append("IS_MODIFIED\n");
      else 
         rslt.append("NOT_MODIFIED\n");
   }//if is immort

   if (viewer->isUsingClient()) {
      rslt.append("</OBJECT>\n");
   }
}//toStringStat


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
      SCell<object*> cll(inv);
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


StatBonus* object::hasStatAffect(int stat_num) {
   Cell<StatBonus*> cll(stat_affects);
   StatBonus* ptr;

   while ((ptr = cll.next())) {
      if (ptr->stat == stat_num)
         return ptr;
   }//while
   return NULL;
}//has_stat_affect


object* object::haveObjNumbered(int i_th, int num, int see_bit, room& rm) {
   SCell<object*> cll(inv);
   object* ptr;
   int count = 0;

   while ((ptr = cll.next())) {
      if (ptr->OBJ_NUM == num) {
         if (detect(see_bit, (ptr->getVisBit() | rm.getVisBit()))) { 
            count++;
            if (count == i_th) {
               return ptr;
            }//if
         }//if detect
      }//if obj nums agree
   }//while
   return NULL;
}//have_obj_numbered



/** These default to english, makes a copy of incoming data. */
void object::addShortDesc(String& new_val) {
   short_desc.addString(English, new_val);
}

void object::addShortDesc(LanguageE l, String& buf) {
   short_desc.addString(l, buf);
}

void object::addInRoomDesc(String& new_val) {
   in_room_desc.addString(English, new_val);
}

void object::addInRoomDesc(LanguageE l, String& new_val) {
   in_room_desc.addString(l, new_val);
}

void object::appendShortDesc(CSentryE msg) {
   for (int i = 0; i<LS_PER_ENTRY; i++) {
      LString nm((LanguageE)(i), CSHandler::getString(msg, (LanguageE)(i)));
      short_desc.appendString(nm);
   }
}

void object::appendShortDesc(String& msg) {
   short_desc.appendString(English, msg);
}

void object::prependShortDesc(String& str) {
   short_desc.prependString(English, str);
}

void object::appendInRoomDesc(CSentryE msg) {
   for (int i = 0; i<LS_PER_ENTRY; i++) {
      LString nm((LanguageE)(i), CSHandler::getString(msg, (LanguageE)(i)));
      in_room_desc.appendString(nm);
   }
}

   /** Makes a copy of incoming data. */
void object::addShortDesc(LString& new_val) {
   short_desc.addLstring(new_val);
}

void object::addInRoomDesc(LString& new_val) {
   in_room_desc.addLstring(new_val);
}

String* object::getShortDesc(critter* observer) {
   if (detect(observer->getSeeBit(), vis_bit)) {
      return short_desc.getString(observer->getLanguage());
   }
   else {
      return &UNKNOWN;
   }
}

String* object::getShortDesc(int see_bit) {
   if (detect(see_bit, vis_bit)) {
      return short_desc.getString(English);
   }
   else {
      return &UNKNOWN;
   }
} 

String* object::getInRoomDesc(critter* observer) {
   if (detect(observer->getSeeBit(), vis_bit)) {
      return in_room_desc.getString(observer->getLanguage());
   }
   else {
      return &UNKNOWN;
   }
}

object* object::haveObjNamed(int i_th, const String* name, critter* viewer) {
   int foo = 0;
   int c_bit = ~0;
   LanguageE lang = English;
   if (viewer) {
      c_bit = viewer->getSeeBit();
      lang = viewer->getLanguage();
   }
   return ::have_obj_named(inv, i_th, name, c_bit, *(getCurRoom()),
                           foo, lang);
}

StatBonus* object::haveStatAffect(int stat_num) {
   Cell<StatBonus*> cll(stat_affects);
   StatBonus* ptr;

   while ((ptr = cll.next())) {
      if (ptr->stat == stat_num)
         return ptr;
   }//while
   return NULL;
}//is_affected_by

