// $Id: classes.cc,v 1.20 1999/09/08 06:11:36 greear Exp $
// $Revision: 1.20 $  $Author: greear $ $Date: 1999/09/08 06:11:36 $

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

///********************  classes.cc  *****************************///

#include "classes.h" 
#include "misc.h"
#include "misc2.h"
#include "room.h"
#include "const.h"
#include "commands.h"    //for wear_eq_effects...
#include "battle.h"
#include <PtrArray.h>
#include "command5.h"
#include "command4.h"
#include "SkillSpell.h"
#include "lang_strings.h"


int LstrArray::_cnt = 0;

LstrArray::LstrArray(int length, const CSentryE* _names,
                     const CSentryE col_name) 
      : len(length), names(_names), header(col_name) {
   _cnt++;
}


const char* LstrArray::getHeader(critter* viewer) const {
   LanguageE lang = English;
   if (viewer)
      lang = viewer->getLanguage();

   return cstr(header, lang);
}

const char* LstrArray::getName(int idx, critter* viewer) const {
   LanguageE lang = English;
   if (viewer)
      lang = viewer->getLanguage();

   if ((idx >= 0) && (idx < len))
      return cstr(names[idx], lang);
   return "";
}

/** put names into the buf for display */
void LstrArray::listNames(String& buf, critter* viewer) const {
   String tmp(200);

   LanguageE lang = English;
   if (viewer)
      lang = viewer->getLanguage();

   buf = getHeader(viewer);
   buf += cstr(CS_DEFINED, lang);
   int sofar = 0;

   for (int i = 0; i<len; i++) {
      Sprintf(tmp, "[%i] %s, ", i, getName(i, viewer));
      if ((sofar + tmp.Strlen()) > 80) {
         buf += "\n\t";
         sofar = tmp.Strlen();
      }
      else {
         sofar += tmp.Strlen();
      }
      buf += tmp;
   }
   buf += "\n";
}//listNames



int MTPair::write(ostream& dafile) {
   dafile << key << "=\"" << val << "\" ";
   return 0;
}


MetaTags::MetaTags(const char* first_tag, istream& dafile) {
   construct(first_tag, dafile);
}

int MetaTags::construct(const char* first_tag, istream& dafile) {
   String key(100);
   String val(100);
   char buf[100];
   if (strcasecmp(first_tag, "<META") == 0) {
      while (readTag(key, val, dafile) >= 0) {
         addTag(key, val);
         mudlog << "MetaTags::construct, Adding key -:" << key
                << ":-  val -:" << val << ":-\n";
      }
      dafile.getline(buf, 99); // junk the trailing META>
   }//if
   else {
      if (mudlog.ofLevel(DB)) {
         mudlog << "ERROR: reading MetaTags, first_tag bad -:" << first_tag
                << ":-\n";
      }
      ::core_dump(__FUNCTION__);
   }
   if (dafile)
      return 0;
   else
      return -1;
}//construct


/** Tags are stored like this:  key="VAL", the delimiter key-value pair
 * is ~=~, which causes this method to return -1.
 */
int MetaTags::readTag(String& key, String& val, istream& dafile) {
   String buf(100);

   buf.readUntil('=', dafile);
   buf.strip();
   key = buf;

   // Now, read in the value
   buf.readToken('"', dafile, FALSE);
   val = buf;

   if (strcmp(buf, "~")) {
      return -1;
   }
   return 0;
}//readTag


MetaTags::MetaTags(istream& dafile) {
   read(dafile);
}

int MetaTags::read(istream& dafile) {
   MetaTags::clear();
   String buf(100);
   dafile >> buf;
   return construct(buf, dafile);
}

void MetaTags::clear() {
   lst.clearAndDestroy();
}


int MetaTags::write(ostream& dafile) {
   dafile << "<META ";
   Cell<MTPair*> cll(lst);
   MTPair* ptr;
   while ((ptr = cll.next())) {
      ptr->write(dafile);
   }
   dafile << "META>\n";
   return 0;
}

