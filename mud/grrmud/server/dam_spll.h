// $Id: dam_spll.h,v 1.5 2002/01/31 11:45:54 gingon Exp $
// $Revision: 1.5 $  $Author: gingon $ $Date: 2002/01/31 11:45:54 $

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

#ifndef SpellsInclude_dam_spll
#define SpellsInclude_dam_spll

#include "const.h"
#include <string2.h>
#include "classes.h"
#include "spells.h"

/* define affects constants here */

//void cast_dark_spear(int i_th, const String* victim, critter& pc);
//void do_cast_dark_spear(critter& vict, critter& agg, int is_canned, int lvl);

class SpellSpearOfDarkness : public MobSpell{
	public:
	void doSpellEffects(); // effects of spell
};

extern SpellSpearOfDarkness spellSpearOfDarkness;

class SpellOrbOfPower : public MobSpell{
	public:
	void doSpellEffects(); // effects of spell
};

extern SpellOrbOfPower spellOrbOfPower;

//void cast_orb_of_power(int i_th, const String* dr, critter& agg);
//void do_cast_orb_of_power(critter& vict, critter& pc, int is_canned, int lvl);

void cast_rainbow(int i_th, const String* dr, critter& agg);
void do_cast_rainbow(critter& vict, critter& pc, int is_canned, int lvl);

void cast_lightning(int i_th, const String* dr, critter& agg);
void do_cast_lightning(critter& vict, critter& pc, int is_canned, int lvl);

void cast_group_heal(critter& agg);
void do_cast_group_heal(critter& pc, int is_canned, int lvl);

void cast_harm(int i_th, const String* dr, critter& agg);
void do_cast_harm(critter& vict, critter& pc, int is_canned, int lvl);

void cast_cause_sickness(int i_th, const String* dr, critter& agg);
void do_cast_cause_sickness(critter& vict, critter& pc, int is_canned,
                            int lvl);

void cast_cause_critical(int i_th, const String* dr, critter& agg);
void do_cast_cause_critical(critter& vict, critter& pc, int is_canned,
                            int lvl);

void cast_holy_word(int i_th, const String* dr, critter& agg);
void do_cast_holy_word(critter& vict, critter& pc, int is_canned,
                            int lvl);

void cast_shocking_grasp(int i_th, const String* dr, critter& agg);
void do_cast_shocking_grasp(critter& vict, critter& pc, int is_canned,
                            int lvl);
void cast_burning_hands(int i_th, const String* dr, critter& agg);
void do_cast_burning_hands(critter& vict, critter& pc, int is_canned,
                           int lvl);
void cast_dark_dart(int i_th, const String* dr, critter& agg);
void do_cast_dark_dart(critter& vict, critter& pc, int is_canned, int lvl);

void cast_dispel_evil(int i_th, const String* dr, critter& agg);
void cast_dispel_good(int i_th, const String* dr, critter& agg);
void do_cast_dispel_good(critter& vict, critter& pc, int is_canned, int lvl);


///********** effects of passive spells (when triggered **********///



///**************   auxillary spell/skill functions   *************///

 
#endif 
