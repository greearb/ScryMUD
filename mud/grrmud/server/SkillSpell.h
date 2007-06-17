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

#ifndef SKILL_SPELL_INCLUDE
#define SKILL_SPELL_INCLUDE



#include <list2.h>
#include <string2.h>
#include <rb_tree.h>
#include <PtrArray.h>

///***********************  skill spell  *****************************///

#define NUMBER_OF_SKILL_SPELLS          415

class object;
class critter;

class SkillSpell {
 protected:
   short ss_num;
   String name;
   short min_level; //absolute minimum that ANYONE can learn it at
   short difficulty; //0-100, 100 is more difficult
   short mana_cost;  //baseline mana cost
   int scroll_num; //number of scroll it can be scribed to
   ObjArray<int> objs_casting_spell; //List of all objects casting this spell

 public:

   List<int> prereqs;  //spell/skill numbers of pre-requisites
   List<int> enables;  //spell/skill numbers it enables you to take

   int restrictions[NUMBER_OF_CLASSES];

   SkillSpell();
   SkillSpell(const SkillSpell& source);
   SkillSpell& operator=(const SkillSpell& source);

   void Read(ifstream& da_file);
   void Write(ofstream& da_file);
   void Clear();

   String getHtml();
   String toString();

   /** Return total numbers of objects in the game that can cast this
    * spell.
    */
   int getSpellCastingCount() const;

   /** Will happily ignore duplicates. */
   void addNewCaster(int obj_num);

   object* getScroll(); //can return NULL
   int getScrollNum();
   String& getName() { return name; }
   int getMinLevel() const { return min_level; }
   int getDifficulty() const { return difficulty; }
   int getManaCost() const { return mana_cost; }
   int getIdNum() const { return ss_num; }
   int getSSNum() const { return ss_num; }
   int isInUse() const { return (name.Strlen() > 0); }

   int isValid();
   void normalize();
};//SkillSpell class


class SSCollection {
 protected:
   SkillSpell ss_list[NUMBER_OF_SKILL_SPELLS + 1];
   Tree<String, int>* ss_tree;
   SkillSpell dummy;  //used when an index is bad or something.
   SSCollection(); //dflt constructor
   String ss_descs[NUMBER_OF_SKILL_SPELLS + 1];

 public:

   static SSCollection& instance(); //instance of self

   /** This is the instance of the i_th skill/spell. */
   SkillSpell& getSS(int i);

   String& getSSDesc(int i) { return ss_descs[i]; }
   const char* getNameForNum(int ss_number);
   int getNumForName(const char* name);
   int getNumForName(const String& name);
   void read();
   void treeInsert(const String& ss_name, int ss_num);
   int doHelpFor(const String& key, critter& pc);

   String generateGifDotScript();
   String generatePsDotScript();
   String generateHtml();

 private:
   SSCollection(const SSCollection& src); //implement or don't call.
   SSCollection& operator=(const SSCollection& src); //ditto

};//SSCollection



#endif

