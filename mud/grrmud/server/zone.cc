// $Id$
// $Revision: 1.7 $  $Author$ $Date$

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
#ifdef USEMYSQL
#include "mysql/mysql.h"
#endif

#include "zone.h"
#include "const.h"
#include "misc.h"
#include "misc2.h"
#include "command4.h"
#include "command5.h"


int ZoneList::_cnt = 0; //instance count
extern String UNKNOWN;

ZoneList& ZoneList::instance() {
   static ZoneList self;
   return self;
}

void ZoneList::readSelf() {
#ifdef USEMYSQL
   if (config.useMySQL)
      dbReadSelf();
   else
#endif
      fileReadSelf();
}

#ifdef USEMYSQL
void ZoneList::dbReadSelf() {
   long i;
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select ZONE_NUM from Zones";

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In ZoneList::dbReadSelf():\n";
            mudlog << "Eror retrieving query results: " << mysql_error(database)
                   << endl;
         }
         return;
      }
      while ((row=mysql_fetch_row(result))) {
         i=atol(row[0]);
         if ((i >= 0) && (i < NUMBER_OF_ZONES)) {
            add(i);
         }
         else {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In ZoneList::dbReadSelf():\n";
               mudlog << "Bad zone number " << i << " in Zones table. You may\n"
                      << "need to increase NUMBER_OF_ZONES in const.h, then\n"
                      << "recompile.\n";
               return;
            }
         }
      }
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In ZoneList::dbReadSelf():\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
   }
}
#endif

void ZoneList::fileReadSelf() {
   ifstream dafile("./World/ENABLED_ZONES");
   int i;
   if (dafile) {
      dafile >> i;
      while (i != -1) {
         if ((i >= 0) && (i < NUMBER_OF_ZONES)) {
            add(i);
         }//if
         dafile >> i;
      }//while
   }//if
}//read()

void ZoneList::writeSelf() {
   // In theory, writing the Zones collection will take care of this
   // in the MySQL database.
   fileWriteSelf();
}

void ZoneList::fileWriteSelf() {
   ofstream dafile("./World/ENABLED_ZONES");
   Cell<int*> cll(nums);
   int* ptr;

   while ((ptr = cll.next())) {
      dafile << *ptr << endl;
   }
   dafile << "-1";
}//write

void ZoneList::add(int i) {
   if (!contains(i)) {
      nums.append(new int(i));
   }//if
}//add

void ZoneList::remove(int i) {
   Cell<int*> cll(nums);
   int* ptr;

   while ((ptr = cll.next())) {
      if (*ptr == i) {
         nums.lose(cll);
         return;
      }//if
   }//while
}//remove

int ZoneList::contains(int i) {
   Cell<int*> cll(nums);
   int* ptr;

   while ((ptr = cll.next())) {
      if (*ptr == i) {
         return TRUE;
      }//if
   }//while

   return FALSE;
}

String ZoneList::toString() {
   String retval(150);

   Cell<int*> cll(nums);
   int* ptr;

   retval = "These zones are currently open to mortals:\n";

   while ((ptr = cll.next())) {
      retval.Append(*ptr);
      retval.Append(" ");
   }//while

   return retval;
}//toString

void ZoneList::execute() {
   Cell<int*> cll(nums);
   int* ptr;

   while ((ptr = cll.next())) {
      ZoneCollection::instance().zunlock(*ptr);
   }//while
}//execute



int ZoneCollection::_cnt = 0;

ZoneCollection& ZoneCollection::instance() {
   static ZoneCollection self;
   return self;
}

zone& ZoneCollection::elementAt(int i) {
   zone* foo = zone_list.elementAt(i);
   if (!foo)
      return dummy;
   else
      return *foo;
}//elementAt


zone& ZoneCollection::getZoneFor(room& rm) {

   int rm_num = rm.getIdNum();

   for (int i = 0; i<NUMBER_OF_ZONES; i++) {
      if ((zone_list[i].getBeginRoomNum() <= rm_num) && 
          (zone_list[i].getEndRoomNum() >= rm_num)) {
         return zone_list[i];
      }//if
   }//for

   if (mudlog.ofLevel(WRN)) {
      mudlog << "WARNING: getZone(room: " << rm.getIdNum()
             << ") returning dummy" << endl;
   }//if

   return dummy;
}//getZoneNum

