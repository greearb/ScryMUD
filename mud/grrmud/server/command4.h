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

/************************  command4.h  ***************************/
/*  Contains all the prototypes for all commands found in command4.cc */
/*  These commands are mostly imm-only commands, for building.        */


#ifndef Commands4Include
#define Commands4Include

#include "const.h"
#include "classes.h"
#include "room.h"
#include "object.h"
#include "critter.h"
#include "door.h"

void roll(int i, int j, critter& pc);
void uptime(critter& pc);
void scan(critter& pc);
void passwd(const String* old, const String* new1, const String* new2,
            critter& pc);
void unsiteban(const String* site, critter& pc);
void list_site_bans(critter& pc);
void siteban(const String* site, critter& pc);

void snoop(int i_th, const String* vict, critter& pc);
void unsnoop(critter& pc);
void possess(int i_th, const String* vict, critter& pc);
//void unpossess(critter& pc);  called directly before processing input, critter.h

void unpost(int i_th, const String* post, critter& pc);
void write_board(object& obj);
void title(const String* tit, critter& pc);
void poofin(String* tit, critter& pc);
void poofout(String* tit, critter& pc);
void oreload(critter& pc);
void mreload(critter& pc);
void read(int i_th, const String* post, critter& pc);
void post(const String* title, critter& pc);
void do_post(critter& pc);
void log_out(critter& pc);
void go(int i_th, const String* direction, critter& pc, int& is_dead);
void _goto(int i_th, const String* name, critter& pc); //goto room numbered
void where(int i_th, const String* name, critter& pc); 
void zgoto(int i_th, critter& pc); //goto zone numbered

void rclear(int i_th, critter& pc); //these three can delete from DB when
void mclear(int i_th, critter& pc); //used with asave, amsave, aosave
void dclear(int i_th, critter& pc); //used with adsave
void oclear(int i_th, critter& pc); //respectively.

void brief(critter& pc);
void olist(int start, int end, critter& pc);
void mlist(int start, int end, critter& pc);
void rlist(int start, int end, critter& pc);
void dlist(int start, int end, critter& pc);
void adlist(critter& pc);
void zlist(int start, int end, critter& pc);

void tog_rflag(int flagnum, critter& pc);
void tog_zflag(int flagnum, critter& pc); //loop through all rooms in zone.
void tog_oflag(int flagnum, const String* flag_type, 
               int i_th, const String* obj, critter& pc); 
void tog_mflag(int flagnum, const String* flag_type, 
               int i_th, const String* mob, critter& pc); 

void rset(const String* targ, int new_value, critter& pc);
void mset(int i_th, const String* vict, const String* targ, 
	  int new_value, const String* new_string_val, critter& pc);
void oset(int i_th, const String* vict, const String* targ,
	  int new_value, const String* new_val_string, critter& pc);
void dset(int i_th, const String* vict, const String* targ,
	  int new_value, critter& pc);

void give(int i_th, const String* obj, int i_th, const String* targ,
               critter& pc);  //does checks, mob_to_smob
void do_give(critter& targ, critter& pc, object& obj);  //does actual work

void prompt(const String* pr, critter& pc);
void zlock(int zone_num, critter& pc);
void zunlock(int zone_num, critter& pc);

void write_zone(int zone_num, critter& pc); //writes zone pc is in..if locked
void do_write_zone(int znum);

void replace_door(int new_door_num, const String* direction, critter& pc);
void delete_door(int i_th, const String* door, critter& pc);
int  door_to(int dest_room_num, int distance,
     const String* exit_direction, critter& pc);
void do_door_to(room& cur_room, room& dest_room, int distance,
     critter& pc, const String* exit_direction, int is_gate = FALSE);
void flip_door(int i_th, const String* door, critter& pc);


///**********************************************************************///
///************************  auxillary functions  ***********************///
///**********************************************************************///

void do_lock_room(room& rm);
void do_unlock_room(room& rm);
int num_of_generic_door(const String* direction);
void do_gecho(const char* msg);

#endif //include
