// $Id: rm_spll.cc,v 1.4 1999/07/12 01:16:51 greear Exp $
// $Revision: 1.4 $  $Author: greear $ $Date: 1999/07/12 01:16:51 $

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

#include "rm_spll.h"
#include "commands.h"
#include "command2.h"
#include <stdio.h>
#include "misc.h"
#include "misc2.h"
#include "battle.h"
#include "classes.h"
#include "spells.h"
#include "skills.h"
#include <PtrArray.h>



void cast_portal(int i_th, const String* dr, critter& pc) {
   int spell_num = PORTAL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
   critter* targ;
   
                       /* check out pc */
   if (get_percent_lrnd(spell_num, pc) == -1) {
      show(NO_KNOW_SPELL_MSG, pc);
      return;
   }//if
   if (pc.MANA < spell_mana) {
      show(NO_MANA_MSG, pc);
      return;
   }//if
   if (pc.POS != POS_STAND) {
      show(SPELL_MUST_BE_STANDING_MSG, pc);   
      return;
   }//if

   targ = have_crit_named(pc_list, i_th, dr, pc.SEE_BIT, ROOM);
   if (!targ) {
      show("That person isn't logged on.\n", pc);
      return;
   }//if

   if (pc.getCurRoom()->isNoMagExit()) {
      pc.show("You may not leave this room by magical means!\n");
      return;
   }//if
   
   if (targ->getCurRoom()->isNoMagEntry()) {
      pc.show("You may not enter that room by magical means!\n");
      return;
   }

   do_cast_portal(room_list[targ->getCurRoomNum()], pc, FALSE, 0);
}//cast_portal


