// $Id: Scriptable.cc,v 1.3 1999/08/25 06:35:11 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date: 1999/08/25 06:35:11 $

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


#include "Scriptable.h"


void Scriptable::finishedObjProc() {
   if (cur_script) {
      pending_scripts.loseData(cur_script);
      delete cur_script;
   }

   // This could easily end up being NULL, that's OK!
   cur_script = pending_scripts.peekFront();
}//finishedRoomProc


void Scriptable::addProcScript(const String& txt, GenScript* script_data) {
   //similar to reading it in...
   //first, see if we are over-writing one...
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In object::addProcScript, txt:  \n" << txt
             << "\nscript data:  "
             << script_data->toStringBrief(0, 0, ENTITY_ROOM, 0) << endl;
   }

   Cell<GenScript*> cll;
   GenScript* ptr;
   obj_proc_scripts.head(cll);

   while ((ptr = cll.next())) {
      if (ptr->matches(*script_data)) {
         //got a match.
         mudlog.log("object::addProcScript, they match.");
         *ptr = *script_data;
         ptr->setScript(txt);
         delete script_data;
         return;
      }//if
   }//while
   
   mudlog.log(DBG, "About to setScript.");
   
   script_data->setScript(txt);
   mudlog.log(DBG, "done with setScript.");

   if (!script_data) {
      mudlog.log(ERR, "script_data is NULL, object::addProcScript.");
      return;
   }

   obj_proc_scripts.append(script_data);
}//addProcScript


void Scriptable::listScripts(LEtypeE type, critter& pc) {
   String buf(500);

   String tmp(100);
   int found_one = FALSE;
   Cell<GenScript*> cll(scripts);
   GenScript* ptr;
   int idx = 0;
   while ((ptr = cll.next())) {
      found_one = TRUE;
      tmp = ptr->toStringBrief(FALSE, 0, type, idx);
      Sprintf(buf, "[%i] %S\n", idx, &(tmp));
      pc.show(buf);
      idx++;
   }

   if (!found_one) {
      buf.Append("No scripts defined for this object.\n");
      show(buf, pc);
   }
}//listScripts

int Scriptable::removeScript(String& trigger, int i_th, critter& pc) {
   int sofar = 1;
   String buf(500);
 
   Cell<GenScript*> cll(obj_proc_scripts);
   GenScript* ptr;
   ptr = cll.next();
   while (ptr) {
      if (strcasecmp(*(ptr->getTrigger()), trigger) == 0) {
         if (sofar == i_th) {
            delete ptr;
            ptr = obj_proc_scripts.lose(cll);
            show("Deleted it...\n", pc);
            return 0;
         }//if
         else {
            ptr = cll.next();
         }
         sofar++;
      }//if
      else {
         ptr = cll.next();
      }
   }//while

   show("Didn't find that script..\n", pc);
   return -1;
}//removeScript


int Scriptable::insertNewScript(ObjectScript* script) {
   GenScript* ptr;
   Cell<GenScript*> cll(pending_scripts);

   // Don't append scripts that have a zero precedence, if there
   // are other scripts in the queue.
   if ((script->getPrecedence() == 0) && (!pending_scripts.isEmpty())) {
      delete script;
      return 0;
   }

   while ((ptr = cll.next())) {
      if (ptr->getPrecedence() < script->getPrecedence()) {
         // Then insert it
         pending_scripts.insertBefore(cll, script);
         return 0;
      }//if
   }//while

   // If here, then we need to place it at the end.
   pending_scripts.append(script);
   return 0;
}


void GenScript::doScriptJump(int abs_offset) {
   if (cur_script)
      cur_script->doScriptJump(abs_offset);
}

int Scriptable::read(istream& ofile, int read_all = TRUE) {
   int sent_;
   int script_type;

   GenScript* ptr = NULL;

   ofile >> script_type;

   MetaTags mt;
   mt.read(ofile);

   ofile >> pause >> endl;

   ofile >> sent_;
   while (sent_ != -1) {
      if (mudlog.ofLevel(DB))
         mudlog << "\nReading script# " << sent_ << endl;
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  reading script da_file FALSE." << endl;
         }
         return;
      }

      switch (script_type) {
      case LE_OSCRIPT:
         ptr = new ObjectScript(); break;
      case LE_MSCRIPT:
         ptr = new MobScript(); break;
      case LE_RSCRIPT:
         ptr = new RoomScript(); break;
      default:
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  unknown type in Scriptable::read, type: "
                   << sent_ << endl;
         }
         return -1;
      }//switch

      ptr->read(ofile);
      obj_proc_scripts.append(ptr);
      ofile >> sent_;
      ofile.getline(tmp, 80);
      if (mudlog.ofLevel(DB))
         mudlog << "Got rest of line -:" << tmp << ":-" << endl;
   }//while
}//read


int Scriptable::write(ostream& ofile) {
   if (!scripts.isEmpty()) {
      ofile << scripts.peekFront()->getEntityType();
   }
   else {
      ofile << LE_UNKNOWN;
   }

   // Write out the meta data
   MetaTags mt(this);
   mt.write(ofile);

   ofile << pause << endl;

   Cell<GenScript*> cll;
   scripts.head(cll);
   GenScript* ptr;

   int i = 1;
   while ((ptr = cll.next())) {
      ofile << i++ <<  "  Start of a room proc script\n";
      ptr->write(ofile);
   }
      
   ofile << "-1  End of room proc scripts" << endl;
}//write


