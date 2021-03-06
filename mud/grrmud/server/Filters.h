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

///********************  Filters.h  *****************************///



#ifndef __FILTERS_INCLUDE
#define __FILTERS_INCLUDE

#include "critter.h"

// Use all variables to make the compiler warnings go away.
//#define USE_VARS  if (!(1 || &pc || b)) return FALSE
#define USE_VARS  (void)(pc); (void)(b);

/** Matches all. */
class CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* b) { USE_VARS; return TRUE; }
   virtual const char* name() { return "CritterSelector"; }
   virtual ~CritterSelector() { };
};

/** Matches None. */
class SelectNone : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* b) { USE_VARS; return FALSE; }
   virtual const char* name() { return "SelectNone"; }
   virtual ~SelectNone() { };
};

/** matches when pc is an NPC */
class SelectNPC : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* b) {
      USE_VARS; return pc->isNPC();
   }
   virtual const char* name() { return "SelectNPC"; }
   virtual ~SelectNPC() { };
};

/** matches when pc != b */
class SelectAreSame : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* actor) {
      return pc == actor;
   }
   virtual const char* name() { return "SelectAreSame"; }
   virtual ~SelectAreSame() { };
};

/** matches when pc is sleeping */
class SelectIsSleeping : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* b) {
      USE_VARS; return pc->isSleeping();
   }
   virtual const char* name() { return "SelectIsSleeping"; }
   virtual ~SelectIsSleeping() { };
};


/** matches when pc is meditating */
class SelectIsMeditating : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* b) {
      USE_VARS; return pc->isMeditating();
   }
   virtual const char* name() { return "SelectIsMeditating"; }
   virtual ~SelectIsMeditating() { };
};

/** matches when pc is an NPC and is possessed. */
class SelectNPC_Possessed : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* b) {
      USE_VARS; return (pc->isNPC() && pc->isPossessed());
   }
   virtual const char* name() { return "SelectNPC_Possessed"; }
   virtual ~SelectNPC_Possessed() { };
};

/** matches when pc is a PC. */
class SelectPC : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* b) {
      USE_VARS; return pc->isPc();
   }
   virtual const char* name() { return "SelectPC"; }
   virtual ~SelectPC() { };
};

/** matches mobs who MAY SEE mob entry message. */
class SelectActorEntryMsgs : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* actor) {
      return (actor->isPc() || (pc->pc && !pc->PC_FLAGS.get(28)));
   }
   virtual const char* name() { return "SelectActorEntryMsgs"; }
   virtual ~SelectActorEntryMsgs() { };
};

/** matches b if pc can see it. */
class SelectVisibleToActor : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* actor) {
      return actor->canDetect(*pc);
   }
   virtual const char* name() { return "SelectVisibleToActor"; }
   virtual ~SelectVisibleToActor() { };
};

/** matches b if pc can see it. */
class SelectCanDetectActor : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* actor) {
      return pc->canDetect(*actor);
   }
   virtual const char* name() { return "CanDetectActor"; }
   virtual ~SelectCanDetectActor() { };
};

/** returns true if pc cannot detect b's sneaking. */
class SelectActorSneakWorked : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* actor);
   virtual const char* name() { return "SelectActorSneakWorked"; }
   virtual ~SelectActorSneakWorked() { };
};


/** returns true if pc should get extra info (imm only for now) */
class SelectGetsInfo : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* b) {
      USE_VARS; return (pc->isPc() && pc->PC_FLAGS.get(14));
   }
   virtual const char* name() { return "SelectGetsInfo"; }
   virtual ~SelectGetsInfo() { };
};


/** returns true if pc is using the Hegemon Client */
class SelectUsingClient : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* b) {
      USE_VARS; return (pc->isUsingClient());
   }
   virtual const char* name() { return "SelectUsingClient"; }
   virtual ~SelectUsingClient() { };
};

/** returns true if pc is NOT using the Hegemon Client */
class SelectNotUsingClient : public CritterSelector {
public:
   virtual int matches(const critter* pc, const critter* b) {
      USE_VARS; return (!pc->isUsingClient());
   }
   virtual const char* name() { return "SelectNotUsingClient"; }
   virtual ~SelectNotUsingClient() { };
};


class CSelectorColl {
public:
   enum SelectorRule {
      SELECTOR_AND,
      SELECTOR_OR
   };

protected:
   ObjArray<CritterSelector*> selections;
   SelectorRule select_rule;

public:
   CSelectorColl() : select_rule(SELECTOR_OR) { }
   CSelectorColl(SelectorRule rule, CritterSelector* c1 = NULL,
                 CritterSelector* c2 = NULL, CritterSelector* c3 = NULL,
                 CritterSelector* c4 = NULL, CritterSelector* c5 = NULL,
                 CritterSelector* c6 = NULL) {
      select_rule = rule;
      addSelector(c1);
      addSelector(c2);
      addSelector(c3);
      addSelector(c4);
      addSelector(c5);
      addSelector(c6);
   }//constructor

   ~CSelectorColl() { };

   void setRule(SelectorRule rule) { select_rule = rule; }

   int matches(critter* pc_, critter* other = NULL) const;

   /** Does NOT make a deep copy of incomming data. */
   void addSelector(CritterSelector* selector) {
      if (selector)
         selections.addElement(selector);
   }//addSelector

private:
   CSelectorColl& operator=(const CSelectorColl& src);
   CSelectorColl(const CSelectorColl& src);
};


/** Just collects our filters for us. */
class Selectors {
protected:
   Selectors();

public:
   CritterSelector selectAll;
   SelectNone selectNone;
   SelectCanDetectActor selectCanDetectActor;
   SelectNPC selectNPC;
   SelectNPC_Possessed selectNPC_Possessed;
   SelectActorEntryMsgs selectActorEntryMsgs;
   SelectAreSame selectAreSame;
   SelectActorSneakWorked selectActorSneakWorked;
   SelectIsSleeping selectIsSleeping;
   SelectIsMeditating selectIsMeditating;
   SelectGetsInfo selectGetsInfo;
   SelectUsingClient selectUsingClient;
   SelectNotUsingClient selectNotUsingClient;

   CSelectorColl CC_all;
   CSelectorColl CC_none;
   CSelectorColl CC_mob_entry_allow;
   CSelectorColl CC_mob_entry_deny;
   CSelectorColl CC_gets_info_allow;
   CSelectorColl CC_using_client;
   CSelectorColl CC_not_using_client;

   static Selectors& instance();

private:
   Selectors& operator=(const Selectors& src);
   Selectors(const Selectors& src);
};




#endif
