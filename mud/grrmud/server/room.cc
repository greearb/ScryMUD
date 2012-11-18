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
//To contact the maintainer, Edward Roper: edro+scrymud [at] wanfear.net
//

//**********************************************************************//
///************************  room  ********************************///

#ifdef USEMYSQL
#include <mysql/mysql.h>
#endif
#include "door.h"
#include "room.h"
#include "misc.h"
#include "misc2.h"
#include "battle.h"
#include <PtrArray.h>
#include "const.h"
#include "zone.h"
#include "load_wld.h"
#include "Filters.h"
#include <map>
#include "weather.h"

int KeywordPair::_cnt = 0;

void KeywordPair::show(int idx, critter& pc) {
   String buf(100);
   Sprintf(buf, "Keyword [%i] ", idx);
   
   Cell<String*> cll(names);
   String* ptr;

   while ((ptr = cll.next())) {
      buf.Append(*ptr);
      buf.Append(" ");
   }//while

   buf.Append("\n\n");
   pc.show(buf);
   pc.show("\n");
   
   pc.show(desc);
   pc.show("\n");

   if (pc.isUsingClient()) {
      Sprintf(buf, "<KEYWORD %i ", idx);

      names.head(cll);
      while ((ptr = cll.next())) {
         buf.Append(*ptr);
         buf.Append(" ");
      }//while

      buf.Append(">\n");

      pc.show(buf);
      
      pc.show("<KEYWORD_DESC>");
      pc.show(desc);
      pc.show("</KEYWORD_DESC>");
   }//if
}//show
      

int KeywordPair::isNamed(const String* name) {
   Cell<String*> cll(names);
   String* ptr;

   while ((ptr = cll.next())) {
      if (strncasecmp(*name, *ptr, name->Strlen()) == 0) {
         return TRUE;
      }//if
   }//while

   return FALSE;
}//isNamed

void KeywordPair::clear() {
   names.clearAndDestroy();
   desc.Clear();
}

KeywordPair::~KeywordPair() {
   _cnt--;
   clear();
}

KeywordPair::KeywordPair(const KeywordPair& src) {
   _cnt++;
   *this = src;
}

KeywordPair& KeywordPair::operator=(const KeywordPair& rhs) {
   if (&rhs != this) {
      clear();
      
      Cell<String*> cll(rhs.names);
      String* ptr;

      while ((ptr = cll.next())) {
         names.append(new String(*ptr));
      }

      desc = rhs.desc;
   }//if

   return *this;
}//operator=


void KeywordPair::write(ofstream& dafile) {
   Cell<String*> cll(names);
   String* ptr;

   while ((ptr = cll.next())) {
      dafile << *ptr << " ";
   }//while

   dafile << "~" << endl;

   dafile << desc << "\n~\n";
}//write


void KeywordPair::read(ifstream& dafile) {
   String buf(80);
   char buf2[100];

   while (TRUE) {
      dafile >> buf;
      if (strcmp(buf, "~") == 0) {
         dafile.getline(buf2, 79);
         break;
      }
      else {
         names.append(new String(buf));
      }
   }//while

   desc.termedRead(dafile);
}//read   

KeywordPair* room::haveKeyword(int i_th, const String* name) {
   int foo = 0;
   return haveKeyword(i_th, name, foo);
}

KeywordPair* room::haveKeyword(int i_th, const String* name, int& count_sofar) {
   int cnt = 0;
   Cell<KeywordPair*> cll(keywords);
   KeywordPair* ptr;

   while ((ptr = cll.next())) {
      if (ptr->isNamed(name)) {
         cnt++;
         count_sofar++;
         if (cnt == i_th) {
            return ptr;
         }
      }//if
   }//while

   return NULL;
}//haveKeyword


int room::_cnt = 0;

room::room() {
   _cnt++;
   for (int i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = 0;
   }
   setVisBit(1024); //hack, forgot 'normal' vis bit earlier
   pause = 0;
   obj_ptr_log << "RM_CON " << getIdNum() << " " << this << "\n";
} // sub_room constructor

room::room(int rm_num) {
   _cnt++;
   for (int i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = 0;
   }
   cur_stats[2] = rm_num;
   setVisBit(1024); //hack, forgot 'normal' vis bit earlier
   pause = 0;

   obj_ptr_log << "RM_CON_I " << getIdNum() << " " << this << "\n";
} // sub_room constructor


int room::makeReadyForAreaSpell() {
   Cell<critter*> cll(critters);
   critter* ptr;
   
   while ((ptr = cll.next())) {
      ptr->CRIT_FLAGS.turn_off(20);
   }
   return 0;
}

critter* room::findNextSpellCritter() {
   Cell<critter*> cll(critters);
   critter* ptr;

   while ((ptr = cll.next())) {
      if (!ptr->CRIT_FLAGS.get(20)) { //if spell_tested
         ptr->CRIT_FLAGS.turn_on(20);
         return ptr;
      }//if not affected yet
   }//while

   return NULL;
}//findNextSpellCritter


critter* room::findNextProcMob() {
   Cell<critter*> cll(critters);
   critter* ptr;
   
   while ((ptr = cll.next())) {
      if (!(ptr->pc)) {
         if (ptr->mob) {
            if (!ptr->master) {
               if (ptr->MOB_FLAGS.get(3)) {
                  ptr->MOB_FLAGS.turn_off(3);
                  return ptr;
               }//if haven't done procs yet
            }//if
         }//if a mob
      }//needed if obj has BOTH mob and pc data fields
   }//while

   return NULL;
}//findNextProcMob



void room::recursivelyLoad() { //mobs and objects
   Cell<critter*> ccll(critters);
   critter* cptr;
   
   while ((cptr = ccll.next())) {
      recursive_init_loads(*cptr);
      cptr->incrementCurInGame(); //recursive_... no longer does it.
   }//while
   
   Cell<object*> ocll(inv);
   object* optr;
   while ((optr = ocll.next())) {
      recursive_init_loads(*optr, 0);
   }//while
}//recursivelyLoad()


room::room(room& source) {
   _cnt++;
   *this = source; //overloaded = operator
   
   obj_ptr_log << "RM_CC " << getIdNum() << " " << this << "\n";
} // sub_room copy constructor


room::~room() {
   _cnt--;

   obj_ptr_log << "RM_DES " << getIdNum() << " " << this << "\n";

   if (!do_shutdown) {
      affected_rooms.loseData(this);
      pulsed_proc_rooms.loseData(this);
      embattled_rooms.loseData(this);
   }//if

   Clear();
}//sub_room deconstructor


room& room::operator=(room& source) {

   if (this == &source)
      return *this;

   Cell<stat_spell_cell*> cell;
   stat_spell_cell *tmp_stat, *tmp_stat2;
   int i;
   String* string;
   Cell<String*> cll(source.names);
   Cell<object*> obj_cll;
   object* obj_ptr;
   Cell<critter*> crit_cll;
   critter* crit_ptr;
   
   //mudlog.log(DBG, "In rm operator= overload.\n");
   Clear(); //clear this thing out!!
   
   while ((string = cll.next())) {
      names.append(new String(*(string)));
   }//while
   
   short_desc = source.short_desc;
   long_desc = source.long_desc;
   
   room_flags = source.room_flags;

   Cell<KeywordPair*> kcll(source.keywords);
   KeywordPair* kptr;
   while ((kptr = kcll.next())) {
      keywords.append(new KeywordPair(*kptr));
   }
   
   for (i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = source.cur_stats[i];
   }

   affected_by.clearAndDestroy();
   source.affected_by.head(cell);
   while ((tmp_stat = cell.next())) {
      tmp_stat2 = new stat_spell_cell;
      *tmp_stat2 = *tmp_stat; //shallow copy should work 
      affected_by.append(tmp_stat2);
   }//while
   
   source.inv.head(obj_cll);
   while ((obj_ptr = obj_cll.next())) {
      if (!obj_ptr->IN_LIST) { //no multiple ptrs to SOBJ's
         inv.append(obj_ptr);
      }//if
   }//while

   Cell<door*> dcell(source.doors);
   door* dptr;
   while ((dptr = dcell.next())) {
      doors.append(new door(*dptr));
   }//while
   
   source.critters.head(crit_cll);
   while ((crit_ptr = crit_cll.next())) {
      if (crit_ptr->isMob()) { //only copy MOB's
         gainCritter(crit_ptr); //will increment cur_in_game
      }//if
   }//while

   Cell<RoomScript*> script_cll(room_proc_scripts);
   RoomScript* script_ptr;
   while ((script_ptr = script_cll.next())) {
      room_proc_scripts.append(new RoomScript(*script_ptr));
   }

   // Don't copy pending scripts.

   return *this;
}//room::operator= overload


void room::Clear() {
   int i;
   
   if (mudlog.ofLevel(TRC)) {
      mudlog << "room::Clear..." << endl;
   }

   names.clearAndDestroy();
   short_desc.Clear();
   long_desc.Clear();
   
   room_flags.Clear();

   keywords.clearAndDestroy();
   
   for (i = 0; i<ROOM_CUR_STATS; i++) {
      cur_stats[i] = 0;
   }

   affected_by.clearAndDestroy();
   clear_obj_list(inv);

   // Assume here that the mob is only on the affected_mobs list.
   // Otherwise, have to do a better cleanup to get em out of the game
   // cleanly.
   critter* tmp;
   while (!critters.isEmpty()) {
      tmp = critters.peekFront();
      removeCritter(tmp); //decrement cur_in_game
      if (tmp->isSmob()) {
         affected_mobs.loseData(tmp);
         delete tmp;
      }
   }//while

   doors.clearAndDestroy();

   if (mudlog.ofLevel(TRC)) {
      mudlog << "room::Clear...  DONE." << endl;
   }

   pending_scripts.clearAndDestroy();
   room_proc_scripts.clearAndDestroy();
   cur_script = NULL; // points into the pending_scripts
   pause = 0;
}// Clear

