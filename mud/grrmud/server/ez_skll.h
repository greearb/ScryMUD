// $Id$
// $Revision: 1.5 $  $Author$ $Date$

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

///***********************  ez_skll.h  *****************************///

#ifndef SkillsInclude_ez
#define SkillsInclude_ez

#include "const.h"
#include <string2.h>
#include "classes.h"
#include "spells.h"

int picklock(int i_th, const String* vict, critter& pc);
int do_picklock(door& vict, critter& pc);
int do_picklock(object& vict, critter& pc);

int guard(int i_th, const String* vict, critter& pc);
int do_guard(critter& vict, critter& pc);
int unguard(critter& pc);
int shield(int i_th, const String* vict, critter& pc, int was_ordered);
int do_shield(critter& vict, critter& pc);
int rescue(int i_th, const String* vict, critter& pc);
int do_rescue(critter& vict, critter& pc);

int earthmeld(critter& pc);
int do_earthmeld(critter& pc);

///*****************  auxillary functions  ***************************///

 
#endif 