String& MetaTags::getValue(const char* key) {
   Cell<MTPair*> cll(lst);
   MTPair* ptr;
   while ((ptr = cll.next())) {
      if (strcasecmp(key, ptr->key) == 0) {
         return ptr->val;
      }
   }//while
   return NULL_STRING;
}

void MetaTags::addTag(String& key, String& val) {
   lst.append(new MTPair(key, val));
}

void MetaTags::generateTags(MetaTags& rslt, Serialized& obj) {
   rslt.clear();
   String key(100);
   String val(100);

   // version
   key = "VERSION";
   val = obj.getVersion();
   rslt.addTag(key, val);

   key = "ENTITY_TYPE";
   val = (int)(obj.getEntityType());
   rslt.addTag(key, val);
}//generateTags
      

void Closable::toStringStat(critter* viewer, String& rslt, ToStringTypeE st) {
   String buf(200);
   if (!viewer || viewer->isUsingClient()) {
      Sprintf(rslt, "<CLOSABLE %i %i>", key, token);
      Markup::toString(flags, CLOSABLE_FLAGS_NAMES, viewer, buf);
      rslt.append(buf);
      rslt.append("</CLOSABLE>\n");
   }
   else {
      Sprintf(rslt, "Key#: %i  Token#: %i\n", key, token);
      Markup::toString(flags, CLOSABLE_FLAGS_NAMES, viewer, buf);
      rslt.append(buf);
      rslt.append("\n");
   }
}

int Closable::write(ostream& dafile) { 
   MetaTags mt(*this);

   mt.write(dafile);
   flags.write(dafile);
   dafile << key << " " << token << endl;
   return 0;
}

int Closable::read(istream& dafile, int read_all = TRUE) {
   char buf[100];
   MetaTags mt(dafile);
   flags.read(dafile);
   dafile >> key >> token;
   dafile.getline(buf, 99);
   return 0;
}

int Closable::_cnt = 0;

/** Returns TRUE if we could flip it, false otherwise.  We will not let
 * you flip some flags here, though they can be explicitly set through
 * other methods.
 */
int Closable::flipFlag(int flg) {
   if ((flg != 10) && (flg != 15) && (flg < 50)) {
      flags.flip(flg);
      return TRUE;
   }
   return FALSE;
}


LString& LString::operator=(const String& src) {
   lang = English;
   *((String*)(this)) = src;
   return *this;
}
   

/* WARNING:  this will fail if input is > 299 chars. */
istream& operator>> (istream& stream, LString& str) {
   int foo;
   stream >> foo;
   str.termedRead(stream);
   return stream;
}//write_out operator


ostream& operator<< (ostream& stream, const LString& str) {
   stream << (int)(str.lang) << " " << (const char*)(str) << "\n~\n";
   return stream;
}//write_out operator


LStringCollection::~LStringCollection() {
   clearAndDestroy();
}

LString LString::NULL_STRING;


LString* LStringCollection::getString(critter* viewer) {
   return getString(viewer->getLanguage());
} //just call the one above.


/** This will add the new string to the collection.  If a string with the
 * same language is already in it, then the old one will be
 * replaced by the new one.
 */
void LStringCollection::addLstring(LString& new_string) {
   LString* ptr = getString(new_string.getLanguage());
   if (ptr != &(LString::NULL_STRING)) {
      *ptr = new_string;
      return;
   }
   append(new LString(new_string));
}


/** Give as much info as possible, used by immortals.  Include client
 * tags if viewer has them enabled.
 */
