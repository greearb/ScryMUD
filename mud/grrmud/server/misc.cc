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

#include "command4.h"
#include <time.h>
#include <iostream.h>
#include <fstream.h>
#include "commands.h"
#include "battle.h"
#include "misc.h"
#include "misc2.h"
#include <stdio.h>
#include "classes.h"
#include <sys/types.h>
#include <unistd.h>
#include "const.h"
#include "spells.h"
#include <PtrArray.h>
#include "vehicle.h"
#include "load_wld.h"
#include "command3.h"
#include "skills.h"
#include <time.h>


String& getCurTime() { //in the real world (tm) :)
   static String retval(100);

   long int cur_time;
   time(&cur_time);
   retval = ctime(&cur_time);

   retval.Strip();
   return retval;
}

int bound(int low, int high, int val) {
   if (val < low)
      val = low;
   if (val > high)
      val = high;
   return val;
}


void critter::save() {
   if (!pc) {
      mudlog.log(ERR, "ERROR:  non PC sent to critter::save()\n");
      return;
   }

   if (LEVEL == 0) {
      mudlog << "ERROR:  level 0 player trying to save, name:  "
             << *(getName()) << endl;
      return;
   }

   String buf(*(getName()));
   buf.Tolower();
   buf.Prepend("./Pfiles/");
   ofstream ofile(buf);
   if (!ofile) {
      mudlog.log(ERR, "ERROR:  file not opened in save_pc.\n");
      return;
   }//if

   Write(ofile);
}//save_pc


/**  msg defaults to NULL if not entered explicitly.. */
short check_l_range(long i, long l, long h, critter& pc, short disp,
                    const char* msg) {
   String buf(100);
   if (i < l) {
      if (disp) {
         if (msg) {
            Sprintf(buf, "%s:  The number is too low.\n", msg);
            pc.show(buf);
         }
         else 
            show("The number is too low.\n", pc);
	 return FALSE;
      }//if
      return FALSE;
   }//if
   else if (i > h) {
      if (disp) {
         if (msg) {
            Sprintf(buf, "%s:  The number is too high.\n", msg);
            pc.show(buf);
         }
	 else
            show("The number is too high.\n", pc);
	 return FALSE;
      }//if
      return FALSE;
   }//else
   return TRUE;
}//check_l_range()


char* get_his_her(const critter& crit) {

   if (crit.SEX == 0) { 
      return "her";
   }//if
   else if (crit.SEX == 1) {
      return "his";
   }//else
   else {
      return "its";
   }//else
}//get_his_her

char* get_hisself_herself(const critter& crit) { //this is
//grammatically incorrect, the code is same as get_himself_herself.

   if (crit.SEX == 0) { 
      return "herself";
   }//if
   else if (crit.SEX == 1) {
      return "himself";
   }//else
   else {
      return "itself";
   }//else
}//get_hisself_herself


char* get_he_she(const critter& crit) {

   if (crit.SEX == 0) { 
      return "she";
   }//if
   else if (crit.SEX == 1) {
      return "he";
   }//else
   else {
      return "it";
   }//else
}//get_he_she


int d(const int num_rolls, const int dice_sides) {
   int rtvalue = 0;

   for (int i = 0; i<num_rolls; i++) {
      rtvalue += 1 +(int)((rand() * (float)(dice_sides)) / (RAND_MAX + 1.0));
   }//for
   return rtvalue;
}//d()

/* in file parse.cc */
short isnum(String& word) {
   int len = word.Strlen();
   int i = 0;
   
   if (len == 0)
      return FALSE;

   if ((word[0] == '-') || (word[0] == '+'))
      i++;
   for ( ; i<len; i++) {
      if (!isdigit(word[i])) {
	 return FALSE;
      }//if
   }//for
   return TRUE;
}//isnum


void join_in_battle(critter& agg, critter& vict) {
  //log("In join_in_battle...\n");
   if (agg.isMob()) {
      mudlog.log(ERR, "ERROR: join_in_battle called with agg as mob\n");
      return;
   }//if
   if (vict.isMob()) {
      mudlog.log(ERR, "ERROR: join_in_battle called with vict as mob\n");
      return;
   }//if
   
   if (&vict == &agg) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  vict == agg in join_in_battle, name:  "
                << *(name_of_crit(agg, ~0)) << "  address:  " << &agg
                << endl;
      }      
      return;
   }//if

   if (vict.isImmort() && vict.isNoHassle()
       && vict.getImmLevel() >= agg.getImmLevel()) {
      if (mudlog.ofLevel(INF)) {
         mudlog << "INFO:  Not joining battle because of !hassle, agg: "
                << agg.getName() << "  vict: " << vict.getName() << endl;
      }//if
      return;
   }//if

   //ensure only one copy (that's what gainData does)
   if (agg.getCurRoomNum() == vict.getCurRoomNum()) {
      embattled_rooms.gainData(&(room_list[agg.getCurRoomNum()]));
   }
   else {
      embattled_rooms.gainData(&(room_list[agg.getCurRoomNum()]));
      embattled_rooms.gainData(&(room_list[vict.getCurRoomNum()]));
   }

		/* make sure agg hits first */
   if (room_list[agg.getCurRoomNum()].removeCritter(&agg)) {
      room_list[vict.getCurRoomNum()].gainCritter(&agg);
   }
   else {
      if (mudlog.ofLevel(DIS)) {
         mudlog << "ERROR:  could not 'LoseData' in join_in_battle, agg:  "
                << *(name_of_crit(agg, ~0)) << "  address:  " << &agg
                << "  getCurRoomNum():  " << agg.getCurRoomNum() << endl;
      }
   }

   Put(&vict, agg.IS_FIGHTING);
   Put(&agg, vict.IS_FIGHTING);

   //Some mobs will remember....
   if (vict.isNpc()) {
      if ((((vict.getLevel() > 20) && vict.mob->getBadAssedness() > 0) ||
          ((vict.mob->getBadAssedness() > 3))) ||
          (((vict.getLevel() > 20) && vict.getBenevolence() <= 0) ||
          ((vict.getBenevolence() < 0)))) {
         vict.remember(agg);
         do_tell(vict, "I'll remember that!!", agg, FALSE,
                 agg.getCurRoomNum());
      }//if
      if ((vict.getBenevolence() <= -4) || (vict.mob->getBadAssedness() >= 5)) {
         vict.setTrackingTarget(*(agg.getShortName()));
      }
   }//if

   agg.PAUSE += 1; //vict should get at least one hit in
}//join_in_battle


void do_tick() {
   //gossip("An hour has passed in our fine land.", mob_list[2]);

		   /* do all decrementing */
   decrease_timed_affecting_doors(); //takes care of affected_dors list
   decrease_timed_affecting_objects(); //takes care of affected_objects list
   decrease_timed_affecting_lds();  //takes care of alllink deads
   decrease_timed_affecting_pcs();  //takes care of all pc's
   decrease_timed_affecting_smobs(); //takes care of affected_mobs list
   decrease_timed_affecting_rooms(); //takes care of affected_rooms

		   /* do all incrementing */
   do_regeneration_pcs();  //takes care of all pc's
   do_regeneration_smobs();  //takes care of affected_mobs list
   do_regeneration_objects();  //takes care of affected_objects list
   ZoneCollection::instance().doRegeneration(); //update mobs, objs, rooms if needed

                  /* time */

   Hour++;
   if (Hour >= 24) {
      Hour = 0;
      Day++;
   }//if
   if (Day >= 365) {
      Year++;
      Day = 0;
   }//if

   write_setup();  //save time data to file basically
}//do_tick


