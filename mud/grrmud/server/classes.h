// $Id: classes.h,v 1.29 1999/09/06 02:24:25 greear Exp $
// $Revision: 1.29 $  $Author: greear $ $Date: 1999/09/06 02:24:25 $

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

///********************  classes.h  *****************************///



#ifndef __CLASSES_INCLUDE
#define __CLASSES_INCLUDE


#include <string2.h>
#include <bitfield.h>
#include <list2.h>
#include <SafeList.h>
#include "lang_strings.h"
#include "Serialized.h"


class critter;  //foward declaration
class Scriptable;
class object; 


/** Used to determine the scope of the toStringStat methods. They will
 * be used like flags and possible ORed together.
 */
enum ToStringTypeE {
   ST_NORMAL = 1,
   ST_LONG = 2,
   ST_LORE = 4,
   ST_IDENTIFY = 8,
   ST_ALL = ~0 /* all bits set */
};


///************************  StatBonus  ************************///
 
/** A pairing of Stat and Bonus */
class StatBonus {
private:
   static int _cnt;

public:
   int stat;
   int bonus;
   
   StatBonus() : stat(0), bonus(0) { _cnt++; }
   StatBonus(int ss, int bd) :
         stat(ss), bonus(bd) { _cnt++; }
   StatBonus(const StatBonus& src) :
         stat(src.stat),
         bonus(src.bonus) { _cnt++; }
   ~StatBonus() { _cnt--; }

   String toString() const ;

   static int getInstanceCount() { return _cnt; }
};//StatBonus


class MTPair {
public:
   String key;
   String val;
   MTPair(String& k, String& v) : key(k), val(v) { }
   MTPair(const MTPair& src) : key(src.key), val(src.val) { }
   MTPair();

   int write(ostream& dafile);
};

class MetaTags {
protected:
   PtrList<MTPair> lst;

public:
   /** first_tag should be <META */
   MetaTags(const char* first_tag, istream& dafile);
   MetaTags(istream& dafile);
   MetaTags(MetaTags& src) { lst.becomeDeepCopyOf(src.lst); }
   MetaTags(Serialized& obj) { generateTags(*this, obj); }
   virtual ~MetaTags() { lst.clearAndDestroy(); }
   int construct(const char* first_tag, istream& dafile);

   virtual int read(istream& dafile);
   virtual int readTag(String& key, String& val, istream& dafile);
   virtual int write(ostream& dafile);
   virtual void clear();

   virtual String& getValue(const char* key);

   virtual void addTag(String& key, String& val);
   

   static void generateTags(MetaTags& rslt, Serialized& obj);
   
};//MetaTags


class LString : public String {
protected:
   LanguageE lang;
public:
   LString() : String(5), lang(English) { }
   LString(int len) : String(len), lang(English) { }
   LString(LanguageE language, int len) : String(len), lang(language) { }
   LString(const LString& src) : String((String)(src)), lang(src.lang) { }
   LString(LanguageE language, const String& s) : String(s), lang(language) { }
   LString(LanguageE language, const char* s) : String(s), lang(language) { }
   virtual ~LString() { }
   virtual LString& operator=(const String& src); //default to english
   
   friend ostream& operator<< (ostream& stream, const LString& str);
   friend istream& operator>> (istream& stream, LString& str);

   LanguageE getLanguage() const { return lang; }
   void setLanguage(LanguageE l) { lang = l; }
   int isLanguage(LanguageE l) const { return lang == l; }

   static LString NULL_STRING;
};

class LstrArray {
protected:
   int len;
   CSentryE* names;
   CSentryE header;
   static int _cnt;
public:
   /** Does not copy char** names, so make sure you don't delete it!
    */
   LstrArray(int length, const CSentryE* names, const CSentryE col_name);
   
   ~LstrArray() {
      _cnt--;
   }

   int getLength() const { return len; }
   const CSentryE* getNames() const { return (const CSentryE*)names; }
   const char* getName(int idx, critter* viewer) const;
   const char* getHeader(critter* viewer) const;

   /** put names into the buf for display */
   void listNames(String& buf, critter* viewer) const;

   static int getInstanceCount() { return _cnt; } 
};


