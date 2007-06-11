// $Id$

//
//ScryMUD Server Code
//Copyright (C) 2007  Edward Roper
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
//To contact the maintainer, Edward Roper: edro+scrymud [at] wanfear.net
//

#ifndef GRRMUD_NECROMANCER
#include "critter.h"

#define RUST_EFFECT               25 /* a/c */
#define DISFAVOR_EFFECT           20 /* all resistances */
#define REMOVE_SOUL_EFFECT        100 /* spell resistance */
#define SANCTUM_EFFECT            30 /* dam receive mod */

void cast_rust(int i_th, const String* vict, critter& agg);
void do_cast_rust(critter& vict, critter& agg, int is_canned, int lvl);

void cast_disfavor(int i_th, const String* vict, critter& agg);
void do_cast_disfavor(critter& vict, critter& agg, int is_canned, int lvl);

void cast_remove_soul(int i_th, const String* vict, critter& agg);
void do_cast_remove_soul(critter& vict, critter& agg, int is_canned, int lvl);

void cast_remove_hope(int i_th, const String* vict, critter& agg);
void do_cast_remove_hope(critter& vict, critter& agg, int is_canned, int lvl);

void cast_remove_karma(int i_th, const String* vict, critter& agg);
void do_cast_remove_karma(critter& vict, critter& agg, int is_canned, int lvl);

void cast_sanctum_of_the_victim(int i_th, const String* vict, critter& agg);
void do_cast_sanctum_of_the_victim(critter& vict, critter& agg, int is_canned, int lvl);

void cast_fear(int i_th, const String* vict, critter& agg);
void do_cast_fear(critter& vict, critter& agg, int is_canned, int lvl);

void cast_weaken(int i_th, const String* vict, critter& agg);
void do_cast_weaken(critter& vict, critter& agg, int is_canned, int lvl);

void cast_raise_undead(critter& agg);
void do_cast_raise_undead(critter& pc, int is_canned, int lvl);

void cast_blood_ritual(critter& agg);
/*TODO: these need to be implemented
void cast_spirit_ritual(critter& agg);
void cast_stamina_ritual(critter& agg);
void cast_ritual_of_power(critter& agg);
*/

#define GRRMUD_NECROMANCER
#endif