void do_mini_tick() { // decrement pause count ect
   Cell<critter*> cll;
   pc_list.head(cll);
   critter* crit_ptr;
   Cell<stat_spell_cell*> scell;
   stat_spell_cell* sc_ptr;

   do_vehicle_moves();  //in misc2.cc

   while ((crit_ptr = cll.next())) {

      // Turn un-blocked, if not in battle.
      if (!crit_ptr->isInBattle()) {
         crit_ptr->crit_flags.turn_off(21);
      }

      /* Take care of round-by-round affects on other spells/skills. */
      if ((sc_ptr = is_affected_by(EARTHMELD_SKILL_NUM, *crit_ptr))) {
         if (crit_ptr->isFighting()) {
            crit_ptr->MANA -= EARTHMELD_BATTLE_MANA_COST;
         }
         else {
            crit_ptr->MANA -= EARTHMELD_NON_BATTLE_MANA_COST;
         }

         if (crit_ptr->MANA <= 0) {
            crit_ptr->breakEarthMeld();
         }//if
      }//if

      // Take care of other things...
      if (crit_ptr->PAUSE > 0) {
	 crit_ptr->PAUSE--;
      }//
      crit_ptr->MINI_AFFECTED_BY.head(scell);
      sc_ptr = scell.next();
      while (sc_ptr) {
         sc_ptr->bonus_duration--;
         if (sc_ptr->bonus_duration == 0) {
            rem_effects_crit(sc_ptr->stat_spell, *crit_ptr, TRUE);
            delete sc_ptr;
            sc_ptr = crit_ptr->MINI_AFFECTED_BY.lose(scell);
         }//if
         else
            sc_ptr = scell.next();
      }//while
   }//while

   affected_mobs.head(cll);
   while ((crit_ptr = cll.next())) {

      // Turn un-blocked, if not in battle.
      if (!crit_ptr->isInBattle()) {
         crit_ptr->crit_flags.turn_off(21);
      }

      if (crit_ptr->PAUSE > 0)
	 crit_ptr->PAUSE--;

      crit_ptr->MINI_AFFECTED_BY.head(scell);
      sc_ptr = scell.next();
      while (sc_ptr) {
         sc_ptr->bonus_duration--;
         if (sc_ptr->bonus_duration == 0) {
            rem_effects_crit(sc_ptr->stat_spell, *crit_ptr, TRUE);
            delete sc_ptr;
            sc_ptr = crit_ptr->MINI_AFFECTED_BY.lose(scell);
         }//if
         else
            sc_ptr = scell.next();
      }//while
   }//while

   // Decrease pause count for rooms that are running scripts.
   room* rm_ptr;
   for (int cnt = 0; cnt < proc_action_rooms.getCurLen(); cnt++) {
      if (!(rm_ptr = proc_action_rooms.elementAt(cnt))) {
         continue;
      }//if
      else {
         rm_ptr->decrementPause();
      }
   }//for

   // Decrease pause count for objects that are running scripts.
   object* o_ptr;
   for (int cnt = 0; cnt < proc_action_objs.getCurLen(); cnt++) {
      if (!(o_ptr = proc_action_objs.elementAt(cnt))) {
         continue;
      }//if
      else {
         o_ptr->decrementPause();
      }
   }//for

}//do_mini_tick


void do_regeneration_pcs() {
   float adj = 1.0, posn_mod;
   Cell<critter*> crit_cell;
   pc_list.head(crit_cell);
   critter* crit_ptr;

   //log("In do_regeneration_pcs\n");
 
   while ((crit_ptr = crit_cell.next())) {
      if ((crit_ptr->THIRST == 0) || (crit_ptr->HUNGER == 0)) {
	 adj = 0.2; // big penalty for being hungry
      }

      if ((crit_ptr->POS > POS_SIT) &&
	  room_list[crit_ptr->getCurRoomNum()].canCamp()) {
         adj *= ((float)(200 + get_percent_lrnd(CAMPING_SKILL_NUM,
                                               *crit_ptr))) / 200.0;
      }//if camping is an issue

      posn_mod = (2.0 + (float)(crit_ptr->POS)) / 4.0;

      crit_ptr->HP += (int)((((float)(crit_ptr->CON) + 5.0) / 15.0) * 
                            (((float)(crit_ptr->HP_MAX)) / 9.0) * 
                            posn_mod * (((float)(crit_ptr->HP_REGEN)) / 100.0)
                            * adj + 10.0);

      crit_ptr->MANA += (int)(((((float)(crit_ptr->INT)) + 5.0) / 16.0) *
                              posn_mod *
                              (((float)(crit_ptr->MA_MAX)) / 7.0) *
                              (((float)(crit_ptr->MA_REGEN)) / 100.0) *
                              adj + 4.0);

      int tmp_mov;
      tmp_mov = (int)(((((float)(crit_ptr->DEX)) + 5.0) / 16.0) *
                      posn_mod * adj *
                      (((float)(crit_ptr->MV_MAX)) / 3.0) * 
                      (((float)(crit_ptr->MV_REGEN)) / 100.0) + 3.0);

      crit_ptr->MOV += tmp_mov;

      //if (tmp_mov < 0) {
      //   if (mudlog.ofLevel(DBG)) {
      //      mudlog << "WARNING:  mov regen < 0 for PC:  "
      //             <<  *(name_of_crit(*crit_ptr, ~0)) << "  it was:  "
      //             << tmp_mov << endl;
      //   }
      //}

      if (crit_ptr->HP > crit_ptr->HP_MAX)
	 crit_ptr->HP = crit_ptr->HP_MAX;
      if (crit_ptr->MANA > crit_ptr->MA_MAX)
	 crit_ptr->MANA = crit_ptr->MA_MAX;
      if (crit_ptr->MOV > crit_ptr->MV_MAX)
	 crit_ptr->MOV = crit_ptr->MV_MAX;
      if (crit_ptr->HP < 0)
	 crit_ptr->HP = 0;
      if (crit_ptr->MANA < 0)
	 crit_ptr->MANA = 0;
      if (crit_ptr->MOV < 0)
	 crit_ptr->MOV = 0;

      if ((((float)(crit_ptr->HP)) / (float)(crit_ptr->HP_MAX)) > 0.80) {
         stat_spell_cell* ssptr = 
            is_affected_by(BIND_WOUND_SKILL_NUM, *crit_ptr);
         if (ssptr) {
            crit_ptr->affected_by.loseData(ssptr);
            rem_effects_crit(BIND_WOUND_SKILL_NUM, *crit_ptr, TRUE);
	}
      }//if
   }//while

}//do_regeneration_pcs


void do_regeneration_smobs() {
   float posn_mod, adj = 1.0;
   Cell<critter*> crit_cell;
   affected_mobs.head(crit_cell);
   critter* crit_ptr;

   //log("In do_regeneration_smobs\n");

   while ((crit_ptr = crit_cell.next())) {
      posn_mod = (2.0 + crit_ptr->POS) / 4.0;
      crit_ptr->HP +=
         (int)(((crit_ptr->CON + 5.0) / 15.0) * 
               (crit_ptr->HP_MAX / 9.0) * 
               posn_mod * (crit_ptr->HP_REGEN / 100.0) * adj + 10);
      crit_ptr->MANA +=
         (int)(((crit_ptr->INT + 5.0) / 16.0)  * posn_mod *
               (crit_ptr->MA_MAX / 7.0) * (crit_ptr->MA_REGEN / 100.0) * adj + 4);
      crit_ptr->MOV += 
         (int)(((crit_ptr->DEX + 5.0) / 16.0) * posn_mod * 
               adj *
               (crit_ptr->MV_MAX / 2.0) * (crit_ptr->MV_REGEN / 100.0) + 5);
      
      if (crit_ptr->HP > crit_ptr->HP_MAX)
	 crit_ptr->HP = crit_ptr->HP_MAX;
      if (crit_ptr->MANA > crit_ptr->MA_MAX)
	 crit_ptr->MANA = crit_ptr->MA_MAX;
      if (crit_ptr->MOV > crit_ptr->MV_MAX)
	 crit_ptr->MOV = crit_ptr->MV_MAX;
      
      if (((float)crit_ptr->HP / (float)crit_ptr->HP_MAX) > 0.80) {
         stat_spell_cell* ssptr = 
            is_affected_by(BIND_WOUND_SKILL_NUM, *crit_ptr);
         if (ssptr) {
            crit_ptr->affected_by.loseData(ssptr);
            rem_effects_crit(BIND_WOUND_SKILL_NUM, *crit_ptr, TRUE);
         }
      }//if
   }//while
   
}//do_regeneration_smobs