#ifdef USEMYSQL
void room::dbRead(int room_num, short read_all) {
   int quantity, obj_num, sobj_num, mob_num;
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select * from Rooms where ROOM_NUM=";
   query += room_num;

   Clear();

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbRead(int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         } // if
         return;
      } // if
      row=mysql_fetch_row(result);
      
      short_desc = row[ROOMTBL_SHORT_DESC];
      long_desc = row[ROOMTBL_LONG_DESC];
      
      cur_stats[0] = atoi(row[ROOMTBL_VIS_BIT]);
      cur_stats[1] = atoi(row[ROOMTBL_MOVEMENT_COST]);
      cur_stats[2] = atoi(row[ROOMTBL_ROOM_NUM]);
      cur_stats[3] = atoi(row[ROOMTBL_FALL_TO]);

      room_flags.set(ROOMFLAG_NO_RESTRICTIONS, atoi(row[ROOMTBL_NO_RESTRICTIONS]));
      room_flags.set(ROOMFLAG_NO_IMM, atoi(row[ROOMTBL_NO_IMM]));
      room_flags.set(ROOMFLAG_NO_GOD, atoi(row[ROOMTBL_NO_GOD]));
      room_flags.set(ROOMFLAG_IS_PERM_DARK, atoi(row[ROOMTBL_IS_PERM_DARK]));
      room_flags.set(ROOMFLAG_SUNLIGHT, atoi(row[ROOMTBL_WEATHER]));
      room_flags.set(ROOMFLAG_NO_SHOUT, atoi(row[ROOMTBL_NO_SHOUT]));
      room_flags.set(ROOMFLAG_NO_MAGICAL_EXIT, atoi(row[ROOMTBL_NO_MAGICAL_EXIT]));
      room_flags.set(ROOMFLAG_IS_HAVEN, atoi(row[ROOMTBL_IS_HAVEN]));
      room_flags.set(ROOMFLAG_NO_PK, atoi(row[ROOMTBL_NO_PK]));
      room_flags.set(ROOMFLAG_NO_MAGIC, atoi(row[ROOMTBL_NO_MAGIC]));
      room_flags.set(ROOMFLAG_NO_MOB, atoi(row[ROOMTBL_NO_MOB]));
      room_flags.set(ROOMFLAG_NO_POTIONS, atoi(row[ROOMTBL_NO_POTIONS]));
      room_flags.set(ROOMFLAG_NO_STAFFS, atoi(row[ROOMTBL_NO_STAFFS]));
      room_flags.set(ROOMFLAG_NO_MORTALS, atoi(row[ROOMTBL_NO_MORTALS]));
      room_flags.set(ROOMFLAG_NORMALLY_DARK, atoi(row[ROOMTBL_NORMALLY_DARK]));
      room_flags.set(ROOMFLAG_SHALLOW_WATER, atoi(row[ROOMTBL_SHALLOW_WATER]));
      room_flags.set(ROOMFLAG_DEEP_WATER, atoi(row[ROOMTBL_DEEP_WATER]));
      room_flags.set(ROOMFLAG_SWAMP, atoi(row[ROOMTBL_SWAMP]));
      room_flags.set(ROOMFLAG_NEED_FLY, atoi(row[ROOMTBL_NEED_FLY]));
      room_flags.set(ROOMFLAG_NEED_BOAT, atoi(row[ROOMTBL_NEED_BOAT]));
      room_flags.set(ROOMFLAG_NEED_CLIMB, atoi(row[ROOMTBL_NEED_CLIMB]));
      room_flags.set(ROOMFLAG_NO_MAGICAL_ENTRY, atoi(row[ROOMTBL_NO_MAGICAL_ENTRY]));
      room_flags.set(ROOMFLAG_NO_VEHICLES, atoi(row[ROOMTBL_NO_VEHICLES]));
      room_flags.set(ROOMFLAG_CRAMPED, atoi(row[ROOMTBL_CRAMPED]));
      room_flags.set(ROOMFLAG_NO_RANGED_ATTACK, atoi(row[ROOMTBL_NO_RANGED_ATTACK]));
      room_flags.set(ROOMFLAG_NEED_DIVE, atoi(row[ROOMTBL_NEED_DIVE]));
      room_flags.set(ROOMFLAG_USED_IN_TRACK, atoi(row[ROOMTBL_USED_IN_TRACK]));
      room_flags.set(ROOMFLAG_CAN_CAMP, atoi(row[ROOMTBL_CAN_CAMP]));
      room_flags.set(ROOMFLAG_HAS_KEYWORDS, atoi(row[ROOMTBL_HAS_KEYWORDS]));
      room_flags.set(ROOMFLAG_NO_WANDERING_MOBS, atoi(row[ROOMTBL_NO_WANDERING_MOBS]));
      room_flags.set(ROOMFLAG_NO_FOREIGN_WANDERING_MOBS, atoi(row[ROOMTBL_NO_FOREIGN_WANDERING_MOBS]));
      room_flags.set(ROOMFLAG_HAS_PROC_SCRIPT, atoi(row[ROOMTBL_HAS_PROC_SCRIPT]));
      mysql_free_result(result);
   } // if
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbRead(int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else
   
   // names
   query="select NAME from RoomNames where ROOM_NUM=";
   query+=room_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbRead(int, short):\n";
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
         mudlog << "In room::dbRead(int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else

   // inventory
   query="select OBJ_NUM, SOBJ_NUM, QUANTITY from RoomInv where ROOM_NUM=";
   query+=room_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbRead(int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         } // if
         return;
      } // if
      while ((row=mysql_fetch_row(result))) {
         obj_num = atoi(row[0]);
         sobj_num = atoi(row[1]);
         quantity = atoi(row[2]);

         for (;quantity > 0; quantity--) {
            if (check_l_range(obj_num, 0, NUMBER_OF_ITEMS, mob_list[0], FALSE)) {
               if (obj_list[obj_num].isInUse()) {
                  if (read_all || ((obj_list[obj_num].OBJ_PRCNT_LOAD *
                                  config.currentLoadModifier)/100) > d(1,100)) {
                     if (obj_list[obj_num].isPlayerOwned()) {
                        object* po_sack = load_player_box(obj_num);
                        if (po_sack) {
                           po_sack->IN_LIST = &(inv);
                           po_sack->setCurRoomNum(getIdNum(), 0);
                           affected_objects.gainData(po_sack);
                           gainInv(po_sack);
                        } // if
                     } // if
                     else {
                        gainInv(&(obj_list[obj_num]));
                     } // else
                  } // if
               } // if
               else {
                  if (mudlog.ofLevel(LS_ERROR)) {
                     mudlog << "In room::dbRead(int, short):\n";
                     mudlog << "ERROR: Trying to load non-existant object"
                            << obj_num << " in room " << room_num << ".\n";
                  } // if
               } // else
            } // if
         } // for
      } // while
      mysql_free_result(result);
   } // if
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbRead(int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else

   // doors
   query="select EXIT_NUM, DOOR_NUM, DESTINATION, DISTANCE from RoomExits ";
   query+="where ROOM_NUM=";
   query+=room_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbRead(int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         } // if
         return;
      } // if

      Cell<door*> cll;
      door* dr_ptr;
      door* walker;
      short did_insert = FALSE;
      while ((row=mysql_fetch_row(result))) {
         dr_ptr = new door;
         dr_ptr->destination = atoi(row[2]);
         dr_ptr->distance = atoi(row[3]);
         dr_ptr->dr_data = &(door_list[atoi(row[1])]);
         dr_ptr->in_room = room_num;

         MYSQL_RES* tmp_res;
         MYSQL_ROW tmp_row;
         String tmp_qry="select SPELL_NUM, DURATION from RoomExitSplls ";
         tmp_qry+="where ROOM_NUM=";
         tmp_qry+=room_num;
         tmp_qry+=" and EXIT_NUM=";
         tmp_qry+=atoi(row[0]);

         if (mysql_real_query(database, tmp_qry, strlen(tmp_qry))==0) {
            if ((tmp_res=mysql_store_result(database))==NULL) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "In door::dbRead(int, short):\n";
                  mudlog << "Error retrieving query results: "
                         << mysql_error(database) << endl;
               } // if
               return;
            } // if
            while ((tmp_row=mysql_fetch_row(result))) {
               dr_ptr->affected_by.append(new stat_spell_cell(atoi(row[0]), atoi(row[1])));
            } // while
         } // if
         else {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In door::dbRead(int, short):\n";
               mudlog << "Error executing query: " << mysql_error(database)
                      << endl;
            } // if
         } // else
         mysql_free_result(tmp_res);

         doors.head(cll);
         String* dr_name = name_of_door(*dr_ptr, ~0);
         String* walk_name;
         while ((walker = cll.next())) {
            walk_name = name_of_door(*walker, ~0);
            if (*walk_name > *dr_name) {
               doors.insertBefore(cll, dr_ptr);
               did_insert=TRUE;
               break;
            } // if
         } // while

         if (!did_insert)
            doors.append(dr_ptr);
      } // while
      mysql_free_result(result);
   } // if
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbRead(int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else

   // critters
   query="select MOB_NUM, QUANTITY from RoomMobs where ROOM_NUM=";
   query+=room_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbRead(int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         } // if
         return;
      } // if
      while ((row=mysql_fetch_row(result))) {
         mob_num = atoi(row[0]);
         if (mob_list[mob_num].isInUse()) {
            if (mob_list[mob_num].isPlayerShopKeeper()) {
               critter* shop_owner = load_player_shop_owner(mob_num);
               if (shop_owner) {
                  shop_owner->setCurRoomNum(room_num);
                  gainCritter(shop_owner);
               } // if
            } // if
            else {
               gainCritter(&(mob_list[mob_num]));
            } // else
         } // if
         else {
            if (mudlog.ofLevel(LS_ERROR)) {
               mudlog << "In room::dbRead(int, short):\n";
               mudlog << "Error: Trying to load non-existant mob" << mob_num
                      << " in room " << room_num << ".\n";
            } // if
         } // else
      } // while
      mysql_free_result(result);
   } // if
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbRead(int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else

   if (room_flags.get(ROOMFLAG_HAS_PROC_SCRIPT)) {
      RoomScript* rs_ptr;
      query="select * from RoomScripts where ROOM_NUM=";
      query+=room_num;

      if (mysql_real_query(database, query, strlen(query))==0) {
         if ((result=mysql_store_result(database))==NULL) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In room::dbRead(int, short):\n";
               mudlog << "Error retrieving query results: "
                      << mysql_error(database) << endl;
            } // if
            return;
         } // if
         while ((row=mysql_fetch_row(result))) {
            String* tmp_disc = new String(row[RMSCRTBL_DISCRIMINATOR]);
            tmp_disc->Strip();
            tmp_disc->Prepend(" ");
            tmp_disc->Append(" ");

            rs_ptr = new RoomScript(*(new String(row[RMSCRTBL_TRIGGER_CMD])),
                                    atoi(row[RMSCRTBL_TARGET]),
                                    atoi(row[RMSCRTBL_ACTOR]),
                                    *tmp_disc,
                                    atoi(row[RMSCRTBL_PRECEDENCE]));
            
            String sbuf(1024);
            sbuf = row[RMSCRTBL_SCRIPT_COMMANDS];

            String* tmp_str;
            while ((tmp_str = sbuf.getUntil(';'))) {
               rs_ptr->appendCmd(*tmp_str);
               delete tmp_str;
            } // while
            rs_ptr->compile();
         } // while
         mysql_free_result(result);
      } // if
      else {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbRead(int, short):\n";
            mudlog << "Error executing query: " << mysql_error(database) << endl;
         } // if
         return;
      } // else
   }

   // spells
   query="select SPELL_NUM, DURATION from RoomSpells where ROOM_NUM=";
   query+=room_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbRead(int, short):\n";
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
         mudlog << "In room::dbRead(int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      } // if
      return;
   } // else

   // keywords
   if (room_flags.get(ROOMFLAG_HAS_KEYWORDS)) {
      query="select KEYWORD_INDEX, DESCRIPTION from RoomKeywords where ROOM_NUM=";
      query+=room_num;

      if (mysql_real_query(database, query, strlen(query))==0) {
         if ((result=mysql_store_result(database))==NULL) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In room::dbRead(int, short):\n";
               mudlog << "Error retrieving query results: "
                      << mysql_error(database) << endl;
            } // if
            return;
         } // if
         while ((row=mysql_fetch_row(result))) {
            KeywordPair* kwd_ptr;
            MYSQL_RES* tmp_res;
            MYSQL_ROW tmp_row;
            String tmp_qry="select KEYWORD from RoomKeynames where ROOM_NUM=";
            tmp_qry+=room_num;
            tmp_qry+=" and KEYWORD_INDEX=";
            tmp_qry+=row[0];
            if (mysql_real_query(database, tmp_qry, strlen(tmp_qry))==0) {
               if ((tmp_res=mysql_store_result(database))==NULL) {
                  if (mudlog.ofLevel(WRN)) {
                     mudlog << "In room::dbRead(int, short):\n";
                     mudlog << "Error retrieving query results: "
                            << mysql_error(database) << endl;
                  } // if
                  return;
               } // if
               kwd_ptr = new KeywordPair();
               while ((tmp_row=mysql_fetch_row(tmp_res))) {
                  kwd_ptr->names.append(new String(tmp_row[0]));
               } // while
               mysql_free_result(tmp_res);
               kwd_ptr->desc = *(new String(row[1]));
            } // if
         } // while
         mysql_free_result(result);
      } // if
      else {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbRead(int, short):\n";
            mudlog << "Error executing query: " << mysql_error(database) << endl;
         } // if
         return;
      } // else
   } // if

   setVisBit(getVisBit() | 1024);
   if (getVisBit() & 1) {
      room_flags.turn_on(14);
   } // if
} // dbRead
#endif

