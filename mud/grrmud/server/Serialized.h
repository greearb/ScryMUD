// $Id: Serialized.h,v 1.7 1999/09/07 07:00:26 greear Exp $
// $Revision: 1.7 $  $Author: greear $ $Date: 1999/09/07 07:00:26 $

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

#ifndef __SERIALIZED_INCLUDE_GRRMUD__
#define __SERIALIZED_INCLUDE_GRRMUD__

#include <string2.h>


/** I'd wrather not do it this way, but I can't find a better solution.
 * So, I'm going to make the Entity's aware of what kind of object
 * they are.
 *
 * NOTE:  It is extremely important to assign numbers to these, as they will
 * be written into the database/world files, and should not change values
 * with the code.
 */
enum LEtypeE {
   LE_ENTITY = 0,
   LE_CRITTER = 1,
   LE_OBJECT = 2,
   LE_BAG_OBJ = 3,
   LE_ROOM = 4,
   LE_VEHICLE = 5,
   LE_DOOR_DATA = 6,
   LE_DOOR = 7,
   LE_GEN_SCRIPT = 8,
   LE_SKILL_SPELL = 9,
   LE_SERIALIZED = 10,
   LE_LS_COLLECTION = 11,
   LE_SCRIPT_COLL = 12,
   LE_CLOSABLE = 13,
   LE_OBJ_PROC = 14,
   LE_IMM_DATA = 15,
   LE_TEACHER_DATA = 16,
   LE_PLAYER_SHOP_DATA = 17,
   LE_SHOP_DATA = 18,
   LE_MOB_PROC_DATA = 19,
   LE_MOB_DATA = 20,
   LE_PC_DATA = 21,
   LE_KW_COLLECTION = 22,
   LE_RM_KEYWORD = 23,
   LE_PATH_CELL = 24,
   LE_UNKNOWN = 0xFFFFFFE,
   LE_ANY = 0xFFFFFFF
};

class Serialized {
protected:
   
public:
   virtual int write(ostream& dafile) = 0;
   virtual int read(istream& dafile, int read_all = TRUE) = 0;

   /** Stuff used to generate meta data. */
   virtual LEtypeE getEntityType() { return LE_SERIALIZED; }
   virtual const char* getVersion() { return "3.0"; }
   virtual String getOptionalMetaTags() { String r; return r; } //ie blank
};


#endif
