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

//***********************************************************//
//************************  mob data  **********************///

#include "script.h"
#include "critter.h"
#include "misc.h"
#include "misc2.h"
#include <PtrArray.h>
#include "room.h"
#include "const.h"
#include "commands.h"
#include "command5.h"
#include "batl_prc.h"
#include "command2.h"



const char* PcPositionStrings[] = {"stand", "sit", "rest", "sleep", "meditate",
                                 "stun", "dead", "prone"};

//**********************************************************//
///*********************** immort data ********************///

int immort_data::_cnt = 0;

immort_data::immort_data() {
   _cnt++;
   olc_counter = temp_olc_int = imm_level = 0;
   olc_mob = NULL;
   olc_room = NULL;
   olc_obj = NULL;
   olc_door = NULL;
   edit_string = NULL;
   tmp_proc_script = NULL;
} // constructor


immort_data::immort_data(const immort_data& source) {
   _cnt++;
   olc_counter = source.olc_counter;
   temp_olc_int = source.temp_olc_int;
   olc_mob = source.olc_mob;
   olc_room = source.olc_room;
   olc_obj = source.olc_obj;
   olc_door = source.olc_door;
   imm_level = source.imm_level;
   edit_string = source.edit_string;
   tmp_proc_script = NULL;
} // constructor


immort_data::~immort_data() {
   _cnt--;
   Clear();
}//deconstructor
   
void immort_data::Clear() {
   if (olc_mob) {
      olc_mob->Clear();
      olc_mob = NULL;
   }//if
   
   if (olc_room) {
      olc_room->Clear();
      olc_room = NULL;
   }//if
   
   if (olc_obj) {
      olc_obj->Clear();
      olc_obj = NULL;
   }//if
   
   if (olc_door) {
      olc_door->Clear();
      olc_door = NULL;
   }//if
   clear_ptr_list(tmplist);
   olc_counter = temp_olc_int = imm_level = 0;
   edit_string = NULL; //clearing it probably not the right solution

   delete tmp_proc_script;
   tmp_proc_script = NULL;

} // Clear


void immort_data::Read(ifstream& dafile) {
   char buf[82];

   dafile >> imm_level;

   dafile.getline(buf, 80); //junk end of line
}//Read

void immort_data::Write(ofstream& dafile) {
   dafile << imm_level << "\tImm level\n";
}//Write


//**********************************************************//
///*********************  teacher_data  ********************///

int teacher_data::_cnt = 0;

teacher_data::teacher_data() {
   _cnt++;
}//constructor

teacher_data::teacher_data(const teacher_data& source) {
   _cnt++;
   teach_data_flags = source.teach_data_flags;
}//constructor

void teacher_data::Read(ifstream& da_file) {
   teach_data_flags.Read(da_file);
}//Read

void teacher_data::Write(ofstream& da_file) const {
   teach_data_flags.Write(da_file);
}//Write

//****************************************************************//
///*********************  Player Shop Data  *********************///

int PlayerShopData::_cnt = 0; // Instance Count

int PlayerShopData::read(ifstream& da_file) {
   int sentinel;
   char tmp[81];

   da_file >> sentinel;
   if (sentinel == -1) {
      da_file.getline(tmp, 80);
      return 0;
   }
   else {
      da_file >> object_num;
      da_file >> sell_price;
      da_file >> buy_price;
      da_file.getline(tmp, 80);
      return object_num;
   }//else
}//read
      
void PlayerShopData::write(ofstream& da_file) const {
   da_file << 1 << " " << object_num << " "
           << sell_price << " " << buy_price << " PlayerShopData \n";
}//write




//**********************************************************//
///*********************  shop data  ***********************///

int shop_data::_cnt = 0;

shop_data::shop_data() {
   _cnt++;
   markup = buy_percentage = open_time = close_time = 0;
}//constructor;

shop_data::shop_data(const shop_data& source) {
   _cnt++;
   *this = source;
}//constructor;

shop_data::~shop_data() {
   _cnt--;
   Clear();
}


shop_data& shop_data::operator=(const shop_data& source) {
   
   if (this == &source)
      return *this;

   clear_obj_list(perm_inv);

   markup = source.markup;
   buy_percentage = source.buy_percentage;
   open_time = source.open_time;
   close_time = source.close_time;
   
   Cell<object*> cll(source.perm_inv);
   object* ptr;
   while ((ptr = cll.next())) {
      if (!ptr->IN_LIST) {
         perm_inv.append(ptr);
      }//if
   }//while

   // Deep copy here...
   Cell<PlayerShopData*> pcll(source.ps_data_list);
   PlayerShopData* pptr;
   while ((pptr = pcll.next())) {
      ps_data_list.pushBack(new PlayerShopData(*pptr));
   }
  
   perm_inv = source.perm_inv;
   shop_data_flags = source.shop_data_flags;

   return *this;
}//operator=;

void shop_data::Clear() {
   markup = buy_percentage = close_time = open_time = 0;
   clear_obj_list(perm_inv);
   shop_data_flags.Clear();
   clear_ptr_list(ps_data_list);
}//Clear

void shop_data::Read(ifstream& da_file, short read_all) {
   int i;
   String buf(100);
   char tmp[81];
   
   Clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in shop_data read." << endl;
      }
      return;
   }


   da_file >> markup >> buy_percentage >> open_time >> close_time;
   da_file.getline(tmp, 80);
   
   shop_data_flags.Read(da_file);
   
   /*  Inventory */
   da_file >> i;
   while (i != -1) {
      if (i == -2) { //assume its gonna load fer sure
         object* new_obj = new object;
         da_file.getline(tmp, 80);  //junk message
         new_obj->Read(da_file, read_all);
         new_obj->IN_LIST = &(perm_inv); //make sure its a SOBJ
         perm_inv.append(new_obj);    //add it to inventory
      }//if
      else {
         if (obj_list[i].OBJ_FLAGS.get(10)) {
            if (read_all || 
                ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 100) > 
                d(1,100)) {
               perm_inv.append(&(obj_list[i]));    //add it to inventory
            }//if
         }//if
         else {
            Sprintf(buf, 
                    "ERROR:  trying to load non_exist. obj: %i in perm_inv.\n",
                    i);
            mudlog.log(ERR, buf);
         }//else
      }//else
      da_file >> i;
   }//while
   da_file.getline(tmp, 80);

   if (isPlayerRun()) {
      PlayerShopData* ps_data = new PlayerShopData();
      while (ps_data->read(da_file)) {
         ps_data_list.pushBack(ps_data);
         ps_data = new PlayerShopData();
      }//while

      // Delete the last one, was not added to the list.
      delete ps_data;
      ps_data = NULL;

      da_file >> manager;
      da_file.getline(tmp, 80);

   }//if is a Player Run Shopkeeper


}//Read


void shop_data::Write(ofstream& da_file) const {
   Cell<object*> ob_cell(perm_inv);
   object* ob_ptr;
   int i = 0;
   
   da_file << " " << markup << " " << buy_percentage << " " 
           << open_time << " " << close_time 
           << "\tshop_data shorts\n";
   shop_data_flags.Write(da_file);
   
   /*  Inventory */
   i = 0;
   while ((ob_ptr = ob_cell.next())) {
      if (ob_ptr->IN_LIST) {
         da_file << -2 << "\t Start of SOBJ\n";
         ob_ptr->Write(da_file);
      }//if
      else {
         if (obj_list[ob_ptr->OBJ_NUM].OBJ_FLAGS.get(10)) {
            da_file << ob_ptr->OBJ_NUM << " ";
            if ((++i % 20) == 0)
               da_file << endl;
         }//if it exists
      }//else
   }//while
   da_file << "-1  End of perm_inv\n";

   if (isPlayerRun()) {
      Cell<PlayerShopData*> pcll(ps_data_list);
      PlayerShopData* pptr;
      while ((pptr = pcll.next())) {
         pptr->write(da_file);
      }
      da_file << "-1  End of PlayerShopData list\n";

      if (manager.Strlen() == 0) {
         da_file << "Grock" << "  Manager\n"; //default value...
      }
      else {
         da_file << manager << "  Manager\n";
      }
   }//if

}//Write


void shop_data::valueRem(int idx, critter& manager) {
   PlayerShopData* psd = ps_data_list.elementAt(idx);
   if (psd) {
      ps_data_list.loseData(psd);
      delete psd;
      
      manager.show("Ok, removed it.\n");
   }//if
   else {
      manager.show("That index does not exist.\n");
   }//else
}//valueRem

void shop_data::valueAdd(object& obj, critter& manager) {
   if (getPsdFor(obj)) {
      manager.show("That object has already been added.\n");
   }
   else {
      ps_data_list.pushBack(new PlayerShopData(obj.getIdNum(),
                                               obj.getDefaultPrice() * 3 / 2,
                                               obj.getDefaultPrice() * 2 / 3));
      manager.show("Value added, you will probably want to set it next.\n");
   }//else
}//valueAdd

int shop_data::findItemSalePrice(object& obj) {
   PlayerShopData* ptr = getPsdFor(obj);

   if (ptr) {
      return ptr->getSellPrice();
   }

   return -1;
}//findItemSalePrice

int shop_data::findItemBuyPrice(object& obj) {
   PlayerShopData* ptr = getPsdFor(obj);

   if (ptr) {
      return ptr->getBuyPrice();
   }

   return -1;
}//findItemBuyPrice


PlayerShopData* shop_data::getPsdFor(object& obj) {
   Cell<PlayerShopData*> cll(ps_data_list);
   PlayerShopData* ptr;
   
   while ((ptr = cll.next())) {
      if (ptr->getObjNum() == obj.getIdNum()) {
         return ptr;
      }//if
   }//while

   return NULL;
}//getPsdFor


void shop_data::valueList(int i_th, const String* targ, critter& keeper, 
                          critter& manager) {
   String normal(500);
   String client(500);
   String buf(100);
   int idx = 0;

   Cell<PlayerShopData*> cll(ps_data_list);
   PlayerShopData* ptr;

   if (keeper.isManagedBy(manager)) {
      Sprintf(client, "<VALUE_LIST %i %S %i> ", i_th, targ, keeper.GOLD);
      Sprintf(normal, "Value listing for %i.%S, register:  %i\n", i_th,
              targ, keeper.GOLD);
   }
   else {
      Sprintf(client, "<VALUE_LIST %i %S ??> ", i_th, targ);
      Sprintf(normal, "Value listing for %i.%S:\n", i_th, targ);
   }


   while ((ptr = cll.next())) {
      if (manager.isUsingClient()) {
         Sprintf(buf, "<VALUE_ITEM %i %i %i %i> ", idx,
                 ptr->getObjNum(), ptr->getSellPrice(),
                 ptr->getBuyPrice());
         client.Append(buf);

         Sprintf(buf, "<VALUE_INAME %i %S> ", idx,
                 obj_list[ptr->getObjNum()].getLongName());
         client.Append(buf);

      }//if
      
      Sprintf(buf, "    [%i]  Sell: %i %P20 Buy: %i %P30 Item: %S(%i)\n", idx,
              ptr->getSellPrice(),
              ptr->getBuyPrice(),
              obj_list[ptr->getObjNum()].getLongName(),
              ptr->getObjNum());
      normal.Append(buf);
      idx++;
   }//while

   if (manager.isUsingClient()) {
      manager.show(client);
   }

   manager.show(normal);
}//valueList


