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

#include "SkillSpell.h"

#include "object.h"
#include "misc.h"

///******************************************************************///
///*********************  skill spell  ******************************///

SkillSpell::SkillSpell() {
   ss_num = 0;
   min_level = 0;
   difficulty = 0;
   mana_cost = 0;
   scroll_num = 0;
}//constructor

SkillSpell::SkillSpell(SkillSpell& source) {
   *this = source; //no need for overloaded = yet
}//copy constructor


String SkillSpell::getHtml() {
   String buf(100);
   String retval(500);
   Sprintf(retval, "<center><A name=\"%S\">\n<h3>Skill name: %S</h3>\n",
           &name, &name);

   Sprintf(buf, "num: %i &nbsp; mininum level: %i &nbsp; difficulty: %i/100 &nbsp;",
           ss_num, min_level, difficulty);
   retval.Append(buf);

   Sprintf(buf, "mana cost: %i  scroll number: %i<br>\n Pre-requisites:<br>\n",
           mana_cost, scroll_num);
   retval.Append(buf);

   int tmp;
   Cell<int> cll(prereqs);
   if (!prereqs.isEmpty()) {
      while ((tmp = cll.next())) {
         const char* foo = SSCollection::instance().getNameForNum(tmp);
         Sprintf(buf, "<A href = \"#%s\"> %s</a> ", foo, foo);
         retval.Append(buf);
      }//while
   }//if

   if (!enables.isEmpty()) {
      retval.Append("\n<br> Is a prerequisite for:  &nbsp;");

      enables.head(cll);
      while ((tmp = cll.next())) {
         const char* foo = SSCollection::instance().getNameForNum(tmp);
         Sprintf(buf, "<A href = \"#%s\"> %s</a> ", foo, foo);
         retval.Append(buf);
      }//while
   }
   retval.Append("</center>\n");
   return retval;
}//getHtml();

   

void SkillSpell::Read(ifstream& da_file) {
   char buf[100];
   int tmp;
   String bf(100);

   Clear();

   if (!da_file) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  da_file FALSE in SkillSpell read." << endl;
      }
      return;
   }

   da_file >> ss_num;
   da_file.getline(buf, 80);
   
   mudlog << "SkillSpell::READ, ss_num = " << ss_num << endl;

   da_file.getline(buf, 80);
   name = buf;

   name.Strip();

   mudlog << "Name -:" << name << ":-" << endl;

   da_file >> min_level >> difficulty >> mana_cost >> scroll_num;
   da_file.getline(buf, 80);
  
   da_file >> tmp;
   while (tmp != -1) {
      prereqs.append(tmp);
      da_file >> tmp;
   }//while
   da_file.getline(buf, 80);
   
   da_file >> tmp;
   while (tmp != -1) {
      enables.append(tmp);
      da_file >> tmp;
   }//while
   da_file.getline(buf, 80);
   da_file.getline(buf, 80);
}//read

void SkillSpell::Write(ofstream& da_file) {
   int tmp;

   da_file << ss_num << "SPELL/SKILL NUMBER\n";
   da_file << name << endl;
   da_file << min_level << " " << difficulty << " " << mana_cost 
           << " " << scroll_num << " minlvl, diff, mana, scroll#\n";
   
   Cell<int> cll(prereqs);
   while ((tmp = cll.next())) {
      da_file << tmp << " ";
   }//while
   da_file << -1 << "\tpre-reqs\n";
   
   enables.head(cll);
   while ((tmp = cll.next())) {
      da_file << tmp << " ";
   }//while
   da_file << -1 << "\tenables\n";
   da_file << endl;
}//write


int SkillSpell::getScrollNum() {
   object* scrl = getScroll();
   if (scrl)
     return scrl->getIdNum();
   return -1;
}

void SkillSpell::Clear() {
   min_level = 0;
   name.Clear();
   ss_num = 0;
   difficulty = 0;
   mana_cost = 0;
   scroll_num = 0;
   
   enables.clear();
   prereqs.clear();
}//clear

 
object* SkillSpell::getScroll() {
   String buf(100);
   object* retval = NULL;

   if (mana_cost && !ss_num) {
      Sprintf(buf, "ERROR:  need to create a scroll for %S.\n",
              &name);
      mudlog.log(ERR, buf);
      retval = NULL;
   }//if
   else {
      retval = &(obj_list[scroll_num]);
   }//else

