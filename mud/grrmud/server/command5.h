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

/************************  command5.h  ***************************/
/*  Contains all the prototypes for all commands found in comman5.cc */


#ifndef Commands5Include
#define Commands5Include

#include "const.h"
#include "classes.h"



// SMOB only, used in scripts
void pause(int rounds, critter& pc);

void page_break(int lines, critter& pc);

void color(String& var, String& val, critter& pc);

/* Toggle client usage to TRUE */
void using_client(critter& pc);

void update_cur_in_game(critter& pc);

void list_scripts(int mob_num, critter& pc);
void stat_script(int mob_num, int script_idx, critter& pc);
void rem_script(int mob_num, String& trigger, int i_th, critter& pc);

void list_room_scripts(int rm_num, critter& pc);
void stat_room_script(int rm_num, int script_idx, critter& pc);
void rem_room_script(int mob_num, String& trigger, int i_th, critter& pc);

void slist(int begin, int howmany, critter& pc);
void teach(int i_th, const String* name, int prcnt, const String* skill, 
	   critter& pc);
void add_perm_inv(int i_th, const String* name, int obj_num, critter& pc);
void rem_perm_inv(int i_th, const String* name, int j_th,
		  const String* mob_name, critter& pc);
void beep(int i_th, const String* name, critter& pc);
void opurge(int i_th, const String* name, critter& pc);
void do_transport(critter& targ, critter& pc, room& dest);
void itrans(int i_th, const String* targ, int rm_num, critter& pc);
void discuss(const String* topic, int i_th, const String* targ, critter& pc);
void suicide(const String* passwd,  critter& pc);
void pay(int amount, int i_th, const String* targ,  critter& pc);
void mvnum(int i_th, const String* name, critter& pc);
void ovnum(int i_th, const String* name, critter& pc);
void dvnum(int i_th, const String* name, critter& pc);
void arlist(int zone, int how_many, critter& pc);
void aolist(int zone, int how_many, critter& pc);

void write_world(critter& pc);

void aosave(int zone, critter& pc);
void do_aosave(int zone);

void amsave(int zone, critter& pc);
void do_amsave(int zone);

void adsave(int zone, critter& pc);
void do_adsave(int zone);

void dsys(critter& pc);

void amlist(int zone, int how_many, critter& pc);
void make_builder(int i_th, const String* name, critter& pc);
void gag(int i_th, const String* targ, critter& pc);
void freeze(int i_th, const String* targ, critter& pc);
void ungag(int i_th, const String* targ, critter& pc);
void thaw(int i_th, const String* targ, critter& pc);
void com_emote(const String* msg, critter& pc);
void com_pemote(const String* msg, critter& pc);
void com_gecho(const String* msg, critter& pc);
void com_recho(const String* msg, critter& pc);
void com_pecho(int i_th, const String* targ, const String* msg, critter& pc);
void com_zecho(const String* msg, critter& pc);
void prone(critter& pc);
void shoot(int i_th, const String* dir, int j_th, const String* mob, 
	   critter& pc);
void do_shoot(critter& targ, critter& pc);
void _throw(int i_th, const String* dir, int j_th, const String* mob, 
	   critter& pc);
void do_throw(critter& targ, critter& pc);

void zcreate(int num_rooms, const String* name, int num_ticks,
	     critter& pc);
void write_zone_list();
void zenable(int znum, const String* char_name, critter& pc);
void zdisable(int znum, const String* char_name, critter& pc);
void ch_zname(int znum, const String* zone_name, critter& pc);

// Allows a player to set his/her own description.
void describe(critter& pc);

void see_all(critter& pc);
void wizinvis(critter& pc);
void visible(critter& pc);

void consider(int i_th, const String* vict, critter& pc);

short did_shot_hit(critter& targ, critter& pc, int throwing = FALSE);

//  These next 5 relate to player-run shops
void value_set(int i_th, const String* targ, int val_idx,
               int sell_val, int buy_val, critter& pc);

void value_list(int i_th, const String* targ, critter& pc);
void value_add(int i_th, const String* targ, int j_th,
               const String* obj, critter& pc);
void value_rem(int i_th, const String* targ, int val_idx, critter& pc);
void adjust_register(int i_th, const String* targ, int new_balance, critter& pc);


int handle_olc(critter& pc, int do_sub);

#endif