void LStringCollection::toStringStat(const char* pre, const char* post, 
                                     critter* viewer, String& rslt) {
   LString* ptr = NULL;
   String buf(100);   
   rslt.clear();

   LanguageE lang = English;
   if (viewer)
      lang = viewer->getLanguage();

   if (!viewer || viewer->isUsingClient()) {
      Sprintf(rslt, "%s<LS_COLL>", pre);
      if (!viewer || viewer->isImmort()) {
         Cell<LString*> cll(*this);
         while((ptr = cll.next())) {
            Sprintf(buf, "<LS %i>%S</LS>\n", (int)(ptr->getLanguage()),
                    (String*)(ptr));
            rslt.append(buf);
         }//while
      }//if
      else {
         ptr = getString(lang);
         Sprintf(buf, "<LS %i>%S</LS>\n", (int)(ptr->getLanguage()),
                 (String*)(ptr));
         rslt.append(buf);
      }//else
      Sprintf(buf, "</LS_COLL>%s", post);
      rslt.append(buf);
   }//if
   else {
      if (!viewer || viewer->isImmort()) {
         Cell<LString*> cll(*this);
         LString* ptr = NULL;
         while((ptr = cll.next())) {
            Sprintf(buf, "%s: %S\n", str(ptr->getLanguage()),
                    (String*)(ptr));
            rslt.append(buf);
         }//while
      }//if
      else {
         ptr = getString(lang);
         Sprintf(buf, "%S\n", (String*)(ptr));
         rslt.append(buf);
      }//else
   }//else, no client tags.
}//toStringStat


void LStringCollection::appendString(LanguageE lang, String& buf) {
   LString* ptr = getString(lang);
   if (ptr != &(LString::NULL_STRING) && ptr->getLanguage() == lang) {
      ptr->append(buf);
   }
   else {
      //add it
      addString(lang, buf);
   }
}//appendString


void LStringCollection::prependString(LanguageE lang, String& buf) {
   LString* ptr = getString(lang);
   if (ptr != &(LString::NULL_STRING) && ptr->getLanguage() == lang) {
      ptr->prepend(buf);
   }
   else {
      //add it
      addString(lang, buf);
   }
}//prependString

void LStringCollection::appendString(LString& ls) {
   LString* ptr = getString(ls.getLanguage());
   if (ptr != &(LString::NULL_STRING) && ptr->getLanguage() == ls.getLanguage()) {
      ptr->append((String)(ls));
   }
   else {
      //add it
      addLstring(ls);
   }
}//appendString


void LStringCollection::prependString(LString& ls) {
   LString* ptr = getString(ls.getLanguage());
   if (ptr != &(LString::NULL_STRING) && ptr->getLanguage() == ls.getLanguage()) {
      ptr->prepend((String)(ls));
   }
   else {
      //add it
      addLstring(ls);
   }
}//prependString


void LStringCollection::addString(LanguageE lang, String& buf) {
   LString ls(lang, buf);
   addLstring(ls);
}


int LStringCollection::write(ostream& dafile) {
   Cell<LString*> cll(*this);
   LString* ptr = NULL;
   while((ptr = cll.next())) {
      dafile << "1\n"; //sentinel
      dafile << *ptr << endl;
   }
   dafile << "-1\n";
   return 0;
}

int LStringCollection::read(istream& dafile, int read_all) {
   int sent = 0;
   char buf[100];
   LString ls;
   dafile >> sent;
   while (dafile && (sent != -1)) {
      dafile >> ls;
      dafile.getline(buf, 80);

      addLstring(ls);
      dafile >> sent;
   }//while
   if (dafile)
      return 0;
   return -1;
}//read

LString* LStringCollection::getString(LanguageE for_lang) {
   Cell<LString*> cll(*this);
   LString* ptr = NULL;
   LString* eng_ptr = NULL;
   while((ptr = cll.next())) {
      if (ptr->getLanguage() == for_lang) {
         return ptr;
      }//if
      else if (ptr->getLanguage() == English) { //dflt to this
         eng_ptr = ptr;
      }
   }//while
   if (eng_ptr) {
      return eng_ptr;
   }

   //if here, then return anything.
   if (!isEmpty()) {
      return peekFront();
   }

   // Give up, nothing to return, so return zero-length string.
   LString::NULL_STRING.clear(); //make sure it's null.
   return &(LString::NULL_STRING);
}

