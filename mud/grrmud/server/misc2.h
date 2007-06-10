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
// To contact the Author, Ben Greear:  greear@cyberhighway.net, (preferred)
//                                     greearb@agcs.com
//

#ifndef Misc2Include
#define Misc2Include

#include "const.h"
#include "string2.h"
#include "classes.h"
#include "list2.h"
#include "object.h"
#include "critter.h"
#include "door.h"
#include "room.h"
#include <vector>

int core_dump(const char* msg);
short a_will_help_b_against_c(critter& a, critter& b, critter& pc);
void lose_fly(critter& pc, short do_msg = FALSE);
void unpetrify(critter& pc, short do_msg = FALSE);
void show_stat_affects(object& obj, critter& pc);

critter* check_for_diversions(critter& pc, char* tests, critter& agg);

/** Tests this performs:  B(!pc in_battle)  C(owns aux_crit) F(frozen)
 *                        G(!gagged) I(is immort) K(know spell), M(has mana), 
 *                        m(!mob, smob ok), N(!magic),
 *                        P(paralyzed), R(owns aux_rm), r(resting or standing)
 *                        S(is_standing), V(!violence), Z(room is zlocked)
 *                                                
 *  Case matters.
 *  Evaluates the values (described in parentheses).  Returns TRUE only
 *  if all are true.
 */
int ok_to_do_action(critter* vict, const char* flags, int spell_num,
                    critter& pc, room* aux_rm = NULL, critter* aux_crit = NULL,
                    int do_msg = TRUE);

stat_spell_cell* is_affected_by(int spell_num, const critter& pc);
stat_spell_cell* is_affected_by(int spell_num, const object& obj);

stat_spell_cell* has_stat_affect(int affect_num, object& obj);

void leave_room_effects(room& rm, critter& pc, int check_proc);
void leave_room_effects(room& rm, object& obj);

int is_grouped(critter& crit, critter& pc);

const char* get_opposite_dir(const char* dir);
int get_race_num(const char* name);
int get_class_num(const char* name);
const char* get_race_name(int num);
const char* get_class_name(int num);

int max(int i, int j); //simple find-max function
int min(int i, int j); //simple find-max function

void save_all();  //pc_list and linkdead_list

String* direction_of_door(const door& dr);
const char* abbrev_dir_of_door(const door& dr);

void parse_communication(String& str);
void parse_for_max_80(String& str);
void strip_hegemon_tags(String& str);

String* dir_of_room(room& rm, int dest_room);
int get_next_msg_num(object& board);
String owner_of_zone(int rm_num);
short zone_is_total_loaded(int znum);
short zone_is_locked(int znum);



object* have_obj_numbered(const List<object*>& lst, const int i_th,
                          const int obj_num, const int see_bit,
                          const room& rm);

say_proc_cell* have_topic_named(List<say_proc_cell*> lst, const String& msg);

const String* next_mov_dir(room& car);
short is_car_at_dest(room& car);
short is_next_stop_a_dest(room& car);
void move_room(room& car, int i_th, const String* direction);
void do_vehicle_moves();    //moves em if needed
int mob_can_enter(critter& pc, room& rm, short do_msg, int check_no_wander = FALSE);
int car_can_enter(room& car, room& rm, short do_msg);

room* get_next_room(int zone);
int get_next_obj(); // returns # of first un-used obj in obj_list
int get_next_mob(); // returns # of first un-used mob in obj_list
int get_next_door(); // returns # of first un-used door in obj_list

int get_game_time(); //returns current game time
const char* get_month(int day);
const int get_day_of_month(int day);
const char* military_to_am(int m_time);
const char* class_of_crit(critter& pc);

int get_percent_lrnd(int skill_num, const critter& pc, short automatic = FALSE);
void increment_percent_lrnd(int skill_num, critter& pc);

void init_masks();
int obj_count(List<object*>& lst, object& src);
int crit_count(List<critter*>& lst, critter& src);
void clear_crit_list(PtrList<critter>& lst);
void clear_obj_list(PtrList<object>& lst);

void out_field(const bitfield& field, critter& pc, const BitfieldNames& names);
void out_stat_list(const List<stat_spell_cell*>& lst, critter& pc,
                   const BitfieldNames& names);
void out_spell_list(const List<stat_spell_cell*>& lst, critter& pc,
        int show_ostat_dat_flag);
//void critter::doBecomeNonPet();
const String* single_obj_name(object& obj, int see_bit);
short name_is_secret(const String* name, door& dr);

const char* get_himself_herself(critter& pc);
const char* get_him_her(critter& pc);
const char* get_dude_chic(critter& pc);
const char* get_fellow_lady(critter& pc);

// Attempt to load the player-run shop owner of that number.
// Returns a newly allocated SMOB, or NULL if a problem is
// encountered.
critter* load_player_shop_owner(int mob_num);
#ifdef USEMYSQL
critter* db_load_player_shop_owner(int mob_num);
#endif
critter* file_load_player_shop_owner(int mob_num);
int save_player_shop_owner(critter& mob_to_save);

object* load_player_box(int box_num);
#ifdef USEMYSQL
object* db_load_player_box(int box_num);
#endif
object* file_load_player_box(int box_num);
int save_player_box(object& obj);

int find_and_delete_obj(object* obj_to_find, int room_num);
int find_and_delete_obj(object* obj_to_find, critter* crit_ptr);
int find_and_delete_obj(object* obj_to_find, object* find_in);

char icharswap(char oldch, char newch);
String transform(const String &input,
      const PtrList<String> &wordpats, const PtrList<String> &wordreps,
      const PtrList<String> &fragpats, const PtrList<String> &fragreps);


critter* get_target_mob(int i_th, const String* target, critter& pc, char* diversions); //self explainatory
object* get_target_obj(int i_th, const String* target, critter& pc);
door* get_target_door(int i_th, const String* target, critter& pc);
room* get_target_room(critter& pc);
SeasonType get_season(int day);
String* get_season_string(int day);

//distributed probability matrix code
//only handles single dimension probability matrices
//multidimension would be nice though...
template <typename T>
class DistProbMatrix {
private:
	vector<T> matrix; //stores pointer to probability matrix
   unsigned int size; 
public:
	void add(T value, unsigned int weight){
		size += weight;
		matrix.reserve(size);
		for(unsigned int i = 0; i < weight; i++){
			matrix.push_back(value);
		}
	}
	T get(){return matrix[(rand()%(size-1))];}
	unsigned int Size(){ return size;}
	
   DistProbMatrix(){size = 0;}
};		

void combine_weights(int* target, const float* in, unsigned int length);

#endif 
