// $Id: object.cc,v 1.19 1999/07/30 06:42:23 greear Exp $
// $Revision: 1.19 $  $Author: greear $ $Date: 1999/07/30 06:42:23 $

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

int ConstructData::_cnt = 0;

ConstructData::ConstructData() {
   _cnt++;
   target_object = item1 = item2 = item3 = item4 = item5 = 0;
}//constructor

ConstructData::ConstructData(const ConstructData& source) {
   _cnt++;
   target_object = source.target_object;
   item1 = source.item1;
   item2 = source.item2;
   item3 = source.item3;
   item4 = source.item4;
   item5 = source.item5;
}//copy constructor

void ConstructData::Read(ifstream& da_file) {
   char buf[100];

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in ConstructData read." << endl;
      }
      return;
   }

   da_file >> target_object >> item1 >> item2 >> item3 >> item4 >> item5;
   da_file.getline(buf, 99);
}//Read

void ConstructData::Write(ofstream& da_file) const {
   da_file << target_object << " " << item1 << " " << item2 << " "
           << item3 << " " << item4 << " " << item5;
   da_file << "\t target object, companions\n";
}//Write

void ConstructData::makeComponent(int targ, int comp1, int comp2, int comp3,
                                  int comp4, int comp5) {
   target_object = targ;
   item1 = comp1;
   item2 = comp2;
   item3 = comp3;
   item4 = comp4;
   item5 = comp5;
}

int ProcObject::_cnt = 0;

ProcObject::ProcObject() { //constructor
   _cnt++;
   construct_data = NULL;
   skin_ptr = NULL;
   w_eye_owner = NULL;
}//constructor

ProcObject::ProcObject(const ProcObject& source) { //copy constructr
   _cnt++;
   construct_data = NULL;
   skin_ptr = NULL;
   w_eye_owner = NULL;
   *this = source;
}//constructor

ProcObject::~ProcObject() { //destructor
   _cnt--;
   Clear();
}//destructor

