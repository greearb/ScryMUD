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


void log_level(int lvl, critter& pc);
void wimpy(int i, critter& pc);
void abilities(critter& pc); //show's skills can lrn, %lrned
void score(const String* str2, critter& pc);
void score_long(critter& pc);
//void doUngroup(int i_th, const String* vict);
void exit(critter& pc); //disp exits
void auto_exit(critter& pc); //disp exits, concisely
int  open(int i_th, const String* name, critter& pc);
int  unlock(int i_th, const String* name, critter& pc);
int  lock(int i_th, const String* name, critter& pc);
void close(int i_th, const String* name, critter& pc);
void save(critter& pc);
void nogossip(critter& pc);
void eat(int i_th, const String* name, critter& pc);
void drink(int i_th, const String* name, critter& pc);
void fill(int i_th, const String* targ, int j_th, const String* source, 
          critter& pc);
void empty(int i_th, const String* name, critter& pc);
void help(int i_th, String* command, critter& pc);
void buy(int i_th, const String* item, int j_th, const String* keeper,
         critter& pc);
void offer(int i_th, const String* item, int j_th, const String* keeper,
         critter& pc);
void sell(int i_th, const String* item, int j_th, const String* keeper,
         critter& pc);
void practice(const String* spell, int j_th, const String* teacher, critter& 
              pc); 
void toggle_prompt(const String* field, critter& pc);
void list_merchandise(int i_th, const String* keeper, critter& pc);

void mstat(int i_th, const String* name, critter& pc); //stat critter
void do_mstat(critter& targ, critter& pc);
void lore(int i_th, const String* name, critter& pc); //stat object
void do_lore(object& targ, critter& pc);
void ostat(int i_th, const String* name, critter& pc); //stat object
void do_ostat(object& obj, critter& pc);
void rstat(int i_th, critter& pc); //stat room
void do_rstat(room& rm, critter& pc); //stat room
void do_dstat(door_data& dr, critter& pc);
void dstat(int i_th, const String* name, critter& pc); //stat door

void shutdown(const String* cond, critter& pc); //do clean shutdown of game
void rezone(critter& pc);
void total_rezone(critter& pc);

///******************** auxillary commands ***************************///

String get_page(const char* path_name);


#endif //include
