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

/************************  command5.h  ***************************/
/*  Contains all the prototypes for all commands found in comman5.cc */


#ifndef Commands5Include
#define Commands5Include

#include "const.h"
#include "classes.h"



int test(critter& pc);

int withdraw(int i_th, const String& coins, int j_th,
             const String& banker, critter& pc);

int balance(int i_th, const String* banker, critter& pc);

int deposit(int i_th, const String* coins, int j_th,
            const String* banker, critter& pc);

// SMOB only, used in scripts
int pause(int rounds, critter& pc, int was_ordered = FALSE);

int page_break(int lines, critter& pc);

int color(String& var, String& val, critter& pc);

/* Toggle client usage to TRUE */
bool using_client(critter& pc);

int update_cur_in_game(critter& pc);

int list_scripts(int mob_num, critter& pc);
int do_list_scripts(critter& targ, critter& pc);
int stat_script(int mob_num, int script_idx, critter& pc);
int rem_script(int mob_num, String& trigger, int i_th, critter& pc);

int list_room_scripts(int rm_num, critter& pc);
int stat_room_script(int rm_num, int script_idx, critter& pc);
int rem_room_script(int rm_num, String& trigger, int i_th, critter& pc);

int list_obj_scripts(int obj_num, critter& pc);
int stat_obj_script(int obj_num, int script_idx, critter& pc);
int rem_obj_script(int obj_num, String& trigger, int i_th, critter& pc);

int slist(int begin, const String& name, int howmany, critter& pc);
int teach(int i_th, const String* name, int prcnt, const String* skill, 
           critter& pc);
int add_perm_inv(int obj_num, int i_th, const String* name, critter& pc);
int rem_perm_inv(int i_th, const String* name, int j_th,
                 const String* mob_name, critter& pc);
int beep(int i_th, const String* name, critter& pc);
int opurge(int i_th, const String* name, critter& pc);
int do_transport(critter& targ, critter& pc, room& dest);
int itrans(int i_th, const String* targ, int rm_num, critter& pc);
int discuss(const String* topic, int i_th, const String* targ, critter& pc);
int suicide(const String* passwd,  critter& pc);
int pay(int amount, int i_th, const String* targ,  critter& pc);
int mvnum(int i_th, const String* name, critter& pc);
int ovnum(int i_th, const String* name, critter& pc);
int dvnum(int i_th, const String* name, critter& pc);
int arlist(int zone, int how_many, critter& pc);
int aolist(int zone, int how_many, critter& pc);

int write_world(critter& pc);

int aosave(int zone, critter& pc);
int do_aosave(int zone);

int amsave(int zone, critter& pc);
int do_amsave(int zone);

int adsave(int zone, critter& pc);
int do_adsave(int zone);

int dsys(int i, const String& cmd, const String& arg1, critter& pc);

int amlist(int zone, int how_many, critter& pc);
int make_builder(int i_th, const String* name, critter& pc);
int gag(int i_th, const String* targ, critter& pc);
int freeze(int i_th, const String* targ, critter& pc);
int ungag(int i_th, const String* targ, critter& pc);
int thaw(int i_th, const String* targ, critter& pc);
int com_emote(const String* msg, critter& pc);
int com_pemote(const String* msg, critter& pc);
int com_gemote(const String* msg, critter& pc);
int com_gecho(const String* msg, critter& pc);
int com_recho(const String* msg, critter& pc);
int com_pecho(int i_th, const String* targ, const String* msg, critter& pc);
int com_zecho(const String* msg, critter& pc);
int prone(critter& pc);
int shoot(int i_th, const String* dir, int j_th, const String* mob, 
           critter& pc);
int do_shoot(critter& targ, critter& pc);
int _throw(int i_th, const String* dir, int j_th, const String* mob, 
           critter& pc);
int do_throw(critter& targ, critter& pc);

int zcreate(int num_rooms, const String* name, int num_ticks,
             critter& pc);
int write_zone_list();
int zenable(int znum, const String* char_name, critter& pc);
int zdisable(int znum, const String* char_name, critter& pc);
int ch_zname(int znum, const String* zone_name, critter& pc);

// Allows a player to set his/her own description.
int describe(critter& pc);

int see_all(critter& pc);
int wizinvis(critter& pc);
int visible(critter& pc);

int consider(int i_th, const String* vict, critter& pc);

int did_shot_hit(critter& targ, critter& pc, int throwing = FALSE);

//  These next 5 relate to player-run shops
int value_set(int i_th, const String* targ, int val_idx,
               int sell_val, int buy_val, critter& pc);

int value_list(int i_th, const String* targ, critter& pc);
int value_add(int i_th, const String* targ, int j_th,
               const String* obj, critter& pc);
int value_rem(int i_th, const String* targ, int val_idx, critter& pc);
int adjust_register(int i_th, const String* targ, int new_balance, critter& pc);


int handle_olc(critter& pc, int do_sub);

void client_command(const String* vict, critter& pc);

int set_veh_croom(int vhcl, int rm, critter& pc);

int weatherreport(critter& pc);
int changeweather(critter& pc);
int setweather(const String* climate_str, const String* weather_str, critter& pc);
#endif




