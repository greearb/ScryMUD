// $Id$
// $Revision: 1.11 $  $Author$ $Date$

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

#include "misc.h"
#include "commands.h"
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include "classes.h"
#include "olc2.h"
#include "load_wld.h"
#include <PtrArray.h>
#include "vehicle.h"
#include "SkillSpell.h"


void recursive_init_loads(object& obj, int depth) {
   obj_list[obj.OBJ_NUM].incrementCurInGame();

   if (depth > 80) {
      mudlog << "ERROR:  reached max depth in recursive_init_loads(obj), obj: "
             << obj.getIdNum() << endl;
      return;
   }

   Cell<object*> cll(obj.inv);
   object* ptr;
   while ((ptr = cll.next())) {
      recursive_init_loads(*ptr, depth+1);
   }//while
}//recursive object loads


// Only do objects now, critters are taken care of as they are put
// into a room.
void recursive_init_loads(critter& mob) {

   for(int i = 1; i<MAX_EQ; i++) {
      if (mob.EQ[i])
         recursive_init_loads(*(mob.EQ[i]), 0);
   }//for

   Cell<object*> cll(mob.inv);
   object* ptr;
   while ((ptr = cll.next())) {
      recursive_init_loads(*ptr, 0);
   }//while
}//recursive mob loads



void recursive_init_unload(object& obj, int depth) {
   obj.decrementCurInGame();

   if (depth > 80) {
      mudlog << "WARNING:  reached max depth in recursive_init_unload(obj), obj: "
             << obj.getIdNum() << endl;
      return;
   }

   Cell<object*> cll(obj.inv);
   object* ptr;
   while ((ptr = cll.next())) {
      recursive_init_unload(*ptr, ++depth);
   }//while
}//recursive object unloads


// Only do objects now, critters are taken care of as they are put
// into a room.
void recursive_init_unload(critter& mob) {

   for(int i = 1; i<MAX_EQ; i++) {
      if (mob.EQ[i])
         recursive_init_unload(*(mob.EQ[i]), 0);
   }//for

   Cell<object*> cll(mob.inv);
   object* ptr;
   while ((ptr = cll.next())) {
      recursive_init_unload(*ptr, 0);
   }//while
}//recursive mob unload


void init_loads() { //this takes a LOT of time, use sparingly (once I hope)
   int i;
   /* this assumes cur_in_game is zero to start with */

   // Zero these guys out...
   for (i = 0; i<NUMBER_OF_MOBS; i++) {
      if (mob_list[i].isInUse()) {
         mob_list[i].setCurInGame(0);
      }//if
   }

   // Zero out the objects...
   for (i = 0; i<NUMBER_OF_ITEMS; i++) {
      if (obj_list[i].isInUse()) {
         obj_list[i].setCurInGame(0);
      }
   }

   for (i = 0; i<NUMBER_OF_ROOMS; i++) {
      if (room_list.elementAtNoCreate(i)) {
         room_list[i].recursivelyLoad();
      }//if
   }//for
}//init_loads


void readSiteBanned() {
   ifstream file("./World/SITE_BANNED");
   if (!file) {
      mudlog << "WARNING:  Creating a new ./World/SITE_BANNED file.\n";
      system("echo \"~\" > ./World/SITE_BANNED");
   }//if

   String buf(100);

   while (TRUE) {
      buf.Getline(file, 98);
      buf.Strip();
      if ((buf == "~") || (!file))
         break;
      if (buf.Strlen() > 0)
         banned_hosts.append(new String(buf));
   }//while
}//readSiteBanned


void writeSiteBanned() {
   ofstream file("./World/SITE_BANNED");
   if (!file) {
      mudlog << "ERROR:  Could NOT open ./World/SITE_BANNED file to write."
             << endl;
      return;
   }//if

   Cell<String*> cll(banned_hosts);
   String* ptr;

   while ((ptr = cll.next())) {
      file << *ptr << endl;
   }
   file << "~" << endl;
}//writeSiteBanned


