// $Id$

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

#ifndef GRRMUD_MAPPER_INCLUDE_H
#define GRRMUD_MAPPER_INCLUDE_H

#include <list2.h>

class maproom {
   protected:
      int room_num;
      int xpos;
      int ypos;
      int zpos;
      int zone_num;
      bool iswater;
      bool isdeepwater;
      bool isplaceholder;
      bool cause_of_collision;

   public:
      maproom() {
         room_num = 0;
         xpos = 0;
         ypos = 0;
         zpos = 0;
         zone_num = 0;
         iswater = false;
         isdeepwater = false;
         isplaceholder = false;
         cause_of_collision = false;
      }

      int num(void) { return room_num; }
      int num(int newval) { room_num = newval; return room_num; }
      int x(void) { return xpos; }
      int x(int newval) { xpos = newval; return xpos; }
      int y(void) { return ypos; }
      int y(int newval) { ypos = newval; return ypos; }
      int z(void) { return zpos; }
      int z(int newval) { zpos = newval; return zpos; }
      int zone(void) { return zone_num; }
      int zone(int newval) { zone_num = newval; return zone_num; }

      bool water(void) { return iswater; }
      bool water(bool newval) { iswater = newval; return iswater; }
      bool deepwater(void) { return isdeepwater; }
      bool deepwater(bool newval) { isdeepwater = newval; return isdeepwater; }
      bool placeholder(void) { return isplaceholder; }
      bool placeholder(bool newval) { isplaceholder = newval; return
         isplaceholder; }
      bool collided(void) { return cause_of_collision; }
      bool collided(bool newval) { cause_of_collision = newval; return
         cause_of_collision; }
};

class maprooms {
   protected:
      PtrList<maproom> rlist;
   public:
      maprooms();
      ~maprooms();

      enum direction {
         NORTH,
         SOUTH,
         EAST,
         WEST,
         NORTHEAST,
         SOUTHEAST,
         SOUTHWEST,
         NORTHWEST,
         UP,
         DOWN
      };

      PtrList<maproom> *getList(void) { return &rlist; }

      int gain(maproom &new_room);
      int shift_n(int from_y, int count);
      int shift_s(int from_y, int count);
      int shift_e(int from_x, int count);
      int shift_w(int from_x, int count);
      int shift_ne(int from_x, int from_y, int count);
      int shift_se(int from_x, int from_y, int count);
      int shift_sw(int from_x, int from_y, int count);
      int shift_nw(int from_x, int from_y, int count);
      int shift_u(int from_z, int count);
      int shift_d(int from_z, int count);

      int min_z(void);
      int max_z(void);
      int compress_zlayers(void);

      maproom *byNum(int room_num);
      int collision(direction dir, int range, int x, int y, int z);
      int collision(direction dir, int range, maproom &test_rm);

      maproom &placeroom(maproom &current, maproom &newroom,
            const direction placement_dir, int distance);
};

class path {
   protected:
      int start_room;
      int end_room;
      int dist;
      bool hidden;
   public:
      path() { start_room = -1; end_room = -1; dist = 1; hidden = false; }
      int start(void) { return start_room; }
      int end(void) { return end_room; }
      int start(int s) { start_room = s; return start_room; }
      int end(int e) { end_room = e; return end_room; }
      int distance(void) { return dist; }
      int distance(int d) { dist = d; return dist; }
};

class zonepath : public path {
   protected:
      int dst_zone;
   public:
      zonepath() { path(); dst_zone = -1; }
      int zone(int z) { dst_zone = z; return dst_zone; }
      int zone(void) { return dst_zone; }
};

#endif
