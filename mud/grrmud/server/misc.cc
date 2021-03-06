// $Id$
// $Revision$  $Author$ $Date$

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
//To contact the maintainer, Edward Roper: edro+scrymud [at] wanfear.net
//

#include "command4.h"
#include <sys/time.h>
#include <time.h>
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
#include "Filters.h"
#include <math.h>
#include <mt19937ar.h>


/** calculate the ratio between objects casting spell a spell and the
 * number of players online.  It will be fudged so that 1 will be
 * considered a 'fair' amount.  If > 1.0 then the spell should probably
 * be weakened somewhat.
 */
float spell_objs_ratio(int spell_num) {
   // Take potions & scrolls into affect, all 2 per person.
   float cnt = SSCollection::instance().getSS(spell_num).getSpellCastingCount();
   if (cnt == 0)
      cnt = 1.0;

   float pcnt = (((float)(MudStats::instance().getPlayerCount()) 
                 * 1.5 + 10.0) / cnt);
   if (pcnt < 1.0)
      pcnt = 1.0;
   return pcnt;
}//spell_objs_ratio


const char* cstr(CSentryE e, critter& c) {
   return CSHandler::getString(e, c.getLanguageChoice());
}


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
      mudlog.log(LS_ERROR, "ERROR:  non PC sent to critter::save()\n");
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
      mudlog.log(LS_ERROR, "ERROR:  file not opened in save_pc.\n");
      return;
   }//if

   int i;
   for(i = 0; i<MAX_EQ; i++) {
       if (EQ[i]) {
           remove_eq_effects(*(EQ[i]), *this, FALSE, FALSE, i);
       }
   }

   Write(ofile);

   for(i=0;i<MAX_EQ;i++) {
       if (EQ[i]) {
           wear_eq_effects(*(EQ[i]), *this, i, FALSE);
       }
   }
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
            pc.show("The number is too low.\n");
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
            pc.show("The number is too high.\n");
         return FALSE;
      }//if
      return FALSE;
   }//else
   return TRUE;
}//check_l_range()