void do_regeneration_objects() {
  return;
}//do_regeneration_objects


void update_critters(int zone_num, short read_all) {
   Cell<object*> ocell;
   critter tmp_mob;
   int k, i;
   object* obj_ptr;
   char temp_str[81];
   String buf(100);

   //Sprintf(buf, "In update_critters, zone: %i.", zone_num);
   //log(buf);

   Sprintf(buf, "./World/critters_%i", zone_num);
   ifstream mobfile(buf);
   if (!mobfile) { 
      buf.Prepend("cp ./World/DEFAULT_CRITTERS ");
      system(buf);
      mudlog.log(ERR, "WARNING:  created CRITTER_FILE in update_critters");
      tmp_mob.CRITTER_TYPE = 1; //make it a SMOB, so destructor fires correctly
      return;
   }//if

   mobfile >> k;
   mobfile.getline(temp_str, 80); //junk line
   while (k != -1) {

      Sprintf(buf, "About to read mob:  %i, zone %i.", k, zone_num);
      mudlog.log(DBG, buf);

      if (!mobfile) {
         mudlog << "ERROR:  mobfile is not valid, misc.cc" << endl;
         break;
      }

      tmp_mob.Read(mobfile, read_all);

      if (!mob_list[k].mob) {
         mudlog << "WARNING:  mob[" << k 
                << "] is NULL in update critters." << endl;
      }//if
      else {
         if (mob_list[k].MOB_FLAGS.get(4)) {

            //log("About to check if need to load more inv.\n");
            tmp_mob.inv.head(ocell);
            while ((obj_ptr = ocell.next())) {
               if ((obj_count(tmp_mob.inv, *obj_ptr) >
                   obj_count(mob_list[k].inv, *obj_ptr))
                   && (obj_ptr->getCurInGame() < obj_ptr->getMaxInGame())) {
                  Put(&(obj_list[obj_ptr->getIdNum()]), mob_list[k].inv);
                  recursive_init_loads(obj_list[obj_ptr->getIdNum()], 0);
               }//if
            }//while      
            
            for (i = 1; i<MAX_EQ; i++) {
               if ((tmp_mob.EQ[i] && !mob_list[k].EQ[i]) 
                   && (tmp_mob.EQ[i]->getCurInGame() 
                       < tmp_mob.EQ[i]->getMaxInGame())) {
                  mob_list[k].EQ[i] = &(obj_list[tmp_mob.EQ[i]->getIdNum()]);
                  recursive_init_loads(*(mob_list[k].EQ[i]), 0);
               }//if
            }//for
         }//if should update
      }//else

      mobfile >> k;
      mobfile.getline(temp_str, 80);
   }//while, the big loop, reads in a whole mob list
   //log("Done w/update critters.\n");

   tmp_mob.CRITTER_TYPE = 1; //make it a SMOB, so destructor fires correctly
}//update_critters


void update_objects(int zone_num, short read_all) {
   Cell<object*> ocell;
   int k;
   object temp_obj;
   object* obj_ptr;
   char temp_str[81];
   String buf(100);

//   log("In update_objects.\n");

   Sprintf(buf, "./World/objects_%i", zone_num);
   ifstream objfile(buf);
   if (!objfile) { 
      buf.Prepend("cp ./World/DEFAULT_OBJECTS ");
      system(buf);
      mudlog.log(ERR, "WARNING:  created new OBJECT_FILE in update_objects");
      temp_obj.IN_LIST = room_list[0].getInv(); //hack to let destructor fire
                                                //happily, turn it into a SOBJ
      return;
   }//if

   objfile >> k;
   objfile.getline(temp_str, 80); //junk line
   while (k != -1) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "About to read obj:  " << k
                << " zone:  " << zone_num << endl;
      }

      temp_obj.Read(objfile, read_all);

      if (obj_list[k].OBJ_FLAGS.get(70)) {

         //log("About to check if need to load more inv.\n");
         temp_obj.inv.head(ocell);
         while ((obj_ptr = ocell.next())) {
            if ((obj_count(temp_obj.inv, *obj_ptr) >
                 obj_count(obj_list[k].inv, *obj_ptr))
                && (obj_ptr->getCurInGame() < obj_ptr->getMaxInGame())) {
               Put(&(obj_list[obj_ptr->getIdNum()]), obj_list[k].inv);
               recursive_init_loads(obj_list[obj_ptr->getIdNum()], 0);
            }//if
         }//while      
      }//if should update

      objfile >> k;
      objfile.getline(temp_str, 80);
   }//while, the big loop, reads in a whole mob list

   temp_obj.IN_LIST = room_list[0].getInv(); //hack to let destructor fire
                                             //happily, turn it into a SOBJ
   //log("Done w/update objects.\n");
}//update_objects



void update_zone(int zone_num, short read_all) {
   int k = 0;
   room tmp_room;
   vehicle tmp_veh;

   object* obj_ptr;
   Cell<object*> ocell;
   critter* crit_ptr;
   Cell<critter*> ccell;
   char temp_str[100];
   String buf(100);

   //Sprintf(buf, "In update_zone:  zone# = %i.\n", zone_num);
   //log(buf);

         //if it's locked, don't mess w/it
   if (ZoneCollection::instance().elementAt(zone_num).isLocked() &&
       (read_all == FALSE)) { //if locked and an automatic update
      //log("Locked, returning...\n");
      return; //don't do anything
   }//if
      
                 /* this gossip is temporary */
   //gossip("The light flickers and you almost see something...", 
   //	  mob_list[2]);

   Sprintf(buf, "./World/zone_%i", zone_num);
   ifstream rfile(buf);
   if (!rfile) { 
      buf.Prepend("cp ./World/DEFAULT_ZONE ");
      system(buf);
      mudlog.log(ERR, "WARNING: zone file created in update_zone.");
      mudlog.log(ERR, buf);
      return;
   }//if

   rfile >> k;
   rfile.getline(temp_str, 80); //junk line
   while (k != -1) {
      room* room_ptr;

      if (k & 0x01000000) { //it's a vehicle
         k = (k & ~(0x01000000));

         if (mudlog.ofLevel(DBG)) {
            mudlog << "About to read vehicle:  " << k
                << " zone:  " << zone_num << endl;
         }
         tmp_veh.Read(rfile, read_all);
         room_ptr = &tmp_veh;
      }
      else {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "About to read room:  " << k
                   << " zone:  " << zone_num << endl;
         }
         tmp_room.Read(rfile, read_all);
         room_ptr = &tmp_room;
      }

      if (read_all) {
         room_list[k].setTotalLoaded(TRUE); //total_loaded flag
      }//if

      //log("About to check if need to load more inv into the room.\n");
      room_ptr->getInv()->head(ocell);
      while ((obj_ptr = ocell.next())) {
         if ((obj_count(*(room_ptr->getInv()), *obj_ptr) >
              obj_count(*(room_list[k].getInv()), *obj_ptr)) &&
	     (obj_ptr->getCurInGame()
              < obj_list[obj_ptr->getIdNum()].getMaxInGame())) {

            //points into object array
            room_list[k].gainObject(&(obj_list[obj_ptr->getIdNum()]));
	    recursive_init_loads(obj_list[obj_ptr->getIdNum()], 0);
         }//if
      }//while      

      //log("Checking for load-more-critters?\n");
      List<critter*> tmp_lst(room_ptr->getCrits());
      tmp_lst.head(ccell);
      int tmp_cc;
      int ingame_cc;
      while ((crit_ptr = ccell.next())) {
         tmp_cc = room_ptr->getCritCount(*crit_ptr);
         ingame_cc = room_list[k].getCritCount(*crit_ptr);
         if ((tmp_cc > ingame_cc) &&
	     ((crit_ptr->getCurInGame() - tmp_cc) <
              crit_ptr->getMaxInGame())) {

            //ptr into mob array
            if (mudlog.ofLevel(DBG)) {
               mudlog << "LOAD: Loading critter#: " << crit_ptr->getIdNum()
                      << " in room: " << k << " curInGame: "
                      << crit_ptr->getCurInGame() << " maxInGame: "
                      << crit_ptr->getMaxInGame() << " tmp_cc: "
                      << tmp_cc << " ingame_cc: " << ingame_cc << endl;
            }

            if (crit_ptr->isPlayerShopKeeper()) {
               critter* shop_keeper = load_player_shop_owner(crit_ptr->getIdNum());
               if (shop_keeper) {
                  room_list[k].gainCritter(shop_keeper);
               }//if
            }//if
            else {
               room_list[k].gainCritter(&(mob_list[crit_ptr->getIdNum()]));
            }

            // Only does objects now.
	    recursive_init_loads(mob_list[crit_ptr->getIdNum()]);
         }//if
      }//while      
      rfile >> k;
      rfile.getline(temp_str, 80);
   }//while, the big loop, reads in a whole zone
   tmp_room.Clear();
}//update_zone