void do_cast_portal(room& rm, critter& agg, int is_canned,
                        int lvl) {
   int spell_num = PORTAL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
   String buf(100);
   short do_effects = FALSE;
 
   if (room_list[agg.getCurRoomNum()].isNoMagic()) {
      show(NOT_IN_NOMAG_RM_MSG, agg);
      return;
   }//if

   if (!is_canned)
      lvl = agg.LEVEL;

   if (is_canned || !lost_concentration(agg, spell_num)) {
      if (!is_canned)
	agg.MANA -= spell_mana;
      do_effects = TRUE;
      show("A shimmering portal opens before you!\n", agg); 
      emote("opens a shimmering portal to...somewhere!", agg, 
	    room_list[agg.getCurRoomNum()], TRUE);
      show_all("A portal opens up before you!!\n", rm); 
   }//if canned
   else {//lost con
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else !canned

   agg.PAUSE += 1; 

   if (do_effects) {
     do_door_to(room_list[agg.getCurRoomNum()], rm, 0, agg, &NULL_STRING, 
		GATE_DOOR_NUM);
     do_door_to(rm, room_list[agg.getCurRoomNum()], 0, agg, &NULL_STRING, 
		GATE_DOOR_NUM);
   }//if
}//do_cast_portal



void cast_gate(int i_th, const String* dr, critter& pc) {
   int spell_num = GATE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
   critter* targ;
   
                       /* check out pc */
   if (get_percent_lrnd(spell_num, pc) == -1) {
      show(NO_KNOW_SPELL_MSG, pc);
      return;
   }//if
   if (pc.MANA < spell_mana) {
      show(NO_MANA_MSG, pc);
      return;
   }//if
   if (pc.POS != POS_STAND) {
      show(SPELL_MUST_BE_STANDING_MSG, pc);   
      return;
   }//if

   targ = have_crit_named(pc_list, i_th, dr, pc.SEE_BIT, ROOM);
   if (!targ) {
      show("That person isn't logged on.\n", pc);
      return;
   }//if

   if (pc.getCurRoom()->isNoMagExit()) {
      pc.show("You may not leave this room by magical means!\n");
      return;
   }//if
   
   if (targ->getCurRoom()->isNoMagEntry()) {
      pc.show("You may not enter that room by magical means!\n");
      return;
   }

   do_cast_gate(room_list[targ->getCurRoomNum()], pc, FALSE, 0);
}//cast_gate


void do_cast_gate(room& rm, critter& agg, int is_canned, int lvl) {
   int spell_num = GATE_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
   String buf(100);
   short do_effects = FALSE;
 
   if (room_list[agg.getCurRoomNum()].isNoMagic()) {
      show(NOT_IN_NOMAG_RM_MSG, agg);
      return;
   }//if

   if (!is_canned)
      lvl = agg.LEVEL;

   agg.PAUSE += 1; 

   if (is_canned || !lost_concentration(agg, spell_num)) {
      if (!is_canned)
	agg.MANA -= spell_mana;
      do_effects = TRUE;
      show("A shimmering gate opens before you!\n", agg); 
      emote("opens a shimmering gate to...somewhere!", agg, 
	    room_list[agg.getCurRoomNum()], TRUE); 
   }//if canned
   else {//lost con
     show(LOST_CONCENTRATION_MSG_SELF, agg);
     emote(LOST_CONCENTRATION_MSG_OTHER, agg, 
	   room_list[agg.getCurRoomNum()], TRUE);
     if (!is_canned)
       agg.MANA -= spell_mana / 2;
   }//else !canned

   if (do_effects) {
     do_door_to(room_list[agg.getCurRoomNum()], rm, 0, agg, &NULL_STRING, 
		GATE_DOOR_NUM);
   }//if
}//do_cast_gate



void cast_distortion_wall(int i_th, const String* dr, critter& pc) {
   int spell_num = DISTORTION_WALL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
   door* dptr;
   
                       /* check out pc */
   if (get_percent_lrnd(spell_num, pc) == -1) {
      show(NO_KNOW_SPELL_MSG, pc);
      return;
   }//if
   if (pc.MANA < spell_mana) {
      show(NO_MANA_MSG, pc);
      return;
   }//if
   if (pc.POS != POS_STAND) {
      show(SPELL_MUST_BE_STANDING_MSG, pc);   
      return;
   }//if

   dptr = door::findDoor(ROOM.DOORS, i_th, dr, pc.SEE_BIT, ROOM);
   if (!dptr) {
      show("You don't see that door.\n", pc);
      return;
   }//if

   do_cast_distortion_wall(*dptr, pc, FALSE, 0);  //does no error checking
}//cast_distortion_wall



void do_cast_distortion_wall(door& dr, critter& agg, int is_canned,
                        int lvl) {
   int spell_num = DISTORTION_WALL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
   String buf(100);
   short do_effects = FALSE;
   room* rm = &(room_list[agg.getCurRoomNum()]);
 
   if (room_list[agg.getCurRoomNum()].isNoMagic()) {
      show(NOT_IN_NOMAG_RM_MSG, agg);
      return;
   }//if

   if (!is_canned)
      lvl = agg.LEVEL;

   agg.PAUSE += 1; 

   if (is_canned) {
      do_effects = TRUE;
      show("A shimmering sheet of energy covers the entrance!\n", agg); 
      emote("calls forth a shimmering wall of energy.", agg, *rm, TRUE); 
   }//if canned
   else {//not canned 
      if (!lost_concentration(agg, spell_num)) { 
         do_effects = TRUE;
         show("A shimmering sheet of energy covers the entrance!\n", agg); 
         emote("calls forth a shimmering wall of energy.", agg, *rm, TRUE); 
         agg.MANA -= spell_mana;
      }//if ! lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, *rm, TRUE);
         agg.MANA -= spell_mana / 2;
      }//else
   }//else !canned

   if (do_effects) {
      door* dptr = door::findDoorByDest(room_list[abs(dr.destination)].DOORS, 
                                        agg.getCurRoomNum());
      if (!dptr) {
	 return;
      }//if
      stat_spell_cell* sp;
      Cell<stat_spell_cell*> cll(dptr->affected_by);
      while ((sp = cll.next())) {
         if (sp->stat_spell == spell_num) {
            sp->bonus_duration += (int)((float)(lvl) / 4.0);
            return;
         }//if
      }//while
      Put(new stat_spell_cell(spell_num, lvl/3), dptr->affected_by);
      affected_doors.gainData(dptr); //add to global aff'd list

      dr.affected_by.head(cll);
      while ((sp = cll.next())) {
         if (sp->stat_spell == spell_num) {
            sp->bonus_duration += (int)((float)(lvl) / 4.0);
            return;
         }//if
      }//while
      Put(new stat_spell_cell(spell_num, lvl/3), dr.affected_by);
      affected_doors.gainData(&dr); //add to global aff'd list
   }//if
}//do_cast_distortion_wall




