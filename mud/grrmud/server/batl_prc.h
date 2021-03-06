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

/************************  batl_prc.h  ***************************/


#ifndef Batl_prcInclude
#define Batl_prcInclude

#include "const.h"
#include "classes.h"
#include "room.h"
#include "critter.h"
#include "object.h"
#include "door.h"

#define PROJECTILE_DEATH                0        //mode for alert_room_proc

short is_tank(critter& pc);
critter* find_weakest(List<critter*>& lst);

void do_battle_proc(critter& pc);

void do_entered_room_procs(critter& pc, door* by_dr, const char* from_dir,
                           room& to_rm, int& is_dead, int sanity);
void do_entered_room_procs(object& obj, door* by_dr, const char* from_dir,
                           room& to_rm, int sanity);

void path_from_a_to_b(int a, int b, List<int>& path); //a,b are room numbers
void track_to_kill(critter& vict, critter& agg);
void a_summons_help_against_b(critter& agg, critter& vict);
void alert_room_proc(int rm_num, int alert_type, critter& targ,
                     critter& agg);
void do_hunt_proc(critter& pc, int num_steps);

///**********************************************************************///
///************************  auxillary functions  ***********************///
///**********************************************************************///

#endif //include