void decrease_timed_affecting_pcs() {  //will decrease all
	    //timed affects by one time unit, if zero, it will  
	    //delete them from the spells_affecting list, it
	    //will output all neccessary messages
	    //UNLESS THE SPELL IS PERMANENT (ie if time_left = -1)

   // log("In decrease_timed_affecting_pcs\n");

   Cell<critter*> crit_cell;
   pc_list.head(crit_cell);
   critter* crit_ptr;
   Cell<stat_spell_cell*> sp_cell;
   stat_spell_cell* sp_ptr;
   String buf(100);

   while ((crit_ptr = crit_cell.next())) {
      if ((++(crit_ptr->pc->idle_ticks) > 30) && 
	  (!crit_ptr->pc->imm_data || (crit_ptr->IMM_LEVEL < 2))) {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "Logging off player in decrease_timed_affecting_pcs,"
                   << " name:  " << *(name_of_crit(*crit_ptr, ~0)) 
                   << "  address:  " << crit_ptr << "  ticks:  "
                   << crit_ptr->pc->idle_ticks << endl;
         }
         log_out(*crit_ptr);
      }//if
      if (TRUE /*crit_ptr->pc->mode == MODE_NORMAL*/) {
         crit_ptr->affected_by.head(sp_cell);
         sp_ptr = sp_cell.next();
         while (sp_ptr) {
   	    if (sp_ptr->bonus_duration != -1)
	       sp_ptr->bonus_duration--;
	    if (sp_ptr->bonus_duration == 0) {
	       rem_effects_crit(sp_ptr->stat_spell, *crit_ptr, TRUE);
	       delete sp_ptr;
               sp_ptr = crit_ptr->affected_by.lose(sp_cell);
	    }//if
	    else
	       sp_ptr = sp_cell.next();
         }//while

         if (crit_ptr->HUNGER > 0)
	    crit_ptr->HUNGER--;  //food
         if (crit_ptr->THIRST > 0)
	    crit_ptr->THIRST--;  //drink
         if (crit_ptr->DRUGGED > 0)
	    crit_ptr->DRUGGED--;  //drugged

	 if (crit_ptr->MODE == MODE_NORMAL) {
            if (crit_ptr->HUNGER == 0)
      	       show("You are famished.\n", *crit_ptr);
            if (crit_ptr->THIRST == 0)
	       show("You are thirsty.\n", *crit_ptr);
            if (crit_ptr->DRUGGED == 0) {
	       show("You no longer feel drugged.\n", *crit_ptr);
	       crit_ptr->DRUGGED = -1;
            }//if
	 }//if

		/* check for lights about to go out */
         if (crit_ptr->EQ[11]) {
	    if (!crit_ptr->EQ[11]->IN_LIST) {
	       crit_ptr->EQ[11] = obj_to_sobj(*(crit_ptr->EQ[11]), 
					      &(crit_ptr->inv), crit_ptr->getCurRoomNum());
	    }//if
	    if (crit_ptr->EQ[11]->extras[0] == 1) {
	       Sprintf(buf, "%S flickers.\n", 
                       long_name_of_obj(*(crit_ptr->EQ[11]), crit_ptr->SEE_BIT));
               buf.Cap();
	       show(buf, *crit_ptr);
	    }//if
         }//if

		/* check for lights gone out */
         if (crit_ptr->EQ[11]) {
	    if (crit_ptr->EQ[11]->extras[0] == 0) {
	       crit_ptr->EQ[11]->extras[0] = -2;
	       Sprintf(buf, "%S dims and glows its last.\n", 
                       long_name_of_obj(*(crit_ptr->EQ[11]), crit_ptr->SEE_BIT));
               buf.Cap();
	       show(buf, *crit_ptr);
	       crit_ptr->crit_flags.turn_off(USING_LIGHT_SOURCE);
	       room_list[crit_ptr->getCurRoomNum()].checkLight(FALSE);
               crit_ptr->EQ[11]->short_desc += "(OUT)";
               crit_ptr->EQ[11]->in_room_desc += "(OUT)";
	    }//if
         }//if
      }//if
   }//while

}//decrease_timed_affecting_pcs


void decrease_timed_affecting_lds() {

   // log("In decrease_timed_affecting_pcs\n");

   Cell<critter*> crit_cell;
   linkdead_list.head(crit_cell);
   critter* crit_ptr;
   Cell<stat_spell_cell*> sp_cell;
   stat_spell_cell* sp_ptr;
   String buf(100);

   while ((crit_ptr = crit_cell.next())) {
      if (++(crit_ptr->pc->idle_ticks) > 30) {
         mudlog << "Logging off player in decrease_timed_affecting_lds,"
                << " name:  " << *(name_of_crit(*crit_ptr, ~0)) 
                << "  address:  " << crit_ptr << endl;
         log_out(*crit_ptr);
      }//if
      if (TRUE /*crit_ptr->pc->mode == MODE_NORMAL*/) {
         crit_ptr->affected_by.head(sp_cell);
         sp_ptr = sp_cell.next();
         while (sp_ptr) {
   	    if (sp_ptr->bonus_duration != -1)
	       sp_ptr->bonus_duration--;
	    if (sp_ptr->bonus_duration == 0) {
	       rem_effects_crit(sp_ptr->stat_spell, *crit_ptr, TRUE);
	       delete sp_ptr;
               sp_ptr = crit_ptr->affected_by.lose(sp_cell);
	    }//if
	    else
	       sp_ptr = sp_cell.next();
         }//while

         if (crit_ptr->HUNGER > 0)
	    crit_ptr->HUNGER--;  //food
         if (crit_ptr->THIRST > 0)
	    crit_ptr->THIRST--;  //drink
         if (crit_ptr->DRUGGED == 0)
	    crit_ptr->DRUGGED--;  //food

	 if (crit_ptr->MODE == MODE_NORMAL) {
            if (crit_ptr->HUNGER == 0)
      	       show("You are famished.\n", *crit_ptr);
            if (crit_ptr->THIRST == 0)
	       show("You are thirsty.\n", *crit_ptr);
            if (crit_ptr->DRUGGED == 0) {
	       show("You no longer feel drugged.\n", *crit_ptr);
	       crit_ptr->DRUGGED = -1;
            }//if
	 }//if

		/* check for lights about to go out */
         if (crit_ptr->EQ[11]) {
	    if (!crit_ptr->EQ[11]->IN_LIST) {
	       crit_ptr->EQ[11] = obj_to_sobj(*(crit_ptr->EQ[11]), 
					      &(crit_ptr->inv), crit_ptr->getCurRoomNum());
	    }//if
	    if (crit_ptr->EQ[11]->extras[0] == 1) {
	       Sprintf(buf, "%S flickers.\n", 
		    long_name_of_obj(*(crit_ptr->EQ[11]), crit_ptr->SEE_BIT));
               buf.Cap();
	       show(buf, *crit_ptr);
	    }//if
         }//if

		/* check for lights gone out */
         if (crit_ptr->EQ[11]) {
	    if (crit_ptr->EQ[11]->extras[0] == 0) {
	       crit_ptr->EQ[11]->extras[0] = -2;
	       Sprintf(buf, "%S dims and glows its last.\n", 
		    long_name_of_obj(*(crit_ptr->EQ[11]), crit_ptr->SEE_BIT));
               buf.Cap();
	       show(buf, *crit_ptr);
	       crit_ptr->crit_flags.turn_off(USING_LIGHT_SOURCE);
	       room_list[crit_ptr->getCurRoomNum()].checkLight(FALSE);
	    }//if
         }//if
      }//if
   }//while

}//decrease_timed_affecting_lds


