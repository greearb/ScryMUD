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

#ifndef GRRMUD_ZONE_INCLUDE_H
#define GRRMUD_ZONE_INCLUDE_H

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#include <stdlib.h>
#include <PtrArray.h>
#include <string2.h>
#include <list2.h>
#include <bitfield.h>
#include "const.h"

class room;
class critter;
class door;





/**  The zones that are to be enabled, ie zunlocked when we transition
 * from the development DB to the beta/release DB.
 */
class ZoneList {
private:
   static int _cnt;

protected:
   ZoneList(const ZoneList& src); //maybe code this some day
   ZoneList() { _cnt++; }

   List<int*> nums;

public:
   ~ZoneList() { _cnt--; clear_ptr_list(nums); }

   static ZoneList& instance(); //singleton.
   static int getInstanceCount() { return _cnt; } //how many ZoneList objects exist?

   void add(int i);

   void remove(int i);

   void readSelf();
   void writeSelf();
   int contains(int i);
   void execute();
   String toString();
};


///************************  zone  ************************///

/** This represents a zone, or area in the game.  It does not actually
 * have any rooms or objects defined in it, but it knows the starting
 * and ending rooms, and from that, all else can be deduced, though it
 * could be more efficient in some cases.
 */
class zone {
private:
   static int _cnt;

protected:
   bitfield zone_flags; //0  NULL
   String zone_name;
   int ticks_in_regen_cycle;
   int ticks_till_regen;
   int begin_room_num;
   int end_room_num;
   int zone_num;
   List<String*> owners; //who has right to edit it

   //Helper, should probably be added to the string class or something.
   void spaceToNewlines(String& str);

public:
   
   zone();  //default constructor
   zone(int id_num);
   zone(const zone& src); //copy constructor
   ~zone();  //destructor
   
   int getTicksInRegenCycle() const { return ticks_in_regen_cycle; }
   int getTicksTillRegen() const { return ticks_till_regen; }
   int getBeginRoomNum() const { return begin_room_num; }
   int getEndRoomNum() const { return end_room_num; }
   int getIdNum() const { return zone_num; }
   int isInUse() const { return end_room_num > 0; }

   void resetTicksTillRegen() { ticks_till_regen = ticks_in_regen_cycle; }
   void decrementTicksTillRegen() { ticks_till_regen--; }
   void setName(const String& str) { zone_name = str; }
   void setTicksInRegenCycle(int i) { if (i > 0) ticks_in_regen_cycle = i; }
   void setEndRoomNum(int i);
   void setBeginRoomNum(int i);
   void setTicksTillRegen(int i) { if (i > 0) ticks_till_regen = i; }

   /**  znum will be assigned to zone_num, kinda a hack to fix
    * an oversight.
    */
   int Read(ifstream& dafile, int znum);
   int Write(ofstream& dafile);

   void Clear();


   /**  From mortal intervention, mob procs, vehicle movement.. */
   int isLocked();
   int isOwnedBy(critter& pc);
   void addOwner(const String& str);
   int removeOwner(const String& str); //returns TRUE if could find and remove it.

   String& getFirstOwner(); //returns NONE if has none.
   void stat(critter& pc); //display to pc

   /** create some output that the graph-viz program neato can use to
    * create a horribly complex looking graph of the zone!
    */
   String createNeatoMapFile(); 


   int isTotallyLoaded();
   int canWriteObjects();
   int canWriteMobs();
   int canWriteRooms();
   int canWriteDoors();
   static int getInstanceCount() { return _cnt; } //how many ZoneList objects exist?
};//zone



/** A collection of zones.  Singleton makes global variables not
 * so messy.
 */
class ZoneCollection {
private:
   static int _cnt;
   
protected:
   LazyPtrArray<zone> zone_list;
   zone dummy;
   ZoneCollection() { _cnt++; zone_list.ensureCapacity(NUMBER_OF_ZONES + 1); };
   ZoneCollection(const ZoneCollection& src); //not implemented.

public:

   ~ZoneCollection() { _cnt--; }

   static ZoneCollection& instance();

   void readSelf();
   void writeSelf();

   void writeWorld(critter& pc); //write all rooms, objs, doors, and critters!!

   void doRegeneration(); //for all areas that need it.
   void zunlock(int znum);
   void zlock(int znum);
   void zlist(critter& pc, int start, int end); //print out self to pc
   void createNewZone(critter& pc, int num_ticks, int num_rooms,
                      const String& name);

   void createNeatoFiles();

   zone& getZoneFor(room& rm);
   zone& elementAt(int i);
   static int getInstanceCount() { return _cnt; } //how many ZoneList objects exist?
}; //ZoneCollection


#endif
