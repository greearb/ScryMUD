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

/************************  commands.h  ***************************/
/*  Contains all the prototypes for all commands found in command*.cc */


#ifndef __CommandsInclude__
#define __CommandsInclude__

#include "const.h"
#include "classes.h"
#include "critter.h"
#include "object.h"
#include "door.h"
#include "room.h"

int show_eq(critter& pc);
int drop(int i_th, const String* obj, const String* obj_all, critter& pc);
int donate(int i_th, const String* obj, const String* obj_all, critter& pc);

int wear(int i_th, const String* obj, int j, const String* posn, critter& 
          pc); 
int inventory(critter& pc);
int look(int i_th, const String* object, critter& looker,
         int ignore_brief = FALSE);
int do_look(int i_th, const String* obj, critter& pc, room& rm,
            int ignore_brief);

int cast(const String* spell_name, int j, const String* victim,
         critter& caster, int was_ordered);
int put(int i, const String* object, int j, const String* bag, critter& 
          putter); 
int get(int i, const String* object, int j, const String* bag, critter& 
          getter);

int hit(int i_th, const String* victim, critter& hitter); 
int assist(int i_th, const String* friendly, critter& hitter); 
int do_hit(critter& vict, critter& pc);
 
int say(const char* message, critter& pc, room& rm);
int yell(const char* message, critter& pc);
int shout(const char* message, critter& pc);
int gossip(const char* message, critter& pc);
int group_say(const char* message, critter& pc);
int wizchat(const char* message, critter& pc);
int auction(const char* message, critter& pc);
int west(critter& pc, room& rm, int& is_dead);
int east(critter& pc, room& rm, int& is_dead);
int south(critter& pc, room& rm, int& is_dead);
int north(critter& pc, room& rm, int& is_dead);
int northwest(critter& pc, room& rm, int& is_dead);
int southwest(critter& pc, room& rm, int& is_dead);
int southeast(critter& pc, room& rm, int& is_dead);
int northeast(critter& pc, room& rm, int& is_dead);
int up(critter& pc, room& rm, int& is_dead);
int down(critter& pc, room& rm, int& is_dead);
int remove(int i_th, const String* obj, critter& pc); 
int rest(critter& pc);
int sleep(critter& pc);
int meditate(critter& pc);
int stand(critter& pc);
int wake(critter& pc);
int sit(critter& pc);
int examine(int i_th, const String* obj, critter& pc);

//don't show msg to this last one if specified, wraps do_emote
int emote(const char* msg, critter& pc, room& rm, short show_non_detects,
          critter* noshow = NULL);
// Possessive emote, wraps do_emote
int pemote(const char* msg, critter& pc, room& rm, short show_non_detects,
           critter* noshow = NULL);
// Worker method.
int do_emote(const char* msg, critter& pc, room& rm, short show_non_detects,
             int possessive, critter* crit = NULL);


///************************  login functions  ************************///

int start_login(critter& pc);
int do_login(critter& pc);
int quit_do_login_new(critter& pc);
int quit_do_login_old(critter& pc);



///*********************  auxillary functions  ***********************///

int source_give_to(critter& pc, object& obj, critter& targ); //does msgs
int move(critter& pc, int i_th, const char* direction, short do_followers,
          room& cur_room, int& is_dead);
int don_obj(object& obj, critter& pc);
int out_eq(critter& targ, critter& looker);

int obj_wear_by(object& obj, critter& pc, int posn, short do_msg);
int obj_get_by(object& obj, critter& pc, short do_msg);
int obj_drop_by(object& obj, critter& pc);
int obj_remove_by(object& obj, critter& pc);
int eq_put_by(object& obj, object& bag, critter& pc, short obj_in_bag); 

int wear_eq_effects(object& obj, critter& pc, int posn, short do_msg);

int remove_eq_effects(object& obj, critter& pc, short from_corpse, 
                       short do_msg, int posn);

int gain_eq_effects(object& obj, object& bag, critter& pc, short 
                     obj_in_bag, short do_msg); 
int drop_eq_effects(object& obj, critter& pc, short do_msg, short is_junk = FALSE);
int donate_eq_effects(object& obj, critter& pc, short do_msg);
int consume_eq_effects(object& obj, critter& pc, short do_msg);


#endif //include
