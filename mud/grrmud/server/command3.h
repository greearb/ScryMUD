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

/************************  command3.h  ***************************/
/*  Contains all the prototypes for all commands found in command3.cc */


#ifndef Commands3Include
#define Commands3Include

#include "const.h"
#include "classes.h"

void oclone(int i_th, const String* name, critter& pc);
void mclone(int i_th, const String* name, critter& pc);
void oload(int i_th, const String* name, critter& pc);
void mload(int i_th, const String* name, critter& pc);
void rclone(int i_th, const String* direction, int distance, critter& pc);
void rinit(int src_room, int dest_rm, critter& pc);

void wield(int i_th, const String* name, critter& pc);
void hold(int i_th, const String* name, critter& pc);
void flee(critter& pc, int& is_dead);
void flee_to_safety(critter& pc, int& is_dead);  // MOB PROC
void slay(int i_th, const String* name, critter& pc);

// Dealing with active (zunlocked) zones.
void addZone(int i, critter& pc);
void showZones(critter& pc);
void remZone(int i, critter& pc);


void junk(int i_th, const String* str1, const String* str2, critter& pc);
void silent_junk(int i_th, const String* str1, const String* str2,
                 critter& pc);
void do_junk(int do_msg, int i_th, const String* str1,
             const String* str2, critter& pc);

void enslave(int i_th, const String* name, critter& pc);
void time(critter& pc);
void order(String* str, critter& pc);
void force(String* str, critter& pc);
void force_all(String* str, critter& pc);
void date(critter& pc);
void group(int i_th, const String* name, critter& pc);

void tail(int i_th, const String* name, critter& pc, int do_smob = FALSE);
void follow(int i_th, const String* name, critter& pc, int do_msg = TRUE);
//void critter::doFollow(critter& vict, int do_msg = TRUE);

void do_tell(critter& pc, const char* msg, critter& targ, short show_teller,
	     int room_num_of_targ); //last arg is for mob_to_smob in spec procs
void tell(int i_th, const String* targ, const char* msg, critter& pc);

void who(critter& pc);
void ldwho(critter& pc);
void sockets(critter& pc);  //verbose who/ldwho

void idea(const String& str, critter& pc);
void bug(const String& str, critter& pc);
void use(int i_th, const String* target, critter& pc); // must be holding
void recite(int i_th, const String* target, int j_th, const String* vict, 
            critter& pc); //can be holding, but no have to
void quaf(int i_th, const String* potion, critter& pc); //only target user

#endif //include