void room::fileRead(ifstream& ofile, short read_all) {
   int i, test = TRUE;
   stat_spell_cell* ss_ptr;
   char tmp[81];
   String tmp_str(80);
   String* string;
   door* dr_ptr;

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room, read_all:  " << read_all << endl;
   }

   Clear();  //stop up any memory leaks etc.

   if (!ofile) {
      if (mudlog.ofLevel(LS_ERROR)) {
         mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
      }
      return;
   }

   while (test) {
      if (!ofile) {
         if (mudlog.ofLevel(LS_ERROR)) {
            mudlog << "ERROR:  da_file FALSE in room read." << endl;
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

   short_desc.termedRead(ofile);
   //mudlog.log(DBG, short_desc);

   long_desc.termedRead(ofile);

   room_flags.Read(ofile);
   room_flags.turn_on(23); //turn on in_use flag for CERTAIN
   if (room_flags.get(21)) { //if zlocked
      read_all = TRUE;
      room_flags.turn_on(22); //it will be totally loaded
   }//if
   setComplete(); //if we can read it, it's complete!

   KeywordPair* kwd_ptr;
   if (room_flags.get(32)) { //if has keywords
      while (TRUE) {
         ofile >> i;
         ofile.getline(tmp, 80);
         if (i != -1) {
            kwd_ptr = new KeywordPair();
            kwd_ptr->read(ofile);
            keywords.append(kwd_ptr);
         }
         else { //read line, then read desc
            break;
         }
      }//while
   }//if

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room, about to do cur_stats.  " << endl;
   }

   
   for (i = 0; i<ROOM_CUR_STATS; i++) {
      ofile >> cur_stats[i];
   }
   ofile.getline(tmp, 80);


                /*  Affected By */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(LS_ERROR)) {
            mudlog << "ERROR:  da_file FALSE in room read." << endl;
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


   if (!affected_by.isEmpty()) {
      // Also place it on the list of rooms to be checked for loss of spell...
      affected_rooms.gainData(this);
   }//if

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room, about to do inventory." << endl;
   }
                /*  Inventory */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(LS_ERROR)) {
            mudlog << "ERROR:  da_file FALSE in room read, inv."
              << endl;
         }
         return;
      }
      if (check_l_range(i, 0, NUMBER_OF_ITEMS, mob_list[0], FALSE)) {
         if (obj_list[i].isInUse()) {
            if (read_all || 
                ((obj_list[i].OBJ_PRCNT_LOAD*config.currentLoadModifier)/100) > 
                d(1,100)) {
               if (obj_list[i].isPlayerOwned()) {
                  object* po_sack = load_player_box(i);
                  if (po_sack) {
                     // Denote it as 'modified', (SOBJ)
                     po_sack->IN_LIST = &(inv);
                     po_sack->setCurRoomNum(getIdNum(), 0);
                     affected_objects.gainData(po_sack);
                     gainInv(po_sack);
                  }
                  else {
                     //put a virgin copy in the room
                     gainInv(&(obj_list[i]));
                  }
               }
               else {
                  gainInv(&(obj_list[i]));    //add it to inventory
               }
//               obj_list[i].OBJ_CUR_IN_GAME++; //increment cur_in_game
            }//if
         }//if
         else {
            Sprintf(tmp_str, 
             "ERROR:  trying to load non_existant inv: %i in room:  %i.\n",
             i, cur_stats[2]);
            mudlog.log(LS_ERROR, tmp_str);
         }//else
      }//if
      ofile >> i;
   }//while
   ofile.getline(tmp, 80);   

                       /*  Doors  */
   Cell<door*> cll;
   door* walker;
   short did_insert = FALSE;
   ofile >> i; //check for terminal value
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(LS_ERROR)) {
            mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
         }
         return;
      }

      dr_ptr = new door;
      dr_ptr->Read(ofile);
      dr_ptr->in_room = cur_stats[2]; //RM_NUM
      doors.head(cll);
      String* dr_name = name_of_door(*dr_ptr, ~0);
      if (!dr_name) {
         mudlog.log(LS_ERROR, "ERROR:  dr_name is NULL.");
      }//if

      String* walk_name;
      while ((walker = cll.next())) {
         walk_name = name_of_door(*walker, ~0);
         if (*walk_name > *dr_name) {
            doors.insertBefore(cll, dr_ptr);
            did_insert = TRUE;
            break;
         }//if
      }//while

      if (!did_insert)
         doors.append(dr_ptr);
      ofile >> i; //check for next terminal value
   }//while 
   ofile.getline(tmp, 80); //clear rest of line

   if (mudlog.ofLevel(DB)) {
      mudlog << "Reading room About to read critters.." << endl;
   }

                      /*  Critters, stored mobl#  */
   ofile >> i;
   while (i != -1) {
      if (!ofile) {
         if (mudlog.ofLevel(LS_ERROR)) {
            mudlog << "ERROR:  da_file FALSE in sub_room read." << endl;
         }
         return;
      }

      if (mob_list[i].isInUse()) {
         if (mob_list[i].isPlayerShopKeeper()) {
            critter* shop_owner = load_player_shop_owner(i);
            if (shop_owner) {
               // it will be a SMOB, but it needs it's room set here.
               shop_owner->setCurRoomNum(getIdNum());
               gainCritter(shop_owner);
            }//if
         }//if
         else { //regular case
            gainCritter(&(mob_list[i])); //this will increment cur_in_game
         }
      }//if
      else {
         Sprintf(tmp_str, 
                 "ERROR:  trying to load non_existant mob: %i, in room: %i\n",
                 i, cur_stats[2]);
         mudlog.log(LS_ERROR, tmp_str);
      }//else
      ofile >> i;
   }//while 
   ofile.getline(tmp, 80); //get comments after mobs...

   /* read procs, if we have them. */
   if (room_flags.get(35)) {
      //mudlog.log("Mob has proc scripts...");
      int sent_;
      RoomScript* ptr;

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
            if (mudlog.ofLevel(LS_ERROR)) {
               mudlog << "ERROR:  mob_data reading script da_file FALSE." << endl;
            }
            return;
         }

         ptr = new RoomScript();
         ptr->read(ofile);
         room_proc_scripts.append(ptr);
         ofile >> sent_;
         ofile.getline(tmp, 80);
         if (mudlog.ofLevel(DB))
            mudlog << "Got rest of line -:" << tmp << ":-" << endl;
      }
   }//if it had proc scripts

   ofile.getline(tmp, 80); //get comments at end of room..

   // Post processing...

   int vb = getVisBit();
   setVisBit(vb | 1024); //hack, forgot a flag :P
   if (vb & 1) {
      room_flags.turn_on(14); //make normally_dark
   }

   if (mudlog.ofLevel(DB)) {
      mudlog << "Done reading room, number:  " << getRoomNum() << endl;
   }
}//Read


stat_spell_cell* room::isAffectedBy(int spell_num) const {
   Cell<stat_spell_cell*> cll(affected_by);
   stat_spell_cell* ptr;
   
   while ((ptr = cll.next())) {
      if (ptr->stat_spell == spell_num)
         return ptr;
   }//while

   return NULL;
}//is_affected_by


void room::checkLight(int do_msg) {
   int vb = getVisBit();
   //mudlog << "In room::checkLight(), id_num: " << getIdNum() << endl;

   if (isPermDark()) {
      //mudlog << "Permdark, trying to make the room dark.\n";
      vb |= 1; //perm dark it is
   }//if
   else if (isNormallyDark() || (isNightTime() && room_flags.get(4))) {
      //normally dark, or outside and night time
      //mudlog << "Normally dark, trying to make the room dark.\n";
      setVisBit(vb | 1);
      if (isAffectedBy(ILLUMINATE_SKILL_NUM)) {
         //mudlog << "Room was ILLUMINATED, making it light." << endl;
         vb &= ~1;
      }//if
      else {
         Cell<critter*> cll(critters);
         critter* crit_ptr;
         while ((crit_ptr = cll.next())) {
            if (crit_ptr->CRIT_FLAGS.get(1)) { //if using_light source 
               //if (mudlog.ofLevel(DBG)) {
               //   mudlog << "Room was ILLUMINATED, making it light." << endl;
               //}

               vb &= ~1;
               break;
            }//if
         }//while
      }//else
   }//if normally_dark
   else {
      vb &= ~1;
   }//else
   
   if (do_msg) {
      if (vb != getVisBit()) {
         if (vb & 1) { //it was changed to dark
            showAllCept("The room becomes dark.\n");
         }//if
         else {
            showAllCept("The room becomes light.\n");
         }//else
      }//if
   }//if do_messages
   setVisBit(vb);
}//checkLight


