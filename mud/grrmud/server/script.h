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
//#include "room.h"

class room;

/** Used to provide branching in scripts..  The lengths of the
 * cooked* arrays passed in are directly related to the cooked*
 * arrays found in parse.cc. 
 * Branches when the command evaluates to true.
 */
void script_jump_true(String* cooked_strs, int* cooked_ints,
                      critter& script_targ, critter& script_owner);

void script_jump_false(String* cooked_strs, int* cooked_ints,
                       critter& script_targ, critter& script_owner);

// The other two call this one with the first argument set accordingly.
void script_jump(int on_test, String* cooked_strs, int* cooked_ints,
                 critter& script_targ, critter& script_owner);


///*************************  Conditionals  *************************///

/** used in mob scripts primarily, test to see if the mob owns
 * all of the items listed here.  Can be in inventory or eq.
 */
int does_own(critter& pc, int obj1, int obj2, int obj3, int obj4,
             int obj5, int obj6);

/** Check if the mob's posn is.  */
int is_in_posn(critter& pc, String& posn);

/**  Does a great many comparisons. */
int does_equal(critter& pc, String& rhs, String& rhs);

/**  Does a great many comparisons. */
int is_greater_than(critter& pc, String& lhs, String& rhs);

/** For instance, is fighting PC, MOB, CLASS, LEVEL. */
void is_fighting(critter& pc, String& discrim);

/** Remove self from battle, use in times of mercy.  Use flee
 * otherwise, to be fair to players.
 */
void stop_battle(critter& pc);

/** "Exit" out of the script, all done. */
void stop_script(critter& pc);

/** Go some random direction. */
void wander(critter& pc);


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
 
class MobScript {
private:
   static int _cnt;

protected:
   int takes_precedence;
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

   void parseBlockFP(int& start_idx, const PtrArray<ScriptCmd>& incomming,
                     PtrArray<ScriptCmd>& rslts);

   void optimizeLabels(const PtrArray<ScriptCmd>& incomming,
                       PtrArray<ScriptCmd>& rslts);

   String getNextLabel();

   int findOffset(List<KVPair<String, int>*>& lst, const String& str);

public:
   static char* triggers[];

   MobScript();
   MobScript(String& trig, int targ, int act, String& discriminator,
             int takes_precedence);
   MobScript(const MobScript& src);
   ~MobScript();

   void clear();
   void appendCmd(String& cmd);
   void read(ifstream& da_file);
   void write(ofstream& da_file) const;
   int matches(const String& cmd, String& arg1, critter& act,
               int targ);
   int matches(const MobScript& src);
   void generateScript(String& cmd, String& arg1, critter& act,
                       int targ, room& rm, critter* script_owner);
   int takesPrecedence() { return takes_precedence; }
   void compile(); //compile into script assembly...
   static int validTrigger(const char* trig);
   static int checkScript(const String& str);
   void setScript(const String& txt);
   const String* getTrigger() { return &trigger_cmd; }
   String toStringBrief(int client_format, int mob_num);

   /** Source code. */
   String getScript();

   /** Object code, % substitutions not made yet. */
   String getCompiledScript();

   /**  This gives the compiled, running version.  Think of it
    * as machine code!
    */
   String getRunningScript();

   MobScript& operator=(const MobScript& src);

   void doScriptJump(int abs_offset);
   const ScriptCmd* getNextCommand();

   void resetStackPtr() { stack_ptr = 0; } //get ready to start 'running'

   void clean(); // get rid of running commands.

   int isInProgress() { return (stack_ptr >= 0); }
   static int getInstanceCount() { return _cnt; }
   static void parseScriptCommand(ScriptCmd& cmd, critter& owner);
};//MobScript


#endif