class LStringCollection : public PtrList<LString>, public Serialized {
public:
   LStringCollection() : PtrList<LString>() { }
   virtual ~LStringCollection();
   LStringCollection& operator=(LStringCollection& src) {
      if (&src != this)
         becomeDeepCopyOf(src);
      return *this;
   }
   LString* getString(LanguageE for_lang = English);
   inline LString* getString(critter* viewer);

   /** This will add the new string to the collection.  If a string with the
    * same language is already in it, then the old one will be
    * replaced by the new one.
    */
   virtual void addLstring(LString& new_string);
   virtual void addString(LanguageE lang, String& buf);

   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);

   /** Give as much info as possible, used by immortals.  Include client
    * tags if viewer has them enabled.
    */
   virtual void toStringStat(const char* pre, const char* post, 
                             critter* viewer, String& rslt);
   virtual void toStringStat(critter* viewer, String& rslt) {
      toStringStat(NULL, NULL, viewer, rslt);
   }

   virtual void appendString(LanguageE lang, String& buf);
   virtual void appendString(LString& ls);

   virtual void prependString(LanguageE lang, String& buf);
   virtual void prependString(LString& ls);

   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_LS_COLLECTION; }
};//LStringCollection


class KeywordEntry : public PtrList<LString> {
protected:
   LanguageE lang;
public:
   KeywordEntry() : lang(English) { }
   KeywordEntry(LanguageE l) : lang(l) { };
   KeywordEntry(KeywordEntry& src) : PtrList<LString>(src), lang(src.lang) { }
   virtual ~KeywordEntry() { clearAndDestroy(); }
   KeywordEntry& operator=(KeywordEntry& src) {
      if (&src != this) {
         becomeDeepCopyOf(src);
         lang = src.lang;
      }
      return *this;
   }

   virtual void toStringStat(critter* viewer, String& rslt);
   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);

   LanguageE getLanguage() { return lang; }
};//KeywordEntry

/** This will be a list of lists of single words, like names (keywords) for an
 * object.  The LStringCollection above is more general in that it
 * will take whole lines or more.
 */
class LKeywordCollection : public PtrList<KeywordEntry>, public Serialized {
public:
   LKeywordCollection() { }
   virtual ~LKeywordCollection() { clearAndDestroy(); }
   LKeywordCollection& operator=(LKeywordCollection& src) {
      if (this != &src)
         becomeDeepCopyOf(src);
      return *this;
   }
   /** Will default to english if it has to, and if no entry is found at all,
    * it will return NULL.
    */
   KeywordEntry* getCollection(LanguageE for_lang);

   /** This will add the new string to the collection.  The collection takes
    * ownership of the memory when new_string is passed in, so passing a
    * reference to a stack variable, for example, is a real bad idea.
    * Duplicate languages are accepted.
    */
   void addLstring(LString* new_string);

   /** Clear all keywords of this language type. */
   void clearLanguage(LanguageE lang);

   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);

   virtual void toStringStat(critter* viewer, String& rslt);

   /** First keyword is default 'shortName' for PC's, but for mobiles,
    * we often use the short_desc.
    */
   virtual LString* getFirstKeyword(LanguageE preferred_lang);

   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_KW_COLLECTION; }
}; //LKeywordCollection


/** Keeps a list of what contains it so it can clean up easily upon
 * destruction and hopefully not leak memory or dangle pointers.
 */
class ContainedObject {
public:
   ContainedObject() : contained_by(NULL) { }

   /** Remove from all containers. */
   virtual ~ContainedObject();

   List<SafeList<ContainedObject*>*>& getContainerList() {
      return contained_by;
   }

   // Don't call these from 'user' code.
   void privAddToContainer(SafeList<ContainedObject*>* ptr) {
      contained_by.gainData(ptr);
   }
   void privRemoveFromContainer(SafeList<ContainedObject*>* ptr) {
      contained_by.loseData(ptr);
   }
private:
   List<SafeList<ContainedObject*>*> contained_by;

};//ContainedObject


class critter;
class SpellDuration;

/** This will be the base class for all objects, doors, critters, and rooms.
 */