#ifdef USEMYSQL
void room::dbWrite() {
   String query;
   String fields;
   String values;
   char* buf;

   // Delete the old records
   query="delete from Rooms where ROOM_NUM=";
   query+=cur_stats[2];

   if (mysql_real_query(database, query, strlen(query))!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query="delete from RoomSpells where ROOM_NUM=";
   query+=cur_stats[2];

   if (mysql_real_query(database, query, strlen(query))!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query="delete from RoomExits where ROOM_NUM=";
   query+=cur_stats[2];

   if (mysql_real_query(database, query, strlen(query))!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query="delete from RoomExitSpells where ROOM_NUM=";
   query+=cur_stats[2];

   if (mysql_real_query(database, query, strlen(query))!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query="delete from RoomScripts where ROOM_NUM=";
   query+=cur_stats[2];

   if (mysql_real_query(database, query, strlen(query))!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query="delete from RoomInv where ROOM_NUM=";
   query+=cur_stats[2];

   if (mysql_real_query(database, query, strlen(query))!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query="delete from RoomMobs where ROOM_NUM=";
   query+=cur_stats[2];

   if (mysql_real_query(database, query, strlen(query))!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query="delete from RoomKeywords where ROOM_NUM=";
   query+=cur_stats[2];

   if (mysql_real_query(database, query, strlen(query))!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query="delete from RoomKeynames where ROOM_NUM=";
   query+=cur_stats[2];

   if (mysql_real_query(database, query, strlen(query))!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   query="delete from RoomNames where ROOM_NUM=";
   query+=cur_stats[2];

   if (mysql_real_query(database, query, strlen(query))!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   // Now write out the new data.
   normalize();
   query="insert into Rooms ";
   fields="(ROOM_NUM, SHORT_DESC, LONG_DESC, VIS_BIT, MOVEMENT_COST, FALL_TO, ";
   fields+="IN_ZONE, NO_RESTRICTIONS, NO_IMM, NO_GOD, IS_PERM_DARK, WEATHER, ";
   fields+="NO_SHOUT, NO_MAGICAL_EXIT, IS_HAVEN, NO_PK, NO_MAGIC, NO_MOB, ";
   fields+="NO_POTIONS, NO_STAFFS, NO_MORTALS, NORMALLY_DARK, SHALLOW_WATER, ";
   fields+="DEEP_WATER, SWAMP, NEED_FLY, NEED_BOAT, NO_MAGICAL_ENTRY, ";
   fields+="NO_VEHICLES, CRAMPED, NO_RANGED_ATTACK, NEED_DIVE, NEED_CLIMB, ";
   fields+="USED_IN_TRACK, CAN_CAMP, HAS_KEYWORDS, NO_WANDERING_MOBS, ";
   fields+="NO_FOREIGN_WANDERING_MOBS, HAS_PROC_SCRIPT) ";
   values="values (";

   // ROOM_NUM
   values+=cur_stats[2]+", ";

   // SHORT_DESC, LONG_DESC
   buf=(char*)malloc(short_desc.Strlen()*2+1);
   mysql_escape_string(buf, short_desc, short_desc.Strlen());
   values+=*"'"+buf+*"', ";
   free(buf);
   buf=(char*)malloc(long_desc.Strlen()*2+1);
   mysql_escape_string(buf, long_desc, long_desc.Strlen());
   values+=*"'"+buf+*"', ";
   free(buf);

   // VIS_BIT, MOVEMENT_COST, FALL_TO
   values+=cur_stats[0]+", ";
   values+=cur_stats[1]+", ";
   values+=cur_stats[3]+", ";

   // NO_RESTRICTIONS to HAS_PROC_SCRIPT
   values+=room_flags.get(ROOMFLAG_NO_RESTRICTIONS)+", ";
   values+=room_flags.get(ROOMFLAG_NO_IMM)+", ";
   values+=room_flags.get(ROOMFLAG_NO_GOD)+", ";
   values+=room_flags.get(ROOMFLAG_IS_PERM_DARK)+", ";
   values+=room_flags.get(ROOMFLAG_SUNLIGHT)+", ";
   values+=room_flags.get(ROOMFLAG_NO_SHOUT)+", ";
   values+=room_flags.get(ROOMFLAG_NO_MAGICAL_EXIT)+", ";
   values+=room_flags.get(ROOMFLAG_IS_HAVEN)+", ";
   values+=room_flags.get(ROOMFLAG_NO_PK)+", ";
   values+=room_flags.get(ROOMFLAG_NO_MAGIC)+", ";
   values+=room_flags.get(ROOMFLAG_NO_MOB)+", ";
   values+=room_flags.get(ROOMFLAG_NO_POTIONS)+", ";
   values+=room_flags.get(ROOMFLAG_NO_STAFFS)+", ";
   values+=room_flags.get(ROOMFLAG_NO_MORTALS)+", ";
   values+=room_flags.get(ROOMFLAG_NORMALLY_DARK)+", ";
   values+=room_flags.get(ROOMFLAG_SHALLOW_WATER)+", ";
   values+=room_flags.get(ROOMFLAG_DEEP_WATER)+", ";
   values+=room_flags.get(ROOMFLAG_SWAMP)+", ";
   values+=room_flags.get(ROOMFLAG_NEED_FLY)+", ";
   values+=room_flags.get(ROOMFLAG_NEED_BOAT)+", ";
   values+=room_flags.get(ROOMFLAG_NO_MAGICAL_ENTRY)+", ";
   values+=room_flags.get(ROOMFLAG_NO_VEHICLES)+", ";
   values+=room_flags.get(ROOMFLAG_CRAMPED)+", ";
   values+=room_flags.get(ROOMFLAG_NO_RANGED_ATTACK)+", ";
   values+=room_flags.get(ROOMFLAG_NEED_DIVE)+", ";
   values+=room_flags.get(ROOMFLAG_NEED_CLIMB)+", ";
   values+=room_flags.get(ROOMFLAG_USED_IN_TRACK)+", ";
   values+=room_flags.get(ROOMFLAG_CAN_CAMP)+", ";
   values+=room_flags.get(ROOMFLAG_HAS_KEYWORDS)+", ";
   values+=room_flags.get(ROOMFLAG_NO_WANDERING_MOBS)+", ";
   values+=room_flags.get(ROOMFLAG_NO_FOREIGN_WANDERING_MOBS)+", ";
   values+=room_flags.get(ROOMFLAG_HAS_PROC_SCRIPT)+")";
   // end of values

   query+=fields+values;

   if (mysql_real_query(database, query, query.Strlen())!=0) {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In room::dbWrite()";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return;
   }

   // names
   if (keywords.size() > 0) {
      Cell<String*> n_cell(names);
      String* n_ptr;

      query="insert into RoomNames ";
      fields="(ROOM_NUM, NAME) ";
      values="values (";
      
      n_ptr = n_cell.next();
      values+=cur_stats[2]+", ";
      buf=(char*)malloc(n_ptr->Strlen()*2+1);
      mysql_escape_string(buf, *n_ptr, n_ptr->Strlen());
      values+=*"'"+buf+*"', ";
      free(buf);
      while ((n_ptr=n_cell.next())) {
         values+=", (";
         values+=cur_stats[2]+", ";
         buf=(char*)malloc(n_ptr->Strlen()*2+1);
         mysql_escape_string(buf, *n_ptr, n_ptr->Strlen());
         values+=*"'"+buf+*"', ";
         free(buf);
      }

      query+=fields+values;
      if (mysql_real_query(database, query, query.Strlen())!=0) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbWrite()";
            mudlog << "Error executing query: "
                   << mysql_error(database) << endl;
         }
      }
   }
   // keywords
   if (room_flags.get(ROOMFLAG_HAS_KEYWORDS) && keywords.size()>0) {
      String descflds, keynmflds;
      String descvals, keynmvals;
      int writeNames=false;
      int keyindex=0;
      Cell<String*> ncll;
      String* n_ptr;
      Cell<KeywordPair*> kcll(keywords);
      KeywordPair* kptr;

      descflds="(ROOM_NUM, KEYWORD_INDEX, DESCRIPTION) ";
      keynmflds="(ROOM_NUM, KEYWORD_INDEX, KEYWORD) ";
      descvals="values ";
      keynmvals="values ";

      while ((kptr=kcll.next())) {
         descvals+="(";
         descvals+=cur_stats[2]+", ";
         descvals+=keyindex+", ";
         buf=(char*)malloc(kptr->desc.Strlen()*2+1);
         mysql_escape_string(buf, kptr->desc, kptr->desc.Strlen());
         descvals+=*"'"+buf+*"'), ";
         free(buf);
         if (kptr->names.size() > 0) {
            writeNames=true;
            kptr->names.head(ncll);
            while ((n_ptr=ncll.next())) {
               keynmvals+="(";
               keynmvals+=cur_stats[2]+", ";
               keynmvals+=keyindex+", ";
               buf=(char*)malloc(n_ptr->Strlen()*2+1);
               mysql_escape_string(buf, *n_ptr, n_ptr->Strlen());
               keynmvals+=*"'"+buf+*"'), ";
            }
         }
         keyindex++;
      }

      query="insert into RoomKeywords "+*descflds+*descvals;
      query.dropFromEnd(2); // Lose the trailing ", "
      if (mysql_real_query(database, query, query.Strlen())!=0) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbWrite()";
            mudlog << "Error executing query: "
                   << mysql_error(database) << endl;
         }
      }
      if (writeNames) {
         query="insert into RoomKeynames"+*keynmflds+*keynmvals;
         query.dropFromEnd(2); // Lose the trailing ", "
         if (mysql_real_query(database, query, query.Strlen())!=0) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "In room::dbWrite()";
               mudlog << "Error executing query: "
                      << mysql_error(database) << endl;
            }
         }
      }
   }

   // affected by
   if (affected_by.size()>0) {
      Cell<stat_spell_cell*> ss_cell(affected_by);
      stat_spell_cell* ss_ptr;

      query = "insert into RoomSpells ";
      fields = "(ROOM_NUM, SPELL_NUM, DURATION) ";
      values = "values ";
      while ((ss_ptr = ss_cell.next())) {
         values += "(";
         values += cur_stats[2] + ", ";
         values += ss_ptr->stat_spell + ", ";
         values += ss_ptr->bonus_duration + "), ";
      }

      query += fields + values;
      query.dropFromEnd(2); // Lose the trailing ", "
      if (mysql_real_query(database, query, query.Strlen())!=0) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbWrite()";
            mudlog << "Error executing query: "
                   << mysql_error(database) << endl;
         }
      }
   }
   // inventory
   if (inv.size()>0) {
      Cell<object*> ob_cell(inv);
      map<int, int> onums;
      object* ob_ptr;

      query = "insert into RoomInv ";
      fields = "(ROOM_NUM, OBJ_NUM, SOBJ_NUM, QUANTITY) ";
      values = "values ";

      while ((ob_ptr = ob_cell.next()))
         onums[ob_ptr->cur_stats[2]]++;

      inv.head(ob_cell);
      
      while ((ob_ptr = ob_cell.next())) {
         values += "(" + cur_stats[2] + *", " + ob_ptr->cur_stats[2] + *", 0, "
            + onums[ob_ptr->cur_stats[2]] + *"), ";
      }

      query += fields + values;
      query.dropFromEnd(2); // Lose the trailing ", "
      if (mysql_real_query(database, query, query.Strlen())!=0) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbWrite()";
            mudlog << "Error executing query: "
                   << mysql_error(database) << endl;
         }
      }
   }
   // critters
   if (critters.size()>0) {
      Cell<critter*> c_cell(critters);
      map<int, int> cnums;
      critter* c_ptr;

      query = "insert into RoomMobs ";
      fields = "(ROOM_NUM, MOB_NUM, QUANTITY) ";
      values = "values ";

      while ((c_ptr = c_cell.next()))
         cnums[c_ptr->cur_stats[2]]++;

      critters.head(c_cell);
      
      while ((c_ptr = c_cell.next())) {
         values += "(" + c_ptr->cur_stats[2] + *", "
            + cnums[c_ptr->cur_stats[2]] + *"), ";
      }

      query += fields + values;
      query.dropFromEnd(2); // Lose the trailing ", "
      if (mysql_real_query(database, query, query.Strlen())!=0) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbWrite()";
            mudlog << "Error executing query: "
                   << mysql_error(database) << endl;
         }
      }
   }
   // exits
   if (doors.size()>0) {
      int i=0;
      Cell<door*> d_cell(doors);
      door* d_ptr;

      query = "insert into RoomExits ";
      fields = "(ROOM_NUM, EXIT_NUM, DOOR_NUM, DESTINATION, DISTANCE) ";
      values = "values ";

      while ((d_ptr = d_cell.next())) {
         values += "(" + cur_stats[2] + *", " + i + *", "
            + d_ptr->dr_data->door_num + *", " + d_ptr->destination + *", "
            + d_ptr->distance + *"), ";
         if (d_ptr->affected_by.size() > 0) {
            Cell<stat_spell_cell*> ss_cll(d_ptr->affected_by);
            String spell_query = "insert into RoomExitSpells ";
            String spell_fields = "(ROOM_NUM, EXIT_NUM, SPELL_NUM, DURATION) ";
            String spell_values = "values ";
            stat_spell_cell* ss_ptr;

            while ((ss_ptr = ss_cll.next())) {
               values += "(" + cur_stats[2] + *", " + i + *", "
                  + ss_ptr->stat_spell + *", " + ss_ptr->bonus_duration + *"), ";
            }

            query += fields + values;
            query.dropFromEnd(2);
            if (mysql_real_query(database, query, query.Strlen())!=0) {
               if (mudlog.ofLevel(WRN)) {
                  mudlog << "In room::dbWrite()";
                  mudlog << "Error executing query: "
                         << mysql_error(database) << endl;
               }
            }
         }
         i++;
      }

      query += fields + values;
      query.dropFromEnd(2); // Lose the trailing ", "
      if (mysql_real_query(database, query, query.Strlen())!=0) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In room::dbWrite()";
            mudlog << "Error executing query: "
                   << mysql_error(database) << endl;
         }
      }
   }
}
#endif

