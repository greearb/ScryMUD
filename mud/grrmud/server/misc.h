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

#ifndef __MiscInclude__
#define __MiscInclude__

#include <string2.h>
#include "classes.h"
#include "object.h"
#include "room.h"
#include "door.h"
#include "critter.h"
#include "const.h"
#include <PtrArray.h>
#include "script.h"
#include "zone.h"

extern LazyPtrArray<room> room_list;
extern List<room*> embattled_rooms; 
extern List<room*> affected_rooms;
extern List<object*> obj_to_be_disolved_list; //grrmud.cc
extern List<critter*> pulsed_proc_mobs;
extern List<room*> pulsed_proc_rooms;
extern List<object*> pulsed_proc_objects;
extern PtrArray<room> proc_action_rooms;

// Inclusive.
int bound(int low, int high, int val);

int  d(const int num_rolls, const int dice_sides); 
//void critter::save();

int obj_sub_a_4_b(object* a, List<object*>& lst, 
                  const int i_th, const String* name, const int see_bit,
                  room& rm);
int crit_sub_a_4_b(critter* a, List<critter*>& lst, 
                   const int i_th, const String* name, const int see_bit,
                   room& rm);

short isnum(String& word);

                  /* in olc.cc */
short check_l_range(long n, long l, long h, critter& pc, short disp_err,
                    const char* msg = NULL);
            /*************************/


void join_in_battle(critter& agg, critter& vict);

int  get_a_character(critter& pc);
int  create_pc(critter& pc); //prompts for name, password...creates 
int  add_obj_to_permlist(const object& obj); 
void close_files();  

int detect(int see_bit, int vis_bit); //does bit comparison

void show(const char* message, critter& pc);
void show_all_but_2(critter& A, critter& B, const char* message, 
                    room& rm); 
void show_all_info(const char* msg); //to entire pc_list flaged for info
void show_all(const char* msg, const room& rm);

void out_str(const List<String*>& lst, critter& pc); 
void out_crit(const List<critter*>& lst, critter& pc);  
void out_inv(const List<object*>& lst, critter& pc, const short l_type); 
         //outs the names object*, formats according to l_type


	 /* searches the given list, lst will not be modified */
critter* have_crit_named(List<critter*>& lst, const int i_th, 
         const String* name, const int see_bit, const room& rm,
	 int do_exact = FALSE); 


object*  have_obj_named(const List<object*>& lst, const int i_th, 
         const String* name, const int see_bit, const room& rm);

//String* critter::getName(int see_bit);
const String* name_of_crit(critter& pc, int see_bit);

String* name_of_door(const door& dr, int see_bit);
String* name_of_dr_data(const door_data& dr, int see_bit, int dest);
String* name_of_obj(const object& obj, int see_bit);
String* long_name_of_obj(const object& obj, int see_bit);
String* name_of_room(const room& rm, int see_bit);
char* get_his_her(const critter& crit);
char* get_hisself_herself(const critter& crit);
char* get_he_she(const critter& crit);

int obj_is_named(const object& obj, const String& name);
int mob_is_named(const critter& pc, const String& name);
int door_is_named(const door_data& dr, const String& name);

short is_banned(const String& hostname);
short is_newbie_banned(const String& ip);

void disolve_object(object& obj);
void do_disolve_object(object& obj);

void decrease_timed_affecting_pcs();
void decrease_timed_affecting_lds();
void decrease_timed_affecting_doors();
void decrease_timed_affecting_smobs();
void decrease_timed_affecting_objects();
void decrease_timed_affecting_rooms();
void do_regeneration_pcs();
void do_regeneration_smobs();
void do_regeneration_objects();
void do_regeneration_rooms();
void do_regeneration_zones();
void update_zone(int i_th, short total_read);
void update_critters(int i_th, short read_all);
void update_objects(int i_th, short read_all);
void do_tick();
void do_mini_tick(); //every round of battle....


#endif 