void shop_data::valueSet(int val_idx, int sell_val, int buy_val,
                         critter& manager) {
   PlayerShopData* psd = ps_data_list.elementAt(val_idx);
   if (psd) {
      psd->setSellPrice(sell_val);
      psd->setBuyPrice(buy_val);

      manager.show("Ok, new values set.\n");
   }//if
   else {
      manager.show("That index is not valid.\n");
   }//else
}//valueSet


///***************************************************************///
///*************** temp proc data ***********************///

int temp_proc_data::_cnt = 0;

temp_proc_data::temp_proc_data() {
   _cnt++;
}//constructor

temp_proc_data::temp_proc_data(const temp_proc_data& source) {
   _cnt++;
   *this = source;
}//copy constructor

temp_proc_data::~temp_proc_data() {
   _cnt--;
   clear_ptr_list(hunting);
}//destructor

void temp_proc_data::Clear() {
   clear_ptr_list(hunting);
   tracking.Clear();
}//clear

temp_proc_data& temp_proc_data::operator= (const temp_proc_data& source) {

   if (this == &source)
      return *this;

   String* sp;
   Cell<String*> cll(source.hunting);
   String* ptr;

   Clear();
   while ((ptr = cll.next())) {
      sp = new String(*ptr);
      hunting.append(sp);
   }//while
   tracking = source.tracking;
   return *this;
}//operator= overload


///*******************************************************************///
///**********************  say proc cell  ***************************///

int say_proc_cell::_cnt = 0;

say_proc_cell::say_proc_cell() {
   _cnt++;
   obj_num = 0; //one to be given
   trans_to_room = 0;
}//constructor

say_proc_cell::say_proc_cell(const say_proc_cell& source) {
   _cnt++;
   *this = source;
}//copy constructor

void say_proc_cell::Clear() {
   topic.Clear();
   msg.Clear();
   skill_name.Clear();
   obj_num = 0;
   trans_to_room = 0;
}//Clear  

void say_proc_cell::Read(ifstream& da_file) {
   char buf[82];

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in say_proc_cell read." << endl;
      }
      return;
   }

   
   da_file.getline(buf, 80);         
   topic = buf;
   
   msg.Termed_Read(da_file);
   
   da_file.getline(buf, 80);
   skill_name = buf;

   da_file >> obj_num >> trans_to_room;
   da_file.getline(buf, 80);
}//read()


void say_proc_cell::Write(ofstream& da_file) {

   da_file << topic << endl;

   parse_for_max_80(msg);
   da_file << msg << endl << "~" << endl;
   
   da_file << skill_name << endl;
   da_file << obj_num << " " << trans_to_room << "\t end of say_proc_cell\n";
}//write()

  

///*******************************************************************///
///**********************  action proc data  ***************************///

int action_proc_data::_cnt = 0;

action_proc_data::action_proc_data() {
   _cnt++;
   test_num = 0;
   obj_num = 0; //one to be given
   trans_to_room = 0;
}//constructor

action_proc_data::action_proc_data(const action_proc_data& source) {
   _cnt++;
   *this = source;
}//copy constructor


action_proc_data::~action_proc_data() {
   _cnt--;
}//destructor


action_proc_data& action_proc_data::operator=(const action_proc_data& source) {

   if (this == &source)
      return *this;

   test_num = source.test_num;
   correct_msg = source.correct_msg;
   skill_name = source.skill_name;
   obj_num = source.obj_num;
   trans_to_room = source.trans_to_room;
   wrong_gift_msg = source.wrong_gift_msg;

   return *this;
}//operator = overload


void action_proc_data::Clear() {
   test_num = 0;
   correct_msg.Clear();
   skill_name.Clear();
   obj_num = 0;
   trans_to_room = 0;
   wrong_gift_msg.Clear();
}//Clear  

void action_proc_data::Read(ifstream& da_file) {
   char buf[82];

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in action_proc_data read." << endl;
      }
      return;
   }


   //mudlog.log(DBG, "Reading action_proc.\n");
   da_file >> test_num; 
   da_file.getline(buf, 80);         

   //mudlog.log(DBG, "got test_num.\n");
   correct_msg.Termed_Read(da_file);

   //mudlog.log(DBG, "got correct_msg:");
   //mudlog.log(DBG, correct_msg);
   
   da_file.getline(buf, 80);
   skill_name = buf;

   //mudlog.log(DBG, "Got skillname: ");
   //mudlog.log(DBG, skill_name);

   da_file >> obj_num >> trans_to_room;
   da_file.getline(buf, 80);         

   //mudlog.log(DBG, "Getting wrong_gift_msg:");
   wrong_gift_msg.Termed_Read(da_file);
   da_file.getline(buf, 80);      
   //mudlog.log(DBG, wrong_gift_msg);   
}//read()


void action_proc_data::Write(ofstream& da_file){

   da_file << test_num << "\t test_num, valid for give_proc mostly\n";

   parse_for_max_80(correct_msg);
   da_file << correct_msg << endl << "~" << endl;

   da_file << skill_name << endl;

   da_file << obj_num << " " << trans_to_room << "\tobj_num, trans_room#\n";

   parse_for_max_80(wrong_gift_msg);
   da_file << wrong_gift_msg << endl << "~" << endl;
   da_file << "end action_proc_data\n";
}//write()

  
//************************************************************//
///********************* spec data  *************************///

int spec_data::_cnt = 0;

spec_data::spec_data() {
   _cnt++;
   int1 = 0;
   skill_violence = benevolence = defensiveness = bad_assedness =
      social_awareness = 0;
   sh_data = NULL;
   teach_data = NULL;
   temp_proc = NULL;
   give_proc = NULL;
   bow_proc = NULL;
   curse_proc = NULL;
}//constructor


spec_data::spec_data(const spec_data& source) {
   _cnt++;
   sh_data = NULL;
   teach_data = NULL;
   temp_proc = NULL;
   give_proc = NULL;
   bow_proc = NULL;
   curse_proc = NULL;
   *this = source;  //overloaded =
}//constructor

spec_data::~spec_data() {
   _cnt--;
   delete sh_data;
   sh_data = NULL;
   delete teach_data;
   teach_data = NULL;
   delete temp_proc;
   temp_proc = NULL;
   delete give_proc;
   give_proc = NULL;
   delete bow_proc;
   bow_proc = NULL;
   delete curse_proc;
   curse_proc = NULL;

   clear_ptr_list(topics);
}//destructor

void spec_data::Clear() {
   delete sh_data;
   sh_data = NULL;
   delete teach_data;
   teach_data = NULL;
   delete temp_proc;
   temp_proc = NULL;
   delete give_proc;
   give_proc = NULL;
   delete bow_proc;
   bow_proc = NULL;
   delete curse_proc;
   curse_proc = NULL;
   
   flag1.Clear();
   int1 = 0;
   clear_ptr_list(topics);
   wrong_align_msg.Clear();
   wrong_class_msg.Clear();
   wrong_race_msg.Clear();
   skill_violence = benevolence = defensiveness = bad_assedness =
      social_awareness = 0;
}//Clear


void spec_data::auditAI() {
   if (!skill_violence || !benevolence || !defensiveness ||
       !bad_assedness || !social_awareness) {
      flag1.turn_on(13);
   }//if
   else {
      flag1.turn_off(13);
   }
}//auditAI


spec_data& spec_data::operator=(const spec_data& source) { 

   if (this == &source)
      return *this;

   Clear();
   flag1 = source.flag1;
   int1 = source.int1;
   if (source.sh_data)
      sh_data = new shop_data(*(source.sh_data));
   if (source.teach_data) 
      teach_data = new teacher_data(*(source.teach_data));
   if (source.temp_proc)
      temp_proc = new temp_proc_data(*(source.temp_proc));
   if (source.give_proc)
      give_proc = new action_proc_data(*(source.give_proc));
   if (source.bow_proc)
      bow_proc = new action_proc_data(*(source.bow_proc));
   if (source.curse_proc)
      curse_proc = new action_proc_data(*(source.curse_proc));

   Cell<say_proc_cell*> cll(source.topics);
   say_proc_cell* ptr;
   while ((ptr = cll.next())) {
     topics.append(new say_proc_cell(*ptr));
   }//while

   wrong_align_msg = source.wrong_align_msg;
   wrong_class_msg = source.wrong_class_msg;
   wrong_race_msg = source.wrong_race_msg;
   skill_violence = source.skill_violence;
   benevolence = source.benevolence;
   defensiveness = source.defensiveness;
   bad_assedness = source.bad_assedness;
   social_awareness = source.social_awareness;

   return *this;
}//op = overload


void spec_data::Read(ifstream& da_file, short read_all) {
   char tmp[81];
   int test;
  
   Clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in spec_data read." << endl;
      }
      return;
   }

   flag1.Read(da_file);
   
   flag1.turn_off(12); //hack, get rid of this flag
  
   da_file >> int1;
   da_file.getline(tmp, 80);

   if (flag1.get(1)) {  // shopkeeper
      sh_data = new shop_data;
      sh_data->Read(da_file, read_all);
   }//if

   if (flag1.get(2)) {  // teacher
      teach_data = new teacher_data;
      teach_data->Read(da_file);
   }//if

   if (flag1.get(5)) { //give_proc
      give_proc = new action_proc_data;
      give_proc->Read(da_file);
   }//if

   if (flag1.get(7)) { //bow_proc
      bow_proc = new action_proc_data;
      bow_proc->Read(da_file);
   }//if

   if (flag1.get(8)) { //curse_proc
      curse_proc = new action_proc_data;
      curse_proc->Read(da_file);
   }//if

   if (flag1.get(6)) { //say_proc
      da_file >> test;
      da_file.getline(tmp, 80);
      say_proc_cell* ptr;
      while (test != -1) {
         ptr = new say_proc_cell;
         ptr->Read(da_file);
         topics.append(ptr);
         da_file >> test;
         da_file.getline(tmp, 80);
      }//while
   }//if

   if (flag1.get(5) || flag1.get(6) || flag1.get(7) || flag1.get(8)) {
      // then assume messages are there, ignore otherwise
      wrong_align_msg.Termed_Read(da_file);
      wrong_class_msg.Termed_Read(da_file);
      wrong_race_msg.Termed_Read(da_file);
   }//if

   if (flag1.get(13)) { //if HAS_MOB_AI
      da_file >> skill_violence >> benevolence >> defensiveness
              >>  bad_assedness >> social_awareness;
      da_file.getline(tmp, 80);      
   }//if
}//Read