void ZoneCollection::readSelf() {
   int k;
   char buf[82];

   mudlog.log(DB, "In ZoneCollection::readSelf\n");

   ifstream zfile("./World/ZONE_FILE");
   if (!zfile) {
      mudlog.log(DIS, "ERROR:  ZONE_FILE not opened correctly, fatal.\n");
      do_shutdown = TRUE;
      exit(101);
   }//if

   zfile >> k;
   zfile.getline(buf, 80);
   
   while ((k != -1) && zfile) { //then read it in.
      if (!check_l_range(k, 0, NUMBER_OF_ZONES, mob_list[0], FALSE)) {
         mudlog.log(ERROR, "ERROR:  zone number is out of range, fatal\n");
         do_shutdown = TRUE;
         exit(100);
      }//if
      zone_list[k].Read(zfile, k);
      zfile >> k;
      zfile.getline(buf, 80);
   }//while
}//readSelf


void ZoneCollection::doRegeneration() { //for all areas that need it.
   int i;
   for (i = 0; i<NUMBER_OF_ZONES; i++) {
      if (zone_list.elementAtNoCreate(i)) {
         if (zone_list[i].isInUse()) {
            zone_list[i].decrementTicksTillRegen();
            if (zone_list[i].getTicksTillRegen() <= 0) {
               if (!room_list[zone_list[i].getEndRoomNum()].isZlocked()) {
                  update_objects(i, FALSE);
                  //            log("Objects updated.\n");
                  update_critters(i, FALSE);
                  update_zone(i, FALSE);
               }
               zone_list[i].resetTicksTillRegen();
            }//if
         }//if
      }//if
   }//for
}//do_regeneration_zones


void ZoneCollection::zunlock(int znum) {
   int j;
   if ((znum >= 0) && (znum < NUMBER_OF_ZONES)) {
      for (j = zone_list[znum].getBeginRoomNum();
           j <= zone_list[znum].getEndRoomNum();
           j++) {
         room_list[j].unlock();
      }//for
   }
}//zunlock


void ZoneCollection::zlock(int znum) {
   int j;
   if ((znum >= 0) && (znum < NUMBER_OF_ZONES)) {
      for (j = zone_list[znum].getBeginRoomNum();
           j <= zone_list[znum].getEndRoomNum();
           j++) {
         room_list[j].lock();
      }//for
   }
}//zlock

void ZoneCollection::zlist(critter& pc, int start, int end) {
   String buf(100);

   Cell<String*> cll;
   for (int i = start; i<= end; i++) {
      if (zone_list[i].isInUse()) {
         zone_list[i].stat(pc);
      }//if
      else {
         Sprintf(buf, "[%i]\tTHIS ZONE UNDEFINED.\n", i);
         show(buf, pc);
      }//else
   }//for 
}//zlist

void ZoneCollection::writeWorld(critter& pc) {
   String buf(100);

   for (int i = 0; i<NUMBER_OF_ZONES; i++) {
      if (zone_list[i].isInUse()) {
         if (zone_list[i].canWriteRooms()) {
            do_write_zone(i);
         }
         else {
            Sprintf(buf,
                    "Can't write rooms in zone:  %i, it's being edited.\n",
                    i);
               show(buf, pc);
         }
         
         if (zone_list[i].canWriteMobs()) {
            do_amsave(i);
         }
         else {
            Sprintf(buf,
                    "Can't write mobs in zone: %i, mobs are being edited.\n",
                    i);
            show(buf, pc);
         }
         
         if (zone_list[i].canWriteObjects()) {
            do_aosave(i);
         }
         else {
            Sprintf(buf,
                    "Can't write objs in zone: %i, they're being edited.\n",
                    i);
            show(buf, pc);
         }
         
         if (zone_list[i].canWriteDoors()) {
            do_adsave(i);
         }
         else {
            Sprintf(buf,
                    "Can't write doors in zone: %i, they're being edited.\n",
                    i);
            show(buf, pc);
         }
         
         Sprintf(buf, "Zone %i has been written.\n", i);
         show(buf, pc);
      }//else
      else {
         Sprintf(buf, "Zone %i has not been defined yet.\n", i);
         show(buf, pc);
      }//else
   }//for
}//writeWorld