void decrease_timed_affecting_smobs() {  //will decrease all
	    //timed affects by one time unit, if zero, it will  
	    //delete them from the spells_affecting list, it
	    //will output all neccessary messages
	    //UNLESS THE SPELL IS PERMANENT (ie if time_left = -1)

   // log("In decrease_timed_affecting_mobs\n");

   Cell<critter*> crit_cell;
   affected_mobs.head(crit_cell);
   critter* crit_ptr;
   Cell<stat_spell_cell*> sp_cell;
   stat_spell_cell* sp_ptr;

   crit_ptr = crit_cell.next();
   while (crit_ptr) {
      if (IsEmpty(crit_ptr->affected_by)) {
         
         // Can't ever take them off, because then they
         // don't decrease their PAUSE counts from ordering
         // and such.
         crit_ptr = crit_cell.next(); 

//          if (crit_ptr->PAUSE > 0) {
//             crit_ptr = crit_cell.next();
//          }
//          else {
//             if (crit_ptr->mob->mob_data_flags.get(17)) {
//                //then got spec data, should be on here.
//                crit_ptr = crit_cell.next();
//             }
//             else {
//                // Else, then we can safely take them off the list I believe
//                crit_ptr = crit_cell.lose();
//             }
//          }//else
      }//if
      else {
         crit_ptr->affected_by.head(sp_cell);
         sp_ptr = sp_cell.next();
         while (sp_ptr) {
	    if (sp_ptr->bonus_duration != -1)
	       sp_ptr->bonus_duration--;
   	    if (sp_ptr->bonus_duration == 0) {
	       rem_effects_crit(sp_ptr->stat_spell, *crit_ptr, TRUE);
               delete sp_ptr;
               sp_ptr = crit_ptr->affected_by.lose(sp_cell);
	    }//if
	    else 
	       sp_ptr = sp_cell.next();
         }//while
         crit_ptr = crit_cell.next();
      }//else
   }//while

}//decrease_timed_affecting_smobs



void decrease_timed_affecting_objects() {  
   Cell<stat_spell_cell*> sp_cell;
   stat_spell_cell* sp_ptr;
   Cell<object*> cell;
   affected_objects.head(cell);
   object* obj_ptr;
   String buf(100);

   //log("In decrease_timed_affecting_objects\n");

   obj_ptr = cell.next();
   while (obj_ptr) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "decrease_timed_affecting_objects, obj_number:  "
                << obj_ptr->OBJ_NUM << endl;
      }

      obj_ptr->affected_by.head(sp_cell);
      sp_ptr = sp_cell.next();
      while (sp_ptr) {
	 if (sp_ptr->bonus_duration == 0) {
	    rem_effects_obj(sp_ptr->stat_spell, *obj_ptr);
            delete sp_ptr;
            sp_ptr = obj_ptr->affected_by.lose(sp_cell);
	 }//if
	 else { 
	   if (sp_ptr->bonus_duration != -1)
	     sp_ptr->bonus_duration--;
	   sp_ptr = sp_cell.next();
	 }//else
      }//while

      if (mudlog.ofLevel(DBG)) {
         mudlog << "About to look after lightts.." << endl;
      }
      
		    /* take care of lights */

      if (obj_ptr->OBJ_FLAGS.get(OBJ_LIGHT_SOURCE)) {
	 if (obj_ptr->extras[0] > -1) { //is not perm
	   obj_ptr->extras[0]--;
	 }//if
      }//if

			    /* corpses */

      if (mudlog.ofLevel(DBG)) {
         mudlog << "About to look after corpses.." << endl;
      }

      if (obj_ptr->OBJ_NUM == CORPSE_OBJECT) { //if is corpse
	 if (obj_ptr->bag && 
	     obj_ptr->bag->time_till_disolve == 0) { //disolve it!!
	    disolve_object(*obj_ptr);
	    obj_ptr = affected_objects.lose(cell);
	 }//if
	 else {
	   if (obj_ptr->bag->time_till_disolve != -1) {
	     obj_ptr->bag->time_till_disolve--;
	     obj_ptr = cell.next();
	   }//if
           else {
              obj_ptr = cell.next();
           }
	 }//else
      }//if is a corpse
      else {
	 obj_ptr = cell.next(); //couldn't have disolved
      }//else
   }//while
}//decrease_timed_affecting_objects


void decrease_timed_affecting_doors() {  
   Cell<stat_spell_cell*> sp_cell;
   stat_spell_cell* sp_ptr;
   Cell<door*> cell;
   affected_doors.head(cell);
   door* dr_ptr;
   String buf(100);

   //log("In decrease_timed_affecting_doors\n");

   int take_me_off;
   dr_ptr = cell.next();
   while (dr_ptr) {
      if (dr_ptr->ticks_till_disolve > 0) {
         mudlog.log(DBG, "About to decrement a door's ticks_till_disolve.\n");
         if (--(dr_ptr->ticks_till_disolve) <= 0) {
            mudlog.log(DBG, "Gonna delete a door.\n");
            room_list[dr_ptr->in_room].DOORS.loseData(dr_ptr);
            Sprintf(buf, "%S closes up and vanishes.\n", 
                    name_of_door(*dr_ptr, ~0));
            buf.Cap();
            room_list[dr_ptr->in_room].showAllCept(buf);
            delete dr_ptr;
            dr_ptr = affected_doors.lose(cell);
            continue;
         }//if
      }//if
      
      take_me_off = TRUE;
      dr_ptr->affected_by.head(sp_cell);
      sp_ptr = sp_cell.next();
      while (sp_ptr) {
	 take_me_off = FALSE;
	 if (sp_ptr->bonus_duration != -1)
            sp_ptr->bonus_duration--;
	 if ((sp_ptr->bonus_duration == 0) || (sp_ptr->bonus_duration < -1)) {
	    rem_effects_door(sp_ptr->stat_spell, *dr_ptr, 
			     room_list[abs(dr_ptr->destination)],
			     room_list[dr_ptr->in_room], TRUE);
            delete sp_ptr;
            sp_ptr = dr_ptr->affected_by.lose(sp_cell);
	 }//if
	 else
	    sp_ptr = sp_cell.next();
      }//while
      if (take_me_off)
	 dr_ptr = affected_doors.lose(cell);
      else
	 dr_ptr = cell.next();
   }//while
}//decrease_timed_affecting_doors


void disolve_object(object& obj) {
   obj_to_be_disolved_list.append(&obj);
}//disolve_object


