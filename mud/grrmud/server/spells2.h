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

///************************  Spells2.h  *****************************///

/* These spells are of a kindly nature, no battle will be started. */

#ifndef Spells2Include
#define Spells2Include

#include "const.h"
#include <string2.h>
#include "classes.h"

#define BIND_WOUND_EFFECT         5  /* +5 to hp regen */
#define CURSE_STR_EFFECT          -2
#define CURSE_MA_REGEN_EFFECT     -10
#define DIVINE_PROTECTION_EFFECT  -35 /* to AC */
#define STONE_SKIN_EFFECT_DRM     -25 /* to dam_rec_mod */
#define STONE_SKIN_EFFECT_DEX     -1
#define SANCTUARY_EFFECT_DRM      -35 /* to dam_rec_mod */
#define PRISMATIC_GLOBE_EFFECT_AC -100
#define PRISMATIC_GLOBE_EFFECT_MOV -50
#define MAGIC_SHIELD_AC_EFFECT     -10
#define MAGIC_SHIELD_SAC_EFFECT    -50 /* spell resistance */
#define RUST_EFFECT               25 /* a/c */
#define DISFAVOR_EFFECT           20 /* all resistances */
#define REMOVE_SOUL_EFFECT        100 /* spell resistance */
#define SANCTUM_EFFECT            30 /* dam receive mod */


void cast_blindness(int i_th, const String* dr, critter& agg);
void do_cast_blindness(critter& targ, critter& pc, int is_canned, int lvl);

void cast_cure_blind(int i_th, const String* dr, critter& agg);
void do_cast_cure_blind(critter& targ, critter& pc, int is_canned, int lvl);

void cast_remove_poison(int i_th, const String* dr, critter& agg);
void do_cast_remove_poison(critter& targ, critter& pc, int is_canned, int lvl);

void do_locate_object(object &obj, const String* targ, critter& pc,
                      int rm_num, int sanity, int spell_level);
void do_locate_object(critter& crit, const String* targ, critter& pc,
                      int rm_num, int sanity, int spell_level);

void cast_wizard_eye(int i_th, const String* vict, critter& pc);
void do_cast_wizard_eye(object& obj, critter& pc, int is_canned, int lvl);

void cast_locate(const String* vict, critter& agg);
void do_cast_locate(const String* targ, critter& pc, int is_canned, int lvl);

void cast_armor(int i_th, const String* vict, critter& agg);
void do_cast_armor(critter& vict, critter& agg, int is_canned, int lvl);

void cast_magic_shield(int i_th, const String* vict, critter& agg);
void do_cast_magic_shield(critter& vict, critter& agg, int is_canned, int lvl);

void cast_absorb_blows(critter& agg);
void do_cast_absorb_blows(critter& vict, critter& agg, int is_canned, int lvl);

void cast_prismatic_globe(int i_th, const String* vict, critter& agg);
void do_cast_prismatic_globe(critter& vict, critter& agg, int is_canned,
                             int lvl);

void cast_sanctuary(int i_th, const String* vict, critter& agg);
void do_cast_sanctuary(critter& vict, critter& agg, int is_canned, int lvl);

void cast_divine_protection(int i_th, const String* vict, critter& agg);
void do_cast_divine_protection(critter& vict, critter& agg, int is_canned,
                               int lvl);

void cast_identify(int i_th, const String* vict, critter& agg);
void do_cast_identify(object& vict, critter& agg, int is_canned, int lvl);

void cast_shadows_blessing(int i_th, const String* vict, critter& agg);
void do_cast_shadows_blessing(critter& vict, critter& agg, int is_canned, 
                              int lvl);
void cast_bind_wound(int i_th, const String* vict, critter& agg);
void do_cast_bind_wound(critter& vict, critter& agg, int is_canned, 
                              int lvl);

void cast_weaken(int i_th, const String* vict, critter& agg);
void do_cast_weaken(critter& vict, critter& agg, int is_canned, int lvl);

void cast_remove_curse(int i_th, const String* vict, critter& agg);
void do_cast_remove_curse(critter& vict, critter& agg, int is_canned, int lvl);
void do_cast_remove_curse(critter& vict, object& agg, int is_canned, int lvl);

void cast_curse(int i_th, const String* vict, critter& agg);
void do_cast_curse(object& vict, critter& agg, int is_canned, int lvl);
void do_cast_curse(critter& vict, critter& agg, int is_canned, int lvl);

void cast_strength(int i_th, const String* vict, critter& agg);
void do_cast_strength(critter& vict, critter& agg, int is_canned, int lvl);

void do_cast_flesh_to_stone(critter& vict, critter& agg, int is_canned, int lvl);
void cast_flesh_to_stone(int i_th, const String* victim, critter& pc);

void cast_sleep(int i_th, const String* vict, critter& agg);
void do_cast_sleep(critter& vict, critter& agg, int is_canned, int lvl);

void cast_fly(int i_th, const String* vict, critter& agg);
void do_cast_fly(critter& vict, critter& agg, int is_canned, int lvl);

void cast_recharge(int i_th, const String* vict, critter& agg);
void do_cast_recharge(critter& vict, critter& agg, int is_canned, int lvl);

void cast_fireproof(int i_th, const String* vict, critter& agg);
void do_cast_fireproof(critter& vict, critter& agg, int is_canned, int lvl);

void cast_haste(int i_th, const String* vict, critter& agg);
void do_cast_haste(critter& vict, critter& agg, int is_canned, int lvl);

void cast_dispel_magic(int i_th, const String* vict, critter& agg);
void do_cast_dispel_magic(critter& vict, critter& agg, int is_canned, int lvl);
void do_cast_dispel_magic(object& vict, critter& agg, int is_canned, int lvl);

void cast_stone_skin(int i_th, const String* vict, critter& agg);
void do_cast_stone_skin(critter& vict, critter& agg, int is_canned, int lvl);

void cast_sober(int i_th, const String* vict, critter& agg);
void do_cast_sober(critter& vict, critter& agg, int is_canned, int lvl);

// new for Necromancer class
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

///**************   auxillary functions   *************///

#endif 