const char* get_his_her(const critter& crit) {

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

const char* get_hisself_herself(const critter& crit) { //this is
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


const char* get_he_she(const critter& crit) {

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

/* Initialize the random number generator */
void init_rand() {
    struct timeval tv;
    if ( gettimeofday(&tv, NULL) != 0 ) {
        cerr << "gettimeofday() failed in init_rand()." << endl;
        cerr << "Seeding with 1. This is evil." << endl;
        init_genrand(1);
    } else {
        init_genrand(tv.tv_usec);
        srand(tv.tv_usec); //currently rand() is still used in a few places.
    }
}//init_rand()

int d(const int num_rolls, const int dice_sides) {
   int rtvalue = 0;
   double tmp;
   for (int i = 0; i<num_rolls; i++) {
      // some places pass critter::get_percent_lrnd() here
      if ( dice_sides == 0 ) {
          return(0);
      }
      tmp = genrand_real1();
      rtvalue += 1 + floor( dice_sides * tmp );
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

   if (mudlog.ofLevel(DBG)) {
      mudlog << "join_in_battle: agg: " << &agg << " vict: " << &vict << endl;
   }

   if (agg.isMob()) {
      mudlog.log(LS_ERROR, "ERROR: join_in_battle called with agg as mob\n");
      return;
   }//if
   if (vict.isMob()) {
      mudlog.log(LS_ERROR, "ERROR: join_in_battle called with vict as mob\n");
      return;
   }//if
   
   if (&vict == &agg) {
      if (mudlog.ofLevel(LS_ERROR)) {
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

   agg.IS_FIGHTING.append(&vict);
   vict.IS_FIGHTING.append(&agg);

   //Some mobs will remember....
   if (vict.isNpc()) {
      critter* virt_attacker = NULL;
      if (agg.isPc()) {
         virt_attacker = &agg;
      }
      else {
         virt_attacker = agg.getFollowerOf();
      }

      if (virt_attacker) {
         if ((((vict.getLevel() > 20) && vict.mob->getBadAssedness() > 0) ||
              ((vict.mob->getBadAssedness() > 3))) ||
             (((vict.getLevel() > 20) && vict.getBenevolence() <= 0) ||
              ((vict.getBenevolence() < 0)))) {
            vict.remember(*virt_attacker);
            do_tell(vict, "I'll remember that!!", *virt_attacker, FALSE,
                    virt_attacker->getCurRoomNum());
         }//if
         if ((vict.getBenevolence() <= -4) ||
             (vict.mob->getBadAssedness() >= 5)) {
            vict.setTrackingTarget(*(virt_attacker->getShortName()));
         }
      }//if we found a PC responsible
   }//if

   agg.PAUSE = 1; //vict should get at least one hit in
}//join_in_battle


void do_tick() {
   //gossip("An hour has passed in our fine land.", mob_list[2]);

                   /* do all decrementing */
   decrease_timed_affecting_doors(); //takes care of affected_dors list
   decrease_timed_affecting_lds();  //takes care of alllink deads
   decrease_timed_affecting_pcs();  //takes care of all pc's
   decrease_timed_affecting_smobs(); //takes care of affected_mobs list
   decrease_timed_affecting_objects(); //takes care of affected_objects list
   decrease_timed_affecting_rooms(); //takes care of affected_rooms

                   /* do all incrementing */
   //do_regeneration_pcs();  //takes care of all pc's
   //do_regeneration_smobs();  //takes care of affected_mobs list
   do_regeneration_objects();  //takes care of affected_objects list
   ZoneCollection::instance().doRegeneration(); //update mobs, objs, rooms if needed

                  /* time */

   config.hour++;
   if (config.hour >= 24) {
      config.hour = 0;
      config.day++;
   }//if
   if (config.day >= 365) {
      config.year++;
      config.day = 1;
   }//if
   weather.update();
   config.writeDynamic("dynamic.cfg");
}//do_tick


void do_mini_tick() { // decrement pause count ect
   Cell<critter*> cll;
   pc_list.head(cll);
   critter* crit_ptr;
   Cell<stat_spell_cell*> scell;
   stat_spell_cell* sc_ptr;

   do_vehicle_moves();  //in misc2.cc

   while ((crit_ptr = cll.next())) {

      crit_ptr->spam_cnt = 0;

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

      // Eliminate 15 minutes of real-life battle stun.
      if (crit_ptr->PAUSE > 4) {
         crit_ptr->PAUSE = 4;
      }
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

      crit_ptr->spam_cnt = 0;

      // Turn un-blocked, if not in battle.
      if (!crit_ptr->isInBattle()) {
         crit_ptr->crit_flags.turn_off(21);
      }

      if (crit_ptr->PAUSE > 0)
         crit_ptr->PAUSE--;
      // Eliminate 15 minutes of real-life battle stun.
      if (crit_ptr->PAUSE > 4)
         crit_ptr->PAUSE = 4;

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


//The /6.0)+1.0) is because I'm having regen occur in about 1/6th the amount
//of time that it used to. The +1.0 is just so we don't accidently have people
//that never regen. I'll be watching the effects of this change closely and
//will most likely adjust the algorithms based on what I discover.
void do_regeneration_pcs() {
   float adj = 1.0, posn_mod, align_mod = 1.0, weather_mod = 1.0, temperature_mod = 1.0, env_mod = 1.0;
   Cell<critter*> crit_cell;
   pc_list.head(crit_cell);
   critter* crit_ptr;

   //log("In do_regeneration_pcs\n");

   while ((crit_ptr = crit_cell.next())) {

          if(crit_ptr->getCurRoom()->getWeather()!= wNONE){
            weather_mod = weather_regen_mods[crit_ptr->getCurRoom()->getWeather()];
                        temperature_mod = temperature_regen_mods[crit_ptr->getCurRoom()->getTemperature()];
      }
      //no regen if we're fighting
      if ( crit_ptr->isFighting() ) {
         continue;
      }

      if ((crit_ptr->THIRST == 0) || (crit_ptr->HUNGER == 0)) {
         adj = 0.2; // big penalty for being hungry
      }
      else {
         adj = 1.0;
      }

      if (crit_ptr->POS == POS_PRONE) {
         posn_mod = 1.0;
      }
      else {
         if ((crit_ptr->POS > POS_SIT) &&
             room_list[crit_ptr->getCurRoomNum()].canCamp()) {
                                 float camp = get_percent_lrnd(CAMPING_SKILL_NUM, *crit_ptr) / 200;
                                 env_mod *= (temperature_mod+((camp*(1.0 - temperature_mod))/2));
                                 env_mod *= (weather_mod+(camp*(1.0 - weather_mod)));
         }//if camping is an issue

         posn_mod = (2.0 + (float)(crit_ptr->POS)) / 4.0;
      }

      
      // if we are affected by remove hope we get no hp
      if ( ! (is_affected_by(REMOVE_HOPE_SKILL_NUM, *crit_ptr)) ) {
      crit_ptr->HP += (int)((((((float)(crit_ptr->CON) + 5.0) / 15.0) * 
                            (((float)(crit_ptr->getHP_MAX())) / 9.0) * 
                            posn_mod * (((float)(crit_ptr->getHealthRegen(true))) / 100.0)
                            * adj + 10.0)/6.0)*env_mod+1.0);
      }

      // if we are affected by remove karma we get no mana
      if ( ! (is_affected_by(REMOVE_KARMA_SKILL_NUM, *crit_ptr)) ) {

         // Adjust regeneration for necromancers.
         if(crit_ptr->isNecromancer()){
            // Break even point: -220 at level 1, -800 at level 30.
            int breakeven = 200 + (20 * crit_ptr->LEVEL);
            // Regeneration can be up to 10x the normal rate.
            align_mod = 1/pow(10.0f, (breakeven+crit_ptr->ALIGN)/(1000-breakeven)+1);
         }
      
         crit_ptr->MANA += (int)(((((((float)(crit_ptr->INT)) + 5.0) / 16.0) *
                              posn_mod *
                              (((float)(crit_ptr->getManaMax())) / 7.0) *
                              (((float)(crit_ptr->getManaRegen(true))) / 100.0) *
                              align_mod *
                              adj + 4.0)/6.0)*env_mod+1.0);
      }

      /* Lose 5 mov points if you're idleing in deep water without a boat or
         flying. Regen none. */
      if  ( crit_ptr->getCurRoom()->isBigWater() && 
            ( ! ( crit_ptr->isFlying() || crit_ptr->hasBoat() ) ) ) {
         crit_ptr->MOV -= 5;
         // player should die if mov hits 0 
      } else {
         int tmp_mov;
         tmp_mov = (int)(((((((float)(crit_ptr->DEX)) + 5.0) / 16.0) *
                     posn_mod * adj *
                     (((float)(crit_ptr->getMovMax())) / 3.0) * 
                     (((float)(crit_ptr->getMovRegen(true))) / 100.0) + 3.0)/6.0)*env_mod+1.0);

         crit_ptr->MOV += tmp_mov;
      }

      //if (tmp_mov < 0) {
      //   if (mudlog.ofLevel(DBG)) {
      //      mudlog << "WARNING:  mov regen < 0 for PC:  "
      //             <<  *(name_of_crit(*crit_ptr, ~0)) << "  it was:  "
      //             << tmp_mov << endl;
      //   }
      //}

      if ( (crit_ptr->getHP() > crit_ptr->getHP_MAX()) ) {
          crit_ptr->setHP(crit_ptr->getHP_MAX());
      }
      if (crit_ptr->getMana() > crit_ptr->getManaMax())
          crit_ptr->setMana(crit_ptr->getManaMax());
      if (crit_ptr->getMov() > crit_ptr->getMovMax())
          crit_ptr->setMov(crit_ptr->getMovMax());
      if (crit_ptr->getHP() < 0)
          crit_ptr->setHP(0);
      if (crit_ptr->getMana() < 0)
          crit_ptr->setMana(0);
      if (crit_ptr->getMov() < 0)
          crit_ptr->setMov(0);

      if ((crit_ptr->getHP() > 1) && (crit_ptr->isStunned())) {
         wake(*crit_ptr);
         stand(*crit_ptr);
      }

      if ((((float)(crit_ptr->getHP())) / (float)(crit_ptr->getHP_MAX())) > 0.80) {
         stat_spell_cell* ssptr = 
            is_affected_by(BIND_WOUND_SKILL_NUM, *crit_ptr);
         if (ssptr) {
            rem_effects_crit(BIND_WOUND_SKILL_NUM, *crit_ptr, TRUE);
            crit_ptr->affected_by.loseData(ssptr);
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

      //no regen if we're fighting
      if ( crit_ptr->isFighting() ) {
         continue;
      }
      posn_mod = (2.0 + crit_ptr->POS) / 4.0;

      // if we are affected by remove hope we get no hp
      if ( ! (is_affected_by(REMOVE_HOPE_SKILL_NUM, *crit_ptr)) ) {
      crit_ptr->HP +=
         (int)(((((crit_ptr->CON + 5.0) / 15.0) * 
               (crit_ptr->getHP_MAX() / 9.0) * 
               posn_mod * (crit_ptr->getHealthRegen(true) / 100.0) * adj + 10.0)/6.0)+1.0);
      }
      
      // if we are affected by remove hope we get no mana
      if ( ! (is_affected_by(REMOVE_KARMA_SKILL_NUM, *crit_ptr)) ) {
      crit_ptr->MANA +=
         (int)(((((crit_ptr->INT + 5.0) / 16.0)  * posn_mod *
               (crit_ptr->getManaMax() / 7.0) * (crit_ptr->getManaRegen(true) / 100.0) * adj +
               4.0)/6.0)+1.0);
      }

      crit_ptr->MOV += 
         (int)(((((crit_ptr->DEX + 5.0) / 16.0) * posn_mod * 
               adj *
               (crit_ptr->getMovMax() / 2.0) * (crit_ptr->getMovRegen(true) / 100.0) +
               5.0)/6.0)+1.0);
      
      if (crit_ptr->getHP() > crit_ptr->getHP_MAX())
          crit_ptr->setHP(crit_ptr->getHP_MAX());
      if (crit_ptr->getMana() > crit_ptr->getManaMax())
          crit_ptr->setMana(crit_ptr->getManaMax());
      if (crit_ptr->getMov() > crit_ptr->getMovMax())
          crit_ptr->setMov(crit_ptr->getMovMax());

      if (((float)crit_ptr->getHP() / (float)crit_ptr->getHP_MAX()) > 0.80) {
         stat_spell_cell* ssptr = 
            is_affected_by(BIND_WOUND_SKILL_NUM, *crit_ptr);
         if (ssptr) {
            rem_effects_crit(BIND_WOUND_SKILL_NUM, *crit_ptr, TRUE);
            crit_ptr->affected_by.loseData(ssptr);
         }
      }//if
   }//while
   
}//do_regeneration_smobs


void do_regeneration_objects() {
  return;
}//do_regeneration_objects


int update_critters(int zone_num, short read_all) {
   switch (config.useMySQL) {
#ifdef USEMYSQL
      case true:
         return db_update_critters(zone_num, read_all);
         break;
#endif
      case false:
         return file_update_critters(zone_num, read_all);
         break;
   }
   return -1;
}

#ifdef USEMYSQL
int db_update_critters(int zone_num, short read_all) {
   critter tmp_mob;
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select * from Critters where FROM_ZONE=";
   query+=zone_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In db_update_critters(int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return -1;
      }
      while ((row=mysql_fetch_row(result))) {
         int k = atoi(row[CRITTBL_MOB_NUMBER]);
         tmp_mob.dbRead(k, 0, read_all);
         if (!(&mob_list[k])) {
            if (mudlog.ofLevel(WRN)) {
               mudlog << "WARNING: mob[" << k << "] is NULL.\n";
            }
         }
         else {
            if (mob_list[k].MOB_FLAGS.get(4)) {
               Cell<object*> ocell;
               object* obj_ptr;
               tmp_mob.inv.head(ocell);
               while ((obj_ptr = ocell.next())) {
                  if ((obj_count(tmp_mob.inv, *obj_ptr) >
                      obj_count(mob_list[k].inv, *obj_ptr))
                      && (obj_ptr->getCurInGame() < obj_ptr->getMaxInGame())) {
                     mob_list[k].inv.append(&obj_list[obj_ptr->getIdNum()]);
                     recursive_init_loads(obj_list[obj_ptr->getIdNum()], 0);
                  }
               }
               for (int i = 1; i<MAX_EQ; i++) {
                  if ((tmp_mob.EQ[i] && !mob_list[k].EQ[i])
                      && (tmp_mob.EQ[i]->getCurInGame()
                          < tmp_mob.EQ[i]->getMaxInGame())) {
                     mob_list[k].EQ[i] = &obj_list[tmp_mob.EQ[i]->getIdNum()];
                     recursive_init_loads(*(mob_list[k].EQ[i]), 0);
                  }
               }
            }
         }
      }
      tmp_mob.CRITTER_TYPE = 1;
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In db_update_critters(int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return -1;
   }
   return 0;
}
#endif

int file_update_critters(int zone_num, short read_all) {
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
      mudlog.log(LS_ERROR, "WARNING:  created CRITTER_FILE in update_critters");
      tmp_mob.CRITTER_TYPE = 1; //make it a SMOB, so destructor fires correctly
      return 0;
   }//if

   mobfile >> k;
   mobfile.getline(temp_str, 80); //junk line
   while (k != -1) {

      Sprintf(buf, "About to read mob:  %i, zone %i.", k, zone_num);
      mudlog.log(DBG, buf);

      if (!mobfile) {
         if (mudlog.ofLevel(LS_ERROR)) {
            mudlog << "ERROR: error reading critters for zone: " << zone_num
                   << " trying to read mob# " << k << endl;
         }//if
         return -1;
      }//if

      tmp_mob.fileRead(mobfile, read_all);

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
                  mob_list[k].inv.append(&(obj_list[obj_ptr->getIdNum()]));
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
   return 0;
}//update_critters

int update_objects(int zone_num, short read_all) {
   #ifdef USEMYSQL
   if (config.useMySQL)
      return db_update_objects(zone_num, read_all);
   else
   #endif
      return file_update_objects(zone_num, read_all);
}

#ifdef USEMYSQL
int db_update_objects(int zone_num, short read_all) {
   Cell<object *> ocell;
   int k;
   object temp_obj;
   object* obj_ptr;
   MYSQL_RES* result;
   MYSQL_ROW row;
   String query = "select distinct OBJ_NUM from ";
   query += "RoomInv inner join Rooms on RoomInv.ROOM_NUM=Rooms.ROOM_NUM";
   query += "where IN_ZONE=";
   query += zone_num;

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In db_update_objects(int, short):\n";
            mudlog << "Eror retrieving query results: " << mysql_error(database)
                   << endl;
         }
         return -1;
      }
      while ((row=mysql_fetch_row(result))) {
         k = atol(row[0]);
         temp_obj.dbRead(k, -1, read_all);

         if (obj_list[k].OBJ_FLAGS.get(OBJFLAG_NEEDS_RESETTING)) {
            temp_obj.inv.head(ocell);
            while ((obj_ptr = ocell.next())) {
               if ((obj_count(temp_obj.inv, *obj_ptr) >
                    obj_count(obj_list[k].inv, *obj_ptr)) &&
                   (obj_ptr->getCurInGame() < obj_ptr->getMaxInGame())) {
                  obj_list[k].inv.append(&(obj_list[obj_ptr->getIdNum()]));
                  recursive_init_loads(obj_list[obj_ptr->getIdNum()], 0);
               }
            }
         }
      }
      mysql_free_result(result);
      temp_obj.IN_LIST = room_list[0].getInv();
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In db_update_objects(int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return -1;
   }
   return 0;
}
#endif

int file_update_objects(int zone_num, short read_all) {
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
      mudlog.log(LS_ERROR, "WARNING:  created new OBJECT_FILE in update_objects");
      temp_obj.IN_LIST = room_list[0].getInv(); //hack to let destructor fire
                                                //happily, turn it into a SOBJ
      return 0;
   }//if

   objfile >> k;
   objfile.getline(temp_str, 80); //junk line
   while (k != -1) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "About to read obj:  " << k
                << " zone:  " << zone_num << endl;
      }

      temp_obj.fileRead(objfile, read_all);

      if (!objfile) {
         if (mudlog.ofLevel(LS_ERROR)) {
            mudlog << "ERROR: error reading objects for zone: " << zone_num
                   << " trying to read obj# " << k << endl;
         }//if
         return -1;
      }//if

      if (obj_list[k].OBJ_FLAGS.get(70)) {

         //log("About to check if need to load more inv.\n");
         temp_obj.inv.head(ocell);
         while ((obj_ptr = ocell.next())) {
            if ((obj_count(temp_obj.inv, *obj_ptr) >
                 obj_count(obj_list[k].inv, *obj_ptr))
                && (obj_ptr->getCurInGame() < obj_ptr->getMaxInGame())) {
               obj_list[k].inv.append(&(obj_list[obj_ptr->getIdNum()]));
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
   return 0;
}//update_objects

int update_zone(int zone_num, short read_all) {
   switch (config.useMySQL) {
#ifdef USEMYSQL
      case true:
         return db_update_zone(zone_num, read_all);
#endif
      case false:
         return file_update_zone(zone_num, read_all);
   }
   return -1;
}

#ifdef USEMYSQL
int db_update_zone(int zone_num, short read_all) {
   int k;
   room tmp_room;
   vehicle tmp_veh;
   object* obj_ptr;
   Cell<object*> ocell;
   critter* crit_ptr;
   Cell<critter*> ccell;

   MYSQL_RES* result;
   MYSQL_ROW row;
   String query="select ROOM_NUM from Rooms where IN_ZONE=";
   query+=zone_num;

   if (ZoneCollection::instance().elementAt(zone_num).isLocked() &&
         (read_all == FALSE)) {
      return 0;
   }

   if (mysql_real_query(database, query, strlen(query))==0) {
      if ((result=mysql_store_result(database))==NULL) {
         if (mudlog.ofLevel(WRN)) {
            mudlog << "In db_update_zone(int, short):\n";
            mudlog << "Error retrieving query results: "
                   << mysql_error(database) << endl;
         }
         return -1;
      }
      while ((row=mysql_fetch_row(result))) {
         room* room_ptr;
         k=atoi(row[0]);
         if (k & 0x01000000) {
            tmp_veh.dbRead(k, read_all);
            room_ptr = &tmp_veh;
            k = (k & ~(0x01000000));
         }
         else {
            tmp_room.dbRead(k, read_all);
            room_ptr=&tmp_room;
         }

         if (read_all) {
            room_list[k].setTotalLoaded(TRUE);
         }
         room_ptr->getInv()->head(ocell);
         while ((obj_ptr = ocell.next())) {
            if ((obj_count(*(room_ptr->getInv()), *obj_ptr) >
                 obj_count(*(room_list[k].getInv()), *obj_ptr)) &&
                (obj_ptr->getCurInGame() <
                 obj_list[obj_ptr->getIdNum()].getMaxInGame())) {
               
               if (obj_ptr->isPlayerOwned()) {
                  object* pobox = load_player_box(obj_ptr->getIdNum());
                  if (pobox) {
                     pobox->IN_LIST = room_list[k].getInv();
                     pobox->setCurRoomNum(k, 0);
                     affected_objects.gainData(pobox);
                     room_list[k].gainObject(pobox);
                  }
                  else {
                     room_list[k].gainObject(&(obj_list[obj_ptr->getIdNum()]));
                  }
               }
               else {
                  room_list[k].gainObject(&(obj_list[obj_ptr->getIdNum()]));
               }
               recursive_init_loads(obj_list[obj_ptr->getIdNum()], 0);
            }
         }

         List<critter*> tmp_lst(room_ptr->getCrits());
         tmp_lst.head(ccell);
         int tmp_cc;
         int ingame_cc;
         while ((crit_ptr = ccell.next())) {
            tmp_cc = room_ptr->getCritCount(*crit_ptr);
            ingame_cc = room_list[k].getCritCount(*crit_ptr);
            if ((tmp_cc > ingame_cc) && ((crit_ptr->getCurInGame() - tmp_cc) <
                                         crit_ptr->getMaxInGame())) {

               if (crit_ptr->isPlayerShopKeeper()) {
                  critter* shop_keeper =
                     load_player_shop_owner(crit_ptr->getIdNum());
                  if (shop_keeper) {
                     shop_keeper->setCurRoomNum(k);
                     room_list[k].gainCritter(shop_keeper);
                  }
               }
               else {
                  room_list[k].gainCritter(&(mob_list[crit_ptr->getIdNum()]));
               }
               recursive_init_loads(mob_list[crit_ptr->getIdNum()]);
            }
         }
      }
      tmp_room.Clear();
      tmp_veh.Clear();
      mysql_free_result(result);
   }
   else {
      if (mudlog.ofLevel(WRN)) {
         mudlog << "In db_update_zone(int, short):\n";
         mudlog << "Error executing query: " << mysql_error(database) << endl;
      }
      return -1;
   }
   return 0;
}
#endif

int file_update_zone(int zone_num, short read_all) {
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
      return 0; //don't do anything
   }//if
      
   Sprintf(buf, "./World/zone_%i", zone_num);
   ifstream rfile(buf);
   if (!rfile) { 
      buf.Prepend("cp ./World/DEFAULT_ZONE ");
      system(buf);
      mudlog.log(LS_ERROR, "WARNING: zone file created in update_zone.");
      mudlog.log(LS_ERROR, buf);
      return 0;
   }//if

   rfile >> k;
   rfile.getline(temp_str, 80); //junk line
   while (k != -1) {
      room* room_ptr;

      if (!rfile) {
         if (mudlog.ofLevel(LS_ERROR)) {
            mudlog << "ERROR: error reading rooms for zone: " << zone_num
                   << " trying to read room# " << k << endl;
         }//if
         return -1;
      }//if

      if (k & 0x01000000) { //it's a vehicle
         k = (k & ~(0x01000000));

         if (mudlog.ofLevel(DBG)) {
            mudlog << "About to read vehicle:  " << k
                << " zone:  " << zone_num << endl;
         }
         tmp_veh.fileRead(rfile, read_all);
         room_ptr = &tmp_veh;
      }
      else {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "About to read room:  " << k
                   << " zone:  " << zone_num << endl;
         }
         tmp_room.fileRead(rfile, read_all);
         room_ptr = &tmp_room;
      }

      if (read_all) {
         room_list[k].setTotalLoaded(TRUE); //total_loaded flag
      }//if

      //log("About to check if need to load more inv into the room.\n");
      room_ptr->getInv()->head(ocell);
      while ((obj_ptr = ocell.next())) {

         //make sure containers have their initial inventory reload if
         //necessary.
         //
         // Do _NOT_ regenerate the contents of player owned containers. In
         // particular, these containers are nasty as their database inventory
         // changes as items are inserted/removed from them. These containers
         // wouldn't even regenerate properly as removing an item from them
         // effectively removes it from their db inventory, so they'll never
         // regenerate.
         //
         // Now for the terribly nasty bit. Due to the current handling for
         // dissolving objects (corpses) via. the "in_list" pointer, and not
         // a pointer to an object/room, player owned boxes don't get saved
         // when the corpses in them decay. However, they did get saved when
         // the corpses were initially placed in them. In this case, the corpses
         // were reloading at zone updates, but their "in_list" pointers weren't
         // getting set as the code below this comment doesn't think to do that.
         // Apparently this meant that the do_disolve_object() call (when the
         // freshly reloaded corpses disolved yet again) was doing mean things
         // (*cough* writing) to somewhat random heap locations. This was very
         // very bad and took me a very long time to figure out. So, if you,
         // whoever you are, feel the need to allow player owned containers to
         // regenerate, you know what you need to take care of in addition to
         // removing the sanity check directly below this comment.
         if ( obj_ptr->isContainer() && obj_ptr->isPlayerOwned() ) {
             object *o;
             Cell<object*> oc( *(room_list[k].getInv()) );
             while ( ( o = oc.next() ) ) {
                 if ( obj_ptr->getIdNum() == o->getIdNum() ) {
                     save_player_box(*o);
                 }
             }
         }//<-- save player boxes
         if ( ( obj_ptr->isContainer() ) && ( ! obj_ptr->isPlayerOwned() ) ) {
            object* ig_room_object;
            object* orig_object_inv;
            Cell<object*> ig_room_inv_cell;
            Cell<object*> orig_inv_cell;
            int id_we_want = obj_ptr->getIdNum();
            //don't update more than should load in room, otherwise players
            //have the potential to mass item dupe/farm.
            int max_to_update = obj_count(*(room_ptr->getInv()), *obj_ptr);

            obj_ptr->inv.head(orig_inv_cell);
            room_list[k].getInv()->head(ig_room_inv_cell);

            //look at everything in this room.
            while ( (ig_room_object = ig_room_inv_cell.next()) &&
                  (max_to_update--) ) {
               //if we've found a match for the container
               if ( ig_room_object->getIdNum() == id_we_want ) {
                  obj_ptr->inv.head(orig_inv_cell);
                  //walk the clean ones inventory
                  while ( ( orig_object_inv = orig_inv_cell.next() ) ) {
                     //do we need to add something to our in-game?
                     if ( (obj_count(obj_ptr->inv, *orig_object_inv) >
                           obj_count(ig_room_object->inv,
                                 *orig_object_inv )) &&
                           ( orig_object_inv->getCurInGame() <
                             orig_object_inv->getMaxInGame()) ) {
                        ig_room_object->inv.append(orig_object_inv);
                        recursive_init_loads(
                              obj_list[orig_object_inv->getIdNum()], 0);
                     }
                  }
               }//match
            }
         }//handle containers.

         if ((obj_count(*(room_ptr->getInv()), *obj_ptr) >
              obj_count(*(room_list[k].getInv()), *obj_ptr)) &&
             (obj_ptr->getCurInGame()
              < obj_list[obj_ptr->getIdNum()].getMaxInGame())) {

            if (obj_ptr->isPlayerOwned()) {
               object* pobox = load_player_box(obj_ptr->getIdNum());
               if (pobox) {
                  // Denote it as 'modified', (SMOB)
                  pobox->IN_LIST = room_list[k].getInv();
                  pobox->setCurRoomNum(k, 0);
                  affected_objects.gainData(pobox);
                  room_list[k].gainObject(pobox);
               }
               else {
                  //put a virgin copy in the room
                  room_list[k].gainObject(&(obj_list[obj_ptr->getIdNum()]));
               }
            }
            else {
               //points into object array
               room_list[k].gainObject(&(obj_list[obj_ptr->getIdNum()]));
            }
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
                  shop_keeper->setCurRoomNum(k);
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
   tmp_veh.Clear();
   return 0;
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

   // 1. log anyone who isn't isn't >= IMM level 2 off after 30 ticks of
   //    inactivity.
   // 2. if a player has keepalives enabled, send them out once every 4
   //    ticks (~5 minutes). [currently only telnet implements keepalives]
   while ((crit_ptr = crit_cell.next())) {

      if ( crit_ptr->pc->doKeepAlives() ) {

         crit_ptr->pc->last_keepalive++;

         if ( ++(crit_ptr->pc->last_keepalive) >= 4 ) {
            crit_ptr->pc->output += crit_ptr->pc->p_handler->keepalive();
            crit_ptr->pc->last_keepalive = 0;
         }

      }//if doKeepAlives()

      //This used to be a prefix increment in the conditional, but that means
      //that it could be short-circuited depending on the compiler.
      crit_ptr->pc->idle_ticks++;

      if (
            (crit_ptr->pc->idle_ticks > 30)
            && ( (!crit_ptr->pc->imm_data) || (crit_ptr->IMM_LEVEL < 2) )
         ) {

         if (mudlog.ofLevel(DBG)) {
            mudlog << "Logging off player in decrease_timed_affecting_pcs," << " name:  " <<
               *(name_of_crit(*crit_ptr, ~0)) << "  address:  "
               << crit_ptr << " ticks:  " << crit_ptr->pc->idle_ticks << endl;
         }

         log_out(*crit_ptr);
      }

      if (TRUE /*crit_ptr->pc->mode == MODE_NORMAL*/) {
         crit_ptr->affected_by.head(sp_cell);
         sp_ptr = sp_cell.next();
         while (sp_ptr) {
            if (sp_ptr->bonus_duration != -1)
               sp_ptr->bonus_duration--;
            if (sp_ptr->bonus_duration == 0) {
               rem_effects_crit(sp_ptr->stat_spell, *crit_ptr, TRUE, sp_ptr->bonus_value);
               delete sp_ptr;
               sp_ptr = crit_ptr->affected_by.lose(sp_cell);
            }//if
            else
               sp_ptr = sp_cell.next();
         }//while

         if (crit_ptr->HUNGER > 0)
            crit_ptr->HUNGER--;  //food
                 if (crit_ptr->THIRST > 0){
                         room* rm = crit_ptr->getCurRoom();
                         if(rm->hasWeather() && (weather.climates[rm->getClimate()].temperature == veryhot || 
                                 weather.climates[rm->getClimate()].temperature == burninghot)){
                                         //use more water in high temp
                                         crit_ptr->THIRST--;
                                 }
                                 crit_ptr->THIRST--;  //drink
                                 if(crit_ptr->THIRST < 0) crit_ptr->THIRST = 0;
                 }
         if (crit_ptr->DRUGGED > 0)
            crit_ptr->DRUGGED--;  //drugged
         if (crit_ptr->isViolent()) { // violence timer
            crit_ptr->pc->violence_timer--;
            if (!crit_ptr->isViolent()) {
               crit_ptr->show("You are now free to quit.\n", HL_DEF);
            }
         }

         if (crit_ptr->MODE == MODE_NORMAL) {
            if (crit_ptr->HUNGER == 0)
               crit_ptr->show("You are famished.\n");
            if (crit_ptr->THIRST == 0)
               crit_ptr->show("You are thirsty.\n");
            if (crit_ptr->DRUGGED == 0) {
               crit_ptr->show("You no longer feel drugged.\n");
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
               Sprintf(buf, "%pS flickers.\n", 
                       long_name_of_obj(*(crit_ptr->EQ[11]), crit_ptr->SEE_BIT));
               buf.Cap();
               crit_ptr->show(buf);
            }//if
         }//if

         /* check for lights gone out */
         if (crit_ptr->EQ[11]) {
            if (crit_ptr->EQ[11]->extras[0] == 0) {
               crit_ptr->EQ[11]->extras[0] = -2;
               Sprintf(buf, "%pS dims and glows its last.\n", 
                     long_name_of_obj(*(crit_ptr->EQ[11]), crit_ptr->SEE_BIT));
               buf.Cap();
               crit_ptr->show(buf);
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
      if (++(crit_ptr->pc->idle_ticks) > 2) {
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
                     crit_ptr->show("You are famished.\n");
            if (crit_ptr->THIRST == 0)
               crit_ptr->show("You are thirsty.\n");
            if (crit_ptr->DRUGGED == 0) {
               crit_ptr->show("You no longer feel drugged.\n");
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
               Sprintf(buf, "%pS flickers.\n", 
                    long_name_of_obj(*(crit_ptr->EQ[11]), crit_ptr->SEE_BIT));
               buf.Cap();
               crit_ptr->show(buf);
            }//if
         }//if

                /* check for lights gone out */
         if (crit_ptr->EQ[11]) {
            if (crit_ptr->EQ[11]->extras[0] == 0) {
               crit_ptr->EQ[11]->extras[0] = -2;
               Sprintf(buf, "%pS dims and glows its last.\n", 
                    long_name_of_obj(*(crit_ptr->EQ[11]), crit_ptr->SEE_BIT));
               buf.Cap();
               crit_ptr->show(buf);
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
   critter* tmp_crit;
   Cell<stat_spell_cell*> sp_cell;
   stat_spell_cell* sp_ptr;

   room* rm_ptr;
   object* tmp_obj;
   object* obj_ptr;

   crit_ptr = crit_cell.next();
   while (crit_ptr) {
      if (!crit_ptr->affected_by.isEmpty()) {
         crit_ptr->affected_by.head(sp_cell);
         sp_ptr = sp_cell.next();
         while (sp_ptr) {
            if (sp_ptr->bonus_duration != -1) {
               sp_ptr->bonus_duration--;
            }
            if (sp_ptr->bonus_duration == 0) {
               rem_effects_crit(sp_ptr->stat_spell, *crit_ptr, TRUE);
               delete sp_ptr;
               sp_ptr = crit_ptr->affected_by.lose(sp_cell);
            }//if
            else {
               sp_ptr = sp_cell.next();
            }
         }//while
      }//else

      // Check to see if the should, and can 'disolve'.  This is used for
      // summoned and other 'created' mobs.
      crit_ptr->mob->incrementTicksOld();
      // check to see if we can disolve it...
      if (crit_ptr->mob->isDisolvable() && !crit_ptr->isFighting()) {
         if (crit_ptr->mob->getTicksOld() > 60) {
            if ((crit_ptr->follower_of == NULL) &&
                (crit_ptr->master == NULL)) {
               // check for hunting...
               if (!crit_ptr->isTracking()) {
                  tmp_crit = crit_ptr;
                  crit_ptr = affected_mobs.lose(crit_cell);
                  tmp_crit->emote("wanders away...\n");
                  rm_ptr = tmp_crit->getCurRoom();
                  tmp_obj = rm_ptr->getInv()->peekFront();
                  agg_kills_vict(NULL, *tmp_crit, FALSE); //silently kill

                  // Now, clean up the corpse.
                  obj_ptr = rm_ptr->getInv()->peekFront();
                  if (obj_ptr && (obj_ptr != tmp_obj)) {
                     //ie if a corpse was placed in the room
                     // obj_ptr is it, and should be deleted.
                     rm_ptr->purgeObj(obj_ptr, NULL, FALSE);
                  }  
                  continue;
               }
            }//if
         }//if
      }//if

      crit_ptr->mob->decrementTicksTillFreedom();
      if (crit_ptr->master) {
         if (crit_ptr->mob->getTicksTillFreedom() <= 0) {
            crit_ptr->doFollow(*crit_ptr); //follow self
            // Takes care of guarding/shielding, etc.
            if ( crit_ptr->temp_crit ) {
                crit_ptr->temp_crit->Clear();
            }
         }//if
      }//if

      crit_ptr = crit_cell.next();
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
                << obj_ptr->OBJ_NUM << " obj_ptr: " << obj_ptr << endl;
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

      // If we ever let more than this object disolve, see the normalize_obj
      // method for notes on how not to crash the game!!
      if (obj_ptr->OBJ_NUM == config.corpseObject) { //if is corpse
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

      if (mudlog.ofLevel(DBG)) {
         mudlog << "At end of while loop." << endl;
      }

   }//while

   while (!obj_to_be_disolved_list.isEmpty()) {
      obj_ptr = obj_to_be_disolved_list.popFront();
      if (mudlog.ofLevel(DBG)) {
         mudlog << "About to disolve object: " << obj_ptr << endl;
      }
      do_disolve_object(*obj_ptr);
   }//while

   if (mudlog.ofLevel(DBG)) {
      mudlog << "At end of decrease_timed_affecting_objects." << endl;
   }
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
            Sprintf(buf, "%pS closes up and vanishes.\n", 
                    name_of_door(*dr_ptr, ~0));
            buf.Cap();
            room_list[dr_ptr->in_room].showAllCept(buf);
            door* dp_tmp = dr_ptr;
            dr_ptr = affected_doors.lose(cell);
            delete dp_tmp;;
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
         else {
            sp_ptr = sp_cell.next();
         }
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
   String buf(100);

   if (mudlog.ofLevel(DBG)) {
      mudlog << "do_disolve_obj, addr: " << &obj << endl;
   }

   if (!obj.in_list) {
      core_dump("ERROR:  disolve_object called on non-sobj.\n");
   }//if

   //log("Before while\n");
   while ((obj_ptr = cell.next())) {
      if (obj_ptr->in_list)
         obj_ptr->in_list = obj.in_list;
      obj.in_list->append(obj_ptr);
   }//while
   //log("Before Clear()\n");
   obj.inv.clear();

   if (obj.in_list->loseData(&obj)) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "Successfully removed it from it's list.\n";
      }
   }
   else {
      Sprintf(buf, "Failed to remove object ptr: %p from it's list.",
              &obj);
      mudlog << "Failed to remove object ptr: " << &obj << " from it's list:"
             << obj.in_list << endl;
      core_dump(buf);
   }
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
   room* rm_ptr = NULL;
   Cell<stat_spell_cell*> sp_cell;
   stat_spell_cell* sp_ptr = NULL;
   short take_it_off = FALSE;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "In dta_rooms: affected_rooms.size() == " 
             << affected_rooms.size() << endl;
   }

   rm_ptr = cell.next();
   while (rm_ptr) {
      take_it_off = TRUE;

      if (mudlog.ofLevel(DBG)) {
         mudlog << "dta_rooms, rm_ptr: " << rm_ptr << endl;
      }

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
      
      if (take_it_off) {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "Taking it off...\n";
         }
         rm_ptr = affected_rooms.lose(cell);
      }
      else {
         if (mudlog.ofLevel(DBG)) {
            mudlog << "NOT Taking it off...\n";
         }
         rm_ptr = cell.next();
      }
   }//while
}//decrease_timed_affecting_rooms


void add_spell_affecting_critter(int spell, int duration, critter& vict) {
   stat_spell_cell* spcell = new stat_spell_cell;

   //log("In add_spell_affecting_critter");

   spcell->stat_spell = spell;
   spcell->bonus_duration = duration; 
   vict.affected_by.append(spcell);
}//gain spell_affected_by    


void add_spell_affecting_obj(int spell, int duration, object& vict) {
   stat_spell_cell* spcell = new stat_spell_cell;

   //log("In add_spell_affecting_obj\n");

   spcell->stat_spell = spell;
   spcell->bonus_duration = duration; 
   vict.affected_by.append(spcell);
}//gain spell_affected_by    


void show(const char* message, const critter& pc, hilite_type hl_type) {
   //log(message);

   String *output;

   if (mudlog.ofLevel(XMT)) {
      mudlog << "OUTPUT from -:" << *(pc.getName());
      mudlog << ":-  -:" << message << ":-\n" << endl;
   }

   if (pc.possessed_by) {
      pc.possessed_by->show("[POSSESSED]: ");
      pc.possessed_by->show(message);
   }

   if (pc.pc) {

      critter* snooper = pc.SNOOPED_BY;
      if (snooper) {
         mudlog.log(TRC, "Within snoop if\n");
         String buf2(100);
         Sprintf(buf2, "SNOOP_OUT:  -:%s:-\n", message);
         snooper->show(buf2);
      }//if snoop

      //In theory we'd probably better to make critter::setDoPrompt() appear
      //const and declare the prompt flag to be Mutable.
      critter& t_pc = const_cast<critter&>(pc);
      t_pc.setDoPrompt(true);

      if (!message)
         return;

      // This is commented out because it's not fully functional as of yet.
      output = colorize(message, pc, hl_type);

      if (pc.pc->output.Strlen() < OUTPUT_MAX_LEN) {
         //pc.pc->output.Append(message);
         pc.pc->output.Append(*output);
      }//if
   }//if
}//show 


void show_all(const char* msg, const room& rm) {
   rm.showAllCept(msg, NULL);
}


void show_all_but_2(critter& A, critter& B, const char* msg,
                    room& rm, hilite_type hl_type) {
   Cell<critter*> cell(rm.getCrits());
   critter* crit_ptr;

   // log("In show_all_but_2\n");
   while ((crit_ptr = cell.next()))  { 
      if ((crit_ptr != &A) && (crit_ptr != &B)) 
         if (crit_ptr->POS < POS_SLEEP)  
            crit_ptr->show(msg, hl_type);
   }//while

   Cell<object*> cll(*(rm.getInv()));
   object* obj;

   while ((obj = cll.next())) {
     if (obj->obj_proc && (crit_ptr = obj->obj_proc->w_eye_owner)) {
       if (crit_ptr->POS < POS_SLEEP) {
         crit_ptr->show("#####");
         crit_ptr->show(msg, hl_type);
       }//if
     }//if
   }//while

}//show_all_but_2


int doShowList(critter* pc, CSelectorColl& includes, CSelectorColl& denies,
               List<critter*>& lst, CSentryE cs_entry, ...) {
   va_list argp;
   va_start(argp, cs_entry);
   int retval = 0;
   retval = vDoShowList(pc, includes, denies, lst, cs_entry, argp);
   va_end(argp);
   return retval;
}


int vDoShowList(critter* pc, CSelectorColl& includes, CSelectorColl& denies,
                List<critter*>& lst, CSentryE cs_entry, va_list argp) {
   Cell<critter*> cll(lst);
   critter* ptr;
   String buf(100);
   String buf2(100);

   while ((ptr = cll.next())) {
      if (!(denies.matches(ptr, pc))) {
         if (includes.matches(ptr, pc)) {
            if (mudlog.ofLevel(DBG)) {
               mudlog << "vDoShowList, includes matched." << endl;
               mudlog << "cstr of " << (int)(cs_entry) << "-:"
                      << cstr(cs_entry, *ptr) << ":-" <<  endl;
            }

            vSprintf(buf, cstr(cs_entry, *ptr), argp);
            if (mudlog.ofLevel(DBG)) {
               mudlog << endl << "buf -:" << buf << ":-" << endl << endl;
            }
            ptr->show(buf);
         }//if
      }//if
   }//while
   return 0;
}//voDoShowList


void show_all(const char* msg) {
   Cell<critter*> cell(pc_list);
   critter* crit_ptr;

   while ((crit_ptr = cell.next()))  { 
      if (crit_ptr->pc && crit_ptr->PC_FLAGS.get(14)) {
         crit_ptr->show(msg);
      }//if
   }//while
}//show_all_info


void out_str(const List<String*>& lst, critter& pc) { 
   Cell<String*> cell(lst);
   String *string;
   
   // log("In out_str.\n");

   if (pc.pc) {
      if (lst.isEmpty()) {
         pc.show("You see nothing special.\n");
         return;
      }//if
      while ((string = cell.next())) {
         pc.show(*string);
         pc.show("\n");
      }//while
   }//if
}//out_str

void out_vehicles(const List<door*>& lst, critter& pc) {
   Cell<door*> cell(lst);
   door* door_ptr;
   vehicle* veh_ptr;
   room* room_ptr;
   String buf(100);
   
   while ( (door_ptr = cell.next()) ) {
      room_ptr = door_ptr->getDestRoom();
      if ( 
            (room_ptr) &&
            (room_ptr->isInUse()) &&
            (room_ptr->isVehicle()) &&
            (room_ptr != pc.getCurRoom()) //dont show us our own veh.
         ) {
         veh_ptr = static_cast<vehicle*>(room_ptr);
         if ( veh_ptr->isAtDestination() ) {
            if ( veh_ptr->veh_stopped.Strlen() ) {
               Sprintf(buf,"       %pS\n", &(veh_ptr->veh_stopped));
               pc.show(buf, HL_OBJ_LIST);
            }//has description
         } else {
            if ( veh_ptr->veh_moving.Strlen() ) {
               Sprintf(buf,"       %pS\n", &(veh_ptr->veh_moving));
               pc.show(buf, HL_OBJ_LIST);
            }//has description
         }//not at destination
      }//if vehicle
   }//while doors
}//out_vehicles

/** Can over-ride the VIS/SEE bit stuff if you set see_all to true. */
void out_crit(const List<critter*>& lst, critter& pc, int see_all) {
   Cell<critter*> cell(lst);
   Cell<stat_spell_cell*> cell2;
   stat_spell_cell* sp;
   critter* crit_ptr;
   String buf(100);

   // log("In out_crit\n");

   int see_bits = pc.getSeeBit();
   if (see_all) {
      see_bits = ~0;
   }

   if (pc.isUsingClient())
      pc.show("<MOB_LIST>");

   /* KHAAVREN DELETE ME MARKER 
   else if (pc.isUsingColor()) {
      pc.show(*(pc.getMobListColor()));
   }
   */

   while ((crit_ptr = cell.next())) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "out_crit: got critter:  " << *(crit_ptr->getName())
                << endl;
      }
          if (pc.canSee(*crit_ptr) &&  //detect(see_bits, crit_ptr->VIS_BIT) &&
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

            if (crit_ptr->isParalyzed()) {
               Sprintf(buf, "     %pS%pS %s\n",
                       name_of_crit(*crit_ptr, see_bits),
                       &(crit_ptr->short_desc), cstr(CS_PARALYZED, pc));
            }
            else {
               Sprintf(buf, "     %pS%pS %s\n", 
                       name_of_crit(*crit_ptr, see_bits), 
                       &(crit_ptr->short_desc), crit_ptr->getPosnStr(pc));
            }

            buf.Cap();
            if (crit_ptr->VIS_BIT & 2) {
               buf.setCharAt(1, '*');
            }//if
            
            pc.show(buf, HL_MOB_LIST);
            crit_ptr->affected_by.head(cell2);
            while ((sp = cell2.next())) {
               if (sp->stat_spell == SANCTUARY_SKILL_NUM) {
                  Sprintf(buf, "\t\t%s glows brightly.\n", 
                          get_he_she(*crit_ptr));
                  buf.Cap();
                  pc.show(buf, HL_MOB_LIST);
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
               if (crit_ptr->isParalyzed()) {
                  Sprintf(buf, "     [%i]%P11 %pS %s\n", crit_ptr->MOB_NUM,
                          name_of_crit(*crit_ptr, see_bits), cstr(CS_PARALYZED, pc));
               }
               else {
                  Sprintf(buf, "     [%i]%P11 %pS %s\n", crit_ptr->MOB_NUM,
                          name_of_crit(*crit_ptr, see_bits), 
                          crit_ptr->getPosnStr(pc));
               }
            }
            else {
               if (crit_ptr->isParalyzed()) {
                  Sprintf(buf, "     %pS %s\n",
                          name_of_crit(*crit_ptr, see_bits),
                          cstr(CS_PARALYZED, pc));
               }
               else {
                  Sprintf(buf, "     %pS %s\n",
                          name_of_crit(*crit_ptr, see_bits), 
                          crit_ptr->getPosnStr(pc));
               }
            }
            buf.Cap();

            if (crit_ptr->VIS_BIT & 2) {
               buf.setCharAt(1, '*');
            }//if
            pc.show(buf, HL_MOB_LIST);
            
            crit_ptr->affected_by.head(cell2);
            while ((sp = cell2.next())) {
               if (sp->stat_spell == SANCTUARY_SKILL_NUM) {
                  Sprintf(buf, "\t\t%s glows brightly.\n", 
                          get_he_she(*crit_ptr));
                  buf.Cap();
                  pc.show(buf, HL_MOB_LIST);
                  break;
               }//if
            }//while
         }//if
         else if (crit_ptr->mob) { //then it is a mob, or SMOB w/same posn as MOB

            if (mudlog.ofLevel(DBG)) {
               mudlog << "Doing MOB" << endl;
            }

            if (pc.shouldShowVnums()) {
               if (crit_ptr->isParalyzed()) {
                  Sprintf(buf, "     [%i]%P11 %pS %s\n", crit_ptr->MOB_NUM,
                          name_of_crit(*crit_ptr, see_bits), cstr(CS_PARALYZED, pc));
               }
               else {
                  Sprintf(buf, "     [%i]%P11 %pS\n", crit_ptr->MOB_NUM,
                          &(crit_ptr->in_room_desc));
               }
            }//if vnums
            else {
               if (crit_ptr->isParalyzed()) {
                  Sprintf(buf, "       %pS %s\n",
                          name_of_crit(*crit_ptr, see_bits), cstr(CS_PARALYZED, pc));
               }
               else {
                  Sprintf(buf, "       %pS\n", &(crit_ptr->in_room_desc));
               }
            }

            if (crit_ptr->VIS_BIT & 2) {
               buf.setCharAt(1, '*');
            }//if
            buf.Cap();
            pc.show(buf, HL_MOB_LIST);
            
            crit_ptr->affected_by.head(cell2);
            while ((sp = cell2.next())) {
               if (sp->stat_spell == SANCTUARY_SKILL_NUM) {
                  Sprintf(buf, "\t\t%s glows brightly.\n", 
                          get_he_she(*crit_ptr));
                  buf.Cap();
                  pc.show(buf, HL_MOB_LIST);
                  break;
               }//if
            }//while
         }//else
         else {
            mudlog << __PRETTY_FUNCTION__ << "ERROR: crit_ptr is not sane: "
                   << crit_ptr << endl;
         }
      }//if
   }//while
   if (pc.USING_CLIENT) {
      pc.show("</MOB_LIST>");
   }
   /* KHAAVREN DELETE ME MARKER 
   else if (pc.isUsingColor()) {
      pc.show(*(pc.getDefaultColor()));
   }
   */
}//out_crit



void out_inv(PtrList<object>& lst, critter& pc, 
             const short type_of_list, int is_board) {
                               //outs the names object*

   PtrList<object> *lst_ptr = NULL;
   int lst_idx;

   PtrList<object> reordered_lst;
   Cell<object*> cell(lst);

   PtrList<object> inv_weapons;
   PtrList<object> inv_armor;
   PtrList<object> inv_consumables;
   PtrList<object> inv_containers;
   PtrList<object> inv_potions;
   PtrList<object> inv_scrolls;
   PtrList<object> inv_wands;
   PtrList<object> inv_ammunition;
   PtrList<object> inv_skins;
   PtrList<object> inv_misc;
   PtrList<object> inv_lights;
   
   //when doing obj/crit inventory, PCs walk these lists.
   PtrList<object>* inv_lists[] = {
      &inv_weapons,
      &inv_armor,
      &inv_consumables,
      &inv_containers,
      &inv_potions,
      &inv_scrolls,
      &inv_wands,
      &inv_ammunition,
      &inv_lights,
      &inv_misc,
      NULL
   };

   //when doing obj/crit inventory, MOBs walk these lists.
   PtrList<object>* mob_inv_lists[] = {
      &lst,
      NULL
   };

   const char *inv_names[] = {
      "[ Weapons ]",
      "[ Armor ]",
      "[ Consumables ]",
      "[ Containers ]",
      "[ Potions ]",
      "[ Scrolls ]",
      "[ Wands ]",
      "[ Ammunition ]",
      "[ Lights ]",
      "[ Misc. ]"
   };

   object*  obj_ptr;
   String buf(100);
   String qty_str = "";//important to set this? so it's not undef (if mob)
   int item_counts[NUMBER_OF_ITEMS + 1];
   int id_num;

   // stack if it's a player or possesed mob. Never stack bulletin-board
   // inventory.
   int stack = ( ( (pc.isPc() ) || (pc.isPossessed() ) ) && (!is_board) );

   mudlog.log(TRC, "In out_inv.\n");

   if (pc.isUsingClient()) {
      pc.show("<ITEM_LIST>");
   }

   if (lst.isEmpty() && type_of_list == OBJ_INV) {
      pc.show("        [empty]        \n");
      if (pc.isUsingClient()) {
         pc.show("</ITEM_LIST>");
      }

      mudlog.log(DBG, "Done with out_inv (empty).\n");
      return;
   }//if

   if ( stack ) {
      //set the counts to zero, then update them all.
      memset(item_counts, 0, sizeof(int) * (NUMBER_OF_ITEMS + 1));
      while ( (obj_ptr = cell.next()) ) {
         item_counts[obj_ptr->getIdNum()]++;

         switch (type_of_list) {
            case OBJ_INV:
            case CRIT_INV:
               if(obj_ptr->isWeapon()) {
                  inv_weapons.append(obj_ptr);
               } else if (obj_ptr->isArmor()) {
                  inv_armor.append(obj_ptr);
               } else if (obj_ptr->isPotion()) {
                  inv_potions.append(obj_ptr);
               } else if (obj_ptr->isFood()) {
                  inv_consumables.append(obj_ptr);
               } else if (obj_ptr->isContainer()) {
                  inv_containers.append(obj_ptr);
               } else if (obj_ptr->isWand()) {
                  inv_wands.append(obj_ptr);
               } else if (obj_ptr->isScroll()) {
                  inv_scrolls.append(obj_ptr);
               } else if (obj_ptr->isAmmo()) {
                  inv_ammunition.append(obj_ptr);
               } else if (obj_ptr->isLightSource()) {
                  inv_lights.append(obj_ptr);
               } else {
                  inv_misc.append(obj_ptr);
               }
         }//switch

      }//while

      //if we're showing categories, we actually need to reorganize the real
      //inventory so that get #.name works as players expect.
      switch (type_of_list) {
         case OBJ_INV:
         case CRIT_INV:
            lst_ptr = inv_lists[0];
            lst_idx = 0;
            while (lst_ptr) {
               lst_ptr->head(cell);
               while ( (obj_ptr = cell.next()) ) {
                  reordered_lst.append(obj_ptr);
               }
               lst_ptr = inv_lists[++lst_idx];
            }
            lst.clear();
            reordered_lst.head(cell);
            while ( (obj_ptr = cell.next()) ) {
               lst.append(obj_ptr);
            }
            reordered_lst.clear();
      }//switch

      lst.head(cell);
   }//if (stack)

   switch (type_of_list)
      {
      case ROOM_INV:
         while ((obj_ptr = cell.next())) {

                         if (pc.canSee(*obj_ptr)){  //  detect(pc.SEE_BIT, obj_ptr->OBJ_VIS_BIT)) {

               id_num = obj_ptr->getIdNum();

               if ( stack ) {
                  if (//these objects should not stack. 
                        (id_num == config.corpseObject) ||
                        (id_num == config.pcSkinObject) ||
                        (id_num == config.HeadObject)
                     ) {
                     qty_str = "";
                  } else {
                     if ( item_counts[id_num] == -1 ) {
                        //already done it.
                        continue;
                     } else if ( item_counts[id_num] > 1 ) {
                        Sprintf(qty_str, "(%i%s) ", item_counts[id_num], "x");
                     } else {
                        qty_str = "";
                     }
                     item_counts[id_num] = -1;
                  }//okay to stack
               }

               if (pc.shouldShowVnums()) {
                  char tmp[50];
                  sprintf(tmp, "%p: ", obj_ptr);
                  Sprintf(buf, "   %s [%i] %P11 %pS%pS", tmp, id_num,
                          &qty_str, &(obj_ptr->in_room_desc));
               }
               else {
                  Sprintf(buf, "\t%pS%pS", &qty_str, &(obj_ptr->in_room_desc));
               }
               buf.Cap();

               if (obj_ptr->isHerb()) {
                  if (d(1, 100) <= 
                      d(1, 2 * get_percent_lrnd(HERBALISM_SKILL_NUM, pc))) {
                     buf.Append("(^Gherb^0)");
                  }//if
               }//if

               if (pc.canDetectMagic() &&
                   (!obj_ptr->affected_by.isEmpty() ||
                    !obj_ptr->stat_affects.isEmpty())) {
                  buf.Append("^B{Blue Glow}^0\n");
               }//if
               else {
                  buf.Append("\n");
               }

               if (obj_ptr->OBJ_VIS_BIT & 2) {
                  buf.Prepend("*");
               }//if

               pc.show(buf, HL_OBJ_LIST);
            }//if
         }//while
         break;
      case OBJ_INV:
      case CRIT_INV:
         
         if ( stack ) {
            lst_ptr = inv_lists[0];
         } else {
            lst_ptr = mob_inv_lists[0];
         }
         lst_idx = 0;

         while ( lst_ptr ) {

            lst_ptr->head(cell);

            if ( stack  ) {
               if ( (obj_ptr = cell.next()) ) {
                  Sprintf(buf, "%s\n", inv_names[lst_idx]);
                  pc.show(buf, HL_OBJ_LIST);
                  obj_ptr = cell.prev();
               }
            }

            while ((obj_ptr = cell.next())) {
               if (detect(pc.SEE_BIT, obj_ptr->OBJ_VIS_BIT)) {

                  id_num = obj_ptr->getIdNum();

                  if ( stack ) {
                     if (//these objects should not stack. 
                           (id_num == config.corpseObject) ||
                           (id_num == config.pcSkinObject) ||
                           (id_num == config.HeadObject)
                        ) {
                        qty_str = "";
                     } else {
                        if ( item_counts[id_num] == -1 ) {
                           //already done it.
                           continue;
                        } else if ( item_counts[id_num] > 1 ) {
                           Sprintf(qty_str, "(%i%s) ", item_counts[id_num], "x");
                        } else {
                           qty_str = "";
                        }
                        item_counts[id_num] = -1;
                     }//okay to stack
                  }

                  if (pc.shouldShowVnums()) {
                     char tmp[50];
                     sprintf(tmp, "%p: ", obj_ptr);
                     Sprintf(buf, "   %s [%i] %P11 %pS%pS", tmp, id_num,
                           &qty_str, long_name_of_obj(*obj_ptr, ~0));
                  }
                  else {
                     Sprintf(buf, "\t%pS%pS", &qty_str,
                           long_name_of_obj(*obj_ptr, ~0));
                  }

                  buf.Cap();

                  if (obj_ptr->isHerb()) {
                     if (d(1, 100) <= 
                           d(1, 2 * get_percent_lrnd(HERBALISM_SKILL_NUM, pc))) {
                        buf.Append("(^Gherb^0)");
                     }//if
                  }//if

                  if  (pc.canDetectMagic() &&
                        (!obj_ptr->affected_by.isEmpty() ||
                         !obj_ptr->stat_affects.isEmpty())) {
                     buf.Append("^B{Blue Glow}^0\n");


                  }//if
                  else {
                     buf.Append("\n");
                  }
                  if (obj_ptr->OBJ_VIS_BIT & 2) {
                     buf.Prepend("*");
                  }//if
                  pc.show(buf, HL_OBJ_LIST);
               }//if
            }//while
            lst_ptr = inv_lists[++lst_idx];
         }// while lst_ptr
         break;
      default:
         mudlog.log(LS_ERROR, "ERROR:  default called in out_inv.\n");
         break;
      }//switch type_of_list

   if (pc.isUsingClient()) {
      pc.show("</ITEM_LIST>");
   }

   mudlog.log(DBG, "Done with out_inv.\n");

}//out_inv

critter* have_crit_named(List<critter*>& lst, const int i_th,
                         const String* name, const int see_bit,
                         const room& rm, int do_exact) {
   int foo = 0;
   return have_crit_named(lst, i_th, name, see_bit, foo, rm, do_exact);
}

critter* have_crit_named(List<critter*>& lst, const int i_th,
                         const String* name, const int see_bit,
                         int& count_sofar, const room& rm, int do_exact) {
   Cell<String*> char_cell;
   Cell<critter*> cell(lst);
   critter* crit_ptr;
   int count = 0;
   int ptr_v_bit, len;
   String *string;

   if (mudlog.ofLevel(DBG)) {
      mudlog << "in have_crit_named, i_th: " << i_th << " name -:"
             << *name << ":- see_bit: " << see_bit << " rm# "
             << rm.getIdNum() << " do_exact: " << do_exact << endl;
   }

   if (!name) {
      mudlog.log(LS_ERROR, "ERROR:  NULL name sent to have_crit_named.\n");
      return NULL;
   }//if

   if ((len = name->Strlen()) == 0) 
      return NULL;

   if (i_th <= 0)
      return NULL;

   int matched;
   while ((crit_ptr = cell.next())) {
      ptr_v_bit = (crit_ptr->VIS_BIT | rm.getVisBit()); 
      if (detect(see_bit, ptr_v_bit)) {
         crit_ptr->names.head(char_cell);
         matched = FALSE;
         while ((string = char_cell.next()) && !matched) {
            if (do_exact) {
                  if (strcasecmp(*string, *name) == 0) { 
                  matched = TRUE;
                  count++;
                  count_sofar++;
               }//if
            }//if exact
            else {
                  if (strncasecmp(*string, *name, len) == 0) { 
                  matched = TRUE;
                  count++;
                  count_sofar++;
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


int crit_sub_a_4_b(critter* a, PtrList<critter>& lst, 
                   const int i_th, const String* name,
                   const int see_bit, room& rm) {
   Cell<String*> char_cell;
   Cell<critter*> cell(lst);
   critter* crit_ptr;
   int count = 0, ptr_v_bit;
   String *string;

   //log("In crit_sub_a_4_b.\n");

   if (!name || !a) {
      mudlog.log(LS_ERROR, "ERROR:  NULL(s) sent to crit_sub_a_4_b.\n");
      return FALSE;
   }//if

   if (name->Strlen() == 0) {
      mudlog.log(LS_ERROR, "ERROR:  name of zero length sent to crit_sub_a_4_b.\n"); 
      return FALSE;
   }//if

   int matched;
   while ((crit_ptr = cell.next())) {
      ptr_v_bit = (crit_ptr->VIS_BIT | rm.getVisBit()); 
      if (detect(see_bit, ptr_v_bit)) {
         crit_ptr->names.head(char_cell);
         matched = FALSE;
         while ((string = char_cell.next()) && !matched) {
            if (strncasecmp(*string, *name, name->Strlen()) == 0){ 
               matched = TRUE;
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


int obj_sub_a_4_b(object* a, PtrList<object>& lst, const int i_th,
                  const String* name, const int see_bit, room& rm) {
   Cell<String*> char_cell;
   Cell<object*> cell(lst);
   object* obj_ptr;
   int count = 0, ptr_v_bit;
   String *string;

   //log("In obj_sub_a_4_b.\n");

   if (!name || !a) {
      mudlog.log(LS_ERROR, "ERROR:  NULL(s) sent to obj_sub_a_4_b.\n");
      return FALSE;
   }//if

   if (name->Strlen() == 0) {
      mudlog.log(LS_ERROR, "ERROR:  name of zero length sent to obj_sub_a_4_b.\n"); 
      return FALSE;
   }//if

   int matched;
   while ((obj_ptr = cell.next())) {
      ptr_v_bit = (obj_ptr->OBJ_VIS_BIT | rm.getVisBit()); 
      if (detect(see_bit, ptr_v_bit)) {
         obj_ptr->names.head(char_cell);
         matched = FALSE;
         while ((string = char_cell.next()) && !matched) {
            if (strncasecmp(*string, *name, name->Strlen()) == 0){ 
               matched = TRUE;
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
                       const String* name, const int see_bit,
                       const room& rm) {
   int foo = 0;
   return have_obj_named(lst, i_th, name, see_bit, rm, foo);
}

object* have_obj_named(const List<object*>& lst, const int i_th,
                       const String* name, const int see_bit,
                       const room& rm, int& count_sofar) {
   Cell<String*> char_cell;
   Cell<object*> cell(lst);
   object* obj_ptr;
   int count = 0, ptr_v_bit;
   String *string;

   if (!name) {
      mudlog.log(LS_ERROR, "ERROR:  Null sent to have_obj_named.k\n");
      return NULL;
   }//if

   if (name->Strlen() == 0) 
      return NULL;

   int matched;
   while ((obj_ptr = cell.next())) {
      ptr_v_bit = (obj_ptr->OBJ_VIS_BIT | rm.getVisBit());
      if (detect(see_bit, ptr_v_bit)) {
         obj_ptr->names.head(char_cell);
         matched = FALSE;
         while ((string = char_cell.next()) && !matched) {
            if (strncasecmp(*string, *name, name->Strlen()) == 0){ 
               matched = TRUE;
               count++;
               count_sofar++;
               if (count == i_th) {
                  return obj_ptr;
               }//if
            }//if
         }//while
      }//if
   }//while
   return NULL;
}//have_obj_named 


int obj_named_count(const List<object*>& lst, const String* name,
                    const int see_bit, const room& rm) {
   Cell<String*> char_cell;
   Cell<object*> cell(lst);
   object* obj_ptr;
   int count = 0, ptr_v_bit;
   String *string;

   if (name->Strlen() == 0) 
      return 0;

   while ((obj_ptr = cell.next())) {
      ptr_v_bit = (obj_ptr->OBJ_VIS_BIT | rm.getVisBit());
      if (detect(see_bit, ptr_v_bit)) {
         obj_ptr->names.head(char_cell);
         while ((string = char_cell.next())) {
            if (strncasecmp(*string, *name, name->Strlen()) == 0){ 
               count++;
               break; //out of the internal while loop
            }//if
         }//while
      }//if
   }//while
   return count;
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
      String* tmp = obj.names.peekFront();
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
   if ((rm.names.isEmpty()) || !detect(see_bit, rm.getVisBit())) {
      return &SOMEWHERE;
   }//if
   else {
      return rm.names.peekFront();
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

   if (dr.names.isEmpty())
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
         if (strncmp(ip, *(ptr), min(ip.Strlen(), ptr->Strlen())) == 0)
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

bool isNightTime() {
   int hour_adj;

   if (config.day < 5) hour_adj = 3; // Jan 5
   else if (config.day < 82) hour_adj = 2; // Mar 23
   else if (config.day < 158) hour_adj = 1; // June 7
   else if (config.day < 185) hour_adj = 0; // July 4
   else if (config.day < 265) hour_adj = 2; // Sep 22
   else if (config.day < 341) hour_adj = 1; // Dec 7
   else hour_adj = 3; // Dec 31, or something weird

   if (config.hour <= (5 + hour_adj) || config.hour >= (22 - hour_adj))
      return true;
   else
      return false;
}

String *colorize(const char *message, const critter &pc, hilite_type hl_type)
{

   static String output;
   char *i = NULL;
   char *colstrWhich = NULL;
   short dont_reset = FALSE;

   const char *validFGColorCodes = "0nrgybmcwNRGYBMCW";
   const char *validBGColorCodes = "0nrgybmcw";
   const char *col_comp_ptr;

   output = "";
   
   if ( pc.isUsingColor() && (! pc.isUsingClient()) ) {
      // Parse ANSI stuff
      for(i=(char *)message;*i;i++) {
         
         // This strips those silly << for Hegemon protection
         if ( (*i == '<') && (*(i+1) == '<' ) ) {
            i++;
            if (! *i) {
               break;
            }
         }

         if ( *i == ESC_CHAR ) {
            dont_reset = TRUE;
         }

        /* To prevent bleeding we reset to the default color for the output
         * type at the encounter of every newline or ^0 &0 reset. Okay this
         * conditional is very confusing so I'll simplify it verbally here. If
         * we envounter a newline, OR if we have a color change code AND a 0
         * which means reset we need to do this. We also need to do this the
         * prior to any other processing hence an even ickier logic.
         */
         if ( ( *i == '\n' && ( ! dont_reset ) )
               || (
                  ( ( *i == '^' ) || ( *i == '&' ) )
                  &&
                  ( *(i+1) == '0' ) )  
               || i == message
            ) {

            output.Append(ANSI_NORMAL);

            switch (hl_type) {
               case HL_GOSSIP:
                  output.Append(*(pc.getGossipColor()));
                  break;
               case HL_SAY:
                  output.Append(*(pc.getSayColor()));
                  break;
               case HL_YELL:
                  output.Append(*(pc.getYellColor()));
                  break;
               case HL_TELL:
                  output.Append(*(pc.getTellColor()));
                  break;
               case HL_DESC:
                  output.Append(*(pc.getDescColor()));
                  break;
               case HL_OBJ_LIST:
                  output.Append(*(pc.getObjListColor()));
                  break;
               case HL_MOB_LIST:
                  output.Append(*(pc.getMobListColor()));
                  break;
               case HL_DEF:
                  output.Append(*(pc.getDefaultColor()));
                  break;
               case HL_BAK:
                  output.Append(*(pc.getBackGroundColor()));
                  break;
               case HL_BATTLE:
                  output.Append(*(pc.getBattleColor()));
                  break;
               case HL_USR1:
                  break;
               case HL_USR2:
                  break;
               case HL_USR3:
                  break;
               case HL_ROOM:
                  output.Append(*(pc.getRoomColor()));
                  break;
            }
         }

         if ( *i == '^' ) {
            i++;
            if (! *i) {
               break;
            }

            // handle ^^ or &&
            if ( *i == *(i-1) ) {
               output.Append(*i);
               continue;
            }

            switch (*i) {
               case '0':
                  break;

               case 'n':
                  output.Append(ANSI_NORMAL);
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_BLACK);
                  output.Append(END_ESC);
                  break;
               case 'r':
                  output.Append(ANSI_NORMAL);
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_RED);
                  output.Append(END_ESC);
                  break;
               case 'g':
                  output.Append(ANSI_NORMAL);
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_GREEN);
                  output.Append(END_ESC);
                  break;
               case 'y':
                  output.Append(ANSI_NORMAL);
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_YELLOW);
                  output.Append(END_ESC);
                  break;
               case 'b':
                  output.Append(ANSI_NORMAL);
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_BLUE);
                  output.Append(END_ESC);
                  break;
               case 'm':
                  output.Append(ANSI_NORMAL);
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_MAGENTA);
                  output.Append(END_ESC);
                  break;
               case 'c':
                  output.Append(ANSI_NORMAL);
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_CYAN);
                  output.Append(END_ESC);
                  break;
               case 'w':
                  output.Append(ANSI_NORMAL);
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_WHITE);
                  output.Append(END_ESC);
                  break;

                  // Start HILITE colors now.

               case 'N':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_HILITE);
                  output.Append(BB_ANSI_SEPERATE);
                  output.Append(BB_ANSI_BLACK);
                  output.Append(END_ESC);
                  break;
               case 'R':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_HILITE);
                  output.Append(BB_ANSI_SEPERATE);
                  output.Append(BB_ANSI_RED);
                  output.Append(END_ESC);
                  break;
               case 'G':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_HILITE);
                  output.Append(BB_ANSI_SEPERATE);
                  output.Append(BB_ANSI_GREEN);
                  output.Append(END_ESC);
                  break;
               case 'Y':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_HILITE);
                  output.Append(BB_ANSI_SEPERATE);
                  output.Append(BB_ANSI_YELLOW);
                  output.Append(END_ESC);
                  break;
               case 'B':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_HILITE);
                  output.Append(BB_ANSI_SEPERATE);
                  output.Append(BB_ANSI_BLUE);
                  output.Append(END_ESC);
                  break;
               case 'M':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_HILITE);
                  output.Append(BB_ANSI_SEPERATE);
                  output.Append(BB_ANSI_MAGENTA);
                  output.Append(END_ESC);
                  break;
               case 'C':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_HILITE);
                  output.Append(BB_ANSI_SEPERATE);
                  output.Append(BB_ANSI_CYAN);
                  output.Append(END_ESC);
                  break;
               case 'W':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_HILITE);
                  output.Append(BB_ANSI_SEPERATE);
                  output.Append(BB_ANSI_WHITE);
                  output.Append(END_ESC);
                  break;

               default:
                  output.Append(*(i-1));
                  output.Append(*i);

            } // case
         } // ^ encountered 
         else if ( *i == '&' ) {
            i++;
            if (! *i) {
               break;
            }

            // handle ^^ or &&
            if ( *i == *(i-1) ) {
               output.Append(*i);
               continue;
            }

            switch (*i) {
               case '0':
                  break;
               case 'n':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_BBLACK);
                  output.Append(END_ESC);
                  break;
               case 'r':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_BRED);
                  output.Append(END_ESC);
                  break;
               case 'g':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_BGREEN);
                  output.Append(END_ESC);
                  break;
               case 'y':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_BYELLOW);
                  output.Append(END_ESC);
                  break;
               case 'b':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_BBLUE);
                  output.Append(END_ESC);
                  break;
               case 'm':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_BMAGENTA);
                  output.Append(END_ESC);
                  break;
               case 'c':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_BCYAN);
                  output.Append(END_ESC);
                  break;
               case 'w':
                  output.Append(ESC_SEQ);
                  output.Append(BB_ANSI_BWHITE);
                  output.Append(END_ESC);
                  break;

               default:
                  output.Append(*(i-1));
                  output.Append(*i);
            } // case
         } // & encountered 
         else {
            output.Append(*i);
         }
      } // for loop
      // parse ansi stuff
   } else { // We go here if they aren't using color.
      for(i=(char *)message;*i;i++) {
         
         // This strips those silly << for Hegemon protection
         if ( ! pc.isUsingClient() ) {
            if ( (*i == '<') && (*(i+1) == '<' ) ) {
               i++;
               if (! *i) {
                  break;
               }
            }
         }
         
         if ( *i == '^' || *i == '&' ) {

            i++;
            if (! *i) {
               break;
            }
            if ( *i == '^' || *i == '&' ) {
               output.Append(*i);
            } else {

               if ( *(i-1) == '^' ) {
                  colstrWhich = (char *)validFGColorCodes;
               } else if ( *(i-1) == '&' ) {
                  colstrWhich = (char *)validBGColorCodes;
               }

               // Walk until we run out of valid colors or we hit a match
               for ( col_comp_ptr = colstrWhich;
                     *col_comp_ptr && *col_comp_ptr != *i; col_comp_ptr++ ); 

               // If there's no match print the literal ^ or & and following
               // character.
               if ( ! (*col_comp_ptr == *i) ) {
                  output.Append(*(i-1));
                  output.Append(*i);
               }
            }
         } else {
            // If it's not remotely a color code print it literally
            output.Append(*i);
         }
      }

   } 
   return(&output);
}

int verifydoors(int i_th, critter &pc) {
   Cell<door*> dcell;
   room* rm_ptr = NULL;
   door* dr_ptr = NULL;
   door* dst_dr_ptr = NULL;
   int cur_rm_num;
   int chk_rm_num;
   String buf;
   int i;
   int dir_okay = 0;
   String *dr_dir = NULL;
   String *dst_dir = NULL;

   if (! ok_to_do_action(NULL, "I", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
      return -1;
   }

   for(i=0;i<NUMBER_OF_ROOMS;i++) {
      rm_ptr = &room_list[i];
      if ( (rm_ptr) && (rm_ptr->isUsed()) &&
            (rm_ptr->getZoneNum() == i_th) ) {
         cur_rm_num = abs(rm_ptr->getRoomNum());
         rm_ptr->doors.head(dcell);
         while ( (dr_ptr = dcell.next()) ) {
            chk_rm_num = abs(dr_ptr->getDestRnum());
            dir_okay = false;
            if (! dr_ptr->getDestRoom()) {
               Sprintf(buf,
                     "^c[^B%d^c:^B%d^c] ^N-^w-> ^c[^B%d^c] "
                     "^R** which is not in use.^0\n",
                     rm_ptr->getZoneNum(), cur_rm_num, chk_rm_num);
               pc.show(buf);
               continue;
            }
            if ( ! (dst_dr_ptr = 
                     dr_ptr->findDoorByDest(dr_ptr->getDestRoom()->doors,
                        cur_rm_num)) ) {
               Sprintf(buf, "^c[^B%d^c:^B%d^c] ^N-^w-> ^c[^B%d^c:^B%d^c] "
                     "^r!--> ^c[^B%d^c:^B%d^c]^0\n"
                     "^cLHS: ^C%pS ^cRHS: ^C%pS\n\n^0",
                     rm_ptr->getZoneNum(), cur_rm_num,
                     dr_ptr->getDestRoom()->getZoneNum(), chk_rm_num,
                     rm_ptr->getZoneNum(), cur_rm_num,
                     &(rm_ptr->short_desc), 
                     &(dr_ptr->getDestRoom()->short_desc));
               pc.show(buf);
               //if it isn't reflexive in any direction.
            } else { 
               //there's a way back

               //warn us if the distances aren't the same in both directions
               if ( dr_ptr->distance != dst_dr_ptr->distance ) {
                  Sprintf(buf, "^c[^B%d^c:^B%d^c:^B%pS^c] ^m(^M%d^m)"
                        "^w--> <--"
                        "^m(^M%d^m) ^c[^B%d^c:^B%d^c:^B%pS^c]"
                        " ^GDistance mismatch.\n"
                        "^cLHS: ^C%pS ^cRHS: ^C%pS\n\n^0",
                        rm_ptr->getZoneNum(), cur_rm_num, dr_dir,
                        dr_ptr->distance, dst_dr_ptr->distance,
                        dr_ptr->getDestRoom()->getZoneNum(),
                        chk_rm_num, dst_dir, &(rm_ptr->short_desc),
                        &(dr_ptr->getDestRoom()->short_desc));
                  pc.show(buf);

               }

               //if it does have a way back, make sure it's the opposite
               //direction.
               dr_dir = dr_ptr->getDirection();
               dst_dir = dst_dr_ptr->getDirection();

               if ( strncasecmp((const char*)(*dr_dir),
                        get_opposite_dir((const char*)(*dst_dir)),
                        strlen((const char*)(*dr_dir))) == 0 ) {
                  dir_okay = true;
               }

               if ( ! dir_okay ) {
                  Sprintf(buf, "^c[^B%d^c:^B%d^c:^B%pS^c] ^w<--> "
                        "^c[^B%d^c:^B%d^c:^B%pS^c] "
                        "^GDirection mismatch.\n"
                        "^cLHS: ^C%pS ^cRHS: ^C%pS\n\n^0",
                        rm_ptr->getZoneNum(), cur_rm_num,
                        dr_dir,
                        dr_ptr->getDestRoom()->getZoneNum(), chk_rm_num,
                        dst_dir,
                        &(rm_ptr->short_desc), 
                        &(dr_ptr->getDestRoom()->short_desc));
                  pc.show(buf);

               }
            }//doors go opposite directions.
         }//while there are more doors.
      }//if it's a valid room.
   }//while there are more rooms.
   return(0);
}//verifydoors()

int mia_mobs(int i_th, critter &pc) {
    /* mia_mobs prints out a list of all mobs which are defined in the World
     * DB but are not currently in game. In theory we can extend this later to
     * use i_th to limit the output to mobs from zone #i_th.
     */
    int i;
    String buf;

    if (! ok_to_do_action(NULL, "I", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
        return -1;
    }

    pc.show("^RThe following MOBs are missing from the game:^0\n");
    for( i=0; i<NUMBER_OF_MOBS+1; i++ ) {
        if ( mob_list[i].isInUse() ) {
            if ( mob_list[i].mob->getCurInGame() == 0 ) {
                Sprintf(buf,"^B[%d] %pS^0\n", i, mob_list[i].getName());
                pc.show(buf);
            }
        }//isInUse()
    }//for
    pc.show("^REnd of list.^0\n");
    return(0);
}//mia_mobs()

int mia_objects(int i_th, critter &pc) {
    /* mia_objects prints out a list of all objects which are defined in the
     * World DB but are not currently in game. In theory we can extend this
     * later to use i_th to limit the output to objects from zone #i_th.
     */
    int i;
    String buf;

    if (! ok_to_do_action(NULL, "I", 0, pc, pc.getCurRoom(), NULL, TRUE)) {
        return -1;
    }

    pc.show("^RThe following OBJs are missing from the game:^0\n");
    for( i=0; i<NUMBER_OF_ITEMS+1; i++ ) {
        if ( obj_list[i].isInUse() ) {
            if ( obj_list[i].getCurInGame() == 0 ) {
                Sprintf(buf,"^B[%d] %pS^0\n", i, obj_list[i].getLongName());
                pc.show(buf);
            }
        }//isInUse()
    }//for
    pc.show("^REnd of list.^0\n");
    return(0);
}//mia_objects()