void spec_data::Write(ofstream& da_file){

   if (skill_violence || benevolence || defensiveness || bad_assedness) {
      flag1.turn_on(13); //make sure
   }//if

   flag1.Write(da_file);
   da_file << int1 << "\tint1, above bitfield started spec_data\n";
  
   if (flag1.get(1)) {  // shopkeeper
      if (!sh_data) {
         mudlog.log(ERR, "ERROR:  trying to write sh_data, but it's NULL\n");
         return;
      }//if
      sh_data->Write(da_file);
   }//if

   if (flag1.get(2)) {  // teacher
      if (!teach_data) {
         mudlog.log(ERR, 
                    "ERROR:  trying to write teach_data, but it's NULL\n");
         return; 
      }//if
      teach_data->Write(da_file);
   }//if

   if (flag1.get(5)) { //give_proc
      give_proc->Write(da_file);
   }//if

   if (flag1.get(7)) { //bow_proc
      bow_proc->Write(da_file);
   }//if

   if (flag1.get(8)) { //curse_proc
      curse_proc->Write(da_file);
   }//if

   if (flag1.get(6)) { //say_proc
      Cell<say_proc_cell*> cll(topics);
      say_proc_cell* ptr;

      while ((ptr = cll.next())) {
         da_file << "1    here starts a valid say_proc_cell.\n";
         ptr->Write(da_file);
      }//while
      da_file << "-1    end of say_proc_cells\n";
   }//if

   if (flag1.get(5) || flag1.get(6) || flag1.get(7) || flag1.get(8)) {
      parse_for_max_80(wrong_align_msg);
      da_file << wrong_align_msg << "\n~\n";
      parse_for_max_80(wrong_class_msg);
      da_file << wrong_class_msg << "\n~\n";
      parse_for_max_80(wrong_race_msg);
      da_file << wrong_race_msg << "\n~\n";
   }//if

   if (flag1.get(13)) {
      da_file << skill_violence << " " << benevolence << " "
              << defensiveness << " " << bad_assedness << " "
              << social_awareness
              << "  sk_viol, benev, def, bad_assed, soc_aware\n";
   }//if
}//Write


///*********************************************************************///
////*********************** temp_crit_data  ****************************///
///*********************************************************************///


int temp_crit_data::_cnt = 0;

temp_crit_data::temp_crit_data() {
   guarded_by = guarding = shielded_by = shielding = NULL;
   _cnt++;
}//constructor

temp_crit_data::temp_crit_data(const temp_crit_data& source) {
   _cnt++;
   guarded_by = source.guarded_by;
   guarding = source.guarding;
   shielded_by = source.shielding;
   shielding = source.shielding;
}//copy constructor

temp_crit_data::~temp_crit_data() {
   _cnt--;
   Clear();
}//destructor

void temp_crit_data::Clear() {
   if (guarded_by) {
      if (guarded_by->temp_crit && guarded_by->temp_crit->guarding) {
         guarded_by->temp_crit->guarding = NULL;
      }//if
      guarded_by = NULL;
   }//if
   if (guarding) {
      if (guarding->temp_crit && guarding->temp_crit->guarded_by) {
         guarding->temp_crit->guarded_by = NULL;
      }//if
      guarding = NULL;
   }//if
   if (shielded_by) {
      if (shielded_by->temp_crit && 
          shielded_by->temp_crit->shielding) {
         shielded_by->temp_crit->shielding = NULL;
      }//if
      shielded_by = NULL;
   }//if
   if (shielding) {
      if (shielding->temp_crit && 
          shielding->temp_crit->shielded_by) {
         shielding->temp_crit->shielded_by = NULL;
      }//if
      shielding = NULL;
   }//if
}//Clear





///*******************************  mob data **************************///

int mob_data::_cnt = 0;

mob_data::mob_data() {
   _cnt++;
   mob_num = tmp_num = cur_in_game = max_in_game = 0;
   proc_data = NULL;
   cur_script = NULL;
   skin_num = 0;
}//constructor

mob_data::mob_data(mob_data& source) {
   _cnt++;
   proc_data = NULL;
   cur_script = NULL;
   *this = source;
}//constructor

mob_data::~mob_data() {
   _cnt--;
   delete proc_data; //if doesn't exist, then its NULL, still no prob :)
   clear_ptr_list(mob_proc_scripts);
   proc_data = NULL;
   
   cur_script = NULL; //it was held in the mob_proc_scripts
}//destructor
 
void mob_data::Clear() {
   mob_num = tmp_num = cur_in_game = max_in_game = 0;
   delete proc_data;
   proc_data = NULL;
   mob_data_flags.Clear();
   skin_num = 0;
   clear_ptr_list(mob_proc_scripts);
   
   cur_script = NULL; //its held in the mob_proc_scripts

}//Clear, mob_data

void mob_data::finishedMobProc() {
   if (cur_script) {
      cur_script->clean();
      cur_script = NULL;
   }
}//finishedMobProc


void mob_data::addProcScript(const String& txt, MobScript* script_data) {
   //similar to reading it in...
   //first, see if we are over-writing one...
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In mob_data::addProcScript, txt:  \n" << txt
             << "\nscript data:  "
             << script_data->toStringBrief(0, 0, ENTITY_CRITTER)
             << endl;
   }

   mob_data_flags.turn_on(17); //now it has procs for sure!!

   Cell<MobScript*> cll;
   MobScript* ptr;
   mob_proc_scripts.head(cll);

   while ((ptr = cll.next())) {
      if (ptr->matches(*script_data)) {
         //got a match.
         mudlog.log("mob_data::addProcScript, they match.");
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
      mudlog.log(ERR, "script_data is NULL, mob_data::addProcScript.");
      return;
   }

   mob_proc_scripts.append(script_data);
}//addProcScript


mob_data& mob_data::operator= (mob_data& source) {

   if (this == &source)
      return *this;

   Clear();

   mob_num = source.mob_num;
   tmp_num = source.tmp_num;      
   cur_in_game = source.cur_in_game;
   max_in_game = source.max_in_game;
   mob_data_flags = source.mob_data_flags;  //bitfield's overloaded =
   if (source.proc_data) {
      proc_data = new spec_data(*(source.proc_data));
   }//if
   skin_num = source.skin_num;
   
   Cell<MobScript*> cll;
   source.mob_proc_scripts.head(cll);
   MobScript* ptr;
   
   while ((ptr = cll.next())) {
      Put(new MobScript(*ptr), mob_proc_scripts);
   }
   
   return *this;
}//mob_data operator=


void mob_data::Write(ofstream& ofile) {
   
   ofile << " " << mob_num << " " << tmp_num << " " 
         << max_in_game << "\tmob#, tmp_num, max_n_game\n";
   
   mob_data_flags.Write(ofile);
   
   if (mob_data_flags.get(0)) { //does it have spec_data?
      if (!(proc_data)) {
         mudlog.log(ERR, "ERROR:  told to write proc_data, but is NULL!\n");
         return;
      }//if
      proc_data->Write(ofile);
   }//if
   if (mob_data_flags.get(16)) {
      ofile << skin_num << "	skin number\n";
   }//if

   if (mob_data_flags.get(17)) {
      Cell<MobScript*> cll;
      mob_proc_scripts.head(cll);
      MobScript* ptr;

      int i = 1;
      while ((ptr = cll.next())) {
         ofile << i++ <<  "  Start of a mob proc script\n";
         ptr->write(ofile);
      }
      
      ofile << "-1  End of mob proc scripts" << endl;
   }
}//Write()


void mob_data::doScriptJump(int abs_offset) {
   if (cur_script)
      cur_script->doScriptJump(abs_offset);
}


void mob_data::Read(ifstream& ofile, short read_all) {
   char tmp[81];
   
   Clear();
   
   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in mob_data read." << endl;
      }
      return;
   }

   ofile >> mob_num >> tmp_num >> max_in_game;
   ofile.getline(tmp, 80);

   if (mudlog.ofLevel(DBG))
      mudlog << "Mob number:  " << mob_num << endl;
   
   tmp_num = 0; //since it isn't used..should be zero
   
   mob_data_flags.Read(ofile);
   
   // not currently used.
   mob_data_flags.turn_off(6);
   mob_data_flags.turn_off(7);
   mob_data_flags.turn_off(8);
   mob_data_flags.turn_off(9);
   mob_data_flags.turn_off(10);
   mob_data_flags.turn_off(11);
   mob_data_flags.turn_off(12);
   mob_data_flags.turn_off(13);
   mob_data_flags.turn_off(14);
   mob_data_flags.turn_off(15);
   
   if (mob_data_flags.get(0)) { //does it have spec_data?
      proc_data = new spec_data;
      proc_data->Read(ofile, read_all);
   }//if
   
   if (mob_data_flags.get(16)) {
      ofile >> skin_num;
      ofile.getline(tmp, 80);
   }//if
   
   if (mob_data_flags.get(17)) {
      //mudlog.log("Mob has proc scripts...");
      int sent_;
      MobScript* ptr;

      ofile >> sent_;
      ofile.getline(tmp, 80);

      if (mudlog.ofLevel(DB))
         mudlog << "Tmp, after script#: " << sent_ << " -:" << tmp
                << ":-\n";

      while (sent_ != -1) {
         if (mudlog.ofLevel(DB))
            mudlog << "\nReading script# " << sent_ << endl;
         if (!ofile) {
            if (mudlog.ofLevel(ERR)) {
               mudlog << "ERROR:  mob_data reading script da_file FALSE." << endl;
            }
            return;
         }

         ptr = new MobScript();
         ptr->read(ofile);
         Put(ptr, mob_proc_scripts);
         ofile >> sent_;
         ofile.getline(tmp, 80);
         if (mudlog.ofLevel(DB))
            mudlog << "Got rest of line -:" << tmp << ":-" << endl;
      }
   }//if using mob proc scripts
   
}//Read()


int mob_data::getBenevolence() const {
   if (proc_data) 
      return proc_data->getBenevolence();
   return 0;
}

void mob_data::auditAI() {
   if (proc_data) 
      proc_data->auditAI();
}

int mob_data::getBadAssedness() const {
   if (proc_data) 
      return proc_data->getBadAssedness();
   return 0;
}

int mob_data::getSocialAwareness() const {
   if (proc_data) 
      return proc_data->getSocialAwareness();
   return 0;
}

int mob_data::getSkillViolence() const {
   if (proc_data) 
      return proc_data->getSkillViolence();
   return 0;
}

int mob_data::getDefensiveness() const {
   if (proc_data) 
      return proc_data->getDefensiveness();
   return 0;
}

int mob_data::isSentinel() const {
   if (proc_data) 
      return proc_data->isSentinel();
   return FALSE;
}


void mob_data::setBenevolence(int i) {
   if (!proc_data)
      addProcData();
   proc_data->setBenevolence(i);
}

void mob_data::setBadAssedness(int i) {
   if (!proc_data)
      addProcData();
   proc_data->setBadAssedness(i);
}

void mob_data::setSocialAwareness(int i) {
   if (!proc_data)
      addProcData();
   proc_data->setSocialAwareness(i);
}

void mob_data::setSkillViolence(int i) {
   if (!proc_data)
      addProcData();
   proc_data->setSkillViolence(i);
}

void mob_data::setDefensiveness(int i) {
   if (!proc_data)
      addProcData();
   proc_data->setDefensiveness(i);
}


void mob_data::addProcData() {
   if (!proc_data) {
      mob_data_flags.turn_on(0);
      proc_data = new spec_data;
   }//if
}//addProcData


//*************************************************************//
///*********************** pc data ***************************///

int pc_data::_cnt = 0;

pc_data::pc_data() {
   _cnt++;
   post_msg = NULL;
   snoop_by = NULL;
   snooping = NULL;
   imm_data = NULL;
   w_eye_obj = NULL;
   Clear();
}//constructor

pc_data::pc_data(const pc_data& source) {
   _cnt++;
   post_msg = NULL;
   snoop_by = NULL;
   snooping = NULL;
   imm_data = NULL;
   w_eye_obj = NULL;
   *this = source;
}//constructor

