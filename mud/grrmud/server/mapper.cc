// $Id$

//
//ScryMUD Server Code
//Copyright (C) 2005  Edward Roper
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
// To contact the Author, Edward Roper: eroper@wanfear.com
//

#include "mapper.h"

maprooms::maprooms() {
   rlist.clear();
   return;
}

maprooms::~maprooms() {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   while ( (cur_rm = cll.next()) ) {
      delete cur_rm;
   }
}

int maprooms::gain(maproom &new_room) {
   maproom *tmp = new maproom;
   *tmp = new_room;
   rlist.append(tmp);
   return 0;
}

maproom *maprooms::byNum(int room_num) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->num() == room_num ) {
         return cur_rm;
      }
   }
   return NULL;
}

int maprooms::shift_n(int from_y, int count) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   if ( count == 0 ) {
      return 0;
   }
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->y() <= from_y ) {
         cur_rm->y(cur_rm->y()-count);
      }
   }
   return count;
}

int maprooms::shift_s(int from_y, int count) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   if ( count == 0 ) {
      return 0;
   }
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->y() >= from_y ) {
         cur_rm->y(cur_rm->y()+count);
      }
   }
   return count;
}

int maprooms::shift_e(int from_x, int count) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   if ( count == 0 ) {
      return 0;
   }
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->x() >= from_x ) {
         cur_rm->x(cur_rm->x()+count);
      }
   }
   return count;
}

int maprooms::shift_w(int from_x, int count) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   if ( count == 0 ) {
      return 0;
   }
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->x() <= from_x ) {
         cur_rm->x(cur_rm->x()-count);
      }
   }
   return count;
}

int maprooms::shift_ne(int from_x, int from_y, int count) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   if ( count == 0 ) {
      return 0;
   }
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->x() >= from_x ) {
         cur_rm->x(cur_rm->x()+count);
      }
      if ( cur_rm->y() <= from_y ) {
         cur_rm->y(cur_rm->y()-count);
      }
   }
   return count;
}

int maprooms::shift_se(int from_x, int from_y, int count) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   if ( count == 0 ) {
      return 0;
   }
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->x() >= from_x ) {
         cur_rm->x(cur_rm->x()+count);
      }
      if ( cur_rm->y() >= from_y ) {
         cur_rm->y(cur_rm->y()+count);
      }
   }
   return count;
}

int maprooms::shift_sw(int from_x, int from_y, int count) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   if ( count == 0 ) {
      return 0;
   }
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->x() <= from_x ) {
         cur_rm->x(cur_rm->x()-count);
      }
      if ( cur_rm->y() >= from_y ) {
         cur_rm->y(cur_rm->y()+count);
      }
   }
   return count;
}

int maprooms::shift_nw(int from_x, int from_y, int count) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   if ( count == 0 ) {
      return 0;
   }
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->x() <= from_x ) {
         cur_rm->x(cur_rm->x()-count);
      }
      if ( cur_rm->y() <= from_y ) {
         cur_rm->y(cur_rm->y()-count);
      }
   }
   return count;
}

int maprooms::shift_u(int from_z, int count) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   if ( count == 0 ) {
      return 0;
   }
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->z() >= from_z ) {
         cur_rm->z(cur_rm->z()+count);
      }
   }
   return count;
}

int maprooms::shift_d(int from_z, int count) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   if ( count == 0 ) {
      return 0;
   }
   while ( (cur_rm = cll.next()) ) {
      if ( cur_rm->z() <= from_z ) {
         cur_rm->z(cur_rm->z()-count);
      }
   }
   return count;
}

int maprooms::min_z(void) {
   int ret_val = 0;
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   while ( (cur_rm = cll.next()) ) {
      ret_val = ( (ret_val > cur_rm->z()) ? cur_rm->z() : ret_val );
   }
   return ret_val;
}//maprooms::min_z

int maprooms::max_z(void) {
   int ret_val = 0;
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   while ( (cur_rm = cll.next()) ) {
      ret_val = ( (ret_val < cur_rm->z()) ? cur_rm->z() : ret_val );
   }
   return ret_val;
}//maprooms::max_z

int maprooms::compress_zlayers(void) {
   int squash_count = 0;
   bool used;
   Cell<maproom*> cll;
   Cell<maproom*> sq_cll;
   maproom *cur_rm;
   maproom *sq_rm;

   //walk all z layers
   for(int i=min_z();i<=max_z();i++) {
      used = false;
      rlist.head(cll);
      while ( (cur_rm = cll.next()) ) {
         if ( cur_rm->z() == i ) {
            used = true;
            break;
         }//if
      }//while

      if ( !used ) {
         //everything below this layer needs to shift up by one.
         rlist.head(sq_cll);
         while ( (sq_rm = cll.next()) ) {
            if ( sq_rm->z() < i ) {
               sq_rm->z(sq_rm->z()+1);
            }//if
         }//while more rooms
         squash_count++;
      }//if !used
   }//for all z-layers
   return squash_count;
}//maprooms::compress_zlayers

int maprooms::collision(direction dir, int range, maproom &test_rm) {
   int x,y,z;
   x = test_rm.x();
   y = test_rm.y();
   z = test_rm.z();
   return collision(dir, range, x, y, z);
}