void room::fileWrite(ofstream& ofile) {
   int i, j;
   Cell<stat_spell_cell*> ss_cell(affected_by);
   stat_spell_cell* ss_ptr;
   Cell<String*> n_cell(names);
   String* n_ptr;
   Cell<door*> d_cell(doors);
   door* d_ptr;
   Cell<object*> ob_cell(inv);
   object* ob_ptr;
   Cell<critter*> c_cell(critters);
   critter* c_ptr;
   short was_totally_read = FALSE;

   normalize(); //make sure everything jives!
   
   if (room_flags.get(22)) { //totaly read flag
      was_totally_read = TRUE;
      room_flags.turn_off(22); // this flag is purely temp, don't write
   }//if

   while ((n_ptr = n_cell.next())) {
      ofile << *n_ptr << " ";
   }//while            
   ofile << "~" << "\tnames\n";

   parse_for_max_80(short_desc);
   ofile << short_desc << endl << "~" << endl;
   parse_for_max_80(long_desc);
   ofile << long_desc << endl << "~" << endl;

   room_flags.Write(ofile);

   //mudlog << "Keywords size:  " << keywords.size() << endl;
   
   if (room_flags.get(32)) { //if has keywords
      Cell<KeywordPair*> kcll(keywords);
      KeywordPair* kptr;

      while ((kptr = kcll.next())) {
         ofile << "1   Start of KeywordPair\n";
         kptr->write(ofile);
      }
      ofile << "-1  No more KeywordPairs\n";
   }//if

   //mudlog << "DOne writing out keywordds.." << endl;

   for (i = 0; i<ROOM_CUR_STATS; i++) {
      ofile << cur_stats[i] << " ";
      if ((i + 1) % 20 == 0)
         ofile << endl;
   }//for
   ofile << "\tcur_stats\n";

                /*  Affected By */
   while ((ss_ptr = ss_cell.next())) {
      ofile << ss_ptr->stat_spell << " " << ss_ptr->bonus_duration << " ";
      if ((i + 1) % 20 == 0)
         ofile << endl;
   }//while
   ofile << -1 << "\taffected_by\n";

                /*  Inventory */   
   j = 0;
   inv.head(ob_cell);
   while ((ob_ptr = ob_cell.next())) {
      ofile << ob_ptr->OBJ_NUM << " ";
      if ((++j % 20) == 0)
         ofile << endl;
   }//while
   ofile << -1 << "\tinv\n";

                       /*  Doors, stored door#, dest  */
   
   j = 0;
   while ((d_ptr = d_cell.next())) {
      if (d_ptr->dr_data->door_data_flags.get(10)) {
         ofile << 1 << " "; //test for next, will read untill -1
         d_ptr->Write(ofile);
         if (++j > 8) {
            ofile << endl;
            j = 0;
         }//if
      }//if
      else {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "WARNING: door not in use for room: " << getIdNum() 
                   << " door: " << d_ptr->getIdNum() << endl;
         }
      }
   }//while 
   ofile << -1 << "\tEnd of Doors\n";

                      /*  Critters, stored mob# */
   int cnt = 0;
   while ((c_ptr = c_cell.next())) {
      if (++cnt > 20) {
         ofile << "\n";
         cnt = 0;
      }

      if (c_ptr->mob && !c_ptr->pc)
         ofile << c_ptr->MOB_NUM << " ";
   }//while
   ofile << -1 << "\tmobs\n";

   if (room_flags.get(35)) {
      Cell<RoomScript*> cll;
      room_proc_scripts.head(cll);
      RoomScript* ptr;

      int i = 1;
      while ((ptr = cll.next())) {
         ofile << i++ <<  "  Start of a room proc script\n";
         ptr->write(ofile);
      }
      
      ofile << "-1  End of room proc scripts" << endl;
   }

   ofile << "End of Room\n";
 
   if (was_totally_read) {
      room_flags.turn_on(22); //make it as it was
   }//if
}//Write....sub_room


void room::checkForProc(String& cmd, String& arg1, critter& actor,
                        int targ) {
   Cell<critter*> cll(critters);
   critter* ptr;
   
   mudlog.log("room::checkForProc.");
   while ((ptr = cll.next())) {
      //if (!ptr->pc) { //SMOB (not a PC)
      //   if (ptr->mob->mob_data_flags.get(17)) { //ok then, it has data
      //      if (mudlog.ofLevel(DBG)) {
      //         mudlog << "room::checkForProc, found a mob: " 
      //                << ptr->getName() << endl;
      //      }

      // Make sure that the actor is still in the room.
      if (!critters.haveData(&actor)) {
         return;
      }

      // Have to check all, because mob also checks objects that
      // the mob owns.
      if (ptr->isMob()) { //if it's a MOB
         //mudlog.log("Doing mob_to_smob..");
         ptr = mob_to_smob(*ptr, getRoomNum());
      }

      // don't want mobs triggering their own scripts.
      if ( ptr != &actor ) {
          ptr->checkForProc(cmd, arg1, actor, targ, *this);
      }
      //   }//if
      //}//if
   }//while


   Cell<object*> ocll(inv);
   object* optr;
   while ((optr = ocll.next())) {

      // Make sure that the actor is still in the room.
      if (!critters.haveData(&actor)) {
         return;
      }

      if (optr->hasScript()) {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "room::checkForProc, found an object: " 
                   << optr->getName() << endl;
         }
         if (!optr->isModified()) {
            //mudlog.log("Doing obj_to_sobj..");
            object* tmp = optr;
            optr = obj_to_sobj(*optr, &inv, getIdNum());
            if (!inv.substituteData(tmp, optr, 1)) {
               mudlog.log(LS_ERROR, "ERROR: checkForProc: substituteData  failed after obj_to_sobj.\n");
            }//if
         }
         optr->checkForProc(cmd, arg1, actor, targ, *this);
      }//if
   }//while

   // Now, check for room procs!!
   Cell<RoomScript*> rcll;
   RoomScript* rptr;
         
   room_proc_scripts.head(rcll);

   int idx = 0;
   while ((rptr = rcll.next())) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "room::checkForProc, found room script: " 
                << rptr->toStringBrief(0, getIdNum(), ENTITY_ROOM, idx) << endl;
      }

      // Make sure that the actor is still in the room.
      if (!critters.haveData(&actor)) {
         return;
      }

      if (rptr->matches(cmd, arg1, actor, targ)) {
         mudlog.log("Script matches..\n");
         if (pending_scripts.size() >= 10) { //only queue 10 scripts
            return; //do nothing, don't want to get too much backlog.
         }
         else {
            // add it to the pending scripts.
            mudlog.log("Generating script.\n");
            rptr->generateScript(cmd, arg1, actor, targ, *this, NULL);

            mudlog.log("Inserting new script.\n");
            insertNewScript(new RoomScript(*rptr));

            if (cur_script) {
               mudlog.log("Had a cur_script.\n");
               if (cur_script->getPrecedence() <
                   pending_scripts.peekFront()->getPrecedence()) {
                  
                  mudlog.log("Junking cur_script because of precedence.\n");
                  pending_scripts.loseData(cur_script); //take it out of queue
                  delete cur_script; //junk it!
                  cur_script = pending_scripts.peekFront();
                  cur_script->resetStackPtr(); //get ready to start
               }//if
               // else, it just goes into the queue
            }//if we currently have a script.
            else { //there was none, so grab the first one.
               mudlog.log("Was no cur_script, taking top of pending stack.\n");
               cur_script = pending_scripts.peekFront();
               proc_action_rooms.gainData(this);
               cur_script->resetStackPtr(); //get ready to start
            }

            return;
         }//else
      }//if matches
      idx++;
   }//while
}//checkForProc