// NOTE:  The script owner is *this.  It is likely, but not necessary
// that targ is also *this
void Scriptable::checkForProc(String& cmd, String& arg1, critter* actor,
                              int targ, room* rm) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In object::checkForProc, size of scripts: "
             << obj_proc_scripts.size() << endl;
   }

   if (!isModified()) {
      mudlog.log(ERR, "ERROR:  object::checkForProc, got an OBJ (not modified).");
      return;
   }

   Cell<GenScript*> cll;
   GenScript* ptr;
         
   obj_proc_scripts.head(cll);

   while ((ptr = cll.next())) {
      mudlog.log("In while.");
      mudlog.log(ptr->toStringBrief(0, 0, ENTITY_OBJECT, 0));
      if (ptr->matches(cmd, arg1, actor, targ, getIdNum())) {
         mudlog.log("Matches..");
         if (pending_scripts.size() >= 10) { //only queue 10 scripts
            return; //do nothing, don't want to get too much backlog.
         }
         else {
            // add it to the pending scripts.
            // TODO:  Need to fix this probably, cast to specific object
            // type at least, based on getEntityId?? --Ben
            ptr->generateScript(cmd, arg1, actor, targ, rm, NULL, this);
            insertNewScript(ptr);

            if (cur_script) {
               if (cur_script->getPrecedence() <
                   pending_scripts.peekFront()->getPrecedence()) {
                  
                  pending_scripts.loseData(cur_script); //take it out of queue
                  delete cur_script; //junk it!
                  cur_script = pending_scripts.peekFront();
                  cur_script->resetStackPtr(); //get ready to start
               }//if
               // else, it just goes into the queue
            }//if we currently have a script.
            else { //there was none, so grab the first one.
               cur_script = pending_scripts.peekFront();
               proc_action_objs.gainData(this);
               cur_script->resetStackPtr(); //get ready to start
            }

            return;
         }//else
      }//if matches
   }//while

   // TODO, came from critter class.
   // Look through all objects the person is using.
   for (int i = 1; i < MAX_EQ; i++) {
      if (mudlog.ofLevel(DBG2)) {
         mudlog << "Critter [" << getIdNum() << "] " << *(getName())
                << ":  Checking EQ[" << i << "] == " << EQ[i] << ", in rm: "
                << rm.getIdNum() << endl;
      }
      if (EQ[i] && (EQ[i]->hasScript())) {
         if (mudlog.ofLevel(DBG2)) 
            mudlog << "Found an object with a script: EQ[" << i << "]\n";
         // make it modified, if it is not already so.
         if (!(EQ[i]->isModified())) {
            EQ[i] = obj_to_sobj(*(EQ[i]), &inv, rm.getIdNum());
         }
         EQ[i]->checkForProc(cmd, arg1, actor, targ, rm);
      }//if
   }//for

}//checkForProcAction


int Scriptable::insertNewScript(MobScript* script) {

   // Don't append scripts that have a zero precedence, if there
   // are other scripts in the queue.
   if ((script->getPrecedence() == 0) && (!pending_scripts.isEmpty())) {
      delete script;
      return 0;
   }

   GenScript* ptr;
   Cell<GenScript*> cll(pending_scripts);

   while ((ptr = cll.next())) {
      if (ptr->getPrecedence() < script->getPrecedence()) {
         // Then insert it
         pending_scripts.insertBefore(cll, script);
         return 0;
      }//if
   }//while

   // If here, then we need to place it at the end.
   pending_scripts.append(script);
   return 0;
}


/** Attempt to trigger a room script directly.  So far, we support only
 * pull and push, but more can easily be added.
 */
int Scriptable::attemptExecuteUnknownScript(String& cmd, int i_th, String& arg1,
                                            critter* actor) {

   int targ;
   targ = i_th = -1; //use i_th to quiet the compiler.
   
   // Now, check for room procs!!
   Cell<RoomScript*> rcll;
   RoomScript* rptr;
         
   room_proc_scripts.head(rcll);

   int idx = 0;
   while ((rptr = rcll.next())) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "room::attemptExecuteUnknownScript, found room script: " 
                << rptr->toStringBrief(0, getIdNum(), ENTITY_ROOM, idx) << endl;
      }
      if (rptr->matches(cmd, arg1, actor, targ)) {
         mudlog.log("Script matches..\n");
         if (pending_scripts.size() >= 10) { //only queue 10 scripts
            actor.show("Please try again in a bit.\n");
            return 0; //do nothing, don't want to get too much backlog.
         }
         else {
            // add it to the pending scripts.
            mudlog.log("Generating script.\n");
            rptr->generateScript(cmd, arg1, actor, targ, *this, NULL);

            mudlog.log("Inserting new script.\n");
            insertNewScript(new RoomScript(*rptr));

            if (cur_script) {
               mudlog.log("Had a cur_script.\n");
               if (cur_script->getPrecedence() <
                   pending_scripts.peekFront()->getPrecedence()) {
                  
                  mudlog.log("Junking cur_script because of precedence.\n");
                  pending_scripts.loseData(cur_script); //take it out of queue
                  delete cur_script; //junk it!
                  cur_script = pending_scripts.peekFront();
                  cur_script->resetStackPtr(); //get ready to start
               }//if
               // else, it just goes into the queue
            }//if we currently have a script.
            else { //there was none, so grab the first one.
               mudlog.log("Was no cur_script, taking top of pending stack.\n");
               cur_script = pending_scripts.peekFront();
               proc_action_rooms.gainData(this);
               cur_script->resetStackPtr(); //get ready to start
            }

            return 0;
         }//else
      }//if matches
      idx++;
   }//while
   return -1; //didn't find anything that matched
}//attemptExecuteUnknownScript


