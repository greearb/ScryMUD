// $Id: olc.h,v 1.2 1999/06/05 23:29:14 greear Exp $
// $Revision: 1.2 $  $Author: greear $ $Date$

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

///************************  olc.h  *****************************///

#ifndef OlcInclude
#define OlcInclude


#define OLC_ROOM pc.pc->imm_data->olc_room
#define OLC_MOB pc.pc->imm_data->olc_mob
#define OLC_OBJ pc.pc->imm_data->olc_obj
#define OLC_DOOR pc.pc->imm_data->olc_door


#include "const.h"
#include <string2.h>
#include "classes.h"

void start_olc(critter& pc);
void restart_olc(critter& pc);
void quit_olc(critter& pc);
void do_olc(critter& pc);
void finish_olc_obj(critter& pc); //cleans up mess basically
void finish_olc_mob(critter& pc); //cleans up mess basically
void finish_olc_room(critter& pc); //cleans up mess basically
void finish_olc_door(critter& pc); //cleans up mess basically
#endif 
