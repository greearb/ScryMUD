// $Id: olc2.h,v 1.7 1999/06/05 23:29:14 greear Exp $
// $Revision: 1.7 $  $Author: greear $ $Date: 1999/06/05 23:29:14 $

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

///************************  olc2.h  *****************************///

#ifndef Olc2Include
#define Olc2Include

#include "const.h"
#include <string2.h>
#include "classes.h"


int do_create_construction(int rslt, int comp1, int comp2, int comp3, int comp4,
                       int comp5, critter& pc, ComponentEnum con_type);

int create_concoction(int rslt, int comp1, int comp2, int comp3, int comp4,
                       int comp5, critter& pc);

int create_construction(int rslt, int comp1, int comp2, int comp3, int comp4,
                         int comp5, critter& pc);

int save_mob(int i_th, String* mob_name, critter& pc);
int save_obj(int i_th, String* obj_name, critter& pc);

int normalize_obj(object& obj); //make it fit w/in standards as much
                                 //as possible, set 1024 vis bit...
int normalize_mob(critter& obj);
int normalize_room(room& rm);
int normalize_door(door_data& dr);

int ch_rname(const String* rname, critter& pc);
int ch_rdesc(critter& pc);  
int ch_kdesc(int idx, critter& pc);

int ch_path_desc(int veh_id, int path_cell_num, critter& pc);
int set_path_dir(int veh_id, int path_cell_num, int i_th,
                  const String* dir, critter& pc);

int add_keyword(critter& pc);
int rm_keyword(int idx, critter& pc);
int clear_keyword(int idx, critter& pc);
int add_kname(int idx, const String* name, critter& pc);
int stat_keyword(int kwd_num, critter& pc);

// inserts an empty path in the slot:  path_cell_num
int add_path(int veh_id, int path_cell_num, critter& pc);
int rem_path(int veh_id, int path_cell_num, critter& pc);
int rm_stat_affect(int onum, int stat_num, critter& pc);
int add_stat_affect(int onum, int stat_num, int val, critter& pc);

int rm_casts_spell(int onum, int stat_num, critter& pc);
int add_casts_spell(int onum, int stat_num, int val, critter& pc);

int tog_veh_stop(int veh_id, int path_cell_num, critter& pc);
int list_paths(int veh_id, critter& pc);
int stat_path(int veh_id, int path_cell_num, critter& pc);
int set_path_pointer(int veh_id, int index, critter& pc);
int set_veh_stop(int veh_id, int path_cell_num,
                  const String* yes_no, critter& pc);

int ch_mdesc(int i_th, critter& pc);
int ch_mndesc(int mob_num, const String* desc, critter& pc);
int ch_msdesc(int mob_num, const String* desc, critter& pc);

int ch_ondesc(int obj_num, const String* desc, critter& pc);
int ch_osdesc(int obj_num, const String* desc, critter& pc);
int ch_odesc(int i_th, critter& pc);

int ch_ddesc(int i_th, critter& pc);

int do_ch_desc(critter& pc);  //does stuff, and exits upon '~'


int add_mob_script(critter& pc, int mob_num, String& trigger_cmd,
                    int actor_num, String& descriminator, int target_num,
                    int takes_precedence);
int do_add_mob_script(critter& pc);

int add_room_script(critter& pc, int rm_num, String& trigger_cmd,
                     int actor_num, String& descriminator, int target_num,
                     int takes_precedence);
int do_add_room_script(critter& pc);

int add_obj_script(critter& pc, int obj_num, String& trigger_cmd,
                   int actor_num, String& descriminator, int target_num,
                   int takes_precedence);
int do_add_obj_script(critter& pc);


int clear_mnames(int obj_num, critter& pc);
int clear_onames(int obj_num, critter& pc);

int add_oname(int obj_num, const String* name, critter& pc);
int add_mname(int mob_num, const String* name, critter& pc);
int rem_oname(int obj_num, const String* name, critter& pc);
int rem_mname(int mob_num, const String* name, critter& pc);

int reset_olc(critter& pc);
int add_proc(int mnum, critter& pc);
int rm_give_proc(int mnum, critter& pc);
int rm_discuss_proc(int mnum, critter& pc);
int rm_curse_proc(int mnum, critter& pc);
int rm_bow_proc(int mnum, critter& pc);

int make_pso(int i_th, const String* shop_keeper, critter& pc);

#endif 
