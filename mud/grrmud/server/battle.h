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

///**********************  Battle and related functions ******************///
#ifndef BattleInclude
#define BattleInclude

#include "classes.h"
#include "room.h"
#include "critter.h"
#include "object.h"
#include "door.h"


void do_battle();
void do_battle_round(critter& agg, critter& vict, int posn_of_weapon);

void agg_kills_vict(critter& agg, critter& vict);  //death of a critter
void crit_dies(critter& pc); //indirect deaths

void disburse_xp(critter& agg, const critter& vict);
void dead_crit_to_corpse(critter& vict);
void gain_xp(critter& crit, const long xp, const short do_output); 
void gain_level(critter& crit);
short can_start_battle(critter& targ, critter& pc, short do_msg);

int exact_raw_damage(int damage, int damage_type, critter& pc,
                     critter& agg);
int exact_raw_damage(int damage, int damage_type, critter& pc);

critter* mob_to_smob(critter& mob, const int room_num, int do_sub,
                     int i_th, const String* name, int see_bit);
critter* mob_to_smob(critter& mob, const int room_num,
                     int supress_sub_fail_msg = FALSE);

// Use this one when you can, as opposed to the one above.
critter* mob_to_smob(critter& mob, room& rm,
                     int supress_sub_fail_msg = FALSE);

object*  obj_to_sobj(object& obj, List<object*>* in_list, int do_sub,
                     int i_th, const String* name, int see_bit, room& rm);
object*  obj_to_sobj(object& obj, List<object*>* in_list, int rm_num);


#endif