void KeywordEntry::toStringStat(critter* viewer, String& rslt) {
   if (!viewer || viewer->isUsingClient()) {
      Sprintf(rslt, "<KE_NAMES %i> ", (int)(lang));
   }
   else {
      Sprintf(rslt, "Lang: %s  ", str(lang));
   }

   Cell<LString*> cll(*this);
   LString* ptr;
   while ((ptr = cll.next())) {
      rslt.append("\"");
      rslt.append(*ptr);
      rslt.append("\" ");
   }

   if (!viewer || viewer->isUsingClient()) {
      rslt.append("</KE_NAMES>\n");
   }
   else {
      rslt.append("\n");
   }
}

int KeywordEntry::write(ostream& dafile) {
   Cell<LString*> cll(*this);
   LString* ptr;
   dafile << (int)(lang) << endl;
   while ((ptr = cll.next())) {
      dafile << "99 " << *ptr << endl;
   }
   dafile << "-1\n";
   return 0;
}//write

int KeywordEntry::read(istream& dafile, int read_all) {
   char buf[100];
   LString ls;
   int l;
   int sent;
   dafile >> l;
   lang = (LanguageE)(l);

   dafile >> sent;
   while (dafile && (sent != -1)) {
      dafile >> ls;
      ls.strip(); //get rid of whitespace.
      append(new LString(ls));
      mudlog << "KeywordEntry::read, got name -:" << ls << ":-\n";
      dafile >> sent;
   }//while
   dafile.getline(buf, 99);
   if (dafile)
      return 0;
   return -1;
}//read

KeywordEntry* LKeywordCollection::getCollection(LanguageE for_lang) {
   Cell<KeywordEntry*> cll(*this);
   KeywordEntry* ptr;
   KeywordEntry* eng_ptr = NULL;
   while ((ptr = cll.next())) {
      if (ptr->getLanguage() == for_lang) {
         return ptr;
      }//if
      else if (ptr->getLanguage() == English) {
         eng_ptr = ptr;
      }
   }//while

   if (eng_ptr) {
      return eng_ptr;
   }

   return peekFront();
}//getCollection


/** This will add the new string to the collection.  The collection takes
 * ownership of the memory when new_string is passed in, so passing a
 * reference to a stack variable, for example, is a real bad idea.
 * Duplicate languages are accepted.
 */
void LKeywordCollection::addLstring(LString* new_string) {
   KeywordEntry* ptr = getCollection(new_string->getLanguage());
   if (ptr && ptr->getLanguage() == new_string->getLanguage()) {
      ptr->append(new_string);
   }
   else {
      ptr = new KeywordEntry(new_string->getLanguage());
      ptr->append(new_string);
      append(ptr);
   }//else
}//addLstring


/** First keyword is default 'shortName' for PC's, but for mobiles,
 * we often use the short_desc.
 */
LString* LKeywordCollection::getFirstKeyword(LanguageE preferred_lang) {
   KeywordEntry* ptr = getCollection(preferred_lang);
   if (ptr) {
      return ptr->peekFront();
   }
   LString::NULL_STRING.clear();
   return &(LString::NULL_STRING);
}

/** Clear all keywords of this language type. */
void LKeywordCollection::clearLanguage(LanguageE lang) {
   KeywordEntry* ptr = getCollection(lang);
   if (ptr && ptr->getLanguage() == lang) {
      ptr->clearAndDestroy();
   }
   //else, it wasn't there anyway..
}

void LKeywordCollection::toStringStat(critter* viewer, String& rslt) {
   if (!viewer || viewer->isUsingClient()) {
      rslt = "<KW_COLL>";
   }
   else {
      rslt.clear();
   }

   LanguageE lang = English;
   if (viewer)
      lang = viewer->getLanguage();

   KeywordEntry* ptr;
   String buf(100);
   if (!viewer || viewer->isImmort()) {
      //show all
      Cell<KeywordEntry*> cll(*this);
      while ((ptr = cll.next())) {
         ptr->toStringStat(viewer, buf);
         rslt.append(buf);
      }
   }
   else {
      ptr = getCollection(lang);
      if (ptr) {
         ptr->toStringStat(viewer, buf);
         rslt.append(buf);
      }//if
   }//else

   if (!viewer || viewer->isUsingClient()) {
      rslt.append("<KW_COLL>\n");
   }
   else {
      rslt.append("\n");
   }
}//toStringStat



