// $Id: critter.cc,v 1.50 1999/08/27 03:10:03 greear Exp $
// $Revision: 1.50 $  $Author: greear $ $Date: 1999/08/27 03:10:03 $

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
#include "command2.h"
#include "command3.h"
#include "command4.h"
#include "command5.h"
#include "batl_prc.h"
#include "battle.h"
#include <stdarg.h>
#include "Markup.h"
#include "spells.h"

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
   clear();
}//deconstructor
   
void immort_data::clear() {
   if (olc_mob) {
      olc_mob->clear();
      olc_mob = NULL;
   }//if
   
   if (olc_room) {
      olc_room->clear();
      olc_room = NULL;
   }//if
   
   if (olc_obj) {
      olc_obj->clear();
      olc_obj = NULL;
   }//if
   
   if (olc_door) {
      olc_door->clear();
      olc_door = NULL;
   }//if
   tmplist.clearAndDestroy();
   olc_counter = temp_olc_int = imm_level = 0;
   edit_string = NULL; //clearing it probably not the right solution

   delete tmp_proc_script;
   tmp_proc_script = NULL;

} // Clear


int immort_data::read(istream& dafile, int read_all = TRUE) {
   char buf[82];

   dafile >> imm_level;

   dafile.getline(buf, 80); //junk end of line
   return 0;
}//Read

