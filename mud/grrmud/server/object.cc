// $Id: object.cc,v 1.20 1999/08/01 08:40:23 greear Exp $
// $Revision: 1.20 $  $Author: greear $ $Date: 1999/08/01 08:40:23 $

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

   Clear();  //clear current mess
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

void obj_spec_data::Clear() {
   obj_spec_data_flags.Clear();
   delete construct_data;
   construct_data = NULL;
   
   if (skin_ptr && skin_ptr->isModified()) {
      delete skin_ptr;
   }//if
   skin_ptr = NULL;

   w_eye_owner = NULL;
   casts_these_spells.clearAndDestroy();
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
         casts_these_spells.append(new SpellDuration(first, second));
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


int bag_struct::read(istream& ofile, int read_all = TRUE) {
   char buf[81];
   String str(80);
   ofile >> str; //grab the first token, if V2.x it will be a number (bitfield)

   if (isnum(str)) {
      if (strcmp(str, "-1") != 0) {
         bitfield bf;
         bf.Read(ofile);
         bf.turn_on(atoi(buf));

         // Now, set all these flags.  Hateful, but we gotta translate them to
         // how the Closable's flags like it.
         setClosed(bf.get(2));
         setLocked(bf.get(3));
         setPickable(bf.get(4));
         setMagLockable(bf.get(5));
         setMagLocked(bf.get(6));
         setIsDestructable(bf.get(7));
         setIsCorpse(bf.get(8));
         setIsNoClose(bf.get(9));
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

   Clear();
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
   
   Clear();
   
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
	    

void object::Clear() {
   Entity::clear();
   Scriptable::clear();

   short_desc.clear();
   in_room_desc.clear();
   
   obj_flags.Clear();
   
   delete bag;
   bag = NULL;

   stat_affects.clearAndDestroy();
   clear_obj_list(inv);
   
   delete obj_proc;
   obj_proc = NULL;
}//Clear


int object::getCurWeight() const {
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
   SpellDuration* sd_ptr;
   StatBonus* sb_ptr;
   SCell<object*> ob_cell;
   object* ob_ptr;

   ofile << "<META> ENTITY=OBJ VERSION=3.0 </META>\n";

   Entity::write(ofile);
   Scriptable::write(ofile);

   short_desc.write(ofile);
   in_room_desc.write(ofile);
   
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
         return;
      }//if
      obj_proc->write(ofile);
   }//if has spec_data

   ofile << "End of Object.\n\n";
}//Write...obj


int object::read_v2(ifstream& ofile, String& first_name, short read_all) {
   int i, test = TRUE;
   StatBonus* ss_ptr;
   SpellDuration* sd_ptr;
   char tmp[81];
   String tmp_str(280);
   
   Clear();  //stop up any memory leaks etc.

   mudlog.log(DB, "In obj::Read.\n");

   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in obj read." << endl;
      }
      return -1;
   }

   appendName(new String(first_name));

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
         appendName(new String(tmp_str));
      }//else
   }//while            
   ofile.getline(tmp, 80);         

   mudlog.log(DB, "About to do termed read..");

   tmp_str.Termed_Read(ofile);
   setShortDest(tmp_str);
   
   tmp_str.Termed_Read(ofile);
   setInRoomDesc(tmp_str);

   tmp_str.Termed_Read(ofile);
   setLongDesc(tmp_str);
   
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
         return;
      }

      if (i == -2) { //assume its supposed to load, gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->Read(ofile, TRUE);

         inv.append(new_obj);    //add it to inventory
         affected_objects.append(new_obj);

         new_obj->isModified() = &(inv); //make sure its a SOBJ
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
}//Read_v2


int object::read_v3(ifstream& ofile, short read_all) {
   int i, test = TRUE;
   StatBonus* ss_ptr;
   char tmp[81];
   String tmp_str(80);
   String* string;
   
   Clear();  //stop up any memory leaks etc.

   mudlog.log(DB, "In obj::Read_v3.\n");

   Entity::read(ofile);
   Scriptable::read(ofile);

   short_desc.read(ofile);
   in_room_desc.read(ofile);
   
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

         new_obj->setContainer(this);
         new_obj->setModified(TRUE);
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
      obj_proc->Read(ofile);
   }//if

   /* if charges < -1, make them -1 (infinity) */
   if (extras[0] < -1) {
      extras[0] = -1;
   }

   //mudlog.log(DBG, "Done reading in obj.\n");
   ofile.getline(tmp, 80); //junk end of obj msg
   ofile.getline(tmp, 80); //junk end of obj space

   OBJ_VIS_BIT |= 1024; //hack, turn on 'normal' bit
}//Read_v3

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
