// $Id: skills.h,v 1.5 2001/03/29 03:02:34 eroper Exp $
// $Revision: 1.5 $  $Author: eroper $ $Date: 2001/03/29 03:02:34 $

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

#ifndef SkillsInclude_h
#define SkillsInclude_h

#include "const.h"
#include <string2.h>
#include "classes.h"
#include "spells.h"


#define EARTHMELD_BATTLE_MANA_COST 5
#define EARTHMELD_NON_BATTLE_MANA_COST 1


int bash(int i_th, const String* vict, critter& pc);
int do_bash(critter& vict, critter& pc);
int do_bash(door& vict, critter& pc);

int block(int i_th, const String* vict, critter& pc);
int do_block(critter& vict, critter& pc);
int do_block(door& vict, critter& pc);

int claw(int i_th, const String* vict, critter& pc);
int do_claw(critter& vict, critter& pc);

int kick(int i_th, const String* vict, critter& pc);
int do_kick(critter& vict, critter& pc);

int steal(int i_th, const String* obj, int j_th, const String* vict,
           critter& pc);
int do_steal(object* obj, critter& vict, critter& pc, short is_gold = FALSE);

int trip(int i_th, const String* vict, critter& pc);
int do_trip(critter& vict, critter& pc);

int construct(critter& pc, short do_mob = FALSE);
int concoct(critter& pc, short do_mob = FALSE);

int scribe(const String* spell, critter& pc, short do_mob = FALSE);
int track(int i_th, const String* vict, critter& pc);

int sneak(critter& pc, int smob_too = FALSE);
int hide(critter& pc, int smob_too = FALSE);
int blend(critter& pc, int smob_too = FALSE);

int scan(critter& pc);


///*****************  auxillary functions  ***************************///

short skill_did_hit(critter& agg, int skill_num, critter& vict);
short did_spell_hit(const critter& agg, const int spell_type,
                    const critter& vict, 
                    int lvl = -1, short is_canned = FALSE);
 
#endif 