/* helper function, not directly called by player */
void ZoneCollection::writeSelf() {
#ifdef USEMYSQL
   if (config.useMySQL)
      dbWriteSelf();
   else
#endif
      fileWriteSelf();
}

void ZoneCollection::fileWriteSelf() {

   ofstream dafile("./World/ZONE_FILE");

   for (int i = 0; i<NUMBER_OF_ZONES; i++) {
      if (zone_list[i].isInUse()) {
         dafile << i << "\tBegin of zone\n";
         zone_list[i].fileWrite(dafile);
      }//if
   }//for

   dafile << "-1                END OF ZONE FILE\n" << flush;
}//write_zone_list

#ifdef USEMYSQL
void ZoneCollection::dbWriteSelf() {
   for (int i = 0; i<NUMBER_OF_ZONES; i++) {
      if (zone_list[i].isInUse()) {
         zone_list[i].dbWrite();
      }
   }
}
#endif

void ZoneCollection::createNeatoFiles() {
   String buf(100);

   for (int i = 0; i<NUMBER_OF_ZONES; i++) {
      if (zone_list[i].isInUse()) {
         Sprintf(buf, "./World/zone_%i.nto", i);
         ofstream ofile(buf);
         ofile << zone_list[i].createNeatoMapFile();
      }//if
   }//for
}//createNeatoFiles


void ZoneCollection::createNewZone(critter& pc, int num_ticks, int num_rooms,
                                   const String& name) {
   String buf(100);

           /* find the starting room */
   int start = 0, i = 0;
   for (; i<NUMBER_OF_ZONES; i++) {
      if (zone_list[i].getEndRoomNum() > start) {
         start = zone_list[i].getEndRoomNum();
      }//if
   }//for

   if ((1 + start + num_rooms) > NUMBER_OF_ROOMS) {
      show("ERROR:  there isn't room in the database.\n", pc);
      show("NUMBER_OF_ROOMS needs to be increased in externs.h.\n", pc);
      return;
   }//if

   start += 1;

   int free_zone = -1;
   for (i = 0; i< NUMBER_OF_ZONES; i++) {
      if (!zone_list[i].isInUse()) {
         free_zone = i;
         break;
      }//if
   }//for

   if (free_zone == -1) {
      show("Not enough zones in database.\n", pc);
      show("Increase NUMBER_OF_ZONES in extersn.h.\n", pc);
      return;
   }//if


   if (num_ticks < 10)
      num_ticks = 10;

                /* good to go */
   zone_list[free_zone].setName(name);
   zone_list[free_zone].setTicksInRegenCycle(num_ticks);
   zone_list[free_zone].setTicksTillRegen(num_ticks);
   zone_list[free_zone].setBeginRoomNum(start);
   zone_list[free_zone].setEndRoomNum(start + num_rooms - 1);

   Sprintf(buf, "cp ./World/DEFAULT_DOORS ./World/doors_%i", free_zone);
   system(buf);
   Sprintf(buf, "cp ./World/DEFAULT_DOORS ./World/objects_%i", free_zone);
   system(buf);
   Sprintf(buf, "cp ./World/DEFAULT_DOORS ./World/zone_%i", free_zone);
   system(buf);
   Sprintf(buf, "cp ./World/DEFAULT_DOORS ./World/critters_%i", free_zone);
   system(buf);


   show("New zone created.\n", pc);
   show("Now you should use 'zenable' to specify zone builders.\n", pc);

   writeSelf();
}//createNewZone


///*****************************************************************///
///**********************  zone class  *****************************///

int zone::_cnt = 0;

zone::zone() {
   _cnt++;
   ticks_in_regen_cycle = ticks_till_regen = 0;
   begin_room_num = end_room_num = 0;
   zone_num = 0;
}//default constructor