/** Attempt to trigger a room script directly.  So far, we support only
 * pull and push, but more can easily be added.
 */
int room::attemptExecuteUnknownScript(String& cmd, int i_th, String& arg1,
                                      critter& actor) {

   int targ;
   targ = i_th = -1; //use i_th to quiet the compiler.
   
   // Now, check for room procs!!
   Cell<RoomScript*> rcll;
   RoomScript* rptr;
         
   room_proc_scripts.head(rcll);

   int idx = 0;
   while ((rptr = rcll.next())) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "room::attemptExecuteUnknownScript, found room script: " 
                << rptr->toStringBrief(0, getIdNum(), ENTITY_ROOM, idx) << endl;
      }
      if (rptr->matches(cmd, arg1, actor, targ)) {
         mudlog.log("Script matches..\n");
         if (pending_scripts.size() >= 10) { //only queue 10 scripts
            actor.show("Please try again in a bit.\n");
            return 0; //do nothing, don't want to get too much backlog.
         }
         else {
            // add it to the pending scripts.
            mudlog.log("Generating script.\n");
            rptr->generateScript(cmd, arg1, actor, targ, *this, NULL);

            mudlog.log("Inserting new script.\n");
            insertNewScript(new RoomScript(*rptr));

            if (cur_script) {
               mudlog.log("Had a cur_script.\n");
               if (cur_script->getPrecedence() <
                   pending_scripts.peekFront()->getPrecedence()) {
                  
                  mudlog.log("Junking cur_script because of precedence.\n");
                  pending_scripts.loseData(cur_script); //take it out of queue
                  delete cur_script; //junk it!
                  cur_script = pending_scripts.peekFront();
                  cur_script->resetStackPtr(); //get ready to start
               }//if
               // else, it just goes into the queue
            }//if we currently have a script.
            else { //there was none, so grab the first one.
               mudlog.log("Was no cur_script, taking top of pending stack.\n");
               cur_script = pending_scripts.peekFront();
               proc_action_rooms.gainData(this);
               cur_script->resetStackPtr(); //get ready to start
            }

            return 0;
         }//else
      }//if matches
      idx++;
   }//while
   return -1; //didn't find anything that matched
}//attemptExecuteUnknownScript


int room::getZoneNum() {
   return ZoneCollection::instance().getZoneFor(*this).getIdNum();
}

void room::stat(critter& pc) {
   String buf2(100);

   if (names.peekFront()) {
      pc.show(*(names.peekFront()));
   }//if

   Sprintf(buf2, "  Belongs to zone:  %i.\n", getZoneNum());
   pc.show(buf2);

   pc.show(short_desc);
   pc.show("\n");
   pc.show(long_desc);
   pc.show("\n");

   Cell<KeywordPair*> cll(keywords);
   KeywordPair* ptr;
   int cnt = 0;

   while ((ptr = cll.next())) {
      ptr->show(cnt, pc);
      cnt++;
   }

   out_field(room_flags, pc, ROOM_FLAGS_NAMES);

   pc.show("\n");
   Sprintf(buf2, "v_bit: %i  mv$: %i  r_num: %i  pause: %i  fall_to: %i\n", 
           getVisBit(), getMovCost(), getRoomNum(), getPause(), getFallTo());
   pc.show(buf2);

   Sprintf(buf2, "Number of critters:  %i  Number of Scripts Pending: %i\n\n",
           critters.size(), pending_scripts.size());
   pc.show(buf2);

   if (room_proc_scripts.size() > 0) {
      listScripts(pc);
   }
}//stat


/* called after OLC to enforce as much state as possible. */
void room::normalize() {
   setVisBit(getVisBit() | 1024);

   if (keywords.size() > 0) {
      room_flags.turn_on(32);
   }
   else {
      room_flags.turn_off(32);
   }
}//normalize_room

 
const String* room::getRandomExitDir(critter& pc) {
   int sz = doors.size();
   int idx = d(1, sz);

   Cell<door*> cll(doors);
   door* ptr;

   int cnt = 0;
   while ((ptr = cll.next())) {
      cnt++;
      if (cnt == idx) {
         if (ptr->getDestRoom()) {
            if (!(ptr->getDestRoom()->isNoWanderForeignMob() &&
                  (pc.getNativeZoneNum() != ptr->getDestRoom()->getZoneNum()))) { 
               if (!(ptr->getDestRoom()->isNoWanderMob() &&
                     pc.isNpc()) &&
                   mob_can_enter(pc, *(ptr->getDestRoom()), FALSE)) {
                  return ptr->getDirection();
               }//if
            }//if
         }//if
      }//if
      else if (cnt > idx) {
         break;
      }//else
   }//while

   if (mudlog.ofLevel(DBG)) {
      mudlog << "Didn't find the one we wanted..getting first one." << endl;
   }

   // Didn't find the one we wanted...get the first one.
   doors.head(cll);

   while ((ptr = cll.next())) {
      if (ptr->getDestRoom()) {
         if (!(ptr->getDestRoom()->isNoWanderForeignMob() &&
               (pc.getNativeZoneNum() != ptr->getDestRoom()->getZoneNum()))) { 
            if (!(ptr->getDestRoom()->isNoWanderMob() && 
                  !pc.isPc()) &&
                mob_can_enter(pc, *(ptr->getDestRoom()), FALSE)) {
               return ptr->getDirection();
            }//if
         }//if
      }//if
   }//while

   return &UNKNOWN;
}//getRandomExitDir

int room::isInUse() const {
   return (room_flags.get(23) || critters.size());
}

int room::getCritCount(critter& pc) {
   return crit_count(critters, pc);
}

int room::getObjCount(object& obj) {
   int retval = 0;
   Cell<object*> cll(inv);
   object* ptr;

   while ((ptr = cll.next()))
      if (ptr->getIdNum() == obj.getIdNum()) retval++;
   
   return retval;
}

int room::getObjCount(int onum) {
   int retval = 0;
   Cell<object*> cll(inv);
   object* ptr;

   while ((ptr = cll.next()))
      if (ptr->getIdNum() == onum) retval++;
   
   return retval;
}

/* this returns the critter if exists.  rm_num is the number of
 * the room we found it in.  Returns NULL if not found.
 * searches entire world. */
critter* room::haveCritNamedInWorld(const int i_th, const String* name,
                                    const int see_bit, int& rm_num) {
   Cell<String*> char_cell;
   Cell<critter*> cell;
   critter* crit_ptr;
   int count = 0, len;
   String *string;

   mudlog.log(DBG, "In have_crit_named, for whole room_list");

   if ((len = name->Strlen()) == 0) 
      return NULL;

   if (i_th <= 0)
      return NULL;

   //mudlog.log(DBG, "Looking in room number:  ");
   for (int i = 0; i<NUMBER_OF_ROOMS; i++) {

//       if (mudlog.ofLevel(DBG)) {
//          mudlog << i << " ";
//          if (((i + 1) % 100) == 0)
//             mudlog << flush;
//       }
      if (room_list.elementAtNoCreate(i)) {
         room_list[i].critters.head(cell);
         while ((crit_ptr = cell.next())) {
            if (detect(see_bit, crit_ptr->VIS_BIT)) {
               crit_ptr->names.head(char_cell);
               while ((string = char_cell.next())) {
                  if (strncasecmp(*string, *name, len) == 0){ 
                     count++;
                     if (count == i_th) {
                        rm_num = i;
                        return crit_ptr;
                     }//if
                  }//if
               }//while
            }//if
         }//while
      }//for
   }//if
   rm_num = 0;
   return NULL;
}//have_crit_named for whole room_list, returns room#


/**  Find a critter in a given zone.  Returns the critter, or
 * NULL if not found.  in_room holds the room number we found it
 * in. */
critter* room::haveCritNamedInZone(zone& zn, const int i_th,
                                   const String* name, const int see_bit,
                                   int& in_room) {
   Cell<String*> char_cell;
   Cell<critter*> cell;
   critter* crit_ptr;
   int count = 0, len;
   String *string;

   in_room = 0;

   //log("In have_crit_named, for whole zone");

   if (!name) {
      mudlog.log(LS_ERROR, "ERROR:  NULL name sent to have_crit_named.\n");
      return NULL;
   }//if

   if ((len = name->Strlen()) == 0) 
      return NULL;

   if (i_th <= 0)
      return NULL;

   int end = zn.getEndRoomNum();

   for (int i = zn.getBeginRoomNum(); i <= end; i++) {
      room_list[i].critters.head(cell);
      while ((crit_ptr = cell.next())) {
         if (detect(see_bit, crit_ptr->VIS_BIT)) {
            crit_ptr->names.head(char_cell);
            while ((string = char_cell.next())) {
               if (strncasecmp(*string, *name, len) == 0){ 
                  count++;
                  if (count == i_th) {
                     in_room = i;
                     return crit_ptr;
                  }//if
               }//if
            }//while
         }//if
      }//while
   }//for
   in_room = 0;
   return NULL;
}//have_crit_named for whole zone