void load_wld() {
   int i;
   String buf(100);

   ZoneCollection::instance().readSelf();

   for (i = 0; i < NUMBER_OF_ZONES; i++ ) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) {
         load_objects(i, ZoneCollection::instance().elementAt(i).isLocked());
      }//if
   }//for         
   for (int j = 0; j<NUMBER_OF_ITEMS; j++) {
      obj_list[j].setIdNum(j);
   }//for
   mudlog.log(WRN, "ALL OBJECTS LOADED.\n");

   for (i = 0; i < NUMBER_OF_ZONES; i++ ) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) {
         load_doors(i);
      }//if
   }//for         
   for (int j = 0; j<NUMBER_OF_DOORS; j++) {
      door_list[j].setIdNum(j);
   }//for
   mudlog.log(WRN, "ALL DOORS LOADED.\n");

   for (i = 0; i < NUMBER_OF_ZONES; i++ ) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) {
         load_critters(i, ZoneCollection::instance().elementAt(i).isLocked());
      }//if
   }//for         
   for (int j = 0; j<NUMBER_OF_MOBS; j++) {
      mob_list[j].setIdNum(j);
   }//for
   mudlog.log(WRN, "ALL CRITTERS LOADED.\n");

   for (i = 0; i < NUMBER_OF_ZONES; i++ ) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) {
         load_zone(i, ZoneCollection::instance().elementAt(i).isLocked());
      }//if
      // No need to initialize room numbers here.
   }//for         
   mudlog.log(WRN, "ALL ROOMs LOADED.\n");

   load_boards();  //add messages to all the boards
   load_skill_spells();  //skill/spell DB info

   mudlog << "Max_Obj# is:  " << Cur_Max_Obj_Num << endl;
   mudlog << "Max_Door# is:  " << Cur_Max_Door_Num << endl;
   mudlog << "Max_Crit# is:  " << Cur_Max_Crit_Num << endl;
   mudlog << "Max_Room# is:  " << Cur_Max_Room_Num << endl;
   
   init_loads(); /* calculate how many of each item is in game.
                  *these values are stored in the CUR_IN_GAME fields
                  *as of now, shopkeeper's permanent inventory is
                  * not counted. */

   // Write out addresses of all objects in the object list.
   for (int i = 0; i<Cur_Max_Crit_Num; i++) {
      if (obj_list[i].isInUse()) {
         obj_ptr_log << "OBJ_LST " << i << " " << &(obj_list[i]) << "\n";
      }//if
   }//for

   // Write out addresses of all critters in the object list.
   for (int i = 0; i<Cur_Max_Crit_Num; i++) {
      if (mob_list[i].isInUse()) {
         obj_ptr_log << "CRI_LST " << i << " " << &(mob_list[i]) << "\n";
      }//if
   }//for


   // Write out addresses of all critters in the object list.
   for (int i = 0; i<Cur_Max_Room_Num; i++) {
      if (room_list.elementAtNoCreate(i)) {
         if (room_list[i].isInUse()) {
            obj_ptr_log << "RM_LST " << i << " " << &(room_list[i]) << "\n";
         }//if
      }//if
   }//for

   init_casting_objs(); //Initialize the SkillSpell list

   mudlog << "The world has been constructed at least somewhat successfully!"
        << endl;

   //write_all_zones(); //this used for updating the WORLD DB
   //write_all_doors();
   //log("DB_UPDATE WORKED\n");
   //do_shutdown = TRUE;
   //exit(100);

}//load_wld


void init_casting_objs() {
   for (int i = 0; i<NUMBER_OF_ITEMS; i++) {
      if (obj_list[i].isInUse()) {
         if (obj_list[i].obj_proc) {
            Cell<stat_spell_cell*> cll(obj_list[i].obj_proc->casts_these_spells);
            stat_spell_cell* ptr;
            while ((ptr = cll.next())) {
               SSCollection::instance().getSS(ptr->stat_spell).addNewCaster(i);
            }//while
         }//if could cast spells
      }//if in use
   }//for all objects
}//init_casting_objects


void write_all_zones() {
   switch (config.useMySQL) {
      #ifdef USEMYSQL
      case true:
         db_write_all_zones();
         break;
      #endif
      case false:
         file_write_all_zones();
         break;
   }
}

#ifdef USEMYSQL
void db_write_all_zones() {
   String buf(50);
   int j;

   for (int i = 0; i<NUMBER_OF_ZONES; i++) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) { //if exists
         Sprintf(buf, "./World/zone_%i", i);

         for (j = ZoneCollection::instance().elementAt(i).getBeginRoomNum();
              j <= ZoneCollection::instance().elementAt(i).getEndRoomNum();
              j++) {
           if (room_list[j].isInUse()) {
              room_list[j].dbWrite();
           }//if
         }//for
      }//if
   }//for
}
#endif

