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
//To contact the maintainer, Edward Roper: edro+scrymud [at] wanfear.net
//

///************************  wep_skll.h  *****************************///

#ifndef SkillsInclude_wep
#define SkillsInclude_wep

#include "const.h"
#include <string2.h>
#include "classes.h"
#include "spells.h"
#include "skills.h"

int backstab(int i_th, const String* vict, critter& pc);
int do_backstab(critter& vict, critter& pc);
int circle(int i_th, const String* vict, critter& pc);
int do_circle(critter& vict, critter& pc);
int disarm(int i_th, const String* vict, critter& pc);
int do_disarm(critter& vict, critter& pc);


///*****************  auxillary functions  ***************************///

 
#endif 
