// $Id: script.h,v 1.11 1999/07/29 06:35:09 greear Exp $
// $Revision: 1.11 $  $Author: greear $ $Date: 1999/07/29 06:35:09 $

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

///**********************  script.h  ****************************///
// This file contains methods pertaining to mob scripting.  This is where
// new conditional statements, and other modifications should go.
//

#ifndef BEN_SCRIPT_CMDS_H
#define BEN_SCRIPT_CMDS_H

#include "const.h"
#include "classes.h"
#include <PtrArray.h>
#include <KVPair.h>

class room;
class critter;

/**  This will not kill, but can set HP to 1 in worst case. */
int affect_crit_stat(StatTypeE ste, String& up_down, int i_th,
                     String* victim, int dice_cnt, int dice_sides,
                     room* rm, critter* caller = NULL);


/** Used to provide branching in scripts..  The lengths of the
 * cooked* arrays passed in are directly related to the cooked*
 * arrays found in parse.cc. 
 * Branches when the command evaluates to true.
 */
int script_jump_true(String* cooked_strs, int* cooked_ints,
                     critter& script_targ, critter* c_script_owner,
                     room* r_script_owner, int sanity);

int script_jump_false(String* cooked_strs, int* cooked_ints,
                      critter& script_targ, critter* c_script_owner,
                      room* r_script_owner, int sanity);

// The other two call this one with the first argument set accordingly.
int script_jump(int on_test, String* cooked_strs, int* cooked_ints,
                critter& script_targ, critter* c_script_owner,
                room* r_script_owner, int sanity);


///*************************  Conditionals  *************************///

/** used in mob scripts primarily, test to see if the mob owns
 * all of the items listed here.  Can be in inventory or eq.
 */
int does_own(critter& pc, int obj1, int obj2, int obj3, int obj4,
             int obj5, int obj6);

/** Check if the mob's posn is.  */
int is_in_posn(String& posn, critter& pc);

/**  Does a great many comparisons.   Supported fields are:
 *  HP, MANA, MOV, ALIGN
 */
int is_equal_to(int i_th, const String& rhs_critter,
                const String& field, critter& pc);

/**  Does a great many comparisons.  Supported fields are:
 *  HP, MANA, MOV, ALIGN
 */
int is_greater_than(int i_th, const String& rhs_critter,
                    const String& field, critter& pc);

/**  Does a great many comparisons.  Supported fields are:
 *  HP, MANA, MOV, ALIGN
 */
int is_less_than(int i_th, const String& rhs_critter,
                 const String& field, critter& pc);

/** Do the work for the specific comparisons above. */
int do_mob_comparison(int i_th, const String& rhs_critter,
                      const String& field, critter& pc, ComparisonTypeE ct);

/** For instance, is fighting PC, MOB, CLASS, LEVEL. */
int is_fighting(critter& pc, String& discrim);

/** Remove self from battle, use in times of mercy.  Use flee
 * otherwise, to be fair to players.
 */
int stop_battle(critter& pc);

/** "Exit" out of the script, all done. */
int stop_script(critter& pc);

/** Go some random direction. */
int wander(critter& pc);

/** Exact some damage. */
int exact_damage(int dice_cnt, int dice_sides, String& msg, critter& pc);

class ScriptCmd {
private:
   String target;
   String cmd;
   static int _cnt;

public:
   ScriptCmd() { _cnt++; }
   ScriptCmd(const char* t, const char* c) : target(t), cmd(c) { _cnt++; }
   ScriptCmd(const ScriptCmd& src) : target(src.target), cmd(src.cmd) { _cnt++; }
   ~ScriptCmd() { _cnt--; }

   String getTarget() const { return target; }
   String getCommand() const { return cmd; }

   void setTarget(const char* val) { target = val; }
   void setCommand(const char* val) { cmd = val; }

   static int getInstanceCount() { return _cnt; }
};

///********************  Mob Script  ***************************///
 
class GenScript {
private:
   static int _cnt;

protected:
   int precedence;
   String trigger_cmd;
   int target;
   int actor;
   int next_lbl_num;

   String trig_discriminator;
   PtrArray<ScriptCmd> script_cmds;  // read/write/edit this one
   PtrArray<ScriptCmd> compiled_cmds; // compile into this one
   PtrArray<ScriptCmd> running_cmds; /* generate each time script is needed from
                                      * compiled_cmds above.
                                      */

   int stack_ptr; //points to the next command to be executed.
   int needs_compiling; //internal state information, transient

   virtual void parseBlockFP(int& start_idx, const PtrArray<ScriptCmd>& incomming,
                             PtrArray<ScriptCmd>& rslts);

   virtual void optimizeLabels(const PtrArray<ScriptCmd>& incomming,
                               PtrArray<ScriptCmd>& rslts);

   virtual String getNextLabel();

   virtual int findOffset(List<KVPair<String, int>*>& lst, const String& str);

public:
   static char* triggers[];

   GenScript();
   GenScript(String& trig, int targ, int act, String& discriminator,
             int precedence);
   GenScript(const GenScript& src);
   virtual ~GenScript();