void file_write_all_zones() {
   String buf(50);
   int j;

   for (int i = 0; i<NUMBER_OF_ZONES; i++) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) { //if exists
         Sprintf(buf, "./World/zone_%i", i);
         ofstream rfile(buf);
         if (!rfile) {
            mudlog.log(ERROR, 
                       "ERROR:  rfile not opened in 'write_all_zones'.\n");
            return;
         }//if

         for (j = ZoneCollection::instance().elementAt(i).getBeginRoomNum();
              j <= ZoneCollection::instance().elementAt(i).getEndRoomNum();
              j++) {
           if (room_list[j].isInUse()) {
              if (room_list[j].isVehicle()) {
                 rfile << (j | 0x01000000) <<"\t\tRoom Number\n";
              }
              else {
                 rfile << j <<"\t\tRoom Number\n";
              }
              room_list[j].fileWrite(rfile);
           }//if
         }//for
         rfile << "\n\n" << -1 << "\t\tEND OF FILE MARKER\n" << flush;
      }//if
   }//for
}//write_all_zones

 

void load_skill_spells() {
   SSCollection::instance().read();
}//load_skill_spells()

void load_zone(int zone_num, int read_all) {
#ifdef USEMYSQL
   if (config.useMySQL)
      db_load_zone(zone_num, read_all);
   else
#endif
      file_load_zone(zone_num, read_all);
}

#ifdef USEMYSQL
void db_load_zone(int zone_num, int read_all) {
   int k;
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select ROOM_NUM from Rooms where IN_ZONE=";
   query += zone_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In db_load_zone(int, int):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      while ((row=mysql_fetch_row(result))) {
         k = atoi(row[0]);
         if ((k & ~(0x01000000)) > NUMBER_OF_ROOMS) {
            if (mudlog.ofLevel(ERROR)) {
               mudlog << "In db_load_zone(int, int):\n";
               mudlog << "Room# " << k << "is too high for the room_list.\n"
                      << "Consider increasing the value of NUMBER_OF_ROOMS "
                      << "in const.h\n";
            }
            return;
         }
         if ((k & ~(0x01000000)) > Cur_Max_Room_Num)
            Cur_Max_Room_Num = (k & ~(0x01000000));

         if ((k & 0x01000000)) {
            vehicle* tmp_v = new vehicle;
            tmp_v->dbRead(k, read_all);

            pulsed_proc_rooms.gainData(tmp_v);
            room_list.set(tmp_v, k);
         }
         room_list[k].dbRead(k, read_all);
      }
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In db_load_zone(int, int):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }
}
#endif

void file_load_zone(int zone_num, int read_all) {
   char buf[81];  
   int k = 0;
   String buff(100);
   
   Sprintf(buff, "In load_zone:  %i.\n", zone_num);
   mudlog.log(TRC, buff);

   sprintf(buf, "./World/zone_%i", zone_num);
   ifstream rfile(buf);
   if (!rfile) { 
      String tmp("cp ./World/DEFAULT_ZONE ");
      tmp += buf;
      system(tmp);   //create the file then
      return;
   }//if

   rfile >> k;
   rfile.getline(buf, 80); //junk line
   while ((k != -1) && rfile) {

      // This is embarrassing, but it works!  Don't you just love backwards
      // compatibility!
      if ((k & ~(0x01000000)) > NUMBER_OF_ROOMS) {
         sprintf(buf, "Room# %i is too high for the room_list.\n", k);
         mudlog.log(ERROR, buf);
         exit(100);
      }//if
      if ((k & ~(0x01000000)) > Cur_Max_Room_Num)
         Cur_Max_Room_Num = (k & ~(0x01000000));


      Sprintf(buff, "Reading room number: %i in zone %i.\n", k, zone_num);
      mudlog.log(DBG, buff);


      if ((k & 0x01000000)) { //then it's a vehicle
         k = (k & ~(0x01000000));
         if (mudlog.ofLevel(DB))
            mudlog << "Reading a vehicle number:  " << k << endl;
         vehicle* tmp_v = new vehicle;
         pulsed_proc_rooms.gainData(tmp_v);
         room_list.set(tmp_v, k);
      }
      room_list[k].fileRead(rfile, read_all);
      room_list[k].setRoomNum(k); //just in case

      rfile >> k;

      mudlog << "About to read room:  " << k << endl;

      rfile.getline(buf, 80); //comments after the room number
   }//while, the big loop, reads in a whole room
   
   if (!rfile) {
      mudlog << "Dropped out of reading zone because of !rfile." << endl;
   }
}//load_zone