zone::zone(int id_num) {
   _cnt++;
   ticks_in_regen_cycle = ticks_till_regen = 0;
   begin_room_num = end_room_num = 0;
   zone_num = id_num;
}//default constructor

zone::zone(const zone& src) { //copy constructor
   _cnt++;
   ticks_in_regen_cycle = src.ticks_in_regen_cycle;
   ticks_till_regen = src.ticks_till_regen;
   begin_room_num = src.begin_room_num;
   end_room_num = src.end_room_num;
   zone_num = src.zone_num;
}//copy constructor


zone::~zone() {
   _cnt--;
   owners.clearAndDestroy();
}//~zone


/** create some output that the graph-viz program neato can use to
 * create a horribly complex looking graph of the zone!
 */
String zone::createNeatoMapFile() {
   String retval(10000);
   String buf(100);

   // This will create lots of duplicate entries, but the
   // neato program should deal with it easier than I can!
   Cell<door*> cll;
   door* ptr;

   String cur_room_name(50);
   String dest_room_name(50);

   Sprintf(retval, "graph zone_%i {\n\tpage=\"40,80\";\n\tsize=\"40,80\";\n",
           zone_num);

   for (int i = begin_room_num; i <= end_room_num; i++) {
      if (room_list.elementAtNoCreate(i)) {
         Sprintf(cur_room_name, "[%i] %S", i, &(room_list[i].short_desc));
         //spaceToNewlines(cur_room_name);

         room_list[i].doors.head(cll);
         
         while ((ptr = cll.next())) {

            if (ptr->getDestRoom()) {
               Sprintf(dest_room_name, "[%i] %S",
                       ptr->getDestRoom()->getIdNum(),
                       &(ptr->getDestRoom()->short_desc));
               //spaceToNewlines(dest_room_name);
               
               if (ptr->getDestRoom()->getZoneNum() != zone_num) {
                  Sprintf(buf, "\t\"%S\" -- \"%S\" [color=blue];\n",
                          &cur_room_name, &dest_room_name);
               }
               else {
                  Sprintf(buf, "\t\"%S\" -- \"%S\";\n", &cur_room_name,
                          &dest_room_name);
               }
               
               retval.Append(buf);
            }//if
         }//while
      }//if
   }//for

   retval.Append("}\n");
   return retval;
}//createNeatoFile


void zone::spaceToNewlines(String& str) {
   for (int i = 0; i<str.Strlen(); i++) {
      if (isspace(str[i])) {
         str.setCharAt(i, '\n');
      }//if
   }//for
}//spaceToNewlines

 
void zone::setEndRoomNum(int i) {
   if ((i >= 0) && (i < NUMBER_OF_ROOMS))
      end_room_num = i;
}
 
void zone::setBeginRoomNum(int i) {
   if ((i >= 0) && (i < NUMBER_OF_ROOMS))
      begin_room_num = i;
}


void zone::addOwner(const String& str) {
   owners.append(new String(str));
}//addOwner


int zone::removeOwner(const String& char_name) {
   Cell<String*> cll(owners);
   String* ptr;

   while ((ptr = cll.next())) {
      if (strcasecmp(*ptr, char_name) == 0) {
         String* ptr2 = ptr;
         ptr = owners.lose(cll);
         delete ptr2;
         ptr2 = NULL;
         return TRUE;
      }//if
   }//while

   return FALSE;
}//removeOwner

void zone::stat(critter& pc) {
   String buf(100);
   Cell<String*> cll;
   String* ptr;

   Sprintf(buf, "^B[^b%i^B]  ^Y%S^0  %P35 Begin#  %i, End#  %i, Ticks in Regen:  %i\n",
           zone_num, &(zone_name), begin_room_num, end_room_num,
           ticks_in_regen_cycle);
   show(buf, pc);

   show("    Owners:  ", pc);
   
   owners.head(cll);
   buf = "";
   while ((ptr = cll.next())) {
      buf.Append(*ptr);
      buf.Append(" ");
   }//while
   
   buf.Append("\n");
   pc.show(buf);
}//stat


