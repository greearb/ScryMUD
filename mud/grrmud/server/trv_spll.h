// $Id: trv_spll.h,v 1.2 1999/06/05 23:29:15 greear Exp $
// $Revision: 1.2 $  $Author: greear $ $Date: 1999/06/05 23:29:15 $

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

#ifndef SpellsInclude_trv_spll
#define SpellsInclude_trv_spll

#include "const.h"
#include <string2.h>
#include "classes.h"

/* defines for effects go here */


void cast_meteorstorm(critter& pc);
void do_cast_meteorstorm(critter& agg, int is_canned, int lvl);

void cast_icestorm(critter& pc);
void do_cast_icestorm(critter& agg, int is_canned, int lvl);

void cast_firestorm(critter& pc);
void do_cast_firestorm(critter& agg, int is_canned, int lvl);

void cast_flame_strike(critter& pc);
void do_cast_flame_strike(critter& agg, int is_canned, int lvl);

void cast_typhoon(critter& pc);
void do_cast_typhoon(critter& agg, int is_canned, int lvl);

void cast_lightning_storm(critter& pc);
void do_cast_lightning_storm(critter& agg, int is_canned, int lvl);

void cast_tornado(critter& pc);
void do_cast_tornado(critter& agg, int is_canned, int lvl);

void cast_quake(critter& pc);
void do_cast_quake(critter& agg, int is_canned, int lvl);

void cast_group_heal(critter& pc);
void do_cast_group_heal(critter& agg, int is_canned, int lvl);

///********** effects of passive spells (when triggered **********///



///**************   auxillary spell/skill functions   *************///

 
#endif 
