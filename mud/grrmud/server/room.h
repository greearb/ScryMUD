// $Id: room.h,v 1.39 2002/03/01 17:28:16 gingon Exp $
// $Revision: 1.39 $  $Author: gingon $ $Date: 2002/03/01 17:28:16 $

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


 
///*************************  Room  *****************************///

#ifndef GRRMUD_ROOM_INCLUDE_H
#define GRRMUD_ROOM_INCLUDE_H

#include "critter.h"
#include "door.h"
#include "object.h"
#include <bitfield.h>
#include <list2.h>
#include <string2.h>
#include "classes.h"
#include "script.h"
#include "commands.h"
#include "lang_strings.h"

class zone;
class CSelectorColl;

class KeywordPair {
protected:
   static int _cnt;

public:
   PtrList<String> names;
   String desc;

   KeywordPair() { _cnt++; }
   KeywordPair(const KeywordPair& src);
   ~KeywordPair();

   void clear();
   void write(ofstream& dafile);
   void read(ifstream& dafile);
   void show(int idx, critter& pc);

   KeywordPair& operator=(const KeywordPair& rhs);
   
   int isNamed(const String* name);
   static int getInstanceCount() { return _cnt; }
};//KeywordPair


class room {
private:
   static int _cnt;

protected:
   int pause;
   RoomScript* cur_script;             /* currently executing script */
   PtrList<RoomScript> pending_scripts;  /* queue holds scripts to be run */
   PtrList<RoomScript> room_proc_scripts; /* Scripts this room has. */

   PtrList<critter> critters;

   PtrList<object> inv;

   int cur_stats[ROOM_CUR_STATS + 1];  
           // 0 vis_bit {1 dark, 2 invis, 4 hidden,
           //            8 imm_invis1, 16 imm_invis3, 32 link_dead_invis
           //            64 imm_invis5, 128 imm_invis7, 256 imm_invis9,
           //            512 imm_invis10, 1024 NORMAL
           // 1 movement_cost 
           // 2 room_num; 
           // 3 fall_to

   bitfield room_flags;
           // 0 no_rest, 1 !imm, 2 !god,
           // 3 is_perm_dark, 4 weather, 5 !ranged_communication, 6 !mag_exit,
           // 7 is_haven, 8 !pk, 9 !magic, 10 !mob,
           // 11 !potions, 12 !staffs, 13 !mort
           // 14 normally_dark, 15 river/small lake,
           // 16 ocean/big lake, 17 swamp, 
           // 18 need_fly, 19 need_boat, 20 need_climb
           // 21 is_zlocked (for olc), 22 is_total_loaded,
           // 23 is_used, 24 !magic_entry
           // 25 !vehicle (vehicles can't drive here), 26 cramped (!huge)
           // 27 !ranged, 28 need_dive_ability, 29 used_in_track
           // 30 can_camp 31 !complete (olc wise) 32 has_keywords
           // 33 !mob_wander, 34 !foreign_mob_wander, 35 has_proc_script,
           // 36 coliseum, 37 is_vehicle

   
public:
   PtrList<stat_spell_cell> affected_by;
   
   PtrList<String> names;
   String short_desc;
   String long_desc;

   PtrList<door> doors;   
   PtrList<KeywordPair> keywords;

   room();
   room(room& source);
   room(int rm_num);
   virtual ~room ();

   virtual room& operator= (room& source);

   bitfield& getFlags() { return room_flags; }

   KeywordPair* haveKeyword(int i_th, const String* str);
   KeywordPair* haveKeyword(int i_th, const String* str, int& count_sofar);

   static int getInstanceCount() { return _cnt; }

   const PtrList<object>* peekInv() { return &inv; }
   PtrList<object>* getInv() { return &inv; }

   int getPause() const { return pause; }
   int decrementPause() { if (pause > 0) pause--; return pause; }

   int insertNewScript(RoomScript* ptr);
   const ScriptCmd* getNextScriptCmd() { return cur_script->getNextCommand(); }
   void finishedRoomProc();
   int isInProcNow() { return (cur_script && (cur_script->isInProgress())); }
   void addProcScript(const String& txt, RoomScript* script_data);
   void doScriptJump(int abs_offset);
   void listScripts(critter& pc);
   RoomScript* getScriptAt(int idx) { return room_proc_scripts.elementAt(idx); }
   void removeScript(String& trigger, int i_th, critter& pc);