void do_disolve_object(object& obj) {
   Cell<object*> cell(obj.inv);
   object *obj_ptr;

   //log("In disolve_object\n");

   if (!obj.in_list) {
      mudlog.log(ERR, "ERROR:  disolve_object called on non-sobj.\n");
      do_shutdown = TRUE;
      exit(100);
   }//if

   //log("Before while\n");
   while ((obj_ptr = cell.next())) {
      if (obj_ptr->in_list)
         obj_ptr->in_list = obj.in_list;
      obj.in_list->append(obj_ptr);
   }//while
   //log("Before Clear()\n");
   obj.inv.clear();

   obj.in_list->loseData(&obj); //delete the original 
				       //obj to be disolved
   //log("Lost data.\n");
   delete &obj; //should fire deconstructors
   //log("Done.\n");
}//do_disolve_obj


void decrease_timed_affecting_rooms() {  //will decrease all
	    //timed affects by one time unit, if zero, it will  
	    //delete them from the spells_affecting list, it
	    //will output all neccessary messages
	    //UNLESS THE SPELL IS PERMANENT (ie if time_left = -1)

   Cell<room*> cell;
   affected_rooms.head(cell);
   room* rm_ptr;
   Cell<stat_spell_cell*> sp_cell;
   stat_spell_cell* sp_ptr;
   short take_it_off = FALSE;

  // log("In decrease_timed_affecting_rooms\n");
   rm_ptr = cell.next();
   while (rm_ptr) {
      take_it_off = TRUE;
      
      
      Cell<door*> dcll(rm_ptr->DOORS);
      door* dptr, *tmp_dptr;
      dptr = dcll.next();
      while (dptr) {
         if (dptr->ticks_till_disolve != -1) {
            if ((--dptr->ticks_till_disolve) == 0) {
               tmp_dptr = dptr;
               dptr = rm_ptr->DOORS.lose(dcll);
               delete tmp_dptr;
            }//if
            else {
               take_it_off = FALSE;
               dptr = dcll.next();
            }//else
         }//if
         else
            dptr = dcll.next();
      }//while
      
      rm_ptr->affected_by.head(sp_cell);
      sp_ptr = sp_cell.next();
      while (sp_ptr) {
	 if (sp_ptr->bonus_duration != -1) { //if not permanent
            take_it_off = FALSE;
	    sp_ptr->bonus_duration--;
         }//if
	 if (sp_ptr->bonus_duration == 0) {
	    rem_effects_room(sp_ptr->stat_spell, *rm_ptr, TRUE);
            delete sp_ptr;
            sp_ptr = rm_ptr->affected_by.lose(sp_cell);
	 }//if
	 else
	    sp_ptr = sp_cell.next();
      }//while
      
      if (take_it_off) 
         rm_ptr = affected_rooms.lose(cell);
      else
         rm_ptr = cell.next();
   }//while
}//decrease_timed_affecting_rooms


void add_spell_affecting_critter(int spell, int duration, critter& vict) {
   stat_spell_cell* spcell = new stat_spell_cell;

   //log("In add_spell_affecting_critter");

   spcell->stat_spell = spell;
   spcell->bonus_duration = duration; 
   Put(spcell, vict.affected_by);
}//gain spell_affected_by    


void add_spell_affecting_obj(int spell, int duration, object& vict) {
   stat_spell_cell* spcell = new stat_spell_cell;

   //log("In add_spell_affecting_obj\n");

   spcell->stat_spell = spell;
   spcell->bonus_duration = duration; 
   Put(spcell, vict.affected_by);
}//gain spell_affected_by    


void show(const char* message, critter& pc) {
   //log(message);

   if (mudlog.ofLevel(XMT)) {
      mudlog << "OUTPUT from -:" << *(pc.getName());
         mudlog << ":-  -:" << message << ":-\n" << endl;
   }

   if (pc.possessed_by) {
      pc.possessed_by->show("[POSSESSED]: ");
      pc.possessed_by->show(message);
   }

   if (pc.pc) {

      critter* snooper;
      if ((snooper = pc.SNOOPED_BY)) {
         mudlog.log(TRC, "Within snoop if\n");
         String buf2(100);
         Sprintf(buf2, "SNOOP_OUT:  -:%s:-\n", message);
         snooper->show(buf2);
      }//if snoop

      pc.setDoPrompt(TRUE);
      if (!message)
         return;

      if (pc.pc->output.Strlen() < OUTPUT_MAX_LEN) {
	 pc.pc->output.Append(message);
      }//if
   }//if
}//show 


void show_all(const char* msg, const room& rm) {
   rm.showAllCept(msg, NULL);
}


void show_all_but_2(critter& A, critter& B, const char* msg,
                    room& rm) {
   Cell<critter*> cell(rm.getCrits());
   critter* crit_ptr;

   // log("In show_all_but_2\n");
   while ((crit_ptr = cell.next()))  { 
      if ((crit_ptr != &A) && (crit_ptr != &B)) 
	 if (crit_ptr->POS < POS_SLEEP)  
	    show(msg, *crit_ptr);
   }//while

   Cell<object*> cll(*(rm.getInv()));
   object* obj;

   while ((obj = cll.next())) {
     if (obj->obj_proc && (crit_ptr = obj->obj_proc->w_eye_owner)) {
       if (crit_ptr->POS < POS_SLEEP) {
	 show("#####", *crit_ptr);
	 show(msg, *crit_ptr);
       }//if
     }//if
   }//while

}//show_all_but_2


void show_all_info(const char* msg) {
   Cell<critter*> cell(pc_list);
   critter* crit_ptr;

   while ((crit_ptr = cell.next()))  { 
      if (crit_ptr->pc && crit_ptr->PC_FLAGS.get(14)) {
	 show(msg, *crit_ptr);
      }//if
   }//while
}//show_all_info


void out_str(const List<String*>& lst, critter& pc) { 
   Cell<String*> cell(lst);
   String *string;
   
   // log("In out_str.\n");

   if (pc.pc) {
      if (IsEmpty(lst)) {
	 show("You see nothing special.\n", pc);
	 return;
      }//if
      while ((string = cell.next())) {
	 show(*string, pc);
	 show("\n", pc);
      }//while
   }//if
}//out_str