pc_data::~pc_data() {
   _cnt--;
   delete imm_data;
   imm_data = NULL;
   if (post_msg) {
      delete post_msg;
      post_msg = NULL;
   }
   snoop_by = NULL;
   snooping = NULL;
   if (w_eye_obj) {
      w_eye_obj->ob->obj_proc->w_eye_owner = NULL;
      w_eye_obj = NULL;
   }//if
}//destructor
 
void pc_data::Clear() {
   delete post_msg;
   post_msg = NULL;
   password.Clear();
   input.Clear();
   output.Clear();
   last_input.Clear();
   host.Clear();
   mode = MODE_NORMAL;
   descriptor = age = index = hunger = thirst = drugged = 0;
   birth_year = birth_day = rent_day = rent_year = pk_count = 
      died_count = quest_points = idle_ticks = 0;
   link_condition = CON_LOGGING_IN;
   delete imm_data;
   imm_data = NULL;
   snoop_by = NULL;
   snooping = NULL;
   pc_data_flags.Clear();
   skills_spells_known.Clear();
   prompt.Clear();
   poofout.Clear();
   poofin.Clear();
   
   gos_str = ANSI_BLACK;
   say_str = ANSI_BLACK;
   yell_str = ANSI_BLACK;
   tell_str = ANSI_BLACK;
   desc_str = ANSI_BLACK;
   obj_list_str = ANSI_BLACK;
   mob_list_str = ANSI_BLACK;
   dflt_str = ANSI_BLACK;
   bk_str = ANSI_BWHITE;
   battle_str = ANSI_BLACK;
   user1_str = ANSI_BLACK;
   user2_str = ANSI_BLACK;
   user3_str = ANSI_BLACK;
   
   last_login_time = 0; //in seconds, since 1970 etc...
   total_time_online = 0; //in seconds
   lines_on_page = 20;
   if (w_eye_obj) {
      w_eye_obj->ob->obj_proc->w_eye_owner = NULL;
      w_eye_obj = NULL;
   }//if
}//Clear, pc_data


pc_data& pc_data::operator=(const pc_data& source) {

   if (this == &source)
      return *this;
   
   Clear();
   
   password = source.password;
   input = source.input;
   output = source.output;
   last_input = source.last_input;
   descriptor = source.descriptor;
   pc_data_flags = source.pc_data_flags;
   age = source.age;
   birth_day = source.birth_day;
   birth_year = source.birth_year;
   rent_day = source.rent_day;
   rent_year = source.rent_year;
   host = source.host;
   link_condition = source.link_condition;
   snoop_by = source.snoop_by;
   snooping = source.snooping;
   imm_data = new immort_data(*(source.imm_data));
   mode = source.mode;
   lines_on_page = source.lines_on_page;
   index = source.index;
   hunger = source.hunger;
   thirst = source.thirst;
   drugged = source.drugged;
   pk_count = source.pk_count;
   died_count = source.died_count;
   quest_points = source.quest_points;
   idle_ticks = source.idle_ticks;
   skills_spells_known = source.skills_spells_known; //overload operator
   prompt = source.prompt;
   w_eye_obj = source.w_eye_obj;
   last_login_time = source.last_login_time;
   total_time_online = source.total_time_online;
   poofin = source.poofin;
   poofout = source.poofout;

   gos_str = source.gos_str;
   say_str = source.say_str;
   yell_str = source.yell_str;
   tell_str = source.tell_str;
   desc_str = source.desc_str;
   obj_list_str = source.obj_list_str;
   mob_list_str = source.mob_list_str;
   dflt_str = source.dflt_str;
   bk_str = source.bk_str;
   battle_str = source.battle_str;
   user1_str = source.user1_str;
   user2_str = source.user2_str;
   user3_str = source.user3_str;

   return *this;
}//operator=, pc_data


void pc_data::Write(ofstream& ofile) {
   int i;
   
   ofile << password << "\tpasswd\n";
   
   pc_data_flags.Write(ofile);
   
   if (pc_data_flags.get(22)) {
      ofile << lines_on_page << " ";
   }

   ofile << birth_day << " " << birth_year 
         << " " << rent_year << " " << rent_day << " "
         << last_login_time << " " << total_time_online
         << "\t\tbday, byr, ryr, rday, llt, tto\n";
   
   ofile << age << " " << hunger << " " << thirst << " " << drugged
         << " " << pk_count << " " << died_count << " " << quest_points
         << " age hgr thr drg pk died qp\n";
   
   int key;
   i = 1;
   int retval = 0;
   if (skills_spells_known.Min(key)) {//if it isn't empty
      ofile << key << " ";
      skills_spells_known.Find(key, retval);
      ofile << retval << " ";
      while (skills_spells_known.Next(key)) {
         ofile << key << " ";
         skills_spells_known.Find(key, retval);
         ofile << retval << " ";
         if (++i > 9) {
            ofile << endl;
            i = 0;
         }//if
      }//while
   }//if
   ofile << -1 << "\tss_known\n";         
   
   ofile << prompt << endl;

   if (pc_data_flags.get(21)) {
      ofile << poofin << endl;
      ofile << poofout << endl;
   }

   if (pc_data_flags.get(2) || pc_data_flags.get(11)) {  //has imm_data
      if (imm_data) {
         imm_data->Write(ofile);
      }//if
      else {
         mudlog.log(ERR, "ERROR:  flagged imm, but no imm_data file!\n");
      }//else
   }//if

   if (pc_data_flags.get(25)) {
      ofile << gos_str << " Gos color" << endl;
      ofile << say_str << " Say color" << endl;
      ofile << yell_str << " Yell color" << endl;
      ofile << tell_str << " Tell color" << endl;
      ofile << desc_str << " Desc color" << endl;
      ofile << obj_list_str << " Obj List color" << endl;
      ofile << mob_list_str << " Mob List Color" << endl;
      ofile << dflt_str << " Default Color" << endl;
      ofile << bk_str << " Background Color" << endl;
      ofile << battle_str << " Battle Color" << endl;
      ofile << user1_str << " User1 color" << endl;
      ofile << user2_str << " User2 Color" << endl;
      ofile << user3_str << " User3 color" << endl;
   }//if

   ofile << "*** end of pc data ***\n";
}//Write()       


void pc_data::Read(ifstream& ofile) {
   int i;
   char tmp[81];
   
   Clear();

   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in pc_data read." << endl;
      }
      return;
   }
   
   ofile >> password;
   ofile.getline(tmp, 80);
   
   pc_data_flags.Read(ofile);

   pc_data_flags.turn_off(23); //don't log in with page break on
   
   if (pc_data_flags.get(22)) {
      ofile >> lines_on_page;
   }

   ofile >> birth_day >> birth_year >> rent_year >> rent_day
         >> last_login_time >> total_time_online;
   ofile.getline(tmp, 80);
   
   ofile >> age >> hunger >> thirst >> drugged >> pk_count >> died_count
         >> quest_points;
   ofile.getline(tmp, 80);
   
   /* skills spells known */
   ofile >> i;
   int tmp_int;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in pc_data read." << endl;
         }
         return;
      }
      ofile >> tmp_int;
      skills_spells_known.Insert(i, tmp_int);
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);
   
   ofile.getline(tmp, 80); //grabs prompt
   prompt = tmp;

   if (pc_data_flags.get(21)) {
      ofile.getline(tmp, 80);
      poofin = tmp;
      ofile.getline(tmp, 80);
      poofout = tmp;
   }
   
   if (pc_data_flags.get(11)) {
      pc_data_flags.turn_on(2);
   }//if
   
   if (pc_data_flags.get(2))	 {
      imm_data = new immort_data;
      imm_data->Read(ofile);
   }//if

   if (pc_data_flags.get(25)) {
      ofile >> gos_str;
      ofile.getline(tmp, 80);
      
      ofile >> say_str;
      ofile.getline(tmp, 80);
      
      ofile >> yell_str;
      ofile.getline(tmp, 80);

      ofile >> tell_str;
      ofile.getline(tmp, 80);
      
      ofile >> desc_str;
      ofile.getline(tmp, 80);
      
      ofile >> obj_list_str;
      ofile.getline(tmp, 80);
      
      ofile >> mob_list_str;
      ofile.getline(tmp, 80);
      
      ofile >> dflt_str;
      ofile.getline(tmp, 80);
      
      ofile >> bk_str;
      ofile.getline(tmp, 80);
      
      ofile >> battle_str;
      ofile.getline(tmp, 80);

      ofile >> user1_str;
      ofile.getline(tmp, 80);
      
      ofile >> user2_str;
      ofile.getline(tmp, 80);
      
      ofile >> user3_str;
      ofile.getline(tmp, 80);
   }//if

   ofile.getline(tmp, 80); //grabs extra line/comment
}//Read()       


//*************************************************************//
///*********************  critter  ***************************///

int critter::_cnt = 0;

critter::critter() {
//   if (mudlog.ofLevel(DBG)) {
//      mudlog << "MEMORY:  creating crit:  " << this << endl;
//   }
   _cnt++;
   pc = NULL;
   mob = NULL;
   possessed_by = NULL;
   possessing = NULL;
   temp_crit = NULL;
   Clear();
} // crit constructor




critter::critter(critter& source) {
//   if (mudlog.ofLevel(DBG)) {
//      mudlog << "MEMORY:  copy constructing crit:  " << this << endl;
//   }
   _cnt++;
   pc = NULL;
   mob = NULL;
   possessed_by = NULL;
   possessing = NULL;
   temp_crit = NULL;
   *this = source;  //utilize overloaded =
} // crit constructor


critter::~critter() {
//   if (mudlog.ofLevel(DBG)) {
//      mudlog << "MEMORY:  deleting critter:  " << this << endl;
//   }
   _cnt--;

   if (isMob() && (!do_shutdown)) {
      mudlog.log(ERR, "ERROR:  trying to delete a MOB before shutdown.\n");
      exit(100);
   }//if

   if (! do_shutdown) {

      /* Eventually, could take these checks out. --BEN */

      if (affected_mobs.haveData(this)) {
         mudlog << "ERROR:  deleting critter, but it's in affected_mobs: "
                << this << "  mob num:  " << *(names.peekFront()) << endl;
      }//if
      if (affected_mobs.haveData(this)) {
         mudlog << "ERROR:  deleting critter, but it's in affected_mobs: "
                << this << "  mob num:  " << *(names.peekFront()) << endl;
      }//if
      if (linkdead_list.haveData(this)) {
         mudlog << "ERROR:  deleting critter, but it's in linkdead_list: "
                << this << "  mob num:  " << *(names.peekFront()) << endl;
      }//if
      if (pc_list.haveData(this)) {
         mudlog << "ERROR:  deleting critter, but it's in pc_list: "
                << this << "  mob num:  " << *(names.peekFront()) << endl;
      }//if
      if (pulsed_proc_mobs.haveData(this)) {
         mudlog << "ERROR:  deleting critter, but it's in pulsed_proc_mobs: "
                << this << "  mob num:  " << *(names.peekFront()) << endl;
      }//if
      if (proc_action_mobs.haveData(this)) {
         mudlog << "ERROR:  deleting critter, but it's in proc_action_mobs: "
                << this << "  mob num:  " << *(names.peekFront()) << endl;
      }//if
      if (room_list[IN_ROOM].haveCritter(this)) {
         mudlog << "ERROR:  deleting critter, but it's in ROOM::CRITTERS: "
                << this << "  mob num:  " << *(names.peekFront())
                << "  room num:  " << IN_ROOM << endl;
      }//if

   }//if

   Clear();
}//destructor