int immort_data::write(ostream& dafile) {
   dafile << imm_level << "\tImm level\n";
   return 0;
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

int teacher_data::read(istream& da_file, int read_all = TRUE) {
   return teach_data_flags.read(da_file);
}//Read

int teacher_data::write(ostream& da_file) {
   return teach_data_flags.write(da_file);
}//Write


void teacher_data::toStringStat(critter* viewer, String& rslt) {
   String buf(100);
   if (viewer->isUsingClient()) {
      rslt = "<TEACHER_DATA>\n";
   }
   else {
      rslt = "Teacher Data\n";
   }

   Markup::toString(NULL, teach_data_flags, TEACH_DATA_FLAGS_NAMES,
                    viewer, NULL, buf);
   rslt.append(buf);

   if (viewer->isUsingClient()) {
      rslt = "</TEACHER_DATA>\n";
   }
}


//****************************************************************//
///*********************  Player Shop Data  *********************///

int PlayerShopData::_cnt = 0; // Instance Count

int PlayerShopData::read(istream& da_file, int read_all = TRUE) {
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
      
int PlayerShopData::write(ostream& da_file) {
   da_file << 1 << " " << object_num << " "
           << sell_price << " " << buy_price << " PlayerShopData \n";
   return 0;
}//write




//**********************************************************//
///*********************  shop data  ***********************///

int shop_data::_cnt = 0;

shop_data::shop_data() : perm_inv((object*)(NULL)) {
   _cnt++;
   markup = buy_percentage = open_time = close_time = 0;
}//constructor;

shop_data::shop_data(shop_data& source) : perm_inv(NULL) {
   _cnt++;
   *this = source;
}//constructor;

shop_data::~shop_data() {
   _cnt--;
   clear();
}

shop_data& shop_data::operator=(shop_data& source) {
   
   if (this == &source)
      return *this;

   clear();

   markup = source.markup;
   buy_percentage = source.buy_percentage;
   open_time = source.open_time;
   close_time = source.close_time;

   SCell<object*> cll(source.perm_inv);
   object* optr;
   while ((optr = cll.next())) {
      if (!optr->isModified()) {
         perm_inv.append(optr);
      }//if
   }//while

   // Deep copy here...
   ps_data_list.becomeDeepCopyOf(source.ps_data_list);
  
   perm_inv = source.perm_inv;
   shop_data_flags = source.shop_data_flags;

   return *this;
}//operator=;

void shop_data::clear() {
   markup = buy_percentage = close_time = open_time = 0;
   perm_inv.clear();
   shop_data_flags.clear();
   ps_data_list.clearAndDestroy();
}//Clear

int shop_data::read(istream& da_file, int read_all, critter* container) {
   char tmp[100];
   int i;
   String buf(100);
   clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in shop_data read." << endl;
      }
      return -1;
   }


   da_file >> markup >> buy_percentage >> open_time >> close_time;
   da_file.getline(tmp, 80);
   
   shop_data_flags.read(da_file);
   
   /*  Inventory */
   da_file >> i;
   while (i != -1) {
      if (i == -2) { //assume its gonna load fer sure
         object* new_obj = new object;
         da_file.getline(tmp, 80);  //junk message
         new_obj->read(da_file, read_all);
         new_obj->setContainer(container);
         new_obj->setModified(TRUE);
         //TODO:  gainObj??
         perm_inv.append(new_obj);    //add it to inventory
         affected_objects.append(new_obj);
      }//if
      else {
         if (obj_list[i].isInUse()) {
            if (read_all || 
                ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 100) > 
                d(1,100)) {
               object* ptr = &(obj_list[i]);
               //TODO: gainObj??
               perm_inv.append(ptr);    //add it to inventory
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
         ps_data_list.append(ps_data);
         ps_data = new PlayerShopData();
      }//while

      // Delete the last one, was not added to the list.
      delete ps_data;
      ps_data = NULL;

      da_file >> manager;
      da_file.getline(tmp, 80);

   }//if is a Player Run Shopkeeper

   return 0;
}//Read


int shop_data::write(ostream& da_file) {
   SCell<object*> ob_cell(perm_inv);
   object* ob_ptr;
   int i = 0;
   
   da_file << " " << markup << " " << buy_percentage << " " 
           << open_time << " " << close_time 
           << "\tshop_data shorts\n";
   shop_data_flags.write(da_file);
   
   /*  Inventory */
   i = 0;
   while ((ob_ptr = ob_cell.next())) {
      if (ob_ptr->isModified()) {
         da_file << -2 << "\t Start of SOBJ\n";
         ob_ptr->write(da_file);
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
   if (da_file)
      return 0;
   return -1;
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
      ps_data_list.append(new PlayerShopData(obj.getIdNum(),
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

void shop_data::toStringStat(critter* viewer, String& rslt) {
   String buf(100);
   if (viewer->isUsingClient()) {
      rslt = "<SHOP_DATA>\n";
   }
   else {
      rslt = "Shop Data:\n";
   }

   Sprintf(buf, "markup  %i, buy_percentage  %i, open_time %i, close_time %i",
           markup, buy_percentage, open_time, close_time);
   rslt.append(buf);

   Markup::toString(NULL, shop_data_flags, SHOP_DATA_FLAGS_NAMES, 
                    viewer, NULL, buf);
   rslt.append(buf);

   rslt.append("Permanent Inventory:\n");
   out_inv(perm_inv, *viewer, CRIT_INV);

   if (viewer->isUsingClient()) {
      rslt.append("</SHOP_DATA>\n");
   }
}//toStringStat



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

void temp_proc_data::clear() {
   hunting.clearAndDestroy();
   tracking.clear();
}//clear

temp_proc_data& temp_proc_data::operator= (const temp_proc_data& source) {

   if (this == &source)
      return *this;

   hunting.becomeDeepCopyOf(source.hunting);
   tracking = source.tracking;
   return *this;
}//operator= overload

  
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
}//constructor


spec_data::spec_data(const spec_data& source) {
   _cnt++;
   sh_data = NULL;
   teach_data = NULL;
   temp_proc = NULL;
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
}//destructor

void spec_data::clear() {
   delete sh_data;
   sh_data = NULL;
   delete teach_data;
   teach_data = NULL;
   delete temp_proc;
   temp_proc = NULL;
   flag1.clear();
   int1 = 0;
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

   clear();
   flag1 = source.flag1;
   int1 = source.int1;
   if (source.sh_data)
      sh_data = new shop_data(*(source.sh_data));
   if (source.teach_data) 
      teach_data = new teacher_data(*(source.teach_data));
   if (source.temp_proc)
      temp_proc = new temp_proc_data(*(source.temp_proc));

   skill_violence = source.skill_violence;
   benevolence = source.benevolence;
   defensiveness = source.defensiveness;
   bad_assedness = source.bad_assedness;
   social_awareness = source.social_awareness;

   return *this;
}//op = overload


int spec_data::read(istream& da_file, int read_all = TRUE) {
   char tmp[81];
  
   clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in spec_data read." << endl;
      }
      return -1;
   }

   flag1.read(da_file);
   
   flag1.turn_off(12); //hack, get rid of this flag
  
   da_file >> int1;
   da_file.getline(tmp, 80);

   if (flag1.get(1)) {  // shopkeeper
      sh_data = new shop_data;
      sh_data->read(da_file, read_all);
   }//if

   if (flag1.get(2)) {  // teacher
      teach_data = new teacher_data;
      teach_data->read(da_file);
   }//if

   if (flag1.get(5) || flag1.get(7) || flag1.get(8) || flag1.get(6)) {
      cerr << "ERROR:  action and say procs are no longer supported, you must"
           << "hand edit them out of the mobs which have them..sorry for the"
           << "inconvenience!!" << endl;
      mudlog << "ERROR:  action and say procs are no longer supported, you must"
             << "hand edit them out of the mobs which have them..sorry for the"
             << "inconvenience!!" << endl;
      exit(25);
   }

   if (flag1.get(13)) { //if HAS_MOB_AI
      da_file >> skill_violence >> benevolence >> defensiveness
              >>  bad_assedness >> social_awareness;
      da_file.getline(tmp, 80);      
   }//if

   if (da_file)
      return 0;
   return -1;

}//Read


int spec_data::write(ostream& da_file){

   if (skill_violence || benevolence || defensiveness || bad_assedness) {
      flag1.turn_on(13); //make sure
   }//if

   flag1.write(da_file);
   da_file << int1 << "\tint1, above bitfield started spec_data\n";
  
   if (flag1.get(1)) {  // shopkeeper
      if (!sh_data) {
         mudlog.log(ERR, "ERROR:  trying to write sh_data, but it's NULL\n");
         return -1;
      }//if
      sh_data->write(da_file);
   }//if

   if (flag1.get(2)) {  // teacher
      if (!teach_data) {
         mudlog.log(ERR, 
                    "ERROR:  trying to write teach_data, but it's NULL\n");
         return -1; 
      }//if
      teach_data->write(da_file);
   }//if

   if (flag1.get(13)) {
      da_file << skill_violence << " " << benevolence << " "
              << defensiveness << " " << bad_assedness << " "
              << social_awareness
              << "  sk_viol, benev, def, bad_assed, soc_aware\n";
   }//if
   if (da_file)
      return 0;
   return -1;
}//Write


void spec_data::toStringStat(critter* viewer, String& rslt) {
   String buf(100);

   if (viewer->isUsingClient()) {
      rslt = "<MOB_SPEC_DATA>\n";
   }
   else {
      rslt = "Special Procedure Data:\n";
   }

   Markup::toString(NULL, flag1, MOB_PROC_DATA_FLAGS_NAMES, viewer, NULL, buf);
   rslt.append(buf);

   Sprintf(buf, "\tDirection Guarding:  %i\n", int1);
   rslt.append(buf);

   Sprintf(buf, "skill_violnc:  %i, benev:  %i, def:  %i, bad_ass %i, Soc_Aware: %i\n",
           getSkillViolence(), getBenevolence(), getDefensiveness(),
           getBadAssedness(), getSocialAwareness());
   rslt.append(buf);

   if (teach_data) {
      teach_data->toStringStat(viewer, buf);
      rslt.append(buf);
   }

   if (sh_data) {
      sh_data->toStringStat(viewer, buf);
      rslt.append(buf);
   }//if

   if (viewer->isUsingClient()) {
      rslt.append("</MOB_SPEC_DATA>\n");
   }

}//toStringStat


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
   clear();
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


void temp_crit_data::clear() {
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
}//clear


void temp_crit_data::toStringStat(critter* viewer, String& rslt) {
   String buf(100);
   rslt.clear();

   if (guarded_by) {
      Sprintf(buf, cstr(CS_GUARDED_BY, *viewer), guarded_by->getName(viewer));
      rslt.append(buf);
   }//if

   if (guarding) {
      Sprintf(buf, cstr(CS_GUARDING, *viewer), guarding->getName(viewer));
      rslt.append(buf);
   }//if

   if (shielded_by) {
      Sprintf(buf, cstr(CS_SHIELDED_BY, *viewer), shielded_by->getName(viewer));
      rslt.append(buf);
   }//if

   if (shielding) {
      Sprintf(buf, cstr(CS_SHIELDING, *viewer), shielding->getName(viewer));
      rslt.append(buf);
   }//if

}//toStringStat




///*******************************  mob data **************************///

int mob_data::_cnt = 0;

mob_data::mob_data() {
   _cnt++;
   mob_num = tmp_num = cur_in_game = max_in_game = 0;
   proc_data = NULL;
   skin_num = 0;
   home_room = 0;
}//constructor

mob_data::mob_data(mob_data& source) {
   _cnt++;
   proc_data = NULL;
   *this = source;
}//constructor

mob_data::~mob_data() {
   _cnt--;
   delete proc_data; //if doesn't exist, then its NULL, still no prob :)
   proc_data = NULL;
}//destructor
 
void mob_data::clear() {
   mob_num = tmp_num = cur_in_game = max_in_game = 0;
   delete proc_data;
   proc_data = NULL;
   mob_data_flags.clear();
   skin_num = 0;
   home_room = 0;

}//Clear, mob_data

mob_data& mob_data::operator= (mob_data& source) {

   if (this == &source)
      return *this;

   clear();

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
   
   return *this;
}//mob_data operator=


int mob_data::write(ostream& ofile) {
   
   ofile << " " << mob_num << " " << tmp_num << " " 
         << max_in_game << "\tmob#, tmp_num, max_n_game\n";
   
   mob_data_flags.write(ofile);
   
   if (mob_data_flags.get(0)) { //does it have spec_data?
      if (!(proc_data)) {
         mudlog.log(ERR, "ERROR:  told to write proc_data, but is NULL!\n");
         return -1;
      }//if
      proc_data->write(ofile);
   }//if
   if (mob_data_flags.get(16)) {
      ofile << skin_num << "	skin number\n";
   }//if
   return 0;
}//Write()


int mob_data::read(istream& ofile, int read_all = TRUE) {
   char tmp[81];
   
   clear();
   
   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in mob_data read." << endl;
      }
      return -1;
   }

   ofile >> mob_num >> tmp_num >> max_in_game;
   ofile.getline(tmp, 80);

   if (mudlog.ofLevel(DBG))
      mudlog << "Mob number:  " << mob_num << endl;
   
   tmp_num = 0; //since it isn't used..should be zero
   
   mob_data_flags.read(ofile);
   
   // not currently used.
   mob_data_flags.turn_off(9);
   mob_data_flags.turn_off(10);
   mob_data_flags.turn_off(11);
   mob_data_flags.turn_off(12);
   mob_data_flags.turn_off(13);
   mob_data_flags.turn_off(14);
   mob_data_flags.turn_off(15);
   
   if (mob_data_flags.get(0)) { //does it have spec_data?
      proc_data = new spec_data;
      proc_data->read(ofile, read_all);
   }//if
   
   if (mob_data_flags.get(16)) {
      ofile >> skin_num;
      ofile.getline(tmp, 80);
   }//if
   return 0;
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


void mob_data::toStringStat(critter* viewer, String& rslt) {
   String buf(100);

   if (viewer->isUsingClient()) {
      rslt = "<MOB_DATA>\n";
   }
   else {
      rslt = "NPC Data:\n";
   }

   Sprintf(buf, "\tCur in game: %i,  Max in game: %i, Home Room: %i, Sessile: %i.\n",
           getCurInGame(), getMaxInGame(), home_room, isSessile());
   rslt.append(rslt);

   if (isTracking()) {
      Sprintf(buf, "\tTracking target:  %S", getTrackingTarget());
      rslt.append(buf);
   }

   Markup::toString(NULL, mob_data_flags, MOB_DATA_FLAGS_NAMES, viewer, NULL, buf);
   rslt.append(buf);
      
   if (proc_data) {
      proc_data->toStringStat(viewer, buf);
      rslt.append(buf);
   }

   Sprintf(buf, "Skin num: %i\n\n", skin_num);
   rslt.append(buf);


   if (viewer->isUsingClient()) {
      rslt = "</MOB_DATA>\n";
   }
}//toStringStat


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
   clear();
}//constructor

pc_data::pc_data(pc_data& source) {
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
      w_eye_obj->obj_proc->w_eye_owner = NULL;
      w_eye_obj = NULL;
   }//if
}//destructor
 
