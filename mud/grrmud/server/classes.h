// $Id: classes.h,v 1.14 1999/08/04 06:29:16 greear Exp $
// $Revision: 1.14 $  $Author: greear $ $Date: 1999/08/04 06:29:16 $

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
   LString() : String(5) { }
   LString(LanguageE language, int len) : String(len), lang(language) { }
   LString(const LString& src);
   LString(LanguageE language, const String& s) : String(s), lang(language) { }
   LString(LanguageE language, const char* s) : String(s), lang(language) { }
   virtual ~LString() { }
   
   LanguageE getLanguage() const { return lang; }
   void setLanguage(LanguageE l) { lang = l; }
   int isLanguage(LanguageE l) const { return lang == l; }
};

class LStringCollection : public PtrList<LString>, public Serialized {
public:
   LStringCollection() : PtrList<LString>() { }
   virtual ~LStringCollection();
   LString& getString(LanguageE for_lang);

   /** This will add the new string to the collection.  If a string with the
    * same language is already in it, then the old one will be deleted and
    * replaced by the new one.  The collection takes ownership of the memory
    * when new_string is passed in, so passing a reference to a stack variable,
    * for example, is a real bad idea.
    */
   void addLstring(LString* new_string);

   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);

   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_LS_COLLECTION; }
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
class Entity : public ContainedObject, public Serialized {
protected:
   Entity* container;
   int vis_bit;
   int id_num;
   int zone_num;

   PtrList<SpellDuration> affected_by;
   LStringCollection names;
   LStringCollection long_desc;

public:
   Entity() : container(NULL) { }
   virtual ~Entity() { }
   virtual void clear();

   /** Returns zero if we can't find anything better. */
   virtual int getCurRoomNum() = 0;

   virtual void setContainer(Entity* cont) { container = cont; }
   Entity* getContainer() { return container; }

   virtual const String* getName(int c_bit = ~0);
   virtual const String* getShortName(int c_bit = ~0);
   virtual const String* getLongName(int c_bit = ~0);
   /**  These will take care of language translation and see-bits. */
   virtual const String* getName(critter* observer);
   virtual const String* getShortName(critter* observer);
   virtual const String* getLongDesc(critter* observer);
   virtual int isNamed(const String& name) const;
   void appendName(String* nm);
   void appendName(LString* nm);

   void setLongDesc(LString& new_val);
   void setLongDesc(String& new_val);

   void addAffectedBy(SpellDuration* new_affect);

   virtual SpellDuration* isAffectedBy(int spell_num);

   int getVisBit() const { return vis_bit; }
   int getIdNum() const { return id_num; }
   /** Zone it 'belongs' to. */
   int getZoneNum() const { return zone_num; }
   void setZoneNum(int z) { zone_num = z; }
   void setIdNum(int i) { id_num = i; }

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

public:
   virtual ~Closable() { }

   int isOpen() const { return !(flags.get(2)); }
   int canClose() const { return canOpen(); }
   int isVehicleExit() const { return flags.get(12); }
   int canOpen() const { return (!flags.get(11) 
                                  && flags.get(8)); }
   int canLock() const { return flags.get(5); }

   int isClosed() const { return flags.get(2); }
   int isLocked() const { return flags.get(3); }
   int isMagLocked() const { return flags.get(6); }
   int isSecret() const { return (flags.get(13) || flags.get(16)); }
   int isInUse() const { return flags.get(10); }
   int isNotComplete() const { return flags.get(15); }
   int isSecretWhenOpen()  const { return flags.get(16); }
   int consumesKey() const { return flags.get(17); }
   int isNoPassdoor() const { return flags.get(18); }

   void setComplete() { flags.turn_off(15); }
   void setNotComplete() { flags.turn_on(15); }
   void close() { flags.turn_on(2); }
   void open() { flags.turn_off(2); }
   void lock() { close(); flags.turn_on(3); }
   void unlock() { flags.turn_off(3); }

   void setClosed(int val) { flags.set(2, val); }
   void setLocked(int val) { flags.set(3, val); }
   void setPickable(int val) { flags.set(4, val); }
   void setMagLockable(int val) { flags.set(5, val); }
   void setMagLocked(int val) { flags.set(6, val); }
   void setIsDestructable(int val) { flags.set(7, val); }
   void setIsCorpse(int val) { flags.set(19, val); }
   void setIsNoClose(int val) { flags.set(8, !val); }
   void setConsumesKey(int val) { flags.set(17, val); }

   int getKey() const { return key; }
   void setKey(int k) { key = k; }

   virtual int write(ostream& dafile);
   virtual int read(istream& dafile, int read_all = TRUE);
   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_CLOSABLE; }
};//class Closable   

#endif
