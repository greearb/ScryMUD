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

///************************  Spells.h  *****************************///

#ifndef SpellsInclude
#define SpellsInclude

#include "const.h"
#include <string2.h>
#include "classes.h"
#include "room.h"
#include "critter.h"
#include "object.h"
#include "door.h"

#define SHADOWS_BLESSING_EFFECT           15
#define ARMOR_EFFECT		          30
#define POISON_HP_REGEN_AUGMENTATION     -75
#define POISON_MV_REGEN_AUGMENTATION     -75
#define POISON_MA_REGEN_AUGMENTATION     -75
#define FIRE_PROOF_EFFECT                -60
#define WEAKEN_EFFECT                    -2
#define STRENGTH_EFFECT                   2

void cast_fireball(int i_th, const String* vict, critter& agg);
void cast_poison(int i_th, const String* vict, critter& agg);
void cast_illuminate(critter& pc);

void do_cast_fireball(critter& agg, critter& vict, int is_canned, int lvl);
void do_cast_poison(critter& agg, critter& vict, int is_canned, int lvl);
void do_cast_illuminate(room& rm, critter& agg, int is_canned, int lvl);

void cast_teleport(critter& pc);
void do_cast_teleport(critter& vict, critter& pc, int is_canned, int lvl);

void cast_passdoor(int i_th, const String* vict, critter& pc);
void do_cast_passdoor(door& dr, critter& vict, int is_canned, int lvl);

void cast_recall(int i_th, const String* vict, critter& pc);
void do_cast_recall(critter& vict, critter& pc, int is_canned, int lvl);

void cast_summon(int i_th, const String* vict, critter& pc);
void do_cast_summon(critter& vict, critter& pc, int is_canned, int lvl);


///********** effects of passive spells (when triggered **********///

void do_firewall_effects(critter& pc, int& is_dead);
void do_distortion_wall_effects(critter& pc, int& is_dead);


///**************   auxillary spell/skill functions   *************///

void rem_effects_crit(int spell_num, critter& pc, short do_msg);
void rem_effects_obj(int spell_num, object& obj);
void rem_effects_room(int spell_num, room& rm, short do_msg);
void rem_effects_door(int spell_num, door& dr, room& rm1, room& rm2,
		      short do_msg);

int relocate_within_zone(critter& pc); //silent teleport

int has_all_prereqs(int skill_num, critter& pc); 
int get_skill_difficulty(int skill_num);


int get_min_skill_level(int skill_num);

void update_skill(int last_gained, critter& pc); //used in practice
void update_skills(critter& pc);

short lost_concentration(critter& agg, int spell_num);

int get_mana_cost(int spell_num);
//int get_mana_cost(const char* name);

int get_number_of_scroll(int spell_num); //returns -1 if !exist
 
#endif 








