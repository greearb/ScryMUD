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

///************************  Skills.h  *****************************///

#ifndef SkillsInclude
#define SkillsInclude

#include "const.h"
#include <string2.h>
#include "classes.h"
#include "spells.h"

void bash(int i_th, const String* vict, critter& pc);
void do_bash(critter& vict, critter& pc);
void do_bash(door& vict, critter& pc);

void block(int i_th, const String* vict, critter& pc);
void do_block(critter& vict, critter& pc);
void do_block(door& vict, critter& pc);

void claw(int i_th, const String* vict, critter& pc);
void do_claw(critter& vict, critter& pc);

void kick(int i_th, const String* vict, critter& pc);
void do_kick(critter& vict, critter& pc);

void steal(int i_th, const String* obj, int j_th, const String* vict,
	   critter& pc);
void do_steal(object* obj, critter& vict, critter& pc, short is_gold = FALSE);

void trip(int i_th, const String* vict, critter& pc);
void do_trip(critter& vict, critter& pc);

void construct(critter& pc, short do_mob = FALSE);
void concoct(critter& pc, short do_mob = FALSE);

void scribe(const String* spell, critter& pc, short do_mob = FALSE);
void track(int i_th, const String* vict, critter& pc);

///*****************  auxillary functions  ***************************///

short skill_did_hit(critter& agg, int skill_num, critter& vict);
short did_spell_hit(const critter& agg, const int spell_type,
		    const critter& vict, 
		    int lvl = -1, short is_canned = FALSE);
 
#endif 