String& zone::getFirstOwner() { //returns NONE if has none.
   if (owners.isEmpty()) {
      return UNKNOWN;
   }//if
   else {
      return *(owners.peekFront());
   }//else
}//getFirstOwner


int zone::isOwnedBy(critter& pc) {
   if (pc.isImmort()) {
      Cell<String*> cll(owners);
      String* ptr;
      
      while ((ptr = cll.next())) {
         if (strcasecmp(*ptr, *(Top(pc.names))) == 0)
            return TRUE;
      }//wile
   }//if

   return FALSE;
}//isOwnedBy


/** znum will be the number for this zone. */
int zone::Read(ifstream& dafile, int znum) {
   mudlog.log(DBG, "In zone::read.\n");
   char buf[81];

   Clear();

   if ((znum < 0) || (znum > NUMBER_OF_ZONES)) {
      mudlog << "ERROR:  znum is out of range: " << znum << endl;
      zone_num = 0;
   }//if
   else {
      zone_num = znum;
   }

   if (!dafile) {
      if (mudlog.ofLevel(ERROR)) {
         mudlog << "ERROR:  da_file FALSE in obj read." << endl;
      }
      return FALSE;
   }

                /* zone_name */
   dafile.getline(buf, 80);
   zone_name = buf;
        
                /* numeric data */
   dafile >> ticks_in_regen_cycle >> ticks_till_regen >>
                    begin_room_num >> end_room_num;
   if ((ticks_till_regen < 0) || (ticks_till_regen > ticks_in_regen_cycle)) {
      ticks_till_regen = ticks_in_regen_cycle;
   }
   dafile.getline(buf, 80);  //clear junk

   //mudlog.log(DBG, "About to read zone_flags.\n");
   zone_flags.Read(dafile);
   //mudlog.log(DBG, "read em.\n");

   short test = TRUE;
   while (test) {

      if (!dafile) {
         if (mudlog.ofLevel(ERROR)) {
            mudlog << "ERROR:  da_file FALSE in zone read." << endl;
         }
         return FALSE;
      }

      dafile >> buf;
      mudlog.log(DBG, buf);
      if (strcmp(buf, "~") == 0) {
         test = FALSE;
      }//if
      else {
         owners.append(new String(buf));
      }//else
   }//while            

   dafile.getline(buf, 80); //junk stuff at end of names

   dafile.getline(buf, 80); //junk extra line
   return TRUE;
}//Read


int zone::fileWrite(ofstream& dafile) {
   Cell<String*> st_cell(owners);
   String* st_ptr;

   dafile << zone_name << endl;
   dafile << ticks_in_regen_cycle << " " << ticks_till_regen << " "
          << begin_room_num << " " << end_room_num << endl;

   int len = 0;

   zone_flags.Write(dafile);

   while ((st_ptr = st_cell.next())) {
      len += st_ptr->Strlen() + 1;
      if (len > 79) {
         dafile << endl;
         len = 0;
      }//if
      dafile << *st_ptr << " ";
   }//while
   dafile << "~" << "\towners\n\n";
   return true;
}//fileWrite

