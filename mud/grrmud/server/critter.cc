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
// To contact the Author, Ben Greear:  greear@cyberhighway.net, (preferred)
//                                     greearb@agcs.com
//

//***********************************************************//
//************************  mob data  **********************///

#include <arpa/telnet.h>
#include "script.h"
#include "critter.h"
#include "misc.h"
#include "misc2.h"
#include <PtrArray.h>
#include "room.h"
#include "const.h"
#include "commands.h"
#include "command2.h"
#include "command3.h"
#include "command5.h"
#include "batl_prc.h"
#include "battle.h"
#include <stdarg.h>
#include "clients.h"
#include "telnet_handler.h"
#include "hegemon_handler.h"

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
   tmplist.clearAndDestroy();
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
   ps_data_list.clearAndDestroy();
}//Clear

void shop_data::Read(ifstream& da_file, short read_all) {
   int i;
   String buf(100);
   char tmp[81];
   
   Clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERROR)) {
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
         new_obj->fileRead(da_file, read_all);
         new_obj->IN_LIST = &(perm_inv); //make sure its a SOBJ
         perm_inv.append(new_obj);    //add it to inventory
      }//if
      else {
         if (obj_list[i].OBJ_FLAGS.get(10)) {
            if (read_all || 
                ((obj_list[i].OBJ_PRCNT_LOAD*config.currentLoadModifier)/100) > 
                d(1,100)) {
               perm_inv.append(&(obj_list[i]));    //add it to inventory
            }//if
         }//if
         else {
            Sprintf(buf, 
                    "ERROR:  trying to load non_exist. obj: %i in perm_inv.\n",
                    i);
            mudlog.log(ERROR, buf);
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
      Sprintf(client, "<VALUE_LIST %i %S ?> ", i_th, targ);
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
   hunting.clearAndDestroy();
}//destructor

void temp_proc_data::Clear() {
   hunting.clearAndDestroy();
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
      if (mudlog.ofLevel(ERROR)) {
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
      if (mudlog.ofLevel(ERROR)) {
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

   topics.clearAndDestroy();
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
   topics.clearAndDestroy();
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
      if (mudlog.ofLevel(ERROR)) {
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
         mudlog.log(ERROR, "ERROR:  trying to write sh_data, but it's NULL\n");
         return;
      }//if
      sh_data->Write(da_file);
   }//if

   if (flag1.get(2)) {  // teacher
      if (!teach_data) {
         mudlog.log(ERROR, 
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

int temp_crit_data::doUnShield() {
   if (shielding) {
      if (shielding->temp_crit && 
          shielding->temp_crit->shielded_by) {
         shielding->temp_crit->shielded_by = NULL;
      }//if
      shielding = NULL;
      return 0;
   }//if
   return -1;
}//doUnShield


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

   doUnShield();
}//Clear





///*******************************  mob data **************************///

int mob_data::_cnt = 0;

mob_data::mob_data() {
   _cnt++;
   mob_num = tmp_num = cur_in_game = max_in_game = 0;
   proc_data = NULL;
   cur_script = NULL;
   skin_num = 0;
   home_room = 0;
   ticks_old = ticks_till_freedom = 0;
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
   mob_proc_scripts.clearAndDestroy();
   proc_data = NULL;
   
   cur_script = NULL; //it was held in the mob_proc_scripts
}//destructor
 
void mob_data::Clear() {
   mob_num = tmp_num = cur_in_game = max_in_game = 0;
   delete proc_data;
   proc_data = NULL;
   mob_data_flags.Clear();
   skin_num = 0;
   mob_proc_scripts.clearAndDestroy();
   pending_scripts.clearAndDestroy();

   cur_script = NULL; //its held in the mob_proc_scripts
   home_room = 0;
   ticks_old = ticks_till_freedom = 0;
}//Clear, mob_data

void mob_data::finishedMobProc() {
   if (cur_script) {
      pending_scripts.loseData(cur_script);
      delete cur_script;
   }

   cur_script = pending_scripts.peekFront();
}//finishedMobProc


void mob_data::addProcScript(const String& txt, MobScript* script_data) {
   //similar to reading it in...
   //first, see if we are over-writing one...
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In mob_data::addProcScript, txt:  \n" << txt
             << "\nscript data:  "
             << script_data->toStringBrief(0, 0, ENTITY_CRITTER, 0)
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
      mudlog.log(ERROR, "script_data is NULL, mob_data::addProcScript.");
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
   home_room = source.home_room;
   ticks_old = source.ticks_old;
   ticks_till_freedom = source.ticks_till_freedom;

   Cell<MobScript*> cll;
   source.mob_proc_scripts.head(cll);
   MobScript* ptr;
   
   while ((ptr = cll.next())) {
      mob_proc_scripts.append(new MobScript(*ptr));
   }
   
   return *this;
}//mob_data operator=


void mob_data::Write(ofstream& ofile) {
   
   ofile << " " << mob_num << " " << tmp_num << " " 
         << max_in_game << "\tmob#, tmp_num, max_n_game\n";
   
   mob_data_flags.Write(ofile);
   
   if (mob_data_flags.get(0)) { //does it have spec_data?
      if (!(proc_data)) {
         mudlog.log(ERROR, "ERROR:  told to write proc_data, but is NULL!\n");
         return;
      }//if
      proc_data->Write(ofile);
   }//if
   if (mob_data_flags.get(16)) {
      ofile << skin_num << "        skin number\n";
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


void mob_data::Read(ifstream& ofile, short read_all, int format_version) {
   char tmp[81];
   
   Clear();
   
   if (!ofile) {
      if (mudlog.ofLevel(ERROR)) {
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
            if (mudlog.ofLevel(ERROR)) {
               mudlog << "ERROR:  mob_data reading script da_file FALSE." << endl;
            }
            return;
         }

         ptr = new MobScript();
         ptr->read(ofile);
         mob_proc_scripts.append(ptr);
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
   p_handler = NULL;
   post_msg = NULL;
   snoop_by = NULL;
   snooping = NULL;
   imm_data = NULL;
   w_eye_obj = NULL;
   client = NO_CLIENT;
   Clear();
}//constructor

pc_data::pc_data(const pc_data& source) {
   _cnt++;
   p_handler = NULL;
   post_msg = NULL;
   snoop_by = NULL;
   snooping = NULL;
   imm_data = NULL;
   w_eye_obj = NULL;
   client = NO_CLIENT;
   *this = source;
}//constructor

pc_data::~pc_data() {
   _cnt--;

   Clear();
}//destructor
 
void pc_data::Clear() {

   if (post_msg) {
      delete post_msg;
      post_msg = NULL;
   }

   password.Clear();
   input.Clear();
   output.Clear();
   last_input.Clear();
   host.Clear();
   mode = MODE_NORMAL;
   descriptor = age = index = hunger = thirst = drugged = violence_timer = 0;
   birth_year = birth_day = rent_day = rent_year = pk_count
      = died_count = quest_points = idle_ticks = last_keepalive
      = remort_count = 0;
   link_condition = CON_LOGGING_IN;

   if (imm_data) {
      delete imm_data;
      imm_data = NULL;
   }

   if (snoop_by) {
      snoop_by->pc->snooping = NULL;
      snoop_by = NULL;
   }

   if (snooping) {
      snooping->pc->snoop_by = NULL;
      snooping = NULL;
   }

   pc_data_flags.Clear();
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
   room_str = ANSI_BLACK;

   preferred_language = English;
   
   last_login_time = 0; //in seconds, since 1970 etc...
   total_time_online = 0; //in seconds
   lines_on_page = 20;
   columns_on_page = 80;
   if (w_eye_obj) {
      w_eye_obj->obj_proc->w_eye_owner = NULL;
      w_eye_obj = NULL;
   }//if

   // Volatile members (not saved to disk).
   bug_num = 0;
   bug_comment.Clear();

   // Clear wanted_in[] data
   for (int i = 0; i<NUMBER_OF_ZONES+1; i++ ) {
      wanted_in[i] = 0;
   }

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
   columns_on_page = source.columns_on_page;
   index = source.index;
   hunger = source.hunger;
   thirst = source.thirst;
   drugged = source.drugged;
   pk_count = source.pk_count;
   died_count = source.died_count;
   remort_count = source.remort_count;
   quest_points = source.quest_points;
   idle_ticks = source.idle_ticks;
   last_keepalive = source.last_keepalive;
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
   room_str = source.room_str;


   preferred_language = source.preferred_language;

   return *this;
}//operator=, pc_data


void pc_data::Write(ofstream& ofile) {
   int i, ii;

   ofile << "%" << PC_DATA_FORMAT_VERSION << "\tpc_data format version" << endl;
   
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
         << " " << remort_count << " age hgr thr drg pk died qp remort_cnt\n";
   
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
         mudlog.log(ERROR, "ERROR:  flagged imm, but no imm_data file!\n");
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
      ofile << room_str << " Room Color" << endl;
   }//if

   if (pc_data_flags.get(27)) {
      ofile << (int)(preferred_language) << " -1 preferred_language\n";
   }

   // store wanted_in[] information
   ii = 0;
   for (i = 0; i < NUMBER_OF_ZONES+1; i++) {
      if ( wanted_in[i] ) {
         if ( ((ii++)+1) % 20 == 0) {
            ofile << endl;
         }
         ofile << i << " ";
      }
   }//for
   ofile << "-1 \twanted in\n";
   
   ofile << "*** end of pc data ***\n";
}//Write()       


void pc_data::Read(critter* parent, ifstream& ofile) {
   int i;
   char tmp[81];
   int format_version;
   
   Clear();

   if (!ofile) {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  da_file FALSE in pc_data read." << endl;
      }
      return;
   }

   // look for a file-version identifier
   {
      char tst_char;
      ofile.get(tst_char);
      if ( tst_char != '%' ) {
         // no file version 
         ofile.putback(tst_char);
         format_version = 0;
      } else {
         ofile >> format_version;
         ofile.getline(tmp, 80);
      }
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
   if ( format_version > 0 ) {
      ofile >> remort_count;
   }
   ofile.getline(tmp, 80);
   
   if ( format_version == 0 ) {
      /* skills spells known */
      ofile >> i;
      int tmp_int;
      while (i != -1) {
         if (!ofile) {
            if (mudlog.ofLevel(ERROR)) {
               mudlog << "ERROR:  da_file FALSE in pc_data read." << endl;
            }
            return;
         }
         ofile >> tmp_int;
         parent->skills_spells_known.Insert(i, tmp_int);
         ofile >> i;
      }//while
      ofile.getline(tmp, 80);
   }//if format_version == 0
   
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
   
   if (pc_data_flags.get(2))         {
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

      ofile >> room_str;
      ofile.getline(tmp, 80);
   }//if

   if (pc_data_flags.get(27)) {
      int t;
      ofile >> t;
      if ((t < 0) || (t > LastLanguage)) {
         t = English;
      }
      preferred_language = (LanguageE)(t);
      ofile.getline(tmp, 80);
   }

   if ( format_version > 0 ) {
      // Read in wanted_in information
      ofile >> i;
      while (i != -1) {
         wanted_in[i] = 1;
         ofile >> i;
      }
      ofile.getline(tmp, 80);
   }

   ofile.getline(tmp, 80); //grabs extra line/comment
}//Read()       

//This gets called when we have a socket problem and need to transistion to
//some other state.
void pc_data::SocketProblem() {

   switch ( link_condition ) {

      case 1://CON_PLAYING
         link_condition = 2;
      break;

      case 2://CON_LINKDEAD
         mudlog << "Already linkdead but getting more socket errors." << endl;
      break;

      case 3://CON_LOGGING_IN
         link_condition = 2;
      break;
   };

   switch ( mode ) {

      case MODE_LOGGING_IN:// safe to go bye bye from here
         mode = MODE_LOGOFF_NEWBIE_PLEASE;
      break;

      case MODE_QUIT_ME_PLEASE:
      case MODE_LOGOFF_NEWBIE_PLEASE:
         mudlog << "Got socket errors while awaiting logoff." << endl;
      break;

      case MODE_GO_LINKDEAD_PLEASE:
         mudlog << "Already in MODE_GO_LINKDEAD_PLEASE and received socket errors." << endl;
      break;

      default:
         mode = MODE_GO_LINKDEAD_PLEASE;

   }

}//pc_data::SocketProblem()


//*************************************************************//
///*********************  critter  ***************************///

int critter::_cnt = 0;

critter::critter() {
//   if (mudlog.ofLevel(DBG)) {
//      mudlog << "MEMORY:  creating crit:  " << this << endl;
//   }
   _cnt++;

   obj_ptr_log << "CRI_CON 0 " << this << "\n";

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
   obj_ptr_log << "CRI_CC " << getIdNum() << " " << this << "\n";
} // crit constructor


critter::~critter() {
//   if (mudlog.ofLevel(DBG)) {
//      mudlog << "MEMORY:  deleting critter:  " << this << endl;
//   }
   _cnt--;

   obj_ptr_log << "CRI_DES " << getIdNum() << " " << this << "\n";

   if (isMob() && (!do_shutdown)) {
      core_dump("ERROR:  trying to delete a MOB before shutdown.\n");
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
      names.append(new String(*sptr));
   }//while

   short_desc = source.short_desc;
   in_room_desc = source.in_room_desc;
   long_desc = source.long_desc;
   skills_spells_known = source.skills_spells_known; //overload operator

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
      mini_affected_by.append(tmp_stat2);
   }//while
   
   source.inv.head(cll);
   while ((obj_ptr = cll.next())) {
      if (!obj_ptr->IN_LIST) { //no multiple ptrs to SOBJ's
         inv.append(obj_ptr);
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


int critter::showTime() {
   if (isSleeping()) {
      show("Time moves in funny ways when you are asleep!!\n");
      return -1;
   }

   String buf(50);

   Sprintf(buf, "It is %s.\n", military_to_am(config.hour));
   show(buf);
   return 0;
}//time




/** NOTE:  This may normalize the real value.
 */
int critter::getDamRecMod() const {
   int min_val = 25;
   if (is_affected_by(FLESH_TO_STONE_SKILL_NUM, *this)) {
      min_val = 1;
   }

   if (isImmort() || (isNPC() && !isCharmed())) {
      min_val = 1;
   }

   return (max(min_val, DAM_REC_MOD));
}


int critter::isInGroupWith(critter* v) {
   return GROUPEES.haveData(v);
}


/** Make sure that everyone in THIS critter's group list is in all the other
 * member's lists.
 */
int critter::makeGroupSane() {
   Cell<critter*> cll(GROUPEES);
   Cell<critter*> cll2;
   critter* ptr, *ptr2;
   
   // For every one in the group....
   while ((ptr = cll.next())) {
      // Make sure that they know about all the others.
      GROUPEES.head(cll2);
      while ((ptr2 = cll2.next())) {
         ptr->GROUPEES.gainData(ptr2); //Will not insert duplicates, btw.
      }
   }
   return 0;
}


void critter::split(int amt, int do_msg) {
   int d = GROUPEES.size();
   String buf(100);

   if (d > 0) {
      Cell<critter*> cll(GROUPEES);
      critter* ptr;

      d = 0;
      while ((ptr = cll.next())) {
         if( ptr->getCurRoomNum() == getCurRoomNum() ) {
            d++;
         }
      };

      GROUPEES.head(cll);

      while ((ptr = cll.prev())) {
         if (ptr->getCurRoomNum() == getCurRoomNum()) {
            ptr->GOLD += amt / d;

            if (do_msg) {
               Sprintf(buf, "%S splits %i coins, your share is: %i\n", getName(*ptr), amt, amt/d);
               ptr->show(buf);
            }
         }
      }
   }
   else {
      GOLD += amt;
      Sprintf(buf, "You split %i coins with yourself!\n", amt);
      show(buf);
   }
}


int critter::isGroupLeader() {
   if (GROUPEES.isEmpty()) {
      return FALSE;
   }
   else {
      // We have groupees...
      if (!FOLLOWER_OF || (FOLLOWER_OF == this)) {
         // following self, so must be the leader.
         return TRUE;
      }
      else {
         // Is following someone other than self
         if (FOLLOWER_OF->GROUPEES.haveData(this)) {
            //in follower's group
            return FALSE;
         }
         else {
            // Not in follower_of's group, but has a group of it's own...
            return TRUE;
         }
      }
   }
}


void critter::Clear() {
   int i;
   critter* ptr;

   //mudlog.log(DBG, "In crit Clear().\n");
   names.clearAndDestroy();
   short_desc.Clear();
   in_room_desc.Clear();
   long_desc.Clear();
   skills_spells_known.Clear();

   crit_flags.Clear();
   spam_cnt = 0;

   if (pc) {
      delete pc;
      pc = NULL;
   }

   if (mob) {
      delete mob;
      mob = NULL;
   }

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

   affected_by.clearAndDestroy();
   mini_affected_by.clearAndDestroy();
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

      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  followers not empty in Clear, for mob: "
                << *(getName()) << " follower: " << *(ptr->getName()) << endl;
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
   possessed_by = NULL;

   //mudlog.log(DBG, "Done w/clear.\n");
}//crit Clear


int critter::doUnShield() {
   if (temp_crit) {
      return temp_crit->doUnShield();
   }
   return -1;
}


const char* critter::getPosnStr(critter& for_this_pc) {
   switch (getPosn())
   {
      case POS_STAND:
         if ( isFlying() ) {
            return cstr(CS_FLYING, for_this_pc);
         } else {
            return cstr(CS_STANDING, for_this_pc);
         }
      case POS_SIT:
         return cstr(CS_SITTING, for_this_pc);
      case POS_PRONE:
         return cstr(CS_PRONE, for_this_pc);
      case POS_REST:
         return cstr(CS_RESTING, for_this_pc);
      case POS_SLEEP:
         return cstr(CS_SLEEPING, for_this_pc);
      case POS_MED:
         return cstr(CS_MEDITATING, for_this_pc);
      case POS_STUN:
         return cstr(CS_STUNNED, for_this_pc);
      case POS_DEAD:
         return cstr(CS_DEAD, for_this_pc);
      default:
         return cstr(CS_UNKNOWN, for_this_pc);
   }//switch
}//getPosnStr


int critter::travelToRoom(int targ_room_num, int num_steps, int& is_dead) {
   List<int> path(0);

   if (!isStanding()) {
      //Try to get up...
      wake(*this);
      stand(*this);
   }

   path_from_a_to_b(getCurRoomNum(), targ_room_num, path);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "travelToRoom: targ_room: " << targ_room_num << " num_steps: "
             << num_steps << " from_room: " << getCurRoomNum() << endl;
      Cell<int> cll(path);
      int foo;
      mudlog << "Path: ";
      while ((foo = cll.next())) {
         mudlog << " [" << foo << "] ";
      }//while
      mudlog << "\n\n";
   }//if

   if (path.isEmpty()) {
      return -1; //couldn't find it
   }//if
   else { //got a valid path
      // First, get rid of the first step, that is the current room.
      path.popFront();

      int iter = min(path.size(), num_steps);

      for (int i = 0; ((i < iter) && (IS_FIGHTING.isEmpty())); i++) {
         int next_room = path.popFront();
         String* dir = dir_of_room(*(getCurRoom()), next_room);

         if (!dir) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "WARNING: Could not find door to room: "
                      << next_room << " from room: " << getCurRoomNum()
                      << " critter: " << *(getName()) << " targ_room_num: "
                      << targ_room_num << " num_steps: " << num_steps << endl;
            }//if
            return -1;
         }//if
         else {
            if (mudlog.ofLevel(DBG)) {
               mudlog << "\ttravelToRoom, dir: " << *dir << endl;
            }
         }

         door* dptr = door::findDoor(getCurRoom()->DOORS, 1, dir, ~0,
                                     *(getCurRoom()));
         
         if (!dptr) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "WARNING: Could not find door: " << *dir 
                      << " to room: "
                      << next_room << " from room: " << getCurRoomNum()
                      << endl;
            }//if
            return -1;
         }//if
         

         if (dptr->isLocked()) {
            if (unlock(1, dir, *this) < 0) {
               if (mudlog.ofLevel(DBG)) {
                  mudlog << "WARNING: Could not unlock door: " << *dir 
                         << " to room: " << next_room << " from room: "
                         << getCurRoomNum() << endl;
               }//if
               return -1;  //cant get there from here
            }//if
         }//if

         if (dptr->isClosed()) {
            if (!open(1, dir, *this)) {
               if (mudlog.ofLevel(DBG)) {
                  mudlog << "WARNING: Could not open door: " << *dir 
                         << " to room: " << next_room << " from room: "
                         << getCurRoomNum() << endl;
               }//if
               return -1;  //cant get there from here
            }//if
         }//if
         
         //move will take care of attack
         if (move(*this, 1, *dir, TRUE, *(getCurRoom()), is_dead) < 0) {
            if (mudlog.ofLevel(DBG)) {
               if (!is_dead) {
                  mudlog << "INFO: failed to move in travelToRoom, next_room: "
                         << next_room << " from room: " << getCurRoomNum()
                         << " critter: " << *(getName()) << " targ_room_num: "
                         << targ_room_num << " num_steps: " << num_steps << endl;
               }
               else {
                  mudlog << "INFO: failed to move in travelToRoom, next_room: "
                         << next_room << " targ_room_num: " << targ_room_num 
                         << " num_steps: " << num_steps << endl;
               }
            }//if
            return -1;
         }//if

         if (is_dead) {
            return -1;
         }
      }//for
   }//else
   return 0;
}//travelToRoom


int critter::getNakedWeight() const {
   if (short_cur_stats[41] == 0) {
      return 150;
   }
   return short_cur_stats[41];
}//getNakedWeight


int critter::getCurWeight() {// of inv...eq not taken into account here
   Cell<object*> cll(inv);
   object* tmp_obj;
   int retval = 0;
   while ((tmp_obj = cll.next())) {
      retval += tmp_obj->getCurWeight();
   }//while

   for (int i = 1; i<MAX_EQ; i++) {
      if (EQ[i]) {
         retval += EQ[i]->getCurWeight() / 2;
      }//if
   }//for

   retval += GOLD / config.goldPerLb; //gold weighs a little..though not much

   return retval + getNakedWeight();
} // cur_weight

int critter::getMaxWeight() {
   return (getSTR(true) * 10 + getNakedWeight());  // 10 times their str
}//max_weight

int critter::getCurRoomNum() {
   return IN_ROOM;
}

int critter::getSTR(bool include_modifiers=false) const {
   int p_lrnd;
   int modifier = 0;

   if ( include_modifiers && pc ) {
      p_lrnd = get_percent_lrnd(STRENGTH_CONDITIONING_SKILL_NUM, *this);
      if ( p_lrnd > 0 ) {
         modifier += p_lrnd/30;
      }
   }

   return STR+modifier;
}

int critter::getDEX(bool include_modifiers=false) const {
   int p_lrnd;
   int modifier = 0;

   if ( include_modifiers && pc ) {
      p_lrnd = get_percent_lrnd(QUICKFOOT_SKILL_NUM, *this);
      if ( p_lrnd > 0 ) {
         modifier += p_lrnd/30;
      }
   }

   return DEX+modifier;
}

int critter::getWIS(bool include_modifiers=false) const {
   int p_lrnd;
   int modifier = 0;

   if ( include_modifiers && pc ) {
      p_lrnd = get_percent_lrnd(HONOR_CODE_SKILL_NUM, *this);
      if ( p_lrnd > 0 ) {
         modifier += p_lrnd/30;
      }
   }

   return WIS+modifier;
}

int critter::getCHA(bool include_modifiers=false) const {
   int p_lrnd;
   int modifier = 0;

   if ( include_modifiers && pc ) {
      p_lrnd = get_percent_lrnd(LEADERSHIP_SKILL_NUM, *this);
      if ( p_lrnd > 0 ) {
         modifier += p_lrnd/30;
      }
   }

   return CHA+modifier;
}

int critter::getCON(bool include_modifiers=false) const {
   return CON;
}//critter::getCON()

int critter::getINT(bool include_modifiers=false) const {
   int p_lrnd;
   int modifier = 0;

   if ( include_modifiers && pc ) {
       p_lrnd = get_percent_lrnd(LOGIC_SKILL_NUM, *this);
       if ( p_lrnd > 0 ) {
           modifier += p_lrnd/30;
       }
   }

   return INT+modifier;
}//critter::getINT()

int critter::getBHDC(bool include_modifiers=false) const {
   int modifier = 0;

   return BH_DICE_COUNT+modifier;
}

int critter::getBHDS(bool include_modifiers=false) const {
   int p_lrnd;
   int modifier = 0;

   if ( include_modifiers && pc ) {
      p_lrnd = get_percent_lrnd(MARTIAL_ARTS_SKILL_NUM, *this);
      if ( p_lrnd > 0 ) {
         modifier += p_lrnd/50;
      }
   }

   return BH_DICE_SIDES+modifier;
}

int critter::getDAM(bool include_modifiers=false) const {
   int p_lrnd;
   int modifier = 0;

   if ( include_modifiers && pc ) {
      p_lrnd = get_percent_lrnd(WEAPON_MASTERY_SKILL_NUM, *this);
      if ( p_lrnd > 0 ) {
         modifier += p_lrnd/30;
      }
   }

   return DAM+modifier;
}

int critter::getHIT(bool include_modifiers=false, object* weapon = NULL) const {
   int p_lrnd;
   int modifier = 0;
   int weapon_skill = 0;

   if ( include_modifiers && pc ) {

      //not bare handed
      if ( weapon ) {
         if ( weapon->isSlash() ) {
            weapon_skill = SWORD_SKILL_NUM;
         } else if ( weapon->isPierce() ) {
            weapon_skill = DAGGER_SKILL_NUM;
         } else if ( weapon->isSmash() ) {
            weapon_skill = MACE_SKILL_NUM;
         } else if ( weapon->isWhip() ) {
            weapon_skill = WHIP_SKILL_NUM;
         } else if ( weapon->isBow() ) {
            weapon_skill = BOW_SKILL_NUM;
         }
      } else { //not weapon
         weapon_skill = MARTIAL_ARTS_SKILL_NUM;
      }

      p_lrnd = get_percent_lrnd(weapon_skill, *this);
      if ( p_lrnd > 0 ) {
         modifier += p_lrnd/10;
      }

   }//if include modifiers

      return HIT+modifier;
}//critter::getHIT

//min_max is 0 or 1. If 0, the minimum damage done by that hand is returned,
//otherwise the maximum possible damage. This will handle both weapons and
//bare handed. Originally writter for use in the "score" output.
int critter::getWeapRange(short min_max, int position, bool include_modifiers=false) const {

   int ret_val, count, sides, weapon_skill, p_lrnd;
   object *weapon = EQ[position];

   ret_val = count = sides = weapon_skill = p_lrnd = 0;

   if ( weapon && weapon->isWeapon() )  {

      count = weapon->getDiceCnt();
      sides = weapon->getDiceSides();

      if ( weapon->isSlash() ) {
         weapon_skill = SWORD_SKILL_NUM;
      } else if ( weapon->isPierce() ) {
         weapon_skill = DAGGER_SKILL_NUM;
      } else if ( weapon->isSmash() ) {
         weapon_skill = MACE_SKILL_NUM;
      } else if ( weapon->isWhip() ) {
         weapon_skill = WHIP_SKILL_NUM;
      } else if ( weapon->isBow() ) {
         weapon_skill = BOW_SKILL_NUM;
      }

      p_lrnd = get_percent_lrnd(weapon_skill, *this);
      if ( p_lrnd > 0 ) {
         count += p_lrnd/50;
         sides += p_lrnd/33;
      }

   } else {//we're bare handed
      count = getBHDC(true);
      sides = getBHDS(true);
   }

   if ( min_max == 0 ) {// minimum
      ret_val += count;
   } else {//maximum
      ret_val += count * sides;
   }

   ret_val += getDAM(true);

   ret_val += (int)( (getSTR(true) - 10) / 2.0 );

   if ( position == 10 ) {//penalty for off-hand
      p_lrnd = get_percent_lrnd(DUAL_WIELD_SKILL_NUM, *this);
      ret_val = (int)( ret_val * ( 0.50 + p_lrnd/500.0 ) );
   }

   ret_val += getDAM(TRUE);
   return ret_val;
}//critter::getWeapRange

float critter::getWeapDAM(int position, bool include_modifiers=false) const {
   int count = 0;
   int sides = 0;
   int weapon_skill = 0;
   int p_lrnd;
   object *weapon = EQ[position];

   if ( (weapon) && (weapon->isWeapon()) ) {

      if ( weapon->isSlash() ) {
         weapon_skill = SWORD_SKILL_NUM;
      } else if ( weapon->isPierce() ) {
         weapon_skill = DAGGER_SKILL_NUM;
      } else if ( weapon->isSmash() ) {
         weapon_skill = MACE_SKILL_NUM;
      } else if ( weapon->isWhip() ) {
         weapon_skill = WHIP_SKILL_NUM;
      } else if ( weapon->isBow() ) {
         weapon_skill = BOW_SKILL_NUM;
      }

      p_lrnd = get_percent_lrnd(weapon_skill, *this);
      if ( p_lrnd > 0 ) {
         count = p_lrnd/50;
         sides = p_lrnd/33;
      }

      count += weapon->getDiceCnt();
      sides += weapon->getDiceSides();

   } else {//we're bare handed
      count = getBHDC(true);
      sides = getBHDS(true);
   }

   float ret_val = d(count,sides);

   ret_val += getDAM(true);

   ret_val += (getSTR(true) - 10) / 2.0 ;

   if ( position == 10 ) {//penalty for off-hand
      p_lrnd = get_percent_lrnd(DUAL_WIELD_SKILL_NUM, *this);
      ret_val = ret_val * ( 0.50 + p_lrnd/500.0 );
   }

   return ret_val;
}//critter::getWeapDAM

void critter::checkForBattle(room& rm) {

   List<critter*> tmp_crits(rm.getCrits()); //just do shallow copy
   Cell<critter*> cll(tmp_crits);

   critter* ptr;
   while ((ptr = cll.next())) {
      if (!rm.haveCritter(this)) {
         return;
      }
      if (rm.haveCritter(ptr)) {
         if (ptr->doesRemember(*this) && ptr->canDetect(*this)) {
            if (ptr->HP >= (ptr->WIMPY*2 < ptr->HP_MAX*.75?ptr->WIMPY*2:ptr->HP_MAX*.75)) {
               say("There you are!!", *ptr, *(ptr->getCurRoom()));
               try_hit(*this, *ptr);
            }
            else {
               int i = FALSE;
               say("No, stay away!!", *ptr, *(ptr->getCurRoom()));
               flee_to_safety(*ptr, i);
            }
         }//if
      }//if

      if (!rm.haveCritter(this)) {
         return;
      }
      if (rm.haveCritter(ptr)) { //make sure we're still there
         if (doesRemember(*ptr) && canDetect(*ptr)) {
            if (HP >= (WIMPY*2 < HP_MAX*.75?WIMPY*2:HP_MAX*.75)) {
               say("I've found you now!!", *this, *(getCurRoom()));
               try_hit(*ptr, *this);
            }
            else {
               int i = FALSE;
               say("No, stay away!!", *this, *(getCurRoom()));
               flee_to_safety(*this, i);
            }
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

   ofile << "%" << CRITTER_FORMAT_VERSION << "\tcritter format version" << endl;

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
      ofile << " " << pc->host << " __LEVEL__ " << getLevel();
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
      ofile << ss_ptr->stat_spell << " " << ss_ptr->bonus_duration << " "
      << ss_ptr->bonus_value << " ";

      if ((++num_written % 20) == 0)
         ofile << endl;
   }//while
   ofile << -1 << "\taffected_by\n";

   //Skills&Spells
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
         mudlog.log(ERROR, "ERROR: trying to write out pc_data when pc is NULL.\n");
         return;
      }//if
      pc->Write(ofile);  //write it out
   }//if
   else { //its a mob/smob
      if (!mob) {
         mudlog.log(ERROR, "ERROR: trying to write out mob_data when mob is NULL.\n");
         return;
      }//if
      mob->Write(ofile); //write it out
   }//else
   ofile << "\tend_of_crit\n";  //done
}//Write....crit

#ifdef USEMYSQL
void critter::dbRead(int mob_num, int pc_num, short read_all) {
   String wrong_a, wrong_c, wrong_r;
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select * from Critters where PC_NUM=";
   query+=pc_num;
   query+=" and MOB_NUM=";
   query+=mob_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In critter::dbRead(int, int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         } // if
         return;
      } // if
      row=mysql_fetch_row(result);

      short_desc = row[CRITTBL_SHORT_DESC];
      in_room_desc = row[CRITTBL_IN_ROOM_DESC];
      long_desc = row[CRITTBL_LONG_DESC];

      // might not be needed, but this saves re-querying when we actually do
      wrong_a = row[CRITTBL_WRONG_ALIGN_MSG];
      wrong_c = row[CRITTBL_WRONG_CLASS_MSG];
      wrong_r = row[CRITTBL_WRONG_RACE_MSG];

      long_data[0] = atol(row[CRITTBL_GOLD]);
      long_data[1] = atol(row[CRITTBL_EXP_WORTH]);
      long_data[2] = atol(row[CRITTBL_BANK_GOLD]);

      short_cur_stats[0] = atoi(row[CRITTBL_POSITION]);
      short_cur_stats[1] = atoi(row[CRITTBL_STR]);
      short_cur_stats[2] = atoi(row[CRITTBL_INTELLIGENCE]);
      short_cur_stats[3] = atoi(row[CRITTBL_CON]);
      short_cur_stats[4] = atoi(row[CRITTBL_CHA]);
      short_cur_stats[5] = atoi(row[CRITTBL_WIS]);
      short_cur_stats[6] = atoi(row[CRITTBL_DEX]);
      short_cur_stats[7] = atoi(row[CRITTBL_HIT]);
      short_cur_stats[8] = atoi(row[CRITTBL_DAM]);
      short_cur_stats[9] = atoi(row[CRITTBL_AC]);
      short_cur_stats[10] = atoi(row[CRITTBL_ATTACKS]);
      short_cur_stats[11] = atoi(row[CRITTBL_PAUSE_COUNT]);
      short_cur_stats[12] = atoi(row[CRITTBL_SEX]);
      short_cur_stats[13] = atoi(row[CRITTBL_CLASS]);
      short_cur_stats[14] = atoi(row[CRITTBL_RACE]);
      short_cur_stats[15] = atoi(row[CRITTBL_HP]);
      short_cur_stats[16] = atoi(row[CRITTBL_MANA]);
      short_cur_stats[17] = atoi(row[CRITTBL_MOV]);
      short_cur_stats[18] = atoi(row[CRITTBL_ALIGNMENT]);
      short_cur_stats[19] = atoi(row[CRITTBL_LEVEL]);
      short_cur_stats[20] = atoi(row[CRITTBL_HOMETOWN]);
      short_cur_stats[21] = atoi(row[CRITTBL_WIMPY]);
      short_cur_stats[22] = atoi(row[CRITTBL_PRACTICES]);
      short_cur_stats[23] = atoi(row[CRITTBL_HP_MAX]);
      short_cur_stats[24] = atoi(row[CRITTBL_MANA_MAX]);
      short_cur_stats[25] = atoi(row[CRITTBL_MOV_MAX]);
      short_cur_stats[26] = atoi(row[CRITTBL_MOB_TYPE]);
      short_cur_stats[27] = atoi(row[CRITTBL_DAM_REC_MOD]);
      short_cur_stats[28] = atoi(row[CRITTBL_DAM_GIV_MOD]);
      short_cur_stats[29] = atoi(row[CRITTBL_HEAT_RESIS]);
      short_cur_stats[30] = atoi(row[CRITTBL_COLD_RESIS]);
      short_cur_stats[31] = atoi(row[CRITTBL_ELECT_RESIS]);
      short_cur_stats[32] = atoi(row[CRITTBL_SPELL_RESIS]);
      short_cur_stats[33] = atoi(row[CRITTBL_RELIGION]);
      short_cur_stats[34] = atoi(row[CRITTBL_FROM_ZONE]);
      short_cur_stats[35] = atoi(row[CRITTBL_BHD_COUNT]);
      short_cur_stats[36] = atoi(row[CRITTBL_BHD_SIDES]);
      short_cur_stats[37] = atoi(row[CRITTBL_HP_REGEN]);
      short_cur_stats[38] = atoi(row[CRITTBL_MANA_REGEN]);
      short_cur_stats[39] = atoi(row[CRITTBL_MOV_REGEN]);
      short_cur_stats[40] = atoi(row[CRITTBL_GUILD]);
      short_cur_stats[41] = atoi(row[CRITTBL_WEIGHT]);

      cur_stats[0] = atoi(row[CRITTBL_VIS_BITS]);
      cur_stats[1] = atoi(row[CRITTBL_SEE_BITS]);
      cur_stats[2] = atoi(row[CRITTBL_IN_ROOM]);

      crit_flags.set(CRITFLAG_CAN_SEE_INV, atoi(row[CRITTBL_CAN_SEE_INVENTORY]));
      crit_flags.set(CRITFLAG_USING_LIGHT, atoi(row[CRITTBL_USING_LIGHT_SOURCE]));
      crit_flags.set(CRITFLAG_IS_FLYING, atoi(row[CRITTBL_IS_FLYING]));
      crit_flags.set(CRITFLAG_HAS_BOAT, atoi(row[CRITTBL_HAS_BOAT]));
      crit_flags.set(CRITFLAG_CAN_CLIMB, atoi(row[CRITTBL_CAN_CLIMB]));
      crit_flags.set(CRITFLAG_GOSSIPS, atoi(row[CRITTBL_GOSSIPS]));
      crit_flags.set(CRITFLAG_YELLS, atoi(row[CRITTBL_YELLS]));
      crit_flags.set(CRITFLAG_GRATZ, atoi(row[CRITTBL_GRATZ]));
      crit_flags.set(CRITFLAG_AUCTIONS, atoi(row[CRITTBL_AUCTIONS]));
      crit_flags.set(CRITFLAG_SHOUTS, atoi(row[CRITTBL_SHOUTS]));
      crit_flags.set(CRITFLAG_SAYS, atoi(row[CRITTBL_SAYS]));
      crit_flags.set(CRITFLAG_TELLS, atoi(row[CRITTBL_TELLS]));
      crit_flags.set(CRITFLAG_WIZNETS, atoi(row[CRITTBL_WIZNETS]));
      crit_flags.set(CRITFLAG_IS_PARALYZED, atoi(row[CRITTBL_IS_PARALYZED]));
      crit_flags.set(CRITFLAG_IS_PERM_SLEEPED, atoi(row[CRITTBL_IS_PERM_SLEEPED]));
      crit_flags.set(CRITFLAG_IS_DUAL_WIELDING, atoi(row[CRITTBL_IS_DUAL_WIELDING]));
      crit_flags.set(CRITFLAG_IS_SNEAK, atoi(row[CRITTBL_IS_SNEAKING]));
      crit_flags.set(CRITFLAG_IN_USE, atoi(row[CRITTBL_IN_USE]));
      crit_flags.set(CRITFLAG_CAN_DIVE, atoi(row[CRITTBL_CAN_DIVE]));
      crit_flags.set(CRITFLAG_IS_HIDE, atoi(row[CRITTBL_IS_HIDE]));

      if (short_cur_stats[26] == 0) {
         if (!pc) {
            pc = new pc_data;
         } // if
         pc->pc_data_flags.set(PCFLAG_FROZEN, atoi(row[CRITTBL_IS_FROZEN]));
         pc->pc_data_flags.set(PCFLAG_GAGGED, atoi(row[CRITTBL_IS_GAGGED]));
         pc->pc_data_flags.set(PCFLAG_HAS_IMM_DATA, atoi(row[CRITTBL_HAS_IMM_DATA]));
         pc->pc_data_flags.set(PCFLAG_CLOAKED, atoi(row[CRITTBL_CLOAKED]));
         pc->pc_data_flags.set(PCFLAG_TANK_GRAPH, atoi(row[CRITTBL_TANK_GRAPH]));
         pc->pc_data_flags.set(PCFLAG_USING_CLIENT, atoi(row[CRITTBL_USING_CLIENT]));
         pc->pc_data_flags.set(PCFLAG_AUTOEXIT, atoi(row[CRITTBL_AUTO_EXIT]));
         pc->pc_data_flags.set(PCFLAG_NO_HASSLE, atoi(row[CRITTBL_NO_HASSLE]));
         pc->pc_data_flags.set(PCLFAG_BRIEF, atoi(row[CRITTBL_BRIEF]));
         pc->pc_data_flags.set(PCFLAG_AUTOSPLIT, atoi(row[CRITTBL_AUTOSPLIT]));
         pc->pc_data_flags.set(PCFLAG_IS_BUILDER, atoi(row[CRITTBL_IS_BUILDER]));
         pc->pc_data_flags.set(PCFLAG_AUTOLOOT, atoi(row[CRITTBL_AUTOLOOT]));
         pc->pc_data_flags.set(PCFLAG_EXTRA_INFO, atoi(row[CRITTBL_EXTRA_INFO]));
         pc->pc_data_flags.set(PCFLAG_CR_BEHIND, atoi(row[CRITTBL_CR_BEHIND]));
         pc->pc_data_flags.set(PCFLAG_DO_CARRIAGE_RETURN, atoi(row[CRITTBL_DO_CARRIAGE_RETURN]));
         pc->pc_data_flags.set(PCFLAG_CAN_DET_MAGIC, atoi(row[CRITTBL_CAN_DETECT_MAGIC]));
         pc->pc_data_flags.set(PCFLAG_DETECT_INVENTORY, atoi(row[CRITTBL_DETECT_INVENTORY]));
         pc->pc_data_flags.set(PCFLAG_SHOW_VNUMS, atoi(row[CRITTBL_SHOW_VNUMS]));
         pc->pc_data_flags.set(PCFLAG_HAS_POOFIN_POOFOUT_MSG, atoi(row[CRITTBL_HAS_POOFIN_POOFOUT_MSG]));
         pc->pc_data_flags.set(PCFLAG_PAGE_OUTPUT, atoi(row[CRITTBL_PAGE_OUTPUT]));
         pc->pc_data_flags.set(PCFLAG_NO_WIZCHAT, atoi(row[CRITTBL_NO_WIZCHAT]));
         pc->pc_data_flags.set(PCFLAG_HAS_COLORS, atoi(row[CRITTBL_HAS_COLORS]));
         pc->pc_data_flags.set(PCFLAG_USE_COLOR, atoi(row[CRITTBL_USE_COLOR]));
         pc->pc_data_flags.set(PCFLAG_HAS_LANGUAGE_CHOICE, atoi(row[CRITTBL_HAS_LANGUAGE_CHOICE]));
         pc->pc_data_flags.set(PCFLAG_NO_SHOW_MOB_ENTRY, atoi(row[CRITTBL_NO_SHOW_NPC_ENTRY]));
         pc->pc_data_flags.set(PCFLAG_NO_BEEP, atoi(row[CRITTBL_NO_BEEP]));
         pc->pc_data_flags.set(PCFLAG_IS_REMORT, atoi(row[CRITTBL_IS_REMORT]));
         pc->pc_data_flags.set(PCFLAG_HAS_SACRIFICED, atoi(row[CRITTBL_HAS_SACRIFICED]));

         pc->password = row[CRITTBL_PASSWORD];
         pc->birth_day = atoi(row[CRITTBL_BIRTH_DAY]);
         pc->birth_year = atoi(row[CRITTBL_BIRTH_YEAR]);
         pc->rent_day = atoi(row[CRITTBL_RENT_DAY]);
         pc->rent_year = atoi(row[CRITTBL_RENT_YEAR]);
         pc->last_login_time = atoi(row[CRITTBL_LAST_LOGIN_TIME]);
         pc->total_time_online = atoi(row[CRITTBL_TOTAL_TIME_ONLINE]);
         pc->age = atoi(row[CRITTBL_AGE]);
         pc->hunger = atoi(row[CRITTBL_HUNGER]);
         pc->thirst = atoi(row[CRITTBL_THIRST]);
         pc->drugged = atoi(row[CRITTBL_DRUGGEDNESS]);
         pc->pk_count = atoi(row[CRITTBL_PKILLS]);
         pc->died_count = atoi(row[CRITTBL_DEATHS]);
         pc->quest_points = atoi(row[CRITTBL_QUEST_POINTS]);
         pc->lines_on_page = atoi(row[CRITTBL_LINES_ON_PAGE]);
         pc->prompt = row[CRITTBL_PROMPT];
         if (pc->pc_data_flags.get(PCFLAG_HAS_POOFIN_POOFOUT_MSG)) {
            pc->poofin = row[CRITTBL_POOFIN_MSG];
            pc->poofout = row[CRITTBL_POOFOUT_MSG];
         } // if

         if (pc->pc_data_flags.get(PCFLAG_IS_BUILDER))
            pc->pc_data_flags.turn_on(PCFLAG_HAS_IMM_DATA);

         if (pc->pc_data_flags.get(PCFLAG_HAS_IMM_DATA)) {
            pc->imm_data = new immort_data;
            pc->imm_data->imm_level = atoi(row[CRITTBL_IMM_LEVEL]);
         } // if

         if (pc->pc_data_flags.get(PCFLAG_HAS_COLORS)) {
            pc->gos_str = row[CRITTBL_GOS_COLOR];
            pc->say_str = row[CRITTBL_SAY_COLOR];
            pc->yell_str = row[CRITTBL_YELL_COLOR];
            pc->tell_str = row[CRITTBL_TELL_COLOR];
            pc->desc_str = row[CRITTBL_DESC_COLOR];
            pc->obj_list_str = row[CRITTBL_OBJ_LIST_COLOR];
            pc->mob_list_str = row[CRITTBL_MOB_LIST_COLOR];
            pc->dflt_str = row[CRITTBL_DEFAULT_COLOR];
            pc->bk_str = row[CRITTBL_BACKGROUND_COLOR];
            pc->battle_str = row[CRITTBL_BATTLE_COLOR];
            pc->user1_str = row[CRITTBL_USER1_COLOR];
            pc->user2_str = row[CRITTBL_USER2_COLOR];
            pc->user3_str = row[CRITTBL_USER3_COLOR];
            pc->room_str = row[CRITTBL_ROOM_NAME_COLOR];
         } // if

         if (pc->pc_data_flags.get(PCFLAG_HAS_LANGUAGE_CHOICE))
            pc->preferred_language = (LanguageE)(atoi(row[CRITTBL_PREFERRED_LANGUAGE]));
      } // if
      else {
         if (!mob)
            mob = new mob_data;

         mob->mob_num = mob_num;
         mob->setMaxInGame(atoi(row[CRITTBL_MAX_IN_GAME]));
         mob->mob_data_flags.set(MOBFLAG_HAS_PROC_DATA, atoi(row[CRITTBL_HAS_PROC_DATA]));
         mob->mob_data_flags.set(MOBFLAG_SCAVENGE, atoi(row[CRITTBL_SCAVENGE]));
         mob->mob_data_flags.set(MOBFLAG_WANDER, atoi(row[CRITTBL_WANDER]));
         mob->mob_data_flags.set(MOBFLAG_SHOULD_DO_PROCS, atoi(row[CRITTBL_SHOULD_DO_PROCS]));
         mob->mob_data_flags.set(MOBFLAG_EDIBLE_CORPSE, atoi(row[CRITTBL_EDIBLE]));
         mob->mob_data_flags.set(MOBFLAG_IS_BANKER, atoi(row[CRITTBL_IS_BANKER]));
         mob->mob_data_flags.set(MOBFLAG_SESSILE, atoi(row[CRITTBL_IS_SESSILE]));
         mob->mob_data_flags.set(MOBFLAG_NOT_HOMING, atoi(row[CRITTBL_NO_HOMING]));
         mob->mob_data_flags.set(MOBFLAG_DISOLVABLE, atoi(row[CRITTBL_DISOLVABLE]));
         mob->mob_data_flags.set(MOBFLAG_HAS_SKIN, atoi(row[CRITTBL_HAS_SKIN]));
         mob->mob_data_flags.set(MOBFLAG_HAS_MOB_SCRIPT, atoi(row[CRITTBL_HAS_MOB_SCRIPTS]));
         if (mob->mob_data_flags.get(CRITTBL_HAS_PROC_DATA)) {
            mob->proc_data = new spec_data;
            mob->proc_data->flag1.set(SPECFLAG_SHOPKEEPER,
                  atoi(row[CRITTBL_IS_SHOPKEEPER]));
            mob->proc_data->flag1.set(SPECFLAG_TEACHER,
                  atoi(row[CRITTBL_IS_TEACHER]));
            mob->proc_data->flag1.set(SPECFLAG_LET_SAME_CLASS_PASS,
                  atoi(row[CRITTBL_LET_SAME_CLASS_PASS]));
            mob->proc_data->flag1.set(SPECFLAG_LET_SAME_RACE_PASS,
                  atoi(row[CRITTBL_LET_SAME_RACE_PASS]));
            mob->proc_data->flag1.set(SPECFLAG_HAS_MOB_GIVE_PROC,
                  atoi(row[CRITTBL_HAS_MOB_GIV_PROC]));
            mob->proc_data->flag1.set(SPECFLAG_HAS_MOB_SAY_PROC,
                  atoi(row[CRITTBL_HAS_MOB_SAY_PROC]));
            mob->proc_data->flag1.set(SPECFLAG_HAS_MOB_BOW_PROC,
                  atoi(row[CRITTBL_HAS_MOB_BOW_PROC]));
            mob->proc_data->flag1.set(SPECFLAG_HAS_MOB_CURSE_PROC,
                  atoi(row[CRITTBL_HAS_MOB_CURSE_PROC]));
            mob->proc_data->flag1.set(SPECFLAG_PROC_WITH_SAME_RACE,
                  atoi(row[CRITTBL_PROC_WITH_SAME_RACE]));
            mob->proc_data->flag1.set(SPECFLAG_PROC_WITH_SAME_ALIGN,
                  atoi(row[CRITTBL_PROC_WITH_SAME_ALIGN]));
            mob->proc_data->flag1.set(SPECFLAG_PROC_WITH_SAME_CLASS,
                  atoi(row[CRITTBL_PROC_WITH_SAME_CLASS]));
            mob->proc_data->flag1.set(SPECFLAG_HAS_AI,
                  atoi(row[CRITTBL_HAS_AI]));

            if (mob->proc_data->flag1.get(SPECFLAG_SHOPKEEPER)) {
               mob->proc_data->sh_data = new shop_data;
               mob->proc_data->sh_data->markup = atoi(row[CRITTBL_MARKUP]);
               mob->proc_data->sh_data->buy_percentage = atoi(row[CRITTBL_BUY_PERCENTAGE]);
               mob->proc_data->sh_data->open_time = atoi(row[CRITTBL_OPEN_TIME]);
               mob->proc_data->sh_data->close_time = atoi(row[CRITTBL_CLOSE_TIME]);

               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_BUY0,
                  atoi(row[CRITTBL_BUY_0]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_SELL0,
                  atoi(row[CRITTBL_SELL_0]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_OFFER0,
                  atoi(row[CRITTBL_OFFER_0]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_PLAYER_OWNED,
                  atoi(row[CRITTBL_IS_PLAYER_OWNED]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_NON_WEAPON,
                  atoi(row[CRITTBL_BUYS_NON_WEAPONS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_SLASH,
                  atoi(row[CRITTBL_BUYS_SLASHING_WEAPONS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_SMASH,
                  atoi(row[CRITTBL_BUYS_SMASHING_WEAPONS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_PIERCE,
                  atoi(row[CRITTBL_BUYS_PIERCING_WEAPONS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_WHIP,
                  atoi(row[CRITTBL_BUYS_WHIPS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_NEEDS_AMMO,
                  atoi(row[CRITTBL_BUYS_AMMO_NEEDING_WEAPONS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_DART_THROWER,
                  atoi(row[CRITTBL_BUYS_DART_THROWERS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_BOW,
                  atoi(row[CRITTBL_BUYS_BOWS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_DART,
                  atoi(row[CRITTBL_BUYS_DARTS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_ARROW,
                  atoi(row[CRITTBL_BUYS_ARROWS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_JUNK,
                  atoi(row[CRITTBL_BUYS_JUNK]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_WAND,
                  atoi(row[CRITTBL_BUYS_WANDS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_POTION,
                  atoi(row[CRITTBL_BUYS_POTIONS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_SCROLL,
                  atoi(row[CRITTBL_BUYS_SCROLLS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_CONTAINER,
                  atoi(row[CRITTBL_BUYS_CONTAINERS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_COINS,
                  atoi(row[CRITTBL_BUYS_COINS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_ARMOR,
                  atoi(row[CRITTBL_BUYS_ARMOR]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_WEAPON,
                  atoi(row[CRITTBL_BUYS_WEAPONS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_LIGHT_SOURCE,
                  atoi(row[CRITTBL_BUYS_LIGHTS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_CANTEEN,
                  atoi(row[CRITTBL_BUYS_CANTEENS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_LIQUID,
                  atoi(row[CRITTBL_BUYS_LIQUIDS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_FOOD,
                  atoi(row[CRITTBL_BUYS_FOOD]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_BOAT,
                  atoi(row[CRITTBL_BUYS_BOATS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_TOOLBOX,
                  atoi(row[CRITTBL_BUYS_TOOLBOXES]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_CAULDRON,
                  atoi(row[CRITTBL_BUYS_CAULDRENS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_PEN,
                  atoi(row[CRITTBL_BUYS_PENS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_CONSTRUCT_COMPONENT,
                  atoi(row[CRITTBL_BUYS_CONSTRUCT_COMPONENTS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_CONCOCT_COMPONENT,
                  atoi(row[CRITTBL_BUYS_CONCOCT_COMPONENTS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_PARCHMENT,
                  atoi(row[CRITTBL_BUYS_PARCHMENTS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_HERB,
                  atoi(row[CRITTBL_BUYS_HERBS]));
               mob->proc_data->sh_data->shop_data_flags.set(
                  SHOPFLAG_VEND_MACHINE,
                  atoi(row[CRITTBL_BUYS_VENDING_MACHINES]));
            } // if

            if (mob->proc_data->flag1.get(SPECFLAG_TEACHER)) {
               mob->proc_data->teach_data = new teacher_data;
               mob->proc_data->teach_data->teach_data_flags.set(1,
                  CRITTBL_TEACHES_WARRIORS);
               mob->proc_data->teach_data->teach_data_flags.set(2,
                  CRITTBL_TEACHES_SAGES);
               mob->proc_data->teach_data->teach_data_flags.set(3,
                  CRITTBL_TEACHES_WIZARDS);
               mob->proc_data->teach_data->teach_data_flags.set(4,
                  CRITTBL_TEACHES_RANGERS);
               mob->proc_data->teach_data->teach_data_flags.set(5,
                  CRITTBL_TEACHES_THIEVES);
               mob->proc_data->teach_data->teach_data_flags.set(6,
                  CRITTBL_TEACHES_ALCHEMISTS);
               mob->proc_data->teach_data->teach_data_flags.set(7,
                  CRITTBL_TEACHES_CLERICS);
               mob->proc_data->teach_data->teach_data_flags.set(8,
                  CRITTBL_TEACHES_BARDS);
            } // if

            if (mob->proc_data->flag1.get(SPECFLAG_HAS_AI)) {
               mob->proc_data->setSkillViolence(
                  atoi(row[CRITTBL_SKILL_VIOLENCE]));
               mob->proc_data->setBenevolence(atoi(row[CRITTBL_BENEVOLENCE]));
               mob->proc_data->setDefensiveness(
                  atoi(row[CRITTBL_DEFENSIVENESS]));
               mob->proc_data->setBadAssedness(
                  atoi(row[CRITTBL_BAD_ASSEDNESS]));
               mob->proc_data->setSocialAwareness(
                  atoi(row[CRITTBL_SOCIAL_AWARENESS]));
            } // if
         } // if

         if (mob->mob_data_flags.get(MOBFLAG_HAS_SKIN))
            mob->skin_num = atoi(row[CRITTBL_SKIN]);
      } // else

      mysql_free_result(result);
   } // if
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In critter::dbRead(int, int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else

   // names
   query="select NAME from CritNames where MOB_NUMBER=";
   query+=mob_num;
   query+=" and PC_NUMBER=";
   query+=pc_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In critter::dbRead(int, int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         } // if
         return;
      } // if
      while ((row=mysql_fetch_row(result))) {
         names.append(new String(row[0]));
      } // while
      mysql_free_result(result);
   } // if
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In critter::dbRead(int, int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else

   // equipment
   query = "select POSITION, OBJ_NUM, SOBJ_NUM from CritEq where MOB_NUMBER=";
   query+=mob_num;
   query+=" and PC_NUMBER=";
   query+=pc_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In critter::dbRead(int, int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         } // if
         return;
      } // if

      int obj_num, sobj_num, pos;
      while ((row=mysql_fetch_row(result))) {
         pos = atoi(row[0]);
         obj_num = atoi(row[1]);
         sobj_num = atoi(row[2]);

         if (check_l_range(pos, 1, MAX_EQ, mob_list[0], FALSE)) {
            if (sobj_num != 0) {
               object* new_obj = new object;
               new_obj->dbRead(obj_num, sobj_num, read_all);
               new_obj->IN_LIST = &(inv);
               eq[pos] = new_obj;
            } // if
            else {
               if (obj_list[obj_num].isInUse()) {
                  if (read_all || ((obj_list[obj_num].OBJ_PRCNT_LOAD *
                      config.currentLoadModifier)/100) > d(1,100)) {
                     eq[pos]=&obj_list[obj_num];
                  } // if
               } // if
            } // else
         } // if
      } // while
      mysql_free_result(result);
   } // if
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In critter::dbRead(int, int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else
   // affected by
   query="select SPELL_NUMBER, DURATION from CritSpells where MOB_NUMBER=";
   query+=mob_num;
   query+=" and PC_NUM=";
   query+=pc_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In critter::dbRead(int, int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         } // if
         return;
      } // if
      stat_spell_cell* ss_ptr;
      while ((row=mysql_fetch_row(result))) {
         ss_ptr = new stat_spell_cell;
         ss_ptr->stat_spell = atoi(row[0]);
         ss_ptr->bonus_duration = atoi(row[1]);
         affected_by.append(ss_ptr);
      } // while
      mysql_free_result(result);
   } // if
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In critter::dbRead(int, int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else

   // inventory
   query="select OBJ_NUM, SOBJ_NUM, QUANTITY from CritInv where MOB_NUMBER=";
   query+=mob_num;
   query+=" and PC_NUMBER=";
   query+=pc_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In critter::dbRead(int, int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         } // if
         return;
      } // if
      int quantity, obj_num, sobj_num;
      while ((row=mysql_fetch_row(result))) {
         obj_num = atoi(row[0]);
         sobj_num = atoi(row[1]);
         quantity = atoi(row[2]);
         for (;quantity > 0; quantity--) {
            if (sobj_num != 0) {
               object* new_obj = new object;
               new_obj->dbRead(obj_num, sobj_num, read_all);
               new_obj->IN_LIST = &inv;
               inv.append(new_obj);
               affected_objects.append(new_obj);
            } // if
            else {
               if (obj_list[obj_num].OBJ_FLAGS.get(OBJFLAG_IN_USE)) {
                  if (read_all || ((obj_list[obj_num].OBJ_PRCNT_LOAD *
                      config.currentLoadModifier)/100) > d(1, 100)) {
                     inv.append(&obj_list[obj_num]);
                  } // if
               } // if
               else {
                  if (mudlog.ofLevel(ERROR)) {
                     mudlog << "ERROR: trying to load non-existant object: "
                            << obj_num << " in critter named: " << *(getName())
                            << endl;
                  } // if
               } // else
            } // else
         } // for
      } // while
      mysql_free_result(result);
   } // if
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In critter::dbRead(int, int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else
   // known skills (pc's only)
   if (pc) {
      String query="select SKILL_NUMBER, PERCENT_LEARNED from CritLrnd where PC_NUM=";
      query+=pc_num;

      if (mysql_real_query(database, query, strlen(query))==0) {
         if ((result=mysql_store_result(database))==NULL) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In critter::dbRead(int, int, short):\n";
               mudlog << "Error retrieving query results: "
                      << mysql_error(database) << endl;
            } // if
            return;
         } // if
         while((row=mysql_fetch_row(result))) {
            pc->skills_spells_known.Insert(atoi(row[0]), atoi(row[1]));
         } // while
         mysql_free_result(result);
      } // if
      else {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In critter::dbRead(int, int, short):\n";
            mudlog << "Error executing query: " << mysql_error(database) << endl;
         } // if
         return;
      } // else
   } // if

   // procs (mobs only)
   if (mob && mob->proc_data) {
      if (mob->proc_data->flag1.get(SPECFLAG_HAS_MOB_GIVE_PROC)) {
         mob->proc_data->give_proc = new action_proc_data;
         query="select * from CritProcs where MOB_NUMBER=";
         query+=mob_num;
         query+=" and PROC_TYPE=";
         query+=PROCTYPE_GIVE;

         if (mysql_real_query(database, query, strlen(query))==0) {
            if ((result=mysql_store_result(database))==NULL) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "In critter::dbRead(int, int, short):\n";
                  mudlog << "Error retrieving query results: "
                         << mysql_error(database) << endl;
               } // if
               return;
            } // if
            row=mysql_fetch_row(result);
            mob->proc_data->give_proc->test_num = atoi(row[CRPTBL_TEST_ONUM]);
            mob->proc_data->give_proc->correct_msg = row[CRPTBL_CORRECT_MSG];
            mob->proc_data->give_proc->skill_name = row[CRPTBL_SKILL_NAME];
            mob->proc_data->give_proc->trans_to_room = atoi(row[CRPTBL_TRANS_ROOM]);
            mob->proc_data->give_proc->wrong_gift_msg = row[CRPTBL_INCORRECT_MSG];
            mysql_free_result(result);
         } // if
         else {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In critter::dbRead(int, int, short):\n";
               mudlog << "Error executing query: " << mysql_error(database)
                      << endl;
            } // if
            return;
         } // else
      } // if

      if (mob->proc_data->flag1.get(SPECFLAG_HAS_MOB_BOW_PROC)) {
         mob->proc_data->bow_proc = new action_proc_data;
         query="select * from CritProcs where MOB_NUMBER=";
         query+=mob_num;
         query+=" and PROC_TYPE=";
         query+=PROCTYPE_BOW;

         if (mysql_real_query(database, query, strlen(query))==0) {
            if ((result=mysql_store_result(database))==NULL) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "In critter::dbRead(int, int, short):\n";
                  mudlog << "Error retrieving query results: "
                         << mysql_error(database) << endl;
               } // if
               return;
            } // if
            row=mysql_fetch_row(result);
            mob->proc_data->bow_proc->test_num = atoi(row[CRPTBL_TEST_ONUM]);
            mob->proc_data->bow_proc->correct_msg = row[CRPTBL_CORRECT_MSG];
            mob->proc_data->bow_proc->skill_name = row[CRPTBL_SKILL_NAME];
            mob->proc_data->bow_proc->trans_to_room = atoi(row[CRPTBL_TRANS_ROOM]);
            mob->proc_data->bow_proc->wrong_gift_msg = row[CRPTBL_INCORRECT_MSG];
            mysql_free_result(result);
         } // if
         else {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In critter::dbRead(int, int, short):\n";
               mudlog << "Error executing query: " << mysql_error(database)
                      << endl;
            } // if
            return;
         } // else
      } // if

      if (mob->proc_data->flag1.get(SPECFLAG_HAS_MOB_CURSE_PROC)) {
         mob->proc_data->curse_proc = new action_proc_data;
         query="select * from CritProcs where MOB_NUMBER=";
         query+=mob_num;
         query+=" and PROC_TYPE=";
         query+=PROCTYPE_CURSE;

         if (mysql_real_query(database, query, strlen(query))==0) {
            if ((result=mysql_store_result(database))==NULL) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "In critter::dbRead(int, int, short):\n";
                  mudlog << "Error retrieving query results: "
                         << mysql_error(database) << endl;
               } // if
               return;
            } // if
            row=mysql_fetch_row(result);
            mob->proc_data->curse_proc->test_num = atoi(row[CRPTBL_TEST_ONUM]);
            mob->proc_data->curse_proc->correct_msg = row[CRPTBL_CORRECT_MSG];
            mob->proc_data->curse_proc->skill_name = row[CRPTBL_SKILL_NAME];
            mob->proc_data->curse_proc->trans_to_room = atoi(row[CRPTBL_TRANS_ROOM]);
            mob->proc_data->curse_proc->wrong_gift_msg = row[CRPTBL_INCORRECT_MSG];
            mysql_free_result(result);
         } // if
         else {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In critter::dbRead(int, int, short):\n";
               mudlog << "Error executing query: " << mysql_error(database)
                      << endl;
            } // if
            return;
         } // else
      } // if

      if (mob->proc_data->flag1.get(SPECFLAG_HAS_MOB_SAY_PROC)) {
         // say proc
         say_proc_cell* ptr;
         query="select * from CritProcs where MOB_NUMBER=";
         query+=mob_num;
         query+=" and PROC_TYPE=";
         query+=PROCTYPE_SAY;

         if (mysql_real_query(database, query, strlen(query))==0) {
            if ((result=mysql_store_result(database))==NULL) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "In critter::dbRead(int, int, short):\n";
                  mudlog << "Error retrieving query results: "
                         << mysql_error(database) << endl;
               } // if
               return;
            } // if
            while ((row=mysql_fetch_row(result))) {
               ptr = new say_proc_cell;
               ptr->topic = row[CRPTBL_PROC_TOPIC];
               ptr->msg = row[CRPTBL_CORRECT_MSG];
               ptr->skill_name = row[CRPTBL_SKILL_NAME];
               ptr->obj_num = atoi(row[CRPTBL_TEST_ONUM]);
               ptr->trans_to_room = atoi(row[CRPTBL_TRANS_ROOM]);
               mob->proc_data->topics.append(ptr);
            } // while
            mysql_free_result(result);
         } // if
         else {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In critter::dbRead(int, int, short):\n";
               mudlog << "Error executing query: " << mysql_error(database)
                      << endl;
            } // if
            return;
         } // else
      } // if
      if (mob->proc_data->flag1.get(SPECFLAG_HAS_MOB_GIVE_PROC) ||
          mob->proc_data->flag1.get(SPECFLAG_HAS_MOB_SAY_PROC)  ||
          mob->proc_data->flag1.get(SPECFLAG_HAS_MOB_BOW_PROC)  ||
          mob->proc_data->flag1.get(SPECFLAG_HAS_MOB_CURSE_PROC)) {
         mob->proc_data->wrong_align_msg = wrong_a;
         mob->proc_data->wrong_class_msg = wrong_c;
         mob->proc_data->wrong_race_msg = wrong_r;
      } // if
      else {
         delete wrong_a;
         delete wrong_c;
         delete wrong_r;
      } // else

      // scripts
      if (mob->mob_data_flags.get(MOBFLAG_HAS_MOB_SCRIPT)) {
         query="select * from CritScripts where MOB_NUMBER=";
         query+=mob_num;

         if (mysql_real_query(database, query, strlen(query))==0) {
            if ((result=mysql_store_result(database))==NULL) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "In critter::dbRead(int, int, short):\n";
                  mudlog << "Error retrieving query results: "
                         << mysql_error(database) << endl;
               } // if
               return;
            } // if
            MobScript* ptr;
            while ((row=mysql_fetch_row(result))) {
               String* tmp_disc = new String(row[CRSTBL_DISCRIMINATOR]);
               tmp_disc->Strip();
               tmp_disc->Prepend(" ");
               tmp_disc->Append(" ");

               ptr = new MobScript(*(new String[CRSTBL_TRIGGER_CMD]),
                     atoi(row[CRSTBL_TARGET]),
                     atoi(row[CRSTBL_ACTOR]),
                     *tmp_disc,
                     atoi(row[CRSTBL_PRECEDENCE]));

               String sbuf, *tmp_str;
               sbuf=row[CRSTBL_SCRIPT_COMMANDS];
               while ((tmp_str = sbuf.getUntil(';'))) {
                  ptr->appendCmd(*tmp_str);
                  delete tmp_str;
               } // while
               mob->mob_proc_scripts.append(ptr);
               ptr->compile();
            } // while
            mysql_free_result(result);
         } // if
         else {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In critter::dbRead(int, int, short):\n";
               mudlog << "Error executing query: " << mysql_error(database) << endl;
            } // if
            return;
         } // else
      } // if
   } // if

   // shop inventory
   if (mob && mob->proc_data && mob->proc_data->sh_data) {
      query="select OBJECT_NUMBER, SOBJ_NUMBER, QUANTITY from CritPermInv where MOB_NUMBER=";
      query+=mob_num;

      if (mysql_real_query(database, query, strlen(query))==0) {
         if ((result=mysql_store_result(database))==NULL) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In critter::dbRead(int, int, short):\n";
               mudlog << "Error retrieving query results: "
                      << mysql_error(database) << endl;
            } // if
            return;
         } // if
         int quantity, obj_num, sobj_num;
         while ((row=mysql_fetch_row(result))) {
            obj_num = atoi(row[0]);
            sobj_num = atoi(row[1]);
            quantity = atoi(row[2]);
            for (;quantity>0;quantity--) {
               if (sobj_num != 0) {
                  object* new_obj = new object;
                  new_obj->dbRead(obj_num, sobj_num, read_all);
                  new_obj->IN_LIST = &mob->proc_data->sh_data->perm_inv;
                  mob->proc_data->sh_data->perm_inv.append(new_obj);
               } // if
               else {
                  if (obj_list[obj_num].OBJ_FLAGS.get(OBJFLAG_IN_USE)) {
                     if (read_all || ((obj_list[obj_num].OBJ_PRCNT_LOAD*
                         config.currentLoadModifier)/100) > d(1,100)) {
                        mob->proc_data->sh_data->perm_inv.append(&obj_list[obj_num]);
                     } // if
                  } // if
                  else {
                     if (mudlog.ofLevel(ERROR)) {
                        mudlog << "ERROR: Trying to load non-existant object: "
                               << obj_num << " on shopkeeper " << mob_num;
                     } // if
                  } // else
               } // else
            } // for
         } // while
         mysql_free_result(result);
      } // if
      else {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In critter::dbRead(int, int, short):\n";
            mudlog << "Error executing query: " << mysql_error(database)
                   << endl;
         } // if
         return;
      } // else
   } // if
   // player shop inventory
   if (mob && mob->proc_data && mob->proc_data->sh_data &&
   mob->proc_data->sh_data->isPlayerRun()) {
      query="select OBJECT_NUMBER, SELL_AT, BUY_AT where MOB_NUMBER=";
      query+=mob_num;

      if (mysql_real_query(database, query, strlen(query))==0) {
         if ((result=mysql_store_result(database))==NULL) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In critter::dbRead(int, int, short):\n";
               mudlog << "Error retrieving query results: "
                      << mysql_error(database) << endl;
            } // if
            return;
         } // if
         PlayerShopData* ps_data;
         while ((row=mysql_fetch_row(result))) {
            ps_data = new PlayerShopData(atoi(row[0]), atoi(row[1]),
               atoi(row[2]));
            mob->proc_data->sh_data->ps_data_list.pushBack(ps_data);
         } // while
         mysql_free_result(result);
      } // if
      else {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In critter::dbRead(int, int, short):\n";
            mudlog << "Error executing query: " << mysql_error(database)
                   << endl;
         } // if
         return;
      } // else
   } // if

   if (!is_affected_by(BLINDNESS_SKILL_NUM, *this)) {
      SEE_BIT |= 1024;
   } // if
} // critter::dbRead()
#endif

void critter::fileRead(ifstream& ofile, short read_all) {
   int i, test = TRUE, z;
   stat_spell_cell* ss_ptr;
   char tmp[81];
   String tmp_str(80);
   String* string;
   int format_version;

   Clear();

   if (!ofile) {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  da_file FALSE in crit read." << endl;
      }
      return;
   }

   // look for a file-version identifier
   {
      char tst_char;
      ofile.get(tst_char);
      if ( tst_char != '%' ) {
         // no file version 
         ofile.putback(tst_char);
         format_version = 0;
      } else {
         ofile >> format_version;
         ofile.getline(tmp, 80);
      }
   }

   test = TRUE;
   while (test) {
      if (!ofile) {
         if (mudlog.ofLevel(ERROR)) {
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
         names.append(string);
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
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  da_file FALSE in crit read." << endl;
         }
         return;
      }

      ofile >> z;
      if (!check_l_range(z, 1, MAX_EQ, mob_list[0], FALSE)) {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  wear_posn out of range, crit.Read():  " << z
                   << " short desc:  " << short_desc << endl;
         }
         ofile >> i;
         continue;
      }//if
      if (i == -2) { //gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->fileRead(ofile, TRUE);
         new_obj->IN_LIST = &(inv); //make sure its a SOBJ

         eq[z] = new_obj;      //now it wears it
         affected_objects.append(new_obj);
 
      }//if
      else {
         if (obj_list[i].isInUse()) {  //exists?
            if (read_all || 
                     ((obj_list[i].OBJ_PRCNT_LOAD * config.currentLoadModifier)/
                        100) > d(1,100)) {
               eq[z] = &(obj_list[i]);      //now it wears it
            }//if
         }//
         else {
            Sprintf(tmp_str, 
                    "ERROR:  trying to load non-existant obj: %i, "
              "in critter's: %S  eq.\n", i, &short_desc);
            mudlog.log(ERROR, tmp_str);
         }//else
      }//else
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);

                /*  Affected By */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  da_file FALSE in crit read." << endl;
         }
         return;
      }

      ss_ptr = new stat_spell_cell;
      ss_ptr->stat_spell = i;
      ofile >> ss_ptr->bonus_duration;
      if ( format_version > 0 ) {
         ofile >> ss_ptr->bonus_value;
      }

      affected_by.append(ss_ptr);
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);

   if ( format_version > 0 ) {
      /* skills spells known */
      ofile >> i;
      int tmp_int;
      while (i != -1) {
         if (!ofile) {
            if (mudlog.ofLevel(ERROR)) {
               mudlog << "ERROR:  da_file FALSE in pc_data read." << endl;
            }
            return;
         }
         ofile >> tmp_int;
         skills_spells_known.Insert(i, tmp_int);
         ofile >> i;
      }//while
      ofile.getline(tmp, 80);
   }//format_version > 0

                /*  Inventory */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  da_file FALSE in crit read." << endl;
         }
         return;
      }

      if (i == -2) { //gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->fileRead(ofile, TRUE);
         new_obj->IN_LIST = &(inv); //make sure its a SOBJ

         inv.append(new_obj); // add it to inventory
         affected_objects.append(new_obj);

      }//if
      else {
         if (obj_list[i].OBJ_FLAGS.get(10)) {
            if (read_all ||
              ((obj_list[i].OBJ_PRCNT_LOAD * config.currentLoadModifier)/100) > 
                      d(1,100)) {
               inv.append(&(obj_list[i])); //add it to inventory
            }//if
         }//if
         else {
            if (mudlog.ofLevel(ERROR)) {
               mudlog << "ERROR: trying to load non-existant object: " << i
                      << " in critter named: " << *(getName()) << endl;
            }
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
      pc->Read(this, ofile);  

      pc->file_format_version = format_version;
   }//if
   else { //its a mob
      if (!(mob)) {
         mob = new mob_data;
      }//if
      mob->Read(ofile, read_all, format_version); 
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


int critter::shouldBeHoming() {
   return (mob && isSmob() && !mob->isNoHoming() && !isTracking() &&
           (getHomeRoom() != getCurRoomNum()));
}


int critter::canBeHurled() {
   if (!isPc()) {
      return !(CRIT_FLAGS.get(25));
   }
   return TRUE; //Pc's can always be hurled.
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
   if (!pc) return;
   PC_FLAGS.turn_off(5); 
   setClient(NO_CLIENT);   
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
// that targ is also *this.  Looks through all the objects the critter
// is wearing.
void critter::checkForProc(String& cmd, String& arg1, critter& actor,
                           int targ, room& rm) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In critter::checkForProc, this#: " << getIdNum()
             << " actor#: " << actor.getIdNum() << " cmd -:"
             << cmd << ":- arg1 -:" << arg1 << ":-  targ: "
             << targ << " room#: " << rm.getIdNum() << endl;
   }
   String buf(100);

   // Charmed mobs shouldn't even check for their scripts. At least for now
   if ( isCharmed() ) {
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
            if (mob->pending_scripts.size() >= 10) { //only queue 10 scripts
               break; //do nothing, don't want to get too much backlog.
            }
            else {
               // add it to the pending scripts.
               ptr->generateScript(cmd, arg1, actor, targ, rm, this);
               insertNewScript(new MobScript(*ptr));

               if (mob->cur_script) {
                  if (mob->cur_script->getPrecedence() <
                      mob->pending_scripts.peekFront()->getPrecedence()) {
                  
                     mob->pending_scripts.loseData(mob->cur_script); //take it out of queue
                     delete mob->cur_script; //junk it!
                     mob->cur_script = mob->pending_scripts.peekFront();
                     mob->cur_script->resetStackPtr(); //get ready to start
                  }//if
                  if (!proc_action_mobs.haveData(this)) {
                     int idx = proc_action_mobs.gainData(this);
                     mudlog << "ERROR: had to add mob to proc_action_mobs....but"
                            << " should have already been in there, idx: " << idx
                            << " me: " << *(getName()) << endl;
                     Sprintf(buf, "ERROR: Just had to add you to the proc_action_mobs, idx: %i\n",
                             idx);
                     this->show(buf);
                  }
                  // else, it just goes into the queue
               }//if we currently have a script.
               else { //there was none, so grab the first one.
                  mob->cur_script = mob->pending_scripts.peekFront();
                  int idx = proc_action_mobs.gainData(this);
                  Sprintf(buf, "Just added you to the proc_action_mobs, idx: %i\n",
                          idx);
                  this->show(buf);
                  mob->cur_script->resetStackPtr(); //get ready to start
               }

               break;
            }//else
         }//if matches
      }//while
   }//if

   // Look through all objects the person is using.
   for (int i = 1; i < MAX_EQ; i++) {
      if (mudlog.ofLevel(DBG2)) {
         mudlog << "Critter [" << getIdNum() << "] " << *(getName())
                << ":  Checking EQ[" << i << "] == " << EQ[i] << ", in rm: "
                << rm.getIdNum() << endl;
      }
      if (EQ[i] && (EQ[i]->hasScript())) {
         if (mudlog.ofLevel(DBG2)) 
            mudlog << "Found an object with a script: EQ[" << i << "]\n";
         // make it modified, if it is not already so.
         if (!(EQ[i]->isModified())) {
            EQ[i] = obj_to_sobj(*(EQ[i]), &inv, rm.getIdNum());
         }
         EQ[i]->checkForProc(cmd, arg1, actor, targ, rm);
      }//if
   }//for

   // Look through all inventory (recursively)

   Cell<object*> cll(inv);
   object* ptr;
   while ((ptr = cll.next())) {
      if (ptr->hasScript()) {
         if (!ptr->isModified()) {
            object* tmp = ptr;
            ptr = obj_to_sobj(*ptr, &inv, getCurRoomNum());
            if (!inv.substituteData(tmp, ptr, 1)) {
               mudlog.log(ERROR, "ERROR: critter::checkForProc: substituteData  failed after obj_to_sobj.\n");
            }//if
         }
         ptr->checkForProc(cmd, arg1, actor, targ, rm);
      }//if
   }//while

}//checkForProcAction


int critter::insertNewScript(MobScript* script) {

   if (!mob)
      return -1;

   // Don't append scripts that have a zero precedence, if there
   // are other scripts in the queue.
   if ((script->getPrecedence() == 0) && (!mob->pending_scripts.isEmpty())) {
      delete script;
      return 0;
   }

   MobScript* ptr;
   Cell<MobScript*> cll(mob->pending_scripts);

   while ((ptr = cll.next())) {
      if (ptr->getPrecedence() < script->getPrecedence()) {
         // Then insert it
         mob->pending_scripts.insertBefore(cll, script);
         return 0;
      }//if
   }//while

   // If here, then we need to place it at the end.
   mob->pending_scripts.append(script);
   return 0;
}


void critter::trackToKill(critter& vict, int& is_dead) {
   if ((isMob()) || (vict.isMob())) {
      mudlog.log(ERROR, "ERROR:  MOB's in trackToKill.\n");
      return;
   }//f

   if (!mob) {
      mudlog.log(ERROR, "ERROR:  pc trying to track_to_kill.\n");
      return;
   }//if

   // Not going to track non-player characters, don't move if sessile
   if (vict.isNpc() || isSessile())
      return;

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
   critter* crit_ptr;
   while (!IS_FIGHTING.isEmpty()) {
      crit_ptr = IS_FIGHTING.popFront();
      crit_ptr->IS_FIGHTING.loseData(this);
   }//if
}//doRemoveFromBattle

void critter::doGoToRoom(int dest_room, const char* from_dir, door* by_door,
                        int& is_dead, int cur_room, int sanity, int do_msgs) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In doGoToRoom, dest_room:  " << dest_room
             << "  cur_room:  " << cur_room
             << "  by_door:  " << by_door << endl;
   }

   doRemoveFromBattle();

   leave_room_effects(room_list[cur_room], *this, TRUE);

   // If we are in earth-meld, break it.
   breakEarthMeld();

   room_list[cur_room].removeCritter(this);
   room_list[dest_room].gainCritter(this);

   // This can cause us to delete ourselves btw...not very good coding
   // but..maybe it will work!
   do_entered_room_procs(*this, by_door, from_dir, room_list[dest_room],
                         is_dead, sanity);
   if (!is_dead && do_msgs) {
      look(1, &NULL_STRING, *this, FALSE); //don't ignore brief
   }

}//doGoToRoom

stat_spell_cell* critter::isAffectedBy(int spell_num) {
   return is_affected_by(spell_num, *this);
}

void critter::breakEarthMeld() {
   stat_spell_cell* sp;
   if ((sp = is_affected_by(EARTHMELD_SKILL_NUM, *this))) {
      show("Your meld with the earth has been severed.\n");
      affected_by.loseData(sp);
      delete sp;
   }//if
}//breakEarthMeld

void critter::doDie(int do_msg, int& show_tags, int leave_corpse)  {

   if ( do_msg ) {
      emote("is dead!");
   }

   is_fighting.clear();

   if ( pc ) {
      pc->died_count++;
   }

   if ( leave_corpse ) {
      dead_crit_to_corpse(*this, show_tags);
   }

}//doDie

int critter::isNamed(const String& name) const {
   Cell<String*> char_cell(names);
   String *string;

   if (name.Strlen() == 0)
      return FALSE;

   int len = name.Strlen();
   while ((string = char_cell.next())) {
      if (strncasecmp(*string, name, len) == 0) {
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
   if (obj->IN_LIST) {
      obj->IN_LIST = &inv;
      obj->setCurRoomNum(getCurRoomNum(), 0);
   }
}

void critter::setHP(int i) {
   if (i < 0)
      i = 0;
   if (i > 32000)
      i = 32000;
   HP = i;
}

void critter::setMana(int i) {
   if (i > 32000)
      i = 32000;
   MANA = i;
}

void critter::setMov(int i) {
   if (i > 32000)
      i = 32000;
   MOV = i;
}

void critter::show(const char* msg, hilite_type hl_type) {
   ::show(msg, *this, hl_type);
}

void critter::show(CSentryE which_string, hilite_type hl_type) {
   show(CSHandler::getString(which_string, getLanguageChoice()), hl_type);
}

LanguageE critter::getLanguageChoice() const {
   if (pc) {
      return pc->preferred_language;
   }
   return English;
}

void critter::setHP_MAX(int i) {
   if (i < 0)
      i = 0;
   if (i > 32000)
      i = 32000;
   short_cur_stats[23] = i; //MAX_HP of course...
}

void critter::setManaMax(int i) {
   if (i < 0)
      i = 0;
   if (i > 32000)
      i = 32000;
   short_cur_stats[24] = i;
}

void critter::setMovMax(int i) {
   if (i < 0)
      i = 0;
   if (i > 32000)
      i = 32000;
   short_cur_stats[25] = i;
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

   buf = *(names.peekFront());
   buf.Tolower();
   buf.Prepend("rm ./Pfiles/");
   system(buf);
   
   Sprintf(buf, "leaves this world forever by %s own hand!\n", 
           get_his_her(*this));
   ::emote(buf, *this, *(getCurRoom()), TRUE);

   setMode(MODE_QUIT_ME_PLEASE);
}//doSuicide


void critter::setMode(PcMode val) {
   mudlog << "Setting mode to:  " << (int)val << endl;
   pc->mode = val;
}


void critter::emote(const char* msg) {
   ::emote(msg, *this, room_list[getCurRoomNum()], TRUE);
}

void critter::pemote(const char* msg) {
   ::pemote(msg, *this, room_list[getCurRoomNum()], TRUE);
}

void critter::emote(CSelectorColl& includes, CSelectorColl& denies,
                    CSentryE cs_entry, .../*Sprintf args*/) {
   va_list argp;
   va_start(argp, cs_entry);
   getCurRoom()->vDoEmote(*this, includes, denies, cs_entry, argp);
   va_end(argp);
}//emote


void critter::doPrompt() {
   if (!pc) {
      return;
   }

   // for visible exits
   Cell<door*> cll(getCurRoom()->doors);
   door* dr_ptr;
   String buf;
   String reg_disp;
   int dest;

   unsigned int i;
   String targ(200);
   // Conversion buf for longs, NOTE: update append() method --Khaav
   String conv_buf(20); 
   unsigned int source_len = PROMPT_STRING.Strlen();

   if (! isAFK() ) {
      for (i = 0; i < source_len; ) {
         if (PROMPT_STRING[i] == '%') {
            i++;
            if (i >= source_len) {
               break;
            }
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
                  case 'a':     /* align */
                     targ.Append(ALIGN);
                     break;
                  case 'p':     /* pracs */
                     targ.Append(PRACS);
                     break;
                  case 'x':     /* xp */
                     Sprintf(conv_buf, "%i", EXP);
                     targ.Append(conv_buf);
                     break;
                  case 'X':     /* xp to level */
                     Sprintf(conv_buf, "%i", getXpToNextLevel());
                     targ.Append(conv_buf);
                     break;
                  case 'g':     /* gold */
                     Sprintf(conv_buf, "%i", GOLD);
                     targ.Append(conv_buf);
                     break;
                  case 'G':     /* bank gold */
                     Sprintf(conv_buf, "%i", BANK_GOLD);
                     targ.Append(conv_buf);
                     break;
                  case '1':     /* medical conditions */
                     if ( isGettingHungry() ) {
                        targ.Append("(hun)");
                     } if ( isGettingThirsty() ) {
                        targ.Append("(thi)");
                     } if ( isDrugged() ) {
                        targ.Append("(dru)");
                     }
                     break;
                  case '2':     /* visibility conditions */
                     if ( isSneaking() ) {
                        targ.Append("(snk)");
                     } if ( isHiding() ) {
                        targ.Append("(hid)");
                     } if ( isInvis() ) {
                        targ.Append("(inv)");
                     }
                     break;
                  case 'R':     /* name of the current room */
                     targ.Append(getCurRoom()->short_desc);
                     break;
                  case 'E':     /* visible exits */
                     while ((dr_ptr = cll.next())) {
                        if (detect(SEE_BIT, dr_ptr->dr_data->vis_bit)) {
                           if (!((dr_ptr->isClosed() && dr_ptr->isSecret()) ||
                                    dr_ptr->isSecretWhenOpen())) {
                              dest = abs(dr_ptr->destination);
                              if (isImmort()) { //if immortal, show extra info
                                 Sprintf(buf, "%s[%i] ", abbrev_dir_of_door(*dr_ptr),
                                       dest);
                              }//if
                              else {
                                 Sprintf(buf, "%s ", abbrev_dir_of_door(*dr_ptr));
                              }//else
                              reg_disp.Append(buf);
                           }//if its open, don't show closed exits
                        }//if detect
                     }//while
                     reg_disp.Strip();
                     targ.Append(reg_disp);
                     break;
                  case 'w':   /* weight currently being carried */
                     Sprintf(conv_buf, "%i", CRIT_WT_CARRIED);
                     targ.Append(conv_buf);
                     break;
                  case 'W':   /* weight currently being carried */
                     Sprintf(conv_buf, "%i", CRIT_MAX_WT_CARRY);
                     targ.Append(conv_buf);
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
   } else {
      targ.Append("< AFK > ");
   }

   // Instead of spamming out all the pending commands, be nice about it
   {
      String tmp;
      unsigned int i;

      tmp.Clear();
      for(i=0;i<pc->input.Strlen();i++) {
         if ( pc->input.charAt(i) == '\n' ) {
            tmp.append(";");
         } else {
            tmp.append(pc->input.charAt(i));
         } // else
      } // for
      targ.Append(tmp);
   }

   targ.Append(pc->p_handler->end_of_record());

   show(targ);

   if (isUsingClient()) {
      // HP, HP-MAX, MANA, MANA-MAX, MOV, MOV-MAX
      Sprintf(targ, "<PROMPT %i %i %i %i %i %i> ", // the trailing space is important.
            getHP(), getHP_MAX(), getMana(), getManaMax(),
            getMov(), getMovMax());
      show(targ);
   }

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



int critter::getImmLevel() const {
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

   // The first time we are placed in a room, lets remember it
   // so we can get home once we've tracked down anybody that is
   // bothering us!!
   if (mob && (mob->home_room == 0)) {
      mob->home_room = i;
   }

   // So now I want to know the current room for all Objects that
   // may be scripting.  If they are scripting, then we know they
   // have an IN_LIST (ie they are a SOBJ (more antiquity)).
   
   // First, check all equipment.  Remember to check inside containers
   // too.

   for (int z = 1; z < MAX_EQ; z++) {
      if (EQ[z] && EQ[z]->isModified()) {
         EQ[z]->setCurRoomNum(i, 0); //will recurse if needed
      }
   }

   Cell<object*> cll(inv);
   object* ptr;
   
   while ((ptr = cll.next())) {
      if (ptr->isModified()) {
         ptr->setCurRoomNum(i, 0); //is recursive if needed
      }
   }//while
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

int critter::isUsingClient() const {
   return ((pc && pc->pc_data_flags.get(5)) || 
           (possessed_by && possessed_by->pc->pc_data_flags.get(5)));
}//isUsingClient

int critter::whichClient() {
   if (pc)
      return pc->client;
   else
      return 0;
}//whichClient   

bool critter::setClient(int which) {

   if (NUM_CLIENTS < which) {
      return false;
   }

   if ( pc->client == which ) {
      return false;
   }

   pc->client = which;

   switch ( which ) {

      case HEGEMON:
         delete pc->p_handler;
         pc->p_handler = new HegemonHandler(this);
      break;

      case NO_CLIENT:
         delete pc->p_handler;
         pc->p_handler = new TelnetHandler(this);
      break;

   };

   return true;
}


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

int critter::isImmort() const {
   return (pc && pc->imm_data);
}

int critter::isFlying() {
   if ( isAvian() && (getLevel() >= 10) ) {
      return true;
   } else { 
      return (crit_flags.get(3));
   }
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
      PtrList<String> wordpats, wordreps, fragpats, fragreps;
      wordpats.append(new String("and"));    wordreps.append(new String("und"));
      wordpats.append(new String("I said")); wordreps.append(new String("I'm like"));
      wordpats.append(new String("I say"));  wordreps.append(new String("I'm like"));
      wordpats.append(new String("it's"));   wordreps.append(new String("ish"));
      wordpats.append(new String("its"));    wordreps.append(new String("ish"));
      wordpats.append(new String("have"));   wordreps.append(new String("gots"));
      wordpats.append(new String("that"));   wordreps.append(new String("tha"));
      wordpats.append(new String("their"));  wordreps.append(new String("thar"));
      fragpats.append(new String("th"));     fragreps.append(new String("f"));
      fragpats.append(new String("ck"));     fragreps.append(new String("fk"));
      fragpats.append(new String("sc"));     fragreps.append(new String("shhc"));
      fragpats.append(new String("s"));      fragreps.append(new String("sh"));
      fragpats.append(new String("ou"));     fragreps.append(new String("oo"));
      fragpats.append(new String("ur"));     fragreps.append(new String("uh"));
      fragpats.append(new String(","));      fragreps.append(new String(" (hic!)"));
      msg = transform(msg, wordpats, wordreps, fragpats, fragreps);
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

int critter::isSneaking() const {
   return crit_flags.get(17);
}

int critter::isHiding() const {
   return crit_flags.get(22);
}

int critter::isInvis() const {
    return (cur_stats[0] & 2);
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
                             int see_bit, room& rm, int& posn,
                             int& count_sofar) {
   int count = 0;
   for (int i = 1; i<MAX_EQ; i++) {
      if (EQ[i] && 
          detect(see_bit, rm.getVisBit() | EQ[i]->OBJ_VIS_BIT) &&
         obj_is_named(*(EQ[i]), name)) {
         count++;
         count_sofar++;
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

String* critter::getRoomColor() {
   if (pc) {
      return &(pc->room_str);
   }
   return &NULL_STRING;
}

int critter::isUsingColor() {
   return pc && pc->pc_data_flags.get(26);
}

int critter::getAge() const {
   int age = 18;

   if (pc) {

      if (config.day >= BIRTH_DAY) {
         age++;
      }//if

      age += config.year - BIRTH_YEAR;
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

   if (isPc() || targ.isNpc())
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
   int targ_room_num;

   if (!mob || !mob->proc_data || 
       !mob->proc_data->temp_proc) { 
      mudlog.log(ERROR, "ERROR:  bad pc sent to do_hunt_proc.\n");
      return;
   }//if

   // Let critters rest a bit before they hunt
   if (HP <= ((WIMPY * 2 < HP_MAX * .75) ? (WIMPY*2) : (HP_MAX*.75))) {
      return;
   }

   // If yer sessile, then you can't hunt!
   if (isSessile()) {
      return;
   }

   critter* targ = have_crit_named(pc_list, 1, getTrackingTarget(),
                                   ~0, *(getCurRoom()));

   // No longer hunt MOBS
   //if (!targ) {
   //   targ = have_crit_named(affected_mobs, 1,
   //                          getTrackingTarget(), ~0, *(getCurRoom()));
   //}//if

   if (!targ) { //quit hunting that person (maybe)
      Cell<String*> cll(HUNTING);
      String* sp, *stp;
      setTrackingTarget(NULL_STRING);

      sp = cll.next();
      while (sp) {
         /* There is a chance we will forget if the target is not
          * logged on currently.
          */
         if ((strcasecmp(*sp, TRACKING) == 0) && (d(1,100) == 5)) {
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
         mudlog.log(ERROR, "ERROR:  targ is MOB in do_hunt_proc.\n");
         return;
      }//if

      targ_room_num = targ->getCurRoomNum();

      if (targ_room_num == getCurRoomNum()) {
         checkForBattle(*(getCurRoom()));
      }
      else {
         //mudlog << "WARNING:  travelToRoom from doHuntProc" << endl;
         travelToRoom(targ_room_num, num_steps, is_dead);
      }
   }//else go get em
}//doHuntProc


int critter::isTeacher() {
   return (mob && mob->proc_data && FLAG1.get(2));
}

int critter::isShopKeeper() {
   return (mob && mob->proc_data && FLAG1.get(1));
}


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
         price = (int)((float)(price) * config.outOfTownModifier);
      }//if
      

      if (pc.pc && (d(1, 100) <
                    get_percent_lrnd(COMMERCE_SKILL_NUM, pc))) {
         price = (int)((float)price * config.commerceSkillBuyEffect);
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
         price = (int)(((float)(price))  * (1.0 / config.outOfTownModifier));

      if (pc.pc && (d(1, 100) <
                    get_percent_lrnd(COMMERCE_SKILL_NUM, pc))) {
         price = (int)((float)price * config.commerceSkillSellEffect);
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

   sink.mob->proc_data->sh_data->ps_data_list.clearAndDestroy();

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

int critter::isManagedBy(critter& the_pc) {
   if (the_pc.isNpc()) {
      String buf(100);
      Sprintf(buf, "%i_NPC", the_pc.getIdNum());
      return (isPlayerShopKeeper() &&
              (strcasecmp(mob->proc_data->sh_data->manager, buf) == 0));
   }
   else {
      return (isPlayerShopKeeper() &&
              (strcasecmp(mob->proc_data->sh_data->manager, *(the_pc.getName())) == 0));
   }
}//isManagedBy


int critter::withdrawCoins(int count, critter& banker) { //do messages
   String buf(100);

   if (long_data[2] >= count) {
      long_data[2] -= count;
      long_data[0] += count;
      Sprintf(buf, "%S gives you %i coins.\n", banker.getName(), count);
      show(buf);
      Sprintf(buf, "Your balance is now %i coins.", long_data[2]);
      do_tell(banker, buf, *this, FALSE, getCurRoomNum());
      return 0;
   }
   Sprintf(buf, "Your balance is only %i coins.", long_data[2]);

   do_tell(banker, buf, *this, FALSE, getCurRoomNum());
   return -1;
}

int critter::balanceCoins(critter& banker) {
   String buf(100);
   Sprintf(buf, "Your balance is %i coins.", long_data[2]);
   do_tell(banker, buf, *this, FALSE, getCurRoomNum());
   return 0;
}

int critter::depositCoins(int count, critter& banker) { //do messages
   String buf(100);

   if (long_data[0] >= count) {
      long_data[0] -= count;
      long_data[2] += count;
      Sprintf(buf, "%S puts %i coins into your account.\n", banker.getName(),
              count);
      show(buf);
      Sprintf(buf, "Your balance is now %i coins.", long_data[2]);
      do_tell(banker, buf, *this, FALSE, getCurRoomNum());
      return 0;
   }
   Sprintf(buf, "You have only %i coins.", long_data[0]);

   do_tell(banker, buf, *this, FALSE, getCurRoomNum());
   return -1;
}

/**  Translates an asci string (like HP, MANA, MOV, ALIGN, etc)
 * and returns the integer value.  Returns 0 if it can't
 * resolve the field.
 */
int critter::getFieldValue(const char* field) const {
   if (strcasecmp(field, "HP") == 0) {
      return getHP();
   }
   else if (strcasecmp(field, "MANA") == 0) {
      return getMana();
   }
   else if (strcasecmp(field, "MOV") == 0) {
      return getMov();
   }
   else if (strcasecmp(field, "ALIGN") == 0) {
      return getAlignment();
   }
   else if (strcasecmp(field, "BENEVOLENCE") == 0) {
      return getBenevolence();
   }
   else if (strcasecmp(field, "HOMETOWN") == 0) {
      return getHomeTown();
   }
   else if (strcasecmp(field, "NATIVEZONE") == 0) {
      return getNativeZoneNum();
   }
   else if (strcasecmp(field, "AGE") == 0) {
      return getAge();
   }
   else if (strcasecmp(field, "VISBIT") == 0) {
      return getVisBit();
   }
   else if (strcasecmp(field, "SEEBIT") == 0) {
      return getSeeBit();
   }
   else if (strcasecmp(field, "NAKED_WEIGHT") == 0) {
      return getNakedWeight();
   }
   else if (strcasecmp(field, "CHARISMA") == 0) {
      return getCharisma();
   }
   else if (strcasecmp(field, "HP_MAX") == 0) {
      return getHP_MAX();
   }
   else if (strcasecmp(field, "MANA_MAX") == 0) {
      return getManaMax();
   }
   else if (strcasecmp(field, "MOV_MAX") == 0) {
      return getMovMax();
   }
   else if (strcasecmp(field, "WIMPY") == 0) {
      return getWimpy();
   }
   else if (strcasecmp(field, "LEVEL") == 0) {
      return getLevel();
   }
   else if (strcasecmp(field, "CLASS") == 0) {
      return getClass();
   }
   else if (strcasecmp(field, "GOLD") == 0) {
      return GOLD;
   }
   else if (strcasecmp(field, "RACE") == 0) {
      return RACE;
   }
   else {
      return 0;
   }
}

/** Only valid for shop owners. */
int critter::isOpen(int cmt, int do_msg, critter& pc) const {
   if (mob && mob->proc_data && mob->proc_data->sh_data) {

      // 9 AM to 4 PM  (9 - 16)
      
      if (OPEN_TIME <= CLOSE_TIME) {
         if ((cmt >= OPEN_TIME) && (cmt <= CLOSE_TIME)) {
            return TRUE;
         }
      }
      // 4 PM to 9 AM (16(open) - 9(close))
      else if (CLOSE_TIME <= OPEN_TIME) {
         if (!((cmt > CLOSE_TIME) && (cmt < OPEN_TIME))) {
            return TRUE;
         }
      }
   }//if

   if (do_msg) {
      String buf(100);
      Sprintf(buf, cstr(CS_HOURS_FROM, pc),
              military_to_am(OPEN_TIME),
              military_to_am(CLOSE_TIME));
      pc.show(buf);
   }

   return FALSE;
}//isOpen

/** Can fail if cnt is bad, does all messages. */
int critter::doDropCoins(int cnt) {
   if (GOLD < cnt) {
      show(CS_TOO_LITTLE_GOLD);
      return -1;
   }
   else if (cnt <= 0) {
      show(CS_MUST_DROP_MORE_GOLD);
      return -1;
   }
   else {
      String buf(100);
      Sprintf(buf, cstr(CS_DROP_I_COINS, *this), cnt);
      show(buf);

      object* gold;
      gold = obj_to_sobj(obj_list[config.goldCoinsObject], getCurRoom()->getInv(),
                         getCurRoomNum());
      
      obj_list[config.goldCoinsObject].incrementCurInGame();
      
      gold->cur_stats[1] = cnt;
      GOLD -= cnt;
      
      getCurRoom()->gainObject(gold);
      return 0;
   }//all is well
}//doDropCoins


/** Can fail if cnt is bad, does all messages. */
int critter::doPutCoins(int cnt, object* bag) {
   if (GOLD < cnt) {
      show(CS_TOO_LITTLE_GOLD);
      return -1;
   }
   else if (cnt <= 0) {
      show(CS_MUST_DROP_MORE_GOLD);
      return -1;
   }
   else {
      String buf(100);
      Sprintf(buf, cstr(CS_PUT_I_COINS, *this), cnt, bag->getLongName(*this));
      show(buf);

      object* gold;
      gold = obj_to_sobj(obj_list[config.goldCoinsObject], getCurRoom()->getInv(),
                         getCurRoomNum());
      
      obj_list[config.goldCoinsObject].incrementCurInGame();
      
      gold->cur_stats[1] = cnt;
      GOLD -= cnt;
      
      bag->gainInv(gold);
      return 0;
   }//all is well
}//doPutCoins


critter* critter::makeSmob(int suppress_sub_fail_msg) {
   room* rm = getCurRoom();
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In mob_to_smob:  " << getName() << endl;
   }

   /*  This function DOES DO SUBSTITUTION itself, calling code need not worry
       about it. */

   
   
   if (isSmob()) {
	   //already an smob, so return this
      mudlog.log(DBG, "ERROR:  mob_to_smob called on nonMOB.\n");
	  return this;
   }//if

   if (isPc()) {
      mudlog.log(DBG, "ERROR:  mob has pc_data in mob_to_smob.\n");
	  //we can return a pc here, since they can be modified safely(more or less)
	  return this;
   }//if

   MOB_FLAGS.turn_on(4); //now it can be/should be reset when pulsed

   critter* crit_ptr = new critter(*this);

   obj_ptr_log << "CRI_CR " << getIdNum() << " " << crit_ptr << "\n";

   crit_ptr->setCurRoomNum(rm->getIdNum());

   if (!rm->sub_a_4_b(crit_ptr, this, 1)) {
      /* note:  create golem and others will fail on this now with no bad
         effects, so this error message has been commented out. */
      if (!suppress_sub_fail_msg) {
         mudlog << "ERROR:  SubstituteData failed in mob_to_smob, mob#: "
                << crit_ptr->getIdNum() << "  name: " << *(crit_ptr->getName())
                << " room# " << rm->getIdNum() << endl;
      }//if

      // If couldn't substitute, then we need to increment it's
      // CUR_IN_GAME
      crit_ptr->incrementCurInGame();
   }//if
   
   affected_mobs.gainData(crit_ptr);  //basically a list of smobs

   if (mudlog.ofLevel(DBG)) {
      mudlog << "mob_to_smob:  returning critter, address:  " << crit_ptr
             << endl;
   }
   crit_ptr->mob->setTicksOld(0);

   return crit_ptr;
}

int critter::takeDamage(int damage, int type, critter& agg) {

	//this is NOT the same as the one in battle.cc
	//a bug with earthmeld was fixed, it used agg's pl, not vict 
	//but there may be a reason for that that i am unaware of
	//note to self: add crit.damageMagicShields so we don't rely on
	// the one in battle.cc

   int armor_value = 100;
   float dam = (float)damage;
   
   switch (type) {
    case FIRE: case D_BREATH:
      armor_value = HEAT_RESIS;
      break;
    case ICE:
      armor_value = COLD_RESIS;
      break;
    case ELECTRICITY:
      armor_value = ELEC_RESIS;
      break;
   }//switch
   
   armor_value += 100;
   if (armor_value < 0) {
      armor_value = 0;
   }//if

   // Special damages add an additional 10% owie at the 1:1 ratio.
   switch(type) {
      case FIRE:
      case D_BREATH:
      case ICE:
      case ELECTRICITY:
         dam += ( dam*0.10 );
         break;
   }

   dam = dam * ((float)armor_value/200.0);
   
   damage_magic_shields(dam, *this);

   dam = (dam * (((float)getDamRecMod())/100.0) * 
          ((float)(agg.DAM_GIV_MOD)/100.0));

   int pl;
   if (agg.pc && 
       (pl = get_percent_lrnd(ENHANCED_DAMAGE_SKILL_NUM, agg)) > 0) {
       // between 0.5% at 1%lrnd to 50% at 100%lrnd extra damage MAX
      dam += ( dam * ( ((float)d(1, (pl/2)+1))/100) ); 
      // old method dam += (float)(d(1, pl/20));
   }//if
   
   if ((agg.ALIGN < -350) && is_affected_by(PFE_SKILL_NUM, *this)) {
      dam -= (float)(d(1, 10));
      if (dam < 0)
        dam = 0;
   }//if
   
   if ((agg.ALIGN > 350) &&is_affected_by(PFG_SKILL_NUM, *this)) {
      dam -= (float)(d(1, 10));
      if (dam < 0)
        dam = 0;
   }//if
   
   if (is_affected_by(ABSORB_BLOWS_SKILL_NUM, *this)) {
      MANA -= (short)dam;
      if (MANA < 0) {
         dam *= 2;
         MANA = 0;
      }//if
      else {
         dam = -(dam/1.5);
      }//else
   }//if

   if (is_affected_by(EARTHMELD_SKILL_NUM, *this)) {
      if ((pl = get_percent_lrnd(EARTHMELD_SKILL_NUM, *this)) > d(1,115)) {
         dam *= (100.0 - ((float)(pl))/7.0) / 100.0;
      }//if
   }//if
   
   // Gonna make low level mobs a little weaker across the board to
   // help out the newbies!
   if (agg.isNPC() && (agg.getLevel() < 10)) {
      dam *= 0.75;
   }

   if (isImmort() && isNoHassle()
       && getImmLevel() >= agg.getImmLevel()) {
      dam = 0.0;
      if (mudlog.ofLevel(INF)) {
         mudlog << "INFO:  setting damage to zero because of !hassle.\n"
                << endl;
      }//if
   }//if

   // Special checks for Flesh-to-Stone
   if (is_affected_by(FLESH_TO_STONE_SKILL_NUM, *this)) {
      if (dam > 3.0) {
         dam = 3.0;
      }
   }

   if (is_affected_by(FLESH_TO_STONE_SKILL_NUM, agg)) {
      // Just in case someone lets some damage slip through...
      if (dam > 5.0) {
         dam = 5.0;
      }
   }

  HP -= (int)(dam);
  return abs((int)dam);
}//exact_raw_damage


int critter::takeDamage(int damage, int type) {

	//this is NOT the same as the one in battle.cc
	//a bug with earthmeld was fixed, it used agg's pl, not vict 
	//but there may be a reason for that that i am unaware of
	//note to self: add crit.damageMagicShields so we don't rely on
	// the one in battle.cc

   int armor_value = 100;
   float dam = (float)damage;
   
   switch (type) {
    case FIRE: case D_BREATH:
      armor_value = HEAT_RESIS;
      break;
    case ICE:
      armor_value = COLD_RESIS;
      break;
    case ELECTRICITY:
      armor_value = ELEC_RESIS;
      break;
   }//switch
   
   armor_value += 100;
   if (armor_value < 0) {
      armor_value = 0;
   }//if

   // Special damages add an additional 10% owie at the 1:1 ratio.
   switch(type) {
      case FIRE:
      case D_BREATH:
      case ICE:
      case ELECTRICITY:
         dam += ( dam*0.10 );
         break;
   }

   dam = dam * ((float)armor_value/200.0);
   
   damage_magic_shields(dam, *this);

   dam *= (((float)getDamRecMod())/100.0);
   HP -= (int)(dam);
   return (int)dam;
}

float critter::combatBonusVal(bool is_aggressor)  {

   int p_lrnd;
   float ret_val = 0;

   ret_val += getLevel()/1.35;

   if ( getPosn() > 2 ) {
      //sleeping or worse, no bonus for you
      return 0;
   }

   if ( getPosn() > 1 ) {//if they're resting or worse
      //return with just a level bonus
      return ret_val;
   }

   ret_val += (getDEX(true)-10)/2.0;

   if ( getPosn() > 0 ) {//less than standing, you get dex+level
      return ret_val;
   }

   //currently defenders have a few additional helping skills
   if ( !is_aggressor ) {

      //TODO: heavy armor _should_ negatively impact this
      ret_val += get_percent_lrnd(DODGE_SKILL_NUM, *this)/15.0;

      if ( isUsingShield() ) {//can only block when using a shield
         ret_val += get_percent_lrnd(BLOCK_SKILL_NUM, *this)/15.0;
      }

      //add in their ability to parry, but only if they have a weapon
      p_lrnd = get_percent_lrnd(PARRY_SKILL_NUM, *this);
      if ( eq[9] ) {
         ret_val += p_lrnd/15.0;
      }

      //TODO: really you shouldn't be able to dual-wield _AND_ use a shield at
      //all.
      //
      //if you're using a shield, you can't parry with your off-hand. there's
      //also a sizeable penalty for the off-hand.
      if ( eq[10] && isDualWielding() && ( ! isUsingShield() ) ) {
         ret_val += p_lrnd/30.0;
      }
   }//defender-specific skills


   //things that apply to anyone follow
   //TODO: Again, heavy armor should negatively impact both of these
   ret_val += get_percent_lrnd(QUICKFOOT_SKILL_NUM, *this)/15.0;
   ret_val += get_percent_lrnd(MARTIAL_ARTS_SKILL_NUM, *this)/15.0;

   //the following only give credit if they've got a weapon
   if ( eq[9] || ( eq[10] && isDualWielding() && (! isUsingShield()) ) ) {

      p_lrnd = get_percent_lrnd(WEAPON_MASTERY_SKILL_NUM, *this);

      for ( int i = 9; i <= 10; i++ ) {

         if ( ! eq[i] ) {//skip if there isn't a weapon in this slot.
            continue;
         }

         //only check the off-hand if they're dual wielding and not using a
         //shield.
         if ( i == 10 && ( (! isDualWielding()) || (isUsingShield()) ) ) {
            continue;
         }

         float bonus;
         bonus = 0;

         //apply weapon specific bonuses
         if ( eq[i]->isSlash() ) {
            bonus += get_percent_lrnd(SWORD_SKILL_NUM, *this)/15.0;
            bonus += get_percent_lrnd(FENCING_SKILL_NUM, *this)/15.0;
         }
         else if ( eq[i]->isPierce() ) {
            bonus += get_percent_lrnd(DAGGER_SKILL_NUM, *this)/15.0;
         }
         else if ( eq[i]->isSmash() ) {
            bonus += get_percent_lrnd(MACE_SKILL_NUM, *this)/15.0;
         }
         else if ( eq[i]->isWhip() ) {
            bonus += get_percent_lrnd(WHIP_SKILL_NUM, *this)/15.0;
         }
         else if ( eq[i]->isBow() ) {
            bonus += get_percent_lrnd(BOW_SKILL_NUM, *this)/15.0;
         }

         if ( i == 10 ) {//penalize off-hand bonuses
            bonus *= 0.50;
         }

         ret_val += bonus;

      }//check both main-hand and off-hand

   }//if they have at least one weapon

   return ret_val;
}//critter::combatBonusVal()