critter& critter::operator=(critter& source) { //automagically makes SMOB

   if (this == &source)
      return *this;

   Cell<stat_spell_cell*> cell;
   stat_spell_cell *tmp_stat, *tmp_stat2;
   Cell<object*> cll;
   object* obj_ptr;
   int i;

      //mudlog.log(DBG, "In crit operator= overload.\n");

   Clear();		

   Cell<String*> scll(source.names);
   String* sptr;
   while ((sptr = scll.next())) {
      Put((new String(*sptr)), names);
   }//while

   short_desc = source.short_desc;
   in_room_desc = source.in_room_desc;
   long_desc = source.long_desc;

   if (source.pc) {
      pc = new pc_data(*(source.pc));
   }//if
   if (source.mob) {
      mob = new mob_data(*(source.mob));
   }//if

   crit_flags = source.crit_flags;  //bitfields

   for (i = 0; i<MOB_LONG_DATA; i++) 
      long_data[i] = source.long_data[i];
   for (i = 0; i<MOB_SHORT_CUR_STATS; i++) 
      short_cur_stats[i] = source.short_cur_stats[i];
   for (i = 0; i<MOB_CUR_STATS; i++) 
      cur_stats[i] = source.cur_stats[i];

   for (i = 0; i<MAX_EQ; i++) {
      if (source.eq[i]) {
         if (!(source.eq[i]->IN_LIST)) { //don't need multiple ptrs to SOBJ's
	   eq[i] = source.eq[i];
         }//if
	 else {
	   eq[i] = new object(*(source.eq[i]));
	   if (eq[i]) // if had the memory
	     eq[i]->IN_LIST = &(this->inv);
	 }//else
      }//if
      else {
         eq[i] = NULL;
      }//else
   }//for

   source.affected_by.head(cell);
   while ((tmp_stat = cell.next())) {
      tmp_stat2 = new stat_spell_cell;
      *tmp_stat2 = *tmp_stat; //shallow copy should work 
      affected_by.append(tmp_stat2);
   }//while

   source.mini_affected_by.head(cell);
   while ((tmp_stat = cell.next())) {
      tmp_stat2 = new stat_spell_cell;
      *tmp_stat2 = *tmp_stat; //shallow copy should work 
      Put(tmp_stat2, mini_affected_by);
   }//while
   
   source.inv.head(cll);
   while ((obj_ptr = cll.next())) {
      if (!obj_ptr->IN_LIST) { //no multiple ptrs to SOBJ's
         Put(obj_ptr, inv);
      }//if
   }//while

   follower_of = source.follower_of;

   if (source.temp_crit) {
     temp_crit = new temp_crit_data(*(source.temp_crit));
   }//if
   else {
     temp_crit = NULL;
   }//else

   master = source.master;
   //pets = source.pets;              //these can cause multiple ptrs
   //followers = source.followers;    //to SMOB's, a definate SEGV
   //groupees = source.groupees;
   //is_fighting = source.is_fighting;
   if (source.short_cur_stats[26] == 0) 
      short_cur_stats[26] = 0; //Stays a pc
   else
      short_cur_stats[26] = 1; //it IS a SMOB now!
   mirrors = source.mirrors;
   possessing = source.possessing;
   possessed_by = source.possessed_by;

   return *this;
}//crit::operator= overload


void critter::Clear() {
   int i;
   critter* ptr;

   //mudlog.log(DBG, "In crit Clear().\n");
   clear_ptr_list(names);
   short_desc.Clear();
   in_room_desc.Clear();
   long_desc.Clear();

   crit_flags.Clear();

   delete pc;
   delete mob;
   pc = NULL;
   mob = NULL;

   for (i = 0; i<MOB_LONG_DATA; i++) 
      long_data[i] = 0;
   for (i = 0; i<MOB_SHORT_CUR_STATS; i++) 
      short_cur_stats[i] = 0;
   for (i = 0; i<MOB_CUR_STATS; i++) 
      cur_stats[i] = 0;
   for (i = 0; i<MAX_EQ; i++) {
      eq[i] = NULL;
   }//for

   short_cur_stats[26] = 2; // slight hack, default it to MOB

   clear_ptr_list(affected_by);
   clear_ptr_list(mini_affected_by);
   clear_obj_list(inv);
   follower_of = master = NULL;

   delete temp_crit;
   temp_crit = NULL;

	/* just get rid of ptrs, clean up SMOBs before clear is called */
   pets.clear();
   groupees.clear();

   /* This should always be empty, I would think. */
   while (!followers.isEmpty()) {
      ptr = followers.popFront();

      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  followers not empty in Clear, for mob: "
                << getName() << " follower: " << ptr->getName() << endl;
      }//if

      ptr->FOLLOWER_OF = NULL;
   }//while

   is_fighting.clear();
   mirrors = 0;

   if (possessing) {
      unPossess();
   }

   if (possessed_by) {
      possessed_by->unPossess();
   }

   //mudlog.log(DBG, "Done w/clear.\n");
}//crit Clear

int critter::getNakedWeight() const {
   if (short_cur_stats[41] == 0) {
      return 150;
   }
   return short_cur_stats[41];
}//getNakedWeight


int critter::cur_weight() {// of inv...eq not taken into account here
   Cell<object*> cll(inv);
   object* tmp_obj;
   int retval = 0;
   while ((tmp_obj = cll.next())) {
      retval += tmp_obj->ob->cur_weight();
   }//while

   for (int i = 1; i<MAX_EQ; i++) {
      if (EQ[i]) {
         retval += EQ[i]->ob->cur_weight() / 2;
      }//if
   }//for

   retval += GOLD / 10000; //gold weighs a little..though not much

   return retval + getNakedWeight();
} // cur_weight

int critter::max_weight() {
   return (short_cur_stats[1] * 10 + getNakedWeight());  // 10 times their str
}//max_weight

int critter::getCurRoomNum() {
   return IN_ROOM;
}

void critter::checkForBattle(room& rm) {

   List<critter*> tmp_crits(rm.getCrits()); //just do shallow copy
   Cell<critter*> cll(tmp_crits);
   String buf(50);

   critter* ptr;
   Sprintf(buf, "hit %S\n", getShortName());
   String buf2(50);
   while ((ptr = cll.next())) {
      if (!rm.haveCritter(this)) {
         return;
      }
      if (rm.haveCritter(ptr)) {
         if (ptr->doesRemember(*this) && ptr->canDetect(*this) &&
             (d(1, DEX) < (d(1, ptr->DEX * 2 + ptr->INT)))) {
            say("There you are!!", *ptr, *(ptr->getCurRoom()));
            ptr->processInput(buf, FALSE);
         }//if
      }//if

      if (!rm.haveCritter(this)) {
         return;
      }
      if (rm.haveCritter(ptr)) { //make sure we're still there
         if (doesRemember(*ptr) && canDetect(*ptr) &&
             (d(1, ptr->DEX) < (d(1, DEX * 2 + INT)))) {
            say("I've found you now!!", *this, *(getCurRoom()));
            Sprintf(buf2, "hit %S\n", ptr->getShortName());
            processInput(buf2, FALSE);
         }//if
      }//if
   }//while
}//checkForBattle

int critter::canDetect(const critter& other) const {
   return detect(getSeeBit(), other.getVisBit());
}


void critter::Write(ofstream& ofile) {
   int i, num_written;
   Cell<stat_spell_cell*> ss_cell;
   stat_spell_cell* ss_ptr;
   Cell<object*> ob_cell;
   object* ob_ptr;
   Cell<String*> st_cell(names);
   String* st_ptr;
   String tmp_str(100);

   int len = 0;
   while ((st_ptr = st_cell.next())) {
      len += st_ptr->Strlen();
      if (len > 79) {
	 ofile << endl;
	 len = 0;
      }//if
      ofile << *st_ptr << " ";
   }//while
   ofile << "~" << "\tnames";

   if (pc) {
      ofile << " " << pc->host;
   }

   ofile << "\n";

   parse_for_max_80(short_desc);
   ofile << (short_desc) << endl << "~" << endl;
   parse_for_max_80(in_room_desc);
   ofile << (in_room_desc) << endl << "~" << endl;
   parse_for_max_80(long_desc);
   ofile << (long_desc) << endl << "~" << endl;

   crit_flags.Write(ofile);

   for (i = 0; i<MOB_LONG_DATA; i++)
      ofile << long_data[i] << " ";
   ofile << "\tlong_data\n";

   for (i = 0; i<MOB_SHORT_CUR_STATS; i++) {
      if ((i + 1) % 20 == 0)
         ofile << endl;
      ofile << short_cur_stats[i] << " ";
   }//for
   ofile << "\tshrt_cur_stats\n";

   for (i = 0; i<MOB_CUR_STATS; i++)
      ofile << cur_stats[i] << " ";
   ofile << "\tcur_stats\n";

         	/* EQ, write out item_num, posn */
   num_written = 0;
   for (i = 0; i < MAX_EQ; i++) {
      if (eq[i]) {
         if (!eq[i]->IN_LIST) {
            ofile << eq[i]->OBJ_NUM << " " << i << " ";
            if ((++num_written % 20) == 0)
               ofile << endl;
         }//if
         else { //then its a SOBJ
            ofile << -2 << " " << i << "\t Start of SOBJ\n";
            eq[i]->Write(ofile);
         }//else
      }//if
   }//for
   ofile << -1 << "\teq: num, posn\n";

		/*  Affected By */
   num_written = 0;
   affected_by.head(ss_cell);
   while ((ss_ptr = ss_cell.next())) {
      ofile << ss_ptr->stat_spell << " " << ss_ptr->bonus_duration << " ";
      if ((++num_written % 20) == 0)
         ofile << endl;
   }//while
   ofile << -1 << "\taffected_by\n";

		/*  Inventory */
   
   num_written = 0;
   inv.head(ob_cell);
   while ((ob_ptr = ob_cell.next())) {
      if (ob_ptr->IN_LIST) {
         ofile << -2 << "\t Start of SOBJ\n";
         ob_ptr->Write(ofile);
      }//if
      else {
         ofile << ob_ptr->OBJ_NUM << " ";
         if ((++num_written % 20) == 0)
            ofile << endl;
      }//else
   }//while
   ofile << -1 << "\tinv\n";

      		/*  Do PC, MOB  */

   if (short_cur_stats[26] == 0) { // If its a pc
      if (!pc) {
         mudlog.log(ERR, "ERROR: trying to write out pc_data when pc is NULL.\n");
         return;
      }//if
      pc->Write(ofile);  //write it out
   }//if
   else { //its a mob/smob
      if (!mob) {
         mudlog.log(ERR, "ERROR: trying to write out mob_data when mob is NULL.\n");
         return;
      }//if
      mob->Write(ofile); //write it out
   }//else
   ofile << "\tend_of_crit\n";  //done
}//Write....crit