   virtual void clear();
   virtual void appendCmd(String& cmd);
   virtual void read(ifstream& da_file);
   virtual void write(ofstream& da_file) const;
   virtual int matches(const String& cmd, String& arg1, critter& act,
                       int targ, int obj_actor_num = -1);
   virtual int matches(const GenScript& src);
   virtual void generateScript(String& cmd, String& arg1, critter& act,
                               int targ, room& rm, critter* script_owner,
                               object* object_owner = NULL);
   virtual int getPrecedence() { return precedence; }
   virtual void compile(); //compile into script assembly...
   virtual void setScript(const String& txt);
   virtual const String* getTrigger() { return &trigger_cmd; }
   virtual String toStringBrief(int client_format, int mob_num,
                                entity_type entity, int idx);

   /** Source code. */
   virtual String getScript();

   /** Object code, % substitutions not made yet. */
   virtual String getCompiledScript();

   /**  This gives the compiled, running version.  Think of it
    * as machine code!
    */
   virtual String getRunningScript();

   virtual GenScript& operator=(const GenScript& src);

   virtual void doScriptJump(int abs_offset);
   virtual const ScriptCmd* getNextCommand();

   virtual void resetStackPtr() { stack_ptr = 0; } //get ready to start 'running'

   virtual void clean(); // get rid of running commands.

   virtual int isInProgress() { return (stack_ptr >= 0); }

   static int getInstanceCount() { return _cnt; }
   static int validTrigger(const char* trig);
   static int checkScript(const String& str);
};//GenScript


class MobScript : public GenScript {
private:
   static int _cnt;

protected:

public:
   MobScript() : GenScript() { _cnt++; };
   MobScript(String& trig, int targ, int act, String& discriminator,
             int precedence) : GenScript(trig, targ, act,
                                         discriminator, precedence) {
      _cnt++;
   }

   MobScript(const MobScript& src) : GenScript((GenScript)src) { _cnt++; }

   virtual ~MobScript() { _cnt--; }

   MobScript& operator=(const MobScript& src) {
      if (this != &src) {
         *((GenScript*)(this)) = (GenScript)(src);
      }
      return *this;
   }

   static int getInstanceCount() { return _cnt; }
   static int parseScriptCommand(ScriptCmd& cmd, critter& owner);
};//MobScript


class RoomScript : public GenScript {
private:
   static int _cnt;

protected:

public:
   RoomScript() : GenScript() { _cnt++; };
   RoomScript(String& trig, int targ, int act, String& discriminator,
             int precedence) : GenScript(trig, targ, act,
                                         discriminator, precedence) {
      _cnt++;
   }

   RoomScript(const RoomScript& src) : GenScript((GenScript)src) { _cnt++; }

   virtual ~RoomScript() { _cnt--; }

   RoomScript& operator=(const RoomScript& src) {
      if (this != &src) {
         *((GenScript*)(this)) = (GenScript)(src);
      }//if
      return *this;
   }

   static int getInstanceCount() { return _cnt; }
   static int parseScriptCommand(ScriptCmd& cmd, room& owner);
};//RoomScript


class ObjectScript : public GenScript {
private:
   static int _cnt;

protected:

public:
   ObjectScript() : GenScript() { _cnt++; };
   ObjectScript(String& trig, int targ, int act, String& discriminator,
             int precedence) : GenScript(trig, targ, act,
                                         discriminator, precedence) {
      _cnt++;
   }

   ObjectScript(const ObjectScript& src) : GenScript((GenScript)src) { _cnt++; }

   virtual ~ObjectScript() { _cnt--; }

   ObjectScript& operator=(const ObjectScript& src) {
      if (this != &src) {
         *((GenScript*)(this)) = (GenScript)(src);
      }//if
      return *this;
   }

   static int getInstanceCount() { return _cnt; }
   static int parseScriptCommand(ScriptCmd& cmd, object& owner);
};//ObjectScript


/**  This will distill all the methods needed to be able to script.  Objects,
 * rooms, mobs and others may inherit it.
 */
class Scriptable {
protected:
   List<GenScript*> scripts;
   GenScript* cur_script; // a pointer into the List of MobScripts.
   List<GenScript*> pending_scripts;
   int pause;

public:
   /** For scripts, the script_targ is always *this.  The script_owner
    * will be null (for non-script specific stuff) and should be specified
    * if it relates to scripting of course.
    */
   int processInput(String& input, short do_sub, int script_driven, 
                    critter* c_script_owner = NULL,
                    room* r_script_owner = NULL, int was_ordered = FALSE);
   int processInput(String& input);
   int processInput(String& input, room* rm); /* for object scripts */


   /**  Run a command after it has been parsed by processInput.  Also called
    * from the script_jump method.
    */
   int executeCommand(String* cooked_strs, int* cooked_ints,
                      int sanity, critter* c_script_owner,
                      room* r_script_owner, int do_sub, int was_ordered);

   void addProcScript(const String& txt, GenScript* script_data);
   void removeScript(String& trigger, int i_th, critter& pc);
   void finishedScript();

   int isRunningScript() const { return (cur_script && (cur_script->isInProgress())); }

   const ScriptCmd* getNextScriptCmd() { return cur_script->getNextCommand(); }

   int getPause() const { return pause; }
   void setPause(int i) { pause = i; }

   void doScriptJump(int abs_index);
   int insertNewScript(MobScript* script);
   GenScript* getScriptAt(int idx) { return scripts.elementAt(idx); }

   /** Attempt to trigger a room script directly.  So far, we support only
    * pull and push, but more can easily be added.
    */
   int attemptExecuteUnknownScript(String& cmd, int i_th, String& arg1,
                                   critter* actor);

   void listScripts(critter* pc);

};//Scriptable


#endif
