// $Id$
// $Revision: 1.9 $  $Author$ $Date$

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

/************************  command2.h  ***************************/
/*  Contains all the prototypes for all commands found in command2.cc */


#ifndef Commands2Include
#define Commands2Include

#include "const.h"
#include "classes.h"


int log_level(int lvl, critter& pc);
int wimpy(int i, critter& pc);
int language(const String& lang,  critter& pc);
int abilities(critter& pc); //show's skills can lrn, %lrned
int score(const String* str2, critter& pc);
int score_long(critter& pc);
int do_score_long(critter& targ, critter& pc);
//int doUngroup(int i_th, const String* vict);
int exit(critter& pc); //disp exits
int auto_exit(critter& pc); //disp exits, concisely
int  open(int i_th, const String* name, critter& pc);
int  unlock(int i_th, const String* name, critter& pc);
int  lock(int i_th, const String* name, critter& pc);
int close(int i_th, const String* name, critter& pc);
int save(critter& pc);
int nogossip(critter& pc);
int eat(int i_th, const String* name, critter& pc);
int drink(int i_th, const String* name, critter& pc);
int fill(int i_th, const String* targ, int j_th, const String* source, 
          critter& pc);
int empty(int i_th, const String* name, critter& pc);
int help(int i_th, String* command, String* mort, critter& pc);
int buy(int i_th, const String* item, int j_th, const String* keeper,
         critter& pc);
int offer(int i_th, const String* item, int j_th, const String* keeper,
         critter& pc);
int sell(int i_th, const String* item, int j_th, const String* keeper,
         critter& pc);
int practice(const String* spell, int j_th, const String* teacher, critter& 
              pc); 
int toggle_prompt(const String* field, critter& pc);
int list_merchandise(int i_th, const String* keeper, critter& pc);
int do_list_merchandise(List<object*>& inv, List<object*>& perm_inv,
                        critter* crit_owner, object* obj_owner,
                        critter& pc);

int mstat(int i_th, const String* name, critter& pc); //stat critter
int do_mstat(critter& targ, critter& pc);

//stat object
int lore(int i_th, const String* name, critter& pc, int show_extra = FALSE);
int do_lore(object& targ, critter& pc, int show_extra);

int ostat(int i_th, const String* name, critter& pc); //stat object
int do_ostat(object& obj, critter& pc);
int rstat(int i_th, critter& pc); //stat room
int do_rstat(room& rm, critter& pc); //stat room
int do_dstat(door_data& dr, critter& pc);
int dstat(int i_th, const String* name, critter& pc); //stat door

int shutdown(const String* cond, critter& pc); //do clean shutdown of game
int rezone(critter& pc);
int total_rezone(critter& pc);

///******************** auxillary commands ***************************///

String get_page(const char* path_name);


#endif //include