ProcObject& ProcObject::operator=(const ProcObject& source) { 

   if (this == &source)
      return *this;

   Clear();  //clear current mess
   proc_flags = source.proc_flags;
   if (source.construct_data) {
      construct_data = new ConstructData(*(source.construct_data));
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

void ProcObject::Clear() {
   proc_flags.Clear();

   delete construct_data;
   construct_data = NULL;
   
   if (skin_ptr && skin_ptr->isModified()) {
      delete skin_ptr;
   }//if
   skin_ptr = NULL;

   if (w_eye_owner) {
      w_eye_owner->setWizardEyeObject(NULL);
      w_eye_owner = NULL;
   }//if

   w_eye_owner = NULL;
   casts_these_spells.clearAndDestroy();
}//clear


void ProcObject::Read(ifstream& da_file) {
   char buf[82];
   int tmp;

   Clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in ProcObject read." << endl;
      }
      return;
   }

   proc_flags.Read(da_file);

   proc_flags.turn_off(4); //hack!!
   proc_flags.turn_off(5);
   proc_flags.turn_off(6);
   proc_flags.turn_off(7);
   proc_flags.turn_off(8);
   proc_flags.turn_off(9);

   if (proc_flags.get(1)) { //if have construct data
      if (!construct_data) {
         construct_data = new ConstructData;
      }//if
      construct_data->Read(da_file);
   }//if

   if (proc_flags.get(2)) {
      da_file >> tmp;
      if (check_l_range(tmp, 1, NUMBER_OF_ITEMS, mob_list[0], FALSE)) {
         skin_ptr = &(obj_list[tmp]);
      }//if
      else {
         skin_ptr = NULL;
         proc_flags.turn_off(2); //doesn't have one after all
      }//else
      da_file.getline(buf, 80);
   }//if
   
   if (proc_flags.get(10)) {
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


void ProcObject::Write(ofstream& da_file) const {
   proc_flags.Write(da_file);
   if (proc_flags.get(1)) { //if have construct data
      if (!construct_data) {
         mudlog.log(ERR, "ERROR:  trying to write NULL construct data.\n");
         return;
      }//if
      construct_data->Write(da_file);
   }//if
   if (proc_flags.get(2)) {
     if (!skin_ptr) {
       da_file << 0 << "     skin number\n";
     }//if
     else {
       da_file << skin_ptr->getIdNum() << "	skin number\n";
     }//else
   }//if

   if (proc_flags.get(10)) {
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

int BaseBag::_cnt = 0;

BaseBag::BaseBag() {
   _cnt++;
}//constructor

BaseBag::BaseBag(BaseBag& source) {
   _cnt++;
   *this = source; //a shallow copy should work
}//copy constructor

BaseBag& BaseBag::operator=(BaseBag& src) {
   object* obj_ptr;
   Cell<object*> cll(src.getInv());
   while ((obj_ptr = cll.next())) {
      if (!obj_ptr->isModified()) { //multiple ptrs to SOBJ's will be a problem
         append(obj_ptr);
      }//if
   }//while

   max_weight = src.max_weight;
   percentage_weight = src.percentage_weight;
   time_till_disolve = src.time_till_disolve;
   return *this;
}//operator=
   


void BaseBag::Write(ofstream& ofile) const {
   ofile << " " << max_weight << " " << percentage_weight << " "
         << time_till_disolve << " " << "\t bag_stuff\n";
}//Write()


void BaseBag::Read(ifstream& ofile) {
   char buf[81];
   
   ofile >> max_weight >> percentage_weight 
         >> time_till_disolve;
   ofile.getline(buf, 80);
}//Read()


//*************************************************************//
///**********************  object  ***************************///

int object::_cnt = 0;

object::object (object& source) { //copy constructor
   _cnt++;

   *this = source;

   obj_ptr_log << "OBJ_CC " << getIdNum() << " " << this << "\n";
}//obj copy constructor


object::object() {
   int i;

   _cnt++;
  
   for (i = 0; i<OBJ_MAX_EXTRAS; i++)
      extras[i] = 0;       
   for (i = 0; i<OBJ_CUR_STATS; i++)
      cur_stats[i] = 0;    
   
   // Can't have an obj# at this point.
   obj_ptr_log << "OBJ_CON 0 " << this << "\n";
}//default constructor


object::~object() {
   _cnt--;

   obj_ptr_log << "OBJ_DES " << getIdNum() << " " << this << "\n";

   if ((!isModified()) && (!do_shutdown)) {
      mudlog.log(ERR, "ERROR:  trying to delete OBJ before shutdown.\n");
      core_dump("~object");
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
   
   int i;
   
   if (!isModified() && obj_flags.get(10)) {
      mudlog.log(ERR, "ERROR:  OBJ being assigned to... should be a SOBJ!\n");
      core_dump("object::operator=");
      //do_shutdown = TRUE;
      //exit(2);
   }//if
   
   Clear();
   
   short_desc = source.short_desc;
   in_room_desc = source.in_room_desc;
   long_desc = source.long_desc;
   
   obj_flags = source.obj_flags; //use bitfield's overloaded =

   if (!isInUse()) { //in use
      mudlog << "ERROR:  assigning obj, flag 10 (in use) not set."
             << endl;
      obj_flags.turn_on(10);
   }
   
   for (i = 0; i<OBJ_MAX_EXTRAS; i++) 
      extras[i] = source.extras[i];
   
   for (i = 0; i<OBJ_CUR_STATS; i++) 
      cur_stats[i] = source.cur_stats[i];

   stat_affects.becomeDeepCopyOf(source.stat_affects);
   
   return *this;
}//operator= overload
	    

void object::Clear() {
   short_desc.Clear();
   in_room_desc.Clear();
   
   obj_flags.Clear();
   
   stat_affects.clearAndDestroy();
   
   setContainer(NULL);
}//Clear


int BaseBag::getCurWeight() {
   Cell<object*> cll(getInv());
   object* ptr;
   int tmp_wt = 0;
   while ((ptr = cll.next())) {
      tmp_wt += ptr->getCurWeight();
   }//while
   return tmp_wt;
}//getCurWeight


void object::Write(ofstream& ofile) {
   // TODO
   ofile << "End of Object.\n\n";
}//Write...obj


void object::Read(ifstream& ofile, short read_all) {
   // TODO
}

int object::isMagic() {
   if (!stat_affects.isEmpty() || !affected_by.isEmpty())
      return TRUE;
   else
      return FALSE;
}//is_magic


int object::getZoneNum() const {
   return OBJ_IN_ZONE;
}

int object::isFood() const {
   return (OBJ_FLAGS.get(61));
}

int object::isPotion() const {
   return (OBJ_FLAGS.get(52));
}

int object::isScroll() const {
   return (OBJ_FLAGS.get(53));
}

int BaseBag::getEntityCount(LEtypeE type, int id_num, int sanity) {
   Cell<object*> cll(getInv());
   object* ptr;
   int count = 0;

   if (sanity > 20) {
      return 0;
   }

   while ((ptr = cll.next())) {
      if ((ptr->getIdNum() == id_num) && ptr->isEntityType(type)) {
         count++;
      }//if detect
      count += ptr->getEntityCount(type, id_num, sanity + 1);
   }//while
   
   return count;
}//getEntityCount


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
   
int object::decrementCurInGame() {
   return --(obj_list[getIdNum()].extras[3]);
}

int object::incrementCurInGame() {
   return ++(obj_list[getIdNum()].extras[3]);
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

   room_list[cur_room].loseObject(this);

   leave_room_effects(room_list[cur_room], *this);

   room_list[dest_room].gainObject(this);

   // This can cause us to delete ourselves btw...not very good coding
   // but..maybe it will work!
   do_entered_room_procs(*this, by_door, from_dir, room_list[dest_room],
                         sanity);

   return 0;
}//doGoToRoom

void BaseBag::loseObject_(object* obj) {
   remove(obj);
}

void Bag::gainObject(Entity* obj) {
   gainObject_((object*)(obj));
}

void Bag::gainObject_(object* obj) {
   prepend(obj);
   if (obj->isModified()) {
      obj->setContainer(this);
   }
}//gainObject

void ProcBag::gainObject(Entity* obj) {
   gainObject_((object*)(obj));
}

void ProcBag::gainObject_(object* obj) {
   prepend(obj);
   if (obj->isModified()) {
      obj->setContainer(this);
   }
}//gainObject


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



void ProcObject::makeComponent(int targ, int comp1, int comp2, int comp3,
                               int comp4, int comp5, ComponentEnum con_type) {

   OBJ_FLAGS.turn_on(63); //has spec proc data

   if (!construct_data) {
      construct_data = new ConstructData();
   }

   construct_data->makeComponent(targ, comp1, comp2, comp3, comp4, comp5);

   proc_flags.turn_on(1); //set the flag to make it a construct object

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