   if (retval) {
      if (!retval->isInUse()) {
         Sprintf(buf, 
                 "ERROR:  Tried to return non-existant scroll:  %i in get_number_of_scroll.\n",
                 retval);
         mudlog.log(ERR, buf);
         return NULL;
      }//if
      if (!retval->isScroll()) {
         Sprintf(buf, "ERROR:  Tried to return scroll that was NOT a 
scroll:  %i in get_number_of_scroll.\n", retval->getIdNum());
         mudlog.log(ERR, buf);
         return NULL;
      }//if
   }//if

   return retval;
}//getScroll()




///***********************************************************************///
///************************  SSCollection  *******************************///
///***********************************************************************///

int Strncompare(const String& a, const String& b) {
   return strcasecmp(a.string, b.string);
}

SSCollection::SSCollection() {
   ss_tree = new Tree<String, int>(&Strncompare);
}//constructor   


SSCollection& SSCollection::instance() {
   static SSCollection self;
   return self;
}

void SSCollection::treeInsert(const String& ss_name, int ss_num) {
   ss_tree->Insert(ss_name, ss_num);
}


const char* SSCollection::getNameForNum(int skill_num) {
   
   if (!check_l_range(skill_num, 0, NUMBER_OF_SKILL_SPELLS, mob_list[0],
		      FALSE)) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  skill_num out of range: " << skill_num
                << " get_skill_name!!\n";
      }//if
      return "UNKNOWN";
   }//if

   if (ss_list[skill_num].getName().Strlen()) {
      return (const char*)(ss_list[skill_num].getName());
   }//if
   else
      return "UNKNOWN";
}//get_spell_name


int SSCollection::getNumForName(const char* name) {
   String buf(name);
   return getNumForName(buf);
}

int SSCollection::getNumForName(const String& name) {
   int num = 0;

   if (ss_tree->Find(name, num)) {
      return num;
   }//if
   else {
      return -1;
   }//if
}//getNumForName


String SSCollection::generatePsDotScript() {
   String retval(5000);
   String buf(200);

   retval = "digraph ss_graph {\n\tpage=\"8.5,11\";\n\tsize=\"64,30\";\n";
   retval += "\t//orientation=landscape;\n";

   for (int i = 0; i<NUMBER_OF_SKILL_SPELLS; i++) {
      if (ss_list[i].getIdNum() != 0) {
         Cell<int> cll(ss_list[i].enables);
         int val = 0;
         while ((val = cll.next())) {
            Sprintf(buf, "\t\"%S\" -> \"%S\";\n", &(ss_list[i].getName()),
                    &(getSS(val).getName()));
            retval.Append(buf);
         }//while
         ss_list[i].prereqs.head(cll);
         while ((val = cll.next())) {
            Sprintf(buf, "\t\"%S\" -> \"%S\" [color=blue,dir=back];\n",
                    &(getSS(val).getName()), &(ss_list[i].getName()));
            retval.Append(buf);
         }//while

         //retval.Append("\n");
      }//if
   }//for

   retval.Append("\n}");

   return retval;
}//generatePsDotScript


String SSCollection::generateGifDotScript() {
   String retval(5000);
   String buf(200);

   retval = "digraph ss_graph {\n\tpage=\"40,80\";\n\tsize=\"40,80\";\n";
   retval += "\t//orientation=landscape;\n";

   for (int i = 0; i<NUMBER_OF_SKILL_SPELLS; i++) {
      if (ss_list[i].getIdNum() != 0) {
         Cell<int> cll(ss_list[i].enables);
         int val = 0;
         while ((val = cll.next())) {
            Sprintf(buf, "\t\"%S\" -> \"%S\";\n", &(ss_list[i].getName()),
                    &(getSS(val).getName()));
            retval.Append(buf);
         }//while
         ss_list[i].prereqs.head(cll);
         while ((val = cll.next())) {
            Sprintf(buf, "\t\"%S\" -> \"%S\" [color=blue, dir=back];\n",
                    &(getSS(val).getName()), &(ss_list[i].getName()));
            retval.Append(buf);
         }//while

         //retval.Append("\n");
      }//if
   }//for

   retval.Append("\n}");

   return retval;
}//generateGifDotScript