void cast_firewall(critter& pc) {
   int spell_num = FIREWALL_SKILL_NUM;

   if (!ok_to_do_action(NULL, "KMVSN", spell_num, pc))
     return;

   do_cast_firewall(ROOM, pc, FALSE, 0);  //does no error checking
}//cast_firewall


void do_cast_firewall(room& rm, critter& agg, int is_canned,
                        int lvl) {
   int spell_num = FIREWALL_SKILL_NUM;
   int spell_mana = get_mana_cost(spell_num);
   String buf(100);
   short do_effects = FALSE;
 
   if (room_list[agg.getCurRoomNum()].isNoMagic()) {
      show(NOT_IN_NOMAG_RM_MSG, agg);
      return;
   }//if

   if (!is_canned)
      lvl = agg.LEVEL;

   agg.PAUSE += 1; 

   if (is_canned) {
      if (rm.isSmallWater() || rm.isBigWater()) { //if water
	 show("There is nothing here to sustain a fire!\n", agg);
	 return;
      }//if
      else {
         do_effects = TRUE;
         show("A ring of fire rises up around you!\n", agg); 
         emote("calls forth a circle of flames.", agg, rm, TRUE); 
      }//else
   }//if canned
   else {//not canned 
      if (!lost_concentration(agg, spell_num)) { 
         if (rm.isSmallWater() || rm.isBigWater()) { //if water
   	    show("There is nothing here to sustain a fire!\n", agg);
	    return;
         }//if
         else {
            do_effects = TRUE;
            show("A ring of flames rises up around you!\n", agg); 
            emote("calls forth a circle of flames.", agg, rm, TRUE); 
            agg.MANA -= spell_mana;
	 }//else
      }//if ! lost concentration
      else { //lost concentration
         show(LOST_CONCENTRATION_MSG_SELF, agg);
         emote(LOST_CONCENTRATION_MSG_OTHER, agg, rm, TRUE);
         agg.MANA -= spell_mana / 2;
      }//else
   }//else !canned

   if (do_effects) {
      stat_spell_cell* sp;

      short already_there;
      Cell<door*> d_cll(rm.DOORS);
      door* d_ptr;
      while ((d_ptr = d_cll.next())) {
	 d_ptr = 
            door::findDoorByDest(room_list[abs(d_ptr->destination)].DOORS,
                                 rm.getRoomNum());
	 if (!d_ptr) { 
	    continue;
	 }//if
         already_there = FALSE;
         Cell<stat_spell_cell*> cll(d_ptr->affected_by);
         while ((sp = cll.next())) {
            if (sp->stat_spell == spell_num) {
               sp->bonus_duration += (int)((float)(lvl) / 4.0);
               already_there = TRUE;
	       break;
            }//if
         }//while
	 if (!already_there) {
	    Put(new stat_spell_cell(spell_num, lvl/3),
		d_ptr->affected_by);
	 }//if
         affected_doors.gainData(d_ptr); //add to global aff'd list
      }//while
   }//if

   if (do_effects) { //for doors leading out
      stat_spell_cell* sp;

      short already_there;
      Cell<door*> d_cll(rm.DOORS);
      door* d_ptr;
      while ((d_ptr = d_cll.next())) {
         already_there = FALSE;
         Cell<stat_spell_cell*> cll(d_ptr->affected_by);
         while ((sp = cll.next())) {
            if (sp->stat_spell == spell_num) {
               sp->bonus_duration += (int)((float)(lvl) / 4.0);
               already_there = TRUE;
	       break;
            }//if
         }//while
	 if (!already_there) {
	    Put(new stat_spell_cell(spell_num, lvl/3),
		d_ptr->affected_by);
	 }//if
         affected_doors.gainData(d_ptr); //add to global aff'd list
      }//while
   }//if
}//do_cast_firewall