class Entity : virtual public ContainedObject, virtual public Serialized {
protected:
   Entity* container;
   int vis_bit;
   int id_num;
   int zone_num;

   PtrList<SpellDuration> affected_by;
   LKeywordCollection names;
   LStringCollection long_desc;

public:
   Entity() : container(NULL), vis_bit(0), id_num(0), zone_num(0) { }
   virtual ~Entity();
   virtual void clear();

   /** Returns zero if we can't find anything better. */
   virtual int getCurRoomNum() = 0;
   virtual void gainInv(object* obj) = 0;

   virtual void setContainer(Entity* cont) { container = cont; }
   Entity* getContainer() { return container; }

   virtual KeywordEntry* getNamesCollection(LanguageE forLang) {
      return names.getCollection(forLang);
   }

   LKeywordCollection& getNames() { return names; }
   virtual void setNames(LKeywordCollection& incoming_names) {
      names.becomeDeepCopyOf(incoming_names);
   }

   virtual LStringCollection* getLongDescColl() { return &long_desc; }
   virtual void setLongDescColl(LStringCollection& ldesc) {
      long_desc.becomeDeepCopyOf(ldesc);
   }

   virtual String* getName(int c_bit = ~0);
   virtual String* getFirstName(int c_bit = ~0);
   virtual String* getLastName(int c_bit = ~0);
   virtual String* getShortName(int c_bit = ~0);
   virtual String* getLongName(int c_bit = ~0);
   /**  These will take care of language translation and see-bits. */
   virtual String* getName(critter* observer);
   virtual String* getFirstName(critter* observer);
   virtual String* getLastName(critter* observer);
   virtual String* getFirstName(int c_bit, LanguageE lang);
   virtual String* getLastName(int c_bit, LanguageE lang);
   virtual String* getShortName(critter* observer);
   virtual String* getLongName(critter* observer);
   virtual String* getLongDesc(critter* observer);
   virtual int isNamed(const String& name, int c_bit, LanguageE lang);
   virtual int isNamed(const String& name, critter* viewer);
   virtual int isNamed(const String& name, LanguageE lang);
   virtual int isNamed(const String& name); //dflt to english (maybe all??)
   virtual int isNamed(const String& name, int do_exact); //dflt to english (maybe all??)

   /** Makes copy of incoming data. */
   virtual void addName(String& nm);
   virtual void addName(LString& nm);
   virtual void addName(CSentryE nms);
   virtual void addName(LanguageE lang, String& nm);

   /** Makes copy of incoming data. */
   virtual void addLongDesc(LString& new_val);
   virtual void addLongDesc(String& new_val);

   virtual void appendLongDesc(LanguageE lang, String& buf);
   virtual void appendLongDesc(LString& buf);
   virtual void appendLongDesc(CSentryE msg);
   virtual void setLongDesc(CSentryE desc);

   /** Return TRUE if it could be removed.  Exact match is needed. */
   virtual int removeName(LanguageE lang, String& name);

   /** Takes ownership of memory for new_affect.  This *MUST* prepend,
    * or olc.cc will be broken.
    */
   virtual void addAffectedBy(SpellDuration* new_affect);
   virtual void remAffectedBy(SpellDuration* victim);
   virtual void addAffectedBy(int spell, int duration);

   virtual PtrList<SpellDuration>& getAffectedBy() { return affected_by; }

   virtual SpellDuration* isAffectedBy(int spell_num);
   virtual int isAffected() { return !(affected_by.isEmpty()); }
   virtual int affectedByToString(critter* viewer, String& rslt);

   /** Give as much info as possible, used by immortals.  Include client
    * tags if viewer has them enabled.
    */
   virtual void toStringStat(critter* viewer, String& rslt, ToStringTypeE st);

   virtual int getVisBit() const { return vis_bit; }
   virtual int isInvisible() const { return getVisBit() | 2; }

   virtual int getIdNum() const { return id_num; }
   /** Zone it 'belongs' to. */
   virtual int getZoneNum() const { return zone_num; }
   virtual int belongsToZone(int znum) const { return znum == zone_num; }