void out_crit(const List<critter*>& lst, critter& pc) {  //outs the names
   Cell<critter*> cell(lst);
   Cell<stat_spell_cell*> cell2;
   stat_spell_cell* sp;
   critter* crit_ptr;
   String buf(100);

   // log("In out_crit\n");

   if (pc.isUsingClient())
      show("<MOB_LIST>", pc);
   else if (pc.isUsingColor()) {
      pc.show(*(pc.getMobListColor()));
   }

   while ((crit_ptr = cell.next())) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "out_crit: got critter:  " << *(crit_ptr->getName())
                << endl;
      }
      if (detect(pc.SEE_BIT, crit_ptr->VIS_BIT) &&
          (crit_ptr != &pc)) { //can see it, not looker
         if ((crit_ptr->isHiding()) && //if is hiding
             (d(1, pc.LEVEL + 30) < 
              d(1, max(get_percent_lrnd(HIDE_SKILL_NUM, *crit_ptr) * 6,
                       get_percent_lrnd(BLEND_SKILL_NUM, *crit_ptr) * 6)))) {
            continue; //successful hide
         }//if

         if (crit_ptr->pc) { //is a pc

            if (mudlog.ofLevel(DBG)) {
               mudlog << "Doing pc..." << endl;
            }

            Sprintf(buf, "     %S %S %s\n", 
                    name_of_crit(*crit_ptr, pc.SEE_BIT), 
                    &(crit_ptr->short_desc), 
                    critter_positions[crit_ptr->POS]);
            buf.Cap();
            if (crit_ptr->VIS_BIT & 2) {
               buf.setCharAt(1, '*');
            }//if
            
            show(buf, pc);
            crit_ptr->affected_by.head(cell2);
            while ((sp = cell2.next())) {
               if (sp->stat_spell == SANCTUARY_SKILL_NUM) {
                  Sprintf(buf, "\t\t%s glows brightly.\n", 
                          get_he_she(*crit_ptr));
                  buf.Cap();
                  show(buf, pc);
                  break; //cause there are no others to display now
               }//if
            }//while
         }//if
         else if (crit_ptr->isSmob() &&
                  (crit_ptr->POS != mob_list[crit_ptr->getIdNum()].POS)) {
            //is a SMOB with a different posn than corresponding MOB
            if (mudlog.ofLevel(DBG)) {
               mudlog << "Doing SMOB" << endl;
            }

            if (pc.shouldShowVnums()) {
               Sprintf(buf, "     [%i]%P11 %S %s\n", crit_ptr->MOB_NUM,
                       name_of_crit(*crit_ptr, pc.SEE_BIT), 
                       critter_positions[crit_ptr->POS]);
            }
            else {
               Sprintf(buf, "     %S %s\n",
                       name_of_crit(*crit_ptr, pc.SEE_BIT), 
                       critter_positions[crit_ptr->POS]);
            }
            buf.Cap();

            if (crit_ptr->VIS_BIT & 2) {
               buf.setCharAt(1, '*');
            }//if
            show(buf, pc);
            
            crit_ptr->affected_by.head(cell2);
            while ((sp = cell2.next())) {
               if (sp->stat_spell == SANCTUARY_SKILL_NUM) {
                  Sprintf(buf, "\t\t%s glows brightly.\n", 
                          get_he_she(*crit_ptr));
                  buf.Cap();
                  show(buf, pc);
                  break;
               }//if
            }//while
         }//if
         else { //then it is a mob, or SMOB w/same posn as MOB

            if (mudlog.ofLevel(DBG)) {
               mudlog << "Doing MOB" << endl;
            }

            if (pc.shouldShowVnums()) {
               Sprintf(buf, "     [%i]%P11 %S\n", crit_ptr->MOB_NUM,
                       &(crit_ptr->in_room_desc));
            }//if
            else {
               Sprintf(buf, "       %S\n", &(crit_ptr->in_room_desc));
            }

            if (crit_ptr->VIS_BIT & 2) {
               buf.setCharAt(1, '*');
            }//if
            buf.Cap();
            show(buf, pc);
            
            crit_ptr->affected_by.head(cell2);
            while ((sp = cell2.next())) {
               if (sp->stat_spell == SANCTUARY_SKILL_NUM) {
                  Sprintf(buf, "\t\t%s glows brightly.\n", 
                          get_he_she(*crit_ptr));
                  buf.Cap();
                  show(buf, pc);
                  break;
               }//if
            }//while
         }//else
      }//if
   }//while
   if (pc.USING_CLIENT) {
      show("</MOB_LIST>", pc);
   }
   else if (pc.isUsingColor()) {
      pc.show(*(pc.getDefaultColor()));
   }
}//out_crit



void out_inv(const List<object*>& lst, critter& pc, 
	     const short type_of_list) {
			       //outs the names object*
   Cell<object*> cell(lst);
   object*  obj_ptr;
   String buf(100);

   mudlog.log(TRC, "In out_inv.\n");

   if (pc.isUsingClient()) {
      show("<ITEM_LIST>", pc);
   }
   else if (pc.isUsingColor()) {
      pc.show(*(pc.getObjListColor()));
   }
   
   if (IsEmpty(lst) && type_of_list == OBJ_INV) {
      show("	[empty]	\n", pc);
      if (pc.isUsingClient()) {
         show("</ITEM_LIST>", pc);
      }
      else if (pc.isUsingColor()) {
         pc.show(*(pc.getDefaultColor()));
      }

      mudlog.log(DBG, "Done with out_inv (empty).\n");
      return;
   }//if

   switch (type_of_list)
      {
      case ROOM_INV:
         while ((obj_ptr = cell.next())) {

	    if (detect(pc.SEE_BIT, obj_ptr->OBJ_VIS_BIT)) {
               if (pc.shouldShowVnums()) {
                  Sprintf(buf, "     [%i] %P11 %S", obj_ptr->OBJ_NUM,
                          &(obj_ptr->in_room_desc));
               }
               else {
                  Sprintf(buf, "\t%S", &(obj_ptr->in_room_desc));
               }
               buf.Cap();

               if (obj_ptr->isHerb()) {
                  if (d(1, 100) <= 
                      d(1, 2 * get_percent_lrnd(HERBALISM_SKILL_NUM, pc))) {
                     buf.Append("(herb)");
                  }//if
               }//if

               if (pc.canDetectMagic() &&
                   (!IsEmpty(obj_ptr->affected_by) ||
                    !IsEmpty(obj_ptr->stat_affects))) {
                  buf.Append(" {Blue Glow}\n");
               }//if
               else {
                  buf.Append("\n");
               }

               if (obj_ptr->OBJ_VIS_BIT & 2) {
                  buf.Prepend("*");
               }//if

               show(buf, pc);
            }//if
	 }//while
         break;
      case OBJ_INV: case CRIT_INV:
         while ((obj_ptr = cell.next())) {
	    if (detect(pc.SEE_BIT, obj_ptr->OBJ_VIS_BIT)) {
               if (pc.shouldShowVnums()) {
                  Sprintf(buf, "     [%i]%P11 %S", obj_ptr->OBJ_NUM,
                          long_name_of_obj(*obj_ptr, ~0));
               }
               else {
                  Sprintf(buf, "\t%S", long_name_of_obj(*obj_ptr, ~0));
               }

               buf.Cap();

               if (obj_ptr->isHerb()) {
                  if (d(1, 100) <= 
                      d(1, 2 * get_percent_lrnd(HERBALISM_SKILL_NUM, pc))) {
                     buf.Append("(herb)");
                  }//if
               }//if

               if  (pc.canDetectMagic() &&
                    (!IsEmpty(obj_ptr->affected_by) ||
                     !IsEmpty(obj_ptr->stat_affects))) {
                  buf.Append(" {Blue Glow}\n");
               }//if
               else {
                  buf.Append("\n");
               }

               if (obj_ptr->OBJ_VIS_BIT & 2) {
                  buf.Prepend("*");
               }//if
               show(buf, pc);
            }//if
	 }//while
         break;
      default:
	 mudlog.log(ERR, "ERROR:  default called in out_inv.\n");
         break;
      }//switch type_of_list

   if (pc.isUsingClient()) {
      show("</ITEM_LIST>", pc);
   }
   else if (pc.isUsingColor()) {
      pc.show(*(pc.getDefaultColor()));
   }

   mudlog.log(DBG, "Done with out_inv.\n");

}//out_inv



critter* have_crit_named(List<critter*>& lst, const int i_th,
	 const String* name, const int see_bit, const room& rm,
	 int do_exact = FALSE) {
   Cell<String*> char_cell;
   Cell<critter*> cell(lst);
   critter* crit_ptr;
   int count = 0, ptr_v_bit, len;
   String *string;

   //log("In have_crit_named:");

   if (!name) {
      mudlog.log(ERR, "ERROR:  NULL name sent to have_crit_named.\n");
      return NULL;
   }//if

   if ((len = name->Strlen()) == 0) 
      return NULL;

   if (i_th <= 0)
      return NULL;

   while ((crit_ptr = cell.next())) {
      ptr_v_bit = (crit_ptr->VIS_BIT | rm.getVisBit()); 
      if (detect(see_bit, ptr_v_bit)) {
	 crit_ptr->names.head(char_cell);
	 while ((string = char_cell.next())) {
	    if (do_exact) {
   	       if (strcasecmp(*string, *name) == 0){ 
	          count++;
	       }//if
	    }//if exact
	    else {
   	       if (strncasecmp(*string, *name, len) == 0){ 
	          count++;
	       }//if
	    }//else
	    if (count == i_th) {
	       return crit_ptr;
	    }//if
	 }//while
      }//if
   }//while
   return NULL;
}//have_crit_named 


