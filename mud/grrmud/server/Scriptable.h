// $Id: Scriptable.h,v 1.8 1999/08/30 06:30:40 greear Exp $
// $Revision: 1.8 $  $Author: greear $ $Date: 1999/08/30 06:30:40 $

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

#ifndef __SCRIPTABLE_GRRMUD_INCLUDE__
#define __SCRIPTABLE_GRRMUD_INCLUDE__

#include "Serialized.h"
#include "script.h"


/**  This will distill all the methods needed to be able to script.  Objects,
 * rooms, mobs and others may inherit it.
 */
class Scriptable : public Serialized {
private:
   int _cnt;

protected:
   PtrList<GenScript> scripts;
   GenScript* cur_script; // a pointer into the List of MobScripts.
   PtrList<GenScript> pending_scripts;
   int pause;

public:
   Scriptable() : cur_script(NULL), pause(0) { _cnt++; }
   virtual ~Scriptable();

   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);
   virtual int read_v2(istream& dafile); //read in a version 2 script.
   virtual int clear();

   /** Give as much info as possible, used by immortals.  Include client
    * tags if viewer has them enabled.
    */
   virtual void toStringStat(critter* viewer, String& rslt);

   virtual void show(critter* pc);
   virtual void showClient(critter* pc);

   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_SCRIPT_COLL; }

   /**  Run a command after it has been parsed by processInput.  Also called
    * from the script_jump method.
    */
   virtual int executeCommand(String* cooked_strs, int* cooked_ints,
                              int sanity, critter* c_script_owner,
                              room* r_script_owner, int do_sub, int was_ordered);
   virtual void addProcScript(const String& txt, GenScript* script_data);
   /** Does not make a copy of the incoming pointer, it now owns that memory. */
   virtual void addProcScript(GenScript* ptr);
   virtual int removeScript(String& trigger, int i_th, critter& pc);
   virtual void finishedScript();
   virtual int isRunningScript() const { return (cur_script && (cur_script->isInProgress())); }
   virtual const ScriptCmd* getNextScriptCmd() { return cur_script->getNextCommand(); }
   virtual int getPause() const { return pause; }
   virtual void setPause(int i) { pause = i; }
   virtual int decrementPause(int by_what = 1) { if (pause > 0) pause -= by_what; return pause; }
   virtual int incrementPause(int by_what = 1) { pause += by_what; return pause; }

   virtual void doScriptJump(int abs_index);
   virtual int insertNewScript(GenScript* original);
   virtual GenScript* getScriptAt(int idx) { return scripts.elementAt(idx); }

   /** Attempt to trigger a room script directly.  So far, we support only
    * pull and push, but more can easily be added.
    */
   virtual int attemptExecuteUnknownScript(String& cmd, int i_th, String& arg1,
                                           critter* actor);

   virtual void checkForProc(String& cmd, String& arg1, critter* actor,
                             int targ, room* rm, object* obj_actor);

   virtual void listScripts(LEtypeE type, critter* pc);
};//Scriptable


#endif