   /** Attempt to trigger a room script directly.  So far, we support only
    * pull and push, but more can easily be added.
    */
   int attemptExecuteUnknownScript(String& cmd, int i_th, String& arg1,
                                   critter& actor);

   int getVisBit(bool lightAdjust = false) const;
   int getMovCost() const { return cur_stats[1]; }
   int getRoomNum() const { return cur_stats[2]; }
   int getIdNum() const { return getRoomNum(); }
   int getFallTo() const { return cur_stats[3]; }
   void setVisBit(int i) { cur_stats[0] = i; }
   void setMovCost(int i) { cur_stats[1] = i; }
   void setRoomNum(int i) { cur_stats[2] = i; }   
   void setFallTo(int i) { cur_stats[3] = i; }
   void setComplete() { room_flags.turn_off(31); }
   void setNotComplete() { room_flags.turn_on(31); }
   

   int isNotComplete() const { return room_flags.get(31); }
   int canCamp() const { return room_flags.get(30); }
   int canSeeSky();
   int isNoMagEntry() const { return room_flags.get(24); }
   int isNoMagExit() const { return room_flags.get(6); }
   int isPermDark() const { return room_flags.get(3); }
   int hasWeather() const { return room_flags.get(4); }
   int isNoMob() const { return room_flags.get(10); }
   int isNoPotion() const { return room_flags.get(11); }
   int isNoMagic() const { return room_flags.get(9); }
   int isNoWand() const { return room_flags.get(12); }
   int isNormallyDark() const { return room_flags.get(14); }
   int isNoMortal() const { return room_flags.get(13); }
   int isNoPK() const { return room_flags.get(8); }
   int isHaven() const { return room_flags.get(7); }
   int isSwamp() const { return room_flags.get(17); }
   int isSmallWater() const { return room_flags.get(15); }
   int isBigWater() const { return room_flags.get(16); }
   int isInUse() const;
   int isNoVehicle() const { return room_flags.get(25); }
   int needsBoat() const { return room_flags.get(19); }
   int needsClimb() const { return room_flags.get(20); }
   int needsDive() const { return room_flags.get(28); }
   int needsFly() const { return room_flags.get(18); }
   int isZlocked() const { return room_flags.get(21); }
   int isCramped() const { return room_flags.get(26); }
   int isNoRanged() const { return room_flags.get(27); }
   int isTotalLoaded() const { return room_flags.get(22); }
   int isNoWanderMob() const { return room_flags.get(33); }
   int isNoWanderForeignMob() const { return room_flags.get(34); }
   int isColiseum() const { return room_flags.get(36); }

   void unlock() { room_flags.turn_off(21); }
   void lock() { room_flags.turn_on(21); }
   void flipFlag(int flg) { room_flags.flip(flg); }
   void setTotalLoaded(int on_off) { room_flags.set(22, on_off); }
   void setFlag(int flg, int posn) { room_flags.set(flg, posn); }
   int getFlag(int flg) { return room_flags.get(flg); }

   const PtrList<critter>& getCrits() { return critters; }

   void resetProcMobs();
   void purgeCritter(int mob_num, critter& pc);
   void purgeObj(object* obj, critter* pc, int do_msg);

   int processInput(String& input); /* for room scripts */

   int sub_a_4_b(critter* crit_ptr, int i_th, const String& name,
                 int see_bit);
   int sub_a_4_b(critter* a, critter* b, int i_th);

   virtual int isVehicle() { return room_flags.get(37);}

   virtual void Clear();
#ifdef USEMYSQL
   virtual void dbRead(int room_num, short read_all);
#endif
   virtual void fileRead(ifstream& da_file, short read_all);
#ifdef USEMYSQL
   virtual void dbWrite();
#endif
   virtual void fileWrite(ofstream& da_file);

   virtual void checkForProc(String& cmd, String& arg1, critter& actor,
                             int targ);

   const String* getRandomExitDir(critter& for_this_pc);
   int getZoneNum();
   virtual void stat(critter& pc);
   virtual void checkLight(int do_msgs = FALSE);
   virtual stat_spell_cell* isAffectedBy(int spell_num) const;

