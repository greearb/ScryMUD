// $Id: Scriptable.cc,v 1.8 1999/09/08 06:11:36 greear Exp $
// $Revision: 1.8 $  $Author: greear $ $Date: 1999/09/08 06:11:36 $

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
#include "misc.h"
#include "battle.h"


Scriptable::~Scriptable() {
   scripts.clearAndDestroy();
   pending_scripts.clearAndDestroy();
}

void Scriptable::finishedScript() {
   if (cur_script) {
      pending_scripts.loseData(cur_script);
      delete cur_script;
   }

   // This could easily end up being NULL, that's OK!
   cur_script = pending_scripts.peekFront();
}//finishedRoomProc


/** Adds/replaces a script matching script_data, and sets the text of
 * that script to txt.
 */
void Scriptable::addProcScript(const String& txt, GenScript* script_data) {
   //similar to reading it in...
   //first, see if we are over-writing one...
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In object::addProcScript, txt:  \n" << txt
             << "\nscript data:  "
             << script_data->toStringBrief(0, 0) << endl;
   }

   Cell<GenScript*> cll;
   GenScript* ptr;
   scripts.head(cll);

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

   scripts.append(script_data);
}//addProcScript


/** Does not make a copy of the incoming pointer, it now owns that memory,
 * replaces a script if there is one that matches it already.
 */
void Scriptable::addProcScript(GenScript* script_data) {
   Cell<GenScript*> cll;
   GenScript* ptr;
   scripts.head(cll);

   while ((ptr = cll.next())) {
      if (ptr->matches(*script_data)) {
         //got a match.
         mudlog.log("object::addProcScript, they match.");
         *ptr = *script_data;
         delete script_data;
         return;
      }//if
   }//while
   
   scripts.append(script_data);
}//addProcScript


void Scriptable::listScripts(critter* pc) {
   String buf(300);
   toStringStat(pc, buf);
   pc->show(buf);
}//listScripts


int Scriptable::statScript(int script_index, critter* pc) {
   String buf(500);
 
   GenScript* ptr = scripts.elementAt(script_index);
   if (ptr) {
      buf.append(ptr->toStringBrief(pc->isUsingClient(), script_index));
      if (pc->isUsingClient())
         buf.append("\n<SCRIPT_DATA>");
      else
         buf.append("\n");
      buf.append(ptr->getScript());
      
      if (pc->isUsingClient())
         buf.append("</SCRIPT_DATA>\nCompiled Script:\n");
      else
         buf.append("\nCompiled Script:\n");
      
      buf.append(ptr->getCompiledScript());
      
      pc->show(buf);
      return 0;
   }//if

   Entity* e_this = (Entity*)(this);
   Sprintf(buf, "%S[%i] does not have a script of index: %i",
           e_this->getName(), e_this->getIdNum(), script_index);
   pc->show(buf);
   return -1;
}//statScript

/** Give as much info as possible, used by immortals.  Include client
 * tags if viewer has them enabled.
 */
void Scriptable::toStringStat(critter* viewer, String& rslt) {
   String buf(500);

   rslt.clear();

   String tmp(100);
   int found_one = FALSE;
   Cell<GenScript*> cll(scripts);
   GenScript* ptr;
   int idx = 0;
   while ((ptr = cll.next())) {
      found_one = TRUE;
      tmp = ptr->toStringBrief(FALSE, idx);
      Sprintf(buf, "[%i] %S\n", idx, &(tmp));
      rslt.append(buf);
      idx++;
   }

   if (!found_one) {
      rslt.append("No scripts defined for this entity.\n");
   }
}//toStringStat


