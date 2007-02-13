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
// To contact the Author, Ben Greear:  greear@cyberhighway.net, (preferred)
//                                     greearb@agcs.com
//

///************************  Spells.h  *****************************///

#ifndef SpellsInclude
#define SpellsInclude

#include "const.h"
#include <string2.h>
#include "classes.h"
#include "room.h"
#include "critter.h"
#include "object.h"
#include "door.h"

#define SHADOWS_BLESSING_EFFECT          -15
#define ARMOR_EFFECT                         -30
#define ARMOR_EFFECT_M           -30
#define POISON_HP_REGEN_AUGMENTATION     -75
#define POISON_MV_REGEN_AUGMENTATION     -75
#define POISON_MA_REGEN_AUGMENTATION     -75
#define FIRE_PROOF_EFFECT                -60
#define WEAKEN_EFFECT                    -2
#define STRENGTH_EFFECT                   2

void cast_fireball(int i_th, const String* vict, critter& agg);
void cast_poison(int i_th, const String* vict, critter& agg);
void cast_illuminate(critter& pc);

void do_cast_fireball(critter& agg, critter& vict, int is_canned, int lvl);
void do_cast_poison(critter& agg, critter& vict, int is_canned, int lvl);
void do_cast_illuminate(room& rm, critter& agg, int is_canned, int lvl);

void cast_teleport(int i_th, const String* vict, critter& pc);
void do_cast_teleport(critter& vict, critter& pc, int is_canned, int lvl);

void cast_passdoor(int i_th, const String* vict, critter& pc);
void do_cast_passdoor(door& dr, critter& vict, int is_canned, int lvl);

void cast_tammuz(int i_th, const String* vict, critter& pc);
void do_cast_tammuz(critter& vict, critter& pc, int is_canned, int lvl);

void cast_recall(int i_th, const String* vict, critter& pc);
void do_cast_recall(critter& vict, critter& pc, int is_canned, int lvl);

void cast_summon(int i_th, const String* vict, critter& pc);
void do_cast_summon(critter& vict, critter& pc, int is_canned, int lvl);


///********** effects of passive spells (when triggered **********///

void do_firewall_effects(critter& pc, int& is_dead);
int do_distortion_wall_effects(critter& pc, int& is_dead, int sanity);


///**************   auxillary spell/skill functions   *************///

void rem_effects_crit(int spell_num, critter& pc, short do_msg, int bonus_value);
void rem_effects_obj(int spell_num, object& obj);
void rem_effects_room(int spell_num, room& rm, short do_msg);
void rem_effects_door(int spell_num, door& dr, room& rm1, room& rm2,
                      short do_msg);

int relocate_within_zone(critter& pc, int& is_dead, int sanity, int do_msgs);

int has_all_prereqs(int skill_num, critter& pc); 
int get_skill_difficulty(int skill_num);


int get_min_skill_level(int skill_num);

void update_skill(int last_gained, critter& pc); //used in practice
void update_skills(critter& pc);

short lost_concentration(critter& agg, int spell_num);

int get_mana_cost(int spell_num);
int get_mana_cost(int spell_num, critter& pc);
//int get_mana_cost(const char* name);

int get_number_of_scroll(int spell_num); //returns -1 if !exist
 
//////////////////////////////////////////////////////



void config_spells();


class Spell
{
	private:
//	virtual int count; //this isn't used for anything atm
	public:
   critter* agg; // caster
   int clvl; //casting lvl
   int duration; //short ticks or long ticks is defined by what list is in
	int spell_num; //slist number
	int spell_mana; // mana cost
	int pause; // pause from casting
	char* name; //duh
   char* msg_lost_con;// lost concentration message
   
	char* msg_no_target; //no target message
	char* actions;// string passed to ok_to_do_action
	char* diversions;// string passed to check_for_diversions
	int canned;

//	virtual void onCast(int i_th, const String* vict, critter& pc, int is_canned = FALSE, 
//                  int lvl = 0); // checks to see if the spell can be cast, if the target is correct, and if the casting fails
	virtual int doCastEffects();
	virtual void doSpellEffects(); 
	virtual void doWearOffEffects();// what to do when it wears off
	virtual void doMovementEffects(); // mainly for spells like distortion wall
	virtual void doPerTickEffects(); // spells that operate over time, no current spells do this
	
	void doFailureLostCon(); // lost concentration failure stuff
	void doFailureCanned();// canned failure, not used by any spell currently
	void doFailureNoTarget(); // no target failure
	
	//virtual int getSpellTarget(int i_th, const String* target, critter& pc);	
 
	Spell() {}// gets rid of an error message in g++
        virtual ~Spell() {};
	
	virtual void setupSpell(int spelln, int spellp, char* act, char* divers,
                           char* spell_name,
			                  char* no_target,
			                  char lost_con[] = "obviously forgot part of the spell.\n");

	
	//okToCast(int i_th, const String* dr, critter& pccritter* vict, const char* flags, critter& pc, 
	//		room* aux_rm = NULL, critter* aux_crit = NULL,
        //            int do_msg = TRUE);

};


  
////////////////////////////////////////////

class MobSpell : public Spell {
    public:
    critter* victim;
	
    //int getSpellTarget(int i_th, const String* vict, critter& pc);
    void onCast(critter& vict, critter& pc, int is_canned = FALSE,
                       int lvl = 0);
   void onCast(int i_th, const String* vict, critter& pc, int is_canned = FALSE, 
                  int lvl = 0);
    
    
//	int doCastEffects(critter& victim, critter& agg, int lvl); //don't bother defining this here
};    


////////////////////////////////////////////

class ObjSpell : public Spell{
    public:
    object* target;

    int getSpellTarget(int i_th, const String* vict, critter& pc);

};


/////////////////////////////////////////////////////

class DoorSpell : public Spell{
    public:
    door* target;

    int getSpellTarget(int i_th, const String* vict, critter& pc);

};



////////////////////////////////////////////

class RoomSpell : public Spell{
    public:
    room* target;

    int getSpellTarget(int i_th, const String* vict, critter& pc);

};



//////////////////////////////////////////////////


class SelfSpell : public Spell{
    public:

    int getSpellTarget(int i_th, const String* vict, critter& pc) {return TRUE;} // do nothing here, not needed
};




#endif