int crit_sub_a_4_b(critter* a, List<critter*>& lst, 
                   const int i_th, const String* name,
                   const int see_bit, room& rm) {
   Cell<String*> char_cell;
   Cell<critter*> cell(lst);
   critter* crit_ptr;
   int count = 0, ptr_v_bit;
   String *string;

   //log("In crit_sub_a_4_b.\n");

   if (!name || !a) {
      mudlog.log(ERR, "ERROR:  NULL(s) sent to crit_sub_a_4_b.\n");
      return FALSE;
   }//if

   if (name->Strlen() == 0) {
      mudlog.log(ERR, "ERROR:  name of zero length sent to crit_sub_a_4_b.\n"); 
      return FALSE;
   }//if

   while ((crit_ptr = cell.next())) {
      ptr_v_bit = (crit_ptr->VIS_BIT | rm.getVisBit()); 
      if (detect(see_bit, ptr_v_bit)) {
	 crit_ptr->names.head(char_cell);
	 while ((string = char_cell.next())) {
	    if (strncasecmp(*string, *name, name->Strlen()) == 0){ 
	       count++;
	       if (count == i_th) {  //found right one
                  lst.assign(cell, a);
		  return TRUE;
	       }//if
	    }//if
	 }//while
      }//if
   }//while
   return FALSE;
}//crit_sub_a_4_b 


int obj_sub_a_4_b(object* a, List<object*>& lst, const int i_th,
                  const String* name, const int see_bit, room& rm) {
   Cell<String*> char_cell;
   Cell<object*> cell(lst);
   object* obj_ptr;
   int count = 0, ptr_v_bit;
   String *string;

   //log("In obj_sub_a_4_b.\n");

   if (!name || !a) {
      mudlog.log(ERR, "ERROR:  NULL(s) sent to obj_sub_a_4_b.\n");
      return FALSE;
   }//if

   if (name->Strlen() == 0) {
      mudlog.log(ERR, "ERROR:  name of zero length sent to obj_sub_a_4_b.\n"); 
      return FALSE;
   }//if

   while ((obj_ptr = cell.next())) {
      ptr_v_bit = (obj_ptr->OBJ_VIS_BIT | rm.getVisBit()); 
      if (detect(see_bit, ptr_v_bit)) {
         obj_ptr->names.head(char_cell);
	 while ((string = char_cell.next())) {
	    if (strncasecmp(*string, *name, name->Strlen()) == 0){ 
	       count++;
	       if (count == i_th) {  //found right one
                  //obj_ptr = Prev(cell);  //back cell up one
                  lst.assign(cell, a);
		  return TRUE;
	       }//if
	    }//if
	 }//while
      }//if
   }//while
   return FALSE;
}//obj_sub_a_4_b 


object* have_obj_named(const List<object*>& lst, const int i_th,
	const String* name, const int see_bit, const room& rm) {
   Cell<String*> char_cell;
   Cell<object*> cell(lst);
   object* obj_ptr;
   int count = 0, ptr_v_bit;
   String *string;

   //log("In have_obj_named:");

   if (!name) {
      mudlog.log(ERR, "ERROR:  Null sent to have_obj_named.k\n");
      return NULL;
   }//if

   if (name->Strlen() == 0) 
      return NULL;

   while ((obj_ptr = cell.next())) {
      ptr_v_bit = (obj_ptr->OBJ_VIS_BIT | rm.getVisBit());
      if (detect(see_bit, ptr_v_bit)) {
	 obj_ptr->names.head(char_cell);
	 while ((string = char_cell.next())) {
	    if (strncasecmp(*string, *name, name->Strlen()) == 0){ 
	       count++;
	       if (count == i_th) {
		  return obj_ptr;
	       }//if
	    }//if
	 }//while
      }//if
   }//while
   return NULL;
}//have_obj_named 


int  obj_is_named(const object& obj, const String& name) {
   Cell<String*> char_cell(obj.names);
   String *string;
   int len;

   //log("In obj_is_named\n");

   if ((len = name.Strlen()) == 0)
      return FALSE;

   while ((string = char_cell.next())) {
      if (strncasecmp(*string, name, len) == 0) {
	 return TRUE;
      }//if
   }//while
   return FALSE;
}//obj_is_named 


int  door_is_named(const door_data& dr, const String& name) {
   Cell<String*> char_cell(dr.names);
   String *string;
   int len;

   //log("In obj_is_named\n");

   if ((len = name.Strlen()) == 0)
      return FALSE;

   while ((string = char_cell.next())) {
      if (strncasecmp(*string, name, len) == 0) {
	 return TRUE;
      }//if
   }//while
   return FALSE;
}//door_is_named


//boolean functionality here...
int  mob_is_named(const critter& pc, const String& name) {
   return pc.isNamed(name);
}


const String* name_of_crit(critter& pc, int see_bit) {
   return pc.getName(see_bit);
}


String* name_of_obj(const object& obj, int see_bit) {
   if (!detect(see_bit, obj.OBJ_VIS_BIT)) {
      return &SOMETHING;
   }//if
   else {
      String* tmp = Top(obj.names);
      if (tmp)
         return tmp;
      else
         return &SOMETHING;
   }//else
}//name_of_obj


String* long_name_of_obj(object& obj, int see_bit) {
   if (obj.short_desc.Strlen() == 0 ||
	!detect(see_bit, obj.OBJ_VIS_BIT)) {
      return &SOMETHING;
   }//if
   else {
      return &(obj.short_desc);
   }//else
}//long name_of_obj

String* name_of_room(const room& rm, int see_bit) {
   if ((IsEmpty(rm.names)) || !detect(see_bit, rm.getVisBit())) {
      return &SOMEWHERE;
   }//if
   else {
      return Top(rm.names);
   }//else
}//name_of_room


String* name_of_door(const door& dr, int see_bit) {
   return name_of_dr_data(*(dr.dr_data), see_bit, dr.destination);
}//name_of_door

String* name_of_dr_data(const door_data& dr, int see_bit, int dest) {
   Cell<String*> cell(dr.names);
   String *str, *str2;

   if (!detect(see_bit, dr.vis_bit)) {
      return &SOMETHING;
   }//if

   if (IsEmpty(dr.names))
      return &UNKNOWN;

   if (dest >= 0) {
      str = cell.next();
      str2 = cell.next();

      if (str2)
	 if (*str2 != "#")
	    return str2;
	 else
	    return str;
      else  // no specific name, just the direction
	 return str;
    }//if
    else {
      str = cell.prev();
      str2 = cell.prev();

      if (str2)
	 if (*(str2) != "#")
	    return str2;
	 else
	    return str;
      else  // no specific name, just the direction
	 return str;
   }//else
}//name_of_dr_data
       

// Will be sent an IP with an optional N on the beginning,
// all, not just newbies
short is_banned(const String& ip) {
   Cell<String*> cll(banned_hosts);
   String* ptr;

   while ((ptr = cll.next())) {
      if (toupper(ptr->charAt(0)) == 'N') {
         continue; //not interested in newbie bans here.
      }//if
      else {
         if (strncmp(ip, *(ptr), max(ip.Strlen(), ptr->Strlen())) == 0)
            return TRUE;
      }
   }//while
   return FALSE;
}//is_banned


// Will be sent an IP with an optional N on the beginning,
// all, not just newbies
short is_newbie_banned(const String& ip) {
   Cell<String*> cll(banned_hosts);
   String* ptr;

   while ((ptr = cll.next())) {
      if (toupper(ptr->charAt(0)) == 'N') {
         const char* str = *ptr; //convert to const char*
         if (strncmp(ip, str + 1, max(ip.Strlen(), ptr->Strlen() - 1)) == 0)
            return TRUE;
      }//if
   }//while
   return FALSE;
}//is_newbie_banned


int detect(int see_bit, int vis_bit) {
   return ((see_bit & vis_bit) == vis_bit);
}//detect