int Scriptable::removeScript(String& trigger, int i_th, critter& pc) {
   int sofar = 1;
   String buf(500);
 
   Cell<GenScript*> cll(scripts);
   GenScript* ptr;
   ptr = cll.next();
   while (ptr) {
      if (strcasecmp(*(ptr->getTrigger()), trigger) == 0) {
         if (sofar == i_th) {
            delete ptr;
            ptr = scripts.lose(cll);
            pc.show("Deleted it...\n");
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

   pc.show("Didn't find that script..\n");
   return -1;
}//removeScript


int Scriptable::insertNewScript(GenScript* script) {
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


void Scriptable::doScriptJump(int abs_offset) {
   if (cur_script)
      cur_script->doScriptJump(abs_offset);
}

/** This is the v_03 version.  The v_02 version will have to be quite
 * different...
 */
int Scriptable::read(istream& ofile, int read_all = TRUE) {
   int sent_;
   char tmp[100];

   Scriptable::clear();

   GenScript* ptr = NULL;

   MetaTags mt(ofile);

   ofile >> pause;
   
   ofile >> sent_;
   mudlog << "Scriptable::read, got pause: " << pause << " sent_: "
          << sent_ << endl;
   while (sent_ != -1) {
      if (mudlog.ofLevel(DB))
         mudlog << "\nReading script# " << sent_ << endl;
      if (!ofile) {
         if (mudlog.ofLevel(ERR)) {
            mudlog << "ERROR:  reading script da_file FALSE." << endl;
         }
         return -1;
      }

      ptr = new GenScript();
      ptr->read(ofile);
      scripts.append(ptr);
      ofile >> sent_;
      ofile.getline(tmp, 80);
      if (mudlog.ofLevel(DB))
         mudlog << "Got rest of line -:" << tmp << ":-" << endl;
   }//while
   ofile.getline(tmp, 80);
   if (mudlog.ofLevel(DB))
      mudlog << "Got rest of line -:" << tmp << ":-" << endl;
   return 0;
}//read


int Scriptable::write(ostream& ofile) {

   // Write out the meta data
   MetaTags mt(*this);
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
   return 0;
}//write

void Scriptable::clear() {
   scripts.clearAndDestroy();
   pending_scripts.clearAndDestroy();
}


// NOTE:  The script owner is *this.  It is likely, but not necessary
// that targ is also *this
void Scriptable::checkForProc(String& cmd, String& arg1, critter* actor,
                              int targ, room* rm, object* obj_actor) {
   if (mudlog.ofLevel(DBG)) {
      mudlog << "In object::checkForProc, size of scripts: "
             << scripts.size() << endl;
   }

   Cell<GenScript*> cll;
   GenScript* ptr;
         
   scripts.head(cll);

   Entity* e_this = (Entity*)(this);

   int obj_actor_num = -1;
   if (obj_actor)
      obj_actor_num = obj_actor->getIdNum();

   while ((ptr = cll.next())) {
      mudlog.log("In while.");
      mudlog.log(ptr->toStringBrief(0, 0));
      if (ptr->matches(cmd, arg1, *actor, targ, obj_actor_num)) {
         mudlog.log("Matches..");
         if (pending_scripts.size() >= 10) { //only queue 10 scripts
            return; //do nothing, don't want to get too much backlog.
         }
         else {
            // add it to the pending scripts.
            // TODO:  Need to fix this probably, cast to specific object
            // type at least, based on getEntityId?? --Ben
            ptr->generateScript(cmd, arg1, *actor, targ, *rm, e_this);
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
               scripting_entities.gainData(this);
               cur_script->resetStackPtr(); //get ready to start
            }

            return;
         }//else
      }//if matches
   }//while

   if (getEntityType() == LE_CRITTER) {
      critter* c_this = (critter*)(this);
      // Critter specific.
      // Look through all objects the person is using.
      for (int i = 1; i < MAX_EQ; i++) {
         if (mudlog.ofLevel(DBG2)) {
            mudlog << "Critter [" << c_this->getIdNum() << "] " 
                   << *(c_this->getName()) << ":  Checking EQ[" 
                   << i << "] == " << c_this->EQ[i] << ", in rm: "
                   << rm->getIdNum() << endl;
         }
         if (c_this->EQ[i] && (c_this->EQ[i]->hasScript())) {
            if (mudlog.ofLevel(DBG2)) 
               mudlog << "Found an object with a script: EQ[" << i << "]\n";
            // make it modified, if it is not already so.
            if (!(c_this->EQ[i]->isModified())) {
               c_this->EQ[i] = obj_to_sobj(*(c_this->EQ[i]), c_this);
            }
            c_this->EQ[i]->checkForProc(cmd, arg1, actor, targ, rm, c_this->EQ[i]);
         }//if
      }//for
   }//if we are really a critter

   // TODO:  Check for objs in inventory as well (for rooms, objs, and critters??)
}//checkForProc


/** Attempt to trigger a room script directly.  So far, we support only
 * pull and push, but more can easily be added.  Came from room originally.
 */
int Scriptable::attemptExecuteUnknownScript(String& cmd, int i_th, String& arg1,
                                            critter* actor) {

   int targ;
   targ = i_th = -1; //use i_th to quiet the compiler.
   
   Cell<GenScript*> rcll(scripts);
   GenScript* rptr;

   int idx = 0;
   Entity* e_this = (Entity*)(this);

   while ((rptr = rcll.next())) {
      if (mudlog.ofLevel(DBG)) {
         mudlog << "attemptExecuteUnknownScript, found script: " 
                << rptr->toStringBrief(0, idx) << endl;
      }
      if (rptr->matches(cmd, arg1, *actor, targ)) {
         mudlog.log("Script matches..\n");
         if (pending_scripts.size() >= 10) { //only queue 10 scripts
            if (mudlog.ofLevel(WRN)) {
               mudlog << "WARNING:  pending_scripts >= 10 in entity#: "
                      << e_this->getIdNum() << " entity type: " << getEntityType()
                      << endl;
            }
            actor->show("Please try again in a bit.\n");
            return 0; //do nothing, don't want to get too much backlog.
         }
         else {
            // add it to the pending scripts.
            mudlog.log("Generating script.\n");
            rptr->generateScript(cmd, arg1, *actor, targ,
                                 room_list[e_this->getCurRoomNum()],
                                 e_this);

            mudlog.log("Inserting new script.\n");
            insertNewScript(new GenScript(*rptr));

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
               scripting_entities.gainData(this);
               cur_script->resetStackPtr(); //get ready to start
            }

            return 0;
         }//else
      }//if matches
      idx++;
   }//while
   return -1; //didn't find anything that matched
}//attemptExecuteUnknownScript
