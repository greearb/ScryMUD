// $Id: command3.h,v 1.10 2001/03/29 03:02:29 eroper Exp $
// $Revision: 1.10 $  $Author: eroper $ $Date$

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

int do_add_comment(BugTypeE bt, critter& pc);
int do_add_idea_comment(critter& pc);
int do_add_bug_comment(critter& pc);

int oclone(int i_th, const String* name, critter& pc);
int mclone(int i_th, const String* name, critter& pc);
int oload(int i_th, const String* name, critter& pc);
int mload(int i_th, const String* name, critter& pc);
int rclone(int i_th, const String* direction, int distance, critter& pc);
int rinit(int src_room, int dest_rm, critter& pc);

int wield(int i_th, const String* name, critter& pc);
int hold(int i_th, const String* name, critter& pc);
int light(int i_th, const String* name, critter& pc);
int flee(critter& pc, int& is_dead);
int flee_to_safety(critter& pc, int& is_dead);  // MOB PROC
int slay(int i_th, const String* name, critter& pc);

int sacrifice(const String* cmd, critter& pc);
int remort(int i_th, const String* v, const String* new_race,
           const String* new_class, critter& pc);

// Dealing with active (zunlocked) zones.
int addZone(int i, critter& pc);
int showZones(critter& pc);
int remZone(int i, critter& pc);


int junk(int i_th, const String* str1, const String* str2, critter& pc);
int silent_junk(int i_th, const String* str1, const String* str2,
                 critter& pc);
int do_junk(int do_msg, int i_th, const String* str1,
             const String* str2, critter& pc);

int enslave(int i_th, const String* name, critter& pc);
int order(String* str, critter& pc);
int force(String* str, critter& pc);
int force_all(String* str, critter& pc);
int date(critter& pc);
int group(int i_th, const String* name, critter& pc);

int tail(int i_th, const String* name, critter& pc, int do_smob = FALSE);
int follow(int i_th, const String* name, critter& pc, int do_msg = TRUE);
//int critter::doFollow(critter& vict, int do_msg = TRUE);

int do_tell(critter& pc, const char* msg, critter& targ, short show_teller,
             int room_num_of_targ); //last arg is for mob_to_smob in spec procs
int tell(int i_th, const String* targ, const char* msg, critter& pc);

int who(critter& pc);
int ldwho(critter& pc);
int sockets(critter& pc);  //verbose who/ldwho

int buglist(BugTypeE bt, int i, String& cmd, int j, const String& mod,
            const String& notes, critter& pc);

int idea(const String& str, critter& pc);
int bug(String& str, critter& pc);
int use(int i_th, String* wand_name, int j_th, String* target, critter& pc);
int recite(int i_th, const String* target, int j_th, const String* vict, 
            critter& pc); //can be holding, but no have to
int quaff(int i_th, const String* potion, critter& pc); //only target user

#endif //include