void critter::Read(ifstream& ofile, short read_all) {
   int i, test = TRUE, z;
   stat_spell_cell* ss_ptr;
   char tmp[81];
   String tmp_str(80);
   String* string;

   Clear();

   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in crit read." << endl;
      }
      return;
   }

   test = TRUE;
   while (test) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in crit read." << endl;
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
   in_room_desc.Termed_Read(ofile);
   long_desc.Termed_Read(ofile);

   crit_flags.Read(ofile);
   crit_flags.turn_on(18); //always in use
   crit_flags.turn_off(25); // Never been hurled before.

   setComplete(); //if we can read it, it's complete enough!

   for (i = 0; i<MOB_LONG_DATA; i++)
      ofile >> long_data[i];
   ofile.getline(tmp, 80);

   for (i = 0; i<MOB_SHORT_CUR_STATS; i++) {
      ofile >> short_cur_stats[i];
   }
   ofile.getline(tmp, 80);

   for (i = 0; i<MOB_CUR_STATS; i++) {
      ofile >> cur_stats[i];
   }
   ofile.getline(tmp, 80);

         	/* EQ, read in item_num, posn */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in crit read." << endl;
         }
         return;
      }

      ofile >> z;
      if (!check_l_range(z, 1, MAX_EQ, mob_list[0], FALSE)) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  z out of range, crit.Read():  " << z
                   << "short desc:  " << short_desc << endl;
         }
         ofile >> i;
         continue;
      }//if
      if (i == -2) { //gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->Read(ofile, TRUE);
         new_obj->IN_LIST = &(inv); //make sure its a SOBJ

         if (!obj_list[new_obj->OBJ_NUM].ob) { //if it's OBJ doesn't exist, junk it
            delete new_obj;
         }
         else {
            eq[z] = new_obj;      //now it wears it
            affected_objects.append(new_obj);
         }
      }//if
      else {
         if (obj_list[i].isInUse()) {  //exists?
            if (read_all || 
                     ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 
			100) > d(1,100)) {
               eq[z] = &(obj_list[i]);      //now it wears it
            }//if
         }//
         else {
            Sprintf(tmp_str, 
             "ERROR:  trying to load non-existant obj: %i, 
              in critter's: %S  eq.\n", i, &short_desc);
            mudlog.log(ERR, tmp_str);
         }//else
      }//else
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);

		/*  Affected By */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in crit read." << endl;
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

		/*  Inventory */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in crit read." << endl;
         }
         return;
      }

      if (i == -2) { //gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->Read(ofile, TRUE);
         new_obj->IN_LIST = &(inv); //make sure its a SOBJ

         if (!obj_list[new_obj->OBJ_NUM].ob) { //if it's OBJ doesn't exist, junk it
            delete new_obj;
         }
         else {
            Put(new_obj, inv);    //add it to inventory
            affected_objects.append(new_obj);
         }

      }//if
      else {
         if (obj_list[i].OBJ_FLAGS.get(10)) {
            if (read_all ||
              ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 100) > 
                      d(1,100)) {
               Put(&(obj_list[i]), inv);    //add it to inventory
            }//if
         }//if
         else {
            Sprintf(tmp_str, 
             "ERROR: trying to load non-existant object: %i, 
             in critter's:  %S, inv.\n", i, &short_desc);
            mudlog.log(ERR, tmp_str);
         }//else
      }//else
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);   

      		/*  Do PC, MOB  */

   if (short_cur_stats[26] == 0) { // If its a pc
      if (!(pc)) {
         pc = new pc_data;
      }//if
      pc->Read(ofile);  
   }//if
   else { //its a mob
      if (!(mob)) {
         mob = new mob_data;
      }//if
      mob->Read(ofile, read_all); 
   }//else
   ofile.getline(tmp, 80);      
   //mudlog.log(DBG, "Done w/read crit.\n");

   if (!is_affected_by(BLINDNESS_SKILL_NUM, *this))
      SEE_BIT |= 1024; //another bletcherous kludge

}//Read....crit

void critter::notifyHasBeenHurled() {
   if ((getLevel() > 10) || (INT > 14)) { //experienced and smart folks remember
      CRIT_FLAGS.turn_on(25);
   }
}

int critter::canBeHurled() {
   return !(CRIT_FLAGS.get(25));
}

int critter::haveObjNumbered(int cnt, int obj_num) {
   Cell<object*> cll(inv);
   object* ptr;
   int count = 0;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "haveObjNumbered: cnt: " << cnt << " obj_num: " 
             << obj_num << endl;
   }

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

   for (int i = 0; i<MAX_EQ; i++) {
      if ((ptr = eq[i])) {
         count += ptr->getObjCountByNumber(obj_num, 0);
         if (ptr->OBJ_NUM == obj_num) {
            count++;
         }//if obj nums agree
         
         if (count >= cnt) {
            return TRUE;
         }//if
      }//if
   }//for

   return FALSE;
}//haveObjectNumbered


void critter::setIdNum(int i) {
   if (mob)
      mob->mob_num = i;
}

void critter::setNoClient() {
   if (pc)
      PC_FLAGS.turn_off(5);
}

void critter::setImmLevel(int i) {
   if (pc && pc->imm_data) {
      if (i < 0)
         i = 0;
      if (i > 10)
         i = 10;
      
      pc->imm_data->imm_level = i;
   }//if
}
    
int critter::getIdNum() const {
   if (mob) {
      return mob->mob_num;
   }
   return 0;
}


// NOTE:  The script owner is *this.  It is likely, but not necessary
// that targ is also *this
void critter::checkForProc(String& cmd, String& arg1, critter& actor,
                           int targ, room& rm) {
   mudlog.log("In critter::checkForProc.");

   if (isMob()) {
      mudlog.log(ERR, "ERROR:  critter::checkForProc, got a MOB.");
      return;
   }

   if (mob) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "Was a mob, mob#:  " << MOB_NUM << endl;
      }

      Cell<MobScript*> cll;
      MobScript* ptr;
         
      mob->mob_proc_scripts.head(cll);

      while ((ptr = cll.next())) {
         //mudlog.log("In while.");
         //mudlog.log(ptr->toStringBrief(0, 0));
         if (ptr->matches(cmd, arg1, actor, targ)) {
            //mudlog.log("Matches..");
            if (!mob->isInProcNow() || ptr->takesPrecedence()) {
               //mudlog.log("Not in a proc now.");
               // Generate a script, talored to the actors....
               if (mob->cur_script) {
                  mob->cur_script->clean();
               }//if

               ptr->generateScript(cmd, arg1, actor, targ, rm, this);
               mob->cur_script = ptr;

               // Ok then, lets start the script
               proc_action_mobs.gainData(this);
               mob->cur_script->resetStackPtr(); //get ready to start
               return;
            }//if
         }//if
      }//while
   }//if
}//checkForProcAction


void critter::trackToKill(critter& vict, int& is_dead) {
   if ((isMob()) || (vict.isMob())) {
      mudlog.log(ERR, "ERROR:  MOB's in trackToKill.\n");
      return;
   }//f

   if (!mob) {
      mudlog.log(ERR, "ERROR:  pc trying to track_to_kill.\n");
      return;
   }//if

   remember(vict);

   if (isSentinel())
      return;

   setTrackingTarget(*(vict.getShortName()));
   
   doHuntProc(5, is_dead);
}//track_to_kill


void critter::setTrackingTarget(const String& targ_name) {
   if (!mob)
      return;

   if (!mob->proc_data)
      mob->proc_data = new spec_data();

   if (!mob->proc_data->temp_proc)
      mob->proc_data->temp_proc = new temp_proc_data();

   TRACKING = targ_name;
}


int critter::getBenevolence() const {
   if (isPc() || !mob) {
      return 0;
   }
   else {
      return mob->getBenevolence();
   }
}//getAggressiveness


int critter::isSentinel() const {
   if (isPc() || !mob) {
      return FALSE;
   }
   else {
      return mob->isSentinel();
   }
}//isSentinel


void critter::doRemoveFromBattle() {
   if (!IS_FIGHTING.isEmpty()) {
      Cell<critter*> cll(IS_FIGHTING);
      critter* crit_ptr;
      while ((crit_ptr = cll.next())) {
         crit_ptr->IS_FIGHTING.loseData(this);
      }//while
      IS_FIGHTING.clear();
   }//if
}//doRemoveFromBattle

void critter::doGoToRoom(int dest_room, const char* from_dir, door* by_door,
                        int& is_dead, int cur_room) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In doGoToRoom, dest_room:  " << dest_room
             << "  cur_room:  " << cur_room
             << "  by_door:  " << by_door << endl;
   }

   doRemoveFromBattle();

   leave_room_effects(room_list[cur_room], *this);

   // If we are in earth-meld, break it.
   breakEarthMeld();

   room_list[cur_room].removeCritter(this);
   room_list[dest_room].gainCritter(this);

   // This can cause us to delete ourselves btw...not very good coding
   // but..maybe it will work!
   do_entered_room_procs(*this, by_door, from_dir, room_list[dest_room],
                         is_dead);
   if (!is_dead) {
      look(1, &NULL_STRING, *this);
   }
}


void critter::breakEarthMeld() {
   stat_spell_cell* sp;
   if ((sp = is_affected_by(EARTHMELD_SKILL_NUM, *this))) {
      show("Your meld with the earth has been severed.\n");
      affected_by.loseData(sp);
      delete sp;
   }//if
}//breakEarthMeld


int critter::isNamed(const String& name) const {
   Cell<String*> char_cell(names);
   String *string;

   if (name.Strlen() == 0)
      return FALSE;

   while ((string = char_cell.next())) {
      if (strcasecmp(*string, name) == 0) {
	 return TRUE;
      }//if
   }//while
   return FALSE;
}//mob_is_named 

object* critter::loseInv(object* obj) {
   return inv.loseData(obj);
}

void critter::loseObjectFromGame(object* obj) {
   // first, check EQ
   if (mudlog.ofLevel(DBG)) {
      mudlog << "loseObjectFromGame, obj num:  " << obj->getIdNum()
             << "  pc's name:  " << getName() << endl;
   }

   for (int i =1; i<MAX_EQ; i++) {
      if (EQ[i] && (EQ[i]->getIdNum() == obj->getIdNum())) {
         remove_eq_effects(*obj, *this, FALSE, FALSE, i);
         if (EQ[i]->IN_LIST) {
            delete EQ[i];
         }
         EQ[i] = NULL;
      }//if
   }//for

   Cell<object*> cll(inv);
   object* ptr;
   ptr = cll.next();
   while (ptr) {
      if (ptr->getIdNum() == obj->getIdNum()) {
         if (ptr->IN_LIST) {
            delete ptr;
         }
         ptr = inv.lose(cll);
      }//if
      else {
         ptr = cll.next();
      }
   }//while
}//loseObjectFromGame


void critter::gainInv(object* obj) {
   inv.prepend(obj);
   if (obj->IN_LIST)
      obj->IN_LIST = &inv;
}

void critter::setHP(int i) {
   if (i < 0)
      i = 0;
   if (i > 32000)
      i = 32000;
   HP = i;
}

void critter::show(const char* msg) {
   ::show(msg, *this);
}

void critter::setHP_MAX(int i) {
   if (i < 0)
      i = 0;
   if (i > 32000)
      i = 32000;
   short_cur_stats[23] = i; //MAX_HP of course...
}