int LKeywordCollection::write(ostream& dafile) {
   Cell<KeywordEntry*> cll(*this);
   KeywordEntry* ptr;
   while ((ptr = cll.next())) {
      dafile << "1 ";
      ptr->write(dafile);
   }
   dafile << "-1 end of keyword_coll\n";
   return 0;
}

int LKeywordCollection::read(istream& dafile, int read_all) {
   int sent = 0;
   KeywordEntry* ptr;

   dafile >> sent;
   while (dafile && (sent != -1)) {
      ptr = new KeywordEntry();
      ptr->read(dafile);
      append(ptr);

      String buf(100);
      ptr->toStringStat(NULL, buf);
      mudlog << "LKeywordCollection::read, got entry -:" << buf << ":-\n";

      dafile >> sent;
   }//while
   char buf[100];
   dafile.getline(buf, 99);

   if (dafile)
      return 0;
   return -1;
}//read   



ContainedObject::~ContainedObject() {
   SafeList<ContainedObject*>* ptr;
   while (!contained_by.isEmpty()) {
      ptr = contained_by.popFront();
      ContainedObject* tmp_ptr = this;
      ptr->privRemoveObject__(tmp_ptr);
   }//while
}//destructor


int StatBonus::_cnt = 0;

String StatBonus::toString() const {
   String retval(100);
   Sprintf(retval, "stat: %i  bonus: %i", stat, bonus);
   return retval;
}

SpellDuration* Entity::isAffectedBy(int spell_num) {
   Cell<SpellDuration*> cll(affected_by);
   SpellDuration* ptr;

   while ((ptr = cll.next())) {
      if (ptr->spell == spell_num)
         return ptr;
   }//while
   return NULL;
}//is_affected_by


int Entity::affectedByToString(critter* viewer, String& rslt) {
   LanguageE lang = English;
   if (viewer)
      lang = viewer->getLanguage();

   String buf(100);
   rslt.clear();
   if (!affected_by.isEmpty()) {
      rslt.append(cstr(CS_AFFECTED_BY, lang));
      Cell<SpellDuration*> cll(affected_by);
      SpellDuration* sd_ptr;
      while ((sd_ptr = cll.next())) {
         Sprintf(buf, "\t%s.\n", 
                 (const char*)(SSCollection::instance().getNameForNum(sd_ptr->spell)));
         rslt.append(buf);
      }//while
   }//if
   else {
      rslt.append(cstr(CS_NOT_AFFECTED_SPELLS, lang));
   }//else
   return 0;
}//affectedByToString


Entity::~Entity() {
   affected_by.clearAndDestroy();
}


void Entity::clear() {
   container = NULL;
   vis_bit = id_num = zone_num = 0;
   affected_by.clearAndDestroy();
   names.clear();
   long_desc.clear();
}
 

String* Entity::getName(int c_bit, LanguageE lang) {
   String* retval = NULL;
   if (detect(c_bit, vis_bit)) {
      if (names.getCollection(lang)) {
         retval = names.getCollection(lang)->peekFront();
      }
   }
   if (retval) {
      return retval;
   }
   else {
      return &SOMEONE;
   }
}//getName

String* Entity::getFirstName(int c_bit) {
   return Entity::getName(c_bit);
}

String* Entity::getLastName(int c_bit, LanguageE lang) {
   String* retval = NULL;
   if (detect(c_bit, vis_bit)) {
      if (names.getCollection(lang)) {
         retval = names.getCollection(lang)->peekRear();
      }
   }

   if (retval)
      return retval;
   return &SOMEONE;
}

