// $Id: load_wld.h,v 1.3 1999/06/05 23:29:14 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date: 1999/06/05 23:29:14 $

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

#ifndef LOAD_WORLD_INCLUDE
#define LOAD_WORLD_INCLUDE

void writeSiteBanned();
void readSiteBanned();

void recursive_init_loads(object& obj, int depth);
void recursive_init_unload(object& obj, int depth);

void recursive_init_loads(critter& pc);
void recursive_init_unload(critter& mob);

void init_loads(); //count up things in game at start
void init_casting_objs();

void load_boards();
void load_wld();
void load_zone(int i_th); //read in i_th zone file
void load_objects(int of_zone);
void load_critters(int of_zone);
void load_doors(int of_zone);
void read_setup();
void write_setup();
void load_skill_spells();

void write_all_zones(); //used for DB changeover mostly
void write_all_doors(); //NOT CODED now
void write_all_objects(); //NOT CODED now

#endif