void room::doPoofOut(critter& pc) {
   String buf(100);
   Cell<critter*> cll(critters);
   critter* ptr;
   while ((ptr = cll.next())) {
      if ((&pc != ptr) && ptr->canDetect(pc) && !ptr->isSleeping()) {
         if (ptr->isImmort() && (ptr->IMM_LEVEL >= pc.IMM_LEVEL)) {
            Sprintf(buf, "[OUT: %S] %S\n", pc.getName(), &(pc.getPoofout()));
         }
         else {
            Sprintf(buf, "%S\n", &(pc.getPoofout()));
         }
         ptr->show(buf);
      }
   }//while
}//doPoofOut


int room::doRclear(int new_rm_num) {
   critter* crit_ptr;

   while (!critters.isEmpty()) {
      crit_ptr = critters.peekFront();

      if ((crit_ptr->isPc()) || crit_ptr->isSmob()) {
         //mudlog << "Was a SMOB or PC." << endl;
         int is_dead;
         crit_ptr->doGoToRoom(new_rm_num, NULL, NULL, is_dead, getIdNum(), 1);
         if (!is_dead) {
            crit_ptr->show("The void has swallowed your previous location!!\n");
         }
      }
      else {
         //just remove the pointer...
         removeCritter(crit_ptr);
      }//else
   }//while

   Clear();  //clear out the room pc WAS in!!
   return 0;
}//doRclear


int room::doScan(critter& pc) {
   String buf(100);
   Cell<door*> dcll(doors);
   Cell<critter*> cll;
   critter* ptr;
   int a, b;

   int pl = get_percent_lrnd(SCAN_SKILL_NUM, pc);

   door* dptr;
   while ((dptr = dcll.next())) {
      if (detect(pc.SEE_BIT, dptr->getVisBit() | getVisBit())) {
         if (dptr->isOpen()) {
            dptr->getDestRoom()->critters.head(cll);
            while ((ptr = cll.next())) {
               if (detect(pc.SEE_BIT,
                          ptr->VIS_BIT | dptr->getDestRoom()->getVisBit())) {
                  a = max((pc.LEVEL - pc.POS), 10) + pl;
                  b = max((ptr->LEVEL - ptr->POS), 10) + 100;

                  if (ptr->isHiding()) {
                     b *= max(1, max(get_percent_lrnd(HIDE_SKILL_NUM, *ptr),
                                     get_percent_lrnd(BLEND_SKILL_NUM, *ptr)) / 10);
                  }

                  if (d(1, a) > d(1, b)) {                     
                     Sprintf(buf, "     %S%P25 %S.\n", dptr->getDirection(),
                             ptr->getName());
                     pc.show(buf);
                  }//if
               }//if
            }//while
         }//if
      }//if
   }//while
   return 0;
}//doScan

void room::doPoofIn(critter& pc) {
   String buf(100);
   Cell<critter*> cll(critters);
   critter* ptr;
   while ((ptr = cll.next())) {
      if ((&pc != ptr) && !ptr->isSleeping() && ptr->canDetect(pc)) {
         if (ptr->isImmort() && (ptr->IMM_LEVEL >= pc.IMM_LEVEL)) {
            Sprintf(buf, "[IN: %S] %S\n", pc.getName(), &(pc.getPoofin()));
         }
         else {
            Sprintf(buf, "%S\n", &(pc.getPoofin()));
         }
         
         ptr->show(buf);
      }
   }//while
}//doPoofIn


void room::resetProcMobs() {
   Cell<critter*> cll(critters);
   critter* ptr;
   while ((ptr = cll.next())) {
      //mudlog << "Found a critter:  " << ptr << endl << flush;
      if (ptr->mob) {
         //mudlog << "Was a mob..." << endl;
         ptr->MOB_FLAGS.turn_on(3); //should do spec procs
      }//if
   }//while
}//resetProcMobs


void room::purgeCritter(int mob_num, critter& pc) {
   int done = FALSE;
   while (!done) {
      Cell<critter*> cll(critters);
      critter* ptr;

      done = TRUE;
      while ((ptr = cll.next())) {
         if (ptr->isNpc() && (ptr->getIdNum() == mob_num)) {
            agg_kills_vict(&pc, *ptr);
            done = FALSE;
            break;
         }//if
      }//while
   }//for
}//purgeCritter


int room::sub_a_4_b(critter* crit_ptr, int i_th, const String& name,
                    int see_bit) {
   return crit_sub_a_4_b(crit_ptr, critters, i_th, 
                         &name, see_bit, *this);
}

int room::sub_a_4_b(critter* a, critter* b, int i_th) {

   // Don't want to add it if we can't substitute it, but the list<>
   // method does indeed append if it's not already there.
   if (critters.haveData(b)) {
      return critters.substituteData(b, a, i_th);
   }
   return FALSE;
}

void room::gainCritter(critter* crit) {
   if (crit->isNpc()) {
      crit->incrementCurInGame();
   }
   critters.prepend(crit);

   // This nips a bunch of bugs in the bud.  No more will we have
   // MOB's running around in the game.  If it's in the game, it will
   // be a SMOB, which for reasons lost in antiquity, means it is a
   // unique Instance and may be modified independently of the database.
   if (crit->isMob()) {
      crit = mob_to_smob(*crit, *this);
   }//if
   crit->setCurRoomNum(getRoomNum());
}

void room::loseObjectFromGame(object& which_un) {
   Cell<object*> ocll(inv);
   object* optr;
   Cell<critter*> ccll;
   critter* cptr;

   int targ_num = which_un.getIdNum();

   optr = ocll.next();
   while (optr) {
      if (optr->IN_LIST && (optr->getIdNum() == targ_num)) { //if SOBJ
         delete optr;
         optr = inv.lose(ocll);
      }
      else if (optr->getIdNum() == targ_num) {
         optr = inv.lose(ocll);
      }
      else
         optr = ocll.next();
   }//while
   
   critters.head(ccll);
   while ((cptr = ccll.next())) {
      cptr->loseObjectFromGame(&which_un);
   }//while
}//loseObjectFromGame

critter* room::removeCritter(critter* crit) {
   critter* retval;

   retval = critters.loseData(crit);

   if (retval) {
      if (crit->isNpc()) {
         crit->decrementCurInGame();
      }
   }//if
   else {
      // We use room 0 for newbies logging in and such...will get boundary
      // case errors here in some instances...
      if (getIdNum() != 0) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "WARNING:  could not remove the critter in"
                   << " room::removeCrit(), crit_name: " << *(crit->getName()) 
                   << " num: " << crit->getIdNum() << " room: "
                   << getIdNum() << endl;
            core_dump("removeCritter");
         }//if
      }//if
   }//else

   return retval;
}//removeCritter


void room::getPetsFor(critter& owner, List<critter*>& rslts) {
   critter* ptr;
   Cell<critter*> cll(critters);
   while ((ptr = cll.next())) {
      if (owner.PETS.haveData(ptr)) {
         rslts.append(ptr);
      }//if
   }//while
}//getPetsFor

void room::showCritters(critter& pc) {
   out_crit(critters, pc);
}

void room::showVehicles(critter& pc) {
   out_vehicles(doors, pc);
}

//show inv to pc (use out_inv method)
void room::outInv(critter& pc) {
   out_inv(inv, pc, ROOM_INV);
}

void room::gainInv(object* obj) {
   if (obj->obj_flags.get(55)) { //if it's coins
      if (obj->cur_stats[1] == 0) {
         if (obj->IN_LIST) {
            mudlog << "ERROR:  Possible memory leak, gainInv:  obj is SOBJ"
                   << " but has zero coins, obj#" << obj->getIdNum()
                   << " room# " << getIdNum() << endl;
         }
         return; //don't add it, it had zero coins
      }//if
   }//if
         
   // If this ever changes from prepend, must change the disolve-pet
   // code in misc.cc (it disolves the corpse)
   inv.prepend(obj);
   if (obj->IN_LIST) {
      obj->IN_LIST = &inv;
      obj->setCurRoomNum(getIdNum(), 0);
   }
}//gainInv

void room::purgeObj(object* obj, critter* pc, int do_msg) {
   
   loseInv(obj);

   if (!obj->isBulletinBoard()) {
      recursive_init_unload(*obj, 0);
   }
   else {
      obj->decrementCurInGame();
   }

   if (do_msg && pc) {
      pc->show("Ok, object purged from room.\n");
   }

   if (obj->IN_LIST) {
      delete obj;
   }//if
}//purgeObj


object* room::loseInv(object* obj) {
   obj->setCurRoomNum(0, 0);
   return inv.loseData(obj);
}

int room::canSeeSky() {
   return (room_flags.get(4) || room_flags.get(15) || room_flags.get(16)
           || room_flags.get(17));
}


critter* room::haveCritNamed(int i_th, const String* name, int see_bit) {
   return ::have_crit_named(critters, i_th, name, see_bit, *this);
}

critter* room::haveCritNamed(int i_th, const String* name, critter& pc) {
   int foo = 0;
   return haveCritNamed(i_th, name, pc, foo);
}

critter* room::haveCritNamed(int i_th, const String* name, critter& pc,
                             int& count_sofar) {
   if ((strcasecmp(*name, "me") == 0) ||
       (strcasecmp(*name, "self") == 0)) {
      return &pc;
   }
   return ::have_crit_named(critters, i_th, name, pc.SEE_BIT, count_sofar, *this);
}

critter* room::haveCritter(critter* ptr) {
   if (critters.haveData(ptr)) {
      return ptr;
   }
   return NULL;
}

object* room::haveObject(object* ptr) {
   if (inv.haveData(ptr)) {
      return ptr;
   }
   return NULL;
}

object* room::haveObjNamed(int i_th, const String* name, int see_bit) {
   int foo = 0;
   return ::have_obj_named(inv, i_th, name, see_bit, *this, foo);
}

object* room::haveAccessibleObjNamed(int i_th, const String* name,
                                     critter& pc, int& count_sofar, int& posn) {
   object* ptr;
   ptr = have_obj_named(inv, i_th, name, pc.getSeeBit(), *this, count_sofar);
   if (!ptr) {
      ptr = have_obj_named(pc.inv, i_th - count_sofar, name, pc.getSeeBit(), *this,
                           count_sofar);
   }
   if (!ptr) {
      ptr = pc.findObjInEq(i_th - count_sofar, *name, pc.getSeeBit(), *this,
                           posn, count_sofar);
   }
   return ptr;
}//haveAccesibleObjNamed

