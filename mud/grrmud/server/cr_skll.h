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

///***********************  cr_skll.h  *****************************///

#ifndef SkillsInclude_cr
#define SkillsInclude_cr

#include "const.h"
#include <string2.h>
#include "classes.h"
#include "spells.h"

int skin(int i_th, const String* vict, critter& pc);
int do_skin(object& vict, critter& pc);
int butcher(int i_th, const String* vict, critter& pc);
int do_butcher(object& vict, critter& pc);
int behead(int i_th, const String* vict, critter& pc);
int do_behead(object& vict, critter& pc);


///*****************  auxillary functions  ***************************///

 
#endif 