void pc_data::clear() {
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
   pc_data_flags.clear();
   skills_spells_known.Clear();
   prompt.clear();
   poofout.clear();
   poofin.clear();
   
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

   preferred_language = English;
   
   last_login_time = 0; //in seconds, since 1970 etc...
   total_time_online = 0; //in seconds
   lines_on_page = 20;
   if (w_eye_obj) {
      w_eye_obj->obj_proc->w_eye_owner = NULL;
      w_eye_obj = NULL;
   }//if

   // Volatile members (not saved to disk).
   bug_num = 0;
   bug_comment.Clear();

}//Clear, pc_data


pc_data& pc_data::operator=(pc_data& source) {

   if (this == &source)
      return *this;
   
   clear();
   
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

   preferred_language = source.preferred_language;

   return *this;
}//operator=, pc_data


int pc_data::write(ostream& ofile) {
   int i;
   
   ofile << password << "\tpasswd\n";
   
   pc_data_flags.write(ofile);
   
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
         imm_data->write(ofile);
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

   if (pc_data_flags.get(27)) {
      ofile << (int)(preferred_language) << " -1 preferred_language\n" << endl;
   }

   ofile << "*** end of pc data ***\n";
   return 0;
}//write()       


int pc_data::read(istream& ofile, int read_all = TRUE) {
   int i;
   char tmp[81];
   
   pc_data::clear();

   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in pc_data read." << endl;
      }
      return -1;
   }
   
   ofile >> password;
   ofile.getline(tmp, 80);
   
   pc_data_flags.read(ofile);

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
         return -1;
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
      imm_data->read(ofile);
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

   if (pc_data_flags.get(27)) {
      int t;
      ofile >> t;
      preferred_language = (LanguageE)(t);
      ofile.getline(tmp, 80);
   }

   ofile.getline(tmp, 80); //grabs extra line/comment
   return 0;
}//Read()       


