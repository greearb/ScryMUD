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

void show_eq(critter& pc);
void drop(int i_th, const String* obj, const String* obj_all, critter& pc);
void donate(int i_th, const String* obj, const String* obj_all, critter& pc);

void wear(int i_th, const String* obj, int j, const String* posn, critter& 
          pc); 
void inventory(critter& pc);
void look(int i_th, const String* object, critter& looker);
void do_look(int i_th, const String* obj, critter& pc, room& rm);

void cast(const String* spell_name, int j, const String* victim, critter& 
          caster); 
void put(int i, const String* object, int j, const String* bag, critter& 
          putter); 
void get(int i, const String* object, int j, const String* bag, critter& 
          getter);

void hit(int i_th, const String* victim, critter& hitter); 
void assist(int i_th, const String* friendly, critter& hitter); 
void do_hit(critter& vict, critter& pc);
 
void say(const char* message, critter& pc, room& rm);
void yell(const char* message, critter& pc);
void shout(const char* message, critter& pc);
void gossip(const char* message, critter& pc);
void group_say(const char* message, critter& pc);
void wizchat(const char* message, critter& pc);
void auction(const char* message, critter& pc);
void west(critter& pc, room& rm);
void east(critter& pc, room& rm);
void south(critter& pc, room& rm);
void north(critter& pc, room& rm);
void northwest(critter& pc, room& rm);
void southwest(critter& pc, room& rm);
void southeast(critter& pc, room& rm);
void northeast(critter& pc, room& rm);
void up(critter& pc, room& rm);
void down(critter& pc, room& rm);
void remove(int i_th, const String* obj, critter& pc); 
void rest(critter& pc);
void sleep(critter& pc);
void meditate(critter& pc);
void stand(critter& pc);
void wake(critter& pc);
void sit(critter& pc);
void examine(int i_th, const String* obj, critter& pc);

//don't show msg to this last one if specified, wraps do_emote
void emote(const char* msg, critter& pc, room& rm, short show_non_detects,
	   critter* noshow = NULL);
// Possessive emote, wraps do_emote
void pemote(const char* msg, critter& pc, room& rm, short show_non_detects,
            critter* noshow = NULL);
// Worker method.
void do_emote(const char* message, critter& pc, room& rm, short show_non_detects,
              int possessive, critter* crit = NULL);


///************************  login functions  ************************///

void start_login(critter& pc);
void do_login(critter& pc);
void quit_do_login_new(critter& pc);
void quit_do_login_old(critter& pc);



///*********************  auxillary functions  ***********************///

int source_give_to(critter& pc, object& obj, critter& targ); //does msgs
short detect(int see_bit, int vis_bit); //does bit comparison
int move(critter& pc, int i_th, const char* direction, short do_followers,
          room& cur_room, int& is_dead);
int don_obj(object& obj, critter& pc);
void out_eq(critter& targ, critter& looker);

int obj_wear_by(object& obj, critter& pc, int posn, short do_msg);
int obj_get_by(object& obj, critter& pc, short do_msg);
int obj_drop_by(object& obj, critter& pc);
int obj_remove_by(object& obj, critter& pc);
int eq_put_by(object& obj, object& bag, critter& pc, short obj_in_bag); 

void wear_eq_effects(object& obj, critter& pc, int posn, short do_msg);

void remove_eq_effects(object& obj, critter& pc, short from_corpse, 
                       short do_msg, int posn);

void gain_eq_effects(object& obj, object& bag, critter& pc, short 
                     obj_in_bag, short do_msg); 
void drop_eq_effects(object& obj, critter& pc, short do_msg);
void donate_eq_effects(object& obj, critter& pc, short do_msg);
void consume_eq_effects(object& obj, critter& pc, short do_msg);


#endif //include
