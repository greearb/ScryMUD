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

///************************  Spells.h  *****************************///

#ifndef SpellsInclude_rm_spll
#define SpellsInclude_rm_spll

#include "const.h"
#include <string2.h>
#include "classes.h"
#include "command4.h"

/* defines for effects go here */


void cast_firewall(critter& pc);
void do_cast_firewall(room& rm, critter& agg, int is_canned, int lvl);
void cast_gate(int i_th, const String* targ, critter& pc);
void do_cast_gate(room& rm, critter& agg, int is_canned, int lvl);
void cast_portal(int i_th, const String* targ, critter& pc);
void do_cast_portal(room& rm, critter& agg, int is_canned, int lvl);
void cast_distortion_wall(int i_th, const String* dr, critter& pc);
void do_cast_distortion_wall(door& dr, critter& agg, int is_canned, int lvl);

void cast_necrophilia(critter &agg);
void do_cast_necrophilia(critter &agg, int is_canned, int lvl);

///********** effects of passive spells (when triggered **********///



///**************   auxillary spell/skill functions   *************///

 
#endif 