void pc_data::toStringStat(critter* viewer, String& rslt, ToStringTypeE st) {
   String buf(100);

   rslt.clear();

   if (viewer->isUsingClient()) {
      rslt = "<PC_DATA>\n";
   }

   Sprintf(buf, "\tHunger:  %i,  Thirst:  %i,  Drugged:  %i.\n",
           hunger, thirst, drugged);
   rslt.append(buf);
   
   if (viewer->isImmort() || (st | ST_LONG)) {
      Sprintf(buf, "pk_count:  %i, died %i times, quest_points %i\n",
              pk_count, died_count, quest_points);
      rslt.append(buf);
   }

   if (viewer->isImmort()) {
      Markup::toString(NULL, pc_data_flags, PC_DATA_FLAGS_NAMES, viewer, NULL, buf);
      rslt.append(buf);
      
      Sprintf(buf, "\n\tDescriptor:  %i,  Host:  %S.\n", 
              descriptor, &host);
      rslt.append(buf);
      
      Sprintf(buf, "\tLink cond:  %i,  Mode:  %i,  Index:  %i.\n",
              link_condition, mode, index);
      rslt.append(buf);
      
      if (imm_data) {
         Sprintf(buf, "IMM_LEVEL:  %i\n", imm_data->imm_level);
         rslt.append(buf);
      }//if
      
      if (viewer->isUsingClient()) {
         rslt.append("</PC_DATA>\n");
      }
   }//if viewer is immortal
}//toStringStat

//*************************************************************//
///*********************  critter  ***************************///

int critter::_cnt = 0;

critter::critter() : inv(NULL), pets(NULL), followers(NULL), groupees(NULL),
                     is_fighting(NULL) {
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
   clear();
} // crit constructor


critter::critter(critter& source) : inv(NULL), pets(NULL), followers(NULL), groupees(NULL),
                                    is_fighting(NULL)  {
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

   clear();
}//destructor