String* Entity::getFirstName(int c_bit, LanguageE lang) {
   String* retval = NULL;
   if (detect(c_bit, vis_bit)) {
      if (names.getCollection(lang)) {
         retval = names.getCollection(lang)->peekFront();
      }
   }

   if (retval)
      return retval;
   return &SOMEONE;
}


String* Entity::getLastName(int c_bit) {
   return getLastName(c_bit, English);
}

String* Entity::getShortName(int c_bit) {
   return Entity::getName(c_bit);
}

String* Entity::getLongName(int c_bit) {
   return Entity::getName(c_bit);
}

String* Entity::getName(critter* observer) {
   LanguageE lang = English;
   int c_bit = ~0;
   if (observer) {
      lang = observer->getLanguage();
      c_bit = observer->getSeeBit();
   }
   return getName(c_bit, lang);
}

String* Entity::getFirstName(critter* observer) {
   return Entity::getName(observer);
}

String* Entity::getLastName(critter* observer) {
   return getLastName(observer->getSeeBit(), observer->getLanguage());
}

String* Entity::getShortName(critter* observer) {
   return Entity::getName(observer);
}

String* Entity::getLongName(critter* observer) {
   return Entity::getName(observer);
}

String* Entity::getLongDesc(critter* viewer) {
   LanguageE lang = English;
   int c_bit = ~0;
   if (viewer) {
      lang = viewer->getLanguage();
      c_bit = viewer->getSeeBit();
   }

   if (detect(c_bit, vis_bit)) {
      return long_desc.getString(lang);
   }
   else {
      return &UNKNOWN;
   }
}


int Entity::isNamed(const String& name, critter* viewer) {
   return isNamed(name, viewer->getSeeBit(), viewer->getLanguage());
}

int Entity::isNamed(const String& name, LanguageE lang) {
   return isNamed(name, ~0, lang);
}

int Entity::isNamed(const String& name, int c_bit, LanguageE lang) {
   if (detect(c_bit, vis_bit)) {
      KeywordEntry* ptr = names.getCollection(lang);
      if (ptr) {
         Cell<LString*> cll(*ptr);
         LString* lptr;
         int len = name.Strlen();
         while ((lptr = cll.next())) {
            if (strncasecmp(name, *lptr, len) == 0) {
               return TRUE;
            }//if
         }//while
      }//if
   }//if
   return FALSE;
}

int Entity::isNamed(const String& name) {
   KeywordEntry* ptr = names.getCollection(English);
   if (ptr) {
      Cell<LString*> cll(*ptr);
      LString* lptr;
      int len = name.Strlen();
      while ((lptr = cll.next())) {
         if (strncasecmp(name, *lptr, len) == 0) {
            return TRUE;
         }//if
      }//while
   }//if
   return FALSE;
}

int Entity::isNamed(const String& name, int do_exact) {
   KeywordEntry* ptr = names.getCollection(English);
   if (ptr) {
      Cell<LString*> cll(*ptr);
      LString* lptr;
      while ((lptr = cll.next())) {
         if (strncasecmp(name, *lptr, lptr->Strlen()) == 0) {
            return TRUE;
         }//if
      }//while
   }//if
   return FALSE;
}


/** Makes copy of incoming data. */
void Entity::addName(String& nm) {
   LString ls(English, nm);
   addName(ls);
}

void Entity::addName(LString& nm) {
   KeywordEntry* ptr = names.getCollection(nm.getLanguage());
   if (ptr && ptr->getLanguage() == nm.getLanguage()) {
      ptr->append(new LString(nm));
   }
   else {
      ptr = new KeywordEntry(nm.getLanguage());
      names.append(ptr);
      ptr->append(new LString(nm));
   }
}//addName
      
void Entity::addName(CSentryE nms) {
   LString nm;
   for (int i = 0; i<LS_PER_ENTRY; i++) {
      nm = CSHandler::getString(nms, (LanguageE)(i));
      nm.setLanguage((LanguageE)(i));
      addName(nm);
   }
}

void Entity::addName(LanguageE lang, String& nm) {
   LString ls(lang, nm);
   addName(ls);
}