// set in_room to zero
void critter::doLeaveRoom() {
   room_list[IN_ROOM].removeCritter(this);
   IN_ROOM = 0;
}//doLeaveRoom


room* critter::getCurRoom() {
   return &(room_list[IN_ROOM]);
}

   
void critter::doSuicide() {
   if (!isPc())
      return;
   String buf(100);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In doSuicide, pc:  " << *(getName()) << endl;
   }

   buf = *(Top(names));
   buf.Tolower();
   buf.Prepend("rm ./Pfiles/");
   system(buf);
   
   Sprintf(buf, "leaves this world forever by %s own hand!\n", 
           get_his_her(*this));
   ::emote(buf, *this, *(getCurRoom()), TRUE);

   setMode(MODE_QUIT_ME_PLEASE);
}//doSuicide


void critter::setMode(PcMode val) {
   mudlog << "Setting mode to:  " << val << endl;
   pc->mode = val;
}


void critter::emote(const char* msg) {
   ::emote(msg, *this, room_list[getCurRoomNum()], TRUE);
}


void critter::doPrompt() {
   int i;
   String targ(200);
   int source_len = PROMPT_STRING.Strlen();

   for (i = 0; i < source_len; ) {
      if (PROMPT_STRING[i] == '%') {
         i++;
         if (PROMPT_STRING[i] != '%') {
            switch (PROMPT_STRING[i]) {
               case 'N':    /* newline */  
                  targ.Append("\n");
                  break;
               case 'h':    /* cur hp */  
                  targ.Append(HP);
                  break;
               case 'H':     /* max hp */
                  targ.Append(HP_MAX);
                  break;
               case 'v':    /* cur mov */  
                  targ.Append(MOV);
                  break;
               case 'V':     /* max mov */
                  targ.Append(MV_MAX);
                  break;
               case 'm':    /* cur mana */  
                  targ.Append(MANA);
                  break;
               case 'M':     /* max mana */
                  targ.Append(MA_MAX);
                  break;
               default:
                  targ.Append(PROMPT_STRING[i]);
            }//switch
            i++;
         }//if
         else { //
            i++;
            targ.Append("%");
         }//else
      }//if
      else {
         targ.Append(PROMPT_STRING[i]);
         i++;
      }//else
   }//for
   targ.Append(pc->input);
   show(targ);
}//doPrompt

critter* critter::getFirstFighting() {
   return IS_FIGHTING.peekFront();
}


int critter::getCurZoneNum() {
   return getCurRoom()->getZoneNum();
}

int critter::shouldSeeInventory() {
   return ((pc && PC_FLAGS.get(19)) || 
           (possessed_by && possessed_by->pc && possessed_by->PC_FLAGS.get(19)));
}

int critter::shouldShowVnums() {
   return ((pc && pc->imm_data && !PC_FLAGS.get(20)) || 
           (possessed_by && possessed_by->pc
            && possessed_by->pc->imm_data && !possessed_by->PC_FLAGS.get(20)));
}

int critter::shouldDoAutoExits() {
   return ((pc && PC_FLAGS.get(6)) || 
           (possessed_by && possessed_by->pc && possessed_by->PC_FLAGS.get(6)));
}



int critter::getImmLevel() {
   if (pc && pc->imm_data)
      return pc->imm_data->imm_level;

   return 0;
}

void critter::setCurRoomNum(int i) {
   if (i < 0)
      i = 0;
   if (i >= NUMBER_OF_ROOMS)
      i = 0;

   IN_ROOM = i;
}//setCurRoomNum

void critter::addProcScript(const String& txt, MobScript* script_data) {
   if (mob)
      mob->addProcScript(txt, script_data);
}//addProcScript


/** If return is > 0, self is more powerful, if less than 0,
 * b is more powerful...  The larger/smaller the value, the more
 * extreme the difference.
 */
int critter::compareTo(critter& b) {  //is a weaker than b
   int str_a = 0;
   int str_b = 0;
  
   str_a += HP * 3 + MANA + DAM * 15 + HIT * 5 + LEVEL * 4 + STR;
   str_b += b.HP * 3 + b.MANA + b.DAM * 15 + b.HIT * 5 + b.LEVEL * 4 + b.STR;

   if (pc)
      str_a += 20;
   if (b.pc)
      str_b += 20;
   
   str_a *= ATTACKS;
   str_b *= b.ATTACKS;

   return (str_a - str_b);
}//compareTo

String* critter::getTrackingTarget() {
   if (mob && mob->proc_data && mob->proc_data->temp_proc)
      return &(TRACKING);
   return &NULL_STRING;
}


void critter::unPossess() {
   if (possessing) {
      possessing->possessed_by = NULL;
      possessing = NULL;
   }
}//unpossess

int critter::isUsingClient() {
   return ((pc && pc->pc_data_flags.get(5)) || 
           (possessed_by && possessed_by->pc->pc_data_flags.get(5)));
}//isUsingClient

int critter::isBrief() {
   return ((pc && pc->pc_data_flags.get(8)) ||
           (possessed_by && possessed_by->pc->pc_data_flags.get(8)));
}//isBrief

int critter::isGagged() {
   return (pc && PC_FLAGS.get(1));
}

int critter::isFrozen() {
   return (pc && PC_FLAGS.get(0));
}

int critter::isWizchat() const {
   return (pc && PC_FLAGS.get(24));
}

int critter::isImmort() {
   return (pc && pc->imm_data);
}

int critter::isFlying() {
   return (crit_flags.get(3));
}

int critter::isParalyzed() {
   return (crit_flags.get(14));
}


int critter::canClimb() {
   return (crit_flags.get(5) ||
           (d(1, 100) < d(1, 
                          get_percent_lrnd(CLIMBING_SKILL_NUM, *this)
                          * 4 + 10)));
}//canClimb


void critter::drunkifyMsg(String& msg) {
   if (pc && (pc->drugged > 0)) {
      for (int i = 0; i<msg.Strlen(); i++) {
         if (d(1, 15) < d(1, pc->drugged)) {
            msg.setCharAt(i, (char)(d(1, 52) + 41));
         }//if
      }//for
   }//if
}//drunkifyMsg


int critter::shouldDoPoofin() {
   return (pc && PC_FLAGS.get(21));
}

int critter::shouldDoPoofout() {
   return (pc && PC_FLAGS.get(21));
}

String& critter::getPoofin() {
   if (pc)
      return pc->poofin;
   return NULL_STRING;
}

String& critter::getPoofout() {
   if (pc)
      return pc->poofout;
   return NULL_STRING;
}


int critter::isInUse() const {
   return CRIT_FLAGS.get(18);
}

void critter::setComplete() {
   CRIT_FLAGS.turn_off(24);
}

void critter::setNotComplete() {
   CRIT_FLAGS.turn_on(24);
}

int critter::isNotComplete() const {
   return CRIT_FLAGS.get(24);
}

void critter::setPoofin(const char* pin) {
   if (pc) {
      pc->poofin = pin;
      pc->poofin.Strip();
      PC_FLAGS.turn_on(21);
   }
}

void critter::setPoofout(const char* pout) {
   if (pc) {
      pc->poofout = pout;
      pc->poofout.Strip();
      PC_FLAGS.turn_on(21);
   }
}

int critter::doesOwnRoom(room& rm) {
   return (ZoneCollection::instance().elementAt(rm.getZoneNum()).isOwnedBy(*this));
}


int critter::doesOwnObject(object& obj) {
   return (ZoneCollection::instance().elementAt(obj.getZoneNum()).isOwnedBy(*this));
}

int critter::doesOwnCritter(critter& mobile) {
   return (mobile.isNpc() &&
           ZoneCollection::instance().elementAt(mobile.getNativeZoneNum()).isOwnedBy(*this));
}

int critter::doesOwnDoor(door& dr) {
   return (dr.dr_data && doesOwnDoor(*dr.dr_data));
}

int critter::doesOwnDoor(door_data& dd) {
   return (ZoneCollection::instance().elementAt(dd.getZoneNum()).isOwnedBy(*this));
}

const String* critter::getName(int see_bit) const {
   if (!detect(see_bit, VIS_BIT)) {
      return &SOMEONE;  //global string
   }
   if (isPc()) { //is a pc
      const String* tmp = names.peekFront();
      if (tmp) {
         return tmp;
      }
      else {
         return &SOMEONE;
      }
   }//if
   else {
      return &(short_desc);
   }//else
}//name_of_crit

const String* critter::getShortName(int see_bit) const {
   if (!detect(see_bit, VIS_BIT)) {
      return &SOMEONE;  //global string
   }

   const String* tmp = names.peekFront();
   if (tmp) {
      return tmp;
   }
   else {
      return &SOMEONE;
   }
}//getShortName


// return current value of cur_in_game after operation
int critter::getCurInGame() {
   if (mob && mob_list[getIdNum()].mob) {
      return mob_list[getIdNum()].mob->getCurInGame();
   }

   mudlog << "ERROR: could not get curInGame for critter: "
          << *(getName()) << "  id: " << getIdNum() << endl;
   return 0;
}


// return current value of cur_in_game after operation
int critter::getMaxInGame() {
   if (mob && mob_list[getIdNum()].mob) {
      return mob_list[getIdNum()].mob->getMaxInGame();
   }

   mudlog << "ERROR: could not get maxInGame for critter: "
          << *(getName()) << "  id: " << getIdNum() << endl;
   return 0;
}

int critter::setCurInGame(int i) {
   if (mob && mob_list[getIdNum()].mob) {
      return mob_list[getIdNum()].mob->setCurInGame(i);
   }

   mudlog << "ERROR: could not set curInGame for critter: "
          << *(getName()) << "  id: " << getIdNum() << endl;
   return 0;
}

int critter::setMaxInGame(int i) {
   if (mob && mob_list[getIdNum()].mob) {
      return mob_list[getIdNum()].mob->setMaxInGame(i);
   }

   mudlog << "ERROR: could not set maxInGame for critter: "
          << *(getName()) << "  id: " << getIdNum() << endl;
   return 0;
}


int critter::decrementCurInGame() {
   if (mob && mob_list[getIdNum()].mob) {
      return mob_list[getIdNum()].mob->decrementCurInGame();
   }

   mudlog << "ERROR: could not decrementCurInGame for critter: "
          << *(getName()) << "  id: " << getIdNum() << endl;
   return 0;
}

int critter::incrementCurInGame() {
   if (mob && mob_list[getIdNum()].mob) {

      return mob_list[getIdNum()].mob->incrementCurInGame();
   }

   mudlog << "ERROR: could not incrementCurInGame for critter: "
          << *(getName()) << "  id: " << getIdNum() << endl;
   return 0;
}

int critter::isNoHassle() {
   return (pc && PC_FLAGS.get(7));
}

int critter::isSneaking() {
   return crit_flags.get(17);
}

int critter::isHiding() {
   return crit_flags.get(22);
}

int critter::hasAI() {
   return (mob && mob->proc_data && FLAG1.get(13));
}

int critter::getLinesOnPage() {
   if (pc) {
      return pc->lines_on_page;
   }
   return 40; //default, probably never used
}

void critter::setLinesOnPage(int lines) {
   if (pc) {
      pc->lines_on_page = lines;
   }
}


