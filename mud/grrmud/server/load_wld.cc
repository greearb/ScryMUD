// $Id: load_wld.cc,v 1.6 1999/07/18 21:16:18 greear Exp $
// $Revision: 1.6 $  $Author: greear $ $Date: 1999/07/18 21:16:18 $

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

   if (depth > 50) {
      mudlog << "WARNING:  reached max depth in recursive_init_loads(obj)"
             << endl;
      return;
   }

   Cell<object*> cll(obj.inv);
   object* ptr;
   while ((ptr = cll.next())) {
      recursive_init_loads(*ptr, ++depth);
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

   if (depth > 50) {
      mudlog << "WARNING:  reached max depth in recursive_init_unload(obj)"
             << endl;
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


void read_setup() {
   String buf(100);

   ifstream file("./grrmud.ini");
   if (!file) {
      mudlog << "ERROR:  ./grrmud.ini not opened correctly." << endl;
      do_shutdown = TRUE;
      exit(100);
   }//if

   file >> MAX_DESCRIPTORS_AVAILABLE;
   buf.Getline(file, 80);

   file >> Boot_Load_Modifier;
   buf.Getline(file, 80);

   file >> Regular_Load_Modifier;
   buf.Getline(file, 80);

   file >> Year;
   buf.Getline(file, 80);

   file >> Day;
   buf.Getline(file, 80);

   file >> Hour;
   buf.Getline(file, 80);

   file >> MAX_PLAYERS;
   buf.Getline(file, 80);

   file >> DFLT_PORT;
   buf.Getline(file, 80);

   file >> OPT_USEC;
   buf.Getline(file, 80);

   CONVERT_WORLD_FROM_DEV = FALSE;
   file >> CONVERT_WORLD_FROM_DEV;
   buf.Getline(file, 80);

   file >> EXP_DIVISOR;
   buf.Getline(file, 80);

}//read_setup; 


void write_setup() {
   ofstream file("./grrmud.ini");
   if (!file) {
      mudlog << "ERROR:  ./grrmud.ini not opened correctly for write."
             << endl;
      do_shutdown = TRUE;
      exit(100);
   }//if

   file << MAX_DESCRIPTORS_AVAILABLE;
   file << "\t\t\tMAX_DESCRIPTORS_AVAILABLE\n";

   file << Boot_Load_Modifier;
   file << "\t\t\tBoot_Load_Modifier\n";

   file << Regular_Load_Modifier;
   file << "\t\t\tRegular_Load_Modifier\n";

   file << Year;
   file << "\t\t\tYear\n";

   file << Day;
   file << "\t\t\tDay\n";

   file << Hour;
   file << "\t\t\tHour\n";

   file << MAX_PLAYERS;
   file << "\t\t\tMAX_PLAYERS\n";

   file << DFLT_PORT;
   file << "\t\t\tDFLT_PORT\n";

   file << OPT_USEC;
   file << "\t\t\tOPT_USEC\n";

   file << CONVERT_WORLD_FROM_DEV << "\t\t\tCONVERT_WORLD_FROM_DEV" << endl;

   file << EXP_DIVISOR << "\t\tEXP_DIVISOR, should be around 100-150" << endl;

}//write_setup;



void load_wld() {
   int i;
   String buf(100);

   ZoneCollection::instance().readSelf();

   for (i = 0; i < NUMBER_OF_ZONES; i++ ) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) {
         load_objects(i);
         for (int j = 0; j<NUMBER_OF_ITEMS; j++) {
            obj_list[j].setIdNum(j);
         }//for
      }//if
   }//for         

   mudlog.log(WRN, "ALL OBJECTS LOADED.\n");
   for (i = 0; i < NUMBER_OF_ZONES; i++ ) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) {
         load_doors(i);
         for (int j = 0; j<NUMBER_OF_DOORS; j++) {
            door_list[j].setIdNum(j);
         }//for
      }//if
   }//for         

   mudlog.log(WRN, "ALL DOORS LOADED.\n");
   for (i = 0; i < NUMBER_OF_ZONES; i++ ) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) {
         load_critters(i);
         for (int j = 0; j<NUMBER_OF_MOBS; j++) {
            mob_list[j].setIdNum(j);
         }//for
      }//if
   }//for         
   mudlog.log(WRN, "ALL CRITTERS LOADED.\n");

   for (i = 0; i < NUMBER_OF_ZONES; i++ ) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) {
         load_zone(i);
      }//if
      // No need to initialize room numbers here.
   }//for         
   mudlog.log(WRN, "ALL ROOMs LOADED.\n");

   load_boards();  //add messages to all the boards
   load_skill_spells();  //skill/spell DB info

   cout << "Max_Obj# is:  " << Cur_Max_Obj_Num << endl;
   cout << "Max_Door# is:  " << Cur_Max_Door_Num << endl;
   cout << "Max_Crit# is:  " << Cur_Max_Crit_Num << endl;
   cout << "Max_Room# is:  " << Cur_Max_Room_Num << endl;
   
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

   cout << "The world has been constructed at least somewhat successfully!"
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
   String buf(50);
   int j;

   for (int i = 0; i<NUMBER_OF_ZONES; i++) {
      if (ZoneCollection::instance().elementAt(i).isInUse()) { //if exists
         Sprintf(buf, "./World/zone_%i", i);
         ofstream rfile(buf);
	 if (!rfile) {
            mudlog.log(ERR, 
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
              room_list[j].Write(rfile);
	   }//if
	 }//for
	 rfile << "\n\n" << -1 << "\t\tEND OF FILE MARKER\n" << flush;
      }//if
   }//for
}//write_all_zones

 

void load_skill_spells() {
   SSCollection::instance().read();
}//load_skill_spells()


void load_zone(int zone_num) {
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
         mudlog.log(ERR, buf);
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

      room_list[k].Read(rfile, FALSE);
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
	       new_obj->Read(rfile, FALSE);
/*
                cout << "Names:  ";
                Cell<String*> nmcll(new_obj->ob->names);
                String* ssptr;
                while ((ssptr = nmcll.next())) {
                   cout << *ssptr << " ";
                }
*/
               Cell<object*> sort_cll(obj_list[i].inv);
               object* tmp_optr;
               int inserted = FALSE;
               while ((tmp_optr = sort_cll.next())) {
                  //cout << *(tmp_optr->ob->names.peekBack()) << " "
                  //     << atoi(*(tmp_optr->ob->names.peekBack()))
                  //     << " " << *(new_obj->ob->names.peekFront()) << " "
                  //     << atoi(*(new_obj->ob->names.peekFront())) << endl;
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


void load_objects(int for_zone) {
  
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
     
      obj_list[k].Read(rfile, FALSE);
      normalize_obj(obj_list[k]); //make it normal as possible
      obj_list[k].OBJ_FLAGS.turn_off(70);   //doesn't need resetting
      obj_list[k].OBJ_IN_ZONE = for_zone;

      rfile >> k;
      rfile.getline(buf, 80);
   }//while, the big loop, reads in a whole room
}//load_items



void load_critters(int for_zone) {
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

      mob_list[k].Read(rfile, FALSE);
      if (mob_list[k].mob) {
         mob_list[k].MOB_FLAGS.turn_off(4); //init to !need_resetting
      }//if

      rfile >> k;
      rfile.getline(buf, 80);
   }//while, the big loop, reads in a whole critter
}//load_critters



void load_doors(int for_zone) {
  
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
    
      door_list[k].Read(rfile);

      rfile >> k;
      rfile.getline(temp_str, 80);
   }//while, the big loop, reads in a whole room
}//load_doors