/** Makes copy of incoming data. */
void Entity::addLongDesc(LString& new_val) {
   long_desc.addLstring(new_val);
}

void Entity::addLongDesc(String& new_val) {
   long_desc.addString(English, new_val);
}

void Entity::appendLongDesc(LanguageE lang, String& buf) {
   long_desc.appendString(lang, buf);
}

void Entity::appendLongDesc(LString& buf) {
   long_desc.appendString(buf);
}

void Entity::appendLongDesc(CSentryE msg) {
   for (int i = 0; i<LS_PER_ENTRY; i++) {
      LString nm((LanguageE)(i), CSHandler::getString(msg, (LanguageE)(i)));
      appendLongDesc(nm);
   }
}


/** Return TRUE if it could be removed.  Exact match is needed. */
int Entity::removeName(LanguageE lang, String& name) {
   KeywordEntry* ptr = getNamesCollection(lang);
   if (ptr && ptr->getLanguage() == lang) {
      Cell<LString*> cll(*ptr);
      LString* lptr;
      while ((lptr = cll.next())) {
         if (strcasecmp(*lptr, name) == 0) {
            delete lptr;
            lptr = ptr->lose(cll);
            return TRUE;
         }//if
      }//while
   }//if
   return FALSE;
}

/** Takes ownership of memory for new_affect.  This *MUST* prepend,
 * or olc.cc will be broken.
 */
void Entity::addAffectedBy(SpellDuration* new_affect) {
   affected_by.prepend(new_affect);
}

void Entity::remAffectedBy(SpellDuration* victim) {
   affected_by.loseData(victim);
}

void Entity::addAffectedBy(int spell, int duration) {
   affected_by.prepend(new SpellDuration(spell, duration));
}

/** Give as much info as possible, used by immortals.  Include client
 * tags if viewer has them enabled.
 */
void Entity::toStringStat(critter* viewer, String& rslt, ToStringTypeE st) {
   String buf(100);
   if (!viewer || viewer->isUsingClient()) {
      Sprintf(rslt, "<ENTITY %i>", id_num);
   }
   else {
      Sprintf(rslt, "Entity#:  %i\n", id_num);
   }

   names.toStringStat(viewer, buf);
   rslt.append(buf);
   long_desc.toStringStat(viewer, buf);
   rslt.append(buf);

   if ((st | ST_IDENTIFY) || (!viewer || viewer->isImmort())) {
      Sprintf(buf, "\nid_num: %i  vis_bits: %i  zone_num: %i\n",
              id_num, vis_bit, zone_num);
      rslt.append(buf);
      affectedByToString(viewer, buf);
      rslt.append(buf);
   }//if
}//toStringStat

int Entity::write(ostream& dafile) {
   MetaTags mt(*this);
   mt.write(dafile);

   dafile << id_num << " " << vis_bit << " " << zone_num << endl;

   Cell<SpellDuration*> cll(affected_by);
   SpellDuration* ptr;
   while ((ptr = cll.next())) {
      dafile << ptr->spell << " " << ptr->duration << " ";
   }
   dafile << "-1\n";

   names.write(dafile);
   long_desc.write(dafile);
   return 0;
}

int Entity::read(istream& dafile, int read_all) {
   MetaTags mt(dafile);
   dafile >> id_num >> vis_bit >> zone_num;
   
   Entity::clear();

   SpellDuration* ptr;
   int spell;
   int dur;
   dafile >> spell;
   while (dafile && (spell != -1)) {
      dafile >> dur;
      ptr = new SpellDuration(spell, dur);
      affected_by.append(ptr);
      dafile >> spell;
   }//while

   names.read(dafile);
   long_desc.read(dafile);
   return 0;
}


void Entity::setLongDesc(CSentryE msg) {
   for (int i = 0; i<LS_PER_ENTRY; i++) {
      LString nm((LanguageE)(i), CSHandler::getString(msg, (LanguageE)(i)));
      long_desc.addLstring(nm);
   }
}


