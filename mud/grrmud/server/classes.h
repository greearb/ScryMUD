// $Id: classes.h,v 1.22 1999/08/19 06:34:35 greear Exp $
// $Revision: 1.22 $  $Author: greear $ $Date: 1999/08/19 06:34:35 $

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


class HegemonMarkup {
public:
   /**  Turn any hot characters, such as <,> into Hegemon
    * markup characters.  This will make this string safe for
    * inclusion in hegemon markup text.
    */
   static String makeSafeForHegTag(const char* string);
};

class MTPair {
public:
   String key;
   String val;
   MTPair(String& k, String& v) : key(k), val(v) { }
   MTPair(const MTPair& src) : key(src.key), val(src.val) { }
   MTPair();
};

class MetaTags {
protected:
   PtrList<MTPair> lst;

public:
   MetaTags(const char* first_tag, istream& dafile);
   MetaTags(istream& dafile);
   MetaTags(MetaTags& src) { lst.becomeDeepCopyOf(src.lst); }
   MetaTags(Serialized& obj) { generateTags(*this, obj); }
   virtual ~MetaTags() { lst.clearAndDestroy(); }

   virtual int read(istream& dafile);
   virtual int write(ostream& dafile);
   virtual String& getValue(const char* key);

   static void generateTags(MetaTags& rslt, Serialized& obj);
   
};//MetaTags


class LString : public String {
protected:
   LanguageE lang;

public:
   LString() : String(5), lang(English) { }
   LString(int len) : String(len), lang(English) { }
   LString(LanguageE language, int len) : String(len), lang(language) { }
   LString(const LString& src);
   LString(LanguageE language, const String& s) : String(s), lang(language) { }
   LString(LanguageE language, const char* s) : String(s), lang(language) { }
   virtual ~LString() { }
   virtual LString& operator=(const String& src); //default to english
   
   LanguageE getLanguage() const { return lang; }
   void setLanguage(LanguageE l) { lang = l; }
   int isLanguage(LanguageE l) const { return lang == l; }
};

class LStringCollection : public PtrList<LString>, public Serialized {
public:
   LStringCollection() : PtrList<LString>() { }
   virtual ~LStringCollection();

   LString& getString(LanguageE for_lang);
   LString& getString(critter* viewer); //just call the one above.

   /** This will add the new string to the collection.  If a string with the
    * same language is already in it, then the old one will be deleted and
    * replaced by the new one.  The collection takes ownership of the memory
    * when new_string is passed in, so passing a reference to a stack variable,
    * for example, is a real bad idea.
    */
   void addLstring(LString* new_string);

   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);

   /** Give as much info as possible, used by immortals.  Include client
    * tags if viewer has them enabled.
    */
   virtual void toStringStat(const char* pre, const char* post, 
                             critter* viewer, String& rslt);

   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_LS_COLLECTION; }
};

/** This will be a list of single words, like names (keywords) for an
 * object.  The LStringCollection above is more general in that it
 * will take whole lines or more.
 */
class LKeywordCollection : public PtrList<LStringCollection>, public Serialized {
public:
   LKeywordCollection() { }
   virtual ~LKeywordCollection();
   LKeywordCollection& getCollection(LanguageE for_lang);

   /** This will add the new string to the collection.  If a string with the
    * same language is already in it, then the old one will be deleted and
    * replaced by the new one.  The collection takes ownership of the memory
    * when new_string is passed in, so passing a reference to a stack variable,
    * for example, is a real bad idea.
    */
   void addLstring(LString* new_string);

   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);

   /** First keyword is default 'shortName' for PC's, but for mobiles,
    * we often use the short_desc.
    */
   virtual const LString* getFirstKeyword(LanguageE preferred_lang);

   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_KW_COLLECTION; }
};


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
   virtual ~Entity() { }
   virtual void clear();

   /** Returns zero if we can't find anything better. */
   virtual int getCurRoomNum() = 0;
   virtual void gainInv(object* obj) = 0;

   virtual void setContainer(Entity* cont) { container = cont; }
   Entity* getContainer() { return container; }

   virtual LStringCollection* getNamesCollection(LanguageE forLang);

   virtual const String* getName(int c_bit = ~0);
   virtual const String* getFirstName(int c_bit = ~0);
   virtual const String* getShortName(int c_bit = ~0);
   virtual const String* getLongName(int c_bit = ~0);
   /**  These will take care of language translation and see-bits. */
   virtual const String* getName(critter* observer);
   virtual const String* getFirstName(critter* observer);
   virtual const String* getShortName(critter* observer);
   virtual const String* getLongName(critter* observer);
   virtual const String* getLongDesc(critter* observer);
   virtual int isNamed(const String& name, critter* viwer);
   virtual int isNamed(const String& name); //dflt to english (maybe all??)
   virtual int isNamed(const String& name, int do_exact); //dflt to english (maybe all??)

   /** Makes copy of incoming data. */
   virtual void addName(String& nm);
   virtual void addName(LString& nm);

   /** Makes copy of incoming data. */
   virtual void addLongDesc(LString& new_val);
   virtual void addLongDesc(String& new_val);

   virtual void appendLongDesc(LanguageE lang, String& buf);


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
   virtual int getIdNum() const { return id_num; }
   /** Zone it 'belongs' to. */
   virtual int getZoneNum() const { return zone_num; }
   virtual void setZoneNum(int z) { zone_num = z; }
   virtual void setIdNum(int i) { id_num = i; }
   virtual void setVisBit(int i) { vis_bit = i; }

   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);
   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_ENTITY; }
};


/**  This class holds a bitfield that concentrates all of the flags
 * that relate to all closable objects, including doors and containers.
 */
class Closable : public Serialized {
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
   virtual ~Closable() { }
   virtual void clear() { flags.clear(); key = 0; }

   virtual void flipFlag(int flg);
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
   void setKey(int k) { key = k; }
   int getToken() const { return token; }
   void setToken(int k) { token = k; }

   virtual void toStringStat(critter* viewer, String& rslt, ToStringTypeE st);
   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);
   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_CLOSABLE; }
};//class Closable   

#endif