   virtual void normalize(); /* called after OLC to enforce as much state as
                              * possible. */
   virtual object* haveObjNamed(int i_th, const String* name, int see_bit);
   virtual object* haveAccessibleObjNamed(int i_th, const String* name, critter& pc,
                                          int& count_sofar, int& posn);
   virtual critter* haveCritNamed(int i_th, const String* name, int see_bit);
   virtual critter* haveCritNamed(int i_th, const String* name, critter& pc);
   virtual critter* haveCritNamed(int i_th, const String* name, critter& pc,
                                  int& count_sofar);
   virtual critter* haveCritter(critter* ptr);
   virtual object* haveObject(object* ptr);
   virtual critter* getLastCritter() { return critters.peekBack(); }
   virtual critter* findFirstBanker();
   virtual critter* findFirstShopKeeper();
   virtual critter* findFirstTeacher();

   virtual void gainCritter(critter* crit);
   virtual critter* removeCritter(critter* crit);

   virtual void gainObject(object* obj) { gainInv(obj); }
   virtual object* removeObject(object* obj) { return loseInv(obj); }

   virtual void getPetsFor(critter& owner, List<critter*>& rslts);
   virtual int getCritCount(critter& pc);
   // both objs and sobjs counted
   virtual int getObjCount(object& obj);
   virtual int getObjCount(int onum);

   virtual void showCritters(critter& pc);
   virtual void outInv(critter& pc); //show inv to pc (use out_inv method)
   virtual void gainInv(object* obj); //set IN_LIST if needed
   virtual object* loseInv(object* obj);
   virtual void showAllCept(const char* msg, critter* pc = NULL) const;
   virtual void recursivelyLoad(); //mobs and objects

   virtual void doPoofOut(critter& pc);
   virtual void doPoofIn(critter& pc);
   virtual int doScan(critter& pc);
   virtual int doRclear(int new_rm_num);
   virtual void loseObjectFromGame(object& which_un);

   /* this returns the critter if exists.  rm_num is the number of
    * the room we found it in.  Returns NULL if not found.
    * searches entire world. */
   static critter* haveCritNamedInWorld(const int i_th, const String* name,
                                        const int see_bit, int& rm_num);

   /* this returns the critter if exists.  rm_num is the number of
    * the room we found it in.  Returns NULL if not found.
    * searches entire zone. */
   static critter* haveCritNamedInZone(zone& zn, const int i_th,
                                       const String* name, const int see_bit,
                                       int& in_room);


   /* Found in rm_cmds.cc */
   /**  Move all in room out some door.  Does no checks, just puts em
    * through the door, even if it's closed??!!
    */
   int move_all(int i_th, const String* dir);
   int omove_all(int i_th, const String* dir); //objects

   /** No door needed for these. */
   int otransport_all(int dest_rm); //objects
   int transport_all(int dest_rm);

   /**  Move all in room out some door.  Does no checks, just puts em
    * through the door, even if it's closed!!??
    */
   int move(int i_th, const String* pc, int j_th, const String* dir);
   int omove(int i_th, const String* obj, int j_th, const String* dir);

   /** No door needed for these. */
   int transport(int i_th, const String* pc, int dest_rm);
   int otransport(int i_th, const String* obj, int dest_rm); //objects

   /** Echo message into the room in this direction */
   int neighbor_echo(int i_th, const String* dir, const String& buf);

   /** No door needed for this one. */
   int other_room_echo(int dest_rm, const String& buf);

   int tell(int i, const String* name, String& msg);
   int do_tell(const char* msg, critter& targ); /* room tells targ */

   int doOload(int num);
   int doMload(int num);

   int doClose(int i_th, const String* name);

   int com_recho(const String* msg);
   int com_zecho(const String* msg);
   int wizchat(const char* message);
   int rm_pause(int ticks);

   int doEmote(critter& pc, CSelectorColl& includes,
               CSelectorColl& denies, CSentryE cs_entry, ...);
   int vDoEmote(critter& pc, CSelectorColl& includes,
                CSelectorColl& denies, CSentryE cs_entry, va_list argp);

   int makeReadyForAreaSpell();
   critter* findNextProcMob();
   critter* findNextSpellCritter();
}; // class room

#endif