String SSCollection::generateHtml() {
   String retval(5000);
   retval = "
<HTML>
<HEAD>
   <TITLE>  SKILLS SPELLS List </TITLE>
</HEAD>
<BODY TEXT=\"#33DDFF\" BGCOLOR=\"#000000\" LINK=\"#FFD700\"
      VLINK=\"#DAA520\" ALINK=\"#FF0000\">

<CENTER>
<H2>
<FONT COLOR=\"#FFCC00\">Skills and Spells</FONT></H2></CENTER>
This is a list of skills and spells that have been added to the official
list of skills and spells for ScryMUD. Not all of these have been written
into the game yet, but I'm working on it. It became clear to me that we
need more, and a better structure as well. I will comment this first one
so you can get a feal for what the list means. The general ideas is that
you must first gain all the prerequisites for a skill or spell before you
can learn it. Thus, we can determine the range of skills and spells a person
can learn by limiting the prerequisites we give them upon login for the
first time.<P>";

   for (int i = 0; i<NUMBER_OF_SKILL_SPELLS; i++) {
      if (ss_list[i].getIdNum() != 0) {
         retval.Append(ss_list[i].getHtml());
         retval.Append("<P>\n");
         retval.Append(getSSDesc(i));
         retval.Append("<hr><P>\n\n");
      }//if
   }//for

   retval.Append("\n\n
<ADDRESS>
Generated by:<A HREF=\"mailto:greear@cyberhighway.net\">Grock</A>
</ADDRESS>
</BODY>
</HTML>\n");

   return retval;
}//generateHtml


/** This is the instance of the i_th skill/spell. */
SkillSpell& SSCollection::getSS(int i) {
   if ((i >= 0) && (i < NUMBER_OF_SKILL_SPELLS)) {
      return ss_list[i];
   }

   if (i != -1) {
      if (mudlog.ofLevel(ERR)) {
         mudlog << "ERROR:  Returning DUMMY in SSCollection::getSS, for spll: "
                << i << endl;
      }//if
   }//if

   return dummy;
}//getSS

void SSCollection::read() {
   int k;
   char buf[200];

   mudlog.log(TRC, "In SSCollection::read()\n");

   ifstream zfile("./World/SKILLS_SPELLS");
   if (!zfile) {
      mudlog.log(ERR, "ERROR:  SKILLS_SPELLS not opened correctly, fatal.\n");
      return;
   }//if

   zfile >> k;
   zfile.getline(buf, 80);
   
   while ((k != -1) && zfile) { //then read it in.
      if (!check_l_range(k, 0, NUMBER_OF_SKILL_SPELLS, mob_list[0], FALSE)) {
         mudlog.log(ERR, "ERROR:  zone number is out of range, fatal\n");
         return;
      }//if
      instance().getSS(k).Read(zfile);
      zfile >> k;
      zfile.getline(buf, 80);
   }//while

   /* this next bit gives us O(log(n)) access to numbers of skill/spells,
      when given the name.  This is much nicer than the old brute-force
      method which yielded O(n) time.  New globals give some a CONSTANT
      time look_up now!! */
   int i;
   for (i = 0; i<NUMBER_OF_SKILL_SPELLS; i++) {
      if (getSS(i).getName().Strlen()) { //if it has a name
         mudlog << "SkillSpell.read():  Adding spell to tree, num: " << i
                << "  name -:" << getSS(i).getName()
                << ":-" << endl;

         if (strcasecmp("absorb blows", getSS(i).getName()) == 0)
            ABSORB_BLOWS_SKILL_NUM = i;
         else if (strcasecmp("acrobatics", getSS(i).getName()) == 0)
            ACROBATICS_SKILL_NUM = i;
         else if (strcasecmp("ancient languages", getSS(i).getName()) == 0)
            ANCIENT_LANGUAGES_SKILL_NUM = i;
         else if (strcasecmp("alchemy", getSS(i).getName()) == 0)
            ALCHEMY_SKILL_NUM = i;
         else if (strcasecmp("archery", getSS(i).getName()) == 0)
            ARCHERY_SKILL_NUM = i;
         else if (strcasecmp("armor", getSS(i).getName()) == 0)
            ARMOR_SKILL_NUM = i;
         else if (strcasecmp("backstab", getSS(i).getName()) == 0)
            BACKSTAB_SKILL_NUM = i;
         else if (strcasecmp("balistics", getSS(i).getName()) == 0)
            BALISTICS_SKILL_NUM = i;
         else if (strcasecmp("bash door", getSS(i).getName()) == 0)
            BASH_DOOR_SKILL_NUM = i;
         else if (strcasecmp("bash", getSS(i).getName()) == 0)
            BASH_SKILL_NUM = i;
         else if (strcasecmp("berserk", getSS(i).getName()) == 0)
            BERZERK_SKILL_NUM = i;
         else if (strcasecmp("bind wound", getSS(i).getName()) == 0)
            BIND_WOUND_SKILL_NUM = i;
         else if (strcasecmp("blacksmithing", getSS(i).getName()) == 0)
            BLACKSMITHING_SKILL_NUM = i;
         else if (strcasecmp("blend", getSS(i).getName()) == 0)
            BLEND_SKILL_NUM = i;
         else if (strcasecmp("bless", getSS(i).getName()) == 0)
            BLESS_SKILL_NUM = i;
         else if (strcasecmp("blindness", getSS(i).getName()) == 0)
            BLINDNESS_SKILL_NUM = i;
         else if (strcasecmp("block", getSS(i).getName()) == 0)
            BLOCK_SKILL_NUM = i;
         else if (strcasecmp("block door", getSS(i).getName()) == 0)
            BLOCK_DOOR_SKILL_NUM = i;
         else if (strcasecmp("bodyslam", getSS(i).getName()) == 0)
            BODYSLAM_SKILL_NUM = i;
         else if (strcasecmp("bow", getSS(i).getName()) == 0)
            BOW_SKILL_NUM = i;
         else if (strcasecmp("brawling", getSS(i).getName()) == 0)
            BRAWLING_SKILL_NUM = i;
         else if (strcasecmp("brew", getSS(i).getName()) == 0)
            BREW_SKILL_NUM = i;
         else if (strcasecmp("burning hands", getSS(i).getName()) == 0)
            BURNING_HANDS_SKILL_NUM = i;
         else if (strcasecmp("butcher", getSS(i).getName()) == 0)
            BUTCHER_SKILL_NUM = i;
         else if (strcasecmp("calm", getSS(i).getName()) == 0)
            CALM_SKILL_NUM = i;
         else if (strcasecmp("camouflage", getSS(i).getName()) == 0)
            CAMOUFLAGE_SKILL_NUM = i;
         else if (strcasecmp("camping", getSS(i).getName()) == 0)
            CAMPING_SKILL_NUM = i;
         else if (strcasecmp("cause critical", getSS(i).getName()) == 0)
            CAUSE_CRITICAL_SKILL_NUM = i;
         else if (strcasecmp("cause sickness", getSS(i).getName()) == 0)
            CAUSE_SICKNESS_SKILL_NUM = i;
         else if (strcasecmp("charm", getSS(i).getName()) == 0)
            CHARM_SKILL_NUM = i;
         else if (strcasecmp("channeling", getSS(i).getName()) == 0)
            CHANNELLING_SKILL_NUM = i;
         else if (strcasecmp("circle", getSS(i).getName()) == 0)
            CIRCLE_SKILL_NUM = i;
         else if (strcasecmp("claw", getSS(i).getName()) == 0)
            CLAW_SKILL_NUM = i;
         else if (strcasecmp("climbing", getSS(i).getName()) == 0)
            CLIMBING_SKILL_NUM = i;
         else if (strcasecmp("commerce", getSS(i).getName()) == 0)
            COMMERCE_SKILL_NUM = i;
         else if (strcasecmp("construct", getSS(i).getName()) == 0)
            CONSTRUCT_SKILL_NUM = i;
         else if (strcasecmp("conjure horde", getSS(i).getName()) == 0)
            CONJURE_HORDE_SKILL_NUM = i;
         else if (strcasecmp("conjuring", getSS(i).getName()) == 0)
            CONJURING_SKILL_NUM = i;
         else if (strcasecmp("conjure minion", getSS(i).getName()) == 0)
            CONJURE_MINION_SKILL_NUM = i;
         else if (strcasecmp("create food", getSS(i).getName()) == 0)
            CREATE_FOOD_SKILL_NUM = i;
         else if (strcasecmp("create golem", getSS(i).getName()) == 0)
            CREATE_GOLEM_SKILL_NUM = i;
         else if (strcasecmp("create light", getSS(i).getName()) == 0)
            CREATE_LIGHT_SKILL_NUM = i;
         else if (strcasecmp("create water", getSS(i).getName()) == 0)
            CREATE_WATER_SKILL_NUM = i;
         else if (strcasecmp("creation", getSS(i).getName()) == 0)
            CREATION_SKILL_NUM = i;
         else if (strcasecmp("critical strike", getSS(i).getName()) == 0)
            CRITICAL_STRIKE_SKILL_NUM = i;
         else if (strcasecmp("cure blindness", getSS(i).getName()) == 0)
            CURE_BLINDNESS_SKILL_NUM = i;
         else if (strcasecmp("cure critical", getSS(i).getName()) == 0)
            CURE_CRITICAL_SKILL_NUM = i;
         else if (strcasecmp("cure serious", getSS(i).getName()) == 0)
            CURE_SERIOUS_SKILL_NUM = i;
         else if (strcasecmp("curse", getSS(i).getName()) == 0)
            CURSE_SKILL_NUM = i;
         else if (strcasecmp("dagger", getSS(i).getName()) == 0)
            DAGGER_SKILL_NUM = i;
         else if (strcasecmp("detection", getSS(i).getName()) == 0)
            DETECTION_SKILL_NUM = i;
         else if (strcasecmp("detect alignment",
                             getSS(i).getName()) == 0)
            DETECT_ALIGNMENT_SKILL_NUM = i;
         else if (strcasecmp("detect hidden", getSS(i).getName()) == 0)
            DETECT_HIDDEN_SKILL_NUM = i;
         else if (strcasecmp("detect invisibility", 
                             getSS(i).getName()) == 0)
            DETECT_INVISIBILITY_SKILL_NUM = i;
         else if (strcasecmp("detect magic", getSS(i).getName()) == 0)
            DETECT_MAGIC_SKILL_NUM = i;
         else if (strcasecmp("detect poison", getSS(i).getName()) == 0)
            DETECT_POISON_SKILL_NUM = i;
         else if (strcasecmp("disarm", getSS(i).getName()) == 0)
            DISARM_SKILL_NUM = i;
         else if (strcasecmp("distortion wall", getSS(i).getName()) == 0)
            DISTORTION_WALL_SKILL_NUM = i;
         else if (strcasecmp("dispel evil", getSS(i).getName()) == 0)
            DISPEL_EVIL_SKILL_NUM = i;
         else if (strcasecmp("dispel good", getSS(i).getName()) == 0)
            DISPEL_GOOD_SKILL_NUM = i;
         else if (strcasecmp("dispel magic", getSS(i).getName()) == 0)
            DISPEL_MAGIC_SKILL_NUM = i;
         else if (strcasecmp("divine protection",
                             getSS(i).getName()) == 0)
            DIVINE_PROTECTION_SKILL_NUM = i;
         else if (strcasecmp("dart of darkness",
                             getSS(i).getName()) == 0)
            DOD_SKILL_NUM = i;
         else if (strcasecmp("dodge", getSS(i).getName()) == 0)
            DODGE_SKILL_NUM = i;
         else if (strcasecmp("door bash", getSS(i).getName()) == 0)
            DOOR_BASH_SKILL_NUM = i;
         else if (strcasecmp("dual wield", getSS(i).getName()) == 0)
            DUAL_WIELD_SKILL_NUM = i;
         else if (strcasecmp("earthmeld", getSS(i).getName()) == 0)
            EARTHMELD_SKILL_NUM = i;
         else if (strcasecmp("elementalism", getSS(i).getName()) == 0)
            ELEMENTALISM_SKILL_NUM = i;
         else if (strcasecmp("enchant armor", getSS(i).getName()) == 0)
            ENCHANT_ARMOR_SKILL_NUM = i;
         else if (strcasecmp("enchantment", getSS(i).getName()) == 0)
            ENCHANTMENT_SKILL_NUM = i;
         else if (strcasecmp("enchant weapon", getSS(i).getName()) == 0)
            ENCHANT_WEAPON_SKILL_NUM = i;
         else if (strcasecmp("enhanced damage", getSS(i).getName()) == 0)
            ENHANCED_DAMAGE_SKILL_NUM = i;
         else if (strcasecmp("entangle", getSS(i).getName()) == 0)
            ENTANGLE_SKILL_NUM = i;
         else if (strcasecmp("faerie fire", getSS(i).getName()) == 0)
            FAERIE_FIRE_SKILL_NUM = i;
         else if (strcasecmp("fencing", getSS(i).getName()) == 0)
            FENCING_SKILL_NUM = i;
         else if (strcasecmp("fireball", getSS(i).getName()) == 0)
            FIREBALL_SKILL_NUM = i;
         else if (strcasecmp("fireproof", getSS(i).getName()) == 0)
            FIREPROOF_SKILL_NUM = i;
         else if (strcasecmp("firewall", getSS(i).getName()) == 0)
            FIREWALL_SKILL_NUM = i;
         else if (strcasecmp("firestorm", getSS(i).getName()) == 0)
            FIRESTORM_SKILL_NUM = i;
         else if (strcasecmp("fire blade", getSS(i).getName()) == 0)
            FIRE_BLADE_SKILL_NUM = i;
         else if (strcasecmp("flame strike", getSS(i).getName()) == 0)
            FLAME_STRIKE_SKILL_NUM = i;
         else if (strcasecmp("fly", getSS(i).getName()) == 0)
            FLY_SKILL_NUM = i;
         else if (strcasecmp("forestry", getSS(i).getName()) == 0)
            FORESTRY_SKILL_NUM = i;
         else if (strcasecmp("frost blade", getSS(i).getName()) == 0)
            FROST_BLADE_SKILL_NUM = i;
         else if (strcasecmp("gate", getSS(i).getName()) == 0)
            GATE_SKILL_NUM = i;
         else if (strcasecmp("group heal", getSS(i).getName()) == 0)
            GROUP_HEAL_SKILL_NUM = i;
         else if (strcasecmp("guard", getSS(i).getName()) == 0)
            GUARD_SKILL_NUM = i;
         else if (strcasecmp("haste", getSS(i).getName()) == 0)
            HASTE_SKILL_NUM = i;
         else if (strcasecmp("harm", getSS(i).getName()) == 0)
            HARM_SKILL_NUM = i;
         else if (strcasecmp("harming", getSS(i).getName()) == 0)
            HARMING_SKILL_NUM = i;
         else if (strcasecmp("heal", getSS(i).getName()) == 0)
            HEAL_SKILL_NUM = i;
         else if (strcasecmp("healing", getSS(i).getName()) == 0)
            HEALING_SKILL_NUM = i;
         else if (strcasecmp("herbalism", getSS(i).getName()) == 0)
            HERBALISM_SKILL_NUM = i;
         else if (strcasecmp("heros feast", getSS(i).getName()) == 0)
            HEROS_FEAST_SKILL_NUM = i;
         else if (strcasecmp("hide", getSS(i).getName()) == 0)
            HIDE_SKILL_NUM = i;
         else if (strcasecmp("holy word", getSS(i).getName()) == 0)
            HOLY_WORD_SKILL_NUM = i;
         else if (strcasecmp("honor code", getSS(i).getName()) == 0)
            HONOR_CODE_SKILL_NUM = i;
         else if (strcasecmp("hurl", getSS(i).getName()) == 0)
            HURL_SKILL_NUM = i;
         else if (strcasecmp("icestorm", getSS(i).getName()) == 0)
            ICESTORM_SKILL_NUM = i;
         else if (strcasecmp("identify", getSS(i).getName()) == 0)
            IDENTIFY_SKILL_NUM = i;
         else if (strcasecmp("illuminate", getSS(i).getName()) == 0)
            ILLUMINATE_SKILL_NUM = i;
         else if (strcasecmp("illusion", getSS(i).getName()) == 0)
            ILLUSION_SKILL_NUM = i;
         else if (strcasecmp("invisibility", getSS(i).getName()) == 0)
            INVISIBILITY_SKILL_NUM = i;
         else if (strcasecmp("kick", getSS(i).getName()) == 0)
            KICK_SKILL_NUM = i;
         else if (strcasecmp("leadership", getSS(i).getName()) == 0)
            LEADERSHIP_SKILL_NUM = i;
         else if (strcasecmp("lightning bolt", getSS(i).getName()) == 0)
            LIGHTNING_SKILL_NUM = i;
         else if (strcasecmp("lightning storm", getSS(i).getName()) == 0)
            LIGHTNING_STORM_SKILL_NUM = i;
         else if (strcasecmp("literacy", getSS(i).getName()) == 0)
            LITERACY_SKILL_NUM = i;
         else if (strcasecmp("locate", getSS(i).getName()) == 0)
            LOCATE_SKILL_NUM = i;
         else if (strcasecmp("logic", getSS(i).getName()) == 0)
            LOGIC_SKILL_NUM = i;
         else if (strcasecmp("lore", getSS(i).getName()) == 0)
            LORE_SKILL_NUM = i;
         else if (strcasecmp("mace", getSS(i).getName()) == 0)
            MACE_SKILL_NUM = i;
         else if (strcasecmp("magic shield", getSS(i).getName()) == 0)
            MAGIC_SHIELD_SKILL_NUM = i;
         else if (strcasecmp("mana", getSS(i).getName()) == 0)
            MANA_SKILL_NUM = i;
         else if (strcasecmp("martial arts", getSS(i).getName()) == 0)
            MARTIAL_ARTS_SKILL_NUM = i;
         else if (strcasecmp("mass charm", getSS(i).getName()) == 0)
            MASS_CHARM_SKILL_NUM = i;
         else if (strcasecmp("meditation", getSS(i).getName()) == 0)
            MEDITATION_SKILL_NUM = i;
         else if (strcasecmp("meteorstorm", getSS(i).getName()) == 0)
            METEORSTORM_SKILL_NUM = i;
         else if (strcasecmp("mirror image", getSS(i).getName()) == 0)
            MIRROR_IMAGE_SKILL_NUM = i;
         else if (strcasecmp("necromancy", getSS(i).getName()) == 0)
            NECROMANCY_SKILL_NUM = i;
         else if (strcasecmp("parry", getSS(i).getName()) == 0)
            PARRY_SKILL_NUM = i;
         else if (strcasecmp("passdoor", getSS(i).getName()) == 0)
            PASSDOOR_SKILL_NUM = i;
         else if (strcasecmp("philosophy", getSS(i).getName()) == 0)
            PHILOSOPHY_SKILL_NUM = i;
         else if (strcasecmp("physik", getSS(i).getName()) == 0)
            PHYSIK_SKILL_NUM = i;
         else if (strcasecmp("picklock", getSS(i).getName()) == 0)
            PICKLOCK_SKILL_NUM = i;
         else if (strcasecmp("poison", getSS(i).getName()) == 0)
            POISON_SKILL_NUM = i;
         else if (strcasecmp("plague", getSS(i).getName()) == 0)
            PLAGUE_SKILL_NUM = i;
         else if (strcasecmp("portal", getSS(i).getName()) == 0)
            PORTAL_SKILL_NUM = i;
         else if (strcasecmp("prismatic globe", getSS(i).getName()) == 0)
            PRISMATIC_GLOBE_SKILL_NUM = i;
         else if (strcasecmp("protection", getSS(i).getName()) == 0)
            PROTECTION_SKILL_NUM = i;
         else if (strcasecmp("protection from evil",
                             getSS(i).getName()) == 0)
            PFE_SKILL_NUM = i;
         else if (strcasecmp("protection from good",
                             getSS(i).getName()) == 0)
            PFG_SKILL_NUM = i;
         else if (strcasecmp("quake", getSS(i).getName()) == 0)
            QUAKE_SKILL_NUM = i;
         else if (strcasecmp("quickfoot", getSS(i).getName()) == 0)
            QUICKFOOT_SKILL_NUM = i;
         else if (strcasecmp("rainbow", getSS(i).getName()) == 0)
            RAINBOW_SKILL_NUM = i;
         else if (strcasecmp("raise undead", getSS(i).getName()) == 0)
            RAISE_UNDEAD_SKILL_NUM = i;
         else if (strcasecmp("recall", getSS(i).getName()) == 0)
            RECALL_SKILL_NUM = i;
         else if (strcasecmp("recharge", getSS(i).getName()) == 0)
            RECHARGE_SKILL_NUM = i;
         else if (strcasecmp("religion", getSS(i).getName()) == 0)
            RELIGION_SKILL_NUM = i;
         else if (strcasecmp("remove poison", getSS(i).getName()) == 0)
            REMOVE_POISON_SKILL_NUM = i;
         else if (strcasecmp("remove curse", getSS(i).getName()) == 0)
            REMOVE_CURSE_SKILL_NUM = i;
         else if (strcasecmp("rescue", getSS(i).getName()) == 0)
            RESCUE_SKILL_NUM = i;
         else if (strcasecmp("restore", getSS(i).getName()) == 0)
            RESTORE_SKILL_NUM = i;
         else if (strcasecmp("rune edge", getSS(i).getName()) == 0)
            RUNE_EDGE_SKILL_NUM = i;
         else if (strcasecmp("sanctuary", getSS(i).getName()) == 0)
            SANCTUARY_SKILL_NUM = i;
         else if (strcasecmp("second attack", getSS(i).getName()) == 0)
            SECOND_ATTACK_SKILL_NUM = i;
         else if (strcasecmp("scan", getSS(i).getName()) == 0)
            SCAN_SKILL_NUM = i;
         else if (strcasecmp("scribe", getSS(i).getName()) == 0)
            SCRIBE_SKILL_NUM = i;
         else if (strcasecmp("scrolls", getSS(i).getName()) == 0)
            SCROLLS_SKILL_NUM = i;
         else if (strcasecmp("scrying", getSS(i).getName()) == 0)
            SCRYING_SKILL_NUM = i;
         else if (strcasecmp("shadows blessing",
                             getSS(i).getName()) == 0)
            SHADOWS_BLESSING_SKILL_NUM = i;
         else if (strcasecmp("shield", getSS(i).getName()) == 0)
            SHIELD_SKILL_NUM = i;
         else if (strcasecmp("shocking grasp", getSS(i).getName()) == 0)
            SHOCKING_GRASP_SKILL_NUM = i;
         else if (strcasecmp("sleep", getSS(i).getName()) == 0)
            SLEEP_SKILL_NUM = i;
         else if (strcasecmp("skin", getSS(i).getName()) == 0)
            SKIN_SKILL_NUM = i;
         else if (strcasecmp("sneak", getSS(i).getName()) == 0)
            SNEAK_SKILL_NUM = i;
         else if (strcasecmp("stone skin", getSS(i).getName()) == 0)
            STONE_SKIN_SKILL_NUM = i;
         else if (strcasecmp("steal", getSS(i).getName()) == 0)
            STEAL_SKILL_NUM = i;
         else if (strcasecmp("strength", getSS(i).getName()) == 0)
            STRENGTH_SKILL_NUM = i;
         else if (strcasecmp("strength conditioning", getSS(i).getName()) == 0)
            STRENGTH_CONDITIONING_SKILL_NUM = i;
         else if (strcasecmp("summon", getSS(i).getName()) == 0)
            SUMMON_SKILL_NUM = i;
         else if (strcasecmp("sword", getSS(i).getName()) == 0)
            SWORD_SKILL_NUM = i;
         else if (strcasecmp("swordbond", getSS(i).getName()) == 0)
            SWORDBOND_SKILL_NUM = i;
         else if (strcasecmp("tail", getSS(i).getName()) == 0)
            TAIL_SKILL_NUM = i;
         else if (strcasecmp("teleport", getSS(i).getName()) == 0)
            TELEPORT_SKILL_NUM = i;
         else if (strcasecmp("throwing", getSS(i).getName()) == 0)
            THROWING_SKILL_NUM = i;
         else if (strcasecmp("track", getSS(i).getName()) == 0)
            TRACK_SKILL_NUM = i;
         else if (strcasecmp("tornado", getSS(i).getName()) == 0)
            TORNADO_SKILL_NUM = i;
         else if (strcasecmp("trip", getSS(i).getName()) == 0)
            TRIP_SKILL_NUM = i;
         else if (strcasecmp("typhoon", getSS(i).getName()) == 0)
            TYPHOON_SKILL_NUM = i;
         else if (strcasecmp("weaken", getSS(i).getName()) == 0)
            WEAKEN_SKILL_NUM = i;
         else if (strcasecmp("weapon mastery", getSS(i).getName()) == 0)
            WEAPON_MASTERY_SKILL_NUM = i;
         else if (strcasecmp("web", getSS(i).getName()) == 0)
            WEB_SKILL_NUM = i;
         else if (strcasecmp("whip", getSS(i).getName()) == 0)
            WHIP_SKILL_NUM = i;
         else if (strcasecmp("wizard eye", getSS(i).getName()) == 0)
            WIZARD_EYE_SKILL_NUM = i;
         else if (strcasecmp("wrestling", getSS(i).getName()) == 0)
            WRESTLING_SKILL_NUM = i;
         else {
            mudlog << "ERROR:  could not match the spell with a constant, spell_num:" 
                   << i << "  name -:" << getSS(i).getName() << ":- " << endl;
         }
         SSCollection::instance().treeInsert(getSS(i).getName(), i);
      }//if
   }//for

   int ss_num;
   String str_buf(500);

   ifstream desc_file("./World/SS_DESCS");
   desc_file.getline(buf, 180);
   
   while (desc_file) {
      String tstr(buf);
      tstr.Strip();
      ss_num = getNumForName(tstr);
      if (ss_num <= 0) {
         mudlog << "ERROR:  could not find number for ss -:" << tstr << ":-" << endl;
         return;
      }
      else {
         mudlog << "Reading ss -:" << tstr << ":-" << endl;
         
         str_buf.Termed_Read(desc_file);
         
         mudlog << "Got Description -:" << str_buf << ":-" << endl;
         
         ss_descs[ss_num] = str_buf;
      }//else

      desc_file.getline(buf, 180);

   }//while
}//read