critter& critter::operator=(critter& source) { //automagically makes SMOB

   if (this == &source)
      return *this;

   SCell<object*> cll;
   object* obj_ptr;
   int i;

      //mudlog.log(DBG, "In crit operator= overload.\n");

   clear();

   short_desc = source.short_desc;
   in_room_desc = source.in_room_desc;

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
         if (!(source.eq[i]->isModified())) { //don't need multiple ptrs to SOBJ's
            eq[i] = source.eq[i];
         }//if
	 else {
            eq[i] = new object(*(source.eq[i]));
            if (eq[i]) // if had the memory
               eq[i]->setContainer(this);
	 }//else
      }//if
      else {
         eq[i] = NULL;
      }//else
   }//for

   mini_affected_by.becomeDeepCopyOf(source.mini_affected_by);
   
   source.inv.head(cll);
   while ((obj_ptr = cll.next())) {
      if (!obj_ptr->isModified()) { //no multiple ptrs to SOBJ's
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


void critter::clear() {
   int i;
   critter* ptr;

   //mudlog.log(DBG, "In crit clear().\n");
   short_desc.clear();
   in_room_desc.clear();

   crit_flags.clear();

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

      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  followers not empty in clear, for mob: "
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

   //mudlog.log(DBG, "Done w/clear.\n");
}//crit clear


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
         return cstr(CS_STANDING, for_this_pc);
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

      for (int i = 0; ((i < iter) && (!isFighting())); i++) {
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

         int count_sofar = 0;
         door* dptr = door::findDoor(getCurRoom()->DOORS, 1, dir,
                                     count_sofar, NULL);
         
         if (!dptr) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "WARNING: Could not find door: " << *dir 
                      << " to room: "
                      << next_room << " from room: " << getCurRoomNum()
                      << endl;
            }//if
            return -1;
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
   SCell<object*> cll(inv);
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

   retval += GOLD / GOLD_PER_LB; //gold weighs a little..though not much

   return retval + getNakedWeight();
} // cur_weight

int critter::getMaxWeight() {
   return (short_cur_stats[1] * 10 + getNakedWeight());  // 10 times their str
}//max_weight

int critter::getCurRoomNum() {
   return IN_ROOM;
}

void critter::checkForBattle(room& rm) {
   rm.checkForBattle(*this);
}

int critter::canDetect(const critter& other) const {
   return detect(getSeeBit(), other.getVisBit());
}


int critter::write(ostream& ofile) {
   int i, num_written;
   SCell<object*> ob_cell;
   object* ob_ptr;
   String tmp_str(100);

   //TODO:  Write out super-classes.

   if (pc) {
      ofile << " " << pc->host << " __LEVEL__ " << getLevel();
   }

   ofile << "\n";

   short_desc.write(ofile);
   in_room_desc.write(ofile);

   crit_flags.write(ofile);

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
         if (!eq[i]->isModified()) {
            ofile << eq[i]->OBJ_NUM << " " << i << " ";
            if ((++num_written % 20) == 0)
               ofile << endl;
         }//if
         else { //then its a SOBJ
            ofile << -2 << " " << i << "\t Start of SOBJ\n";
            eq[i]->write(ofile);
         }//else
      }//if
   }//for
   ofile << -1 << "\teq: num, posn\n";

		/*  Inventory */
   
   num_written = 0;
   inv.head(ob_cell);
   while ((ob_ptr = ob_cell.next())) {
      if (ob_ptr->isModified()) {
         ofile << -2 << "\t Start of SOBJ\n";
         ob_ptr->write(ofile);
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
         return -1;
      }//if
      pc->write(ofile);  //write it out
   }//if
   else { //its a mob/smob
      if (!mob) {
         mudlog.log(ERR, "ERROR: trying to write out mob_data when mob is NULL.\n");
         return -1;
      }//if
      mob->write(ofile); //write it out
   }//else
   ofile << "\tend_of_crit\n";  //done
   return 0;
}//Write....crit


