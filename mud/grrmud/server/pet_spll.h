// $Id$
// $Revision: 1.3 $  $Author$ $Date$

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

///************************  pet_spll.h  *****************************///


#ifndef Spells_pet_Include
#define Spells_pet_Include

#include "const.h"
#include <string2.h>
#include "classes.h"


void cast_create_greater_golem(critter& pc);
void do_cast_create_greater_golem(critter& pc, int is_canned, int lvl);


void cast_charm(int i_th, const String* targ, critter& agg);
void do_cast_charm(critter& targ, critter& pc, int is_canned,
                   int lvl);

void cast_mass_charm(critter& agg);
void do_cast_mass_charm(room& rm, critter& pc, int is_canned,
                        int lvl);

void cast_illusion(critter& agg);
void do_cast_illusion(critter& pc, int is_canned, int lvl);

void cast_create_food(critter& agg);
void do_cast_create_food(critter& pc, int is_canned, int lvl);

void cast_heros_feast(critter& agg);
void do_cast_heros_feast(critter& pc, int is_canned, int lvl);

void cast_create_golem(critter& agg);
void do_cast_create_golem(critter& pc, int is_canned, int lvl);

void cast_create_light(critter& agg);
void do_cast_create_light(critter& pc, int is_canned, int lvl);

void cast_raise_undead(critter& agg);
void do_cast_raise_undead(critter& pc, int is_canned, int lvl);

void cast_conjure_minion(critter& agg);
void do_cast_conjure_minion(critter& pc, int is_canned, int lvl);

void cast_conjure_horde(critter& agg);
void do_cast_conjure_horde(critter& pc, int is_canned, int lvl);



#endif 