void load_boards() {
  
   int k, i;
   char buf[100];
   String sbuf(100);
   object* new_obj;

   mudlog.log(TRC, "in load_boards\n");


   for (i = 0; i< NUMBER_OF_ITEMS; i++) {
      if (obj_list[i].OBJ_FLAGS.get(10)) {
         if (obj_list[i].OBJ_FLAGS.get(74)) { //if a board
            sbuf = "./Boards/board_";
            sbuf.Append(i);

            ifstream rfile(sbuf);
            if (!rfile) { 
               String tmp("cp ./World/DEFAULT_OBJECTS ");
               tmp += sbuf;
               system(tmp);   //create the file then
               return;
            }//if

            rfile >> k;
            rfile.getline(buf, 80);  //allows comments after the mob num
            while ((k != -1) && rfile) {
               if (k > NUMBER_OF_ITEMS) {
                  sprintf(buf, "Item# %i is too high for the obj_list.\n", k);
                  mudlog.log(DIS, buf);
                  exit(100);
               }//if
               sprintf(buf, "Reading message Number %i.\n", k);
               mudlog.log(DBG, buf);
               new_obj = new object;
               new_obj->fileRead(rfile, TRUE);

               Cell<object*> sort_cll(obj_list[i].inv);
               object* tmp_optr;
               int inserted = FALSE;
               while ((tmp_optr = sort_cll.next())) {
                  if (atoi(*(tmp_optr->names.peekFront())) > 
                      (atoi(*(new_obj->names.peekFront())))) {
                     inserted = TRUE;
                     obj_list[i].inv.insertBefore(sort_cll, new_obj);
                     break;
                  }//if
               }//while
               if (!inserted) {
                  obj_list[i].inv.append(new_obj);
               }

               new_obj->in_list = &(obj_list[i].inv); //make it SOBJ            
               rfile >> k;
               rfile.getline(buf, 80);
            }//while, the big loop, reads in a whole room
         }//if
      }//if
   }//for
}//load_boardss

void load_objects(int for_zone, int read_all) {
#ifdef USEMYSQL
   if (config.useMySQL)
      db_load_objects(for_zone, read_all);
   else
#endif
      file_load_objects(for_zone, read_all);
}

#ifdef USEMYSQL
void db_load_objects(int for_zone, int read_all) {
   long k;
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select OBJ_NUM, SOBJ_NUM from Objects";

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
               mudlog << "In db_load_objects(int, int):\n";
               mudlog << "Error retrieving query results: "
                      << mysql_error(database) << endl;
            }
            return;
         }
         while ((row=mysql_fetch_row(result))) {
            k=atol(row[1]);
            if (k != -1) {
               // Must be an SOBJ, we'll skip it now and load it when we load the
               // critter or object it's on.
               continue;
            }
            k=atol(row[0]);
            if (k > NUMBER_OF_ITEMS) {
               if (mudlog.ofLevel(ERROR)) {
                  mudlog << "In db_load_objects(int, int): \n";
               mudlog << "Item# " << k << " is too high for the obj_list.\n";
               mudlog << "Consider increasing NUMBER_OF_ITEMS in const.h\n";
               continue;
            }
         }
         if (k > Cur_Max_Obj_Num)
            Cur_Max_Obj_Num = k;

         obj_list[k].dbRead(k, -1, read_all);
         obj_list[k].OBJ_FLAGS.turn_off(70);
         obj_list[k].OBJ_IN_ZONE = for_zone;
      }
   }
}
#endif

void file_load_objects(int for_zone, int read_all) {
  
   int k;
   char buf[100];
   String sbuf(100);

   mudlog.log(TRC, "in load_objects\n");

   sbuf = "./World/objects_";
   sbuf.Append(for_zone);

   ifstream rfile(sbuf);
   if (!rfile) { 
      String tmp("cp ./World/DEFAULT_OBJECTS ");
      tmp += sbuf;
      system(tmp);   //create the file then
      return;
   }//if

   rfile >> k;
   rfile.getline(buf, 80);  //allows comments after the mob num
   while ((k != -1) && rfile) {
      if (k > NUMBER_OF_ITEMS) {
         sprintf(buf, "Item# %i is too high for the obj_list.\n", k);
         mudlog.log(DIS, buf);
         exit(100);
      }//if
      if (k > Cur_Max_Obj_Num)
         Cur_Max_Obj_Num = k;
      sprintf(buf, "Reading Item Number %i.\n", k);
      mudlog.log(DBG, buf);
     
      obj_list[k].fileRead(rfile, read_all);
      //normalize_obj(obj_list[k]); //make it normal as possible
      obj_list[k].OBJ_FLAGS.turn_off(70);   //doesn't need resetting
      obj_list[k].OBJ_IN_ZONE = for_zone;

      rfile >> k;
      rfile.getline(buf, 80);
   }//while, the big loop, reads in a whole room
}//load_items

void load_critters(int for_zone, int read_all) {
#ifdef USEMYSQL
   if (config.useMySQL)
      db_load_critters(for_zone, read_all);
   else
#endif
      file_load_critters(for_zone, read_all);
}