int room::doClose(int i_th, const String* name) {
   door* dr_ptr;
   object* ob_ptr;
   String buf(100);

   if ((dr_ptr = door::findDoor(this->DOORS, i_th, name, ~0, *this))) {
      if (dr_ptr->isOpen()) {
         if (dr_ptr->canClose()) {
            dr_ptr->close();
            return 0;
         }
      }
   }
   else {
      ob_ptr = this->haveObjNamed(i_th, name, ~0);
      if (ob_ptr && ob_ptr->bag && !ob_ptr->isModified()) {
         ob_ptr = obj_to_sobj(*ob_ptr, this->getInv(), TRUE, 
                              i_th, name, ~0, *this);
      }
      if (ob_ptr && ob_ptr->bag && !ob_ptr->isClosed()) {
         ob_ptr->bag->close();
         return 0;
      }
   }
   return -1;
}

int room::doMload(int i_th) {
   //TODO: Figure out what happens if the following conditional isn't true. --eroper
   critter* crit_ptr = NULL; 

   if ((i_th >= 1) && (i_th < NUMBER_OF_MOBS)) {
      crit_ptr = &(mob_list[i_th]);
      if (!crit_ptr->CRIT_FLAGS.get(18)) {
         return -1;
      }//if
   }//if
                 /* do it then */

   if (crit_ptr->isPlayerShopKeeper()) {
      critter* shop_keeper = load_player_shop_owner(crit_ptr->getIdNum());
      if (shop_keeper) {
         gainCritter(shop_keeper);
      }//if
   }//if
   else {
      gainCritter(&(mob_list[crit_ptr->getIdNum()]));
   }

   recursive_init_loads(*crit_ptr);
   return 0;
}//mload


int room::doOload(int i_th) {
   object* obj_ptr;
   //   mudlog.log(TRC, "In oclone.\n");

   if ((i_th >= 1) && (i_th < NUMBER_OF_ITEMS)) {
      obj_ptr = &(obj_list[i_th]);
      if (!obj_ptr->OBJ_FLAGS.get(10)) {
         return -1;
      }//if
   }//if
   else {
      return -1;
   }//else
         
   if (!obj_ptr->isBulletinBoard()) {
      recursive_init_loads(*obj_ptr, 0);
   }
   else {
      obj_ptr->incrementCurInGame();
   }
   gainInv(obj_ptr);
   return 0;
}//oload


critter* room::findFirstBanker() {
   Cell<critter*> cell(critters);
   critter* crit_ptr;

   while ((crit_ptr = cell.next())) { 
      if (crit_ptr->isBanker()) {
         return crit_ptr;
      }//if
   }//while
   
   return NULL;
}//findFirstBanker


critter* room::findFirstShopKeeper() {
   Cell<critter*> cell(critters);
   critter* crit_ptr;

   while ((crit_ptr = cell.next())) { 
      if (crit_ptr->isShopKeeper()) {
         return crit_ptr;
      }//if
   }//while
   
   return NULL;
}//findFirstShopOwner


critter* room::findFirstTeacher() {
   Cell<critter*> cell(critters);
   critter* crit_ptr;

   while ((crit_ptr = cell.next())) { 
      if (crit_ptr->isTeacher()) {
         return crit_ptr;
      }//if
   }//while
   
   return NULL;
}//findFirstTeacher



void room::showAllCept(const char* msg, critter* pc) const {
   Cell<critter*> cell(critters);
   critter* crit_ptr;

   while ((crit_ptr = cell.next())) { 
      if (crit_ptr != pc)  
         if (crit_ptr->POS < POS_SLEEP)  
            crit_ptr->show(msg);
   }//while

   Cell<object*> cll(inv);
   object* obj;

   while ((obj = cll.next())) {
     if (obj->obj_proc && (crit_ptr = obj->obj_proc->w_eye_owner)) {
       if (crit_ptr != pc) {
         if (crit_ptr->POS < POS_SLEEP) {
           crit_ptr->show("#####");
           crit_ptr->show(msg);
         }//if
       }//if
     }//if
   }//while
}// show_all_cept

void room::finishedRoomProc() {
   if (cur_script) {
      pending_scripts.loseData(cur_script);
      delete cur_script;
   }

   // This could easily end up being NULL, that's OK!
   cur_script = pending_scripts.peekFront();
}//finishedRoomProc

void room::addProcScript(const String& txt, RoomScript* script_data) {
   //similar to reading it in...
   //first, see if we are over-writing one...
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In room::addProcScript, txt:  \n" << txt
             << "\nscript data:  "
             << script_data->toStringBrief(0, 0, ENTITY_ROOM, 0) << endl;
   }

   room_flags.turn_on(35);

   Cell<RoomScript*> cll;
   RoomScript* ptr;
   room_proc_scripts.head(cll);

   while ((ptr = cll.next())) {
      if (ptr->matches(*script_data)) {
         //got a match.
         mudlog.log("room::addProcScript, they match.");
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
      mudlog.log(LS_ERROR, "script_data is NULL, room::addProcScript.");
      return;
   }

   room_proc_scripts.append(script_data);
}//addProcScript

void room::listScripts(critter& pc) {
   String buf(500);
   buf.Append("These scripts are defined for this room, the actual scripts\n"
         "may be seen by using the stat_room_script [rm_num] [script_index] command.\n\n");

   pc.show(buf);

   String tmp(100);
   int found_one = FALSE;
   Cell<RoomScript*> cll(room_proc_scripts);
   RoomScript* ptr;
   int idx = 0;
   while ((ptr = cll.next())) {
      found_one = TRUE;
      tmp = ptr->toStringBrief(FALSE, 0, ENTITY_ROOM, idx);
      Sprintf(buf, "[%i] %S\n", idx, &(tmp));
      pc.show(buf);
      idx++;
   }

   if (!found_one) {
      buf.Append("No scripts defined for this room.\n");
      pc.show(buf);
   }
}//listScripts

void room::removeScript(String& trigger, int i_th, critter& pc) {
   int sofar = 1;
   String buf(500);
 
   Cell<RoomScript*> cll(room_proc_scripts);
   RoomScript* ptr;
   ptr = cll.next();
   while (ptr) {
      if (strcasecmp(*(ptr->getTrigger()), trigger) == 0) {
         if (sofar == i_th) {
            delete ptr;
            ptr = room_proc_scripts.lose(cll);
            pc.show("Deleted it...\n");
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

   pc.show("Didn't find that script..\n");
}//removeScript


int room::insertNewScript(RoomScript* script) {
   RoomScript* ptr;
   Cell<RoomScript*> cll(pending_scripts);

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


void room::doScriptJump(int abs_offset) {
   if (cur_script)
      cur_script->doScriptJump(abs_offset);
}


int room::doEmote(critter& pc, CSelectorColl& includes,
                  CSelectorColl& denies, CSentryE cs_entry, ...) {
   va_list argp;
   va_start(argp, cs_entry);
   int retval = 0;
   retval = vDoEmote(pc, includes, denies, cs_entry, argp);
   va_end(argp);
   return retval;
}

int room::vDoEmote(critter& pc, CSelectorColl& includes, CSelectorColl& denies,
                   CSentryE cs_entry, va_list argp) {
   Cell<critter*> cll(critters);
   critter* ptr;
   String buf(100);
   String buf2(100);


   while ((ptr = cll.next())) {
      if (!(denies.matches(ptr, &pc))) {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "room::doEmote, not denied." << endl;
         }
         if (includes.matches(ptr, &pc)) {
            if (mudlog.ofLevel(DBG)) {
               mudlog << "room::doEmote, includes matched." << endl;
               mudlog << "cstr of " << (int)(cs_entry) << "-:"
                      << cstr(cs_entry, *ptr) << ":-" <<  endl;
            }

            vSprintf(buf, cstr(cs_entry, *ptr), argp);
            if (mudlog.ofLevel(DBG)) {
               mudlog << endl << "buf -:" << buf << ":-" << endl << endl;
            }
            Sprintf(buf2, "%S %S", pc.getName(ptr->SEE_BIT), &buf);
            buf2.Cap();
            ptr->show(buf2);
         }//if
      }//if
   }//while
   return 0;
}//vDoEmote

int room::getVisBit(bool lightAdjust) const {
   int vb = cur_stats[0];
   if (lightAdjust) {
      if (isPermDark()) {
         vb |= 1; //perm dark it is
      }//if
      else if (isNormallyDark() || (isNightTime() && room_flags.get(4))) {
         //normally dark, or outside and night time
         vb |= 1;
         if (isAffectedBy(ILLUMINATE_SKILL_NUM)) {
            vb &= ~1;
         }//if
         else {
            Cell<critter*> cll(critters);
            critter* crit_ptr;
            while ((crit_ptr = cll.next())) {
               if (crit_ptr->CRIT_FLAGS.get(1)) { //if using_light source 
                  vb &= ~1;
                  break;
               }//if
            }//while
         }//else
      }//if normally_dark
      else {
         vb &= ~1;
      }//else
   }
   return vb;
}


WeatherType room::getWeather() const {

   if(!hasWeather()) return wNONE;
   
   if(getFlag(38)){       //temperate
      return weather.climates[0].weather;
   }else if(getFlag(39)){ //savanah
      return weather.climates[1].weather;
   }else if(getFlag(40)){ //mountain
      return weather.climates[2].weather;
   }else if(getFlag(41)){ //snowymountain
      return weather.climates[3].weather;
   }else if(getFlag(42)){ //sandydesrt
      return weather.climates[4].weather;
   }else if(getFlag(43)){ //dirtdesert
      return weather.climates[5].weather;
   }else if(getFlag(44)){ //swamp
      return weather.climates[6].weather;
   }else if(getFlag(45)){ //tropical
      return weather.climates[7].weather;
   }else if(getFlag(46)){ //arctic
      return weather.climates[8].weather;
   } else return wNONE;

}

WindType room::getWind() const {
   //NOTE FOR ED: the following line is what we were looking for!!!!
   //return weather.climates[getWeather()].wind;
   return weather.climates[getClimate()].wind;
}

TemperatureType room::getTemperature() const {
   return weather.climates[getClimate()].temperature;
}

ClimateType room::getClimate() const{

   if(!hasWeather()) return cNONE;
   
   if(getFlag(38)){       //temperate
      return temperate;
   }else if(getFlag(39)){ //savanah
      return savanah;
   }else if(getFlag(40)){ //mountain
      return mountain;
   }else if(getFlag(41)){ //snowymountain
      return snowymountain;
   }else if(getFlag(42)){ //sandydesrt
      return sandydesert;
   }else if(getFlag(43)){ //dirtdesert
      return dirtdesert;
   }else if(getFlag(44)){ //swamp
      return swamp;
   }else if(getFlag(45)){ //tropical
      return tropical;
   }else if(getFlag(46)){ //arctic
      return arctic;
   } else return cNONE;
}