int critter::read_v3(istream& ofile, int read_all = TRUE) {
   int i, z;
   char tmp[81];
   String tmp_str(80);

   clear();

   if (!ofile) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in crit read." << endl;
      }
      return -1;
   }

   Entity::read(ofile, read_all);
   Scriptable::read(ofile, read_all);

   short_desc.read(ofile);
   in_room_desc.read(ofile);

   crit_flags.read(ofile);
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
         return -1;
      }

      ofile >> z;
      if (!check_l_range(z, 1, MAX_EQ, mob_list[0], FALSE)) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  wear_posn out of range, crit.Read():  " << z
                   << " short desc:  " << *(getName()) << endl;
         }
         ofile >> i;
         continue;
      }//if
      if (i == -2) { //gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->read(ofile, TRUE);
         new_obj->setContainer(this);
         new_obj->setModified(true);

         eq[z] = new_obj;      //now it wears it
         affected_objects.append(new_obj);
 
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

		/*  Inventory */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  da_file FALSE in crit read." << endl;
         }
         return -1;
      }

      if (i == -2) { //gonna load fer sure
         object* new_obj = new object;
         ofile.getline(tmp, 80);  //junk message
         new_obj->read(ofile, TRUE);
         new_obj->setContainer(this);
         new_obj->setModified(TRUE);

         inv.append(new_obj);
         affected_objects.append(new_obj);

      }//if
      else {
         if (obj_list[i].isInUse()) {
            if (read_all ||
              ((obj_list[i].OBJ_PRCNT_LOAD * Load_Modifier) / 100) > 
                      d(1,100)) {
               object* ptr = &(obj_list[i]);
               inv.append(ptr); //TODO: gainObj??
            }//if
         }//if
         else {
            if (mudlog.ofLevel(ERR)) {
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
      pc->read(ofile);  
   }//if
   else { //its a mob
      if (!(mob)) {
         mob = new mob_data;
      }//if
      mob->read(ofile, read_all); 
   }//else
   ofile.getline(tmp, 80);      
   //mudlog.log(DBG, "Done w/read crit.\n");

   if (!is_affected_by(BLINDNESS_SKILL_NUM, *this))
      SEE_BIT |= 1024; //another bletcherous kludge

   return 0;
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
   return !(CRIT_FLAGS.get(25));
}

int critter::haveObjNumbered(int cnt, int obj_num) {
   SCell<object*> cll(inv);
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


void critter::trackToKill(critter& vict, int& is_dead) {
   if ((isMob()) || (vict.isMob())) {
      mudlog.log(ERR, "ERROR:  MOB's in trackToKill.\n");
      return;
   }//f

   if (!mob) {
      mudlog.log(ERR, "ERROR:  pc trying to track_to_kill.\n");
      return;
   }//if

   // Not going to track non-player characters, don't move if sessile
   if (vict.isNpc() || isSessile())
      return;

   remember(vict);

   if (isSentinel())
      return;

   setTrackingTarget(*(vict.getShortName(this)));
   
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
      SCell<critter*> cll(IS_FIGHTING);
      critter* crit_ptr;
      critter* ptr2;
      while ((crit_ptr = cll.next())) {
         ptr2 = this; //compiler issue
         crit_ptr->IS_FIGHTING.loseData(ptr2);
      }//while
      IS_FIGHTING.clear();
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

   leave_room_effects(room_list[cur_room], *this);

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


void critter::breakEarthMeld() {
   SpellDuration* sp;
   if ((sp = is_affected_by(EARTHMELD_SKILL_NUM, *this))) {
      show("Your meld with the earth has been severed.\n");
      affected_by.loseData(sp);
      delete sp;
   }//if
}//breakEarthMeld


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
         if (EQ[i]->isModified()) {
            delete EQ[i];
         }
         EQ[i] = NULL;
      }//if
   }//for

   SCell<object*> cll(inv);
   object* ptr;
   ptr = cll.next();
   while (ptr) {
      if (ptr->getIdNum() == obj->getIdNum()) {
         if (ptr->isModified()) {
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
   if (obj->isModified()) {
      obj->setContainer(this);
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

void critter::show(const char* msg) {
   ::show(msg, *this);
}

void critter::show(CSentryE which_string) {
   show(CSHandler::getString(which_string, getLanguageChoice()));
}

LanguageE critter::getLanguageChoice() const {
   if (pc) {
      return pc->preferred_language;
   }
   return English;
}

void critter::setHpMax(int i) {
   if (i < 0)
      i = 0;
   if (i > 32000)
      i = 32000;
   short_cur_stats[23] = i; //MAX_HP of course...
}

void critter::toStringStat(critter* viewer, String& rslt, ToStringTypeE st) {
   String buf(1000);
   mudlog.log(TRC, __FUNCTION__);
   rslt.clear();

   if (!isInUse()) {
      rslt.append("This critter is UNDEFINED.\n");
      return;
   }//if

   if (viewer->isUsingClient()) {
      Sprintf(rslt, "<CRITTER %i>\n", getIdNum());
   }
   else {
      if (viewer->isImmort()) {
         Sprintf(rslt, "Critter:  %i\n", getIdNum());
      }
   }

   Entity::toStringStat(viewer, buf, st);
   rslt.append(buf);

   Scriptable::toStringStat(viewer, buf);
   rslt.append(buf);

   short_desc.toStringStat("<SHORT_DESC>", "</SHORT_DESC>", viewer, buf);
   rslt.append(buf);

   in_room_desc.toStringStat("<NROOM_DESC>", "</NROOM_DESC>", viewer, buf);
   rslt.append(buf);

   if (viewer->isImmort()) {
      Markup::toString(NULL, crit_flags, CRIT_FLAGS_NAMES, viewer, NULL, buf);
      rslt.append(buf);

      Sprintf(buf, "\nCLASS:  %s(%d)  race:  %s(%d)    Belongs to ZONE:  %i\n", 
              getClassName(viewer), getClass(), getRaceName(viewer), getRace(),
              getNativeZoneNum());
      rslt.append(buf);
      
      Sprintf(buf, "VIS_BIT:  %i, see_bit:  %i, GOLD:  %i, exp: %i XP_WORTH: %i\n",
              getVisBit(), getSeeBit(), getGold(), getExp(), getExp() / EXP_DIVISOR);
      rslt.append(buf);
      
      Sprintf(buf, "POS: %i str: %i  int: %i  con: %i  cha: %i wis: %i  dex: %i\n",
              getPosn(), getStr(), getInt(), getCon(), getCha(), getWis(), getDex());
      rslt.append(buf);
      
      Sprintf(buf, "hit: %i  dam: %i  ac: %i  atks: %i  sex: %i\n",
              getHit(), getDam(), getAC(), getAttacks(),
              getSex() /* if only 'twere so easy!! :)*/);
      rslt.append(buf);
      
      Sprintf(buf, "H: %i  M: %i V: %i  align: %i\n", getHp(), getMana(),
              getMov(), getAlign());
      rslt.append(buf);

      /* done through 18 */
      Sprintf(buf, "lvl: %i  Home_Town: %i  wimpy: %i  Prac: %i Hmx: %i  Mmx: %i\n",
              getLevel(), getHomeTown(), getWimpy(), getPractices(),
              getHpMax(), getManaMax());
      rslt.append(buf);
      
      Sprintf(buf, "Vmx: %i  CRITTER_TYPE: %i  dam_rec_mod: %i  DAM_GIV_MOD: %i\n",
              getMovMax(), CRITTER_TYPE, getDamRcvMod(), getDamGivMod());
      rslt.append(buf);
      
      Sprintf(buf, "heat_resis: %i  COLD_RESIS: %i  elect_resis: %i  SPELL_RESIS: %i\n",
              getHeatResist(), getColdResist(), getElectResist(), getSpellResist());
      rslt.append(buf);
      
      Sprintf(buf, "RELIGION: %i  BH_dice_count: %i  BH_DICE_SIDES: %i\n",
              getReligion(), getBhDiceCount(), getBhDiceSides());
      rslt.append(buf);
      
      Sprintf(buf, "CUR_WEIGHT: %i  max_weight: %i PAUSE: %i HP_rgn: %i  MANA_rgn: %i\n",
              getCurWeight(), getMaxWeight(), getPause(), getHpRegen(), getManaRegen());
      rslt.append(buf);
      
      Sprintf(buf, "MOV_rgn: %i  naked_weight: %i\n\n", getMovRegen(),
              getNakedWeight());
      rslt.append(buf);
   }//if isImmortal

   if (temp_crit) {
      temp_crit->toStringStat(viewer, buf);
      rslt.append(buf);
   }

   if (pc) {
      pc->toStringStat(viewer, buf, st);
      rslt.append(buf);
   }

   if (mob) {
      mob->toStringStat(viewer, buf);
      rslt.append(buf);

      if (shouldBeHoming()) {
         Sprintf(buf, "\t\tHoming to room:  %i\n", getHomeRoom());
         rslt.append(buf);
      }
   }

   if (master) {
      Sprintf(buf, "Master:  %S.\n", master->getName(viewer));
      rslt.append(buf);
   }//if

   if (follower_of) {
      Sprintf(buf, "Following:  %S.\n", follower_of->getName(viewer));
      rslt.append(buf);
   }//if

   if (viewer->isImmort() || (st | ST_LONG)) {
      if (isFighting()) {
         rslt.append("Fighting:\n");
         Markup::toString(&is_fighting, viewer, buf);
         rslt.append(buf);
      }//if
      
      if (!pets.isEmpty()) {
         rslt.append("Pets:\n");
         Markup::toString(&pets, viewer, buf);
         rslt.append(buf);
      }//if
      
      if (!followers.isEmpty()) {
         rslt.append("Followers:\n");
         Markup::toString(&followers, viewer, buf);
         rslt.append(buf);
      }//if
      
      if (!groupees.isEmpty()) {
         rslt.append("Groupees:\n");
         Markup::toString(&groupees, viewer, buf);
         rslt.append(buf);
      }//if
   }

   if (viewer->isUsingClient()) {
      rslt.append("</CRITTER>\n");
   }
}//toStringStat

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
   LString buf(100);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In doSuicide, pc:  " << *(getName()) << endl;
   }

   buf = *(getName());
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

void critter::emote(CSelectorColl& includes, CSelectorColl& denies,
                    CSentryE cs_entry, .../*Sprintf args*/) {
   va_list argp;
   va_start(argp, cs_entry);
   getCurRoom()->vDoEmote(*this, includes, denies, cs_entry, argp);
   va_end(argp);
}//emote


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
               targ.Append(getHpMax());
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

   if (isUsingClient()) {
      // HP, HP-MAX, MANA, MANA-MAX, MOV, MOV-MAX
      Sprintf(targ, "<PROMPT %i %i %i %i %i %i> ", // the trailing space is important.
              getHp(), getHpMax(), getMana(), getManaMax(),
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



int critter::getImmLevel() {
   if (pc && pc->imm_data)
      return pc->imm_data->imm_level;

   return 0;
}


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
   char ch;
   if (pc && (pc->drugged > 0)) {
      for (int i = 0; i<msg.Strlen(); i++) {
         if ((msg.charAt(i) != '<') && (d(1, 15) < d(1, pc->drugged))) {
            ch = (char)(d(1, 52) + 41);

            // Don't want to translate < in case of hegemon markup messups.
            if (ch == '<') {
               ch = '!';
            }
            msg.setCharAt(i, ch);
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
   return (dr.isOwnedBy(*this));
}

int critter::doesOwnDoor(door_data& dd) {
   return (ZoneCollection::instance().elementAt(dd.getZoneNum()).isOwnedBy(*this));
}

String* critter::getName(critter* viewer) {
   if (!viewer->canDetect(*this)) {
      return &SOMEONE;  //global string
   }
   if (isPc()) { //is a pc
      String* tmp = names.getFirstKeyword(viewer->getLanguage());
      if (tmp) {
         return tmp;
      }
      else {
         return &SOMEONE;
      }
   }//if
   else {
      return short_desc.getString(viewer);
   }//else
}//getName


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

int critter::hasAI() {
   return (mob && mob->proc_data && FLAG1.get(13));
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
      mudlog.log(ERR, "ERROR:  bad pc sent to do_hunt_proc.\n");
      return;
   }//if

   // If yer sessile, then you can't hunt!
   if (isSessile()) {
      return;
   }

   critter* targ = have_crit_named(pc_list, 1, getTrackingTarget(),
                                   NULL);

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
	 mudlog.log(ERR, "ERROR:  targ is MOB in do_hunt_proc.\n");
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

   // Yeesh, what a dereference :)
   sink.mob->proc_data->sh_data->ps_data_list.clearAndDestroy();

   Cell<PlayerShopData*> cll(mob->proc_data->sh_data->ps_data_list);
   PlayerShopData* ptr;

   while ((ptr = cll.next())) {
      sink.mob->proc_data->sh_data->ps_data_list.append(new PlayerShopData(*ptr));
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
   Sprintf(buf, "Your have only %i coins.", long_data[0]);

   do_tell(banker, buf, *this, FALSE, getCurRoomNum());
   return -1;
}

/**  Translates an asci string (like HP, MANA, MOV, ALIGN, etc)
 * and returns the integer value.  Returns 0 if it can't
 * resolve the field.
 */
int critter::getFieldValue(const char* field) const {
   if (strcasecmp(field, "HP") == 0) {
      return getHp();
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
   else {
      return 0;
   }
}

const char* critter::getSexName(critter* viewer) const {
   if (SEX == 0)
      return cstr(CS_FEMALE, *viewer);
   else if (SEX == 1) 
      return cstr(CS_MALE, *viewer);
   else if (SEX == 2)
      return cstr(CS_NEUTER, *viewer);
   return "";
}//getSexName


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
      gold = obj_to_sobj(obj_list[GOLD_OBJECT], getCurRoom());
      
      obj_list[GOLD_OBJECT].incrementCurInGame();
      
      gold->cur_stats[1] = cnt;
      GOLD -= cnt;
      
      getCurRoom()->gainObject(gold);
      return 0;
   }//all is well
}//doDropCoins



void critter::doDecreaseTimedAffecting() {
   Cell<SpellDuration*> sd_cell;
   SpellDuration* sd_ptr;
   String buf(100);
   
   if (isPc()) {
      if ((++(pc->idle_ticks) > 30) && 
          (!pc->imm_data || (IMM_LEVEL < 2))) {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "Logging off player in decrease_timed_affecting_pcs,"
                   << " name:  " << *(getName(~0))
                   << "  address:  " << this << "  ticks:  "
                   << pc->idle_ticks << endl;
         }
         log_out(*this);
      }//if
      
      if (TRUE /*crit_ptr->pc->mode == MODE_NORMAL*/) {
         getAffectedBy().head(sd_cell);
         sd_ptr = sd_cell.next();
         while (sd_ptr) {
            if (sd_ptr->duration != -1)
               sd_ptr->duration--;
            if (sd_ptr->duration == 0) {
               rem_effects_crit(sd_ptr->spell, *this, TRUE);
               delete sd_ptr;
               sd_ptr = getAffectedBy().lose(sd_cell);
            }//if
            else {
               sd_ptr = sd_cell.next();
            }
         }//while
         
         if (HUNGER > 0)
            HUNGER--;  //food
         if (THIRST > 0)
            THIRST--;  //drink
         if (DRUGGED > 0)
            DRUGGED--;  //drugged
         
         if (MODE == MODE_NORMAL) {
            if (HUNGER == 0)
               show("You are famished.\n");
            if (THIRST == 0)
               show("You are thirsty.\n");
            if (DRUGGED == 0) {
               show("You no longer feel drugged.\n");
               DRUGGED = -1;
            }//if
         }//if
      }//if a PC

      // These work for both NPC's and PCs

      /* check for lights about to go out */
      if (EQ[11]) {
         if (!EQ[11]->isModified()) {
            EQ[11] = obj_to_sobj(*(EQ[11]), this);
         }//if
         if (EQ[11]->extras[0] == 1) {
            Sprintf(buf, "%S flickers.\n", EQ[11]->getLongName(this));
            buf.Cap();
            show(buf);
         }//if
      }//if
      
      /* check for lights gone out */
      if (EQ[11]) {
         if (EQ[11]->extras[0] == 0) {
            EQ[11]->extras[0] = -2;
            Sprintf(buf, "%S dims and glows its last.\n", 
                    EQ[11]->getLongName(this));
            buf.Cap();
            show(buf);
            crit_flags.turn_off(USING_LIGHT_SOURCE);
            room_list[getCurRoomNum()].checkLight(FALSE);
            EQ[11]->appendShortDesc(CS_OUT);
            EQ[11]->appendInRoomDesc(CS_OUT);
         }//if
      }//if
   }//if
}//doDecreaseTimedAffecting

const char* critter::getRaceName(critter* viewer) {
   if (viewer) {
      return get_race_name(RACE);
   }
   else {
      //  TODO:  involve viewer
      return get_race_name(RACE);
   }
}

const char* critter::getClassName(critter* viewer) {
   if (!viewer) {
      return get_class_name(CLASS);
   }
   else {
      //TODO:  Incorporate viewer
      return get_class_name(CLASS);
   }
}//class_of_crit
