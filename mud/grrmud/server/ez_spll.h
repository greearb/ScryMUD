// $Id: ez_spll.h,v 1.3 1999/06/20 02:01:44 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date: 1999/06/20 02:01:44 $

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

#ifndef SpellsInclude_ez_spll
#define SpellsInclude_ez_spll

#include "const.h"
#include <string2.h>
#include "classes.h"

/* define affects constants here */

#define WEB_DEX_AUGMENTATION		-2
#define WEB_MV_REGEN_AUGMENTATION	-50
#define ENTANGLE_DEX_AUGMENTATION	-5
#define ENTANGLE_MV_REGEN_AUGMENTATION	-85
#define FAERIE_FIRE_AC_AUGMENTATION      40
#define RUNE_EDGE_EFFECTS                1 /* to the dam dice sidesd! */
#define BLESS_EFFECTS                    1 /* to hit */


void cast_faerie_fire(int i_th, const String* dr, critter& agg);
void do_cast_faerie_fire(critter& targ, critter& pc, int is_canned, int lvl);

void cast_rune_edge(int i_th, const String* dr, critter& agg);
void do_cast_rune_edge(object& targ, critter& pc, int is_canned, int lvl);

void cast_fire_blade(int i_th, const String* dr, critter& agg);
void do_cast_fire_blade(object& targ, critter& pc, int is_canned, int lvl);

void cast_frost_blade(int i_th, const String* dr, critter& agg);
void do_cast_frost_blade(object& targ, critter& pc, int is_canned, int lvl);

void cast_create_water(int i_th, const String* dr, critter& agg);
void do_cast_create_water(object& targ, critter& pc, int is_canned, int lvl);

void cast_enchant_armor(int i_th, const String* dr, critter& agg);
void do_cast_enchant_armor(object& targ, critter& pc, int is_canned, int lvl);

void cast_detect_magic(int i_th, const String* dr, critter& agg);
void do_cast_detect_magic(critter& targ, critter& pc, int is_canned,
 			  int lvl);

void cast_infravision(int i_th, const String* targ, critter& pc);
void do_cast_infravision(critter& targ, critter& pc, int is_canned,
                         int lvl);

void cast_detect_hidden(int i_th, const String* dr, critter& agg);
void do_cast_detect_hidden(critter& targ, critter& pc, int is_canned,
 			  int lvl);
void cast_detect_invisibility(int i_th, const String* dr, critter& agg);
void do_cast_detect_invisibility(critter& targ, critter& pc, int is_canned,
 			  int lvl);
void cast_detect_alignment(int i_th, const String* dr, critter& agg);
void do_cast_detect_alignment(critter& targ, critter& pc, int is_canned,
 			  int lvl);
void cast_detect_poison(int i_th, const String* dr, critter& agg);
void do_cast_detect_poison(object& targ, critter& pc, int is_canned,
 			  int lvl);
void cast_pfg(critter& agg);
void do_cast_pfg(critter& targ, critter& pc, int is_canned, int lvl);

void cast_pfe(critter& agg);
void do_cast_pfe(critter& targ, critter& pc, int is_canned, int lvl);

void cast_mirror_image(critter& agg);
void do_cast_mirror_image(critter& pc, int is_canned, int lvl);

void cast_invisibility(int i_th, const String* dr, critter& agg);
void do_cast_invisibility(critter& targ, critter& pc, int is_canned, int lvl);
void do_cast_invisibility(object& targ, critter& pc, int is_canned, int lvl);

void cast_web(int i_th, const String* dr, critter& agg);
void do_cast_web(critter& targ, critter& pc, int is_canned, int lvl);

void cast_enchant_weapon(int i_th, const String* dr, critter& agg);
void do_cast_enchant_weapon(object& targ, critter& pc, int is_canned, int lvl);

void cast_restore(int i_th, const String* vict, critter& agg);
void do_cast_restore(critter& targ, critter& pc, int is_canned, int lvl);

void do_cast_heal(critter& targ, critter& pc, int is_canned, int lvl);
void cast_heal(int i_th, const String* vict, critter& agg);

void do_cast_cure_serious(critter& targ, critter& pc, int is_canned, int lvl);
void cast_cure_serious(int i_th, const String* vict, critter& agg);

void do_cast_cure_critical(critter& targ, critter& pc, int is_canned, int lvl);
void cast_cure_critical(int i_th, const String* vict, critter& agg);

void do_cast_bless(critter& targ, critter& pc, int is_canned, int lvl);
void cast_bless(int i_th, const String* vict, critter& agg);

void do_cast_entangle(critter& targ, critter& pc, int is_canned, int lvl);
void cast_entangle(int i_th, const String* vict, critter& agg);

void do_cast_calm(critter& vict, int is_canned, int lvl);
void cast_calm(critter& agg);

///********** effects of passive spells (when triggered **********///



///**************   auxillary spell/skill functions   *************///

#endif 