   virtual void setZoneNum(int z) { zone_num = z; }
   virtual void setIdNum(int i) { id_num = i; }
   virtual void setVisBit(int i) { vis_bit = i; }

   virtual SafeList<object*>& getInv() = 0;

   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);
   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_ENTITY; }
};


/**  This class holds a bitfield that concentrates all of the flags
 * that relate to all closable objects, including doors and containers.
 */
class Closable : public Serialized {
private:
   static int _cnt;

protected:
   // When modifying this, modify CLOSABLE_FLAGS_NAMES in const.cc
   bitfield flags;
	      // 0 open exit, basically no door
              // 1 is_mag_lockable, 2 is_closed, 3 is_locked, 4 is_pckabl,
	      // 5 is_lockable, 6 mag_locked (spell only can open it)
	      // 7 is_destructable, 8 is_closeable, 9 is_flippable
              // 10 in_use, 11 is_unopenable (by players, auto only)
	      // 12 is_vehicle_exit, 13 is_secret, 14 is_blocked,
              // 15 !complete, 16 secret_when_open_too, 17 consume_key
              // 18 !passdoor, 19 is_corpse
   int key;
   int token;

public:
   Closable() : key(0), token(0) { _cnt++; }
   Closable(const Closable& src) : flags(src.flags), key(src.key), token(src.token) {
      _cnt++; 
   }
   virtual ~Closable() { _cnt--; }
   virtual void clear() { flags.clear(); key = 0; }

   /** Returns TRUE if we could flip it, false otherwise.  We will not let
    * you flip some flags here, though they can be explicitly set through
    * other methods.
    */
   virtual int flipFlag(int flg);
   virtual bitfield& getFlags() { return flags; }

   int isOpen() const { return !(flags.get(2)); }
   int canClose() const { return canOpen(); }
   int canOpen() const { return (!flags.get(11) 
                                  && flags.get(8)); }
   int canLock() const { return flags.get(5); }

   int isDestructable() const { return flags.get(7); }
   int isClosed() const { return flags.get(2); }
   int isLocked() const { return flags.get(3); }
   int isBlocked() const { return flags.get(14); }
   int isMagLocked() const { return flags.get(6); }
   int isVehicleExit() const { return flags.get(12); }
   int isSecret() const { return (flags.get(13) || flags.get(16)); }
   int isInUse() const { return flags.get(10); }
   int isNotComplete() const { return flags.get(15); }
   int isSecretWhenOpen()  const { return flags.get(16); }
   int consumesKey() const { return flags.get(17); }
   int isNoPassdoor() const { return flags.get(18); }
   int isFlippable() const { return flags.get(9); }
   int isPickable() const { return flags.get(4); }

   void close() { flags.turn_on(2); }
   void open() { flags.turn_off(2); }
   void lock() { close(); flags.turn_on(3); }
   void unlock() { flags.turn_off(3); }
   void setFlags(bitfield& flgs) { flags = flgs; }

   void setComplete() { flags.turn_off(15); }
   void setNotComplete() { flags.turn_on(15); }
   void setLockable(int val) { flags.set(5, val); }
   void setUnOpenable(int val) { flags.set(11, val); }
   void setBlocked(int val) { flags.set(14, val); }
   void setClosed(int val) { flags.set(2, val); }
   void setLocked(int val) { flags.set(3, val); }
   void setPickable(int val) { flags.set(4, val); }
   void setMagLockable(int val) { flags.set(5, val); }
   void setMagLocked(int val) { flags.set(6, val); }
   void setDestructable(int val) { flags.set(7, val); }
   void setCorpse(int val) { flags.set(19, val); }
   void setNoClose(int val) { flags.set(8, !val); }
   void setConsumesKey(int val) { flags.set(17, val); }
   void setInUse(int val) { flags.set(10, val); }
   void setFlippable(int val) { flags.set(9, val); }

   int getKey() const { return key; }
   int setKey(int k) { key = k; return TRUE; }
   int getToken() const { return token; }
   int setToken(int k) { token = k; return TRUE; }

   virtual void toStringStat(critter* viewer, String& rslt, ToStringTypeE st);
   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);
   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_CLOSABLE; }
};//class Closable   

#endif
