// $Id: spec_prc.h,v 1.5 1999/09/06 07:12:52 greear Exp $
// $Revision: 1.5 $  $Author: greear $ $Date: 1999/09/06 07:12:52 $

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

#ifndef SPEC_PROC_INCLUDE
#define SPEC_PROC_INCLUDE

#include "const.h"
#include "classes.h"
#include <string2.h>

int do_was_calmed_procs(critter& calmed, critter& calmer);
int do_just_killed_procs(critter& pc);

int do_this_obj_proc(int type_of_proc, int proc_num, critter& owner, 
                      object& obj, int posn);
int do_pulsed_spec_procs(int first_room, int last_room);

int do_sell_proc(int prcnum, critter& shop_keeper, int i_th, const String* 
		  item, critter& buyer);
int do_vend_buy(object& vender, int i_th, const String* item, critter& pc);
int do_buy_proc(int prcnum, critter& shop_keeper, int i_th, const String* 
		  item, critter& buyer);
int do_offer_proc(int prcnum, critter& shop_keeper, int i_th, const String* 
		  item, critter& buyer);

int do_wand_scroll_proc(critter* vict, int proc_num, critter& pc,
                          int spell_lvl);
int do_wand_scroll_proc(door* vict, int proc_num, critter& pc,
                          int spell_lvl);
int do_wand_scroll_proc(int proc_num, critter& pc, int spell_lvl);
int do_wand_scroll_proc(object* vict, int proc_num, critter& pc,
                          int spell_lvl);

int do_shot_proc(critter& targ, critter& pc, short did_hit,
                  int& is_targ_dead); //assumes SMOB

#endif