/** Assume we are removing this object at this posn. */
void critter::checkLight(object* obj, int posn) {
   //check all EQ for a burning light...
   crit_flags.turn_off(1); //not using light
   for (int i = 1; i<MAX_EQ; i++) {
      if (i == posn && (EQ[i] == obj)) {
         continue;
      }
      else {
         if (EQ[i] && EQ[i]->isLit()) {
            crit_flags.turn_on(1);
            return;
         }//if
      }
   }//for
}//checkLight

object* critter::findObjInEq(int i_th, const String& name,
                             int see_bit, room& rm, int& posn) {
   int count = 0;
   for (int i = 1; i<MAX_EQ; i++) {
      if (EQ[i] && 
          detect(see_bit, rm.getVisBit() | EQ[i]->OBJ_VIS_BIT) &&
         obj_is_named(*(EQ[i]), name)) {
         count++;
         if (count == i_th) {
            posn = i;
            return EQ[i];
         }//if
      }//if
   }//for
   
   posn = 0;
   return NULL;
}//findObjInEq


String* critter::getHostName() const {
   if (pc) {
      return &(pc->host);
   }
   return &UNKNOWN;
}

String* critter::getSayColor() {
   if (pc) {
      return &(pc->say_str);
   }
   return &NULL_STRING;
}

String* critter::getYellColor() {
   if (pc) {
      return &(pc->yell_str);
   }
   return &NULL_STRING;
}

String* critter::getTellColor() {
   if (pc) {
      return &(pc->tell_str);
   }
   return &NULL_STRING;
}

String* critter::getGossipColor() {
   if (pc) {
      return &(pc->gos_str);
   }
   return &NULL_STRING;
}

String* critter::getDescColor() {
   if (pc) {
      return &(pc->desc_str);
   }
   return &NULL_STRING;
}

String* critter::getObjListColor() {
   if (pc) {
      return &(pc->obj_list_str);
   }
   return &NULL_STRING;
}

String* critter::getMobListColor() {
   if (pc) {
      return &(pc->mob_list_str);
   }
   return &NULL_STRING;
}

String* critter::getDefaultColor() { //for foreground
   if (pc) {
      return &(pc->dflt_str);
   }
   return &NULL_STRING;
}

String* critter::getBackGroundColor() {
   if (pc) {
      return &(pc->bk_str);
   }
   return &NULL_STRING;
}

String* critter::getBattleColor() {
   if (pc) {
      return &(pc->battle_str);
   }
   return &NULL_STRING;
}

int critter::isUsingColor() {
   return pc && pc->pc_data_flags.get(26);
}

int critter::getAge() const {
   int age = 18;

   if (pc) {

      if (Day >= BIRTH_DAY) {
         age++;
      }//if

      age += Year - BIRTH_YEAR;
   }//if
   return age;
}//get_age

int critter::getXpToNextLevel() const {
   int lvl = bound(0, MAX_LEVEL -1, LEVEL);
   return levels[lvl] - EXP;
}//getXpToNextLevel



void critter::doScriptJump(int abs_index) {
   if (mob)
      mob->doScriptJump(abs_index);
}


void critter::remember(critter& targ) {

   if (isPc())
     return;

   if (doesRemember(targ))
      return; //already remembering this fellow

   if (!mob->proc_data) {
      mob->proc_data = new spec_data;
   }//if

   if (!mob->proc_data->temp_proc) {
      mob->proc_data->temp_proc = new temp_proc_data;
   }//if

   HUNTING.prepend(new String(*(targ.getShortName())));
}//remember


int critter::doesRemember(critter& targ) {

   if (isPc())
      return FALSE;

   if (!mob->proc_data || !mob->proc_data->temp_proc) {
      return FALSE;
   }//if

   Cell<String*> cll(HUNTING);
   String* ptr;
   while ((ptr = cll.next())) {
      if (targ.isNamed(*ptr)) {
         return TRUE;
      }//if
   }//while

   return FALSE;
}//doesRemember


void critter::doHuntProc(int num_steps, int& is_dead) {
   List<int> path;
   int targ_room_num;

   if (!mob || !mob->proc_data || 
       !mob->proc_data->temp_proc) { 
      mudlog.log(ERR, "ERROR:  bad pc sent to do_hunt_proc.\n");
      return;
   }//if

   critter* targ = have_crit_named(pc_list, 1, getTrackingTarget(),
                                   ~0, *(getCurRoom()));
   if (!targ) {
      targ = have_crit_named(affected_mobs, 1,
                             getTrackingTarget(), ~0, *(getCurRoom()));
   }//if

   if (!targ) { //quit hunting that person (maybe)
      Cell<String*> cll(HUNTING);
      String* sp, *stp;
      setTrackingTarget(NULL_STRING);

      sp = cll.next();
      while (sp) {
         /* There is a chance we will forget if the target is not
          * logged on currently.
          */
	 if ((strcasecmp(*sp, TRACKING) == 0) && (d(1,50) == 5)) {
	    stp = sp;
	    sp = HUNTING.lose(cll);
	    delete stp;
	    stp = NULL;
         }//if
	 else {
	    TRACKING = *sp;
	    break;
	 }//if
      }//while
   }//if can't find target
   else { //go get em!
      if (targ->isMob()) { 
	 mudlog.log(ERR, "ERROR:  targ is MOB in do_hunt_proc.\n");
	 return;
      }//if

      if (!targ->isStanding()) {
         //Try to get up...
         wake(*targ);
         stand(*targ);
      }

      targ_room_num = targ->getCurRoomNum();

      if (targ_room_num == getCurRoomNum()) {
         checkForBattle(*(getCurRoom()));
         return; //already there!
      }

      path_from_a_to_b(getCurRoomNum(), targ_room_num, path);

      if (IsEmpty(path)) {
	 return; //couldn't find it
      }//if
      else { //got a valid path
         // First, get rid of the first step, that is the current room.
         path.popFront();

	 int iter = min(path.size(), num_steps);

	 for (int i = 0; ((i < iter) && (IsEmpty(IS_FIGHTING))); i++) {
            int next_room = path.popFront();
	    String* dir = dir_of_room(*(getCurRoom()), next_room);

            if (!dir) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "WARNING: Could not find door to room: "
                         << next_room << " from room: " << getCurRoomNum()
                         << endl;
               }//if
               return;
            }//if

	    door* dptr = door::findDoor(getCurRoom()->DOORS, 1, dir, ~0,
                                        *(getCurRoom()));

            if (!dptr) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "WARNING: Could not find door: " << *dir 
                         << " to room: "
                         << next_room << " from room: " << getCurRoomNum()
                         << endl;
               }//if
               return;
            }//if

	    if (dptr->isClosed()) {
	       if (!open(1, dir, *this)) {
	          return;  //cant get there from here
	       }//if
	    }//if

            //move will take care of attack
	    move(*this, 1, *dir, TRUE, *(getCurRoom()), is_dead);
            
            if (is_dead) {
               break;
            }
	 }//for
      }//else
   }//else go get em
}//do_hunt_proc


int critter::isPlayerShopKeeper() {
   return (mob && mob->proc_data && mob->proc_data->sh_data
           && mob->proc_data->sh_data->shop_data_flags.get(3));
}//isPlayerShopKeeper


// Price this keeper will sell the object for.
int critter::findItemSalePrice(object& item, critter& pc) {
   int price = 0;

   if (!mob || !mob->proc_data || !mob->proc_data->sh_data) {
      return 0;
   }

   if (isPlayerShopKeeper()) {
      price = mob->proc_data->sh_data->findItemSalePrice(item);
   }//if
   else { // automagically figure it out
      price = (int)((float)(item.PRICE) * 
                    ((float)(MARKUP) / 100.0));
      
      if (pc.getHomeTown() != (getHomeTown())) {
         price = (int)((float)(price) * OUT_OF_TOWN_MODIFIER);
      }//if
      

      if (pc.pc && (d(1, 100) <
                    get_percent_lrnd(COMMERCE_SKILL_NUM, pc))) {
         price = (int)((float)price * COMMERCE_SKILL_EFFECT_BUY);
      }//if
   }//else

   return price;
}//findItemSalePrice

// Price this keeper will buy the object for.
int critter::findItemBuyPrice(object& item, critter& pc) {
   int price = 0;

   if (!mob || !mob->proc_data || !mob->proc_data->sh_data) {
      return 0;
   }

   if (isPlayerShopKeeper()) {
      price = mob->proc_data->sh_data->findItemBuyPrice(item);
   }//if
   else { // automagically figure it out
      price = (int)((float)(item.PRICE) * 
                    ((float)(BUY_PERCENTAGE) / 100.0));

      if (pc.getHomeTown() != getHomeTown()) 
         price = (int)(((float)(price))  * (1.0 / OUT_OF_TOWN_MODIFIER));

      if (pc.pc && (d(1, 100) <
                    get_percent_lrnd(COMMERCE_SKILL_NUM, pc))) {
         price = (int)((float)price * COMMERCE_SKILL_EFFECT_SELL);
      }//if
   }//else

   return price;
}//findItemBuyPrice


void critter::transferShopDataTo(critter& sink) {
   if (!mob || !mob->proc_data || !mob->proc_data->sh_data) {
      mudlog << "ERROR: transferShopDataTo, this does not have sh_data"
             << endl;
      return;
   }

   if (!sink.mob || !sink.mob->proc_data || !sink.mob->proc_data->sh_data) {
      mudlog << "ERROR: transferShopDataTo, sink does not have sh_data"
             << endl;
      return;
   }

   clear_ptr_list(sink.mob->proc_data->sh_data->ps_data_list);

   Cell<PlayerShopData*> cll(mob->proc_data->sh_data->ps_data_list);
   PlayerShopData* ptr;

   while ((ptr = cll.next())) {
      sink.mob->proc_data->sh_data->ps_data_list.pushBack(new PlayerShopData(*ptr));
   }//while

}//transferShopDataTo

void critter::valueRem(int idx, critter& manager) {
   if (!mob || !mob->proc_data || !mob->proc_data->sh_data) {
      mudlog << "ERROR: valueRem, this does not have sh_data"
             << endl;
      return;
   }

   mob->proc_data->sh_data->valueRem(idx, manager);
}//valueRem

void critter::valueAdd(object& obj, critter& manager) {
   if (!mob || !mob->proc_data || !mob->proc_data->sh_data) {
      mudlog << "ERROR: valueAdd, this does not have sh_data"
             << endl;
      return;
   }

   mob->proc_data->sh_data->valueAdd(obj, manager);
}//valueAdd

void critter::valueList(int i_th, const String* targ, critter& manager) {
   if (!mob || !mob->proc_data || !mob->proc_data->sh_data) {
      mudlog << "ERROR: valueList, this does not have sh_data"
             << endl;
      return;
   }

   mob->proc_data->sh_data->valueList(i_th, targ, *this, manager);
}//valueList

void critter::valueSet(int val_idx, int sell_val, int buy_val,
                       critter& manager) {
   if (!mob || !mob->proc_data || !mob->proc_data->sh_data) {
      mudlog << "ERROR: valueSet, this does not have sh_data"
             << endl;
      return;
   }

   mob->proc_data->sh_data->valueSet(val_idx, sell_val, buy_val, manager);
}//valueSet

int critter::isManagedBy(critter& pc) {
   return (isPlayerShopKeeper() &&
           (strcasecmp(mob->proc_data->sh_data->manager, *(pc.getName())) == 0));
}//isManagedBy
