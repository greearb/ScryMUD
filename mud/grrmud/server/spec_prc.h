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

void do_was_calmed_procs(critter& calmed, critter& calmer);
void do_battle_proc(critter& pc);
void do_just_killed_procs(critter& pc);

void do_this_obj_proc(int type_of_proc, int proc_num, critter& owner, 
                      object& obj, int posn);
void do_pulsed_spec_procs(int first_room, int last_room);

void do_sell_proc(int prcnum, critter& shop_keeper, int i_th, const String* 
		  item, critter& buyer);
void do_vend_buy(object& vender, int i_th, const String* item, critter& pc);
void do_buy_proc(int prcnum, critter& shop_keeper, int i_th, const String* 
		  item, critter& buyer);
void do_offer_proc(int prcnum, critter& shop_keeper, int i_th, const String* 
		  item, critter& buyer);

short do_wand_scroll_proc(critter* vict, int proc_num, critter& pc,
                          int spell_lvl);
short do_wand_scroll_proc(door* vict, int proc_num, critter& pc,
                          int spell_lvl);
short do_wand_scroll_proc(int proc_num, critter& pc, int spell_lvl);
short do_wand_scroll_proc(object* vict, int proc_num, critter& pc,
                          int spell_lvl);

void do_shot_proc(critter& targ, critter& pc, short did_hit,
                  int& is_targ_dead); //assumes SMOB

void do_domob_give_proc(critter& targ, critter& pc, object& obj);
void do_domob_say_proc(critter& targ, critter& pc, const String& msg);
void do_domob_bow_proc(critter& targ, critter& pc);
void do_domob_curse_proc(critter& targ, critter& pc);

short pass_domob_checks(critter& mob, critter& pc);

#endif