#ifdef USEMYSQL
void db_load_critters(int for_zone, int read_all) {
   int k;
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select MOB_NUMBER from Critters where PC_NUM=0 and FROM_ZONE=";
   query+=for_zone;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In db_load_critters(int, int):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return;
      }
      while ((row=mysql_fetch_row(result))) {
         k=atoi(row[0]);
         if (k > NUMBER_OF_MOBS) {
            if (mudlog.ofLevel(ERROR)) {
               mudlog << "In db_load_critters(int, int):\n";
               mudlog << "Critter# " << k << " is too high for the mob list.\n"
                      << "Consider increasing NUMBER_OF_MOBS in const.h.\n";
            }
            return;
         }
         if (k > Cur_Max_Crit_Num)
            Cur_Max_Crit_Num = k;

         mob_list[k].dbRead(k, 0, read_all);
         if (mob_list[k].mob)
            mob_list[k].MOB_FLAGS.turn_off(4);
      }
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In db_load_critters(int, int):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }
}
#endif

void file_load_critters(int for_zone, int read_all) {
   int k;
   char buf[100];
   String sbuf(100);

   Sprintf(sbuf, "In load_critters:  zone %i\n", for_zone);
   mudlog.log(TRC, sbuf);

   sbuf = "./World/critters_";
   sbuf.Append(for_zone);

   ifstream rfile(sbuf);
   if (!rfile) { 
      String tmp("cp ./World/DEFAULT_CRITTERS ");
      tmp += sbuf;
      system(tmp);   //create the file then
      return;
   }//if

   rfile >> k; //critter number
   rfile.getline(buf, 80);
   while ((k != -1) && rfile) {
      Sprintf(sbuf, "Reading critter#  %i", k);
      mudlog.log(DBG, sbuf);
      if (k > NUMBER_OF_MOBS) {
         sprintf(buf, "Critter# %i is too high for the mob_list.\n", k);
         mudlog.log(DIS, buf);
         exit(100);
      }//if
      if (k > Cur_Max_Crit_Num)
         Cur_Max_Crit_Num = k;

      mob_list[k].fileRead(rfile, read_all);
      if (mob_list[k].mob) {
         mob_list[k].MOB_FLAGS.turn_off(4); //init to !need_resetting
      }//if

      rfile >> k;
      rfile.getline(buf, 80);
   }//while, the big loop, reads in a whole critter
}//load_critters

void load_doors(int for_zone) {
#ifdef USEMYSQL
   if (config.useMySQL)
      db_load_doors(for_zone);
   else
#endif
      file_load_doors(for_zone);
}

#ifdef USEMYSQL
void db_load_doors(int for_zone) {
   long k;
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select DOOR_NUM from Doors";

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
               mudlog << "In db_load_doors(int):\n";
               mudlog << "Error retrieving query results: "
                      << mysql_error(database) << endl;
            }
            return;
         }
         while ((row=mysql_fetch_row(result))) {
            k=atol(row[0]);
            if (k > NUMBER_OF_DOORS) {
               if (mudlog.ofLevel(ERROR)) {
                  mudlog << "In db_load_doors(int): \n";
               mudlog << "Door# " << k << " is too high for the obj_list.\n";
               mudlog << "Consider increasing NUMBER_OF_DOORS in const.h\n";
               continue;
            }
         }
         if (k > Cur_Max_Door_Num)
            Cur_Max_Door_Num = k;

         door_list[k].dbRead(k);
      }
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In db_load_critters(int, int):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }
}
#endif

void file_load_doors(int for_zone) {
  
   int k;
   char temp_str[100];
   String sbuf(100);

   mudlog.log(TRC, "in load_doors\n");

   sbuf = "./World/doors_";
   sbuf.Append(for_zone);
   ifstream rfile(sbuf);
   if (!rfile) { 
      String tmp("cp ./World/DEFAULT_DOORS ");
      tmp += sbuf;
      system(tmp);   //create the file then
      return;
   }//if

   rfile >> k;
   rfile.getline(temp_str, 80);
   while ((k != -1) && rfile) {
      if (k > NUMBER_OF_DOORS) {
         sprintf(temp_str, "Door# %i is too high for the door_list.\n", k);
         mudlog.log(DIS, temp_str);
         exit(100);
      }//if
      if (k > Cur_Max_Door_Num)
         Cur_Max_Door_Num = k;
    
      door_list[k].fileRead(rfile);

      rfile >> k;
      rfile.getline(temp_str, 80);
   }//while, the big loop, reads in a whole room
}//load_doors