#ifdef USEMYSQL
int zone::dbWrite() {
   Cell<String*> st_cell(owners);
   String query;
   String* st_ptr;

   query = "DELETE FROM Zones WHERE ZONE_NUM = ";
   query += zone_num;

   if (mysql_real_query(database, query, strlen(query)) !=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In zone::dbWrite():\n";
         mudlog << "Eror executing query: " << mysql_error(database)
                << endl;
      }
      return FALSE;
   }

   query = "DELETE FROM ZoneOwners WHERE ZONE_NUM = ";
   query += zone_num;

   if (mysql_real_query(database, query, strlen(query)) !=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In zone::dbWrite():\n";
         mudlog << "Eror executing query: " << mysql_error(database)
                << endl;
      }
      return FALSE;
   }

   query = "INSERT INTO Zones (ZONE_NUM, TICKS_IN_REGEN_CYCLE, ";
   query += "TICKS_TILL_REGEN, ZONE_NAME, BEG_ROOM_NUM, END_ROOM_NUM) VALUES ";

   query += "("; query += zone_num;             query += ", ";
                 query += ticks_in_regen_cycle; query += ", ";
                 query += ticks_till_regen;     query += ", ";
                 query += zone_name;            query += ", ";
                 query += begin_room_num;       query += ", ";
                 query += end_room_num;
   query += ")";

   if (mysql_real_query(database, query, strlen(query)) !=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In zone::dbWrite():\n";
         mudlog << "Eror executing query: " << mysql_error(database)
                << endl;
      }
      return false;
   }

   query = "INSERT INTO ZoneOwners (ZONE_NUM, OWNER) VALUES ";
   while ((st_ptr = st_cell.next())) {
      query += "(";
      query += zone_num; query += ", ";
      query += *st_ptr;  query += "), ";
   }//while
   query.dropFromEnd(2);
   if (mysql_real_query(database, query, strlen(query)) !=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In zone::dbWrite():\n";
         mudlog << "Eror executing query: " << mysql_error(database)
                << endl;
      }
      return false;
   }

   return true;
}
#endif

void zone::Clear() {
   zone_name.Clear();
   zone_flags.Clear();
   owners.clearAndDestroy();
   ticks_in_regen_cycle = ticks_till_regen = begin_room_num = end_room_num = 0;
   zone_num = 0;
}//clear


int zone::isTotallyLoaded() {
   int i;

   for (i = begin_room_num; i <= end_room_num; i++) {
      if (room_list.elementAtNoCreate(i) && room_list[i].isInUse()) {
         if (room_list[i].isTotalLoaded()) { //if total loaded
            return TRUE;
         }//if
            else {
            return FALSE;
         }//else
      }//if
   }//for

   if (room_list[begin_room_num].isTotalLoaded())
      return TRUE;
   else
      return FALSE;
}//isTotallyLoaded


/**  From mortal intervention, mob procs, vehicle movement.. */
int zone::isLocked() {
   int i;

   for (i = begin_room_num; i <= end_room_num; i++) {
      if (room_list.elementAtNoCreate(i) && room_list[i].isInUse()) {
         if (room_list[i].isZlocked()) { //if total loaded
            return TRUE;
         }//if
            else {
            return FALSE;
         }//else
      }//if
   }//for

   if (room_list[begin_room_num].isZlocked())
      return TRUE;
   else
      return FALSE;
}//isLocked


int zone::canWriteRooms() {
   int i;

   for (i = begin_room_num; i <= end_room_num; i++) {
      if (room_list.elementAtNoCreate(i) && room_list[i].isInUse()
          && (room_list[i].isNotComplete())) {
         return FALSE;
      }//if
   }//for

   return TRUE;

}//canWriteRooms



/**  Check to make sure there are no mobs that are only half-way 
 * constructed in OLC.
 */
int zone::canWriteMobs() {
   int i;

   for (i = 0; i < NUMBER_OF_MOBS; i++) {
      if (mob_list[i].isInUse() && mob_list[i].isNotComplete()
          && (mob_list[i].getNativeZoneNum() == zone_num)) {
         return FALSE;
      }//if
   }//for
   return TRUE;
}//canWriteMobs


/**  Check to make sure there are no doors that are only half-way 
 * constructed in OLC.
 */
int zone::canWriteDoors() {
   int i;

   for (i = 0; i < NUMBER_OF_DOORS; i++) {
      if (door_list[i].isInUse() && door_list[i].isNotComplete()
          && (door_list[i].getZoneNum() == zone_num)) {
         return FALSE;
      }//if
   }//for
   return TRUE;
}//canWriteDoors


/**  Check to make sure there are no objects that are only half-way 
 * constructed in OLC.
 */
int zone::canWriteObjects() {
   int i;

   for (i = 0; i < NUMBER_OF_ITEMS; i++) {
      if (obj_list[i].isInUse() && obj_list[i].isNotComplete()
          && (obj_list[i].getZoneNum() == zone_num)) {
         return FALSE;
      }//if
   }//for
   return TRUE;
}//canWriteObjects

