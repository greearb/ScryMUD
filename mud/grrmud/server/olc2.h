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


void do_create_construction(int rslt, int comp1, int comp2, int comp3, int comp4,
                       int comp5, critter& pc, ComponentEnum con_type);

void create_concoction(int rslt, int comp1, int comp2, int comp3, int comp4,
                       int comp5, critter& pc);

void create_construction(int rslt, int comp1, int comp2, int comp3, int comp4,
                         int comp5, critter& pc);

void save_mob(int i_th, String* mob_name, critter& pc);
void save_obj(int i_th, String* obj_name, critter& pc);

void normalize_obj(object& obj); //make it fit w/in standards as much
                                 //as possible, set 1024 vis bit...
void normalize_mob(critter& obj);
void normalize_room(room& rm);
void normalize_door(door_data& dr);

void ch_rname(const String* rname, critter& pc);
void ch_rdesc(critter& pc);  
void ch_kdesc(int idx, critter& pc);

void ch_path_desc(int veh_id, int path_cell_num, critter& pc);
void set_path_dir(int veh_id, int path_cell_num, int i_th,
                  const String* dir, critter& pc);

void add_keyword(critter& pc);
void rm_keyword(int idx, critter& pc);
void clear_keyword(int idx, critter& pc);
void add_kname(int idx, const String* name, critter& pc);
void stat_keyword(int kwd_num, critter& pc);

// inserts an empty path in the slot:  path_cell_num
void add_path(int veh_id, int path_cell_num, critter& pc);
void rem_path(int veh_id, int path_cell_num, critter& pc);
void rm_stat_affect(int onum, int stat_num, critter& pc);
void add_stat_affect(int onum, int stat_num, int val, critter& pc);

void rm_casts_spell(int onum, int stat_num, critter& pc);
void add_casts_spell(int onum, int stat_num, int val, critter& pc);

void tog_veh_stop(int veh_id, int path_cell_num, critter& pc);
void list_paths(int veh_id, critter& pc);
void stat_path(int veh_id, int path_cell_num, critter& pc);
void set_path_pointer(int veh_id, int index, critter& pc);
void set_veh_stop(int veh_id, int path_cell_num,
                  const String* yes_no, critter& pc);

void ch_mdesc(int i_th, critter& pc);
void ch_mndesc(int mob_num, const String* desc, critter& pc);
void ch_msdesc(int mob_num, const String* desc, critter& pc);

void ch_ondesc(int obj_num, const String* desc, critter& pc);
void ch_osdesc(int obj_num, const String* desc, critter& pc);
void ch_odesc(int i_th, critter& pc);

void ch_ddesc(int i_th, critter& pc);

void do_ch_desc(critter& pc);  //does stuff, and exits upon '~'


void add_mob_script(critter& pc, int mob_num, String& trigger_cmd,
                    int actor_num, String& descriminator, int target_num,
                    int takes_precedence);
void do_add_mob_script(critter& pc);


void clear_mnames(int obj_num, critter& pc);
void clear_onames(int obj_num, critter& pc);

void add_oname(int obj_num, const String* name, critter& pc);
void add_mname(int mob_num, const String* name, critter& pc);
void rem_oname(int obj_num, const String* name, critter& pc);
void rem_mname(int mob_num, const String* name, critter& pc);

void reset_olc(critter& pc);
void add_proc(int mnum, critter& pc);
void rm_give_proc(int mnum, critter& pc);
void rm_discuss_proc(int mnum, critter& pc);
void rm_curse_proc(int mnum, critter& pc);
void rm_bow_proc(int mnum, critter& pc);


#endif 
