// $Id: command4.h,v 1.6 1999/06/14 06:05:43 greear Exp $
// $Revision: 1.6 $  $Author: greear $ $Date: 1999/06/14 06:05:43 $

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

int roll(int i, int j, critter& pc);
int uptime(critter& pc);
int passwd(const String* old, const String* new1, const String* new2,
            critter& pc);
int unsiteban(const String* site, critter& pc);
int list_site_bans(critter& pc);
int siteban(const String* site, critter& pc);

int snoop(int i_th, const String* vict, critter& pc);
int unsnoop(critter& pc);
int possess(int i_th, const String* vict, critter& pc);
//int unpossess(critter& pc);  called directly before processing input, critter.h

int unpost(int i_th, const String* post, critter& pc);
int write_board(object& obj);
int title(const String* tit, critter& pc);
int poofin(String* tit, critter& pc);
int poofout(String* tit, critter& pc);
int oreload(critter& pc);
int mreload(critter& pc);
int read(int i_th, const String* post, int j_th, const String* board, critter& pc);
int post(const String* title, critter& pc);
int do_post(critter& pc);
int log_out(critter& pc);
int go(int i_th, const String* direction, critter& pc, int& is_dead);
int _goto(int i_th, const String* name, critter& pc); //goto room numbered
int where(int i_th, const String* name, critter& pc); 
int zgoto(int i_th, critter& pc); //goto zone numbered

int rclear(int i_th, critter& pc); //these three can delete from DB when
int mclear(int i_th, critter& pc); //used with asave, amsave, aosave
int dclear(int i_th, critter& pc); //used with adsave
int oclear(int i_th, critter& pc); //respectively.

int brief(critter& pc);
int olist(int start, int end, critter& pc);
int mlist(int start, int end, critter& pc);
int rlist(int start, int end, critter& pc);
int dlist(int start, int end, critter& pc);
int adlist(critter& pc);
int zlist(int start, int end, critter& pc);

//loop through all rooms in zone.
int set_zflag(int flagnum, const String* on_off, critter& pc);

int tog_rflag(int flagnum, critter& pc);
int tog_oflag(int flagnum, const String* flag_type, 
               int i_th, const String* obj, critter& pc); 
int tog_mflag(int flagnum, const String* flag_type, 
               int i_th, const String* mob, critter& pc); 

int rset(const String* targ, int new_value, critter& pc);
int mset(int i_th, const String* vict, const String* targ, 
	  int new_value, const String* new_string_val, critter& pc);
int oset(int i_th, const String* vict, const String* targ,
	  int new_value, const String* new_val_string, critter& pc);
int dset(int i_th, const String* vict, const String* targ,
	  int new_value, critter& pc);

int give(int i_th, const String* obj, int i_th, const String* targ,
               critter& pc);  //does checks, mob_to_smob
int do_give(critter& targ, critter& pc, object& obj);  //does actual work

int prompt(const String* pr, critter& pc);
int zlock(int zone_num, critter& pc);
int zunlock(int zone_num, critter& pc);

int write_zone(int zone_num, critter& pc); //writes zone pc is in..if locked
int do_write_zone(int znum);

int replace_door(int new_door_num, const String* direction, critter& pc);
int delete_door(int i_th, const String* door, critter& pc);
int  door_to(int dest_room_num, int distance,
     const String* exit_direction, critter& pc);
int do_door_to(room& cur_room, room& dest_room, int distance,
     critter& pc, const String* exit_direction, int is_gate = FALSE);
int flip_door(int i_th, const String* door, critter& pc);


///**********************************************************************///
///************************  auxillary functions  ***********************///
///**********************************************************************///

int do_lock_room(room& rm);
int do_unlock_room(room& rm);
int num_of_generic_door(const String* direction);
int do_gecho(const char* msg);

#endif //include