int maprooms::collision(direction dir, int range, int x, int y, int z) {
   Cell<maproom*> cll(rlist);
   maproom *cur_rm;
   int cdist = 0; //how far do we go before colliding
   int i;

   while ( (cur_rm = cll.next()) ) {
      switch (dir) {
         case NORTH:
            for(i=1;i<=range;i++) {
               if ((cur_rm->x()==x)&&(cur_rm->y()==y-i)&&(cur_rm->z()==z)) {
                  cdist = ( (cdist>i||cdist==0) ? i : cdist );
               }
            }
            break;
         case SOUTH:
            for(i=1;i<=range;i++) {
               if ((cur_rm->x()==x)&&(cur_rm->y()==y+i)&&(cur_rm->z()==z)) {
                  cdist = ( (cdist>i||cdist==0) ? i : cdist );
               }
            }
            break;
         case EAST:
            for(i=1;i<=range;i++) {
               if ((cur_rm->x()==x+i)&&(cur_rm->y()==y)&&(cur_rm->z()==z)) {
                  cdist = ( (cdist>i||cdist==0) ? i : cdist );
               }
            }
            break;
         case WEST:
            for(i=1;i<=range;i++) {
               if ((cur_rm->x()==x-i)&&(cur_rm->y()==y)&&(cur_rm->z()==z)) {
                  cdist = ( (cdist>i||cdist==0) ? i : cdist );
               }
            }
            break;
         case NORTHEAST:
            for(i=1;i<=range;i++) {
               if ((cur_rm->x()==x+i)&&(cur_rm->y()==y-i)&&(cur_rm->z()==z)) {
                  cdist = ( (cdist>i||cdist==0) ? i : cdist );
               }
            }
            break;
         case SOUTHEAST:
            for(i=1;i<=range;i++) {
               if ((cur_rm->x()==x+i)&&(cur_rm->y()==y+i)&&(cur_rm->z()==z)) {
                  cdist = ( (cdist>i||cdist==0) ? i : cdist );
               }
            }
            break;
         case SOUTHWEST:
            for(i=1;i<=range;i++) {
               if ((cur_rm->x()==x-i)&&(cur_rm->y()==y+i)&&(cur_rm->z()==z)) {
                  cdist = ( (cdist>i||cdist==0) ? i : cdist );
               }
            }
            break;
         case NORTHWEST:
            for(i=1;i<=range;i++) {
               if ((cur_rm->x()==x-i)&&(cur_rm->y()==y-i)&&(cur_rm->z()==z)) {
                  cdist = ( (cdist>i||cdist==0) ? i : cdist );
               }
            }
            break;
         case UP:
            for(i=1;i<=range;i++) {
               if ((cur_rm->x()==x)&&(cur_rm->y()==y)&&(cur_rm->z()==z+i)) {
                  cdist = ( (cdist>i||cdist==0) ? i : cdist );
               }
            }
            break;
         case DOWN:
            for(i=1;i<=range;i++) {
               if ((cur_rm->x()==x)&&(cur_rm->y()==y)&&(cur_rm->z()==z-i)) {
                  cdist = ( (cdist>i||cdist==0) ? i : cdist );
               }
            }
            break;
      }//switch(dir)
   }//while cur_rm

   if ( cdist > 0) {
      return ((range-cdist)+1); //amount of shift needed to avoid collision.
   }
   return 0;
}//maprooms::collision()

maproom &maprooms::placeroom(maproom &current, maproom &newroom,
                         const direction placement_dir, int distance) {
   int shift_by;
   int x, y, z;
   x = y = z = 0;

   //shift_by is 0 if we don't need to shift and the calls to shift_?? below
   //will return immediately if passed a 0.
   if ( (shift_by = collision(placement_dir, distance, current)) ) {
      newroom.collided(true);
   }

   switch(placement_dir) {
      case NORTHEAST:
         shift_sw(current.x(), current.y(), shift_by);
         x = current.x()+distance;
         y = current.y()-distance;
         z = current.z();
         break;
      case SOUTHEAST:
         shift_nw(current.x(), current.y(), shift_by);
         x = current.x()+distance;
         y = current.y()+distance;
         z = current.z();
         break;
      case SOUTHWEST:
         shift_ne(current.x(), current.y(), shift_by);
         x = current.x()-distance;
         y = current.y()+distance;
         z = current.z();
         break;
      case NORTHWEST:
         shift_se(current.x(), current.y(), shift_by);
         x = current.x()-distance;
         y = current.y()-distance;
         z = current.z();
         break;
      case NORTH:
         shift_s(current.y(), shift_by);
         x = current.x();
         y = current.y()-distance;
         z = current.z();
         break;
      case SOUTH:
         shift_n(current.y(), shift_by);
         x = current.x();
         y = current.y()+distance;
         z = current.z();
         break;
      case EAST:
         shift_w(current.x(), shift_by);
         x = current.x()+distance;
         y = current.y();
         z = current.z();
         break;
      case WEST:
         shift_e(current.x(), shift_by);
         x = current.x()-distance;
         y = current.y();
         z = current.z();
         break;
      case UP:
         shift_d(current.z(), shift_by);
         x = current.x();
         y = current.y();
         z = current.z()+distance;
         break;
      case DOWN:
         shift_u(current.z(), shift_by);
         x = current.x();
         y = current.y();
         z = current.z()-distance;
         break;
   }//switch

   newroom.x(x);
   newroom.y(y);
   newroom.z(z);
   return newroom;
}//maprooms::placeroom
